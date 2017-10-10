/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "os.h"
#include "aos/aos.h"
/******************************** Thread Middleware ********************************/

#define THREAD_NAME_MAX_CHAR_LEN    (32)
#define THREAD_NAME_SIZE            (THREAD_NAME_MAX_CHAR_LEN + 1)
#define THREAD_NUM                  (20)

//TS, thread state
typedef enum {
    TS_INEXISTENCE = 0,
    TS_EXISTENCE,
    TS_RUNNING,
    TS_WAIT2STOP,
    TS_STOP,
    TS_ZOMBIE,
} thread_state_t;

typedef struct {
    char name[THREAD_NAME_SIZE];    //thread unique identification.
    void *handle;                                   //thread handle from lower level.
    void *stack;                                    //[1:0]: 0, os; 1, inner dynamic allocate; 2, user allocate;
    void *mutex;                                    // not need
    thread_state_t state;
    void *msg;
    //int join_count;
} thread_info_t, *pthread_info_t;

typedef struct {
    void *rlock;
    void *wlock;
    int readers;
    int writer;
} rwlock;

static void thread_os_free_resource(void *thread_info);
static int thread_init_info(pthread_info_t thread_info);
static void thread_os_free_info(pthread_info_t thread_info);

static thread_info_t thread_info_pool[THREAD_NUM];
static void *thread_info_mutex = NULL;

//return:
//0, success;
//-1, failure;
static int thread_init_info(pthread_info_t thread_info)
{
    memset(thread_info, 0, sizeof(thread_info_t));

    thread_info->mutex = platform_mutex_init();
    if (NULL == thread_info->mutex) {
        return -1;
    }

    thread_info->state = TS_EXISTENCE;

    return 0;
}

static void thread_os_free_info(pthread_info_t thread_info)
{
    //assert whether @thread is valid.

    //need not to lock, because no other thread will access it in this state.

    memset(thread_info, 0, sizeof(thread_info_t));
    thread_info->state = TS_INEXISTENCE;
}

static pthread_info_t thread_allocate_info(const char *thread_name)
{
    int i, record_os_free = -1;

    if (NULL == thread_info_mutex) {
        thread_info_mutex = platform_mutex_init();
        if (NULL == thread_info_mutex) {
            return NULL;
        }
    }

    platform_mutex_lock(thread_info_mutex);
    for (i = 0; i < THREAD_NUM; ++i) {
        pthread_info_t pthread_info = &thread_info_pool[i];

        if (0 == strcmp(pthread_info->name, thread_name)) {
            OS_ASSERT(TS_RUNNING != pthread_info->state,
                      "%s: Must call os_thread_exit before exiting\n",
                      pthread_info->name);

            //os_free the thread resource if not any other thread recycle its resource.
            if (TS_STOP == pthread_info->state) {
                thread_os_free_resource(pthread_info);
                pthread_info->state = TS_ZOMBIE;
            }

            OS_ASSERT((TS_ZOMBIE == pthread_info->state),
                      "must in ZOMBIE state when reallocate, thread name = %s\n",
                      thread_info_pool[i].name);

            if (0 != thread_init_info(pthread_info)) {
                pthread_info = NULL;
            }

            platform_mutex_unlock(thread_info_mutex);
            return pthread_info;
        }

        if ((-1 == record_os_free) && (TS_INEXISTENCE == thread_info_pool[i].state)) {
            record_os_free = i;
        }
    }

    //no match, allocate a new element
    if (-1 != record_os_free) {
        thread_init_info(&thread_info_pool[record_os_free]);
        platform_mutex_unlock(thread_info_mutex);
        return &thread_info_pool[record_os_free];
    }

    platform_mutex_unlock(thread_info_mutex);

    OS_ASSERT(0, "should have enough element");

    //get from the element that in ZOMBIE state
    //todo

    return NULL;
}

/**
 * @brief os_free resource of the thread(@thread), but keep the state.
 *
 * @param thread [in].
 * @return none
 * @see none.
 * @note after calling this function, must not to access mutex.
 */
static void thread_os_free_resource(void *thread_info)
{
    pthread_info_t pthread_info = (pthread_info_t) thread_info;

    OS_ASSERT((NULL != pthread_info) && (TS_STOP == pthread_info->state), NULL);

    platform_mutex_lock(thread_info_mutex);

    if (NULL != pthread_info->stack) {
        platform_free(pthread_info->stack);
        pthread_info->stack = NULL;
    }

    if (NULL != pthread_info->mutex) {
        platform_mutex_destroy(pthread_info->mutex);
        pthread_info->mutex = NULL;
    }

    platform_mutex_unlock(thread_info_mutex);
}

void *os_thread_create(const char *thread_name, void *thread_func, void *arg)
{
    pthread_info_t thread_info;
    void *thread, *stack, *stack_orign;
    int stack_used = 0;
    uint32_t stack_size;

    OS_ASSERT(NULL != thread_name &&
              '\0' != *thread_name, "thread must have a name.\n");

    thread_info = thread_allocate_info(thread_name);
    if (NULL == thread_info) {
        return NULL;
    }

    stack_size = platform_thread_get_stack_size(thread_name);

    stack_orign = stack = platform_malloc(stack_size);
    if (NULL == stack) {
        goto do_error;
    }

    if (0 != platform_thread_create(&thread, thread_name, thread_func, arg, stack,
                                    stack_size, &stack_used)) {
        goto do_error;
    }

    OS_ASSERT(NULL != thread, NULL);

    if (0 == stack_used) {                  //stack that allocated just now was not be used, so os_free it.
        platform_free(stack_orign);
        stack_orign = stack = NULL;
    }

    platform_mutex_lock(thread_info->mutex);

    //write string end mark if length(name) > THREAD_NAME_MAX_CHAR_LEN
    thread_info->name[THREAD_NAME_MAX_CHAR_LEN] = '\0';
    strncpy(thread_info->name, thread_name, THREAD_NAME_MAX_CHAR_LEN);

    thread_info->handle = thread;
    thread_info->stack = stack;     //record the source of stack.
    thread_info->msg = NULL;
    thread_info->state = TS_RUNNING;

    platform_mutex_unlock(thread_info->mutex);

    LOG("--->succeed to create thread.\n");
    return thread_info;

do_error:
    if (NULL != thread_info) {
        thread_os_free_info(thread_info);
    }

    if (NULL != stack) {
        platform_free(stack);
    }

    return NULL;
}

void os_thread_exit(void *thread_info)
{
    pthread_info_t pthread_info = (pthread_info_t) thread_info;

    platform_mutex_lock(pthread_info->mutex);

    pthread_info->state = TS_STOP;

    platform_mutex_unlock(pthread_info->mutex);

    platform_thread_exit(pthread_info->handle);
}

int os_thread_is_existence(void *thread_info)
{
    return (NULL == thread_info) ? 0 : 1;
}

int os_thread_join(void *thread_info)
{
    OS_ASSERT(NULL != thread_info, NULL);

    pthread_info_t pthread_info = (pthread_info_t) thread_info;

    while (TS_STOP != pthread_info->state) {
        platform_msleep(10);
    }

    thread_os_free_resource(pthread_info);

    pthread_info->handle = NULL;
    pthread_info->state = TS_ZOMBIE;
    pthread_info->msg = NULL;

    /*
     * Note: at this moment, platform_thread_exit() may still work in progress
     * so better to wait a while.
     *
     * os_thread_create("thread A") right after os_thread_join("thread A")
     * still may fail because of platform_thread_exit("thread A") still WIP
     */
    platform_msleep(10);

    return 0;
}

//return
//0, success;
//-1, failure, already has message.
//should not be NULL
int os_thread_post_msg(void *thread_info, void *msg)
{
    OS_ASSERT(NULL != thread_info, NULL);
    OS_ASSERT(NULL != msg, "msg should not be NULL ");

    int ret;
    pthread_info_t pthread_info = (pthread_info_t) thread_info;

    platform_mutex_lock(pthread_info->mutex);

    if (NULL == pthread_info->msg) {
        ret = 0;
        pthread_info->msg = msg;
    } else {
        ret = -1;
    }

    platform_mutex_unlock(pthread_info->mutex);

    return ret;
}

//return message(@thread_info->msg) and set at NULL.
//NULL represent that no message.
void *os_thread_wait_msg(void *thread_info, uint32_t timeout)
{
    OS_ASSERT(NULL != thread_info, NULL);

    void *msg = NULL;
    pthread_info_t pthread_info = (pthread_info_t) thread_info;
    int sleep_time;

    do {
        platform_mutex_lock(pthread_info->mutex);
        msg = pthread_info->msg;
        pthread_info->msg = NULL;
        platform_mutex_unlock(pthread_info->mutex);

        if (NULL != msg) {
            return msg;
        } else {
            if (PLATFORM_WAIT_INFINITE == timeout) {
                sleep_time = 10;
            } else if (0 == timeout) {
                return NULL;
            } else {
                sleep_time = (timeout >= 10) ? (timeout -= 10, 10) : (timeout = 0, timeout);
            }
        }

        platform_msleep(sleep_time);

    } while (1);
}


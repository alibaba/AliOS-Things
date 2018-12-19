/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <string.h>
#include <stdlib.h>
#include <time.h>

#undef WITH_LWIP
#undef WITH_SAL
#include "aos/kernel.h"
#include <windows.h>
#include <stdio.h>

#define MAX_SEM_COUNT 32

void aos_reboot(void)
{
    exit(0);
}

int aos_get_hz(void)
{
    return 100;
}

const char *aos_version_get(void)
{
    return "aos-winnt-xxx";
}

struct targ
{
    const char *name;
    void (*fn)(void *);
    void *arg;
};

int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 int stack_size)
{
    DWORD dwThreadId = 0;

    HANDLE hTID = 0;
    hTID        = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)fn,
                        (LPVOID)arg, // argument pointer
                        0,           // start immediatly
                        &dwThreadId);

    return (hTID != NULL ? 0 : -1);
}

int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg, int stack_size, int prio)
{
    return aos_task_new(name, fn, arg, stack_size);
}

void aos_task_exit(int code)
{
    ExitThread(code);
}

int aos_task_key_create(aos_task_key_t *key)
{
    *key = TlsAlloc();
    return 0;
}

void aos_task_key_delete(aos_task_key_t key)
{
    TlsFree(key);
}

int aos_task_setspecific(aos_task_key_t key, void *vp)
{
    return (TlsSetValue(key, (LPVOID)vp) ? 0 : -1);
}

void *aos_task_getspecific(aos_task_key_t key)
{
    return TlsGetValue(key);
}

int aos_mutex_new(aos_mutex_t *mutex)
{
    int               rc              = -1;
    CRITICAL_SECTION *CriticalSection = malloc(sizeof(CRITICAL_SECTION));
    if (NULL != CriticalSection) {
        InitializeCriticalSection(CriticalSection);
        mutex->hdl = CriticalSection;
        rc         = 0;
    }
    return rc;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    DeleteCriticalSection(mutex->hdl);
    free(mutex->hdl);
    mutex->hdl = NULL;
}

int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout)
{
    int rc = -1;
    if ((NULL != mutex) && (NULL != mutex->hdl)) {
        EnterCriticalSection(mutex->hdl);
        rc = 0;
    }
    return rc;
}

int aos_mutex_unlock(aos_mutex_t *mutex)
{
    int rc = -1;
    if ((NULL != mutex) && (NULL != mutex->hdl)) {
        LeaveCriticalSection(mutex->hdl);
        rc = 0;
    }
    return rc;
}

int aos_mutex_is_valid(aos_mutex_t *mutex)
{
    return mutex->hdl != NULL;
}

int aos_sem_new(aos_sem_t *sem, int count)
{
    int     rc             = -1;
    HANDLE *eventSemaphore = malloc(sizeof(HANDLE));

    *eventSemaphore = CreateSemaphore(NULL, // default security attributes
                                      0,    // initial count
                                      MAX_SEM_COUNT, // maximum count
                                      NULL);         // unnamed semaphore
    if (NULL == *eventSemaphore) {
        printf("###error when create semahore %d\n", GetLastError());
        free(eventSemaphore);
    } else {
        sem->hdl = eventSemaphore;
        rc       = 0;
    }
    return rc;
}

void aos_sem_free(aos_sem_t *sem)
{
    if (sem == NULL || sem->hdl == NULL) {
        return;
    }

    CloseHandle(*(HANDLE *)sem->hdl);
    free(sem->hdl);
}

int aos_sem_wait(aos_sem_t *sem, unsigned int timeout)
{
    int         rc           = -1;
    const DWORD dwWaitResult = WaitForSingleObject(
      *(HANDLE *)sem->hdl, (AOS_WAIT_FOREVER == timeout) ? INFINITE : timeout);
    switch (dwWaitResult) {
        // The semaphore object was signaled.
        case WAIT_OBJECT_0:
            rc = 0;
            // printf("Thread %d: wait succeeded\n", GetCurrentThreadId());

            break;
        case WAIT_TIMEOUT:
            // printf("Thread %d: wait timed out\n", GetCurrentThreadId());
            break;
        default:
            break;
    }
    return rc;
}

void aos_sem_signal(aos_sem_t *sem)
{
    if ((NULL != sem) && (NULL != sem->hdl)) {
        ReleaseSemaphore(*(HANDLE *)sem->hdl, // handle to semaphore
                         1,                   // increase count by one
                         NULL);
    }
}

int aos_sem_is_valid(aos_sem_t *sem)
{
    return sem && (sem->hdl != NULL);
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    aos_sem_signal(sem);
}

struct queue
{
    MSG         *msg;
    DWORD        iMqdes; // Specify the Thread ID who own above msg
    unsigned int size;
    unsigned int msg_size;
};

#ifndef MSG_AOS_BASE
#define MSG_AOS_BASE (WM_USER + 100)
#endif /*MSG_AOS_BASE*/

#define MSG_THREAD_RCV (MSG_AOS_BASE + 5)

int aos_queue_new(aos_queue_t *queue, void *buf, unsigned int size, int max_msg)
{
    struct queue *q = malloc(sizeof(queue));
    q->size         = size;
    q->msg_size     = max_msg;
    q->iMqdes       = GetCurrentThreadId();
    q->msg          = malloc(sizeof(MSG));
    queue->hdl      = q;
    PeekMessage(q->msg, NULL, MSG_THREAD_RCV, MSG_THREAD_RCV, PM_NOREMOVE);

    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    if ((NULL != queue) && (NULL != queue->hdl)) {
        struct queue *q = queue->hdl;
        free(q->msg);
        q->iMqdes = 0;
        free(q);
    }
}

int aos_queue_send(aos_queue_t *queue, void *msg, unsigned int size)
{
    int rc = -1;
    if (NULL != queue && NULL != queue->hdl) {
        struct queue *q = queue->hdl;
        if (size <= q->msg_size) {
            char *tmpBuf = (char *)malloc(size);
            if (!PostThreadMessage(q->iMqdes, MSG_THREAD_RCV, (WPARAM)tmpBuf,
                                   size)) {
                free(tmpBuf);
            } else {
                rc = 0;
            }
        }
    }
    return rc;
}

int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg,
                   unsigned int *size)
{

    // AOS_WAIT_FOREVER
    int rc = -1;
    if (queue != NULL && msg != NULL) {
        struct queue *q            = queue->hdl;
        bool          skipNextStep = false;
        if (0 ==
            ms) { // First peek, read it out if there is already msg received
            if (!PeekMessage(q->msg, NULL, MSG_THREAD_RCV, MSG_THREAD_RCV,
                             PM_NOREMOVE)) {
                skipNextStep = true;
            }
        } else if (ms != AOS_WAIT_FOREVER) {
            SetTimer(NULL, 0, ms, NULL);
        }
        if (!skipNextStep) {
            if (GetMessage(q->msg, NULL, 0, 0)) {
                switch (q->msg->message) {
                    case WM_TIMER:
                        break;
                    case MSG_THREAD_RCV:
                        if ((char *)q->msg->wParam != NULL &&
                            q->msg->lParam > 0) {
                            memcpy(msg, (void *)q->msg->wParam, q->msg->lParam);
                            *size = q->msg->lParam;
                            free((void *)q->msg->wParam);
                            rc = 0;
                        }

                        break;

                    default:
                        break;
                }
            }
        }
    }
    return rc;
}

int aos_queue_is_valid(aos_queue_t *queue)
{
    return queue->hdl != NULL;
}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    return NULL;
}

int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                  int ms, int repeat)
{
    return -1;
}

void aos_timer_free(aos_timer_t *timer) {}

int aos_timer_start(aos_timer_t *timer)
{
    return -1;
}

int aos_timer_stop(aos_timer_t *timer)
{
    return -1;
}

int aos_timer_change(aos_timer_t *timer, int ms)
{
    return -1;
}

int aos_workqueue_create(aos_workqueue_t *workqueue, int pri, int stack_size)
{
    return -1;
}


struct work
{
    void (*fn)(void *);
    void *arg;
    int   dly;
};

int aos_work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly)
{
    struct work *w = malloc(sizeof(*w));
    w->fn          = fn;
    w->arg         = arg;
    w->dly         = dly;
    work->hdl      = w;
    return 0;
}

void aos_work_destroy(aos_work_t *work)
{
    free(work->hdl);
}

int aos_work_run(aos_workqueue_t *workqueue, aos_work_t *work)
{
    return aos_work_sched(work);
}

static void worker_entry(void *arg)
{
    struct work *w = arg;
    if (w->dly != 0) {
        Sleep(w->dly);
    }
    w->fn(w->arg);
}

int aos_work_sched(aos_work_t *work)
{
    struct work *w = work->hdl;
    return aos_task_new("worker", worker_entry, w, 8192);
}

int aos_work_cancel(aos_work_t *work)
{
    return -1;
}

void *aos_zalloc(unsigned int size)
{
    return calloc(size, 1);
}

void *aos_malloc(unsigned int size)
{
    return malloc(size);
}

void *aos_calloc(unsigned int nitems, unsigned int size)
{
    return calloc(nitems, size);
}

void *aos_realloc(void *mem, unsigned int size)
{
    return realloc(mem, size);
}

void aos_alloc_trace(void *addr, size_t allocator) {}

void aos_free(void *mem)
{
    free(mem);
}

int gettimeofday(struct timeval *tp, struct timezone *tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch
    // has 9 trailing zero's This magic number is the number of 100 nanosecond
    // intervals since January 1, 1601 (UTC) until 00:00:00 January 1, 1970
    static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    SYSTEMTIME system_time;
    FILETIME   file_time;
    uint64_t   time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long)((time - EPOCH) / 10000000L);
    tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
    return 0;
}

#ifndef timersub
#define timersub(a, b, result)                           \
    do {                                                 \
        (result)->tv_sec  = (a)->tv_sec - (b)->tv_sec;   \
        (result)->tv_usec = (a)->tv_usec - (b)->tv_usec; \
        if ((result)->tv_usec < 0) {                     \
            --(result)->tv_sec;                          \
            (result)->tv_usec += 1000000;                \
        }                                                \
    } while (0)
#endif // timersub

static struct timeval sys_start_time;
long long             aos_now(void)
{
    struct timeval tv;
    long long      ns;
    gettimeofday(&tv, NULL);
    timersub(&tv, &sys_start_time, &tv);
    ns = tv.tv_sec * 1000000LL + tv.tv_usec;
    return ns * 1000LL;
}

long long aos_now_ms(void)
{
    struct timeval tv;
    long long      ms;
    gettimeofday(&tv, NULL);
    timersub(&tv, &sys_start_time, &tv);
    ms = tv.tv_sec * 1000LL + tv.tv_usec / 1000;
    return ms;
}

char *aos_now_time_str(char *buffer, const int len)
{
    if (buffer != NULL && len > 0) {
        struct timeval curTime;
        gettimeofday(&curTime, NULL);
        time_t time_second = curTime.tv_sec;
        memset(buffer, 0, len);
        struct tm now_time;
        localtime_s(&now_time, &time_second);

        strftime(buffer, len, "%m-%d %H:%M:%S", &now_time);

        if ((int)(len - strlen(buffer) - 1) > 0) {
            const int milli    = curTime.tv_usec / 1000;
            char      msStr[5] = "";
            snprintf(msStr, sizeof(msStr), ".%03d", milli);
            strncat(buffer, msStr, len - strlen(buffer) - 1);
        }
    }
    return buffer;
}

void aos_msleep(int ms)
{
    Sleep(ms);
}

void aos_init(void)
{
    gettimeofday(&sys_start_time, NULL);
}

void aos_start(void)
{
    while (1) {
        Sleep(1000);
    }
}

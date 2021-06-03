/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/prctl.h>
#include <sys/time.h>

#include <poll.h>
#include "amp_platform.h"
#include "aos_system.h"
#include "aos/kernel.h"
#include "aos_network.h"
#include "ulog/ulog.h"

#define _SYSINFO_DEVICE_NAME "aos-linux"
#define SYSINFO_VERSION      "0.0.1"

typedef struct {
    timer_t timer;
    void (*cb)(void *,void *);
    void *user_data;
    uint32_t ms;
    uint32_t repeat;
} osal_timer_inner_t;

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
    return SYSINFO_VERSION;
}

struct targ {
    const char *name;
    void (*fn)(void *);
    void *arg;
};

static void *dfl_entry(void *arg)
{
    struct targ *targ  = arg;
    void (*fn)(void *) = targ->fn;
    void *farg         = targ->arg;
    prctl(PR_SET_NAME, (unsigned long)targ->name, 0, 0, 0);
    free(targ);

    fn(farg);

    return 0;
}

int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 size_t stack_size)
{
    int          ret;
    pthread_t    th;
    struct targ *targ = malloc(sizeof(*targ));
    targ->name        = strdup(name);
    targ->fn          = fn;
    targ->arg         = arg;
    ret               = pthread_create(&th, NULL, dfl_entry, targ);
    if (ret == 0) {
        ret = pthread_detach(th);
    }
    return ret;
}

aos_status_t aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg, size_t stack_size, int32_t prio)
{
    return aos_task_new(name, fn, arg, stack_size);
}

void aos_task_exit(int code)
{
    pthread_exit((void *)code);
}

const char *aos_task_name(void)
{
    static char name[16];
    prctl(PR_GET_NAME, (unsigned long)name, 0, 0, 0);
    return name;
}

int aos_task_key_create(aos_task_key_t *key)
{
    return pthread_key_create(key, NULL);
}

void aos_task_key_delete(aos_task_key_t key)
{
    pthread_key_delete(key);
}

int aos_task_setspecific(aos_task_key_t key, void *vp)
{
    return pthread_setspecific(key, vp);
}

void *aos_task_getspecific(aos_task_key_t key)
{
    return pthread_getspecific(key);
}

aos_status_t aos_mutex_new(aos_mutex_t *mutex)
{
    pthread_mutex_t *mtx = malloc(sizeof(*mtx));
    pthread_mutex_init(mtx, NULL);
    *mutex = mtx;
    return 0;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    pthread_mutex_destroy(*mutex);
    free(*mutex);
}

int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout)
{
    if (mutex) {
        pthread_mutex_lock(*mutex);
    }
    return 0;
}

int aos_mutex_unlock(aos_mutex_t *mutex)
{
    if (mutex) {
        pthread_mutex_unlock(*mutex);
    }
    return 0;
}

bool aos_mutex_is_valid(aos_mutex_t *mutex)
{
    return *mutex != NULL;
}

#include <semaphore.h>
aos_status_t aos_sem_new(aos_sem_t *sem, uint32_t count)
{
    sem_t *s = malloc(sizeof(*s));
    sem_init(s, 0, count);
    *sem = s;
    return 0;
}

void aos_sem_free(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    sem_destroy(*sem);
    free(*sem);
}

int aos_sem_wait(aos_sem_t *sem, unsigned int timeout)
{
    int sec;
    int nsec;

    if (sem == NULL) {
        return -EINVAL;
    }

    if (timeout == AOS_WAIT_FOREVER) {
        return sem_wait(*sem);
    } else if (timeout == 0) {
        return sem_trywait(*sem);
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    sec  = timeout / 1000;
    nsec = (timeout % 1000) * 1000000;

    ts.tv_nsec += nsec;
    sec += (ts.tv_nsec / 1000000000);
    ts.tv_nsec %= 1000000000;
    ts.tv_sec += sec;

    return sem_timedwait(*sem, &ts);
}

void aos_sem_signal(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    sem_post(*sem);
}

bool aos_sem_is_valid(aos_sem_t *sem)
{
    return sem && *sem != NULL;
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    sem_post(*sem);
}

struct queue {
    int   fds[2];
    void *buf;
    int   size;
    int   msg_size;
};

aos_status_t aos_queue_new(aos_queue_t *queue, void *buf, size_t size, size_t max_msg)
{
    struct queue *q = malloc(sizeof(*q) + size);
    pipe(q->fds);
    q->buf      = q + sizeof(*q);
    q->size     = size;
    q->msg_size = max_msg;
    *queue  = q;
    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    struct queue *q = *queue;
    close(q->fds[0]);
    close(q->fds[1]);
    free(q);
}

aos_status_t aos_queue_send(aos_queue_t *queue, void *msg, size_t size)
{
    struct queue *q = *queue;
    write(q->fds[1], msg, size);
    return 0;
}

aos_status_t aos_queue_recv(aos_queue_t *queue, uint32_t ms, void *msg, size_t *size)
{
    struct queue *q   = *queue;
    struct pollfd rfd = {
        .fd     = q->fds[0],
        .events = POLLIN,
    };

    poll(&rfd, 1, ms);
    if (rfd.revents & POLLIN) {
        int len = read(q->fds[0], msg, q->msg_size);
        *size   = len;
        return len < 0 ? -1 : 0;
    }

    return -1;
}

bool aos_queue_is_valid(aos_queue_t *queue)
{
    return *queue != NULL;
}

static void timer_common_cb(union sigval arg)
{
    osal_timer_inner_t *amp_timer = (osal_timer_inner_t *)arg.sival_ptr;

    if (amp_timer && amp_timer->cb) {
        amp_timer->cb(amp_timer, amp_timer->user_data);
    }
}

aos_status_t aos_timer_create(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                              uint32_t ms, uint32_t options)
{
    struct sigevent ent;
    osal_timer_inner_t *amp_timer = NULL;

    /* check parameter */
    if (fn == NULL) {
        return -1;
    }

    amp_timer = aos_malloc(sizeof(osal_timer_inner_t));

    if (!amp_timer) {
        return -1;
    }
    memset(amp_timer, 0, sizeof(osal_timer_inner_t));

    /* Init */
    memset(&ent, 0x00, sizeof(struct sigevent));

    /* create a timer */
    ent.sigev_notify          = SIGEV_THREAD;
    ent.sigev_notify_function = (void (*)(union sigval))timer_common_cb;
    ent.sigev_value.sival_ptr = amp_timer;

    if (timer_create(CLOCK_MONOTONIC, &ent, &amp_timer->timer) != 0) {
        aos_free(amp_timer);
        return -1;
    }

    amp_timer->cb = fn;
    amp_timer->ms = ms;
    amp_timer->user_data = arg;
    amp_timer->repeat = (options & AOS_TIMER_REPEAT) ? 1 : 0;

    *timer = amp_timer;

    return 0;
}

void aos_timer_free(aos_timer_t *timer)
{
    int ret = 0;
    osal_timer_inner_t *amp_timer = NULL;

    /* check parameter */
    if (timer == NULL) {
        return;
    }

    amp_timer = (osal_timer_inner_t *)*timer;

    if (!amp_timer)
        return;

    ret = timer_delete(amp_timer->timer);

    aos_free(amp_timer);
    return;
}

int aos_timer_start(aos_timer_t *timer)
{
    struct itimerspec ts;
    osal_timer_inner_t *amp_timer = NULL;

    /* check parameter */
    if (timer == NULL) {
        return -1;
    }

    amp_timer = (osal_timer_inner_t *)*timer;

    if (amp_timer->repeat) {
        /* it_value=0: stop timer */
        ts.it_interval.tv_sec = amp_timer->ms / 1000;
        ts.it_interval.tv_nsec = (amp_timer->ms % 1000) * 1000000;
    } else {
        /* it_interval=0: timer run only once */
        ts.it_interval.tv_sec = 0;;
        ts.it_interval.tv_nsec = 0;
    }

    /* it_value=0: stop timer */
    ts.it_value.tv_sec = amp_timer->ms / 1000;
    ts.it_value.tv_nsec = (amp_timer->ms % 1000) * 1000000;

    return timer_settime(amp_timer->timer, 0, &ts, NULL);
}

int aos_timer_stop(aos_timer_t *timer)
{
    struct itimerspec ts;
    osal_timer_inner_t *amp_timer = NULL;

    /* check parameter */
    if (timer == NULL) {
        return -1;
    }

    amp_timer = (osal_timer_inner_t *)*timer;

    /* it_interval=0: timer run only once */
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    /* it_value=0: stop timer */
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;

    return timer_settime(amp_timer->timer, 0, &ts, NULL);
}

void *aos_zalloc(size_t size)
{
    return calloc(size, 1);
}

void *aos_malloc(size_t size)
{
    return malloc(size);
}

void *aos_calloc(size_t nitems, size_t size)
{
    return calloc(nitems, size);
}

void *aos_realloc(void *mem, size_t size)
{
    return realloc(mem, size);
}

void aos_alloc_trace(void *addr, size_t allocator) {}

void aos_free(void *mem)
{
    free(mem);
}

static struct timeval sys_start_time;
uint64_t aos_now(void)
{
    struct timeval tv;
    long long      ns;
    gettimeofday(&tv, NULL);
    timersub(&tv, &sys_start_time, &tv);
    ns = tv.tv_sec * 1000000LL + tv.tv_usec;
    return ns * 1000LL;
}

uint64_t aos_now_ms(void)
{
    struct timeval tv;
    long long      ms;
    gettimeofday(&tv, NULL);
    timersub(&tv, &sys_start_time, &tv);
    ms = tv.tv_sec * 1000LL + tv.tv_usec / 1000;
    return ms;
}

aos_status_t aos_now_time_str(char *buffer, size_t len)
{
    if (buffer != NULL && len > 0) {
        struct timeval curTime;

        gettimeofday(&curTime, NULL);

        memset(buffer, 0, len);
        strftime(buffer, len, "%m-%d %H:%M:%S", localtime(&curTime.tv_sec));

        if ((int)(len - strlen(buffer) - 1) > 0) {
            const int milli    = curTime.tv_usec / 1000;
            char      msStr[8] = {0};
            snprintf(msStr, sizeof(msStr), ".%03d", milli);
            strncat(buffer, msStr, len - strlen(buffer) - 1);
        }
    }
    return 0;
}

void aos_msleep(uint32_t ms)
{
    usleep(ms * 1000);
}

void aos_init(void)
{
    gettimeofday(&sys_start_time, NULL);
}

void aos_start(void)
{
    while (1) {
        usleep(1000 * 1000 * 100);
    }
}

#include <sys/types.h>
#include <dirent.h>
void dumpsys_task_func(void)
{
    DIR *proc = opendir("/proc/self/task");
    while (1) {
        struct dirent *ent = readdir(proc);
        if (!ent) {
            break;
        }
        if (ent->d_name[0] == '.') {
            continue;
        }

        char fn[128];
        snprintf(fn, sizeof fn, "/proc/self/task/%s/comm", ent->d_name);
        FILE *fp = fopen(fn, "r");
        if (!fp) {
            continue;
        }
        bzero(fn, sizeof fn);
        fread(fn, sizeof(fn) - 1, 1, fp);
        fclose(fp);
        printf("%8s - %s", ent->d_name, fn);
    }
    closedir(proc);
}

void aos_printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

int aos_snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int aos_vsnprintf(char *str, const int len, const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

const char *aos_get_system_version(void)
{
    return SYSINFO_VERSION;
}

const char *aos_get_platform_type(void)
{
    return _SYSINFO_DEVICE_NAME;
}

int aos_get_ip(char *ip)
{
    return 0;
}

int aos_get_mac_addr(unsigned char mac[8])
{
    // TODO: ?
    return 0;
}


int aos_network_status_registercb(void (*cb)(int status, void *), void *arg)
{
    // TODO: ?
    return 0;
}

int aos_get_network_status(void)
{
    // TODO: ?
    return 1;
}

const char *aos_get_device_name(void)
{
    return "linux";
}

int amp_heap_memory_info(amp_heap_info_t *heap_info)
{
    return 0;
}

int aos_system_init(void)
{
    return 0;
}

const char *aos_get_module_hardware_version(void)
{
    return "Module_Hardware_version";
}

const char *aos_get_module_software_version(void)
{
    return "Module_Software_version";
}

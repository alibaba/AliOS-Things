/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <pthread.h>

#undef WITH_LWIP
#undef WITH_SAL
#include <aos/aos.h>
#include <poll.h>

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
    return "aos-linux-xxx";
}

struct targ
{
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
                 int stack_size)
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

int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg, int stack_size, int prio)
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

int aos_mutex_new(aos_mutex_t *mutex)
{
    pthread_mutex_t *mtx = malloc(sizeof(*mtx));
    pthread_mutex_init(mtx, NULL);
    mutex->hdl = mtx;
    return 0;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    pthread_mutex_destroy(mutex->hdl);
    free(mutex->hdl);
}

int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout)
{
    if (mutex) {
        pthread_mutex_lock(mutex->hdl);
    }
    return 0;
}

int aos_mutex_unlock(aos_mutex_t *mutex)
{
    if (mutex) {
        pthread_mutex_unlock(mutex->hdl);
    }
    return 0;
}

int aos_mutex_is_valid(aos_mutex_t *mutex)
{
    return mutex->hdl != NULL;
}

#include <semaphore.h>
int aos_sem_new(aos_sem_t *sem, int count)
{
    sem_t *s = malloc(sizeof(*s));
    sem_init(s, 0, count);
    sem->hdl = s;
    return 0;
}

void aos_sem_free(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    sem_destroy(sem->hdl);
    free(sem->hdl);
}

int aos_sem_wait(aos_sem_t *sem, unsigned int timeout)
{
    int sec;
    int nsec;

    if (sem == NULL) {
        return -EINVAL;
    }

    if (timeout == AOS_WAIT_FOREVER) {
        return sem_wait(sem->hdl);
    } else if (timeout == 0) {
        return sem_trywait(sem->hdl);
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    sec  = timeout / 1000;
    nsec = (timeout % 1000) * 1000000;

    ts.tv_nsec += nsec;
    sec += (ts.tv_nsec / 1000000000);
    ts.tv_nsec %= 1000000000;
    ts.tv_sec += sec;

    return sem_timedwait(sem->hdl, &ts);
}

void aos_sem_signal(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    sem_post(sem->hdl);
}

int aos_sem_is_valid(aos_sem_t *sem)
{
    return sem && sem->hdl != NULL;
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    sem_post(sem->hdl);
}

struct queue
{
    int   fds[2];
    void *buf;
    int   size;
    int   msg_size;
};

int aos_queue_new(aos_queue_t *queue, void *buf, unsigned int size, int max_msg)
{
    struct queue *q = malloc(sizeof(*q));
    pipe(q->fds);
    q->buf      = buf;
    q->size     = size;
    q->msg_size = max_msg;
    queue->hdl  = q;
    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    struct queue *q = queue->hdl;
    close(q->fds[0]);
    close(q->fds[1]);
    free(q);
}

int aos_queue_send(aos_queue_t *queue, void *msg, unsigned int size)
{
    struct queue *q = queue->hdl;
    write(q->fds[1], msg, size);
    return 0;
}

int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg,
                   unsigned int *size)
{
    struct queue *q   = queue->hdl;
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

int aos_queue_is_valid(aos_queue_t *queue)
{
    return queue->hdl != NULL;
}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    struct queue *q = queue->hdl;
    return q->buf;
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
    if (w->dly) {
        usleep(w->dly * 1000);
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

        memset(buffer, 0, len);
        strftime(buffer, len, "%m-%d %H:%M:%S", localtime(&curTime.tv_sec));

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

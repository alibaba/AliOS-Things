/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>

#include <pthread.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <semaphore.h>
#include <errno.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "amp_system.h"

#define PLATFORM_WAIT_INFINITE (~0)
#define SYSINFO_DEVICE_NAME "Linux"
#define SYSINFO_VERSION "0.0.1"
#define DEFAULT_TASK_PRIORITY 32

static amp_heap_info_t heap_usage = {0, 0};

void *HAL_Malloc(uint32_t size)
{
    void *ptr = NULL;
    if((ptr = malloc(size)) == NULL){
        return NULL;
    }

    /* heap usage increase */
    heap_usage.heap_used += malloc_usable_size(ptr);
    return ptr;
}

void HAL_Free(void *ptr)
{
    if(ptr == NULL){
        return;
    }

    /* heap usage decrease */
    heap_usage.heap_used -= malloc_usable_size(ptr);
    free(ptr);
}

void *HAL_Calloc(uint32_t nitems, uint32_t size)
{
    void *ptr = NULL;

    if((ptr = calloc((size_t)nitems, (size_t)size)) == NULL){
        return NULL;
    }

    /* heap usage increase */
    heap_usage.heap_used += malloc_usable_size(ptr);
    return ptr;
}

void *HAL_Realloc(void *ptr, uint32_t size)
{
    void *next = NULL;

    /* ptr = NULL, equal to call malloc(size) */
    if(ptr == NULL){
        if ((next = HAL_Malloc(size)) == NULL){
            return NULL;
        }
        return next;
    }

    /* size = 0，equal to call free(ptr) */
    if (size == 0){
        HAL_Free(ptr);
        return NULL;
    }

    /* heap usage decrease */
    heap_usage.heap_used -= malloc_usable_size(ptr);

    if((next = realloc(ptr, (size_t)size)) == NULL){
        return NULL;
    }

    /* heap usage increase */
    heap_usage.heap_used += malloc_usable_size(next);
    return next;
}

uint64_t HAL_UptimeMs(void)
{
    uint64_t            time_ms;
    struct timespec     ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    time_ms = ((uint64_t)ts.tv_sec * (uint64_t)1000) + (ts.tv_nsec / 1000 / 1000);

    return time_ms;
}

void HAL_SleepMs(uint32_t ms)
{
    usleep(1000 * ms);
}

void HAL_Srandom(uint32_t seed)
{
    srandom(seed);
}

uint32_t HAL_Random(uint32_t region)
{
    FILE *handle;
    ssize_t ret = 0;
    uint32_t output = 0;
    handle = fopen("/dev/urandom", "r");
    if (handle == NULL) {
        perror("open /dev/urandom failed\n");
        return 0;
    }
    ret = fread(&output, sizeof(uint32_t), 1, handle);
    if (ret != 1) {
        printf("fread error: %d\n", (int)ret);
        fclose(handle);
        return 0;
    }
    fclose(handle);
    return (region > 0) ? (output % region) : output;
}

void HAL_Printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

int HAL_Snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

void HAL_Reboot(void)
{
    if (system("reboot")) {
        perror("HAL_Reboot failed");
    }
}

void *HAL_SemaphoreCreate(void)
{
    sem_t *sem = (sem_t *)malloc(sizeof(sem_t));
    if (NULL == sem) {
        return NULL;
    }

    if (0 != sem_init(sem, 0, 0)) {
        free(sem);
        return NULL;
    }

    return sem;
}

void HAL_SemaphoreDestroy(void *sem)
{
    sem_destroy((sem_t *)sem);
    free(sem);
}

void HAL_SemaphorePost(void *sem)
{
    sem_post((sem_t *)sem);
}

int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
    if (PLATFORM_WAIT_INFINITE == timeout_ms) {
        sem_wait(sem);
        return 0;
    } else {
        struct timespec ts;
        int s;
        /* Restart if interrupted by handler */
        do {
            if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
                return -1;
            }

            s = 0;
            ts.tv_nsec += (timeout_ms % 1000) * 1000000;
            if (ts.tv_nsec >= 1000000000) {
                ts.tv_nsec -= 1000000000;
                s = 1;
            }

            ts.tv_sec += timeout_ms / 1000 + s;

        } while (((s = sem_timedwait(sem, &ts)) != 0) && errno == EINTR);

        return (s == 0) ? 0 : -1;
    }
}

int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            amp_os_thread_param_t *amp_os_thread_param,
            int *stack_used)
{
	pthread_t handler;
    int ret = -1;

    if (stack_used) {
        *stack_used = 0;
    }

    ret = pthread_create((pthread_t *)&handler, NULL, work_routine, arg);
    if (ret != 0) {
        printf("pthread_create error: %d\n", (int)ret);
        return -1;
    }
	//*thread_handle = handler;
    return 0;
}

int HAL_ThreadDelete(void *thread_handle)
{
    pthread_exit(0);
    return 0;
}

int HAL_GetDefaultTaskPriority()
{
    return DEFAULT_TASK_PRIORITY;
}

void *HAL_MutexCreate(void)
{
    int err_num;
    pthread_mutex_t *mutex = (pthread_mutex_t *)HAL_Malloc(sizeof(pthread_mutex_t));
    if (NULL == mutex) {
        return NULL;
    }

    if (0 != (err_num = pthread_mutex_init(mutex, NULL))) {
        perror("create mutex failed\n");
        HAL_Free(mutex);
        return NULL;
    }

    return mutex;
}

void HAL_MutexDestroy(void *mutex)
{
    int err_num;

    if (!mutex) {
        perror("mutex want to destroy is NULL!\n");
        return;
    }
    if (0 != (err_num = pthread_mutex_destroy((pthread_mutex_t *)mutex))) {
        perror("destroy mutex failed\n");
    }

    HAL_Free(mutex);
}

void HAL_MutexLock(void *mutex)
{
    int err_num;
    if (0 != (err_num = pthread_mutex_lock((pthread_mutex_t *)mutex))) {
        printf("lock mutex failed: - '%s' (%d)\n", strerror(err_num), err_num);
    }
}

void HAL_MutexUnlock(void *mutex)
{
    int err_num;
    if (0 != (err_num = pthread_mutex_unlock((pthread_mutex_t *)mutex))) {
        printf("unlock mutex failed - '%s' (%d)\n", strerror(err_num), err_num);
    }
}

void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data)
{
    timer_t *timer = NULL;

    struct sigevent ent;

    /* check parameter */
    if (func == NULL) {
        return NULL;
    }

    timer = (timer_t *)malloc(sizeof(time_t));
    if (timer == NULL) {
        return NULL;
    }

    /* Init */
    memset(&ent, 0x00, sizeof(struct sigevent));

    /* create a timer */
    ent.sigev_notify = SIGEV_THREAD;
    ent.sigev_notify_function = (void (*)(union sigval))func;
    ent.sigev_value.sival_ptr = user_data;

    if (timer_create(CLOCK_MONOTONIC, &ent, timer) != 0) {
        free(timer);
        return NULL;
    }

    return (void *)timer;
}

void *HAL_GetTimerPrams(void *timerHandle)
{
    return timerHandle;
}

int HAL_Timer_Start(void *timer, int ms, amp_timer_type repeat)
{
    struct itimerspec ts;

    /* check parameter */
    if (timer == NULL) {
        return -1;
    }

    if (repeat) {
        /* it_value=0: stop timer */
        ts.it_interval.tv_sec = ms / 1000;
        ts.it_interval.tv_nsec = (ms % 1000) * 1000000;
    } else {
        /* it_interval=0: timer run only once */
        ts.it_interval.tv_sec = 0;;
        ts.it_interval.tv_nsec = 0;
    }

    /* it_value=0: stop timer */
    ts.it_value.tv_sec = ms / 1000;
    ts.it_value.tv_nsec = (ms % 1000) * 1000000;

    return timer_settime(*(timer_t *)timer, 0, &ts, NULL);
}

int HAL_Timer_Stop(void *timer)
{
    struct itimerspec ts;

    /* check parameter */
    if (timer == NULL) {
        return -1;
    }

    /* it_interval=0: timer run only once */
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    /* it_value=0: stop timer */
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;

    return timer_settime(*(timer_t *)timer, 0, &ts, NULL);
}

int HAL_Timer_Delete(void *timer)
{
    int ret = 0;

    /* check parameter */
    if (timer == NULL) {
        return -1;
    }

    ret = timer_delete(*(timer_t *)timer);

    free(timer);

    return ret;
}

#define MESSAGE_QUEUE_NAME   "/amp"
#define QUEUE_PERMISSIONS     0666
void *HAL_MessageQCreate(int32_t queue_length, int32_t item_size)
{
    mqd_t mq;
    struct mq_attr attr;

    /* config a message queue */
    attr.mq_flags = 0;
    attr.mq_maxmsg = queue_length;
    attr.mq_msgsize = item_size;
    attr.mq_curmsgs = 0;

    if ((mq = mq_open (MESSAGE_QUEUE_NAME, O_RDWR | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("HAL: mq_open (mq)");
        exit (1);
    }

    return (void *)mq;
}

int32_t HAL_MessageQPut(void *mq, void *p_info, uint32_t size, uint32_t millisec)
{
    int32_t ret = -1;

    if (mq == NULL || p_info == NULL) {
        return ret;
    }
    if (mq_send(mq, p_info, size, 0) == -1) {
            perror ("HAL: Not able to send message");
            return ret;
    }

    return 0;
}

int32_t HAL_MessageQGet(void *mq, void *p_info, uint32_t size, uint32_t millisec)
{
    int32_t ret = -1;

    if (mq == NULL || p_info == NULL) {
        return ret;
    }
    if (mq_receive(mq, p_info, size, 0) == -1) {
            perror ("HAL: Not able to receive message");
            return ret;
    }

    return 0;
}

int32_t HAL_MessageQDelete(void *mq)
{
    int32_t ret = -1;

    if (mq == NULL) {
        return ret;
    }

    if (mq_close(mq) == -1) {
            perror ("HAL: Not able to close message");
            return ret;
    }

    if (mq_unlink (MESSAGE_QUEUE_NAME) == -1) {
        perror ("Client: mq_unlink");
        return ret;
    }

    return 0;
}

int HAL_GetFirmwareVersion(char *version)
{
    return 0;
}

const char *HAL_Get_System_Version(void)
{
    return SYSINFO_VERSION;
}

const char *HAL_Get_system_platform_type(void)
{
    return SYSINFO_DEVICE_NAME;
}

int32_t HAL_Get_ip(char *ip)
{
    return 0;
}

int32_t HAL_Get_mac(unsigned char mac[8])
{
    memset(mac, 0, 8);
    char default_value = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    memcpy(mac, default_value, 8);
    return 0;
}

int32_t HAL_network_status_registercb(void (*cb)(int status, void *), void *arg)
{
    return 0;
}

int32_t HAL_Get_network_status(void)
{
    return 1;
}

int HAL_GetWirelessInfo(struct _amp_wireless_info_t *info)
{
	return 0;
}

const char *HAL_Get_Device_Name(void)
{
    return "WO4Iu2FJokrZ4Ku7KyQ6";
}

int32_t HAL_Heap_Memory_Info(amp_heap_info_t *heap_info)
{
    int32_t ret = -1;

    struct rlimit rlim;
    if(getrlimit(RLIMIT_STACK, &rlim) != 0){
        perror("getrlimit failed\r\n");
        return;
    }

    heap_info->heap_total = (size_t)rlim.rlim_cur;
    heap_info->heap_used = heap_usage.heap_used;

    // printf("heap_total:%ld, heap_used:%ld\r\n",heap_info->heap_total, heap_info->heap_used);
    return 0;
}

int32_t HAL_Sysinit(void)
{
    return 0;
}

const char *HAL_Get_Module_Hardware_Version(void)
{
    return "Module_Hardware_version";
}

const char *HAL_Get_Module_Software_Version(void)
{
    return "Module_Software_version";
}
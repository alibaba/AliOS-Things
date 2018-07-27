/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "ota_hal_os.h"
#include "ota_log.h"
#include "crc.h"
#include "iot_export.h"
#include "iot_export_coap.h"
#include "mqtt_instance.h"

#ifdef _PLATFORM_IS_LINUX_
#include <unistd.h>
#include <sys/reboot.h>
#include <semaphore.h>
#include <pthread.h>
#else
#include <aos/aos.h>
#include <aos/yloop.h>
#include <hal/hal.h>
#endif

#define DEFAULT_THREAD_PRI   10

const char *iotx_ca_get(void);
int  IOT_CoAP_ParseOption_block(void *p_message, int type, unsigned int *num, unsigned int *more, unsigned int *size);
int IOT_CoAP_SendMessage_block(iotx_coap_context_t *p_context, char *p_path, iotx_message_t *p_message,
                               unsigned int block_type, unsigned int num, unsigned int more, unsigned int size);
#ifndef _IS_LINKKIT_
void *ota_malloc(uint32_t size)
{
    #ifndef _PLATFORM_IS_LINUX_
    return aos_malloc(size);
    #else
    return malloc(size);
    #endif
    
}

void ota_free(void *ptr)
{  
    #ifndef _PLATFORM_IS_LINUX_
    aos_free(ptr);
    #else
    free(ptr);
    #endif
}
#endif

void *ota_zalloc(uint32_t size)
{
    #ifndef _PLATFORM_IS_LINUX_
    return aos_zalloc(size);
    #else
    return calloc(size, 1);
    #endif
}

void *ota_realloc(void* ptr, uint32_t size)
{
    #ifndef _PLATFORM_IS_LINUX_
    return aos_realloc(ptr, size);
    #else
    return realloc(ptr, size);
    #endif
}
void *ota_mutex_init(void)
{
    #ifndef _PLATFORM_IS_LINUX_
    aos_mutex_t *mutex = (aos_mutex_t *)ota_malloc(sizeof(aos_mutex_t));
    if (NULL == mutex) {
        return NULL;
    }

    if (0 != aos_mutex_new(mutex)) {
        ota_free(mutex);
        return NULL;
    }
    #else
    pthread_mutex_t *mutex = (pthread_mutex_t *)ota_malloc(sizeof(pthread_mutex_t));
    if (NULL == mutex) {
        return NULL;
    }
    if (0 != pthread_mutex_init(mutex, NULL)) {
        ota_free(mutex);
        return NULL;
    }

    OTA_LOG_I("HAL_MutexCreate:%p\n", mutex);
    #endif

    return mutex;
}

int ota_mutex_lock(void *mutex)
{   
    #ifndef _PLATFORM_IS_LINUX_
    return aos_mutex_lock((aos_mutex_t *)mutex, AOS_WAIT_FOREVER);
    #else
    if (NULL == mutex) {
        return 0;
    }
    if (0 != pthread_mutex_lock((pthread_mutex_t *)mutex)) {
        OTA_LOG_E("lock mutex failed");
    }
    #endif
    return 0;
}

int ota_mutex_unlock(void *mutex)
{
    #ifndef _PLATFORM_IS_LINUX_
    return aos_mutex_unlock((aos_mutex_t *)mutex);
    #else
    if (NULL == mutex) {
        return 0;
    }
    if (0 != pthread_mutex_unlock((pthread_mutex_t *)mutex)) {
        OTA_LOG_E("unlock mutex failed");
    }
    #endif
    return 0;
}

void ota_mutex_destroy(void *mutex)
{
    #ifndef _PLATFORM_IS_LINUX_
    aos_mutex_free((aos_mutex_t *)mutex);
    aos_free(mutex);
    #else
    if(mutex == NULL) {
        OTA_LOG_E("mutex null.");
        return;
    }

    if (0 != pthread_mutex_destroy((pthread_mutex_t *)mutex)) {
        OTA_LOG_E("destroy mutex failed");
    }

    OTA_LOG_I("HAL_MutexDestroy:%p\n", mutex);
    ota_free(mutex);
    #endif
}

void *ota_semaphore_init(void)
{
    #ifndef _PLATFORM_IS_LINUX_
    aos_sem_t *sem = (aos_sem_t *)ota_malloc(sizeof(aos_sem_t));
    if (NULL == sem) {
        return NULL;
    }

    if (0 != aos_sem_new(sem, 0)) {
        ota_free(sem);
        return NULL;
    }
    #else
    sem_t *sem = (sem_t *)ota_malloc(sizeof(sem_t));
    if (NULL == sem) {
        return NULL;
    }

    if (0 != sem_init(sem, 0, 0)) {
        ota_free(sem);
        return NULL;
    }
    #endif
    return sem;
}

int ota_semaphore_wait( void *sem, int32_t timeout_ms)
{   
    #ifndef _PLATFORM_IS_LINUX_
    return aos_sem_wait((aos_sem_t *)sem, timeout_ms);
    #else
    if ((~0) == timeout_ms) {
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
    #endif
    return 0;
}

void ota_semaphore_post(void *sem)
{
    #ifndef _PLATFORM_IS_LINUX_
    aos_sem_signal((aos_sem_t *)sem);
    #else
    sem_post((sem_t *)sem);
    #endif
}

void ota_semaphore_destroy(void *sem)
{
    #ifndef _PLATFORM_IS_LINUX_
    aos_sem_free((aos_sem_t *)sem);
    aos_free(sem);
    #else
    sem_destroy((sem_t *)sem);
    free(sem);
    #endif
}

uint32_t ota_now_ms(void)
{
    uint32_t time_ms;
    #ifndef _PLATFORM_IS_LINUX_
    return aos_now_ms();
    #else
    struct timeval tv = { 0 };
    gettimeofday(&tv, NULL);
    time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    #endif
    return time_ms;

}

void ota_msleep(uint32_t ms)
{  
    #ifndef _PLATFORM_IS_LINUX_
    aos_msleep(ms);
    #else
    usleep(1000 * ms);
    #endif
}

void ota_reboot(void)
{
    #ifndef _PLATFORM_IS_LINUX_
    aos_reboot();
    #else
    reboot(0);
    #endif
}

#ifndef _PLATFORM_IS_LINUX_
typedef struct {
    aos_task_t task;
    int detached;
    void *arg;
    void *(*routine)(void *arg);
} task_context_t;

static void task_wrapper(void *arg)
{
    task_context_t *task = arg;

    task->routine(task->arg);

    if (task) {
        aos_free(task);
        task = NULL;
    }
}
#endif
#define DEFAULT_THREAD_NAME "AosThread"
#define DEFAULT_THREAD_SIZE 4096

int ota_thread_create(
    void **thread_handle,
    void *(*work_routine)(void *),
    void *arg,
    void* param,
    int *stack_used)
{
    int ret = -1;
    #ifndef _PLATFORM_IS_LINUX_
    char *tname;
    uint32_t ssiz;
    int detach_state = 0;
    hal_os_thread_param_t* hal_os_thread_param = (hal_os_thread_param_t*)param;
    if (hal_os_thread_param) {
        detach_state = hal_os_thread_param->detach_state;
    }
    if (!hal_os_thread_param || !hal_os_thread_param->name) {
        tname = DEFAULT_THREAD_NAME;
    } else {
        tname = hal_os_thread_param->name;
    }

    if (!hal_os_thread_param || hal_os_thread_param->stack_size == 0) {
        ssiz = DEFAULT_THREAD_SIZE;
    } else {
        ssiz = hal_os_thread_param->stack_size;
    }
    task_context_t *task = aos_malloc(sizeof(task_context_t));
    if (!task) {
        return -1;
    }
    memset(task, 0, sizeof(task_context_t));

    task->arg = arg;
    task->routine = work_routine;
    task->detached = detach_state;

    ret = aos_task_new_ext(&task->task,
                           tname, task_wrapper, task,
                           ssiz, DEFAULT_THREAD_PRI);

    *thread_handle = (void *)task;
    #else
    ret = pthread_create((pthread_t *)thread_handle, NULL, work_routine, arg);
    #endif
    return ret;
}

void ota_thread_exit(void *thread)
{
    #ifndef _PLATFORM_IS_LINUX_
    aos_task_exit(0);
    #else
    pthread_exit(0);
    #endif
}


#ifndef _PLATFORM_IS_LINUX_
#define OTA_KV_START  "ota.kv_%s"

int ota_kv_set(const char *key, const void *val, int len, int sync)
{
    int ret;
    int real_len = strlen(key) + strlen(OTA_KV_START) + 1;
    char *temp = aos_malloc(real_len);
    if (!temp) {
        return -1;
    }
    ota_snprintf(temp, real_len, OTA_KV_START, key);
    ret = aos_kv_set(temp, val, len, sync);
    aos_free(temp);
    return ret;
}

int ota_kv_get(const char *key, void *buffer, int *buffer_len)
{
    int ret;
    int real_len = strlen(key) + strlen(OTA_KV_START) + 1;
    char *temp = aos_malloc(real_len);
    if (!temp) {
        return -1;
    }
    ota_snprintf(temp, real_len, OTA_KV_START, key);
    ret = aos_kv_get(temp, buffer, buffer_len);
    aos_free(temp);
    return ret;
}

int ota_kv_del(const char *key)
{
    int ret = 0;
    int real_len = strlen(key) + strlen(OTA_KV_START) + 1;
    char *temp = aos_malloc(real_len);
    if (!temp) {
        return -1;
    }
    ota_snprintf(temp, real_len, OTA_KV_START, key);
    ret = aos_kv_del(temp);
    aos_free(temp);
    return ret;
}

int ota_kv_erase_all(void)
{
    return aos_kv_del_by_prefix("ota.kv_");
}

typedef struct {
    const char *name;
    int ms;
    aos_call_t cb;
    void *data;
} schedule_timer_t;


static void schedule_timer(void *p)
{
    if (p == NULL) {
        return;
    }

    schedule_timer_t *pdata = p;
    aos_post_delayed_action(pdata->ms, pdata->cb, pdata->data);
}

static void schedule_timer_cancel(void *p)
{
    if (p == NULL) {
        return;
    }

    schedule_timer_t *pdata =p;
    aos_cancel_delayed_action(pdata->ms, pdata->cb, pdata->data);
}

static void schedule_timer_delete(void *p)
{
    if (p == NULL) {
        return;
    }

    schedule_timer_t *pdata =p;
    aos_cancel_delayed_action(pdata->ms, pdata->cb, pdata->data);
    aos_free(p);
}

#define USE_YLOOP
void *ota_timer_create(const char *name, void (*func)(void *), void *user_data)
{
#ifdef USE_YLOOP
    schedule_timer_t  *timer = (schedule_timer_t *)aos_malloc(sizeof(schedule_timer_t));
    if(timer == NULL) {
        return NULL;
    }

    timer->name = name;
    timer->cb = func;
    timer->data = user_data;

    return timer;
#else
    return NULL;
#endif
}

int ota_timer_start(void *t, int ms)
{
#ifdef USE_YLOOP
    if(t == NULL) {
        return -1;
    }
    schedule_timer_t *timer = t;
    timer->ms = ms;
    return aos_schedule_call(schedule_timer, t);
#else
    return 0;
#endif
}

int ota_timer_stop(void *t)
{
#ifdef USE_YLOOP
    if(t == NULL) {
        return -1;
    }

    return aos_schedule_call(schedule_timer_cancel, t);
#else
    return 0;
#endif
}
int ota_timer_delete(void *timer)
{
#ifdef USE_YLOOP
    if(timer == NULL) {
        return -1;
    }
    return aos_schedule_call(schedule_timer_delete, timer);
#else
     return 0;
#endif
}
#else
#define KV_FILE_PATH            "./uota.kv"
#define ITEM_MAX_KEY_LEN        128
#define ITEM_MAX_VAL_LEN        256
#define ITEM_LEN                512

typedef struct {
    int flag;
    int val_len;
    int crc32;
    int reserved[ITEM_LEN - ITEM_MAX_KEY_LEN - ITEM_MAX_VAL_LEN - 12];
}kv_state_t;

typedef struct {
    char key[ITEM_MAX_KEY_LEN];
    char val[ITEM_MAX_VAL_LEN];
    kv_state_t state;
}kv_t;

static pthread_mutex_t mutex_kv = PTHREAD_MUTEX_INITIALIZER;
/* get file size and item num */
static int hal_fopen(FILE **fp, int *size, int *num)
{
    /* create an file to save the kv */
    if((*fp = fopen(KV_FILE_PATH, "a+")) == NULL)
    {
        OTA_LOG_E("fopen(create) %s error:%s\n", KV_FILE_PATH, strerror(errno));
        return -1;
    }

    fseek(*fp, 0L, SEEK_END);

    OTA_LOG_I("ftell:%d\n", (int)ftell(*fp));
    if((*size = ftell(*fp)) % ITEM_LEN)
    {
        OTA_LOG_E("%s is not an kv file\n", KV_FILE_PATH);
        fclose(*fp);
        return -1;
    }

    *num = ftell(*fp) / ITEM_LEN;

    fseek(*fp, 0L, SEEK_SET);

    OTA_LOG_I("file size:%d, block num:%d\n", *size, *num);

    return 0;
}
/* pthread mutex protect */
int ota_kv_set(const char *key, const void *val, int len, int sync)
{
    FILE *fp = NULL;
    int file_size = 0, block_num = 0,  ret = 0, cur_pos = 0;
    kv_t kv_item;
    int     i;

    /* check parameter */
    if(key == NULL || val == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&mutex_kv);
    if(hal_fopen(&fp, &file_size, &block_num) != 0)
    {
        goto _hal_set_error;
    }

    for(i = 0; i < block_num; i++)
    {
        memset(&kv_item, 0, sizeof(kv_t));
        cur_pos = ftell(fp);
        /* read an kv item(512 bytes) from file */
        if((ret = fread(&kv_item, 1, ITEM_LEN, fp)) != ITEM_LEN)
        {
            goto _hal_set_error;
        }

        /* key compared */
        if(strcmp(kv_item.key, key) == 0)
        {
            OTA_LOG_I("HAL_Kv_Set@key compared:%s\n", key);
            /* set value and write to file */
            memset(kv_item.val, 0, ITEM_MAX_VAL_LEN);
            memcpy(kv_item.val, val, len);
            kv_item.state.val_len = len;
            CRC32_Context ctx;
            CRC32_Init(&ctx);
            CRC32_Update(&ctx, val, len);
            CRC32_Final(&ctx, (uint32_t *)&kv_item.state.crc32);
            fseek(fp, cur_pos, SEEK_SET);
            fwrite(&kv_item, 1, ITEM_LEN, fp);

            goto _hal_set_ok;
        }
    }

    OTA_LOG_I("HAL_Kv_Set key:%s\n", key);
    /* key not compared, append an kv to file */
    memset(&kv_item, 0, sizeof(kv_t));
    strcpy(kv_item.key, key);
    memcpy(kv_item.val, val, len);
    kv_item.state.val_len = len;
    CRC32_Context ctx;
    CRC32_Init(&ctx);
    CRC32_Update(&ctx, val, len);
    CRC32_Final(&ctx, (uint32_t *)&kv_item.state.crc32);

    fseek(fp, 0L, SEEK_END);
    fwrite(&kv_item, 1, ITEM_LEN, fp);

    goto _hal_set_ok;

_hal_set_error:
    if(fp == NULL)
    {
        pthread_mutex_unlock(&mutex_kv);
        return -1;
    }
    OTA_LOG_E("read %s error:%s\n", KV_FILE_PATH, strerror(errno));
    fflush(fp);
    fclose(fp);
    pthread_mutex_unlock(&mutex_kv);

    return -1;
_hal_set_ok:
    fflush(fp);
    fclose(fp);
    pthread_mutex_unlock(&mutex_kv);

    return 0;
}

/* 0: get key ok
   -1: get key failed or read file error
*/
int ota_kv_get(const char *key, void *buffer, int *buffer_len)
{
    FILE *fp = NULL;
    int i;
    /* read from file */
    int file_size = 0, block_num = 0;
    kv_t kv_item;

    /* check parameter */
    if(key == NULL || buffer == NULL || buffer_len == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&mutex_kv);

    if(hal_fopen(&fp, &file_size, &block_num) != 0)
    {
        goto _hal_get_error;
    }

    for(i = 0; i < block_num; i++)
    {
        memset(&kv_item, 0, sizeof(kv_t));
        /* read an kv item(512 bytes) from file */
        if(fread(&kv_item, 1, ITEM_LEN, fp) != ITEM_LEN)
        {
            goto _hal_get_error;
        }

        /* key compared */
        if(strcmp(kv_item.key, key) == 0)
        {
            OTA_LOG_I("HAL_Kv_Get@key compared:%s\n", key);
            /* set value and write to file */
            *buffer_len = kv_item.state.val_len;
            memcpy(buffer, kv_item.val, *buffer_len);

            goto _hal_get_ok;
        }
    }

    OTA_LOG_I("can not find the key:%s\n", key);

    goto _hal_get_ok;

_hal_get_error:
    if(fp == NULL)
    {
        pthread_mutex_unlock(&mutex_kv);
        return -1;
    }
    OTA_LOG_E("read %s error:%s\n", KV_FILE_PATH, strerror(errno));
    fflush(fp);
    fclose(fp);
    pthread_mutex_unlock(&mutex_kv);

    return -1;
_hal_get_ok:
    fflush(fp);
    fclose(fp);
    pthread_mutex_unlock(&mutex_kv);

    return 0;
}

int ota_kv_del(const char *key)
{
    FILE *fp = NULL;
    int i;
    /* read from file */
    int file_size = 0, block_num = 0, cur_pos = 0;
    kv_t kv_item;
    kv_t kv_item_last;

    /* check parameter */
    if(key == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&mutex_kv);

    if(hal_fopen(&fp, &file_size, &block_num) != 0)
    {
        goto _hal_del_error;
    }

    for(i = 0; i < block_num; i++)
    {
        memset(&kv_item, 0, sizeof(kv_t));
        cur_pos = ftell(fp);

        /* read an kv item(512 bytes) from file */
        if(fread(&kv_item, 1, ITEM_LEN, fp) != ITEM_LEN)
        {
            goto _hal_del_error;
        }

        /* key compared */
        if(strcmp(kv_item.key, key) == 0)
        {
            /* use last kv item merge this kv item and delete the last kv item */
            OTA_LOG_I("HAL_Kv_Del@key compared:%s, cur_pos:%d\n", kv_item.key, cur_pos);

            /* fp pointer to last kv item */
            fseek(fp, -ITEM_LEN, SEEK_END);
            /* read an kv item(512 bytes) from file */
            if(fread(&kv_item_last, 1, ITEM_LEN, fp) != ITEM_LEN)
            {
                goto _hal_del_error;
            }

            OTA_LOG_I("last item key:%s, val:%s\n", kv_item_last.key, kv_item_last.val);

            /* pointer to currect kv item */
            fseek(fp, cur_pos, SEEK_SET);
            fwrite(&kv_item_last,1, ITEM_LEN, fp);
            goto _hal_del_ok;
        }
    }
    OTA_LOG_E("HAL_Kv_Del@ can not find the key:%s\n", key);
    goto _hal_del_ok;

_hal_del_error:
    if(fp == NULL)
    {
        pthread_mutex_unlock(&mutex_kv);
        return -1;
    }
    OTA_LOG_E("read %s error:%s\n", KV_FILE_PATH, strerror(errno));
    fflush(fp);
    fclose(fp);
    pthread_mutex_unlock(&mutex_kv);

    return -1;
_hal_del_ok:

    fflush(fp);
    fclose(fp);
    if(truncate(KV_FILE_PATH, file_size - ITEM_LEN) != 0)
    {
        OTA_LOG_E("truncate %s error:%s\n", KV_FILE_PATH, strerror(errno));
        pthread_mutex_unlock(&mutex_kv);
        return -1;
    }
    pthread_mutex_unlock(&mutex_kv);

    return 0;
}

int ota_kv_erase_all(void)
{
    pthread_mutex_lock(&mutex_kv);

    OTA_LOG_I("HAL_Erase_All_Kv\n");

    if(truncate(KV_FILE_PATH, 0) != 0)
    {
        OTA_LOG_E("truncate %s error:%s\n", KV_FILE_PATH, strerror(errno));
        pthread_mutex_unlock(&mutex_kv);
        return -1;
    }

    pthread_mutex_unlock(&mutex_kv);
    return 0;
}

void *ota_timer_create(const char *name, void (*func)(void *), void *user_data)
{
    timer_t *timer = NULL;
    struct sigevent ent;
    /* check parameter */
    if(func == NULL)    return NULL;
    timer = (timer_t *)malloc(sizeof(time_t));
    /* Init */
    memset(&ent, 0x00, sizeof(struct sigevent));
    /* create a timer */
    ent.sigev_notify = SIGEV_THREAD;
    ent.sigev_notify_function = (void (*)(union sigval))func;
    ent.sigev_value.sival_ptr = user_data;
    if(timer_create(CLOCK_MONOTONIC, &ent, timer) != 0) {
        OTA_LOG_E("timer_create");
        return NULL;
    }

    return (void *)timer;
}

int ota_timer_start(void *timer, int ms)
{
    struct itimerspec ts;
    /* check parameter */
    if(timer == NULL)   return -1;
    /* it_interval=0: timer run only once */
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    /* it_value=0: stop timer */
    ts.it_value.tv_sec = ms / 1000;
    ts.it_value.tv_nsec = (ms % 1000) * 1000;

    return timer_settime(*(timer_t *)timer, 0, &ts, NULL);
}

int ota_timer_stop(void *timer)
{
    struct itimerspec ts;
    /* check parameter */
    if(timer == NULL)   return -1;

    /* it_interval=0: timer run only once */
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    /* it_value=0: stop timer */
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;

    return timer_settime(*(timer_t *)timer, 0, &ts, NULL);
}

int ota_timer_delete(void *timer)
{
    int ret = 0;
    /* check parameter */
    if(timer == NULL)   return -1;
    ret = timer_delete(*(timer_t *)timer);

    free(timer);

    return ret;
}
#endif


extern void* mbedtls_ssl_connect(int socketfd, const char *ca_crt, uint32_t ca_crt_len);
extern int32_t mbedtls_ssl_send(void* ssl,  char *buf, uint32_t len);
extern int32_t mbedtls_ssl_recv(void* ssl,  char *buf, uint32_t len);
void* ota_ssl_connect(int socketfd, const char *ca_crt, uint32_t ca_crt_len) {
    return mbedtls_ssl_connect(socketfd, ca_crt, ca_crt_len);
}

int32_t ota_ssl_send(void* ssl,  char *buf, uint32_t len) {
    return mbedtls_ssl_send(ssl, buf, len);
}

int32_t ota_ssl_recv(void* ssl,  char *buf, uint32_t len) {
    return mbedtls_ssl_recv(ssl, buf, len);
}

int ota_HAL_GetProductKey(char product_key[PRODUCT_KEY_MAXLEN]){
    return HAL_GetProductKey(product_key);
}

int ota_HAL_GetDeviceName(char device_name[DEVICE_NAME_MAXLEN]){
    return HAL_GetDeviceName(device_name);
}

int ota_HAL_GetDeviceSecret(char ds[DEVICE_SECRET_MAXLEN]) {
    return HAL_GetDeviceSecret(ds);
}

#if (OTA_SIGNAL_CHANNEL) == 1
int ota_hal_mqtt_publish(char *topic, int qos, void *data, int len){
    return mqtt_publish(topic, qos, data, len);
}

int ota_hal_mqtt_subscribe(char *topic, void (*cb)(char *topic, int topic_len, void *payload, int payload_len, void *ctx), void *ctx){
    return mqtt_subscribe(topic,cb,ctx);
}

int ota_hal_mqtt_deinit_instance(void){
    return mqtt_deinit_instance();
}

int ota_hal_mqtt_init_instance(char *productKey, char *deviceName, char *deviceSecret, int maxMsgSize){
    return mqtt_init_instance(productKey, deviceName, deviceSecret, maxMsgSize);
}

void* ota_hal_iot_mqtt_field(void* ctx){
   while(1) {
       IOT_MQTT_Yield(mqtt_get_instance(), 200);
       HAL_SleepMs(1000);
   }
   return NULL;
}
#else
int ota_hal_mqtt_publish(char *topic, int qos, void *data, int len){
    return 0;
}

int ota_hal_mqtt_subscribe(char *topic, void (*cb)(char *topic, int topic_len, void *payload, int payload_len, void *ctx), void *ctx){
    return 0;
}

int ota_hal_mqtt_deinit_instance(void){
    return 0;
}

int ota_hal_mqtt_init_instance(char *productKey, char *deviceName, char *deviceSecret, int maxMsgSize){
    return 0;
}

void* ota_hal_iot_mqtt_field(void* ctx){
   return NULL;
}
#endif
const char *ota_iotx_ca_get(void){
   return iotx_ca_get();
}
#if (OTA_SIGNAL_CHANNEL) == 2
int ota_IOT_CoAP_SendMessage(void *p_context,   char *p_path, void *p_message){
    return IOT_CoAP_SendMessage(p_context, p_path, p_message);
}
int ota_IOT_CoAP_SendMessage_block(void *p_context, char *p_path, void *p_message,
                               unsigned int block_type, unsigned int num, unsigned int more, unsigned int size){
    return IOT_CoAP_SendMessage_block(p_context, p_path, p_message, block_type, num, more, size);
}
int ota_IOT_CoAP_ParseOption_block(void *p_message, int type, unsigned int *num,
                                unsigned int *more, unsigned int *size){
    return IOT_CoAP_ParseOption_block(p_message, type, num, more, size);
}
int ota_IOT_CoAP_GetMessagePayload(void *p_message, unsigned char **pp_payload, int *p_len){
    return IOT_CoAP_GetMessagePayload(p_message, pp_payload, p_len);
}
int ota_IOT_CoAP_GetMessageCode(void *p_message, void *p_resp_code){
    return IOT_CoAP_GetMessageCode(p_message, p_resp_code);
}
void *ota_IOT_CoAP_Init(void *p_config){
    return (void*)IOT_CoAP_Init(p_config);
}
int ota_IOT_CoAP_DeviceNameAuth(void *p_context) {
    return IOT_CoAP_DeviceNameAuth(p_context);
}
int ota_IOT_CoAP_Deinit(void **pp_context){
    IOT_CoAP_Deinit(pp_context);
    return 0;
}
#else
int ota_IOT_CoAP_SendMessage(void *p_context,   char *p_path, void *p_message){
    return 0;
}
int ota_IOT_CoAP_SendMessage_block(void *p_context, char *p_path, void *p_message,
                               unsigned int block_type, unsigned int num, unsigned int more, unsigned int size){
    return 0;
}
int ota_IOT_CoAP_ParseOption_block(void *p_message, int type, unsigned int *num,
                                unsigned int *more, unsigned int *size){
    return 0;
}
int ota_IOT_CoAP_GetMessagePayload(void *p_message, unsigned char **pp_payload, int *p_len){
    return 0;
}
int ota_IOT_CoAP_GetMessageCode(void *p_message, void *p_resp_code){
    return 0;
}
void *ota_IOT_CoAP_Init(void *p_config){
    return NULL;
}
int ota_IOT_CoAP_DeviceNameAuth(void *p_context) {
    return 0;
}
int ota_IOT_CoAP_Deinit(void **pp_context){
    return 0;
}
#endif

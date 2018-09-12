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


#ifndef BUILD_AOS
#ifdef _PLATFORM_IS_LINUX_
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/reboot.h>
#include "network.h"
#endif
#else
#include <aos/aos.h>
#include <aos/yloop.h>
#include <hal/hal.h>
#endif

int IOT_CoAP_ParseOption_block(void *p_message, int type, unsigned int *num,
                               unsigned int *more, unsigned int *size);
int IOT_CoAP_SendMessage_block(iotx_coap_context_t *p_context, char *p_path,
                               iotx_message_t *p_message,
                               unsigned int block_type, unsigned int num,
                               unsigned int more, unsigned int size);

#ifndef OTA_WITH_LINKKIT
/*Memory realloc*/
void *ota_realloc(void *ptr, uint32_t size)
{
#ifndef _PLATFORM_IS_LINUX_
    return (void *)aos_realloc(ptr, size);
#else
    return realloc(ptr, size);
#endif
}

/*Memory calloc*/
void *ota_calloc(uint32_t n, uint32_t size)
{
#ifndef _PLATFORM_IS_LINUX_
    return (void *)aos_calloc(n, size);
#else
    return calloc(n, size);
#endif
}

/*Reboot*/
void ota_reboot(void)
{
#ifndef _PLATFORM_IS_LINUX_
    aos_reboot();
#else
    reboot(0x1234567);
#endif
}

/*Memory malloc*/
void *ota_malloc(uint32_t size)
{
#ifndef _PLATFORM_IS_LINUX_
    return aos_malloc(size);
#else
    return malloc(size);
#endif
}

/*Memory free*/
void ota_free(void *ptr)
{
#ifndef _PLATFORM_IS_LINUX_
    aos_free(ptr);
#else
    free(ptr);
#endif
}

/*Mutex init*/
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
    pthread_mutex_t *mutex =
      (pthread_mutex_t *)ota_malloc(sizeof(pthread_mutex_t));
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

/*Mutex lock*/
void ota_mutex_lock(void *mutex)
{
    if (NULL == mutex) {
        OTA_LOG_E("mutex is NULL");
        return;
    }
#ifndef _PLATFORM_IS_LINUX_
    aos_mutex_lock((aos_mutex_t *)mutex, AOS_WAIT_FOREVER);
#else
    if (0 != pthread_mutex_lock((pthread_mutex_t *)mutex)) {
        OTA_LOG_E("lock mutex failed");
    }
#endif
}

/*Mutex unlock*/
void ota_mutex_unlock(void *mutex)
{
    if (NULL == mutex) {
        OTA_LOG_E("mutex is NULL");
        return;
    }
#ifndef _PLATFORM_IS_LINUX_
    aos_mutex_unlock((aos_mutex_t *)mutex);
#else
    if (0 != pthread_mutex_unlock((pthread_mutex_t *)mutex)) {
        OTA_LOG_E("unlock mutex failed");
    }
#endif
}

/*Mutex destroy*/
void ota_mutex_destroy(void *mutex)
{
    if (mutex == NULL) {
        OTA_LOG_E("mutex null.");
        return;
    }
#ifndef _PLATFORM_IS_LINUX_
    aos_mutex_free((aos_mutex_t *)mutex);
    aos_free(mutex);
#else
    if (0 != pthread_mutex_destroy((pthread_mutex_t *)mutex)) {
        OTA_LOG_E("destroy mutex failed");
    }

    OTA_LOG_I("HAL_MutexDestroy:%p\n", mutex);
    ota_free(mutex);
#endif
}

/*Semaphore init*/
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

/*Semaphore wait*/
int ota_semaphore_wait(void *sem, uint32_t timeout_ms)
{
#ifndef _PLATFORM_IS_LINUX_
    return aos_sem_wait((aos_sem_t *)sem, timeout_ms);
#else
    if ((~0) == timeout_ms) {
        sem_wait(sem);
        return 0;
    } else {
        struct timespec ts;
        int             s;
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

/*Semaphore post*/
void ota_semaphore_post(void *sem)
{
#ifndef _PLATFORM_IS_LINUX_
    aos_sem_signal((aos_sem_t *)sem);
#else
    sem_post((sem_t *)sem);
#endif
}

/*Semaphore destroy*/
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

/*Sleep ms*/
void ota_msleep(uint32_t ms)
{
#ifndef _PLATFORM_IS_LINUX_
    aos_msleep(ms);
#else
    usleep(1000 * ms);
#endif
}

#ifndef _PLATFORM_IS_LINUX_
typedef struct
{
    aos_task_t task;
    int        detached;
    void *     arg;
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
#define DEFAULT_THREAD_NAME "AOSThread"
#define DEFAULT_THREAD_SIZE 4096
#define DEFAULT_THREAD_PRI 32
/*Thread create*/
int ota_thread_create(void **thread_handle, void *(*work_routine)(void *),
                      void *arg, hal_os_thread_param_t *param, int *stack_used)
{
    int ret = -1;
#ifndef _PLATFORM_IS_LINUX_
    char *                 tname;
    uint32_t               ssiz;
    int                    detach_state        = 0;
    if (param) {
        detach_state = param->detach_state;
    }
    if (!param || !param->name) {
        tname = DEFAULT_THREAD_NAME;
    } else {
        tname = param->name;
    }

    if (!param || param->stack_size == 0) {
        ssiz = DEFAULT_THREAD_SIZE;
    } else {
        ssiz = param->stack_size;
    }
    task_context_t *task = aos_malloc(sizeof(task_context_t));
    if (!task) {
        return -1;
    }
    memset(task, 0, sizeof(task_context_t));

    task->arg      = arg;
    task->routine  = work_routine;
    task->detached = detach_state;

    ret = aos_task_new_ext(&task->task, tname, task_wrapper, task, ssiz,
                           DEFAULT_THREAD_PRI);

    *thread_handle = (void *)task;
#else
    ret = pthread_create((pthread_t *)thread_handle, NULL, work_routine, arg);
#endif
    return ret;
}

/*Thread exit*/
void ota_thread_exit(void *thread)
{
#ifndef _PLATFORM_IS_LINUX_
    aos_task_exit(0);
#else
    pthread_exit(0);
#endif
}

#ifndef _PLATFORM_IS_LINUX_
/*KV set*/
int ota_kv_set(const char *key, const void *val, int len, int sync)
{
    int ret = 0;
    ret     = aos_kv_set(key, val, len, sync);
    return ret;
}

/*KV get*/
int ota_kv_get(const char *key, void *buffer, int *buffer_len)
{
    int ret = 0;
    ret     = aos_kv_get(key, buffer, buffer_len);
    return ret;
}

typedef struct
{
    const char *name;
    int         ms;
    aos_call_t  cb;
    void *      data;
} schedule_timer_t;

static void schedule_timer(void *p)
{
    if (p == NULL) {
        return;
    }

    schedule_timer_t *pdata = p;
    aos_post_delayed_action(pdata->ms, pdata->cb, pdata->data);
}

/*Timer create*/
void *ota_timer_create(const char *name, void (*func)(void *), void *user_data)
{
    schedule_timer_t *timer =
      (schedule_timer_t *)aos_malloc(sizeof(schedule_timer_t));
    if (timer == NULL) {
        return NULL;
    }

    timer->name = name;
    timer->cb   = func;
    timer->data = user_data;

    return timer;
}

/*Timer start*/
int ota_timer_start(void *t, int ms)
{
    if (t == NULL) {
        return -1;
    }
    schedule_timer_t *timer = t;
    timer->ms               = ms;
    return aos_schedule_call(schedule_timer, t);
}
#else
#define KV_FILE_PATH "./uota.kv"
#define ITEM_MAX_KEY_LEN 128
#define ITEM_MAX_VAL_LEN 256
#define ITEM_LEN 512

typedef struct
{
    int flag;
    int val_len;
    int crc32;
    int reserved[ITEM_LEN - ITEM_MAX_KEY_LEN - ITEM_MAX_VAL_LEN - 12];
} kv_state_t;

typedef struct
{
    char key[ITEM_MAX_KEY_LEN];
    char val[ITEM_MAX_VAL_LEN];
    kv_state_t state;
} kv_t;

static pthread_mutex_t mutex_kv = PTHREAD_MUTEX_INITIALIZER;
/* get file size and item num */
static int hal_fopen(FILE **fp, int *size, int *num)
{
    /* create an file to save the kv */
    if ((*fp = fopen(KV_FILE_PATH, "a+")) == NULL) {
        OTA_LOG_E("fopen(create) %s error:%s\n", KV_FILE_PATH, strerror(errno));
        return -1;
    }

    fseek(*fp, 0L, SEEK_END);

    OTA_LOG_I("ftell:%d\n", (int)ftell(*fp));
    if ((*size = ftell(*fp)) % ITEM_LEN) {
        OTA_LOG_E("%s is not an kv file\n", KV_FILE_PATH);
        fclose(*fp);
        return -1;
    }

    *num = ftell(*fp) / ITEM_LEN;

    fseek(*fp, 0L, SEEK_SET);

    OTA_LOG_I("file size:%d, block num:%d\n", *size, *num);

    return 0;
}
/*KV set*/
int ota_kv_set(const char *key, const void *val, int len, int sync)
{
    FILE *fp = NULL;
    int file_size = 0, block_num = 0, ret = 0, cur_pos = 0;
    kv_t kv_item;
    int i;

    /* check parameter */
    if (key == NULL || val == NULL) {
        return -1;
    }

    pthread_mutex_lock(&mutex_kv);
    if (hal_fopen(&fp, &file_size, &block_num) != 0) {
        goto _hal_set_error;
    }

    for (i = 0; i < block_num; i++) {
        memset(&kv_item, 0, sizeof(kv_t));
        cur_pos = ftell(fp);
        /* read an kv item(512 bytes) from file */
        if ((ret = fread(&kv_item, 1, ITEM_LEN, fp)) != ITEM_LEN) {
            goto _hal_set_error;
        }

        /* key compared */
        if (strcmp(kv_item.key, key) == 0) {
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
    if (fp == NULL) {
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

/*KV get*/
int ota_kv_get(const char *key, void *buffer, int *buffer_len)
{
    FILE *fp = NULL;
    int i;
    /* read from file */
    int file_size = 0, block_num = 0;
    kv_t kv_item;
    /* check parameter */
    if (key == NULL || buffer == NULL || buffer_len == NULL) {
        return -1;
    }

    pthread_mutex_lock(&mutex_kv);
    if (hal_fopen(&fp, &file_size, &block_num) != 0) {
        goto _hal_get_error;
    }

    for (i = 0; i < block_num; i++) {
        memset(&kv_item, 0, sizeof(kv_t));
        /* read an kv item(512 bytes) from file */
        if (fread(&kv_item, 1, ITEM_LEN, fp) != ITEM_LEN) {
            goto _hal_get_error;
        }

        /* key compared */
        if (strcmp(kv_item.key, key) == 0) {
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
    if (fp == NULL) {
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

/*Timer create*/
void *ota_timer_create(const char *name, void (*func)(void *), void *user_data)
{
    timer_t *timer = NULL;
    struct sigevent ent;
    /* check parameter */
    if (func == NULL)
        return NULL;
    timer = (timer_t *)malloc(sizeof(time_t));
    /* Init */
    memset(&ent, 0x00, sizeof(struct sigevent));
    /* create a timer */
    ent.sigev_notify = SIGEV_THREAD;
    ent.sigev_notify_function = (void (*)(union sigval))func;
    ent.sigev_value.sival_ptr = user_data;
    if (timer_create(CLOCK_MONOTONIC, &ent, timer) != 0) {
        OTA_LOG_E("timer_create");
        return NULL;
    }

    return (void *)timer;
}

/*Timer start*/
int ota_timer_start(void *timer, int ms)
{
    struct itimerspec ts;
    /* check parameter */
    if (timer == NULL)
        return -1;
    /* it_interval=0: timer run only once */
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    /* it_value=0: stop timer */
    ts.it_value.tv_sec = ms / 1000;
    ts.it_value.tv_nsec = (ms % 1000) * 1000;

    return timer_settime(*(timer_t *)timer, 0, &ts, NULL);
}
#endif /*Linux end*/
#endif/*Linkkit end*/

/*Socket API*/
int ota_socket_connect(char *host, int port)
{
   return HAL_TCP_Establish(host, port);
}

int ota_socket_send(int fd, char *buf, size_t len)
{
   return HAL_TCP_Write((uintptr_t)fd, buf, len, OTA_SSL_TIMEOUT);
}

int ota_socket_recv(int fd,  char *buf, size_t len)
{
   return HAL_TCP_Read((uintptr_t)fd, buf, len, OTA_SSL_TIMEOUT);
}

void ota_socket_close(int fd)
{
   HAL_TCP_Destroy((uintptr_t)fd);
}

/*SSL connect*/
void *ota_ssl_connect(const char *host, uint16_t port, const char *ca_crt, uint32_t ca_crt_len)
{
    return (void*)HAL_SSL_Establish(host, port, ca_crt, ca_crt_len);
}

/*SSL send*/
int32_t ota_ssl_send(void *ssl, char *buf, uint32_t len)
{
    return HAL_SSL_Write((uintptr_t)ssl, buf, len, OTA_SSL_TIMEOUT);
}

/*SSL recv*/
int32_t ota_ssl_recv(void *ssl, char *buf, uint32_t len)
{
    return HAL_SSL_Read((uintptr_t)ssl, buf, len, OTA_SSL_TIMEOUT);
}

/*Get PK*/
int ota_HAL_GetProductKey(char pk[PRODUCT_KEY_MAXLEN])
{
    return HAL_GetProductKey(pk);
}

/*Get PS*/
int ota_HAL_GetProductSecret(char ps[PRODUCT_SECRET_MAXLEN])
{
    return HAL_GetProductSecret(ps);
}

/*Get DN*/
int ota_HAL_GetDeviceName(char dn[DEVICE_NAME_MAXLEN])
{
    return HAL_GetDeviceName(dn);
}

/*Get DS*/
int ota_HAL_GetDeviceSecret(char ds[DEVICE_SECRET_MAXLEN])
{
    return HAL_GetDeviceSecret(ds);
}

/*MQTT API*/
#if (OTA_SIGNAL_CHANNEL) == 1
int ota_hal_mqtt_publish(char *topic, int qos, void *data, int len)
{
    return mqtt_publish(topic, qos, data, len);
}

int ota_hal_mqtt_subscribe(char *topic,
                           void (*cb)(char *topic, int topic_len, void *payload,
                                      int payload_len, void *ctx),
                           void *ctx)
{
    return mqtt_subscribe(topic, cb, ctx);
}

int ota_hal_mqtt_deinit_instance(void)
{
    return mqtt_deinit_instance();
}

int ota_hal_mqtt_init_instance(char *productKey, char *deviceName,
                               char *deviceSecret, int maxMsgSize)
{
    return mqtt_init_instance(productKey, deviceName, deviceSecret, maxMsgSize);
}
#else
int ota_hal_mqtt_publish(char *topic, int qos, void *data, int len)
{
    return 0;
}

int ota_hal_mqtt_subscribe(char *topic,
                           void (*cb)(char *topic, int topic_len, void *payload,
                                      int payload_len, void *ctx),
                           void *ctx)
{
    return 0;
}

int ota_hal_mqtt_deinit_instance(void)
{
    return 0;
}

int ota_hal_mqtt_init_instance(char *productKey, char *deviceName,
                               char *deviceSecret, int maxMsgSize)
{
    return 0;
}
#endif

/*CoAP API*/
#if (OTA_SIGNAL_CHANNEL) == 2
int ota_IOT_CoAP_SendMessage(void *p_context, char *p_path, void *p_message)
{
    return IOT_CoAP_SendMessage(p_context, p_path, p_message);
}
int ota_IOT_CoAP_SendMessage_block(void *p_context, char *p_path,
                                   void *p_message, unsigned int block_type,
                                   unsigned int num, unsigned int more,
                                   unsigned int size)
{
    return IOT_CoAP_SendMessage_block(p_context, p_path, p_message, block_type,
                                      num, more, size);
}
int ota_IOT_CoAP_ParseOption_block(void *p_message, int type, unsigned int *num,
                                   unsigned int *more, unsigned int *size)
{
    return IOT_CoAP_ParseOption_block(p_message, type, num, more, size);
}
int ota_IOT_CoAP_GetMessagePayload(void *p_message, unsigned char **pp_payload,
                                   int *p_len)
{
    return IOT_CoAP_GetMessagePayload(p_message, pp_payload, p_len);
}
int ota_IOT_CoAP_GetMessageCode(void *p_message, void *p_resp_code)
{
    return IOT_CoAP_GetMessageCode(p_message, p_resp_code);
}
void *ota_IOT_CoAP_Init(void *p_config)
{
    return (void *)IOT_CoAP_Init(p_config);
}
int ota_IOT_CoAP_DeviceNameAuth(void *p_context)
{
    return IOT_CoAP_DeviceNameAuth(p_context);
}
int ota_IOT_CoAP_Deinit(void **pp_context)
{
    IOT_CoAP_Deinit(pp_context);
    return 0;
}
#else
int ota_IOT_CoAP_SendMessage(void *p_context, char *p_path, void *p_message)
{
    return 0;
}
int ota_IOT_CoAP_SendMessage_block(void *p_context, char *p_path,
                                   void *p_message, unsigned int block_type,
                                   unsigned int num, unsigned int more,
                                   unsigned int size)
{
    return 0;
}
int ota_IOT_CoAP_ParseOption_block(void *p_message, int type, unsigned int *num,
                                   unsigned int *more, unsigned int *size)
{
    return 0;
}
int ota_IOT_CoAP_GetMessagePayload(void *p_message, unsigned char **pp_payload,
                                   int *p_len)
{
    return 0;
}
int ota_IOT_CoAP_GetMessageCode(void *p_message, void *p_resp_code)
{
    return 0;
}
void *ota_IOT_CoAP_Init(void *p_config)
{
    return NULL;
}
int ota_IOT_CoAP_DeviceNameAuth(void *p_context)
{
    return 0;
}
int ota_IOT_CoAP_Deinit(void **pp_context)
{
    return 0;
}
#endif

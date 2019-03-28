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
#if !defined (AOS_COMP_OTA_BLE)
#include "mbedtls/sha256.h"
#include "mbedtls/md5.h"
#include "mbedtls/base64.h"
#endif

#if (OTA_SIGNAL_CHANNEL) == 1
#include "iot_export.h"
#include "iot_import.h"
#include "iot_export_coap.h"
#endif

#if (OTA_SIGNAL_CHANNEL) == 2
#include "iot_import.h"
#endif

#ifdef OTA_LINUX
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/reboot.h>
#else
#include "aos/kernel.h"
#include "aos/kv.h"
#endif

/*Memory realloc*/
void *ota_realloc(void *ptr, int size)
{
    return aos_realloc(ptr, size);
}

/*Memory calloc*/
void *ota_calloc(int n, int size)
{
    return aos_calloc(n, size);
}

/*Reboot*/
void ota_reboot(void)
{
    aos_reboot();
}

/*Memory malloc*/
void *ota_malloc(int size)
{
    return aos_malloc(size);
}

/*Memory free*/
void ota_free(void *ptr)
{
    aos_free(ptr);
}

/*Semaphore init*/
void *ota_semaphore_create(void)
{
#if !defined OTA_LINUX
    aos_sem_t *sem = (aos_sem_t *)ota_malloc(sizeof(aos_sem_t));
#else
    sem_t *sem = (sem_t *)ota_malloc(sizeof(sem_t));
#endif
    if (NULL == sem) {
        return NULL;
    }

    if (0 != aos_sem_new(sem, 0)) {
        ota_free(sem);
        return NULL;
    }
    return sem;
}

/*Semaphore wait*/
int ota_semaphore_wait(void *sem, int ms)
{
#if !defined OTA_LINUX
    return aos_sem_wait((aos_sem_t *)sem, ms);
#else
    if ((~0) == ms) {
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
            ts.tv_nsec += (ms % 1000) * 1000000;
            if (ts.tv_nsec >= 1000000000) {
                ts.tv_nsec -= 1000000000;
                s = 1;
            }
            ts.tv_sec += ms / 1000 + s;
        } while (((s = sem_timedwait(sem, &ts)) != 0) && errno == EINTR);
        return (s == 0) ? 0 : -1;
    }
    return 0;
#endif
}

/*Semaphore post*/
void ota_semaphore_post(void *sem)
{
#if !defined OTA_LINUX
    aos_sem_signal((aos_sem_t *)sem);
#else
    sem_post((sem_t *)sem);
#endif
}

/*Semaphore destroy*/
void ota_semaphore_destroy(void *sem)
{
#if !defined OTA_LINUX
    aos_sem_free((aos_sem_t *)sem);
    aos_free(sem);
#else
    sem_destroy((sem_t *)sem);
    free(sem);
#endif
}

/*Sleep ms*/
void ota_msleep(int ms)
{
#if !defined OTA_LINUX
    aos_msleep(ms);
#else
    usleep(1000 * ms);
#endif
}

#if !defined OTA_LINUX
typedef struct
{
    aos_task_t task;
    int       detached;
    void      *arg;
    void      *(*routine)(void *arg);
} task_context_t;

static void task_wrapper(void *arg)
{
    task_context_t *task = arg;
    if(task != NULL) {
        task->routine(task->arg);

        if (task != NULL) {
            aos_free(task);
            task = NULL;
        }
    }
}
#endif
#define OTA_THREAD_NAME "OTA_Thread"
#define OTA_THREAD_SIZE 4096
#define OTA_THREAD_PRI 30
/*Thread create*/
int ota_thread_create(void **thread_handle, void *(*work_routine)(void *), void *arg, void *pm, int stack_size)
{
    int ret = -1;
#if !defined OTA_LINUX
    char * tname = OTA_THREAD_NAME;
    if(stack_size <= 0) {
        stack_size = OTA_THREAD_SIZE;
    }
    task_context_t *task = aos_malloc(sizeof(task_context_t));
    if (!task) {
        return -1;
    }
    memset(task, 0, sizeof(task_context_t));
    task->arg      = arg;
    task->routine  = work_routine;

    ret = aos_task_new_ext(&task->task, tname, task_wrapper, task, stack_size, OTA_THREAD_PRI);
    *thread_handle = (void *)task;
#else
    ret = pthread_create((pthread_t *)thread_handle, NULL, work_routine, arg);
#endif
    return ret;
}

/*Thread exit*/
void ota_thread_exit(void *thread)
{
#if !defined OTA_LINUX
    aos_task_exit(0);
#else
    pthread_exit(0);
#endif
}

#if !defined OTA_LINUX
/*KV set*/
int ota_kv_set(const char *key, const void *val, int len, int sync)
{
    return aos_kv_set(key, val, len, sync);
}

/*KV get*/
int ota_kv_get(const char *key, void *buffer, int *len)
{
    return aos_kv_get(key, buffer, len);
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
} kv_state_t;

typedef struct
{
    char       key[ITEM_MAX_KEY_LEN];
    char       val[ITEM_MAX_VAL_LEN];
    kv_state_t state;
} kv_t;

static pthread_mutex_t mutex_kv = PTHREAD_MUTEX_INITIALIZER;
/* get file size and item num */
static int hal_fopen(FILE **fp, int *size, int *num)
{
    /* create an file to save the kv */
    if ((*fp = fopen(KV_FILE_PATH, "a+")) == NULL) {
        OTA_LOG_E("open err:%s\n", strerror(errno));
        return -1;
    }
    fseek(*fp, 0L, SEEK_END);
    if ((*size = ftell(*fp)) % ITEM_LEN) {
        fclose(*fp);
        return -1;
    }
    *num = ftell(*fp) / ITEM_LEN;
    fseek(*fp, 0L, SEEK_SET);
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
        goto ERR;
    }
    for (i = 0; i < block_num; i++) {
        memset(&kv_item, 0, sizeof(kv_t));
        cur_pos = ftell(fp);
        /* read an kv item(512 bytes) from file */
        if ((ret = fread(&kv_item, 1, ITEM_LEN, fp)) != ITEM_LEN) {
            goto ERR;
        }
        /* key compared */
        if (strcmp(kv_item.key, key) == 0) {
            /* set value and write to file */
            memset(kv_item.val, 0, ITEM_MAX_VAL_LEN);
            memcpy(kv_item.val, val, len);
            kv_item.state.val_len = len;
            fseek(fp, cur_pos, SEEK_SET);
            fwrite(&kv_item, 1, ITEM_LEN, fp);
            goto END;
        }
    }

    /* key not compared, append an kv to file */
    memset(&kv_item, 0, sizeof(kv_t));
    strcpy(kv_item.key, key);
    memcpy(kv_item.val, val, len);
    kv_item.state.val_len = len;
    fseek(fp, 0L, SEEK_END);
    fwrite(&kv_item, 1, ITEM_LEN, fp);
    goto END;
ERR:
    if (fp == NULL) {
        pthread_mutex_unlock(&mutex_kv);
        return -1;
    }
    OTA_LOG_E("read err:%s\n", strerror(errno));
    fflush(fp);
    fclose(fp);
    pthread_mutex_unlock(&mutex_kv);

    return -1;
END:
    fflush(fp);
    fclose(fp);
    pthread_mutex_unlock(&mutex_kv);
    return 0;
}

/*KV get*/
int ota_kv_get(const char *key, void *buffer, int *len)
{
    FILE *fp = NULL;
    int i;
    /* read from file */
    int file_size = 0, block_num = 0;
    kv_t kv_item;
    /* check parameter */
    if (key == NULL || buffer == NULL || len == NULL) {
        return -1;
    }
    pthread_mutex_lock(&mutex_kv);
    if (hal_fopen(&fp, &file_size, &block_num) != 0) {
        goto ERR;
    }
    for (i = 0; i < block_num; i++) {
        memset(&kv_item, 0, sizeof(kv_t));
        /* read an kv item(512 bytes) from file */
        if (fread(&kv_item, 1, ITEM_LEN, fp) != ITEM_LEN) {
            goto ERR;
        }
        /* key compared */
        if (strcmp(kv_item.key, key) == 0) {
            /* set value and write to file */
            *len = kv_item.state.val_len;
            memcpy(buffer, kv_item.val, *len);
            goto END;
        }
    }
    goto END;
ERR:
    if (fp == NULL) {
        pthread_mutex_unlock(&mutex_kv);
        return -1;
    }
    OTA_LOG_E("read err:%s\n", strerror(errno));
    fflush(fp);
    fclose(fp);
    pthread_mutex_unlock(&mutex_kv);
    return -1;
END:
    fflush(fp);
    fclose(fp);
    pthread_mutex_unlock(&mutex_kv);
    return 0;
}
#endif /*Linux end*/

#if !defined(AOS_COMP_OTA_BLE)
/*Socket API*/
void *ota_socket_connect(char *host, int port)
{
   return (void*)HAL_TCP_Establish(host, port);
}

int ota_socket_send(void* fd, char *buf, int len)
{
   return HAL_TCP_Write((uintptr_t)fd, buf, len, OTA_SSL_TIMEOUT);
}

int ota_socket_recv(void *fd, char *buf, int len)
{
   return HAL_TCP_Read((uintptr_t)fd, buf, len, OTA_SSL_TIMEOUT);
}

void ota_socket_close(void *fd)
{
   HAL_TCP_Destroy((uintptr_t)fd);
}

/*SSL connect*/
void *ota_ssl_connect(const char *host, unsigned short port, const char *ca_crt, int ca_crt_len)
{
    return (void*)HAL_SSL_Establish(host, port, ca_crt, ca_crt_len);
}

/*SSL send*/
int ota_ssl_send(void *ssl, char *buf, int len)
{
    return HAL_SSL_Write((uintptr_t)ssl, buf, len, OTA_SSL_TIMEOUT);
}

/*SSL recv*/
int ota_ssl_recv(void *ssl, char *buf, int len)
{
    return HAL_SSL_Read((uintptr_t)ssl, buf, len, OTA_SSL_TIMEOUT);
}
#endif

#if !defined (AOS_COMP_OTA_BLE)
/*SHA256*/
#if !defined(ESPOS_FOR_ESP32)
extern void mbedtls_sha256_free_alt(mbedtls_sha256_context *ctx);
extern void mbedtls_sha256_init_alt(mbedtls_sha256_context *ctx);
extern void mbedtls_sha256_starts_alt(mbedtls_sha256_context *ctx, int is224);
extern void mbedtls_sha256_update_alt(mbedtls_sha256_context*ctx, const unsigned char *input, size_t ilen);
extern void mbedtls_sha256_finish_alt(mbedtls_sha256_context *ctx, unsigned char output[32]);
#endif
void ota_sha256_free(ota_sha256_context *ctx)
{
#if !defined(ESPOS_FOR_ESP32)
    mbedtls_sha256_free_alt((mbedtls_sha256_context *)ctx);
#endif
}

void ota_sha256_init(ota_sha256_context *ctx)
{
#if !defined(ESPOS_FOR_ESP32)
    mbedtls_sha256_init_alt((mbedtls_sha256_context *)ctx);
#endif
}

void ota_sha256_starts(ota_sha256_context *ctx, int is224)
{
#if !defined(ESPOS_FOR_ESP32)
    mbedtls_sha256_starts_alt((mbedtls_sha256_context *)ctx, is224);
#endif
}

void ota_sha256_update(ota_sha256_context *ctx, const unsigned char *input, unsigned int ilen)
{
#if !defined(ESPOS_FOR_ESP32)
    mbedtls_sha256_update_alt((mbedtls_sha256_context *)ctx, input, ilen);
#endif
}

void ota_sha256_finish(ota_sha256_context *ctx, unsigned char output[32])
{
#if !defined(ESPOS_FOR_ESP32)
    mbedtls_sha256_finish_alt((mbedtls_sha256_context *)ctx, output);
#endif
}
/*MD5*/
extern void mbedtls_md5_free_alt(mbedtls_md5_context *ctx);
extern void mbedtls_md5_init_alt(mbedtls_md5_context *ctx);
extern void mbedtls_md5_starts_alt(mbedtls_md5_context *ctx);
extern void mbedtls_md5_update_alt(mbedtls_md5_context*ctx, const unsigned char *input, size_t ilen);
extern void mbedtls_md5_finish_alt(mbedtls_md5_context *ctx, unsigned char output[32]);

void ota_md5_free(ota_md5_context *ctx)
{
    mbedtls_md5_free_alt((mbedtls_md5_context *)ctx);
}

void ota_md5_init(ota_md5_context *ctx)
{
    mbedtls_md5_init_alt((mbedtls_md5_context *)ctx);
}

void ota_md5_starts(ota_md5_context *ctx)
{
    mbedtls_md5_starts_alt((mbedtls_md5_context *)ctx);
}

void ota_md5_update(ota_md5_context *ctx, const unsigned char *input, unsigned int ilen)
{
    mbedtls_md5_update_alt((mbedtls_md5_context *)ctx, input, ilen);
}

void ota_md5_finish(ota_md5_context *ctx, unsigned char output[16])
{
    mbedtls_md5_finish_alt((mbedtls_md5_context *)ctx, output);
}
/*RSA*/
extern int ali_rsa_get_pubkey_size(unsigned int keybits, unsigned int *size);
extern int ali_rsa_init_pubkey(unsigned int keybits, const unsigned char *n, unsigned int n_size,
                      const unsigned char *e, unsigned int e_size, ota_rsa_pubkey_t *pubkey);
extern int ali_rsa_verify(const ota_rsa_pubkey_t *pub_key, const unsigned char *dig, unsigned int dig_size,
                      const unsigned char *sig, unsigned int sig_size, ota_rsa_padding_t padding, bool *p_result);

int ota_rsa_get_pubkey_size(unsigned int keybits, unsigned int *size)
{
    return ali_rsa_get_pubkey_size(keybits, size);
}

int ota_rsa_init_pubkey(unsigned int keybits, const unsigned char *n, unsigned int n_size,
                      const unsigned char *e, unsigned int e_size, ota_rsa_pubkey_t *pubkey){
    return ali_rsa_init_pubkey(keybits, n, n_size, e, e_size, pubkey);
}

int ota_rsa_verify(const ota_rsa_pubkey_t *pub_key, const unsigned char *dig, unsigned int dig_size,
                      const unsigned char *sig, unsigned int sig_size, ota_rsa_padding_t padding, bool *p_result)
{
    return ali_rsa_verify(pub_key, dig, dig_size, sig, sig_size, padding, p_result);
}
/*base64*/
int ota_base64_decode(const unsigned char *src, unsigned int slen, unsigned char *dst, unsigned int *dlen)
{
    unsigned int len = 0;
    if(mbedtls_base64_decode(dst, *dlen, &len, src, slen) != 0) {
        return -1;
    }
    *dlen = len;
    return 0;
}
#endif

/*CRC16*/
static unsigned short update_crc16(unsigned short crcIn, unsigned char byte)
{
    unsigned int crc = crcIn;
    unsigned int in = byte | 0x100;

    do {
        crc <<= 1;
        in <<= 1;
        if (in & 0x100) {
            ++crc;
        }
        if (crc & 0x10000) {
            crc ^= 0x1021;
        }
    } while (!(in & 0x10000));
    return crc & 0xffffu;
}

void ota_crc16_init(ota_crc16_ctx *inCtx)
{
    inCtx->crc = 0;
}


void ota_crc16_update(ota_crc16_ctx *inCtx, const void *inSrc, unsigned int inLen)
{
    const unsigned char *src = (const unsigned char *) inSrc;
    const unsigned char *srcEnd = src + inLen;
    while (src < srcEnd) {
        inCtx->crc = update_crc16(inCtx->crc, *src++);
    }
}

void ota_crc16_final(ota_crc16_ctx *inCtx, unsigned short *outResult )
{
    inCtx->crc = update_crc16(inCtx->crc, 0);
    inCtx->crc = update_crc16(inCtx->crc, 0);
    *outResult = inCtx->crc & 0xffffu;
}

/*MQTT API*/
int ota_hal_mqtt_publish(char *topic, int qos, void *data, int len)
{
    #if (OTA_SIGNAL_CHANNEL) == 1
    return IOT_MQTT_Publish_Simple(NULL, topic, qos, data, len);
    #else
    return 0;
    #endif
}

int ota_hal_mqtt_subscribe(char *topic, void *cb, void *ctx)
{
    #if (OTA_SIGNAL_CHANNEL) == 1
    return IOT_MQTT_Subscribe_Sync(NULL, topic, 0, cb, ctx, 1000);
    #else
    return 0;
    #endif
}

int ota_hal_mqtt_deinit(void)
{
    #if (OTA_SIGNAL_CHANNEL) == 1
    return IOT_MQTT_Destroy(NULL);
    #else
    return 0;
    #endif
}

int ota_hal_mqtt_init(void)
{
    #if (OTA_SIGNAL_CHANNEL) == 1
    return (IOT_MQTT_Construct(NULL) == NULL) ? -1 : 0;
    #else
    return 0;
    #endif
}

/*CoAP API*/
int ota_coap_send(void *p_context, char *p_path, void *p_message)
{
    #if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_SendMessage(p_context, p_path, p_message);
    #else
    return 0;
    #endif
}

int ota_coap_parse_block(void *p_message, int type, int *num, int *more, int *size)
{
    #if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_ParseOption_block(p_message, type, num, more, size);
    #else
    return 0;
    #endif
}

int ota_coap_send_block(void *p_context, char *p_path, void *p_message, int block_type, int num, int more, int size)
{
    #if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_SendMessage_block(p_context, p_path, p_message, block_type,num, more, size);
    #else
    return 0;
    #endif
}

int ota_coap_get_payload(void *p_message, const char **pp_payload, int *p_len)
{
    #if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_GetMessagePayload(p_message, pp_payload, p_len);
    #else
    return 0;
    #endif
}

int ota_coap_get_code(void *p_message, void *p_resp_code)
{
    #if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_GetMessageCode(p_message, p_resp_code);
    #else
    return 0;
    #endif
}

int ota_coap_init(void)
{
    #if (OTA_SIGNAL_CHANNEL) == 2
    #define COAP_ONLINE_DTLS_SERVER_URL "coaps://%s.iot-as-coap.cn-shanghai.aliyuncs.com:5684"
    int ret = 0;
    iotx_coap_config_t   config;
    iotx_device_info_t   dev;
    memset(&config, 0, sizeof(config));
    memset(&dev, 0, sizeof(dev));
    strncpy(dev.device_id, get_ota_service()->ps, sizeof(dev.device_id) - 1);
    strncpy(dev.product_key, get_ota_service()->pk, sizeof(dev.product_key) - 1);
    strncpy(dev.device_name, get_ota_service()->dn, sizeof(dev.device_name) - 1);
    strncpy(dev.device_secret, get_ota_service()->ds, sizeof(dev.device_secret) - 1);
    config.p_devinfo = &dev;
    char url[256] = { 0 };
    ota_snprintf(url, sizeof(url), COAP_ONLINE_DTLS_SERVER_URL, get_ota_service()->pk);
    config.p_url = url;
    get_ota_service()->h_ch = (void*)ota_IOT_CoAP_Init(&config);
    if (get_ota_service()->h_ch) {
        ret = ota_IOT_CoAP_DeviceNameAuth(get_ota_service()->h_ch);
        if (ret < 0) {
            OTA_LOG_E("COAP error");
            return ret;
        }
    }
    #else
    return 0;
    #endif
}

int ota_coap_deinit(void)
{
    #if (OTA_SIGNAL_CHANNEL) == 2
    IOT_CoAP_Deinit(get_ota_service()->h_ch);
    #endif
    return 0;
}

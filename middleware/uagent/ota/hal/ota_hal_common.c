/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "ota_log.h"
#include "wrappers.h"
#include "infra_defs.h"
#include "ota_hal_common.h"

#if (OTA_SIGNAL_CHANNEL) == 2
#include "linkkit/coap_api.h"
#else
#include "linkkit/mqtt_api.h"
#endif

#if !defined (AOS_COMP_OTA_BLE)
#include "mbedtls/sha256.h"
#include "mbedtls/md5.h"
#include "mbedtls/base64.h"
#include "mbedtls/bignum.h"
#include "mbedtls/rsa.h"
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

int ota_to_capital(char *value, int len)
{
    int i = 0;
    int ret = -1;
    if ((value != NULL) && (len > 0)) {
        ret = 0;
        for (; i < len; i++) {
            if (*(value + i) >= 'a' && *(value + i) <= 'z') {
                *(value + i) -= 'a' - 'A';
            }
        }
    }
    return ret;
}

int ota_hex2str(char *dest_buf, const unsigned char *src_buf, unsigned int dest_len, unsigned int src_len)
{
    int i = 0;
    int ret = -1;
    if((dest_buf != NULL) && (src_buf != NULL) && (dest_len > 2 * src_len)) {
        ret = 0;
        memset(dest_buf, 0x00, dest_len);
        for (i = 0; i < src_len; i++) {
           ota_snprintf(dest_buf + i * 2, 2 + 1, "%02X", src_buf[i]);
        }
    }
    return ret;
}

int ota_str2hex(const char *src, char *dest, unsigned int dest_len)
{
    int i, n = 0;
    int ret = -1;
    if((src != NULL) && (dest != NULL) && (strlen(src) % 2 == 0) && (dest_len >= strlen(src) / 2)) {
        ret = 0;
        for(i = 0; src[i]; i += 2) {
            if(src[i] >= 'A' && src[i] <= 'F') {
                dest[n] = src[i] - 'A' + 10;
            }
            else {
                dest[n] = src[i] - '0';
            }
            if(src[i + 1] >= 'A' && src[i + 1] <= 'F') {
                dest[n] = (dest[n] << 4) | (src[i + 1] - 'A' + 10);
            }
            else {
               dest[n] = (dest[n] << 4) | (src[i + 1] - '0');
            }
            ++n;
        }
    }
    return ret;
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

#if !defined (AOS_COMP_OTA_BLE)
void ota_sha256_free(ota_sha256_context *ctx)
{
    mbedtls_sha256_free((mbedtls_sha256_context *)ctx);
}

void ota_sha256_init(ota_sha256_context *ctx)
{
    mbedtls_sha256_init((mbedtls_sha256_context *)ctx);
}

void ota_sha256_starts(ota_sha256_context *ctx, int is224)
{
    mbedtls_sha256_starts((mbedtls_sha256_context *)ctx, is224);
}

void ota_sha256_update(ota_sha256_context *ctx, const unsigned char *input, unsigned int ilen)
{
    mbedtls_sha256_update((mbedtls_sha256_context *)ctx, input, ilen);
}

void ota_sha256_finish(ota_sha256_context *ctx, unsigned char output[32])
{
    mbedtls_sha256_finish((mbedtls_sha256_context *)ctx, output);
}
/*MD5*/

void ota_md5_free(ota_md5_context *ctx)
{
    mbedtls_md5_free((mbedtls_md5_context *)ctx);
}

void ota_md5_init(ota_md5_context *ctx)
{
    mbedtls_md5_init((mbedtls_md5_context *)ctx);
}

void ota_md5_starts(ota_md5_context *ctx)
{
    mbedtls_md5_starts((mbedtls_md5_context *)ctx);
}

void ota_md5_update(ota_md5_context *ctx, const unsigned char *input, unsigned int ilen)
{
    mbedtls_md5_update((mbedtls_md5_context *)ctx, input, ilen);
}

void ota_md5_finish(ota_md5_context *ctx, unsigned char output[16])
{
    mbedtls_md5_finish((mbedtls_md5_context *)ctx, output);
}
/*RSA*/
int ota_rsa_pubkey_verify(const unsigned char *pubkey_n,
                          const unsigned char *pubkey_e,
                          unsigned int pubkey_n_size,
                          unsigned int pubkey_e_size,
                          const unsigned char *dig,
                          unsigned int dig_size,
                          const unsigned char *sig,
                          unsigned int sig_size)
{
    int                   ret = 0;
    mbedtls_rsa_context   ctx;

    if (pubkey_n == NULL || pubkey_n == NULL || dig == NULL || sig == NULL) {
        ret = -1;
        goto OTA_RSA_PUBKEY_VERIFY_OVER;
    }
    if (pubkey_n_size == 0 || pubkey_e_size == 0 || sig_size == 0 || dig_size != OTA_SHA256_HASH_SIZE) {
        ret = -1;
        goto OTA_RSA_PUBKEY_VERIFY_OVER;
    }
    mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_SHA256);
    ret = mbedtls_mpi_read_binary(&ctx.N, pubkey_n, pubkey_n_size);
    if (0 != ret) {
       goto OTA_RSA_PUBKEY_VERIFY_OVER;
    }
    ret = mbedtls_mpi_read_binary(&ctx.E, pubkey_e, pubkey_e_size);
    if (0 != ret) {
        goto OTA_RSA_PUBKEY_VERIFY_OVER;
    }
    ctx.len = pubkey_n_size;
    ret = mbedtls_rsa_check_pubkey(&ctx);
    if (0 != ret) {
        OTA_LOG_E("rsa key invalid(%08x)\n", ret);
        goto OTA_RSA_PUBKEY_VERIFY_OVER;
    }
    ret = mbedtls_rsa_pkcs1_verify(&ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256,(unsigned int)0, (const unsigned char *)dig, (const unsigned char *)sig);
    if (0 != ret) {
        OTA_LOG_E("ali_algo_rsa_pkcs1_verify fail %d", ret);
        goto OTA_RSA_PUBKEY_VERIFY_OVER;
    }
OTA_RSA_PUBKEY_VERIFY_OVER:
    if(ret != 0) {
        OTA_LOG_E("ota:rsa_pubkey_verify_err = %x", ret);
    }
    mbedtls_rsa_free(&ctx);
    return ret;
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

unsigned short ota_get_data_crc16(const unsigned char *buf, unsigned int len)
{
    ota_crc16_ctx ctx;
    unsigned short crc16 = 0xffff;
    if((buf != NULL) && (len > 0)) {
        ota_crc16_init(&ctx);
        ota_crc16_update(&ctx, buf, len);
        ota_crc16_final(&ctx, &crc16);
    }
    return crc16;
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
int ota_coap_send(char *topic, void *message)
{
#if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_SendMessage(NULL, topic, message);
#else
    return 0;
#endif
}

int ota_coap_parse_block(void *message, int type, int *num, int *more, int *size)
{
#if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_ParseOption_block(message, type, num, more, size);
#else
    return 0;
#endif
}

int ota_coap_send_block(char *topic, void *message, int block_type, int num, int more, int size)
{
#if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_SendMessage_block(NULL, topic, message, block_type,num, more, size);
#else
    return 0;
#endif
}

int ota_coap_get_payload(void *message, const char **payload, int *len)
{
#if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_GetMessagePayload(message, payload, len);
#else
    return 0;
#endif
}

int ota_coap_get_code(void *message, void *code)
{
#if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_GetMessageCode(message, code);
#else
    return 0;
#endif
}

int ota_coap_init(void)
{
#if (OTA_SIGNAL_CHANNEL) == 2
    int ret = 0;
    void *h_ch = NULL;
    h_ch = (void*)ota_IOT_CoAP_Init(NULL);
    if (h_ch) {
        ret = ota_IOT_CoAP_DeviceNameAuth(h_ch);
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
    IOT_CoAP_Deinit(NULL);
#endif
    return 0;
}

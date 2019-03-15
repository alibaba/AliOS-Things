/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_PLATFORM_OS_H_
#define OTA_PLATFORM_OS_H_
#include "ota/ota_service.h"

#define OTA_SSL_TIMEOUT 5000
#define ota_snprintf snprintf

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#ifndef bool
#define bool char
#endif

#ifndef NULL
#define NULL 0
#endif


#define OTA_IMAGE_MD5_LEN          (16)
#define OTA_IMAGE_RESERVER_SIZE    (2)

typedef struct
{
    unsigned int   image_magic;
    unsigned int   image_size;
    unsigned char  image_md5_value[OTA_IMAGE_MD5_LEN];
    unsigned char  image_reserver[OTA_IMAGE_RESERVER_SIZE];
    unsigned short image_crc16;
} ota_image_t;

/*memory*/
void *ota_malloc(int size);
void *ota_realloc(void *ptr, int size);
void *ota_calloc(int n, int size);
void ota_free(void *ptr);

/*Semaphore*/
void *ota_semaphore_create(void);
int ota_semaphore_wait(void *sem, int ms);
void ota_semaphore_post(void *sem);
void ota_semaphore_destroy(void *sem);

/*Thread*/
int ota_thread_create(
    void **thread_handle,
    void *(*work_routine)(void *),
    void *arg,
    void *param,
    int  stack_size);
void ota_thread_exit(void *thread);
void ota_msleep(int ms);

/*KV store*/
int ota_kv_set(const char *key, const void *val, int len, int sync);
int ota_kv_get(const char *key, void *buffer, int *len);

/*Reboot*/
void ota_reboot(void);

/*Socket API*/
void* ota_socket_connect(char *host, int port);
int ota_socket_send(void* fd,  char *buf, int len);
int ota_socket_recv(void* fd,  char *buf, int len);
void ota_socket_close(void* fd);

/*SSL*/
void* ota_ssl_connect(const char *host, unsigned short port, const char *ca, int len);
int ota_ssl_send(void* ssl,  char *buf, int len);
int ota_ssl_recv(void* ssl,  char *buf, int len);

/*Verify API*/
typedef struct
{
    unsigned int total[2];
    unsigned int state[4];
    unsigned char buffer[64];
}ota_md5_context;
typedef struct {
    unsigned int total[2];
    unsigned int state[8];
    unsigned char buffer[64];
    int is224;
}ota_sha256_context;
/*SHA256*/
void ota_sha256_free(ota_sha256_context *ctx);
void ota_sha256_init(ota_sha256_context *ctx);
void ota_sha256_starts(ota_sha256_context *ctx, int is224);
void ota_sha256_update(ota_sha256_context *ctx, const unsigned char *input, unsigned int ilen);
void ota_sha256_finish(ota_sha256_context *ctx, unsigned char output[32]);
/*MD5*/
void ota_md5_free(ota_md5_context *ctx);
void ota_md5_init(ota_md5_context *ctx);
void ota_md5_starts(ota_md5_context *ctx);
void ota_md5_update(ota_md5_context *ctx, const unsigned char *input, unsigned int ilen);
void ota_md5_finish(ota_md5_context *ctx, unsigned char output[16]);
/*CRC16*/
typedef struct {
    unsigned short crc;
} ota_crc16_ctx;
void ota_crc16_init(ota_crc16_ctx *ctx);
void ota_crc16_update(ota_crc16_ctx *ctx, const void *inSrc, unsigned int inLen);
void ota_crc16_final(ota_crc16_ctx *ctx, unsigned short *outResult);
/*Base64*/
int ota_base64_decode(const unsigned char *input, unsigned int input_len, unsigned char *output, unsigned int *output_len);
/*RSA*/
#define HASH_NONE OTA_HASH_NONE
#define SHA256 OTA_SHA256
#define MD5 OTA_MD5
#define RSASSA_PKCS1_V1_5 OTA_RSASSA_PKCS1_V1_5
typedef enum {
    HASH_NONE   = 0,
    SHA256      = 3,
    MD5         = 6,
} OTA_HASH_E;
typedef enum {
    RSASSA_PKCS1_V1_5       = 20,
} ota_rsa_pad_type_t;

typedef struct{
    ota_rsa_pad_type_t type;
    union {
        struct {
            OTA_HASH_E type;
        } rsaes_oaep;
        struct {
            OTA_HASH_E type;
        } rsassa_v1_5;
        struct {
            OTA_HASH_E type;
            unsigned int salt_len;
        } rsassa_pss;
    } pad;
} ota_rsa_padding_t;

#define TEE_MIN_RSA_KEY_SIZE      (256)
#define TEE_MAX_RSA_KEY_SIZE      (2048)

typedef struct{
    unsigned int magic;
    unsigned int n_size;
    unsigned int e_size;
    unsigned char n[(TEE_MAX_RSA_KEY_SIZE >> 3)];
    unsigned char e[(TEE_MAX_RSA_KEY_SIZE >> 3)];
} ota_rsa_pubkey_t;
int ota_rsa_get_pubkey_size(unsigned int keybits, unsigned int *size);
int ota_rsa_init_pubkey(unsigned int keybits, const unsigned char *n, unsigned int n_size,
                      const unsigned char *e, unsigned int e_size, ota_rsa_pubkey_t *pubkey);
/*MQTT*/
int ota_hal_mqtt_publish(char *topic, int qos, void *data, int len);
int ota_hal_mqtt_subscribe(char *topic, void* cb, void *ctx);
int ota_hal_mqtt_deinit(void);
int ota_hal_mqtt_init(void);

/*COAP*/
int ota_coap_send(void *p_context, char *p_path, void *p_message);
int ota_coap_send_block(void *p_context, char *p_path, void *p_message,
                                  int block_type, int num, int more, int size);
int ota_coap_parse_block(void *p_message, int type, int *num,int *more, int *size);
int ota_coap_get_payload(void *p_message, const char **pp_payload, int *p_len);
int ota_coap_get_code(void *p_message, void *p_resp_code);
int ota_coap_init(void);
int ota_coap_deinit(void);
#endif /*OTA_PLATFORM_OS_H*/


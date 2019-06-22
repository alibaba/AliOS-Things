/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __OTA_HAL_COMMON_H__
#define __OTA_HAL_COMMON_H__

#define ota_snprintf snprintf
#define OTA_SHA256_HASH_SIZE            (32)
#define OTA_MD5_HASH_SIZE               (16)
#define OTA_IMAGE_INFO_RESERVER_SIZE    (2)

#define OTA_MD5_TOTAL_SIZE              (2)
#define OTA_MD5_STATE_SIZE              (4)
#define OTA_MD5_BUF_SIZE                (64)

#define OTA_SHA256_TOTAL_SIZE           (2)
#define OTA_SHA256_STATE_SIZE           (8)
#define OTA_SHA256_BUF_SIZE             (64)

typedef struct
{ 
    unsigned int   image_magic;
    unsigned int   image_size;
    unsigned char  image_md5_value[OTA_MD5_HASH_SIZE];
    unsigned char  image_reserver[OTA_IMAGE_INFO_RESERVER_SIZE];
    unsigned short image_crc16;
} ota_image_t;

typedef struct {
    unsigned short crc;
} ota_crc16_ctx;

/*Verify API*/
typedef struct
{
    unsigned int  total[OTA_MD5_TOTAL_SIZE];
    unsigned int  state[OTA_MD5_STATE_SIZE];
    unsigned char buffer[OTA_MD5_BUF_SIZE];
} ota_md5_context;

typedef struct {
    unsigned int  total[OTA_SHA256_TOTAL_SIZE];
    unsigned int  state[OTA_SHA256_STATE_SIZE];
    unsigned char buffer[OTA_SHA256_BUF_SIZE];
    int is224;
} ota_sha256_context;

/*memory*/
void ota_free(void *ptr);
void *ota_malloc(int size);
void *ota_calloc(int n, int size);
void *ota_realloc(void *ptr, int size);

/*string<-->hex*/
int ota_to_capital(char *value, int len);
int ota_str2hex(const char *src, char *dest, unsigned int dest_len);
int ota_hex2str(char *dest_buf, const unsigned char *src_buf, unsigned int dest_len, unsigned int src_len);

/*Semaphore*/
void *ota_semaphore_create(void);
void ota_semaphore_post(void *sem);
void ota_semaphore_destroy(void *sem);
int  ota_semaphore_wait(void *sem, int ms);

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

/*Verify API*/
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
void ota_crc16_init(ota_crc16_ctx *ctx);
void ota_crc16_update(ota_crc16_ctx *ctx, const void *inSrc, unsigned int inLen);
void ota_crc16_final(ota_crc16_ctx *ctx, unsigned short *outResult);
unsigned short ota_get_data_crc16(const unsigned char *buf, unsigned int len);
/*Base64*/
int ota_base64_decode(const unsigned char *input, unsigned int input_len, unsigned char *output, unsigned int *output_len);
/*RSA*/
int ota_rsa_pubkey_verify(const unsigned char *pubkey_n,
                          const unsigned char *pubkey_e,
                          unsigned int pubkey_n_size,
                          unsigned int pubkey_e_size,
                          const unsigned char *dig,
                          unsigned int dig_size,
                          const unsigned char *sig,
                          unsigned int sig_size);
/*MQTT*/
typedef struct {
    unsigned short  packet_id;
    unsigned char   qos;
    unsigned char   dup;
    unsigned char   retain;
    unsigned short  topic_len;
    unsigned int    payload_len;
    const char     *ptopic;
    const char     *payload;
} ota_mqtt_topic_t;

typedef enum {
    OTA_MQTT_EVENT_SUB_SUCCESS  = 3,
    OTA_MQTT_EVENT_PUB_RECEIVED = 12,
    OTA_MQTT_EVENT_BUF_OVERFLOW = 13,
} ota_mqtt_event_t;

typedef struct {
    ota_mqtt_event_t  event;
    ota_mqtt_topic_t *topic;
} ota_mqtt_msg_t;

int ota_hal_mqtt_init(void);
int ota_hal_mqtt_deinit(void);
int ota_hal_mqtt_subscribe(char *topic, void* cb, void *ctx);
int ota_hal_mqtt_publish(char *topic, int qos, void *data, int len);

/*COAP*/
typedef enum {
    COAP_CONTENT_TYPE_JSON,
    COAP_CONTENT_TYPE_CBOR,
} coap_content_type_e;

typedef enum {
    COAP_MESSAGE_CON = 0, /* confirmable message */
    COAP_MESSAGE_NON = 1, /* non-confirmable message */
} coap_msg_type_e;

typedef void (*coap_cb_t)(void *p_arg, void *p_message);

typedef struct {
    unsigned char       *p_payload;
    unsigned short       payload_len;
    coap_content_type_e  content_type;
    coap_msg_type_e      msg_type;
    void                *user_data;
    coap_cb_t            resp_callback;
} ota_coap_message_t;

int ota_coap_init(void);
int ota_coap_deinit(void);
int ota_coap_send(char *topic, void *message);
int ota_coap_get_code(void *message, void *code);
int ota_coap_get_payload(void *message, const char **payload, int *len);
int ota_coap_parse_block(void *message, int type, int *num,int *more, int *size);
int ota_coap_send_block(char *topic, void *message, int block_type, int num, int more, int size);
#endif /*__OTA_HAL_COMMON_H__*/


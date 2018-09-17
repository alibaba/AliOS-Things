/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef OTA_TRANSPORT_H_
#define OTA_TRANSPORT_H_
#include <stdint.h>


#define MSG_REPORT_LEN (256)
#define MSG_INFORM_LEN (128)
#define MSG_LEN_MAX    (2048)

#define OSC_COAP_URI_MAX_LEN      (135) /* IoTx CoAP uri maximal length */
#define OTA_VERSION_STR_LEN_MIN   (1)
#define OTA_VERSION_STR_LEN_MAX   (32)

#define OTA_MQTT_TOPIC_LEN (136)
#define OTA_URL_MAX_LEN (1024)
#define OTA_RESP_MAX_LEN (OTA_URL_MAX_LEN + 192)
#define MAX_MD5_LEN 34
#define MAX_SHA256_LEN 66
#define MAX_HASH_VALUE_LEN 66
#define MAX_SIGN_VALUE_LEN 256
#define OTA_MD5_LEN 32
#define OTA_SHA256_LEN 64
#undef MAX_VERSION_LEN
#define MAX_VERSION_LEN 64
#define MAX_ID_LEN 64

#ifndef bool
#define bool char
#endif

typedef struct
{
    const char *primary_version;
    const char *secondary_version;
    const char *product_type;
    const char *device_uuid;
} ota_request_params;

typedef struct
{
    char        primary_version[MAX_VERSION_LEN];
    char        secondary_version[MAX_VERSION_LEN];
    const char *product_type;
    char        download_url[OTA_URL_MAX_LEN];
    int         frimware_size;
    char        device_uuid[MAX_ID_LEN];
    char        hash_value[MAX_HASH_VALUE_LEN];
    char        hash_method;
    char        sign_enable;
    char        sign_hash_method;
    unsigned int sign_value_len;
    unsigned char sign_value[MAX_SIGN_VALUE_LEN];
} ota_response_params;

enum
{
    CLOUD_CONNECTED,
    CLOUD_DISCONNECTED,
    GET_DEVICE_STATUS,
    SET_DEVICE_STATUS,
    GET_DEVICE_RAWDATA,
    SET_DEVICE_RAWDATA,
    UPGRADE_DEVICE,
    CANCEL_UPGRADE_DEVICE,
    GET_SUB_DEVICE_STATUS,
    SET_SUB_DEVICE_STATUS,
    MAX_EVENT_TYPE,
};

typedef void (*ota_cloud_cb_t)(int event, const char *json_buffer);

typedef struct
{
    int (*init)(void);
    int8_t (*parse_request)(const char *request, int *buf_len,
                            ota_request_params *request_parmas);
    int8_t (*parse_response)(const char *buf, int buf_len,
                             ota_response_params *response_parmas);
    int8_t (*parse_cancel_response)(const char *response, int buf_len,
                                    ota_response_params *response_parmas);
    int8_t (*subscribe_upgrade)(ota_cloud_cb_t msgCallback);
    int8_t (*cancel_upgrade)(ota_cloud_cb_t msgCallback);
    int8_t (*publish_request)(ota_request_params *request_parmas);
    int8_t (*status_post)(int status, int percent);
    int8_t (*result_post)(void);
    const char *(*get_uuid)(void);
    int (*deinit)(void);
} ota_transport;

const void *get_ota_service_manager(void);
const void *ota_get_transport(int protcol);
const void *ota_get_transport_mqtt(void);
const void *ota_get_transport_coap(void);
#endif /* OTA_TRANSPORT_H_ */

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef OTA_TRANSPORT_H_
#define OTA_TRANSPORT_H_
#include <stdint.h>
#include <aos/aos.h>

#define OTA_URL_MAX_LEN  1024
#define OTA_RESP_MAX_LEN (OTA_URL_MAX_LEN+192)
#define MAX_MD5_LEN      34
#undef MAX_VERSION_LEN
#define MAX_VERSION_LEN  64
#define MAX_ID_LEN       64

typedef struct {
    const char *primary_version;
    const char *secondary_version;
    const char *product_type;
    const char *device_uuid;
} ota_request_params;

typedef struct {
    char primary_version[MAX_VERSION_LEN];
    char secondary_version[MAX_VERSION_LEN];
    const char *product_type;
    char download_url[OTA_URL_MAX_LEN];
    int frimware_size;
    char device_uuid[MAX_ID_LEN];
    char md5[MAX_MD5_LEN];
} ota_response_params;

void platform_ota_init( void *signal);

int8_t platform_ota_parse_requset(const char *request, int *buf_len, ota_request_params *request_parmas);

int8_t platform_ota_parse_response(const char *buf, int buf_len, ota_response_params *response_parmas);

int8_t platform_ota_parse_cancel_responce(const char *response, int buf_len, ota_response_params *response_parmas);

int8_t platform_ota_subscribe_upgrade(aos_cloud_cb_t msgCallback);

int8_t platform_ota_cancel_upgrade(aos_cloud_cb_t msgCallback);

int8_t platform_ota_publish_request(ota_request_params *request_parmas);

int8_t platform_ota_status_post(int status, int percent);

int8_t platform_ota_result_post(void);

const char *platform_ota_get_id(void);
#endif /* OTA_TRANSPORT_H_ */

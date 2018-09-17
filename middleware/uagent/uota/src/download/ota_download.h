/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef OTA_DOWNLOAD_H_
#define OTA_DOWNLOAD_H_
#include "ota_manifest.h"

typedef struct {
    int (*init)(void);
    int (*start)(char *url, ota_write_cb_t wcb, void *sign);
    int (*stop)(void);
    int (*check)(const char *buffer, const int32_t len);
}ota_download;

const void * ota_get_download_http(void);
const void * ota_get_download_coap(void);
const void * ota_get_download(int protcol);
#endif /* OTA_DOWNLOAD_H_ */

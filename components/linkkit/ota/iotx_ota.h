/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IOTX_OTA_H__
#define __IOTX_OTA_H__

#include "ota_api.h"

typedef struct  {
    const char *product_key;    /* point to product key */
    const char *device_name;    /* point to device name */

    uint32_t id;                /* message id */
    IOT_OTA_State_t state;      /* OTA state */
    IOT_OTA_Type_t type;        /* OTA Type */
    uint32_t size_last_fetched; /* size of last downloaded */
    uint32_t size_fetched;      /* size of already downloaded */
    uint32_t size_file;         /* size of file */
    char *purl;                 /* point to URL */
    char *version;              /* point to string */
    char md5sum[33];            /* MD5 string */

    void *md5;                  /* MD5 handle */
    void *sha256;               /* Sha256 handle */
    void *ch_signal;            /* channel handle of signal exchanged with OTA server */
    void *ch_fetch;             /* channel handle of download */

    /* cota */
    char *configId;
    uint32_t configSize;
    char *sign;
    char *signMethod;
    char *cota_url;
    char *getType;
    int err;                    /* last error code */
    ota_event_fpt  ota_event_cb;  
} OTA_Struct_t, *OTA_Struct_pt;

int iotx_ota_get_config(void *handle, const char *configScope, const char *getType,
                        const char *attributeKeys);

int iotx_req_image(void *handle, const char *version);

#endif  /* #ifndef __IOTX_OTA_H__ */



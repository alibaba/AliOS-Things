/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_OTA_H_
#define _DM_OTA_H_

typedef struct {
    void *ota_handle;
    char product_key[PRODUCT_KEY_MAXLEN];
    char device_name[DEVICE_NAME_MAXLEN];
} dm_ota_ctx_t;

int dm_ota_init(void);
int dm_ota_sub(void);
int dm_ota_deinit(void);
int dm_ota_get_ota_handle(void **handle);

#endif
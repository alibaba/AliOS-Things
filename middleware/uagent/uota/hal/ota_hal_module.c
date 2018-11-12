/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "ota_log.h"
#include "ota_hal_plat.h"

static ota_hal_module_t *ota_module = NULL;

void ota_hal_register_module(ota_hal_module_t *module)
{
    ota_module = module;
}

int ota_hal_init(void *something)
{
    if (ota_module == NULL) {
        OTA_LOG_I("ota init is NULL");
        return -1;
    }

    if (ota_module != NULL && ota_module->init != NULL) {
        return ota_module->init(something);
    }
    return 0;
}

int ota_hal_write(int *off_set, char *in_buf , int in_buf_len)
{
    if (ota_module == NULL) {
        OTA_LOG_I("ota write is NULL");
        return -1;
    }

    if (ota_module != NULL && ota_module->write != NULL) {
        return ota_module->write(off_set, in_buf, in_buf_len);
    }

    return 0;
}

int ota_hal_read(int *off_set, char *out_buf, int out_buf_len)
{
    if (ota_module == NULL) {
        OTA_LOG_I("ota read is NULL");
        return -1;
    }

    if (ota_module != NULL && ota_module->read != NULL) {
        return ota_module->read(off_set, out_buf, out_buf_len);
    }

    return 0;
}

int ota_hal_boot(void *something)
{
    if (ota_module == NULL) {
        OTA_LOG_I("ota boot is NULL");
        return -1;
    }

    if (ota_module != NULL && ota_module->boot != NULL) {
        return ota_module->boot(something);
    }

    return 0;
}

int ota_hal_rollback(void *something)
{
    if (ota_module == NULL) {
        OTA_LOG_I("ota rollback is NULL");
        return -1;
    }

    if (ota_module != NULL && ota_module->rollback != NULL) {
        return ota_module->rollback(something);
    }

    return 0;
}

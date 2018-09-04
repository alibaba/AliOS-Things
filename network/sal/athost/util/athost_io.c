/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>
#include "athost_io.h"

#define TAG "athost_io"

static athost_io_t *g_athost_module = NULL;

int athost_io_register(athost_io_t *module)
{
    if (NULL == module) {
        LOGE(TAG, "athost module register invalid input\n");
        return -1;
    }

    if (NULL != g_athost_module) {
        LOGE(TAG, "athost module have already registered\n");
        return -1;
    }

    g_athost_module = module;

    return 0;
}

int athost_at_read(char *outbuf, uint32_t len)
{
    int ret = 0;

    if (NULL == g_athost_module) {
        LOGI(TAG, "athost module init fail for there is no athost module "
                  "registered yet \n");
        return 0;
    }

    if (NULL == g_athost_module->at_read) {
        LOGE(TAG, "at read function in athost module is null \n");
        return -1;
    }

    ret = g_athost_module->at_read(outbuf, len);
    if (ret < 0) {
        LOGE(TAG, "module read fail\n");
    }

    return ret;
}

int athost_at_write(const char *header, const uint8_t *data, uint32_t len,
                    const char *tailer)
{
    int ret = 0;

    if (NULL == g_athost_module) {
        LOGI(TAG, "athost module init fail for there is no athost module "
                  "registered yet \n");
        return 0;
    }

    if (NULL == g_athost_module->at_write) {
        LOGE(TAG, "at read function in athost module is null \n");
        return -1;
    }

    ret = g_athost_module->at_write(header, data, len, tailer);
    if (ret < 0) {
        LOGE(TAG, "module write fail\n");
    }

    return ret;
}

int athost_handle_register_cb(const char *prefix, athost_atcmd_handle_cb_t fn)
{
    int ret = 0;

    if (NULL == g_athost_module) {
        LOGI(TAG, "athost module init fail for there is no athost module "
                  "registered yet \n");
        return 0;
    }

    if (NULL == g_athost_module->at_write) {
        LOGE(TAG, "at read function in athost module is null \n");
        return -1;
    }

    ret = g_athost_module->at_handle_register_cb(prefix, fn);
    if (ret < 0) {
        LOGE(TAG, "module handle register cb fail\n");
    }

    return ret;
}

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <ulog/ulog.h>
#include <string.h>
#include "control_dev.h"

#define LOG_TAG    "[control_dev]"

ctrl_device_t *audio_ctrl_device_new(int id)
{
    ctrl_device_t *dev = NULL;
    dev = (ctrl_device_t *)malloc(sizeof(ctrl_device_t));
    if(!dev) {
        LOGE(LOG_TAG, "%s:%d, malloc dev failed!!!", __func__, __LINE__);
        return NULL;
    }

    memset(dev, 0, sizeof(ctrl_device_t));
    dev->id = id;
    snprintf(dev->name, sizeof(dev->name), "/dev/controlC%d", id);
    dlist_init(&dev->kcontrol_list);

    return dev;
}

int audio_ctrl_device_free(ctrl_device_t *dev)
{
    if(!dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null!!!", __func__, __LINE__);
        return -1;
    }
    dev->parent_data = NULL;
    free(dev);
    dev = NULL;
    return 0;
}
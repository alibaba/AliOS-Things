/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <ulog/ulog.h>
#include <string.h>
#include "pcm_dev.h"


#define LOG_TAG    "[pcm_dev]"

pcm_device_t * audio_pcm_device_new(int dirType, const char *name, pcm_device_ops_t *ops, void *private_data)
{
    pcm_device_t *dev = NULL;

    if((dirType < AUDIO_DEVICE_TYPE_PCM_CAPTURE) || (dirType > AUDIO_DEVICE_TYPE_PCM_PLAYBACK)) {
        LOGE(LOG_TAG, "%s:%d, invalid dirType %d", __func__, __LINE__, dirType);
        return NULL;
    }
    if(!name) {
        LOGE(LOG_TAG, "%s:%d, pcm name is null", __func__, __LINE__);
        return NULL;
    }
    
    dev = malloc(sizeof(pcm_device_t));
    if(!dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return NULL;
    }
    dev->dirType = dirType;
    dev->name = strdup(name);
    if(!dev->name) {
        LOGE(LOG_TAG, "%s:%d, strdup name failed", __func__, __LINE__);
        free(dev);
        return NULL;
    }
    dev->ops = ops;
    dev->private_data = private_data;
    dlist_init(&dev->list);

    return dev;
}

int audio_pcm_device_free(pcm_device_t *dev)
{
    if(!dev) {
        LOGE(LOG_TAG, "%s:%d, dev is null", __func__, __LINE__);
        return -EINVAL;
    }
    dlist_del(&dev->list);
    free(dev->name);
    free(dev);
    dev = NULL;
    return 0;
}

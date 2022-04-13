/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PCM_DEV_H
#define PCM_DEV_H

#include <stdio.h>
#include <aos/list.h>
#include "audio_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int (*open)(void *dev);
    int (*hw_params)(void *dev, audio_hw_params_t *params);
    int (*sw_params)(void *dev, audio_sw_params_t *params);
    int (*hw_prepare)(void *dev);
    int (*start)(void *dev);
    int (*readi)(void *dev, audio_xferi_t *xbuf);
    int (*writei)(void *dev, audio_xferi_t *xbuf);
    int (*readn)(void *dev, audio_xfern_t *xbuf);
    int (*writen)(void *dev, audio_xfern_t *xbuf);
    int (*drain)(void *dev);
    int (*pause)(void *dev, int enable);
    int (*stop)(void *dev);
    int (*close)(void *dev);
    int (*recover)(void *dev);
    int (*suspend)(void *dev);
    int (*resume)(void *dev);
} pcm_device_ops_t;

typedef struct {
    int dirType;
    char *name;
    audio_hw_params_t hwParams;
    pcm_device_ops_t *ops;
    audio_device_t *audioDevice;
    void *private_data;
    struct dlist_s list;
} pcm_device_t;

pcm_device_t *audio_pcm_device_new(int dirType, const char *name, pcm_device_ops_t *ops, void *private_data);
int audio_pcm_device_free(pcm_device_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* PCM_DEV_H */
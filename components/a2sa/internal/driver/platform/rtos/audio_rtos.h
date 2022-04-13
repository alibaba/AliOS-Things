/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AUDIO_RTOS_H
#define AUDIO_RTOS_H

#include <stdio.h>
#include <stdbool.h>
#include "audio_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* pcm_stream_handler_t;

typedef enum {
    PCM_STREAM_IN = 0,
    PCM_STREAM_OUT = 1,
} pcm_stream_dir_t;

typedef enum {
    /** Signed, 8-bit */
    PCM_STREAM_FORMAT_S8 = 1,
    /** Signed 16-bit, little endian */
    PCM_STREAM_FORMAT_S16_LE = 0,
    /** Signed, 16-bit, big endian */
    PCM_STREAM_FORMAT_S16_BE = 2,
    /** Signed, 24-bit (32-bit in memory), little endian */
    PCM_STREAM_FORMAT_S24_LE,
    /** Signed, 24-bit (32-bit in memory), big endian */
    PCM_STREAM_FORMAT_S24_BE,
    /** Signed, 24-bit, little endian */
    PCM_STREAM_FORMAT_S24_3LE,
    /** Signed, 24-bit, big endian */
    PCM_STREAM_FORMAT_S24_3BE,
    /** Signed, 32-bit, little endian */
    PCM_STREAM_FORMAT_S32_LE,
    /** Signed, 32-bit, big endian */
    PCM_STREAM_FORMAT_S32_BE,
    /** Max of the enumeration list, not an actual format. */
    PCM_STREAM_FORMAT_MAX
} pcm_stream_format_t;

typedef struct {
    pcm_stream_handler_t (*open)(int mode, int sampleRate, int channels, int format, aos_hdl_t *event_hdl);
    int (*start)(pcm_stream_handler_t hdl);
    int (*write)(pcm_stream_handler_t hdl, void *buf, unsigned int len);
    int (*read)(pcm_stream_handler_t hdl, void *buf, unsigned int len);
    int (*pause)(pcm_stream_handler_t hdl, int enable);
    int (*stop)(pcm_stream_handler_t hdl);
    int (*close)(pcm_stream_handler_t hdl);
    int (*recover)(pcm_stream_handler_t hdl);
    int (*suspend)(pcm_stream_handler_t hdl);
    int (*resume)(pcm_stream_handler_t hdl);
} pcm_stream_ops_t;

typedef struct {
    pcm_stream_dir_t mode;
    audio_hw_params_t params;
    audio_sw_params_t swParams;
    bool isOpenState;
    pcm_stream_ops_t *ops;
    pcm_stream_handler_t hdl;
    void *parent_data;
} native_pcm_device_t;

int audio_native_card_register(int rec_num, int spk_num, pcm_stream_ops_t *ops, const struct audio_kcontrol_new *controls, int controls_num);


#ifdef __cplusplus
}
#endif

#endif /* AUDIO_RTOS_H */
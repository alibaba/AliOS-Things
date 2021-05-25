/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_AUDIO_H__
#define __UVOICE_AUDIO_H__


#define VOLUME_LEVEL_MIN    0
#define VOLUME_LEVEL_MAX    10

typedef enum {
    SND_DEVICE_NONE = 0,
    SND_DEVICE_MIN = SND_DEVICE_NONE,
    SND_DEVICE_OUT_BEGIN,
    SND_DEVICE_OUT_SPEAKER = SND_DEVICE_OUT_BEGIN,
    SND_DEVICE_OUT_HEADPHONE,
    SND_DEVICE_OUT_RECEIVER,
    SND_DEVICE_OUT_HEADSET,
    SND_DEVICE_OUT_SPEAKER_AND_HEADPHONE,
    SND_DEVICE_OUT_SPEAKER_AND_HEADSET,
    SND_DEVICE_OUT_RECEIVER_AND_HEADPHONE,
    SND_DEVICE_OUT_RECEIVER_AND_HEADSET,
    SND_DEVICE_OUT_END = SND_DEVICE_OUT_RECEIVER_AND_HEADSET,
    SND_DEVICE_IN_BEGIN,
    SND_DEVICE_IN_PRIMARY_MIC = SND_DEVICE_IN_BEGIN,
    SND_DEVICE_IN_SECONDARY_MIC,
    SND_DEVICE_IN_TERTIARY_MIC,
    SND_DEVICE_IN_QUATERNARY_MIC,
    SND_DEVICE_IN_QUINARY_MIC,
    SND_DEVICE_IN_HEADSET_MIC,
    SND_DEVICE_IN_END = SND_DEVICE_IN_HEADSET_MIC,
    SND_DEVICE_MAX = SND_DEVICE_IN_END,
} snd_device_t;

typedef enum {
    PCM_MSG_TX_WAITING = 0,
    PCM_MSG_RX_WAITING,
    PCM_MSG_WRITE_DONE, /* write buffer copied */
    PCM_MSG_READ_DONE, /* read buffer filled */
    PCM_MSG_TX_UNDERRUN,
    PCM_MSG_TX_OVERRUN,
    PCM_MSG_RX_UNDERRUN,
    PCM_MSG_RX_OVERRUN,
} pcm_message_t;

enum {
    PCM_STATE_CLOSED = 0,
    PCM_STATE_SETUP,
    PCM_STATE_OPEN,
    PCM_STATE_RUNNING,
    PCM_STATE_STANDBY,
};

#ifdef UVOICE_ON_XR871
#include "audio/pcm/audio_pcm.h"
#include "audio/manager/audio_manager.h"
#else
#define PCM_OUT        0
#define PCM_IN        1

enum pcm_format {
    PCM_FORMAT_INVALID = -1,
    PCM_FORMAT_S16_LE = 0,  /* 16-bit signed */
    PCM_FORMAT_S32_LE,      /* 32-bit signed */
    PCM_FORMAT_S8,          /* 8-bit signed */
    PCM_FORMAT_S24_LE,      /* 24-bits in 4-bytes */
    PCM_FORMAT_S24_3LE,     /* 24-bits in 3-bytes */

    PCM_FORMAT_MAX,
};

struct pcm_config {
    int rate;
    int channels;
    int period_size;
    int period_count;
    enum pcm_format format;
};
#endif

struct pcm_device {
    struct pcm_config config;
    uint8_t dir:1;
    uint8_t card:2;
    uint8_t state:3;
    void *private_data;
};

struct external_pa_info {
    int used;
    int active_high;
    int pin;
    int delay_ms;
};

int audio_pcm_notify(pcm_message_t msg);

#ifdef UVOICE_BUILD_RELEASE
#define snd_debug(fmt, ...)
#define snd_info(fmt, ...)
#else
#define snd_debug(fmt, ...)        printf(fmt, ##__VA_ARGS__)
#define snd_info(fmt, ...)        printf(fmt, ##__VA_ARGS__)
#endif
#define snd_warn(fmt, ...)        printf(fmt, ##__VA_ARGS__)
#define snd_err(fmt, ...)        printf(fmt, ##__VA_ARGS__)


#endif /* __UVOICE_AUDIO_H__ */

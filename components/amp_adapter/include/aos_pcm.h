/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __AOS_PCM_H__
#define __AOS_PCM_H__

#define AOS_PCM_DIR_OUT 0
#define AOS_PCM_DIR_IN  1

typedef enum {
    AOS_PCM_FORMAT_INVALID = -1,
    AOS_PCM_FORMAT_S16_LE = 0,  /* 16-bit signed */
    AOS_PCM_FORMAT_S32_LE,      /* 32-bit signed */
    AOS_PCM_FORMAT_S8,          /* 8-bit signed */
    AOS_PCM_FORMAT_S24_LE,      /* 24-bits in 4-bytes */
    AOS_PCM_FORMAT_S24_3LE,     /* 24-bits in 3-bytes */
    AOS_PCM_FORMAT_MAX,
} aos_pcm_format_t;

typedef struct {
    int rate;
    int channels;
    int period_size;
    int period_count;
    aos_pcm_format_t format;
} aos_pcm_config_t;

typedef struct {
    aos_pcm_config_t config;
    unsigned char   dir:1;
    unsigned char   card:2;
    unsigned char   state:3;
    void *private_data;
} aos_pcm_device_t;

typedef enum {
    AOS_SND_DEVICE_OUT_SPEAKER = 1,
    AOS_SND_DEVICE_OUT_HEADPHONE,
    AOS_SND_DEVICE_OUT_HEADSET,
    AOS_SND_DEVICE_OUT_RECEIVER,
    AOS_SND_DEVICE_OUT_SPEAKER_AND_HEADPHONE,
    AOS_SND_DEVICE_OUT_SPEAKER_AND_HEADSET,
    AOS_SND_DEVICE_IN_MAIN_MIC,
    AOS_SND_DEVICE_IN_HEADSET_MIC,
    AOS_SND_DEVICE_MAX,
} aos_snd_device_t;

typedef struct {
    aos_snd_device_t out_device;
    int external_pa;
    int external_pa_pin;
    int external_pa_delay_ms;
    int external_pa_active_high;
    void *priv;
} aos_audio_dev_t;

/*
 * Set output volume.
 *
 * Return:
 *   0 -- success or unsupport
 *  -1 -- failed
 */
int aos_set_volume(aos_snd_device_t device, int volume);


/*
 * Select audio output/input device
 *
 * Return:
 *   0 -- success or unsupport
 *  -1 -- failed
 */
int aos_set_path(aos_pcm_device_t *pcm, aos_snd_device_t device);


/*
 * Mute audio output/input.
 *
 * Return:
 *   0 -- success or unsupport
 *  -1 -- failed
 */
int aos_dev_mute(aos_pcm_device_t *pcm, aos_snd_device_t device, int mute);


/**
 * Configure pcm parameters
 *
 * Return:
 *   0 -- success
 *  -1 -- failed
 */
int aos_pcm_setup(aos_pcm_device_t *pcm);

/**
 * Open pcm device
 *
 * Return:
 *   0 -- success
 *  -1 -- failed
 */
int aos_pcm_open(aos_pcm_device_t *pcm);

/**
 * Read data in. Block reading if input data not ready.
 *
 * Return read length, or negative if failed.
 */
int aos_pcm_read(aos_pcm_device_t *pcm, unsigned char *buffer, int nbytes);

/*
 * Block write if free dma buffer not ready, otherwise,
 * please return after copied.
 *
 * Return writen length, or negative if failed.
 *
 */
int aos_pcm_write(aos_pcm_device_t *pcm, unsigned char *buffer, int nbytes);

/*
 * Flush remaining data in dma buffer
 *
 * Return:
 *   0 -- success or unsupport
 *  -1 -- failed
 */
int aos_pcm_flush(aos_pcm_device_t *pcm);

/*
 * close pcm device
 *
 * Return:
 *   0 -- success
 *  -1 -- failed
 *
 */
int aos_pcm_close(aos_pcm_device_t *pcm);

#endif

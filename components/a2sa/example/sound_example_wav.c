/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifdef HAAS_AUDIO_DEMO
#include <posix/pthread.h>
#else
#include <pthread.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "sound_pcm.h"
#include "sound_mixer.h"
#include "audio_drv.h"
#include "ulog/ulog.h"

#define LOG_TAG  "[sound_example_wav]"

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define AUDIO_PLAYER_HIGH_STACKSIZE        8192
#define AUDIO_PLAYER_DEFAULT_PRIORITY      33

struct riff_wave_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t wave_id;
};

struct chunk_header {
    uint32_t id;
    uint32_t sz;
};

struct chunk_fmt {
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

static pthread_cond_t  g_play_cond;
static pthread_mutex_t  g_play_mutex;
static pthread_t g_play_thread;
static bool bCreateAudioThreadFlag = false;

static struct chunk_fmt chunk_fmt;
static unsigned int period_size = 3200;
static unsigned int period_count = 1;
static unsigned int total_cnt = 0;
static char *filename;

static int play_sample(FILE *file, unsigned int channels, unsigned int rate, unsigned int bits, unsigned int period_size,
                        unsigned int period_count)
{
    char *buffer;
    int size;
    int num_read, frame_size;
    int ret = -1;
    aos_pcm_t *pcm;
    aos_pcm_format_t format;

    LOGD(LOG_TAG, "%s:%d: channels %d, sample_rate %d, sample_bits %d, period_size %d, period_count %d", __func__, __LINE__,
                    channels, rate, bits, period_size, period_count);

    ret = aos_pcm_open(&pcm, "default", AOS_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        LOGE(LOG_TAG, "%s:%d, could NOT open audio device", __func__, __LINE__);
        return -1;
    }
    switch(bits) {
        case 16:
            format = AOSRV_PCM_FORMAT_S16_LE;
            break;
        case 24:
            format = AOSRV_PCM_FORMAT_S24_LE;
            break;
        case 32:
            format = AOSRV_PCM_FORMAT_S32_LE;
            break;
        default:
            format = AOSRV_PCM_FORMAT_S16_LE;
            break;
    }

    aos_pcm_set_params(pcm, format, AOS_PCM_ACCESS_RW_INTERLEAVED, channels, rate, 0, 0);
    aos_pcm_prepare(pcm);

    frame_size = channels * bits / 8;
    //size = period_size * period_count * frame_size;
    size = rate /10 * frame_size;
    LOGD(LOG_TAG, "%s:%d, size = %d", __func__, __LINE__, size);
    buffer = malloc(size);
    if (!buffer) {
        LOGE(LOG_TAG, "%s:%d: failed to malloc %d bytes", __func__, __LINE__, size);
        free(buffer);
        aos_pcm_close(pcm);
        return -1;
    }

    aos_pcm_start(pcm);
    do {
        num_read = fread(buffer, 1, size, file);
        if (num_read > 0) {
            if (aos_pcm_writei(pcm, buffer, num_read/frame_size) < 0) {
                LOGE(LOG_TAG, "%s:%d: aos_pcm_writei error. ", __func__, __LINE__);
                break;
            }
        }
    } while (num_read > 0);

    //aos_pcm_drain(pcm);
    aos_pcm_stop(pcm);
    aos_pcm_close(pcm);
    free(buffer);
    return 0;
}

static void sound_wav_thread(void *arg)
{
    FILE *file;
    struct riff_wave_header riff_wave_header;
    struct chunk_header chunk_header;
    int more_chunks = 1;

    while(1) {
        if(total_cnt <= 0) {
            LOGD(LOG_TAG, "%s:%d, wav player end !!!", __func__, __LINE__);
            pthread_cond_wait(&g_play_cond, &g_play_mutex);
        }
        total_cnt --;

        if(!filename) {
            total_cnt = 0;
            continue;
        }
        file = fopen(filename, "rb");
        if (!file) {
            LOGE(LOG_TAG, "%s:%d, failed to open file '%s'", __func__, __LINE__, filename);
            total_cnt = 0;
            continue;
        }
        LOGD(LOG_TAG, "%s:%d, open '%s' successfully", __func__, __LINE__, filename);

        fread(&riff_wave_header, sizeof(riff_wave_header), 1, file);
        if ((riff_wave_header.riff_id != ID_RIFF) ||
            (riff_wave_header.wave_id != ID_WAVE)) {
            LOGE(LOG_TAG, "%s:%d, Error: '%s' is not a riff/wave file", __func__, __LINE__, filename);
            total_cnt = 0;
            fclose(file);
            continue;
        }

        do {
            fread(&chunk_header, sizeof(chunk_header), 1, file);

            switch (chunk_header.id) {
            case ID_FMT:
                fread(&chunk_fmt, sizeof(chunk_fmt), 1, file);
                /* If the format header is larger, skip the rest */
                if (chunk_header.sz > sizeof(chunk_fmt))
                    fseek(file, chunk_header.sz - sizeof(chunk_fmt), SEEK_CUR);
                break;
            case ID_DATA:
                /* Stop looking for chunks */
                more_chunks = 0;
                break;
            default:
                /* Unknown chunk, skip bytes */
                fseek(file, chunk_header.sz, SEEK_CUR);
            }
        } while (more_chunks);

        if(play_sample(file, chunk_fmt.num_channels, chunk_fmt.sample_rate, chunk_fmt.bits_per_sample, period_size, period_count) < 0) {
            total_cnt = 0;
        }

        fclose(file);
    }

    return 0;
}

static void sound_wav_init(void)
{
    if(bCreateAudioThreadFlag) {
        return;
    }
    LOGD(LOG_TAG, "%s:%d, -->>", __func__, __LINE__);

    pthread_attr_t attr;
    struct sched_param sched;

    pthread_cond_init(&g_play_cond, NULL);
    pthread_mutex_init(&g_play_mutex, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, AUDIO_PLAYER_HIGH_STACKSIZE);
    sched.sched_priority = AUDIO_PLAYER_DEFAULT_PRIORITY;
    pthread_attr_setschedparam(&attr, &sched);

    pthread_create(&g_play_thread, &attr, sound_wav_thread, NULL);
    pthread_setname_np(g_play_thread, "wav_player_thread");

    pthread_attr_destroy(&attr);
    bCreateAudioThreadFlag = true;
}

void sound_example_wav_entry(int argc, char **argv)
{
    if (argc < 3) {
        LOGD(LOG_TAG, "%s:%d: Usage: %s file.wav cnt ", __func__, __LINE__, argv[0]);
        return;
    }
    filename = strdup(argv[1]);
    total_cnt = atoi(argv[2]);
    sound_wav_init();
    pthread_cond_signal(&g_play_cond);
}


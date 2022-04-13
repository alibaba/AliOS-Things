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
#include "audio_drv.h"
#include "ulog/ulog.h"

#define LOG_TAG  "[sound_example_lb]"
#define AUDIO_PLAYER_HIGH_STACKSIZE        8192
#define AUDIO_PLAYER_DEFAULT_PRIORITY      33

static char *param = NULL;
static pthread_cond_t  g_play_cond;
static pthread_mutex_t  g_play_mutex;
static pthread_t g_play_thread;
static bool bCreateAudioThreadFlag = false;

static void sound_loopback_thread(void *arg)
{
    aos_pcm_t *pb_pcm = NULL, *cap_pcm = NULL;
    unsigned char * dataBuf = NULL;
    unsigned int buf_size = 0;
    //int freq = 22050, channels = 1;
    int freq = 16000, channels = 1;
    int ret = -1;

    LOGD(LOG_TAG, "%s:%d: sound_loopback_thread entry!!", __func__, __LINE__);
    buf_size = freq * channels * 2 / 5;
    dataBuf = (unsigned char *)malloc(buf_size);
    if(!dataBuf) {
        LOGE(LOG_TAG, "%s:%d: malloc %d failed. ", __func__, __LINE__, buf_size);
        return 0;
    }
    while(1) {
        if(!strcmp(param, "start")) {
            if(!pb_pcm) {
                LOGD(LOG_TAG, "%s:%d: open capture & playback stream!!", __func__, __LINE__);
                /* open playback stream */
                ret = aos_pcm_open(&pb_pcm, "default", AOS_PCM_STREAM_PLAYBACK, 0);
                if(!ret) {
                    aos_pcm_hw_params_alloca(&pb_pcm->hw_params);
                    aos_pcm_sw_params_alloca(&pb_pcm->sw_params);
                    aos_pcm_sw_params_any(pb_pcm->sw_params);
                    aos_pcm_set_params(pb_pcm, AOSRV_PCM_FORMAT_S16_LE, AOS_PCM_ACCESS_RW_INTERLEAVED, channels, freq, 0, 0);
                    aos_pcm_prepare(pb_pcm);
                    aos_pcm_start(pb_pcm);
                } else {
                    LOGE(LOG_TAG, "%s:%d: open playback stream failed. ", __func__, __LINE__);
                }
            }
            if(!cap_pcm) {
                /* open capture stream */
                ret = aos_pcm_open(&cap_pcm, "default", AOS_PCM_STREAM_CAPTURE, 0);
                if(!ret) {
                    aos_pcm_hw_params_alloca(&cap_pcm->hw_params);
                    aos_pcm_sw_params_alloca(&cap_pcm->sw_params);
                    aos_pcm_sw_params_any(cap_pcm->sw_params);
                    aos_pcm_set_params(cap_pcm, AOSRV_PCM_FORMAT_S16_LE, AOS_PCM_ACCESS_RW_INTERLEAVED, channels, freq, 0, 0);
                    aos_pcm_prepare(cap_pcm);
                    aos_pcm_start(cap_pcm);
                } else {
                    LOGE(LOG_TAG, "%s:%d: open capture stream failed. ", __func__, __LINE__);
                }
            }
            ret = aos_pcm_readi(cap_pcm, dataBuf, aos_pcm_bytes_to_frames(cap_pcm, buf_size));
            if(ret > 0) {
                //LOGD(LOG_TAG, "%s:%d: readi frames(%d) bytes(%d) successfully. ", __func__, __LINE__, ret, aos_pcm_frames_to_bytes(cap_pcm, ret));
                ret = aos_pcm_writei(pb_pcm, dataBuf, aos_pcm_bytes_to_frames(pb_pcm, aos_pcm_frames_to_bytes(cap_pcm, ret)));
            }
        } else {
            if(pb_pcm) {
                LOGD(LOG_TAG, "%s:%d: close capture & playback stream!!", __func__, __LINE__);
                //aos_pcm_drain(pb_pcm);
                aos_pcm_stop(pb_pcm);
                aos_pcm_close(pb_pcm);
                pb_pcm = NULL;
            }
            if(cap_pcm) {
                aos_pcm_stop(cap_pcm);
                aos_pcm_close(cap_pcm);
                cap_pcm = NULL;
            }
            usleep(200*1000);
        }
    }

    return 0;
}

static void sound_example_loopback_init(void)
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

    pthread_create(&g_play_thread, &attr, sound_loopback_thread, NULL);
    pthread_setname_np(g_play_thread, "soundplaythread");

    pthread_attr_destroy(&attr);
    bCreateAudioThreadFlag = true;
}

void sound_example_loopback_entry(int argc, char **argv)
{
    if (argc < 2) {
        LOGD(LOG_TAG, "%s:%d: Usage: %s start/stop ", __func__, __LINE__, argv[0]);
        return;
    }
    param = strdup(argv[1]);
    if(param && !strcmp(param, "start"))
    {
        printf("sound loopback test begin ...\r\n");
    } else {
        printf("sound loopback test end !!!\r\n");
    }
    sound_example_loopback_init();
}
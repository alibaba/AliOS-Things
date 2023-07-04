/*
 * Copyright (C) 2015-2023 Alibaba Group Holding Limited
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <uvoice_audio.h>
#include <uvoice_pcm.h>
#include <yunit/yunit.h>
#include "aos/kernel.h"

/* module name used by log print*/
#define MODULE_NAME "pcm_test"

/* configure of export pa */
//#define EXIST_EX_PA
#define PA_ACTIVE_HIGH 1
#define PA_PIN 20
#define PA_DELAY_MS 500
struct external_pa_info pa_info;

/* configure of pcm */
#define EXIST_EX_PA
#define PCM_RATE 48000
#define PCM_CHANNEL 1
#define PCM_PERIOD_SIZE  4
#define PCM_PERIOD_COUNT  5
#define PCM_FORMAT  PCM_FORMAT_S16_LE
struct pcm_device *pcm;

/* configure of snd */
uint8_t * sound_data;
int sound_len = 0;
#define SOUND_LEN_MAX 1000

/* test pcm init */
void pcm_test_init(void)
{
    int ret;
#ifdef EXIST_EX_PA
    pa_info.active_high = PA_ACTIVE_HIGH;
    pa_info.pin = PA_PIN;
    pa_info.delay_ms = PA_DELAY_MS;
    ret = uvoice_extpa_config(&pa_info);
    YUNIT_ASSERT_EQUAL(ret, 0);
#endif
    ret = uvoice_pcm_init();
    YUNIT_ASSERT_EQUAL(ret, 0);
    uvoice_pcm_notify(PCM_MSG_WRITE_DONE);
}

/* test pcm open */
void pcm_test_open(void)
{
    int ret;
    pcm->config.rate = PCM_RATE;
    pcm->config.channels = PCM_CHANNEL;
    pcm->config.period_size = PCM_PERIOD_SIZE;
    pcm->config.period_count = PCM_PERIOD_COUNT;
    pcm->config.format = PCM_FORMAT;
    ret = uvoice_pcm_open(pcm);
    YUNIT_ASSERT_EQUAL(ret, 0);
}

/* test pcm setpath and read */
void pcm_test_read(void)
{
    int ret;
    ret = uvoice_set_path(pcm, SND_DEVICE_IN_PRIMARY_MIC);
    YUNIT_ASSERT_EQUAL(ret, 0);
    sound_data = aos_malloc(SOUND_LEN_MAX);
    ret = uvoice_pcm_read(pcm,sound_data,SOUND_LEN_MAX);
    YUNIT_ASSERT_TRUE(ret>0);
    sound_len = ret;
}

/* test pcm setpath and write */
void pcm_test_write(void)
{
    int ret;
    ret = uvoice_set_path(pcm, SND_DEVICE_OUT_SPEAKER);
    YUNIT_ASSERT_EQUAL(ret, 0);
    ret = uvoice_pcm_write(pcm,sound_data,sound_len);
    YUNIT_ASSERT_TRUE(ret>0);
    aos_free(sound_data);
}

/* test pcm setup */
void pcm_test_setup(void)
{
    int ret;
    pcm->config.period_size=PCM_PERIOD_SIZE*2;
    ret=uvoice_pcm_setup(pcm);
    YUNIT_ASSERT_EQUAL(ret, 0);
}

/* test pcm mute snd */
void pcm_test_mute(void)
{
    int ret;
    ret = uvoice_dev_mute(pcm, SND_DEVICE_OUT_SPEAKER,1);
    YUNIT_ASSERT_EQUAL(ret, 0);
}

/* test pcm silence */
void pcm_test_silence(void)
{
    int ret;
    ret = uvoice_pcm_silence(pcm);
    YUNIT_ASSERT_EQUAL(ret, 0);
}

/* test pcm flush */
void pcm_test_flush(void)
{
    int ret;
    ret = uvoice_pcm_flush(pcm);
    YUNIT_ASSERT_EQUAL(ret, 0);
}

/* test pcm close */
void pcm_test_close(void)
{
    int ret;
    ret = uvoice_pcm_close(pcm);
    YUNIT_ASSERT_EQUAL(ret, 0);
}
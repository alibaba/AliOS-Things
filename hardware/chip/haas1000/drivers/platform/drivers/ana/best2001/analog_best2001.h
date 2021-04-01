/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __ANALOG_BEST2001_H__
#define __ANALOG_BEST2001_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ISPI_ANA_REG(reg)                   (((reg) & 0xFFF) | 0x1000)

#define MAX_ANA_MIC_CH_NUM                  3

uint32_t analog_aud_get_max_dre_gain(void);

int analog_debug_config_vad_mic(bool enable);

void analog_aud_vad_enable(enum AUD_VAD_TYPE_T type, bool en);

void analog_aud_vad_adc_enable(bool en);

void analog_aud_codec_set_dev(int speaker_dev);

int analog_aud_codec_get_dev();

void analog_aud_codec_set_pa(int speaker_pa);

#ifdef __cplusplus
}
#endif

#endif


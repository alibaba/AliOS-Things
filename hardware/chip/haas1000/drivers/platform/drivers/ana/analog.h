/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __ANALOG_H__
#define __ANALOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "hal_analogif.h"
#include "hal_cmu.h"
#include "hal_aud.h"
#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(analog)

#ifndef ISPI_ANA_REG
#define ISPI_ANA_REG(reg)               (reg)
#endif
#define analog_read(reg, val)           hal_analogif_reg_read(ISPI_ANA_REG(reg), val)
#define analog_write(reg, val)          hal_analogif_reg_write(ISPI_ANA_REG(reg), val)

#define FLOAT_TO_PPB_INT(f)             ((int)(f * 1000 * 1000 * 1000))

enum ANA_AUD_PLL_USER_T {
    ANA_AUD_PLL_USER_CODEC,
    ANA_AUD_PLL_USER_I2S,
    ANA_AUD_PLL_USER_SPDIF,
    ANA_AUD_PLL_USER_PCM,

    ANA_AUD_PLL_USER_QTY
};

void analog_aud_freq_pll_config(uint32_t freq, uint32_t div);

void analog_aud_get_dc_calib_value(int16_t *dc_l, int16_t *dc_r);

void analog_open(void);

void analog_aud_xtal_tune(float ratio);

void analog_aud_pll_tune(float ratio);

void analog_aud_pll_open(enum ANA_AUD_PLL_USER_T user);

void analog_aud_pll_close(enum ANA_AUD_PLL_USER_T user);

void analog_aud_set_dac_gain(int32_t v);

uint32_t analog_codec_get_dac_gain(void);

uint32_t analog_codec_dac_gain_to_db(int32_t gain);

int32_t analog_codec_dac_max_attn_db(void);

void analog_aud_apply_anc_adc_gain_offset(enum ANC_TYPE_T type, int16_t offset_l, int16_t offset_r);

void analog_aud_apply_adc_gain_offset(enum AUD_CHANNEL_MAP_T ch_map, int16_t offset);

void analog_aud_codec_open(void);

void analog_aud_codec_close(void);

void analog_aud_codec_mute(void);

void analog_aud_codec_nomute(void);

void analog_aud_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en);

void analog_aud_codec_dac_enable(bool en);

void analog_aud_codec_speaker_enable(bool en);

void analog_aud_codec_anc_enable(enum ANC_TYPE_T type, bool en);

void analog_aud_mickey_enable(bool en);

void analog_sleep(void);

void analog_wakeup(void);

int analog_debug_config_audio_output(bool diff);

int analog_debug_config_codec(uint16_t mv);

int analog_debug_config_low_power_adc(bool enable);

#ifdef __cplusplus
}
#endif

#endif


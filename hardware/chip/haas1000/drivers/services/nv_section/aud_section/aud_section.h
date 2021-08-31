/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __aud_section_h__
#define __aud_section_h__

#include "section_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define aud_section_debug
#ifdef aud_section_debug
#define aud_trace     TRACE
#else
#define aud_trace(...)
#endif
#include "hal_aud.h"
#define audsec_tag  "audsec_tag"

#define aud_section_magic               0xdad1
#define aud_section_struct_version      1

#define AUD_COEF_LEN        (500)
#if (AUD_SECTION_STRUCT_VERSION == 1)
#define AUD_IIR_NUM         (6)
#elif (AUD_SECTION_STRUCT_VERSION == 2)
#define AUD_IIR_NUM         (8)
#elif (AUD_SECTION_STRUCT_VERSION == 3)
#define AUD_IIR_NUM         (8)
#else
#error "Can not support this version!!!"
#endif

typedef struct _anc_rir_coefs {
    int32_t coef_b[3];
	int32_t coef_a[3];
} anc_iir_coefs;

typedef struct _aud_item {
    int32_t     total_gain;

    uint16_t   iir_bypass_flag;
    uint16_t   iir_counter;
    anc_iir_coefs   iir_coef[AUD_IIR_NUM];

#if (AUD_SECTION_STRUCT_VERSION == 1)
    uint16_t    fir_bypass_flag;
    uint16_t    fir_len;
    int16_t     fir_coef[AUD_COEF_LEN];
    int8_t      pos_tab[16];
#elif (AUD_SECTION_STRUCT_VERSION == 2)
    int32_t     reserved_for_drc[32];
#elif (AUD_SECTION_STRUCT_VERSION == 3)

#endif
    int16_t     reserved1;
    int8_t      dac_gain_offset; // in qdb (quater of dB)
    int8_t      adc_gain_offset; // in qdb (quater of dB)
} aud_item;

typedef struct {
#if (AUD_SECTION_STRUCT_VERSION == 1)
	aud_item anc_cfg_ff_l;
  	aud_item anc_cfg_ff_r;
   	aud_item anc_cfg_fb_l;
  	aud_item anc_cfg_fb_r;
#elif (AUD_SECTION_STRUCT_VERSION == 2)
	aud_item anc_cfg_ff_l;
  	aud_item anc_cfg_ff_r;
   	aud_item anc_cfg_fb_l;
  	aud_item anc_cfg_fb_r;

   	aud_item anc_cfg_tt_l;
  	aud_item anc_cfg_tt_r;
   	aud_item anc_cfg_mc_l;
  	aud_item anc_cfg_mc_r;
#elif (AUD_SECTION_STRUCT_VERSION == 3)
	aud_item anc_cfg_ff_l;
   	aud_item anc_cfg_fb_l;
   	aud_item anc_cfg_tt_l;
   	aud_item anc_cfg_mc_l;
#endif

} struct_anc_cfg;

enum ANC_INDEX {
    ANC_INDEX_0 = 0,
    ANC_INDEX_1,
    ANC_INDEX_2,
    ANC_INDEX_3,
    ANC_INDEX_TOTAL,
};

typedef struct {
    unsigned char anc_ver[16];
    unsigned char batch_info[16];
    unsigned char serial[16];
} anc_ident;

enum auditem_sample_enum_t {
#if (AUD_SECTION_STRUCT_VERSION == 3)
    PCTOOL_SAMPLERATE_48X8K,
#else
    PCTOOL_SAMPLERATE_44_1X8K,
    PCTOOL_SAMPLERATE_48X8K,
    PCTOOL_SAMPLERATE_50_7X8K = PCTOOL_SAMPLERATE_48X8K,
#endif
    PCTOOL_AUDITEM_SAMPLERATE_COUNT
};

typedef struct {
    struct_anc_cfg    anc_cfg[PCTOOL_AUDITEM_SAMPLERATE_COUNT];
} pctool_struct_anc_cfg;

#if (AUD_SECTION_STRUCT_VERSION == 3)
#define PCTOOL_ANC_APPMODE_COUNT      15
#define ANC_SECTIONE_SIZE      (16*1024)
#else
#define PCTOOL_ANC_APPMODE_COUNT      4
#define ANC_SECTIONE_SIZE      (64*1024)
#endif

typedef struct {
    pctool_struct_anc_cfg anc_config_arr[PCTOOL_ANC_APPMODE_COUNT];
} pctool_anc_config_t;

#define PCTOOL_AUDSEC_RESERVED_LEN (ANC_SECTIONE_SIZE -sizeof(section_head_t) - sizeof(anc_ident) - sizeof(pctool_anc_config_t))

typedef struct {
    anc_ident               ancIdent;
    pctool_anc_config_t   anc_config;
    unsigned char           reserved[PCTOOL_AUDSEC_RESERVED_LEN];
} audsec_body;

typedef struct {
    section_head_t  sec_head;
    audsec_body     sec_body;
} pctool_aud_section;

typedef struct {
    uint8_t     io_pin;
    uint8_t     set_flag;
} pctool_iocfg;
/*
typedef struct{
    uint8_t digmic_ck_iomux_pin;
    uint8_t digmic_d0_iomux_pin;
    uint8_t digmic_d1_iomux_pin;
    uint8_t digmic_d2_iomux_pin;
    uint8_t digmic_phase;
}digital_mic_cfg;
*/

// Add audio and speech support
#define AUDIO_SECTION_DEBUG

// Device
#define AUDIO_SECTION_DEVICE_ANC            (0)
#define AUDIO_SECTION_DEVICE_AUDIO          (1)
#define AUDIO_SECTION_DEVICE_SPEECH         (2)
#define AUDIO_SECTION_DEVICE_NUM            (3)

// If add device, need add length to section_device_length
#define AUDIO_SECTION_LENGTH_ANC            (1024*8)
#define AUDIO_SECTION_LENGTH_AUDIO          (1024*8)
#define AUDIO_SECTION_LENGTH_SPEECH         (1024*8)

#define AUDIO_SECTION_CFG_RESERVED_LEN      (sizeof(audio_section_t))

typedef struct {
    section_head_t      head;
    uint32_t            device;
    uint32_t            cfg_len;
} audio_section_t;

// README: Important!!!!!!
// App cfg struecture must define reserved like follow:
// typedef struct {
//     uint8_t         reserved[AUDIO_SECTION_CFG_RESERVED_LEN];
//     custom_cfg_t    custom_cfg;
// } audio_section_custom_cfg_t;

int audio_section_store_cfg(uint32_t device, uint8_t *cfg, uint32_t len);
int audio_section_load_cfg(uint32_t device, uint8_t *cfg, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif

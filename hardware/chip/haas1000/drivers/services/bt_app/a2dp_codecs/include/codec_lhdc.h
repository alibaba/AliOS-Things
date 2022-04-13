/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __CODEC_LHDC_H__
#define __CODEC_LHDC_H__

#define A2DP_LHDC_OCTET_NUMBER                     (9)
#define A2DP_LHDC_VENDOR_ID                       0x0000053a
#if defined(A2DP_LHDC_V3)
#define A2DP_LHDC_CODEC_ID                        0x4C33
#else
#define A2DP_LHDC_CODEC_ID                        0x4C32
#endif
//To indicate Sampling Rate.
#define A2DP_LHDC_SR_96000                        0x01
#define A2DP_LHDC_SR_88200                        0x02
#define A2DP_LHDC_SR_48000                        0x04
#define A2DP_LHDC_SR_44100                        0x08
#define A2DP_LHDC_SR_DATA(X)                      (X & (A2DP_LHDC_SR_96000 | A2DP_LHDC_SR_88200 | A2DP_LHDC_SR_48000 | A2DP_LHDC_SR_44100))

//To indicate bits per sample.
#define A2DP_LHDC_FMT_24                          0x10
#define A2DP_LHDC_FMT_16                          0x20
#define A2DP_LHDC_FMT_DATA(X)                     (X & (A2DP_LHDC_FMT_24 | A2DP_LHDC_FMT_16))

#if defined(A2DP_LHDC_V3)
#define A2DP_LHDC_VERSION_NUM                      0x01
#else
#define A2DP_LHDC_VERSION_NUM                      0x00
#endif
#define A2DP_LHDC_MAX_SR_900                       0x00
#define A2DP_LHDC_MAX_SR_500                       0x10
#define A2DP_LHDC_MAX_SR_400                       0x20
#define A2DP_LHDC_MAX_SR_RESERVED                  0x30
#define A2DP_LHDC_LLC_ENABLE                       0x40

#define A2DP_LHDC_COF_CSC_DISABLE                  0x01
#define A2DP_LHDC_COF_CSC                          0x02 //
#define A2DP_LHDC_COF_CSC_PRE                      0x04
#define A2DP_LHDC_COF_CSC_RESERVED                 0x08
#define A2DP_LHDC_COF_DATA(X)                     (X & (A2DP_LHDC_COF_CSC_DISABLE | A2DP_LHDC_COF_CSC | A2DP_LHDC_COF_CSC_PRE))

typedef enum {
    LHDC_CHANNEL_SPLIT_DISABLE = 0,
    LHDC_CHANNEL_SPLIT,         //For forwarding type TWS used
    LHDC_CHANNEL_SPLIT_FROM_ENCODER,    // Pre-split left/right frame at encode side.
    LHDC_CHANNEL_SPLIT_INVALID,
} compressor_output_format_t;

typedef struct {
    uint32_t vendor_id;
    uint16_t codec_id;
    uint8_t bits;
    uint8_t sample_rater;       //uint:K
    uint8_t version_num;
    uint16_t max_sample_rate;   //uint:K  
    bool llc_enable;            //low latency control
    compressor_output_format_t compress_output_format;
} lhdc_info_t;

#ifdef __cplusplus
extern "C" {
#endif                          /*  */

    void lhdc_info_parse(uint8_t * elements, lhdc_info_t * info);
    uint8_t a2dp_lhdc_get_sample_rate(uint8_t * elements);

#ifdef __cplusplus
}
#endif

#endif /* __CODEC_LHDC_H__ */
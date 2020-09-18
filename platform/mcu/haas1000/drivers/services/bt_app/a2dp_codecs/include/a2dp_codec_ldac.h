/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __A2DP_CODEC_LDAC_H__
#define __A2DP_CODEC_LDAC_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "avdtp_api.h"

#define A2DP_LDAC_OCTET_NUMBER                     (8)
#define A2DP_LDAC_VENDOR_ID                       0x0000012d
#define A2DP_LDAC_CODEC_ID                        0x00aa
//To indicate Sampling Rate.
#define A2DP_LDAC_SR_96000                        0x04
#define A2DP_LDAC_SR_88200                        0x08
#define A2DP_LDAC_SR_48000                        0x10
#define A2DP_LDAC_SR_44100                        0x20
#define A2DP_LDAC_SR_DATA(X)                      (X & (A2DP_LDAC_SR_96000 | A2DP_LDAC_SR_88200 | A2DP_LDAC_SR_48000 | A2DP_LDAC_SR_44100))
#define A2DP_LDAC_CM_DATA(X)                      (X & (A2DP_LDAC_CM_MONO | A2DP_LDAC_CM_DUAL | A2DP_LDAC_CM_STEREO))
//To indicate channel mode(a2dp)
#define A2DP_LDAC_CM_MONO                        0x04
#define A2DP_LDAC_CM_DUAL                        0x02
#define A2DP_LDAC_CM_STEREO                      0x01

#if defined(A2DP_LDAC_ON)
extern const unsigned char a2dp_codec_ldac_elements[A2DP_LDAC_OCTET_NUMBER];
bt_status_t a2dp_codec_ldac_init(int index);
#endif /* A2DP_LDAC_ON */

#if defined(__cplusplus)
}
#endif

#endif /* __A2DP_CODEC_LDAC_H__ */
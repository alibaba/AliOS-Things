/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BTC_AUDIO_A2DP_SINK_H_
#define _BTC_AUDIO_A2DP_SINK_H_

#include <stdint.h>
#include "bt_types.h"
#include "avdt_api.h"


typedef struct {
  // Initialize the decoder. Can be called multiple times, will reinitalize.
  bool (*decoder_init)();

  // Cleanup the A2DP decoder.
  void (*decoder_cleanup)();

  // Decodes |p_buf| and calls |decode_callback| passed into init for the
  // decoded data.
  bool (*decode_packet)(BT_HDR* p_buf, uint8_t **buffer, int32_t *size);
} tA2DP_DECODER_INTERFACE;

/* AV Media Codec Types (Audio Codec ID) */
#define A2DP_MEDIA_CT_SBC 0x00 /* SBC media codec type */
#define A2DP_MEDIA_CT_AAC 0x02 /* AAC media codec type */
/* Non-A2DP media codec type (vendor-specific codec) */
#define A2DP_MEDIA_CT_NON_A2DP 0xFF

typedef uint8_t tA2DP_CODEC_TYPE; /* A2DP Codec type: A2DP_MEDIA_CT_* */

/* Success */
#define A2DP_SUCCESS 0

/* Failed */
#define A2DP_FAIL 0x0A

/* A2DP_FindService is already in progress */
#define A2DP_BUSY 0x0B

/* Bad parameters */
#define A2DP_INVALID_PARAMS 0x0C

/* Wrong codec info */
#define A2DP_WRONG_CODEC 0x0D

/* Media Codec Type is not valid */
#define A2DP_BAD_CODEC_TYPE 0xC1

/* Media Codec Type is not supported */
#define A2DP_NS_CODEC_TYPE 0xC2

/* Sampling Frequency is not valid or multiple values have been selected */
#define A2DP_BAD_SAMP_FREQ 0xC3

/* Sampling Frequency is not supported */
#define A2DP_NS_SAMP_FREQ 0xC4

/* Channel Mode is not valid or multiple values * have been selected */
#define A2DP_BAD_CH_MODE 0xC5

/* Channel Mode is not supported */
#define A2DP_NS_CH_MODE 0xC6

/* None or multiple values have been selected for Number of Subbands */
#define A2DP_BAD_SUBBANDS 0xC7

/* Number of Subbands is not supported */
#define A2DP_NS_SUBBANDS 0xC8

/* None or multiple values have been selected for Allocation Method */
#define A2DP_BAD_ALLOC_METHOD 0xC9

/* Allocation Method is not supported */
#define A2DP_NS_ALLOC_METHOD 0xCA

/* Minimum Bitpool Value is not valid */
#define A2DP_BAD_MIN_BITPOOL 0xCB

/* Minimum Bitpool Value is not supported */
#define A2DP_NS_MIN_BITPOOL 0xCC

/* Maximum Bitpool Value is not valid */
#define A2DP_BAD_MAX_BITPOOL 0xCD

/* Maximum Bitpool Value is not supported */
#define A2DP_NS_MAX_BITPOOL 0xCE

/* None or multiple values have been selected for Layer */
#define A2DP_BAD_LAYER 0xCF

/* Layer is not supported */
#define A2DP_NS_LAYER 0xD0

/* CRC is not supported */
#define A2DP_NS_CRC 0xD1

/* MPF-2 is not supported */
#define A2DP_NS_MPF 0xD2

/* VBR is not supported */
#define A2DP_NS_VBR 0xD3

/* None or multiple values have been selected for Bit Rate */
#define A2DP_BAD_BIT_RATE 0xD4

/* Bit Rate is not supported */
#define A2DP_NS_BIT_RATE 0xD5

/* Either 1) Object type is not valid (b3-b0) or 2) None or multiple values
 * have been * selected for Object Type
 */
#define A2DP_BAD_OBJ_TYPE 0xD6

/* Object type is not supported */
#define A2DP_NS_OBJ_TYPE 0xD7

/* None or multiple values have been selected for Channels */
#define A2DP_BAD_CHANNEL 0xD8

/* Channels is not supported */
#define A2DP_NS_CHANNEL 0xD9

/* None or multiple values have been selected for Block Length */
#define A2DP_BAD_BLOCK_LEN 0xDD

/* The requested CP Type is not supported. */
#define A2DP_BAD_CP_TYPE 0xE0

/* The format of Content Protection Service Capability/Content Protection
 * Scheme Dependent Data is not correct.
 */
#define A2DP_BAD_CP_FORMAT 0xE1

typedef uint8_t tA2DP_STATUS;

/* the return values from A2DP_BitsSet() */
#define A2DP_SET_ONE_BIT 1   /* one and only one bit is set */
#define A2DP_SET_ZERO_BIT 0  /* all bits clear */
#define A2DP_SET_MULTL_BIT 2 /* multiple bits are set */


#endif

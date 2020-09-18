/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BTC_AUDIO_A2DP_SBC_DECODER_H_
#define _BTC_AUDIO_A2DP_SBC_DECODER_H_

/*****************************************************************************
 *  Constants
 ****************************************************************************/
/* the length of the SBC Media Payload header. */
#define A2DP_SBC_MPL_HDR_LEN 1

/* the LOSC of SBC media codec capabilitiy */
#define A2DP_SBC_INFO_LEN 6

/* for Codec Specific Information Element */
#define A2DP_SBC_IE_SAMP_FREQ_MSK 0xF0 /* b7-b4 sampling frequency */
#define A2DP_SBC_IE_SAMP_FREQ_16 0x80  /* b7:16  kHz */
#define A2DP_SBC_IE_SAMP_FREQ_32 0x40  /* b6:32  kHz */
#define A2DP_SBC_IE_SAMP_FREQ_44 0x20  /* b5:44.1kHz */
#define A2DP_SBC_IE_SAMP_FREQ_48 0x10  /* b4:48  kHz */

#define A2DP_SBC_IE_CH_MD_MSK 0x0F    /* b3-b0 channel mode */
#define A2DP_SBC_IE_CH_MD_MONO 0x08   /* b3: mono */
#define A2DP_SBC_IE_CH_MD_DUAL 0x04   /* b2: dual */
#define A2DP_SBC_IE_CH_MD_STEREO 0x02 /* b1: stereo */
#define A2DP_SBC_IE_CH_MD_JOINT 0x01  /* b0: joint stereo */

#define A2DP_SBC_IE_BLOCKS_MSK 0xF0 /* b7-b4 number of blocks */
#define A2DP_SBC_IE_BLOCKS_4 0x80   /* 4 blocks */
#define A2DP_SBC_IE_BLOCKS_8 0x40   /* 8 blocks */
#define A2DP_SBC_IE_BLOCKS_12 0x20  /* 12blocks */
#define A2DP_SBC_IE_BLOCKS_16 0x10  /* 16blocks */

#define A2DP_SBC_IE_SUBBAND_MSK 0x0C /* b3-b2 number of subbands */
#define A2DP_SBC_IE_SUBBAND_4 0x08   /* b3: 4 */
#define A2DP_SBC_IE_SUBBAND_8 0x04   /* b2: 8 */

#define A2DP_SBC_IE_ALLOC_MD_MSK 0x03 /* b1-b0 allocation mode */
#define A2DP_SBC_IE_ALLOC_MD_S 0x02   /* b1: SNR */
#define A2DP_SBC_IE_ALLOC_MD_L 0x01   /* b0: loundess */

#define A2DP_SBC_IE_MIN_BITPOOL 2
#define A2DP_SBC_IE_MAX_BITPOOL 250

/* for media payload header */
#define A2DP_SBC_HDR_F_MSK 0x80
#define A2DP_SBC_HDR_S_MSK 0x40
#define A2DP_SBC_HDR_L_MSK 0x20
#define A2DP_SBC_HDR_NUM_MSK 0x0F

int A2DP_GetTrackSampleRateSbc(const uint8_t* p_codec_info);
int A2DP_GetTrackChannelCountSbc(const uint8_t* p_codec_info);
int A2DP_GetTrackBitsPerSampleSbc(const uint8_t* p_codec_info);
int A2DP_GetSinkTrackChannelTypeSbc(const uint8_t* p_codec_info);
bool A2DP_GetPacketTimestampSbc(const uint8_t* p_codec_info, const uint8_t* p_data, uint32_t* p_timestamp);
const tA2DP_DECODER_INTERFACE* A2DP_GetDecoderInterfaceSbc(const uint8_t* p_codec_info);


#endif

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "common/bt_target.h"
#include "common/bt_trace.h"
#include "btc_audio_a2dp_sink.h"
#include "btc_audio_a2dp_sbc_decoder.h"
#include "oi_codec_sbc.h"
#include "oi_status.h"

#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)
typedef struct {
  uint8_t samp_freq;    /* Sampling frequency */
  uint8_t ch_mode;      /* Channel mode */
  uint8_t block_len;    /* Block length */
  uint8_t num_subbands; /* Number of subbands */
  uint8_t alloc_method; /* Allocation method */
  uint8_t min_bitpool;  /* Minimum bitpool */
  uint8_t max_bitpool;  /* Maximum bitpool */
  uint8_t bits_per_sample;
} tA2DP_SBC_CIE;

bool a2dp_sbc_decoder_init(void);
void a2dp_sbc_decoder_cleanup(void);
bool a2dp_sbc_decoder_decode_packet(BT_HDR* p_buf, uint8_t **buffer, int32_t *size);
static tA2DP_STATUS A2DP_ParseInfoSbc(tA2DP_SBC_CIE* p_ie, const uint8_t* p_codec_info, bool is_capability);

static const tA2DP_DECODER_INTERFACE a2dp_decoder_interface_sbc = {
    a2dp_sbc_decoder_init, a2dp_sbc_decoder_cleanup,
    a2dp_sbc_decoder_decode_packet,
};

typedef struct {
    OI_CODEC_SBC_DECODER_CONTEXT decoder_context;
    uint32_t context_data[CODEC_DATA_WORDS(2, SBC_CODEC_FAST_FILTER_BUFFERS)];
    int16_t decode_buf[15 * SBC_MAX_SAMPLES_PER_FRAME * SBC_MAX_CHANNELS];
} tA2DP_SBC_DECODER_CB;

static tA2DP_SBC_DECODER_CB a2dp_sbc_decoder_cb;

/*******************************************************************************
 **
 ** Function         a2dp_sbc_decoder_init
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
bool a2dp_sbc_decoder_init(void)
{
    OI_STATUS status = OI_CODEC_SBC_DecoderReset(&a2dp_sbc_decoder_cb.decoder_context, a2dp_sbc_decoder_cb.context_data,
                                                 sizeof(a2dp_sbc_decoder_cb.context_data), 2, 2, false, 0);
    if (!OI_SUCCESS(status)) {
        BTC_TRACE_ERROR("%s: OI_CODEC_SBC_DecoderReset failed with error code %d",
                    __func__, status);
        return false;
    }
  
    return true;
}

/*******************************************************************************
 **
 ** Function         a2dp_sbc_decoder_cleanup
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
void a2dp_sbc_decoder_cleanup(void)
{
    // Do nothing.
}

/*******************************************************************************
 **
 ** Function         a2dp_sbc_decoder_decode_packet
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
bool a2dp_sbc_decoder_decode_packet(BT_HDR* p_buf, uint8_t **buffer, int32_t *size)
{
    uint8_t* data = p_buf->data + p_buf->offset;
    size_t data_size = p_buf->len;
  
    if (data_size == 0) {
        BTC_TRACE_ERROR("%s: Empty packet", __func__);
        return false;
    }
    size_t num_frames = data[0] & 0xf;
    data += 1;
    data_size -= 1;
  
    const OI_BYTE* oi_data = data;
    uint32_t oi_size = data_size;
    size_t out_avail = sizeof(a2dp_sbc_decoder_cb.decode_buf);
    int16_t* out_ptr = a2dp_sbc_decoder_cb.decode_buf;

    for (size_t i = 0; i < num_frames; ++i) {
        uint32_t out_size = out_avail;
        OI_STATUS status = OI_CODEC_SBC_DecodeFrame(&a2dp_sbc_decoder_cb.decoder_context, &oi_data,
                                   &oi_size, out_ptr, &out_size);
        if (!OI_SUCCESS(status)) {
            BTC_TRACE_ERROR("%s: Decoding failure: %d", __func__, status);
            return false;
        }
        out_avail -= out_size;
        out_ptr += out_size / sizeof(*out_ptr);
    }

    size_t out_used = (out_ptr - a2dp_sbc_decoder_cb.decode_buf) * sizeof(*out_ptr);

    *buffer = (uint8_t *)a2dp_sbc_decoder_cb.decode_buf;
    *size = out_used;

    BTC_TRACE_DEBUG("%s, frame = %d, out_used = %d", __func__, num_frames, out_used);

    return true;
}

/*******************************************************************************
 **
 ** Function         A2DP_GetTrackSampleRateSbc
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
int A2DP_GetTrackSampleRateSbc(const uint8_t* p_codec_info)
{
    tA2DP_SBC_CIE sbc_cie;
  
    tA2DP_STATUS a2dp_status = A2DP_ParseInfoSbc(&sbc_cie, p_codec_info, false);
    if (a2dp_status != A2DP_SUCCESS) {
        BTC_TRACE_ERROR("%s: cannot decode codec information: %d", __func__, a2dp_status);
        return -1;
    }
  
    switch (sbc_cie.samp_freq) {
    case A2DP_SBC_IE_SAMP_FREQ_16:
        return 16000;
    case A2DP_SBC_IE_SAMP_FREQ_32:
        return 32000;
    case A2DP_SBC_IE_SAMP_FREQ_44:
        return 44100;
    case A2DP_SBC_IE_SAMP_FREQ_48:
        return 48000;
    default:
        break;
    }
  
    return -1;
}

/*******************************************************************************
 **
 ** Function         A2DP_GetTrackChannelCountSbc
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
int A2DP_GetTrackChannelCountSbc(const uint8_t* p_codec_info)
{
   tA2DP_SBC_CIE sbc_cie;
 
   tA2DP_STATUS a2dp_status = A2DP_ParseInfoSbc(&sbc_cie, p_codec_info, false);
   if (a2dp_status != A2DP_SUCCESS) {
        BTC_TRACE_ERROR("%s: cannot decode codec information: %d", __func__, a2dp_status);
        return -1;
   }
 
   switch (sbc_cie.ch_mode) {
   case A2DP_SBC_IE_CH_MD_MONO:
       return 1;
   case A2DP_SBC_IE_CH_MD_DUAL:
   case A2DP_SBC_IE_CH_MD_STEREO:
   case A2DP_SBC_IE_CH_MD_JOINT:
       return 2;
   default:
       break;
   }
 
   return -1;
}

/*******************************************************************************
 **
 ** Function         A2DP_ParseInfoSbc
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
int A2DP_GetTrackBitsPerSampleSbc(const uint8_t* p_codec_info)
{
    tA2DP_SBC_CIE sbc_cie;
  
    tA2DP_STATUS a2dp_status = A2DP_ParseInfoSbc(&sbc_cie, p_codec_info, false);
    if (a2dp_status != A2DP_SUCCESS) {
        BTC_TRACE_ERROR("%s: cannot decode codec information: %d", __func__, a2dp_status);
        return -1;
    }
  
    // NOTE: The bits per sample never changes for SBC
    return 16;
}

/*******************************************************************************
 **
 ** Function         A2DP_GetSinkTrackChannelTypeSbc
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
int A2DP_GetSinkTrackChannelTypeSbc(const uint8_t* p_codec_info)
{
    tA2DP_SBC_CIE sbc_cie;
  
    tA2DP_STATUS a2dp_status = A2DP_ParseInfoSbc(&sbc_cie, p_codec_info, false);
    if (a2dp_status != A2DP_SUCCESS) {
        BTC_TRACE_ERROR("%s: cannot decode codec information: %d", __func__, a2dp_status);
        return -1;
    }
  
    switch (sbc_cie.ch_mode) {
    case A2DP_SBC_IE_CH_MD_MONO:
        return 1;
    case A2DP_SBC_IE_CH_MD_DUAL:
    case A2DP_SBC_IE_CH_MD_STEREO:
    case A2DP_SBC_IE_CH_MD_JOINT:
        return 3;
    default:
        break;
    }
  
    return -1;
}

/*******************************************************************************
 **
 ** Function         A2DP_GetPacketTimestampSbc
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
bool A2DP_GetPacketTimestampSbc(const uint8_t* p_codec_info,
                                const uint8_t* p_data, uint32_t* p_timestamp)
{
    *p_timestamp = *(const uint32_t*)p_data;
    
    return true;
}

/*******************************************************************************
 **
 ** Function         A2DP_ParseInfoSbc
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static tA2DP_STATUS A2DP_ParseInfoSbc(tA2DP_SBC_CIE* p_ie, const uint8_t* p_codec_info, bool is_capability)
{
    uint8_t losc;
    uint8_t media_type;
    tA2DP_CODEC_TYPE codec_type;
  
    if (p_ie == NULL || p_codec_info == NULL)
        return A2DP_INVALID_PARAMS;
  
    // Check the codec capability length
    losc = *p_codec_info++;
    if (losc != A2DP_SBC_INFO_LEN)
        return A2DP_WRONG_CODEC;
  
    media_type = (*p_codec_info++) >> 4;
    codec_type = *p_codec_info++;
    /* Check the Media Type and Media Codec Type */
    if (media_type != AVDT_MEDIA_AUDIO || codec_type != A2DP_MEDIA_CT_SBC) {
        return A2DP_WRONG_CODEC;
    }
  
    p_ie->samp_freq = *p_codec_info & A2DP_SBC_IE_SAMP_FREQ_MSK;
    p_ie->ch_mode = *p_codec_info & A2DP_SBC_IE_CH_MD_MSK;
    p_codec_info++;
    p_ie->block_len = *p_codec_info & A2DP_SBC_IE_BLOCKS_MSK;
    p_ie->num_subbands = *p_codec_info & A2DP_SBC_IE_SUBBAND_MSK;
    p_ie->alloc_method = *p_codec_info & A2DP_SBC_IE_ALLOC_MD_MSK;
    p_codec_info++;
    p_ie->min_bitpool = *p_codec_info++;
    p_ie->max_bitpool = *p_codec_info++;
    if (p_ie->min_bitpool < A2DP_SBC_IE_MIN_BITPOOL ||
        p_ie->min_bitpool > A2DP_SBC_IE_MAX_BITPOOL) {
        return A2DP_BAD_MIN_BITPOOL;
    }
  
    if (p_ie->max_bitpool < A2DP_SBC_IE_MIN_BITPOOL ||
        p_ie->max_bitpool > A2DP_SBC_IE_MAX_BITPOOL ||
        p_ie->max_bitpool < p_ie->min_bitpool) {
        return A2DP_BAD_MAX_BITPOOL;
    }

    BTC_TRACE_DEBUG("p_ie->samp_freq = %x", p_ie->samp_freq);
    BTC_TRACE_DEBUG("p_ie->ch_mode= %x", p_ie->ch_mode);
    BTC_TRACE_DEBUG("p_ie->block_len = %x", p_ie->block_len);
    BTC_TRACE_DEBUG("p_ie->alloc_method = %x", p_ie->alloc_method);
    if (is_capability) {
        // NOTE: The checks here are very liberal. We should be using more
        // pedantic checks specific to the SRC or SNK as specified in the spec.
        if (A2DP_BitsSet(p_ie->samp_freq) == A2DP_SET_ZERO_BIT)
            return A2DP_BAD_SAMP_FREQ;
        if (A2DP_BitsSet(p_ie->ch_mode) == A2DP_SET_ZERO_BIT)
            return A2DP_BAD_CH_MODE;
        if (A2DP_BitsSet(p_ie->block_len) == A2DP_SET_ZERO_BIT)
            return A2DP_BAD_BLOCK_LEN;
        if (A2DP_BitsSet(p_ie->num_subbands) == A2DP_SET_ZERO_BIT)
            return A2DP_BAD_SUBBANDS;
        if (A2DP_BitsSet(p_ie->alloc_method) == A2DP_SET_ZERO_BIT)
            return A2DP_BAD_ALLOC_METHOD;
    
        return A2DP_SUCCESS;
    }
  
    if (A2DP_BitsSet(p_ie->samp_freq) != A2DP_SET_ONE_BIT)
        return A2DP_BAD_SAMP_FREQ;
    if (A2DP_BitsSet(p_ie->ch_mode) != A2DP_SET_ONE_BIT) 
        return A2DP_BAD_CH_MODE;
    if (A2DP_BitsSet(p_ie->block_len) != A2DP_SET_ONE_BIT)
        return A2DP_BAD_BLOCK_LEN;
    if (A2DP_BitsSet(p_ie->num_subbands) != A2DP_SET_ONE_BIT)
        return A2DP_BAD_SUBBANDS;
    if (A2DP_BitsSet(p_ie->alloc_method) != A2DP_SET_ONE_BIT)
        return A2DP_BAD_ALLOC_METHOD;
  
    return A2DP_SUCCESS;
}

/*******************************************************************************
 **
 ** Function         A2DP_GetDecoderInterfaceSbc
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
bool A2DP_IsSinkCodecValidSbc(const uint8_t* p_codec_info)
{
  tA2DP_SBC_CIE cfg_cie;

  /* Use a liberal check when parsing the codec info */
  return (A2DP_ParseInfoSbc(&cfg_cie, p_codec_info, false) == A2DP_SUCCESS) ||
         (A2DP_ParseInfoSbc(&cfg_cie, p_codec_info, true) == A2DP_SUCCESS);
}


/*******************************************************************************
 **
 ** Function         A2DP_GetDecoderInterfaceSbc
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
const tA2DP_DECODER_INTERFACE* A2DP_GetDecoderInterfaceSbc(const uint8_t* p_codec_info)
{
    if (!A2DP_IsSinkCodecValidSbc(p_codec_info)) 
        return NULL;

    return &a2dp_decoder_interface_sbc;
}
#endif

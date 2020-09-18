/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "common/bt_target.h"
#include "common/bt_trace.h"

#if BTC_AV_SINK_INCLUDED

#include "btc_audio_a2dp_sink.h"
#include "btc_audio_a2dp_sbc_decoder.h"

tA2DP_CODEC_TYPE A2DP_GetCodecType(const uint8_t* p_codec_info)
{
    return (tA2DP_CODEC_TYPE)(p_codec_info[AVDT_CODEC_TYPE_INDEX]);
}

int A2DP_GetTrackSampleRate(const uint8_t* p_codec_info)
{
    tA2DP_CODEC_TYPE codec_type = A2DP_GetCodecType(p_codec_info);
  
    BTC_TRACE_DEBUG("%s: codec_type = 0x%x", __func__, codec_type);
  
    switch (codec_type) {
#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_SBC:
        return A2DP_GetTrackSampleRateSbc(p_codec_info);
#endif
#if (defined(AAC_DEC_INCLUDED) && AAC_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_AAC:
        return A2DP_GetTrackSampleRateAac(p_codec_info);
#endif
#if (defined(VENDOR_DEC_INCLUDED) && VENDOR_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_NON_A2DP:
        return A2DP_VendorGetTrackSampleRate(p_codec_info);
#endif
    default:
        break;
    }
  
    BTC_TRACE_ERROR("%s: unsupported codec type 0x%x", __func__, codec_type);
    return -1;
}

int A2DP_GetTrackBitsPerSample(const uint8_t* p_codec_info)
{
    tA2DP_CODEC_TYPE codec_type = A2DP_GetCodecType(p_codec_info);
  
    BTC_TRACE_VERBOSE("%s: codec_type = 0x%x", __func__, codec_type);
  
    switch (codec_type) {
#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_SBC:
        return A2DP_GetTrackBitsPerSampleSbc(p_codec_info);
#endif
#if (defined(AAC_DEC_INCLUDED) && AAC_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_AAC:
        return A2DP_GetTrackBitsPerSampleAac(p_codec_info);
#endif
#if (defined(VENDOR_DEC_INCLUDED) && VENDOR_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_NON_A2DP:
        return A2DP_VendorGetTrackBitsPerSample(p_codec_info);
#endif
    default:
        break;
    }
  
    BTC_TRACE_ERROR("%s: unsupported codec type 0x%x", __func__, codec_type);
    return -1;
}

int A2DP_GetTrackChannelCount(const uint8_t* p_codec_info)
{
    tA2DP_CODEC_TYPE codec_type = A2DP_GetCodecType(p_codec_info);
  
    BTC_TRACE_VERBOSE("%s: codec_type = 0x%x", __func__, codec_type);
  
    switch (codec_type) {
#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_SBC:
        return A2DP_GetTrackChannelCountSbc(p_codec_info);
#endif
#if (defined(AAC_DEC_INCLUDED) && AAC_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_AAC:
        return A2DP_GetTrackChannelCountAac(p_codec_info);
#endif
#if (defined(VENDOR_DEC_INCLUDED) && VENDOR_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_NON_A2DP:
        return A2DP_VendorGetTrackChannelCount(p_codec_info);
#endif
    default:
      break;
    }
  
    BTC_TRACE_ERROR("%s: unsupported codec type 0x%x", __func__, codec_type);
    return -1;
}

int A2DP_GetSinkTrackChannelType(const uint8_t* p_codec_info)
{
    tA2DP_CODEC_TYPE codec_type = A2DP_GetCodecType(p_codec_info);
    
    BTC_TRACE_VERBOSE("%s: codec_type = 0x%x", __func__, codec_type);
    
    switch (codec_type) {
#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_SBC:
        return A2DP_GetSinkTrackChannelTypeSbc(p_codec_info);
#endif
#if (defined(AAC_DEC_INCLUDED) && AAC_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_AAC:
        return A2DP_GetSinkTrackChannelTypeAac(p_codec_info);
#endif
#if (defined(VENDOR_DEC_INCLUDED) && VENDOR_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_NON_A2DP:
        return A2DP_VendorGetSinkTrackChannelType(p_codec_info);
#endif
    default:
        break;
    }
    
    BTC_TRACE_ERROR("%s: unsupported codec type 0x%x", __func__, codec_type);
    return -1;
}

bool A2DP_GetPacketTimestamp(const uint8_t* p_codec_info, const uint8_t* p_data,
                             uint32_t* p_timestamp)
{
    tA2DP_CODEC_TYPE codec_type = A2DP_GetCodecType(p_codec_info);
  
    switch (codec_type) {
#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_SBC:
        return A2DP_GetPacketTimestampSbc(p_codec_info, p_data, p_timestamp);
#endif
#if (defined(AAC_DEC_INCLUDED) && AAC_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_AAC:
        return A2DP_GetPacketTimestampAac(p_codec_info, p_data, p_timestamp);
#endif
#if (defined(VENDOR_DEC_INCLUDED) && VENDOR_DEC_INCLUDED == TRUE)
    case A2DP_MEDIA_CT_NON_A2DP:
        return A2DP_VendorGetPacketTimestamp(p_codec_info, p_data, p_timestamp);
#endif
    default:
        break;
    }
  
    BTC_TRACE_ERROR("%s: unsupported codec type 0x%x", __func__, codec_type);
    return false;
}



const tA2DP_DECODER_INTERFACE* A2DP_GetDecoderInterface(const uint8_t* p_codec_info)
{
    tA2DP_CODEC_TYPE codec_type = A2DP_GetCodecType(p_codec_info);
  
    BTC_TRACE_VERBOSE("%s: codec_type = 0x%x", __func__, codec_type);
  
    switch (codec_type) {
#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)
      case A2DP_MEDIA_CT_SBC:
        return A2DP_GetDecoderInterfaceSbc(p_codec_info);
#endif
#if (defined(AAC_DEC_INCLUDED) && AAC_DEC_INCLUDED == TRUE)
      case A2DP_MEDIA_CT_AAC:
        return A2DP_GetDecoderInterfaceAac(p_codec_info);
#endif
#if (defined(VENDOR_DEC_INCLUDED) && VENDOR_DEC_INCLUDED == TRUE)
      case A2DP_MEDIA_CT_NON_A2DP:
        return A2DP_VendorGetDecoderInterface(p_codec_info);
#endif
      default:
        break;
    }
  
    BTC_TRACE_ERROR("%s: unsupported codec type 0x%x", __func__, codec_type);
    return NULL;
}

/******************************************************************************
 * Function         A2DP_BitsSet
 *
 * Description      Check the given num for the number of bits set
 * Returns          A2DP_SET_ONE_BIT, if one and only one bit is set
 *                  A2DP_SET_ZERO_BIT, if all bits clear
 *                  A2DP_SET_MULTL_BIT, if multiple bits are set
 *****************************************************************************/
uint8_t A2DP_BitsSet(uint32_t num)
{
    if (num == 0) 
        return A2DP_SET_ZERO_BIT;
    if ((num & (num - 1)) == 0)
        return A2DP_SET_ONE_BIT;
    return A2DP_SET_MULTL_BIT;
}

#endif

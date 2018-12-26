/*******************************************************************************
--                                                                            --
--                    CedarX Multimedia Framework                             --
--                                                                            --
--          the Multimedia Framework for Linux/Android System                 --
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                         Softwinner Products.                               --
--                                                                            --
--                   (C) COPYRIGHT 2011 SOFTWINNER PRODUCTS                   --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
*******************************************************************************/
#ifndef CDX_Common_H_
#define CDX_Common_H_

typedef enum CEDARX_AUDIO_CHANNEL_TYPE
{
    CEDARX_AUDIO_CHANNEL_STEREO = 0,
    CEDARX_AUDIO_CHANNEL_LEFT ,
    CEDARX_AUDIO_CHANNEL_RIGHT,
} CEDARX_AUDIO_CHANNEL_TYPE;

typedef enum CDX_COMP_PRIV_FLAGS
{
    CDX_COMP_PRIV_FLAGS_REINIT    = 1,
    CDX_COMP_PRIV_FLAGS_STREAMEOF = 2,
}CDX_COMP_PRIV_FLAGS;

enum CEDARXAUDIOFLAGSENUM
{
    ADEC_DISABLE_AAC_PACKING = (1<<0),
};


typedef enum CDX_DECODE_MODE {
    CDX_DECODER_MODE_NORMAL    = 0,
    CDX_DECODER_MODE_RAWMUSIC,
}CDX_DECODE_MODE;

#endif
/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : adecoder.h
* Description :
* History :
*   Author  : gvc-al2 <gvc-al2@allwinnertech.com>
*   Date    : long long ago
*/

#ifndef ADECODER_H
#define ADECODER_H

#include "unistd.h"
#include <stdint.h>

#define OS_LINUX
//#include <stdlib.h>
//typedef signed long long int64_t;
//typedef int int32_t;

#ifdef __cplusplus
extern "C" {
#endif

#define ADEC_SET_BUFFER_ENABLE
#ifdef ADEC_SET_BUFFER_ENABLE
extern int maxBitStreamBufferSize;
extern int maxBitStreamFrameCount;
extern int maxPcmBufferSize;

#define AUDIO_BITSTREAM_BUFFER_SIZE (maxBitStreamBufferSize)//(1024 * 6)//(1024*4096)
#define AUDIO_BITSTREAM_BUFFER_MAX_FRAME_NUM (maxBitStreamFrameCount)//(6)//(4096)
#define AUDIO_PCM_BUFFER_SIZE (maxPcmBufferSize)//(16 * 1024)//(512*1024)
#else
#define AUDIO_BITSTREAM_BUFFER_SIZE (1024 * 6)//(1024*4096)
#define AUDIO_BITSTREAM_BUFFER_MAX_FRAME_NUM (6)//(4096)
#define AUDIO_PCM_BUFFER_SIZE (16 * 1024)//(512*1024)
#endif

typedef enum __AUDIO_DEC_RESULT
{
    ERR_AUDIO_DEC_EXIT          = -4,   // exit
    ERR_AUDIO_DEC_ENDINGCHKFAIL = -3,   //big ending or little ending check failed
    ERR_AUDIO_DEC_ABSEND        = -2,   //audio bitstream decode end
    ERR_AUDIO_DEC_ONEFRMFAIL    = -1,   //decode one frame failed, can try again

    ERR_AUDIO_DEC_NONE          =  0,   //decode successed, no error
    ERR_AUDIO_DEC_FFREVRETURN   =  1,   //return from fast-forward or fast-reverse
    ERR_AUDIO_DEC_RETAPOINT     =  2,   //return from A point under A/B play mode
    ERR_AUDIO_DEC_RETTAG        =  3,   //return from the first frame of tag play
    ERR_AUDIO_DEC_VIDEOJUMP     =  4,   //0X88 maybe return 4 or 0
    ERR_AUDIO_DEC_NO_BITSTREAM  =  5,   //No enough bitstream ,try again
    ERR_AUDIO_DEC_

} __audio_dec_result_t;

enum EAUDIOCODECFORMAT
{
    AUDIO_CODEC_FORMAT_UNKNOWN = 0,
    AUDIO_CODEC_FORMAT_MP1,
    AUDIO_CODEC_FORMAT_MP2,
    AUDIO_CODEC_FORMAT_MP3,
    AUDIO_CODEC_FORMAT_MPEG_AAC_LC,
    AUDIO_CODEC_FORMAT_AC3,         //* not supported.
    AUDIO_CODEC_FORMAT_DTS,         //* not supported.
    AUDIO_CODEC_FORMAT_LPCM_V,
    AUDIO_CODEC_FORMAT_LPCM_A,
    AUDIO_CODEC_FORMAT_ADPCM,
    AUDIO_CODEC_FORMAT_PCM,
    AUDIO_CODEC_FORMAT_WMA_STANDARD,
    AUDIO_CODEC_FORMAT_FLAC,
    AUDIO_CODEC_FORMAT_APE,
    AUDIO_CODEC_FORMAT_OGG,
    AUDIO_CODEC_FORMAT_RAAC,
    AUDIO_CODEC_FORMAT_COOK,
    AUDIO_CODEC_FORMAT_SIPR,
    AUDIO_CODEC_FORMAT_ATRC,
    AUDIO_CODEC_FORMAT_AMR,
    AUDIO_CODEC_FORMAT_RA,
    AUDIO_CODEC_FORMAT_PPCM,        //* not supported.
    AUDIO_CODEC_FORMAT_WMA_LOSS,    //* not supported.
    AUDIO_CODEC_FORMAT_WMA_PRO,     //* not supported.
    AUDIO_CODEC_FORMAT_MP3_PRO,     //* not supported.
    AUDIO_CODEC_FORMAT_ALAC,
    AUDIO_CODEC_FORMAT_G729,
    AUDIO_CODEC_FORMAT_DSD,
    AUDIO_CODEC_FORMAT_OPUS,
    AUDIO_CODEC_FORMAT_MLP = AUDIO_CODEC_FORMAT_AC3,
    AUDIO_CODEC_FORMAT_ = -1,
};

//**************************************************************************//
//* Below are definition for sub codec types.
//**************************************************************************//
#define ABS_EDIAN_FLAG_MASK         ((unsigned int)(1<<16))
#define ABS_EDIAN_FLAG_BIG          ((unsigned int)(1<<16))

/* Windows WAVE File Encoding Tags */
#define WAVE_FORMAT_UNKNOWN                 0x0000  /* Unknown Format */
#define WAVE_FORMAT_PCM                     0x0001  /* PCM */
#define WAVE_FORMAT_ADPCM                   0x0002  /* Microsoft ADPCM Format */
#define WAVE_FORMAT_IEEE_FLOAT              0x0003  /* IEEE Float */
#define WAVE_FORMAT_VSELP                   0x0004  /* Compaq Computer's VSELP */
#define WAVE_FORMAT_IBM_CVSD                0x0005 /* IBM CVSD */
#define WAVE_FORMAT_ALAW                    0x0006  /* ALAW */
#define WAVE_FORMAT_MULAW                   0x0007  /* MULAW */
#define WAVE_FORMAT_DTS                     0x0008 /* Microsoft Corporation */
#define WAVE_FORMAT_DRM                     0x0009 /* Microsoft Corporation */
#define WAVE_FORMAT_WMAVOICE9               0x000A /* Microsoft Corporation */
#define WAVE_FORMAT_WMAVOICE10              0x000B /* Microsoft Corporation */
#define WAVE_FORMAT_OKI_ADPCM               0x0010  /* OKI ADPCM */
#define WAVE_FORMAT_DVI_ADPCM               0x0011  /* Intel's DVI ADPCM */
#define WAVE_FORMAT_IMA_ADPCM               (WAVE_FORMAT_DVI_ADPCM) /*  Intel Corporation */
#define WAVE_FORMAT_MEDIASPACE_ADPCM        0x0012  /* Videologic's MediaSpace ADPCM*/
#define WAVE_FORMAT_SIERRA_ADPCM            0x0013  /* Sierra ADPCM */
#define WAVE_FORMAT_G723_ADPCM              0x0014  /* G.723 ADPCM */
#define WAVE_FORMAT_DIGISTD                 0x0015  /* DSP Solution's DIGISTD */
#define WAVE_FORMAT_DIGIFIX                 0x0016  /* DSP Solution's DIGIFIX */
#define WAVE_FORMAT_DIALOGIC_OKI_ADPCM      0x0017  /* Dialogic OKI ADPCM */
#define WAVE_FORMAT_MEDIAVISION_ADPCM       0x0018  /* MediaVision ADPCM */
#define WAVE_FORMAT_CU_CODEC                0x0019  /* HP CU */
#define WAVE_FORMAT_YAMAHA_ADPCM            0x0020  /* Yamaha ADPCM */
#define WAVE_FORMAT_SONARC                  0x0021  /* Speech Compression's Sonarc */
#define WAVE_FORMAT_TRUESPEECH              0x0022  /* DSP Group's True Speech */
#define WAVE_FORMAT_ECHOSC1                 0x0023  /* Echo Speech's EchoSC1 */
#define WAVE_FORMAT_AUDIOFILE_AF36          0x0024  /* Audiofile AF36 */
#define WAVE_FORMAT_APTX                    0x0025  /* APTX */
#define WAVE_FORMAT_AUDIOFILE_AF10          0x0026  /* AudioFile AF10 */
#define WAVE_FORMAT_PROSODY_1612            0x0027  /* Prosody 1612 */
#define WAVE_FORMAT_LRC                     0x0028  /* LRC */
#define WAVE_FORMAT_AC2                     0x0030  /* Dolby AC2 */
#define WAVE_FORMAT_GSM610                  0x0031  /* GSM610 */
#define WAVE_FORMAT_MSNAUDIO                0x0032  /* MSNAudio */
#define WAVE_FORMAT_ANTEX_ADPCME            0x0033  /* Antex ADPCME */
#define WAVE_FORMAT_CONTROL_RES_VQLPC       0x0034  /* Control Res VQLPC */
#define WAVE_FORMAT_DIGIREAL                0x0035  /* Digireal */
#define WAVE_FORMAT_DIGIADPCM               0x0036  /* DigiADPCM */
#define WAVE_FORMAT_CONTROL_RES_CR10        0x0037  /* Control Res CR10 */
#define WAVE_FORMAT_VBXADPCM                0x0038  /* NMS VBXADPCM */
#define WAVE_FORMAT_ROLAND_RDAC             0x0039  /* Roland RDAC */
#define WAVE_FORMAT_ECHOSC3                 0x003A  /* EchoSC3 */
#define WAVE_FORMAT_ROCKWELL_ADPCM          0x003B  /* Rockwell ADPCM */
#define WAVE_FORMAT_ROCKWELL_DIGITALK       0x003C  /* Rockwell Digit LK */
#define WAVE_FORMAT_XEBEC                   0x003D  /* Xebec */
#define WAVE_FORMAT_G721_ADPCM              0x0040  /* Antex Electronics G.721 */
#define WAVE_FORMAT_G728_CELP               0x0041  /* G.728 CELP */
#define WAVE_FORMAT_MSG723                  0x0042  /* MSG723 */
#define WAVE_FORMAT_MPEG                    0x0050  /* MPEG Layer 1,2 */
#define WAVE_FORMAT_RT24                    0x0051  /* RT24 */
#define WAVE_FORMAT_PAC                     0x0051  /* PAC */
#define WAVE_FORMAT_MPEGLAYER3              0x0055  /* MPEG Layer 3 */
//#define WAVE_FORMAT_CIRRUS                  0x0059 /* Cirrus */
#define WAVE_FORMAT_LUCENT_G723             0x0059 /* Lucent Technologies */
#define WAVE_FORMAT_CIRRUS                  0x0060 /* Cirrus Logic */
#define WAVE_FORMAT_ESPCM                   0x0061  /* ESPCM */
#define WAVE_FORMAT_VOXWARE                 0x0062  /* Voxware (obsolete) */
#define WAVE_FORMAT_CANOPUS_ATRAC           0x0063  /* Canopus Atrac */
#define WAVE_FORMAT_G726_ADPCM              0x0064  /* G.726 ADPCM */
#define WAVE_FORMAT_G722_ADPCM              0x0065  /* G.722 ADPCM */
#define WAVE_FORMAT_DSAT                    0x0066  /* DSAT */
#define WAVE_FORMAT_DSAT_DISPLAY            0x0067  /* DSAT Display */
#define WAVE_FORMAT_VOXWARE_BYTE_ALIGNED    0x0069  /* Voxware Byte Aligned (obsolete) */
#define WAVE_FORMAT_VOXWARE_AC8             0x0070  /* Voxware AC8 (obsolete) */
#define WAVE_FORMAT_VOXWARE_AC10            0x0071  /* Voxware AC10 (obsolete) */
#define WAVE_FORMAT_VOXWARE_AC16            0x0072  /* Voxware AC16 (obsolete) */
#define WAVE_FORMAT_VOXWARE_AC20            0x0073  /* Voxware AC20 (obsolete) */
#define WAVE_FORMAT_VOXWARE_RT24            0x0074  /* Voxware MetaVoice (obsolete) */
#define WAVE_FORMAT_VOXWARE_RT29            0x0075  /* Voxware MetaSound (obsolete) */
#define WAVE_FORMAT_VOXWARE_RT29HW          0x0076  /* Voxware RT29HW (obsolete) */
#define WAVE_FORMAT_VOXWARE_VR12            0x0077  /* Voxware VR12 (obsolete) */
#define WAVE_FORMAT_VOXWARE_VR18            0x0078  /* Voxware VR18 (obsolete) */
#define WAVE_FORMAT_VOXWARE_TQ40            0x0079  /* Voxware TQ40 (obsolete) */
#define WAVE_FORMAT_SOFTSOUND               0x0080  /* Softsound */
#define WAVE_FORMAT_VOXWARE_TQ60            0x0081  /* Voxware TQ60 (obsolete) */
#define WAVE_FORMAT_MSRT24                  0x0082  /* MSRT24 */
#define WAVE_FORMAT_G729A                   0x0083  /* G.729A */
#define WAVE_FORMAT_MVI_MV12                0x0084  /* MVI MV12 */
#define WAVE_FORMAT_DF_G726                 0x0085  /* DF G.726 */
#define WAVE_FORMAT_DF_GSM610               0x0086  /* DF GSM610 */
#define WAVE_FORMAT_ISIAUDIO                0x0088  /* ISIAudio */
#define WAVE_FORMAT_ONLIVE                  0x0089  /* Onlive */
#define WAVE_FORMAT_SBC24                   0x0091  /* SBC24 */
#define WAVE_FORMAT_DOLBY_AC3_SPDIF         0x0092  /* Dolby AC3 SPDIF */
#define WAVE_FORMAT_MEDIASONIC_G723         0x0093 /* MediaSonic */
#define WAVE_FORMAT_PROSODY_8KBPS           0x0094 /* Aculab plc */
#define WAVE_FORMAT_ZYXEL_ADPCM             0x0097  /* ZyXEL ADPCM */
#define WAVE_FORMAT_PHILIPS_LPCBB           0x0098  /* Philips LPCBB */
#define WAVE_FORMAT_PACKED                  0x0099  /* Packed */
#define WAVE_FORMAT_MALDEN_PHONYTALK        0x00A0 /* Malden Electronics Ltd. */
#define WAVE_FORMAT_RHETOREX_ADPCM          0x0100  /* Rhetorex ADPCM */
#define WAVE_FORMAT_IRAT                    0x0101  /* BeCubed Software's IRAT */
#define WAVE_FORMAT_VIVO_G723               0x0111  /* Vivo G.723 */
#define WAVE_FORMAT_VIVO_SIREN              0x0112  /* Vivo Siren */
#define WAVE_FORMAT_DIGITAL_G723            0x0123  /* Digital G.723 */
#define WAVE_FORMAT_SANYO_LD_ADPCM          0x0125 /* Sanyo Electric Co., Ltd. */
#define WAVE_FORMAT_SIPROLAB_ACEPLNET       0x0130 /* Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_ACELP4800      0x0131 /* Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_ACELP8V3       0x0132 /* Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_G729           0x0133 /* Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_G729A          0x0134 /* Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_KELVIN         0x0135 /* Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_G726ADPCM               0x0140 /* Dictaphone Corporation */
#define WAVE_FORMAT_QUALCOMM_PUREVOICE      0x0150 /* Qualcomm, Inc. */
#define WAVE_FORMAT_QUALCOMM_HALFRATE       0x0151 /* Qualcomm, Inc. */
#define WAVE_FORMAT_TUBGSM                  0x0155 /* Ring Zero Systems, Inc. */
#define WAVE_FORMAT_MSAUDIO1                0x0160 /* Microsoft Corporation */
#define WAVE_FORMAT_WMAUDIO2                0x0161 /* Microsoft Corporation */
#define WAVE_FORMAT_WMAUDIO3                0x0162 /* Microsoft Corporation */
#define WAVE_FORMAT_WMAUDIO_LOSSLESS        0x0163 /* Microsoft Corporation */
#define WAVE_FORMAT_WMASPDIF                0x0164 /* Microsoft Corporation */
#define WAVE_FORMAT_UNISYS_NAP_ADPCM        0x0170 /* Unisys Corp. */
#define WAVE_FORMAT_UNISYS_NAP_ULAW         0x0171 /* Unisys Corp. */
#define WAVE_FORMAT_UNISYS_NAP_ALAW         0x0172 /* Unisys Corp. */
#define WAVE_FORMAT_UNISYS_NAP_16K          0x0173 /* Unisys Corp. */
#define WAVE_FORMAT_CREATIVE_ADPCM          0x0200  /* Creative ADPCM */
#define WAVE_FORMAT_CREATIVE_FASTSPEECH8    0x0202  /* Creative FastSpeech8 */
#define WAVE_FORMAT_CREATIVE_FASTSPEECH10   0x0203  /* Creative FastSpeech10 */
#define WAVE_FORMAT_UHER_ADPCM              0x0210 /* UHER informatic GmbH */
#define WAVE_FORMAT_QUARTERDECK             0x0220  /* Quarterdeck */
#define WAVE_FORMAT_ILINK_VC                0x0230 /* I-link Worldwide */
#define WAVE_FORMAT_RAW_SPORT               0x0240 /* Aureal Semiconductor */
#define WAVE_FORMAT_ESST_AC3                0x0241 /* ESS Technology, Inc. */
#define WAVE_FORMAT_GENERIC_PASSTHRU        0x0249
#define WAVE_FORMAT_IPI_HSX                 0x0250 /* Interactive Products, Inc. */
#define WAVE_FORMAT_IPI_RPELP               0x0251 /* Interactive Products, Inc. */
#define WAVE_FORMAT_CS2                     0x0260 /* Consistent Software */
#define WAVE_FORMAT_SONY_SCX                0x0270 /* Sony Corp. */
#define WAVE_FORMAT_FM_TOWNS_SND            0x0300  /* FM Towns Snd */
#define WAVE_FORMAT_BTV_DIGITAL             0x0400  /* BTV Digital */
#define WAVE_FORMAT_QDESIGN_MUSIC           0x0450 /* QDesign Corporation */
#define WAVE_FORMAT_VME_VMPCM               0x0680  /* VME VMPCM */
#define WAVE_FORMAT_TPC                     0x0681 /* AT&T Labs, Inc. */
#define WAVE_FORMAT_OLIGSM                  0x1000  /* OLIGSM */
#define WAVE_FORMAT_OLIADPCM                0x1001  /* OLIADPCM */
#define WAVE_FORMAT_OLICELP                 0x1002  /* OLICELP */
#define WAVE_FORMAT_OLISBC                  0x1003  /* OLISBC */
#define WAVE_FORMAT_OLIOPR                  0x1004  /* OLIOPR */
#define WAVE_FORMAT_LH_CODEC                0x1100  /* LH Codec */
#define WAVE_FORMAT_NORRIS                  0x1400  /* Norris */
//#define WAVE_FORMAT_ISIAUDIO                0x1401 /* ISIAudio */
#define WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS 0x1500  /* Soundspace Music Compression */
#define WAVE_FORMAT_MPEG_ADTS_AAC           0x1600 /* Microsoft Corporation */
#define WAVE_FORMAT_MPEG_RAW_AAC            0x1601 /* Microsoft Corporation */
#define WAVE_FORMAT_NOKIA_MPEG_ADTS_AAC     0x1608 /* Microsoft Corporation */
#define WAVE_FORMAT_NOKIA_MPEG_RAW_AAC      0x1609 /* Microsoft Corporation */
#define WAVE_FORMAT_VODAFONE_MPEG_ADTS_AAC  0x160A /* Microsoft Corporation */
#define WAVE_FORMAT_VODAFONE_MPEG_RAW_AAC   0x160B /* Microsoft Corporation */
#define WAVE_FORMAT_DVM                     0x2000  /* DVM */
#define WAVE_FORMAT_EXTENSIBTSMIRACAST      0xFFFC /* LSZHANG TS  Miracast WIFI*/
#define WAVE_FORMAT_EXTENSIBTS              0xFFFD /* LSZHANG TS */
#define WAVE_FORMAT_EXTENSIBLE              0xFFFE  /* SubFormat */
#define WAVE_FORMAT_DVD_LPCM                0xFFA0  /* Added by Khan for DVD-LPCM*/

#define WAVE_FORMAT_DEVELOPMENT             0xFFFF  /* Development */

//user define adpcm codec type from video file
#define ADPCM_CODEC_ID_IMA_QT               0xE000
#define ADPCM_CODEC_ID_IMA_WAV              0xE001  /* from avi file format */
#define ADPCM_CODEC_ID_IMA_DK3              0xE002
#define ADPCM_CODEC_ID_IMA_DK4              0xE003
#define ADPCM_CODEC_ID_IMA_WS               0xE004
#define ADPCM_CODEC_ID_IMA_SMJPEG           0xE005
#define ADPCM_CODEC_ID_MS                   0xE006
#define ADPCM_CODEC_ID_4XM                  0xE007
#define ADPCM_CODEC_ID_XA                   0xE008
#define ADPCM_CODEC_ID_ADX                  0xE009
#define ADPCM_CODEC_ID_EA                   0xE00A
#define ADPCM_CODEC_ID_G726                 0xE00B
#define ADPCM_CODEC_ID_CT                   0xE00C
#define ADPCM_CODEC_ID_SWF                  0xE00D  /* from flv/swf file format */
#define ADPCM_CODEC_ID_YAMAHA               0xE00E
#define ADPCM_CODEC_ID_SBPRO_4              0xE00F
#define ADPCM_CODEC_ID_SBPRO_3              0xE010
#define ADPCM_CODEC_ID_SBPRO_2              0xE011

#define AMR_FORMAT_NONE                     0           // undefine amr format
#define AMR_FORMAT_NARROWBAND               1           // narrow band amr format
#define AMR_FORMAT_WIDEBAND                 2           // wide band amr format

#define ADECODER_MAX_LANG_CHAR_SIZE  (64)

typedef struct Cedar_raw_data CdxPlaybkCfg;

enum AUIDO_RAW_DATA_TYPE
{
    AUDIO_RAW_DATA_UNKOWN = 0,
    AUDIO_RAW_DATA_PCM = 1,
    AUDIO_RAW_DATA_AC3 = 2,
    AUDIO_RAW_DATA_MPEG1 = 3,
    AUDIO_RAW_DATA_MP3 = 4,
    AUDIO_RAW_DATA_MPEG2 = 5,
    AUDIO_RAW_DATA_AAC = 6,
    AUDIO_RAW_DATA_DTS = 7,
    AUDIO_RAW_DATA_ATRAC = 8,
    AUDIO_RAW_DATA_ONE_BIT_AUDIO = 9,
    AUDIO_RAW_DATA_DOLBY_DIGITAL_PLUS = 10,
    AUDIO_RAW_DATA_DTS_HD = 11,
    AUDIO_RAW_DATA_MAT = 12,
    AUDIO_RAW_DATA_DST = 13,
    AUDIO_RAW_DATA_WMAPRO = 14
};

struct Cedar_raw_data
{
    int                      nRoutine;//UI set 0:pcm;1:hdmi raw data;2:spdif raw data;
    int                      nNeedDirect;//if init flag 0:no init raw;1:init raw
    int                      nChannels;
    int                      nSamplerate;
    int                      nBitpersample;
    enum AUIDO_RAW_DATA_TYPE nDataType;
};

typedef struct AUDIOSTREAMINFO
{
    enum EAUDIOCODECFORMAT  eCodecFormat;
    int                     eSubCodecFormat; //
    int                     nChannelNum;
    int                     nBitsPerSample;
    int                     nSampleRate;
    int                     nAvgBitrate;
    int                     nMaxBitRate;
    unsigned int            nFileSize;
    int                     eAudioBitstreamSource;
    int                     eDataEncodeType;
    unsigned char           strLang[ADECODER_MAX_LANG_CHAR_SIZE];
    int                     nCodecSpecificDataLen;
    char*                   pCodecSpecificData;
    int                     nFlags;
    int                     nBlockAlign;
    CdxPlaybkCfg            raw_data;
}AudioStreamInfo;

typedef struct __BsInFor
{
    int    TotalplayTime;
    int    NowPlayTime;
    int    Samplerate;
    int    bitrate;
    int    chan;
    int    CBRflag;//1 cbr 0 vbr
    int    framesize;
    int    framecount;
    int    oldfs;
    int    oldbs;
    int    firstflag;
    int    framepcms;
    int    modeflag;//flag 0bits: 1:raw data 0:pcm
    int    ulMode;//decode flag 0:pcm ,>raw data out ,1:hdmi,2:spdif
    int    bitpersample;//bits per sample
    //add for control from ad_cedar_ctx
    int    nBitStreamUnderFlow;
    int    nShowBitsReturn;
    int    out_channels;
    int    out_samplerate;
    //global
    int    nDecodeMode;
    int    nDemuxType;
    int    nIsHwCodec;
    int64_t NowPTSTime;

}BsInFor;

typedef struct AudioDecoder AudioDecoder;
struct AudioDecoder
{
    int    nPrivFlag;//0:play,1:reset,2:file end
    int    nGetAudioInfoFlag;
    int    nAudioChannel;
    int    volumegain;
    int    mute;
    int    nEnableResample;
    int     RawPlayFlag;
};

int ParserRequestBsBuffer(AudioDecoder* pDecoder,
                          int                nRequireSize,
                          unsigned char**    ppBuf,
                          int*               pBufSize,
                          unsigned char**    ppRingBuf,
                          int*               pRingBufSize,
                          int*               nOffset);

int ParserUpdateBsBuffer(AudioDecoder* pDecoder,
                         int                nFilledLen,
                         int64_t            nTimeStamp,
                         int nOffset);

void BsQueryQuality(AudioDecoder* pDecoder,
                    int*               pValidPercent,
                    int*               vbv);

int AudioStreamDataSize(AudioDecoder* pDecoder);

int AudioStreamBufferSize(void);

int AudioStreamBufferMaxFrameNum(void);

int AudioPCMDataSize(AudioDecoder* pDecoder);

int DecRequestPcmBuffer(AudioDecoder* pDecoder, char **pOutWritePtr);

int DecUpdatePcmBuffer(AudioDecoder* pDecoder, int nPcmOutSize);

int PlybkRequestPcmBuffer(AudioDecoder* pDecoder, unsigned char **pOutReadPtr, int *psize);

int PlybkUpdatePcmBuffer(AudioDecoder* pDecoder, int nPcmOutSize);

int64_t PlybkRequestPcmPts(AudioDecoder* pDecoder);

void PcmQueryQuality(AudioDecoder* pDecoder, int* pValidPercent, int* vbv);

void AudioDecoderSeek(AudioDecoder* pDecoder,   int64_t nSeekTime);

int InitializeAudioDecoder(AudioDecoder*   pDecoder,
                           AudioStreamInfo*     pAudioStreamInfo,
                           BsInFor*             pBsInFor);

int ResetAudioDecoder(AudioDecoder*    pDecoder, int64_t nSeekTime);

int DecodeAudioStream(AudioDecoder*    pDecoder,
                      AudioStreamInfo*      pAudioStreamInfo,
                      char*                 ppBuf,
                      int*                  pBufSize);

int DestroyAudioDecoder(AudioDecoder* pDecoder);

AudioDecoder* CreateAudioDecoder(void);
#ifdef OS_LINUX
void SetRawPlayParam(AudioDecoder* pDecoder,void *self,int flag);
#else
void SetRawPlayParam(AudioDecoder* pDecoder,void *self);
#endif

#ifdef CEDARX_SUPPORT_SOUNDTOUCH
void AudioSoundTouchSetPitch(AudioDecoder* pDecoder, float pitchDelta);
#endif

#if VOLUME_SUPPORT
#ifdef OS_LINUX
int SetAudioVolume(AudioDecoder* pDecoder, int32_t nVolume);
#endif
#endif
#ifdef __cplusplus
}
#endif

#endif

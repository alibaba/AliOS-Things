/******************************************************************************

  Copyright (C), 2014-2020, ZheJiang Dahua Technology CO.,LTD.

 ******************************************************************************
  File Name      : api_venc.h
  Version          : Initial Draft
  Author           : Lv Zhuqing
  Created         : 2014.2.28
  Last Modified :
  Description    : 各模块非媒体视频类的公共数据类型
  Function List  :
  DHstory        :
  1.Date          : 2014/2/28
    Author        : 24497
    Modification: Create
******************************************************************************/
#ifndef __DH_COMMON_H__
#define __DH_COMMON_H__

#include "dh_type.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef VER_X
    #define VER_X 1
#endif

#ifndef VER_Y
    #define VER_Y 0
#endif

#ifndef VER_Z
    #define VER_Z 0
#endif

#ifndef VER_P
    #define VER_P 0
#endif

#ifndef VER_B
    #define VER_B "B000"
#endif

#ifdef DH_DEBUG
    #define VER_D " Debug"
#else
    #define VER_D " Release"
#endif

#define VERSION_NAME_MAX_LEN     (64)
/* ddr 名字长度 */
#define MAX_DDR_NAME_LEN         (10)


typedef struct tagMdkVersion
{
	DH_CHAR szVersion[VERSION_NAME_MAX_LEN];
}MDK_VERSION_S;

typedef struct tagPoint
{
    DH_S32 s32X;
    DH_S32 s32Y;
}POINT_S;

typedef struct tagSize
{
    DH_U32 u32Width;
    DH_U32 u32Height;
} SIZE_S;

typedef struct tagRect
{
    DH_S32 s32X;
    DH_S32 s32Y;
    DH_U32 u32Width;
    DH_U32 u32Height;
}RECT_S;

/* rectangle position */
typedef struct tagRectPos
{
	DH_S32	s32BeginX;    	/* 起始点横坐标*/
	DH_S32	s32BeginY;	    /* 起始点纵坐标*/
	DH_S32	s32EndX;		/* 结束点横坐标*/
	DH_S32	s32EndY;		/* 结束点纵坐标*/
} RECT_POS_S;

typedef struct tagBuffer
{
    DH_U32 u32Size;
    DH_U32 u32PhyAddr;
    DH_U32 u32VirtAddr;	
} BUFFER_S;

typedef enum tagRegBitWidth 
{
    REG_BIT_WIDTH1     = 0x1,   
    REG_BIT_WIDTH2     = 0x3,   
    REG_BIT_WIDTH3     = 0x7,   
    REG_BIT_WIDTH4     = 0xf,  
    REG_BIT_WIDTH5     = 0x1f,  
    REG_BIT_WIDTH6     = 0x3f, 
    REG_BIT_WIDTH7     = 0x7f, 
    REG_BIT_WIDTH8     = 0xff, 
    REG_BIT_WIDTH9     = 0x1ff, 
    REG_BIT_WIDTH10    = 0x3ff, 
    REG_BIT_WIDTH11    = 0x7ff, 
    REG_BIT_WIDTH12    = 0xfff, 
    REG_BIT_WIDTH13    = 0x1fff, 
    REG_BIT_WIDTH14    = 0x3fff, 
    REG_BIT_WIDTH15    = 0x7fff, 
    REG_BIT_WIDTH16    = 0xffff, 
    REG_BIT_WIDTH17    = 0x1ffff, 
    REG_BIT_WIDTH18    = 0x3ffff, 
    REG_BIT_WIDTH19    = 0x7ffff, 
    REG_BIT_WIDTH20    = 0xfffff, 
    REG_BIT_WIDTH21    = 0x1fffff, 
    REG_BIT_WIDTH22    = 0x3fffff, 
    REG_BIT_WIDTH23    = 0x7fffff, 
    REG_BIT_WIDTH24    = 0xffffff, 
    REG_BIT_WIDTH25    = 0x1ffffff, 
    REG_BIT_WIDTH26    = 0x3ffffff, 
    REG_BIT_WIDTH27    = 0x7ffffff, 
    REG_BIT_WIDTH28    = 0xfffffff, 
    REG_BIT_WIDTH29    = 0x1fffffff, 
    REG_BIT_WIDTH30    = 0x3fffffff, 
    REG_BIT_WIDTH31    = 0x7fffffff, 
    REG_BIT_WIDTH32    = 0xffffffff, 
} REG_BIT_WIDTH_E;

typedef DH_S32 VI_DEV;
typedef DH_S32 VI_CHN;
typedef DH_S32 VO_DEV;
typedef DH_S32 VO_CHN;
typedef DH_S32 VPS_GRP;
typedef DH_S32 VPS_CHN;
typedef DH_S32 VENC_GRP;
typedef DH_S32 VENC_CHN;
typedef DH_S32 VENC_STRM;
typedef DH_S32 VDEC_CHN;
typedef DH_S32 AI_CHN;
typedef DH_S32 AO_CHN;
typedef DH_S32 AENC_CHN;
typedef DH_S32 ADEC_CHN;
typedef DH_S32 AUDIO_DEV;
typedef DH_S32 ISP_DEV;
typedef DH_S32 SENSOR_ID;
typedef DH_S32 VD_CHN;

#define DH_INVALID_CHN (-1)
#define DH_INVALID_WAY (-1)
#define DH_INVALID_LAYER (-1)
#define DH_INVALID_DEV (-1)
#define DH_INVALID_HANDLE (-1)

#define DH_INVALID_VALUE (-1)
#define DH_INVALID_TYPE (-1)

typedef enum tagModId
{
    DH_ID_MDK     = 0,
    DH_ID_PROC    = 1,
    DH_ID_LOG     = 2,

    DH_ID_ISP	  = 3,
    DH_ID_VPS     = 4,

    DH_ID_VENC    = 5,
    DH_ID_H264E   = 6,
    DH_ID_H265E   = 7,
    DH_ID_JPEGE   = 8,
    DH_ID_MPEG4E  = 9,
    DH_ID_RC      = 10,

    DH_ID_AI      = 11,
    DH_ID_AO      = 12,
    DH_ID_AENC    = 13,
    DH_ID_ADEC    = 14,

    DH_ID_MBUF    = 15,
    DH_ID_VI      = 16,
    DH_ID_VO      = 17,

    DH_ID_VDEC    = 18,
    DH_ID_H264D   = 19,
    DH_ID_H265D   = 20,
    DH_ID_JPEGD   = 21,
    DH_ID_MPEG4D  = 22,
    
    DH_ID_VD      = 23,

    DH_ID_PCIV    = 24,
    DH_ID_USER    = 25,
	
    DH_ID_SYS     = 26,
    DH_ID_IAS     = 27,
    DH_ID_IAS_LL  = 28,
    DH_ID_FAAC    = 29,
    DH_ID_CIPHER  = 30,
    DH_ID_AEC     = 31,
    DH_ID_MIPI    = 32,
    DH_ID_OSD     = 33,
    DH_ID_VCTRL   = 34,

    DH_ID_BUTT
} MOD_ID_E;

typedef struct tagMdkChn
{
    MOD_ID_E    enModId;
    DH_S32      s32DevId;
    DH_S32      s32ChnId;
} MDK_CHN_S;

#define MDK_MOD_ISP       "ISP"
#define MDK_MOD_VI        "VI"
#define MDK_MOD_VPS       "VPS"
#define MDK_MOD_VO        "VO"
#define MDK_MOD_HDMI      "HDMI"
#define MDK_MOD_MIPI      "MIPI"
#define MDK_MOD_OSD       "OSD"
#define MDK_MOD_VCTRL     "VCTRL"

#define MDK_MOD_VENC      "VENC"
#define MDK_MOD_H264E     "H264E"
#define MDK_MOD_H265E     "H265E"
#define MDK_MOD_JPEGE     "JPEGE"
#define MDK_MOD_MPEG4E    "MPEG4E"

#define MDK_MOD_VDEC      "VDEC"
#define MDK_MOD_H264D     "H264D"
#define MDK_MOD_H265D     "H265D"
#define MDK_MOD_JPEGD     "JPEGD"
#define MDK_MOD_MPEG4D    "MPEG4D"

#define MDK_MOD_AUDIO     "AUDIO"
#define MDK_MOD_AI        "AI"
#define MDK_MOD_AO        "AO"
#define MDK_MOD_AENC      "AENC"
#define MDK_MOD_ADEC      "ADEC"
#define MDK_MOD_FAAC      "FAAC"
#define MDK_MOD_AEC       "AEC"


#define MDK_MOD_PCIV      "PCIV"

#define MDK_MOD_PROC      "PROC"
#define MDK_MOD_LOG       "LOGMDK"

#define MDK_MOD_FB        "FB"

#define MDK_MOD_RC        "RC"

#define MDK_MOD_TDE       "TDE"
#define MDK_MOD_MBUF      "MBUF"
#define MDK_MOD_VD        "VD"
#define MDK_MOD_SYS       "SYS"
#define MDK_MOD_IAS       "IAS"
#define MDK_MOD_USR       "USR"
#define MDK_MOD_MDK       "MDK"
#define MDK_MOD_IAS_LL    "IAS_LL"

/* We just coyp this value of payload type from RTP/RTSP definition */
typedef enum tagPayloadType
{
    PT_PCMU          = 0,
    PT_1016          = 1,
    PT_G721          = 2,
    PT_GSM           = 3,
    PT_G723          = 4,
    PT_DVI4_8K       = 5,
    PT_DVI4_16K      = 6,
    PT_LPC           = 7,
    PT_PCMA          = 8,
    PT_G722          = 9,
    PT_S16BE_STEREO  = 10,
    PT_S16BE_MONO    = 11,
    PT_QCELP         = 12,
    PT_CN            = 13,
    PT_MPEGAUDIO     = 14,
    PT_G728          = 15,
    PT_DVI4_3        = 16,
    PT_DVI4_4        = 17,
    PT_G729          = 18,
    PT_G711A         = 19,
    PT_G711U         = 20,
    PT_G726          = 21,
    PT_G729A         = 22,
    PT_LPCM          = 23,
    PT_CelB          = 25,
    PT_JPEG          = 26,
    PT_CUSM          = 27,
    PT_NV            = 28,
    PT_PICW          = 29,
    PT_CPV           = 30,
    PT_H261          = 31,
    PT_MPEGVIDEO     = 32,
    PT_MPEG2TS       = 33,
    PT_H263          = 34,
    PT_SPEG          = 35,
    PT_MPEG2VIDEO    = 36,
    PT_AAC           = 37,
    PT_WMA9STD       = 38,
    PT_HEAAC         = 39,
    PT_PCM_VOICE     = 40,
    PT_PCM_AUDIO     = 41,
    PT_AACLC         = 42,
    PT_MP3           = 43,
    PT_ADPCMA        = 49,
    PT_AEC           = 50,
    PT_X_LD          = 95,
    PT_H264          = 96,
    PT_D_GSM_HR      = 200,
    PT_D_GSM_EFR     = 201,
    PT_D_L8          = 202,
    PT_D_RED         = 203,
    PT_D_VDVI        = 204,
    PT_D_BT656       = 220,
    PT_D_H263_1998   = 221,
    PT_D_MP1S        = 222,
    PT_D_MP2P        = 223,
    PT_D_BMPEG       = 224,
    PT_MP4VIDEO      = 230,
    PT_MP4AUDIO      = 237,
    PT_VC1           = 238,
    PT_JVC_ASF       = 255,
    PT_D_AVI         = 256,
    PT_DIVX3		 = 257,
    PT_AVS		     = 258,
    PT_REAL8		 = 259,
    PT_REAL9		 = 260,
    PT_VP6		     = 261,
    PT_VP6F		     = 262,
    PT_VP6A	         = 263,
    PT_SORENSON	     = 264,
    PT_H265          = 265,
    PT_MAX           = 266,

    /* add by dahua */
    PT_MJPEG         = 801,
    PT_BYPASS        = 802,
    
    PT_BUTT
}PAYLOAD_TYPE_E;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* _DH_COMMON_H_ */

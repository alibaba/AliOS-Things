
#ifndef _MED_AEC3_COMM_H_
#define _MED_AEC3_COMM_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "med_fft.h"
#include "codec_com_codec.h"
#include "med_aec3_main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* FFT IFFT回调函数 */
typedef VOS_VOID (*MED_AEC_FFT_CALLBACK)(VOS_INT16 *, VOS_INT16 *);

#ifdef AEC_ONLY_SUPPORT_8k
#ifdef SPEECH_STREAM_UNIT_128
#define MED_AEC_FFT_LEN_NB              (256)
#else
#define MED_AEC_FFT_LEN_NB              (240)
#endif
#define MED_AEC_MAX_FFT_LEN             (MED_AEC_FFT_LEN_NB)                    /* 宽带FFT长度 */
#else
#ifdef SPEECH_STREAM_UNIT_128
#define MED_AEC_FFT_LEN_NB              (256)
#define MED_AEC_FFT_LEN_WB              (512)
#else
#define MED_AEC_FFT_LEN_NB              (240)
#define MED_AEC_FFT_LEN_WB              (480)
#endif
#define MED_AEC_MAX_FFT_LEN             (MED_AEC_FFT_LEN_WB)                    /* 宽带FFT长度 */
#endif

#define MED_AEC_HF_AF_M                 (6)

/* 临时全局数组，供AEC各模块使用 */
//extern VOS_INT16                        g_ashwMedAecTmp1Len640[MED_AEC_MAX_FFT_LEN];
//extern VOS_INT16                        g_ashwMedAecTmp2Len640[MED_AEC_MAX_FFT_LEN];
//extern VOS_INT32                        g_aswMedAecTmp1Len320[CODEC_FRAME_LENGTH_WB];
//extern VOS_INT32                        g_aswMedAecTmp1Len640[MED_AEC_MAX_FFT_LEN];

/* AEC 公共宏定义*/
//#define MED_AEC_GetshwVecTmp640Ptr1()   (&g_ashwMedAecTmp1Len640[0])            /* 临时全局数组指针 长度640 INT16 */
//#define MED_AEC_GetshwVecTmp640Ptr2()   (&g_ashwMedAecTmp2Len640[0])            /* 临时全局数组指针 长度640 INT16 */
//#define MED_AEC_GetswVecTmp320Ptr1()    (&g_aswMedAecTmp1Len320[0])             /* 临时全局数组指针 长度320 INT32 */
//#define MED_AEC_GetswVecTmp640Ptr1()    (&g_aswMedAecTmp1Len640[0])             /* 临时全局数组指针 长度640 INT32 */

#define MED_AEC_OFFSET_THD              (60)                                    /* 延迟补偿阈值 */
#define MED_AEC_MAX_OFFSET              (960)                                   /* 最大补偿长度，单位采样点 Q0*/
#define MED_AEC_MAX_TAIL_LEN            (960)                                   /* 最大尾端长度，单位ms，最大支持60ms尾端延迟 Q0*/
/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/* AF FFT长度 */
enum MED_AEC_AF_FFT_LEN_ENUM
{
#ifdef SPEECH_STREAM_UNIT_128
    MED_AEC_AF_FFT_LEN_NB = 256,                                                /* 窄带FFT长度 */
    MED_AEC_AF_FFT_LEN_WB = 512,                                                /* 宽带FFT长度 */
#else
    MED_AEC_AF_FFT_LEN_NB = 240,                                                /* 窄带FFT长度 */
    MED_AEC_AF_FFT_LEN_WB = 480,                                                /* 宽带FFT长度 */
#endif
    MED_AEC_AF_FFT_LEN_BUTT
};
typedef VOS_INT16  MED_AEC_AF_FFT_LEN_ENUM_INT16;

/* 开关量枚举 */
enum CODEC_SWITCH_ENUM
{
	CODEC_SWITCH_OFF = 0,
	CODEC_SWITCH_ON,
	CODEC_SWITCH_BUTT
};
typedef VOS_UINT16 CODEC_SWITCH_ENUM_UINT16;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/

/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/
#ifdef DEBUG_AEC
static VOS_VOID fprint_vec_int32(FILE *fd, VOS_INT32 *buf, VOS_INT32 len)
{
    for (VOS_INT32 i = 0; i < len - 1; i++) {
        fprintf(fd, "%d ", buf[i]);
    }
    fprintf(fd, "%d\n", buf[len - 1]);
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_main.h*/


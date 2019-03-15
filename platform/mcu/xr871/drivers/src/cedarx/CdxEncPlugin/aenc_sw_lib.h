/*
********************************************************************************
*                                    eMOD
*                   the Easy Portable/Player Develop Kits
*                               mod_herb sub-system
*                          (module name, e.g.mpeg4 decoder plug-in) module
*
*          (c) Copyright 2009-2010, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : aenc_sw_lib.h
* Version: V1.0
* By     : Eric_wang
* Date   : 2010-4-26
* Description:
********************************************************************************
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "aencoder.h"

#ifndef _AENC_SW_LIB_H_
#define _AENC_SW_LIB_H_

#define FIFO_LEVEL	(4)//(32)

#define BS_BUFFER_SIZE       (12*1024)//(128*1024)
//#define PCM_FRAME_SIZE       (32*1024)
#define BS_HEADER_SIZE       (1*1024)

//#define OUT_BUFFER_SIZE         (512*1024)
#define OUT_ENCODE_BUFFER_SIZE  (2*1024)//(4*1024)    //byte
#define MAXDECODESAMPLE (1024)

//encode input data information
typedef struct __audio_enc_inf_t
{
    int     InSamplerate;   //输入fs
    int     InChan;         //输入pcm chan 1:mon 2:stereo
    int     bitrate;        //bs
    int     SamplerBits;    //only for 16bits
    int     OutSamplerate;  //输出fs,now OutSamplerate must equal InSamplerate
    int     OutChan;        //编码输出 chan
    int     frame_style;    //for aac: 0:add head,1:raw data; for pcm: 2:mpegTs pcm(big endian), other: common pcm(little endian)

}__audio_enc_inf_t;

//#define BsBuffLEN 20*1024
typedef struct __PCM_BUF_MANAGER
{
    unsigned char       *pBufStart;         //buffer首地址
    int                 uBufTotalLen;       //总长度
    unsigned char       *pBufReadPtr;       //正在读的指针
    int                 uDataLen;           //有效数据长度
    unsigned char       *pBufWritPtr;       //正在写的指针
    int                 uFreeBufSize;       //空余长度
	int                 uDataFlowflag;      //数据是否超过预定解码，1：超过补0帧，0：没有超过，正常编码
	void				*parent;			// encode use,not lib use;

} __pcm_buf_manager_t;


typedef struct __COM_INTERNAL_PARAMETER
{
    unsigned int        ulNowTimeMS;        //当前时间ms
    unsigned int        ulPCMgainSet;       //PCM增益
    unsigned int        framecount;         //frames

    unsigned int        ulEncCom;           //0: normal enc 1: ff end   5: stop
    unsigned char       BsHeaderBuf[BS_HEADER_SIZE];
    unsigned int        ValidHeaderLen;
	unsigned int        *pEncInfoSet;		// encode lib use;

} __com_internal_prameter_t;


typedef struct __AudioENC_AC320
{
	__pcm_buf_manager_t         *pPcmBufManager;
	__audio_enc_inf_t           *AudioBsEncInf;
	__com_internal_prameter_t   *EncoderCom;
	int                         Encinitedflag;

	int	(*EncInit)(struct __AudioENC_AC320 *p);
	int	(*EncFrame)(struct __AudioENC_AC320 *p,char *OutBuffer,int *OutBuffLen);
	int	(*EncExit)(struct __AudioENC_AC320 *p);

}AudioEnc_AC320;

extern struct __AudioENC_AC320 *EncInit(void);
extern int  EncExit(struct __AudioENC_AC320 *p);

struct __AudioENC_AC320 *AudioMP3ENCEncInit(void);
int  AudioMP3ENCEncExit(struct __AudioENC_AC320 *p);

//编码库所需要的需上层模块实现的函数
extern int GetPcmDataSize(__pcm_buf_manager_t *pPcmMan);
extern int ReadPcmDataForEnc(void *pBuf, int uGetLen, __pcm_buf_manager_t *pPcmMan);

#endif  /* _AENC_SW_LIB_H_ */

#ifdef __cplusplus
}
#endif

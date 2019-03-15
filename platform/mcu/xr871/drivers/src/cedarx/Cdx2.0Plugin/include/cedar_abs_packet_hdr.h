/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                              mod_mmp sub-system
*
*                                    (c) Copyright 2008-2009, Kevin.Z China
*                                              All Rights Reserved
*
* File   : cedar_abs_packet_hdr.h
* Version: V1.0
* By     : kevin.z
* Date   : 2009-4-17 10:23
*********************************************************************************************************
*/
#ifndef _CEDAR_ABS_PACKET_HDR_H_
#define _CEDAR_ABS_PACKET_HDR_H_

#include "adecoder.h"
#include "ad_cedarlib_com.h"

int AdCedarBuildAACPacketHdr(unsigned char* extraData, 
                       int            extraDataLen , 
                       int            uPacketLen, 
                       unsigned char* pBuf, 
                       int*           pHdrLen,
                       int            channels,
                       int            sampleRate);
                             
int AdCedarUpdateAACPacketHdr(unsigned char* pBuf, int uHdrLen, int uPacketLen);

int split_xiph_headers(unsigned char* codec_priv, 
                       int            codec_priv_size,
                       char**         extradata, 
                       int*           extradata_size);
                       
int SetAudioBsHeader_PCM(AudioStreamInfo* tmpAbsFmt, Ac320FileRead* FileReadInfo);

/*
--------------------------------------------------------------------------------
AAC ADTS HEADER BIT INFORMATION DEFINE, BASED ON BIT STREAM
--------------------------------------------------------------------------------
syncword:12     0xfff
id      :1      0x0
layer   :2      0x0
protect :1      0x1
profile :2
fsidx   :4
priv    :1      0x0
chncfg  :3
origcpy :1      0x0
home    :1      0x0

cpybit  :1      0x0
cpystart:1      0x0
frmlen  :13
buffull :11     0x7ff
rawblk  :2      0x1
--------------------------------------------------------------------------------
*/


#endif //_CEDAR_ABS_PACKET_HDR_H_

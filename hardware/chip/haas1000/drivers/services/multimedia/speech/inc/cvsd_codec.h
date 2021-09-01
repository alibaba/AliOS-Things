/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
/*******************************************************************************
** namer��CVSD CODEC header
** description��CVSD encoder and CVSD decoder header
** version��V1.0
** author: xuml
** modify��2015.8.21.
*******************************************************************************/

#ifndef _CVSD_CODEC_H_
#define _CVSD_CODEC_H_


//Q fixed
#define CVSD_Q 10

struct CvsdEncSt
{
	unsigned short CvsdEncJudgeAlpha;
	int   CvsdEncLastData;
	int   CvsdEncDelta;
	int CvsdEncFirstFrameFlag;
	int FilterState0[3];
	int FilterState1[3];
	int FilterState2[3];
};
//decoder
struct CvsdDecSt
{
	unsigned short CvsdDecJudgeAlpha;
	int   CvsdDecLastData;
	int   CvsdDecDelta;
	int FilterState0[3];
	int FilterState1[3];
	int FilterState2[3];
};

#ifdef __cplusplus
extern "C" {
#endif

//encoder
void CvsdEncInit(struct CvsdEncSt *CvsdEnc);
int CvsdEncode(struct CvsdEncSt *CvsdEnc, short *pInPutData, short *pOutPutdata, int iEncSize);

//decoder
void CvsdDecInit(struct CvsdDecSt *CvsdDec);
int CvsdDecode(struct CvsdDecSt *CvsdDec, short *pInPutData, short *pOutPutdata, int iDecSize);

#ifdef __cplusplus
}
#endif

#endif













/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
/////////////////////////////////////////////////////////////////////////
//PCM8K <=> CVSD
/////////////////////////////////////////////////////////////////////////
#ifndef _PCM8K_CVSD_H_
#define _PCM8K_CVSD_H_



//max num of input samples
#define MAXNUMOFSAMPLES 128


//resample delay
#define RESAMPLE_DELAY 32


void Pcm8k_CvsdInit(void);
void Pcm8kToCvsd(short *PcmInBuf, unsigned char *CvsdOutBuf, int numsample);
void CvsdToPcm8k(unsigned char *CvsdInBuf, short *PcmOutBuf, int numsample, int LossFlag);


#endif













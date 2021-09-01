/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
/////////////////////////////////////////////////////////////////////////
//Packets Loss Concealment                                              /
/////////////////////////////////////////////////////////////////////////
#ifndef _PLC_8000_H_
#define _PLC_8000_H_


typedef struct PlcSt_8000_ PlcSt_8000;

PlcSt_8000 *speech_plc_8000_init(void* (* speex_alloc_ext)(int));
int speech_plc_8000(PlcSt_8000 *lc, short *InBuf, int len);

#endif













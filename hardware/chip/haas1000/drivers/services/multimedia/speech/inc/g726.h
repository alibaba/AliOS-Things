/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __G726_H
#define __G726_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned long g726_Encode(unsigned char *speech,char *bitstream, uint32_t sampleCount, uint8_t isReset);
unsigned long g726_Decode(char *bitstream,unsigned char *speech, uint32_t sampleCount, uint8_t isReset);

#ifdef __cplusplus
}
#endif

#endif


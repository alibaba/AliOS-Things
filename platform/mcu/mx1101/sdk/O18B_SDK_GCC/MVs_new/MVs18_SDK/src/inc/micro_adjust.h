//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: micro_adjust.h
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////
/*

*/
#ifndef __MICRO_ADJUST_H__
#define __MICRO_ADJUST_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

void DacAdjust(void);

#ifdef FUNC_SOFT_ADJUST_EN

    
void SoftAdjustInit(uint32_t ChannelNum);
uint32_t BtSoftAdjustApply(uint32_t SampleLevel, int16_t *PcmInAddr, uint32_t PcmLen, uint32_t CurChannelNum);
    
#endif

#ifdef __cplusplus
}
#endif//__cplusplus

#endif //__MICRO_ADJUST_H__


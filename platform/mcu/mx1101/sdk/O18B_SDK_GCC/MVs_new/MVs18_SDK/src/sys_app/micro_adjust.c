//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: micro_adjust.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////
/*

*/

#include "type.h"
#include "app_config.h"
#include "sra.h"
#include "dac.h"
#include "pcm_fifo.h"
#include "bt_stack_api.h"

#include "micro_adjust.h"

#define  PCM_FIFO_SBC_SAMPLES   4096

#ifdef FUNC_SOFT_ADJUST_EN
int16_t AudioSoftAdjust[130*4];
SRAContext  gSRAContext;
#endif

static uint32_t DacAdjustCnt = 0;
void DacAdjust(void)
{
//    uint32_t i;
    
    #define PPMSLOW  -4000
    #define PPMFAST   3000
    
    
    if(DacAdjustCnt++ < 500)
    {
        return;
    }
    DacAdjustCnt = 0;
    
    //if(GetRcvListItemNum() < 1)
    {
        DacSampRateAdjust(TRUE, PPMSLOW*3);//DacAdjustSlower
    }
   // else if(GetRcvListItemNum() < 2)//0,1
    {
        DacSampRateAdjust(TRUE, PPMSLOW*2);//DacAdjustSlower       
    }
   // else if(GetRcvListItemNum() < 4)//2,3
    {
        if(PcmFifoGetRemainSamples() > PCM_FIFO_SBC_SAMPLES - 128)
        {
            DacSampRateAdjust(TRUE, PPMSLOW/2);//DacAdjustSlower
        }
        else
        {
            DacSampRateAdjust(TRUE, PPMSLOW);//DacAdjustSlower
        }
    }
   // else if(GetRcvListItemNum() >= 5)//5,6,others
    {
        if(PcmFifoGetRemainSamples() > PCM_FIFO_SBC_SAMPLES - 256)
        {
            DacSampRateAdjust(TRUE, PPMFAST);//DacAdjustFaster
        }
        else if(0 == PcmFifoGetRemainSamples())//empty
        {
            DacSampRateAdjust(TRUE, PPMSLOW*2);//DacAdjustSlower
        }
        else
        {
            DacSampRateAdjust(TRUE, PPMSLOW/2);//DacAdjustNormal
        }  
    }
   // else //4
    {
        if(PcmFifoGetRemainSamples() > PCM_FIFO_SBC_SAMPLES - 512)
        {
            DacSampRateAdjust(TRUE, 0);
        }
        else
        {
            DacSampRateAdjust(TRUE, PPMSLOW/2);
        }
    }
 
}
#ifdef FUNC_SOFT_ADJUST_EN
void SoftAdjustInit(uint32_t ChannelNum)
{
    init_sra(&gSRAContext, ChannelNum);   
}

static int32_t  AdjustStep = 0;
//static int32_t  RcvIdx = 0;

//该函数仅适用于蓝牙接收，因为GetRcvListItemNum
uint32_t BtSoftAdjustApply(uint32_t SampleLevel, int16_t *PcmInAddr, uint32_t PcmLen, uint32_t CurChannelNum)
{
    uint32_t PcmRemainLen;
    

    PcmRemainLen = PcmFifoGetRemainSamples();
    if(GetRcvListItemNum() < 1)// 0
    {
        AdjustStep = 1;
    }
    else if(GetRcvListItemNum() < 2)// 1
    {
        if(PcmRemainLen >= SampleLevel - 30)
        {
            AdjustStep = 0;
        }
        else
        {
            AdjustStep = 1;
        }
          
    }
    else if(GetRcvListItemNum() < 4)//2,3
    {
        if(PcmRemainLen <= 128)
        {
            AdjustStep = 1;
        }    
        else
        {
            AdjustStep = 0;            
        }
        
    }
    else if(GetRcvListItemNum() >= 5)//5,6,others
    {
        if(PcmRemainLen >= SampleLevel - 256)
        {
            AdjustStep = (AdjustStep == 1? 0: -1);
        }
        else if(PcmRemainLen <=  128)
        {
            AdjustStep = 1;
        }
        else
        {
            AdjustStep = (AdjustStep == 1? 0: -1);
        }
    }
    else                            //4
    {
        if(PcmRemainLen >= SampleLevel - 256)
        {
            AdjustStep = (AdjustStep == 1? 0: -1);
        }
        else if(PcmRemainLen <= 128)
        {
            AdjustStep = 1;
        }    
        else
        {
            AdjustStep = 0;
        }
    }
//    if(RcvIdx != GetRcvListItemNum())
//    {
//        DBG("%d, %d\n",GetRcvListItemNum(), PcmRemainLen);
//        RcvIdx = GetRcvListItemNum();
//    }
    apply_sra(&gSRAContext, PcmInAddr, (int16_t *)AudioSoftAdjust, AdjustStep);
    memcpy(PcmInAddr, AudioSoftAdjust, ((PcmLen + AdjustStep) << CurChannelNum));
    return (PcmLen + AdjustStep);
}

#endif
//


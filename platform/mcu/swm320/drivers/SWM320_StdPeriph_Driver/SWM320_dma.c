/******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/

#include "SWM320.h"
#include "SWM320_dma.h"

void DMA_CHM_Config(uint32_t chn, uint32_t src_addr, uint32_t src_addr_incr, uint32_t dst_addr, uint32_t dst_addr_incr, uint32_t num_word, uint32_t int_en)
{
    DMA->EN = 1;

    DMA_CH_Close(chn);

    DMA->CH[chn].SRC = src_addr;
    DMA->CH[chn].DST = dst_addr;

    DMA->CH[chn].CR = ((num_word*4-1) << DMA_CR_LEN_Pos) |
                      (0 << DMA_CR_AUTORE_Pos);

    DMA->CH[chn].AM = (src_addr_incr << DMA_AM_SRCAM_Pos) |
                      (dst_addr_incr << DMA_AM_DSTAM_Pos) |
                      (0 << DMA_AM_BURST_Pos);

    DMA->IF  = (1 << chn);
    DMA->IE |= (1 << chn);
    if(int_en)  DMA->IM &= ~(1 << chn);
    else        DMA->IM |=  (1 << chn);

    if(int_en)
    {
        NVIC_EnableIRQ(DMA_IRQn);
    }
}

void DMA_CH_Open(uint32_t chn)
{
    DMA->CH[chn].CR |= (1 << DMA_CR_TXEN_Pos);
}

void DMA_CH_Close(uint32_t chn)
{
    DMA->CH[chn].CR &= ~(1 << DMA_CR_TXEN_Pos);
}

void DMA_CH_INTEn(uint32_t chn)
{
    DMA->IM &= ~(1 << chn);
}

void DMA_CH_INTDis(uint32_t chn)
{
    DMA->IM |= (1 << chn);
}

void DMA_CH_INTClr(uint32_t chn)
{
    DMA->IF = (1 << chn);
}

uint32_t DMA_CH_INTStat(uint32_t chn)
{
    return (DMA->IF & (1 << chn)) ? 1 : 0;
}

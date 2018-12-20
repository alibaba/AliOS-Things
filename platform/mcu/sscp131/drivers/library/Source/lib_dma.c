/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#include "lib_dma.h"
#include "type.h"

volatile DMA_Descriptor_Typedef dma_ctrl_base[64]  __attribute__((aligned(1024)));

void DMA_Init(DMA_Channel_Typedef DMA_Channel, DMA_InitStruType *DMA_InitStruct)
{
    DMA_Descriptor_Typedef *descr;
    
    DMA->CTRLBASEPTR.Word = (uint32_t)&dma_ctrl_base;
    
    
    if (DMA_InitStruct->primary)
        descr = (DMA_Descriptor_Typedef *)(DMA->CTRLBASEPTR.Word) + DMA_InitStruct->channel;
    else
        descr = (DMA_Descriptor_Typedef *)(DMA->ALTCTRLBASEPTR.Alt_ctrl_base_ptr) + DMA_InitStruct->channel;

    if (DMA_InitStruct->src_inc == DMA_DATA_INC_NONE)
        descr->src = DMA_InitStruct->src;
    else
        descr->src = (void *)((uint32_t)DMA_InitStruct->src + ((DMA_InitStruct->size - 1) << DMA_InitStruct->data_width));

    if (DMA_InitStruct->dst_inc == DMA_DATA_INC_NONE)
        descr->dst = DMA_InitStruct->dst;
    else
        descr->dst = (void *)((uint32_t)DMA_InitStruct->dst + ((DMA_InitStruct->size - 1) << DMA_InitStruct->data_width));

    descr->ctrl.cycle_ctrl    = DMA_InitStruct->CycleCtrl;
    descr->ctrl.next_useburst = 0;
    descr->ctrl.n_minus_1     = DMA_InitStruct->size - 1;
    descr->ctrl.R_power       = DMA_InitStruct->R_power;
    descr->ctrl.src_prot_ctrl = 0,
    descr->ctrl.dst_prot_ctrl = 0,
    descr->ctrl.src_size      = DMA_InitStruct->data_width;
    descr->ctrl.src_inc       = DMA_InitStruct->src_inc;
    descr->ctrl.dst_size      = DMA_InitStruct->data_width;
    descr->ctrl.dst_inc       = DMA_InitStruct->dst_inc;

    if (DMA_InitStruct->primary) 
        DMA->PRIALTCLR.Word |= (1 << DMA_InitStruct->channel);
    else
        DMA->PRIALTSET.Word |= (1 <<DMA_InitStruct->channel);

    if (DMA_InitStruct->burst)
        DMA->USEBURSTSET.Word |= (1 << DMA_InitStruct->channel);
    else
        DMA->USEBURSTCLR.Word |= (1 << DMA_InitStruct->channel);

    if (DMA_InitStruct->high_prio)
        DMA->PRIORITYSET.Word |= (1 << DMA_InitStruct->channel);
    else
        DMA->PRIORITYCLR.Word |= (1 << DMA_InitStruct->channel);

    
    DMA->CFG.EN=1;
    
    
}

void DMA_ChannelEnabled(DMA_Channel_Typedef channel)
{
    DMA->ENBSET.Word |= 1<<channel;
}

void DMA_ChannelDisable(DMA_Channel_Typedef channel)
{
    DMA->ENBCLR.Word |= 1<<channel;
}

FlagStatus DMA_GetChannelStatus(DMA_Channel_Typedef channel)
{
    FlagStatus bitstatus = RESET;
    if(DMA->ENBSET.Chnl_enable_set & (1 << channel))
        bitstatus = SET;
    return bitstatus;
}

void DMA_SoftRequest(DMA_Channel_Typedef channel)
{
    DMA->SWREQ.Chnl_sw_request |= 1 << channel;
}

FlagStatus DMA_GetFlagStatus(DMA_Channel_Typedef channel)
{
    FlagStatus bitstatus = RESET;
    if(SCU->DMADONE.Word & (1 << channel))
        bitstatus = SET;
    return bitstatus;
}

void DMA_ClearITPendingBit(DMA_Channel_Typedef channel)
{
    SCU->DMADONE.Word &= ~(1 << channel);
}

void DMA_MaskRequest(DMA_Channel_Typedef channel, TYPE_FUNCEN state)
{
    if(state == DISABLE)
        DMA->REQMASKCLR.Chnl_req_mask_clr |= 1 << channel;
    else
        DMA->REQMASKSET.Chnl_req_mask_set |= 1 << channel;
}

FlagStatus DMA_GetMaskRequestStype(DMA_Channel_Typedef channel)
{
    FlagStatus bitstatus = RESET;
    if(DMA->REQMASKSET.Word & (1 << channel))
        bitstatus = SET;
    return bitstatus;
}

FlagStatus DMA_GetErrStype(DMA_Channel_Typedef channel)
{
    FlagStatus bitstatus = RESET;
    if(DMA->ERRCLR.Word & (1 << channel))
        bitstatus = SET;
    return bitstatus;
}

void DMA_ClearErr(DMA_Channel_Typedef channel)
{
    DMA->ERRCLR.Word &= ~(1 << channel);
}

void DMA_ClearPriority(DMA_Channel_Typedef channel)
{
    DMA->PRIORITYCLR.Word &= ~(1 << channel);
}

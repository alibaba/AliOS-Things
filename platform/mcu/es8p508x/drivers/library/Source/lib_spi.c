/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/
#include "lib_spi.h"

void SPI_Init(SPI_InitStruType* SPI_InitStruct)
{
    uint32_t Temp = 0;

    Temp = SystemCoreClock / (SPI_InitStruct->SPI_Freq * 2);
    if(Temp > 254)
        Temp = 255;
    
    SPI0->CKS.CKS = Temp;
    
    SPI0->CON.DFS = SPI_InitStruct->SPI_Df;
    SPI0->CON.MS = SPI_InitStruct->SPI_Mode;
    SPI0->CON.DW = SPI_InitStruct->SPI_DW;
    SPI0->CON.DRE = SPI_InitStruct->SPI_DelayRec;
    SPI0->CON.TME = SPI_InitStruct->SPI_DelaySend;

    Temp = (uint32_t)SPI_InitStruct->SPI_SendDelayPeroid - 1;
    if(Temp > 62)
        Temp = 63;

    SPI0->CON.TMP = Temp;
}

void SPI_ITConfig(SPI_TYPE_IT SPI_IE,TYPE_FUNCEN NewState)
{
    if(NewState != DISABLE)
        SPI0->IE.Word |= (uint32_t)SPI_IE;
    else
        SPI0->IE.Word &= ~((uint32_t)SPI_IE);
}

void SPI_DataFormatConfig(SPI_TYPE_DFS Type)
{
    SPI0->CON.DFS = Type;
}

void SPI_SendByte(uint8_t Temp)
{
    SPI0->TBW.Byte[0] = Temp;
}

void SPI_SendHalfWord(uint16_t Temp)
{
    SPI0->TBW.HalfWord[0] = Temp;
}

void SPI_SendWord(uint32_t Temp)
{
    SPI0->TBW.Word = Temp;
}

uint8_t SPI_RecByte(void)
{
    return SPI0->RBR.Byte[0];
}

uint16_t SPI_RecHalfWord(void)
{
    return SPI0->RBR.HalfWord[0];
}

uint32_t SPI_RecWord(void)
{
    return SPI0->RBR.Word;
}

void SPI_TBIMConfig(SPI_TYPE_TRBIM Type)
{
    SPI0->IE.TBIM = Type;
}

void SPI_RBIMConfig(SPI_TYPE_TRBIM Type)
{
    SPI0->IE.RBIM = Type;
}

FlagStatus SPI_GetFlagStatus(SPI_TYPE_FLAG Flag)
{
    FlagStatus bitstatus = RESET;

    if((uint32_t)Flag <= 0x00000040)
    {
        if ((SPI0->IF.Word & Flag) != (uint32_t)RESET)
            bitstatus = SET;
        else
            bitstatus = RESET;
    }

    else
    {
        if ((SPI0->STA.Word & Flag) != (uint32_t)RESET)
            bitstatus = SET;
        else
            bitstatus = RESET;
    }

    return  bitstatus;
}

ITStatus SPI_GetITStatus(SPI_TYPE_IT Flag)
{
    FlagStatus bitstatus = RESET;
    uint32_t itstatus = 0x0, itenable = 0x0;

    itstatus = SPI0->IF.Word & (uint32_t)Flag;
    itenable = SPI0->IE.Word & (uint32_t)Flag;

    if ((itstatus != (uint32_t)RESET) && (itenable != (uint32_t)RESET))
        bitstatus = SET;
    else
        bitstatus = RESET;

    return  bitstatus;    
}

void SPI_ClearITPendingBit(SPI_CLR_IF Flag)
{
    SPI0->IF.Word = (uint32_t)Flag;
}

/*************************END OF FILE**********************/


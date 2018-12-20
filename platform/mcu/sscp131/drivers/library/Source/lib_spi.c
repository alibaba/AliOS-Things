/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/
#include "lib_spi.h"
#include "lib_scu.h"
#include "system_P131.h"

void SPI_Init(SPI_TypeDef *SPIx, SPI_InitStruType *SPI_InitStruct)
{
    uint32_t tmp = 0;
    SCU_ClocksTypeDef SCU_ClocksStatus;
    
    SCU_GetClocksFreq(&SCU_ClocksStatus);
    
    if (SPI_InitStruct->SPI_Freq != SCU_ClocksStatus.APB1_Frequency) {
        tmp = SCU_ClocksStatus.APB1_Frequency / (SPI_InitStruct->SPI_Freq << 1);
        tmp = tmp > 0x3ff ? 0x3ff : tmp;
    }

    SPIx->CKS.CKS = tmp;
    SPIx->CON.DW  = SPI_InitStruct->SPI_DW;
    SPIx->CON.DFS = SPI_InitStruct->SPI_Df;
    SPIx->CON.MS  = SPI_InitStruct->SPI_Mode;
    SPIx->CON.DRE = SPI_InitStruct->SPI_DelayRec;
    SPIx->CON.TME = SPI_InitStruct->SPI_DelaySend;

    tmp = (uint32_t)SPI_InitStruct->SPI_SendDelayPeroid - 1;
    tmp = tmp > 63 ? 63 : tmp;
    SPIx->CON.TMP   = tmp;
    SPIx->CON.TXCLR = 1;
    SPIx->CON.RXCLR = 1;    

    return;
}

void SPI_ITConfig(SPI_TypeDef *SPIx, SPI_TYPE_IT SPI_IE, TYPE_FUNCEN NewState)
{
    if (NewState != DISABLE)
        SPIx->IE.Word |= (uint32_t)SPI_IE;
    else
        SPIx->IE.Word &= ~(uint32_t)SPI_IE;

    return;
}

void SPI_DataFormatConfig(SPI_TypeDef *SPIx, SPI_TYPE_DFS Type)
{
    SPIx->CON.DFS = Type;
    return;
}

void SPI_SendByte(SPI_TypeDef *SPIx, uint8_t Temp)
{
    SPIx->TBW.Byte[0] = Temp;
    return;

}

void SPI_SendHalfWord(SPI_TypeDef *SPIx, uint16_t Temp)
{
    SPIx->TBW.HalfWord[0] = Temp;
    return;
}

void SPI_SendWord(SPI_TypeDef *SPIx, uint32_t Temp)
{
    SPIx->TBW.Word = Temp;
    return;
}

uint8_t SPI_RecByte(SPI_TypeDef *SPIx)
{
    return SPIx->RBR.Byte[0];
}

uint16_t SPI_RecHalfWord(SPI_TypeDef *SPIx)
{
    return SPIx->RBR.HalfWord[0];
}

uint32_t SPI_RecWord(SPI_TypeDef *SPIx)
{
    return SPIx->RBR.Word;
}

void SPI_TBIMConfig(SPI_TypeDef *SPIx, SPI_TYPE_TRBIM Type)
{
    SPIx->IE.TBIM = Type;
    return;
}

void SPI_RBIMConfig(SPI_TypeDef *SPIx, SPI_TYPE_TRBIM Type)
{
    SPIx->IE.RBIM = Type;
    return;
}

FlagStatus SPI_GetFlagStatus(SPI_TypeDef *SPIx, SPI_TYPE_FLAG Flag)
{
    FlagStatus status = RESET;

    if (Flag != SPI_Flag_TMS) {
        if (SPIx->IF.Word & (uint32_t)Flag)
            status = SET;
    }
    else {
        if (SPIx->CON.Word & (uint32_t)Flag)
            status = SET;
    }

    return status;
}

ITStatus SPI_GetITStatus(SPI_TypeDef *SPIx, SPI_TYPE_IT Flag)
{
    ITStatus status = RESET;

    if (SPIx->IE.Word & (uint32_t)Flag)
        status = SET;

    return status;
}

FlagStatus SPI_GetStatus(SPI_TypeDef *SPIx, SPI_TYPE_STA Flag)
{
    FlagStatus status = RESET;

    if (SPIx->STA.Word & (uint32_t)Flag)
        status = SET;

    return status;
}

void SPI_ClearITPendingBit(SPI_TypeDef *SPI, SPI_CLR_IF Flag)
{
    SPI->IF.Word = (uint32_t)Flag;
    return;
}

void Clear_TBW(SPI_TypeDef *SPIx)
{
    SPIx->CON.TXCLR = 1;
    return;
}

void Clear_RBR(SPI_TypeDef *SPIx)
{
    SPIx->CON.RXCLR = 1;
    return;
}

void SPI_configTXDMAEN(SPI_TypeDef *SPIx,TYPE_FUNCEN state)
{
    SPIx->CON.TXDMAEN = state;
}

void SPI_configRXDMAEN(SPI_TypeDef *SPIx,TYPE_FUNCEN state)
{
    SPIx->CON.RXDMAEN = state;
}





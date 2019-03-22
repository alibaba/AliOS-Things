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
#include "SWM320_spi.h"

void SPI_Init(SPI_TypeDef * SPIx, SPI_InitStructure * initStruct)
{
    switch((uint32_t)SPIx)
    {
    case ((uint32_t)SPI0):
        SYS->CLKEN |= (0x01 << SYS_CLKEN_SPI0_Pos);
        break;

    case ((uint32_t)SPI1):
        SYS->CLKEN |= (0x01 << SYS_CLKEN_SPI0_Pos);
        break;
    }

    SPI_Close(SPIx);

    SPIx->CTRL &= ~(SPI_CTRL_FFS_Msk | SPI_CTRL_CPHA_Msk | SPI_CTRL_CPOL_Msk |
                  SPI_CTRL_SIZE_Msk | SPI_CTRL_MSTR_Msk | SPI_CTRL_CLKDIV_Msk | SPI_CTRL_SSN_H_Msk);
    SPIx->CTRL |= (initStruct->FrameFormat   << SPI_CTRL_FFS_Pos) |
                (initStruct->SampleEdge    << SPI_CTRL_CPHA_Pos) |
                (initStruct->IdleLevel     << SPI_CTRL_CPOL_Pos) |
                ((initStruct->WordSize-1)  << SPI_CTRL_SIZE_Pos) |
                (initStruct->Master        << SPI_CTRL_MSTR_Pos) |
                (initStruct->clkDiv        << SPI_CTRL_CLKDIV_Pos) |
                (0                         << SPI_CTRL_SSN_H_Pos);

    SPIx->IF = (0x01 << SPI_IF_RFOVF_Pos);
    SPIx->IE &= ~(SPI_IE_RFHF_Msk | SPI_IE_TFHF_Msk | SPI_IE_FTC_Msk);
    SPIx->IE |= (initStruct->RXHFullIEn << SPI_IE_RFHF_Pos) |
                (initStruct->TXEmptyIEn << SPI_IE_TFHF_Pos) |
                (initStruct->TXCompleteIEn << SPI_IE_FTC_Pos);

    switch((uint32_t)SPIx)
    {
    case ((uint32_t)SPI0):
        if(initStruct->RXHFullIEn | initStruct->TXEmptyIEn | initStruct->TXCompleteIEn)
        {
            NVIC_EnableIRQ(SPI0_IRQn);
        }
        else
        {
            NVIC_DisableIRQ(SPI0_IRQn);
        }
        break;

    case ((uint32_t)SPI1):
        if(initStruct->RXHFullIEn | initStruct->TXEmptyIEn | initStruct->TXCompleteIEn)
        {
            NVIC_EnableIRQ(SPI1_IRQn);
        }
        else
        {
            NVIC_DisableIRQ(SPI1_IRQn);
        }
        break;
    }
}

void SPI_Open(SPI_TypeDef * SPIx)
{
    SPIx->CTRL |= (0x01 << SPI_CTRL_EN_Pos);
}

void SPI_Close(SPI_TypeDef * SPIx)
{
    SPIx->CTRL &= ~SPI_CTRL_EN_Msk;
}

uint32_t SPI_Read(SPI_TypeDef * SPIx)
{
    return SPIx->DATA;
}

void SPI_Write(SPI_TypeDef * SPIx, uint32_t data)
{
    SPIx->DATA = data;
}

void SPI_WriteWithWait(SPI_TypeDef * SPIx, uint32_t data)
{
    SPIx->STAT |= (1 << SPI_STAT_WTC_Pos);

    SPIx->DATA = data;

    while((SPIx->STAT & SPI_STAT_WTC_Msk) == 0);
}

uint32_t SPI_ReadWrite(SPI_TypeDef * SPIx, uint32_t data)
{
    SPIx->DATA = data;
    while(!(SPIx->STAT & SPI_STAT_RFNE_Msk));

    return SPIx->DATA;
}

uint32_t SPI_IsRXEmpty(SPI_TypeDef * SPIx)
{
    return (SPIx->STAT & SPI_STAT_RFNE_Msk) ? 0 : 1;
}

uint32_t SPI_IsTXFull(SPI_TypeDef * SPIx)
{
    return (SPIx->STAT & SPI_STAT_TFNF_Msk) ? 0 : 1;
}

uint32_t SPI_IsTXEmpty(SPI_TypeDef * SPIx)
{
    return (SPIx->STAT & SPI_STAT_TFE_Msk) ? 1 : 0;
}

void SPI_INTRXHalfFullEn(SPI_TypeDef * SPIx)
{
    SPIx->IE |= (0x01 << SPI_IE_RFHF_Pos);
}

void SPI_INTRXHalfFullDis(SPI_TypeDef * SPIx)
{
    SPIx->IE &= ~(0x01 << SPI_IE_RFHF_Pos);
}

uint32_t SPI_INTRXHalfFullStat(SPI_TypeDef * SPIx)
{
    return (SPIx->IF & SPI_IF_RFHF_Msk) ? 1 : 0;
}

void SPI_INTRXFullEn(SPI_TypeDef * SPIx)
{
    SPIx->IE |= (0x01 << SPI_IE_RFF_Pos);
}

void SPI_INTRXFullDis(SPI_TypeDef * SPIx)
{
    SPIx->IE &= ~(0x01 << SPI_IE_RFF_Pos);
}

uint32_t SPI_INTRXFullStat(SPI_TypeDef * SPIx)
{
    return (SPIx->IF & SPI_IF_RFF_Msk) ? 1 : 0;
}

void SPI_INTRXOverflowEn(SPI_TypeDef * SPIx)
{
    SPIx->IE |= (0x01 << SPI_IE_RFOVF_Pos);
}

void SPI_INTRXOverflowDis(SPI_TypeDef * SPIx)
{
    SPIx->IE &= ~(0x01 << SPI_IE_RFOVF_Pos);
}

void SPI_INTRXOverflowClr(SPI_TypeDef * SPIx)
{
    SPIx->IF = (0x01 << SPI_IF_RFOVF_Pos);
}

uint32_t SPI_INTRXOverflowStat(SPI_TypeDef * SPIx)
{
    return (SPIx->IF & SPI_IF_RFOVF_Msk) ? 1 : 0;
}

void SPI_INTTXHalfFullEn(SPI_TypeDef * SPIx)
{
    SPIx->IE |= (0x01 << SPI_IE_TFHF_Pos);
}

void SPI_INTTXHalfFullDis(SPI_TypeDef * SPIx)
{
    SPIx->IE &= ~(0x01 << SPI_IE_TFHF_Pos);
}

uint32_t SPI_INTTXHalfFullStat(SPI_TypeDef * SPIx)
{
    return (SPIx->IF & SPI_IF_TFHF_Msk) ? 1 : 0;
}

void SPI_INTTXEmptyEn(SPI_TypeDef * SPIx)
{
    SPIx->IE |= (0x01 << SPI_IE_TFE_Pos);
}

void SPI_INTTXEmptyDis(SPI_TypeDef * SPIx)
{
    SPIx->IE &= ~(0x01 << SPI_IE_TFE_Pos);
}

uint32_t SPI_INTTXEmptyStat(SPI_TypeDef * SPIx)
{
    return (SPIx->IF & SPI_IF_TFE_Msk) ? 1 : 0;
}

void SPI_INTTXCompleteEn(SPI_TypeDef * SPIx)
{
    SPIx->IE |= (0x01 << SPI_IE_FTC_Pos);
}

void SPI_INTTXCompleteDis(SPI_TypeDef * SPIx)
{
    SPIx->IE &= ~(0x01 << SPI_IE_FTC_Pos);
}

void SPI_INTTXCompleteClr(SPI_TypeDef * SPIx)
{
    SPIx->IF = (1 << SPI_IF_FTC_Pos);
}

uint32_t SPI_INTTXCompleteStat(SPI_TypeDef * SPIx)
{
    return (SPIx->IF & SPI_IF_FTC_Msk) ? 1 : 0;
}

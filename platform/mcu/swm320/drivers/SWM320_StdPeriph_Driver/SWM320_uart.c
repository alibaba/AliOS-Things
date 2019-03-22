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
#include "SWM320_uart.h"

void UART_Init(UART_TypeDef * UARTx, UART_InitStructure * initStruct)
{
    switch((uint32_t)UARTx)
    {
    case ((uint32_t)UART0):
        SYS->CLKEN |= (0x01 << SYS_CLKEN_UART0_Pos);
        break;

    case ((uint32_t)UART1):
        SYS->CLKEN |= (0x01 << SYS_CLKEN_UART1_Pos);
        break;

    case ((uint32_t)UART2):
        SYS->CLKEN |= (0x01 << SYS_CLKEN_UART2_Pos);
        break;

    case ((uint32_t)UART3):
        SYS->CLKEN |= (0x01 << SYS_CLKEN_UART3_Pos);
        break;
    }

    UART_Close(UARTx);

    UARTx->CTRL |= (0x01 << UART_CTRL_BAUDEN_Pos);
    UARTx->BAUD &= ~UART_BAUD_BAUD_Msk;
    UARTx->BAUD |= ((SystemCoreClock/16/initStruct->Baudrate - 1) << UART_BAUD_BAUD_Pos);

    UARTx->CTRL &= ~(UART_CTRL_DATA9b_Msk | UART_CTRL_PARITY_Msk | UART_CTRL_STOP2b_Msk);
    UARTx->CTRL |= (initStruct->DataBits << UART_CTRL_DATA9b_Pos) |
                   (initStruct->Parity   << UART_CTRL_PARITY_Pos) |
                   (initStruct->StopBits << UART_CTRL_STOP2b_Pos);

    UARTx->FIFO &= ~(UART_FIFO_RXTHR_Msk | UART_FIFO_TXTHR_Msk);
    UARTx->FIFO |= (initStruct->RXThreshold << UART_FIFO_RXTHR_Pos) |
                   (initStruct->TXThreshold << UART_FIFO_TXTHR_Pos);

    UARTx->CTRL &= ~UART_CTRL_TOTIME_Msk;
    UARTx->CTRL |= (initStruct->TimeoutTime << UART_CTRL_TOTIME_Pos);

    UARTx->CTRL &= ~(UART_CTRL_RXIE_Msk | UART_CTRL_TXIE_Msk | UART_CTRL_TOIE_Msk);
    UARTx->CTRL |= (initStruct->RXThresholdIEn << UART_CTRL_RXIE_Pos) |
                   (initStruct->TXThresholdIEn << UART_CTRL_TXIE_Pos) |
                   (initStruct->TimeoutIEn << UART_CTRL_TOIE_Pos);

    switch((uint32_t)UARTx)
    {
    case ((uint32_t)UART0):
        if(initStruct->RXThresholdIEn | initStruct->TXThresholdIEn | initStruct->TimeoutIEn)
        {
            NVIC_EnableIRQ(UART0_IRQn);
        }
        else
        {
            NVIC_DisableIRQ(UART0_IRQn);
        }
        break;

    case ((uint32_t)UART1):
        if(initStruct->RXThresholdIEn | initStruct->TXThresholdIEn | initStruct->TimeoutIEn)
        {
            NVIC_EnableIRQ(UART1_IRQn);
        }
        else
        {
            NVIC_DisableIRQ(UART1_IRQn);
        }
        break;

    case ((uint32_t)UART2):
        if(initStruct->RXThresholdIEn | initStruct->TXThresholdIEn | initStruct->TimeoutIEn)
        {
            NVIC_EnableIRQ(UART2_IRQn);
        }
        else
        {
            NVIC_DisableIRQ(UART2_IRQn);
        }
        break;

    case ((uint32_t)UART3):
        if(initStruct->RXThresholdIEn | initStruct->TXThresholdIEn | initStruct->TimeoutIEn)
        {
            NVIC_EnableIRQ(UART3_IRQn);
        }
        else
        {
            NVIC_DisableIRQ(UART3_IRQn);
        }
        break;
    }
}

void UART_Open(UART_TypeDef * UARTx)
{
    UARTx->CTRL |= (0x01 << UART_CTRL_EN_Pos);
}

void UART_Close(UART_TypeDef * UARTx)
{
    UARTx->CTRL &= ~(0x01 << UART_CTRL_EN_Pos);
}

void UART_WriteByte(UART_TypeDef * UARTx, uint8_t data)
{
    UARTx->DATA = data;
}

uint32_t UART_ReadByte(UART_TypeDef * UARTx, uint32_t * data)
{
    uint32_t reg = UARTx->DATA;

    *data = (reg & UART_DATA_DATA_Msk);

    if(reg & UART_DATA_PAERR_Msk) return UART_ERR_PARITY;

    return 0;
}

uint32_t UART_IsTXBusy(UART_TypeDef * UARTx)
{
    return (UARTx->CTRL & UART_CTRL_TXIDLE_Msk) ? 0 : 1;
}

uint32_t UART_IsRXFIFOEmpty(UART_TypeDef * UARTx)
{
    return (UARTx->CTRL & UART_CTRL_RXNE_Msk) ? 0 : 1;
}

uint32_t UART_IsTXFIFOFull(UART_TypeDef * UARTx)
{
    return (UARTx->CTRL & UART_CTRL_TXFF_Msk) ? 1 : 0;
}

void UART_SetBaudrate(UART_TypeDef * UARTx, uint32_t baudrate)
{
    UARTx->BAUD &= ~UART_BAUD_BAUD_Msk;
    UARTx->BAUD |= ((SystemCoreClock/16/baudrate - 1) << UART_BAUD_BAUD_Pos);
}

uint32_t UART_GetBaudrate(UART_TypeDef * UARTx)
{
    return (UARTx->BAUD & UART_BAUD_BAUD_Msk);
}

void UART_CTSConfig(UART_TypeDef * UARTx, uint32_t enable, uint32_t polarity)
{
    UARTx->CTSCR &= ~(UART_CTSCR_EN_Msk | UART_CTSCR_POL_Msk);
    UARTx->CTSCR |= (enable   << UART_CTSCR_EN_Pos) |
                    (polarity << UART_CTSCR_POL_Pos);
}

uint32_t UART_CTSLineState(UART_TypeDef * UARTx)
{
    return (UARTx->CTSCR & UART_CTSCR_STAT_Msk) ? 1 : 0;
}

void UART_RTSConfig(UART_TypeDef * UARTx, uint32_t enable, uint32_t polarity, uint32_t threshold)
{
    UARTx->RTSCR &= ~(UART_RTSCR_EN_Msk | UART_RTSCR_POL_Msk | UART_RTSCR_THR_Msk);
    UARTx->RTSCR |= (enable    << UART_RTSCR_EN_Pos)  |
                    (polarity  << UART_RTSCR_POL_Pos) |
                    (threshold << UART_RTSCR_THR_Pos);
}

uint32_t UART_RTSLineState(UART_TypeDef * UARTx)
{
    return (UARTx->RTSCR & UART_RTSCR_STAT_Msk) ? 1 : 0;
}

void UART_LINConfig(UART_TypeDef * UARTx, uint32_t detectedIEn, uint32_t generatedIEn)
{
    UARTx->LINCR &= ~(UART_LINCR_BRKDETIE_Msk | UART_LINCR_GENBRKIE_Msk);
    UARTx->LINCR |= (detectedIEn  << UART_LINCR_BRKDETIE_Pos) |
                    (generatedIEn << UART_LINCR_GENBRKIE_Pos);
}

void UART_LINGenerate(UART_TypeDef * UARTx)
{
    UARTx->LINCR |= (1 << UART_LINCR_GENBRK_Pos);
}

uint32_t UART_LINIsDetected(UART_TypeDef * UARTx)
{
    return (UARTx->LINCR & UART_LINCR_BRKDETIE_Msk) ? 1 : 0;
}

uint32_t UART_LINIsGenerated(UART_TypeDef * UARTx)
{
    return (UARTx->LINCR & UART_LINCR_GENBRKIF_Msk) ? 1 : 0;
}

void UART_ABRStart(UART_TypeDef * UARTx, uint32_t detectChar)
{
    uint32_t bits;

    if((detectChar == 0xFF) || (detectChar == 0x1FF))      bits = 0;
    else if((detectChar == 0xFE) || (detectChar == 0x1FE)) bits = 1;
    else if((detectChar == 0xF8) || (detectChar == 0x1F8)) bits = 2;
    else if((detectChar == 0x80) || (detectChar == 0x180)) bits = 3;
    else while(1);

    UARTx->BAUD &= ~(UART_BAUD_ABREN_Msk | UART_BAUD_ABRBIT_Msk);
    UARTx->BAUD |= (1    << UART_BAUD_ABREN_Pos) |
                   (bits << UART_BAUD_ABRBIT_Pos);
}

uint32_t UART_ABRIsDone(UART_TypeDef * UARTx)
{
    if(UARTx->BAUD & UART_BAUD_ABREN_Msk)
    {
        return 0;
    }
    else if(UARTx->BAUD & UART_BAUD_ABRERR_Msk)
    {
        return UART_ABR_RES_ERR;
    }
    else
    {
        return UART_ABR_RES_OK;
    }
}

void UART_INTRXThresholdEn(UART_TypeDef * UARTx)
{
    UARTx->CTRL |= (0x01 << UART_CTRL_RXIE_Pos);
}

void UART_INTRXThresholdDis(UART_TypeDef * UARTx)
{
    UARTx->CTRL &= ~(0x01 << UART_CTRL_RXIE_Pos);
}

uint32_t UART_INTRXThresholdStat(UART_TypeDef * UARTx)
{
    return (UARTx->BAUD & UART_BAUD_RXIF_Msk) ? 1 : 0;
}

void UART_INTTXThresholdEn(UART_TypeDef * UARTx)
{
    UARTx->CTRL |= (0x01 << UART_CTRL_TXIE_Pos);
}

void UART_INTTXThresholdDis(UART_TypeDef * UARTx)
{
    UARTx->CTRL &= ~(0x01 << UART_CTRL_TXIE_Pos);
}

uint32_t UART_INTTXThresholdStat(UART_TypeDef * UARTx)
{
    return (UARTx->BAUD & UART_BAUD_TXIF_Msk) ? 1 : 0;
}

void UART_INTTimeoutEn(UART_TypeDef * UARTx)
{
    UARTx->CTRL |= (0x01 << UART_CTRL_TOIE_Pos);
}

void UART_INTTimeoutDis(UART_TypeDef * UARTx)
{
    UARTx->CTRL &= ~(0x01 << UART_CTRL_TOIE_Pos);
}

uint32_t UART_INTTimeoutStat(UART_TypeDef * UARTx)
{
    return (UARTx->BAUD & UART_BAUD_TOIF_Msk) ? 1 : 0;
}

void UART_INTTXDoneEn(UART_TypeDef * UARTx)
{
    UARTx->CTRL |= (0x01 << UART_CTRL_TXDOIE_Pos);
}

void UART_INTTXDoneDis(UART_TypeDef * UARTx)
{
    UARTx->CTRL &= ~(0x01 << UART_CTRL_TXDOIE_Pos);
}

uint32_t UART_INTTXDoneStat(UART_TypeDef * UARTx)
{
    return (UARTx->BAUD & UART_BAUD_TXDOIF_Msk) ? 1 : 0;
}

/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/
#include "lib_uart.h"
#include "lib_scu.h"
#include "system_P131.h"

void UART_Init(UART_TypeDef *UARTx, UART_InitStruType *UART_InitStruct)
{
    float tmp = 0;
    uint32_t baud;
    SCU_ClocksTypeDef SCU_ClocksStatus;
    
    UARTx->CON0.TRST = 1;
    UARTx->CON0.RRST = 1;

    UARTx->CON0.RBCLR = 1;
    UARTx->CON0.TBCLR = 1;
    SCU_GetClocksFreq(&SCU_ClocksStatus);
    
    if(UART_InitStruct->UART_ClockSet == UART_Clock_1) 
    {
        tmp  = (float)SCU_ClocksStatus.APB1_Frequency / (float)(16 * UART_InitStruct->UART_BaudRate);
    }
    else if(UART_InitStruct->UART_ClockSet == UART_Clock_2)
    {
        tmp  = (float)SCU_ClocksStatus.APB1_Frequency / (float)(32 * UART_InitStruct->UART_BaudRate);
    }    
    else if(UART_InitStruct->UART_ClockSet == UART_Clock_3)
    {
        tmp  = (float)SCU_ClocksStatus.APB1_Frequency / (float)(64 * UART_InitStruct->UART_BaudRate);
    }    
    else if (UART_InitStruct->UART_ClockSet == UART_Clock_4)
    {
        tmp  = (float)SCU_ClocksStatus.APB1_Frequency / (float)(128 * UART_InitStruct->UART_BaudRate);
    } 
    
    tmp = tmp < 1.0 ? 1.0 : tmp;    
    baud = (uint32_t)(tmp * 16);

    UARTx->CON1.BCS   = UART_InitStruct->UART_ClockSet;
    UARTx->BRR.Word   = baud;
    UARTx->CON0.TXFS  = UART_InitStruct->UART_StopBits;
    UARTx->CON0.TXMOD = UART_InitStruct->UART_TxMode;
    UARTx->CON0.TXP   = UART_InitStruct->UART_TxPolar;
    UARTx->CON0.RXMOD = UART_InitStruct->UART_RxMode;
    UARTx->CON0.RXP   = UART_InitStruct->UART_RxPolar;

    return;    
}

void UART_ITConfig(UART_TypeDef *UARTx, UART_TYPE_IT UART_IT, TYPE_FUNCEN NewState)
{
    if (NewState == ENABLE)
        UARTx->IE.Word |= (uint32_t)UART_IT;
    else
        UARTx->IE.Word &= ~(uint32_t)UART_IT;

    return;
}

void UART_TBIMConfig(UART_TypeDef *UARTx, UART_TYPE_TRBIM Type)
{
    UARTx->CON1.TBIM = Type;
    return;
}

void UART_RBIMConfig(UART_TypeDef *UARTx, UART_TYPE_TRBIM Type)
{
    UARTx->CON1.RBIM = Type;
    return;
}

void UART_SendByte(UART_TypeDef *UARTx, uint8_t data08)
{
    UARTx->TBW.Byte[0] = data08;
    return;
}

void UART_SendHalfWord(UART_TypeDef *UARTx, uint16_t data16)
{
    UARTx->TBW.HalfWord[0] = data16;
    return;
}

void UART_SendWord(UART_TypeDef *UARTx, uint32_t data32)
{
    UARTx->TBW.Word = data32;
    return;
}

uint8_t UART_RecByte(UART_TypeDef *UARTx)
{
    return UARTx->RBR.Byte[0];
}

uint16_t UART_RecHalfWord(UART_TypeDef *UARTx)
{
    return UARTx->RBR.HalfWord[0];
}

uint32_t UART_RecWord(UART_TypeDef *UARTx)
{
    return UARTx->RBR.Word;
}

FlagStatus UART_GetStatus(UART_TypeDef *UARTx, UART_TYPE_STA UART_Flag)
{
    FlagStatus bitstatus = RESET;

    if (UARTx->STA.Word & UART_Flag)
        bitstatus = SET;

    return bitstatus;
}

FlagStatus UART_GetFlagStatus(UART_TypeDef *UARTx, UART_TYPE_FLAG UART_Flag)
{
    FlagStatus bitstatus = RESET;

    if (UARTx->IF.Word & UART_Flag)
        bitstatus = SET;

    return bitstatus;
}

ITStatus UART_GetITStatus(UART_TypeDef *UARTx, UART_TYPE_IT UART_Flag)
{
    ITStatus bitstatus = RESET;

    if (UARTx->IE.Word & UART_Flag)
        bitstatus = SET;

    return bitstatus;
}

void UART_ClearITPendingBit(UART_TypeDef *UARTx, UART_CLR_IF UART_Flag)
{
    UARTx->IF.Word |= (UART_Flag);
    return;
}

void UART_ConfigDMAONERR(UART_TypeDef *UARTx,TYPE_FUNCEN state)
{
    UARTx->DMACR.DMAONERR = state;
}

void UART_ConfigRXDMAEN(UART_TypeDef *UARTx,TYPE_FUNCEN state)
{
    UARTx->DMACR.RXDMAEN = state;
}

void UART_ConfigTXDMAEN(UART_TypeDef *UARTx,TYPE_FUNCEN state)
{
    UARTx->DMACR.TXDMAEN = state;
}


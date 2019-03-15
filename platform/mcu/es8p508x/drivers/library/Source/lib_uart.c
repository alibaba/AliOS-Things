/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/
#include "lib_uart.h"

void UART_Init(UART_TypeDef* UARTx,UART_InitStruType* UART_InitStruct)
{
    uint32_t BaudClockTemp = 0, BaudTemp = 0;

    if(UART_InitStruct->UART_ClockSet == UART_Clock_1)
    {
        BaudClockTemp = (SystemCoreClock/16)%UART_InitStruct->UART_BaudRate;
        if(UART_InitStruct->UART_BaudRate >= (2*BaudClockTemp))
        {
            BaudTemp = (SystemCoreClock/16/UART_InitStruct->UART_BaudRate)-1;
        }
        else
        {
            BaudTemp = SystemCoreClock/16/UART_InitStruct->UART_BaudRate;
        }
    }
    else if(UART_InitStruct->UART_ClockSet == UART_Clock_2)
    {
        BaudClockTemp = (SystemCoreClock/32)%UART_InitStruct->UART_BaudRate;
        if(UART_InitStruct->UART_BaudRate >= (2*BaudClockTemp))
        {
            BaudTemp = (SystemCoreClock/32/UART_InitStruct->UART_BaudRate)-1;
        }
        else
        {
            BaudTemp = SystemCoreClock/32/UART_InitStruct->UART_BaudRate;
        }
    }
    else if(UART_InitStruct->UART_ClockSet == UART_Clock_4)
    {
        BaudClockTemp = (SystemCoreClock/64)%UART_InitStruct->UART_BaudRate;
        if(UART_InitStruct->UART_BaudRate >= (2*BaudClockTemp))
        {
            BaudTemp = (SystemCoreClock/64/UART_InitStruct->UART_BaudRate)-1;
        }
        else
        {
            BaudTemp = SystemCoreClock/64/UART_InitStruct->UART_BaudRate;
        }
    }

    if(BaudTemp > 2046)
    {
        BaudTemp = 2047;
    }

    UARTx->CON.BCS = UART_InitStruct->UART_ClockSet;
    UARTx->BRR.BRR = BaudTemp ;

    UARTx->CON.TXFS = UART_InitStruct->UART_StopBits;
    UARTx->CON.TXMOD = UART_InitStruct->UART_TxMode;
    UARTx->CON.TXP = UART_InitStruct->UART_TxPolar;
    UARTx->CON.RXMOD = UART_InitStruct->UART_RxMode;
    UARTx->CON.RXP = UART_InitStruct->UART_RxPolar;
}

void UART_ITConfig(UART_TypeDef* UARTx,UART_TYPE_IT UART_IT,TYPE_FUNCEN NewState)
{
    if (NewState != DISABLE)
        UARTx->IE.Word = (UARTx->IE.Word & (uint32_t)0x0000FFFF) | (uint32_t) UART_IT;
    else
        UARTx->IE.Word = (UARTx->IE.Word & (uint32_t)0x0000FFFF) & (~((uint32_t) UART_IT));
}

void UART_TBIMConfig(UART_TypeDef* UARTx,UART_TYPE_TRBIM Type)
{
    UARTx->IE.TBIM = Type;
}

void UART_RBIMConfig(UART_TypeDef* UARTx,UART_TYPE_TRBIM Type)
{
    UARTx->IE.RBIM = Type;
}

void UART_SendByte(UART_TypeDef* UARTx,uint8_t Temp08)
{
    UARTx->TBW.Byte[0] = Temp08;
}
void UART_SendHalfWord(UART_TypeDef* UARTx,uint16_t Temp16)
{
    UARTx->TBW.HalfWord[0] = Temp16;
}
void UART_SendWord(UART_TypeDef* UARTx,uint32_t Temp32)
{
    UARTx->TBW.Word = Temp32;
}

uint8_t UART_RecByte(UART_TypeDef* UARTx)
{
    return UARTx->RBR.Byte[0];
}
uint16_t UART_RecHalfWord(UART_TypeDef* UARTx)
{
    return UARTx->RBR.HalfWord[0];
}
uint32_t UART_RecWord(UART_TypeDef* UARTx)
{
    return UARTx->RBR.Word;
}

FlagStatus UART_GetFlagStatus(UART_TypeDef* UARTx,UART_TYPE_FLAG UART_Flag)
{
    FlagStatus bitstatus = RESET;

    if ((UARTx->IF.Word & ((uint32_t)UART_Flag)) != RESET)
        bitstatus = SET;
    else
        bitstatus = RESET;

    return bitstatus;
}

ITStatus UART_GetITStatus(UART_TypeDef* UARTx,UART_TYPE_IT UART_IT)
{
    FlagStatus bitstatus = RESET;

    if((UARTx->IE.Word & ((uint32_t)UART_IT)) == 0)
        bitstatus = RESET;
    else
        bitstatus = SET;

    return bitstatus;
}

void UART_ClearIFPendingBit(UART_TypeDef* UARTx,UART_CLR_IF UART_Flag)
{
    UARTx->IF.Word = (uint32_t)UART_Flag;
}

/*************************END OF FILE**********************/

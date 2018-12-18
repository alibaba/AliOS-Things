/***************************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/

#include "lib_euart.h"
#include "system_HR8P2xx.h"

void EUART_ModeConfig(EUART_TypeDef* EUARTx,EUART_TYPE_MODE Mode)
{
    EUARTx->MOD.MOD  =  Mode;
}

void EUART_Init(EUART_TypeDef* EUARTx,EUART_InitStruType* EUART_InitStruct)
{
    uint32_t BaudTemp = 0;
    if(EUART_InitStruct->EUART_ClockSet == EUART_Clock_1)
    {
        BaudTemp = (SystemCoreClock/16/EUART_InitStruct->EUART_BaudRate)-1;
    }
    else if(EUART_InitStruct->EUART_ClockSet == EUART_Clock_2)
    {
        BaudTemp = (SystemCoreClock/32/EUART_InitStruct->EUART_BaudRate)-1;
    }
    else if(EUART_InitStruct->EUART_ClockSet == EUART_Clock_3)
    {
        BaudTemp = (SystemCoreClock/64/EUART_InitStruct->EUART_BaudRate)-1;
    }

    if(BaudTemp > 2046)
    {
        BaudTemp = 2047;
    }

    EUARTx->CON.BCS   = EUART_InitStruct->EUART_ClockSet;
    EUARTx->BRR.BRR   = BaudTemp;
    EUARTx->CON.TXFS  = EUART_InitStruct->EUART_StopBits;
    EUARTx->CON.TXMOD = EUART_InitStruct->EUART_TxMode;
    EUARTx->CON.TXP   = EUART_InitStruct->EUART_TxPolar;
    EUARTx->CON.RXMOD = EUART_InitStruct->EUART_RxMode;
    EUARTx->CON.RXP   = EUART_InitStruct->EUART_RxPolar;
}

void EUART_BaudConfig(EUART_TypeDef* EUARTx, uint32_t BaudRate, EUART_TYPE_BCS ClockSet)
{
    uint32_t BaudTemp = 0;
    if(ClockSet == EUART_Clock_1)
    {
        BaudTemp = (SystemCoreClock/16/BaudRate)-1;
    }
    else if(ClockSet == EUART_Clock_2)
    {
        BaudTemp = (SystemCoreClock/32/BaudRate)-1;
    }
    else if(ClockSet == EUART_Clock_3)
    {
        BaudTemp = (SystemCoreClock/64/BaudRate)-1;
    }

    if(BaudTemp > 2046)
    {
        BaudTemp = 2047;
    }

    EUARTx->CON.BCS = ClockSet;
    EUARTx->BRR.BRR = BaudTemp;    
}

void EUART_ITConfig(EUART_TypeDef* EUARTx, EUART_TYPE_IT EUART_IT, TYPE_FUNCEN NewState)
{
    if (NewState != DISABLE)
    {
        EUARTx->IE.Word |=  (uint32_t) EUART_IT;
    }
    else
    {
        EUARTx->IE.Word &=  ~((uint32_t) EUART_IT);
    }
}

void EUART_TBIMConfig(EUART_TypeDef* EUARTx,EUART_TYPE_TRBIM Type)
{
    EUARTx->IE.TBIM = Type;
}

void EUART_RBIMConfig(EUART_TypeDef* EUARTx,EUART_TYPE_TRBIM Type)
{
    EUARTx->IE.RBIM = Type;
}

FlagStatus EUART_GetFlagStatus(EUART_TypeDef* EUARTx,EUART_TYPE_FLAG EUART_Flag)
{
    FlagStatus bitstatus = RESET;

    if ((EUARTx->IF.Word & ((uint32_t)EUART_Flag )) != RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return  bitstatus;
}

ITStatus EUART_GetITStatus(EUART_TypeDef* EUARTx,EUART_TYPE_IT EUART_Flag)
{
    FlagStatus bitstatus = RESET;
    uint32_t itstatus = 0x0, itenable = 0x0;

    itenable = EUARTx->IE.Word & (uint32_t)EUART_Flag;
    itstatus = EUARTx->IF.Word & (uint32_t)EUART_Flag;

    if ((itstatus != (uint32_t)RESET) && (itenable != (uint32_t)RESET))
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return  bitstatus; 
}

void EUART_ClearITPendingBit(EUART_TypeDef* EUARTx,EUART_CLR_IF EUART_Flag)
{
    EUARTx->IF.Word = (uint32_t)EUART_Flag;
}

void  U7816_Init(EUART_TypeDef* EUARTx, U7816_InitStruType* U7816_InitStruct)
{
    EUARTx->MOD.CK0E =  U7816_InitStruct->U7816_ECK0;

    EUARTx->MOD.CK1E =  U7816_InitStruct->U7816_ECK1;

    EUARTx->MOD.CHS  =  U7816_InitStruct->U7816_EIOCh;

    EUARTx->MOD.IOC  =  U7816_InitStruct->U7816_EIODir;

    EUARTx->MOD.DAS  =  U7816_InitStruct->U7816_DataForm;

    EUARTx->MOD.PS   =  U7816_InitStruct->U7816_DataVerify;

    EUARTx->MOD.ARTE =  U7816_InitStruct->U7816_AutoRetryTX;

    EUARTx->MOD.ARRE =  U7816_InitStruct->U7816_AutoRetryRX;

    EUARTx->MOD.TNAS =  U7816_InitStruct->U7816_NACK_Width;

    EUARTx->MOD.ARTS =  U7816_InitStruct->U7816_RetryTimes;

    EUARTx->MOD.CKS  =  U7816_InitStruct->U7816_CLKS;

    EUART_BaudConfig(EUARTx,U7816_InitStruct->U7816_BaudRate,U7816_InitStruct->U7816_ClockSet);
 
    if(U7816_InitStruct->U7816_ETUTime >255)
    {
        U7816_InitStruct->U7816_ETUTime = 255;
    }
    EUARTx->MOD.ETUS  =  U7816_InitStruct->U7816_ETUTime;

}

void  EUART_EIOChConfig(EUART_TypeDef* EUARTx, U7816_TYPE_CHS U7816_IO)
{
    EUARTx->MOD.CHS  =  U7816_IO;
}

void EUART_EIODirection(EUART_TypeDef* EUARTx, U7816_TYPE_EIOC Dir)
{
    EUARTx->MOD.IOC  =  Dir;
}

void EUART_SendByte(EUART_TypeDef* EUARTx, uint8_t ByteData)
{
    EUARTx->TBW.Byte[0] = ByteData;
}

void EUART_SendHalfWord(EUART_TypeDef* EUARTx, uint16_t HalfWordData)
{
    EUARTx->TBW.HalfWord[0] = HalfWordData;
}

void EUART_SendWord(EUART_TypeDef* EUARTx, uint32_t WordData)
{
    EUARTx->TBW.Word = WordData;
}

uint8_t EUART_RecByte(EUART_TypeDef* EUARTx)
{
    return(EUARTx->RBR.Byte[0]);
}

uint16_t EUART_RecHalfWord(EUART_TypeDef* EUARTx)
{
    return(EUARTx->RBR.HalfWord[0]);
}

uint32_t EUART_RecWord(EUART_TypeDef* EUARTx)
{
    return(EUARTx->RBR.Word);
}

/*************************END OF FILE**********************/

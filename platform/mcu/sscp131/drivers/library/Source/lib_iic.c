/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/
#include "lib_iic.h"
#include "lib_scu.h"

void IIC_Init(I2C_TypeDef *IICx, IIC_InitStruType *IIC_InitStruct)
{
    uint32_t tmp;
    SCU_ClocksTypeDef SCU_ClocksStatus;
    
    SCU_GetClocksFreq(&SCU_ClocksStatus);

    if (IIC_InitStruct->IIC_Mode == IIC_Mode_Master) {
        if (IIC_InitStruct->IIC_16XSamp !=DISABLE)
            tmp = SCU_ClocksStatus.APB1_Frequency / 24 /IIC_InitStruct->IIC_Clk - 1;
        else
            tmp = SCU_ClocksStatus.APB1_Frequency / 16 /IIC_InitStruct->IIC_Clk - 1;

        tmp = tmp > 255 ? 255 : tmp;
        IICx->CON.TJP = tmp;
        IICx->CON.TJE = 0x1;
    }

    IICx->CON.SDASE = IIC_InitStruct->IIC_16XSamp;
    IICx->CON.SCLSE = IIC_InitStruct->IIC_16XSamp;
    IICx->CON.SCLOD = IIC_InitStruct->IIC_SckOd;
    IICx->CON.SDAOD = IIC_InitStruct->IIC_SdaOd;
    IICx->MOD.MS   = IIC_InitStruct->IIC_Mode;
    IICx->MOD.SPAE = IIC_InitStruct->IIC_AutoStop;
    IICx->MOD.SRAE = IIC_InitStruct->IIC_AutoCall;

    return;
}

void IIC_ITConfig(I2C_TypeDef *IICx, IIC_TYPE_IT IIC_IT, TYPE_FUNCEN NewState)
{
    if (NewState == ENABLE)
        IICx->IE.Word |= (uint32_t)IIC_IT;
    else
        IICx->IE.Word &= ~((uint32_t)IIC_IT);

    return;
}

void IIC_SendAddress(I2C_TypeDef *IICx, uint8_t IIC_Address, IIC_TYPE_RWMODE Mode)
{
    IICx->CON.SA = IIC_Address >> 1;
    IICx->CON.RW = Mode;

    return;
}

void IIC_SetAddress(I2C_TypeDef *IICx, uint8_t IIC_Address)
{
    IICx->CON.SA = IIC_Address >> 1;
    return;
}

void IIC_RecModeConfig(I2C_TypeDef *IICx, IIC_TYPE_RECMODE RecType)
{
    IICx->MOD.RDM = RecType;
    return;
}

void IIC_TBIMConfig(I2C_TypeDef *IICx, IIC_TYPE_TRBIM Type)
{
    IICx->IE.TBIM = Type;
    return;
}

void IIC_RBIMConfig(I2C_TypeDef *IICx, IIC_TYPE_TRBIM Type)
{
    IICx->IE.RBIM = Type;
    return;
}

void IIC_AckDelay(I2C_TypeDef *IICx, IIC_TYPE_ADLY Type, TYPE_FUNCEN NewStatus)
{
    IICx->MOD.ADLY = Type;
    IICx->MOD.ADE  = NewStatus;
    return;
}

void IIC_TISConfig(I2C_TypeDef *IICx, IIC_TYPE_TIS Time)
{
    IICx->MOD.TIS = Time;
    return;
}

void IIC_SendByte(I2C_TypeDef *IICx, uint8_t Byte)
{
    IICx->TBW.Byte[0] = Byte;
    return;
}

void IIC_SendHalfWord(I2C_TypeDef *IICx, uint16_t HalfWord)
{
    IICx->TBW.HalfWord[0] = HalfWord;
    return;
}

void IIC_SendWord(I2C_TypeDef *IICx, uint32_t Word)
{
    IICx->TBW.Word = Word;
    return;
}

uint8_t IIC_RecByte(I2C_TypeDef *IICx)
{
    return IICx->RBR.Byte[0];
}

uint16_t IIC_RecHalfWord(I2C_TypeDef *IICx)
{
    return IICx->RBR.HalfWord[0];
}

uint32_t IIC_RecWord(I2C_TypeDef *IICx)
{
    return IICx->RBR.Word;
}

IIC_TYPE_RWMODE IIC_GetRWMode(I2C_TypeDef *IICx)
{
    IIC_TYPE_RWMODE status;

    status = IICx->CON.RW == 0 ? IIC_Mode_Write : IIC_Mode_Read;
    return status;
}

FlagStatus IIC_GetTBStatus(I2C_TypeDef *IICx)
{
    FlagStatus status;

    if ((IICx->STA.Word & (uint32_t)0xf00) != (uint32_t)0xf00)
        status = RESET;
    else
        status = SET;

    return status;
}

FlagStatus IIC_GetFlagStatus(I2C_TypeDef *IICx, IIC_TYPE_IF IIC_Flag)
{
    FlagStatus status;

    if ((IICx->IF.Word & (uint32_t)IIC_Flag) != RESET)
        status = SET;
    else
        status = RESET;

    return status;
}

FlagStatus IIC_GetITStatus(I2C_TypeDef *IICx, IIC_TYPE_IT IIC_Flag)
{
    FlagStatus status = RESET;

    if ((IICx->IE.Word & (uint32_t)IIC_Flag) != RESET)
        status = SET;

    return status;
}

void IIC_ClearITPendingBit(I2C_TypeDef *IICx, IIC_CLR_IF IIC_IT)
{
    IICx->IF.Word |= (uint32_t)IIC_IT;
    return;
}

void IIC_configRXDMAEN(I2C_TypeDef *IICx, TYPE_FUNCEN state)
{
    IICx->CON.RXDMAEN = state;
}

void IIC_configTXDMAEN(I2C_TypeDef *IICx, TYPE_FUNCEN state)
{
    IICx->CON.TXDMAEN = state;
}

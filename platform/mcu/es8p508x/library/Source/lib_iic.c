/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/
#include "lib_iic.h"

void I2C_Init(I2C_InitStruType* I2C_InitStruct)
{
    uint32_t temp;

    if(I2C_InitStruct->I2C_Mode == I2C_Mode_Master)
    {
        if(I2C_InitStruct->I2C_16XSamp != DISABLE)
            temp = SystemCoreClock/24/I2C_InitStruct->I2C_Clk - 1;
        else
            temp = SystemCoreClock/16/I2C_InitStruct->I2C_Clk - 1;

        if(temp > 254)
            temp = 255;

        I2C0->CON.TJP = temp;      

        I2C0->CON.TJE = 0x1;       
    }

    I2C0->CON.SDASE = I2C_InitStruct->I2C_16XSamp;  
    I2C0->CON.SCKSE = I2C_InitStruct->I2C_16XSamp; 
    I2C0->CON.SCKOD = I2C_InitStruct->I2C_SclOd;    
    I2C0->CON.SDAOD = I2C_InitStruct->I2C_SdaOd;
    I2C0->MOD.MS = I2C_InitStruct->I2C_Mode;       
    I2C0->MOD.SPAE = I2C_InitStruct->I2C_AutoStop; 
    I2C0->MOD.SRAE = I2C_InitStruct->I2C_AutoCall; 
}

void I2C_ITConfig(I2C_TYPE_IT I2C_IT,TYPE_FUNCEN NewState)
{
    if (NewState != DISABLE)
        I2C0->IE.Word |= (uint32_t)I2C_IT;
    else
        I2C0->IE.Word &= ~((uint32_t)I2C_IT);
}

void I2C_SendAddress(uint8_t I2C_Address,I2C_TYPE_RWMODE Mode)
{
    I2C0->CON.SA = I2C_Address>>1;
    I2C0->CON.RW = Mode;
}


void I2C_SetAddress(uint8_t I2C_Address)
{
    I2C0->CON.SA = I2C_Address>>1;
}

void I2C_RecModeConfig(I2C_TYPE_RECMODE RecType)
{
    I2C0->MOD.RDM = RecType;
}

void I2C_TBIMConfig(I2C_TYPE_TRBIM Type)
{
    I2C0->IE.TBIM = Type;
}

void I2C_RBIMConfig(I2C_TYPE_TRBIM Type)
{
    I2C0->IE.RBIM = Type;
}

void I2C_AckDelay(I2C_TYPE_ADLY Type,TYPE_FUNCEN NewStatus)
{
    I2C0->MOD.ADLY = Type;
    I2C0->MOD.ADE = NewStatus;
}

void I2C_TISConfig(I2C_TYPE_TIS Time)
{
    I2C0->MOD.TIS = Time;
}

void I2C_SendByte(uint8_t Byte)
{
    I2C0->TBW.Byte[0] = Byte;
}

void I2C_SendHalfWord(uint16_t HalfWord)
{
    I2C0->TBW.HalfWord[0] = HalfWord;
}

void I2C_SendWord(uint32_t Word)
{
    I2C0->TBW.Word = Word;
}

uint8_t I2C_RecByte(void)
{
    return I2C0->RBR.Byte[0] ;
}

uint16_t I2C_RecHalfWord(void)
{
    return I2C0->RBR.HalfWord[0];
}

uint32_t I2C_RecWord(void)
{
    return I2C0->RBR.Word ;
}

I2C_TYPE_RWMODE I2C_GetRWMode(void)
{
    I2C_TYPE_RWMODE Status = I2C_Mode_Write;

    if(I2C0->CON.RW != 0)
        Status = I2C_Mode_Read;
    else
        Status = I2C_Mode_Write;

    return Status;
}

FlagStatus I2C_GetTBStatus(void)
{
    FlagStatus bitstatus = RESET;

    if((I2C0->STA.Word&(uint32_t)0x0f00) != (uint32_t)0x0f00)
        bitstatus = RESET;
    else
        bitstatus = SET;

    return  bitstatus;
}

FlagStatus I2C_GetFlagStatus(I2C_TYPE_FLAG I2C_Flag)
{
    FlagStatus bitstatus = RESET;

    if ((I2C0->IF.Word & (uint32_t)I2C_Flag) != RESET)
        bitstatus = SET;
    else
        bitstatus = RESET;

    return  bitstatus;
}

FlagStatus I2C_GetITStatus( I2C_TYPE_IT I2C_Flag)
{
    FlagStatus status = RESET;

    if ((I2C0->IE.Word & (uint32_t)I2C_Flag) == RESET)
        return status;

    if ((I2C0->IF.Word & (uint32_t)I2C_Flag) != RESET)
        status = SET;

    return status;
}

void I2C_ClearITPendingBit(I2C_CLR_IF I2C_IT)
{
    I2C0->IF.Word = (uint32_t)I2C_IT;
}

/*************************END OF FILE**********************/


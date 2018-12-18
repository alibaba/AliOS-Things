/***************************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/

#include "lib_iiC.h"
#include "System_HR8P2xx.h"

void I2C_Init(I2C_InitStruType* I2C_InitStruct)
{
  uint32_t temp;
  if(I2C_InitStruct->I2C_Mode == I2C_Mode_Master)
  {
    if(I2C_InitStruct->I2C_16XSamp != DISABLE)
    {
      temp = SystemCoreClock/24/I2C_InitStruct->I2C_Clk - 1;
    }
    else
    {
      temp = SystemCoreClock/16/I2C_InitStruct->I2C_Clk - 1;
    }
    if(temp > 254) temp = 255;

    I2C->CON.TJP = temp;

    I2C->CON.TJE = 0x1;
  }

  I2C->CON.SDASE = I2C_InitStruct->I2C_16XSamp;
  I2C->CON.SCLSE = I2C_InitStruct->I2C_16XSamp;
  I2C->CON.SCLOD = I2C_InitStruct->I2C_SclOd;
  I2C->CON.SDAOD = I2C_InitStruct->I2C_SdaOd;
  I2C->MOD.MS = I2C_InitStruct->I2C_Mode;
  I2C->MOD.SPAE = I2C_InitStruct->I2C_AutoStop;
  I2C->MOD.SRAE = I2C_InitStruct->I2C_AutoCall;

}

void I2C_ITConfig(I2C_TYPE_IT I2C_IT,TYPE_FUNCEN NewState)
{
  if (NewState != DISABLE)
  {
    I2C->IE.Word |= (uint32_t)I2C_IT;
  }
  else
  {
    I2C->IE.Word &= ~((uint32_t)I2C_IT);
  }
}

void I2C_SendAddress(uint8_t I2C_Address,I2C_TYPE_RWMODE Mode)
{
  I2C->CON.SA = I2C_Address>>1;
  I2C->CON.RW = Mode;
}

void I2C_SetAddress(uint8_t I2C_Address)
{
  I2C->CON.SA = I2C_Address>>1;
}

void I2C_RecModeConfig(I2C_TYPE_RECMODE RecType)
{
  I2C->MOD.RDM = RecType;
}

void I2C_TBIMConfig(I2C_TYPE_TRBIM Type)
{
  I2C->IE.TBIM = Type;
}

void I2C_RBIMConfig(I2C_TYPE_TRBIM Type)
{
  I2C->IE.RBIM = Type;
}

void I2C_AckDelay(I2C_TYPE_ADLY Type,TYPE_FUNCEN NewStatus)
{
  I2C->MOD.ADLY = Type;
  I2C->MOD.ADE = NewStatus;
}

void I2C_TISConfig(I2C_TYPE_TIS Time)
{
  I2C->MOD.TIS = Time;
}

void I2C_SendByte(uint8_t Byte)
{
  I2C->TBW.Byte[0] = Byte;
}

void I2C_SendHalfWord(uint16_t HalfWord)
{
  I2C->TBW.HalfWord[0] = HalfWord;
}

void I2C_SendWord(uint32_t Word)
{
  I2C->TBW.Word = Word;
}

uint8_t I2C_RecByte(void)
{
  return I2C->RBR.Byte[0] ;
}

uint16_t I2C_RecHalfWord(void)
{
  return I2C->RBR.HalfWord[0];
}

uint32_t I2C_RecWord(void)
{
  return I2C->RBR.Word ;
}

I2C_TYPE_RWMODE I2C_GetRWMode(void)
{
  return (I2C->CON.RW==0 ? I2C_Mode_Write : I2C_Mode_Read);
}

FlagStatus I2C_GetTBStatus(void)
{
  FlagStatus bitstatus = RESET;
  if((I2C->STA.Word&(uint32_t)0x0f00) != (uint32_t)0x0f00)
  {
    bitstatus = RESET;
  }
  else
  {
    bitstatus = SET;
  }
  return  bitstatus;
}

FlagStatus I2C_GetFlagStatus(I2C_TYPE_FLAG I2C_Flag)
{
  return ( (I2C->IF.Word & (uint32_t)I2C_Flag)? SET : RESET );
}

FlagStatus I2C_GetITStatus( I2C_TYPE_IT I2C_Flag)
{
  FlagStatus status = RESET;

  if ((I2C->IE.Word & (uint32_t)I2C_Flag) == RESET)
    return status;
    
    if(I2C_Flag == 0x01000)
    {
        if ((I2C->IF.Word & 0x00000800) != RESET)
        {
            status = SET;
            return status;
        }
    }
    
  if ((I2C->IF.Word & (uint32_t)I2C_Flag) != RESET)
    status = SET;

  return status;
}

void I2C_ClearITPendingBit(I2C_CLR_IF I2C_IT)
{
  I2C->IF.Word = (uint32_t)I2C_IT;

}

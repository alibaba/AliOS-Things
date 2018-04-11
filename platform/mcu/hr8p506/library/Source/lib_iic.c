/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_iic.c
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  IIC模块库函数
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#include "lib_iic.h"
#include "system_hr8p506.h"

/*************************************
  函数名：IIC_Init
  描  述：IIC初始化
  输入值：I2C0/I2C1
  初始化结构体
  返回值：无
 **************************************/
void IIC_Init(I2C_TypeDef *IICx, IIC_InitStruType *IIC_InitStruct)
{
	uint32_t tmp;

	if (IIC_InitStruct->IIC_Mode == IIC_Mode_Master) {
		if (IIC_InitStruct->IIC_16XSamp != Disable)
			tmp = SystemCoreClock / 24 /IIC_InitStruct->IIC_Clk - 1;
		else
			tmp = SystemCoreClock / 16 /IIC_InitStruct->IIC_Clk - 1;

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

/*************************************
  函数名：IIC_ITConfig
  描  述：中断使能
  输入值：I2C0/I2C1
  中断类型
  使能/禁止
  返回值：无
 **************************************/
void IIC_ITConfig(I2C_TypeDef *IICx, IIC_TYPE_IT IIC_IT, TYPE_FUNCEN NewState)
{
	if (NewState == Enable)
		IICx->IE.Word |= (uint32_t)IIC_IT;
	else
		IICx->IE.Word &= ~((uint32_t)IIC_IT);

	return;
}

/*************************************
  函数名：IIC_SendAddress
  描  述：发送从机地址
  输入值：I2C0/I2C1
  从机地址
  操作模式:读/写
  返回值：无
 **************************************/
void IIC_SendAddress(I2C_TypeDef *IICx, uint8_t IIC_Address, IIC_TYPE_RWMODE Mode)
{
	IICx->CON.SA = IIC_Address >> 1;
	IICx->CON.RW = Mode;

	return;
}

/*************************************
  函数名：IIC_SetAddress
  描  述：设置从机地址
  输入值：I2C0/I2C1
  从机地址
  返回值：无
 **************************************/
void IIC_SetAddress(I2C_TypeDef *IICx, uint8_t IIC_Address)
{
	IICx->CON.SA = IIC_Address >> 1;
	return;
}

/*************************************
  函数名：IIC_RecModeConfig
  描  述：接收模式配置
  输入值：I2C0/I2C1
  接收模式
  返回值：无
 **************************************/
void IIC_RecModeConfig(I2C_TypeDef *IICx, IIC_TYPE_RECMODE RecType)
{
	IICx->MOD.RDM = RecType;
	return;
}

/*************************************
  函数名：IIC_TBIMConfig
  描  述：发送缓冲器空中断类型
  输入值：I2C0/I2C1
  中断类型
  返回值：无
 **************************************/
void IIC_TBIMConfig(I2C_TypeDef *IICx, IIC_TYPE_TRBIM Type)
{
	IICx->IE.TBIM = Type;
	return;
}

/*************************************
  函数名：IIC_RBIMConfig
  描  述：接收缓冲器满中断类型
  输入值：I2C0/I2C1
  中断类型
  返回值：无
 **************************************/
void IIC_RBIMConfig(I2C_TypeDef *IICx, IIC_TYPE_TRBIM Type)
{
	IICx->IE.RBIM = Type;
	return;
}

/*************************************
  函数名：IIC_AckDelay
  描  述：设置延迟应答
  输入值：I2C0/I2C1
  延迟时间
  使能/禁止
  返回值：无
 **************************************/
void IIC_AckDelay(I2C_TypeDef *IICx, IIC_TYPE_ADLY Type, TYPE_FUNCEN NewStatus)
{
	IICx->MOD.ADLY = Type;
	IICx->MOD.ADE  = NewStatus;
	return;
}

/*************************************
  函数名：IIC_TISConfig
  描  述：设置数据帧传输间隔
  输入值：I2C0/I2C1
  间隔时间
  返回值：无
 **************************************/
void IIC_TISConfig(I2C_TypeDef *IICx, IIC_TYPE_TIS Time)
{
	IICx->MOD.TIS = Time;
	return;
}

/*************************************
  函数名：IIC_SendByte
  描  述：发送一个字节
  输入值：I2C0/I2C1
  需要发送的数据
  返回值：无
 **************************************/
void IIC_SendByte(I2C_TypeDef *IICx, uint8_t Byte)
{
	IICx->TBW.Byte[0] = Byte;
	return;
}

/*************************************
  函数名：IIC_SendHalfWord
  描  述：发送半字
  输入值：I2C0/I2C1
  需要发送的数据
  返回值：无
 **************************************/
void IIC_SendHalfWord(I2C_TypeDef *IICx, uint16_t HalfWord)
{
	IICx->TBW.HalfWord[0] = HalfWord;
	return;
}

/*************************************
  函数名：IIC_SendWord
  描  述：发送半字
  输入值：I2C0/I2C1
  需要发送的数据
  返回值：无
 **************************************/
void IIC_SendWord(I2C_TypeDef *IICx, uint32_t Word)
{
	IICx->TBW.Word = Word;
	return;
}

/*************************************
  函数名：IIC_RecByte
  描  述：接收一个字节
  输入值：I2C0/I2C1
  返回值：接收到的数据
 **************************************/
uint8_t IIC_RecByte(I2C_TypeDef *IICx)
{
	return IICx->RBR.Byte[0];
}

/*************************************
  函数名：IIC_RecHalfWord
  描  述：接收半字
  输入值：I2C0/I2C1
  返回值：接收到的数据
 **************************************/
uint16_t IIC_RecHalfWord(I2C_TypeDef *IICx)
{
	return IICx->RBR.HalfWord[0];
}

/*************************************
  函数名：IIC_RecWord
  描  述：接收字
  输入值：I2C0/I2C1
  返回值：接收到的数据
 **************************************/
uint32_t IIC_RecWord(I2C_TypeDef *IICx)
{
	return IICx->RBR.Word;
}

/*************************************
  函数名：IIC_GetRWMode
  描  述：获取IIC读写状态
  输入值：I2C0/I2C1
  返回值：读写状态
 **************************************/
IIC_TYPE_RWMODE IIC_GetRWMode(I2C_TypeDef *IICx)
{
	IIC_TYPE_RWMODE status;

	status = IICx->CON.RW == 0 ? IIC_Mode_Write : IIC_Mode_Read;
	return status;
}

/*************************************
  函数名：IIC_GetTBStatus
  描  述：获取发送缓冲器状态，TB0-TB3全空返回SET，否则返回RESET
  输入值：I2C0/I2C1
  返回值：SET/RESET
 **************************************/
FlagStatus IIC_GetTBStatus(I2C_TypeDef *IICx)
{
	FlagStatus status;

	if ((IICx->STA.Word & (uint32_t)0xf00) != (uint32_t)0xf00)
		status = RESET;
	else
		status = SET;

	return status;
}

/*************************************
  函数名：IIC_GetFlagStatus
  描  述：获取中断标志位状态
  输入值：I2C0/I2C1
  中断类型
  返回值：SET/RESET
 **************************************/
FlagStatus IIC_GetFlagStatus(I2C_TypeDef *IICx, IIC_TYPE_IF IIC_Flag)
{
	FlagStatus status;

	if ((IICx->IF.Word & (uint32_t)IIC_Flag) != RESET)
		status = SET;
	else
		status = RESET;

	return status;
}

/*************************************
  函数名：IIC_GetITStatus
  描  述：获取中断使能状态
  输入值：I2C0/I2C1
  中断类型
  返回值：SET/RESET
 **************************************/
FlagStatus IIC_GetITStatus(I2C_TypeDef *IICx, IIC_TYPE_IT IIC_Flag)
{
	FlagStatus status = RESET;

	if ((IICx->IE.Word & (uint32_t)IIC_Flag) == RESET)
		return status;

	if ((IICx->IF.Word & (uint32_t)IIC_Flag) != RESET)
		status = SET;

	return status;
}

/*************************************
  函数名：IIC_GetITStatus
  描  述：清除中断状态
  输入值：I2C0/I2C1
  中断类型
  返回值：无
 **************************************/
void IIC_ClearITPendingBit(I2C_TypeDef *IICx, IIC_CLR_IF IIC_IT)
{
	IICx->IF.Word = (uint32_t)IIC_IT;
	return;
}

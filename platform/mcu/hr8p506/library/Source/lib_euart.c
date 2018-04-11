/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_euart.c
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  EUART模块库函数
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#include "lib_euart.h"
#include "system_hr8p506.h"

/*************************************
  函数名：EUART_ModeConfig
  描  述：EUART工作模式选择
  输入值：工作模式枚举类型变量
  返回值：无
 **************************************/
void EUART_ModeConfig(EUART_TYPE_MODE Mode)
{
	EUART0->CON2.MOD = Mode;
	return;
}

/*************************************
  函数名：EUART_Init
  描  述：通用UART模式初始化
  输入值：初始化结构体
  返回值：无
 **************************************/
void EUART_Init(EUART_InitStruType *EUART_InitStruct)
{
	EUART_BaudConfig(EUART_InitStruct->EUART_BaudRate, EUART_InitStruct->EUART_ClockSet);

	EUART0->CON0.TXFS  = EUART_InitStruct->EUART_StopBits;
	EUART0->CON0.TXMOD = EUART_InitStruct->EUART_TxMode;
	EUART0->CON0.TXP   = EUART_InitStruct->EUART_TxPolar;
	EUART0->CON0.RXMOD = EUART_InitStruct->EUART_RxMode;
	EUART0->CON0.RXP   = EUART_InitStruct->EUART_RxPolar;

	return;
}

/*************************************
  函数名：EUART_BaudConfig
  描  述：波特率配置
  输入值：波特率
  时钟选择
  返回值：无
 **************************************/
void EUART_BaudConfig(uint32_t BaudRate, EUART_TYPE_BCS ClockSet)
{
	uint32_t tmp  = 0;
	uint32_t baud = 0;

	if(ClockSet == EUART_Clock_1) {
		tmp =  (SystemCoreClock / 16) % BaudRate;
		if(BaudRate >= (2 * tmp)) 
			baud = (SystemCoreClock / 16 / BaudRate) - 1;
		else
			baud = SystemCoreClock / 16 / BaudRate;
	}
	else if(ClockSet == EUART_Clock_2) {
		tmp =  (SystemCoreClock / 32) % BaudRate;
		if(BaudRate >= (2 * tmp))
			baud = (SystemCoreClock / 32 / BaudRate) - 1;
		else
			baud = SystemCoreClock / 32 / BaudRate;
	}
	else if(ClockSet == EUART_Clock_3){
		tmp =  (SystemCoreClock / 64) % BaudRate;
		if(BaudRate >= (2 * tmp))
			baud = (SystemCoreClock / 64 / BaudRate) - 1;
		else
			baud = SystemCoreClock / 64 / BaudRate;
	}
	else if(ClockSet == EUART_Clock_4) {
		tmp =  (SystemCoreClock / 128) % BaudRate;
		if(BaudRate >= (2 * tmp))
			baud = (SystemCoreClock / 128 / BaudRate) - 1;
		else
			baud = SystemCoreClock / 128 / BaudRate;
	}

	baud = baud > 2046 ? 2047 : baud;
	EUART0->CON1.BCS = ClockSet;
	EUART0->BRR.BRR  = baud;

	return;
}

/*************************************
  函数名：EUART_ITConfig
  描  述：通用UART中断使能
  输入值：中断类型枚举变量
  使能或禁止
  返回值：无
 **************************************/
void EUART_ITConfig(EUART_TYPE_IT EUART_IT, TYPE_FUNCEN NewState)
{
	if (NewState != Disable)
		EUART0->IE.Word |= (uint32_t)EUART_IT;
	else
		EUART0->IE.Word &= ~((uint32_t)EUART_IT);

	return;
}

/*************************************
  函数名：EUART_TBIMConfig
  描  述：发送缓冲空中断类型配置
  输入值：中断类型枚举变量
  返回值：无
 **************************************/
void EUART_TBIMConfig(EUART_TYPE_TRBIM Type)
{
	EUART0->CON1.TBIM = Type;
	return;
}

/*************************************
  函数名：EUART_RBIMConfig
  描  述：接收缓冲满中断类型配置
  输入值：中断类型枚举变量
  返回值：无
 **************************************/
void EUART_RBIMConfig(EUART_TYPE_TRBIM Type)
{
	EUART0->CON1.RBIM = Type;
	return;
}

/*************************************
  函数名：EUART_GetFlagStatus
  描  述：获取中断标志位状态
  输入值：中断类型枚举变量
  返回值：SET/RESET
 **************************************/
FlagStatus EUART_GetFlagStatus(EUART_TYPE_FLAG EUART_Flag)
{
	FlagStatus status = RESET;

	if (EUART0->IF.Word & (uint32_t)EUART_Flag)
		status = SET;

	return status;
}

/*************************************
  函数名：EUART_GetITStatus
  描  述：获取中断使能状态
  输入值：中断类型枚举变量
  返回值：SET/RESET
 **************************************/
ITStatus EUART_GetITStatus(EUART_TYPE_IT EUART_Flag)
{
	ITStatus status = RESET;

	if (EUART0->IE.Word & (uint32_t)EUART_Flag)
		status = SET;

	return status;
}

/*************************************
  函数名：EUART_ClearITPendingBit
  描  述：清除中断标志位
  输入值：中断类型枚举变量
  返回值：无
 **************************************/
void EUART_ClearITPendingBit(EUART_CLR_IF EUART_Flag)
{
	EUART0->IF.Word = (uint32_t)EUART_Flag;
	return;
}

/*************************************
  函数名：U7816_Init
  描  述：7816模式初始化
  输入值：初始化结构体
  返回值：无
 **************************************/
void U7816_Init(U7816_InitStruType *U7816_InitStruct)
{
	EUART_BaudConfig(U7816_InitStruct->U7816_BaudRate, U7816_InitStruct->U7816_ClockSet);

	EUART0->CON2.CK0E = U7816_InitStruct->U7816_ECK0;
	EUART0->CON2.CK1E = U7816_InitStruct->U7816_ECK1;
	EUART0->CON2.CHS  = U7816_InitStruct->U7816_EIOCh;
	EUART0->CON2.IOC  = U7816_InitStruct->U7816_EIODir;
	EUART0->CON2.DAS  = U7816_InitStruct->U7816_DataForm;
	EUART0->CON2.PS   = U7816_InitStruct->U7816_DataVerify;
	EUART0->CON2.ARTE = U7816_InitStruct->U7816_AutoRetryTx;
	EUART0->CON2.ARRE = U7816_InitStruct->U7816_AutoRetryRx;
	EUART0->CON2.TNAS = U7816_InitStruct->U7816_NACK_Width;
	EUART0->CON2.ARTS = U7816_InitStruct->U7816_RetryTimes;
	EUART0->CON2.CKS  = U7816_InitStruct->U7816_CLKS;
	EUART0->CON2.ETUS = U7816_InitStruct->U7816_ETUTime;

	return;
}

/*************************************
  函数名：EUART_EIOChConfig
  描  述：配置EIO通道
  输入值：EIO通道
  返回值：无
 **************************************/
void EUART_EIOChConfig(U7816_TYPE_CHS U7816_IO)
{
	EUART0->CON2.CHS = U7816_IO;
	return;
}

/*************************************
  函数名：EUART_EIODirection
  描  述：配置EIO方向
  输入值：EIO方向
  返回值：无
 **************************************/
void EUART_EIODirection(U7816_TYPE_IOC Dir)
{
	EUART0->CON2.IOC = Dir;
	return;
}

/*************************************
  函数名：EUART_SendByte
  描  述：发送一个字节
  输入值：需要的发送数据
  返回值：无
 **************************************/
void EUART_SendByte(uint8_t ByteData)
{
	EUART0->TBW.Byte[0] = ByteData;
	return;
}

/*************************************
  函数名：EUART_SendHalfWord
  描  述：发送半字
  输入值：需要的发送数据
  返回值：无
 **************************************/
void EUART_SendHalfWord(uint16_t HalfWordData)
{
	EUART0->TBW.HalfWord[0] = HalfWordData;
	return;
}

/*************************************
  函数名：EUART_SendWord
  描  述：发送字
  输入值：需要的发送数据
  返回值：无
 **************************************/
void EUART_SendWord(uint32_t WordData)
{
	EUART0->TBW.Word = WordData;
	return;
}

/*************************************
  函数名：EUART_RecByte
  描  述：接收一个字节
  输入值：无
  返回值：接收到的数据
 **************************************/
uint8_t EUART_RecByte(void)
{
	return EUART0->RBR.Byte[0];
}

/*************************************
  函数名：EUART_RecHalfWord
  描  述：接收半字
  输入值：无
  返回值：接收到的数据
 **************************************/
uint16_t EUART_RecHalfWord(void)
{
	return EUART0->RBR.HalfWord[0];
}

/*************************************
  函数名：EUART_RecWord
  描  述：接收字
  输入值：无
  返回值：接收到的数据
 **************************************/
uint32_t EUART_RecWord(void)
{
	return EUART0->RBR.Word;
}




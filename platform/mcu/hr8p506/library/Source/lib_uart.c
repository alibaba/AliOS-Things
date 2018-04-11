/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_uart.c
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  UART模块库函数
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#include "lib_uart.h"
#include "system_hr8p506.h"

/*************************************
  函数名：UART_Init
  描  述：UART初始化
  输入值：UART0/UART1
  初始化结构体
  返回值：无
 **************************************/
void UART_Init(UART_TypeDef *UARTx, UART_InitStruType *UART_InitStruct)
{
	float tmp;
	uint32_t baud;
	
		tmp = 0;
	if (UART_InitStruct->UART_ClockSet == UART_Clock_1) {
		tmp  = (float)SystemCoreClock / (float)(16 * UART_InitStruct->UART_BaudRate);
	}
	else if (UART_InitStruct->UART_ClockSet == UART_Clock_2) {
		tmp  = (float)SystemCoreClock / (float)(32 * UART_InitStruct->UART_BaudRate);
	}
	else if (UART_InitStruct->UART_ClockSet == UART_Clock_3) {
		tmp  = (float)SystemCoreClock / (float)(64 * UART_InitStruct->UART_BaudRate);
	}
	else if (UART_InitStruct->UART_ClockSet == UART_Clock_4) {
		tmp  = (float)SystemCoreClock / (float)(128 * UART_InitStruct->UART_BaudRate);
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

/*************************************
  函数名：UART_AutoBaudConfig
  描  述：UART自动配置波特率并返回波特率
  输入值：UART0/UART1
	  mode:检测模式选择
  返回值：检测到的波特率的值，如果为0，则配置失败
 **************************************/
void UART_AutoBaudConfig(UART_TypeDef *UARTx,UART_AUTO_BAUD_MODE mode)
{
	UARTx->CON1.BDM = mode;
	UARTx->CON0.BDEN = 1;
}

/*************************************
  函数名：UART_BaudFreeConfig
  描  述：UART空闲检测
  输入值：UART0/UART1
	  mode:检测模式选择
  返回值：检测到的波特率的值，如果为0，则配置失败
 **************************************/
void UART_BaudFreeConfig(UART_TypeDef *UARTx,UART_AUTO_BAUD_FREE_MODE mode)
{
    UART0->CON0.IDEN = 1;               //使能空闲检测
    UART0->CON1.IDM = mode;
}

/*************************************
  函数名：UART_ITConfig
  描  述：中断配置
  输入值：UART0/UART1
  中断类型
  使能/禁止
  返回值：无
 **************************************/
void UART_ITConfig(UART_TypeDef *UARTx, UART_TYPE_IT UART_IT, TYPE_FUNCEN NewState)
{
	if (NewState == Enable)
		UARTx->IE.Word |= (uint32_t)UART_IT;
	else
		UARTx->IE.Word &= ~(uint32_t)UART_IT;

	return;
}

/*************************************
  函数名：UART_TBIMConfig
  描  述：发送缓冲器空中断类型
  输入值：UART0/UART1
  中断类型
  返回值：无
 **************************************/
void UART_TBIMConfig(UART_TypeDef *UARTx, UART_TYPE_TRBIM Type)
{
	UARTx->CON1.TBIM = Type;
	return;
}

/*************************************
  函数名：UART_RBIMConfig
  描  述：接收缓冲器满中断类型
  输入值：UART0/UART1
  中断类型
  返回值：无
 **************************************/
void UART_RBIMConfig(UART_TypeDef *UARTx, UART_TYPE_TRBIM Type)
{
	UARTx->CON1.RBIM = Type;
	return;
}

/*************************************
  函数名：UART_SendByte
  描  述：发送一个字节
  输入值：UART0/UART1
  需要发送的数据
  返回值：无
 **************************************/
void UART_SendByte(UART_TypeDef *UARTx, uint8_t data08)
{
	UARTx->TBW.Byte[0] = data08;
	return;
}

/*************************************
  函数名：UART_SendHalfWord
  描  述：发送半字
  输入值：UART0/UART1
  需要发送的数据
  返回值：无
 **************************************/
void UART_SendHalfWord(UART_TypeDef *UARTx, uint16_t data16)
{
	UARTx->TBW.HalfWord[0] = data16;
	return;
}

/*************************************
  函数名：UART_SendWord
  描  述：发送字
  输入值：UART0/UART1
  需要发送的数据
  返回值：无
 **************************************/
void UART_SendWord(UART_TypeDef *UARTx, uint32_t data32)
{
	UARTx->TBW.Word = data32;
	return;
}

/*************************************
  函数名：UART_RecByte
  描  述：接收一个字节
  输入值：UART0/UART1
  返回值：接收到的数据
 **************************************/
uint8_t UART_RecByte(UART_TypeDef *UARTx)
{
	return UARTx->RBR.Byte[0];
}

/*************************************
  函数名：UART_RecHalfWord
  描  述：接收半字
  输入值：UART0/UART1
  返回值：接收到的数据
 **************************************/
uint16_t UART_RecHalfWord(UART_TypeDef *UARTx)
{
	return UARTx->RBR.HalfWord[0];
}

/*************************************
  函数名：UART_RecWord
  描  述：接收字
  输入值：UART0/UART1
  返回值：接收到的数据
 **************************************/
uint32_t UART_RecWord(UART_TypeDef *UARTx)
{
	return UARTx->RBR.Word;
}

/*************************************
  函数名：UART_GetStatus
  描  述：获取UART状态
  输入值：UART0/UART1
  状态类型
  返回值：SET/RESET
 **************************************/
FlagStatus UART_GetStatus(UART_TypeDef *UARTx, UART_TYPE_STA UART_Flag)
{
	FlagStatus bitstatus = RESET;

	if (UARTx->STA.Word & UART_Flag)
		bitstatus = SET;

	return bitstatus;
}

/*************************************
  函数名：UART_GetFlagStatus
  描  述：获取中断标志位状态
  输入值：UART0/UART1
  状态类型
  返回值：SET/RESET
 **************************************/
FlagStatus UART_GetFlagStatus(UART_TypeDef *UARTx, UART_TYPE_FLAG UART_Flag)
{
	FlagStatus bitstatus = RESET;

	if (UARTx->IF.Word & UART_Flag)
		bitstatus = SET;

	return bitstatus;
}

/*************************************
  函数名：UART_GetITStatus
  描  述：获取中断使能状态
  输入值：UART0/UART1
  状态类型
  返回值：SET/RESET
 **************************************/
ITStatus UART_GetITStatus(UART_TypeDef *UARTx, UART_TYPE_IT UART_IT)
{
	ITStatus bitstatus = RESET;

	if (UARTx->IE.Word & UART_IT)
		bitstatus = SET;

	return bitstatus;
}

/*************************************
  函数名：UART_ClearITPendingBit
  描  述：清除中断状态
  输入值：UART0/UART1
  状态类型
  返回值：无
 **************************************/
void UART_ClearITPendingBit(UART_TypeDef *UARTx, UART_CLR_IF UART_Flag)
{
	UARTx->IF.Word = (UART_Flag);
	return;
}


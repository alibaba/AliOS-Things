/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_spi.c
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  SPI模块库函数
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#include "lib_spi.h"
#include "system_hr8p506.h"

/*************************************
  函数名：SPI_Init
  描  述：SPI初始化
  输入值：SPI0/SPI1
  初始化结构体
  返回值：无
 **************************************/
void SPI_Init(SPI_TypeDef *SPIx, SPI_InitStruType *SPI_InitStruct)
{
	uint32_t tmp = 0;

	if (SPI_InitStruct->SPI_Freq != SystemCoreClock) {
		tmp = SystemCoreClock / (SPI_InitStruct->SPI_Freq << 1);
		tmp = tmp > 0xfe ? 0xff : tmp;
	}

	SPIx->CKS.CKS = tmp;
		SPIx->CON.DW  = SPI_InitStruct->SPI_DW;
	SPIx->CON.DFS = SPI_InitStruct->SPI_Df;
	SPIx->CON.MS  = SPI_InitStruct->SPI_Mode;
	SPIx->CON.DRE = SPI_InitStruct->SPI_DelayRec;
	SPIx->CON.TME = SPI_InitStruct->SPI_DelaySend;

	tmp = (uint32_t)SPI_InitStruct->SPI_SendDelayPeroid - 1;
	tmp = tmp > 63 ? 63 : tmp;
	SPIx->CON.TMP   = tmp;
		SPIx->CON.TXCLR = 1;
		SPIx->CON.RXCLR = 1;	

	return;
}

/*************************************
  函数名：SPI_ITConfig
  描  述：配置中断使能
  输入值：SPI0/SPI1
  中断类型
  使能/禁止
  返回值：无
 **************************************/
void SPI_ITConfig(SPI_TypeDef *SPIx, SPI_TYPE_IT SPI_IE, TYPE_FUNCEN NewState)
{
	if (NewState != Disable)
		SPIx->IE.Word |= (uint32_t)SPI_IE;
	else
		SPIx->IE.Word &= ~(uint32_t)SPI_IE;

	return;
}

/*************************************
  函数名：SPI_DataFormatConfig
  描  述：SPI数据格式
  输入值：SPI0/SPI1
  数据格式
  返回值：无
 **************************************/
void SPI_DataFormatConfig(SPI_TypeDef *SPIx, SPI_TYPE_DFS Type)
{
	SPIx->CON.DFS = Type;
	return;
}

/*************************************
  函数名：SPI_SendByte
  描  述：发送一个字节
  输入值：SPI0/SPI1
  需要发送的数据
  返回值：无
 **************************************/
void SPI_SendByte(SPI_TypeDef *SPIx, uint8_t Temp)
{
	SPIx->TBW.Byte[0] = Temp;
	return;

}

/*************************************
  函数名：SPI_SendHalfWord
  描  述：发送半字
  输入值：SPI0/SPI1
  需要发送的数据
  返回值：无
 **************************************/
void SPI_SendHalfWord(SPI_TypeDef *SPIx, uint16_t Temp)
{
	SPIx->TBW.HalfWord[0] = Temp;
	return;
}

/*************************************
  函数名：SPI_SendWord
  描  述：发送字
  输入值：SPI0/SPI1
  需要发送的数据
  返回值：无
 **************************************/
void SPI_SendWord(SPI_TypeDef *SPIx, uint32_t Temp)
{
	SPIx->TBW.Word = Temp;
	return;
}

/*************************************
  函数名：SPI_RecByte
  描  述：接收一个字节
  输入值：SPI0/SPI1
  返回值：接收到的数据
 **************************************/
uint8_t SPI_RecByte(SPI_TypeDef *SPIx)
{
	return SPIx->RBR.Byte[0];
}

/*************************************
  函数名：SPI_RecHalfWord
  描  述：接收半字
  输入值：SPI0/SPI1
  返回值：接收到的数据
 **************************************/
uint16_t SPI_RecHalfWord(SPI_TypeDef *SPIx)
{
	return SPIx->RBR.HalfWord[0];
}

/*************************************
  函数名：SPI_RecHalfWord
  描  述：接收字
  输入值：SPI0/SPI1
  返回值：接收到的数据
 **************************************/
uint32_t SPI_RecWord(SPI_TypeDef *SPIx)
{
	return SPIx->RBR.Word;
}

/*************************************
  函数名：SPI_TBIMConfig
  描  述：配置发送缓冲器空中断类型
  输入值：SPI0/SPI1
  中断类型
  返回值：无
 **************************************/
void SPI_TBIMConfig(SPI_TypeDef *SPIx, SPI_TYPE_TRBIM Type)
{
	SPIx->IE.TBIM = Type;
	return;
}

/*************************************
  函数名：SPI_RBIMConfig
  描  述：配置接收缓冲器满中断类型
  输入值：SPI0/SPI1
  中断类型
  返回值：无
 **************************************/
void SPI_RBIMConfig(SPI_TypeDef *SPIx, SPI_TYPE_TRBIM Type)
{
	SPIx->IE.RBIM = Type;
	return;
}

/*************************************
  函数名：SPI_GetFlagStatus
  描  述：获取中断标志位状态
  输入值：SPI0/SPI1
  中断类型
  返回值：SET/RESET
 **************************************/
FlagStatus SPI_GetFlagStatus(SPI_TypeDef *SPIx, SPI_TYPE_FLAG Flag)
{
	FlagStatus status = RESET;

	if (Flag != SPI_Flag_TMS) {
		if (SPIx->IF.Word & (uint32_t)Flag)
			status = SET;
	}
	else {
		if (SPIx->CON.Word & (uint32_t)Flag)
			status = SET;
	}

	return status;
}

/*************************************
  函数名：SPI_GetITStatus
  描  述：获取中断使能状态
  输入值：SPI0/SPI1
  中断类型
  返回值：SET/RESET
 **************************************/
ITStatus SPI_GetITStatus(SPI_TypeDef *SPIx, SPI_TYPE_IT Flag)
{
	ITStatus status = RESET;
    
    if ((SPIx->IF.Word & (uint32_t)Flag) == RESET)
		return status;
	if ((SPIx->IE.Word & (uint32_t)Flag) != RESET)
		status = SET;

	return status;
}

/*************************************
  函数名：SPI_GetStatus
  描  述：获取SPI状态
  输入值：SPI0/SPI1
  状态类型
  返回值：SET/RESET
 **************************************/
FlagStatus SPI_GetStatus(SPI_TypeDef *SPIx, SPI_TYPE_STA Flag)
{
	FlagStatus status = RESET;

	if (SPIx->STA.Word & (uint32_t)Flag)
		status = SET;

	return status;
}

/*************************************
  函数名：SPI_ClearITPendingBit
  描  述：清除中断标志
  输入值：SPI0/SPI1
  状态类型
  返回值：无
 **************************************/
void SPI_ClearITPendingBit(SPI_TypeDef *SPI, SPI_CLR_IF Flag)
{
	SPI->IF.Word = (uint32_t)Flag;
	return;
}

/*************************************
  函数名：Clear_TBW
  描  述：清空发送缓冲器
  输入值：SPI0/SPI1
  返回值：无
 **************************************/
void Clear_TBW(SPI_TypeDef *SPIx)
{
	SPIx->CON.TXCLR = 1;
	return;
}

/*************************************
  函数名：Clear_RBR
  描  述：清空接收缓冲器
  输入值：SPI0/SPI1
  返回值：无
 **************************************/
void Clear_RBR(SPI_TypeDef *SPIx)
{
	SPIx->CON.RXCLR = 1;
	return;
}

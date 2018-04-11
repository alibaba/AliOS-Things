/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_led.c
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  LED模块库函数
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#if 0
#include "lib_led.h"

/***************************************************************
  函数名：LED_Init
  描  述：LED功能初始化函数
  输入值：LED_InitStruct 初始化结构体
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus LED_Init(LED_InitStruType * LED_InitStruct)
{
	LED_Reset();

	if(LED_InitStruct->LED_MUX >LED_MUX_MAX
			|| LED_InitStruct->LED_COM >LED_COM_MAX
			|| LED_InitStruct->LED_SEG > LED_SEG_MAX
			|| LED_InitStruct->LED_CLK > LED_CLK_MAX
			|| LED_InitStruct->LED_PRS >LED_PRS_MAX) {

		return ERROR;
	}

	LED->CON1.SEL = LED_SEL_LED;

	LED->CON0.MUX= LED_InitStruct->LED_MUX;
	LED->SEL.COM = LED_InitStruct->LED_COM;
	LED->SEL.SEG = LED_InitStruct->LED_SEG;

	LED->CON0.CLK_SEL = LED_InitStruct->LED_CLK;
	LED->CON0.PRS = LED_InitStruct->LED_PRS;

	LED_EN();
	return SUCCESS;
}

/***************************************************************
函数名：LED_PixelWriteByte
描  述：LED 一次写8bit数据
输入值：LED_DD 	像素寄存器选择
				nByte 		1/4、2/4、3/4、4/4 8bit选择
				LED_data		写入8bit数据
输出值：无
返回值：SUCCESS 成功
			  ERROR 失败
***************************************************************/
ErrorStatus  LED_PixelWriteByte(LED_TYPE_PIXEL LED_DD, LED_DD_BYTE nByte ,uint8_t LED_data)
{
	LED_DD_TypeDef *DD;
    if(nByte >LED_Byte_MAX||LED_DD>LED_Pixel_MAX)
	{
		return ERROR;
	}
	switch (LED_DD)
	{
		case LED_Pixel_LEDD0:
			DD = (LED_DD_TypeDef *)(&(LCD->D0));
			break;
		case LED_Pixel_LEDD1:
			DD = (LED_DD_TypeDef *)(&(LCD->D1));
			break;
		default:
			return	ERROR;
	}
	DD->Byte[nByte] = LED_data;
	return SUCCESS;
}

/***************************************************************
函数名：LED_PixelWriteHalfWord
描  述：LED 一次写16bit数据
输入值：LED_DD 	像素寄存器选择
				nHalfWord 	前/后16bit选择
				LED_data		写入16bit数据
输出值：无
返回值：SUCCESS 成功
			  ERROR 失败
***************************************************************/
ErrorStatus  LED_PixelWriteHalfWord(LED_TYPE_PIXEL LED_DD, LED_DD_HALFWORD nHalfWord , uint16_t LED_data)
{
	LED_DD_TypeDef *DD;
    if(!(nHalfWord>LED_HalfWord_MAX||LED_DD>LED_Pixel_MAX))
	{
		return ERROR;
	}
	switch (LED_DD)
	{
		case LED_Pixel_LEDD0:
			DD = (LED_DD_TypeDef *)(&(LCD->D0));
			break;
		case LED_Pixel_LEDD1:
			DD = (LED_DD_TypeDef *)(&(LCD->D1));
			break;		
		default:
			return 	ERROR;
	}
	DD->HalfWord[nHalfWord] = LED_data;
	return SUCCESS;
}
/***************************************************************
  函数名：LED_PixelWriteWord
  描  述：LED 一次写32bit数据
  输入值：LED_DD 	像素寄存器选择
  LED_data		写入32bit数据
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus  LED_PixelWriteWord(LED_TYPE_PIXEL LED_DD,uint32_t LED_data)
{
	if(LED_DD>LED_Pixel_MAX) {
		return ERROR;
	}
	switch (LED_DD)
	{
		case LED_Pixel_LEDD0:
			LED->D0.Word = LED_data;
			break;
		case LED_Pixel_LEDD1:
			LED->D1.Word = LED_data;
			break;	
		default:
			return 	ERROR;
	}
	return SUCCESS;
}

/***************************************************************
  函数名：LED_Reset
  描  述：LED复位
  输入值：无
  输出值：无
  返回值：无
 ***************************************************************/
void LED_Reset(void)
{
	LED->CON0.Word = 0xf0000000;
	LED->SEL.Word = 0x00000000;
	LED->CON1.Word = 0x00000000;

	LED->D0.Word = 0x00000000;
	LED->D1.Word = 0x00000000;
	LED_RST();
}
#endif

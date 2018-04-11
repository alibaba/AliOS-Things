/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_gpio.c
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  LCD模块库函数
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#include "lib_lcd.h"

/***************************************************************
  函数名：LCD_Init
  描  述：LCD功能初始化函数
  输入值：LCD_InitStruct 初始化结构体
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus LCD_Init(LCD_InitStruType * LCD_InitStruct)
{

	if(	LCD_InitStruct ->LCD_COMS > LCD_COMS_MAX
			|| LCD_InitStruct ->LCD_SEG > LCD_SEG_MAX
			|| LCD_InitStruct ->LCD_VLCD > LCD_VLCD_MAX
			|| LCD_InitStruct ->LCD_WFS > LCD_WFS_MAX
			|| LCD_InitStruct ->LCD_CLK > LCD_CLK_MAX
			|| LCD_InitStruct ->LCD_PRS > LCD_PRS_MAX
			|| LCD_InitStruct ->LCD_BVS > LCD_BVS_MAX
			|| LCD_InitStruct ->LCD_BIAS > LCD_BIAS_MAX
			|| LCD_InitStruct ->LCD_RT > LCD_RT_MAX
			|| LCD_InitStruct ->LCD_RS > LCD_RS_MAX
			|| LCD_InitStruct ->LCD_FLIK > LCD_FLIK_MAX
			|| LCD_InitStruct ->LCD_TOFF > LCD_TOFF_MAX
			|| LCD_InitStruct ->LCD_TON > LCD_TON_MAX) {
		return ERROR;
	}

	LCD_Reset();

	LCD->CON1.SEL = LCD_SEL_LCD; /* 选择LCD驱动器 */

	LCD->CON0.COMS = LCD_InitStruct->LCD_COMS;/* 设置LCD驱动端口*/
	LCD->SEL.SEG = LCD_InitStruct->LCD_SEG;

	LCD->CON0.VLCDEN = LCD_InitStruct ->LCD_VLCD;
	LCD ->CON0.WFS = LCD_InitStruct->LCD_WFS;

	LCD ->CON0.CLK_SEL = LCD_InitStruct ->LCD_CLK;
	LCD ->CON0.PRS = LCD_InitStruct->LCD_PRS;

	LCD ->CON0.BIAS = LCD_InitStruct->LCD_BIAS;
	LCD ->CON0.BVS = LCD_InitStruct->LCD_BVS;

	if(LCD_InitStruct->LCD_RS== LCD_RS_60K_225K || LCD_InitStruct->LCD_RS== LCD_RS_60K_900K) {
		LCD ->CON0.RT = LCD_InitStruct->LCD_RT;
		LCD ->CON0.RS = LCD_InitStruct->LCD_RS;
	}
	else {
		LCD ->CON0.RS = LCD_InitStruct->LCD_RS;
	}


	if(LCD_InitStruct ->LCD_FLIK == LCD_FLIK_YES) {
		LCD->TWI.TOFF = LCD_InitStruct ->LCD_TOFF;
		LCD->TWI.TON = LCD_InitStruct ->LCD_TON;
		LCD->CON0.FLIK = LCD_FLIK_YES;
	}
	else {
		LCD->CON0.FLIK = LCD_FLIK_NO;
	}

	LCD_EN();

	return SUCCESS;

}

/***************************************************************
  函数名：LCD_GrayscaleConfig
  描  述：LCD灰度设置
  输入值：LCD_Grayscale	灰度设置值
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus LCD_GrayscaleConfig(uint8_t BVS_Sel)
{
	if(BVS_Sel>LCD_BVS_MAX) {
		return ERROR;
	}
	LCD->CON0.BVS = BVS_Sel;

	return SUCCESS;
}

/***************************************************************
  函数名：LCD_FlickerTimeConfig
  描  述：LCD闪烁功能设置
  输入值：FLIK_EN		闪烁使能选择
  On_Time		闪烁点亮时间
  Off_Time		闪烁熄灭时间
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus LCD_FlickerTimeConfig(LCD_TYPE_FLIK Flick,uint8_t On_Time,uint8_t Off_Time)
{
	if(On_Time > LCD_TON_MAX ||Off_Time >LCD_TOFF_MAX) {
		return ERROR;
	}
	if(Flick == LCD_FLIK_YES)
	{
		LCD->TWI.TON = On_Time;
		LCD->TWI.TOFF = Off_Time;
		LCD->CON0.FLIK = LCD_FLIK_YES;
	}
	else
	{
		LCD->TWI.TON = 0;
		LCD->TWI.TOFF = 0;
		LCD->CON0.FLIK = LCD_FLIK_NO;
	}

	return SUCCESS;
}

# if 0
/***************************************************************
  函数名：LCD_ResistorConfig
  描  述：LCD内部偏压电阻设置
  输入值：Rsel	内部偏压电阻选择
  Rtime	偏压电阻切换选择
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus LCD_ResistorConfig(LCD_TYPE_RS Rsel,LCD_TYPE_RT Rtime)
{
	if(Rsel>LCD_RS_MAX ||Rtime>LCD_RT_MAX) {
		return ERROR;
	}

	LCD ->CON0.RS = Rsel;
	if(Rsel== LCD_RS_60K_225K||Rsel== LCD_RS_60K_900K) {
		LCD ->CON0.RT = Rtime;
	}

	return SUCCESS;
}
#endif

/***************************************************************
  函数名：LCD_PixelWriteByte
  描  述：LCD 一次写8bit数据
  输入值：LCD_DD 	像素寄存器选择
  nByte 		1/4、2/4、3/4、4/4 8bit选择
  LCD_data		写入8bit数据
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/

uint32_t MASK_BYTE[] = {0xff,0xff00,0xff0000,0xff000000};

ErrorStatus  LCD_PixelWriteByte(LCD_TYPE_PIXEL LCD_DD, LCD_DD_BYTE nByte ,uint8_t LCD_data)
{

	LCD_DD_TypeDef *DD;
	if(nByte >LCD_Byte_MAX || LCD_DD>LCD_Pixel_MAX )
	{
		return ERROR;
	}

	switch (LCD_DD)
	{
		case LCD_Pixel_LCDD0:
			DD = (LCD_DD_TypeDef *)(&(LCD->D0));
			break;
		case LCD_Pixel_LCDD1:
			DD = (LCD_DD_TypeDef *)(&(LCD->D1));
			break;
		case LCD_Pixel_LCDD2:
			DD = (LCD_DD_TypeDef *)(&(LCD->D2));
			break;
		case LCD_Pixel_LCDD3:
			DD = (LCD_DD_TypeDef *)(&(LCD->D3));
			break;
		case LCD_Pixel_LCDD4:
			DD = (LCD_DD_TypeDef *)(&(LCD->D4));
			break;
		case LCD_Pixel_LCDD5:
			DD = (LCD_DD_TypeDef *)(&(LCD->D5));
			break;	
		case LCD_Pixel_LCDD6:
			DD = (LCD_DD_TypeDef *)(&(LCD->D6));
			break;
		default:
			return 	ERROR;
	}
	DD->Byte[nByte] = LCD_data;
	return SUCCESS;
}

/***************************************************************
  函数名：LCD_PixelWriteHalfWord
  描  述：LCD 一次写16bit数据
  输入值：LCD_DD 	像素寄存器选择
  nHalfWord 	前/后16bit选择
  LCD_data		写入16bit数据
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus  LCD_PixelWriteHalfWord(LCD_TYPE_PIXEL LCD_DD, LCD_DD_HALFWORD nHalfWord , uint16_t LCD_data)
{
	LCD_DD_TypeDef *DD;

	if( LCD_DD>LCD_Pixel_MAX || nHalfWord >LCD_HalfWord_MAX)
	{
		return ERROR;
	}
	switch (LCD_DD)
	{
		case LCD_Pixel_LCDD0:
			DD = (LCD_DD_TypeDef *)(&(LCD->D0));
			break;
		case LCD_Pixel_LCDD1:
			DD = (LCD_DD_TypeDef *)(&(LCD->D1));
			break;
		case LCD_Pixel_LCDD2:
			DD = (LCD_DD_TypeDef *)(&(LCD->D2));
			break;
		case LCD_Pixel_LCDD3:
			DD = (LCD_DD_TypeDef *)(&(LCD->D3));
			break;
		case LCD_Pixel_LCDD4:
			DD = (LCD_DD_TypeDef *)(&(LCD->D4));
			break;
		case LCD_Pixel_LCDD5:
			DD = (LCD_DD_TypeDef *)(&(LCD->D5));
			break;
		case LCD_Pixel_LCDD6:
			DD = (LCD_DD_TypeDef *)(&(LCD->D6));
			break;		
		default:
			return 	ERROR;
	}
	DD->HalfWord[nHalfWord] = LCD_data;

	return SUCCESS;
}

/***************************************************************
  函数名：LCD_PixelWriteWord
  描  述：LCD 一次写32bit数据
  输入值：LCD_DD 	像素寄存器选择
  LCD_data		写入32bit数据
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus  LCD_PixelWriteWord(LCD_TYPE_PIXEL LCD_DD,uint32_t LCD_data)
{
	if(LCD_DD>LCD_Pixel_MAX) {
		return ERROR;
	}
	switch (LCD_DD)
	{
		case LCD_Pixel_LCDD0:
			LCD->D0.Word = LCD_data;
			break;
		case LCD_Pixel_LCDD1:
			LCD->D1.Word = LCD_data;
			break;
		case LCD_Pixel_LCDD2:
			LCD->D2.Word = LCD_data;
			break;
		case LCD_Pixel_LCDD3:
			LCD->D3.Word = LCD_data;
			break;
		case LCD_Pixel_LCDD4:
			LCD->D4.Word = LCD_data;
			break;
		case LCD_Pixel_LCDD5:
			LCD->D5.Word = LCD_data;
			break;
		case LCD_Pixel_LCDD6:
			LCD->D6.Word = LCD_data;
			break;		
		default:
			return 	ERROR;
	}
	return SUCCESS;
}

/***************************************************************
  函数名：LCD_Reset
  描  述：LCD复位
  输入值：无
  输出值：无
  返回值：无
 ***************************************************************/
void LCD_Reset(void)
{
	LCD->CON0.Word = 0xf0000000;
	LCD->TWI.Word = 0xf0000000;
	LCD->SEL.Word = 0xf0000000;
	LCD->CON1.Word = 0xf0000000;

	LCD->D0.Word = 0x00000000;
	LCD->D1.Word = 0x00000000;
	LCD->D2.Word = 0x00000000;
	LCD->D3.Word = 0x00000000;
	LCD->D4.Word = 0x00000000;
	LCD->D5.Word = 0x00000000;
	LCD->D6.Word = 0x00000000;

	LCD->CON1.RST = 1;
}

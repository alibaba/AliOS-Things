/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_gpio.h
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  LCD模块库函数头文件
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#ifndef __LIBLCD_H__
#define __LIBLCD_H__

#include "HR8P506.h"
#include "type.h"

/* 公共端选择位*/
typedef enum {
	LCD_COMS_1 = 0,	/*  */
	LCD_COMS_2 = 4,	/* COM1~COM0*/
	LCD_COMS_3 = 6,	/* COM2~COM0*/
	LCD_COMS_4 = 1,	/* COM3~COM0*/
	LCD_COMS_6 = 2,	/* COM5~COM0*/
	LCD_COMS_8 = 3,	/* COM7~COM0*/
	LCD_COMS_MAX = 6,
}LCD_TYPE_COMS;

/* LCD驱动波形类型选择*/
typedef enum {
	LCD_WFS_A = 0, /* A波形(在每一公共端类型内改变相位)*/
	LCD_WFS_B = 1,/* B波形(在每一帧边界改变相位)*/
	LCD_WFS_MAX = 1,
}LCD_TYPE_WFS;

/* LCD时钟源选择*/
typedef enum {
	LCD_CLK_LRC = 0,/* LRC4分频(内部时钟32KHz)*/
	LCD_CLK_LOSE = 1,/* LOSC4分频(外部时钟32KHz)*/
	LCD_CLK_PCLK4096 = 2,/* PCLK4096分频*/
	LCD_CLK_MAX = 2,
}LCD_TYPE_CLK;

/* LCD偏置选择*/
typedef enum {
	LCD_BIAS_1_2 = 0,/* 1/2BIAS*/
	LCD_BIAS_1_3 = 1,/* 1/3BIAS*/
	LCD_BIAS_1_4 = 3,/* 1/4BIAS*/
	LCD_BIAS_MAX = 3,
}LCD_TYPE_BIAS;

/* LCD内部偏压电阻总和选择*/
typedef enum {
	LCD_RS_225K = 0,/* 225K欧姆*/
	LCD_RS_900K =1,/* 900K欧姆*/
	LCD_RS_60K = 2,/* 60K欧姆*/
	LCD_RS_60K_225K = 4,/* 60K与225K欧姆自动切换*/
	LCD_RS_60K_900K = 5,/* 60K与900K欧姆自动切换*/
	LCD_RS_MAX = 5,
}LCD_TYPE_RS;

/* LCD60K电阻保持时间*/
typedef enum {
	LCD_RT_1_4 = 0,/* 1/4 COM周期 */
	LCD_RT_1_8 = 1,/* 1/8 COM周期 */
	LCD_RT_1_16 = 2,/* 1/16 COM周期 */
	LCD_RT_1_32 = 3,/* 1/32 COM周期 */
	LCD_RT_1_64 = 4,/* 1/64 COM周期 */
	LCD_RT_MAX = 4,
}LCD_TYPE_RT;

/* 时钟源预分频比选择*/
typedef enum {
	LCD_PRS_MAX = 63,/* 最大容许预分频比1:64 */
}LCD_TYPE_PRS;

/* LCD显示灰度控制(基于VDD) */
typedef enum {
	LCD_BVS_15_30 = 0,		/* 1/2VDD*/
	LCD_BVS_16_30 = 1,		/* 16/30VDD*/
	LCD_BVS_17_30 = 2,		/* 17/30VDD*/
	LCD_BVS_18_30 = 3,		/* 18/30VDD*/
	LCD_BVS_19_30 = 4,		/* 19/30VDD*/
	LCD_BVS_20_30 = 5,		/* 20/30VDD*/
	LCD_BVS_21_30 = 6,		/* 21/30VDD*/
	LCD_BVS_22_30 = 7,		/* 22/30VDD*/
	LCD_BVS_23_30 = 8,		/* 23/30VDD*/
	LCD_BVS_24_30 = 9,		/* 24/30VDD*/
	LCD_BVS_25_30 = 10,		/* 25/30VDD*/
	LCD_BVS_26_30 = 11,		/* 26/30VDD*/
	LCD_BVS_27_30 = 12,		/* 27/30VDD*/
	LCD_BVS_28_30 = 13,		/* 28/30VDD*/
	LCD_BVS_29_30 = 14,		/* 29/30VDD*/
	LCD_BVS_30_30 = 15,		/* 30/30VDD*/
	LCD_BVS_MAX = 15,
}LCD_TYPE_BVS;

typedef enum {
	LCD_VLCD_INSIDE = 0,
	LCD_VLCD_OUTSIDE = 1,
	LCD_VLCD_MAX = 1,
}LCD_TYPE_VLCD;

typedef enum {
	LCD_FLIK_NO = 0,
	LCD_FLIK_YES = 1,
	LCD_FLIK_MAX = 1,
}LCD_TYPE_FLIK;

typedef enum {
	LCD_TOFF_MAX = 255,	/* (TOFF+1)*0.25 */
}LCD_TYPE_TOFF;

typedef enum {
	LCD_TON_MAX = 255,		/* (TON+1)*0.25 */
}LCD_TYPE_TON;

/* LCD段驱动端口使能位 */
typedef enum {
	LCD_SEG_MAX =	0xfffffff,/* 最大容许值*/
}LCD_TYPE_SEG;

/* LCD LED选择控制器*/
typedef enum {
	LCD_SEL_LCD = 0,/* 选择LCD驱动器*/
	LCD_SEL_LED = 1,/* 选择LED驱动器*/
	LCD_SEL_MAX = 1,
}LCD_TYPE_SEL;


typedef FuncState LCD_TYPE_CON1_EN;

/* LCD 像素寄存器选择*/
typedef enum
{
	LCD_Pixel_LCDD0 = 0x0 , /* 像素寄存器0 */
	LCD_Pixel_LCDD1 = 0x1 , /* 像素寄存器1 */
	LCD_Pixel_LCDD2 = 0x2 , /* 像素寄存器2 */
	LCD_Pixel_LCDD3 = 0x3 , /* 像素寄存器3 */
	LCD_Pixel_LCDD4 = 0x4 , /* 像素寄存器4 */
	LCD_Pixel_LCDD5 = 0x5 , /* 像素寄存器5 */
	LCD_Pixel_LCDD6 = 0x6 , /* 像素寄存器6 */
	LCD_Pixel_MAX = 0x6,
}LCD_TYPE_PIXEL;

/* LCD单像素寄存器内字节选择 */
typedef enum
{
	LCD_Byte_0 = 0x0 , 	/* 寄存器内第0个字节*/
	LCD_Byte_1 = 0x1 , 	/* 寄存器内第1个字节*/
	LCD_Byte_2 = 0x2 , 	/* 寄存器内第2个字节*/
	LCD_Byte_3 = 0x3 , 	/* 寄存器内第3个字节*/
	LCD_Byte_MAX = 0x3,
}LCD_DD_BYTE; 

/* LCD单像素寄存器半字选择 */ 
typedef enum
{
	LCD_HalfWord_0 = 0x0 , 
	LCD_HalfWord_1 = 0x1 , 
	LCD_HalfWord_MAX = 0x1,
}LCD_DD_HALFWORD; 


typedef union
{
	uint8_t Byte[4];
	uint16_t HalfWord[2];
	uint32_t Word;
}LCD_DD_TypeDef;

/* LCD初始化结构体*/
typedef struct
{
	LCD_TYPE_COMS  LCD_COMS;    /*公共端选择位 */
	uint32_t  LCD_SEG;          		/*段使能 0~0xfffffff (bit0~27	每1位控制1 个SEG 	) */

	LCD_TYPE_VLCD LCD_VLCD;      /*外部偏置电压使能位*/
	LCD_TYPE_WFS  LCD_WFS;   /*驱动波形类型选择*/

	LCD_TYPE_CLK LCD_CLK;		/* 时钟选择 */
	uint8_t  LCD_PRS;           		/* 时钟源分频比选择位 0~63*/

	LCD_TYPE_BVS LCD_BVS;
	LCD_TYPE_BIAS  LCD_BIAS;    	/* 偏置选择*/

	LCD_TYPE_RT LCD_RT;
	LCD_TYPE_RS LCD_RS;

	LCD_TYPE_FLIK LCD_FLIK;
	uint8_t LCD_TOFF;
	uint8_t LCD_TON;
}LCD_InitStruType;

#define LCD_EN() 		(LCD->CON1.EN = 1)
#define LCD_DIS() 		(LCD->CON1.EN = 0)
#define LCD_RST() 		(LCD->CON1.RST = 1)

/************LCD模块函数声明***********/
ErrorStatus LCD_Init(LCD_InitStruType *LCD_InitStruct);
ErrorStatus LCD_GrayscaleConfig(uint8_t BVS_Sel);
ErrorStatus LCD_FlickerTimeConfig(LCD_TYPE_FLIK Flick,uint8_t On_Time,uint8_t Off_Time);
//ErrorStatus LCD_ResistorConfig(LCD_TYPE_RS R_Sel,LCD_TYPE_RT R_Time);
ErrorStatus  LCD_PixelWriteByte(LCD_TYPE_PIXEL LCD_DD, LCD_DD_BYTE nByte ,uint8_t LCD_data);
ErrorStatus  LCD_PixelWriteHalfWord(LCD_TYPE_PIXEL LCD_DD, LCD_DD_HALFWORD nHalfWord , uint16_t LCD_data);
ErrorStatus  LCD_PixelWriteWord(LCD_TYPE_PIXEL LCD_DD,uint32_t LCD_data);
void  LCD_Reset(void);

#endif

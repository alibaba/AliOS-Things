/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_gpio.h
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  GPIO模块库函头文件
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#ifndef __LIB_GPIO_H__
#define __LIB_GPIO_H__

#include <stdint.h>
#include "HR8P506.h"
#include "type.h"

/* GPIO引脚编号 */
typedef enum {
	GPIO_Pin_B0 = 0,
	GPIO_Pin_B1 = 1,
	GPIO_Pin_B2 = 2,
	GPIO_Pin_B3 = 3,
	GPIO_Pin_B4 = 4,
	GPIO_Pin_B5 = 5,
	GPIO_Pin_B6 = 6,
	GPIO_Pin_B7 = 7,
	GPIO_Pin_B8 = 8,
	GPIO_Pin_B9 = 9,
	GPIO_Pin_B10 = 10,
	GPIO_Pin_B11 = 11,
	GPIO_Pin_B12 = 12,
	GPIO_Pin_B13 = 13,
	GPIO_Pin_A0 = 14,
	GPIO_Pin_A1 = 15,
	GPIO_Pin_A2 = 16,
	GPIO_Pin_A3 = 17,
	GPIO_Pin_A4 = 18,
	GPIO_Pin_A5 = 19,
	GPIO_Pin_A6 = 20,
	GPIO_Pin_A7 = 21,
	GPIO_Pin_A8 = 22,
	GPIO_Pin_A9 = 23,
	GPIO_Pin_A10 = 24,
	GPIO_Pin_A11 = 25,
	GPIO_Pin_A12 = 26,
	GPIO_Pin_A13 = 27,
	GPIO_Pin_A14 = 28,
	GPIO_Pin_A15 = 29,
	GPIO_Pin_A16 = 30,
	GPIO_Pin_A17 = 31,
	GPIO_Pin_A18 = 32,
	GPIO_Pin_A19 = 33,
	GPIO_Pin_A20 = 34,
	GPIO_Pin_A21 = 35,
	GPIO_Pin_A22 = 36,
	GPIO_Pin_A23 = 37,
	GPIO_Pin_A24 = 38,
	GPIO_Pin_A25 = 39,
	GPIO_Pin_A26 = 40,
	GPIO_Pin_A27 = 41,
	GPIO_Pin_A28 = 42,
	GPIO_Pin_A29 = 43,
	GPIO_Pin_A30 = 44,
	GPIO_Pin_A31 = 45,
} GPIO_Pin;

/* 引脚复用功能选择 */
typedef enum {
	GPIO_Reuse_Func0 = 0,	// 引脚复用功能0
	GPIO_Reuse_Func1 = 1,	// 引脚复用功能1
	GPIO_Reuse_Func2 = 2,	// 引脚复用功能2
	GPIO_Reuse_Func3 = 3,	// 引脚复用功能3
} GPIO_Reuse_Func;

/* 引脚方向选择 */
typedef enum {
	GPIO_Direction_Output = 0,	// 输出方向
	GPIO_Direction_Input = 1,	// 输入方向
} GPIO_Direction;

/* 引脚输出开漏使能位 */
typedef enum {
	GPIO_ODE_Output_Disable = 0,	// 开漏禁止
	GPIO_ODE_Output_Enable = 1,	// 开漏使能
} GPIO_ODE_Output;

/* 引脚输出电流驱动能力选择 */
typedef enum {
	GPIO_DS_Output_Normal = 0,	// 普通电流输出
	GPIO_DS_Output_Strong = 1,	// 强电流输出
} GPIO_DS_Output;

/* 引脚输入弱上拉使能 */
typedef enum {
	GPIO_PUE_Input_Disable = 0,	// 弱上拉禁止
	GPIO_PUE_Input_Enable = 1,	// 弱上拉使能
} GPIO_PUE_Input;

/* 引脚输入弱下拉使能 */
typedef enum {
	GPIO_PDE_Input_Disable = 0,	// 弱下拉禁止
	GPIO_PDE_Input_Enable = 1,	// 弱下拉使能
} GPIO_PDE_Input;

/* 引脚信号类型 */
typedef enum {
	GPIO_Pin_Signal_Digital = 0,	// 数字信号引脚
	GPIO_Pin_Signal_Analog = 1,	// 模拟信号引脚
} GPIO_Pin_Signal;	

/* GPIO初始化配置结构定义 */
typedef struct {
	GPIO_Pin_Signal Signal;		/* 引脚上的信号类型，只有模拟和数字两种 */

	GPIO_Direction Dir;		/* 数字引脚时，引脚输入、输出方向选择 */
	GPIO_Reuse_Func Func;		/* 数字引脚时，引脚复用功能选择 */

	GPIO_ODE_Output ODE;		/* 数字引脚、输出方向时，开漏输出选择 */
	GPIO_DS_Output DS;		/* 数字引脚、输出方向时，电流输出大小选择 */

	GPIO_PUE_Input PUE;		/* 数字引脚、输入方向时，弱上拉选择 */
	GPIO_PDE_Input PDE;		/* 数字引脚、输入方向时，弱下拉选择 */
} GPIO_InitSettingType;

/* 外部端口中断使能 */
typedef enum {
	PINT_IE_Set_Disable = 0,	// 中断禁止
	PINT_IE_Set_Enable = 1,		// 中断使能
} PINT_IE_Set;

/* 外部端口中断触发方式 */
typedef enum {
	PINT_Trigger_Rising_Edge = 0,	// 上升沿触发中断
	PINT_Trigger_Trailing_Edge = 1,	// 下降沿触发中断
	PINT_Trigger_High_Level = 2,	// 高电平触发中断
	PINT_Trigger_Low_Level = 3,	// 低电平触发中断
	PINT_Trigger_Both_Edge = 4,	// 上升沿和下降沿都触发中断
} PINT_Trigger_Style;

/* 外部端口初始化配置结构定义 */
typedef struct {
	PINT_IE_Set IE_Set;			// 中断是否使能
	PINT_Trigger_Style Trigger_Style;	// 中断触发方式
} PINT_InitSettingType;

/* KINT中断使能 */
typedef enum {
	KINT_IE_Set_Disable = 0,	// 中断禁止
	KINT_IE_Set_Enable = 1,		// 中断使能
} KINT_IE_Set;

/* KINT中断方式 */
typedef enum {
	KINT_Trigger_Rising_Edge = 0,	// 上升沿触发中断
	KINT_Trigger_Trailing_Edge = 1,	// 下降沿触发中断
	KINT_Trigger_High_Level = 2,	// 高电平触发中断
	KINT_Trigger_Low_Level = 3,	// 低电平触发中断
	KINT_Trigger_Both_Edge = 4,	// 上升沿和下降沿都触发中断
} KINT_Trigger_Style;

/* KINT初始化配置结构定义 */
typedef struct {
	KINT_IE_Set IE_Set;			// 中断是否使能
	KINT_Trigger_Style Trigger_Style;	// 中断触发方式
} KINT_InitSettingType;

/* 脉宽调制电平 */
typedef enum
{
  UART_TXPLV_Low = 0x0 ,  //脉宽调制电平:低
  UART_TXPLV_High = 0x1 , //脉宽调制电平:高
}UART_TYPE_TXPLV;


/* 脉宽PWM脉冲选择 */
typedef enum
{
  UART_TX0PS_NO = 0x0 ,        //脉宽PWM脉冲选择:非调制输出
	UART_TX0PS_T16N0Out0 = 0x1 , //脉宽PWM脉冲选择:T16N1OUT0
	UART_TX0PS_T16N0Out1 = 0x2 , //脉宽PWM脉冲选择:T16N1OUT1
	UART_TX0PS_BUZ = 0x3 ,       //脉宽PWM脉冲选择:BUZ，需要固定38K输出
}UART_TYPE_TX0PS;

typedef enum
{
  UART_TX1PS_NO = 0x0 ,        //脉宽PWM脉冲选择:非调制输出
	UART_TX1PS_T16N1Out0 = 0x1 , //脉宽PWM脉冲选择:T16N1OUT0
	UART_TX1PS_T16N1Out1 = 0x2 , //脉宽PWM脉冲选择:T16N1OUT1
	UART_TX1PS_BUZ = 0x3 ,       //脉宽PWM脉冲选择:BUZ，需要固定38K输出
}UART_TYPE_TX1PS;


/*调制波形输出管脚配置*/
typedef enum
{
	UART_TYPE_E0TX0 = 0,          //E0TX0管脚输出
	UART_TYPE_T16N1OUT0 = 1,      //T16N1OUT0管脚输出
	UART_TYPE_T16N1OUT1 = 2,      //T16N1OUT1管脚输出
	UART_TYPE_TX1BUZ = 3,         //BUZ管脚输出
}UART_TYPE_TX1Sx;

typedef enum
{
	UART_TYPE_TXD0 = 0,           //TXD0管脚输出
	UART_TYPE_T16N0OUT0 = 1,      //T16N0OUT0管脚输出
	UART_TYPE_T16N0OUT1 = 2,      //T16N0OUT1管脚输出
	UART_TYPE_TX0BUZ = 3,         //BUZ管脚输出
}UART_TYPE_TX0Sx;

/********************* GPIO模块函数声明 *********************/
void GPIO_Init(GPIO_Pin Pin, GPIO_InitSettingType *InitSet);
uint32_t GPIO_ReadBit(GPIO_Pin Pin);
void GPIO_WriteBit(GPIO_Pin Pin, uint32_t value);
void GPIO_SetBit(GPIO_Pin Pin);
void GPIO_ResetBit(GPIO_Pin Pin);
void GPIO_ToggleBit(GPIO_Pin Pin);
void PINT_Init(GPIO_Pin Pin, PINT_InitSettingType *InitSet);
void PINT_ClearITFlag(GPIO_Pin Pin);
uint32_t PINT_GetITFlag(GPIO_Pin Pin);
void KINT_Init(GPIO_Pin Pin, KINT_InitSettingType *InitSet);
void KINT_ClearITFlag(GPIO_Pin Pin);
uint32_t KINT_GetITFlag(GPIO_Pin Pin);
void GPIO_TX0Config(UART_TYPE_TXPLV Plv,UART_TYPE_TX0PS Ps,UART_TYPE_TX0Sx TX0Sx,TYPE_FUNCEN NewState);
void GPIO_TX1Config(UART_TYPE_TXPLV Plv,UART_TYPE_TX1PS Ps,UART_TYPE_TX1Sx TX1Sx,TYPE_FUNCEN NewState);

#endif

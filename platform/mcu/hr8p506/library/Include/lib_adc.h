/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_adc.h
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  ADC模块库函数头文件
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#ifndef __LIBADC_H__
#define __LIBADC_H__

#include "HR8P506.h"
#include "type.h"

/* 内部参考电压选择 */
typedef enum {
	ADC_VREF_SEL_1_8 = 0,	/* 内部参考电压1.8v*/
	ADC_VREF_SEL_2_6 = 1,	/* 内部参考电压2.6v*/
	ADC_VREF_SEL_MAX = 1,
}ADC_TYPE_VREF_SEL;

/* CHOP_CLK输入选择*/
typedef enum {
	ADC_CHOP_CLK_SEL_1K = 0,	/* 1K */
	ADC_CHOP_CLK_SEL_2K = 1,	/* 2K */
	ADC_CHOP_CLK_SEL_4K = 2,	/* 4K */
	ADC_CHOP_CLK_SEL_8K = 3,	/* 8K */
	ADC_CHOP_CLK_SEL_16K = 4,	/* 16K */
	ADC_CHOP_CLK_SEL_32K = 5,	/* 32K */
	ADC_CHOP_CLK_SEL_MAX = 5,
}ADC_TYPE_CHOP_CLK_SEL;

/* 输出滤波时钟选择 */
typedef enum {
	ADC_FILTERCLK_SEL_32K = 0,		/* 输出滤波时钟 32K*/
	ADC_FILTERCLK_SEL_64K = 1,		/* 输出滤波时钟 64K*/
	ADC_FILTERCLK_SEL_128K = 2,	/* 输出滤波时钟 128K*/
	ADC_FILTERCLK_SEL_256K = 3,	/* 输出滤波时钟 256K*/
	ADC_FILTERCLK_SEL_MAX = 3,
}ADC_TYPE_FILTERCLK_SEL;

/* CHOP CLK 时钟源预分频选择*/
typedef enum {
	ADC_CHOP_CLK_DIV_1_1 = 0,		/* 关闭 */
	ADC_CHOP_CLK_DIV_1_4 =1,		/* 1:4 */
	ADC_CHOP_CLK_DIV_1_8 =2,		/* 1:8 */
	ADC_CHOP_CLK_DIV_1_16 =3,		/* 1:16 */
	ADC_CHOP_CLK_DIV_1_32 =4,		/* 1:32 */
	ADC_CHOP_CLK_DIV_1_64 =5,		/* 1:64 */
	ADC_CHOP_CLK_DIV_1_128 =6,		/* 1:128 */
	ADC_CHOP_CLK_DIV_1_256 =7,		/* 1:256 */
	ADC_CHOP_CLK_DIV_MAX = 7,
}ADC_TYPE_CHOP_CLK_DIV;

typedef FuncState ADC_TYPE_CHOP_EN1, ADC_TYPE_CHOP_EN;
typedef FuncState ADC_TYPE_VREF_EN,ADC_TYPE_IREF_EN;
typedef FuncState ADC_TYPE_EN,ADC_TYPE_ACP_EN;

/* A/D时钟源预分频选择 */
typedef enum {
	ADC_CLKDIV_1_1 = 0,	/* 1:1 */
	ADC_CLKDIV_1_2 = 1,	/* 1:2 */
	ADC_CLKDIV_1_4 = 2,	/* 1:4 */
	ADC_CLKDIV_1_8 = 3,	/* 1:8 */
	ADC_CLKDIV_1_16 = 4,	/* 1:16 */
	ADC_CLKDIV_1_32 = 5,	/* 1:32 */
	ADC_CLKDIV_1_64 = 6,	/* 1:64 */
	ADC_CLKDIV_1_256 = 7,	/* 1:256 */
	ADC_CLKDIV_MAX = 7,
}ADC_TYPE_CLKDIV;

/* A/D时钟源选择 */
typedef enum {
	ADC_CLKS_PCLK = 0,		/* PCLK */
	ADC_CLKS_LRC= 1,	/* LRC(32KHz) */
	ADC_CLKS_MAX = 1,
}ADC_TYPE_CLKS;

/* A/D正向参考电压选择 */
typedef enum {
	ADC_VREFP_VDD = 0,		/* 选择AD的工作电压 */
	ADC_VREFP_IO = 1,		/* 选择内部参考电压 AVREFP,端口复用作普通I/O端口*/
	ADC_VREFP_VREF = 2,	/* 选择内部参考电压 AVREFP,端口输出内部参考电压VREF*/
	ADC_VREFP_AVREFP = 3,	/* 外部参考电压AVREFP,电压不能高于AD的工作电压 */
	ADC_VREFP_MAX = 3,
}ADC_TYPE_VREFP;

/* A/D负向参考电压选择*/
typedef enum {
	ADC_VREFN_VSS = 0,		/* 内部地电压VSS */
	ADC_VREFN_AVREFN = 1,	/* 外部参考电压AVREFN */
	ADC_VREFN_MAX = 2,
}ADC_TYPE_VREFN;

/* A/D采样模式选择*/
typedef enum {
	ADC_SMPS_SOFT = 0,		/* 软件控制 */
	ADC_SMPS_HARD = 1,	/* 硬件控制 */
	ADC_SMPS_MAX = 1,
}ADC_TYPE_SMPS;

/* A/D采样软件控制位*/
typedef enum {
	ADC_SMPON_STOP = 0,	/* 关闭AD采样 */
	ADC_SMPON_START = 1,	/* 启动AD采样 */
	ADC_SMPON_MAX = 1,
}ADC_TYPE_SMPON;

/* A/D采样时间选择 (硬件控制有效)*/
typedef enum {
	ADC_ST_MAX = 31,	/* 容许的最大采样时间 */
}ADC_TYPE_ST;
typedef FuncState ADC_TYPE_VRBUF_EN,ADC_TYPE_HS_EN,ADC_TYPE_VCMBUF_EN,ADC_TYPE_VCMBUF_HS_EN;

/* 自动比较低阀值 选择*/
typedef enum {
	ADC_ACP_MIN_MIN = 0x0,		/* 容许的最小低阀值*/
	ADC_ACP_MIN_MAX = 0xfff,	/* 容许的最大低阀值*/
}ADC_TYPE_ACP_MIN;

/* 自动比较高阀值 选择*/
typedef enum {
	ADC_ACP_MAX_MIN = 0x0,	/* 容许的最小高阀值*/
	ADC_ACP_MAX_MAX = 0xfff,	/* 容许的最大高阀值*/
}ADC_TYPE_ACP_MAX;
#if 0
/* A/D转换速度控制位 */
typedef enum {
	ADC_SPEED_LOW = 0,		/* 低速 */
	ADC_SPEED_HIGH = 1,		/* 高速 */
	ADC_SPEED_MAX = 1,
}ADC_TYPE_SPEED;
#endif
/* A/D模拟通道选择位 */
typedef enum {
	ADC_CHS_AIN0 = 0,		/* 通道 0 */
	ADC_CHS_AIN1 = 1,		/* 通道 1 */
	ADC_CHS_AIN2 = 2,		/* 通道 2 */
	ADC_CHS_AIN3 = 3,		/* 通道 3 */
	ADC_CHS_AIN4 = 4,		/* 通道 4 */
	ADC_CHS_AIN5 = 5,		/* 通道 5 */
	ADC_CHS_AIN6 = 6,		/* 通道 6 */
	ADC_CHS_AIN7 = 7,		/* 通道 7 */
	ADC_CHS_AIN8 = 8,		/* 通道 8 */
	ADC_CHS_AIN9 = 9,		/* 通道 9 */
	ADC_CHS_AIN10 = 10,	/* 通道 10 */
	ADC_CHS_AIN11 = 11,	/* 通道 11 */
	ADC_CHS_AIN12 = 12,	/* 通道 12 */
	ADC_CHS_AIN13 = 13,	/* 通道 13 */
	ADC_CHS_AIN14 = 14,	/* 通道 14 */
	ADC_CHS_AIN15 = 15,	/* 通道 15 */
	ADC_CHS_OC = 16,		/*温度传感通道*/
	ADC_CHS_MAX = 16,
}ADC_TYPE_CHS;

typedef FuncState ADC_TYPE_VDD5_FLAG_EN;

typedef FuncState ADC_TYPE_INTR_EN,ADC_TYPE_ACP_MIN_INTR_EN,ADC_TYPE_ACP_MAX_INTR_EN,ADC_TYPE_ACP_OVER_INTR_EN;

/* 每次自动转换比较溢出时间选择 */
typedef enum {
	ADC_ACPC_OVFL_TIME_MAX = 0x9c3,	/* 容许的最大溢出时间 */
}ADC_TYPE_ACPC_OVFL_TIME;

/* 自动转换比较次数 */
typedef enum {
	ADC_ACPC_TIMES_1 = 0,	/* 1次 */
	ADC_ACPC_TIMES_2 = 1,	/* 2次 */
	ADC_ACPC_TIMES_4 = 2,	/* 4次 */
	ADC_ACPC_TIMES_8 = 3,	/* 8次 */
	ADC_ACPC_TIMES_MAX = 3,
}ADC_TYPE_ACPC_TIMES;

/* ACP溢出时钟的时钟源 */
typedef enum {
	ADC_ACPC_CLKS_PCLK = 0,		/* PCLK */
	ADC_ACPC_CLKS_LRC = 1,			/* LRC(32KHz) */
	ADC_ACPC_CLKS_MAX = 1,
}ADC_TYPE_ACPC_CLKS;


typedef enum {
	ADC_IF			 = 0x01,
	ADC_IF_ACPMIN = 0x02,
	ADC_IF_ACPMAX =0x04,
	ADC_IF_ACPOVER = 0x08,
}ADC_TYPE_IF;

/* ADC IE 状态 */
typedef enum {
	ADC_IE	= 0x01,
	ADC_IE_ACPMIN = 0x02,
	ADC_IE_ACPMAX =0x04,
	ADC_IE_ACPOVER = 0x08,
}ADC_TYPE_IE;

/* ADC 初始化结构体 */
typedef struct {	
	ADC_TYPE_CLKS CLKS;				       /* ADC时钟选择 */	
	ADC_TYPE_CLKDIV CLKDIV;			     /* ADC 预分频 */
	ADC_TYPE_VREF_SEL VREF_SEL;		 	 /*参考电压选择 */
	ADC_TYPE_VREFP VREFP;				     /* 参考电压类型选择 */
	ADC_TYPE_VREFN VREFN;
	ADC_TYPE_CHS CHS;					       /* 模拟通道选择 */
	ADC_TYPE_SMPS SMPS;				       /* 控制模式选择 */
	uint8_t ST;							         /* 硬件采样时间 (当控制模式为硬件时设置) 0~31*/	
	ADC_TYPE_VDD5_FLAG_EN VDD5_EN;	 /* VDD使能选择 */
}ADC_InitStruType;

/* 自动比较功能初始化结构体*/
typedef struct {
	ADC_TYPE_ACP_EN ACP_EN;				  	/* 自动比较功能使能位*/
	uint16_t ACPC_OVER_TIME;					/* 单次自动比较溢出时间(即使不想设置请设置成0) 0~0x9c3 */
	ADC_TYPE_ACPC_TIMES ACPC_TIMES;		/* 单次自动比较次数(优先级高于溢出时间) */
	uint16_t ACPC_MIN_TARGET;					/* 平均值最低阀值 (设置0xfff关闭) 0~0xfff */
	uint16_t ACPC_MAX_TARGET;					/* 平均值最高阀值 (设置0x0关闭) 	0~0xfff */
}ADC_ACP_TypeDef;

#define ADC_Enable() (ADC->CON0.EN = 1)
#define ADC_Disable() (ADC->CON0.EN = 0)

/* 自动比较功能使能*/
#define ADC_ACP_Enable() (ADC->CON0.ACP_EN = 1)
#define ADC_ACP_Disable() (ADC->CON0.ACP_EN = 0)

#define ADC_IE_Enable() (ADC ->IE.IE = 1)
#define ADC_IE_Disable() (ADC ->IE.IE = 0)

#define ADC_ACPMINIE_Enable() (ADC ->IE.ACPMINIE = 1)
#define ADC_ACPMINIE_Disable() (ADC ->IE.ACPMINIE = 0)

#define ADC_ACPMAXIE_Enable() (ADC ->IE.ACPMAXIE = 1)
#define ADC_ACPMAXIE_Disable() (ADC ->IE.ACPMAXIE = 0)

#define ADC_ACPOVIE_Enable() (ADC ->IE.ACPOVIE = 1)
#define ADC_ACPOVIE_Disable() (ADC ->IE.ACPOVIE = 0)

/********************* ADC模块函数声明 *********************/
ErrorStatus ADC_Init(ADC_InitStruType * ADC_InitStruct);
ErrorStatus ADC_ACPConfig(ADC_ACP_TypeDef *ADC_ACP_InitStruct);
ErrorStatus ADC_Start(void);
ErrorStatus ADC_SoftStart(void);
ErrorStatus ADC_SoftStop(void);
uint16_t ADC_GetConvValue(void);
FlagStatus ADC_GetConvStatus(void);
uint16_t ADC_GetACPMeanValue(void);
ITStatus ADC_GetIFStatus(ADC_TYPE_IF IFName);
ITStatus ADC_GetIEStatus(ADC_TYPE_IE IEName);
ErrorStatus ADC_ClearIFStatus(ADC_TYPE_IF IFName);
void ADC_Reset(void);

#endif

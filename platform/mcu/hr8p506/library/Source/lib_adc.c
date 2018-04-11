/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_adc.c
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  ADC模块库函数
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#include "lib_adc.h"

/***************************************************************
  函数名：ADC_Init
  描  述：ADC功能初始化函数
  输入值：ADC_InitStruct 初始化结构体
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus ADC_Init(ADC_InitStruType * ADC_InitStruct)
{
	if(ADC_InitStruct->CLKS > ADC_CLKS_MAX
			||ADC_InitStruct->CLKDIV > ADC_CLKDIV_MAX
			||ADC_InitStruct->VREF_SEL > ADC_VREF_SEL_MAX
			||ADC_InitStruct->VREFP > ADC_VREFP_MAX
			||ADC_InitStruct->VREFN > ADC_VREFN_MAX
			||ADC_InitStruct->CHS > ADC_CHS_MAX
			||ADC_InitStruct->SMPS > ADC_SMPS_MAX
			||ADC_InitStruct->ST > ADC_ST_MAX
			||ADC_InitStruct->VDD5_EN > ENABLE) {
		return ERROR;
	}

	ADC_Reset();

	ADC->VREFCON.VREF_EN = ENABLE;
	ADC->VREFCON.IREF_EN = ENABLE;
	ADC->CON1.VCMBUF_EN = ENABLE;	
	ADC ->CON1.VCMBUF_HS = ENABLE;
	ADC->CON1.HSEN = ENABLE;

	ADC->CHS.CHS = ADC_InitStruct->CHS;
	ADC->CON1.CLKS = ADC_InitStruct->CLKS;
	ADC->CON1.CLKDIV = ADC_InitStruct->CLKDIV;
	ADC->CON1.VREFP = ADC_InitStruct->VREFP ;
	ADC->CON1.VREFN = ADC_InitStruct->VREFN;
	ADC->VREFCON.VREF_SEL=ADC_InitStruct->VREF_SEL;
	if((ADC_InitStruct->VREFP == ADC_VREFP_IO)||(ADC_InitStruct->VREFP == ADC_VREFP_VREF)) 
		ADC->CON1.VRBUF_EN = ENABLE;
	else
		ADC->CON1.VRBUF_EN = DISABLE;


	ADC->CHS.VDD5_FLAG_EN = 0;
	ADC->CON1.SMPS = ADC_InitStruct->SMPS;
	ADC->CON1.ST = ADC_InitStruct->ST;
	ADC->CON0.EN = ENABLE;
	return SUCCESS;
}

/***************************************************************
  函数名：ADC_ACPConfig
  描  述：ADC 自动比较功能初始化
  输入值：ADC_ACP_InitStruct 自动比较功能初始化结构体
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus ADC_ACPConfig(ADC_ACP_TypeDef *ADC_ACP_InitStruct)
{

	if(ADC_ACP_InitStruct->ACPC_OVER_TIME > ADC_ACPC_OVFL_TIME_MAX
			||ADC_ACP_InitStruct->ACPC_TIMES > ADC_ACPC_TIMES_MAX
			||ADC_ACP_InitStruct->ACPC_MIN_TARGET > ADC_ACP_MIN_MAX
			||ADC_ACP_InitStruct->ACPC_MAX_TARGET > ADC_ACP_MAX_MAX) {
		return ERROR;
	}

	if(ADC_ACP_InitStruct ->ACP_EN == ENABLE) {
		if(ADC->CON1.CLKS == ADC_CLKS_PCLK) {
			ADC->ACPC.CLKS = ADC_ACPC_CLKS_PCLK;
		}
		else {
			ADC->ACPC.CLKS = ADC_ACPC_CLKS_LRC;
		}

		ADC->ACPC.OVFL_TIME = ADC_ACP_InitStruct ->ACPC_OVER_TIME;
		ADC->ACPC.TIMES = ADC_ACP_InitStruct->ACPC_TIMES;
		ADC->IE.ACPOVIE = ENABLE;

		/* 假设用户将高阀值设置成0(最小值)，我们可以认为其想关闭该功能 */
		if(ADC_ACP_InitStruct ->ACPC_MAX_TARGET ==0x0) {
			ADC->IE.ACPMAXIE = DISABLE;
		}
		else {
			ADC->ACPCMP.CMP_MAX =ADC_ACP_InitStruct ->ACPC_MAX_TARGET;
			ADC->IE.ACPMAXIE = ENABLE;
		}

		/* 假设用户将低阀值设置成0xfff(最大值)，我们可以认为其想关闭该功能 */
		if(ADC_ACP_InitStruct ->ACPC_MIN_TARGET == 0xfff) {
			ADC->IE.ACPMINIE = DISABLE;
		}
		else {
			ADC->ACPCMP.CMP_MIN =ADC_ACP_InitStruct ->ACPC_MIN_TARGET;
			ADC->IE.ACPMINIE = ENABLE;
		}

		ADC->CON0.ACP_EN = ENABLE;

	}
	else {
		ADC->CON0.ACP_EN = DISABLE;
	}
	return SUCCESS;
}

/***************************************************************
  函数名：ADC_Start
  描  述：ADC 启动函数
  输入值：无
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus ADC_Start(void)
{
	if(ADC->CON1.SMPS == ADC_SMPS_SOFT) {
		return ERROR;
	}
	ADC->CON0.TRIG = SET;/* 40001004 BIT1*/
	return SUCCESS;
}

/***************************************************************
  函数名：ADC_SampStart
  描  述：ADC 采样软件控制-启动函数
  输入值：无
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus ADC_SoftStart(void)
{
	if(ADC->CON1.SMPS == ADC_SMPS_HARD)
	{
		return ERROR;
	}
	ADC ->CON1.SMPON = SET;/* 40001008 BIT13*/
	return SUCCESS;
}

/***************************************************************
  函数名：ADC_SampStop
  描  述：ADC 采样软件控制-停止函数
  输入值：无
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus ADC_SoftStop(void)
{
	if(ADC->CON1.SMPS == ADC_SMPS_HARD)
	{
		return ERROR;
	}
	ADC ->CON1.SMPON = RESET;/* 40001008 BIT13*/
	return SUCCESS;
}

/***************************************************************
  函数名：ADC_GetConvValue
  描  述：ADC 获得采样数据函数
  输入值：无
  输出值：无
  返回值：采样数据
 ***************************************************************/
uint16_t ADC_GetConvValue(void)
{
	return  ((uint16_t)ADC->DR.DR);/* 40001000 BIT15-0*/
}

/***************************************************************
  函数名：ADC_GetConvStatus
  描  述：ADC 获得此时状态
  输入值：无
  输出值：无
  返回值：SET  正在进行转换
  RESET	未进行转换或者转换完成
 ***************************************************************/
FlagStatus ADC_GetConvStatus(void)
{
	if(ADC->CON0.TRIG == RESET) {/* 40001004 BIT1*/
		return RESET;
	}
	return SET;
}

/***************************************************************
  函数名：ADC_GetACPMeanValue
  描  述：ADC 获得单次自动比较平均值
  输入值：无
  输出值：无
  返回值：采样数据
 ***************************************************************/
uint16_t ADC_GetACPMeanValue(void)
{
	return  ((uint16_t)ADC->ACPMEAN.MEAN_DATA);
}

/***************************************************************
  函数名：ADC_GetIFStatus
  描  述：ADC 获得特定类型中断情况
  输入值：IFName 中断类型选择
  ADC_IF			ADC中断
  ADC_IF_ACPMIN	自动转换低阀值超出中断
  ADC_IF_ACPMAX	自动转换高阀值超出中断
  ADC_IF_ACPOVER	自动转换溢出中断
  输出值：无
  返回值：中断标志与否
 ***************************************************************/
ITStatus ADC_GetIFStatus(ADC_TYPE_IF IFName)
{
	if(ADC->IF.Word & IFName) {
		return SET;
	}
	return RESET;
}

/***************************************************************
  函数名：ADC_GetIEStatus
  描  述：ADC 获得特定类型中断情况
  输入值：IFName 中断类型选择
  ADC_IE ADC中断
  ADC_IE_ACPMIN	自动转换低阀值超出中断
  ADC_IE_ACPMAX	自动转换高阀值超出中断
  ADC_IE_ACPOVER	自动转换溢出中断
  输出值：无
  返回值：中断标志与否
 ***************************************************************/
ITStatus ADC_GetIEStatus(ADC_TYPE_IE IEName)
{
	if(ADC->IE.Word & IEName) {
		return SET;
	}
	return RESET;
}

/***************************************************************
  函数名：ADC_ClearIFStatus
  描  述：ADC 清除特定类型中断
  输入值：IFName 中断类型选择
  ADC_IF			ADC中断
  ADC_IF_ACPMIN	自动转换低阀值超出中断
  ADC_IF_ACPMAX	自动转换高阀值超出中断
  ADC_IF_ACPOVER	自动转换溢出中断
  输出值：无
  返回值：SUCCESS 成功
  ERROR 失败
 ***************************************************************/
ErrorStatus ADC_ClearIFStatus(ADC_TYPE_IF IFName)
{
	ADC->IF.Word = (uint32_t)IFName;
	return SUCCESS;
}

/***************************************************************
  函数名：ADC_Reset
  描  述：ADC复位
  输入值：无
  输出值：无
  返回值：无
 ***************************************************************/
void ADC_Reset(void)
{
	ADC->CON0.Word = 0x00000000;
	ADC->CON1.Word = 0x00041000;
	ADC->CHS.Word = 0x00000100;
	ADC->IE.Word = 0x00000000; 
	ADC->IF.Word = 0x00000000;
	ADC->ACPC.Word = 0x00000000;
	ADC->ACPCMP.Word = 0x0fff0000;
	ADC->VREFCON.Word = 0x00000000; 
}

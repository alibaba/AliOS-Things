/**
  ******************************************************************************
  * @file    fm33a0xx_lptim.c
  * @author  FM33a0xx Application Team
  * @version V2.0.2
  * @date    
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of....:
  *
*/

/* Includes ------------------------------------------------------------------*/
#include "fm33A0xx_lptim.h" 


/** @addtogroup fm33a0xx_StdPeriph_Driver
  * @{
  */

/** @defgroup LPTIM 
  * @brief LPTIM driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


//Code_Start
//2018-04-02-13-22-48
//FM33A0XX_Driver_Gen_V1.4

/********************************
LPTIM外部Trigger滤波使能函数
功能:外部Trigger滤波使能
输入:使能状态

********************************/
void LPTIM_LPTCFG_FLTEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LPTIM->LPTCFG &= ~(LPTIM_LPTCFG_FLTEN_Msk);
	}
	else
	{
		LPTIM->LPTCFG |= (LPTIM_LPTCFG_FLTEN_Msk);
	}
}
/********************************
读取LPTIM外部Trigger滤波使能状态函数
功能:读取外部Trigger滤波使能状态
输出:使能状态

********************************/
FunState LPTIM_LPTCFG_FLTEN_Getable(void)
{
	if (LPTIM->LPTCFG & (LPTIM_LPTCFG_FLTEN_Msk))
	{
		return DISABLE;
	}
	else
	{
		return ENABLE;
	}
}

/********************************
LPTIM计数时钟分频选择函数
功能:计数时钟分频选择
输入:分频数值

********************************/
void LPTIM_LPTCFG_DIVSEL_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LPTIM->LPTCFG;
	tmpreg &= ~(LPTIM_LPTCFG_DIVSEL_Msk);
	tmpreg |= (SetValue & LPTIM_LPTCFG_DIVSEL_Msk);
	LPTIM->LPTCFG = tmpreg;
}
/********************************
读取LPTIM计数时钟分频数值函数
功能:读取计数时钟分频数值
输出:分频数值

********************************/
uint32_t LPTIM_LPTCFG_DIVSEL_Get(void)
{
	return (LPTIM->LPTCFG & LPTIM_LPTCFG_DIVSEL_Msk);
}

/********************************
LPTIM计数时钟源选择函数
功能:计数时钟源选择
输入:时钟源代号

********************************/
void LPTIM_LPTCFG_CLKSEL_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LPTIM->LPTCFG;
	tmpreg &= ~(LPTIM_LPTCFG_CLKSEL_Msk);
	tmpreg |= (SetValue & LPTIM_LPTCFG_CLKSEL_Msk);
	LPTIM->LPTCFG = tmpreg;
}
/********************************
读取LPTIM计数时钟源函数
功能:读取计数时钟源选择
输出:读取时钟源代号

********************************/
uint32_t LPTIM_LPTCFG_CLKSEL_Get(void)
{
	return (LPTIM->LPTCFG & LPTIM_LPTCFG_CLKSEL_Msk);
}

/********************************
LPTIM输入边沿选择函数
功能:输入边沿选择
输入:输入边沿选择代号
	0：LPTIN的上升沿计数
	1：LPTIN的下降沿计数
********************************/
void LPTIM_LPTCFG_EDGESEL_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LPTIM->LPTCFG;
	tmpreg &= ~(LPTIM_LPTCFG_EDGESEL_Msk);
	tmpreg |= (SetValue & LPTIM_LPTCFG_EDGESEL_Msk);
	LPTIM->LPTCFG = tmpreg;
}
/********************************
读取LPTIM输入边沿选择函数
功能:读取LPTIM输入边沿选择
输出:读取LPTIM输入边沿
	0：LPTIN的上升沿计数
	1：LPTIN的下降沿计数
********************************/
uint32_t LPTIM_LPTCFG_EDGESEL_Get(void)
{
	return (LPTIM->LPTCFG & LPTIM_LPTCFG_EDGESEL_Msk);
}

/********************************
LPTIM外部触发边沿选择函数
功能:外部触发边沿选择
输入:外部触发边沿选择代号

********************************/
void LPTIM_LPTCFG_TRIGCFG_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LPTIM->LPTCFG;
	tmpreg &= ~(LPTIM_LPTCFG_TRIGCFG_Msk);
	tmpreg |= (SetValue & LPTIM_LPTCFG_TRIGCFG_Msk);
	LPTIM->LPTCFG = tmpreg;
}
/********************************
读取LPTIM外部触发边沿选择函数
功能:读取LPTIM外部触发边沿选择
输出:读取LPTIM外部触发边沿

********************************/
uint32_t LPTIM_LPTCFG_TRIGCFG_Get(void)
{
	return (LPTIM->LPTCFG & LPTIM_LPTCFG_TRIGCFG_Msk);
}

/* 输出波形极性选择 相关函数 */
void LPTIM_LPTCFG_POLARITY_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LPTIM->LPTCFG;
	tmpreg &= ~(LPTIM_LPTCFG_POLARITY_Msk);
	tmpreg |= (SetValue & LPTIM_LPTCFG_POLARITY_Msk);
	LPTIM->LPTCFG = tmpreg;
}

uint32_t LPTIM_LPTCFG_POLARITY_Get(void)
{
	return (LPTIM->LPTCFG & LPTIM_LPTCFG_POLARITY_Msk);
}

/********************************
LPTIM脉宽调制模式选择函数
功能:脉宽调制模式选择
输入:脉宽调制模式代号
0：周期方波输出模式
1：PWM输出模式
********************************/
void LPTIM_LPTCFG_PWM_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LPTIM->LPTCFG;
	tmpreg &= ~(LPTIM_LPTCFG_PWM_Msk);
	tmpreg |= (SetValue & LPTIM_LPTCFG_PWM_Msk);
	LPTIM->LPTCFG = tmpreg;
}
/********************************
读取LPTIM脉宽调制模式选择函数
功能:读取脉宽调制模式
输出:读取脉宽调制模式
0：周期方波输出模式
1：PWM输出模式
********************************/
uint32_t LPTIM_LPTCFG_PWM_Get(void)
{
	return (LPTIM->LPTCFG & LPTIM_LPTCFG_PWM_Msk);
}

/********************************
LPTIM计数模式选择函数
功能:计数模式选择
输入:计数模式选择代号
0：连续计数模式
1：单次计数模式
********************************/
void LPTIM_LPTCFG_MODE_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LPTIM->LPTCFG;
	tmpreg &= ~(LPTIM_LPTCFG_MODE_Msk);
	tmpreg |= (SetValue & LPTIM_LPTCFG_MODE_Msk);
	LPTIM->LPTCFG = tmpreg;
}

/********************************
读取LPTIM计数模式选择函数
功能:读取计数模式选择
输出:计数模式选择代号
0：连续计数模式
1：单次计数模式
********************************/
uint32_t LPTIM_LPTCFG_MODE_Get(void)
{
	return (LPTIM->LPTCFG & LPTIM_LPTCFG_MODE_Msk);
}

/********************************
LPTIM工作模式选择函数
功能:工作模式选择
输入:工作模式选择代号
********************************/
void LPTIM_LPTCFG_TMODE_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LPTIM->LPTCFG;
	tmpreg &= ~(LPTIM_LPTCFG_TMODE_Msk);
	tmpreg |= (SetValue & LPTIM_LPTCFG_TMODE_Msk);
	LPTIM->LPTCFG = tmpreg;
}
/********************************
读取LPTIM工作模式选择函数
功能:LPTIM工作模式选择
输出:工作模式选择代号
********************************/
uint32_t LPTIM_LPTCFG_TMODE_Get(void)
{
	return (LPTIM->LPTCFG & LPTIM_LPTCFG_TMODE_Msk);
}
/********************************
LPTIM写计数值函数
功能:写入计数值
输入:计数值
********************************/
void LPTIM_LPTCNT_Write(uint32_t SetValue)
{
	LPTIM->LPTCNT = (SetValue & LPTIM_LPTCNT_CNT16_Msk);
}

/********************************
读取LPTIM计数值函数
功能:读取计数值
输出:计数值
********************************/
uint32_t LPTIM_LPTCNT_Read(void)
{
	return (LPTIM->LPTCNT & LPTIM_LPTCNT_CNT16_Msk);
}
/********************************
LPTIM写比较值函数
功能:写入比较值
输入:比较值
********************************/
void LPTIM_LPTCMP_Write(uint32_t SetValue)
{
	LPTIM->LPTCMP = (SetValue & LPTIM_LPTCMP_COMPARE_REG_Msk);
}
/********************************
读取LPTIM比较值函数
功能:读取比较值
输出:比较值
********************************/
uint32_t LPTIM_LPTCMP_Read(void)
{
	return (LPTIM->LPTCMP & LPTIM_LPTCMP_COMPARE_REG_Msk);
}
/********************************
LPTIM写目标值函数
功能:写入目标值
输入:目标值
********************************/
void LPTIM_LPTTARGET_Write(uint32_t SetValue)
{
	LPTIM->LPTTARGET = (SetValue & LPTIM_LPTTARGET_TARGET_REG_Msk);
}
/********************************
读取LPTIM目标值函数
功能:读取目标值
输出:目标值
********************************/
uint32_t LPTIM_LPTTARGET_Read(void)
{
	return (LPTIM->LPTTARGET & LPTIM_LPTTARGET_TARGET_REG_Msk);
}

/********************************
LPTIM外部触发中断函数
功能:外部触发中断设置
输入:外部触发中断代号
1：外部触发到来中断使能
0：外部触发到来中断禁止
********************************/
void LPTIM_LPTIE_TRIGIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LPTIM->LPTIE |= (LPTIM_LPTIE_TRIGIE_Msk);
	}
	else
	{
		LPTIM->LPTIE &= ~(LPTIM_LPTIE_TRIGIE_Msk);
	}
}
/********************************
读取LPTIM外部触发中断函数
功能:读取外部触发中断设置
输出:外部触发中断代号
1：外部触发到来中断使能
0：外部触发到来中断禁止
********************************/
FunState LPTIM_LPTIE_TRIGIE_Getable(void)
{
	if (LPTIM->LPTIE & (LPTIM_LPTIE_TRIGIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
LPTIM计数器溢出中断函数
功能:计数器溢出中断设置
输入:计数器溢出中断代号
1：计数器溢出中断使能
0：计数器溢出中断禁止
********************************/
void LPTIM_LPTIE_OVIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LPTIM->LPTIE |= (LPTIM_LPTIE_OVIE_Msk);
	}
	else
	{
		LPTIM->LPTIE &= ~(LPTIM_LPTIE_OVIE_Msk);
	}
}
/********************************
读取LPTIM计数器溢出中断函数
功能:读取计数器溢出中断设置
输出:计数器溢出中断代号
1：计数器溢出中断使能
0：计数器溢出中断禁止
********************************/
FunState LPTIM_LPTIE_OVIE_Getable(void)
{
	if (LPTIM->LPTIE & (LPTIM_LPTIE_OVIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
LPTIM比较值匹配中断函数
功能:比较值匹配中断设置
输入:比较值匹配中断代号
1：计数器值和比较值匹配中断使能
0：计数器值和比较值匹配中断禁止
********************************/
void LPTIM_LPTIE_COMPIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LPTIM->LPTIE |= (LPTIM_LPTIE_COMPIE_Msk);
	}
	else
	{
		LPTIM->LPTIE &= ~(LPTIM_LPTIE_COMPIE_Msk);
	}
}
/********************************
读取LPTIM比较值匹配中断函数
功能:读取比较值匹配中断设置
输出:比较值匹配中断代号
1：计数器值和比较值匹配中断使能
0：计数器值和比较值匹配中断禁止
********************************/
FunState LPTIM_LPTIE_COMPIE_Getable(void)
{
	if (LPTIM->LPTIE & (LPTIM_LPTIE_COMPIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
清除LPTIM外部触发标志函数
功能:清除LPTIM外部触发标志
********************************/
void LPTIM_LPTIF_TRIGIF_Clr(void)
{
	LPTIM->LPTIF = LPTIM_LPTIF_TRIGIF_Msk;
}
/********************************
检测LPTIM外部触发标志函数
功能:检测LPTIM外部触发标志
输出：LPTIM外部触发标志
********************************/
FlagStatus LPTIM_LPTIF_TRIGIF_Chk(void)
{
	if (LPTIM->LPTIF & LPTIM_LPTIF_TRIGIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
清除LPTIM计数器溢出标志函数
功能:清除LPTIM计数器溢出标志
********************************/
void LPTIM_LPTIF_OVIF_Clr(void)
{
	LPTIM->LPTIF = LPTIM_LPTIF_OVIF_Msk;
}
/********************************
检测LPTIM计数器溢出标志函数
功能:检测LPTIM计数器溢出标志
输出：LPTIM计数器溢出标志
********************************/
FlagStatus LPTIM_LPTIF_OVIF_Chk(void)
{
	if (LPTIM->LPTIF & LPTIM_LPTIF_OVIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
清除LPTIM比较值匹配标志函数
功能:清除LPTIM比较值匹配标志
********************************/
void LPTIM_LPTIF_COMPIF_Clr(void)
{
	LPTIM->LPTIF = LPTIM_LPTIF_COMPIF_Msk;
}
/********************************
检测LPTIM比较值匹配标志函数
功能:检测LPTIM比较值匹配标志
输出：LPTIM比较值匹配标志
********************************/
FlagStatus LPTIM_LPTIF_COMPIF_Chk(void)
{
	if (LPTIM->LPTIF & LPTIM_LPTIF_COMPIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
LPTIM计数使能函数
功能:LPTIM计数使能
输入：LPTIM计数使能代号
1：使能计数器计数
0：禁止计数器计数
********************************/
void LPTIM_LPTCTRL_LPTEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LPTIM->LPTCTRL |= (LPTIM_LPTCTRL_LPTEN_Msk);
	}
	else
	{
		LPTIM->LPTCTRL &= ~(LPTIM_LPTCTRL_LPTEN_Msk);
	}
}
/********************************
读取LPTIM计数使能函数
功能:读取LPTIM计数使能
输出：LPTIM计数使能代号
1：使能计数器计数
0：禁止计数器计数
********************************/
FunState LPTIM_LPTCTRL_LPTEN_Getable(void)
{
	if (LPTIM->LPTCTRL & (LPTIM_LPTCTRL_LPTEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
LPTIM所有寄存器清零函数
功能:LPTIM所有寄存器清零
********************************/
void LPTIM_Deinit(void)
{
	LPTIM->LPTCFG = 0x00000000;
	//LPTIM->LPTCNT = ;
	LPTIM->LPTCMP = 0x00000000;
	LPTIM->LPTTARGET = 0x00000000;
	LPTIM->LPTIE = 0x00000000;
	//LPTIM->LPTIF = ;
	LPTIM->LPTCTRL = 0x00000000;
}
//Code_End

/********************************
LPTIM初始化函数
功能:LPTIM初始化
********************************/
void LPTIM_Init(LPTIM_InitTypeDef* para)
{	
	LPTIM_LPTCFG_FLTEN_Setable(para->LPTIM_FLTEN);
	LPTIM_LPTCFG_DIVSEL_Set(para->LPTIM_CLK_DIV);
	LPTIM_LPTCFG_CLKSEL_Set(para->LPTIM_CLK_SEL);
	LPTIM_LPTCFG_EDGESEL_Set(para->LPTIM_LPTIN_EDGE);
	LPTIM_LPTCFG_TRIGCFG_Set(para->LPTIM_TRIG_CFG);
	LPTIM_LPTCFG_POLARITY_Set(para->LPTIM_POLAR);
	LPTIM_LPTCFG_PWM_Set(para->LPTIM_PWM);
	LPTIM_LPTCFG_MODE_Set(para->LPTIM_MODE);
	LPTIM_LPTCFG_TMODE_Set(para->LPTIM_TMODE);

	LPTIM_LPTCMP_Write(para->LPTIM_compare_value);
	LPTIM_LPTTARGET_Write(para->LPTIM_target_value);
}


/******END OF FILE****/


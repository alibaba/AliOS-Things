/**
  ******************************************************************************
  * @file    fm33a0xx_btim.c
  * @author  FM33a0 Application Team
  * @version V2.0.2
  * @date    
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of....:
  *
*/

/* Includes ------------------------------------------------------------------*/
#include "fm33A0xx_btim.h"  


/** @addtogroup fm33a0_StdPeriph_Driver
  * @{
  */

/** @defgroup BTIM 
  * @brief BTIM driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup BTIM_Private_Functions
  * @{
  */ 

//Code_Start
//2018-04-02-13-20-21
//FM33A0XX_Driver_Gen_V1.4

/*********************************
basic timer高位计数器启动控制函数 
功能：BT高位计数器启停控制
输入: ENABLE 启动高位计数器
      DISABLE 停止高位计数器
      BTIM1/BTIM2  定时器名称
输出: 无
*********************************/
void BTIMx_BTCR1_CHEN_Setable(BTIMx_Type* BTIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		BTIMx->BTCR1 |= (BTIMx_BTCR1_CHEN_Msk);
	}
	else
	{
		BTIMx->BTCR1 &= ~(BTIMx_BTCR1_CHEN_Msk);
	}
}
/*********************************
basic timer高位计数器控制位状态获取函数 
功能：获取BT高位计数器启停控制状态
输入: BTIM1/BTIM2  定时器名称
输出: ENABLE 当前BT高位计数器已启动
      DISABLE 当前BT高位计数器已停止
*********************************/
FunState BTIMx_BTCR1_CHEN_Getable(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTCR1 & (BTIMx_BTCR1_CHEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
basic timer低位计数器启动控制函数 
功能：BT低位计数器启停控制
输入: ENABLE 启动低位计数器
      DISABLE 停止低位计数器
      BTIM1/BTIM2  定时器名称
输出: 无
*********************************/
void BTIMx_BTCR1_CLEN_Setable(BTIMx_Type* BTIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		BTIMx->BTCR1 |= (BTIMx_BTCR1_CLEN_Msk);
	}
	else
	{
		BTIMx->BTCR1 &= ~(BTIMx_BTCR1_CLEN_Msk);
	}
}
/*********************************
basic timer低位计数器控制位状态获取函数 
功能：获取BT低位计数器启停控制状态
输入: BTIM1/BTIM2  定时器名称
输出: ENABLE 当前BT低位计数器已启动
      DISABLE 当前BT低位计数器已停止
*********************************/
FunState BTIMx_BTCR1_CLEN_Getable(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTCR1 & (BTIMx_BTCR1_CLEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
basic timer工作模式配置函数 
功能：配置BT工作模式
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCR1_MODE_8BITS_TIM_CNT/BTIMx_BTCR1_MODE_16BITS_CAP 8位/16位定时器      
输出: 无
*********************************/
void BTIMx_BTCR1_MODE_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCR1;
	tmpreg &= ~(BTIMx_BTCR1_MODE_Msk);
	tmpreg |= (SetValue & BTIMx_BTCR1_MODE_Msk);
	BTIMx->BTCR1 = tmpreg;
}
/*********************************
basic timer获取工作模式配置函数 
功能：获取BT工作模式
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCR1_MODE_8BITS_TIM_CNT/BTIMx_BTCR1_MODE_16BITS_CAP 8位/16位定时器
*********************************/
uint32_t BTIMx_BTCR1_MODE_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCR1 & BTIMx_BTCR1_MODE_Msk);
}

/*********************************
basic timer工作模式配置函数 
功能：配置BT的计数沿或捕捉沿
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCR1_EDGESEL_POS/BTIMx_BTCR1_EDGESEL_NEG 计数沿或捕捉沿为上升沿/下降沿      
输出: 无
*********************************/
void BTIMx_BTCR1_EDGESEL_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCR1;
	tmpreg &= ~(BTIMx_BTCR1_EDGESEL_Msk);
	tmpreg |= (SetValue & BTIMx_BTCR1_EDGESEL_Msk);
	BTIMx->BTCR1 = tmpreg;
}
/*********************************
basic timer获取工作模式配置函数 
功能：获取BT的计数沿或捕捉沿具体方式
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCR1_EDGESEL_POS/BTIMx_BTCR1_EDGESEL_NEG 计数沿或捕捉沿为上升沿/下降沿 
*********************************/
uint32_t BTIMx_BTCR1_EDGESEL_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCR1 & BTIMx_BTCR1_EDGESEL_Msk);
}


/*********************************
basic timer工作模式配置函数 
功能：配置BT的捕捉模式控制（仅捕捉模式下有效）
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCR1_CAPMOD_PAUSE_PERIOD/BTIMx_BTCR1_CAPMOD_PAUSE_WIDTH 脉冲周期捕捉/脉冲宽度捕捉
输出: 无
*********************************/
void BTIMx_BTCR1_CAPMOD_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCR1;
	tmpreg &= ~(BTIMx_BTCR1_CAPMOD_Msk);
	tmpreg |= (SetValue & BTIMx_BTCR1_CAPMOD_Msk);
	BTIMx->BTCR1 = tmpreg;
}
/*********************************
basic timer获取工作模式配置函数 
功能：获取BT的捕捉模式控制（仅捕捉模式下有效）配置
输入: BTIM1/BTIM2  定时器名称    
输出: BTIMx_BTCR1_CAPMOD_PAUSE_PERIOD/BTIMx_BTCR1_CAPMOD_PAUSE_WIDTH 脉冲周期捕捉/脉冲宽度捕捉
*********************************/
uint32_t BTIMx_BTCR1_CAPMOD_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCR1 & BTIMx_BTCR1_CAPMOD_Msk);
}


/*********************************
basic timer工作模式配置函数 
功能：配置BT的带清零捕捉模式控制是否清零
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCR1_CAPCLR_CAP_CNT_NO_CLR/BTIMx_BTCR1_CAPCLR_CAP_CNT_CLR  捕捉后计数器不清零/捕捉后计数器清零    
输出: 无
*********************************/
void BTIMx_BTCR1_CAPCLR_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCR1;
	tmpreg &= ~(BTIMx_BTCR1_CAPCLR_Msk);
	tmpreg |= (SetValue & BTIMx_BTCR1_CAPCLR_Msk);
	BTIMx->BTCR1 = tmpreg;
}
/*********************************
basic timer获取工作模式配置函数 
功能：获取BT的捕捉模式控制清零配置
输入: BTIM1/BTIM2  定时器名称    
输出: BTIMx_BTCR1_CAPCLR_CAP_CNT_NO_CLR/BTIMx_BTCR1_CAPCLR_CAP_CNT_CLR  捕捉后计数器不清零/捕捉后计数器清零    
*********************************/
uint32_t BTIMx_BTCR1_CAPCLR_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCR1 & BTIMx_BTCR1_CAPCLR_Msk);
}

/*********************************
basic timer工作模式配置函数 
功能：配置BT是否单次捕捉
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCR1_CAPONCE_CONTINUE/BTIMx_BTCR1_CAPONCE_SINGLE    连续捕捉/单次捕捉
输出: 无
*********************************/
void BTIMx_BTCR1_CAPONCE_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCR1;
	tmpreg &= ~(BTIMx_BTCR1_CAPONCE_Msk);
	tmpreg |= (SetValue & BTIMx_BTCR1_CAPONCE_Msk);
	BTIMx->BTCR1 = tmpreg;
}
/*********************************
basic timer工作模式配置函数 
功能：读BT配置捕捉方式
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCR1_CAPONCE_CONTINUE/BTIMx_BTCR1_CAPONCE_SINGLE    连续捕捉/单次捕捉
*********************************/
uint32_t BTIMx_BTCR1_CAPONCE_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCR1 & BTIMx_BTCR1_CAPONCE_Msk);
}

/*********************************
basic timer输出模式配置函数 
功能：配置BT PWM输出是否有效
输入: BTIM1/BTIM2  定时器名称
      ENABLE/DISABLE   PWM输出模式有效/无效
输出: 无
*********************************/
void BTIMx_BTCR1_PWM_Setable(BTIMx_Type* BTIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		BTIMx->BTCR1 |= (BTIMx_BTCR1_PWM_Msk);
	}
	else
	{
		BTIMx->BTCR1 &= ~(BTIMx_BTCR1_PWM_Msk);
	}
}
/*********************************
basic timer输出模式配置函数 
功能：读BT配置 PWM输出是否有效
输入: BTIM1/BTIM2  定时器名称
输出: ENABLE/DISABLE   PWM输出模式有效/无效
*********************************/
FunState BTIMx_BTCR1_PWM_Getable(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTCR1 & (BTIMx_BTCR1_PWM_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
basic timer计数器内部计数源信号选择配置函数 
功能：配置BT 计数器内部计数源信号选择
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCR2_SIG2SEL_GROUP1/BTIMx_BTCR2_SIG2SEL_GROUP2 内部计数源选择Group1/Group2
输出: 无
*********************************/
void BTIMx_BTCR2_SIG2SEL_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCR2;
	tmpreg &= ~(BTIMx_BTCR2_SIG2SEL_Msk);
	tmpreg |= (SetValue & BTIMx_BTCR2_SIG2SEL_Msk);
	BTIMx->BTCR2 = tmpreg;
}
/*********************************
basic timer计数器内部计数源信号选择配置函数
功能：获取BT计数器内部计数源信号选择
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCR2_SIG2SEL_GROUP1/BTIMx_BTCR2_SIG2SEL_GROUP2 内部计数源选择Group1/Group2
*********************************/
uint32_t BTIMx_BTCR2_SIG2SEL_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCR2 & BTIMx_BTCR2_SIG2SEL_Msk);
}

/*********************************
basic timer计数器内部捕捉源信号选择配置函数 
功能：配置BT 计数器内部捕捉源信号选择
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCR2_SIG1SEL_GROUP1/BTIMx_BTCR2_SIG1SEL_GROUP2 内部捕捉源选择Group1/Group2
输出: 无
*********************************/
void BTIMx_BTCR2_SIG1SEL_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCR2;
	tmpreg &= ~(BTIMx_BTCR2_SIG1SEL_Msk);
	tmpreg |= (SetValue & BTIMx_BTCR2_SIG1SEL_Msk);
	BTIMx->BTCR2 = tmpreg;
}
/*********************************
basic timer计数器内部捕捉源信号选择配置函数
功能：获取BT计数器内部捕捉源信号选择
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCR2_SIG1SEL_GROUP1/BTIMx_BTCR2_SIG1SEL_GROUP2 内部捕捉源选择Group1/Group2
*********************************/
uint32_t BTIMx_BTCR2_SIG1SEL_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCR2 & BTIMx_BTCR2_SIG1SEL_Msk);
}

/*********************************
basic timer计数器高位计数器源选择配置函数 
功能：配置BT 高位计数器源选择
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCR2_CNTHSEL_CNTL/BTIMx_BTCR2_CNTHSEL_CAPSRC/BTIMx_BTCR2_CNTHSEL_CNTSRC_DIR 与低位计数器组成16位计数器/选择内部捕捉信号源/选择内部计数信号源或外部DIR输入组合
输出: 无
*********************************/
void BTIMx_BTCR2_CNTHSEL_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCR2;
	tmpreg &= ~(BTIMx_BTCR2_CNTHSEL_Msk);
	tmpreg |= (SetValue & BTIMx_BTCR2_CNTHSEL_Msk);
	BTIMx->BTCR2 = tmpreg;
}
/*********************************
basic timer计数器高位计数器源选择配置函数
功能：获取BT计数器高位计数器源选择
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCR2_CNTHSEL_CNTL/BTIMx_BTCR2_CNTHSEL_CAPSRC/BTIMx_BTCR2_CNTHSEL_CNTSRC_DIR 与低位计数器组成16位计数器/选择内部捕捉信号源/选择内部计数信号源或外部DIR输入组合
*********************************/
uint32_t BTIMx_BTCR2_CNTHSEL_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCR2 & BTIMx_BTCR2_CNTHSEL_Msk);
}

/*********************************
basic timer计数器外部输入DIR控制配置函数 
功能：配置BT 外部输入DIR控制
输入: BTIM1/BTIM2  定时器名称
      ENABLE/DISABLE	外部输入DIR控制有效/无效
输出: 无
*********************************/
void BTIMx_BTCR2_DIREN_Setable(BTIMx_Type* BTIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		BTIMx->BTCR2 |= (BTIMx_BTCR2_DIREN_Msk);
	}
	else
	{
		BTIMx->BTCR2 &= ~(BTIMx_BTCR2_DIREN_Msk);
	}
}
/*********************************
basic timer计数器外部输入DIR控制配置函数 
功能：获取BT外部输入DIR控制状态
输入: BTIM1/BTIM2  定时器名称
输出: NABLE/DISABLE	外部输入DIR控制有效/无效
*********************************/
FunState BTIMx_BTCR2_DIREN_Getable(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTCR2 & (BTIMx_BTCR2_DIREN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
basic timer计数器内部输入DIR控制配置函数 
功能：配置BT 内部输入DIR控制
输入: BTIM1/BTIM2  定时器名称
      ENABLE/DISABLE	内部输入DIR控制有效/无效
输出: 无
*********************************/
void BTIMx_BTCR2_STDIR_Setable(BTIMx_Type* BTIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		BTIMx->BTCR2 |= (BTIMx_BTCR2_STDIR_Msk);
	}
	else
	{
		BTIMx->BTCR2 &= ~(BTIMx_BTCR2_STDIR_Msk);
	}
}
/*********************************
basic timer计数器内部输入DIR控制配置函数 
功能：获取BT内部输入DIR控制状态
输入: BTIM1/BTIM2  定时器名称
输出: NABLE/DISABLE	内部输入DIR控制有效/无效
*********************************/
FunState BTIMx_BTCR2_STDIR_Getable(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTCR2 & (BTIMx_BTCR2_STDIR_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
basic timer低位计数器控制函数 
功能：配置BT 低位计数器使能控制选择信号
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCR2_SRCSEL_STDIR_EXSIG2/BTIMx_BTCR2_SRCSEL_WITHOUT_DIR	低位计数器由STDIR或外部EX_SIG2输入控制/低位计数器不受DIR控制
输出: 无
*********************************/
void BTIMx_BTCR2_SRCSEL_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCR2;
	tmpreg &= ~(BTIMx_BTCR2_SRCSEL_Msk);
	tmpreg |= (SetValue & BTIMx_BTCR2_SRCSEL_Msk);
	BTIMx->BTCR2 = tmpreg;
}

/*********************************
basic timer低位计数器控制函数 
功能：获取BT低位计数器使能控制选择信号
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCR2_SRCSEL_STDIR_EXSIG2/BTIMx_BTCR2_SRCSEL_WITHOUT_DIR	低位计数器由STDIR或外部EX_SIG2输入控制/低位计数器不受DIR控制
*********************************/
uint32_t BTIMx_BTCR2_SRCSEL_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCR2 & BTIMx_BTCR2_SRCSEL_Msk);
}

/*********************************
basic timer输入信号2控制函数 
功能：配置BT 输入信号2极性
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCR2_DIRPO_NO_ANTI/BTIMx_BTCR2_DIRPO_ANTI	对外部输入DIR信号不反向/对外部输入DIR信号取反向
输出: 无
*********************************/
void BTIMx_BTCR2_DIRPO_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCR2;
	tmpreg &= ~(BTIMx_BTCR2_DIRPO_Msk);
	tmpreg |= (SetValue & BTIMx_BTCR2_DIRPO_Msk);
	BTIMx->BTCR2 = tmpreg;
}
/*********************************
basic timer输入信号2控制函数 
功能：获取BT输入信号2极性
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCR2_DIRPO_NO_ANTI/BTIMx_BTCR2_DIRPO_ANTI	对外部输入DIR信号不反向/对外部输入DIR信号取反向
*********************************/
uint32_t BTIMx_BTCR2_DIRPO_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCR2 & BTIMx_BTCR2_DIRPO_Msk);
}

/*********************************
basic timer RTCOUT2信号源选择函数 
功能：配置BT RTCOUT2信号源
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCFG1_RTCSEL2_RTC_32768/	RTC模块输出的32768Hz信号
      BTIMx_BTCFG1_RTCSEL2_RTC_SEC/	RTC模块输出的秒信号
      BTIMx_BTCFG1_RTCSEL2_RTC_MIN/	RTC模块输出的分信号
      BTIMx_BTCFG1_RTCSEL2_LPTIM_OUT	LPTIM模块输出的信号
输出: 无
*********************************/
void BTIMx_BTCFG1_RTCSEL2_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCFG1;
	tmpreg &= ~(BTIMx_BTCFG1_RTCSEL2_Msk);
	tmpreg |= (SetValue & BTIMx_BTCFG1_RTCSEL2_Msk);
	BTIMx->BTCFG1 = tmpreg;
}
/*********************************
basic timer RTCOUT2信号源选择函数 
功能：获取BT RTCOUT2信号源配置
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCFG1_RTCSEL2_RTC_32768/	RTC模块输出的32768Hz信号
      BTIMx_BTCFG1_RTCSEL2_RTC_SEC/	RTC模块输出的秒信号
      BTIMx_BTCFG1_RTCSEL2_RTC_MIN/	RTC模块输出的分信号
      BTIMx_BTCFG1_RTCSEL2_LPTIM_OUT	LPTIM模块输出的信号
*********************************/
uint32_t BTIMx_BTCFG1_RTCSEL2_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCFG1 & BTIMx_BTCFG1_RTCSEL2_Msk);
}


/*********************************
basic timer RTCOUT1信号源选择函数 
功能：配置BT RTCOUT1信号源
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCFG1_RTCSEL1_RTC_32768/	RTC模块输出的32768Hz信号
      BTIMx_BTCFG1_RTCSEL1_RTC_SEC/	RTC模块输出的秒信号
      BTIMx_BTCFG1_RTCSEL1_RTC_MIN/	RTC模块输出的分信号
      BTIMx_BTCFG1_RTCSEL1_LPTIM_OUT	LPTIM模块输出的信号
输出: 无
*********************************/
void BTIMx_BTCFG1_RTCSEL1_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCFG1;
	tmpreg &= ~(BTIMx_BTCFG1_RTCSEL1_Msk);
	tmpreg |= (SetValue & BTIMx_BTCFG1_RTCSEL1_Msk);
	BTIMx->BTCFG1 = tmpreg;
}
/*********************************
basic timer RTCOUT1信号源选择函数  
功能：获取BT RTCOUT1信号源配置
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCFG1_RTCSEL1_RTC_32768/	RTC模块输出的32768Hz信号
      BTIMx_BTCFG1_RTCSEL1_RTC_SEC/	RTC模块输出的秒信号
      BTIMx_BTCFG1_RTCSEL1_RTC_MIN/	RTC模块输出的分信号
      BTIMx_BTCFG1_RTCSEL1_LPTIM_OUT	LPTIM模块输出的信号
*********************************/
uint32_t BTIMx_BTCFG1_RTCSEL1_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCFG1 & BTIMx_BTCFG1_RTCSEL1_Msk);
}

/*********************************
basic timer Group2信号源选择函数 
功能：配置BT Group2信号源
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCFG1_GRP2SEL_APBCLK/	系统时钟APBCLK
      BTIMx_BTCFG1_GRP2SEL_RTCOUT2/	RTC时钟RTCOUT2
      BTIMx_BTCFG1_GRP2SEL_IN_SIG2/	内部输入信号2
      BTIMx_BTCFG1_GRP2SEL_EX_SIG2	外部输入信号2
输出: 无
*********************************/
void BTIMx_BTCFG1_GRP2SEL_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCFG1;
	tmpreg &= ~(BTIMx_BTCFG1_GRP2SEL_Msk);
	tmpreg |= (SetValue & BTIMx_BTCFG1_GRP2SEL_Msk);
	BTIMx->BTCFG1 = tmpreg;
}
/*********************************
basic timer Group2信号源选择函数
功能：获取BT Group2信号源配置
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCFG1_GRP2SEL_APBCLK/	系统时钟APBCLK
      BTIMx_BTCFG1_GRP2SEL_RTCOUT2/	RTC时钟RTCOUT2
      BTIMx_BTCFG1_GRP2SEL_IN_SIG2/	内部输入信号2
      BTIMx_BTCFG1_GRP2SEL_EX_SIG2	外部输入信号2
*********************************/
uint32_t BTIMx_BTCFG1_GRP2SEL_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCFG1 & BTIMx_BTCFG1_GRP2SEL_Msk);
}
/*********************************
basic timer Group1信号源选择函数 
功能：配置BT Group1信号源
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCFG1_GRP1SEL_APBCLK/	系统时钟APBCLK
      BTIMx_BTCFG1_GRP1SEL_RTCOUT2/	RTC时钟RTCOUT2
      BTIMx_BTCFG1_GRP1SEL_IN_SIG2/	内部输入信号2
      BTIMx_BTCFG1_GRP1SEL_EX_SIG2	外部输入信号2
输出: 无
*********************************/
void BTIMx_BTCFG1_GRP1SEL_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCFG1;
	tmpreg &= ~(BTIMx_BTCFG1_GRP1SEL_Msk);
	tmpreg |= (SetValue & BTIMx_BTCFG1_GRP1SEL_Msk);
	BTIMx->BTCFG1 = tmpreg;
}
/*********************************
basic timer Group1信号源选择函数
功能：获取BT Group1信号源配置
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCFG1_GRP1SEL_APBCLK/	系统时钟APBCLK
      BTIMx_BTCFG1_GRP1SEL_RTCOUT2/	RTC时钟RTCOUT2
      BTIMx_BTCFG1_GRP1SEL_IN_SIG2/	内部输入信号2
      BTIMx_BTCFG1_GRP1SEL_EX_SIG2	外部输入信号2
*********************************/
uint32_t BTIMx_BTCFG1_GRP1SEL_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCFG1 & BTIMx_BTCFG1_GRP1SEL_Msk);
}


/*********************************
basic timer 外部输入信号源2选择函数 
功能：配置BT 外部输入信号源2
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCFG2_EXSEL2_BT_IN0	外部输入1
      BTIMx_BTCFG2_EXSEL2_BT_IN1	外部输入2
      BTIMx_BTCFG2_EXSEL2_BT_IN2	外部输入3
      BTIMx_BTCFG2_EXSEL2_BT_IN3	外部输入4
输出: 无
*********************************/
void BTIMx_BTCFG2_EXSEL2_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCFG2;
	tmpreg &= ~(BTIMx_BTCFG2_EXSEL2_Msk);
	tmpreg |= (SetValue & BTIMx_BTCFG2_EXSEL2_Msk);
	BTIMx->BTCFG2 = tmpreg;
}
/*********************************
basic timer 外部输入信号源2选择函数 
功能：获取BT 外部输入信号源2配置
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCFG2_EXSEL2_BT_IN0	外部输入1
      BTIMx_BTCFG2_EXSEL2_BT_IN1	外部输入2
      BTIMx_BTCFG2_EXSEL2_BT_IN2	外部输入3
      BTIMx_BTCFG2_EXSEL2_BT_IN3	外部输入4
*********************************/
uint32_t BTIMx_BTCFG2_EXSEL2_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCFG2 & BTIMx_BTCFG2_EXSEL2_Msk);
}

/*********************************
basic timer 外部输入信号源1选择函数 
功能：配置BT 外部输入信号源1
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCFG2_EXSEL1_BT_IN0	外部输入1
      BTIMx_BTCFG2_EXSEL1_BT_IN1	外部输入2
      BTIMx_BTCFG2_EXSEL1_BT_IN2	外部输入3
      BTIMx_BTCFG2_EXSEL1_BT_IN3	外部输入4
输出: 无
*********************************/
void BTIMx_BTCFG2_EXSEL1_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCFG2;
	tmpreg &= ~(BTIMx_BTCFG2_EXSEL1_Msk);
	tmpreg |= (SetValue & BTIMx_BTCFG2_EXSEL1_Msk);
	BTIMx->BTCFG2 = tmpreg;
}
/*********************************
basic timer 外部输入信号源1选择函数 
功能：获取BT 外部输入信号源1配置
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCFG2_EXSEL1_BT_IN0	外部输入1
      BTIMx_BTCFG2_EXSEL1_BT_IN1	外部输入2
      BTIMx_BTCFG2_EXSEL1_BT_IN2	外部输入3
      BTIMx_BTCFG2_EXSEL1_BT_IN3	外部输入4
*********************************/
uint32_t BTIMx_BTCFG2_EXSEL1_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCFG2 & BTIMx_BTCFG2_EXSEL1_Msk);
}

/*********************************
basic timer 内部输入信号源2选择函数 
功能：配置BT 内部输入信号源2
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCFG2_INSEL2_UART_RX3	选择UART3接收信号做信号源
      BTIMx_BTCFG2_INSEL2_UART_RX4	选择UART4接收信号做信号源
      BTIMx_BTCFG2_INSEL2_UART_RX5	选择UART5接收信号做信号源
      BTIMx_BTCFG2_INSEL2_RCLP		选择RCLP做信号源
      BTIMx_BTCFG2_INSEL2_BT1_OUT	选择BT1的输出做信号源
输出: 无
*********************************/
void BTIMx_BTCFG2_INSEL2_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCFG2;
	tmpreg &= ~(BTIMx_BTCFG2_INSEL2_Msk);
	tmpreg |= (SetValue & BTIMx_BTCFG2_INSEL2_Msk);
	BTIMx->BTCFG2 = tmpreg;
}
/*********************************
basic timer 内部输入信号源2选择函数 
功能：获取BT 内部输入信号源2配置
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCFG2_INSEL2_UART_RX3	信号源为UART3接收信号
      BTIMx_BTCFG2_INSEL2_UART_RX4	信号源为UART4接收信号
      BTIMx_BTCFG2_INSEL2_UART_RX5	信号源为UART5接收信号
      BTIMx_BTCFG2_INSEL2_RCLP		信号源为RCLP做信号源
      BTIMx_BTCFG2_INSEL2_BT1_OUT	信号源为BT1的输出
*********************************/
uint32_t BTIMx_BTCFG2_INSEL2_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCFG2 & BTIMx_BTCFG2_INSEL2_Msk);
}
/*********************************
basic timer 内部输入信号源1选择函数 
功能：配置BT 内部输入信号源1
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTCFG2_INSEL1_UART_RX0	选择UART0接收信号做信号源
      BTIMx_BTCFG2_INSEL1_UART_RX1	选择UART1接收信号做信号源
      BTIMx_BTCFG2_INSEL1_UART_RX2	选择UART2接收信号做信号源
      BTIMx_BTCFG2_INSEL1_RCLP		选择RCLP做信号源
输出: 无
*********************************/
void BTIMx_BTCFG2_INSEL1_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTCFG2;
	tmpreg &= ~(BTIMx_BTCFG2_INSEL1_Msk);
	tmpreg |= (SetValue & BTIMx_BTCFG2_INSEL1_Msk);
	BTIMx->BTCFG2 = tmpreg;
}

/*********************************
basic timer 内部输入信号源1选择函数 
功能：获取BT 内部输入信号源1配置
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTCFG2_INSEL1_UART_RX0	信号源为UART0接收信号
      BTIMx_BTCFG2_INSEL1_UART_RX1	信号源为UART1接收信号
      BTIMx_BTCFG2_INSEL1_UART_RX2	信号源为UART2接收信号
      BTIMx_BTCFG2_INSEL1_RCLP		信号源为RCLP做信号源
*********************************/
uint32_t BTIMx_BTCFG2_INSEL1_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCFG2 & BTIMx_BTCFG2_INSEL1_Msk);
}

/*********************************
basic timer group1输入预分频控制函数 
功能：配置BT group1预分频
输入: BTIM1/BTIM2  定时器名称
      具体分频值：00-FF，00表示1分频，FF表示256分频
输出: 无
*********************************/
void BTIMx_BTPRES_Write(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	BTIMx->BTPRES = (SetValue & BTIMx_BTPRES_PRESCALE_Msk);
}
/*********************************
basic timer group1输入预分频控制函数 
功能：获取BT group1预分频
输入: BTIM1/BTIM2  定时器名称
      
输出: 00-FF 00表示1分频，FF表示256分频
*********************************/
uint32_t BTIMx_BTPRES_Read(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTPRES & BTIMx_BTPRES_PRESCALE_Msk);
}
/*********************************
basic timer group1加载控制函数 
功能：使能BT高位加载
输入: BTIM1/BTIM2  定时器名称
      ENABLE 高位加载启动
      DISABLE 高位加载停止
输出: 无
*********************************/
void BTIMx_BTLOADCR_LHEN_Setable(BTIMx_Type* BTIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		BTIMx->BTLOADCR |= (BTIMx_BTLOADCR_LHEN_Msk);
	}
	else
	{
		BTIMx->BTLOADCR &= ~(BTIMx_BTLOADCR_LHEN_Msk);
	}
}
/*********************************
basic timer group1加载控制函数 
功能：使能BT低位加载
输入: BTIM1/BTIM2  定时器名称
      ENABLE 低位加载启动
      DISABLE 低位加载停止
输出: 无
*********************************/
void BTIMx_BTLOADCR_LLEN_Setable(BTIMx_Type* BTIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		BTIMx->BTLOADCR |= (BTIMx_BTLOADCR_LLEN_Msk);
	}
	else
	{
		BTIMx->BTLOADCR &= ~(BTIMx_BTLOADCR_LLEN_Msk);
	}
}

/*********************************
basic timer 计数器计数值相关函数 
功能：读取计数器低位计数值
输入: BTIM1/BTIM2  定时器名称
输出: 0000-FFFF 具体计数值
*********************************/
uint32_t BTIMx_BTCNTL_Read(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCNTL & BTIMx_BTCNTL_CNTL_Msk);
}

/*********************************
basic timer 计数器计数值相关函数 
功能：读取计数器高位计数值
输入: BTIM1/BTIM2  定时器名称
输出: 0000-FFFF 具体计数值
*********************************/
uint32_t BTIMx_BTCNTH_Read(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCNTH & BTIMx_BTCNTH_CNTH_Msk);
}

/*********************************
basic timer 计数器预置数相关函数 
功能：配置计数器高位预置数
输入: BTIM1/BTIM2  定时器名称
      0000-FFFF  具体预置数值
*********************************/
void BTIMx_BTPRESET_PRESETH_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTPRESET;
	tmpreg &= ~(BTIMx_BTPRESET_PRESETH_Msk);
	tmpreg |= (SetValue & BTIMx_BTPRESET_PRESETH_Msk);
	BTIMx->BTPRESET = tmpreg;
}
/*********************************
basic timer 计数器预置数相关函数 
功能：读取计数器高位预置数
输入: BTIM1/BTIM2  定时器名称
输出: 0000-FFFF  具体预置数值
*********************************/
uint32_t BTIMx_BTPRESET_PRESETH_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTPRESET & BTIMx_BTPRESET_PRESETH_Msk);
}

/*********************************
basic timer 计数器预置数相关函数 
功能：配置计数器低位预置数
输入: BTIM1/BTIM2  定时器名称
      0000-FFFF  具体预置数值
*********************************/
void BTIMx_BTPRESET_PRESETL_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTPRESET;
	tmpreg &= ~(BTIMx_BTPRESET_PRESETL_Msk);
	tmpreg |= (SetValue & BTIMx_BTPRESET_PRESETL_Msk);
	BTIMx->BTPRESET = tmpreg;
}

/*********************************
basic timer 计数器预置数相关函数 
功能：读取计数器低位预置数
输入: BTIM1/BTIM2  定时器名称
输出: 0000-FFFF  具体预置数值
*********************************/
uint32_t BTIMx_BTPRESET_PRESETL_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTPRESET & BTIMx_BTPRESET_PRESETL_Msk);
}

/*********************************
basic timer 计数器加载数相关函数 
功能：配置计数器低位加载数
输入: BTIM1/BTIM2  定时器名称
      0000-FFFF  具体加载数值
*********************************/
void BTIMx_BTLOADL_Write(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	BTIMx->BTLOADL = (SetValue & BTIMx_BTLOADL_LOADL_Msk);
}
/*********************************
basic timer 计数器加载数相关函数 
功能：读取计数器低位加载数
输入: BTIM1/BTIM2  定时器名称
输出: 0000-FFFF  具体加载数值
*********************************/
uint32_t BTIMx_BTLOADL_Read(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTLOADL & BTIMx_BTLOADL_LOADL_Msk);
}

/*********************************
basic timer 计数器加载数相关函数 
功能：配置计数器高位加载数
输入: BTIM1/BTIM2  定时器名称
      0000-FFFF  具体加载数值
*********************************/
void BTIMx_BTLOADH_Write(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	BTIMx->BTLOADH = (SetValue & BTIMx_BTLOADH_LOADH_Msk);
}
/*********************************
basic timer 计数器加载数相关函数 
功能：读取计数器高位加载数
输入: BTIM1/BTIM2  定时器名称
输出: 0000-FFFF  具体加载数值
*********************************/
uint32_t BTIMx_BTLOADH_Read(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTLOADH & BTIMx_BTLOADH_LOADH_Msk);
}
/*********************************
basic timer 计数器比较值相关函数 
功能：配置计数器低位比较值
输入: BTIM1/BTIM2  定时器名称
      0000-FFFF  具体比较值
*********************************/
void BTIMx_BTCMPL_Write(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	BTIMx->BTCMPL = (SetValue & BTIMx_BTCMPL_CMPL_Msk);
}
/*********************************
basic timer 计数器比较值相关函数
功能：读取计数器低位比较值
输入: BTIM1/BTIM2  定时器名称
输出: 0000-FFFF  具体比较值
*********************************/
uint32_t BTIMx_BTCMPL_Read(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCMPL & BTIMx_BTCMPL_CMPL_Msk);
}

/*********************************
basic timer 计数器比较值相关函数 
功能：配置计数器高位比较值
输入: BTIM1/BTIM2  定时器名称
      0000-FFFF  具体比较值
*********************************/
void BTIMx_BTCMPH_Write(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	BTIMx->BTCMPH = (SetValue & BTIMx_BTCMPH_CMPH_Msk);
}
/*********************************
basic timer 计数器比较值相关函数
功能：读取计数器高位比较值
输入: BTIM1/BTIM2  定时器名称
输出: 0000-FFFF  具体比较值
*********************************/
uint32_t BTIMx_BTCMPH_Read(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTCMPH & BTIMx_BTCMPH_CMPH_Msk);
}

/*********************************
basic timer 计数器脉冲输出函数 
功能：配置计数器具体脉冲宽度
输入: BTIM1/BTIM2  定时器名称
      0000-0FFF    宽度为30.5*(1+SetValue)
*********************************/
void BTIMx_BTOUTCNT_Write(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	BTIMx->BTOUTCNT = (SetValue & BTIMx_BTOUTCNT_OUTCNT_Msk);
}
/*********************************
basic timer 计数器脉冲输出函数 
功能：读取计数器具体脉冲宽度定义
输入: BTIM1/BTIM2  定时器名称
输出: 0000-0FFF  具体宽度为30.5*(1+返回值)
*********************************/
uint32_t BTIMx_BTOUTCNT_Read(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTOUTCNT & BTIMx_BTOUTCNT_OUTCNT_Msk);
}

/*********************************
basic timer 计数器输出控制函数 
功能：配置计数器输出是否清零
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTOCR_OUTCLR/BTIMx_BTOCR_OUTCLR_CLR		输出写入无效/输出清零(若设置了反向则输出高，未设置则输出低)
*********************************/
void BTIMx_BTOCR_OUTCLR_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTOCR;
	tmpreg &= ~(BTIMx_BTOCR_OUTCLR_Msk);
	tmpreg |= (SetValue & BTIMx_BTOCR_OUTCLR_Msk);
	BTIMx->BTOCR = tmpreg;
}
/*********************************
basic timer 计数器输出控制函数 
功能：获取计数器输出是否清零
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTOCR_OUTCLR/BTIMx_BTOCR_OUTCLR_CLR		输出写入无效/输出清零(若设置了反向则输出高，未设置则输出低)
*********************************/
uint32_t BTIMx_BTOCR_OUTCLR_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTOCR & BTIMx_BTOCR_OUTCLR_Msk);
}

/*********************************
basic timer 计数器输出控制函数 
功能：配置计数器输出电平
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTOCR_OUTINV_NOT_ANTI/BTIMx_BTOCR_OUTINV_ANTI	输出电平不取反/输出电平取反
*********************************/
void BTIMx_BTOCR_OUTINV_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTOCR;
	tmpreg &= ~(BTIMx_BTOCR_OUTINV_Msk);
	tmpreg |= (SetValue & BTIMx_BTOCR_OUTINV_Msk);
	BTIMx->BTOCR = tmpreg;
}
/*********************************
basic timer 计数器输出控制函数 
功能：获取计数器输出电平
输入: BTIM1/BTIM2  定时器名称
输出: 	BTIMx_BTOCR_OUTMOD_PULSE/BTIMx_BTOCR_OUTMOD_ANTI_LEVEL	输出电平不取反/输出电平取反
*********************************/
uint32_t BTIMx_BTOCR_OUTINV_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTOCR & BTIMx_BTOCR_OUTINV_Msk);
}

/*********************************
basic timer 计数器输出控制函数 
功能：配置计数器输出模式
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTOCR_OUTINV_NOT_ANTI/BTIMx_BTOCR_OUTINV_ANTI	输出脉冲，宽度可调/输出之前的反向电平
输出: 无
*********************************/   
void BTIMx_BTOCR_OUTMOD_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTOCR;
	tmpreg &= ~(BTIMx_BTOCR_OUTMOD_Msk);
	tmpreg |= (SetValue & BTIMx_BTOCR_OUTMOD_Msk);
	BTIMx->BTOCR = tmpreg;
}
/*********************************
basic timer 计数器输出控制函数 
功能：获取计数器输出模式
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTOCR_OUTINV_NOT_ANTI/BTIMx_BTOCR_OUTINV_ANTI	输出脉冲，宽度可调/输出之前的反向电平
*********************************/
uint32_t BTIMx_BTOCR_OUTMOD_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTOCR & BTIMx_BTOCR_OUTMOD_Msk);
}

/*********************************
basic timer 计数器输出控制函数 
功能：配置计数器输出信号源
输入: BTIM1/BTIM2  定时器名称
      BTIMx_BTOCR_OUTSEL_CMPH/		输出高位计数器比较信号，仅计数模式有效
      BTIMx_BTOCR_OUTSEL_CMPL/		输出低位计数器比较信号，仅计数模式有效
      BTIMx_BTOCR_OUTSEL_GROUP1/	输出Group1的输入信号，计数，捕捉模式有效	
      BTIMx_BTOCR_OUTSEL_GROUP2/	输出Group2的输入信号，计数，捕捉模式有效
      BTIMx_BTOCR_OUTSEL_PWM		PWM输出
输出: 无
*********************************/  
void BTIMx_BTOCR_OUTSEL_Set(BTIMx_Type* BTIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = BTIMx->BTOCR;
	tmpreg &= ~(BTIMx_BTOCR_OUTSEL_Msk);
	tmpreg |= (SetValue & BTIMx_BTOCR_OUTSEL_Msk);
	BTIMx->BTOCR = tmpreg;
}
/*********************************
basic timer 计数器输出控制函数 
功能：获取计数器输出信号源
输入: BTIM1/BTIM2  定时器名称
输出: BTIMx_BTOCR_OUTSEL_CMPH/		输出高位计数器比较信号，仅计数模式有效
      BTIMx_BTOCR_OUTSEL_CMPL/		输出低位计数器比较信号，仅计数模式有效
      BTIMx_BTOCR_OUTSEL_GROUP1/	输出Group1的输入信号，计数，捕捉模式有效	
      BTIMx_BTOCR_OUTSEL_GROUP2/	输出Group2的输入信号，计数，捕捉模式有效
      BTIMx_BTOCR_OUTSEL_PWM		PWM输出
*********************************/
uint32_t BTIMx_BTOCR_OUTSEL_Get(BTIMx_Type* BTIMx)
{
	return (BTIMx->BTOCR & BTIMx_BTOCR_OUTSEL_Msk);
}

/*********************************
basic timer计数器中断使能控制函数 
功能：使能BT高位比较发生信号
输入: BTIM1/BTIM2  定时器名称	 
      ENABLE/DISABLE 			高位比较发生信号中断使能/高位比较发生信号中断禁止
输出: 无
*********************************/
void BTIMx_BTIE_CMPHIE_Setable(BTIMx_Type* BTIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		BTIMx->BTIE |= (BTIMx_BTIE_CMPHIE_Msk);
	}
	else
	{
		BTIMx->BTIE &= ~(BTIMx_BTIE_CMPHIE_Msk);
	}
}
/*********************************
basic timer计数器中断使能控制函数 
功能：获取BT高位比较发生信号状态
输入: BTIM1/BTIM2  定时器名称	 
输出: ENABLE/DISABLE 			高位比较发生信号中断使能/高位比较发生信号中断禁止
*********************************/
FunState BTIMx_BTIE_CMPHIE_Getable(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTIE & (BTIMx_BTIE_CMPHIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
basic timer计数器中断使能控制函数 
功能：使能BT低位比较发生信号
输入: BTIM1/BTIM2  定时器名称	 
      ENABLE/DISABLE 			低位比较发生信号中断使能/低位比较发生信号中断禁止
输出: 无
*********************************/
void BTIMx_BTIE_CMPLIE_Setable(BTIMx_Type* BTIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		BTIMx->BTIE |= (BTIMx_BTIE_CMPLIE_Msk);
	}
	else
	{
		BTIMx->BTIE &= ~(BTIMx_BTIE_CMPLIE_Msk);
	}
}
/*********************************
basic timer计数器中断使能控制函数 
功能：获取BT低位比较发生信号状态
输入: BTIM1/BTIM2  定时器名称	 
输出: ENABLE/DISABLE 			低位比较发生信号中断使能/低位比较发生信号中断禁止
*********************************/
FunState BTIMx_BTIE_CMPLIE_Getable(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTIE & (BTIMx_BTIE_CMPLIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/*********************************
basic timer计数器中断使能控制函数 
功能：使能BT高位溢出发生信号
输入: BTIM1/BTIM2  定时器名称	 
      ENABLE/DISABLE 			高位溢出发生信号中断使能/高位溢出发生信号中断禁止
输出: 无
*********************************/
void BTIMx_BTIE_OVHIE_Setable(BTIMx_Type* BTIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		BTIMx->BTIE |= (BTIMx_BTIE_OVHIE_Msk);
	}
	else
	{
		BTIMx->BTIE &= ~(BTIMx_BTIE_OVHIE_Msk);
	}
}
/*********************************
basic timer计数器中断使能控制函数 
功能：获取BT高位溢出发生信号状态
输入: BTIM1/BTIM2  定时器名称	 
输出: ENABLE/DISABLE 			高位溢出发生信号中断使能/高位溢出发生信号中断禁止
*********************************/
FunState BTIMx_BTIE_OVHIE_Getable(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTIE & (BTIMx_BTIE_OVHIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}


/*********************************
basic timer计数器中断使能控制函数 
功能：使能BT低位溢出发生信号
输入: BTIM1/BTIM2  定时器名称	 
      ENABLE/DISABLE 			低位溢出发生信号中断使能/低位溢出发生信号中断禁止
输出: 无
*********************************/
void BTIMx_BTIE_OVLIE_Setable(BTIMx_Type* BTIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		BTIMx->BTIE |= (BTIMx_BTIE_OVLIE_Msk);
	}
	else
	{
		BTIMx->BTIE &= ~(BTIMx_BTIE_OVLIE_Msk);
	}
}
/*********************************
basic timer计数器中断使能控制函数 
功能：获取BT低位溢出发生信号状态
输入: BTIM1/BTIM2  定时器名称	 
输出: ENABLE/DISABLE 			低位溢出发生信号中断使能/低位溢出发生信号中断禁止
*********************************/
FunState BTIMx_BTIE_OVLIE_Getable(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTIE & (BTIMx_BTIE_OVLIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}


/*********************************
basic timer计数器中断使能控制函数 
功能：使能BT捕捉发生信号
输入: BTIM1/BTIM2  定时器名称	 
      ENABLE/DISABLE 			捕捉发生信号中断使能/捕捉发生信号中断禁止
输出: 无
*********************************/
void BTIMx_BTIE_CAPIE_Setable(BTIMx_Type* BTIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		BTIMx->BTIE |= (BTIMx_BTIE_CAPIE_Msk);
	}
	else
	{
		BTIMx->BTIE &= ~(BTIMx_BTIE_CAPIE_Msk);
	}
}
/*********************************
basic timer计数器中断使能控制函数 
功能：获取BT捕捉发生信号状态
输入: BTIM1/BTIM2  定时器名称	 
输出: ENABLE/DISABLE 			捕捉发生信号中断使能/捕捉发生信号中断禁止
*********************************/
FunState BTIMx_BTIE_CAPIE_Getable(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTIE & (BTIMx_BTIE_CAPIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
basic timer计数器中断信号函数 
功能：清BT计数器捕捉沿状态
输入: BTIM1/BTIM2  定时器名称
输出: 无
*********************************/
void BTIMx_BTIF_EDGESTA_Clr(BTIMx_Type* BTIMx)
{
	BTIMx->BTIF = BTIMx_BTIF_EDGESTA_Msk;
}
/*********************************
basic timer计数器中断信号函数 
功能：获取BT计数器捕捉沿状态
输入: BTIM1/BTIM2  定时器名称	 
输出: SET/RESET 			脉冲宽度捕捉模式时表示捕捉到下沿/脉冲宽度捕捉模式时表示捕捉到上沿
*********************************/
FlagStatus BTIMx_BTIF_EDGESTA_Chk(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTIF & BTIMx_BTIF_EDGESTA_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*********************************
basic timer计数器中断信号函数 
功能：清BT计数器高位比较发生信号状态
输入: BTIM1/BTIM2  定时器名称
输出: 无
*********************************/
void BTIMx_BTIF_CMPHIF_Clr(BTIMx_Type* BTIMx)
{
	BTIMx->BTIF = BTIMx_BTIF_CMPHIF_Msk;
}
/*********************************
basic timer计数器中断信号函数 
功能：获取BT计数器高位比较发生信号状态
输入: BTIM1/BTIM2  定时器名称	 
输出: SET/RESET 			当前计数器的值大于等工作寄存器的值/当前计数器的值小于工作寄存器的值
*********************************/
FlagStatus BTIMx_BTIF_CMPHIF_Chk(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTIF & BTIMx_BTIF_CMPHIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*********************************
basic timer计数器中断信号函数 
功能：清BT计数器低位比较发生信号状态
输入: BTIM1/BTIM2  定时器名称
输出: 无
*********************************/
void BTIMx_BTIF_CMPLIF_Clr(BTIMx_Type* BTIMx)
{
	BTIMx->BTIF = BTIMx_BTIF_CMPLIF_Msk;
}

/*********************************
basic timer计数器中断信号函数 
功能：获取BT计数器低位比较发生信号状态
输入: BTIM1/BTIM2  定时器名称	 
输出: SET/RESET 			当前计数器的值大于等工作寄存器的值/当前计数器的值小于工作寄存器的值
*********************************/
FlagStatus BTIMx_BTIF_CMPLIF_Chk(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTIF & BTIMx_BTIF_CMPLIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*********************************
basic timer计数器中断信号函数 
功能：清BT计数器高位溢出发生信号状态
输入: BTIM1/BTIM2  定时器名称
输出: 无
*********************************/
void BTIMx_BTIF_OVHIF_Clr(BTIMx_Type* BTIMx)
{
	BTIMx->BTIF = BTIMx_BTIF_OVHIF_Msk;
}
/*********************************
basic timer计数器中断信号函数 
功能：获取BT计数器高位溢出发生信号状态
输入: BTIM1/BTIM2  定时器名称	 
输出: SET/RESET 			产生计数溢出/未产生溢出
*********************************/
FlagStatus BTIMx_BTIF_OVHIF_Chk(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTIF & BTIMx_BTIF_OVHIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*********************************
basic timer计数器中断信号函数 
功能：清BT计数器低位溢出发生信号状态
输入: BTIM1/BTIM2  定时器名称
输出: 无
*********************************/
void BTIMx_BTIF_OVLIF_Clr(BTIMx_Type* BTIMx)
{
	BTIMx->BTIF = BTIMx_BTIF_OVLIF_Msk;
}
/*********************************
basic timer计数器中断信号函数 
功能：获取BT计数器低位溢出发生信号状态
输入: BTIM1/BTIM2  定时器名称	 
输出: SET/RESET 			产生计数溢出/未产生溢出
*********************************/
FlagStatus BTIMx_BTIF_OVLIF_Chk(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTIF & BTIMx_BTIF_OVLIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}


/*********************************
basic timer计数器中断信号函数 
功能：清BT计数器捕捉发生信号状态
输入: BTIM1/BTIM2  定时器名称
输出: 无
*********************************/
void BTIMx_BTIF_CAPIF_Clr(BTIMx_Type* BTIMx)
{
	BTIMx->BTIF = BTIMx_BTIF_CAPIF_Msk;
}
/*********************************
basic timer计数器中断信号函数 
功能：获取BT计数器捕捉发生信号状态
输入: BTIM1/BTIM2  定时器名称	 
输出: SET/RESET 			捕捉到指定的沿/未捕捉到指定的沿
*********************************/
FlagStatus BTIMx_BTIF_CAPIF_Chk(BTIMx_Type* BTIMx)
{
	if (BTIMx->BTIF & BTIMx_BTIF_CAPIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*********************************
basic timer计数器定义函数 
功能：关闭BT计数器所有功能(缺省定义)
输入: BTIM1/BTIM2  定时器名称	 
输出: 无
*********************************/
void BTIMx_Deinit(BTIMx_Type* BTIMx)
{
	BTIMx->BTCR1 = 0x00000000;
	BTIMx->BTCR2 = 0x00000000;
	BTIMx->BTCFG1 = 0x00000000;
	BTIMx->BTCFG2 = 0x00000000;
	BTIMx->BTPRES = 0x00000000;
	BTIMx->BTLOADCR = 0x00000000;
	//BTIMx->BTCNTL = ;
	//BTIMx->BTCNTH = ;
	BTIMx->BTPRESET = 0x00000000;
	BTIMx->BTLOADL = 0x00000000;
	BTIMx->BTLOADH = 0x00000000;
	//BTIMx->BTCMPL = ;
	//BTIMx->BTCMPH = ;
	BTIMx->BTOUTCNT = 0x00000000;
	BTIMx->BTOCR = 0x00000000;
	BTIMx->BTIE = 0x00000000;
	//BTIMx->BTIF = ;
}
//Code_End



void BTIMx_Init(BTIMx_Type* BTIMx, BTIM_InitTypeDef* para)
{
	BTIMx_Deinit(BTIMx);

	//捕捉参数选择
	BTIMx_BTCR1_CAPMOD_Set(BTIMx, para->cap_para.CAPMOD);//捕捉模式控制，脉冲宽度或脉冲周期
	BTIMx_BTCR1_CAPCLR_Set(BTIMx, para->cap_para.CAPCLR);//捕捉是否需要清零计数器
	BTIMx_BTCR1_CAPONCE_Set(BTIMx, para->cap_para.CAPONCE);//捕捉是否单次捕捉

	//信号选择类
	BTIMx_BTCFG1_RTCSEL1_Set(BTIMx, para->sig_src_para.RTCSEL1);//选择RTCOUT1信号选择
	BTIMx_BTCFG1_RTCSEL2_Set(BTIMx, para->sig_src_para.RTCSEL2);//选择RTCOUT2信号选择
	BTIMx_BTCFG2_INSEL1_Set(BTIMx, para->sig_src_para.INSEL1);//选择内部输入信号1
	BTIMx_BTCFG2_INSEL2_Set(BTIMx, para->sig_src_para.INSEL2);//选择内部输入信号2
    BTIMx_BTCFG2_EXSEL1_Set(BTIMx, para->sig_src_para.EXSEL1);//选择外部输入信号1
    BTIMx_BTCFG2_EXSEL2_Set(BTIMx, para->sig_src_para.EXSEL2);//选择外部输入信号2
	
	
	BTIMx_BTCFG1_GRP1SEL_Set(BTIMx, para->sig_src_para.GRP1SEL);//选择GROUP1信号选择
	BTIMx_BTCFG1_GRP2SEL_Set(BTIMx, para->sig_src_para.GRP2SEL);//选择GROUP2信号选择
	
	BTIMx_BTCR2_SIG2SEL_Set(BTIMx, para->sig_src_para.CNTSRC);//选择内部计数源信号
	BTIMx_BTCR2_SIG1SEL_Set(BTIMx, para->sig_src_para.CAPSRC);//选择内部捕捉源信号

	BTIMx_BTCR2_CNTHSEL_Set(BTIMx, para->sig_src_para.CNTHSEL);//高位计数器计数源选择
	BTIMx_BTCR2_SRCSEL_Set(BTIMx, para->sig_src_para.SRCSEL);//选择低位计数器是否长使能

	//DIR控制类
	BTIMx_BTCR2_DIREN_Setable(BTIMx, para->dir_para.DIREN);//外部dir控制或不使能
	//BTIMx_BTCR2_STDIR_Setable(BTIMx, para->dir_para.STDIR);//内部dir控制信号，这个应该不需要
	BTIMx_BTCR2_DIRPO_Set(BTIMx, para->dir_para.DIRPO);//外部输入dir信号极性选择


	//控制类参数
	BTIMx_BTCR1_MODE_Set(BTIMx, para->ctrl_para.MODE);//工作模式，16位捕捉还是8位定时计数
	BTIMx_BTCR1_EDGESEL_Set(BTIMx, para->ctrl_para.EDGESEL);//沿选择，上升沿还是下降沿
	BTIMx_BTPRES_Write(BTIMx, para->ctrl_para.PRESCALE);//选择分频系数

	BTIMx_BTPRESET_PRESETH_Set(BTIMx, para->ctrl_para.PRESETH);//高位预加载值
	BTIMx_BTPRESET_PRESETL_Set(BTIMx, para->ctrl_para.PRESETL);//低位预加载值

	BTIMx_BTLOADH_Write(BTIMx, para->ctrl_para.LOADH);//高位加载值
	BTIMx_BTLOADL_Write(BTIMx, para->ctrl_para.LOADL);//低位加载值
	
	//输出类参数
	BTIMx_BTOCR_OUTMOD_Set(BTIMx, para->out_para.OUTMOD);//输出模式选择
	BTIMx_BTOCR_OUTSEL_Set(BTIMx, para->out_para.OUTSEL);//输出信号选择
	BTIMx_BTOUTCNT_Write(BTIMx, para->out_para.OUTCNT);//输出脉冲宽度设置
	BTIMx_BTOCR_OUTINV_Set(BTIMx, para->out_para.OUTINV);//输出电平反向选择
	
}



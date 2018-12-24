/**
  ******************************************************************************
  * @file    fm33a0xx_wwdt.c
  * @author  FM33a0xx Application Team
  * @version V2.0.2
  * @date    8-1-2018
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of....:
  *
*/

/* Includes ------------------------------------------------------------------*/
#include "fm33A0xx_wwdt.h" 


/** @addtogroup fm33a0xx_StdPeriph_Driver
  * @{
  */

/** @defgroup WWDT 
  * @brief WWDT driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//Code_Start
//2018-04-02-13-22-31
//FM33A0XX_Driver_Gen_V1.4

/********************************
WWDT启动控制函数
功能:WWDT启动控制
输入：WWDT启动控制的值，写入0XAC
 ********************************/
void WWDT_WWDTCON_Write(uint32_t SetValue)
{
	WWDT->WWDTCON = (SetValue & WWDT_WWDTCON_WWDTCON_Msk);
}

/********************************
配置WWDT溢出时间函数
功能:配置WWDT溢出时间
输入：WWDT溢出时间的值
 ********************************/
void WWDT_WWDTCFG_WWDTCFG_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = WWDT->WWDTCFG;
	tmpreg &= ~(WWDT_WWDTCFG_WWDTCFG_Msk);
	tmpreg |= (SetValue & WWDT_WWDTCFG_WWDTCFG_Msk);
	WWDT->WWDTCFG = tmpreg;
}

/********************************
读取WWDT溢出时间函数
功能:读取WWDT溢出时间
输出：读取溢出时间的值
 ********************************/
uint32_t WWDT_WWDTCFG_WWDTCFG_Get(void)
{
	return (WWDT->WWDTCFG & WWDT_WWDTCFG_WWDTCFG_Msk);
}

/********************************
读取WWDT计数寄存器函数
功能:读取WWDT计数寄存器
输出：WWDT计数寄存器的值
 ********************************/
uint32_t WWDT_WWDTCNT_Read(void)
{
	return (WWDT->WWDTCNT & WWDT_WWDTCNT_WWDTCNT_Msk);
}

/* WWDT中断使能 相关函数 */
void WWDT_WWDTIE_WWDTIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		WWDT->WWDTIE |= (WWDT_WWDTIE_WWDTIE_Msk);
	}
	else
	{
		WWDT->WWDTIE &= ~(WWDT_WWDTIE_WWDTIE_Msk);
	}
}

FunState WWDT_WWDTIE_WWDTIE_Getable(void)
{
	if (WWDT->WWDTIE & (WWDT_WWDTIE_WWDTIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
清WWDT 75%计时中断标志函数
功能:清WWDT 75%计时中断标志
********************************/
void WWDT_WWDTIF_WWDTIF_Clr(void)
{
	WWDT->WWDTIF = WWDT_WWDTIF_WWDTIF_Msk;
}
/********************************
读取WWDT 75%计时中断标志函数
功能:读取WWDT 75%计时中断标志
输出：WWDT 75%计时中断标志
 ********************************/
FlagStatus WWDT_WWDTIF_WWDTIF_Chk(void)
{
	if (WWDT->WWDTIF & WWDT_WWDTIF_WWDTIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
WWDT的4096预分频设置函数
功能:设置WWDT的4096预分频
输出：WWDT的4096预分频的设置值
 ********************************/
uint32_t WWDT_WWDTDIV_Read(void)
{
	return (WWDT->WWDTDIV & WWDT_WWDTDIV_DIVCNT_Msk);
}

void WWDT_Deinit(void)
{
	//WWDT->WWDTCON = ;
	//WWDT->WWDTCFG = 0x00000000;
	//WWDT->WWDTCNT = ;
	//WWDT->WWDTIE = 0x00000000;
	//WWDT->WWDTIF = 0x00000000;
	//WWDT->WWDTDIV = ;
}
//Code_End


/********************************
WWDT启动函数
功能:WWDT的启动
 ********************************/
void WWDT_Start(void)
{
    WWDT->WWDTCON = WWDT_START_key;
}
/********************************
清狗WWDT函数
功能:清狗
 ********************************/
void WWDT_Clr(void)
{
    WWDT->WWDTCON = WWDT_CLR_key;
}

/******END OF FILE****/




/**
  ******************************************************************************
  * @file    fm33a00xx_iwdt.c
  * @author  FM33a0xx Application Team
  * @version V2.0.2
  * @date    
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of....:
  *
*/

/* Includes ------------------------------------------------------------------*/
#include "FM33A0XX_iwdt.h"  

/** @addtogroup fm33a0xx_StdPeriph_Driver
  * @{
  */

/** @defgroup IWDT 
  * @brief IWDT driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup IWDT_Private_Functions
  * @{
  */ 


//Code_Start
//2018-04-02-13-21-26
//FM33A0XX_Driver_Gen_V1.4

/********************************
IWDT清除函数
功能:清除IWDT
输入：写入0x1234_5A5A时清狗
 ********************************/
void IWDT_IWDTSERV_Write(uint32_t SetValue)
{
	IWDT->IWDTSERV = (SetValue);
}

/********************************
设置IWDT溢出周期4096S的函数
功能:设置IWDT溢出周期为4096S
输入：是否要设置4096S的逻辑值
 ********************************/
void IWDT_IWDTCFG_IWDTSLP4096S_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		IWDT->IWDTCFG |= (IWDT_IWDTCFG_IWDTSLP4096S_Msk);
	}
	else
	{
		IWDT->IWDTCFG &= ~(IWDT_IWDTCFG_IWDTSLP4096S_Msk);
	}
}
/********************************
读取IWDT溢出周期函数
功能:读取IWDT溢出周期是否是4096S
输出：4096S的逻辑值的设置
 ********************************/
FunState IWDT_IWDTCFG_IWDTSLP4096S_Getable(void)
{
	if (IWDT->IWDTCFG & (IWDT_IWDTCFG_IWDTSLP4096S_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
读取IWDT溢出周期设置函数
功能:读取IWDT溢出周期的设置
输出：IWDT溢出周期的设置值
 ********************************/
void IWDT_IWDTCFG_IWDTOVP_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = IWDT->IWDTCFG;
	tmpreg &= ~(IWDT_IWDTCFG_IWDTOVP_Msk);
	tmpreg |= (SetValue & IWDT_IWDTCFG_IWDTOVP_Msk);
	IWDT->IWDTCFG = tmpreg;
}
/********************************
读取IWDT溢出周期设置函数
功能:读取IWDT溢出周期的设置
输出：IWDT溢出周期的设置值
 ********************************/
uint32_t IWDT_IWDTCFG_IWDTOVP_Get(void)
{
	return (IWDT->IWDTCFG & IWDT_IWDTCFG_IWDTOVP_Msk);
}

/********************************
读取IWDT当前计数值函数
功能:读取IWDT当前计数值
输出：IWDT当前计数值
 ********************************/
uint32_t IWDT_IWDTCNT_Read(void)
{
	return (IWDT->IWDTCNT & IWDT_IWDTCNT_IWDTCNT_Msk);
}

/********************************
读取IWDT当前计数值函数
功能:读取IWDT当前计数值
输出：IWDT当前计数值
 ********************************/
void IWDT_Deinit(void)
{
	//IWDT->IWDTSERV = ;
	IWDT->IWDTCFG = 0x00000001;
	//IWDT->IWDTCNT = ;
}
//Code_End

/********************************
清狗函数
功能:清狗
********************************/
void IWDT_Clr(void)
{
    IWDT->IWDTSERV = WDTSERV_key;
//	IWDT_IWDTSERV_Write(WDTSERV_key);
}



/******END OF FILE****/




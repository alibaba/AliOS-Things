/**
  ******************************************************************************
  * @file    fm33a0xx_trng.c
  * @author  FM33a0xx Application Team
  * @version V2.0.2
  * @date    30-11-2017
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of....:
  *
*/

/* Includes ------------------------------------------------------------------*/
#include "fm33a0xx_trng.h" 


/** @addtogroup fm33a0xx_StdPeriph_Driver
  * @{
  */

/** @defgroup TRNG 
  * @brief TRNG driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//Code_Start
//2018-04-02-13-22-21
//FM33A0XX_Driver_Gen_V1.4

/* RNG使能寄存器，软件写1使能 相关函数 */
//void TRNG_TRNGCON_RNGEN_Setable(FunState NewState)
//{
//	if (NewState == ENABLE)
//	{
//		TRNG->TRNGCON |= (TRNG_TRNGCON_RNGEN_Msk);
//	}
//	else
//	{
//		TRNG->TRNGCON &= ~(TRNG_TRNGCON_RNGEN_Msk);
//	}
//}

//FunState TRNG_TRNGCON_RNGEN_Getable(void)
//{
//	if (TRNG->TRNGCON & (TRNG_TRNGCON_RNGEN_Msk))
//	{
//		return ENABLE;
//	}
//	else
//	{
//		return DISABLE;
//	}
//}

/********************************
TRNG功能控制函数
功能: 获取随机数输出
输入：无
输出: 随机数生成结果
********************************/
uint32_t TRNG_RNGOUT_Read(void)
{
	return (TRNG->RNGOUT);
}

/********************************
TRNG功能控制函数
功能: 配置随机数生成失败中断使能
输入：ENABLE/DISABLE		随机数生成失败中断使能/随机数生成失败中断禁止
输出: 无
********************************/
void TRNG_RNGIE_RNGIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		TRNG->RNGIE |= (TRNG_RNGIE_RNGIE_Msk);
	}
	else
	{
		TRNG->RNGIE &= ~(TRNG_RNGIE_RNGIE_Msk);
	}
}
/********************************
TRNG功能控制函数
功能: 获取随机数生成失败中断使能
输入：无
输出: ENABLE/DISABLE		随机数生成失败中断使能/随机数生成失败中断禁止
********************************/
FunState TRNG_RNGIE_RNGIE_Getable(void)
{
	if (TRNG->RNGIE & (TRNG_RNGIE_RNGIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
TRNG功能控制函数
功能: 获取随机数LFSR使能标志
输入：无
输出: SET/RESET				LFSR在运行中，不可进行CRC验证/LFSR不在运行中，可进行CRC验证
********************************/
FlagStatus TRNG_RNGIF_LFSREN_Chk(void)
{
	if (TRNG->RNGIF & TRNG_RNGIF_LFSREN_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
TRNG功能控制函数
功能: 清随机数LFSR使能标志
输入：无
输出: 无
********************************/
void TRNG_RNGIF_RNF_Clr(void)
{
	TRNG->RNGIF = TRNG_RNGIF_RNF_Msk;
}
/********************************
TRNG功能控制函数
功能: 获取随机数生成失败标志
输入：无
输出: SET/RESET				随机数未能通过质量检测/随机数通过质量检测
********************************/
FlagStatus TRNG_RNGIF_RNF_Chk(void)
{
	if (TRNG->RNGIF & TRNG_RNGIF_RNF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
TRNG功能控制函数
功能: 配置随机数CRC使能
输入：ENABLE/DISABLE		CRC使能/CRC关闭
输出: 无
********************************/
void TRNG_CRCCON_CRCEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		TRNG->CRCCON |= (TRNG_CRCCON_CRCEN_Msk);
	}
	else
	{
		TRNG->CRCCON &= ~(TRNG_CRCCON_CRCEN_Msk);
	}
}

/********************************
TRNG功能控制函数
功能: 获取随机数CRC使能
输入：无
输出: ENABLE/DISABLE		CRC使能/CRC关闭
********************************/
FunState TRNG_CRCCON_CRCEN_Getable(void)
{
	if (TRNG->CRCCON & (TRNG_CRCCON_CRCEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
TRNG功能控制函数
功能: 配置随机数CRC运算数据输入(仅当LFSREN=0有效)
输入：32位数据
输出: 无
********************************/
void TRNG_CRCIN_Write(uint32_t SetValue)
{
	TRNG->CRCIN = (SetValue);
}
/********************************
TRNG功能控制函数
功能: 获取随机数CRC运算数据输入
输入：无
输出: 32位数据
********************************/
uint32_t TRNG_CRCIN_Read(void)
{
	return (TRNG->CRCIN);
}

/********************************
TRNG功能控制函数
功能: 清CRC计算完成标志
输入：无
输出: 无
********************************/
void TRNG_CRCFLAG_CRCD_Clr(void)
{
	TRNG->CRCFLAG &= ~(TRNG_CRCFLAG_CRCD_Msk);
}
/********************************
TRNG功能控制函数
功能: 获取CRC计算完成标志
输入：无
输出: SET/RESET			CRC计算完成/CRC计算未完成
********************************/
FlagStatus TRNG_CRCFLAG_CRCD_Chk(void)
{
	if (TRNG->CRCFLAG & TRNG_CRCFLAG_CRCD_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
TRNG功能控制函数
功能: 缺省配置
输入：无
输出: 无
********************************/
void TRNG_Deinit(void)
{
	ANAC->TRNGCON = 0x00000000;//TRNG->TRNGCON = 0x00000000;
	//TRNG->RNGOUT = ;
	TRNG->RNGIE = 0x00000000;
	//TRNG->RNGIF = ;
	TRNG->CRCCON = 0x00000000;
	TRNG->CRCIN = 0x00000000;
	TRNG->CRCFLAG = 0x00000000;
}
//Code_End



/******END OF FILE****/




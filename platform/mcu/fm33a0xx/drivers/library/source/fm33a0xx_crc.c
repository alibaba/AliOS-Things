/**
  ******************************************************************************
  * @file    fm33a0xx_crc.c
  * @author  FM33a0xx Application Team
  * @version V2.0.2
  * @date    8-1-2018
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of....:
  *
*/

/* Includes ------------------------------------------------------------------*/
#include "fm33A0xx_crc.h" 


/** @addtogroup fm33a0xx_StdPeriph_Driver
  * @{
  */

/** @defgroup CRC
  * @brief CRC driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//Code_Start
//2018-04-02-13-20-31
//FM33A0XX_Driver_Gen_V1.4

/********************************
CRC 功能控制函数
功能: 填充CRC数据输入(仅低16位有效)
输入：CRC输入
输出: 无
********************************/
void CRC_CRCDR_Write(uint32_t SetValue)
{
	CRC->CRCDR = (SetValue & CRC_CRCDR_CRCDR_Msk);
}
/********************************
CRC 功能控制函数
功能: 获取CRC数据输入(仅低16位有效)
输入：无
输出: CRC输入
********************************/
uint32_t CRC_CRCDR_Read(void)
{
	return (CRC->CRCDR & CRC_CRCDR_CRCDR_Msk);
}

/********************************
CRC 功能控制函数
功能: 配置CRC输入反转
输入：ENABLE/DISABLE			输入按字节反转/输入不反转
输出: 无
********************************/
void CRC_CRCCR_RFLTIN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		CRC->CRCCR |= (CRC_CRCCR_RFLTIN_Msk);
	}
	else
	{
		CRC->CRCCR &= ~(CRC_CRCCR_RFLTIN_Msk);
	}
}
/********************************
CRC 功能控制函数
功能: 获取CRC输入反转配置
输入：无
输出: ENABLE/DISABLE			输入按字节反转/输入不反转
********************************/
FunState CRC_CRCCR_RFLTIN_Getable(void)
{
	if (CRC->CRCCR & (CRC_CRCCR_RFLTIN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
CRC 功能控制函数
功能: 配置CRC输出反转
输入：ENABLE/DISABLE			输出按字节反转/输出不反转
输出: 无
********************************/
void CRC_CRCCR_RFLTO_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		CRC->CRCCR |= (CRC_CRCCR_RFLTO_Msk);
	}
	else
	{
		CRC->CRCCR &= ~(CRC_CRCCR_RFLTO_Msk);
	}
}
/********************************
CRC 功能控制函数
功能: 获取CRC输出反转配置
输入：无
输出: ENABLE/DISABLE			输出按字节反转/输出不反转
********************************/
FunState CRC_CRCCR_RFLTO_Getable(void)
{
	if (CRC->CRCCR & (CRC_CRCCR_RFLTO_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/********************************
CRC 功能控制函数
功能: 获取CRC结果标志位
输入：无
输出:	SET/RESET		CRC结果非全0/CRC结果全0
********************************/
FlagStatus CRC_CRCCR_RES_Chk(void)
{
	if (CRC->CRCCR & CRC_CRCCR_RES_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
CRC 功能控制函数
功能: 获取CRC运算标志位
输入：无
输出:	SET/RESET		CRC运算进行中/CRC运算结束
********************************/
FlagStatus CRC_CRCCR_BUSY_Chk(void)
{
	if (CRC->CRCCR & CRC_CRCCR_BUSY_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
CRC 功能控制函数
功能: 配置CRC输出异或使能
输入：ENABLE/DISABLE			输出异或CRC_XOR/输出不异或CRC_XOR
输出: 无
********************************/
void CRC_CRCCR_XOR_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		CRC->CRCCR |= (CRC_CRCCR_XOR_Msk);
	}
	else
	{
		CRC->CRCCR &= ~(CRC_CRCCR_XOR_Msk);
	}
}
/********************************
CRC 功能控制函数
功能: 获取CRC输出异或配置
输入：无
输出: ENABLE/DISABLE			输出异或CRC_XOR/输出不异或CRC_XOR
********************************/
FunState CRC_CRCCR_XOR_Getable(void)
{
	if (CRC->CRCCR & (CRC_CRCCR_XOR_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
CRC 功能控制函数
功能: 配置CRC校验多项式选择
输入：CRC_CRCCR_CRCSEL_CRC16			00：CRC16
			CRC_CRCCR_CRCSEL_CRC8				01：CRC8
			CRC_CRCCR_CRCSEL_CCITT			10/11：CCITT
输出: 无
********************************/
void CRC_CRCCR_CRCSEL_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = CRC->CRCCR;
	tmpreg &= ~(CRC_CRCCR_CRCSEL_Msk);
	tmpreg |= (SetValue & CRC_CRCCR_CRCSEL_Msk);
	CRC->CRCCR = tmpreg;
}
/********************************
CRC 功能控制函数
功能: 获取CRC校验多项式选择
输入：无
输出: CRC_CRCCR_CRCSEL_CRC16			00：CRC16
			CRC_CRCCR_CRCSEL_CRC8				01：CRC8
			CRC_CRCCR_CRCSEL_CCITT			10/11：CCITT
********************************/
uint32_t CRC_CRCCR_CRCSEL_Get(void)
{
	return (CRC->CRCCR & CRC_CRCCR_CRCSEL_Msk);
}

/********************************
CRC 功能控制函数
功能: 配置CRC运算初值(运算前可配置)
输入：0x00000000-0x0000ffff
输出: 无
********************************/
void CRC_CRCLFSR_Write(uint32_t SetValue)
{
	CRC->CRCLFSR = (SetValue & CRC_CRCLFSR_LFSR_Msk);
}
/********************************
CRC 功能控制函数
功能: 获取CRC运算初值
输入：无
输出: CRC运算初值
********************************/
uint32_t CRC_CRCLFSR_Read(void)
{
	return (CRC->CRCLFSR & CRC_CRCLFSR_LFSR_Msk);
}

/********************************
CRC 功能控制函数
功能: 配置CRC输出异或值
输入：0x00000000-0x0000ffff
输出: 无
********************************/
void CRC_CRCXOR_Write(uint32_t SetValue)
{
	CRC->CRCXOR = (SetValue & CRC_CRCXOR_CRC_XOR_Msk);
}
/********************************
CRC 功能控制函数
功能: 获取CRC输出异或值
输入：无
输出: 0x00000000-0x0000ffff
********************************/
uint32_t CRC_CRCXOR_Read(void)
{
	return (CRC->CRCXOR & CRC_CRCXOR_CRC_XOR_Msk);
}

/********************************
CRC 功能控制函数
功能: 配置CRC校验使能
输入：ENABLE/DISABLE		启动Flash CRC校验/不使能Flash CRC校验
输出: 无
********************************/
void CRC_CRCFLSEN_FLSCRCEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		CRC->CRCFLSEN |= (CRC_CRCFLSEN_FLSCRCEN_Msk);
	}
	else
	{
		CRC->CRCFLSEN &= ~(CRC_CRCFLSEN_FLSCRCEN_Msk);
	}
}
/********************************
CRC 功能控制函数
功能: 获取CRC校验使能配置
输入：无
输出: ENABLE/DISABLE		启动Flash CRC校验/不使能Flash CRC校验
********************************/
FunState CRC_CRCFLSEN_FLSCRCEN_Getable(void)
{
	if (CRC->CRCFLSEN & (CRC_CRCFLSEN_FLSCRCEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
CRC 功能控制函数
功能: 配置FLASH CRC校验起始地址
输入：17位Flash地址(word地址)
输出: 无
********************************/
void CRC_CRCFLSAD_Write(uint32_t SetValue)
{
	CRC->CRCFLSAD = (SetValue & CRC_CRCFLSAD_FLSAD_Msk);
}
/********************************
CRC 功能控制函数
功能: 获取FLASH CRC校验起始地址
输入：无
输出: 17位Flash地址(word地址)
********************************/
uint32_t CRC_CRCFLSAD_Read(void)
{
	return (CRC->CRCFLSAD & CRC_CRCFLSAD_FLSAD_Msk);
}

/********************************
CRC 功能控制函数
功能: 配置FLASH CRC校验长度
输入：17位数据长度(word地址,实际运算长度为(FLSS+1)*4字节)
输出: 无
********************************/
void CRC_CRCFLSSIZE_Write(uint32_t SetValue)
{
	CRC->CRCFLSSIZE = (SetValue & CRC_CRCFLSSIZE_FLSS_Msk);
}
/********************************
CRC 功能控制函数
功能: 获取FLASH CRC校验长度
输入：无
输出: 17位数据长度(word地址,实际运算长度为(FLSS+1)*4字节)
********************************/
uint32_t CRC_CRCFLSSIZE_Read(void)
{
	return (CRC->CRCFLSSIZE & CRC_CRCFLSSIZE_FLSS_Msk);
}

/********************************
CRC 功能控制函数
功能: CRC 缺省配置
输入：无
输出: 无
********************************/
void CRC_Deinit(void)
{
	//CRC->CRCDR = ;
	CRC->CRCCR = 0x00000000;
	CRC->CRCLFSR = 0x0000FFFF;
	CRC->CRCXOR = 0x00000000;
	CRC->CRCFLSEN = 0x00000000;
	CRC->CRCFLSAD = 0x00000000;
	CRC->CRCFLSSIZE = 0x00000000;
}
//Code_End

/* CRC初始化配置函数*/
void CRC_Init(CRC_InitTypeDef* para)
{
	CRC_CRCCR_CRCSEL_Set(para->CRCSEL);			/*!<CRC校验多项式选择*/
	CRC_CRCCR_RFLTIN_Setable(para->RFLTIN);		/*!<CRC输入反转控制*/
	CRC_CRCCR_RFLTO_Setable(para->RFLTO);		/*!<CRC输出反转控制*/
	CRC_CRCCR_XOR_Setable(para->XOR);			/*!<输出异或使能*/
	CRC_CRCXOR_Write(para->CRC_XOR);			/*!<运算结果异或寄存器*/
}

/******END OF FILE****/




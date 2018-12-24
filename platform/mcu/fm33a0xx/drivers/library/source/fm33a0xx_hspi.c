/**
  ******************************************************************************
  * @file    fm33a0xx_spi.c
  * @author  
  * @version V2.0.2
  * @date    
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of SPI:           
  *           
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/ 

#include "fm33A0xx_hspi.h"  



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//Code_Start
//2018-04-02-13-22-40
//FM33A0XX_Driver_Gen_V1.4

/********************************
HSPI 控制函数
功能: 配置Master下延迟采样时间或Slave下提前发送时间
输入：	HSPI_SPICR1_DELAY_CFG_MASTER_NO_DELAY		Master模式，无延迟 *
	HSPI_SPICR1_DELAY_CFG_SLAVE_NORMAL		Slave模式，正常发送 
	HSPI_SPICR1_DELAY_CFG_MASTER_DLY_L1		Master模式，1级延迟 
	HSPI_SPICR1_DELAY_CFG_SLAVE_PHASE_1		Slave模式，发送相位1
	HSPI_SPICR1_DELAY_CFG_MASTER_DLY_L2		Master模式，2级延迟 
	HSPI_SPICR1_DELAY_CFG_SLAVE_PHASE_2		Slave模式，发送相位2
	HSPI_SPICR1_DELAY_CFG_MASTER_DLY_L3		Master模式，3级延迟 
	HSPI_SPICR1_DELAY_CFG_SLAVE_PHASE_3		Slave模式，发送相位3
	HSPI_SPICR1_DELAY_CFG_MASTER_DLY_L4		Master模式，4级延迟 
	HSPI_SPICR1_DELAY_CFG_SLAVE_PHASE_4		Slave模式，发送相位4 

输出:无
********************************/
void HSPI_SPICR1_DELAY_CFG_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = HSPI->SPICR1;
	tmpreg &= ~(HSPI_SPICR1_DELAY_CFG_Msk);
	tmpreg |= (SetValue & HSPI_SPICR1_DELAY_CFG_Msk);
	HSPI->SPICR1 = tmpreg;
}
/********************************
HSPI 控制函数
功能: 获取Master下延迟采样时间或Slave下提前发送时间
输入：无 
输出:	HSPI_SPICR1_DELAY_CFG_MASTER_NO_DELAY		Master模式，无延迟 *
	HSPI_SPICR1_DELAY_CFG_SLAVE_NORMAL		Slave模式，正常发送 
	HSPI_SPICR1_DELAY_CFG_MASTER_DLY_L1		Master模式，1级延迟 
	HSPI_SPICR1_DELAY_CFG_SLAVE_PHASE_1		Slave模式，发送相位1
	HSPI_SPICR1_DELAY_CFG_MASTER_DLY_L2		Master模式，2级延迟 
	HSPI_SPICR1_DELAY_CFG_SLAVE_PHASE_2		Slave模式，发送相位2
	HSPI_SPICR1_DELAY_CFG_MASTER_DLY_L3		Master模式，3级延迟 
	HSPI_SPICR1_DELAY_CFG_SLAVE_PHASE_3		Slave模式，发送相位3
	HSPI_SPICR1_DELAY_CFG_MASTER_DLY_L4		Master模式，4级延迟 
	HSPI_SPICR1_DELAY_CFG_SLAVE_PHASE_4		Slave模式，发送相位4
********************************/
uint32_t HSPI_SPICR1_DELAY_CFG_Get(void)
{
	return (HSPI->SPICR1 & HSPI_SPICR1_DELAY_CFG_Msk);
}


/********************************
HSPI 控制函数
功能: 配置工作模式
输入：	HSPI_SPICR1_MM_SLAVE		SLAVE
	HSPI_SPICR1_MM_MASTER		MASTER

输出:无
********************************/
void HSPI_SPICR1_MM_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = HSPI->SPICR1;
	tmpreg &= ~(HSPI_SPICR1_MM_Msk);
	tmpreg |= (SetValue & HSPI_SPICR1_MM_Msk);
	HSPI->SPICR1 = tmpreg;
}

/********************************
HSPI 控制函数
功能: 获取工作模式
输入：无 
输出:	HSPI_SPICR1_MM_SLAVE		SLAVE
	HSPI_SPICR1_MM_MASTER		MASTER
********************************/
uint32_t HSPI_SPICR1_MM_Get(void)
{
	return (HSPI->SPICR1 & HSPI_SPICR1_MM_Msk);
}
/********************************
HSPI 控制函数
功能: 配置发送完每个byte之后的等待时间
输入：	HSPI_SPICR1_WAIT_WAIT_1		每发完8bit至少加入1个sck周期等待时间再传输下一个8Bit数据		
	HSPI_SPICR1_WAIT_WAIT_2		每发完8bit至少加入2个sck周期等待时间再传输下一个8Bit数据
	HSPI_SPICR1_WAIT_WAIT_3		每发完8bit至少加入3个sck周期等待时间再传输下一个8Bit数据
	HSPI_SPICR1_WAIT_WAIT_4		每发完8bit至少加入4个sck周期等待时间再传输下一个8Bit数据
输出:无
********************************/
void HSPI_SPICR1_WAIT_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = HSPI->SPICR1;
	tmpreg &= ~(HSPI_SPICR1_WAIT_Msk);
	tmpreg |= (SetValue & HSPI_SPICR1_WAIT_Msk);
	HSPI->SPICR1 = tmpreg;
}
/********************************
HSPI 控制函数
功能: 获取发送完每个byte之后的等待时间
输入：无 
输出:	HSPI_SPICR1_WAIT_WAIT_1		每发完8bit至少加入1个sck周期等待时间再传输下一个8Bit数据		
			HSPI_SPICR1_WAIT_WAIT_2		每发完8bit至少加入2个sck周期等待时间再传输下一个8Bit数据
			HSPI_SPICR1_WAIT_WAIT_3		每发完8bit至少加入3个sck周期等待时间再传输下一个8Bit数据
			HSPI_SPICR1_WAIT_WAIT_4		每发完8bit至少加入4个sck周期等待时间再传输下一个8Bit数据
********************************/
uint32_t HSPI_SPICR1_WAIT_Get(void)
{
	return (HSPI->SPICR1 & HSPI_SPICR1_WAIT_Msk);
}

/********************************
HSPI 控制函数
功能: 配置MASTER模式下的波特率
输入：HSPI_SPICR1_BAUD_PCLK					外设时钟不分频
			HSPI_SPICR1_BAUD_PCLK_2				外设时钟2分频
			HSPI_SPICR1_BAUD_PCLK_4				外设时钟4分频
			HSPI_SPICR1_BAUD_PCLK_8				外设时钟8分频
			HSPI_SPICR1_BAUD_PCLK_16			外设时钟16分频
			HSPI_SPICR1_BAUD_PCLK_32			外设时钟32分频
			HSPI_SPICR1_BAUD_PCLK_64			外设时钟64分频
			HSPI_SPICR1_BAUD_PCLK_128			外设时钟128分频
输出:无
********************************/
void HSPI_SPICR1_BAUD_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = HSPI->SPICR1;
	tmpreg &= ~(HSPI_SPICR1_BAUD_Msk);
	tmpreg |= (SetValue & HSPI_SPICR1_BAUD_Msk);
	HSPI->SPICR1 = tmpreg;
}
/********************************
HSPI 控制函数
功能: 获取MASTER模式下的波特率
输入：无 
输出:	HSPI_SPICR1_BAUD_PCLK					外设时钟不分频
			HSPI_SPICR1_BAUD_PCLK_2				外设时钟2分频
			HSPI_SPICR1_BAUD_PCLK_4				外设时钟4分频
			HSPI_SPICR1_BAUD_PCLK_8				外设时钟8分频
			HSPI_SPICR1_BAUD_PCLK_16			外设时钟16分频
			HSPI_SPICR1_BAUD_PCLK_32			外设时钟32分频
			HSPI_SPICR1_BAUD_PCLK_64			外设时钟64分频
			HSPI_SPICR1_BAUD_PCLK_128			外设时钟128分频
********************************/
uint32_t HSPI_SPICR1_BAUD_Get(void)
{
	return (HSPI->SPICR1 & HSPI_SPICR1_BAUD_Msk);
}

/********************************
HSPI 控制函数
功能: 配置帧格式
输入：HSPI_SPICR1_LSBF_MSB		先发送MSB
			HSPI_SPICR1_LSBF_LSB		先发送LSB
输出:无
********************************/
void HSPI_SPICR1_LSBF_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = HSPI->SPICR1;
	tmpreg &= ~(HSPI_SPICR1_LSBF_Msk);
	tmpreg |= (SetValue & HSPI_SPICR1_LSBF_Msk);
	HSPI->SPICR1 = tmpreg;
}
/********************************
HSPI 控制函数
功能: 获取帧格式
输入：无 
输出:	HSPI_SPICR1_LSBF_MSB		先发送MSB
			HSPI_SPICR1_LSBF_LSB		先发送LSB
********************************/
uint32_t HSPI_SPICR1_LSBF_Get(void)
{
	return (HSPI->SPICR1 & HSPI_SPICR1_LSBF_Msk);
}

/********************************
HSPI 控制函数
功能: 配置时钟极性
输入：HSPI_SPICR1_CPHOL_LOW		串行时钟停止在低电平
			HSPI_SPICR1_CPHOL_HIGH	串行时钟停止在高电平
输出:无
********************************/
void HSPI_SPICR1_CPHOL_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = HSPI->SPICR1;
	tmpreg &= ~(HSPI_SPICR1_CPHOL_Msk);
	tmpreg |= (SetValue & HSPI_SPICR1_CPHOL_Msk);
	HSPI->SPICR1 = tmpreg;
}
/********************************
HSPI 控制函数
功能: 获取时钟极性
输入：无 
输出:	HSPI_SPICR1_CPHOL_LOW		串行时钟停止在低电平
			HSPI_SPICR1_CPHOL_HIGH	串行时钟停止在高电平
********************************/
uint32_t HSPI_SPICR1_CPHOL_Get(void)
{
	return (HSPI->SPICR1 & HSPI_SPICR1_CPHOL_Msk);
}

/********************************
HSPI 控制函数
功能: 配置时钟相位
输入：HSPI_SPICR1_CPHA_FIRST			第一个时钟边沿是第一个捕捉边沿
	    HSPI_SPICR1_CPHA_SECOND			第二个时钟边沿是第一个捕捉边沿
输出:无
********************************/
void HSPI_SPICR1_CPHA_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = HSPI->SPICR1;
	tmpreg &= ~(HSPI_SPICR1_CPHA_Msk);
	tmpreg |= (SetValue & HSPI_SPICR1_CPHA_Msk);
	HSPI->SPICR1 = tmpreg;
}
/********************************
HSPI 控制函数
功能: 获取时钟相位
输入：无 
输出:	HSPI_SPICR1_CPHA_FIRST			第一个时钟边沿是第一个捕捉边沿
	    HSPI_SPICR1_CPHA_SECOND			第二个时钟边沿是第一个捕捉边沿
********************************/
uint32_t HSPI_SPICR1_CPHA_Get(void)
{
	return (HSPI->SPICR1 & HSPI_SPICR1_CPHA_Msk);
}

/********************************
HSPI 控制函数
功能: 配置SLAVE下输入管脚滤波使能
输入：ENABLE/DISABLE		输入管脚有滤波/输入管脚无滤波
输出:无
********************************/
void HSPI_SPICR2_FLTEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		HSPI->SPICR2 |= (HSPI_SPICR2_FLTEN_Msk);
	}
	else
	{
		HSPI->SPICR2 &= ~(HSPI_SPICR2_FLTEN_Msk);
	}
}
/********************************
HSPI 控制函数
功能: 获取SLAVE下输入管脚滤波是否使能
输入：无
输出: ENABLE/DISABLE		输入管脚有滤波/输入管脚无滤波
********************************/
FunState HSPI_SPICR2_FLTEN_Getable(void)
{
	if (HSPI->SPICR2 & (HSPI_SPICR2_FLTEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
HSPI 控制函数
功能: 配置MASTER模式下SSN的控制模式
输入：HSPI_SPICR2_SSNM_LOW			每发送完8bit后Master保持SSN为低
      HSPI_SPICR2_SSNM_HIGH			每发送完8bit后Master拉高SSN，维持高电平时间由WAIT寄存器控制
输出:无
********************************/
void HSPI_SPICR2_SSNM_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = HSPI->SPICR2;
	tmpreg &= ~(HSPI_SPICR2_SSNM_Msk);
	tmpreg |= (SetValue & HSPI_SPICR2_SSNM_Msk);
	HSPI->SPICR2 = tmpreg;
}

/********************************
HSPI 控制函数
功能: 获取MASTER模式下SSN的控制模式
输入：无
输出: HSPI_SPICR2_SSNM_LOW			每发送完8bit后Master保持SSN为低
      HSPI_SPICR2_SSNM_HIGH			每发送完8bit后Master拉高SSN，维持高电平时间由WAIT寄存器控制
********************************/
uint32_t HSPI_SPICR2_SSNM_Get(void)
{
	return (HSPI->SPICR2 & HSPI_SPICR2_SSNM_Msk);
}

/********************************
HSPI 控制函数
功能: 配置TXONLY硬件自动清空的使能
输入：HSPI_SPICR2_TXO_AC_CLR_AT_CLOSE			关闭TXONLY硬件自动清零
	    HSPI_SPICR2_TXO_AC_CLR_AT_TX				TXONLY硬件自动清零有效，软件使能TXO后，等待发送完毕后，硬件清零
输出:无
********************************/
void HSPI_SPICR2_TXO_AC_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = HSPI->SPICR2;
	tmpreg &= ~(HSPI_SPICR2_TXO_AC_Msk);
	tmpreg |= (SetValue & HSPI_SPICR2_TXO_AC_Msk);
	HSPI->SPICR2 = tmpreg;
}
/********************************
HSPI 控制函数
功能: 获取TXONLY硬件自动清空的使能配置
输入：无
输出: HSPI_SPICR2_TXO_AC_CLR_AT_CLOSE			关闭TXONLY硬件自动清零
	    HSPI_SPICR2_TXO_AC_CLR_AT_TX				TXONLY硬件自动清零有效，软件使能TXO后，等待发送完毕后，硬件清零
********************************/
uint32_t HSPI_SPICR2_TXO_AC_Get(void)
{
	return (HSPI->SPICR2 & HSPI_SPICR2_TXO_AC_Msk);
}

/********************************
HSPI 控制函数
功能: 配置TXONLY使能(MASTER下单发模式控制)
输入：ENABLE/DISABLE		启动Master的单发送模式/关闭单发送模式
输出:无
********************************/
void HSPI_SPICR2_TXO_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		HSPI->SPICR2 |= (HSPI_SPICR2_TXO_Msk);
	}
	else
	{
		HSPI->SPICR2 &= ~(HSPI_SPICR2_TXO_Msk);
	}
}
/********************************
HSPI 控制函数
功能: 获取TXONLY使能状态(MASTER下单发模式控制)
输入：无
输出: ENABLE/DISABLE		启动Master的单发送模式/关闭单发送模式
********************************/
FunState HSPI_SPICR2_TXO_Getable(void)
{
	if (HSPI->SPICR2 & (HSPI_SPICR2_TXO_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
HSPI 控制函数
功能: 配置MASTER下SSN的输出电平
输入：HSPI_SPICR2_SSN_LOW 	SSN输出低电平
	    HSPI_SPICR2_SSN_HIGH	SSN输出高电平
输出:无
********************************/
void HSPI_SPICR2_SSN_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = HSPI->SPICR2;
	tmpreg &= ~(HSPI_SPICR2_SSN_Msk);
	tmpreg |= (SetValue & HSPI_SPICR2_SSN_Msk);
	HSPI->SPICR2 = tmpreg;
}
/********************************
HSPI 控制函数
功能: 获取MASTER下SSN的输出电平配置
输入：无
输出: HSPI_SPICR2_SSN_LOW 	SSN输出低电平
	    HSPI_SPICR2_SSN_HIGH	SSN输出高电平
********************************/
uint32_t HSPI_SPICR2_SSN_Get(void)
{
	return (HSPI->SPICR2 & HSPI_SPICR2_SSN_Msk);
}

/********************************
HSPI 控制函数
功能: 配置MASTER下SSN的控制方式
输入：HSPI_SPICR2_SSNSEN_HARD		Master模式下SSN输出由硬件自动控制
	    HSPI_SPICR2_SSNSEN_SOFT		Master模式下SSN输出由软件控制
输出:无
********************************/
void HSPI_SPICR2_SSNSEN_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = HSPI->SPICR2;
	tmpreg &= ~(HSPI_SPICR2_SSNSEN_Msk);
	tmpreg |= (SetValue & HSPI_SPICR2_SSNSEN_Msk);
	HSPI->SPICR2 = tmpreg;
}
/********************************
HSPI 控制函数
功能: 获取MASTER下SSN的控制方式配置
输入：无
输出: HSPI_SPICR2_SSNSEN_HARD		Master模式下SSN输出由硬件自动控制
	    HSPI_SPICR2_SSNSEN_SOFT		Master模式下SSN输出由软件控制
********************************/
uint32_t HSPI_SPICR2_SSNSEN_Get(void)
{
	return (HSPI->SPICR2 & HSPI_SPICR2_SSNSEN_Msk);
}

/********************************
HSPI 控制函数
功能: HSPI使能控制
输入：ENABLE/DISABLE	HSPI使能/HSPI禁止
输出:无
********************************/
void HSPI_SPICR2_HSPIEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		HSPI->SPICR2 |= (HSPI_SPICR2_HSPIEN_Msk);
	}
	else
	{
		HSPI->SPICR2 &= ~(HSPI_SPICR2_HSPIEN_Msk);
	}
}
/********************************
HSPI 控制函数
功能: 获取HSPI使能控制状态
输入：无
输出: ENABLE/DISABLE	HSPI使能/HSPI禁止
********************************/
FunState HSPI_SPICR2_HSPIEN_Getable(void)
{
	if (HSPI->SPICR2 & (HSPI_SPICR2_HSPIEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/********************************
HSPI 控制函数
功能: 清除发送缓存
输入：无
输出: 无
********************************/
void HSPI_SPICR3_TXBFC_Clr(void)
{
	HSPI->SPICR3 = HSPI_SPICR3_TXBFC_Msk;
}
/********************************
HSPI 控制函数
功能: 清除接收缓存
输入：无
输出: 无
********************************/
void HSPI_SPICR3_RXBFC_Clr(void)
{
	HSPI->SPICR3 = HSPI_SPICR3_RXBFC_Msk;
}
/********************************
HSPI 控制函数
功能: 清除MASTER ERR标志(HSPISTA.MERR)
输入：无
输出: 无
********************************/
void HSPI_SPICR3_MERRC_Clr(void)
{
	HSPI->SPICR3 = HSPI_SPICR3_MERRC_Msk;
}
/********************************
HSPI 控制函数
功能: 清除SLAVE ERR标志(HSPISTA.SERR)
输入：HSPI_SPICR3_SERRC_Msk
输出:无
********************************/
void HSPI_SPICR3_SERRC_Clr(void)
{
	HSPI->SPICR3 = HSPI_SPICR3_SERRC_Msk;
}
/********************************
HSPI 控制函数
功能: 配置HSPI错误中断使能
输入：ENABLE/DISABLE			错误中断使能/错误中断禁止
输出:无
********************************/
void HSPI_SPIIE_ERRIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		HSPI->SPIIE |= (HSPI_SPIIE_ERRIE_Msk);
	}
	else
	{
		HSPI->SPIIE &= ~(HSPI_SPIIE_ERRIE_Msk);
	}
}
/********************************
HSPI 控制函数
功能: 获取HSPI错误中断使能状态
输入：无
输出: ENABLE/DISABLE			错误中断使能/错误中断禁止
********************************/
FunState HSPI_SPIIE_ERRIE_Getable(void)
{
	if (HSPI->SPIIE & (HSPI_SPIIE_ERRIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/********************************
HSPI 控制函数
功能: 配置HSPI发送中断使能
输入：ENABLE/DISABLE			发送中断使能/错误中断禁止
输出:无
********************************/
void HSPI_SPIIE_TXIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		HSPI->SPIIE |= (HSPI_SPIIE_TXIE_Msk);
	}
	else
	{
		HSPI->SPIIE &= ~(HSPI_SPIIE_TXIE_Msk);
	}
}
/********************************
HSPI 控制函数
功能: 获取HSPI发送中断使能状态
输入：无
输出: ENABLE/DISABLE			发送中断使能/错误中断禁止
********************************/
FunState HSPI_SPIIE_TXIE_Getable(void)
{
	if (HSPI->SPIIE & (HSPI_SPIIE_TXIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/********************************
HSPI 控制函数
功能: 配置HSPI接收中断使能
输入：ENABLE/DISABLE			接收中断使能/错误中断禁止
输出:无
********************************/
void HSPI_SPIIE_RXIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		HSPI->SPIIE |= (HSPI_SPIIE_RXIE_Msk);
	}
	else
	{
		HSPI->SPIIE &= ~(HSPI_SPIIE_RXIE_Msk);
	}
}
/********************************
HSPI 控制函数
功能: 获取HSPI接收中断使能状态
输入：无
输出: ENABLE/DISABLE			接收中断使能/错误中断禁止
********************************/
FunState HSPI_SPIIE_RXIE_Getable(void)
{
	if (HSPI->SPIIE & (HSPI_SPIIE_RXIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
HSPI 控制函数
功能: 获取HSPI MASTER错误中断标志
输入：无
输出: SET/RESET					HSPI错误MASTER中断发生/HSPI MASTER错误中断未发生
********************************/
FlagStatus HSPI_SPIIF_MERR_Chk(void)
{
	if (HSPI->SPIIF & HSPI_SPIIF_MERR_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
HSPI 控制函数
功能: 获取HSPI SLAVE错误中断标志
输入：无
输出: SET/RESET					HSPI SLAVE错误中断发生/HSPI SLAVE错误中断未发生
********************************/
FlagStatus HSPI_SPIIF_SERR_Chk(void)
{
	if (HSPI->SPIIF & HSPI_SPIIF_SERR_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}
/********************************
HSPI 控制函数
功能: 清接收缓存溢出
输入：无
输出: 无
********************************/
void HSPI_SPIIF_RXCOL_Clr(void)
{
	HSPI->SPIIF = HSPI_SPIIF_RXCOL_Msk;
}
/********************************
HSPI 控制函数
功能: 获取接收缓存溢出标志
输入：无
输出: SET/RESET			发生接收缓存溢出/未发生接收缓存溢出
********************************/
FlagStatus HSPI_SPIIF_RXCOL_Chk(void)
{
	if (HSPI->SPIIF & HSPI_SPIIF_RXCOL_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
HSPI 控制函数
功能: 清发送缓存溢出
输入：无
输出: 无
********************************/
void HSPI_SPIIF_TXCOL_Clr(void)
{
	HSPI->SPIIF = HSPI_SPIIF_TXCOL_Msk;
}
/********************************
HSPI 控制函数
功能: 获取发送缓存溢出标志
输入：无
输出: SET/RESET	发生发送缓存溢出/未发生发送缓存溢出
********************************/
FlagStatus HSPI_SPIIF_TXCOL_Chk(void)
{
	if (HSPI->SPIIF & HSPI_SPIIF_TXCOL_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
HSPI 控制函数
功能: 获取HSPI空闲标志
输入：无
输出: SET/RESET	HSPI传输中/HSPI空闲
********************************/
FlagStatus HSPI_SPIIF_BUSY_Chk(void)
{
	if (HSPI->SPIIF & HSPI_SPIIF_BUSY_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
HSPI 控制函数
功能: 获取发送缓存TX buffer空标志
输入：无
输出: SET/RESET	TX buffer 空/TX buffer 满
********************************/
FlagStatus HSPI_SPIIF_TXBE_Chk(void)
{
	if (HSPI->SPIIF & HSPI_SPIIF_TXBE_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
HSPI 控制函数
功能: 获取接收缓存RX buffer满标志
输入：无
输出: SET/RESET	RX buffer 满/RX buffer 空
********************************/
FlagStatus HSPI_SPIIF_RXBF_Chk(void)
{
	if (HSPI->SPIIF & HSPI_SPIIF_RXBF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
HSPI 控制函数
功能: 写发送缓存
输入：0x00000000-0x000000ff(需要发送的data)
输出: 无
********************************/
void HSPI_SPITXBUF_Write(uint32_t SetValue)
{
	HSPI->SPITXBUF = (SetValue & HSPI_SPITXBUF_TXBUF_Msk);
}
/********************************
HSPI 控制函数
功能: 读发送缓存
输入：无
输出: 0x00000000-0x000000ff(需要发送的data)
********************************/
uint32_t HSPI_SPITXBUF_Read(void)
{
	return (HSPI->SPITXBUF & HSPI_SPITXBUF_TXBUF_Msk);
}

/********************************
HSPI 控制函数
功能: 写接收缓存
输入：0x00000000-0x000000ff
输出: 无
********************************/
void HSPI_SPIRXBUF_Write(uint32_t SetValue)
{
	HSPI->SPIRXBUF = (SetValue & HSPI_SPIRXBUF_RXBUF_Msk);
}
/********************************
HSPI 控制函数
功能: 读接收缓存
输入：无
输出: 0x00000000-0x000000ff(接收到的data)
********************************/
uint32_t HSPI_SPIRXBUF_Read(void)
{
	return (HSPI->SPIRXBUF & HSPI_SPIRXBUF_RXBUF_Msk);
}

/********************************
HSPI 控制函数
功能: HSPI缺省设置
输入：无
输出: 无
********************************/
void HSPI_Deinit(void)
{
	HSPI->SPICR1 = 0x00000108;
	HSPI->SPICR2 = 0x00000054;
	HSPI->SPICR3 = 0x00000000;
	HSPI->SPIIE = 0x00000000;
	//HSPI->SPIIF = 0x00000002;
	//HSPI->SPITXBUF = 0x00000000;
	//HSPI->SPIRXBUF = 0x00000000;
}
//Code_End

/********************************
HSPI 控制函数
功能: HSPI MASTER初始化
输入：
输出: 无
********************************/
void HSPI_Master_Init(HSPI_Master_SInitTypeDef* para)
{
	HSPI_Master_InitTypeDef init_para;
	HSPI_Deinit();

	init_para.DELAY_CFG = HSPI_SPICR1_DELAY_CFG_MASTER_NO_DELAY;
	init_para.WAIT = HSPI_SPICR1_WAIT_WAIT_1;
	init_para.BAUD_RATE = para->BAUD_RATE;
	init_para.LSBF = para->LSBF;
	init_para.CPHOL = para->CPHOL;
	init_para.CPHA = para->CPHA;
	init_para.SSNM = para->SSNM;
	init_para.TXO = DISABLE;
	init_para.TXO_AC = HSPI_SPICR2_TXO_AC_CLR_AT_CLOSE;
	init_para.SSNSEN = para->SSNSEN;

	init_para.HSPIEN = DISABLE;
	init_para.ERRIE = DISABLE;
	init_para.TXIE = DISABLE;
	init_para.RXIE = DISABLE;

	HSPI_SPICR1_DELAY_CFG_Set(init_para.DELAY_CFG);
	HSPI_SPICR1_WAIT_Set(init_para.WAIT);
	HSPI_SPICR1_BAUD_Set(init_para.BAUD_RATE);
	HSPI_SPICR1_LSBF_Set(init_para.LSBF);
	HSPI_SPICR1_CPHOL_Set(init_para.CPHOL);
	HSPI_SPICR1_CPHA_Set(init_para.CPHA);
	HSPI_SPICR2_SSNM_Set(init_para.SSNM);
	HSPI_SPICR2_TXO_Setable(init_para.TXO);
	HSPI_SPICR2_TXO_AC_Set(init_para.TXO_AC);
	HSPI_SPICR2_SSNSEN_Set(init_para.SSNSEN);

	HSPI_SPICR1_MM_Set(HSPI_SPICR1_MM_MASTER);//配置成主模式
}

/********************************
HSPI 控制函数
功能: HSPI SLAVER 初始化
输入：
输出: 无
********************************/
void HSPI_Slave_Init(HSPI_Slave_SInitTypeDef* para)
{
	HSPI_Slave_InitTypeDef init_para;
	HSPI_Deinit();

	init_para.DELAY_CFG = HSPI_SPICR1_DELAY_CFG_SLAVE_NORMAL;
	init_para.FLTEN = ENABLE;
	init_para.LSBF = para->LSBF;
	init_para.CPHOL = para->CPHOL;
	init_para.CPHA = para->CPHA;
	init_para.TXO = DISABLE;
	init_para.TXO_AC = HSPI_SPICR2_TXO_AC_CLR_AT_CLOSE;

	init_para.HSPIEN = DISABLE;
	init_para.ERRIE = DISABLE;
	init_para.TXIE = DISABLE;
	init_para.RXIE = DISABLE;

	HSPI_SPICR1_DELAY_CFG_Set(init_para.DELAY_CFG);
	HSPI_SPICR2_FLTEN_Setable(init_para.FLTEN);
	HSPI_SPICR1_LSBF_Set(init_para.LSBF);
	HSPI_SPICR1_CPHOL_Set(init_para.CPHOL);
	HSPI_SPICR1_CPHA_Set(init_para.CPHA);
	HSPI_SPICR2_TXO_Setable(init_para.TXO);
	HSPI_SPICR2_TXO_AC_Set(init_para.TXO_AC);
	
	HSPI_SPICR1_MM_Set(HSPI_SPICR1_MM_SLAVE);//配置成从模式
}

void HSPI_SSN_Set_Low(void)//驱动级
{
	HSPI_SPICR2_SSN_Set(HSPI_SPICR2_SSN_LOW);//拉低SSN信号
}

void HSPI_SSN_Set_High(void)//驱动级
{
	HSPI_SPICR2_SSN_Set(HSPI_SPICR2_SSN_HIGH);//拉高SSN信号
}

unsigned char HSPI_Recv_Byte(void)//HSPI接收一字节
{
	return HSPI_SPIRXBUF_Read();
}
void HSPI_Send_Byte(unsigned char data)//HSPI发送一字节
{
	HSPI_SPITXBUF_Write(data);
}

unsigned char HSPI_RW_Byte(unsigned char data)//SPI发送并接收一字节
{
    unsigned char rx_data;

    while(1)
    {
    	if (HSPI_SPIIF_TXBE_Chk() == SET)
		{
			break;
		}		
    }
    HSPI_Send_Byte(data);

    while(1)
    {
    	if (HSPI_SPIIF_RXBF_Chk() == SET)
        {
            break;
        }
    }
    rx_data = HSPI_Recv_Byte();
    return rx_data;
}



/******END OF FILE****/


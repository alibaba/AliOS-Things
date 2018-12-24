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

#include "fm33A0xx_spi.h" 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



//Code_Start
//2018-04-02-13-22-56
//FM33A0XX_Driver_Gen_V1.4

/********************************
Master对MISO信号的采样位置调整函数
功能:Master对MISO信号的采样位置调整，用于高速通信时补偿PCB走线延迟
输入：采样位置调整的逻辑值
1：采样点延迟半个SCK周期
0：不调整
********************************/
void SPIx_SPICR1_MSPA_Set(SPIx_Type* SPIx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = SPIx->SPICR1;
	tmpreg &= ~(SPIx_SPICR1_MSPA_Msk);
	tmpreg |= (SetValue & SPIx_SPICR1_MSPA_Msk);
	SPIx->SPICR1 = tmpreg;
}
/********************************
读取Master对MISO信号的采样位置调整函数
功能:读取采样位置调整的逻辑值
输出：采样位置调整的逻辑值
1：采样点延迟半个SCK周期
0：不调整
********************************/
uint32_t SPIx_SPICR1_MSPA_Get(SPIx_Type* SPIx)
{
	return (SPIx->SPICR1 & SPIx_SPICR1_MSPA_Msk);
}

/********************************
Slave MISO发送位置调整函数
功能:Slave MISO发送位置调整
输入：Slave MISO发送位置调整的逻辑值
1：提前半个SCK周期发送
0：不调整
********************************/
void SPIx_SPICR1_SSPA_Set(SPIx_Type* SPIx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = SPIx->SPICR1;
	tmpreg &= ~(SPIx_SPICR1_SSPA_Msk);
	tmpreg |= (SetValue & SPIx_SPICR1_SSPA_Msk);
	SPIx->SPICR1 = tmpreg;
}
/********************************
读取Slave MISO发送位置调整函数
功能:读取Slave MISO发送位置调整的逻辑值
输出：Slave MISO发送位置调整的逻辑值
1：提前半个SCK周期发送
0：不调整
********************************/
uint32_t SPIx_SPICR1_SSPA_Get(SPIx_Type* SPIx)
{
	return (SPIx->SPICR1 & SPIx_SPICR1_SSPA_Msk);
}

/********************************
 MASTER/SLAVE模式选择函数
功能: MASTER/SLAVE模式选择
输入： MASTER/SLAVE模式选择的逻辑值
1：Master模式
0：Slave模式
********************************/
void SPIx_SPICR1_MM_Set(SPIx_Type* SPIx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = SPIx->SPICR1;
	tmpreg &= ~(SPIx_SPICR1_MM_Msk);
	tmpreg |= (SetValue & SPIx_SPICR1_MM_Msk);
	SPIx->SPICR1 = tmpreg;
}
/********************************
 读取MASTER/SLAVE模式选择函数
功能: 读取MASTER/SLAVE模式
输出： MASTER/SLAVE模式的逻辑值
1：Master模式
0：Slave模式
********************************/
uint32_t SPIx_SPICR1_MM_Get(SPIx_Type* SPIx)
{
	return (SPIx->SPICR1 & SPIx_SPICR1_MM_Msk);
}
/********************************
 Master模式下设置加入SCK cycle的数值函数
功能: 加入SCK cycle的WAIT数值
输入：  SCK cycle的WAIT数值
********************************/
void SPIx_SPICR1_WAIT_Set(SPIx_Type* SPIx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = SPIx->SPICR1;
	tmpreg &= ~(SPIx_SPICR1_WAIT_Msk);
	tmpreg |= (SetValue & SPIx_SPICR1_WAIT_Msk);
	SPIx->SPICR1 = tmpreg;
}
/********************************
 读取Master模式下设置加入SCK cycle的数值函数
功能: 读取设置的SCK cycle的WAIT数值
输出：  SCK cycle的WAIT数值
********************************/
uint32_t SPIx_SPICR1_WAIT_Get(SPIx_Type* SPIx)
{
	return (SPIx->SPICR1 & SPIx_SPICR1_WAIT_Msk);
}

/********************************
 MASTER模式波特率配置函数
功能: MASTER模式波特率的配置
输入：   MASTER模式波特率的配置数值，fAHBCLK的分频数
********************************/
void SPIx_SPICR1_BAUD_Set(SPIx_Type* SPIx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = SPIx->SPICR1;
	tmpreg &= ~(SPIx_SPICR1_BAUD_Msk);
	tmpreg |= (SetValue & SPIx_SPICR1_BAUD_Msk);
	SPIx->SPICR1 = tmpreg;
}
/********************************
 读取MASTER模式波特率配置的数值函数
功能: 读取MASTER模式波特率的配置
输出：  模式波特率的配置，fAHBCLK的分频数
********************************/
uint32_t SPIx_SPICR1_BAUD_Get(SPIx_Type* SPIx)
{
	return (SPIx->SPICR1 & SPIx_SPICR1_BAUD_Msk);
}

/********************************
 帧格式配置函数
功能: 帧格式的配置
输入：帧格式的配置
0：先发送MSB
1：先发送LSB 
********************************/
void SPIx_SPICR1_LSBF_Set(SPIx_Type* SPIx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = SPIx->SPICR1;
	tmpreg &= ~(SPIx_SPICR1_LSBF_Msk);
	tmpreg |= (SetValue & SPIx_SPICR1_LSBF_Msk);
	SPIx->SPICR1 = tmpreg;
}
/********************************
 读取帧格式的配置的函数
功能: 设置读取帧格式的配置
输出：  帧格式的配置
0：先发送MSB
1：先发送LSB
********************************/
uint32_t SPIx_SPICR1_LSBF_Get(SPIx_Type* SPIx)
{
	return (SPIx->SPICR1 & SPIx_SPICR1_LSBF_Msk);
}

/********************************
 时钟极性选择函数
功能: 设置时钟极性
输入：时钟极性的配置
1：串行时钟停止在高电平
0：串行时钟停止在低电平
********************************/
void SPIx_SPICR1_CPHOL_Set(SPIx_Type* SPIx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = SPIx->SPICR1;
	tmpreg &= ~(SPIx_SPICR1_CPHOL_Msk);
	tmpreg |= (SetValue & SPIx_SPICR1_CPHOL_Msk);
	SPIx->SPICR1 = tmpreg;
}
/********************************
 读取时钟极性选择的函数
功能: 读取设置时钟极性
输出：读取时钟极性的配置
1：串行时钟停止在高电平
0：串行时钟停止在低电平
********************************/
uint32_t SPIx_SPICR1_CPHOL_Get(SPIx_Type* SPIx)
{
	return (SPIx->SPICR1 & SPIx_SPICR1_CPHOL_Msk);
}

/********************************/
/* 时钟相位选择 相关函数 */
void SPIx_SPICR1_CPHA_Set(SPIx_Type* SPIx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = SPIx->SPICR1;
	tmpreg &= ~(SPIx_SPICR1_CPHA_Msk);
	tmpreg |= (SetValue & SPIx_SPICR1_CPHA_Msk);
	SPIx->SPICR1 = tmpreg;
}
/********************************
 读取时钟相位选择函数
功能: 读取时钟相位选择
输出：取时钟相位选择
1：第二个时钟边沿是第一个捕捉边沿
0：第一个时钟边沿是第一个捕捉边沿
********************************/
uint32_t SPIx_SPICR1_CPHA_Get(SPIx_Type* SPIx)
{
	return (SPIx->SPICR1 & SPIx_SPICR1_CPHA_Msk);
}


/********************************
Slave输入管脚滤波使能（SSN/SCK/MOSI）函数
功能: Slave输入管脚滤波使能（SSN/SCK/MOSI）的开启关闭功能
输入： Slave输入管脚滤波使能
1：使能4ns滤波
0：不滤波
********************************/
void SPIx_SPICR2_FLTEN_Setable(SPIx_Type* SPIx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		SPIx->SPICR2 |= (SPIx_SPICR2_FLTEN_Msk);
	}
	else
	{
		SPIx->SPICR2 &= ~(SPIx_SPICR2_FLTEN_Msk);
	}
}
/********************************
读取Slave输入管脚滤波使能（SSN/SCK/MOSI）函数
功能: 读取Slave输入管脚滤波使能（SSN/SCK/MOSI）状态
输出： Slave输入管脚滤波使能的状态
1：使能4ns滤波
0：不滤波
********************************/
FunState SPIx_SPICR2_FLTEN_Getable(SPIx_Type* SPIx)
{
	if (SPIx->SPICR2 & (SPIx_SPICR2_FLTEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}


/********************************
Master模式下SSN控制模式选择函数
功能: Master模式下SSN控制模式的选择
输入： Master模式下SSN控制模式的选择
1：每发送完8bit后Master拉高SSN，维持高电平时间由WAIT寄存器控制
0：每发送完8bit后Master保持SSN为低
********************************/
void SPIx_SPICR2_SSNM_Set(SPIx_Type* SPIx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = SPIx->SPICR2;
	tmpreg &= ~(SPIx_SPICR2_SSNM_Msk);
	tmpreg |= (SetValue & SPIx_SPICR2_SSNM_Msk);
	SPIx->SPICR2 = tmpreg;
}
/********************************
读取Master模式下SSN控制模式选择函数
功能: Master模式下SSN控制模式的选择
输入： Master模式下SSN控制模式的选择
1：每发送完8bit后Master拉高SSN，维持高电平时间由WAIT寄存器控制
0：每发送完8bit后Master保持SSN为低
********************************/
uint32_t SPIx_SPICR2_SSNM_Get(SPIx_Type* SPIx)
{
	return (SPIx->SPICR2 & SPIx_SPICR2_SSNM_Msk);
}

/********************************
TXONLY硬件自动清空的使能函数
功能: TXONLY硬件自动清空的使能开启关闭
输入： TXONLY硬件自动清空的使能状态
1：TXONLY硬件自动清零有效，软件使能TXO后，等待发送完毕后，硬件清零
0：关闭TXONLY硬件自动清零
********************************/
void SPIx_SPICR2_TXO_AC_Set(SPIx_Type* SPIx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = SPIx->SPICR2;
	tmpreg &= ~(SPIx_SPICR2_TXO_AC_Msk);
	tmpreg |= (SetValue & SPIx_SPICR2_TXO_AC_Msk);
	SPIx->SPICR2 = tmpreg;
}
/********************************
读取TXONLY硬件自动清空的使能状态的函数
功能: 读取TXONLY硬件自动清空的使能状态
输出： 读取TXONLY硬件自动清空的使能状态
1：TXONLY硬件自动清零有效，软件使能TXO后，等待发送完毕后，硬件清零
0：关闭TXONLY硬件自动清零
********************************/
uint32_t SPIx_SPICR2_TXO_AC_Get(SPIx_Type* SPIx)
{
	return (SPIx->SPICR2 & SPIx_SPICR2_TXO_AC_Msk);
}

/********************************
TTXONLY控制位设置函数
功能: TTXONLY控制位的设置
输入： TTXONLY控制位的设置
1：启动Master的单发送模式
0：关闭单发送模式
********************************/
void SPIx_SPICR2_TXO_Setable(SPIx_Type* SPIx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		SPIx->SPICR2 |= (SPIx_SPICR2_TXO_Msk);
	}
	else
	{
		SPIx->SPICR2 &= ~(SPIx_SPICR2_TXO_Msk);
	}
}
/********************************
读取TTXONLY控制位设置函数
功能: 读取TTXONLY控制位的设置
输出： TTXONLY控制位的设置
1：启动Master的单发送模式
0：关闭单发送模式
********************************/
FunState SPIx_SPICR2_TXO_Getable(SPIx_Type* SPIx)
{
	if (SPIx->SPICR2 & (SPIx_SPICR2_TXO_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
Master模式下软件控制SSN输出电平函数
功能: 控制SSN输出电平的设置
输入： 控制SSN输出电平
1：SSN输出高电平
0：SSN输出低电平
********************************/
void SPIx_SPICR2_SSN_Set(SPIx_Type* SPIx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = SPIx->SPICR2;
	tmpreg &= ~(SPIx_SPICR2_SSN_Msk);
	tmpreg |= (SetValue & SPIx_SPICR2_SSN_Msk);
	SPIx->SPICR2 = tmpreg;
}
/********************************
读取Master模式下软件控制SSN输出电平函数
功能: 读取控制SSN输出电平的设置
输入： 控制SSN输出电平
1：SSN输出高电平
0：SSN输出低电平
********************************/
uint32_t SPIx_SPICR2_SSN_Get(SPIx_Type* SPIx)
{
	return (SPIx->SPICR2 & SPIx_SPICR2_SSN_Msk);
}

/********************************
Master模式下SSN的控制方式函数
功能: SSN的控制方式的设置
输入： SSN的控制方式
1：Master模式下SSN输出由软件控制
0：Master模式下SSN输出由硬件自动控制
********************************/
void SPIx_SPICR2_SSNSEN_Set(SPIx_Type* SPIx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = SPIx->SPICR2;
	tmpreg &= ~(SPIx_SPICR2_SSNSEN_Msk);
	tmpreg |= (SetValue & SPIx_SPICR2_SSNSEN_Msk);
	SPIx->SPICR2 = tmpreg;
}
/********************************
读取Master模式下SSN的控制方式函数
功能: 读取SSN的控制方式的设置
输出： SSN的控制方式
1：Master模式下SSN输出由软件控制
0：Master模式下SSN输出由硬件自动控制
********************************/
uint32_t SPIx_SPICR2_SSNSEN_Get(SPIx_Type* SPIx)
{
	return (SPIx->SPICR2 & SPIx_SPICR2_SSNSEN_Msk);
}

/********************************
SPI使能函数
功能: SPI使能的设置
输入： SPI使能设置
1：使能SPI
0：关闭SPI，清空发送接收缓存
********************************/
void SPIx_SPICR2_SPIEN_Setable(SPIx_Type* SPIx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		SPIx->SPICR2 |= (SPIx_SPICR2_SPIEN_Msk);
	}
	else
	{
		SPIx->SPICR2 &= ~(SPIx_SPICR2_SPIEN_Msk);
	}
}
/********************************
读取SPI使能函数
功能: 读取SPI使能的设置
输出： SPI使能设置
1：使能SPI
0：关闭SPI，清空发送接收缓存
********************************/
FunState SPIx_SPICR2_SPIEN_Getable(SPIx_Type* SPIx)
{
	if (SPIx->SPICR2 & (SPIx_SPICR2_SPIEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/********************************
清除发送BUFFER函数
功能: 清除发送BUFFER
********************************/
void SPIx_SPICR3_TXBFC_Clr(SPIx_Type* SPIx)
{
	SPIx->SPICR3 = SPIx_SPICR3_TXBFC_Msk;
}
/********************************
清除接收BUFFER函数
功能: 清除接收BUFFER
********************************/
void SPIx_SPICR3_RXBFC_Clr(SPIx_Type* SPIx)
{
	SPIx->SPICR3 = SPIx_SPICR3_RXBFC_Msk;
}
/********************************
清除主模式错误的函数
功能: 清除主模式错误
********************************/
void SPIx_SPICR3_MERRC_Clr(SPIx_Type* SPIx)
{
	SPIx->SPICR3 = SPIx_SPICR3_MERRC_Msk;
}

/********************************
清除从模式错误的函数
功能: 清除从模式错误
********************************/
void SPIx_SPICR3_SERRC_Clr(SPIx_Type* SPIx)
{
	SPIx->SPICR3 = SPIx_SPICR3_SERRC_Msk;
}
/********************************
HSPI错误中断使能函数
功能: HSPI错误中断使能的设置
输入： HSPI错误中断使能设置
********************************/
void SPIx_SPIIE_ERRIE_Setable(SPIx_Type* SPIx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		SPIx->SPIIE |= (SPIx_SPIIE_ERRIE_Msk);
	}
	else
	{
		SPIx->SPIIE &= ~(SPIx_SPIIE_ERRIE_Msk);
	}
}
/********************************
读取HSPI错误中断使能函数
功能: 读取HSPI错误中断使能的设置
输出： 读取HSPI错误中断使能设置
********************************/
FunState SPIx_SPIIE_ERRIE_Getable(SPIx_Type* SPIx)
{
	if (SPIx->SPIIE & (SPIx_SPIIE_ERRIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/********************************
发送中断使能函数
功能: 发送中断使能的设置
输入： 发送中断使能设置
********************************/
void SPIx_SPIIE_TXIE_Setable(SPIx_Type* SPIx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		SPIx->SPIIE |= (SPIx_SPIIE_TXIE_Msk);
	}
	else
	{
		SPIx->SPIIE &= ~(SPIx_SPIIE_TXIE_Msk);
	}
}
/********************************
读取发送中断使能函数
功能: 读取发送中断使能的设置
输出： 发送中断使能设置
********************************/
FunState SPIx_SPIIE_TXIE_Getable(SPIx_Type* SPIx)
{
	if (SPIx->SPIIE & (SPIx_SPIIE_TXIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
接收中断使能函数
功能: 接收中断使能的设置
输入： 接收中断使能设置
********************************/
void SPIx_SPIIE_RXIE_Setable(SPIx_Type* SPIx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		SPIx->SPIIE |= (SPIx_SPIIE_RXIE_Msk);
	}
	else
	{
		SPIx->SPIIE &= ~(SPIx_SPIIE_RXIE_Msk);
	}
}

/********************************
读取接收中断使能函数
功能: 读取接收中断使能的设置
输出： 接收中断使能设置
********************************/
FunState SPIx_SPIIE_RXIE_Getable(SPIx_Type* SPIx)
{
	if (SPIx->SPIIE & (SPIx_SPIIE_RXIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
Master Error标志函数
功能: 读取Master Error标志
输出： Master Error标志
********************************/
FlagStatus SPIx_SPIIF_MERR_Chk(SPIx_Type* SPIx)
{
	if (SPIx->SPIIF & SPIx_SPIIF_MERR_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}
/********************************
Slave Error标志函数
功能: 读取Slave Error标志
输出： Slave Error标志
********************************/
FlagStatus SPIx_SPIIF_SERR_Chk(SPIx_Type* SPIx)
{
	if (SPIx->SPIIF & SPIx_SPIIF_SERR_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
清除接收缓存溢出标志函数
功能: 清除接收缓存溢出标志
输入： 清除接收缓存溢出标志
********************************/
void SPIx_SPIIF_RXCOL_Clr(SPIx_Type* SPIx)
{
	SPIx->SPIIF = SPIx_SPIIF_RXCOL_Msk;
}
/********************************
读取接收缓存溢出标志函数
功能: 读取接收缓存溢出标志
输出： 接收缓存溢出标志
********************************/
FlagStatus SPIx_SPIIF_RXCOL_Chk(SPIx_Type* SPIx)
{
	if (SPIx->SPIIF & SPIx_SPIIF_RXCOL_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
清除发送缓存溢出标志函数
功能: 清除发送缓存溢出标志
输入： 清除发送缓存溢出标志
********************************/
void SPIx_SPIIF_TXCOL_Clr(SPIx_Type* SPIx)
{
	SPIx->SPIIF = SPIx_SPIIF_TXCOL_Msk;
}
/********************************
读取发送缓存溢出标志函数
功能: 读取发送缓存溢出标志
输出： 发送缓存溢出标志
********************************/
FlagStatus SPIx_SPIIF_TXCOL_Chk(SPIx_Type* SPIx)
{
	if (SPIx->SPIIF & SPIx_SPIIF_TXCOL_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
读取SPI空闲标志函数
功能: 读取SPI空闲标志
输出： SPI空闲标志
********************************/
FlagStatus SPIx_SPIIF_BUSY_Chk(SPIx_Type* SPIx)
{
	if (SPIx->SPIIF & SPIx_SPIIF_BUSY_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
读取TX Buffer Empty标志位函数
功能: 读取TX Buffer Empty标志位
输出： TX Buffer Empty标志位
********************************/
FlagStatus SPIx_SPIIF_TXBE_Chk(SPIx_Type* SPIx)
{
	if (SPIx->SPIIF & SPIx_SPIIF_TXBE_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}


/********************************
读取RX Buffer Full标志位函数
功能: 读取RX Buffer Full标志位
输出： RX Buffer Full标志位
********************************/
FlagStatus SPIx_SPIIF_RXBF_Chk(SPIx_Type* SPIx)
{
	if (SPIx->SPIIF & SPIx_SPIIF_RXBF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
SPI发送缓存函数
功能: 写SPI发送缓存
输入： SPI发送缓存
********************************/
void SPIx_SPITXBUF_Write(SPIx_Type* SPIx, uint32_t SetValue)
{
	SPIx->SPITXBUF = (SetValue & SPIx_SPITXBUF_TXBUF_Msk);
}
/********************************
读取SPI发送缓存函数
功能: 读取SPI发送缓存的值
输出： SPI发送缓存的值
********************************/
uint32_t SPIx_SPITXBUF_Read(SPIx_Type* SPIx)
{
	return (SPIx->SPITXBUF & SPIx_SPITXBUF_TXBUF_Msk);
}

/********************************
SPI接收缓存函数
功能: 写SPI接收缓存
输入： SPI接收缓存
********************************/
void SPIx_SPIRXBUF_Write(SPIx_Type* SPIx, uint32_t SetValue)
{
	SPIx->SPIRXBUF = (SetValue & SPIx_SPIRXBUF_RXBUF_Msk);
}
/********************************
读取SPI接收缓存函数
功能: 读取SPI接收缓存的值
输出： SPI接收缓存的值
********************************/
uint32_t SPIx_SPIRXBUF_Read(SPIx_Type* SPIx)
{
	return (SPIx->SPIRXBUF & SPIx_SPIRXBUF_RXBUF_Msk);
}
/********************************
SPI缺省设置函数
功能:SPI缺省设置
********************************/
void SPIx_Deinit(SPIx_Type* SPIx)
{
	SPIx->SPICR1 = 0x00000108;
	SPIx->SPICR2 = 0x00000054;
	SPIx->SPICR3 = 0x00000000;
	SPIx->SPIIE = 0x00000000;
	//SPIx->SPIIF = 0x00000002;
	//SPIx->SPITXBUF = 0x00000000;
	//SPIx->SPIRXBUF = 0x00000000;
}
//Code_End



/********************************
 SPI主模式初始化函数
功能:  SPI主模式初始化
输入： SPI主模式的初始化参数
********************************/
void SPI_Master_Init(SPIx_Type* SPIx, SPI_Master_SInitTypeDef* para)
{
	SPI_Master_InitTypeDef init_para;

	SPIx_Deinit(SPIx);

	init_para.MSPA = SPIx_SPICR1_MSPA_NORMAL;
	init_para.WAIT = SPIx_SPICR1_WAIT_WAIT_1;
	init_para.BAUD_RATE = para->BAUD_RATE;
	init_para.LSBF = para->LSBF;
	init_para.CPHOL = para->CPHOL;
	init_para.CPHA = para->CPHA;
	init_para.SSNM = para->SSNM;
	init_para.TXO = DISABLE;
	init_para.TXO_AC = SPIx_SPICR2_TXO_AC_CLR_AT_CLOSE;
	init_para.SSNSEN = para->SSNSEN;

	init_para.SPIEN = DISABLE;
	init_para.ERRIE = DISABLE;
	init_para.TXIE = DISABLE;
	init_para.RXIE = DISABLE;
	
	SPIx_SPICR1_MSPA_Set(SPIx, init_para.MSPA);
	SPIx_SPICR1_WAIT_Set(SPIx, init_para.WAIT);
	SPIx_SPICR1_BAUD_Set(SPIx, init_para.BAUD_RATE);
	SPIx_SPICR1_LSBF_Set(SPIx, init_para.LSBF);
	SPIx_SPICR1_CPHOL_Set(SPIx, init_para.CPHOL);
	SPIx_SPICR1_CPHA_Set(SPIx, init_para.CPHA);
	SPIx_SPICR2_SSNM_Set(SPIx, init_para.SSNM);
	SPIx_SPICR2_TXO_Setable(SPIx, init_para.TXO);
	SPIx_SPICR2_TXO_AC_Set(SPIx, init_para.TXO_AC);
	SPIx_SPICR2_SSNSEN_Set(SPIx, init_para.SSNSEN);

	SPIx_SPICR1_MM_Set(SPIx, SPIx_SPICR1_MM_MASTER);//配置成主模式
}

/********************************
 SPI从模式初始化函数
功能:  SPI从模式初始化
输入： SPI从模式的初始化参数
********************************/
void SPI_Slave_Init(SPIx_Type* SPIx, SPI_Slave_SInitTypeDef* para)
{
	SPI_Slave_InitTypeDef init_para;

	SPIx_Deinit(SPIx);

	init_para.SSPA = SPIx_SPICR1_SSPA_NORMAL;
	init_para.FLTEN = ENABLE;
	init_para.LSBF = para->LSBF;
	init_para.CPHOL = para->CPHOL;
	init_para.CPHA = para->CPHA;
	init_para.TXO = DISABLE;
	init_para.TXO_AC = SPIx_SPICR2_TXO_AC_CLR_AT_CLOSE;

	init_para.SPIEN = DISABLE;
	init_para.ERRIE = DISABLE;
	init_para.TXIE = DISABLE;
	init_para.RXIE = DISABLE;
	
	SPIx_SPICR1_SSPA_Set(SPIx, init_para.SSPA);
	SPIx_SPICR2_FLTEN_Setable(SPIx, init_para.FLTEN);
	SPIx_SPICR1_LSBF_Set(SPIx, init_para.LSBF);
	SPIx_SPICR1_CPHOL_Set(SPIx, init_para.CPHOL);
	SPIx_SPICR1_CPHA_Set(SPIx, init_para.CPHA);
	SPIx_SPICR2_TXO_Setable(SPIx, init_para.TXO);
	SPIx_SPICR2_TXO_AC_Set(SPIx, init_para.TXO_AC);

	SPIx_SPICR1_MM_Set(SPIx, SPIx_SPICR1_MM_SLAVE);//配置成从模式
}


/********************************
 SPI的SSN设置电平低的函数
********************************/
void SPI_SSN_Set_Low(SPIx_Type* SPIx)//驱动级
{
	SPIx_SPICR2_SSN_Set(SPIx, SPIx_SPICR2_SSN_LOW);//拉低SSN信号
}
/********************************
 SPI的SSN设置电平高的函数
********************************/
void SPI_SSN_Set_High(SPIx_Type* SPIx)//驱动级
{
	SPIx_SPICR2_SSN_Set(SPIx, SPIx_SPICR2_SSN_HIGH);//拉高SSN信号
}
/********************************
 SPI接收一个字节的函数
功能：SPI接收一个字节
输出：SPI接收字节
********************************/
unsigned char SPI_Recv_Byte(SPIx_Type* SPIx)//SPI接收一字节
{
	return SPIx_SPIRXBUF_Read(SPIx);
}
/********************************
 SPI发送一个字节的函数
功能：SPI发送一个字节
输入：SPI要发送的字节
********************************/
void SPI_Send_Byte(SPIx_Type* SPIx, unsigned char data)//SPI发送一字节
{
	SPIx_SPITXBUF_Write(SPIx, data);
}
/********************************
 SPI发送并接收一个字节的函数
功能：SPI发送和接收一个字节
输入：SPI要发送的字节，输出：SPI接收字节
********************************/
unsigned char SPI_RW_Byte(SPIx_Type* SPIx, unsigned char data)//SPI发送并接收一字节
{
    unsigned char rx_data;

    while(1)
    {
    	if (SPIx_SPIIF_TXBE_Chk(SPIx) == SET)
		{
			break;
		}		
    }
    SPI_Send_Byte(SPIx, data);

    while(1)
    {
    	if (SPIx_SPIIF_RXBF_Chk(SPIx) == SET)
        {
            break;
        }
    }
    rx_data = SPI_Recv_Byte(SPIx);
    return rx_data;
}

//Code_End

/******END OF FILE****/


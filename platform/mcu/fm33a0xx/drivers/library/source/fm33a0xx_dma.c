/**
  ******************************************************************************
  * @file    fm33a0xx_dma.c
  * @author  FM385 Application Team
  * @version V2.0.2
  * @date    27-3-2018
  * @brief   This file provides firmware functions to manage the following 
  *
*/

/* Includes ------------------------------------------------------------------*/
#include "fm33A0xx_dma.h" 

/** @addtogroup fm33a0_StdPeriph_Driver
  * @{
  */

/** @defgroup DMA 
  * @brief DMA driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup DMA_Private_Functions
  * @{
  */ 

//Code_Start
//2018-03-02-15-19-03
//FM33A0XX_Driver_Gen_V1.3

/********************************
DMA功能控制函数
功能: 配置DMA功能全局使能
输入：ENABLE/DISABLE	DMA使能/DMA关闭	
输出: 无
********************************/
void DMA_GLOBALCTRL_DMAEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		DMA->GLOBALCTRL |= (DMA_GLOBALCTRL_DMAEN_Msk);
	}
	else
	{
		DMA->GLOBALCTRL &= ~(DMA_GLOBALCTRL_DMAEN_Msk);
	}
}
/********************************
DMA功能控制函数
功能: 获取DMA功能全局使能配置
输入：无
输出: ENABLE/DISABLE	DMA使能/DMA关闭	
********************************/
FunState DMA_GLOBALCTRL_DMAEN_Getable(void)
{
	if (DMA->GLOBALCTRL & (DMA_GLOBALCTRL_DMAEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}


/********************************
DMA功能控制函数
功能: 配置Channelx 传输长度
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
			SetValue: 0-1fff(传输长度为SetValue+1)
输出: 无
********************************/
void DMA_CHxCTRL_TSIZE_Set(DMA_CH_Type CHx, uint32_t SetValue)
{
	uint32_t *pREG;
	uint32_t tmpreg;    
	
	pREG = (uint32_t *)(0x40000404 + CHx*8);//DMA->CH0CTRL;
	
    SetValue <<= DMA_CHxCTRL_TSIZE_Pos;//手工添加
	tmpreg = *pREG;
	tmpreg &= ~(DMA_CHxCTRL_TSIZE_Msk);
	tmpreg |= (SetValue&DMA_CHxCTRL_TSIZE_Msk);
	*pREG = tmpreg;
}
/********************************
DMA功能控制函数
功能: 获取Channelx 传输长度
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
输出: tmpsize: 0-1fff(传输长度为tmpsize+1)
********************************/
uint32_t DMA_CHxCTRL_TSIZE_Get(DMA_CH_Type CHx)
{
	uint32_t *pREG;
    uint32_t tmpsize;
	
	pREG = (uint32_t *)(0x40000404 + CHx*8);
	
    tmpsize = (*pREG & DMA_CHxCTRL_TSIZE_Msk);
    tmpsize >>= DMA_CHxCTRL_TSIZE_Pos;
	return tmpsize;
}

/********************************
DMA功能控制函数
功能: 配置Channelx 优先级
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
			DMA_CHxCTRL_PRI_LOW				低优先级
			/DMA_CHxCTRL_PRI_MEDIUM		中优先级
			/DMA_CHxCTRL_PRI_HIGH			高优先级
			/DMA_CHxCTRL_PRI_VERY_HIGH	最高优先级
输出: 无
********************************/
void DMA_CHxCTRL_PRI_Set(DMA_CH_Type CHx, uint32_t SetValue)
{
	uint32_t *pREG;
	uint32_t tmpreg;
	
	pREG = (uint32_t *)(0x40000404 + CHx*8);
	
	tmpreg = *pREG;
	tmpreg &= ~(DMA_CHxCTRL_PRI_Msk);
	tmpreg |= (SetValue&DMA_CHxCTRL_PRI_Msk);
	*pREG = tmpreg;
}
/********************************
DMA功能控制函数
功能: 获取Channelx 优先级
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
输出: DMA_CHxCTRL_PRI_LOW				低优先级
			/DMA_CHxCTRL_PRI_MEDIUM		中优先级
			/DMA_CHxCTRL_PRI_HIGH			高优先级
			/DMA_CHxCTRL_PRI_VERY_HIGH	最高优先级
********************************/
uint32_t DMA_CHxCTRL_PRI_Get(DMA_CH_Type CHx)
{
	uint32_t *pREG;
	
	pREG = (uint32_t *)(0x40000404 + CHx*8);
	return (*pREG & DMA_CHxCTRL_PRI_Msk);
}

/********************************
DMA功能控制函数
功能: 配置Channelx RAM地址增减设置
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
			DMA_CHxCTRL_INC_DEC			地址递减
			/DMA_CHxCTRL_INC_INC		地址递增
输出: 无
********************************/
void DMA_CHxCTRL_INC_Set(DMA_CH_Type CHx, uint32_t SetValue)
{
	uint32_t *pREG;
	uint32_t tmpreg;
	
	pREG = (uint32_t *)(0x40000404 + CHx*8);
	
	tmpreg = *pREG;
	tmpreg &= ~(DMA_CHxCTRL_INC_Msk);
	tmpreg |= (SetValue&DMA_CHxCTRL_INC_Msk);
	*pREG = tmpreg;
}
/********************************
DMA功能控制函数
功能: 获取Channelx RAM地址增减设置
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
输出: DMA_CHxCTRL_INC_DEC			地址递减
			/DMA_CHxCTRL_INC_INC		地址递增
********************************/
uint32_t DMA_CHxCTRL_INC_Get(DMA_CH_Type CHx)
{
	uint32_t *pREG;
	
	pREG = (uint32_t *)(0x40000404 + CHx*8);
	return (*pREG & DMA_CHxCTRL_INC_Msk);
}

/********************************
DMA功能控制函数
功能: 配置Channelx 外设通道选择
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
			DMA_CH0CTRL_CH0SSEL_ET4_CAP				ET4捕获通道
			/DMA_CH0CTRL_CH0SSEL_ET2_CAP			ET2捕获通道
			/DMA_CH0CTRL_CH0SSEL_BT2_CAP			BT2捕获通道
			/DMA_CH0CTRL_CH0SSEL_ET3_CAP			ET3捕获通道
			/DMA_CH0CTRL_CH0SSEL_BT1_CAP			BT1捕获通道
			/DMA_CH0CTRL_CH0SSEL_CRC_OUT			CRC输出通道
			/DMA_CH0CTRL_CH0SSEL_AES_IN				AES输入通道
			/DMA_CH0CTRL_CH0SSEL_ADC_OUT			ADC输出通道
输出: 无
********************************/
void DMA_CHxCTRL_SSEL_Set(DMA_CH_Type CHx, uint32_t SetValue)
{
	uint32_t *pREG;
	uint32_t tmpreg;
	
	pREG = (uint32_t *)(0x40000404 + CHx*8);
	
	tmpreg = *pREG;
	tmpreg &= ~(DMA_CHxCTRL_SSEL_Msk);
	tmpreg |= (SetValue&DMA_CHxCTRL_SSEL_Msk);
	*pREG = tmpreg;
}
/********************************
DMA功能控制函数
功能: 获取Channelx 外设通道选择
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
输出: DMA_CH0CTRL_CH0SSEL_ET4_CAP				ET4捕获通道
			/DMA_CH0CTRL_CH0SSEL_ET2_CAP			ET2捕获通道
			/DMA_CH0CTRL_CH0SSEL_BT2_CAP			BT2捕获通道
			/DMA_CH0CTRL_CH0SSEL_ET3_CAP			ET3捕获通道
			/DMA_CH0CTRL_CH0SSEL_BT1_CAP			BT1捕获通道
			/DMA_CH0CTRL_CH0SSEL_CRC_OUT			CRC输出通道
			/DMA_CH0CTRL_CH0SSEL_AES_IN				AES输入通道
			/DMA_CH0CTRL_CH0SSEL_ADC_OUT			ADC输出通道
********************************/
uint32_t DMA_CHxCTRL_SSEL_Get(DMA_CH_Type CHx)
{
	uint32_t *pREG;
	
	pREG = (uint32_t *)(0x40000404 + CHx*8);
	return (*pREG & DMA_CHxCTRL_SSEL_Msk);
}

/********************************
DMA功能控制函数
功能: 配置Channelx 传输中断完成使能
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
			ENABLE/DISABLE	使能传输完成中断/关闭传输完成中断		
输出: 无
********************************/
void DMA_CHxCTRL_FTIE_Setable(DMA_CH_Type CHx, FunState NewState)
{
	uint32_t *pREG;
	
	pREG = (uint32_t *)(0x40000404 + CHx*8);
	if (NewState == ENABLE)
	{
		*pREG |= (DMA_CHxCTRL_FTIE_Msk);
	}
	else
	{
		*pREG &= ~(DMA_CHxCTRL_FTIE_Msk);
	}
}
/********************************
DMA功能控制函数
功能: 获取Channelx 传输中断完成使能
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
输出: ENABLE/DISABLE	使能传输完成中断/关闭传输完成中断	
********************************/
FunState DMA_CHxCTRL_FTIE_Getable(DMA_CH_Type CHx)
{
	uint32_t *pREG;
	
	pREG = (uint32_t *)(0x40000404 + CHx*8);
	if (*pREG & (DMA_CHxCTRL_FTIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
DMA功能控制函数
功能: 配置Channelx 半程传输完成中断使能
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
			ENABLE/DISABLE	使能半程中断/关闭半程中断		
输出: 无
********************************/
void DMA_CHxCTRL_HTIE_Setable(DMA_CH_Type CHx, FunState NewState)
{
	uint32_t *pREG;
	
	pREG = (uint32_t *)(0x40000404 + CHx*8);
	if (NewState == ENABLE)
	{
		*pREG |= (DMA_CHxCTRL_HTIE_Msk);
	}
	else
	{
		*pREG &= ~(DMA_CHxCTRL_HTIE_Msk);
	}
}
/********************************
DMA功能控制函数
功能: 配置Channelx 半程传输完成中断使能
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
输出: ENABLE/DISABLE	使能半程中断/关闭半程中断
********************************/
FunState DMA_CHxCTRL_HTIE_Getable(DMA_CH_Type CHx)
{
	uint32_t *pREG;
	
	pREG = (uint32_t *)(0x40000404 + CHx*8);
	if (*pREG & (DMA_CHxCTRL_HTIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
DMA功能控制函数
功能: 配置Channelx 启动
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
			ENABLE/DISABLE	CHx DMA功能启动/CHx DMA功能关闭
输出: 无
********************************/
void DMA_CHxCTRL_EN_Setable(DMA_CH_Type CHx, FunState NewState)
{
	uint32_t *pREG;
	
	pREG = (uint32_t *)(0x40000404 + CHx*8);
	if (NewState == ENABLE)
	{
		*pREG |= (DMA_CHxCTRL_EN_Msk);
	}
	else
	{
		*pREG &= ~(DMA_CHxCTRL_EN_Msk);
	}
}

/********************************
DMA功能控制函数
功能: 配置Channelx RAM指针地址(DMA传输启动前软件向此寄存器写入RAM目标地址，DMA启动后此寄存器随DMA传输自增或自减)
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6)
SetValue:16bit 地址
输出: 无
********************************/
void DMA_CHxRAMAD_Write(DMA_CH_Type CHx, uint32_t SetValue)
{
	uint32_t *pREG;
	
	pREG = (uint32_t *)(0x40000404 + 4 + CHx*8);
	*pREG = SetValue;
}

//uint32_t DMA_CHxRAMAD_Read(DMA_CH_Type CHx)
//{
//	uint32_t *pREG;
//	
//	pREG = (uint32_t *)(0x40000404 + 4 + CHx*8);
//	return (*pREG);
//}



/* Channel7FLASH指针地址 相关函数 */
/********************************
DMA功能控制函数
功能: 配置Channel7 FLASH指针地址(DMA传输启动前软件向此寄存器写入Flash目标地址，DMA启动后此寄存器随DMA传输自增或自减)
输入：CHx(DMA_CH7 = 7)
SetValue:17bit 地址
输出: 无
********************************/
void DMA_CH7FLSAD_Write(uint32_t SetValue)
{
//	DMA->CH7FLSAD = SetValue;
	DMA->CH7FLSAD = SetValue>>2;//CH7的flash地址是字地址，此处做了右移2bit处理，以便使用时可以使用普通地址
}

//uint32_t DMA_CH7FLSAD_Read(void)
//{
//	return (DMA->CH7FLSAD);
//}


/********************************
DMA功能控制函数
功能: 配置Channel7 RAM指针地址(DMA传输启动前软件向此寄存器写入RAM目标地址，DMA启动后此寄存器随DMA传输自增或自减)
输入：CHx(DMA_CH7 = 7)
SetValue:14bit 地址
输出: 无
********************************/
void DMA_CH7RAMAD_Write(uint32_t SetValue)
{
//	DMA->CH7RAMAD = SetValue;
	DMA->CH7RAMAD = SetValue>>2;//CH7的RAM地址是字地址，此处做了右移2bit处理，以便使用时可以使用普通地址
}

//uint32_t DMA_CH7RAMAD_Read(void)
//{
//	return (DMA->CH7RAMAD);
//}


/********************************
DMA功能控制函数
功能: 清Channelx 传输半程标志
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
输出: 无
********************************/
void DMA_CHSTATUS_DMACHxHT_Clr(DMA_CH_Type CHx)
{
	DMA->CHSTATUS = (0x1U<<CHx);
}
/********************************
DMA功能控制函数
功能: 获取Channelx 传输半程标志
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
输出: SET/RESET		对应通道半程传输完成/对应通道半程传输未完成
********************************/
FlagStatus DMA_CHSTATUS_DMACHxHT_Chk(DMA_CH_Type CHx)
{
	if (DMA->CHSTATUS & (0x1U<<CHx))
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
DMA功能控制函数
功能: 清Channelx 传输完成标志
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
输出: 无
********************************/
void DMA_CHSTATUS_DMACHxFT_Clr(DMA_CH_Type CHx)
{
	DMA->CHSTATUS = (0x100U<<CHx);
}
/********************************
DMA功能控制函数
功能: 获取Channelx 传输完成标志
输入：CHx(DMA_CH0 = 0, DMA_CH1 = 1, DMA_CH2 = 2, DMA_CH3 = 3, DMA_CH4 = 4, DMA_CH5 = 5, DMA_CH6 = 6, DMA_CH7 = 7)
输出: SET/RESET		对应通道传输完成/对应通道传输未完成
********************************/
FlagStatus DMA_CHSTATUS_DMACHxFT_Chk(DMA_CH_Type CHx)
{
	if (DMA->CHSTATUS & (0x100U<<CHx))
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}
/********************************
DMA功能控制函数
功能: DMA功能初始化缺省配置
输入：无
输出: 无
********************************/
void DMA_Deinit(void)
{
	DMA->GLOBALCTRL = 0x00000000;
	DMA->CH0CTRL = 0x00000000;
	DMA->CH0RAMAD = 0x00000000;
	DMA->CH1CTRL = 0x00000000;
	DMA->CH1RAMAD = 0x00000000;
	DMA->CH2CTRL = 0x00000000;
	DMA->CH2RAMAD = 0x00000000;
	DMA->CH3CTRL = 0x00000000;
	DMA->CH3RAMAD = 0x00000000;
	DMA->CH4CTRL = 0x00000000;
	DMA->CH4RAMAD = 0x00000000;
	DMA->CH5CTRL = 0x00000000;
	DMA->CH5RAMAD = 0x00000000;
	DMA->CH6CTRL = 0x00000000;
	DMA->CH6RAMAD = 0x00000000;
	DMA->CH7CTRL = 0x00000000;
	DMA->CH7FLSAD = 0x00000000;
	DMA->CH7RAMAD = 0x00000000;
	DMA->CHSTATUS = 0x00000000;
}
//Code_End
          

void DMA_Init(DMA_InitTypeDef* para)
{
	if( DMA_CH7 == para->CHx )
	{
		DMA_CH7RAMAD_Write(para->CHxRAMAD);//CH0~CH7通道RAM指针地址
		DMA_CH7FLSAD_Write(para->CH7FLSAD);//通道FLASH指针地址，仅针对通道7有意义
	}
	else
	{
		DMA_CHxRAMAD_Write(para->CHx, para->CHxRAMAD);//CH0~CH7通道RAM指针地址
	}
	
	DMA_CHxCTRL_TSIZE_Set(para->CHx, para->CHxTSIZE);	//通道传输长度
	DMA_CHxCTRL_PRI_Set(para->CHx, para->CHxPRI);		//通道优先级
	DMA_CHxCTRL_INC_Set(para->CHx, para->CHxINC);		//通道地址增长方向
	DMA_CHxCTRL_SSEL_Set(para->CHx, para->CHxSSEL);		//CH0~CH6外设通道选择,CH7传输方式选择
	DMA_CHxCTRL_FTIE_Setable(para->CHx, para->CHxFTIE);	//通道传输完成中断使能
	DMA_CHxCTRL_HTIE_Setable(para->CHx, para->CHxHTIE);	//通道传输半程中断使能
	DMA_CHxCTRL_EN_Setable(para->CHx, para->CHxEN);		//通道使能
}

/******END OF FILE****/




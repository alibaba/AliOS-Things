/**
  ******************************************************************************
  * @file    fm33a0xx_i2c.c
  * @author  FM33a0xx Application Team
  * @version V2.0.2
  * @date    18-1-2018
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of....:
  *
*/
/* Includes ------------------------------------------------------------------*/

#include "fm33A0xx_i2c.h" 
#include <string.h>
/** @addtogroup fm33a0xx_StdPeriph_Driver
  * @{
  */

/** @defgroup I2C 
  * @brief I2C driver modules
  * @{
  */ 

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

 //Code_Start
//2018-04-02-13-21-22
//FM33A0XX_Driver_Gen_V1.4

/********************************
 I2C模块禁止时，SCL固定为高低电平控制位函数
功能：I2C模块禁止时，设置SCL固定为高低电平控制位
输入：设置SCL固定为高低电平控制位
1 = 将SCL信号转化为高电平，如非必要，建议常态设为1
0 = 将SCL信号转化为低电平
********************************/
void I2C_I2CCTRL_SCLHL_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		I2C->I2CCTRL |= (I2C_I2CCTRL_SCLHL_Msk);
	}
	else
	{
		I2C->I2CCTRL &= ~(I2C_I2CCTRL_SCLHL_Msk);
	}
}
/********************************
读取SCL固定为高低电平控制位函数
功能：读取SCL固定为高低电平控制位的设置
输出：SCL固定为高低电平控制位的设置
1 = 将SCL信号转化为高电平，如非必要，建议常态设为1
0 = 将SCL信号转化为低电平
********************************/
FunState I2C_I2CCTRL_SCLHL_Getable(void)
{
	if (I2C->I2CCTRL & (I2C_I2CCTRL_SCLHL_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
 I2C模块禁止时，SDA固定为高低电平控制位函数
功能：I2C模块禁止时，设置SDA固定为高低电平控制位
输入：设置SDA固定为高低电平控制位
1 = 将SDA信号转化为高电平，如非必要，建议常态设为1
0 = 将SDA信号转化为低电平
********************************/
void I2C_I2CCTRL_SDAHL_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		I2C->I2CCTRL |= (I2C_I2CCTRL_SDAHL_Msk);
	}
	else
	{
		I2C->I2CCTRL &= ~(I2C_I2CCTRL_SDAHL_Msk);
	}
}
/********************************
读取SDA固定为高低电平控制位函数
功能：读取SDA固定为高低电平控制位的设置
输出：SDA固定为高低电平控制位的设置
1 = 将SDA信号转化为高电平，如非必要，建议常态设为1
0 = 将SDA信号转化为低电平
********************************/
FunState I2C_I2CCTRL_SDAHL_Getable(void)
{
	if (I2C->I2CCTRL & (I2C_I2CCTRL_SDAHL_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
 主控接收模式下，主机给从机回应使能位函数
功能：主控接收模式下，主机给从机回应使能位
输入：主机给从机回应使能位
1 = 主机发送回应ACKDT给从机
0 = 主机不响应从机
********************************/
void I2C_I2CCTRL_ACKEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		I2C->I2CCTRL |= (I2C_I2CCTRL_ACKEN_Msk);
	}
	else
	{
		I2C->I2CCTRL &= ~(I2C_I2CCTRL_ACKEN_Msk);
	}
}
/********************************
 主控接收模式下，读取主机给从机回应使能位的函数
功能：主控接收模式下，读取主机给从机回应使能位
输出：主机给从机回应使能位
1 = 主机发送回应ACKDT给从机
0 = 主机不响应从机
********************************/
FunState I2C_I2CCTRL_ACKEN_Getable(void)
{
	if (I2C->I2CCTRL & (I2C_I2CCTRL_ACKEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
 主控接收模式下，接收使能位设置函数
功能：主控接收模式下，接收使能位设置函数
输入：接收使能位设置
1 = 主机接收使能
0 = 接收禁止
********************************/
void I2C_I2CCTRL_RCEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		I2C->I2CCTRL |= (I2C_I2CCTRL_RCEN_Msk);
	}
	else
	{
		I2C->I2CCTRL &= ~(I2C_I2CCTRL_RCEN_Msk);
	}
}
/********************************
 主控接收模式下，读取接收使能位设置函数
功能：主控接收模式下，读取接收使能位设置函数
输出：读取接收使能位设置
1 = 主机接收使能
0 = 接收禁止
********************************/
FunState I2C_I2CCTRL_RCEN_Getable(void)
{
	if (I2C->I2CCTRL & (I2C_I2CCTRL_RCEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
 STOP时序产生使能控制位设置函数
功能：STOP时序产生使能控制位设置函数
输入：STOP时序产生使能控制位的设置
1 = STOP时序产生使能
0 = STOP时序产生禁止
********************************/
void I2C_I2CCTRL_PEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		I2C->I2CCTRL |= (I2C_I2CCTRL_PEN_Msk);
	}
	else
	{
		I2C->I2CCTRL &= ~(I2C_I2CCTRL_PEN_Msk);
	}
}
/********************************
 Repeated START时序产生使能控制位设置函数
功能：Repeated START时序产生使能控制位设置函数
输入：Repeated START时序产生使能控制位的设置
1 = Repeated START时序产生使能
0 = Repeated START时序产生禁止
********************************/
void I2C_I2CCTRL_RSEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		I2C->I2CCTRL |= (I2C_I2CCTRL_RSEN_Msk);
	}
	else
	{
		I2C->I2CCTRL &= ~(I2C_I2CCTRL_RSEN_Msk);
	}
}

/********************************
 START时序产生使能控制位设置函数
功能：START时序产生使能控制位设置函数
输入：START时序产生使能控制位的设置
1 = START时序产生使能
0 = START时序产生禁止
********************************/
void I2C_I2CCTRL_SEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		I2C->I2CCTRL |= (I2C_I2CCTRL_SEN_Msk);
	}
	else
	{
		I2C->I2CCTRL &= ~(I2C_I2CCTRL_SEN_Msk);
	}
}

/********************************
 I2C模块使能控制位设置函数
功能：I2C模块使能控制位设置函数
输入：I2C模块使能控制位的设置
1 = I2C使能
0 = I2C禁止
********************************/
void I2C_I2CCTRL_I2CEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		I2C->I2CCTRL |= (I2C_I2CCTRL_I2CEN_Msk);
	}
	else
	{
		I2C->I2CCTRL &= ~(I2C_I2CCTRL_I2CEN_Msk);
	}
}
/********************************
 读取I2C模块使能控制位设置函数
功能：读取I2C模块使能控制位设置函数
输出：I2C模块使能控制位的设置
1 = I2C使能
0 = I2C禁止
********************************/
FunState I2C_I2CCTRL_I2CEN_Getable(void)
{
	if (I2C->I2CCTRL & (I2C_I2CCTRL_I2CEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
 清除写冲突检测位的函数
功能：清除写冲突检测位
********************************/
void I2C_I2CSTA_WCOL_Clr(void)
{
	I2C->I2CSTA = I2C_I2CSTA_WCOL_Msk;
}

/********************************
 读取写冲突检测位的函数
功能：读取写冲突检测位
输出：写冲突检测位
1 = 发送写冲突
0 = 未发生冲突、
********************************/
FlagStatus I2C_I2CSTA_WCOL_Chk(void)
{
	if (I2C->I2CSTA & I2C_I2CSTA_WCOL_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
 读取I2C接口状态位的函数
功能：读取I2C接口状态
输出：I2C接口状态
1 = 接口处于读写状态，正在进行数据传输，
0 = 已完成数据传输
********************************/
FlagStatus I2C_I2CSTA_RW_Chk(void)
{
	if (I2C->I2CSTA & I2C_I2CSTA_RW_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
 读取 STOP标志位的函数
功能：读取 STOP标志位
输出：STOP标志位
1 = 检测到STOP时序
0 = 未检测到STOP时序
********************************/
FlagStatus I2C_I2CSTA_P_Chk(void)
{
	if (I2C->I2CSTA & I2C_I2CSTA_P_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}


/********************************
 读取 START标志位的函数
功能：读取 START标志位
输出：START标志位
1 = 检测到START时序
0 = 未检测到START时序
********************************/
FlagStatus I2C_I2CSTA_S_Chk(void)
{
	if (I2C->I2CSTA & I2C_I2CSTA_S_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
 读取 缓冲器满状态位的函数
功能：读取 缓冲器满状态位
输出：缓冲器满状态位
接收：
1 = 接收完成，SSPBUF满
0 = 接收未完成，SSPBUF空
发送：
1 = 正在发送，SSPBUF满
0 = 发送完成，SSPBUF空
********************************/
FlagStatus I2C_I2CSTA_BF_Chk(void)
{
	if (I2C->I2CSTA & I2C_I2CSTA_BF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
 清除来自从机的回应信号的函数
功能：清除来自从机的回应信号
********************************/
void I2C_I2CSTA_ACKSTA_Clr(void)
{
	I2C->I2CSTA = I2C_I2CSTA_ACKSTA_Msk;
}
/********************************
 读取 来自从机的回应信号的函数
功能：读取 来自从机的回应信号
输出：来自从机的回应信号
1 = 从机无回应
0 = 接收到从机的回应0
********************************/
FlagStatus I2C_I2CSTA_ACKSTA_Chk(void)
{
	if (I2C->I2CSTA & I2C_I2CSTA_ACKSTA_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
 主机回应信号的状态设置函数
功能：主机回应信号的状态设置函数
输入：主机回应信号的状态的设置
1 = 主机将不给从机回应
0 = 主机将给从机的回应为0
********************************/
void I2C_I2CSTA_ACKDT_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = I2C->I2CSTA;
	tmpreg &= ~(I2C_I2CSTA_ACKDT_Msk);
	tmpreg |= (SetValue & I2C_I2CSTA_ACKDT_Msk);
	I2C->I2CSTA = tmpreg;
}
/********************************
 读取主机回应信号的状态设置函数
功能：读取主机回应信号的状态设置函数
输出：主机回应信号的状态的设置
1 = 主机将不给从机回应
0 = 主机将给从机的回应为0
********************************/
uint32_t I2C_I2CSTA_ACKDT_Get(void)
{
	return (I2C->I2CSTA & I2C_I2CSTA_ACKDT_Msk);
}

/********************************
 波特率设置函数
功能：波特率设置函数
输入：波特率的设置
SCL波特率分频配置
********************************/
void I2C_I2CBRG_Write(uint32_t SetValue)
{
	I2C->I2CBRG = (SetValue & I2C_I2CBRG_SSPBRG_Msk);
}
/********************************
 读取波特率函数
功能：读取波特率函数
输出：波特率
********************************/
uint32_t I2C_I2CBRG_Read(void)
{
	return (I2C->I2CBRG & I2C_I2CBRG_SSPBRG_Msk);
}

/********************************
 写收发缓冲寄存器函数
功能： 写收发缓冲寄存器
输入： 要写收发缓冲寄存器的值
********************************/
void I2C_I2CBUF_Write(uint32_t SetValue)
{
	I2C->I2CBUF = (SetValue & I2C_I2CBUF_SSPBUF_Msk);
}
/********************************
 读收发缓冲寄存器函数
功能： 读收发缓冲寄存器
输出： 收发缓冲寄存器的值
********************************/
uint32_t I2C_I2CBUF_Read(void)
{
	return (I2C->I2CBUF & I2C_I2CBUF_SSPBUF_Msk);
}

/********************************
 I2C中断使能控制位设置函数
功能：I2C中断使能控制位设置
输入：I2C中断使能控制位
1 = I2C中断使能
0 = I2C中断禁止
********************************/
void I2C_I2CIR_I2CIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		I2C->I2CIR |= (I2C_I2CIR_I2CIE_Msk);
	}
	else
	{
		I2C->I2CIR &= ~(I2C_I2CIR_I2CIE_Msk);
	}
}
/********************************
 读取I2C中断使能控制位设置函数
功能：读取I2C中断使能控制位设置
输出：I2C中断使能控制位
1 = I2C中断使能
0 = I2C中断禁止
********************************/
FunState I2C_I2CIR_I2CIE_Getable(void)
{
	if (I2C->I2CIR & (I2C_I2CIR_I2CIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}


/********************************
 清除I2C中断标志位的函数
功能：清除I2C中断标志位
********************************/
void I2C_I2CIR_I2CIF_Clr(void)
{
	if (I2C->I2CIR & (I2C_I2CIR_I2CIE_Msk))
	{
		I2C->I2CIR = (I2C_I2CIR_I2CIE_Msk|I2C_I2CIR_I2CIF_Msk);
	}
	else
	{
		I2C->I2CIR = I2C_I2CIR_I2CIF_Msk;
	}
}
/********************************
 读取I2C中断标志位的函数
功能：读取I2C中断标志位
输出：I2C中断标志位
1 = 产生I2C中断
0 = 未产生I2C中断
********************************/
FlagStatus I2C_I2CIR_I2CIF_Chk(void)
{
	if (I2C->I2CIR & I2C_I2CIR_I2CIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
 读取I2C主状态机编码的函数
功能：读取I2C主状态机编码
输出：I2C主状态机编码
********************************/
uint32_t I2C_I2CFSM_I2CFSM_Get(void)
{
	return (I2C->I2CFSM & I2C_I2CFSM_I2CFSM_Msk);
}

/********************************
 错误标志中断使能设置函数
功能：错误标志中断使能设置
输入：错误标志中断使能
1 = I2C中断使能
0 = I2C中断禁止
********************************/
void I2C_I2CERR_ERRIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		I2C->I2CERR |= (I2C_I2CERR_ERRIE_Msk);
	}
	else
	{
		I2C->I2CERR &= ~(I2C_I2CERR_ERRIE_Msk);
	}
}
/********************************
 读取错误标志中断使能设置函数
功能：读取错误标志中断使能设置
输出：错误标志中断使能
1 = I2C中断使能
0 = I2C中断禁止
********************************/
FunState I2C_I2CERR_ERRIE_Getable(void)
{
	if (I2C->I2CERR & (I2C_I2CERR_ERRIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}


/********************************
 清除OP_IDLE状态下错误标志位的函数
功能：清除OP_IDLE状态下错误标志位
********************************/
void I2C_I2CERR_OIERR_Clr(void)
{
	if (I2C->I2CERR & (I2C_I2CERR_ERRIE_Msk))
	{
		I2C->I2CERR = (I2C_I2CERR_ERRIE_Msk|I2C_I2CERR_OIERR_Msk);
	}
	else
	{
		I2C->I2CERR = I2C_I2CERR_OIERR_Msk;
	}
}
/********************************
 读取OP_IDLE状态下错误标志位的函数
功能：读取OP_IDLE状态下错误标志位
输出：OP_IDLE状态下错误标志位
********************************/
FlagStatus I2C_I2CERR_OIERR_Chk(void)
{
	if (I2C->I2CERR & I2C_I2CERR_OIERR_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
 清除START_DONE状态下错误标志位的函数
功能：清除START_DONE状态下错误标志位
********************************/
void I2C_I2CERR_SDERR_Clr(void)
{
	if (I2C->I2CERR & (I2C_I2CERR_ERRIE_Msk))
	{
		I2C->I2CERR = (I2C_I2CERR_ERRIE_Msk|I2C_I2CERR_SDERR_Msk);
	}
	else
	{
		I2C->I2CERR = I2C_I2CERR_SDERR_Msk;
	}
}
/********************************
 读取START_DONE状态下错误标志位的函数
功能：读取START_DONE状态下错误标志位
输出：START_DONE状态下错误标志位
********************************/
FlagStatus I2C_I2CERR_SDERR_Chk(void)
{
	if (I2C->I2CERR & I2C_I2CERR_SDERR_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
 清除IDLE状态下错误标志位的函数
功能：清除IDLE状态下错误标志位
********************************/
void I2C_I2CERR_IERR_Clr(void)
{
	if (I2C->I2CERR & (I2C_I2CERR_ERRIE_Msk))
	{
		I2C->I2CERR = (I2C_I2CERR_ERRIE_Msk|I2C_I2CERR_IERR_Msk);
	}
	else
	{
		I2C->I2CERR = I2C_I2CERR_IERR_Msk;
	}
}

/********************************
 读取IDLE状态下错误标志位的函数
功能：读取IDLE状态下错误标志位
输出：IDLE状态下错误标志位
********************************/
FlagStatus I2C_I2CERR_IERR_Chk(void)
{
	if (I2C->I2CERR & I2C_I2CERR_IERR_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
 I2C缺省设置的函数
功能：I2C缺省设置
********************************/
void I2C_Deinit(void)
{
	I2C->I2CCTRL = 0x00006000;
	I2C->I2CSTA = 0x00000000;
	I2C->I2CBRG = 0x00000013;
	//I2C->I2CBUF = ;
	I2C->I2CIR = 0x00000000;
	//I2C->I2CFSM = ;
	I2C->I2CERR = 0x00000000;
}
//Code_End

/********************************
 I2C波特率寄存器值计算函数
功能：I2C波特率寄存器值计算
输入：期望波特率@Hz， APBClk @Hz
输出：I2CBRG对应期望波特率的配置值
********************************/
uint32_t I2C_BaudREG_Calc(uint32_t I2CClk, uint32_t APBClk)
{
	uint32_t TempREG;
	
	TempREG = APBClk/(4*I2CClk);
	if( TempREG >= 1 ) TempREG = TempREG - 1;
	
	return TempREG;
}


/******END OF FILE****/

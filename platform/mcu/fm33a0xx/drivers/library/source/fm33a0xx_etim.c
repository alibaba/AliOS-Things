/**
  ******************************************************************************
  * @file    fm33a0xx_etim.c
  * @author  FM33a0 Application Team
  * @version V2.0.2
  * @date    26-2-2018
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of....:
  *
*/

/* Includes ------------------------------------------------------------------*/
#include "fm33A0xx_etim.h"  


/** @addtogroup fm33a0_StdPeriph_Driver
  * @{
  */

/** @defgroup ETIM 
  * @brief ETIM driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//Code_Start
//2018-04-02-13-21-03
//FM33A0XX_Driver_Gen_V1.4

/*********************************
externded timer高位计数器控制函数 
功能：ET计数器外部输入引脚数字滤波功能配置
输入: ETIM1/ETIM2/ETIM3/ETIM4  定时器名称
			ENABLE/DISABLE 		打开引脚输入信号数字滤波/关闭引脚输入信号数字滤波
输出: 无
*********************************/
void ETIMx_ETxCR_EXFLT_Setable(ETIMx_Type* ETIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		ETIMx->ETxCR |= (ETIMx_ETxCR_EXFLT_Msk);
	}
	else
	{
		ETIMx->ETxCR &= ~(ETIMx_ETxCR_EXFLT_Msk);
	}
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET计数器外部输入引脚数字滤波功能配置
输入: ETIM1/ETIM2/ETIM3/ETIM4  定时器名称
输出: ENABLE/DISABLE 		打开引脚输入信号数字滤波/关闭引脚输入信号数字滤波
*********************************/
FunState ETIMx_ETxCR_EXFLT_Getable(ETIMx_Type* ETIMx)
{
	if (ETIMx->ETxCR & (ETIMx_ETxCR_EXFLT_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
externded timer高位计数器控制函数 
功能：ET计数器PWM输出使能配置
输入: ETIM1/ETIM2/ETIM3/ETIM4  定时器名称
			ENABLE/DISABLE 		PWM输出使能/PWM输出禁止
输出: 无
*********************************/
void ETIMx_ETxCR_PWM_Setable(ETIMx_Type* ETIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		ETIMx->ETxCR |= (ETIMx_ETxCR_PWM_Msk);
	}
	else
	{
		ETIMx->ETxCR &= ~(ETIMx_ETxCR_PWM_Msk);
	}
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET计数器PWM输出使能配置
输入: ETIM1/ETIM2/ETIM3/ETIM4  定时器名称
输出: ENABLE/DISABLE 		PWM输出使能/PWM输出禁止
*********************************/
FunState ETIMx_ETxCR_PWM_Getable(ETIMx_Type* ETIMx)
{
	if (ETIMx->ETxCR & (ETIMx_ETxCR_PWM_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/*********************************
externded timer高位计数器控制函数 
功能：ET计数器启动配置
输入: ETIM1/ETIM2/ETIM3/ETIM4  定时器名称
			ENABLE/DISABLE 		 启动定时器/停止定时器
输出: 无
*********************************/
void ETIMx_ETxCR_CEN_Setable(ETIMx_Type* ETIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		ETIMx->ETxCR |= (ETIMx_ETxCR_CEN_Msk);
	}
	else
	{
		ETIMx->ETxCR &= ~(ETIMx_ETxCR_CEN_Msk);
	}
}

/*********************************
externded timer高位计数器控制函数 
功能：获取ET计数器启动配置
输入: ETIM1/ETIM2/ETIM3/ETIM4  定时器名称
输出: ENABLE/DISABLE 		 启动定时器/停止定时器
*********************************/
FunState ETIMx_ETxCR_CEN_Getable(ETIMx_Type* ETIMx)
{
	if (ETIMx->ETxCR & (ETIMx_ETxCR_CEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET计数器工作模式
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			ETIMx_ETxCR_MOD_COUNTER		0 = 计数模式
			/ETIMx_ETxCR_MOD_CAPTURE	1 = 捕捉模式
输出: 无
*********************************/
void ETIMx_ETxCR_MOD_Set(ETIMx_Type* ETIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = ETIMx->ETxCR;
	tmpreg &= ~(ETIMx_ETxCR_MOD_Msk);
	tmpreg |= (SetValue & ETIMx_ETxCR_MOD_Msk);
	ETIMx->ETxCR = tmpreg;
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET计数器工作模式
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: ETIMx_ETxCR_MOD_COUNTER		0 = 计数模式
			/ETIMx_ETxCR_MOD_CAPTURE	1 = 捕捉模式
*********************************/
uint32_t ETIMx_ETxCR_MOD_Get(ETIMx_Type* ETIMx)
{
	return (ETIMx->ETxCR & ETIMx_ETxCR_MOD_Msk);
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET计数器级联功能
输入: ETIM1/ETIM2/ETIM3/ETIM4  定时器名称
			ENABLE/DISABLE 		 ET1（ET3）和ET2（ET4）级联成32bit定时器/16bit定时器独立工作
输出: 无
*********************************/
void ETIMx_ETxCR_CASEN_Setable(ETIMx_Type* ETIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		ETIMx->ETxCR |= (ETIMx_ETxCR_CASEN_Msk);
	}
	else
	{
		ETIMx->ETxCR &= ~(ETIMx_ETxCR_CASEN_Msk);
	}
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET计数器级联功能配置
输入: ETIM1/ETIM2/ETIM3/ETIM4  定时器名称
输出: ENABLE/DISABLE 		 ET1（ET3）和ET2（ET4）级联成32bit定时器/16bit定时器独立工作
*********************************/
FunState ETIMx_ETxCR_CASEN_Getable(ETIMx_Type* ETIMx)
{
	if (ETIMx->ETxCR & (ETIMx_ETxCR_CASEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET计数器计数模式采沿方式(计数时钟选择mcu_clk时该位无效，总是采用mcu_clk时钟上升沿计数)
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			ETIMx_ETxCR_EDGESEL_RISING		0 = 计数模式采上升沿
			/ETIMx_ETxCR_EDGESEL_FALLING	1 = 计数模式采下降沿
输出: 无
*********************************/
void ETIMx_ETxCR_EDGESEL_Set(ETIMx_Type* ETIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = ETIMx->ETxCR;
	tmpreg &= ~(ETIMx_ETxCR_EDGESEL_Msk);
	tmpreg |= (SetValue & ETIMx_ETxCR_EDGESEL_Msk);
	ETIMx->ETxCR = tmpreg;
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET计数器计数模式采沿方式(计数时钟选择mcu_clk时该位无效，总是采用mcu_clk时钟上升沿计数)
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: ETIMx_ETxCR_EDGESEL_RISING		0 = 计数模式采上升沿
			/ETIMx_ETxCR_EDGESEL_FALLING	1 = 计数模式采下降沿
*********************************/
uint32_t ETIMx_ETxCR_EDGESEL_Get(ETIMx_Type* ETIMx)
{
	return (ETIMx->ETxCR & ETIMx_ETxCR_EDGESEL_Msk);
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET计数器捕捉模式
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			ETIMx_ETxCR_CAPMOD_PERIOD		0 = 脉冲周期捕捉
			/ETIMx_ETxCR_CAPMOD_PULSE		1 = 脉宽捕捉
输出: 无
*********************************/
void ETIMx_ETxCR_CAPMOD_Set(ETIMx_Type* ETIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = ETIMx->ETxCR;
	tmpreg &= ~(ETIMx_ETxCR_CAPMOD_Msk);
	tmpreg |= (SetValue & ETIMx_ETxCR_CAPMOD_Msk);
	ETIMx->ETxCR = tmpreg;
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET计数器捕捉模式
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: ETIMx_ETxCR_CAPMOD_PERIOD		0 = 脉冲周期捕捉
			/ETIMx_ETxCR_CAPMOD_PULSE		1 = 脉宽捕捉
*********************************/
uint32_t ETIMx_ETxCR_CAPMOD_Get(ETIMx_Type* ETIMx)
{
	return (ETIMx->ETxCR & ETIMx_ETxCR_CAPMOD_Msk);
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET计数器带清零捕捉模式
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			ENABLE/DISABLE		事件触发捕捉：使能后计数器保持0，捕捉到第一个有效沿之后timer才开始计数/ 捕捉不清零，计数器一直自由计数
输出: 无
*********************************/
void ETIMx_ETxCR_CAPCLR_Setable(ETIMx_Type* ETIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		ETIMx->ETxCR |= (ETIMx_ETxCR_CAPCLR_Msk);
	}
	else
	{
		ETIMx->ETxCR &= ~(ETIMx_ETxCR_CAPCLR_Msk);
	}
}
/*********************************
externded timer高位计数器控制函数 
功能：获取置ET计数器带清零捕捉模式配置
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: ENABLE/DISABLE		事件触发捕捉：使能后计数器保持0，捕捉到第一个有效沿之后timer才开始计数/ 捕捉不清零，计数器一直自由计数
*********************************/
FunState ETIMx_ETxCR_CAPCLR_Getable(ETIMx_Type* ETIMx)
{
	if (ETIMx->ETxCR & (ETIMx_ETxCR_CAPCLR_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET计数器单次捕捉有效
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			ENABLE/DISABLE		单次捕捉有效/连续捕捉
输出: 无
*********************************/
void ETIMx_ETxCR_CAPONCE_Setable(ETIMx_Type* ETIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		ETIMx->ETxCR |= (ETIMx_ETxCR_CAPONCE_Msk);
	}
	else
	{
		ETIMx->ETxCR &= ~(ETIMx_ETxCR_CAPONCE_Msk);
	}
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET计数器单次捕捉有效配置
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: ENABLE/DISABLE		单次捕捉有效/连续捕捉
*********************************/
FunState ETIMx_ETxCR_CAPONCE_Getable(ETIMx_Type* ETIMx)
{
	if (ETIMx->ETxCR & (ETIMx_ETxCR_CAPONCE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET计数器捕捉沿选择
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			ETIMx_ETxCR_CAPEDGE_RISING		0 = 周期捕捉模式时上沿捕捉
			ETIMx_ETxCR_CAPEDGE_FALLING		1 = 周期捕捉模式时下沿捕捉
输出: 无
*********************************/
void ETIMx_ETxCR_CAPEDGE_Set(ETIMx_Type* ETIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = ETIMx->ETxCR;
	tmpreg &= ~(ETIMx_ETxCR_CAPEDGE_Msk);
	tmpreg |= (SetValue & ETIMx_ETxCR_CAPEDGE_Msk);
	ETIMx->ETxCR = tmpreg;
}

/*********************************
externded timer高位计数器控制函数 
功能：获取ET计数器捕捉沿选择配置
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: ETIMx_ETxCR_CAPEDGE_RISING		0 = 周期捕捉模式时上沿捕捉
			ETIMx_ETxCR_CAPEDGE_FALLING		1 = 周期捕捉模式时下沿捕捉
*********************************/
uint32_t ETIMx_ETxCR_CAPEDGE_Get(ETIMx_Type* ETIMx)
{
	return (ETIMx->ETxCR & ETIMx_ETxCR_CAPEDGE_Msk);
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET内部信号2源选择(捕捉源)
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			ETIMx_ETxINSEL_SIG2SEL_GROUP2		0 = 扩展定时器3的内部信号2选择Group2
			/ETIMx_ETxINSEL_SIG2SEL_GROUP1	1 = 扩展定时器3的内部信号2选择Group1
输出: 无
*********************************/
void ETIMx_ETxINSEL_SIG2SEL_Set(ETIMx_Type* ETIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = ETIMx->ETxINSEL;
	tmpreg &= ~(ETIMx_ETxINSEL_SIG2SEL_Msk);
	tmpreg |= (SetValue & ETIMx_ETxINSEL_SIG2SEL_Msk);
	ETIMx->ETxINSEL = tmpreg;
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET内部信号2源选择(捕捉源)
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: ETIMx_ETxINSEL_SIG2SEL_GROUP2		0 = 扩展定时器的内部信号2选择Group2
			/ETIMx_ETxINSEL_SIG2SEL_GROUP1	1 = 扩展定时器的内部信号2选择Group1
*********************************/
uint32_t ETIMx_ETxINSEL_SIG2SEL_Get(ETIMx_Type* ETIMx)
{
	return (ETIMx->ETxINSEL & ETIMx_ETxINSEL_SIG2SEL_Msk);
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET内部信号1源选择（在计数模式下计数源仅由此选择，捕捉模式下计数源）
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			ETIMx_ETxINSEL_SIG1SEL_GROUP1			0 = 扩展定时器的内部信号1选择Group1
			/ETIMx_ETxINSEL_SIG1SEL_GROUP2		1 = 扩展定时器的内部信号1选择Group2 
输出: 无
*********************************/
void ETIMx_ETxINSEL_SIG1SEL_Set(ETIMx_Type* ETIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = ETIMx->ETxINSEL;
	tmpreg &= ~(ETIMx_ETxINSEL_SIG1SEL_Msk);
	tmpreg |= (SetValue & ETIMx_ETxINSEL_SIG1SEL_Msk);
	ETIMx->ETxINSEL = tmpreg;
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET内部信号1源选择（在计数模式下计数源仅由此选择，捕捉模式下计数源）
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: ETIMx_ETxINSEL_SIG1SEL_GROUP1			0 = 扩展定时器的内部信号1选择Group1
			/ETIMx_ETxINSEL_SIG1SEL_GROUP2		1 = 扩展定时器的内部信号1选择Group2 
*********************************/
uint32_t ETIMx_ETxINSEL_SIG1SEL_Get(ETIMx_Type* ETIMx)
{
	return (ETIMx->ETxINSEL & ETIMx_ETxINSEL_SIG1SEL_Msk);
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET GROUP2 信号选择
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			ETIMx_ETxINSEL_GRP2SEL_ET1_UART0_RX				ET1 000 = UART0_RX
			/ETIMx_ETxINSEL_GRP2SEL_ET1_UART1_RX			ET1 001 = UART1_RX
			/ETIMx_ETxINSEL_GRP2SEL_ET1_XTLF					ET1 010 = XTLF
			/ETIMx_ETxINSEL_GRP2SEL_ET1_ET1_IN1				ET1 011 = ET1_IN1
			/ETIMx_ETxINSEL_GRP2SEL_ET1_ET1_IN2				ET1 100 = ET1_IN2
			/ETIMx_ETxINSEL_GRP2SEL_ET1_CMP1O					ET1 101 = CMP1O（比较器1输出）
			/ETIMx_ETxINSEL_GRP2SEL_ET1_CMP2O					ET1 110 = CMP2O（比较器2输出
			/ETIMx_ETxINSEL_GRP2SEL_ET1_LPTO					ET1 111 = LPTO
			/ETIMx_ETxINSEL_GRP2SEL_ET2_UART2_RX			ET2 000 = UART2_RX
			/ETIMx_ETxINSEL_GRP2SEL_ET2_UART3_RX			ET2 001 = UART3_RX
			/ETIMx_ETxINSEL_GRP2SEL_ET2_XTLF					ET2 010 = XTLF
			/ETIMx_ETxINSEL_GRP2SEL_ET2_ET2_IN1				ET2 011 = ET2_IN1
			/ETIMx_ETxINSEL_GRP2SEL_ET2_ET2_IN2				ET2 100 = ET2_IN2
			/ETIMx_ETxINSEL_GRP2SEL_ET2_CMP1O					ET2 101 = CMP1O（比较器1输出）
			/ETIMx_ETxINSEL_GRP2SEL_ET2_CMP2O					ET2 110 = CMP2O（比较器2输出）
			/ETIMx_ETxINSEL_GRP2SEL_ET2_LPTO					ET2 111 = LPTO
			/ETIMx_ETxINSEL_GRP2SEL_ET3_ET3_IN12			ET3 000 = ET3_IN1
			/ETIMx_ETxINSEL_GRP2SEL_ET3_XTLF					ET3 001 = XTLF
			/ETIMx_ETxINSEL_GRP2SEL_ET3_UART4_RX			ET3 010 = UART4_RX
			/ETIMx_ETxINSEL_GRP2SEL_ET3_UART5_RX			ET3 011 = UART5_RX
			/ETIMx_ETxINSEL_GRP2SEL_ET3_RTCSEC				ET3 100 = RTCSEC
			/ETIMx_ETxINSEL_GRP2SEL_ET4_ET4_IN1				ET4 000 = ET4_IN1
			/ETIMx_ETxINSEL_GRP2SEL_ET4_XTLF					ET4 001 = XTLF
			/ETIMx_ETxINSEL_GRP2SEL_ET4_UART_RX22			ET4 010 = UART_RX2
			/ETIMx_ETxINSEL_GRP2SEL_ET4_UART_RX0			ET4 011 = UART_RX0
			/ETIMx_ETxINSEL_GRP2SEL_ET4_CMP1O					ET4 100 = CMP1O（比较器1输出）
			/ETIMx_ETxINSEL_GRP2SEL_ET4_CMP2O					ET4 101 = CMP2O（比较器2输出）
			/ETIMx_ETxINSEL_GRP2SEL_ET4_RTCSEC				ET4 110= RTCSEC
			/ETIMx_ETxINSEL_GRP2SEL_ET4_LPTO					ET4 111= LPTO
输出: 无
*********************************/
void ETIMx_ETxINSEL_GRP2SEL_Set(ETIMx_Type* ETIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = ETIMx->ETxINSEL;
	tmpreg &= ~(ETIMx_ETxINSEL_GRP2SEL_Msk);
	tmpreg |= (SetValue & ETIMx_ETxINSEL_GRP2SEL_Msk);
	ETIMx->ETxINSEL = tmpreg;
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET GROUP2 信号选择
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: ETIMx_ETxINSEL_GRP2SEL_ET1_UART0_RX				ET1 000 = UART0_RX
			/ETIMx_ETxINSEL_GRP2SEL_ET1_UART1_RX			ET1 001 = UART1_RX
			/ETIMx_ETxINSEL_GRP2SEL_ET1_XTLF					ET1 010 = XTLF
			/ETIMx_ETxINSEL_GRP2SEL_ET1_ET1_IN1				ET1 011 = ET1_IN1
			/ETIMx_ETxINSEL_GRP2SEL_ET1_ET1_IN2				ET1 100 = ET1_IN2
			/ETIMx_ETxINSEL_GRP2SEL_ET1_CMP1O					ET1 101 = CMP1O（比较器1输出）
			/ETIMx_ETxINSEL_GRP2SEL_ET1_CMP2O					ET1 110 = CMP2O（比较器2输出
			/ETIMx_ETxINSEL_GRP2SEL_ET1_LPTO					ET1 111 = LPTO
			/ETIMx_ETxINSEL_GRP2SEL_ET2_UART2_RX			ET2 000 = UART2_RX
			/ETIMx_ETxINSEL_GRP2SEL_ET2_UART3_RX			ET2 001 = UART3_RX
			/ETIMx_ETxINSEL_GRP2SEL_ET2_XTLF					ET2 010 = XTLF
			/ETIMx_ETxINSEL_GRP2SEL_ET2_ET2_IN1				ET2 011 = ET2_IN1
			/ETIMx_ETxINSEL_GRP2SEL_ET2_ET2_IN2				ET2 100 = ET2_IN2
			/ETIMx_ETxINSEL_GRP2SEL_ET2_CMP1O					ET2 101 = CMP1O（比较器1输出）
			/ETIMx_ETxINSEL_GRP2SEL_ET2_CMP2O					ET2 110 = CMP2O（比较器2输出）
			/ETIMx_ETxINSEL_GRP2SEL_ET2_LPTO					ET2 111 = LPTO
			/ETIMx_ETxINSEL_GRP2SEL_ET3_ET3_IN12			ET3 000 = ET3_IN1
			/ETIMx_ETxINSEL_GRP2SEL_ET3_XTLF					ET3 001 = XTLF
			/ETIMx_ETxINSEL_GRP2SEL_ET3_UART4_RX			ET3 010 = UART4_RX
			/ETIMx_ETxINSEL_GRP2SEL_ET3_UART5_RX			ET3 011 = UART5_RX
			/ETIMx_ETxINSEL_GRP2SEL_ET3_RTCSEC				ET3 100 = RTCSEC
			/ETIMx_ETxINSEL_GRP2SEL_ET4_ET4_IN1				ET4 000 = ET4_IN1
			/ETIMx_ETxINSEL_GRP2SEL_ET4_XTLF					ET4 001 = XTLF
			/ETIMx_ETxINSEL_GRP2SEL_ET4_UART_RX22			ET4 010 = UART_RX2
			/ETIMx_ETxINSEL_GRP2SEL_ET4_UART_RX0			ET4 011 = UART_RX0
			/ETIMx_ETxINSEL_GRP2SEL_ET4_CMP1O					ET4 100 = CMP1O（比较器1输出）
			/ETIMx_ETxINSEL_GRP2SEL_ET4_CMP2O					ET4 101 = CMP2O（比较器2输出）
			/ETIMx_ETxINSEL_GRP2SEL_ET4_RTCSEC				ET4 110= RTCSEC
			/ETIMx_ETxINSEL_GRP2SEL_ET4_LPTO					ET4 111= LPTO
*********************************/
uint32_t ETIMx_ETxINSEL_GRP2SEL_Get(ETIMx_Type* ETIMx)
{
	return (ETIMx->ETxINSEL & ETIMx_ETxINSEL_GRP2SEL_Msk);
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET GROUP1 信号选择
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			ETIMx_ETxINSEL_GRP1SEL_ET1_APBCLK			ET1 00 = APBCLK
			/ETIMx_ETxINSEL_GRP1SEL_ET1_XTLF			ET1 01 = XTLF
			/ETIMx_ETxINSEL_GRP1SEL_ET1_RCLP			ET1 10 = RCLP
			/ETIMx_ETxINSEL_GRP1SEL_ET1_ET1_IN0		ET1 11 = ET1_IN0
			/ETIMx_ETxINSEL_GRP1SEL_ET2_APBCLK		ET2 00 = APBCLK
			/ETIMx_ETxINSEL_GRP1SEL_ET2_XTLF			ET2 01 = XTLF
			/ETIMx_ETxINSEL_GRP1SEL_ET2_RCLP			ET2 10 = RCLP
			/ETIMx_ETxINSEL_GRP1SEL_ET2_ET2_IN0		ET2 11 = ET2_IN0
			/ETIMx_ETxINSEL_GRP1SEL_ET3_APBCLK		ET3 00 = APBCLK
			/ETIMx_ETxINSEL_GRP1SEL_ET3_ET3_IN0		ET3 01 = ET3_IN0
			/ETIMx_ETxINSEL_GRP1SEL_ET3_RTCSEC		ET3 10 = RTCSEC
			/ETIMx_ETxINSEL_GRP1SEL_ET3_RCLP			ET3 11 = RCLP
			/ETIMx_ETxINSEL_GRP1SEL_ET4_APBCLK		ET4 00 = APBCLK
			/ETIMx_ETxINSEL_GRP1SEL_ET4_ET4_IN0		ET4 01 = ET4_IN0
			/ETIMx_ETxINSEL_GRP1SEL_ET4_RTC64HZ		ET4 10 = RTC64HZ/
			/ETIMx_ETxINSEL_GRP1SEL_ET4_LPTO			ET4 11 = LPTO
输出: 无
*********************************/
void ETIMx_ETxINSEL_GRP1SEL_Set(ETIMx_Type* ETIMx, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = ETIMx->ETxINSEL;
	tmpreg &= ~(ETIMx_ETxINSEL_GRP1SEL_Msk);
	tmpreg |= (SetValue & ETIMx_ETxINSEL_GRP1SEL_Msk);
	ETIMx->ETxINSEL = tmpreg;
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET GROUP1 信号选择
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: ETIMx_ETxINSEL_GRP1SEL_ET1_APBCLK			ET1 00 = APBCLK
			/ETIMx_ETxINSEL_GRP1SEL_ET1_XTLF			ET1 01 = XTLF
			/ETIMx_ETxINSEL_GRP1SEL_ET1_RCLP			ET1 10 = RCLP
			/ETIMx_ETxINSEL_GRP1SEL_ET1_ET1_IN0		ET1 11 = ET1_IN0
			/ETIMx_ETxINSEL_GRP1SEL_ET2_APBCLK		ET2 00 = APBCLK
			/ETIMx_ETxINSEL_GRP1SEL_ET2_XTLF			ET2 01 = XTLF
			/ETIMx_ETxINSEL_GRP1SEL_ET2_RCLP			ET2 10 = RCLP
			/ETIMx_ETxINSEL_GRP1SEL_ET2_ET2_IN0		ET2 11 = ET2_IN0
			/ETIMx_ETxINSEL_GRP1SEL_ET3_APBCLK		ET3 00 = APBCLK
			/ETIMx_ETxINSEL_GRP1SEL_ET3_ET3_IN0		ET3 01 = ET3_IN0
			/ETIMx_ETxINSEL_GRP1SEL_ET3_RTCSEC		ET3 10 = RTCSEC
			/ETIMx_ETxINSEL_GRP1SEL_ET3_RCLP			ET3 11 = RCLP
			/ETIMx_ETxINSEL_GRP1SEL_ET4_APBCLK		ET4 00 = APBCLK
			/ETIMx_ETxINSEL_GRP1SEL_ET4_ET4_IN0		ET4 01 = ET4_IN0
			/ETIMx_ETxINSEL_GRP1SEL_ET4_RTC64HZ		ET4 10 = RTC64HZ/
			/ETIMx_ETxINSEL_GRP1SEL_ET4_LPTO			ET4 11 = LPTO
*********************************/
uint32_t ETIMx_ETxINSEL_GRP1SEL_Get(ETIMx_Type* ETIMx)
{
	return (ETIMx->ETxINSEL & ETIMx_ETxINSEL_GRP1SEL_Msk);
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET Signal1(计数源)的预分频
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			SetValue: 00-FF(00表示1分频，FF表示256分频)
输出: 无
*********************************/
void ETIMx_ETxPESCALE1_Write(ETIMx_Type* ETIMx, uint32_t SetValue)
{
	ETIMx->ETxPESCALE1 = (SetValue & ETIMx_ETxPESCALE1_PRESCALE1_Msk);
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET Signal1(计数源) 的预分频
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: 00-FF(00表示1分频，FF表示256分频)
*********************************/
uint32_t ETIMx_ETxPESCALE1_Read(ETIMx_Type* ETIMx)
{
	return (ETIMx->ETxPESCALE1 & ETIMx_ETxPESCALE1_PRESCALE1_Msk);
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET Signal2(捕捉源)的预分频
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			SetValue: 00-FF(00表示1分频，FF表示256分频)
输出: 无
*********************************/
void ETIMx_ETxPESCALE2_Write(ETIMx_Type* ETIMx, uint32_t SetValue)
{
	ETIMx->ETxPESCALE2 = (SetValue & ETIMx_ETxPESCALE2_PRESCALE2_Msk);
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET Signal2(捕捉源)的预分频
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: 00-FF(00表示1分频，FF表示256分频)
*********************************/
uint32_t ETIMx_ETxPESCALE2_Read(ETIMx_Type* ETIMx)
{
	return (ETIMx->ETxPESCALE2 & ETIMx_ETxPESCALE2_PRESCALE2_Msk);
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET 计数初值
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			SetValue: 00-FFFF
输出: 无
*********************************/
void ETIMx_ETxIVR_Write(ETIMx_Type* ETIMx, uint32_t SetValue)
{
	ETIMx->ETxIVR = (SetValue & ETIMx_ETxIVR_INITVALUE_Msk);
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET 计数值
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: 00-FFFF
*********************************/
uint32_t ETIMx_ETxIVR_Read(ETIMx_Type* ETIMx)
{
	return (ETIMx->ETxIVR & ETIMx_ETxIVR_INITVALUE_Msk);
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET 比较值(该值与计数值比较，若计数值>=该值，则产生计数匹配信号至输出控制模块，并产生相应中断)
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			SetValue: 00-FFFF
输出: 无
*********************************/
void ETIMx_ETxCMP_Write(ETIMx_Type* ETIMx, uint32_t SetValue)
{
	ETIMx->ETxCMP = (SetValue & ETIMx_ETxCMP_CMP_Msk);
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET 比较值
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: 00-FFFF
*********************************/
uint32_t ETIMx_ETxCMP_Read(ETIMx_Type* ETIMx)
{
	return (ETIMx->ETxCMP & ETIMx_ETxCMP_CMP_Msk);
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET 比较中断
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			ENABLE/DISABLE		比较中断使能/比较中断禁止	
输出: 无
*********************************/
void ETIMx_ETxIE_CMPIE_Setable(ETIMx_Type* ETIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		ETIMx->ETxIE |= (ETIMx_ETxIE_CMPIE_Msk);
	}
	else
	{
		ETIMx->ETxIE &= ~(ETIMx_ETxIE_CMPIE_Msk);
	}
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET 比较中断配置
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: ENABLE/DISABLE		比较中断使能/比较中断禁止	
*********************************/
FunState ETIMx_ETxIE_CMPIE_Getable(ETIMx_Type* ETIMx)
{
	if (ETIMx->ETxIE & (ETIMx_ETxIE_CMPIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET 捕捉中断
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			ENABLE/DISABLE		捕捉中断使能/捕捉中断禁止	
输出: 无
*********************************/
void ETIMx_ETxIE_CAPIE_Setable(ETIMx_Type* ETIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		ETIMx->ETxIE |= (ETIMx_ETxIE_CAPIE_Msk);
	}
	else
	{
		ETIMx->ETxIE &= ~(ETIMx_ETxIE_CAPIE_Msk);
	}
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET 捕捉中断
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: ENABLE/DISABLE		捕捉中断使能/捕捉中断禁止	
*********************************/
FunState ETIMx_ETxIE_CAPIE_Getable(ETIMx_Type* ETIMx)
{
	if (ETIMx->ETxIE & (ETIMx_ETxIE_CAPIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
externded timer高位计数器控制函数 
功能：配置ET 溢出中断
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
			ENABLE/DISABLE		溢出中断使能/溢出中断禁止	
输出: 无
*********************************/
void ETIMx_ETxIE_OVIE_Setable(ETIMx_Type* ETIMx, FunState NewState)
{
	if (NewState == ENABLE)
	{
		ETIMx->ETxIE |= (ETIMx_ETxIE_OVIE_Msk);
	}
	else
	{
		ETIMx->ETxIE &= ~(ETIMx_ETxIE_OVIE_Msk);
	}
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET 溢出中断
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: ENABLE/DISABLE		溢出中断使能/溢出中断禁止	
*********************************/
FunState ETIMx_ETxIE_OVIE_Getable(ETIMx_Type* ETIMx)
{
	if (ETIMx->ETxIE & (ETIMx_ETxIE_OVIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
externded timer高位计数器控制函数 
功能：清ET 比较中断标志
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: 无
*********************************/
void ETIMx_ETxIF_CMPIF_Clr(ETIMx_Type* ETIMx)
{
	ETIMx->ETxIF = ETIMx_ETxIF_CMPIF_Msk;
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET 比较中断标志
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: SET/RESET			已发生比较中断/未发生比较中断
*********************************/
FlagStatus ETIMx_ETxIF_CMPIF_Chk(ETIMx_Type* ETIMx)
{
	if (ETIMx->ETxIF & ETIMx_ETxIF_CMPIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*********************************
externded timer高位计数器控制函数 
功能：获取ET 捕捉沿状态
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: SET/RESET			脉冲宽度捕捉模式时表示捕捉到下沿/脉冲宽度捕捉模式时表示捕捉到上沿
*********************************/
FlagStatus ETIMx_ETxIF_EDGESTA_Chk(ETIMx_Type* ETIMx)
{
	if (ETIMx->ETxIF & ETIMx_ETxIF_EDGESTA_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*********************************
externded timer高位计数器控制函数 
功能：清ET 产生捕捉信号标志
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: 无
*********************************/
void ETIMx_ETxIF_CAPIF_Clr(ETIMx_Type* ETIMx)
{
	ETIMx->ETxIF = ETIMx_ETxIF_CAPIF_Msk;
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET 产生捕捉信号标志
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: SET/RESET			捕捉到指定的沿/未捕捉到指定的沿
*********************************/
FlagStatus ETIMx_ETxIF_CAPIF_Chk(ETIMx_Type* ETIMx)
{
	if (ETIMx->ETxIF & ETIMx_ETxIF_CAPIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*********************************
externded timer高位计数器控制函数 
功能：清ET 产生溢出信号标志
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: 无
*********************************/
void ETIMx_ETxIF_OVIF_Clr(ETIMx_Type* ETIMx)
{
	ETIMx->ETxIF = ETIMx_ETxIF_OVIF_Msk;
}
/*********************************
externded timer高位计数器控制函数 
功能：获取ET 产生溢出信号标志
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: SET/RESET			产生计数溢出/未产生溢出
*********************************/
FlagStatus ETIMx_ETxIF_OVIF_Chk(ETIMx_Type* ETIMx)
{
	if (ETIMx->ETxIF & ETIMx_ETxIF_OVIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*********************************
externded timer高位计数器控制函数 
功能：ET初始化为缺省配置
输入: ETIM1/ETIM2/ETIM3/ETIM4  	定时器名称
输出: 无
*********************************/
void ETIMx_Deinit(ETIMx_Type* ETIMx)
{
	ETIMx->ETxCR = 0x00000000;
	ETIMx->ETxINSEL = 0x00000000;
	ETIMx->ETxPESCALE1 = 0x00000000;
	ETIMx->ETxPESCALE2 = 0x00000000;
	ETIMx->ETxIVR = 0x00000000;
	ETIMx->ETxCMP = 0x00000000;
	ETIMx->ETxIE = 0x00000000;
	//ETIMx->ETxIF = 0x00000000;
}
//Code_End


/*ETIMx 初始化配置函数*/
void ETIMx_Init(ETIMx_Type* ETIMx, ETIM_InitTypeDef* para)
{
	//信号源参数
	ETIMx_ETxINSEL_SIG1SEL_Set(ETIMx, para->sig_src_para.SIG1SEL);		/* 内部信号1源选择（在计数模式下计数源仅由此选择，捕捉模式下计数源） */
	ETIMx_ETxINSEL_SIG2SEL_Set(ETIMx, para->sig_src_para.SIG2SEL);		/* 内部信号2源选择(捕捉源) */
	ETIMx_ETxINSEL_GRP1SEL_Set(ETIMx, para->sig_src_para.GRP1SEL);		/* GROUP1 信号选择控制 */
	ETIMx_ETxINSEL_GRP2SEL_Set(ETIMx, para->sig_src_para.GRP2SEL);		/* GROUP2 信号选择控制 */
	ETIMx_ETxPESCALE1_Write(ETIMx, para->sig_src_para.PRESCALE1);		/* ETx预分频寄存器1 */
	ETIMx_ETxPESCALE2_Write(ETIMx, para->sig_src_para.PRESCALE2);		/* ETx预分频寄存器2 */
	
	//控制类参数
	ETIMx_ETxCR_EXFLT_Setable(ETIMx, para->ctrl_para.EXFLT);		/* 引脚输入数字滤波使能 */
	ETIMx_ETxCR_MOD_Set(ETIMx, para->ctrl_para.MOD);				/* 工作模式选择 */
	ETIMx_ETxCR_CASEN_Setable(ETIMx, para->ctrl_para.CASEN);		/* 扩展定时器级联使能 */
	ETIMx_ETxCR_EDGESEL_Set(ETIMx, para->ctrl_para.EDGESEL);		/* 计数模式采沿方式选择（计数时钟选择mcu_clk时该位无效，总是采用mcu_clk时钟上升沿计数） */

	ETIMx_ETxCR_PWM_Setable(ETIMx, para->ctrl_para.PWM);		/* PWM输出控制 */

	ETIMx_ETxCR_CAPMOD_Set(ETIMx, para->ctrl_para.CAPMOD);			/* 捕捉模式控制 */
	ETIMx_ETxCR_CAPCLR_Setable(ETIMx, para->ctrl_para.CAPCLR);		/* 带清零捕捉模式控制 */
	ETIMx_ETxCR_CAPONCE_Setable(ETIMx, para->ctrl_para.CAPONCE);	/* 单次捕捉控制 */
	ETIMx_ETxCR_CAPEDGE_Set(ETIMx, para->ctrl_para.CAPEDGE);		/* 捕捉沿选择 */

	ETIMx_ETxIVR_Write(ETIMx, para->ctrl_para.INITVALUE);	/* ETx初值寄存器 */
	ETIMx_ETxCMP_Write(ETIMx, para->ctrl_para.CMP);			/* ETx比较寄存器 */

	ETIMx_ETxIE_CMPIE_Setable(ETIMx, para->ctrl_para.CMPIE);		/* 扩展定时器比较中断使能 */
	ETIMx_ETxIE_CAPIE_Setable(ETIMx, para->ctrl_para.CAPIE);		/* 扩展定时器捕捉中断使能 */
	ETIMx_ETxIE_OVIE_Setable(ETIMx, para->ctrl_para.OVIE);			/* 扩展定时器溢出中断使能 */

	ETIMx_ETxCR_CEN_Setable(ETIMx, para->ctrl_para.CEN);		/* 启动控制 */
}


/******END OF FILE****/


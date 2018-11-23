/**
  ******************************************************************************
  * @file    fm33a0xx_u7816.c
  * @author  FM33a0 Application Team
  * @version V2.0.2
  * @date    4-MAY-2017
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of....:
  *
*/
/* Includes ------------------------------------------------------------------*/

#include "FM33A0XX_u7816.h" 

//Code_Start
//2018-04-12-09-21-42
//FM33A0XX_Driver_Gen_V1.4

/*******************************************
函数名：U7816x_U7816CTRL_TXEN_Setable
函数功能： U7816通道发送使能控制
输入参数：U7816x：U78160、U78161
          NewState:ENABLE、DISABLE
输出参数：无
*******************************************/
void U7816x_U7816CTRL_TXEN_Setable(U7816x_Type* U7816x, FunState NewState)
{
	if (NewState == ENABLE)
	{
		U7816x->U7816CTRL |= (U7816x_U7816CTRL_TXEN_Msk);
	}
	else
	{
		U7816x->U7816CTRL &= ~(U7816x_U7816CTRL_TXEN_Msk);
	}
}

/*******************************************
函数名：U7816x_U7816CTRL_TXEN_Getable
函数功能： 获取U7816通道发送使能控制状态
输入参数：U7816x：U78160、U78161
输出参数：ENABLE、DISABLE
*******************************************/
FunState U7816x_U7816CTRL_TXEN_Getable(U7816x_Type* U7816x)
{
	if (U7816x->U7816CTRL & (U7816x_U7816CTRL_TXEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*******************************************
函数名：U7816x_U7816CTRL_RXEN_Setable
函数功能： U7816通道接收使能控制
输入参数：U7816x：U78160、U78161
          NewState:ENABLE、DISABLE
输出参数：无
*******************************************/
void U7816x_U7816CTRL_RXEN_Setable(U7816x_Type* U7816x, FunState NewState)
{
	if (NewState == ENABLE)
	{
		U7816x->U7816CTRL |= (U7816x_U7816CTRL_RXEN_Msk);
	}
	else
	{
		U7816x->U7816CTRL &= ~(U7816x_U7816CTRL_RXEN_Msk);
	}
}

/*******************************************
函数名：U7816x_U7816CTRL_RXEN_Getable
函数功能： 获取U7816通道接收使能控制状态
输入参数：U7816x：U78160、U78161
输出参数：ENABLE、DISABLE
*******************************************/
FunState U7816x_U7816CTRL_RXEN_Getable(U7816x_Type* U7816x)
{
	if (U7816x->U7816CTRL & (U7816x_U7816CTRL_RXEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*******************************************
函数名：U7816x_U7816CTRL_CKOEN_Setable
函数功能： U7816时钟CLK输出使能控制 
输入参数：U7816x：U78160、U78161
          NewState:ENABLE、DISABLE
输出参数：无
*******************************************/
void U7816x_U7816CTRL_CKOEN_Setable(U7816x_Type* U7816x, FunState NewState)
{
	if (NewState == ENABLE)
	{
		U7816x->U7816CTRL |= (U7816x_U7816CTRL_CKOEN_Msk);
	}
	else
	{
		U7816x->U7816CTRL &= ~(U7816x_U7816CTRL_CKOEN_Msk);
	}
}

/*******************************************
函数名：U7816x_U7816CTRL_CKOEN_Getable
函数功能： 获取U7816时钟CLK输出使能控制状态
输入参数：U7816x：U78160、U78161
输出参数：ENABLE、DISABLE
*******************************************/
FunState U7816x_U7816CTRL_CKOEN_Getable(U7816x_Type* U7816x)
{
	if (U7816x->U7816CTRL & (U7816x_U7816CTRL_CKOEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}


/*******************************************
函数名：U7816x_U7816CTRL_HPUAT_Setable
函数功能： U7816通道数据发送强上拉电阻自动有效控制  
输入参数：U7816x：U78160、U78161
          NewState:ENABLE、DISABLE
输出参数：无
*******************************************/
void U7816x_U7816CTRL_HPUAT_Setable(U7816x_Type* U7816x, FunState NewState)
{
	if (NewState == ENABLE)
	{
		U7816x->U7816CTRL |= (U7816x_U7816CTRL_HPUAT_Msk);
	}
	else
	{
		U7816x->U7816CTRL &= ~(U7816x_U7816CTRL_HPUAT_Msk);
	}
}

/*******************************************
函数名：U7816x_U7816CTRL_HPUAT_Getable
函数功能： 获取U7816通道数据发送强上拉电阻自动有效控制状态  
输入参数：U7816x：U78160、U78161
输出参数：ENABLE、DISABLE
*******************************************/
FunState U7816x_U7816CTRL_HPUAT_Getable(U7816x_Type* U7816x)
{
	if (U7816x->U7816CTRL & (U7816x_U7816CTRL_HPUAT_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*******************************************
函数名：U7816x_U7816CTRL_HPUEN_Setable
函数功能：U7816通道强上拉使能控制 
输入参数：U7816x：U78160、U78161
          NewState:ENABLE、DISABLE
输出参数：无
*******************************************/
void U7816x_U7816CTRL_HPUEN_Setable(U7816x_Type* U7816x, FunState NewState)
{
	if (NewState == ENABLE)
	{
		U7816x->U7816CTRL |= (U7816x_U7816CTRL_HPUEN_Msk);
	}
	else
	{
		U7816x->U7816CTRL &= ~(U7816x_U7816CTRL_HPUEN_Msk);
	}
}

/*******************************************
函数名：U7816x_U7816CTRL_HPUEN_Getable
函数功能：获取U7816通道强上拉使能控制状态 
输入参数：U7816x：U78160、U78161
输出参数：ENABLE、DISABLE
*******************************************/
FunState U7816x_U7816CTRL_HPUEN_Getable(U7816x_Type* U7816x)
{
	if (U7816x->U7816CTRL & (U7816x_U7816CTRL_HPUEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*******************************************
函数名：U7816x_U7816FRC_ERSW_Set
函数功能:ERROR SIGNAL宽度选择
输入参数：U7816x：U78160、U78161
          SetValue：U7816x_U7816FRC_ERSW_1ETU、U7816x_U7816FRC_ERSW_1P5ETU、U7816x_U7816FRC_ERSW_2ETU
输出参数：无
*******************************************/
void U7816x_U7816FRC_ERSW_Set(U7816x_Type* U7816x, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = U7816x->U7816FRC;
	tmpreg &= ~(U7816x_U7816FRC_ERSW_Msk);
	tmpreg |= (SetValue & U7816x_U7816FRC_ERSW_Msk);
	U7816x->U7816FRC = tmpreg;
}

/*******************************************
函数名：U7816x_U7816FRC_ERSW_Get
函数功能: 获取ERROR SIGNAL宽度选择
输入参数：U7816x：U78160、U78161
输出参数：U7816x_U7816FRC_ERSW_1ETU、U7816x_U7816FRC_ERSW_1P5ETU、U7816x_U7816FRC_ERSW_2ETU
*******************************************/
uint32_t U7816x_U7816FRC_ERSW_Get(U7816x_Type* U7816x)
{
	return (U7816x->U7816FRC & U7816x_U7816FRC_ERSW_Msk);
}


/*******************************************
函数名：U7816x_U7816FRC_ERSGD_Set
函数功能: ERROR SIGNAL后GUARDTIME宽度选择（仅在发送时有效） 
输入参数：U7816x：U78160、U78161
          SetValueU7816x_U7816FRC_ERSGD_1PETU、U7816x_U7816FRC_ERSGD_2PETU
输出参数:无
*******************************************/
void U7816x_U7816FRC_ERSGD_Set(U7816x_Type* U7816x, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = U7816x->U7816FRC;
	tmpreg &= ~(U7816x_U7816FRC_ERSGD_Msk);
	tmpreg |= (SetValue & U7816x_U7816FRC_ERSGD_Msk);
	U7816x->U7816FRC = tmpreg;
}

/*******************************************
函数名：U7816x_U7816FRC_ERSGD_Get
函数功能: 获取ERROR SIGNAL后GUARDTIME宽度选择 
输入参数：U7816x：U78160、U78161
输出参数:U7816x_U7816FRC_ERSGD_1PETU、U7816x_U7816FRC_ERSGD_2PETU
*******************************************/
uint32_t U7816x_U7816FRC_ERSGD_Get(U7816x_Type* U7816x)
{
	return (U7816x->U7816FRC & U7816x_U7816FRC_ERSGD_Msk);
}

/*******************************************
函数名：U7816x_U7816FRC_BGTEN_Setable
函数功能:BGT（block guard time）控制
输入参数：U7816x：U78160、U78161
          NewState：ENABLE、DISABLE
输出参数:无
*******************************************/
void U7816x_U7816FRC_BGTEN_Setable(U7816x_Type* U7816x, FunState NewState)
{
	if (NewState == ENABLE)
	{
		U7816x->U7816FRC |= (U7816x_U7816FRC_BGTEN_Msk);
	}
	else
	{
		U7816x->U7816FRC &= ~(U7816x_U7816FRC_BGTEN_Msk);
	}
}

/*******************************************
函数名：U7816x_U7816FRC_BGTEN_Getable
函数功能:获取BGT（block guard time）控制
输入参数：U7816x：U78160、U78161
输出参数:ENABLE、DISABLE
*******************************************/
FunState U7816x_U7816FRC_BGTEN_Getable(U7816x_Type* U7816x)
{
	if (U7816x->U7816FRC & (U7816x_U7816FRC_BGTEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*******************************************
函数名：U7816x_U7816FRC_REP_T_Set
函数功能:控制接收数据奇偶校验出错时自动重发次数 
输入参数：U7816x：U78160、U78161
          SetValue:U7816x_U7816FRC_REP_T_1TIME、U7816x_U7816FRC_REP_T_3TIME
输出参数:无
*******************************************/
void U7816x_U7816FRC_REP_T_Set(U7816x_Type* U7816x, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = U7816x->U7816FRC;
	tmpreg &= ~(U7816x_U7816FRC_REP_T_Msk);
	tmpreg |= (SetValue & U7816x_U7816FRC_REP_T_Msk);
	U7816x->U7816FRC = tmpreg;
}

/*******************************************
函数名：U7816x_U7816FRC_REP_T_Get
函数功能:获取接收数据奇偶校验出错时自动重发次数 
输入参数：U7816x：U78160、U78161
输出参数:U7816x_U7816FRC_REP_T_1TIME、U7816x_U7816FRC_REP_T_3TIME
*******************************************/
uint32_t U7816x_U7816FRC_REP_T_Get(U7816x_Type* U7816x)
{
	return (U7816x->U7816FRC & U7816x_U7816FRC_REP_T_Msk);
}


/*******************************************
函数名：U7816x_U7816FRC_PAR_Set
函数功能:奇偶校验类型选择 
输入参数：U7816x：U78160、U78161
          SetValue:U7816x_U7816FRC_PAR_EVEN、U7816x_U7816FRC_PAR_ODD、U7816x_U7816FRC_PAR_ALWAYS1、U7816x_U7816FRC_PAR_NONE
输出参数:无
*******************************************/
void U7816x_U7816FRC_PAR_Set(U7816x_Type* U7816x, uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = U7816x->U7816FRC;
	tmpreg &= ~(U7816x_U7816FRC_PAR_Msk);
	tmpreg |= (SetValue & U7816x_U7816FRC_PAR_Msk);
	U7816x->U7816FRC = tmpreg;
}

/*******************************************
函数名：U7816x_U7816FRC_PAR_Get
函数功能:获取奇偶校验类型选择 
输入参数：U7816x：U78160、U78161
输出参数:U7816x_U7816FRC_PAR_EVEN、U7816x_U7816FRC_PAR_ODD、U7816x_U7816FRC_PAR_ALWAYS1、U7816x_U7816FRC_PAR_NONE
*******************************************/
uint32_t U7816x_U7816FRC_PAR_Get(U7816x_Type* U7816x)
{
	return (U7816x->U7816FRC & U7816x_U7816FRC_PAR_Msk);
}

/*******************************************
函数名：U7816x_U7816FRC_FREN_Setable
函数功能:Guard Time长度控制位（发送时严格按照协议2etu）
输入参数：U7816x：U78160、U78161
          NewState：ENABLE、DISABLE
输出参数:无
*******************************************/
void U7816x_U7816FRC_FREN_Setable(U7816x_Type* U7816x, FunState NewState)
{
	if (NewState == ENABLE)
	{
		U7816x->U7816FRC |= (U7816x_U7816FRC_FREN_Msk);
	}
	else
	{
		U7816x->U7816FRC &= ~(U7816x_U7816FRC_FREN_Msk);
	}
}

/*******************************************
函数名：U7816x_U7816FRC_FREN_Getable
函数功能:获取Guard Time长度控制位
输入参数：U7816x：U78160、U78161
输出参数:ENABLE、DISABLE
*******************************************/
FunState U7816x_U7816FRC_FREN_Getable(U7816x_Type* U7816x)
{
	if (U7816x->U7816FRC & (U7816x_U7816FRC_FREN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*******************************************
函数名：U7816x_U7816FRC_TREPEN_Setable
函数功能:发送数据奇偶校验错的处理方式选择 
输入参数：U7816x：U78160、U78161
          NewState：ENABLE、DISABLE
输出参数:无
*******************************************/
void U7816x_U7816FRC_TREPEN_Setable(U7816x_Type* U7816x, FunState NewState)
{
	if (NewState == ENABLE)
	{
		U7816x->U7816FRC |= (U7816x_U7816FRC_TREPEN_Msk);
	}
	else
	{
		U7816x->U7816FRC &= ~(U7816x_U7816FRC_TREPEN_Msk);
	}
}
/*******************************************
函数名：U7816x_U7816FRC_TREPEN_Getable
函数功能:获取发送数据奇偶校验错的处理方式选择 
输入参数：U7816x：U78160、U78161
输出参数:ENABLE、DISABLE
*******************************************/
FunState U7816x_U7816FRC_TREPEN_Getable(U7816x_Type* U7816x)
{
	if (U7816x->U7816FRC & (U7816x_U7816FRC_TREPEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*******************************************
函数名：U7816x_U7816FRC_RREPEN_Setable
函数功能:接收数据奇偶校验错的处理方式选择 
输入参数：U7816x：U78160、U78161
          NewState：ENABLE、DISABLE
输出参数:无
*******************************************/
void U7816x_U7816FRC_RREPEN_Setable(U7816x_Type* U7816x, FunState NewState)
{
	if (NewState == ENABLE)
	{
		U7816x->U7816FRC |= (U7816x_U7816FRC_RREPEN_Msk);
	}
	else
	{
		U7816x->U7816FRC &= ~(U7816x_U7816FRC_RREPEN_Msk);
	}
}

/*******************************************
函数名：U7816x_U7816FRC_RREPEN_Getable
函数功能:获取接收数据奇偶校验错的处理方式选择 
输入参数：U7816x：U78160、U78161
输出参数:ENABLE、DISABLE
*******************************************/
FunState U7816x_U7816FRC_RREPEN_Getable(U7816x_Type* U7816x)
{
	if (U7816x->U7816FRC & (U7816x_U7816FRC_RREPEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*******************************************
函数名：U7816x_U7816FRC_DICONV_Setable
函数功能:传输次序反向编码使能 
输入参数：U7816x：U78160、U78161
          NewState：ENABLE、DISABLE
输出参数:无
*******************************************/
void U7816x_U7816FRC_DICONV_Setable(U7816x_Type* U7816x, FunState NewState)
{
	if (NewState == ENABLE)
	{
		U7816x->U7816FRC |= (U7816x_U7816FRC_DICONV_Msk);
	}
	else
	{
		U7816x->U7816FRC &= ~(U7816x_U7816FRC_DICONV_Msk);
	}
}

/*******************************************
函数名：U7816x_U7816FRC_DICONV_Getable
函数功能:获取传输次序反向编码状态 
输入参数：U7816x：U78160、U78161
输出参数:ENABLE、DISABLE
*******************************************/
FunState U7816x_U7816FRC_DICONV_Getable(U7816x_Type* U7816x)
{
	if (U7816x->U7816FRC & (U7816x_U7816FRC_DICONV_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*******************************************
函数名：U7816x_U7816EGT_Write
函数功能:发送时插入的EGT时间（以ETU为单位） 
输入参数：U7816x：U78160、U78161
          SetValue：插入时间
输出参数:无
*******************************************/
void U7816x_U7816EGT_Write(U7816x_Type* U7816x, uint32_t SetValue)
{
	U7816x->U7816EGT = (SetValue & U7816x_U7816EGT_TXEGT_Msk);
}

/*******************************************
函数名：U7816x_U7816EGT_Read
函数功能:获取发送时插入的EGT时间
输入参数：U7816x：U78160、U78161
输出参数:EGT寄存器值
*******************************************/
uint32_t U7816x_U7816EGT_Read(U7816x_Type* U7816x)
{
	return (U7816x->U7816EGT & U7816x_U7816EGT_TXEGT_Msk);
}

/*******************************************
函数名：U7816x_U7816CLKDIV_Write
函数功能:U7816时钟输出分频控制寄存器
输入参数：U7816x：U78160、U78161
          SetValue：分频值
输出参数:无
*******************************************/
void U7816x_U7816CLKDIV_Write(U7816x_Type* U7816x, uint32_t SetValue)
{
	U7816x->U7816CLKDIV = (SetValue & U7816x_U7816CLKDIV_CLKDIV_Msk);
}

/*******************************************
函数名：U7816x_U7816CLKDIV_Read
函数功能:获取U7816时钟输出分频控制寄存器值
输入参数：U7816x：U78160、U78161
输出参数:CLKDIV寄存器值
*******************************************/
uint32_t U7816x_U7816CLKDIV_Read(U7816x_Type* U7816x)
{
	return (U7816x->U7816CLKDIV & U7816x_U7816CLKDIV_CLKDIV_Msk);
}

/*******************************************
函数名：U7816x_U7816PDIV_Write
函数功能:U7816预分频控制寄存器，控制7816通信分频比（波特率）
输入参数：U7816x：U78160、U78161
SetValue:预分频值
输出参数:无
*******************************************/
void U7816x_U7816PDIV_Write(U7816x_Type* U7816x, uint32_t SetValue)
{
	U7816x->U7816PDIV = (SetValue & U7816x_U7816PDIV_PDIV_Msk);
}

/*******************************************
函数名：U7816x_U7816PDIV_Read
函数功能:获取U7816预分频控制寄存器值
输入参数：U7816x：U78160、U78161
输出参数:PDIV寄存器值
*******************************************/
uint32_t U7816x_U7816PDIV_Read(U7816x_Type* U7816x)
{
	return (U7816x->U7816PDIV & U7816x_U7816PDIV_PDIV_Msk);
}

/*******************************************
函数名：U7816x_U7816RXBUF_Read
函数功能:读U7816数据接收缓存寄存器
输入参数：U7816x：U78160、U78161
输出参数:接收值
*******************************************/
uint32_t U7816x_U7816RXBUF_Read(U7816x_Type* U7816x)
{
	return (U7816x->U7816RXBUF & U7816x_U7816RXBUF_RXBUF_Msk);
}

/*******************************************
函数名：U7816x_U7816TXBUF_Write
函数功能:U7816数据发送数据
输入参数：U7816x：U78160、U78161
          SetValue：发送数据
输出参数:无
*******************************************/
void U7816x_U7816TXBUF_Write(U7816x_Type* U7816x, uint32_t SetValue)
{
	U7816x->U7816TXBUF = (SetValue & U7816x_U7816TXBUF_TXBUF_Msk);
}

/*******************************************
函数名：U7816x_U7816IE_RXIE_Setable
函数功能:数据接收中断使能位。对应RX_FLAG中断标志位 
输入参数：U7816x：U78160、U78161
          NewState：ENABLE、DISABLE
输出参数:无
*******************************************/
void U7816x_U7816IE_RXIE_Setable(U7816x_Type* U7816x, FunState NewState)
{
	if (NewState == ENABLE)
	{
		U7816x->U7816IE |= (U7816x_U7816IE_RXIE_Msk);
	}
	else
	{
		U7816x->U7816IE &= ~(U7816x_U7816IE_RXIE_Msk);
	}
}

/*******************************************
函数名：U7816x_U7816IE_RXIE_Getable
函数功能:获取数据接收中断使能位状态
输入参数：U7816x：U78160、U78161         
输出参数:ENABLE、DISABLE
*******************************************/
FunState U7816x_U7816IE_RXIE_Getable(U7816x_Type* U7816x)
{
	if (U7816x->U7816IE & (U7816x_U7816IE_RXIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*******************************************
函数名：U7816x_U7816IE_TXIE_Setable
函数功能:数据发送中断使能位。对应TX_FLAG中断标志位
输入参数：U7816x：U78160、U78161
          NewState：ENABLE、DISABLE
输出参数:无
*******************************************/
void U7816x_U7816IE_TXIE_Setable(U7816x_Type* U7816x, FunState NewState)
{
	if (NewState == ENABLE)
	{
		U7816x->U7816IE |= (U7816x_U7816IE_TXIE_Msk);
	}
	else
	{
		U7816x->U7816IE &= ~(U7816x_U7816IE_TXIE_Msk);
	}
}
/*******************************************
函数名：U7816x_U7816IE_TXIE_Getable
函数功能:获取数据发送中断使能位
输入参数：U7816x：U78160、U78161        
输出参数:ENABLE、DISABLE
*******************************************/
FunState U7816x_U7816IE_TXIE_Getable(U7816x_Type* U7816x)
{
	if (U7816x->U7816IE & (U7816x_U7816IE_TXIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*******************************************
函数名：U7816x_U7816IE_LSIE_Setable
函数功能:线路状态中断使能位。对应ERROR_FLAG中断标志位
输入参数：U7816x：U78160、U78161
          NewState：ENABLE、DISABLE
输出参数:无
*******************************************/
void U7816x_U7816IE_LSIE_Setable(U7816x_Type* U7816x, FunState NewState)
{
	if (NewState == ENABLE)
	{
		U7816x->U7816IE |= (U7816x_U7816IE_LSIE_Msk);
	}
	else
	{
		U7816x->U7816IE &= ~(U7816x_U7816IE_LSIE_Msk);
	}
}
/*******************************************
函数名：U7816x_U7816IE_LSIE_Getable
函数功能:获取线路状态中断使能位
输入参数：U7816x：U78160、U78161        
输出参数:ENABLE、DISABLE
*******************************************/
FunState U7816x_U7816IE_LSIE_Getable(U7816x_Type* U7816x)
{
	if (U7816x->U7816IE & (U7816x_U7816IE_LSIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*******************************************
函数名：U7816x_U7816IF_ERRIF_Chk
函数功能:获取错误中断标志
输入参数：U7816x：U78160、U78161
输出参数:SET、RESET
*******************************************/
FlagStatus U7816x_U7816IF_ERRIF_Chk(U7816x_Type* U7816x)
{
	if (U7816x->U7816IF & U7816x_U7816IF_ERRIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*******************************************
函数名：U7816x_U7816IF_TXIF_Chk
函数功能:查询发送缓冲区空标志 
输入参数：U7816x：U78160、U78161
输出参数:SET、RESET
*******************************************/
FlagStatus U7816x_U7816IF_TXIF_Chk(U7816x_Type* U7816x)
{
	if (U7816x->U7816IF & U7816x_U7816IF_TXIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*******************************************
函数名：U7816x_U7816IF_RXIF_Chk
函数功能:查询接收完成标志
输入参数：U7816x：U78160、U78161
输出参数:SET、RESET
*******************************************/
FlagStatus U7816x_U7816IF_RXIF_Chk(U7816x_Type* U7816x)
{
	if (U7816x->U7816IF & U7816x_U7816IF_RXIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*******************************************
函数名：U7816x_U7816ERR_TPARERR_Clr
函数功能:发送数据奇偶校验错误标志位清零
输入参数：U7816x：U78160、U78161
输出参数: 无
*******************************************/
void U7816x_U7816ERR_TPARERR_Clr(U7816x_Type* U7816x)
{
	U7816x->U7816ERR = U7816x_U7816ERR_TPARERR_Msk;
}

/*******************************************
函数名：U7816x_U7816ERR_TPARERR_Chk
函数功能:查询发送数据奇偶校验错误标志位
输入参数：U7816x：U78160、U78161
输出参数: SET、RESET 
*******************************************/
FlagStatus U7816x_U7816ERR_TPARERR_Chk(U7816x_Type* U7816x)
{
	if (U7816x->U7816ERR & U7816x_U7816ERR_TPARERR_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*******************************************
函数名：U7816x_U7816ERR_RPARERR_Clr
函数功能:接收数据奇偶校验错误标志位清零
输入参数：U7816x：U78160、U78161
输出参数: 无
*******************************************/
void U7816x_U7816ERR_RPARERR_Clr(U7816x_Type* U7816x)
{
	U7816x->U7816ERR = U7816x_U7816ERR_RPARERR_Msk;
}

/*******************************************
函数名：U7816x_U7816ERR_RPARERR_Chk
函数功能:查询接收数据奇偶校验错误标志位
输入参数：U7816x：U78160、U78161
输出参数: SET、RESET 
*******************************************/
FlagStatus U7816x_U7816ERR_RPARERR_Chk(U7816x_Type* U7816x)
{
	if (U7816x->U7816ERR & U7816x_U7816ERR_RPARERR_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*******************************************
函数名：U7816x_U7816ERR_FRERR_Clr
函数功能:接收帧格式错误标志位清零
输入参数：U7816x：U78160、U78161
输出参数: 无
*******************************************/
void U7816x_U7816ERR_FRERR_Clr(U7816x_Type* U7816x)
{
	U7816x->U7816ERR = U7816x_U7816ERR_FRERR_Msk;
}
/*******************************************
函数名：U7816x_U7816ERR_FRERR_Chk
函数功能:查询接收帧格式错误标志位清零
输入参数：U7816x：U78160、U78161
输出参数: SET、RESET 
*******************************************/
FlagStatus U7816x_U7816ERR_FRERR_Chk(U7816x_Type* U7816x)
{
	if (U7816x->U7816ERR & U7816x_U7816ERR_FRERR_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*******************************************
函数名：U7816x_U7816ERR_OVERR_Clr
函数功能:接收溢出错误标志位清零
输入参数：U7816x：U78160、U78161
输出参数: 无
*******************************************/
void U7816x_U7816ERR_OVERR_Clr(U7816x_Type* U7816x)
{
	U7816x->U7816ERR = U7816x_U7816ERR_OVERR_Msk;
}
/*******************************************
函数名：U7816x_U7816ERR_OVERR_Chk
函数功能:查询接收溢出错误标志位清零
输入参数：U7816x：U78160、U78161
输出参数: SET、RESET 
*******************************************/
FlagStatus U7816x_U7816ERR_OVERR_Chk(U7816x_Type* U7816x)
{
	if (U7816x->U7816ERR & U7816x_U7816ERR_OVERR_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}


/*******************************************
函数名：U7816x_U7816STA_WAIT_RPT_Chk
函数功能: U7816接口发送了错误信号，正在等待对方重发数据.状态机进入发送错误信号状态时置位，获取状态位
输入参数：U7816x：U78160、U78161
输出参数: SET、 RESET
*******************************************/
FlagStatus U7816x_U7816STA_WAIT_RPT_Chk(U7816x_Type* U7816x)
{
	if (U7816x->U7816STA & U7816x_U7816STA_WAIT_RPT_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*******************************************
函数名：U7816x_U7816STA_TXBUSY_Chk
函数功能: 获取发送数据忙标志
输入参数：U7816x：U78160、U78161
输出参数: SET、 RESET
*******************************************/
FlagStatus U7816x_U7816STA_TXBUSY_Chk(U7816x_Type* U7816x)
{
	if (U7816x->U7816STA & U7816x_U7816STA_TXBUSY_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*******************************************
函数名：U7816x_U7816STA_RXBUSY_Chk
函数功能: 获取接收数据忙标志
输入参数：U7816x：U78160、U78161
输出参数: SET、 RESET
*******************************************/
FlagStatus U7816x_U7816STA_RXBUSY_Chk(U7816x_Type* U7816x)
{
	if (U7816x->U7816STA & U7816x_U7816STA_RXBUSY_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*******************************************
函数名：U7816x_Deinit
函数功能: U7816模块寄存器恢复到复位值
输入参数：U7816x：U78160、U78161
输出参数: 无
*******************************************/
void U7816x_Deinit(U7816x_Type* U7816x)
{
	U7816x->U7816CTRL = 0x00000000;
	U7816x->U7816FRC = 0x00000006;
	U7816x->U7816EGT = 0x00000000;
	U7816x->U7816CLKDIV = 0x00000003;
	U7816x->U7816PDIV = 0x00000173;
	//U7816x->U7816RXBUF = ;
	//U7816x->U7816TXBUF = ;
	U7816x->U7816IE = 0x00000000;
	//U7816x->U7816IF = 0x00000002;
	//U7816x->U7816ERR = 0x00000000;
	//U7816x->U7816STA = 0x00000000;
}
//Code_End


/* U7816完整参数初始化函数 */
/*******************************************
函数名：U7816_Init
函数功能: U7816完整参数初始化函数
输入参数：U7816x：U78160、U78161
          para：U7816_InitTypeDef数组
输出参数: 无
*******************************************/
void U7816_Init(U7816x_Type* U7816x, U7816_InitTypeDef* para)
{
	U7816x_U7816CTRL_TXEN_Setable(U7816x, para->TXEN);		//发送使能
	U7816x_U7816CTRL_RXEN_Setable(U7816x, para->RXEN);		//接受使能
	U7816x_U7816CTRL_CKOEN_Setable(U7816x, para->CKOEN);		//U7816时钟CLK输出使能控制
	U7816x_U7816CTRL_HPUAT_Setable(U7816x, para->HPUAT);		//U7816通道数据发送强上拉电阻自动有效控制
	U7816x_U7816CTRL_HPUEN_Setable(U7816x, para->HPUEN);		//U7816通道强上拉使能控制
	
	U7816x_U7816FRC_ERSW_Set(U7816x, para->ERSW);		//ERROR SIGNAL宽度选择
	U7816x_U7816FRC_ERSGD_Set(U7816x, para->ERSGD);		//ERROR SIGNAL后GUARDTIME宽度选择（仅在发送时有效）
	U7816x_U7816FRC_BGTEN_Setable(U7816x, para->BGTEN);		//BGT（block guard time）控制
	U7816x_U7816FRC_REP_T_Set(U7816x, para->REP_T);		//控制接收数据奇偶校验出错时自动重发次数
	U7816x_U7816FRC_PAR_Set(U7816x, para->PAR);		//奇偶校验类型选择
	U7816x_U7816FRC_FREN_Setable(U7816x, para->FREN);		//Guard Time长度控制位（发送时严格按照协议2etu）
	U7816x_U7816FRC_TREPEN_Setable(U7816x, para->TREPEN);		//缓发送数据奇偶校验错的处理方式选择
	U7816x_U7816FRC_RREPEN_Setable(U7816x, para->RREPEN);		//接收数据奇偶校验错的处理方式选择
	U7816x_U7816FRC_DICONV_Setable(U7816x, para->DICONV);		//传输次序反向编码使能
	
	U7816x_U7816EGT_Write(U7816x, para->TXEGT);		//发送时插入的EGT时间（以ETU为单位）
	U7816x_U7816CLKDIV_Write(U7816x, para->CLKDIV);		//U7816时钟输出分频控制寄存器
	U7816x_U7816PDIV_Write(U7816x, para->PDIV);		//U7816预分频控制寄存器，控制7816通信分频比（波特率）
	
	U7816x_U7816IE_RXIE_Setable(U7816x, para->RXIE);		//数据接收中断使能位。对应RX_FLAG中断标志位
	U7816x_U7816IE_TXIE_Setable(U7816x, para->TXIE);		//数据发送中断使能位。对应TX_FLAG中断标志位
	U7816x_U7816IE_LSIE_Setable(U7816x, para->LSIE);		//线路状态中断使能位。对应ERROR_FLAG中断标志位
}

/******END OF FILE****/

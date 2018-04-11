/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_timer.c
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  Timer模块库函数
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#include "lib_timer.h"


/***************************************************************
  函数名：T16Nx_BaseInit
  描  述：T16Nx基本初始化,初始化时钟选择，外部时钟同步，外部时钟计数边缘选择，工作模式。
  输入值：T16Nx：可以是T16N0/1/2/3 、T16Nx_BaseInitStruct：初始化配置结构体地址
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_BaseInit(T16N_TypeDef* T16Nx,T16Nx_BaseInitStruType* T16Nx_BaseInitStruct)
{
	T16Nx->CON0.CS = T16Nx_BaseInitStruct->T16Nx_ClkS;     //计数时钟源选择
	T16Nx->CON0.SYNC = T16Nx_BaseInitStruct->T16Nx_SYNC;   //外部时钟同步使能  Disable:不同步外部时钟，为异步计数模式。Enable：通过PCLK对外部时钟同步，为同步计数模式，外部时钟的高低电平平均至少保持2个时钟周期
	T16Nx->CON0.EDGE = T16Nx_BaseInitStruct->T16Nx_EDGE;   //外部时钟计数边沿选择
	T16Nx->CON0.MOD = T16Nx_BaseInitStruct->T16Nx_Mode;    //工作模式选择
	T16Nx->PREMAT.PREMAT = T16Nx_BaseInitStruct->T16Nx_PREMAT-1;  //预分频比设置，范围：1-256
}


/***************************************************************
  函数名：T16Nx_CapInit
  描  述：T16Nx捕捉初始化
  输入值：：T16Nx：可以是T16N0/1/2/3、 T16Nx_CapInitStruct：初始化配置结构体地址
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_CapInit(T16N_TypeDef* T16Nx,T16Nx_CapInitStruType* T16Nx_CapInitStruct)
{

	T16Nx->CON1.CAPL0=T16Nx_CapInitStruct->T16Nx_CAPCAPL0;   //配置捕捉0重载计数器是否使能
	T16Nx->CON1.CAPL1=T16Nx_CapInitStruct->T16Nx_CAPCAPL1;   //配置捕捉1重载计数器是否使能 
	T16Nx->CON1.CAPPE=T16Nx_CapInitStruct->T16Nx_CapRise;    //配置上升沿输入捕捉是否使能
	T16Nx->CON1.CAPNE=T16Nx_CapInitStruct->T16Nx_CapFall;    //配置下降沿输入捕捉是否使能
	T16Nx->CON1.CAPIS0=T16Nx_CapInitStruct->T16Nx_CapIS0;    //配置捕捉输入端口T16NxIN0是否使能
	T16Nx->CON1.CAPIS1=T16Nx_CapInitStruct->T16Nx_CapIS1;    //配置捕捉输入端口T16NxIN1是否使能
	T16Nx->CON1.CAPT=T16Nx_CapInitStruct->T16Nx_CapTime;     //配置捕捉次数

}

/***************************************************************
  函数名：T16Nx_PMWOutInit
  描  述：T16NxPWM输出初始化
  输入值：：T16Nx：可以是T16N0/1/2/3、 T16Nx_PWMInitStruct：初始化配置结构体地址
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_PMWOutInit(T16N_TypeDef* T16Nx,T16Nx_PWMInitStruType* T16Nx_PWMInitStruct)
{
	T16Nx->CON2.MOE0 = T16Nx_PWMInitStruct->T16Nx_MOE0;   				//配置输出端口0是否使能
	T16Nx->CON2.MOE1 = T16Nx_PWMInitStruct->T16Nx_MOE1;           //配置输出端口1是否使能
	T16Nx->CON2.POL0 = T16Nx_PWMInitStruct->T16Nx_POL0;           //配置T16NxOut0输出极性
	T16Nx->CON2.POL1 = T16Nx_PWMInitStruct->T16Nx_POL1;           //配置T16NxOut1输出极性
	T16Nx->CON2.PWMMOD = T16Nx_PWMInitStruct->T16Nx_PWMMODE;      //配置PWM模式选择
	T16Nx->CON2.PWMDZE = T16Nx_PWMInitStruct->PWMDZE;             //配置PWM互补模式死区是否使能
}

/***************************************************************
  函数名：T16Nx_PMWPDZ_Config
  描  述：T16NxPWM互补模式死区宽度配置
  输入值：：T16Nx：可以是T16N0/1/2/3、 PWM_PDZ_data：死区周期配置1-256
  输出值：无
  返回值：无
 ***************************************************************/

void T16Nx_PWMPDZ_Config(T16N_TypeDef* T16Nx,unsigned int PWM_PDZ_data)
{
	T16Nx->PDZ.PDZ = PWM_PDZ_data-1;
}

/***************************************************************
  函数名：T16Nx_MATxITConfig
  描  述：T16Nx匹配后的工作模式配置
  输入值：T16Nx：可以是T16N0/1/2/3、 Type：匹配后的工作模式
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_MAT0ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATCON Type)
{  
	T16Nx->CON0.MAT0S=Type;
}
void T16Nx_MAT1ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATCON Type)
{
	T16Nx->CON0.MAT1S=Type;
}
void T16Nx_MAT2ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATCON Type)
{
	T16Nx->CON0.MAT2S=Type;
}
void T16Nx_MAT3ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATCON Type)
{
	T16Nx->CON0.MAT3S=Type;
}


/***************************************************************
  函数名：T16Nx_MATxOutxConfig
  描  述：T16Nx匹配后的输出端口的模式配置
  输入值：T16Nx：可以是T16N0/1/2/3 、 Type：匹配后端口的工作模式
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_MAT0Out0Config(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATOUT Type)
{
	T16Nx->CON2.MOM0=Type;
}
void T16Nx_MAT1Out0Config(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATOUT Type)
{
	T16Nx->CON2.MOM1=Type;
}
void T16Nx_MAT2Out1Config(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATOUT Type)
{
	T16Nx->CON2.MOM2=Type;
}
void T16Nx_MAT3Out1Config(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATOUT Type)
{
	T16Nx->CON2.MOM3=Type;
}


/***************************************************************
  函数名：T16Nx_ITConfig
  描  述：T16N中断配置
  输入值：T16Nx：可以是T16N0/1/2/3、 Type：中断类型 、 NewState：使能/失能
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_IT Type,TYPE_FUNCEN NewState)
{
	if (NewState != DISABLE)
	{
		T16Nx->IE.Word |= (uint32_t)Type;
	}
	else
	{
		T16Nx->IE.Word &= (~(uint32_t)Type);
	}
}


/***************************************************************
  函数名：T16Nx_SetCNT0
  描  述：设置CNT0计数值
  输入值：T16Nx：可以是T16N0/1/2/3、 Value：16位数值
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_SetCNT0(T16N_TypeDef* T16Nx,uint16_t Value)
{
	T16Nx->CON0.ASYWEN = Enable;
	T16Nx->CNT0.CNT0 = Value;
	T16Nx->CON0.ASYWEN = Disable;
} 

/***************************************************************
  函数名：T16Nx_SetCNT1
  描  述：设置CNT1计数值
  输入值：T16Nx：可以是T16N0/1/2/3、 Value：16位数值
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_SetCNT1(T16N_TypeDef* T16Nx,uint16_t Value)
{
	T16Nx->CON0.ASYWEN = Enable;
	T16Nx->CNT1.CNT1 = Value;
	T16Nx->CON0.ASYWEN = Disable;
} 



/***************************************************************
  函数名：T16Nx_SetPREMAT
  描  述：设置预分频比
  输入值：T16Nx：可以是T16N0/1/2/3、 Value：1-256
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_SetPREMAT(T16N_TypeDef* T16Nx,uint8_t Value)
{
	T16Nx->PREMAT.PREMAT = Value-1;	 
}

/***************************************************************
  函数名：T16Nx_SetPRECNT
  描  述：设置预分频计数器
  输入值：T16Nx：可以是T16N0/1/2/3、 Value：1-256
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_SetPRECNT(T16N_TypeDef* T16Nx,uint8_t Value)
{
	T16Nx->CON0.ASYWEN = Enable;
	T16Nx->PRECNT.PRECNT = Value;	
	T16Nx->CON0.ASYWEN = Disable;
}


/***************************************************************
  函数名：T16Nx_SetMATx
  描  述：设置匹配寄存器
  输入值：T16Nx：可以是T16N0/1/2/3、 MATx：可以是TIM_MAT0/1/2/3 、 Value：16位数值
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_SetMAT0(T16N_TypeDef* T16Nx,uint16_t Value)
{

	T16Nx->MAT0.MAT0=Value;
}

void T16Nx_SetMAT1(T16N_TypeDef* T16Nx,uint16_t Value)
{

	T16Nx->MAT1.MAT1=Value;
}
void T16Nx_SetMAT2(T16N_TypeDef* T16Nx,uint16_t Value)
{

	T16Nx->MAT2.MAT2=Value;
}
void T16Nx_SetMAT3(T16N_TypeDef* T16Nx,uint16_t Value)
{

	T16Nx->MAT3.MAT3=Value;
}

/***************************************************************
  函数名：T16Nx_SetMATx
  描  述：设置匹配寄存器
  输入值：T16Nx：可以是T16N0/1/2/3、 MATx：可以是TOP0/1 、 Value：16位数值
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_SetTOP0(T16N_TypeDef* T16Nx,uint16_t Value)
{

	T16Nx->TOP0.TOP0=Value;
}

void T16Nx_SetTOP1(T16N_TypeDef* T16Nx,uint16_t Value)
{

	T16Nx->TOP1.TOP1=Value;
}


/***************************************************************
  函数名：T16Nx_GetMATx
  描  述：读取匹配寄存器值
  输入值：T16Nx：可以是T16N0/1/2/3 MATx：可以是TIM_MAT0/1/2/3
  输出值：无
  返回值：16位数值
 ***************************************************************/
uint16_t T16Nx_GetMAT0(T16N_TypeDef* T16Nx)
{
	return (T16Nx->MAT0.MAT0) ;

}

uint16_t T16Nx_GetMAT1(T16N_TypeDef* T16Nx)
{
	return (T16Nx->MAT1.MAT1) ;

}
uint16_t T16Nx_GetMAT2(T16N_TypeDef* T16Nx)
{
	return (T16Nx->MAT2.MAT2) ;

}
uint16_t T16Nx_GetMAT3(T16N_TypeDef* T16Nx)
{
	return (T16Nx->MAT3.MAT3) ;

}


/***************************************************************
  函数名：T16Nx_GetTOPx
  描  述：读取匹配寄存器值
  输入值：T16Nx：可以是T16N0/1/2/3 MATx：可以是TIM_TOP0/1
  输出值：无
  返回值：16位数值
 ***************************************************************/
uint16_t T16Nx_GetTOP0(T16N_TypeDef* T16Nx)
{
	return (T16Nx->TOP0.TOP0) ;

}

uint16_t T16Nx_GetTOP1(T16N_TypeDef* T16Nx)
{
	return (T16Nx->TOP1.TOP1) ;

}

/***************************************************************
  函数名：T16Nx_GetCNTx
  描  述：读取计数寄存器值
  输入值：T16Nx：可以是T16N0/1/2/3、
  返回值：16位数值
 ***************************************************************/
uint16_t T16Nx_GetCNT0(T16N_TypeDef* T16Nx)
{
	return (T16Nx->CNT0.CNT0) ;
}

uint16_t T16Nx_GetCNT1(T16N_TypeDef* T16Nx)
{
	return (T16Nx->CNT1.CNT1) ;
}



/***************************************************************
  函数名：T16Nx_GetFlagStatus
  描  述：读取指定标志位
  输入值：T16Nx：可以是T16N0/1/2/3、 T16Nx_Flag：中断标志位
  输出值：无
  返回值：RESET/SET
 ***************************************************************/
FlagStatus T16Nx_GetFlagStatus(T16N_TypeDef* T16Nx,T16Nx_TYPE_IT T16Nx_Flag)
{
	FlagStatus bitstatus = RESET;

	if((T16Nx->IF.Word  & (uint32_t)T16Nx_Flag) != (uint32_t)RESET)
	{
		bitstatus = SET;
	}
	else   
	{
		bitstatus = RESET;
	}
	return  bitstatus;
}


/***************************************************************
  函数名：T16Nx_ClearITPendingBit
  描  述：清除指定的中断标志位
  输入值：T16Nx：可以是T16N0/1/2/3、 T16Nx_Flag：中断标志位
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_ClearITPendingBit(T16N_TypeDef* T16Nx,T16Nx_TYPE_IT T16Nx_Flag)
{
	T16Nx->IF.Word = (uint32_t)T16Nx_Flag;
}  


/***************************************************************
  函数名：T16Nx_Enable
  描  述：使能T16Nx
  输入值：T16Nx：可以是T16N0/1/2/3
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_Enable(T16N_TypeDef* T16Nx)
{
	T16Nx->CON0.EN = 1;
}  

/***************************************************************
  函数名：T16Nx_Enable
  描  述：禁止T16Nx
  输入值：T16Nx：可以是T16N0/1/2/3
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_Disable(T16N_TypeDef* T16Nx)
{
	T16Nx->CON0.EN = 0;
}  


/***************************************************************
  函数名：T16Nx_PWMBK_Config
  描  述：配置PWM刹车功能
  输入值：T16Nx：可以是T16N0/1/2/3 ,Type 配置PWM刹车结构体
  输出值：无
  返回值：无
 ***************************************************************/
void T16Nx_PWMBK_Config(T16N_TypeDef* T16Nx,T16Nx_PWMBK_Type* type)
{

	T16Nx->CON2.PWMBKL0 = type->T16Nx_PWMBK_L0;    //PWM通道0刹车输出电平选择
	T16Nx->CON2.PWMBKL1 = type->T16Nx_PWMBK_L1;    //PWM通道1刹车输出电平选择
	T16Nx->CON2.PWMBKP0 = type->T16Nx_PWMBK_P0;    //PWM通道0刹车信号极性选择
	T16Nx->CON2.PWMBKP1 = type->T16Nx_PWMBK_P1;    //PWM通道1刹车信号极性选择
	T16Nx->CON2.PWMBKE0 = type->T16Nx_PWMBK_EN0;   //PWM通道0刹车使能
	T16Nx->CON2.PWMBKE1 = type->T16Nx_PWMBK_EN1;   //PWM通道1刹车使能
}  
/***************************************************************
  函数名：T16Nx_GetPWMBKF
  描  述：获取PWMBKF刹车标志位
  输入值：T16Nx：可以是T16N0/1/2/3 
  输出值：PWMBKF标志位的值。SET：发生刹车事件，RESET：未发生刹车事件
  返回值：无
 ***************************************************************/

FlagStatus T16Nx_GetPWMBKF(T16N_TypeDef* T16Nx)
{
	if(T16Nx->CON2.PWMBKF == 1)
		return SET;
	else
		return RESET;
}
/***************************************************************
  函数名：T16Nx_ResetPWMBKF
  描  述: 清除PWMBKF标志，标志清除后PWM端口恢复正常shuc
  输入值：T16Nx：可以是T16N0/1/2/3 
  输出值：无
  返回值：无
 ***************************************************************/

void T16Nx_ResetPWMBKF(T16N_TypeDef* T16Nx)
{
	T16Nx->CON2.PWMBKF = 1;
}

/***************************************************************
  函数名：T16Nx_PTR_Config
  描  述: 配置PWM调试模式ADC触发使能
  输入值：T16Nx：可以是T16N0/1/2/3 
  输出值：无
  返回值：无
 ***************************************************************/

void T16Nx_PTR_Config(T16N_TypeDef* T16Nx,T16Nx_PWMTRE_type Type,TYPE_FUNCEN NewState )
{
	if (NewState != DISABLE)
	{
		T16Nx->PTR.Word |= (uint32_t)Type;
	}
	else
	{
		T16Nx->PTR.Word &= (~(uint32_t)Type);
	}
}



/***************************************************************
  函数名：T32Nx_BaseInit
  描  述：T32Nx基本初始化,初始化时钟选择，外部时钟同步，外部时钟计数边缘选择，工作模式。
  输入值：T32Nx：可以是T32N0 、T32Nx_BaseInitStruct：初始化配置结构体地址
  输出值：无
  返回值：无
 ***************************************************************/
void T32Nx_BaseInit(T32N_TypeDef* T32Nx,T32Nx_BaseInitStruType* T32Nx_BaseInitStruct)
{

	T32Nx->CON0.CS         = T32Nx_BaseInitStruct->T32Nx_ClkS;     //计数时钟源选择
	T32Nx->CON0.SYNC       = T32Nx_BaseInitStruct->T32Nx_SYNC;     //外部时钟同步使能  Disable:不同步外部时钟，为异步计数模式。Enable：通过PCLK对外部时钟同步，为同步计数模式，外部时钟的高低电平平均至少保持2个时钟周期
	T32Nx->CON0.EDGE       = T32Nx_BaseInitStruct->T32Nx_EDGE;     //外部时钟计数边沿选择
	T32Nx->CON0.MOD        = T32Nx_BaseInitStruct->T32Nx_Mode;     //工作模式选择
	T32Nx->PREMAT.PREMAT   = T32Nx_BaseInitStruct->T32Nx_PREMAT-1; //预分频比设置，范围：1-256
}



/***************************************************************
  函数名：T32Nx_CapInit
  描  述：T32Nx捕捉初始化
  输入值：T32Nx：可以是T32N0、 T32Nx_CapInitStruct：初始化配置结构体地址
  输出值：无
  返回值：无
 ***************************************************************/
void T32Nx_CapInit(T32N_TypeDef* T32Nx,T32Nx_CapInitStruType* T32Nx_CapInitStruct)
{

	T32Nx->CON1.CAPL0=T32Nx_CapInitStruct->T32Nx_CAPCAPL0;   //配置捕捉0重载计数器是否使能
	T32Nx->CON1.CAPL1=T32Nx_CapInitStruct->T32Nx_CAPCAPL1;   //配置捕捉1重载计数器是否使能 
	T32Nx->CON1.CAPPE=T32Nx_CapInitStruct->T32Nx_CapRise;    //配置上升沿输入捕捉是否使能
	T32Nx->CON1.CAPNE=T32Nx_CapInitStruct->T32Nx_CapFall;    //配置下降沿输入捕捉是否使能
	T32Nx->CON1.CAPIS0=T32Nx_CapInitStruct->T32Nx_CapIS0;    //配置捕捉输入端口T32NxIN0是否使能
	T32Nx->CON1.CAPIS1=T32Nx_CapInitStruct->T32Nx_CapIS1;    //配置捕捉输入端口T32NxIN1是否使能
	T32Nx->CON1.CAPT=T32Nx_CapInitStruct->T32Nx_CapTime;     //配置捕捉次数
}

/***************************************************************
  函数名：T32Nx_PMWOutInit
  描  述：T32NxPWM输出初始化
  输入值：：T32Nx：可以是T32N0、 T32Nx_PWMInitStruct：初始化配置结构体地址
  输出值：无
  返回值：无
 ***************************************************************/
void T32Nx_PMWOutInit(T32N_TypeDef* T32Nx,T32Nx_PWMInitStruType* T32Nx_PWMInitStruct)
{
	T32Nx->CON1.MOE0 = T32Nx_PWMInitStruct->T32Nx_MOE0;   				//配置输出端口0是否使能
	T32Nx->CON1.MOE1 = T32Nx_PWMInitStruct->T32Nx_MOE1;           //配置输出端口1是否使能

}


/***************************************************************
  函数名：T32Nx_MATxITConfig
  描  述：T32Nx匹配后的工作模式配置
  输入值：T32Nx：可以是T32N0、 Type：匹配后的工作模式
  输出值：无
  返回值：无
 ***************************************************************/
void T32Nx_MAT0ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATCON Type)
{  
	T32Nx->CON0.MAT0S=Type;
}
void T32Nx_MAT1ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATCON Type)
{
	T32Nx->CON0.MAT1S=Type;
}
void T32Nx_MAT2ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATCON Type)
{
	T32Nx->CON0.MAT2S=Type;
}
void T32Nx_MAT3ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATCON Type)
{
	T32Nx->CON0.MAT3S=Type;
}


/***************************************************************
  函数名：T32Nx_MATxOutxConfig
  描  述：T32Nx匹配后的输出端口的模式配置
  输入值：T32Nx：可以是T32N0/1/2/3 、 Type：匹配后端口的工作模式
  输出值：无
  返回值：无
 ***************************************************************/
void T32Nx_MAT0Out0Config(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATOUT Type)
{
	T32Nx->CON1.MOM0=Type;
}
void T32Nx_MAT1Out0Config(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATOUT Type)
{
	T32Nx->CON1.MOM1=Type;
}
void T32Nx_MAT2Out1Config(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATOUT Type)
{
	T32Nx->CON1.MOM2=Type;
}
void T32Nx_MAT3Out1Config(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATOUT Type)
{
	T32Nx->CON1.MOM3=Type;
}


/***************************************************************
  函数名：T32Nx_ITConfig
  描  述：T32N中断配置
  输入值：T32Nx：可以是T32N0、 Type：中断类型 、 NewState：使能/失能
  输出值：无
  返回值：无
 ***************************************************************/
void T32Nx_ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_IT Type,TYPE_FUNCEN NewState)
{
	if (NewState != DISABLE)
	{
		T32Nx->IE.Word |= (uint32_t)Type;
	}
	else
	{
		T32Nx->IE.Word &= (~(uint32_t)Type);
	}
}


/***************************************************************
  函数名：T32Nx_SetCNT0
  描  述：设置CNT0计数值
  输入值：T16Nx：可以是T32N0/1/2/3、 Value：32位数值
  输出值：无
  返回值：无
 ***************************************************************/
void T32Nx_SetCNT(T32N_TypeDef* T32Nx,uint32_t Value)
{
	T32Nx->CON0.ASYNC_WREN = Enable;  
	T32Nx->CNT.CNT = Value;
	T32Nx->CON0.ASYNC_WREN = Disable;  
} 


/***************************************************************
  函数名：T32Nx_SetPREMAT
  描  述：设置预分频比
  输入值：T32Nx：可以是T32N0、 Value：1-256
  输出值：无
  返回值：无
 ***************************************************************/
void T32Nx_SetPREMAT(T32N_TypeDef* T32Nx,uint8_t Value)
{
	T32Nx->PREMAT.PREMAT = Value-1;	 
}

/***************************************************************
  函数名：T32Nx_SetPRECNT
  描  述：设置预分频计数器
  输入值：T32Nx：可以是T32N0、 Value：1-256
  输出值：无
  返回值：无
 ***************************************************************/
void T32Nx_SetPRECNT(T32N_TypeDef* T32Nx,uint8_t Value)
{
	T32Nx->CON0.ASYNC_WREN = Enable; 
	T32Nx->PRECNT.PRECNT = Value;	 
	T32Nx->CON0.ASYNC_WREN = Disable; 
}

/***************************************************************
  函数名：T32Nx_SetMATx
  描  述：设置匹配寄存器
  输入值：T16Nx：可以是T32N0、 MATx：可以是TIM_MAT0/1/2/3 、 Value：32位数值
  输出值：无
  返回值：无
 ***************************************************************/
void T32Nx_SetMAT0(T32N_TypeDef* T32Nx,uint32_t Value)
{

	T32Nx->MAT0.MAT0=Value;
}

void T32Nx_SetMAT1(T32N_TypeDef* T32Nx,uint32_t Value)
{

	T32Nx->MAT1.MAT1=Value;
}
void T32Nx_SetMAT2(T32N_TypeDef* T32Nx,uint32_t Value)
{

	T32Nx->MAT2.MAT2=Value;
}
void T32Nx_SetMAT3(T32N_TypeDef* T32Nx,uint32_t Value)
{

	T32Nx->MAT3.MAT3=Value;
}

/***************************************************************
  函数名：T32Nx_GetMATx
  描  述：读取匹配寄存器值
  输入值：T32Nx：可以是T32N0 MATx：可以是T32Nx_MAT0/1/2/3
  输出值：无
  返回值：32位数值
 ***************************************************************/
uint32_t T32Nx_GetMAT0(T32N_TypeDef* T32Nx)
{
	return (T32Nx->MAT0.MAT0) ;

}

uint32_t T32Nx_GetMAT1(T32N_TypeDef* T32Nx)
{
	return (T32Nx->MAT1.MAT1) ;

}
uint32_t T32Nx_GetMAT2(T32N_TypeDef* T32Nx)
{
	return (T32Nx->MAT2.MAT2) ;

}
uint32_t T32Nx_GetMAT3(T32N_TypeDef* T32Nx)
{
	return (T32Nx->MAT3.MAT3) ;

}




/***************************************************************
  函数名：T32Nx_GetCNT
  描  述：读取计数寄存器值
  输入值：T32Nx：可以是T32N0/1/2/3、
  返回值：32位数值
 ***************************************************************/
uint32_t T32Nx_GetCNT(T32N_TypeDef* T32Nx)
{
	return (T32Nx->CNT.CNT) ;
}

/***************************************************************
  函数名：T32Nx_GetFlagStatus
  描  述：读取指定标志位
  输入值：T32Nx：可以是T32N0、 T32Nx_Flag：中断标志位
  输出值：无
  返回值：RESET/SET
 ***************************************************************/
FlagStatus T32Nx_GetFlagStatus(T32N_TypeDef* T32Nx,T32Nx_TYPE_IT T32Nx_Flag)
{
	FlagStatus bitstatus = RESET;

	if((T32Nx->IF.Word  & (uint32_t)T32Nx_Flag) != (uint32_t)RESET)
	{
		bitstatus = SET;
	}
	else   
	{
		bitstatus = RESET;
	}
	return  bitstatus;
}


/***************************************************************
  函数名：T32Nx_ClearITPendingBit
  描  述：清除指定的中断标志位
  输入值：T32Nx：可以是T16N0、 TIM_Flag：中断标志位
  输出值：无
  返回值：无
 ***************************************************************/
void T32Nx_ClearITPendingBit(T32N_TypeDef* T32Nx,T32Nx_TYPE_IT TIM_Flag)
{
	T32Nx->IF.Word = (uint32_t)TIM_Flag;
}  


/***************************************************************
  函数名：T32Nx_Enable
  描  述：使能T32Nx
  输入值：T32Nx：可以是T32N0
  输出值：无
  返回值：无
 ***************************************************************/
void T32Nx_Enable(T32N_TypeDef* T32Nx)
{
	T32Nx->CON0.EN = 1;
}  

/***************************************************************
  函数名：T32Nx_Enable
  描  述：禁止T32Nx
  输入值：T32Nx：可以是T32N0
  输出值：无
  返回值：无
 ***************************************************************/
void T32Nx_Disable(T32N_TypeDef* T32Nx)
{
	T32Nx->CON0.EN = 0;
}  

/***************************************************************
  函数名：BUZC_Frequence
  描  述：BUZ使能后，配置BUZ信号频率
  输入值：Frequence : BUZ信号频率，sys_buz ：是否使能
  输出值：无
  返回值：无
 ***************************************************************/
void BUZC_Frequence(uint32_t Frequence,TYPE_FUNCEN sys_buz)
{
	 float tmp;
	
	 if(sys_buz == Disable)  //如果BUZC配置为禁止，则直接禁止BUZC，并返回
	 {
		 GPIO->BUZC.BUZEN = 0;
		 return;
	 }
	 
	 tmp = SystemCoreClock /(Frequence * 2) - 1;
	 GPIO->BUZC.BUZ_LOAD = (uint32_t)tmp;
   GPIO->BUZC.BUZEN = 1;	 
}	


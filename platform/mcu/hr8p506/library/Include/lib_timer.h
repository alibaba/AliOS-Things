/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_timer.h
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  Timer模块库函数头文件
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#ifndef __LIB_TIMER_H__
#define __LIB_TIMER_H__

#include <stdint.h>
#include "HR8P506.h"
#include "type.h"
#include "system_HR8P506.h"


/* 时钟源选择 */
typedef enum
{

	T16Nx_ClkS_PCLK = 0x0 , //时钟源选择:内部PCLK
	T16Nx_ClkS_CK0 = 0x1 ,  //时钟源选择:外部CK0时钟输入
	T16Nx_ClkS_CK1 = 0x2 ,  //时钟源选择:外部CK1时钟输入
}T16Nx_TYPE_CLKS;

/* 外部时钟计数边沿选择 */
typedef enum
{
	T16Nx_EDGE_Rise = 0x0 , //外部时钟计数边沿选择:上升沿
	T16Nx_EDGE_Fall = 0x1 , //外部时钟计数边沿选择:下降沿
	T16Nx_EDGE_All = 0x2 ,  //外部时钟计数边沿选择:所有
}T16Nx_TYPE_EDGE;


/* 工作模式选择 */
typedef enum
{
	T16Nx_Mode_TC0 = 0x0 , //工作模式:定时、计数模式
	T16Nx_Mode_TC1 = 0x1 , //工作模式:定时、计数模式
	T16Nx_Mode_CAP = 0x2 , //工作模式:捕捉模式
	T16Nx_Mode_PWM = 0x3 , //工作模式:调制模式
}T16Nx_TYPE_MODE;

/* T16Nx初始化配置结构体定义 */
typedef struct
{
	T16Nx_TYPE_CLKS   T16Nx_ClkS; //时钟源选择

	TYPE_FUNCEN       T16Nx_SYNC;   //外部时钟同步

	T16Nx_TYPE_EDGE   T16Nx_EDGE; //外部时钟计数边沿选择

	T16Nx_TYPE_MODE   T16Nx_Mode; //工作模式选择

	unsigned int      T16Nx_PREMAT;  //设置预分频比，范围 ：1-256


}T16Nx_BaseInitStruType;

/* 捕捉次数控制 */
typedef enum
{
	T16Nx_CapTime_1 = 0x0 , //捕捉次数控制:1
	T16Nx_CapTime_2 = 0x1 , //捕捉次数控制:2
	T16Nx_CapTime_3 = 0x2 , //捕捉次数控制:3
	T16Nx_CapTime_4 = 0x3 , //捕捉次数控制:4
	T16Nx_CapTime_5 = 0x4 , //捕捉次数控制:5
	T16Nx_CapTime_6 = 0x5 , //捕捉次数控制:6
	T16Nx_CapTime_7 = 0x6 , //捕捉次数控制:7
	T16Nx_CapTime_8 = 0x7 , //捕捉次数控制:8
	T16Nx_CapTime_9 = 0x8 , //捕捉次数控制:9
	T16Nx_CapTime_10 = 0x9 , //捕捉次数控制:10
	T16Nx_CapTime_11 = 0xA , //捕捉次数控制:11
	T16Nx_CapTime_12 = 0xB , //捕捉次数控制:12
	T16Nx_CapTime_13 = 0xC , //捕捉次数控制:13
	T16Nx_CapTime_14 = 0xD , //捕捉次数控制:14
	T16Nx_CapTime_15 = 0xE , //捕捉次数控制:15
	T16Nx_CapTime_16 = 0xF , //捕捉次数控制:16
} T16Nx_TYPE_CAPT;

/* 捕捉功能初始化结构体定义 */
typedef struct
{
	TYPE_FUNCEN  T16Nx_CAPCAPL1;   //捕捉1重载计数器使能

	TYPE_FUNCEN  T16Nx_CAPCAPL0;   //捕捉0重载计数器使能

	TYPE_FUNCEN  T16Nx_CapRise;    //上升沿捕捉使能

	TYPE_FUNCEN  T16Nx_CapFall;    //下降沿捕捉使能

	TYPE_FUNCEN  T16Nx_CapIS0;     //输入端口0使能

	TYPE_FUNCEN  T16Nx_CapIS1;     //输入端口1使能

	T16Nx_TYPE_CAPT  T16Nx_CapTime;  //捕捉次数控制

}T16Nx_CapInitStruType;


/* 匹配寄存器值匹配后的工作模式 */
typedef enum
{
	T16Nx_Go_No = 0x0 ,    //匹配寄存器值匹配后的工作模式:继续计数，不产生中断
	T16Nx_Hold_Int = 0x1 , //匹配寄存器值匹配后的工作模式:保持计数，产生中断
	T16Nx_Clr_Int = 0x2 ,  //匹配寄存器值匹配后的工作模式:清零并重新计数，产生中断
	T16Nx_Go_Int = 0x3 ,   //匹配寄存器值匹配后的工作模式:继续计数，产生中断
}T16Nx_TYPE_MATCON;



/* 中断配置 */
typedef enum
{
	T16Nx_IT_MAT0 = 0x01 ,   //匹配0中断
	T16Nx_IT_MAT1 = 0x02 ,   //匹配1中断
	T16Nx_IT_MAT2 = 0x04 ,   //匹配2中断
	T16Nx_IT_MAT3 = 0x08 ,   //匹配3中断
	T16Nx_IT_TOP0 = 0x10 ,   //T16N_CNT0匹配计数器峰值中断
	T16Nx_IT_TOP1 = 0x20 ,	 //T16N_CNT1匹配计数器峰值中断
	T16Nx_IT_CAP0 = 0x40 ,	 //输入端口T16NxIN0输入捕捉中断
	T16Nx_IT_CAP1 = 0x80 ,	 //输入端口T16NxIN1输入捕捉中断
	T16Nx_IT_PBK0 = 0x100 ,  //PWM通道0刹车中断
	T16Nx_IT_PBK1 = 0x200 ,  //PWM通道1刹车中断
}T16Nx_TYPE_IT;


/* PWM输出极性类型 */
typedef enum
{
	POSITIVE = 0X00,   //正极性
	NEGATIVE = 0X01,   //负极性
}T16Nx_PWMOUT_POLAR_Type;  

/* PWM工作模式选择 */
typedef enum
{
	T16Nx_PWMMode_INDEP= 0x0 ,   //独立模式
	T16Nx_PWMMode_SYNC = 0x2 ,   //同步模式
	T16Nx_PWMMode_COMPLE = 0x3 , //互补模式
}T16Nx_PWMTYPE_MODE;


/* 调制功能初始化结构体定义 */
typedef struct
{
	TYPE_FUNCEN  T16Nx_MOE0;   //输出端口0使能

	TYPE_FUNCEN  T16Nx_MOE1;   //输出端口1使能

	T16Nx_PWMOUT_POLAR_Type  T16Nx_POL0;    //T16NxOUT0输出极性选择位

	T16Nx_PWMOUT_POLAR_Type  T16Nx_POL1;    //T16NxOUT1输出极性选择位 

	T16Nx_PWMTYPE_MODE T16Nx_PWMMODE; //T16PWM输出模式选择

	TYPE_FUNCEN PWMDZE; //PWM互补模式死区使能

}T16Nx_PWMInitStruType;


/* 匹配寄存器值匹配后输出端口的工作模式 */
typedef enum
{
	T16Nx_Out_Hold = 0x0 ,   //匹配寄存器值匹配后输出端口的工作模式：保持
	T16Nx_Out_Low = 0x1 ,    //匹配寄存器值匹配后输出端口的工作模式：清0
	T16Nx_Out_High = 0x2 ,   //匹配寄存器值匹配后输出端口的工作模式：置1
	T16Nx_Out_Switch = 0x3 , //匹配寄存器值匹配后输出端口的工作模式：取反
}T16Nx_TYPE_MATOUT;

/* PWM刹车输出电平 */
typedef enum
{
	PWMBKOUT_Low = 0,
	PWMBKOUT_High = 1,
}T16Nx_PWMBKOUT_LEVEl; 


/* PWM刹车信号极性选择 */
typedef enum
{
	PWMBKP_High = 0,
	PWMBKP_Low = 1,		
}T16Nx_PWMBKP_LEVEl;



/*PWM输出刹车控制*/
typedef struct
{
	TYPE_FUNCEN T16Nx_PWMBK_EN0;                 //PWM通段0刹车使能
	TYPE_FUNCEN T16Nx_PWMBK_EN1;                 //PWM通段0刹车使能
	T16Nx_PWMBKOUT_LEVEl T16Nx_PWMBK_L0;         //PWM通道0刹车输出电平选择
	T16Nx_PWMBKOUT_LEVEl T16Nx_PWMBK_L1;				//PWM通道1刹车输出电平选择
	T16Nx_PWMBKP_LEVEl T16Nx_PWMBK_P0;            //PWM通道0刹车信号极性选择
	T16Nx_PWMBKP_LEVEl T16Nx_PWMBK_P1;            //PWM通道1刹车信号极性选择
}T16Nx_PWMBK_Type;



/*PWM调制模式ADC触发使能控制*/

typedef enum
{
	T16Nx_P0MAT0 = 0x02,         //PWM通道0匹配0触发使能
	T16Nx_P0MAT1 = 0x04,         //PWM通道0匹配1触发使能
	T16Nx_P0TOP0 = 0x08,         //PWM通道0峰值0触发使能
	T16Nx_P1MAT2 = 0x20,         //PWM通道1匹配2触发使能
	T16Nx_P1MAT3 = 0x40,         //PWM通道1匹配3触发使能
	T16Nx_P1TOP1 = 0x80,         //PWM通道1峰值1触发使能
}T16Nx_PWMTRE_type;




/****************************************32为时钟数据结构定义*********************************/


/* 时钟源选择 */
typedef enum
{

	T32Nx_ClkS_PCLK = 0x0 , //时钟源选择:内部PCLK
	T32Nx_ClkS_CK0 = 0x1 ,  //时钟源选择:外部CK0时钟输入
	T32Nx_ClkS_CK1 = 0x2 ,  //时钟源选择:外部CK1时钟输入
}T32Nx_TYPE_CLKS;

/* 外部时钟计数边沿选择 */
typedef enum
{
	T32Nx_EDGE_Rise = 0x0 , //外部时钟计数边沿选择:上升沿
	T32Nx_EDGE_Fall = 0x1 , //外部时钟计数边沿选择:下降沿
	T32Nx_EDGE_All = 0x2 ,  //外部时钟计数边沿选择:所有
}T32Nx_TYPE_EDGE;


/* 工作模式选择 */
typedef enum
{
	T32Nx_Mode_TC0 = 0x0 , //工作模式:定时、计数模式
	T32Nx_Mode_TC1 = 0x1 , //工作模式:定时、计数模式
	T32Nx_Mode_CAP = 0x2 , //工作模式:捕捉模式
	T32Nx_Mode_PWM = 0x3 , //工作模式:调制模式
}T32Nx_TYPE_MODE;

/* T32Nx初始化配置结构体定义 */
typedef struct
{
	T32Nx_TYPE_CLKS   T32Nx_ClkS; //时钟源选择

	TYPE_FUNCEN       T32Nx_SYNC;   //外部时钟同步

	T32Nx_TYPE_EDGE   T32Nx_EDGE; //外部时钟计数边沿选择

	T32Nx_TYPE_MODE   T32Nx_Mode; //工作模式选择

	unsigned int      T32Nx_PREMAT;  //设置预分频比，范围 ：1-256


}T32Nx_BaseInitStruType;



/* 捕捉次数控制 */
typedef enum
{
	T32Nx_CapTime_1 = 0x0 , //捕捉次数控制:1
	T32Nx_CapTime_2 = 0x1 , //捕捉次数控制:2
	T32Nx_CapTime_3 = 0x2 , //捕捉次数控制:3
	T32Nx_CapTime_4 = 0x3 , //捕捉次数控制:4
	T32Nx_CapTime_5 = 0x4 , //捕捉次数控制:5
	T32Nx_CapTime_6 = 0x5 , //捕捉次数控制:6
	T32Nx_CapTime_7 = 0x6 , //捕捉次数控制:7
	T32Nx_CapTime_8 = 0x7 , //捕捉次数控制:8
	T32Nx_CapTime_9 = 0x8 , //捕捉次数控制:9
	T32Nx_CapTime_10 = 0x9 , //捕捉次数控制:10
	T32Nx_CapTime_11 = 0xA , //捕捉次数控制:11
	T32Nx_CapTime_12 = 0xB , //捕捉次数控制:12
	T32Nx_CapTime_13 = 0xC , //捕捉次数控制:13
	T32Nx_CapTime_14 = 0xD , //捕捉次数控制:14
	T32Nx_CapTime_15 = 0xE , //捕捉次数控制:15
	T32Nx_CapTime_16 = 0xF , //捕捉次数控制:16
} T32Nx_TYPE_CAPT;

/* 捕捉功能初始化结构体定义 */
typedef struct
{
	TYPE_FUNCEN  T32Nx_CAPCAPL1;   //捕捉1重载计数器使能

	TYPE_FUNCEN  T32Nx_CAPCAPL0;   //捕捉0重载计数器使能

	TYPE_FUNCEN  T32Nx_CapRise;    //上升沿捕捉使能

	TYPE_FUNCEN  T32Nx_CapFall;    //下降沿捕捉使能

	TYPE_FUNCEN  T32Nx_CapIS0;     //输入端口0使能

	TYPE_FUNCEN  T32Nx_CapIS1;     //输入端口1使能

	T32Nx_TYPE_CAPT  T32Nx_CapTime;  //捕捉次数控制

}T32Nx_CapInitStruType;

/* 调制功能初始化结构体定义 */
typedef struct
{
	TYPE_FUNCEN  T32Nx_MOE0;   //输出端口0使能

	TYPE_FUNCEN  T32Nx_MOE1;   //输出端口1使能

}T32Nx_PWMInitStruType;

/* 匹配寄存器值匹配后的工作模式 */
typedef enum
{
	T32Nx_Go_No = 0x0 ,    //匹配寄存器值匹配后的工作模式:继续计数，不产生中断
	T32Nx_Hold_Int = 0x1 , //匹配寄存器值匹配后的工作模式:保持计数，产生中断
	T32Nx_Clr_Int = 0x2 ,  //匹配寄存器值匹配后的工作模式:清零并重新计数，产生中断
	T32Nx_Go_Int = 0x3 ,   //匹配寄存器值匹配后的工作模式:继续计数，产生中断
}T32Nx_TYPE_MATCON;

/* 匹配寄存器值匹配后输出端口的工作模式 */
typedef enum
{
	T32Nx_Out_Hold = 0x0 ,   //匹配寄存器值匹配后输出端口的工作模式：保持
	T32Nx_Out_Low = 0x1 ,    //匹配寄存器值匹配后输出端口的工作模式：清0
	T32Nx_Out_High = 0x2 ,   //匹配寄存器值匹配后输出端口的工作模式：置1
	T32Nx_Out_Switch = 0x3 , //匹配寄存器值匹配后输出端口的工作模式：取反
}T32Nx_TYPE_MATOUT;


/* 中断配置 */
typedef enum
{
	T32Nx_IT_MAT0 = 0x01 ,   //匹配0中断
	T32Nx_IT_MAT1 = 0x02 ,   //匹配1中断
	T32Nx_IT_MAT2 = 0x04 ,   //匹配2中断
	T32Nx_IT_MAT3 = 0x08 ,   //匹配3中断
	T32Nx_IT_IT   = 0x10 ,    //T32N匹配0xFFFFFFFF中断使能位
	T32Nx_IT_CAP0 = 0x20 ,	 //T32N_CNT0匹配计数器峰值中断
	T32Nx_IT_CAP1 = 0x40 ,	 //T32N_CNT0匹配计数器峰值中断
}T32Nx_TYPE_IT;


/*********************T16Nx模块函数声明 *********************/
void T16Nx_BaseInit(T16N_TypeDef* T16Nx,T16Nx_BaseInitStruType* T16Nx_BaseInitStruct);
void T16Nx_CapInit(T16N_TypeDef* T16Nx,T16Nx_CapInitStruType* T16Nx_CapInitStruct);
void T16Nx_PMWOutInit(T16N_TypeDef* T16Nx,T16Nx_PWMInitStruType* T16Nx_PWMInitStruct);
void T16Nx_PWMPDZ_Config(T16N_TypeDef* T16Nx,unsigned int PWM_PDZ_data);
void T16Nx_MAT0ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATCON Type);
void T16Nx_MAT1ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATCON Type);
void T16Nx_MAT2ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATCON Type);
void T16Nx_MAT3ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATCON Type);
void T16Nx_MAT0Out0Config(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATOUT Type);
void T16Nx_MAT1Out0Config(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATOUT Type);
void T16Nx_MAT2Out1Config(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATOUT Type);
void T16Nx_MAT3Out1Config(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATOUT Type);
void T16Nx_ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_IT Type,TYPE_FUNCEN NewState);
void T16Nx_SetCNT0(T16N_TypeDef* T16Nx,uint16_t Value);
void T16Nx_SetCNT1(T16N_TypeDef* T16Nx,uint16_t Value);
void T16Nx_SetPREMAT(T16N_TypeDef* T16Nx,uint8_t Value);
void T16Nx_SetPRECNT(T16N_TypeDef* T16Nx,uint8_t Value);
void T16Nx_SetMAT0(T16N_TypeDef* T16Nx,uint16_t Value);
void T16Nx_SetMAT1(T16N_TypeDef* T16Nx,uint16_t Value);
void T16Nx_SetMAT2(T16N_TypeDef* T16Nx,uint16_t Value);
void T16Nx_SetMAT3(T16N_TypeDef* T16Nx,uint16_t Value);
void T16Nx_SetTOP0(T16N_TypeDef* T16Nx,uint16_t Value);
void T16Nx_SetTOP1(T16N_TypeDef* T16Nx,uint16_t Value);
uint16_t T16Nx_GetMAT0(T16N_TypeDef* T16Nx);
uint16_t T16Nx_GetMAT1(T16N_TypeDef* T16Nx);
uint16_t T16Nx_GetMAT2(T16N_TypeDef* T16Nx);
uint16_t T16Nx_GetMAT3(T16N_TypeDef* T16Nx);
uint16_t T16Nx_GetTOP0(T16N_TypeDef* T16Nx);
uint16_t T16Nx_GetTOP1(T16N_TypeDef* T16Nx);
uint16_t T16Nx_GetCNT0(T16N_TypeDef* T16Nx);
uint16_t T16Nx_GetCNT1(T16N_TypeDef* T16Nx);
FlagStatus T16Nx_GetFlagStatus(T16N_TypeDef* T16Nx,T16Nx_TYPE_IT T16Nx_Flag);
void T16Nx_ClearITPendingBit(T16N_TypeDef* T16Nx,T16Nx_TYPE_IT TIM_Flag);
void T16Nx_Enable(T16N_TypeDef* T16Nx);
void T16Nx_Disable(T16N_TypeDef* T16Nx);
void T16Nx_PWMBK_Config(T16N_TypeDef* T16Nx,T16Nx_PWMBK_Type* type);
FlagStatus T16Nx_GetPWMBKF(T16N_TypeDef* T16Nx);
void T16Nx_ResetPWMBKF(T16N_TypeDef* T16Nx);
void T16Nx_PTR_Config(T16N_TypeDef* T16Nx,T16Nx_PWMTRE_type Type,TYPE_FUNCEN NewState );

/***************************32位定时器函数声明********************************/

void T32Nx_BaseInit(T32N_TypeDef* T32Nx,T32Nx_BaseInitStruType* T32Nx_BaseInitStruct);
void T32Nx_CapInit(T32N_TypeDef* T32Nx,T32Nx_CapInitStruType* T32Nx_CapInitStruct);
void T32Nx_PMWOutInit(T32N_TypeDef* T32Nx,T32Nx_PWMInitStruType* T32Nx_PWMInitStruct);
void T32Nx_MAT0ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATCON Type);
void T32Nx_MAT1ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATCON Type);
void T32Nx_MAT2ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATCON Type);
void T32Nx_MAT3ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATCON Type);
void T32Nx_MAT0Out0Config(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATOUT Type);
void T32Nx_MAT1Out0Config(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATOUT Type);
void T32Nx_MAT2Out1Config(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATOUT Type);
void T32Nx_MAT3Out1Config(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATOUT Type);
void T32Nx_ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_IT Type,TYPE_FUNCEN NewState);
void T32Nx_SetCNT(T32N_TypeDef* T32Nx,uint32_t Value);
void T32Nx_SetPREMAT(T32N_TypeDef* T32Nx,uint8_t Value);
void T32Nx_SetPRECNT(T32N_TypeDef* T32Nx,uint8_t Value);
void T32Nx_SetMAT0(T32N_TypeDef* T32Nx,uint32_t Value);
void T32Nx_SetMAT1(T32N_TypeDef* T32Nx,uint32_t Value);
void T32Nx_SetMAT2(T32N_TypeDef* T32Nx,uint32_t Value);
void T32Nx_SetMAT3(T32N_TypeDef* T32Nx,uint32_t Value);
uint32_t T32Nx_GetMAT0(T32N_TypeDef* T32Nx);
uint32_t T32Nx_GetMAT1(T32N_TypeDef* T32Nx);
uint32_t T32Nx_GetMAT2(T32N_TypeDef* T32Nx);
uint32_t T32Nx_GetMAT3(T32N_TypeDef* T32Nx);
uint32_t T32Nx_GetCNT(T32N_TypeDef* T32Nx);
FlagStatus T32Nx_GetFlagStatus(T32N_TypeDef* T32Nx,T32Nx_TYPE_IT T32Nx_Flag);
void T32Nx_ClearITPendingBit(T32N_TypeDef* T32Nx,T32Nx_TYPE_IT TIM_Flag);
void T32Nx_Enable(T32N_TypeDef* T32Nx);
void T32Nx_Disable(T32N_TypeDef* T32Nx);
void BUZC_Frequence(uint32_t Frequence,TYPE_FUNCEN sys_buz);

#endif

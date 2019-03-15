/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#ifndef __LIB_TIMER_H__
#define __LIB_TIMER_H__

#include "P131.h"
#include "type.h"

typedef enum
{

    T16Nx_ClkS_PCLK = 0x0,
    T16Nx_ClkS_CK0  = 0x1,
    T16Nx_ClkS_CK1  = 0x2,
} T16Nx_TYPE_CLKS;

typedef enum
{
    T16Nx_EDGE_Rise = 0x0,
    T16Nx_EDGE_Fall = 0x1,
    T16Nx_EDGE_All  = 0x2,
} T16Nx_TYPE_EDGE;

typedef enum
{
    T16Nx_Mode_TC0 = 0x0,
    T16Nx_Mode_TC1 = 0x1,
    T16Nx_Mode_CAP = 0x2,
    T16Nx_Mode_PWM = 0x3,
} T16Nx_TYPE_MODE;

typedef struct
{
    T16Nx_TYPE_CLKS   T16Nx_ClkS;

    TYPE_FUNCEN       T16Nx_SYNC;
 
    T16Nx_TYPE_EDGE   T16Nx_EDGE;
 
    T16Nx_TYPE_MODE   T16Nx_Mode;
    
    unsigned int      T16Nx_PREMAT;
        
 
} T16Nx_BaseInitStruType;

typedef enum
{
    T16Nx_CapTime_1  = 0x0,
    T16Nx_CapTime_2  = 0x1,
    T16Nx_CapTime_3  = 0x2,
    T16Nx_CapTime_4  = 0x3,
    T16Nx_CapTime_5  = 0x4,
    T16Nx_CapTime_6  = 0x5,
    T16Nx_CapTime_7  = 0x6,
    T16Nx_CapTime_8  = 0x7,
    T16Nx_CapTime_9  = 0x8,
    T16Nx_CapTime_10 = 0x9,
    T16Nx_CapTime_11 = 0xA,
    T16Nx_CapTime_12 = 0xB,
    T16Nx_CapTime_13 = 0xC,
    T16Nx_CapTime_14 = 0xD,
    T16Nx_CapTime_15 = 0xE,
    T16Nx_CapTime_16 = 0xF,
} T16Nx_TYPE_CAPT;

typedef struct
{
    TYPE_FUNCEN  T16Nx_CAPCAPL1;
    
    TYPE_FUNCEN  T16Nx_CAPCAPL0;
    
    TYPE_FUNCEN  T16Nx_CapRise;

    TYPE_FUNCEN  T16Nx_CapFall;
  
    TYPE_FUNCEN  T16Nx_CapIS0;
 
    TYPE_FUNCEN  T16Nx_CapIS1;

    T16Nx_TYPE_CAPT  T16Nx_CapTime;
 
} T16Nx_CapInitStruType;

typedef enum
{
    T16Nx_Go_No    = 0x0,
    T16Nx_Hold_Int = 0x1,
    T16Nx_Clr_Int  = 0x2,
    T16Nx_Go_Int   = 0x3,
} T16Nx_TYPE_MATCON;

typedef enum
{
    T16Nx_IT_MAT0 = 0x01,
    T16Nx_IT_MAT1 = 0x02,
    T16Nx_IT_MAT2 = 0x04,
    T16Nx_IT_MAT3 = 0x08,
    T16Nx_IT_TOP0 = 0x10,
    T16Nx_IT_TOP1 = 0x20,
    T16Nx_IT_CAP0 = 0x40,
    T16Nx_IT_CAP1 = 0x80,
    T16Nx_IT_PBK0 = 0x100,
    T16Nx_IT_PBK1 = 0x200,
} T16Nx_TYPE_IT;

typedef enum
{
    POSITIVE = 0X00,
    NEGATIVE = 0X01,
} T16Nx_PWMOUT_POLAR_Type;  

typedef enum
{
    T16Nx_PWMMode_INDEP  = 0x0,
    T16Nx_PWMMode_SYNC   = 0x2,
    T16Nx_PWMMode_COMPLE = 0x3,
} T16Nx_PWMTYPE_MODE;

typedef struct
{
    TYPE_FUNCEN  T16Nx_MOE0;
    
    TYPE_FUNCEN  T16Nx_MOE1;
    
    T16Nx_PWMOUT_POLAR_Type  T16Nx_POL0;

    T16Nx_PWMOUT_POLAR_Type  T16Nx_POL1;
    
    T16Nx_PWMTYPE_MODE T16Nx_PWMMODE;
    
    TYPE_FUNCEN PWMDZE;
 
} T16Nx_PWMInitStruType;

typedef enum
{
    T16Nx_Out_Hold   = 0x0,
    T16Nx_Out_Low    = 0x1,
    T16Nx_Out_High   = 0x2,
    T16Nx_Out_Switch = 0x3,
} T16Nx_TYPE_MATOUT;

typedef enum
{
    PWMBKOUT_Low  = 0,
    PWMBKOUT_High = 1,
} T16Nx_PWMBKOUT_LEVEl; 

typedef enum
{
    PWMBKP_High = 0,
    PWMBKP_Low  = 1,     
} T16Nx_PWMBKP_LEVEl;

typedef struct
{
    TYPE_FUNCEN T16Nx_PWMBK_EN0;
    TYPE_FUNCEN T16Nx_PWMBK_EN1;
    T16Nx_PWMBKOUT_LEVEl T16Nx_PWMBK_L0;
    T16Nx_PWMBKOUT_LEVEl T16Nx_PWMBK_L1;
    T16Nx_PWMBKP_LEVEl T16Nx_PWMBK_P0;
    T16Nx_PWMBKP_LEVEl T16Nx_PWMBK_P1;
} T16Nx_PWMBK_Type;

typedef enum
{
    T16Nx_P0MAT0 = 0x02,
    T16Nx_P0MAT1 = 0x04,
    T16Nx_P0TOP0 = 0x08,
    T16Nx_P1MAT2 = 0x20,
    T16Nx_P1MAT3 = 0x40,
    T16Nx_P1TOP1 = 0x80,
} T16Nx_PWMTRE_type;

typedef enum
{
    T32Nx_ClkS_PCLK = 0x0,
    T32Nx_ClkS_CK0  = 0x1,
    T32Nx_ClkS_CK1  = 0x2,
} T32Nx_TYPE_CLKS;

typedef enum
{
    T32Nx_EDGE_Rise = 0x0,
    T32Nx_EDGE_Fall = 0x1,
    T32Nx_EDGE_All  = 0x2,
} T32Nx_TYPE_EDGE;

typedef enum
{
    T32Nx_Mode_TC0 = 0x0,
    T32Nx_Mode_TC1 = 0x1,
    T32Nx_Mode_CAP = 0x2,
    T32Nx_Mode_PWM = 0x3,
} T32Nx_TYPE_MODE;

typedef struct
{
    T32Nx_TYPE_CLKS   T32Nx_ClkS;

    TYPE_FUNCEN       T32Nx_SYNC;
 
    T32Nx_TYPE_EDGE   T32Nx_EDGE;
 
    T32Nx_TYPE_MODE   T32Nx_Mode;
    
    unsigned int      T32Nx_PREMAT;
} T32Nx_BaseInitStruType;

typedef enum
{
    T32Nx_CapTime_1  = 0x0,
    T32Nx_CapTime_2  = 0x1,
    T32Nx_CapTime_3  = 0x2,
    T32Nx_CapTime_4  = 0x3,
    T32Nx_CapTime_5  = 0x4,
    T32Nx_CapTime_6  = 0x5,
    T32Nx_CapTime_7  = 0x6,
    T32Nx_CapTime_8  = 0x7,
    T32Nx_CapTime_9  = 0x8,
    T32Nx_CapTime_10 = 0x9,
    T32Nx_CapTime_11 = 0xA,
    T32Nx_CapTime_12 = 0xB,
    T32Nx_CapTime_13 = 0xC,
    T32Nx_CapTime_14 = 0xD,
    T32Nx_CapTime_15 = 0xE,
    T32Nx_CapTime_16 = 0xF,
} T32Nx_TYPE_CAPT;

typedef struct
{
    TYPE_FUNCEN  T32Nx_CAPCAPL1;

    TYPE_FUNCEN  T32Nx_CAPCAPL0;
    
    TYPE_FUNCEN  T32Nx_CapRise;

    TYPE_FUNCEN  T32Nx_CapFall;
  
    TYPE_FUNCEN  T32Nx_CapIS0;
 
    TYPE_FUNCEN  T32Nx_CapIS1;

    T32Nx_TYPE_CAPT  T32Nx_CapTime;
 
} T32Nx_CapInitStruType;

typedef struct
{
    TYPE_FUNCEN  T32Nx_MOE0;
    
    TYPE_FUNCEN  T32Nx_MOE1;
 
} T32Nx_PWMInitStruType;

typedef enum
{
    T32Nx_Go_No    = 0x0,
    T32Nx_Hold_Int = 0x1,
    T32Nx_Clr_Int  = 0x2,
    T32Nx_Go_Int   = 0x3,
} T32Nx_TYPE_MATCON;

typedef enum
{
    T32Nx_Out_Hold   = 0x0,
    T32Nx_Out_Low    = 0x1,
    T32Nx_Out_High   = 0x2,
    T32Nx_Out_Switch = 0x3,
} T32Nx_TYPE_MATOUT;

typedef enum
{
    T32Nx_IT_MAT0 = 0x01,
    T32Nx_IT_MAT1 = 0x02,
    T32Nx_IT_MAT2 = 0x04,
    T32Nx_IT_MAT3 = 0x08,
    T32Nx_IT_IT   = 0x10,
    T32Nx_IT_CAP0 = 0x20,
    T32Nx_IT_CAP1 = 0x40,
} T32Nx_TYPE_IT;

typedef enum
{
     RTC_LOSC    = 0x0,
     RTC_LRC     = 0x1,
     RTC_PCLK256 = 0x2, //PCLK/256
     RTC_PCLK    = 0x3, //PCLK
} RTC_TYPE_CLKS;

typedef enum
{
    RTC_HOUR12 = 0X0,
    RTC_HOUR24 = 0X1,
} RTC_TYPE_TIME;

typedef enum {
    RTC_Interrupt_Source_Second = 0,
    RTC_Interrupt_Source_Minute = 1,
    RTC_Interrupt_Source_Hour   = 2,
    RTC_Interrupt_Source_Day    = 3,
    RTC_Interrupt_Source_Month  = 4,
} RTC_Interrupt_Source;

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
uint16_t T32Nx_GetCNT(T32N_TypeDef* T32Nx);
FlagStatus T32Nx_GetFlagStatus(T32N_TypeDef* T32Nx,T32Nx_TYPE_IT T32Nx_Flag);
void T32Nx_ClearITPendingBit(T32N_TypeDef* T32Nx,T32Nx_TYPE_IT TIM_Flag);
void T32Nx_Enable(T32N_TypeDef* T32Nx);
void T32Nx_Disable(T32N_TypeDef* T32Nx);

#endif

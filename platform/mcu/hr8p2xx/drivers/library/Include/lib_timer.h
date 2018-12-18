/******************************************************************************************
 * Copyright (C), 2016, Shanghai Eastsoft Microelectronics Co., Ltd.
 *******************************************************************************************/
#ifndef __LIBTIMER_H__
#define __LIBTIMER_H__

#include "HR8P2xx.h"
#include "type.h"

typedef enum
{
    TIM_ClkS_PCLK = 0x0,
    TIM_ClkS_CK0  = 0x1,
    TIM_ClkS_CK1  = 0x2,
} TIM_TYPE_CLKS;

typedef enum
{
    TIM_EDGE_Rise = 0x0,
    TIM_EDGE_Fall = 0x1,
    TIM_EDGE_All  = 0x2,
} TIM_TYPE_EDGE;

typedef enum
{
    TIM_Mode_TC0 = 0x0,
    TIM_Mode_TC1 = 0x1,
    TIM_Mode_CAP = 0x2,
    TIM_Mode_PWM = 0x3,
} TIM_TYPE_MODE;

typedef struct
{
    TIM_TYPE_CLKS TIM_ClkS;

    TYPE_FUNCEN   TIM_SYNC;

    TIM_TYPE_EDGE TIM_EDGE;

    TIM_TYPE_MODE TIM_Mode;

} TIM_BaseInitStruType;

typedef enum
{
    TIM_Go_No    = 0x0,
    TIM_Hold_Int = 0x1,
    TIM_Clr_Int  = 0x2,
    TIM_Go_Int   = 0x3,
} TIM_TYPE_MATCON;

typedef enum
{
    TIM_Out_Hold   = 0x0,
    TIM_Out_Low    = 0x1,
    TIM_Out_High   = 0x2,
    TIM_Out_Switch = 0x3,
} TIM_TYPE_MATOUT;

typedef enum
{
    TIM_CapTime_1  = 0x0,
    TIM_CapTime_2  = 0x1,
    TIM_CapTime_3  = 0x2,
    TIM_CapTime_4  = 0x3,
    TIM_CapTime_5  = 0x4,
    TIM_CapTime_6  = 0x5,
    TIM_CapTime_7  = 0x6,
    TIM_CapTime_8  = 0x7,
    TIM_CapTime_9  = 0x8,
    TIM_CapTime_10 = 0x9,
    TIM_CapTime_11 = 0xA,
    TIM_CapTime_12 = 0xB,
    TIM_CapTime_13 = 0xC,
    TIM_CapTime_14 = 0xD,
    TIM_CapTime_15 = 0xE,
    TIM_CapTime_16 = 0xF,
} TIM_TYPE_CAPT;

typedef struct
{
    TYPE_FUNCEN   TIM_CapRise;

    TYPE_FUNCEN   TIM_CapFall;

    TYPE_FUNCEN   TIM_CapIS0;

    TYPE_FUNCEN   TIM_CapIS1;

    TIM_TYPE_CAPT TIM_CapTime;

} TIM_CapInitStruType;

typedef enum
{
    TIM_IT_MAT0 = 0x01,
    TIM_IT_MAT1 = 0x02,
    TIM_IT_MAT2 = 0x04,
    TIM_IT_MAT3 = 0x08,
    TIM_IT_N    = 0x10,
    TIM_IT_CAP0 = 0x20,
    TIM_IT_CAP1 = 0x40,
} TIM_TYPE_IT;

typedef enum
{
    TIM_MAT0 = 0x00,
    TIM_MAT1 = 0x01,
    TIM_MAT2 = 0x02,
    TIM_MAT3 = 0x03,
} TIM_TYPE_MATX;

typedef enum
{
    T16N0_OUT = 0x00,
    T16N1_OUT = 0x01,
    T16N2_OUT = 0x02,
    T16N3_OUT = 0x03,
} TIM_TYPE_PWMOC;

typedef enum
{
    NO_PWM    = 0x0,
    IR38K_PWM = 0x1,
    PWM_OUT0  = 0x2,
    PWM_OUT1  = 0x3,
} PWM_TYPE_OUT;

typedef enum
{
    LOW_LEVEL  = 0x0,
    HIGH_LEVEL = 0x1,
} TYPE_LEVEL;

#define T16N0_Enable() (T16N0->CON0.EN = 1)
#define T16N1_Enable() (T16N1->CON0.EN = 1)
#define T16N2_Enable() (T16N2->CON0.EN = 1)
#define T16N3_Enable() (T16N3->CON0.EN = 1)
#define T32N0_Enable() (T32N0->CON0.EN = 1)
#define T32N1_Enable() (T32N1->CON0.EN = 1)
#define T32N2_Enable() (T32N2->CON0.EN = 1)
#define T16N0_Disable() (T16N0->CON0.EN = 0)
#define T16N1_Disable() (T16N1->CON0.EN = 0)
#define T16N2_Disable() (T16N2->CON0.EN = 0)
#define T16N3_Disable() (T16N3->CON0.EN = 0)
#define T32N0_Disable() (T32N0->CON0.EN = 0)
#define T32N1_Disable() (T32N1->CON0.EN = 0)
#define T32N2_Disable() (T32N2->CON0.EN = 0)

#define T16N0_ASYNCWR_Enable() (T16N0->CON0.ASYWEN = 1)
#define T16N1_ASYNCWR_Enable() (T16N1->CON0.ASYWEN = 1)
#define T16N2_ASYNCWR_Enable() (T16N2->CON0.ASYWEN = 1)
#define T16N3_ASYNCWR_Enable() (T16N3->CON0.ASYWEN = 1)
#define T32N0_ASYNCWR_Enable() (T32N0->CON0.ASYNCWREN = 1)
#define T32N1_ASYNCWR_Enable() (T32N1->CON0.ASYNCWREN = 1)
#define T32N2_ASYNCWR_Enable() (T32N2->CON0.ASYNCWREN = 1)
#define T16N0_ASYNCWR_Disable() (T16N0->CON0.ASYWEN= 0)
#define T16N1_ASYNCWR_Disable() (T16N1->CON0.ASYWEN = 0)
#define T16N2_ASYNCWR_Disable() (T16N2->CON0.ASYWEN = 0)
#define T16N3_ASYNCWR_Disable() (T16N3->CON0.ASYWEN = 0)
#define T32N0_ASYNCWR_Disable() (T32N0->CON0.ASYNCWREN = 0)
#define T32N1_ASYNCWR_Disable() (T32N1->CON0.ASYNCWREN = 0)
#define T32N2_ASYNCWR_Disable() (T32N2->CON0.ASYNCWREN = 0)

#define T16N0_PwmOut0_Enable() (T16N0->CON1.MOE0 = 1)
#define T16N1_PwmOut0_Enable() (T16N1->CON1.MOE0 = 1)
#define T16N2_PwmOut0_Enable() (T16N2->CON1.MOE0 = 1)
#define T16N3_PwmOut0_Enable() (T16N3->CON1.MOE0 = 1)
#define T32N0_PwmOut0_Enable() (T32N0->CON1.MOE0 = 1)
#define T32N1_PwmOut0_Enable() (T32N1->CON1.MOE0 = 1)
#define T32N2_PwmOut0_Enable() (T32N2->CON1.MOE0 = 1)
#define T16N0_PwmOut1_Enable() (T16N0->CON1.MOE1 = 1)
#define T16N1_PwmOut1_Enable() (T16N1->CON1.MOE1 = 1)
#define T16N2_PwmOut1_Enable() (T16N2->CON1.MOE1 = 1)
#define T16N3_PwmOut1_Enable() (T16N3->CON1.MOE1 = 1)
#define T32N0_PwmOut1_Enable() (T32N0->CON1.MOE1 = 1)
#define T32N1_PwmOut1_Enable() (T32N1->CON1.MOE1 = 1)
#define T32N2_PwmOut1_Enable() (T32N2->CON1.MOE1 = 1)
#define T16N0_PwmOut0_Disable() (T16N0->CON1.MOE0 = 0)
#define T16N1_PwmOut0_Disable() (T16N1->CON1.MOE0 = 0)
#define T16N2_PwmOut0_Disable() (T16N2->CON1.MOE0 = 0)
#define T16N3_PwmOut0_Disable() (T16N3->CON1.MOE0 = 0)
#define T32N0_PwmOut0_Disable() (T32N0->CON1.MOE0 = 0)
#define T32N1_PwmOut0_Disable() (T32N1->CON1.MOE0 = 0)
#define T32N2_PwmOut0_Disable() (T32N2->CON1.MOE0 = 0)
#define T16N0_PwmOut1_Disable() (T16N0->CON1.MOE1 = 0)
#define T16N1_PwmOut1_Disable() (T16N1->CON1.MOE1 = 0)
#define T16N2_PwmOut1_Disable() (T16N2->CON1.MOE1 = 0)
#define T16N3_PwmOut1_Disable() (T16N3->CON1.MOE1 = 0)
#define T32N0_PwmOut1_Disable() (T32N0->CON1.MOE1 = 0)
#define T32N1_PwmOut1_Disable() (T32N1->CON1.MOE1 = 0)
#define T32N2_PwmOut1_Disable() (T32N2->CON1.MOE1 = 0)

void T16Nx_BaseInit(T16N_TypeDef* T16Nx,TIM_BaseInitStruType* TIM_BaseInitStruct);
void T16Nx_CapInit(T16N_TypeDef* T16Nx,TIM_CapInitStruType* TIM_CapInitStruct);
void T16Nx_MAT0ITConfig(T16N_TypeDef* T16Nx,TIM_TYPE_MATCON Type);
void T16Nx_MAT1ITConfig(T16N_TypeDef* T16Nx,TIM_TYPE_MATCON Type);
void T16Nx_MAT2ITConfig(T16N_TypeDef* T16Nx,TIM_TYPE_MATCON Type);
void T16Nx_MAT3ITConfig(T16N_TypeDef* T16Nx,TIM_TYPE_MATCON Type);
void T16Nx_MAT0Out0Config(T16N_TypeDef* T16Nx,TIM_TYPE_MATOUT Type);
void T16Nx_MAT1Out0Config(T16N_TypeDef* T16Nx,TIM_TYPE_MATOUT Type);
void T16Nx_MAT2Out1Config(T16N_TypeDef* T16Nx,TIM_TYPE_MATOUT Type);
void T16Nx_MAT3Out1Config(T16N_TypeDef* T16Nx,TIM_TYPE_MATOUT Type);
void T16Nx_ITConfig(T16N_TypeDef* T16Nx,TIM_TYPE_IT Type,TYPE_FUNCEN NewState);
void T16Nx_PWMOutConfig(TIM_TYPE_PWMOC Pwms,PWM_TYPE_OUT PwmType,TYPE_LEVEL PwmLevel);
void T16Nx_SetCNT(T16N_TypeDef* T16Nx,uint16_t Value);
void T16Nx_SetPRECNT(T16N_TypeDef* T16Nx,uint8_t Value);
void T16Nx_SetPREMAT(T16N_TypeDef* T16Nx,uint8_t Value);
void T16Nx_SetMAT0(T16N_TypeDef* T16Nx,uint16_t Value);
void T16Nx_SetMAT1(T16N_TypeDef* T16Nx,uint16_t Value);
void T16Nx_SetMAT2(T16N_TypeDef* T16Nx,uint16_t Value);
void T16Nx_SetMAT3(T16N_TypeDef* T16Nx,uint16_t Value);
uint16_t T16Nx_GetMAT0(T16N_TypeDef* T16Nx);
uint16_t T16Nx_GetMAT1(T16N_TypeDef* T16Nx);
uint16_t T16Nx_GetMAT2(T16N_TypeDef* T16Nx);
uint16_t T16Nx_GetMAT3(T16N_TypeDef* T16Nx);
uint16_t T16Nx_GetCNT(T16N_TypeDef* T16Nx);
uint8_t T16Nx_GetPRECNT(T16N_TypeDef* T16Nx);
FlagStatus T16Nx_GetFlagStatus(T16N_TypeDef* T16Nx,TIM_TYPE_IT TIM_Flag);
ITStatus T16Nx_GetITStatus(T16N_TypeDef* T16Nx,TIM_TYPE_IT TIM_Flag);
void T16Nx_ClearITPendingBit(T16N_TypeDef* T16Nx,TIM_TYPE_IT TIM_Flag);

void T32Nx_BaseInit(T32N_TypeDef* T32Nx,TIM_BaseInitStruType* TIM_BaseInitStruct);
void T32Nx_CapInit(T32N_TypeDef* T32Nx,TIM_CapInitStruType* TIM_CapInitStruct);
void T32Nx_MAT0ITConfig(T32N_TypeDef* T32Nx,TIM_TYPE_MATCON Type);
void T32Nx_MAT1ITConfig(T32N_TypeDef* T32Nx,TIM_TYPE_MATCON Type);
void T32Nx_MAT2ITConfig(T32N_TypeDef* T32Nx,TIM_TYPE_MATCON Type);
void T32Nx_MAT3ITConfig(T32N_TypeDef* T32Nx,TIM_TYPE_MATCON Type);
void T32Nx_MAT0Out0Config(T32N_TypeDef* T32Nx,TIM_TYPE_MATOUT Type);
void T32Nx_MAT1Out0Config(T32N_TypeDef* T32Nx,TIM_TYPE_MATOUT Type);
void T32Nx_MAT2Out1Config(T32N_TypeDef* T32Nx,TIM_TYPE_MATOUT Type);
void T32Nx_MAT3Out1Config(T32N_TypeDef* T32Nx,TIM_TYPE_MATOUT Type);
void T32Nx_ITConfig(T32N_TypeDef* T32Nx,TIM_TYPE_IT Type,TYPE_FUNCEN NewState);
void T32Nx_SetCNT(T32N_TypeDef* T16Nx,uint32_t Value);
void T32Nx_SetPRECNT(T32N_TypeDef* T32Nx,uint8_t Value);
void T32Nx_SetPREMAT(T32N_TypeDef* T32Nx,uint8_t Value);
void T32Nx_SetMAT0(T32N_TypeDef* T32Nx,uint32_t Value);
void T32Nx_SetMAT1(T32N_TypeDef* T32Nx,uint32_t Value);
void T32Nx_SetMAT2(T32N_TypeDef* T32Nx,uint32_t Value);
void T32Nx_SetMAT3(T32N_TypeDef* T32Nx,uint32_t Value);
uint32_t T32Nx_GetMAT0(T32N_TypeDef* T32Nx);
uint32_t T32Nx_GetMAT1(T32N_TypeDef* T32Nx);
uint32_t T32Nx_GetMAT2(T32N_TypeDef* T32Nx);
uint32_t T32Nx_GetMAT3(T32N_TypeDef* T32Nx);
uint32_t T32Nx_GetCNT(T32N_TypeDef* T32Nx);
uint8_t T32Nx_GetPRECNT(T32N_TypeDef* T32Nx);
FlagStatus T32Nx_GetFlagStatus(T32N_TypeDef* T32Nx,TIM_TYPE_IT TIM_Flag);
ITStatus T32Nx_GetITStatus(T32N_TypeDef* T32Nx,TIM_TYPE_IT TIM_Flag);
void T32Nx_ClearITPendingBit(T32N_TypeDef* T32Nx,TIM_TYPE_IT TIM_Flag);

#endif

/*************************END OF FILE**********************/

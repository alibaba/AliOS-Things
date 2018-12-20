/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#ifndef __LIB_GPIO_H__
#define __LIB_GPIO_H__

#include "p131.h"
#include "type.h"

typedef enum {
    GPIO_Pin_A0  = 0,
    GPIO_Pin_A1  = 1,
    GPIO_Pin_A2  = 2,
    GPIO_Pin_A3  = 3,
    GPIO_Pin_A4  = 4,
    GPIO_Pin_A5  = 5,
    GPIO_Pin_A6  = 6,
    GPIO_Pin_A7  = 7,
    GPIO_Pin_A8  = 8,
    GPIO_Pin_A9  = 9,
    GPIO_Pin_A10 = 10,
    GPIO_Pin_A11 = 11,
    GPIO_Pin_A12 = 12,
    GPIO_Pin_A13 = 13,
    GPIO_Pin_A14 = 14,
    GPIO_Pin_A15 = 15,
    GPIO_Pin_A16 = 16,
    GPIO_Pin_A17 = 17,
    GPIO_Pin_A18 = 18,
    GPIO_Pin_A19 = 19,
    GPIO_Pin_A20 = 20,
    GPIO_Pin_A21 = 21,
    GPIO_Pin_A22 = 22,
    GPIO_Pin_A23 = 23,
    GPIO_Pin_A24 = 24,
    GPIO_Pin_A25 = 25,
    GPIO_Pin_A26 = 26,
    GPIO_Pin_A27 = 27,
    GPIO_Pin_A28 = 28,
    GPIO_Pin_A29 = 29,
    GPIO_Pin_A30 = 30,
    GPIO_Pin_A31 = 31,
} GPIO_Pin;

typedef enum {
    GPIO_PINT0  = 0,
    GPIO_PINT1  = 1,
    GPIO_PINT2  = 2,
    GPIO_PINT3  = 3,
    GPIO_PINT4  = 4,
    GPIO_PINT5  = 5,
    GPIO_PINT6  = 6,
    GPIO_PINT7  = 7,
} GPIO_PINT;

typedef enum {
    GPIO_KINT0  = 0,
    GPIO_KINT1  = 1,
    GPIO_KINT2  = 2,
    GPIO_KINT3  = 3,
    GPIO_KINT4  = 4,
    GPIO_KINT5  = 5,
    GPIO_KINT6  = 6,
    GPIO_KINT7  = 7,
} GPIO_KINT;

typedef enum {
    GPIO_Func0 = 0,
    GPIO_Func1 = 1,
    GPIO_Func2 = 2,
    GPIO_Func3 = 3,
} GPIO_Func;

typedef enum {
    GPIO_Direction_Output = 0,
    GPIO_Direction_Input  = 1,
} GPIO_Direction;

typedef enum {
    GPIO_ODE_Output_Disable = 0,
    GPIO_ODE_Output_Enable  = 1,
} GPIO_ODE_Output;

typedef enum {
    GPIO_DS_Output_Normal = 0,
    GPIO_DS_Output_Strong = 1,
} GPIO_DS_Output;

typedef enum {
    GPIO_PUE_Input_Disable = 0,
    GPIO_PUE_Input_Enable  = 1,
} GPIO_PUE_Input;

typedef enum {
    GPIO_PDE_Input_Disable = 0,
    GPIO_PDE_Input_Enable  = 1,
} GPIO_PDE_Input;

/* signal type */
typedef enum {
    GPIO_Signal_Digital = 0,
    GPIO_Signal_Analog  = 1,
} GPIO_Signal_Type;

typedef struct {
    GPIO_Signal_Type Signal;
    GPIO_Direction Dir;
    GPIO_Func Func;

    GPIO_ODE_Output ODE;
    GPIO_DS_Output DS;

    GPIO_PUE_Input PUE;
    GPIO_PDE_Input PDE;
} GPIO_InitSettingType;

typedef enum {
    PINT_PMASK_Disable = 0,
    PINT_PMASK_Enable  = 1,
} PINT_PMASK_Set;

typedef enum {
    PINT_IE_Disable = 0,
    PINT_IE_Enable  = 1,
} PINT_IE_Set;

typedef enum {
    PINT_Trigger_Rising_Edge   = 0,
    PINT_Trigger_Trailing_Edge = 1,
    PINT_Trigger_High_Level    = 2,
    PINT_Trigger_Low_Level     = 3,
    PINT_Trigger_Both_Edge     = 4,
} PINT_Trigger_Style;

typedef struct {
    PINT_IE_Set IE_Set;
    PINT_PMASK_Set PMASK_Set;
    PINT_Trigger_Style Trigger_Style;
} PINT_InitSettingType;

typedef enum {
    KINT_KMASK_Disable = 0,
    KINT_KMASK_Enable  = 1,
} KINT_KMASK_Set;

typedef enum {
    KINT_IE_Disable = 0,
    KINT_IE_Enable  = 1,
} KINT_IE_Set;

typedef enum {
    KINT_Trigger_Rising_Edge   = 0,
    KINT_Trigger_Trailing_Edge = 1,
    KINT_Trigger_High_Level    = 2,
    KINT_Trigger_Low_Level     = 3,
    KINT_Trigger_Both_Edge     = 4,
} KINT_Trigger_Style;

typedef struct {
    KINT_IE_Set IE_Set;
    KINT_KMASK_Set KMASK_Set;
    KINT_Trigger_Style Trigger_Style;
} KINT_InitSettingType;

typedef enum {
    FLE_20ns = 0,       /* 20ns */
    FLE_2ns = 1,        /* 2ns */
} GPIO_FLT_Set;

typedef enum {
    TXPWM0 = 0,     /* 20ns */
    TXPWM1 = 1,     /* 2ns */
} TXPWM_Type;

typedef enum {
    TXPWM_Disable       = 0,
    TXPWM_WithT16N1OUT0 = 1,
    TXPWM_WithT16N1OUT1 = 2,
    TXPWM_WithBUZ       = 3,
} TXPWM_ModulationType;

typedef enum {
    TXPWM_Low  = 0,
    TXPWM_High = 1,
} TXPWM_Modulation_level;

typedef struct{
    TXPWM_Type TXPWMType;
    TXPWM_ModulationType ModulationType;
    TXPWM_Modulation_level Modulation_level;
    TYPE_FUNCEN BUZ_TXPWMOUT_EN;
    TYPE_FUNCEN T16N1OUT0_TXPWMOUT_EN;
    TYPE_FUNCEN T16N1OUT1_TXPWMOUT_EN;
    TYPE_FUNCEN TXD0_TXPWMOUT_EN;
} TXPWM_InitSettingType;


/* check param */
#define IS_GPIO_PIN(x) (((x) == GPIO_Pin_A0)  ||   \
                        ((x) == GPIO_Pin_A1)  ||   \
                        ((x) == GPIO_Pin_A2)  ||   \
                        ((x) == GPIO_Pin_A3)  ||   \
                        ((x) == GPIO_Pin_A4)  ||   \
                        ((x) == GPIO_Pin_A5)  ||   \
                        ((x) == GPIO_Pin_A6)  ||   \
                        ((x) == GPIO_Pin_A7)  ||   \
                        ((x) == GPIO_Pin_A8)  ||   \
                        ((x) == GPIO_Pin_A9)  ||   \
                        ((x) == GPIO_Pin_A10) ||   \
                        ((x) == GPIO_Pin_A11) ||   \
                        ((x) == GPIO_Pin_A12) ||   \
                        ((x) == GPIO_Pin_A13) ||   \
                        ((x) == GPIO_Pin_A14) ||   \
                        ((x) == GPIO_Pin_A15) ||   \
                        ((x) == GPIO_Pin_A16) ||   \
                        ((x) == GPIO_Pin_A17) ||   \
                        ((x) == GPIO_Pin_A18) ||   \
                        ((x) == GPIO_Pin_A19) ||   \
                        ((x) == GPIO_Pin_A20) ||   \
                        ((x) == GPIO_Pin_A21) ||   \
                        ((x) == GPIO_Pin_A22) ||   \
                        ((x) == GPIO_Pin_A23) ||   \
                        ((x) == GPIO_Pin_A24) ||   \
                        ((x) == GPIO_Pin_A25) ||   \
                        ((x) == GPIO_Pin_A26) ||   \
                        ((x) == GPIO_Pin_A27) ||   \
                        ((x) == GPIO_Pin_A28) ||   \
                        ((x) == GPIO_Pin_A29) ||   \
                        ((x) == GPIO_Pin_A30) ||   \
                        ((x) == GPIO_Pin_A31))

#define IS_GPIO_FUNC(x) (((x) == GPIO_Func0) ||     \
                         ((x) == GPIO_Func1) ||     \
                         ((x) == GPIO_Func2) ||     \
                         ((x) == GPIO_Func3))

#define IS_PINT_TRIGGER(x) (((x) == PINT_Trigger_Rising_Edge)   || \
                            ((x) == PINT_Trigger_Trailing_Edge) || \
                            ((x) == PINT_Trigger_High_Level)    || \
                            ((x) == PINT_Trigger_Low_Level)     || \
                            ((x) == PINT_Trigger_Both_Edge))
                
#define IS_PINT(x) (((x) == GPIO_PINT0) ||   \
                    ((x) == GPIO_PINT1) ||   \
                    ((x) == GPIO_PINT2) ||   \
                    ((x) == GPIO_PINT3) ||   \
                    ((x) == GPIO_PINT4) ||   \
                    ((x) == GPIO_PINT5) ||   \
                    ((x) == GPIO_PINT6) ||   \
                    ((x) == GPIO_PINT7))
                            
                            
#define IS_KINT_TRIGGER(x) (((x) == KINT_Trigger_Rising_Edge)   || \
                            ((x) == KINT_Trigger_Trailing_Edge) || \
                            ((x) == KINT_Trigger_High_Level)    || \
                            ((x) == KINT_Trigger_Low_Level)     || \
                            ((x) == KINT_Trigger_Both_Edge))
                
#define IS_KINT(x) (((x) == GPIO_KINT0) ||   \
                    ((x) == GPIO_KINT1) ||   \
                    ((x) == GPIO_KINT2) ||   \
                    ((x) == GPIO_KINT3) ||   \
                    ((x) == GPIO_KINT4) ||   \
                    ((x) == GPIO_KINT5) ||   \
                    ((x) == GPIO_KINT6) ||   \
                    ((x) == GPIO_KINT7))
                            
                            
                            
void GPIO_SetFunc(GPIO_Pin Pin, GPIO_Func Func);

void GPIO_SetDir_OUT(GPIO_Pin Pin);

void GPIO_SetDir_IN(GPIO_Pin Pin);

void GPIO_SetDir(GPIO_Pin Pin, GPIO_Direction Dir);

void GPIO_DirBrr(GPIO_Pin Pin);

void GPIO_SetODE(GPIO_Pin Pin, GPIO_ODE_Output ODE);

void GPIO_SetDS(GPIO_Pin Pin, GPIO_DS_Output DS);

void GPIO_SetPUE(GPIO_Pin Pin, GPIO_PUE_Input PUE);

void GPIO_SetPDE(GPIO_Pin Pin, GPIO_PDE_Input PDE);

void GPIO_SetSingalType(GPIO_Pin Pin, GPIO_Signal_Type Signal);

void GPIO_Init(GPIO_Pin Pin, GPIO_InitSettingType *InitSet);

uint32_t GPIO_ReadBit(GPIO_Pin Pin);

void GPIO_SetBit(GPIO_Pin Pin);
void GPIO_ResetBit(GPIO_Pin Pin);
void GPIO_WriteBit(GPIO_Pin Pin, uint32_t bit);

void GPIO_DataBrr(GPIO_Pin Pin);

void PINT_SetPMASK(GPIO_PINT PINT, PINT_PMASK_Set x);

void PINT_SetEnable(GPIO_PINT PINT, PINT_IE_Set IE);

ErrorStatus PINT_SetPINTSource(GPIO_PINT PINT, GPIO_Pin Pin);

void PINT_SetTriggerStyle(GPIO_PINT PINT, PINT_Trigger_Style Style);

ErrorStatus PINT_Init(GPIO_PINT PINT,GPIO_Pin Pin, PINT_InitSettingType *InitSet);

void PINT_ClearITFlag(GPIO_PINT PINT);

uint32_t PINT_GetITFlag(GPIO_PINT PINT);

void KINT_SetKMASK(GPIO_KINT KINT, KINT_KMASK_Set x);

void KINT_SetEnable(GPIO_KINT KINT, KINT_IE_Set IE);

ErrorStatus KINT_SetKINTSource(GPIO_KINT KINT, GPIO_Pin Pin);

void KINT_SetTriggerStyle(GPIO_KINT KINT, KINT_Trigger_Style Style);

ErrorStatus KINT_Init(GPIO_KINT KINT,GPIO_Pin Pin, KINT_InitSettingType *InitSet);

void KINT_ClearITFlag(GPIO_KINT KINT);

uint32_t KINT_GetITFlag(GPIO_KINT KINT);
void GPIO_SetFLT(GPIO_Pin Pin,GPIO_FLT_Set FLT_Set);

void GPIO_TXPWMInit(TXPWM_InitSettingType *InitSet);


void GPIO_setBUZ_LOAD(uint32_t load);

void GPIO_setBUZ_EN(TYPE_FUNCEN EN);

#endif

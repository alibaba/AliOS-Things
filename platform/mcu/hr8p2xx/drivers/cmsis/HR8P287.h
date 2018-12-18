/*******************************************************************
 * *file       : HR8P287.h
 * *description: HR8P287 Device Head File
 * *author     : Eastsoft MCU Software Team
 * *version    : V0.01
 * *data       : 1/29/2016
 *
 * *Copyright (C) 2016 Shanghai Eastsoft Microelectronics Co., Ltd.
 *******************************************************************/

#ifndef __HR8P287_H__
#define __HR8P287_H__


#define __I volatile const /* defines 'read only' permissions */
#define __O volatile /* defines 'write only' permissions */
#define __IO volatile /* defines 'read / write' permissions */


#define __CM0_REV 0 /* Core Revision r0p0 */
#define __NVIC_PRIO_BITS 2 /* HR8P287 uses 2 Bits for the Priority Levels */
#define __Vendor_SysTickConfig 0 /* Set to 1 if different SysTick Config is used */

typedef enum IRQn
{
    /****** Cortex-M0 Processor Exceptions Numbers ******************************************************/
    RST_IRQn = -15,
    NonMaskableInt_IRQn = -14,
    HardFault_IRQn = -13,
    SVC_IRQn = -5,
    PendSV_IRQn = -2,
    SysTick_IRQn = -1,
    PINT0_IRQn = 0,
    PINT1_IRQn = 1,
    PINT2_IRQn = 2,
    PINT3_IRQn = 3,
    PINT4_IRQn = 4,
    PINT5_IRQn = 5,
    PINT6_IRQn = 6,
    PINT7_IRQn = 7,
    T16N0_IRQn = 8,
    T16N1_IRQn = 9,
    T16N2_IRQn = 10,
    T16N3_IRQn = 11,
    T32N0_IRQn = 12,
    T32N1_IRQn = 13,
    T32N2_IRQn = 14,
    Reserved0_IRQn = 15,
    WDTINT_IRQn = 16,
    RTCINT_IRQn = 17,
    Reserved1_IRQn = 18,
    ADCINT_IRQn = 19,
    LCDINT_IRQn = 20,
    LVDINT_IRQn = 21,
    Reserved2_IRQn = 22,
    UART0_IRQn = 23,
    UART1_IRQn = 24,
    UART2_IRQn = 25,
    UART3_IRQn = 26,
    EUART0_IRQn = 27,
    Reserved3_IRQn = 28,
    SPIINT_IRQn = 29,
    I2CINT_IRQn = 30,
    Reserved4_IRQn = 31,
} IRQn_Type;


#include "core_cm0.h"
#include <stdint.h>

#pragma anon_unions

typedef union
{
    struct
    {
        uint32_t PROT: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} SCU_PROT_Typedef;

typedef union
{
    struct
    {
        uint32_t NMIEN: 1;
        uint32_t NMICS: 5;
        uint32_t RESERVED0: 26;
    };
    uint32_t Word;
} SCU_NMIC_Typedef;

typedef union
{
    struct
    {
        uint32_t PORVF: 1;
        uint32_t PORRCF: 1;
        uint32_t PORF: 1;
        uint32_t BORF: 1;
        uint32_t WDTRSTF: 1;
        uint32_t MRSTF: 1;
        uint32_t SOFT_RSTF: 1;
        uint32_t POR_LOST: 1;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} SCU_PWRC_Typedef;

typedef union
{
    struct
    {
        uint32_t LVDEN: 1;
        uint32_t LVD_FLTEN: 1;
        uint32_t RESERVED0: 2;
        uint32_t LVDVS: 4;
        uint32_t LVDIF: 1;
        uint32_t LVDIE: 1;
        uint32_t LVDIFS: 3;
        uint32_t RESERVED1: 2;
        uint32_t LVDO: 1;
        uint32_t RESERVED2: 16;
    };
    uint32_t Word;
} SCU_LVDC_Typedef;

typedef union
{
    struct
    {
        uint32_t CFGWORD: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} SCU_CFGWORD0_Typedef;

typedef union
{
    struct
    {
        uint32_t ACCT: 4;
        uint32_t RESERVED0: 28;
    };
    uint32_t Word;
} SCU_FLASHW_Typedef;

typedef union
{
    struct
    {
        uint32_t SYSCLK_SSE: 3;
        uint32_t RESERVED0: 1;
        uint32_t SYSCLK_SST: 3;
        uint32_t RESERVED1: 1;
        uint32_t SYSCLK_DIV: 3;
        uint32_t RESERVED2: 1;
        uint32_t SYSCLKOE: 1;
        uint32_t SYSCLK_CHG: 1;
        uint32_t RESERVED3: 2;
        uint32_t HRC_EN: 1;
        uint32_t HRC_ON: 1;
        uint32_t HRC_WAIT_SEL: 2;
        uint32_t RESERVED4: 4;
        uint32_t LRC_EN: 1;
        uint32_t LRC_ON: 1;
        uint32_t RESERVED5: 6;
    };
    uint32_t Word;
} SCU_SCLKEN0_Typedef;

typedef union
{
    struct
    {
        uint32_t HOSC_EN: 1;
        uint32_t HOSC_RDY: 1;
        uint32_t HOSC_TS: 2;
        uint32_t RESERVED0: 10;
        uint32_t LOSC_RDY: 1;
        uint32_t LOSC_LPEN: 1;
        uint32_t PLL_EN: 1;
        uint32_t PLL_RDY: 1;
        uint32_t PLL_ULOCK_SET: 2;
        uint32_t PLL_SSE: 3;
        uint32_t PLL_LOCK: 1;
        uint32_t ADC32K_SEL: 1;
        uint32_t LCD32K_SEL: 1;
        uint32_t WDT32K_SEL: 1;
        uint32_t RESERVED1: 5;
    };
    uint32_t Word;
} SCU_SCLKEN1_Typedef;

typedef union
{
    struct
    {
        uint32_t SCU_EN: 1;
        uint32_t GPIO_EN: 1;
        uint32_t IAP_EN: 1;
        uint32_t RESERVED0: 1;
        uint32_t ADC_EN: 1;
        uint32_t RTC_EN: 1;
        uint32_t LCDC_EN: 1;
        uint32_t WDT_EN: 1;
        uint32_t RESERVED1: 24;
    };
    uint32_t Word;
} SCU_PCLKEN0_Typedef;

typedef union
{
    struct
    {
        uint32_t T16N0_EN: 1;
        uint32_t T16N1_EN: 1;
        uint32_t T16N2_EN: 1;
        uint32_t T16N3_EN: 1;
        uint32_t RESERVED0: 4;
        uint32_t T32N0_EN: 1;
        uint32_t T32N1_EN: 1;
        uint32_t T32N2_EN: 1;
        uint32_t RESERVED1: 5;
        uint32_t UART0_EN: 1;
        uint32_t UART1_EN: 1;
        uint32_t UART2_EN: 1;
        uint32_t UART3_EN: 1;
        uint32_t EUART0_EN: 1;
        uint32_t RESERVED2: 3;
        uint32_t SPI_EN: 1;
        uint32_t RESERVED3: 3;
        uint32_t I2C_EN: 1;
        uint32_t RESERVED4: 3;
    };
    uint32_t Word;
} SCU_PCLKEN1_Typedef;

typedef union
{
    struct
    {
        uint32_t VR30_EN: 1;
        uint32_t RESERVED0: 3;
        uint32_t RESERVED3: 1;
        uint32_t RESERVED1: 3;
        uint32_t VR15_HEN: 1;
        uint32_t RESERVED2: 23;
    };
    uint32_t Word;
} SCU_VRCON_Typedef;

typedef union
{
    struct
    {
        uint32_t TBLREMAPEN: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} SCU_TBLRMEN_Typedef;

typedef union
{
    struct
    {
        uint32_t TBLOFF: 32;
    };
    uint32_t Word;
} SCU_TBLOFFS_Typedef;

typedef struct
{
    __IO SCU_PROT_Typedef PROT;
    __IO SCU_NMIC_Typedef NMIC;
    __IO SCU_PWRC_Typedef PWRC;
    uint32_t RESERVED0 ;
    __IO SCU_LVDC_Typedef LVDC;
    uint32_t RESERVED1[2] ;
    __I SCU_CFGWORD0_Typedef CFGWORD0;
    __IO SCU_FLASHW_Typedef FLASHW;
    uint32_t RESERVED2[7] ;
    __IO SCU_SCLKEN0_Typedef SCLKEN0;
    __IO SCU_SCLKEN1_Typedef SCLKEN1;
    __IO SCU_PCLKEN0_Typedef PCLKEN0;
    __IO SCU_PCLKEN1_Typedef PCLKEN1;
    __IO SCU_VRCON_Typedef VRCON;
    uint32_t RESERVED3[3] ;
    __IO SCU_TBLRMEN_Typedef TBLRMEN;
    __IO SCU_TBLOFFS_Typedef TBLOFFS;
} SCU_TypeDef;

typedef union
{
    struct
    {
        uint32_t DIR_0: 1;
        uint32_t DIR_1: 1;
        uint32_t DIR_2: 1;
        uint32_t DIR_3: 1;
        uint32_t DIR_4: 1;
        uint32_t DIR_5: 1;
        uint32_t DIR_6: 1;
        uint32_t DIR_7: 1;
        uint32_t DIR_8: 1;
        uint32_t DIR_9: 1;
        uint32_t DIR_10: 1;
        uint32_t DIR_11: 1;
        uint32_t DIR_12: 1;
        uint32_t DIR_13: 1;
        uint32_t DIR_14: 1;
        uint32_t DIR_15: 1;
        uint32_t DIR_16: 1;
        uint32_t DIR_17: 1;
        uint32_t DIR_18: 1;
        uint32_t DIR_19: 1;
        uint32_t DIR_20: 1;
        uint32_t DIR_21: 1;
        uint32_t DIR_22: 1;
        uint32_t DIR_23: 1;
        uint32_t DIR_24: 1;
        uint32_t DIR_25: 1;
        uint32_t DIR_26: 1;
        uint32_t DIR_27: 1;
        uint32_t DIR_28: 1;
        uint32_t DIR_29: 1;
        uint32_t DIR_30: 1;
        uint32_t DIR_31: 1;
    };
    uint32_t Word;
} GPIO_PADIR_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PADIRS_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PADIRC_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PADIRI_Typedef;

typedef union
{
    struct
    {
        uint32_t DATA_0: 1;
        uint32_t DATA_1: 1;
        uint32_t DATA_2: 1;
        uint32_t DATA_3: 1;
        uint32_t DATA_4: 1;
        uint32_t DATA_5: 1;
        uint32_t DATA_6: 1;
        uint32_t DATA_7: 1;
        uint32_t DATA_8: 1;
        uint32_t DATA_9: 1;
        uint32_t DATA_10: 1;
        uint32_t DATA_11: 1;
        uint32_t DATA_12: 1;
        uint32_t DATA_13: 1;
        uint32_t DATA_14: 1;
        uint32_t DATA_15: 1;
        uint32_t DATA_16: 1;
        uint32_t DATA_17: 1;
        uint32_t DATA_18: 1;
        uint32_t DATA_19: 1;
        uint32_t DATA_20: 1;
        uint32_t DATA_21: 1;
        uint32_t DATA_22: 1;
        uint32_t DATA_23: 1;
        uint32_t DATA_24: 1;
        uint32_t DATA_25: 1;
        uint32_t DATA_26: 1;
        uint32_t DATA_27: 1;
        uint32_t DATA_28: 1;
        uint32_t DATA_29: 1;
        uint32_t DATA_30: 1;
        uint32_t DATA_31: 1;
    };
    uint32_t Word;
} GPIO_PA_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PAS_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PAC_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PAI_Typedef;

typedef union
{
    struct
    {
        uint32_t PA0: 2;
        uint32_t RESERVED0: 2;
        uint32_t PA1: 2;
        uint32_t RESERVED1: 2;
        uint32_t PA2: 2;
        uint32_t RESERVED2: 2;
        uint32_t PA3: 2;
        uint32_t RESERVED3: 2;
        uint32_t PA4: 2;
        uint32_t RESERVED4: 2;
        uint32_t PA5: 2;
        uint32_t RESERVED5: 2;
        uint32_t PA6: 2;
        uint32_t RESERVED6: 2;
        uint32_t PA7: 2;
        uint32_t RESERVED7: 2;
    };
    uint32_t Word;
} GPIO_PAFUN0_Typedef;

typedef union
{
    struct
    {
        uint32_t PA8: 2;
        uint32_t RESERVED0: 2;
        uint32_t PA9: 2;
        uint32_t RESERVED1: 2;
        uint32_t PA10: 2;
        uint32_t RESERVED2: 2;
        uint32_t PA11: 2;
        uint32_t RESERVED3: 2;
        uint32_t PA12: 2;
        uint32_t RESERVED4: 2;
        uint32_t PA13: 2;
        uint32_t RESERVED5: 2;
        uint32_t PA14: 2;
        uint32_t RESERVED6: 2;
        uint32_t PA15: 2;
        uint32_t RESERVED7: 2;
    };
    uint32_t Word;
} GPIO_PAFUN1_Typedef;

typedef union
{
    struct
    {
        uint32_t PA16: 2;
        uint32_t RESERVED0: 2;
        uint32_t PA17: 2;
        uint32_t RESERVED1: 2;
        uint32_t PA18: 2;
        uint32_t RESERVED2: 2;
        uint32_t PA19: 2;
        uint32_t RESERVED3: 2;
        uint32_t PA20: 2;
        uint32_t RESERVED4: 2;
        uint32_t PA21: 2;
        uint32_t RESERVED5: 2;
        uint32_t PA22: 2;
        uint32_t RESERVED6: 2;
        uint32_t PA23: 2;
        uint32_t RESERVED7: 2;
    };
    uint32_t Word;
} GPIO_PAFUN2_Typedef;

typedef union
{
    struct
    {
        uint32_t PA24: 2;
        uint32_t RESERVED0: 2;
        uint32_t PA25: 2;
        uint32_t RESERVED1: 2;
        uint32_t PA26: 2;
        uint32_t RESERVED2: 2;
        uint32_t PA27: 2;
        uint32_t RESERVED3: 2;
        uint32_t PA28: 2;
        uint32_t RESERVED4: 2;
        uint32_t PA29: 2;
        uint32_t RESERVED5: 2;
        uint32_t PA30: 2;
        uint32_t RESERVED6: 2;
        uint32_t PA31: 2;
        uint32_t RESERVED7: 2;
    };
    uint32_t Word;
} GPIO_PAFUN3_Typedef;

typedef union
{
    struct
    {
        uint32_t PUEN_0: 1;
        uint32_t PUEN_1: 1;
        uint32_t PUEN_2: 1;
        uint32_t PUEN_3: 1;
        uint32_t PUEN_4: 1;
        uint32_t PUEN_5: 1;
        uint32_t PUEN_6: 1;
        uint32_t PUEN_7: 1;
        uint32_t PUEN_8: 1;
        uint32_t PUEN_9: 1;
        uint32_t PUEN_10: 1;
        uint32_t PUEN_11: 1;
        uint32_t PUEN_12: 1;
        uint32_t PUEN_13: 1;
        uint32_t PUEN_14: 1;
        uint32_t PUEN_15: 1;
        uint32_t PUEN_16: 1;
        uint32_t PUEN_17: 1;
        uint32_t PUEN_18: 1;
        uint32_t PUEN_19: 1;
        uint32_t PUEN_20: 1;
        uint32_t PUEN_21: 1;
        uint32_t PUEN_22: 1;
        uint32_t PUEN_23: 1;
        uint32_t PUEN_24: 1;
        uint32_t PUEN_25: 1;
        uint32_t PUEN_26: 1;
        uint32_t PUEN_27: 1;
        uint32_t PUEN_28: 1;
        uint32_t PUEN_29: 1;
        uint32_t PUEN_30: 1;
        uint32_t PUEN_31: 1;
    };
    uint32_t Word;
} GPIO_PAPUEN_Typedef;

typedef union
{
    struct
    {
        uint32_t PDEN_0: 1;
        uint32_t PDEN_1: 1;
        uint32_t PDEN_2: 1;
        uint32_t PDEN_3: 1;
        uint32_t PDEN_4: 1;
        uint32_t PDEN_5: 1;
        uint32_t PDEN_6: 1;
        uint32_t PDEN_7: 1;
        uint32_t PDEN_8: 1;
        uint32_t PDEN_9: 1;
        uint32_t PDEN_10: 1;
        uint32_t PDEN_11: 1;
        uint32_t PDEN_12: 1;
        uint32_t PDEN_13: 1;
        uint32_t PDEN_14: 1;
        uint32_t PDEN_15: 1;
        uint32_t PDEN_16: 1;
        uint32_t PDEN_17: 1;
        uint32_t PDEN_18: 1;
        uint32_t PDEN_19: 1;
        uint32_t PDEN_20: 1;
        uint32_t PDEN_21: 1;
        uint32_t PDEN_22: 1;
        uint32_t PDEN_23: 1;
        uint32_t PDEN_24: 1;
        uint32_t PDEN_25: 1;
        uint32_t PDEN_26: 1;
        uint32_t PDEN_27: 1;
        uint32_t PDEN_28: 1;
        uint32_t PDEN_29: 1;
        uint32_t PDEN_30: 1;
        uint32_t PDEN_31: 1;
    };
    uint32_t Word;
} GPIO_PAPDEN_Typedef;

typedef union
{
    struct
    {
        uint32_t OD_0: 1;
        uint32_t OD_1: 1;
        uint32_t OD_2: 1;
        uint32_t OD_3: 1;
        uint32_t OD_4: 1;
        uint32_t OD_5: 1;
        uint32_t OD_6: 1;
        uint32_t OD_7: 1;
        uint32_t OD_8: 1;
        uint32_t OD_9: 1;
        uint32_t OD_10: 1;
        uint32_t OD_11: 1;
        uint32_t OD_12: 1;
        uint32_t OD_13: 1;
        uint32_t OD_14: 1;
        uint32_t OD_15: 1;
        uint32_t OD_16: 1;
        uint32_t OD_17: 1;
        uint32_t OD_18: 1;
        uint32_t OD_19: 1;
        uint32_t OD_20: 1;
        uint32_t OD_21: 1;
        uint32_t OD_22: 1;
        uint32_t OD_23: 1;
        uint32_t OD_24: 1;
        uint32_t OD_25: 1;
        uint32_t OD_26: 1;
        uint32_t OD_27: 1;
        uint32_t OD_28: 1;
        uint32_t OD_29: 1;
        uint32_t OD_30: 1;
        uint32_t OD_31: 1;
    };
    uint32_t Word;
} GPIO_PAOD_Typedef;

typedef union
{
    struct
    {
        uint32_t DIR_0: 1;
        uint32_t DIR_1: 1;
        uint32_t DIR_2: 1;
        uint32_t DIR_3: 1;
        uint32_t DIR_4: 1;
        uint32_t DIR_5: 1;
        uint32_t DIR_6: 1;
        uint32_t DIR_7: 1;
        uint32_t DIR_8: 1;
        uint32_t DIR_9: 1;
        uint32_t DIR_10: 1;
        uint32_t DIR_11: 1;
        uint32_t DIR_12: 1;
        uint32_t DIR_13: 1;
        uint32_t DIR_14: 1;
        uint32_t DIR_15: 1;
        uint32_t DIR_16: 1;
        uint32_t DIR_17: 1;
        uint32_t DIR_18: 1;
        uint32_t DIR_19: 1;
        uint32_t DIR_20: 1;
        uint32_t DIR_21: 1;
        uint32_t DIR_22: 1;
        uint32_t DIR_23: 1;
        uint32_t RESERVED0: 8;
    };
    uint32_t Word;
} GPIO_PBDIR_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PBDIRS_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PBDIRC_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PBDIRI_Typedef;

typedef union
{
    struct
    {
        uint32_t DATA_0: 1;
        uint32_t DATA_1: 1;
        uint32_t DATA_2: 1;
        uint32_t DATA_3: 1;
        uint32_t DATA_4: 1;
        uint32_t DATA_5: 1;
        uint32_t DATA_6: 1;
        uint32_t DATA_7: 1;
        uint32_t DATA_8: 1;
        uint32_t DATA_9: 1;
        uint32_t DATA_10: 1;
        uint32_t DATA_11: 1;
        uint32_t DATA_12: 1;
        uint32_t DATA_13: 1;
        uint32_t DATA_14: 1;
        uint32_t DATA_15: 1;
        uint32_t DATA_16: 1;
        uint32_t DATA_17: 1;
        uint32_t DATA_18: 1;
        uint32_t DATA_19: 1;
        uint32_t DATA_20: 1;
        uint32_t DATA_21: 1;
        uint32_t DATA_22: 1;
        uint32_t DATA_23: 1;
        uint32_t RESERVED0: 8;
    };
    uint32_t Word;
} GPIO_PB_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PBS_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PBC_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PBI_Typedef;

typedef union
{
    struct
    {
        uint32_t PB0: 2;
        uint32_t RESERVED0: 2;
        uint32_t PB1: 2;
        uint32_t RESERVED1: 2;
        uint32_t PB2: 2;
        uint32_t RESERVED2: 2;
        uint32_t PB3: 2;
        uint32_t RESERVED3: 2;
        uint32_t PB4: 2;
        uint32_t RESERVED4: 2;
        uint32_t PB5: 2;
        uint32_t RESERVED5: 2;
        uint32_t PB6: 2;
        uint32_t RESERVED6: 2;
        uint32_t PB7: 2;
        uint32_t RESERVED7: 2;
    };
    uint32_t Word;
} GPIO_PBFUN0_Typedef;

typedef union
{
    struct
    {
        uint32_t PB8: 2;
        uint32_t RESERVED0: 2;
        uint32_t PB9: 2;
        uint32_t RESERVED1: 2;
        uint32_t PB10: 2;
        uint32_t RESERVED2: 2;
        uint32_t PB11: 2;
        uint32_t RESERVED3: 2;
        uint32_t PB12: 2;
        uint32_t RESERVED4: 2;
        uint32_t PB13: 2;
        uint32_t RESERVED5: 2;
        uint32_t PB14: 2;
        uint32_t RESERVED6: 2;
        uint32_t PB15: 2;
        uint32_t RESERVED7: 2;
    };
    uint32_t Word;
} GPIO_PBFUN1_Typedef;

typedef union
{
    struct
    {
        uint32_t PB16: 2;
        uint32_t RESERVED0: 2;
        uint32_t PB17: 2;
        uint32_t RESERVED1: 2;
        uint32_t PB18: 2;
        uint32_t RESERVED2: 2;
        uint32_t PB19: 2;
        uint32_t RESERVED3: 2;
        uint32_t PB20: 2;
        uint32_t RESERVED4: 2;
        uint32_t PB21: 2;
        uint32_t RESERVED5: 2;
        uint32_t PB22: 2;
        uint32_t RESERVED6: 2;
        uint32_t PB23: 2;
        uint32_t RESERVED7: 2;
    };
    uint32_t Word;
} GPIO_PBFUN2_Typedef;

typedef union
{
    struct
    {
        uint32_t PUEN_0: 1;
        uint32_t PUEN_1: 1;
        uint32_t PUEN_2: 1;
        uint32_t PUEN_3: 1;
        uint32_t PUEN_4: 1;
        uint32_t PUEN_5: 1;
        uint32_t PUEN_6: 1;
        uint32_t PUEN_7: 1;
        uint32_t PUEN_8: 1;
        uint32_t PUEN_9: 1;
        uint32_t PUEN_10: 1;
        uint32_t PUEN_11: 1;
        uint32_t PUEN_12: 1;
        uint32_t PUEN_13: 1;
        uint32_t PUEN_14: 1;
        uint32_t PUEN_15: 1;
        uint32_t PUEN_16: 1;
        uint32_t PUEN_17: 1;
        uint32_t PUEN_18: 1;
        uint32_t PUEN_19: 1;
        uint32_t PUEN_20: 1;
        uint32_t PUEN_21: 1;
        uint32_t PUEN_22: 1;
        uint32_t PUEN_23: 1;
        uint32_t RESERVED0: 8;
    };
    uint32_t Word;
} GPIO_PBPUEN_Typedef;

typedef union
{
    struct
    {
        uint32_t PDEN_0: 1;
        uint32_t PDEN_1: 1;
        uint32_t PDEN_2: 1;
        uint32_t PDEN_3: 1;
        uint32_t PDEN_4: 1;
        uint32_t PDEN_5: 1;
        uint32_t PDEN_6: 1;
        uint32_t PDEN_7: 1;
        uint32_t PDEN_8: 1;
        uint32_t PDEN_9: 1;
        uint32_t PDEN_10: 1;
        uint32_t PDEN_11: 1;
        uint32_t PDEN_12: 1;
        uint32_t PDEN_13: 1;
        uint32_t PDEN_14: 1;
        uint32_t PDEN_15: 1;
        uint32_t PDEN_16: 1;
        uint32_t PDEN_17: 1;
        uint32_t PDEN_18: 1;
        uint32_t PDEN_19: 1;
        uint32_t PDEN_20: 1;
        uint32_t PDEN_21: 1;
        uint32_t PDEN_22: 1;
        uint32_t PDEN_23: 1;
        uint32_t RESERVED0: 8;
    };
    uint32_t Word;
} GPIO_PBPDEN_Typedef;

typedef union
{
    struct
    {
        uint32_t OD_0: 1;
        uint32_t OD_1: 1;
        uint32_t OD_2: 1;
        uint32_t OD_3: 1;
        uint32_t OD_4: 1;
        uint32_t OD_5: 1;
        uint32_t OD_6: 1;
        uint32_t OD_7: 1;
        uint32_t OD_8: 1;
        uint32_t OD_9: 1;
        uint32_t OD_10: 1;
        uint32_t OD_11: 1;
        uint32_t OD_12: 1;
        uint32_t OD_13: 1;
        uint32_t OD_14: 1;
        uint32_t OD_15: 1;
        uint32_t OD_16: 1;
        uint32_t OD_17: 1;
        uint32_t OD_18: 1;
        uint32_t OD_19: 1;
        uint32_t OD_20: 1;
        uint32_t OD_21: 1;
        uint32_t OD_22: 1;
        uint32_t OD_23: 1;
        uint32_t RESERVED0: 8;
    };
    uint32_t Word;
} GPIO_PBOD_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 16;
        uint32_t SMITSEL_16: 1;
        uint32_t SMITSEL_17: 1;
        uint32_t SMITSEL_18: 1;
        uint32_t SMITSEL_19: 1;
        uint32_t SMITSEL_20: 1;
        uint32_t SMITSEL_21: 1;
        uint32_t SMITSEL_22: 1;
        uint32_t SMITSEL_23: 1;
        uint32_t RESERVED1: 8;
    };
    uint32_t Word;
} GPIO_PBSMITSEL_Typedef;


typedef union
{
    struct
    {
        uint32_t DIR_0: 1;
        uint32_t DIR_1: 1;
        uint32_t DIR_2: 1;
        uint32_t DIR_3: 1;
        uint32_t DIR_4: 1;
        uint32_t DIR_5: 1;
        uint32_t DIR_6: 1;
        uint32_t DIR_7: 1;
        uint32_t DIR_8: 1;
        uint32_t DIR_9: 1;
        uint32_t DIR_10: 1;
        uint32_t DIR_11: 1;
        uint32_t DIR_12: 1;
        uint32_t DIR_13: 1;
        uint32_t DIR_14: 1;
        uint32_t DIR_15: 1;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} GPIO_PCDIR_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PCDIRS_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PCDIRC_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PCDIRI_Typedef;

typedef union
{
    struct
    {
        uint32_t DATA_0: 1;
        uint32_t DATA_1: 1;
        uint32_t DATA_2: 1;
        uint32_t DATA_3: 1;
        uint32_t DATA_4: 1;
        uint32_t DATA_5: 1;
        uint32_t DATA_6: 1;
        uint32_t DATA_7: 1;
        uint32_t DATA_8: 1;
        uint32_t DATA_9: 1;
        uint32_t DATA_10: 1;
        uint32_t DATA_11: 1;
        uint32_t DATA_12: 1;
        uint32_t DATA_13: 1;
        uint32_t DATA_14: 1;
        uint32_t DATA_15: 1;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} GPIO_PC_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PCS_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PCC_Typedef;

typedef union
{
    uint32_t Word;
} GPIO_PCI_Typedef;

typedef union
{
    struct
    {
        uint32_t PC0: 2;
        uint32_t RESERVED0: 2;
        uint32_t PC1: 2;
        uint32_t RESERVED1: 2;
        uint32_t PC2: 2;
        uint32_t RESERVED2: 2;
        uint32_t PC3: 2;
        uint32_t RESERVED3: 2;
        uint32_t PC4: 2;
        uint32_t RESERVED4: 2;
        uint32_t PC5: 2;
        uint32_t RESERVED5: 2;
        uint32_t PC6: 2;
        uint32_t RESERVED6: 2;
        uint32_t PC7: 2;
        uint32_t RESERVED7: 2;
    };
    uint32_t Word;
} GPIO_PCFUN0_Typedef;

typedef union
{
    struct
    {
        uint32_t PC8: 2;
        uint32_t RESERVED0: 2;
        uint32_t PC9: 2;
        uint32_t RESERVED1: 2;
        uint32_t PC10: 2;
        uint32_t RESERVED2: 2;
        uint32_t PC11: 2;
        uint32_t RESERVED3: 2;
        uint32_t PC12: 2;
        uint32_t RESERVED4: 2;
        uint32_t PC13: 2;
        uint32_t RESERVED5: 2;
        uint32_t PC14: 2;
        uint32_t RESERVED6: 2;
        uint32_t PC15: 2;
        uint32_t RESERVED7: 2;
    };
    uint32_t Word;
} GPIO_PCFUN1_Typedef;

typedef union
{
    struct
    {
        uint32_t PUEN_0: 1;
        uint32_t PUEN_1: 1;
        uint32_t PUEN_2: 1;
        uint32_t PUEN_3: 1;
        uint32_t PUEN_4: 1;
        uint32_t PUEN_5: 1;
        uint32_t PUEN_6: 1;
        uint32_t PUEN_7: 1;
        uint32_t PUEN_8: 1;
        uint32_t PUEN_9: 1;
        uint32_t PUEN_10: 1;
        uint32_t PUEN_11: 1;
        uint32_t PUEN_12: 1;
        uint32_t PUEN_13: 1;
        uint32_t PUEN_14: 1;
        uint32_t PUEN_15: 1;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} GPIO_PCPUEN_Typedef;

typedef union
{
    struct
    {
        uint32_t PDEN_0: 1;
        uint32_t PDEN_1: 1;
        uint32_t PDEN_2: 1;
        uint32_t PDEN_3: 1;
        uint32_t PDEN_4: 1;
        uint32_t PDEN_5: 1;
        uint32_t PDEN_6: 1;
        uint32_t PDEN_7: 1;
        uint32_t PDEN_8: 1;
        uint32_t PDEN_9: 1;
        uint32_t PDEN_10: 1;
        uint32_t PDEN_11: 1;
        uint32_t PDEN_12: 1;
        uint32_t PDEN_13: 1;
        uint32_t PDEN_14: 1;
        uint32_t PDEN_15: 1;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} GPIO_PCPDEN_Typedef;

typedef union
{
    struct
    {
        uint32_t OD_0: 1;
        uint32_t OD_1: 1;
        uint32_t OD_2: 1;
        uint32_t OD_3: 1;
        uint32_t OD_4: 1;
        uint32_t OD_5: 1;
        uint32_t OD_6: 1;
        uint32_t OD_7: 1;
        uint32_t OD_8: 1;
        uint32_t OD_9: 1;
        uint32_t OD_10: 1;
        uint32_t OD_11: 1;
        uint32_t OD_12: 1;
        uint32_t OD_13: 1;
        uint32_t OD_14: 1;
        uint32_t OD_15: 1;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} GPIO_PCOD_Typedef;

typedef union
{
    struct
    {
        uint32_t PINTIE: 8;
        uint32_t PMASK: 8;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} GPIO_PIE_Typedef;

typedef union
{
    struct
    {
        uint32_t PINTIF_0: 1;
        uint32_t PINTIF_1: 1;
        uint32_t PINTIF_2: 1;
        uint32_t PINTIF_3: 1;
        uint32_t PINTIF_4: 1;
        uint32_t PINTIF_5: 1;
        uint32_t PINTIF_6: 1;
        uint32_t PINTIF_7: 1;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} GPIO_PIF_Typedef;

typedef union
{
    struct
    {
        uint32_t PINT0: 3;
        uint32_t RESERVED0: 1;
        uint32_t PINT1: 3;
        uint32_t RESERVED1: 1;
        uint32_t PINT2: 3;
        uint32_t RESERVED2: 1;
        uint32_t PINT3: 3;
        uint32_t RESERVED3: 1;
        uint32_t PINT4: 3;
        uint32_t RESERVED4: 1;
        uint32_t PINT5: 3;
        uint32_t RESERVED5: 1;
        uint32_t PINT6: 3;
        uint32_t RESERVED6: 1;
        uint32_t PINT7: 3;
        uint32_t RESERVED7: 1;
    };
    uint32_t Word;
} GPIO_PSEL_Typedef;

typedef union
{
    struct
    {
        uint32_t PINT0: 3;
        uint32_t RESERVED0: 1;
        uint32_t PINT1: 3;
        uint32_t RESERVED1: 1;
        uint32_t PINT2: 3;
        uint32_t RESERVED2: 1;
        uint32_t PINT3: 3;
        uint32_t RESERVED3: 1;
        uint32_t PINT4: 3;
        uint32_t RESERVED4: 1;
        uint32_t PINT5: 3;
        uint32_t RESERVED5: 1;
        uint32_t PINT6: 3;
        uint32_t RESERVED6: 1;
        uint32_t PINT7: 3;
        uint32_t RESERVED7: 1;
    };
    uint32_t Word;
} GPIO_PCFG_Typedef;

typedef union
{
    struct
    {
        uint32_t TX0PS: 2;
        uint32_t TX0PLV: 1;
        uint32_t RESERVED0: 1;
        uint32_t TX1PS: 2;
        uint32_t TX1PLV: 1;
        uint32_t RESERVED1: 1;
        uint32_t TX2PS: 2;
        uint32_t TX2PLV: 1;
        uint32_t RESERVED2: 1;
        uint32_t TX3PS: 2;
        uint32_t TX3PLV: 1;
        uint32_t RESERVED3: 17;
    };
    uint32_t Word;
} GPIO_TXPC_Typedef;

typedef union
{
    struct
    {
        uint32_t PROT_0: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} GPIO_PROT_Typedef;

typedef struct
{
    __IO GPIO_PADIR_Typedef PADIR;
    __O GPIO_PADIRS_Typedef PADIRS;
    __O GPIO_PADIRC_Typedef PADIRC;
    __O GPIO_PADIRI_Typedef PADIRI;
    __IO GPIO_PA_Typedef PA;
    __O GPIO_PAS_Typedef PAS;
    __O GPIO_PAC_Typedef PAC;
    __O GPIO_PAI_Typedef PAI;
    __IO GPIO_PAFUN0_Typedef PAFUN0;
    __IO GPIO_PAFUN1_Typedef PAFUN1;
    __IO GPIO_PAFUN2_Typedef PAFUN2;
    __IO GPIO_PAFUN3_Typedef PAFUN3;
    __IO GPIO_PAPUEN_Typedef PAPUEN;
    __IO GPIO_PAPDEN_Typedef PAPDEN;
    __IO GPIO_PAOD_Typedef PAOD;
    uint32_t RESERVED0[17] ;
    __IO GPIO_PBDIR_Typedef PBDIR;
    __O GPIO_PBDIRS_Typedef PBDIRS;
    __O GPIO_PBDIRC_Typedef PBDIRC;
    __O GPIO_PBDIRI_Typedef PBDIRI;
    __IO GPIO_PB_Typedef PB;
    __O GPIO_PBS_Typedef PBS;
    __O GPIO_PBC_Typedef PBC;
    __O GPIO_PBI_Typedef PBI;
    __IO GPIO_PBFUN0_Typedef PBFUN0;
    __IO GPIO_PBFUN1_Typedef PBFUN1;
    __IO GPIO_PBFUN2_Typedef PBFUN2;
    uint32_t RESERVED2 ;
    __IO GPIO_PBPUEN_Typedef PBPUEN;
    __IO GPIO_PBPDEN_Typedef PBPDEN;
    __IO GPIO_PBOD_Typedef PBOD;
    uint32_t RESERVED3 ;
    __IO GPIO_PBSMITSEL_Typedef PBSMITSEL;
    uint32_t RESERVED4[15] ;
    __IO GPIO_PCDIR_Typedef PCDIR;
    __O GPIO_PCDIRS_Typedef PCDIRS;
    __O GPIO_PCDIRC_Typedef PCDIRC;
    __O GPIO_PCDIRI_Typedef PCDIRI;
    __IO GPIO_PC_Typedef PC;
    __O GPIO_PCS_Typedef PCS;
    __O GPIO_PCC_Typedef PCC;
    __O GPIO_PCI_Typedef PCI;
    __IO GPIO_PCFUN0_Typedef PCFUN0;
    __IO GPIO_PCFUN1_Typedef PCFUN1;
    uint32_t RESERVED6[2];
    __IO GPIO_PCPUEN_Typedef PCPUEN;
    __IO GPIO_PCPDEN_Typedef PCPDEN;
    __IO GPIO_PCOD_Typedef PCOD;
    uint32_t RESERVED7[49] ;
    __IO GPIO_PIE_Typedef PIE;
    __IO GPIO_PIF_Typedef PIF;
    __IO GPIO_PSEL_Typedef PSEL;
    __IO GPIO_PCFG_Typedef PCFG;
    uint32_t RESERVED8[4] ;
    __IO GPIO_TXPC_Typedef TXPC;
    uint32_t RESERVED9[54] ;
    __IO GPIO_PROT_Typedef PROT;
} GPIO_TypeDef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t RST: 1;
        uint32_t RESERVED0: 2;
        uint32_t FLASH_REQ: 1;
        uint32_t FLASH_ACK: 1;
        uint32_t RESERVED1: 1;
        uint32_t FLASH_FAIL: 1;
        uint32_t RESERVED2: 24;
    };
    uint32_t Word;
} IAP_CON_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 2;
        uint32_t IAPCA: 8;
        uint32_t IAPPA: 6;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} IAP_ADDR_Typedef;

typedef union
{
    struct
    {
        uint32_t IAPD: 32;
    };
    uint32_t Word;
} IAP_DATA_Typedef;

typedef union
{
    struct
    {
        uint32_t IAPT: 32;
    };
    uint32_t Word;
} IAP_TRIG_Typedef;

typedef union
{
    struct
    {
        uint32_t IAPUL: 32;
    };
    uint32_t Word;
} IAP_UL_Typedef;

typedef union
{
    struct
    {
        uint32_t BSY: 1;
        uint32_t ERASE_END: 1;
        uint32_t PROG_END: 1;
        uint32_t TIMEOUT_ERR: 1;
        uint32_t RESERVED0: 28;
    };
    uint32_t Word;
} IAP_STA_Typedef;

typedef struct
{
    __IO IAP_CON_Typedef CON;
    __IO IAP_ADDR_Typedef ADDR;
    __IO IAP_DATA_Typedef DATA;
    __IO IAP_TRIG_Typedef TRIG;
    __IO IAP_UL_Typedef UL;
    __IO IAP_STA_Typedef STA;
} IAP_TypeDef;

typedef union
{
    struct
    {
        uint32_t DR: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} ADC_DR_Typedef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t TRG: 1;
        uint32_t RESERVED0: 30;
    };
    uint32_t Word;
} ADC_CON0_Typedef;

typedef union
{
    struct
    {
        uint32_t CLKDIV: 3;
        uint32_t CLKS: 1;
        uint32_t RESERVED0: 2;
        uint32_t FM: 1;
        uint32_t RESERVED1: 1;
        uint32_t VREFP: 2;
        uint32_t RESERVED2: 2;
        uint32_t SMPS: 1;
        uint32_t SMPON: 1;
        uint32_t ST: 2;
        uint32_t HSEN: 1;
        uint32_t RESERVED3: 15;
    };
    uint32_t Word;
} ADC_CON1_Typedef;

typedef union
{
    struct
    {
        uint32_t CHS: 4;
        uint32_t RESERVED0: 28;
    };
    uint32_t Word;
} ADC_CHS_Typedef;

typedef union
{
    struct
    {
        uint32_t IE: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} ADC_IE_Typedef;

typedef union
{
    struct
    {
        uint32_t IF: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} ADC_IF_Typedef;

typedef union
{
    struct
    {
        uint32_t BUF_EN: 2;
        uint32_t LP: 2;
        uint32_t RESERVED0: 28;
    };
    uint32_t Word;
} ADC_BUF_Typedef;

typedef struct
{
    __I ADC_DR_Typedef DR;
    __IO ADC_CON0_Typedef CON0;
    __IO ADC_CON1_Typedef CON1;
    __IO ADC_CHS_Typedef CHS;
    __IO ADC_IE_Typedef IE;
    __IO ADC_IF_Typedef IF;
    uint32_t RESERVED0[2] ;
    __IO ADC_BUF_Typedef BUF;
} ADC_TypeDef;

typedef union
{
    struct
    {
        uint32_t TMUP: 1;
        uint32_t TMWR: 1;
        uint32_t HSWI: 1;
        uint32_t CLKS: 1;
        uint32_t RESERVED0: 2;
        uint32_t XST: 1;
        uint32_t PON: 1;
        uint32_t RESERVED1: 24;
    };
    uint32_t Word;
} RTC_CON_Typedef;

typedef union
{
    struct
    {
        uint32_t DEV: 3;
        uint32_t RESERVED0: 1;
        uint32_t COCR: 4;
        uint32_t CLKC: 1;
        uint32_t RESERVED1: 23;
    };
    uint32_t Word;
} RTC_CALC_Typedef;

typedef union
{
    struct
    {
        uint32_t CALF: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} RTC_CALF_Typedef;

typedef union
{
    struct
    {
        uint32_t WM: 7;
        uint32_t RESERVED0: 1;
        uint32_t WH: 6;
        uint32_t RESERVED1: 2;
        uint32_t WW: 7;
        uint32_t RESERVED2: 9;
    };
    uint32_t Word;
} RTC_WA_Typedef;

typedef union
{
    struct
    {
        uint32_t DM: 7;
        uint32_t RESERVED0: 1;
        uint32_t DH: 6;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} RTC_DA_Typedef;

typedef union
{
    struct
    {
        uint32_t S01: 1;
        uint32_t S02: 1;
        uint32_t S04: 1;
        uint32_t S08: 1;
        uint32_t S10: 1;
        uint32_t S20: 1;
        uint32_t S40: 1;
        uint32_t RESERVED0: 25;
    };
    uint32_t Word;
} RTC_SEC_Typedef;

typedef union
{
    struct
    {
        uint32_t M01: 1;
        uint32_t M02: 1;
        uint32_t M04: 1;
        uint32_t M08: 1;
        uint32_t M10: 1;
        uint32_t M20: 1;
        uint32_t M40: 1;
        uint32_t RESERVED0: 25;
    };
    uint32_t Word;
} RTC_MIN_Typedef;

typedef union
{
    struct
    {
        uint32_t HOUR: 6;
        uint32_t RESERVED0: 26;
    };
    uint32_t Word;
} RTC_HOUR_Typedef;

typedef union
{
    struct
    {
        uint32_t WW: 3;
        uint32_t RESERVED0: 29;
    };
    uint32_t Word;
} RTC_WEEK_Typedef;

typedef union
{
    struct
    {
        uint32_t D01: 1;
        uint32_t D02: 1;
        uint32_t D04: 1;
        uint32_t D08: 1;
        uint32_t D10: 1;
        uint32_t D20: 1;
        uint32_t RESERVED0: 26;
    };
    uint32_t Word;
} RTC_DAY_Typedef;

typedef union
{
    struct
    {
        uint32_t MN01: 1;
        uint32_t MN02: 1;
        uint32_t MN04: 1;
        uint32_t MN08: 1;
        uint32_t MN10: 1;
        uint32_t RESERVED0: 27;
    };
    uint32_t Word;
} RTC_MON_Typedef;

typedef union
{
    struct
    {
        uint32_t Y01: 1;
        uint32_t Y02: 1;
        uint32_t Y04: 1;
        uint32_t Y08: 1;
        uint32_t Y10: 1;
        uint32_t Y20: 1;
        uint32_t Y40: 1;
        uint32_t Y80: 1;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} RTC_YEAR_Typedef;

typedef union
{
    struct
    {
        uint32_t WP: 32;
    };
    uint32_t Word;
} RTC_WP_Typedef;

typedef union
{
    struct
    {
        uint32_t SCDIE: 1;
        uint32_t MINIE: 1;
        uint32_t HORIE: 1;
        uint32_t DAYIE: 1;
        uint32_t MONIE: 1;
        uint32_t RESERVED0: 3;
        uint32_t DALE: 1;
        uint32_t WALE: 1;
        uint32_t RESERVED1: 2;
        uint32_t OSCDIE: 1;
        uint32_t RESERVED2: 19;
    };
    uint32_t Word;
} RTC_IE_Typedef;

typedef union
{
    struct
    {
        uint32_t SCDIF: 1;
        uint32_t MINIF: 1;
        uint32_t HORIF: 1;
        uint32_t DAYIF: 1;
        uint32_t MONIF: 1;
        uint32_t RESERVED0: 3;
        uint32_t DAFG: 1;
        uint32_t WAFG: 1;
        uint32_t RESERVED1: 2;
        uint32_t OSCDIF: 1;
        uint32_t RESERVED2: 19;
    };
    uint32_t Word;
} RTC_IF_Typedef;

typedef struct
{
    __IO RTC_CON_Typedef CON;
    __IO RTC_CALC_Typedef CALC;
    __IO RTC_CALF_Typedef CALF;
    uint32_t RESERVED0 ;
    __IO RTC_WA_Typedef WA;
    __IO RTC_DA_Typedef DA;
    __IO RTC_SEC_Typedef SEC;
    __IO RTC_MIN_Typedef MIN;
    __IO RTC_HOUR_Typedef HOUR;
    __IO RTC_WEEK_Typedef WEEK;
    __IO RTC_DAY_Typedef DAY;
    __IO RTC_MON_Typedef MON;
    __IO RTC_YEAR_Typedef YEAR;
    uint32_t RESERVED1[3] ;
    __IO RTC_WP_Typedef WP;
    uint32_t RESERVED2[3] ;
    __IO RTC_IE_Typedef IE;
    __IO RTC_IF_Typedef IF;
} RTC_TypeDef;

typedef union
{
    struct
    {
        uint32_t COMS: 3;
        uint32_t RESERVED0: 2;
        uint32_t FLIK: 1;
        uint32_t WFS: 1;
        uint32_t RESERVED1: 1;
        uint32_t BIAS: 2;
        uint32_t RS: 2;
        uint32_t BVS: 3;
        uint32_t RESERVED2: 1;
        uint32_t PRS: 6;
        uint32_t RESERVED3: 2;
        uint32_t BIAS_SEL: 2;
        uint32_t RESERVED4: 2;
        uint32_t BIAS_PWMSEL: 4;
    };
    uint32_t Word;
} LCDC_CON0_Typedef;

typedef union
{
    struct
    {
        uint32_t TON: 8;
        uint32_t TOFF: 8;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} LCDC_FLKT_Typedef;

typedef union
{
    struct
    {
        uint32_t SEG: 32;
    };
    uint32_t Word;
} LCDC_SEL0_Typedef;

typedef union
{
    struct
    {
        uint32_t SEG: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} LCDC_SEL1_Typedef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t RESERVED0: 3;
        uint32_t RST: 1;
        uint32_t RESERVED1: 3;
        uint32_t RFREQ: 1;
        uint32_t RESERVED2: 3;
        uint32_t RESERVED4: 4;
        uint32_t RESERVED3: 16;
    };
    uint32_t Word;
} LCDC_CON1_Typedef;

typedef union
{
    struct
    {
        uint32_t OFFIE: 1;
        uint32_t RFIE: 1;
        uint32_t RESERVED0: 30;
    };
    uint32_t Word;
} LCDC_IE_Typedef;

typedef union
{
    struct
    {
        uint32_t OFFIF: 1;
        uint32_t RFIF: 1;
        uint32_t RESERVED0: 30;
    };
    uint32_t Word;
} LCDC_IF_Typedef;

typedef union
{
    struct
    {
        uint32_t LCDD0: 32;
    };
    uint32_t Word;
} LCDC_D0_Typedef;

typedef union
{
    struct
    {
        uint32_t LCDD1: 32;
    };
    uint32_t Word;
} LCDC_D1_Typedef;

typedef union
{
    struct
    {
        uint32_t LCDD2: 32;
    };
    uint32_t Word;
} LCDC_D2_Typedef;

typedef union
{
    struct
    {
        uint32_t LCDD3: 32;
    };
    uint32_t Word;
} LCDC_D3_Typedef;

typedef union
{
    struct
    {
        uint32_t LCDD4: 32;
    };
    uint32_t Word;
} LCDC_D4_Typedef;

typedef union
{
    struct
    {
        uint32_t LCDD5: 32;
    };
    uint32_t Word;
} LCDC_D5_Typedef;

typedef union
{
    struct
    {
        uint32_t LCDD6: 32;
    };
    uint32_t Word;
} LCDC_D6_Typedef;

typedef union
{
    struct
    {
        uint32_t LCDD7: 32;
    };
    uint32_t Word;
} LCDC_D7_Typedef;

typedef union
{
    struct
    {
        uint32_t LCDD8: 32;
    };
    uint32_t Word;
} LCDC_D8_Typedef;

typedef union
{
    struct
    {
        uint32_t LCDD9: 32;
    };
    uint32_t Word;
} LCDC_D9_Typedef;

typedef union
{
    struct
    {
        uint32_t LCDD10: 32;
    };
    uint32_t Word;
} LCDC_D10_Typedef;

typedef union
{
    struct
    {
        uint32_t LCDD11: 32;
    };
    uint32_t Word;
} LCDC_D11_Typedef;

typedef struct
{
    __IO LCDC_CON0_Typedef CON0;
    __IO LCDC_FLKT_Typedef FLKT;
    __IO LCDC_SEL0_Typedef SEL0;
    __IO LCDC_SEL1_Typedef SEL1;
    __IO LCDC_CON1_Typedef CON1;
    __IO LCDC_IE_Typedef IE;
    __IO LCDC_IF_Typedef IF;
    uint32_t RESERVED0 ;
    __IO LCDC_D0_Typedef D0;
    __IO LCDC_D1_Typedef D1;
    __IO LCDC_D2_Typedef D2;
    __IO LCDC_D3_Typedef D3;
    __IO LCDC_D4_Typedef D4;
    __IO LCDC_D5_Typedef D5;
    __IO LCDC_D6_Typedef D6;
    __IO LCDC_D7_Typedef D7;
    __IO LCDC_D8_Typedef D8;
    __IO LCDC_D9_Typedef D9;
    __IO LCDC_D10_Typedef D10;
    __IO LCDC_D11_Typedef D11;
} LCDC_TypeDef;

typedef union
{
    struct
    {
        uint32_t LOAD: 32;
    };
    uint32_t Word;
} WDT_LOAD_Typedef;

typedef union
{
    struct
    {
        uint32_t VALUE: 30;
        uint32_t ST: 2;
    };
    uint32_t Word;
} WDT_VALUE_Typedef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t IE: 1;
        uint32_t RSTEN: 1;
        uint32_t CLKS: 1;
        uint32_t RESERVED0: 28;
    };
    uint32_t Word;
} WDT_CON_Typedef;

typedef union
{
    struct
    {
        uint32_t INTCLR: 32;
    };
    uint32_t Word;
} WDT_INTCLR_Typedef;

typedef union
{
    struct
    {
        uint32_t WDTIF: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} WDT_RIS_Typedef;

typedef union
{
    struct
    {
        uint32_t WDTLOCK: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} WDT_LOCK_Typedef;

typedef struct
{
    __O WDT_LOAD_Typedef LOAD;
    __I WDT_VALUE_Typedef VALUE;
    __IO WDT_CON_Typedef CON;
    __O WDT_INTCLR_Typedef INTCLR;
    __I WDT_RIS_Typedef RIS;
    uint32_t RESERVED0[59] ;
    __IO WDT_LOCK_Typedef LOCK;
} WDT_TypeDef;

typedef union
{
    struct
    {
        uint32_t CNT: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} T16N_CNT_Typedef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t CS: 2;
        uint32_t SYNC: 1;
        uint32_t EDGE: 2;
        uint32_t MOD: 2;
        uint32_t MAT0S: 2;
        uint32_t MAT1S: 2;
        uint32_t MAT2S: 2;
        uint32_t MAT3S: 2;
        uint32_t ASYWEN: 1;
        uint32_t RESERVED0: 15;
    };
    uint32_t Word;
} T16N_CON0_Typedef;

typedef union
{
    struct
    {
        uint32_t CAPPE: 1;
        uint32_t CAPNE: 1;
        uint32_t CAPIS0: 1;
        uint32_t CAPIS1: 1;
        uint32_t CAPT: 4;
        uint32_t RESERVED0: 8;
        uint32_t MOE0: 1;
        uint32_t MOE1: 1;
        uint32_t RESERVED1: 6;
        uint32_t MOM0: 2;
        uint32_t MOM1: 2;
        uint32_t MOM2: 2;
        uint32_t MOM3: 2;
    };
    uint32_t Word;
} T16N_CON1_Typedef;

typedef union
{
    struct
    {
        uint32_t PRECNT: 8;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} T16N_PRECNT_Typedef;

typedef union
{
    struct
    {
        uint32_t PREMAT: 8;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} T16N_PREMAT_Typedef;

typedef union
{
    struct
    {
        uint32_t MAT0IE: 1;
        uint32_t MAT1IE: 1;
        uint32_t MAT2IE: 1;
        uint32_t MAT3IE: 1;
        uint32_t IE: 1;
        uint32_t CAP0IE: 1;
        uint32_t CAP1IE: 1;
        uint32_t RESERVED0: 25;
    };
    uint32_t Word;
} T16N_IE_Typedef;

typedef union
{
    struct
    {
        uint32_t MAT0IF: 1;
        uint32_t MAT1IF: 1;
        uint32_t MAT2IF: 1;
        uint32_t MAT3IF: 1;
        uint32_t IF: 1;
        uint32_t CAP0IF: 1;
        uint32_t CAP1IF: 1;
        uint32_t RESERVED0: 25;
    };
    uint32_t Word;
} T16N_IF_Typedef;

typedef union
{
    struct
    {
        uint32_t MAT0: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} T16N_MAT0_Typedef;

typedef union
{
    struct
    {
        uint32_t MAT1: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} T16N_MAT1_Typedef;

typedef union
{
    struct
    {
        uint32_t MAT2: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} T16N_MAT2_Typedef;

typedef union
{
    struct
    {
        uint32_t MAT3: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} T16N_MAT3_Typedef;

typedef struct
{
    __IO T16N_CNT_Typedef CNT;
    __IO T16N_CON0_Typedef CON0;
    __IO T16N_CON1_Typedef CON1;
    uint32_t RESERVED0 ;
    __IO T16N_PRECNT_Typedef PRECNT;
    __IO T16N_PREMAT_Typedef PREMAT;
    __IO T16N_IE_Typedef IE;
    __IO T16N_IF_Typedef IF;
    __IO T16N_MAT0_Typedef MAT0;
    __IO T16N_MAT1_Typedef MAT1;
    __IO T16N_MAT2_Typedef MAT2;
    __IO T16N_MAT3_Typedef MAT3;
} T16N_TypeDef;

typedef union
{
    struct
    {
        uint32_t CNT: 32;
    };
    uint32_t Word;
} T32N_CNT_Typedef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t CS: 2;
        uint32_t SYNC: 1;
        uint32_t EDGE: 2;
        uint32_t MOD: 2;
        uint32_t MAT0S: 2;
        uint32_t MAT1S: 2;
        uint32_t MAT2S: 2;
        uint32_t MAT3S: 2;
        uint32_t ASYNCWREN: 1;
        uint32_t RESERVED0: 15;
    };
    uint32_t Word;
} T32N_CON0_Typedef;

typedef union
{
    struct
    {
        uint32_t CAPPE: 1;
        uint32_t CAPNE: 1;
        uint32_t CAPIS0: 1;
        uint32_t CAPIS1: 1;
        uint32_t CAPT: 4;
        uint32_t RESERVED0: 8;
        uint32_t MOE0: 1;
        uint32_t MOE1: 1;
        uint32_t RESERVED1: 6;
        uint32_t MOM0: 2;
        uint32_t MOM1: 2;
        uint32_t MOM2: 2;
        uint32_t MOM3: 2;
    };
    uint32_t Word;
} T32N_CON1_Typedef;

typedef union
{
    struct
    {
        uint32_t PRECNT: 8;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} T32N_PRECNT_Typedef;

typedef union
{
    struct
    {
        uint32_t PREMAT: 8;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} T32N_PREMAT_Typedef;

typedef union
{
    struct
    {
        uint32_t MAT0IE: 1;
        uint32_t MAT1IE: 1;
        uint32_t MAT2IE: 1;
        uint32_t MAT3IE: 1;
        uint32_t IE: 1;
        uint32_t CAP0IE: 1;
        uint32_t CAP1IE: 1;
        uint32_t RESERVED0: 25;
    };
    uint32_t Word;
} T32N_IE_Typedef;

typedef union
{
    struct
    {
        uint32_t MAT0IF: 1;
        uint32_t MAT1IF: 1;
        uint32_t MAT2IF: 1;
        uint32_t MAT3IF: 1;
        uint32_t IF: 1;
        uint32_t CAP0IF: 1;
        uint32_t CAP1IF: 1;
        uint32_t RESERVED0: 25;
    };
    uint32_t Word;
} T32N_IF_Typedef;

typedef union
{
    struct
    {
        uint32_t MAT0: 32;
    };
    uint32_t Word;
} T32N_MAT0_Typedef;

typedef union
{
    struct
    {
        uint32_t MAT1: 32;
    };
    uint32_t Word;
} T32N_MAT1_Typedef;

typedef union
{
    struct
    {
        uint32_t MAT2: 32;
    };
    uint32_t Word;
} T32N_MAT2_Typedef;

typedef union
{
    struct
    {
        uint32_t MAT3: 32;
    };
    uint32_t Word;
} T32N_MAT3_Typedef;

typedef struct
{
    __IO T32N_CNT_Typedef CNT;
    __IO T32N_CON0_Typedef CON0;
    __IO T32N_CON1_Typedef CON1;
    uint32_t RESERVED0 ;
    __IO T32N_PRECNT_Typedef PRECNT;
    __IO T32N_PREMAT_Typedef PREMAT;
    __IO T32N_IE_Typedef IE;
    __IO T32N_IF_Typedef IF;
    __IO T32N_MAT0_Typedef MAT0;
    __IO T32N_MAT1_Typedef MAT1;
    __IO T32N_MAT2_Typedef MAT2;
    __IO T32N_MAT3_Typedef MAT3;
} T32N_TypeDef;

typedef union
{
    struct
    {
        uint32_t TXEN: 1;
        uint32_t TRST: 1;
        uint32_t TXFS: 1;
        uint32_t RESERVED0: 1;
        uint32_t TXMOD: 3;
        uint32_t TXP: 1;
        uint32_t RXEN: 1;
        uint32_t RRST: 1;
        uint32_t RESERVED1: 2;
        uint32_t RXMOD: 3;
        uint32_t RXP: 1;
        uint32_t RESERVED2: 12;
        uint32_t BCS: 2;
        uint32_t RESERVED3: 2;
    };
    uint32_t Word;
} UART_CON_Typedef;

typedef union
{
    struct
    {
        uint32_t BRR: 11;
        uint32_t RESERVED0: 21;
    };
    uint32_t Word;
} UART_BRR_Typedef;

typedef union
{
    uint8_t Byte[4];
    uint16_t HalfWord[2];
    uint32_t Word;
} UART_TBW_Typedef;

typedef union
{
    uint8_t Byte[4];
    uint16_t HalfWord[2];
    uint32_t Word;
} UART_RBR_Typedef;

typedef union
{
    struct
    {
        uint32_t TB0: 9;
        uint32_t RESERVED0: 3;
        uint32_t TP0: 1;
        uint32_t TBEF0: 1;
        uint32_t RESERVED1: 2;
        uint32_t TB1: 9;
        uint32_t RESERVED2: 3;
        uint32_t TP1: 1;
        uint32_t TBEF1: 1;
        uint32_t RESERVED3: 2;
    };
    uint32_t Word;
} UART_TB01_Typedef;

typedef union
{
    struct
    {
        uint32_t TB2: 9;
        uint32_t RESERVED0: 3;
        uint32_t TP2: 1;
        uint32_t TBEF2: 1;
        uint32_t RESERVED1: 2;
        uint32_t TB3: 9;
        uint32_t RESERVED2: 3;
        uint32_t TP3: 1;
        uint32_t TBEF3: 1;
        uint32_t RESERVED3: 2;
    };
    uint32_t Word;
} UART_TB23_Typedef;

typedef union
{
    struct
    {
        uint32_t RB0: 9;
        uint32_t RESERVED0: 3;
        uint32_t RP0: 1;
        uint32_t RBFF0: 1;
        uint32_t FE0: 1;
        uint32_t PE0: 1;
        uint32_t RB1: 9;
        uint32_t RESERVED1: 3;
        uint32_t RP1: 1;
        uint32_t RBFF1: 1;
        uint32_t FE1: 1;
        uint32_t PE1: 1;
    };
    uint32_t Word;
} UART_RB01_Typedef;

typedef union
{
    struct
    {
        uint32_t RB2: 9;
        uint32_t RESERVED0: 3;
        uint32_t RP2: 1;
        uint32_t RBFF2: 1;
        uint32_t FE2: 1;
        uint32_t PE2: 1;
        uint32_t RB3: 9;
        uint32_t RESERVED1: 3;
        uint32_t RP3: 1;
        uint32_t RBFF3: 1;
        uint32_t FE3: 1;
        uint32_t PE3: 1;
    };
    uint32_t Word;
} UART_RB23_Typedef;

typedef union
{
    struct
    {
        uint32_t TBIE: 1;
        uint32_t RBIE: 1;
        uint32_t ROIE: 1;
        uint32_t FEIE: 1;
        uint32_t PEIE: 1;
        uint32_t TBWEIE: 1;
        uint32_t RESERVED0: 2;
        uint32_t TBIM: 2;
        uint32_t RBIM: 2;
        uint32_t TXIDLEIE: 1;
        uint32_t RXIDLEIE: 1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} UART_IE_Typedef;

typedef union
{
    struct
    {
        uint32_t TBIF: 1;
        uint32_t RBIF: 1;
        uint32_t ROIF: 1;
        uint32_t FEIF: 1;
        uint32_t PEIF: 1;
        uint32_t TBWEIF: 1;
        uint32_t RESERVED0: 6;
        uint32_t TXIDLEIF: 1;
        uint32_t RXIDLEIF: 1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} UART_IF_Typedef;

typedef struct
{
    __IO UART_CON_Typedef CON;
    __IO UART_BRR_Typedef BRR;
    __O UART_TBW_Typedef TBW;
    __I UART_RBR_Typedef RBR;
    __I UART_TB01_Typedef TB01;
    __I UART_TB23_Typedef TB23;
    __I UART_RB01_Typedef RB01;
    __I UART_RB23_Typedef RB23;
    __IO UART_IE_Typedef IE;
    __IO UART_IF_Typedef IF;
} UART_TypeDef;

typedef union
{
    struct
    {
        uint32_t TXEN: 1;
        uint32_t TRST: 1;
        uint32_t TXFS: 1;
        uint32_t RESERVED0: 1;
        uint32_t TXMOD: 3;
        uint32_t TXP: 1;
        uint32_t RXEN: 1;
        uint32_t RRST: 1;
        uint32_t RESERVED1: 2;
        uint32_t RXMOD: 3;
        uint32_t RXP: 1;
        uint32_t RESERVED2: 12;
        uint32_t BCS: 2;
        uint32_t RESERVED3: 2;
    };
    uint32_t Word;
} EUART_CON_Typedef;

typedef union
{
    struct
    {
        uint32_t BRR: 11;
        uint32_t RESERVED0: 21;
    };
    uint32_t Word;
} EUART_BRR_Typedef;

typedef union
{
    uint8_t Byte[4];
    uint16_t HalfWord[2];
    uint32_t Word;
} EUART_TBW_Typedef;

typedef union
{
    uint8_t Byte[4];
    uint16_t HalfWord[2];
    uint32_t Word;
} EUART_RBR_Typedef;

typedef union
{
    struct
    {
        uint32_t TB0: 9;
        uint32_t RESERVED0: 3;
        uint32_t TP0: 1;
        uint32_t TBEF0: 1;
        uint32_t RESERVED1: 2;
        uint32_t TB1: 9;
        uint32_t RESERVED2: 3;
        uint32_t TP1: 1;
        uint32_t TBEF1: 1;
        uint32_t RESERVED3: 2;
    };
    uint32_t Word;
} EUART_TB01_Typedef;

typedef union
{
    struct
    {
        uint32_t TB2: 9;
        uint32_t RESERVED0: 3;
        uint32_t TP2: 1;
        uint32_t TBEF2: 1;
        uint32_t RESERVED1: 2;
        uint32_t TB3: 9;
        uint32_t RESERVED2: 3;
        uint32_t TP3: 1;
        uint32_t TBEF3: 1;
        uint32_t RESERVED3: 2;
    };
    uint32_t Word;
} EUART_TB23_Typedef;

typedef union
{
    struct
    {
        uint32_t RB0: 9;
        uint32_t RESERVED0: 3;
        uint32_t RP0: 1;
        uint32_t RBEF0: 1;
        uint32_t FE0: 1;
        uint32_t PE0: 1;
        uint32_t RB1: 9;
        uint32_t RESERVED1: 3;
        uint32_t RP1: 1;
        uint32_t RBEF1: 1;
        uint32_t FE1: 1;
        uint32_t PE1: 1;
    };
    uint32_t Word;
} EUART_RB01_Typedef;

typedef union
{
    struct
    {
        uint32_t RB2: 9;
        uint32_t RESERVED0: 3;
        uint32_t RP2: 1;
        uint32_t RBEF2: 1;
        uint32_t FE2: 1;
        uint32_t PE2: 1;
        uint32_t RB3: 9;
        uint32_t RESERVED1: 3;
        uint32_t RP3: 1;
        uint32_t RBEF3: 1;
        uint32_t FE3: 1;
        uint32_t PE3: 1;
    };
    uint32_t Word;
} EUART_RB23_Typedef;

typedef union
{
    struct
    {
        uint32_t TBIE: 1;
        uint32_t RBIE: 1;
        uint32_t ROIE: 1;
        uint32_t FEIE: 1;
        uint32_t PEIE: 1;
        uint32_t TBWEIE: 1;
        uint32_t ARTEIE: 1;
        uint32_t RNAIE: 1;
        uint32_t TXIDLEIE: 1;
        uint32_t RXIDLEIE: 1;
        uint32_t RESERVED0: 2;
        uint32_t TBIM: 2;
        uint32_t RBIM: 2;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} EUART_IE_Typedef;

typedef union
{
    struct
    {
        uint32_t TBIF: 1;
        uint32_t RBIF: 1;
        uint32_t ROIF: 1;
        uint32_t FEIF: 1;
        uint32_t PEIF: 1;
        uint32_t TBWEIF: 1;
        uint32_t ARTEIF: 1;
        uint32_t RNAIF: 1;
        uint32_t TXIDLEIF: 1;
        uint32_t RXIDLEIF: 1;
        uint32_t RNACK: 1;
        uint32_t RESERVED0: 21;
    };
    uint32_t Word;
} EUART_IF_Typedef;

typedef union
{
    struct
    {
        uint32_t MOD: 1;
        uint32_t ERST: 1;
        uint32_t CK0E: 1;
        uint32_t CK1E: 1;
        uint32_t CHS: 1;
        uint32_t IOC: 1;
        uint32_t DAS: 1;
        uint32_t PS: 1;
        uint32_t ARTE: 1;
        uint32_t ARRE: 1;
        uint32_t TNAS: 2;
        uint32_t ARTS: 2;
        uint32_t CKS: 2;
        uint32_t ETUS: 8;
        uint32_t RESERVED0: 8;
    };
    uint32_t Word;
} EUART_MOD_Typedef;

typedef struct
{
    __IO EUART_CON_Typedef CON;
    __IO EUART_BRR_Typedef BRR;
    __O EUART_TBW_Typedef TBW;
    __I EUART_RBR_Typedef RBR;
    __I EUART_TB01_Typedef TB01;
    __I EUART_TB23_Typedef TB23;
    __I EUART_RB01_Typedef RB01;
    __I EUART_RB23_Typedef RB23;
    __IO EUART_IE_Typedef IE;
    __IO EUART_IF_Typedef IF;
    uint32_t RESERVED0[2] ;
    __IO EUART_MOD_Typedef MOD;
} EUART_TypeDef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t RST: 1;
        uint32_t MS: 1;
        uint32_t REN: 1;
        uint32_t RESERVED0: 1;
        uint32_t DRE: 1;
        uint32_t DFS: 2;
        uint32_t CKS: 8;
        uint32_t TME: 1;
        uint32_t TMS: 1;
        uint32_t TMP: 6;
        uint32_t RESERVED1: 8;
    };
    uint32_t Word;
} SPI_CON_Typedef;

typedef union
{
    uint8_t Byte[4];
    uint16_t HalfWord[2];
    uint32_t Word;
} SPI_TBW_Typedef;

typedef union
{
    uint8_t Byte[4];
    uint16_t HalfWord[2];
    uint32_t Word;
} SPI_RBR_Typedef;

typedef union
{
    struct
    {
        uint32_t TBIE: 1;
        uint32_t RBIE: 1;
        uint32_t TEIE: 1;
        uint32_t ROIE: 1;
        uint32_t IDIE: 1;
        uint32_t NSSIE: 1;
        uint32_t TBWEIE: 1;
        uint32_t RESERVED0: 1;
        uint32_t TBIM: 2;
        uint32_t RBIM: 2;
        uint32_t RESERVED1: 20;
    };
    uint32_t Word;
} SPI_IE_Typedef;

typedef union
{
    struct
    {
        uint32_t TBIF: 1;
        uint32_t RBIF: 1;
        uint32_t TEIF: 1;
        uint32_t ROIF: 1;
        uint32_t IDIF: 1;
        uint32_t NSSIF: 1;
        uint32_t TBWEIF: 1;
        uint32_t RESERVED0: 25;
    };
    uint32_t Word;
} SPI_IF_Typedef;

typedef union
{
    struct
    {
        uint32_t TB0: 8;
        uint32_t TB1: 8;
        uint32_t TB2: 8;
        uint32_t TB3: 8;
    };
    uint32_t Word;
} SPI_TB_Typedef;

typedef union
{
    struct
    {
        uint32_t RB0: 8;
        uint32_t RB1: 8;
        uint32_t RB2: 8;
        uint32_t RB3: 8;
    };
    uint32_t Word;
} SPI_RB_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 7;
        uint32_t NSS: 1;
        uint32_t TBEF0: 1;
        uint32_t TBEF1: 1;
        uint32_t TBEF2: 1;
        uint32_t TBEF3: 1;
        uint32_t RBFF0: 1;
        uint32_t RBFF1: 1;
        uint32_t RBFF2: 1;
        uint32_t RBFF3: 1;
        uint32_t IDLE: 1;
        uint32_t RESERVED1: 15;
    };
    uint32_t Word;
} SPI_STA_Typedef;

typedef struct
{
    __IO SPI_CON_Typedef CON;
    uint32_t RESERVED0 ;
    __O SPI_TBW_Typedef TBW;
    __I SPI_RBR_Typedef RBR;
    __IO SPI_IE_Typedef IE;
    __IO SPI_IF_Typedef IF;
    __I SPI_TB_Typedef TB;
    __I SPI_RB_Typedef RB;
    __I SPI_STA_Typedef STA;
} SPI_TypeDef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t RST: 1;
        uint32_t SCLOD: 1;
        uint32_t SDAOD: 1;
        uint32_t SCLSE: 1;
        uint32_t SDASE: 1;
        uint32_t RESERVED0: 1;
        uint32_t TJE: 1;
        uint32_t TJP: 8;
        uint32_t RW: 1;
        uint32_t SA: 7;
        uint32_t RESERVED1: 8;
    };
    uint32_t Word;
} I2C_CON_Typedef;

typedef union
{
    struct
    {
        uint32_t MS: 1;
        uint32_t RDM: 3;
        uint32_t CSE: 1;
        uint32_t ANAE: 1;
        uint32_t SRAE: 1;
        uint32_t SPAE: 1;
        uint32_t ADLY: 3;
        uint32_t ADE: 1;
        uint32_t TIS: 4;
        uint32_t SRT: 1;
        uint32_t SPT: 1;
        uint32_t RDT: 1;
        uint32_t BLD: 1;
        uint32_t RESERVED0: 4;
        uint32_t TAS: 1;
        uint32_t RESERVED1: 7;
    };
    uint32_t Word;
} I2C_MOD_Typedef;

typedef union
{
    struct
    {
        uint32_t SRIE: 1;
        uint32_t SPIE: 1;
        uint32_t TBIE: 1;
        uint32_t RBIE: 1;
        uint32_t TEIE: 1;
        uint32_t ROIE: 1;
        uint32_t NAIE: 1;
        uint32_t RESERVED0: 1;
        uint32_t TBIM: 2;
        uint32_t RBIM: 2;
        uint32_t TIDLEIE: 1;
        uint32_t RESERVED1: 19;
    };
    uint32_t Word;
} I2C_IE_Typedef;

typedef union
{
    struct
    {
        uint32_t SRIF: 1;
        uint32_t SPIF: 1;
        uint32_t TBIF: 1;
        uint32_t RBIF: 1;
        uint32_t TEIF: 1;
        uint32_t ROIF: 1;
        uint32_t NAIF: 1;
        uint32_t RESERVED0: 4;
        uint32_t TIDLEIF_R: 1;
        uint32_t TIDLEIF_W: 1;
        uint32_t RESERVED1: 19;
    };
    uint32_t Word;
} I2C_IF_Typedef;

typedef union
{
    uint8_t Byte[4];
    uint16_t HalfWord[2];
    uint32_t Word;
} I2C_TBW_Typedef;

typedef union
{
    uint8_t Byte[4];
    uint16_t HalfWord[2];
    uint32_t Word;
} I2C_RBR_Typedef;

typedef union
{
    struct
    {
        uint32_t TB0: 8;
        uint32_t TB1: 8;
        uint32_t TB2: 8;
        uint32_t TB3: 8;
    };
    uint32_t Word;
} I2C_TB_Typedef;

typedef union
{
    struct
    {
        uint32_t RB0: 8;
        uint32_t RB1: 8;
        uint32_t RB2: 8;
        uint32_t RB3: 8;
    };
    uint32_t Word;
} I2C_RB_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 8;
        uint32_t TBEF0: 1;
        uint32_t TBEF1: 1;
        uint32_t TBEF2: 1;
        uint32_t TBEF3: 1;
        uint32_t RBFF0: 1;
        uint32_t RBFF1: 1;
        uint32_t RBFF2: 1;
        uint32_t RBFF3: 1;
        uint32_t ACK: 1;
        uint32_t IDLE: 1;
        uint32_t RESERVED1: 14;
    };
    uint32_t Word;
} I2C_STA_Typedef;

typedef struct
{
    __IO I2C_CON_Typedef CON;
    __IO I2C_MOD_Typedef MOD;
    __IO I2C_IE_Typedef IE;
    __IO I2C_IF_Typedef IF;
    __O I2C_TBW_Typedef TBW;
    __I I2C_RBR_Typedef RBR;
    __I I2C_TB_Typedef TB;
    __I I2C_RB_Typedef RB;
    __I I2C_STA_Typedef STA;
} I2C_TypeDef;

/* Base addresses */
#define FLASH_BASE (0x00000000UL)
#define SRAM_BASE (0x20000000UL)
#define APB_BASE (0x40000000UL)
#define RAM_BASE (0x60000000UL)
#define SYS_BASE (0xE0000000UL)

/* APB peripherals */
#define SCU_BASE (APB_BASE + 0x00000)
#define GPIO_BASE (APB_BASE + 0x00400)
#define IAP_BASE (APB_BASE + 0x00800)
#define ADC_BASE (APB_BASE + 0x01000)
#define RTC_BASE (APB_BASE + 0x01400)
#define LCDC_BASE (APB_BASE + 0x01800)
#define WDT_BASE (APB_BASE + 0x01C00)
#define T16N0_BASE (APB_BASE + 0x02000)
#define T16N1_BASE (APB_BASE + 0x02400)
#define T16N2_BASE (APB_BASE + 0x02800)
#define T16N3_BASE (APB_BASE + 0x02C00)
#define T32N0_BASE (APB_BASE + 0x04000)
#define T32N1_BASE (APB_BASE + 0x04400)
#define T32N2_BASE (APB_BASE + 0x04800)
#define UART0_BASE (APB_BASE + 0x06000)
#define UART1_BASE (APB_BASE + 0x06400)
#define UART2_BASE (APB_BASE + 0x06800)
#define UART3_BASE (APB_BASE + 0x06C00)
#define EUART0_BASE (APB_BASE + 0x07000)
#define SPI_BASE (APB_BASE + 0x08000)
#define I2C_BASE (APB_BASE + 0x09000)

#define SCU ((SCU_TypeDef *) SCU_BASE )
#define GPIO ((GPIO_TypeDef *) GPIO_BASE )
#define IAP ((IAP_TypeDef *) IAP_BASE )
#define ADC ((ADC_TypeDef *) ADC_BASE )
#define RTC ((RTC_TypeDef *) RTC_BASE )
#define LCDC ((LCDC_TypeDef *) LCDC_BASE )
#define WDT ((WDT_TypeDef *) WDT_BASE )
#define T16N0 ((T16N_TypeDef *) T16N0_BASE )
#define T16N1 ((T16N_TypeDef *) T16N1_BASE )
#define T16N2 ((T16N_TypeDef *) T16N2_BASE )
#define T16N3 ((T16N_TypeDef *) T16N3_BASE )
#define T32N0 ((T32N_TypeDef *) T32N0_BASE )
#define T32N1 ((T32N_TypeDef *) T32N1_BASE )
#define T32N2 ((T32N_TypeDef *) T32N2_BASE )
#define UART0 ((UART_TypeDef *) UART0_BASE )
#define UART1 ((UART_TypeDef *) UART1_BASE )
#define UART2 ((UART_TypeDef *) UART2_BASE )
#define UART3 ((UART_TypeDef *) UART3_BASE )
#define EUART0 ((EUART_TypeDef *) EUART0_BASE )
#define SPI ((SPI_TypeDef *) SPI_BASE )
#define I2C ((I2C_TypeDef *) I2C_BASE )

#define UID0 (*(uint32_t *) 0x00200100 )
#define UID1 (*(uint32_t *) 0x00200104 )
#define UID2 (*(uint32_t *) 0x00200108 )

#endif

/*******************************************************************
* *file       : P131.h
* *description: P131 Device Head File
* *author     : Eastsoft MCU Software Team
* *version    : V0.01
* *data       : 3/2/2017
*
* *Copyright (C) 2017 Shanghai Eastsoft Microelectronics Co., Ltd.
*******************************************************************/

#ifndef __P131_H__
#define __P131_H__

#ifdef __cplusplus
 extern "C" {
#endif


/* IO definitions (access restrictions to peripheral registers) */
#ifdef __cplusplus
 #define __I volatile /* defines 'read only' permissions */
#else
 #define __I volatile const /* defines 'read only' permissions */
#endif
#define __O volatile /* defines 'write only' permissions */
#define __IO volatile /* defines 'read / write' permissions */
 

typedef enum IRQn
{
/****** Cortex-M3 Processor Exceptions Numbers ******************************************************/
 RST_IRQn = -15, 
 NonMaskableInt_IRQn = -14, 
 HardFault_IRQn = -13, 
 SVC_IRQn = -5, 
 PendSV_IRQn = -2, 
 SysTick_IRQn = -1, 
 GPIO0_IRQn = 0, 
 GPIO1_IRQn = 1, 
 GPIO2_IRQn = 2, 
 GPIO3_IRQn = 3, 
 GPIO4_IRQn = 4, 
 GPIO5_IRQn = 5, 
 GPIO6_IRQn = 6, 
 GPIO7_IRQn = 7, 
 T16N0_IRQn = 8, 
 T16N1_IRQn = 9, 
 T16N2_IRQn = 10, 
 RESERVED0_IRQn = 11, 
 T32N0_IRQn = 12, 
 T32N1_IRQn = 13, 
 T32N2_IRQn = 14, 
 T32N3_IRQn = 15, 
 IWDTINT_IRQn = 16, 
 WWDTINT_IRQn = 17, 
 SPI2_IRQn = 18, 
 SPI3_IRQn = 19, 
 UART0_IRQn = 20, 
 UART1_IRQn = 21, 
 UART2_IRQn = 22, 
 UART3_IRQn = 23, 
 SPI0_IRQn = 24, 
 SPI1_IRQn = 25, 
 I2C0_IRQn = 26, 
 I2C1_IRQn = 27, 
 AES_IRQn = 28,
 ADC_IRQn = 29,
 Tsensor_IRQn = 30,
 KINT_IRQn = 31,
 PLCRX_IRQn = 32,
 PLCTX_IRQn = 33,
 PLCTIMERIN_IRQn = 34,
 RESERVED4_IRQn = 35,
 DMACDONE_IRQn = 36,
 DMACRR_IRQn = 37,
 ECC_IRQn = 38,
 RNG_IRQn = 39,
 Cache_IRQn = 40,
 USB_IRQn = 41,
 USB_BWERR_IRQn = 42,
 ETHERNETINT_IRQn = 43,
 LVDINT_IRQn = 44,
 COMP_IRQn = 45,
} IRQn_Type;


#include "core_cm3.h"
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
        uint32_t NMICS: 8;
        uint32_t EN: 1;
        uint32_t RESERVED0: 23;
    };
    uint32_t Word;
} SCU_NMICON_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 1;
        uint32_t CHIPRSTF: 1;
        uint32_t PORF_3: 1;
        uint32_t RC_PORF: 1;
        uint32_t BORF: 1;
        uint32_t IWDTRSTF: 1;
        uint32_t MRSTF: 1;
        uint32_t SOFT_RSTF: 1;
        uint32_t POR_LOST: 1;
        uint32_t LOCKUP_RSTF: 1;
        uint32_t WWDTRSTF: 1;
        uint32_t ECCER_RSTF: 1;
        uint32_t RESERVED1: 4;
        uint32_t LOCKUPRST: 1;
        uint32_t CHIPRST: 1;
        uint32_t RESERVED2: 14;
    };
    uint32_t Word;
} SCU_PWRC_Typedef;

typedef union
{
    struct
    {
        uint32_t ACCT: 4;
        uint32_t RESERVED0: 12;
        uint32_t SE_WIDTH: 2;
        uint32_t RESERVED1: 6;
        uint32_t ECC_BY: 1;
        uint32_t RESERVED2: 7;
    };
    uint32_t Word;
} SCU_FLASHWAIT_Typedef;

typedef union
{
    struct
    {
        uint32_t FT_FLAG0: 1;
        uint32_t FT_FLAG1: 1;
        uint32_t FT_FLAG2: 1;
        uint32_t RESERVED0: 29;
    };
    uint32_t Word;
} SCU_FAULTFLAG_Typedef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t FLT: 1;
        uint32_t RESERVED0: 2;
        uint32_t VS: 4;
        uint32_t IF: 1;
        uint32_t IE: 1;
        uint32_t IFS: 3;
        uint32_t RESERVED1: 2;
        uint32_t LVD: 1;
        uint32_t RESERVED2: 16;
    };
    uint32_t Word;
} SCU_LVDCON_Typedef;

typedef union
{
    struct
    {
        uint32_t TBLREMPEN: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} SCU_TBLREMAPEN_Typedef;

typedef union
{
    struct
    {
        uint32_t BTFREMAP: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} SCU_BFREGION_Typedef;

typedef union
{
    struct
    {
        uint32_t T16N0EN: 1;
        uint32_t T16N1EN: 1;
        uint32_t T16N2EN: 1;
        uint32_t T16N3EN: 1;
        uint32_t RESERVED0: 4;
        uint32_t T32N0EN: 1;
        uint32_t T32N1EN: 1;
        uint32_t T32N2EN: 1;
        uint32_t T32N3EN: 1;
        uint32_t RESERVED1: 20;
    };
    uint32_t Word;
} SCU_TIMEREN_Typedef;

typedef union
{
    struct
    {
        uint32_t T16N0DIS: 1;
        uint32_t T16N1DIS: 1;
        uint32_t T16N2DIS: 1;
        uint32_t T16N3DIS: 1;
        uint32_t RESERVED0: 4;
        uint32_t T32N0DIS: 1;
        uint32_t T32N1DIS: 1;
        uint32_t T32N2DIS: 1;
        uint32_t T32N3DIS: 1;
        uint32_t RESERVED1: 20;
    };
    uint32_t Word;
} SCU_TIMERDIS_Typedef;

typedef union
{
    struct
    {
        uint32_t WAKEUPTIME: 14;
        uint32_t RESERVED0: 18;
    };
    uint32_t Word;
} SCU_WAKEUPTIME_Typedef;

typedef union
{
    struct
    {
        uint32_t MOSC_EN: 1;
        uint32_t CLKFLT_EN: 1;
        uint32_t MCU_DIV: 2;
        uint32_t HRC_EN: 1;
        uint32_t HRC_SEL_2M: 1;
        uint32_t RESERVED0: 1;
        uint32_t RESERVED1: 1;
        uint32_t XO_LP_SEL: 1;
        uint32_t XO_BUF_EN: 1;
        uint32_t XO_EN: 1;
        uint32_t XO_EXTEN: 1;
        uint32_t RESERVED2: 20;
    };
    uint32_t Word;
} SCU_SCLKEN0_Typedef;

typedef union
{
    struct
    {
        uint32_t OSCCLK_SEL: 1;
        uint32_t RESERVED0: 1;
        uint32_t DPLL_OSC_SEL: 1;
        uint32_t RESERVED1: 1;
        uint32_t DPLL2CLK_SEL: 1;
        uint32_t SYSCLK_SEL: 1;
        uint32_t RESERVED2: 2;
        uint32_t RESERVED3: 8;
        uint32_t RESERVED4: 1;
        uint32_t RESERVED5: 1;
        uint32_t DPLL2_EN: 1;
        uint32_t DPLL2_LDTEN: 1;
        uint32_t RESERVED6: 12;
    };
    uint32_t Word;
} SCU_SCLKEN1_Typedef;

typedef union
{
    struct
    {
        uint32_t CLKEN_DMA: 1;
        uint32_t CLKEN_USB: 1;
        uint32_t CLKEN_SRAM: 1;
        uint32_t CLKEN_APB1: 1;
        uint32_t CLKEN_APB2: 1;
        uint32_t CLKEN_ETH: 1;
        uint32_t RESERVED0: 10;
        uint32_t ABP1_PRE1: 2;
        uint32_t RESERVED1: 2;
        uint32_t ABP2_PRE1: 2;
        uint32_t RESERVED2: 10;
    };
    uint32_t Word;
} SCU_AHBEN_Typedef;

typedef union
{
    struct
    {
        uint32_t CLKEN_GPIO: 1;
        uint32_t CLKEN_FLASH: 1;
        uint32_t CLKEN_ADC: 1;
        uint32_t CLKEN_IWDT: 1;
        uint32_t CLKEN_T16N0: 1;
        uint32_t CLKEN_T16N1: 1;
        uint32_t CLKEN_T16N2: 1;
        uint32_t CLKEN_T16N3: 1;
        uint32_t CLKEN_T32N0: 1;
        uint32_t CLKEN_T32N1: 1;
        uint32_t CLKEN_T32N2: 1;
        uint32_t CLKEN_T32N3: 1;
        uint32_t CLKEN_UART0: 1;
        uint32_t CLKEN_UART1: 1;
        uint32_t CLKEN_UART2: 1;
        uint32_t CLKEN_UART3: 1;
        uint32_t CLKEN_SPI0: 1;
        uint32_t CLKEN_SPI1: 1;
        uint32_t CLKEN_I2C0: 1;
        uint32_t CLKEN_I2C1: 1;
        uint32_t CLKEN_TENSOR: 1;
        uint32_t RESERVED0: 2;
        uint32_t CLKEN_ECC: 1;
        uint32_t CLKEN_CRC: 1;
        uint32_t CLKEN_WWDT: 1;
        uint32_t CLKEN_OPAMP: 1;
        uint32_t CLKEN_COMP: 1;
        uint32_t RESERVED1: 1;
        uint32_t CLKEN_RNG: 1;
        uint32_t CLKEN_SPI2: 1;
        uint32_t CLKEN_SPI3: 1;
    };
    uint32_t Word;
} SCU_APB1EN_Typedef;

typedef union
{
    struct
    {
        uint32_t CLKEN_AES: 1;
        uint32_t CLKEN_BPLC: 1;
        uint32_t CLKEN_150M: 1;
        uint32_t CLKEN_TIMER: 1;
        uint32_t RESERVED0: 28;
    };
    uint32_t Word;
} SCU_APB2EN_Typedef;

typedef union
{
    struct
    {
        uint32_t TPIU_CLK_SEL: 1;
        uint32_t RESERVED0: 15;
        uint32_t BGR0_EN33: 1;
        uint32_t BGR0_BUF_EN: 1;
        uint32_t BGR1_EN: 1;
        uint32_t BGR1_BUF_EN: 1;
        uint32_t RESERVED1: 8;
    };
    uint32_t Word;
} SCU_MCUCTR_Typedef;

typedef union
{
    struct
    {
        uint32_t DMAEF: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} SCU_DMADONE_Typedef;

typedef union
{
    struct
    {
        uint32_t ETH_CS: 1;
        uint32_t ETH_RT: 1;
        uint32_t RESERVED0: 6;
        uint32_t ETH_FV: 6;
        uint32_t RESERVED1: 2;
        uint32_t RESERVED2: 16;
    };
    uint32_t Word;
} SCU_ETHCON_Typedef;

typedef union
{
    struct
    {
        uint32_t PREDIV: 3;
        uint32_t OFFSETEN: 1;
        uint32_t DNOFFSET: 2;
        uint32_t UPOFFSET: 2;
        uint32_t CPI: 6;
        uint32_t RESERVED0: 2;
        uint32_t LPFRES: 3;
        uint32_t RESERVED1: 5;
        uint32_t RESERVED2: 4;
        uint32_t LOCKED: 1;
        uint32_t DPLL2_FLAG1: 1;
        uint32_t DPLL2_FLAG0: 1;
        uint32_t RESERVED3: 1;
    };
    uint32_t Word;
} SCU_DPLL2CON0_Typedef;

typedef union
{
    struct
    {
        uint32_t NDIV: 8;
        uint32_t RESERVED0: 4;
        uint32_t RESERVED1: 1;
        uint32_t POSTDIV_ADC: 3;
        uint32_t RESERVED2: 16;
    };
    uint32_t Word;
} SCU_DPLL2CON1_Typedef;

typedef union
{
    struct
    {
        uint32_t CHIPID0L: 16;
        uint32_t CHIPID0H: 16;
    };
    uint32_t Word;
} SCU_CHIPID0_Typedef;

typedef union
{
    struct
    {
        uint32_t CHIPID1: 32;
    };
    uint32_t Word;
} SCU_CHIPID1_Typedef;

typedef union
{
    struct
    {
        uint32_t CHIPID2: 32;
    };
    uint32_t Word;
} SCU_CHIPID2_Typedef;

typedef union
{
    struct
    {
        uint32_t CHIPID3: 32;
    };
    uint32_t Word;
} SCU_CHIPID3_Typedef;

typedef union
{
    struct
    {
        uint32_t SRT: 1;
        uint32_t B33: 1;
        uint32_t PWR: 1;
        uint32_t B12: 1;
        uint32_t IWD: 1;
        uint32_t WWD: 1;
        uint32_t B33_VS: 3;
        uint32_t B12_VS: 3;
        uint32_t WDT_VDS: 1;
        uint32_t RESERVED0: 1;
        uint32_t DBG: 2;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} SCU_CFGWORD0_Typedef;

typedef union
{
    struct
    {
        uint32_t CTM: 1;
        uint32_t MDCK_FLT: 3;
        uint32_t MCCK_FLT: 3;
        uint32_t FWP: 1;
        uint32_t FWPS: 8;
        uint32_t RESERVED0: 2;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} SCU_CFGWORD1_Typedef;

typedef union
{
    struct
    {
        uint32_t CFG_XTCAL: 8;
        uint32_t RESERVED0: 2;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} SCU_CFGXTCAL_Typedef;

typedef union
{
    struct
    {
        uint32_t ENC: 32;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} SCU_CFGENCRY_Typedef;

typedef union
{
    struct
    {
        uint32_t CFG_ADC_MSB: 2;
        uint32_t RESERVED0: 14;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} SCU_CFGADCCAL_Typedef;

typedef union
{
    struct
    {
        uint32_t BLD_ST: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} SCU_CFGCAL_RESV1_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 16;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} SCU_CFGCAL_RESV2_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 8;
        uint32_t CFG_XO_IB_TUNE: 6;
        uint32_t RESERVED1: 22;
        uint32_t RESERVED2: 2;
    };
    uint32_t Word;
} SCU_CFGOSCCAL0_Typedef;

typedef union
{
    struct
    {
        uint32_t CFG_OSCRC_CAP: 2;
        uint32_t CFG_OSCRC_IB_ISEL: 2;
        uint32_t CFG_OSCRC_CAL: 10;
        uint32_t RESERVED0: 18;
    };
    uint32_t Word;
} SCU_CFGOSCCAL1_Typedef;

typedef union
{
    struct
    {
        uint32_t CFG_OSC_ISEL: 4;
        uint32_t CFG_OSC_BUF_ISEL: 2;
        uint32_t RESERVED0: 2;
        uint32_t CFG_OSC_CAP_SEL: 8;
        uint32_t : 16;
    };
    uint32_t Word;
} SCU_CFGOSCCAL2_Typedef;

typedef union
{
    struct
    {
        uint32_t CFG_OSC_LDO: 14;
        uint32_t RESERVED0: 2;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} SCU_CFGOSCCAL3_Typedef;

typedef union
{
    struct
    {
        uint32_t CFG_BGR0TCAL: 4;
        uint32_t CFG_BGR0VCAL: 4;
        uint32_t CFG_BGR1TCAL: 4;
        uint32_t CFG_BGR1VCAL: 4;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} SCU_CFGVR0_Typedef;

typedef union
{
    struct
    {
        uint32_t CFG_VCTL_DCDC: 4;
        uint32_t RESERVED0: 4;
        uint32_t CFG_DPLL2_KVCOI: 3;
        uint32_t RESERVED1: 1;
        uint32_t CFG_DPLL2_VCOI: 3;
        uint32_t RESERVED2: 1;
        uint32_t RESERVED3: 16;
    };
    uint32_t Word;
} SCU_CFGVR1_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 32;
    };
    uint32_t Word;
} SCU_CFGAFE1_Typedef;



typedef union
{
    struct
    {
        uint32_t SYS_TSTMD: 4;
        uint32_t RESERVED0: 4;
        uint32_t SRAM_TM: 4;
        uint32_t FLASH_TM: 4;
        uint32_t XOSC_FLAG1: 1;
        uint32_t XOSC_FLAG0: 1;
        uint32_t RESERVED1: 2;
        uint32_t CTL_TST_DCDC: 1;
        uint32_t : 3;
        uint32_t BGR0_TSTSEL: 1;
        uint32_t BGR0_CHOP: 1;
        uint32_t BGR1_TSTSEL: 1;
        uint32_t BGR1_CHOP: 1;
        uint32_t RESERVED2: 4;
    };
    uint32_t Word;
} SCU_SYS_Typedef;

typedef union
{
    struct
    {
        uint32_t FDIVTSTEN: 1;
        uint32_t CPTST: 3;
        uint32_t VCTSTEN: 1;
        uint32_t RESERVED0: 27;
    };
    uint32_t Word;
} SCU_DPLL2TEST_Typedef;

typedef union
{
    struct
    {
        uint32_t BOR_FLT: 3;
        uint32_t RESERVED0: 5;
        uint32_t BOR12_VS: 3;
        uint32_t RESERVED1: 1;
        uint32_t BOR33_VS: 3;
        uint32_t RESERVED2: 1;
        uint32_t BOR12F: 1;
        uint32_t BOR33F: 1;
        uint32_t BORFTF: 1;
        uint32_t RESERVED3: 13;
    };
    uint32_t Word;
} SCU_BORTEST_Typedef;

typedef union
{
    struct
    {
        uint32_t TST_ADC_MSB: 2;
        uint32_t RESERVED0: 14;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} SCU_ADCCAL_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 16;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} SCU_CAL_RESV1_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 16;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} SCU_CAL_RESV2_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 8;
        uint32_t TST_XO_IB_TUNE: 6;
        uint32_t RESERVED1: 22;
        uint32_t RESERVED2: 2;
    };
    uint32_t Word;
} SCU_OSCCAL0_Typedef;

typedef union
{
    struct
    {
        uint32_t TST_OSCRC_CAP: 2;
        uint32_t TST_OSCRC_IB_ISEL: 2;
        uint32_t TST_OSCRC_CAL: 10;
        uint32_t : 18;
    };
    uint32_t Word;
} SCU_OSCCAL1_Typedef;

typedef union
{
    struct
    {
        uint32_t TST_OSC_ISEL: 4;
        uint32_t TST_OSC_BUF_ISEL: 2;
        uint32_t RESERVED0: 2;
        uint32_t TST_OSC_CAP_SEL: 8;
        uint32_t : 16;
    };
    uint32_t Word;
} SCU_OSCCAL2_Typedef;

typedef union
{
    struct
    {
        uint32_t TST_OSC_LDO: 14;
        uint32_t RESERVED0: 2;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} SCU_OSCCAL3_Typedef;

typedef union
{
    struct
    {
        uint32_t TST_BGR0TCAL: 4;
        uint32_t TST_BGR0VCAL: 4;
        uint32_t TST_BGR1TCAL: 4;
        uint32_t TST_BGR1VCAL: 4;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} SCU_VR0_Typedef;

typedef union
{
    struct
    {
        uint32_t TST_VCTL_DCDC: 4;
        uint32_t RESERVED0: 4;
        uint32_t TST_DPLL2_KVCOI: 3;
        uint32_t RESERVED1: 1;
        uint32_t TST_DPLL2_VCOI: 3;
        uint32_t RESERVED2: 1;
        uint32_t RESERVED3: 16;
    };
    uint32_t Word;
} SCU_VR1_Typedef;

typedef union
{
    struct
    {
        uint32_t TST_AFEBGR_TCAL: 4;
        uint32_t TST_AFEBGR_ICAL: 4;
        uint32_t : 8;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} SCU_AFE0_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 16;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} SCU_AFE1_Typedef;

typedef struct
{
    __IO SCU_PROT_Typedef PROT;
    __IO SCU_NMICON_Typedef NMICON;
    __IO SCU_PWRC_Typedef PWRC;
    __IO SCU_FLASHWAIT_Typedef FLASHWAIT;
    __IO SCU_FAULTFLAG_Typedef FAULTFLAG;
    __IO SCU_LVDCON_Typedef LVDCON;
    __IO SCU_TBLREMAPEN_Typedef TBLREMAPEN;
    __IO SCU_BFREGION_Typedef BFREGION;
    __O SCU_TIMEREN_Typedef TIMEREN;
    __O SCU_TIMERDIS_Typedef TIMERDIS;
    __IO SCU_WAKEUPTIME_Typedef WAKEUPTIME;
    uint32_t RESERVED0[5] ;
    __IO SCU_SCLKEN0_Typedef SCLKEN0;
    __IO SCU_SCLKEN1_Typedef SCLKEN1;
    uint32_t RESERVED1 ;
    __IO SCU_AHBEN_Typedef AHBEN;
    __IO SCU_APB1EN_Typedef APB1EN;
    __IO SCU_APB2EN_Typedef APB2EN;
    uint32_t RESERVED2 ;
    __IO SCU_MCUCTR_Typedef MCUCTR;
    uint32_t RESERVED3 ;
    __IO SCU_DMADONE_Typedef DMADONE;
    __IO SCU_ETHCON_Typedef  ETHCON;
    uint32_t RESERVED4[5] ;
    __IO SCU_DPLL2CON0_Typedef DPLL2CON0;
    __IO SCU_DPLL2CON1_Typedef DPLL2CON1;
    uint32_t RESERVED5[14] ;
    __I SCU_CFGWORD0_Typedef CFGWORD0;
    __I SCU_CFGWORD1_Typedef CFGWORD1;
    __I SCU_CFGXTCAL_Typedef CFGXTCAL;
    uint32_t RESERVED6[3] ;
    __I SCU_CFGENCRY_Typedef CFGENCRY;
    uint32_t RESERVED7[9] ;
    __I SCU_CFGADCCAL_Typedef CFGADCCAL;
    __I SCU_CFGCAL_RESV1_Typedef CFGCAL_RESV1;
    __I SCU_CFGCAL_RESV2_Typedef CFGCAL_RESV2;
    uint32_t RESERVED8[3] ;
    __I SCU_CFGOSCCAL0_Typedef CFGOSCCAL0;
    __I SCU_CFGOSCCAL1_Typedef CFGOSCCAL1;
    __I SCU_CFGOSCCAL2_Typedef CFGOSCCAL2;
    __I SCU_CFGOSCCAL3_Typedef CFGOSCCAL3;
    uint32_t RESERVED9[2] ;
    __I SCU_CFGVR0_Typedef CFGVR0;
    __I SCU_CFGVR1_Typedef CFGVR1;
    uint32_t RESERVED10 ;
    __I SCU_CFGAFE1_Typedef CFGAFE1;
    uint32_t RESERVED11[51] ;
    __IO SCU_DPLL2TEST_Typedef DPLL2TEST;
    uint32_t RESERVED12 ;
    __IO SCU_ADCCAL_Typedef ADCCAL;
    __IO SCU_CAL_RESV1_Typedef CAL_RESV1;
    __IO SCU_CAL_RESV2_Typedef CAL_RESV2;
    __IO SCU_OSCCAL0_Typedef OSCCAL0;
    __IO SCU_OSCCAL1_Typedef OSCCAL1;
    __IO SCU_OSCCAL2_Typedef OSCCAL2;
    __IO SCU_OSCCAL3_Typedef OSCCAL3;
    uint32_t RESERVED13[2] ;
    __IO SCU_VR0_Typedef VR0;
    __IO SCU_VR1_Typedef VR1;
    __IO SCU_AFE0_Typedef AFE0;
    __IO SCU_AFE1_Typedef AFE1;
} SCU_TypeDef;

typedef union
{
    struct
    {
        uint32_t PORT_0: 1;
        uint32_t PORT_1: 1;
        uint32_t PORT_2: 1;
        uint32_t PORT_3: 1;
        uint32_t PORT_4: 1;
        uint32_t PORT_5: 1;
        uint32_t PORT_6: 1;
        uint32_t PORT_7: 1;
        uint32_t PORT_8: 1;
        uint32_t PORT_9: 1;
        uint32_t PORT_10: 1;
        uint32_t PORT_11: 1;
        uint32_t PORT_12: 1;
        uint32_t PORT_13: 1;
        uint32_t PORT_14: 1;
        uint32_t PORT_15: 1;
        uint32_t PORT_16: 1;
        uint32_t PORT_17: 1;
        uint32_t PORT_18: 1;
        uint32_t PORT_19: 1;
        uint32_t PORT_20: 1;
        uint32_t PORT_21: 1;
        uint32_t PORT_22: 1;
        uint32_t PORT_23: 1;
        uint32_t PORT_24: 1;
        uint32_t PORT_25: 1;
        uint32_t PORT_26: 1;
        uint32_t PORT_27: 1;
        uint32_t PORT_28: 1;
        uint32_t PORT_29: 1;
        uint32_t PORT_30: 1;
        uint32_t PORT_31: 1;
    };
    uint32_t Word;
} GPIO_PORT_Typedef;

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
} GPIO_DATA_Typedef;

typedef union
{
    struct
    {
        uint32_t DATABSR_0: 1;
        uint32_t DATABSR_1: 1;
        uint32_t DATABSR_2: 1;
        uint32_t DATABSR_3: 1;
        uint32_t DATABSR_4: 1;
        uint32_t DATABSR_5: 1;
        uint32_t DATABSR_6: 1;
        uint32_t DATABSR_7: 1;
        uint32_t DATABSR_8: 1;
        uint32_t DATABSR_9: 1;
        uint32_t DATABSR_10: 1;
        uint32_t DATABSR_11: 1;
        uint32_t DATABSR_12: 1;
        uint32_t DATABSR_13: 1;
        uint32_t DATABSR_14: 1;
        uint32_t DATABSR_15: 1;
        uint32_t DATABSR_16: 1;
        uint32_t DATABSR_17: 1;
        uint32_t DATABSR_18: 1;
        uint32_t DATABSR_19: 1;
        uint32_t DATABSR_20: 1;
        uint32_t DATABSR_21: 1;
        uint32_t DATABSR_22: 1;
        uint32_t DATABSR_23: 1;
        uint32_t DATABSR_24: 1;
        uint32_t DATABSR_25: 1;
        uint32_t DATABSR_26: 1;
        uint32_t DATABSR_27: 1;
        uint32_t DATABSR_28: 1;
        uint32_t DATABSR_29: 1;
        uint32_t DATABSR_30: 1;
        uint32_t DATABSR_31: 1;
    };
    uint32_t Word;
} GPIO_DATABSR_Typedef;

typedef union
{
    struct
    {
        uint32_t DATABCR_0: 1;
        uint32_t DATABCR_1: 1;
        uint32_t DATABCR_2: 1;
        uint32_t DATABCR_3: 1;
        uint32_t DATABCR_4: 1;
        uint32_t DATABCR_5: 1;
        uint32_t DATABCR_6: 1;
        uint32_t DATABCR_7: 1;
        uint32_t DATABCR_8: 1;
        uint32_t DATABCR_9: 1;
        uint32_t DATABCR_10: 1;
        uint32_t DATABCR_11: 1;
        uint32_t DATABCR_12: 1;
        uint32_t DATABCR_13: 1;
        uint32_t DATABCR_14: 1;
        uint32_t DATABCR_15: 1;
        uint32_t DATABCR_16: 1;
        uint32_t DATABCR_17: 1;
        uint32_t DATABCR_18: 1;
        uint32_t DATABCR_19: 1;
        uint32_t DATABCR_20: 1;
        uint32_t DATABCR_21: 1;
        uint32_t DATABCR_22: 1;
        uint32_t DATABCR_23: 1;
        uint32_t DATABCR_24: 1;
        uint32_t DATABCR_25: 1;
        uint32_t DATABCR_26: 1;
        uint32_t DATABCR_27: 1;
        uint32_t DATABCR_28: 1;
        uint32_t DATABCR_29: 1;
        uint32_t DATABCR_30: 1;
        uint32_t DATABCR_31: 1;
    };
    uint32_t Word;
} GPIO_DATABCR_Typedef;

typedef union
{
    struct
    {
        uint32_t DATABRR_0: 1;
        uint32_t DATABRR_1: 1;
        uint32_t DATABRR_2: 1;
        uint32_t DATABRR_3: 1;
        uint32_t DATABRR_4: 1;
        uint32_t DATABRR_5: 1;
        uint32_t DATABRR_6: 1;
        uint32_t DATABRR_7: 1;
        uint32_t DATABRR_8: 1;
        uint32_t DATABRR_9: 1;
        uint32_t DATABRR_10: 1;
        uint32_t DATABRR_11: 1;
        uint32_t DATABRR_12: 1;
        uint32_t DATABRR_13: 1;
        uint32_t DATABRR_14: 1;
        uint32_t DATABRR_15: 1;
        uint32_t DATABRR_16: 1;
        uint32_t DATABRR_17: 1;
        uint32_t DATABRR_18: 1;
        uint32_t DATABRR_19: 1;
        uint32_t DATABRR_20: 1;
        uint32_t DATABRR_21: 1;
        uint32_t DATABRR_22: 1;
        uint32_t DATABRR_23: 1;
        uint32_t DATABRR_24: 1;
        uint32_t DATABRR_25: 1;
        uint32_t DATABRR_26: 1;
        uint32_t DATABRR_27: 1;
        uint32_t DATABRR_28: 1;
        uint32_t DATABRR_29: 1;
        uint32_t DATABRR_30: 1;
        uint32_t DATABRR_31: 1;
    };
    uint32_t Word;
} GPIO_DATABRR_Typedef;

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
} GPIO_DIR_Typedef;

typedef union
{
    struct
    {
        uint32_t DIRBSR_0: 1;
        uint32_t DIRBSR_1: 1;
        uint32_t DIRBSR_2: 1;
        uint32_t DIRBSR_3: 1;
        uint32_t DIRBSR_4: 1;
        uint32_t DIRBSR_5: 1;
        uint32_t DIRBSR_6: 1;
        uint32_t DIRBSR_7: 1;
        uint32_t DIRBSR_8: 1;
        uint32_t DIRBSR_9: 1;
        uint32_t DIRBSR_10: 1;
        uint32_t DIRBSR_11: 1;
        uint32_t DIRBSR_12: 1;
        uint32_t DIRBSR_13: 1;
        uint32_t DIRBSR_14: 1;
        uint32_t DIRBSR_15: 1;
        uint32_t DIRBSR_16: 1;
        uint32_t DIRBSR_17: 1;
        uint32_t DIRBSR_18: 1;
        uint32_t DIRBSR_19: 1;
        uint32_t DIRBSR_20: 1;
        uint32_t DIRBSR_21: 1;
        uint32_t DIRBSR_22: 1;
        uint32_t DIRBSR_23: 1;
        uint32_t DIRBSR_24: 1;
        uint32_t DIRBSR_25: 1;
        uint32_t DIRBSR_26: 1;
        uint32_t DIRBSR_27: 1;
        uint32_t DIRBSR_28: 1;
        uint32_t DIRBSR_29: 1;
        uint32_t DIRBSR_30: 1;
        uint32_t DIRBSR_31: 1;
    };
    uint32_t Word;
} GPIO_DIRBSR_Typedef;

typedef union
{
    struct
    {
        uint32_t DIRBCR_0: 1;
        uint32_t DIRBCR_1: 1;
        uint32_t DIRBCR_2: 1;
        uint32_t DIRBCR_3: 1;
        uint32_t DIRBCR_4: 1;
        uint32_t DIRBCR_5: 1;
        uint32_t DIRBCR_6: 1;
        uint32_t DIRBCR_7: 1;
        uint32_t DIRBCR_8: 1;
        uint32_t DIRBCR_9: 1;
        uint32_t DIRBCR_10: 1;
        uint32_t DIRBCR_11: 1;
        uint32_t DIRBCR_12: 1;
        uint32_t DIRBCR_13: 1;
        uint32_t DIRBCR_14: 1;
        uint32_t DIRBCR_15: 1;
        uint32_t DIRBCR_16: 1;
        uint32_t DIRBCR_17: 1;
        uint32_t DIRBCR_18: 1;
        uint32_t DIRBCR_19: 1;
        uint32_t DIRBCR_20: 1;
        uint32_t DIRBCR_21: 1;
        uint32_t DIRBCR_22: 1;
        uint32_t DIRBCR_23: 1;
        uint32_t DIRBCR_24: 1;
        uint32_t DIRBCR_25: 1;
        uint32_t DIRBCR_26: 1;
        uint32_t DIRBCR_27: 1;
        uint32_t DIRBCR_28: 1;
        uint32_t DIRBCR_29: 1;
        uint32_t DIRBCR_30: 1;
        uint32_t DIRBCR_31: 1;
    };
    uint32_t Word;
} GPIO_DIRBCR_Typedef;

typedef union
{
    struct
    {
        uint32_t DIRBRR_0: 1;
        uint32_t DIRBRR_1: 1;
        uint32_t DIRBRR_2: 1;
        uint32_t DIRBRR_3: 1;
        uint32_t DIRBRR_4: 1;
        uint32_t DIRBRR_5: 1;
        uint32_t DIRBRR_6: 1;
        uint32_t DIRBRR_7: 1;
        uint32_t DIRBRR_8: 1;
        uint32_t DIRBRR_9: 1;
        uint32_t DIRBRR_10: 1;
        uint32_t DIRBRR_11: 1;
        uint32_t DIRBRR_12: 1;
        uint32_t DIRBRR_13: 1;
        uint32_t DIRBRR_14: 1;
        uint32_t DIRBRR_15: 1;
        uint32_t DIRBRR_16: 1;
        uint32_t DIRBRR_17: 1;
        uint32_t DIRBRR_18: 1;
        uint32_t DIRBRR_19: 1;
        uint32_t DIRBRR_20: 1;
        uint32_t DIRBRR_21: 1;
        uint32_t DIRBRR_22: 1;
        uint32_t DIRBRR_23: 1;
        uint32_t DIRBRR_24: 1;
        uint32_t DIRBRR_25: 1;
        uint32_t DIRBRR_26: 1;
        uint32_t DIRBRR_27: 1;
        uint32_t DIRBRR_28: 1;
        uint32_t DIRBRR_29: 1;
        uint32_t DIRBRR_30: 1;
        uint32_t DIRBRR_31: 1;
    };
    uint32_t Word;
} GPIO_DIRBRR_Typedef;

typedef union
{
    struct
    {
        uint32_t GPIO0FUNC: 2;
        uint32_t RESERVED0: 2;
        uint32_t GPIO1FUNC: 2;
        uint32_t RESERVED1: 2;
        uint32_t GPIO2FUNC: 2;
        uint32_t RESERVED2: 2;
        uint32_t GPIO3FUNC: 2;
        uint32_t RESERVED3: 2;
        uint32_t GPIO4FUNC: 2;
        uint32_t RESERVED4: 2;
        uint32_t GPIO5FUNC: 2;
        uint32_t RESERVED5: 2;
        uint32_t GPIO6FUNC: 2;
        uint32_t RESERVED6: 2;
        uint32_t GPIO7FUNC: 2;
        uint32_t RESERVED7: 2;
    };
    uint32_t Word;
} GPIO_FUNC0_Typedef;

typedef union
{
    struct
    {
        uint32_t GPIO8FUNC: 2;
        uint32_t RESERVED0: 2;
        uint32_t GPIO9FUNC: 2;
        uint32_t RESERVED1: 2;
        uint32_t GPIO10FUNC: 2;
        uint32_t RESERVED2: 2;
        uint32_t GPIO11FUNC: 2;
        uint32_t RESERVED3: 2;
        uint32_t GPIO12FUNC: 2;
        uint32_t RESERVED4: 2;
        uint32_t GPIO13FUNC: 2;
        uint32_t RESERVED5: 2;
        uint32_t GPIO14FUNC: 2;
        uint32_t RESERVED6: 2;
        uint32_t GPIO15FUNC: 2;
        uint32_t RESERVED7: 2;
    };
    uint32_t Word;
} GPIO_FUNC1_Typedef;

typedef union
{
    struct
    {
        uint32_t GPIO16FUNC: 2;
        uint32_t RESERVED0: 2;
        uint32_t GPIO17FUNC: 2;
        uint32_t RESERVED1: 2;
        uint32_t GPIO18FUNC: 2;
        uint32_t RESERVED2: 2;
        uint32_t GPIO19FUNC: 2;
        uint32_t RESERVED3: 2;
        uint32_t GPIO20FUNC: 2;
        uint32_t RESERVED4: 2;
        uint32_t GPIO21FUNC: 2;
        uint32_t RESERVED5: 2;
        uint32_t GPIO22FUNC: 2;
        uint32_t RESERVED6: 2;
        uint32_t GPIO23FUNC: 2;
        uint32_t RESERVED7: 2;
    };
    uint32_t Word;
} GPIO_FUNC2_Typedef;

typedef union
{
    struct
    {
        uint32_t GPIO24FUNC: 2;
        uint32_t RESERVED0: 2;
        uint32_t GPIO25FUNC: 2;
        uint32_t RESERVED1: 2;
        uint32_t GPIO26FUNC: 2;
        uint32_t RESERVED2: 2;
        uint32_t GPIO27FUNC: 2;
        uint32_t RESERVED3: 2;
        uint32_t GPIO28FUNC: 2;
        uint32_t RESERVED4: 2;
        uint32_t GPIO29FUNC: 2;
        uint32_t RESERVED5: 2;
        uint32_t GPIO30FUNC: 2;
        uint32_t RESERVED6: 2;
        uint32_t GPIO31FUNC: 2;
        uint32_t RESERVED7: 2;
    };
    uint32_t Word;
} GPIO_FUNC3_Typedef;

typedef union
{
    struct
    {
        uint32_t INEB_0: 1;
        uint32_t INEB_1: 1;
        uint32_t INEB_2: 1;
        uint32_t INEB_3: 1;
        uint32_t INEB_4: 1;
        uint32_t INEB_5: 1;
        uint32_t INEB_6: 1;
        uint32_t INEB_7: 1;
        uint32_t INEB_8: 1;
        uint32_t INEB_9: 1;
        uint32_t INEB_10: 1;
        uint32_t INEB_11: 1;
        uint32_t INEB_12: 1;
        uint32_t INEB_13: 1;
        uint32_t INEB_14: 1;
        uint32_t INEB_15: 1;
        uint32_t INEB_16: 1;
        uint32_t INEB_17: 1;
        uint32_t INEB_18: 1;
        uint32_t INEB_19: 1;
        uint32_t INEB_20: 1;
        uint32_t INEB_21: 1;
        uint32_t INEB_22: 1;
        uint32_t INEB_23: 1;
        uint32_t INEB_24: 1;
        uint32_t INEB_25: 1;
        uint32_t INEB_26: 1;
        uint32_t INEB_27: 1;
        uint32_t INEB_28: 1;
        uint32_t INEB_29: 1;
        uint32_t INEB_30: 1;
        uint32_t INEB_31: 1;
    };
    uint32_t Word;
} GPIO_INEB_Typedef;

typedef union
{
    struct
    {
        uint32_t ODE_0: 1;
        uint32_t ODE_1: 1;
        uint32_t ODE_2: 1;
        uint32_t ODE_3: 1;
        uint32_t ODE_4: 1;
        uint32_t ODE_5: 1;
        uint32_t ODE_6: 1;
        uint32_t ODE_7: 1;
        uint32_t ODE_8: 1;
        uint32_t ODE_9: 1;
        uint32_t ODE_10: 1;
        uint32_t ODE_11: 1;
        uint32_t ODE_12: 1;
        uint32_t ODE_13: 1;
        uint32_t ODE_14: 1;
        uint32_t ODE_15: 1;
        uint32_t ODE_16: 1;
        uint32_t ODE_17: 1;
        uint32_t ODE_18: 1;
        uint32_t ODE_19: 1;
        uint32_t ODE_20: 1;
        uint32_t ODE_21: 1;
        uint32_t ODE_22: 1;
        uint32_t ODE_23: 1;
        uint32_t ODE_24: 1;
        uint32_t ODE_25: 1;
        uint32_t ODE_26: 1;
        uint32_t ODE_27: 1;
        uint32_t ODE_28: 1;
        uint32_t ODE_29: 1;
        uint32_t ODE_30: 1;
        uint32_t ODE_31: 1;
    };
    uint32_t Word;
} GPIO_ODE_Typedef;

typedef union
{
    struct
    {
        uint32_t PUE_0: 1;
        uint32_t PUE_1: 1;
        uint32_t PUE_2: 1;
        uint32_t PUE_3: 1;
        uint32_t PUE_4: 1;
        uint32_t PUE_5: 1;
        uint32_t PUE_6: 1;
        uint32_t PUE_7: 1;
        uint32_t PUE_8: 1;
        uint32_t PUE_9: 1;
        uint32_t PUE_10: 1;
        uint32_t PUE_11: 1;
        uint32_t PUE_12: 1;
        uint32_t PUE_13: 1;
        uint32_t PUE_14: 1;
        uint32_t PUE_15: 1;
        uint32_t PUE_16: 1;
        uint32_t PUE_17: 1;
        uint32_t PUE_18: 1;
        uint32_t PUE_19: 1;
        uint32_t PUE_20: 1;
        uint32_t PUE_21: 1;
        uint32_t PUE_22: 1;
        uint32_t PUE_23: 1;
        uint32_t PUE_24: 1;
        uint32_t PUE_25: 1;
        uint32_t PUE_26: 1;
        uint32_t PUE_27: 1;
        uint32_t PUE_28: 1;
        uint32_t PUE_29: 1;
        uint32_t PUE_30: 1;
        uint32_t PUE_31: 1;
    };
    uint32_t Word;
} GPIO_PUE_Typedef;

typedef union
{
    struct
    {
        uint32_t PDE_0: 1;
        uint32_t PDE_1: 1;
        uint32_t PDE_2: 1;
        uint32_t PDE_3: 1;
        uint32_t PDE_4: 1;
        uint32_t PDE_5: 1;
        uint32_t PDE_6: 1;
        uint32_t PDE_7: 1;
        uint32_t PDE_8: 1;
        uint32_t PDE_9: 1;
        uint32_t PDE_10: 1;
        uint32_t PDE_11: 1;
        uint32_t PDE_12: 1;
        uint32_t PDE_13: 1;
        uint32_t PDE_14: 1;
        uint32_t PDE_15: 1;
        uint32_t PDE_16: 1;
        uint32_t PDE_17: 1;
        uint32_t PDE_18: 1;
        uint32_t PDE_19: 1;
        uint32_t PDE_20: 1;
        uint32_t PDE_21: 1;
        uint32_t PDE_22: 1;
        uint32_t PDE_23: 1;
        uint32_t PDE_24: 1;
        uint32_t PDE_25: 1;
        uint32_t PDE_26: 1;
        uint32_t PDE_27: 1;
        uint32_t PDE_28: 1;
        uint32_t PDE_29: 1;
        uint32_t PDE_30: 1;
        uint32_t PDE_31: 1;
    };
    uint32_t Word;
} GPIO_PDE_Typedef;

typedef union
{
    struct
    {
        uint32_t DS_0: 1;
        uint32_t DS_1: 1;
        uint32_t DS_2: 1;
        uint32_t DS_3: 1;
        uint32_t DS_4: 1;
        uint32_t DS_5: 1;
        uint32_t DS_6: 1;
        uint32_t DS_7: 1;
        uint32_t DS_8: 1;
        uint32_t DS_9: 1;
        uint32_t DS_10: 1;
        uint32_t DS_11: 1;
        uint32_t DS_12: 1;
        uint32_t DS_13: 1;
        uint32_t DS_14: 1;
        uint32_t DS_15: 1;
        uint32_t DS_16: 1;
        uint32_t DS_17: 1;
        uint32_t DS_18: 1;
        uint32_t DS_19: 1;
        uint32_t DS_20: 1;
        uint32_t DS_21: 1;
        uint32_t DS_22: 1;
        uint32_t DS_23: 1;
        uint32_t DS_24: 1;
        uint32_t DS_25: 1;
        uint32_t DS_26: 1;
        uint32_t DS_27: 1;
        uint32_t DS_28: 1;
        uint32_t DS_29: 1;
        uint32_t DS_30: 1;
        uint32_t DS_31: 1;
    };
    uint32_t Word;
} GPIO_DS_Typedef;

typedef union
{
    struct
    {
        uint32_t PINTIE: 8;
        uint32_t PMASK: 8;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} GPIO_PINTIE_Typedef;

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
} GPIO_PINTIF_Typedef;

typedef union
{
    struct
    {
        uint32_t PINT0_SEL: 3;
        uint32_t RESERVED0: 1;
        uint32_t PINT1_SEL: 3;
        uint32_t RESERVED1: 1;
        uint32_t PINT2_SEL: 3;
        uint32_t RESERVED2: 1;
        uint32_t PINT3_SEL: 3;
        uint32_t RESERVED3: 1;
        uint32_t PINT4_SEL: 3;
        uint32_t RESERVED4: 1;
        uint32_t PINT5_SEL: 3;
        uint32_t RESERVED5: 1;
        uint32_t PINT6_SEL: 3;
        uint32_t RESERVED6: 1;
        uint32_t PINT7_SEL: 3;
        uint32_t RESERVED7: 1;
    };
    uint32_t Word;
} GPIO_PINTSEL_Typedef;

typedef union
{
    struct
    {
        uint32_t PINT0_CFG: 3;
        uint32_t RESERVED0: 1;
        uint32_t PINT1_CFG: 3;
        uint32_t RESERVED1: 1;
        uint32_t PINT2_CFG: 3;
        uint32_t RESERVED2: 1;
        uint32_t PINT3_CFG: 3;
        uint32_t RESERVED3: 1;
        uint32_t PINT4_CFG: 3;
        uint32_t RESERVED4: 1;
        uint32_t PINT5_CFG: 3;
        uint32_t RESERVED5: 1;
        uint32_t PINT6_CFG: 3;
        uint32_t RESERVED6: 1;
        uint32_t PINT7_CFG: 3;
        uint32_t RESERVED7: 1;
    };
    uint32_t Word;
} GPIO_PINTCFG_Typedef;

typedef union
{
    struct
    {
        uint32_t KINTIE: 8;
        uint32_t KMASK: 8;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} GPIO_KINTIE_Typedef;

typedef union
{
    struct
    {
        uint32_t KINTIF_0: 1;
        uint32_t KINTIF_1: 1;
        uint32_t KINTIF_2: 1;
        uint32_t KINTIF_3: 1;
        uint32_t KINTIF_4: 1;
        uint32_t KINTIF_5: 1;
        uint32_t KINTIF_6: 1;
        uint32_t KINTIF_7: 1;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} GPIO_KINTIF_Typedef;

typedef union
{
    struct
    {
        uint32_t KINT0_SEL: 3;
        uint32_t RESERVED0: 1;
        uint32_t KINT1_SEL: 3;
        uint32_t RESERVED1: 1;
        uint32_t KINT2_SEL: 3;
        uint32_t RESERVED2: 1;
        uint32_t KINT3_SEL: 3;
        uint32_t RESERVED3: 1;
        uint32_t KINT4_SEL: 3;
        uint32_t RESERVED4: 1;
        uint32_t KINT5_SEL: 3;
        uint32_t RESERVED5: 1;
        uint32_t KINT6_SEL: 3;
        uint32_t RESERVED6: 1;
        uint32_t KINT7_SEL: 3;
        uint32_t RESERVED7: 1;
    };
    uint32_t Word;
} GPIO_KINTSEL_Typedef;

typedef union
{
    struct
    {
        uint32_t KINT0_CFG: 3;
        uint32_t RESERVED0: 1;
        uint32_t KINT1_CFG: 3;
        uint32_t RESERVED1: 1;
        uint32_t KINT2_CFG: 3;
        uint32_t RESERVED2: 1;
        uint32_t KINT3_CFG: 3;
        uint32_t RESERVED3: 1;
        uint32_t KINT4_CFG: 3;
        uint32_t RESERVED4: 1;
        uint32_t KINT5_CFG: 3;
        uint32_t RESERVED5: 1;
        uint32_t KINT6_CFG: 3;
        uint32_t RESERVED6: 1;
        uint32_t KINT7_CFG: 3;
        uint32_t RESERVED7: 1;
    };
    uint32_t Word;
} GPIO_KINTCFG_Typedef;

typedef union
{
    struct
    {
        uint32_t FLT_SEL_0: 1;
        uint32_t FLT_SEL_1: 1;
        uint32_t FLT_SEL_2: 1;
        uint32_t FLT_SEL_3: 1;
        uint32_t FLT_SEL_4: 1;
        uint32_t FLT_SEL_5: 1;
        uint32_t FLT_SEL_6: 1;
        uint32_t FLT_SEL_7: 1;
        uint32_t FLT_SEL_8: 1;
        uint32_t FLT_SEL_9: 1;
        uint32_t FLT_SEL_10: 1;
        uint32_t FLT_SEL_11: 1;
        uint32_t FLT_SEL_12: 1;
        uint32_t FLT_SEL_13: 1;
        uint32_t FLT_SEL_14: 1;
        uint32_t FLT_SEL_15: 1;
        uint32_t FLT_SEL_16: 1;
        uint32_t FLT_SEL_17: 1;
        uint32_t FLT_SEL_18: 1;
        uint32_t FLT_SEL_19: 1;
        uint32_t FLT_SEL_20: 1;
        uint32_t FLT_SEL_21: 1;
        uint32_t FLT_SEL_22: 1;
        uint32_t FLT_SEL_23: 1;
        uint32_t FLT_SEL_24: 1;
        uint32_t FLT_SEL_25: 1;
        uint32_t FLT_SEL_26: 1;
        uint32_t FLT_SEL_27: 1;
        uint32_t FLT_SEL_28: 1;
        uint32_t FLT_SEL_29: 1;
        uint32_t FLT_SEL_30: 1;
        uint32_t FLT_SEL_31: 1;
    };
    uint32_t Word;
} GPIO_IOFLTSEL_Typedef;

typedef union
{
    struct
    {
        uint32_t TX0_PLV: 1;
        uint32_t RESERVED0: 1;
        uint32_t TX0_S0: 1;
        uint32_t TX0_S1: 1;
        uint32_t TX0_S2: 1;
        uint32_t TX0_S3: 1;
        uint32_t TX1PS: 2;
        uint32_t TX0PS: 2;
        uint32_t TX1_PLV: 1;
        uint32_t RESERVED1: 2;
        uint32_t TX1_S1: 1;
        uint32_t TX1_S2: 1;
        uint32_t TX1_S3: 1;
        uint32_t RESERVED2: 16;
    };
    uint32_t Word;
} GPIO_TXPWM_Typedef;

typedef union
{
    struct
    {
        uint32_t BUZEN: 1;
        uint32_t RESERVED0: 7;
        uint32_t BUZ_LOAD: 20;
        uint32_t RESERVED1: 4;
    };
    uint32_t Word;
} GPIO_BUZC_Typedef;

typedef struct
{
    __I GPIO_PORT_Typedef PORT;
    __IO GPIO_DATA_Typedef DATA;
    __O GPIO_DATABSR_Typedef DATABSR;
    __O GPIO_DATABCR_Typedef DATABCR;
    __O GPIO_DATABRR_Typedef DATABRR;
    __IO GPIO_DIR_Typedef DIR;
    __O GPIO_DIRBSR_Typedef DIRBSR;
    __O GPIO_DIRBCR_Typedef DIRBCR;
    __O GPIO_DIRBRR_Typedef DIRBRR;
    __IO GPIO_FUNC0_Typedef FUNC0;
    __IO GPIO_FUNC1_Typedef FUNC1;
    __IO GPIO_FUNC2_Typedef FUNC2;
    __IO GPIO_FUNC3_Typedef FUNC3;
    __IO GPIO_INEB_Typedef INEB;
    __IO GPIO_ODE_Typedef ODE;
    __IO GPIO_PUE_Typedef PUE;
    __IO GPIO_PDE_Typedef PDE;
    __IO GPIO_DS_Typedef DS;
    uint32_t RESERVED0[174] ;
    __IO GPIO_PINTIE_Typedef PINTIE;
    __IO GPIO_PINTIF_Typedef PINTIF;
    __IO GPIO_PINTSEL_Typedef PINTSEL;
    __IO GPIO_PINTCFG_Typedef PINTCFG;
    __IO GPIO_KINTIE_Typedef KINTIE;
    __IO GPIO_KINTIF_Typedef KINTIF;
    __IO GPIO_KINTSEL_Typedef KINTSEL;
    __IO GPIO_KINTCFG_Typedef KINTCFG;
    uint32_t RESERVED1[4] ;
    __IO GPIO_IOFLTSEL_Typedef IOFLTSEL;
    uint32_t RESERVED2[19] ;
    __IO GPIO_TXPWM_Typedef TXPWM;
    __IO GPIO_BUZC_Typedef BUZC;
} GPIO_TypeDef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t RST: 1;
        uint32_t FLASH_ACK: 1;
        uint32_t FLASH_FAIL: 1;
        uint32_t IAPCKS: 3;
        uint32_t RESERVED0: 1;
        uint32_t IAPENC_ST: 1;
        uint32_t IAPENC_ED: 1;
        uint32_t IAPENC_EN: 1;
        uint32_t EFR_ERR: 2;
        uint32_t RESERVED1: 19;
    };
    uint32_t Word;
} IAP_CON_Typedef;

typedef union
{
    struct
    {
        uint32_t IAPCA: 13;
        uint32_t IAPPA: 7;
        uint32_t IFREN: 1;
        uint32_t RESERVED0: 11;
    };
    uint32_t Word;
} IAP_ADDR_Typedef;

typedef union
{
    struct
    {
        uint32_t TRIG: 32;
    };
    uint32_t Word;
} IAP_TRIG_Typedef;

typedef union
{
    struct
    {
        uint32_t UL: 32;
    };
    uint32_t Word;
} IAP_UL_Typedef;

typedef union
{
    struct
    {
        uint32_t BSY: 1;
        uint32_t PERASE_END: 1;
        uint32_t WPROG_END: 1;
        uint32_t PPROGS_END: 1;
        uint32_t PPROGD_END: 1;
        uint32_t PPROGE_END: 1;
        uint32_t MERASE_END: 1;
        uint32_t RESERVED0: 25;
    };
    uint32_t Word;
} IAP_STA_Typedef;

typedef union
{
    struct
    {
        uint32_t DATA: 32;
    };
    uint32_t Word;
} IAP_DATA0_Typedef;

typedef union
{
    struct
    {
        uint32_t DATA: 32;
    };
    uint32_t Word;
} IAP_DATA1_Typedef;

typedef union
{
    struct
    {
        uint32_t DATA: 32;
    };
    uint32_t Word;
} IAP_DATA3_Typedef;

typedef union
{
    struct
    {
        uint32_t ENC_DATA0: 8;
        uint32_t ENC_DATA1: 8;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} IAP_ENC_DATA_Typedef;

typedef struct
{
    __IO IAP_CON_Typedef CON;
    __IO IAP_ADDR_Typedef ADDR;
    uint32_t RESERVED0 ;
    __IO IAP_TRIG_Typedef TRIG;
    __IO IAP_UL_Typedef UL;
    __I IAP_STA_Typedef STA;
    __IO IAP_DATA0_Typedef DATA0;
    __IO IAP_DATA1_Typedef DATA1;
    uint32_t RESERVED1 ;
    __IO IAP_DATA3_Typedef DATA3;
} IAP_TypeDef;

typedef union
{
    struct
    {
        uint32_t DR: 12;
        uint32_t RESERVED0: 20;
    };
    uint32_t Word;
} ADC_DR_Typedef;

typedef union
{
    struct
    {
        uint32_t ADC_EN: 1;
        uint32_t ADC_TRIG: 1;
        uint32_t RESERVED0: 1;
        uint32_t RESERVED1: 29;
    };
    uint32_t Word;
} ADC_CON0_Typedef;

typedef union
{
    struct
    {
        uint32_t CLKDIV: 3;
        uint32_t CLKS: 1;
        uint32_t AD_DIFFMODE: 1;
        uint32_t AD_STIME: 2;
        uint32_t RESERVED0: 1;
        uint32_t ADR_LP: 1;
        uint32_t ADR_VCM_EN: 1;
        uint32_t ADR_VREF_EN: 1;
        uint32_t ADR_BP: 1;
        uint32_t ADR_VRSEL: 3;
        uint32_t RESERVED1: 1;
        uint32_t RESERVED2: 16;
    };
    uint32_t Word;
} ADC_CON1_Typedef;

typedef union
{
    struct
    {
        uint32_t ADCHS: 12;
        uint32_t RESERVED0: 20;
    };
    uint32_t Word;
} ADC_CHS_Typedef;

typedef union
{
    struct
    {
        uint32_t ADIF: 1;
        uint32_t ADIE: 1;
        uint32_t RESERVED0: 30;
    };
    uint32_t Word;
} ADC_INT_Typedef;

typedef struct
{
    __I ADC_DR_Typedef DR;
    __IO ADC_CON0_Typedef CON0;
    __IO ADC_CON1_Typedef CON1;
    __IO ADC_CHS_Typedef CHS;
    __IO ADC_INT_Typedef INT;
} ADC_TypeDef;

typedef union
{
    struct
    {
        uint32_t LOAD: 32;
    };
    uint32_t Word;
} IWDT_LOAD_Typedef;

typedef union
{
    struct
    {
        uint32_t VALUE: 32;
    };
    uint32_t Word;
} IWDT_VALUE_Typedef;

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
} IWDT_CON_Typedef;

typedef union
{
    struct
    {
        uint32_t INTCLR: 32;
    };
    uint32_t Word;
} IWDT_INTCLR_Typedef;

typedef union
{
    struct
    {
        uint32_t WDTIF: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} IWDT_RIS_Typedef;

typedef union
{
    struct
    {
        uint32_t LOCK: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} IWDT_LOCK_Typedef;

typedef struct
{
    __IO IWDT_LOAD_Typedef LOAD;
    __I IWDT_VALUE_Typedef VALUE;
    __IO IWDT_CON_Typedef CON;
    __IO IWDT_INTCLR_Typedef INTCLR;
    __I IWDT_RIS_Typedef RIS;
    uint32_t RESERVED0[59] ;
    __IO IWDT_LOCK_Typedef LOCK;
} IWDT_TypeDef;

typedef union
{
    struct
    {
        uint32_t LOAD: 32;
    };
    uint32_t Word;
} WWDT_LOAD_Typedef;

typedef union
{
    struct
    {
        uint32_t VALUE: 32;
    };
    uint32_t Word;
} WWDT_VALUE_Typedef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t IE: 1;
        uint32_t RSTEN: 1;
        uint32_t CLKS: 1;
        uint32_t WWDTWIN: 2;
        uint32_t RESERVED0: 26;
    };
    uint32_t Word;
} WWDT_CON_Typedef;

typedef union
{
    struct
    {
        uint32_t INTCLR: 32;
    };
    uint32_t Word;
} WWDT_INTCLR_Typedef;

typedef union
{
    struct
    {
        uint32_t WDTIF: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} WWDT_RIS_Typedef;

typedef union
{
    struct
    {
        uint32_t LOCK: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} WWDT_LOCK_Typedef;

typedef struct
{
    __IO WWDT_LOAD_Typedef LOAD;
    __I WWDT_VALUE_Typedef VALUE;
    __IO WWDT_CON_Typedef CON;
    __IO WWDT_INTCLR_Typedef INTCLR;
    __I WWDT_RIS_Typedef RIS;
    uint32_t RESERVED0[59] ;
    __IO WWDT_LOCK_Typedef LOCK;
} WWDT_TypeDef;

typedef union
{
    struct
    {
        uint32_t CNT0: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} T16N_CNT0_Typedef;

typedef union
{
    struct
    {
        uint32_t CNT1: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} T16N_CNT1_Typedef;

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
        uint32_t CAPL0: 1;
        uint32_t CAPL1: 1;
        uint32_t RESERVED0: 22;
    };
    uint32_t Word;
} T16N_CON1_Typedef;

typedef union
{
    struct
    {
        uint32_t MOE0: 1;
        uint32_t MOE1: 1;
        uint32_t POL0: 1;
        uint32_t POL1: 1;
        uint32_t PWMMOD: 2;
        uint32_t PWMDZE: 1;
        uint32_t RESERVED0: 1;
        uint32_t MOM0: 2;
        uint32_t MOM1: 2;
        uint32_t MOM2: 2;
        uint32_t MOM3: 2;
        uint32_t PWMBKE0: 1;
        uint32_t PWMBKE1: 1;
        uint32_t PWMBKL0: 1;
        uint32_t PWMBKL1: 1;
        uint32_t PWMBKS0: 1;
        uint32_t PWMBKS1: 1;
        uint32_t PWMBKP0: 1;
        uint32_t PWMBKP1: 1;
        uint32_t PWMBKF: 1;
        uint32_t RESERVED1: 7;
    };
    uint32_t Word;
} T16N_CON2_Typedef;

typedef union
{
    struct
    {
        uint32_t MAT0IE: 1;
        uint32_t MAT1IE: 1;
        uint32_t MAT2IE: 1;
        uint32_t MAT3IE: 1;
        uint32_t TOP0IE: 1;
        uint32_t TOP1IE: 1;
        uint32_t CAP0IE: 1;
        uint32_t CAP1IE: 1;
        uint32_t PBK0IE: 1;
        uint32_t PBK1IE: 1;
        uint32_t RESERVED0: 22;
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
        uint32_t TOP0IF: 1;
        uint32_t TOP1IF: 1;
        uint32_t CAP0IF: 1;
        uint32_t CAP1IF: 1;
        uint32_t PBK0IF: 1;
        uint32_t PBK1IF: 1;
        uint32_t RESERVED0: 22;
    };
    uint32_t Word;
} T16N_IF_Typedef;

typedef union
{
    struct
    {
        uint32_t PDZ: 8;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} T16N_PDZ_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 1;
        uint32_t P0MAT0TRE: 1;
        uint32_t P0MAT1TRE: 1;
        uint32_t P0TOP0TRE: 1;
        uint32_t RESERVED1: 1;
        uint32_t P1MAT2TRE: 1;
        uint32_t P1MAT3TRE: 1;
        uint32_t P1TOP1TRE: 1;
        uint32_t RESERVED2: 24;
    };
    uint32_t Word;
} T16N_PTR_Typedef;

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

typedef union
{
    struct
    {
        uint32_t TOP0: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} T16N_TOP0_Typedef;

typedef union
{
    struct
    {
        uint32_t TOP1: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} T16N_TOP1_Typedef;

typedef struct
{
    __IO T16N_CNT0_Typedef CNT0;
    __IO T16N_CNT1_Typedef CNT1;
    __IO T16N_PRECNT_Typedef PRECNT;
    __IO T16N_PREMAT_Typedef PREMAT;
    __IO T16N_CON0_Typedef CON0;
    __IO T16N_CON1_Typedef CON1;
    __IO T16N_CON2_Typedef CON2;
    uint32_t RESERVED0 ;
    __IO T16N_IE_Typedef IE;
    __IO T16N_IF_Typedef IF;
    __IO T16N_PDZ_Typedef PDZ;
    __IO T16N_PTR_Typedef PTR;
    __IO T16N_MAT0_Typedef MAT0;
    __IO T16N_MAT1_Typedef MAT1;
    __IO T16N_MAT2_Typedef MAT2;
    __IO T16N_MAT3_Typedef MAT3;
    __IO T16N_TOP0_Typedef TOP0;
    __IO T16N_TOP1_Typedef TOP1;
} T16N_TypeDef;

typedef union
{
    struct
    {
        uint32_t CNT0: 32;
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
        uint32_t ASYNC_WREN: 1;
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
        uint32_t CAPL0: 1;
        uint32_t CAPL1: 1;
        uint32_t RESERVED0: 6;
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
        uint32_t TBCLR: 1;
        uint32_t TXI: 1;
        uint32_t RESERVED0: 4;
        uint32_t TXMOD: 4;
        uint32_t TXP: 1;
        uint32_t TXFS: 1;
        uint32_t RESERVED1: 2;
        uint32_t RXEN: 1;
        uint32_t RRST: 1;
        uint32_t RBCLR: 1;
        uint32_t RXI: 1;
        uint32_t BDEN: 1;
        uint32_t IDEN: 1;
        uint32_t RESERVED2: 2;
        uint32_t RXMOD: 4;
        uint32_t RXP: 1;
        uint32_t RESERVED3: 3;
    };
    uint32_t Word;
} UART_CON0_Typedef;

typedef union
{
    struct
    {
        uint32_t TBIM: 2;
        uint32_t RESERVED0: 2;
        uint32_t RBIM: 2;
        uint32_t RESERVED1: 2;
        uint32_t BCS: 3;
        uint32_t RESERVED2: 1;
        uint32_t BDM: 2;
        uint32_t RESERVED3: 2;
        uint32_t IDM: 2;
        uint32_t RESERVED4: 14;
    };
    uint32_t Word;
} UART_CON1_Typedef;

typedef union
{
    struct
    {
        uint32_t BRFRA: 4;
        uint32_t BRINT: 12;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} UART_BRR_Typedef;

typedef union
{
    struct
    {
        uint32_t TBPTR: 4;
        uint32_t TBOV: 1;
        uint32_t TXBUSY: 1;
        uint32_t RESERVED0: 2;
        uint32_t RBPTR: 4;
        uint32_t RBOV: 1;
        uint32_t RXBUSY: 1;
        uint32_t RESERVED1: 2;
        uint32_t FER0: 1;
        uint32_t PER0: 1;
        uint32_t FER1: 1;
        uint32_t PER1: 1;
        uint32_t FER2: 1;
        uint32_t PER2: 1;
        uint32_t FER3: 1;
        uint32_t PER3: 1;
        uint32_t RESERVED2: 8;
    };
    uint32_t Word;
} UART_STA_Typedef;

typedef union
{
    struct
    {
        uint32_t TBIE: 1;
        uint32_t TCIE: 1;
        uint32_t RESERVED0: 6;
        uint32_t TBWEIE: 1;
        uint32_t TBWOIE: 1;
        uint32_t RESERVED1: 6;
        uint32_t RBIE: 1;
        uint32_t IDIE: 1;
        uint32_t RESERVED2: 6;
        uint32_t ROIE: 1;
        uint32_t FEIE: 1;
        uint32_t PEIE: 1;
        uint32_t BDEIE: 1;
        uint32_t RBREIE: 1;
        uint32_t RBROIE: 1;
        uint32_t RESERVED3: 2;
    };
    uint32_t Word;
} UART_IE_Typedef;

typedef union
{
    struct
    {
        uint32_t TBIF: 1;
        uint32_t TCIF: 1;
        uint32_t RESERVED0: 6;
        uint32_t TBWEIF: 1;
        uint32_t TBWOIF: 1;
        uint32_t RESERVED1: 6;
        uint32_t RBIF: 1;
        uint32_t IDIF: 1;
        uint32_t RESERVED2: 6;
        uint32_t ROIF: 1;
        uint32_t FEIF: 1;
        uint32_t PEIF: 1;
        uint32_t BDEIF: 1;
        uint32_t RBREIF: 1;
        uint32_t RBROIF: 1;
        uint32_t RESERVED3: 2;
    };
    uint32_t Word;
} UART_IF_Typedef;

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
        uint32_t TBFF0: 1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} UART_TB0_Typedef;

typedef union
{
    struct
    {
        uint32_t TB1: 9;
        uint32_t RESERVED0: 3;
        uint32_t TP1: 1;
        uint32_t TBFF1: 1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} UART_TB1_Typedef;

typedef union
{
    struct
    {
        uint32_t TB2: 9;
        uint32_t RESERVED0: 3;
        uint32_t TP2: 1;
        uint32_t TBFF2: 1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} UART_TB2_Typedef;

typedef union
{
    struct
    {
        uint32_t TB3: 9;
        uint32_t RESERVED0: 3;
        uint32_t TP3: 1;
        uint32_t TBFF3: 1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} UART_TB3_Typedef;

typedef union
{
    struct
    {
        uint32_t TB4: 9;
        uint32_t RESERVED0: 3;
        uint32_t TP4: 1;
        uint32_t TBFF4: 1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} UART_TB4_Typedef;

typedef union
{
    struct
    {
        uint32_t TB5: 9;
        uint32_t RESERVED0: 3;
        uint32_t TP5: 1;
        uint32_t TBFF5: 1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} UART_TB5_Typedef;

typedef union
{
    struct
    {
        uint32_t TB6: 9;
        uint32_t RESERVED0: 3;
        uint32_t TP6: 1;
        uint32_t TBFF6: 1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} UART_TB6_Typedef;

typedef union
{
    struct
    {
        uint32_t TB7: 9;
        uint32_t RESERVED0: 3;
        uint32_t TP7: 1;
        uint32_t TBFF7: 1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} UART_TB7_Typedef;

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
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} UART_RB0_Typedef;

typedef union
{
    struct
    {
        uint32_t RB1: 9;
        uint32_t RESERVED0: 3;
        uint32_t RP1: 1;
        uint32_t RBFF1: 1;
        uint32_t FE1: 1;
        uint32_t PE1: 1;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} UART_RB1_Typedef;

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
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} UART_RB2_Typedef;

typedef union
{
    struct
    {
        uint32_t RB3: 9;
        uint32_t RESERVED0: 3;
        uint32_t RP3: 1;
        uint32_t RBFF3: 1;
        uint32_t FE3: 1;
        uint32_t PE3: 1;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} UART_RB3_Typedef;

typedef union
{
    struct
    {
        uint32_t RB4: 9;
        uint32_t RESERVED0: 3;
        uint32_t RP4: 1;
        uint32_t RBFF4: 1;
        uint32_t FE4: 1;
        uint32_t PE4: 1;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} UART_RB4_Typedef;

typedef union
{
    struct
    {
        uint32_t RB5: 9;
        uint32_t RESERVED0: 3;
        uint32_t RP5: 1;
        uint32_t RBFF5: 1;
        uint32_t FE5: 1;
        uint32_t PE5: 1;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} UART_RB5_Typedef;

typedef union
{
    struct
    {
        uint32_t RB6: 9;
        uint32_t RESERVED0: 3;
        uint32_t RP6: 1;
        uint32_t RBFF6: 1;
        uint32_t FE6: 1;
        uint32_t PE6: 1;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} UART_RB6_Typedef;

typedef union
{
    struct
    {
        uint32_t RB7: 9;
        uint32_t RESERVED0: 3;
        uint32_t RP7: 1;
        uint32_t RBFF7: 1;
        uint32_t FE7: 1;
        uint32_t PE7: 1;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} UART_RB7_Typedef;
typedef union
{
    struct
    {
        uint32_t TXDMAEN:1;
        uint32_t RXDMAEN:1;
        uint32_t DMAONERR:1;
        uint32_t RESERVENEN1:29;
    };
    uint32_t Word;
}
UART_DMACR_Typedef;


typedef struct
{
    __IO UART_CON0_Typedef CON0;
    __IO UART_CON1_Typedef CON1;
    __IO UART_DMACR_Typedef DMACR;
    uint32_t RESERVED0[1] ;
    __IO UART_BRR_Typedef BRR;
    __I UART_STA_Typedef STA;
    __IO UART_IE_Typedef IE;
    __IO UART_IF_Typedef IF;
    __O UART_TBW_Typedef TBW;
    __I UART_RBR_Typedef RBR;
    uint32_t RESERVED1[6] ;
    __I UART_TB0_Typedef TB0;
    __I UART_TB1_Typedef TB1;
    __I UART_TB2_Typedef TB2;
    __I UART_TB3_Typedef TB3;
    __I UART_TB4_Typedef TB4;
    __I UART_TB5_Typedef TB5;
    __I UART_TB6_Typedef TB6;
    __I UART_TB7_Typedef TB7;
    __I UART_RB0_Typedef RB0;
    __I UART_RB1_Typedef RB1;
    __I UART_RB2_Typedef RB2;
    __I UART_RB3_Typedef RB3;
    __I UART_RB4_Typedef RB4;
    __I UART_RB5_Typedef RB5;
    __I UART_RB6_Typedef RB6;
    __I UART_RB7_Typedef RB7;
} UART_TypeDef;

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
        uint32_t TXDMAEN: 1;
        uint32_t RXDMAEN: 1;
        uint32_t RESERVED1: 6;
        uint32_t TME: 1;
        uint32_t TMS: 1;
        uint32_t TMP: 6;
        uint32_t DW: 3;
        uint32_t RESERVED2: 3;
        uint32_t TXCLR: 1;
        uint32_t RXCLR: 1;
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
        uint32_t TBFF0: 1;
        uint32_t TBFF1: 1;
        uint32_t TBFF2: 1;
        uint32_t TBFF3: 1;
        uint32_t RBFF0: 1;
        uint32_t RBFF1: 1;
        uint32_t RBFF2: 1;
        uint32_t RBFF3: 1;
        uint32_t IDLE: 1;
        uint32_t RESERVED1: 15;
    };
    uint32_t Word;
} SPI_STA_Typedef;

typedef union
{
    struct
    {
        uint32_t CKS: 10;
        uint32_t RESERVED0: 22;
    };
    uint32_t Word;
} SPI_CKS_Typedef;

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
    __IO SPI_CKS_Typedef CKS;
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
        uint32_t TXDMAEN: 1;
        uint32_t RXDMAEN: 1;
        uint32_t RESERVED1: 6;
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
        uint32_t TBWEIE: 1;
        uint32_t TBIM: 2;
        uint32_t RBIM: 2;
        uint32_t TIDLEIE: 1;
        uint32_t RESERVED0: 19;
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
        uint32_t TBWEIF: 1;
        uint32_t RESERVED0: 4;
        uint32_t TIDLEIF: 1;
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

typedef union
{
    struct
    {
        uint32_t DATA0: 32;
    };
    uint32_t Word;
} AES_DATA0_Typedef;

typedef union
{
    struct
    {
        uint32_t DATA1: 32;
    };
    uint32_t Word;
} AES_DATA1_Typedef;

typedef union
{
    struct
    {
        uint32_t DATA2: 32;
    };
    uint32_t Word;
} AES_DATA2_Typedef;

typedef union
{
    struct
    {
        uint32_t DATA3: 32;
    };
    uint32_t Word;
} AES_DATA3_Typedef;

typedef union
{
    struct
    {
        uint32_t KEY0: 32;
    };
    uint32_t Word;
} AES_KEY0_Typedef;

typedef union
{
    struct
    {
        uint32_t KEY1: 32;
    };
    uint32_t Word;
} AES_KEY1_Typedef;

typedef union
{
    struct
    {
        uint32_t KEY2: 32;
    };
    uint32_t Word;
} AES_KEY2_Typedef;

typedef union
{
    struct
    {
        uint32_t KEY3: 32;
    };
    uint32_t Word;
} AES_KEY3_Typedef;

typedef union
{
    struct
    {
        uint32_t GODONE: 1;
        uint32_t ENCRYPT: 1;
        uint32_t RESERVED0: 4;
        uint32_t IE: 1;
        uint32_t IF: 1;
        uint32_t DMA_EN: 1;
        uint32_t RESERVED1: 23;
    };
    uint32_t Word;
} AES_CON_Typedef;

typedef struct
{
    __IO AES_DATA0_Typedef DATA0;
    __IO AES_DATA1_Typedef DATA1;
    __IO AES_DATA2_Typedef DATA2;
    __IO AES_DATA3_Typedef DATA3;
    __IO AES_KEY0_Typedef KEY0;
    __IO AES_KEY1_Typedef KEY1;
    __IO AES_KEY2_Typedef KEY2;
    __IO AES_KEY3_Typedef KEY3;
    __IO AES_CON_Typedef CON;
} AES_TypeDef;

typedef union
{
    struct
    {
        uint32_t PD: 1;
        uint32_t SEED: 1;
        uint32_t DATA_MODE: 1;
        uint32_t RESERVED0: 5;
        uint32_t TEST_EN: 1;
        uint32_t RESERVED1: 7;
        uint32_t RNG_VALID: 1;
        uint32_t RESERVED2: 7;
        uint32_t RNG_IE: 1;
        uint32_t RNG_IF: 1;
        uint32_t RESERVED3: 2;
        uint32_t DMA_EN: 1;
        uint32_t RESERVED4: 3;
    };
    uint32_t Word;
} TRNG_CON0_Typedef;

typedef union
{
    struct
    {
        uint32_t START_TIME: 24;
        uint32_t RESERVED0: 8;
    };
    uint32_t Word;
} TRNG_CON1_Typedef;

typedef union
{
    struct
    {
        uint32_t TRNG_DATA: 32;
    };
    uint32_t Word;
} TRNG_DATA_Typedef;

typedef struct
{
    __IO TRNG_CON0_Typedef CON0;
    __IO TRNG_CON1_Typedef CON1;
    __I TRNG_DATA_Typedef DATA;
} TRNG_TypeDef;

typedef union
{
    struct
    {
        uint32_t RST: 1;
        uint32_t Mode: 3;
        uint32_t RES: 3;
        uint32_t OPR: 3;
        uint32_t OPL: 3;
        uint32_t RESERVED0: 14;
        uint32_t ComMod: 2;
        uint32_t Remap: 1;
        uint32_t Comb: 1;
        uint32_t RUN: 1;
    };
    uint32_t Word;
} ECC_CON_Typedef;

typedef union
{
    struct
    {
        uint32_t WK: 1;
        uint32_t DONE: 1;
        uint32_t ERR: 1;
        uint32_t RESERVED0: 29;
    };
    uint32_t Word;
} ECC_STA_Typedef;

typedef union
{
    struct
    {
        uint32_t DMA_EN: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} ECC_DMACTR_Typedef;

typedef struct
{
    uint32_t RESERVED0[192] ;
    __IO ECC_CON_Typedef CON;
    __I ECC_STA_Typedef STA;
    __IO ECC_DMACTR_Typedef DMACTR;
} ECC_TypeDef;

typedef union
{
    struct
    {
        uint32_t UL: 32;
    };
    uint32_t Word;
} CRC_UL_Typedef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t RST: 1;
        uint32_t HS: 1;
        uint32_t DS: 1;
        uint32_t REQ: 1;
        uint32_t ACK: 1;
        uint32_t RESERVED0: 2;
        uint32_t MOD: 2;
        uint32_t BYTE: 2;
        uint32_t REFIN: 1;
        uint32_t REFOUT: 1;
        uint32_t XOROUT: 1;
        uint32_t RESERVED1: 1;
        uint32_t DMAEN: 1;
        uint32_t RESERVED2: 15;
    };
    uint32_t Word;
} CRC_CON_Typedef;

typedef union
{
    struct
    {
        uint32_t TRIG: 32;
    };
    uint32_t Word;
} CRC_TRIG_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 4;
        uint32_t ADDR: 16;
        uint32_t RESERVED1: 1;
        uint32_t IFREN: 1;
        uint32_t RESERVED2: 10;
    };
    uint32_t Word;
} CRC_ADDR_Typedef;

typedef union
{
    struct
    {
        uint32_t CRC_SIZE: 16;
        uint32_t RESERVED0: 16;
    };
    uint32_t Word;
} CRC_SIZE_Typedef;

typedef union
{
    struct
    {
        uint32_t DI: 32;
    };
    uint32_t Word;
} CRC_DI_Typedef;

typedef union
{
    struct
    {
        uint32_t DO: 32;
    };
    uint32_t Word;
} CRC_DO_Typedef;

typedef union
{
    struct
    {
        uint32_t BUSY: 1;
        uint32_t RSTF: 1;
        uint32_t EMPTY_ERR: 1;
        uint32_t RESERVED0: 29;
    };
    uint32_t Word;
} CRC_STA_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 4;
        uint32_t FADR: 16;
        uint32_t RESERVED1: 1;
        uint32_t IFREN: 1;
        uint32_t RESERVED2: 10;
    };
    uint32_t Word;
} CRC_FA_Typedef;

typedef struct
{
    __IO CRC_UL_Typedef UL;
    __IO CRC_CON_Typedef CON;
    __IO CRC_TRIG_Typedef TRIG;
    __IO CRC_ADDR_Typedef ADDR;
    __IO CRC_SIZE_Typedef SIZE;
    __IO CRC_DI_Typedef DI;
    __IO CRC_DO_Typedef DO;
    __IO CRC_STA_Typedef STA;
    __I CRC_FA_Typedef FA;
} CRC_TypeDef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t RESERVED0: 3;
        uint32_t State: 4;
        uint32_t RESERVED1: 8;
        uint32_t Chnls_minus1: 5;
        uint32_t RESERVED2: 7;
        uint32_t Test_status: 4;
    };
    uint32_t Word;
} DMA_STA_Typedef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t RESERVED0: 4;
        uint32_t Chnl_prot_ctrl0: 1;
        uint32_t Chnl_prot_ctrl1: 1;
        uint32_t Chnl_prot_ctrl2: 1;
        uint32_t RESERVED1: 24;
    };
    uint32_t Word;
} DMA_CFG_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 9;
        uint32_t Ctrl_base_ptr: 23;
    };
    uint32_t Word;
} DMA_CTRLBASEPTR_Typedef;


typedef union
{
    struct
    {
        uint32_t Alt_ctrl_base_ptr; 
    };
    uint32_t Word;
} DMA_ALTCTRLBASEPTR_Typedef;

typedef union
{
    struct
    {
        uint32_t Dma_waitonreq_status: 32;
    };
    uint32_t Word;
} DMA_WTonREQSTA_Typedef;

typedef union
{
    struct
    {
        uint32_t Chnl_sw_request: 32;
    };
    uint32_t Word;
} DMA_SWREQ_Typedef;

typedef union
{
    struct
    {
        uint32_t Chnl_useburst: 32;
    };
    uint32_t Word;
} DMA_USEBURSTSET_Typedef;

typedef union
{
    struct
    {
        uint32_t Chnl_useburst: 32;
    };
    uint32_t Word;
} DMA_USEBURSTCLR_Typedef;

typedef union
{
    struct
    {
        uint32_t Chnl_req_mask_set: 32;
    };
    uint32_t Word;
} DMA_REQMASKSET_Typedef;

typedef union
{
    struct
    {
        uint32_t Chnl_req_mask_clr: 32;
    };
    uint32_t Word;
} DMA_REQMASKCLR_Typedef;

typedef union
{
    struct
    {
        uint32_t Chnl_enable_set: 32;
    };
    uint32_t Word;
} DMA_ENBSET_Typedef;

typedef union
{
    struct
    {
        uint32_t Chnl_enable_clr: 32;
    };
    uint32_t Word;
} DMA_ENBCLR_Typedef;

typedef union
{
    struct
    {
        uint32_t Chnl_pri_alt_set: 32;
    };
    uint32_t Word;
} DMA_PRIALTSET_Typedef;

typedef union
{
    struct
    {
        uint32_t Chnl_pri_alt_clr: 32;
    };
    uint32_t Word;
} DMA_PRIALTCLR_Typedef;

typedef union
{
    struct
    {
        uint32_t Chnl_priority_set: 32;
    };
    uint32_t Word;
} DMA_PRIORITYSET_Typedef;

typedef union
{
    struct
    {
        uint32_t Chnl_priority_clr: 32;
    };
    uint32_t Word;
} DMA_PRIORITYCLR_Typedef;


typedef union
{
    struct
    {
        uint32_t Err_clr: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} DMA_ERRCLR_Typedef;

typedef union
{
    struct
    {
        uint32_t Int_test_en: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} DMA_INTGRCFG_Typedef;

typedef union
{
    struct
    {
        uint32_t Dma_stall_status: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} DMA_STALLSTA_Typedef;

typedef union
{
    struct
    {
        uint32_t Dma_req_status: 32;
    };
    uint32_t Word;
} DMA_REQSTA_Typedef;

typedef union
{
    struct
    {
        uint32_t Dma_sreq_status: 32;
    };
    uint32_t Word;
} DMA_SREQSTA_Typedef;

typedef union
{
    struct
    {
        uint32_t Dma_done_set: 32;
    };
    uint32_t Word;
} DMA_DONESET_Typedef;

typedef union
{
    struct
    {
        uint32_t Dma_done_clr: 32;
    };
    uint32_t Word;
} DMA_DONECLR_Typedef;

typedef union
{
    struct
    {
        uint32_t Dma_active_set: 32;
    };
    uint32_t Word;
} DMA_ACTIVESET_Typedef;

typedef union
{
    struct
    {
        uint32_t Dma_active_clr: 32;
    };
    uint32_t Word;
} DMA_ACTIVECLR_Typedef;

typedef union
{
    struct
    {
        uint32_t Err_set: 1;
        uint32_t RESERVED0: 31;
    };
    uint32_t Word;
} DMA_ERRSET_Typedef;

typedef struct
{
    __I DMA_STA_Typedef STA;
    __O DMA_CFG_Typedef CFG;
    __IO DMA_CTRLBASEPTR_Typedef CTRLBASEPTR;
    __I DMA_ALTCTRLBASEPTR_Typedef ALTCTRLBASEPTR;
    __I DMA_WTonREQSTA_Typedef WTonREQSTA;
    __O DMA_SWREQ_Typedef SWREQ;
    __IO DMA_USEBURSTSET_Typedef USEBURSTSET;
    __O DMA_USEBURSTCLR_Typedef USEBURSTCLR;
    __IO DMA_REQMASKSET_Typedef REQMASKSET;
    __O DMA_REQMASKCLR_Typedef REQMASKCLR;
    __IO DMA_ENBSET_Typedef ENBSET;
    __O DMA_ENBCLR_Typedef ENBCLR;
    __IO DMA_PRIALTSET_Typedef PRIALTSET;
    __O DMA_PRIALTCLR_Typedef PRIALTCLR;
    __IO DMA_PRIORITYSET_Typedef PRIORITYSET;
    __O DMA_PRIORITYCLR_Typedef PRIORITYCLR;
    uint32_t RESERVED1[4] ;
    __IO DMA_ERRCLR_Typedef ERRCLR;
    uint32_t RESERVED2[876] ;
    __IO DMA_INTGRCFG_Typedef INTGRCFG;
    uint32_t RESERVED3 ;
    __I DMA_STALLSTA_Typedef STALLSTA;
    uint32_t RESERVED4 ;
    __I DMA_REQSTA_Typedef REQSTA;
    uint32_t RESERVED5 ;
    __I DMA_SREQSTA_Typedef SREQSTA;
    uint32_t RESERVED6 ;
    __IO DMA_DONESET_Typedef DONESET;
    __O DMA_DONECLR_Typedef DONECLR;
    __IO DMA_ACTIVESET_Typedef ACTIVESET;
    __O DMA_ACTIVECLR_Typedef ACTIVECLR;
    uint32_t RESERVED7[6] ;
    __O DMA_ERRSET_Typedef ERRSET;
} DMA_TypeDef;

typedef union
{
    struct
    {
        uint32_t EN: 1;
        uint32_t INV_REQ: 1;
        uint32_t POW_REQ: 1;
        uint32_t SMAN_POW: 1;
        uint32_t SMAN_INV: 1;
        uint32_t PRFETCH: 1;
        uint32_t STC_EN: 1;
        uint32_t RESERVED0: 25;
    };
    uint32_t Word;
} CACHE_CCR_Typedef;

typedef union
{
    struct
    {
        uint32_t POW_ERR: 1;
        uint32_t MAN_INV_E: 1;
        uint32_t RESERVED0: 30;
    };
    uint32_t Word;
} CACHE_IRQMASK_Typedef;

typedef union
{
    struct
    {
        uint32_t POW_ERR: 1;
        uint32_t MAN_INV_E: 1;
        uint32_t : 30;
    };
    uint32_t Word;
} CACHE_IRQSTAT_Typedef;

typedef union
{
    struct
    {
        uint32_t AW: 5;
        uint32_t CW: 5;
        uint32_t CW_N: 2;
        uint32_t R_REGS: 1;
        uint32_t ST_LOG: 1;
        uint32_t RESERVED0: 18;
    };
    uint32_t Word;
} CACHE_HWPAR_Typedef;

typedef union
{
    struct
    {
        uint32_t CSHR: 32;
    };
    uint32_t Word;
} CACHE_CSHR_Typedef;

typedef union
{
    struct
    {
        uint32_t CSMR: 32;
    };
    uint32_t Word;
} CACHE_CSMR_Typedef;

typedef union
{
    struct
    {
        uint32_t DES_2: 4;
        uint32_t SIZE: 4;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} CACHE_PIDR4_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 32;
    };
    uint32_t Word;
} CACHE_PIDR5_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 32;
    };
    uint32_t Word;
} CACHE_PIDR6_Typedef;

typedef union
{
    struct
    {
        uint32_t RESERVED0: 32;
    };
    uint32_t Word;
} CACHE_PIDR7_Typedef;

typedef union
{
    struct
    {
        uint32_t PART_0: 8;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} CACHE_PIDR0_Typedef;

typedef union
{
    struct
    {
        uint32_t PART_1: 4;
        uint32_t DES_0: 4;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} CACHE_PIDR1_Typedef;

typedef union
{
    struct
    {
        uint32_t DES_1: 3;
        uint32_t JEDEC: 1;
        uint32_t REVISION: 4;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} CACHE_PIDR2_Typedef;

typedef union
{
    struct
    {
        uint32_t CMOD: 3;
        uint32_t RESERVED0: 1;
        uint32_t REVAND: 4;
        uint32_t RESERVED1: 24;
    };
    uint32_t Word;
} CACHE_PIDR3_Typedef;

typedef union
{
    struct
    {
        uint32_t PRMBL_0: 8;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} CACHE_CIDR0_Typedef;

typedef union
{
    struct
    {
        uint32_t PRMBL_1: 4;
        uint32_t CLASS: 4;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} CACHE_CIDR1_Typedef;

typedef union
{
    struct
    {
        uint32_t PRMBL_2: 8;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} CACHE_CIDR2_Typedef;

typedef union
{
    struct
    {
        uint32_t PRMBL_3: 8;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} CACHE_CIDR3_Typedef;

typedef struct
{
    __IO CACHE_CCR_Typedef CCR;
    uint32_t RESERVED0 ;
    __IO CACHE_IRQMASK_Typedef IRQMASK;
    __IO CACHE_IRQSTAT_Typedef IRQSTAT;
    __I CACHE_HWPAR_Typedef HWPAR;
    __IO CACHE_CSHR_Typedef CSHR;
    __IO CACHE_CSMR_Typedef CSMR;
    uint32_t RESERVED1[1005] ;
    __I CACHE_PIDR4_Typedef PIDR4;
    __I CACHE_PIDR5_Typedef PIDR5;
    __I CACHE_PIDR6_Typedef PIDR6;
    __I CACHE_PIDR7_Typedef PIDR7;
    __I CACHE_PIDR0_Typedef PIDR0;
    __I CACHE_PIDR1_Typedef PIDR1;
    __I CACHE_PIDR2_Typedef PIDR2;
    __I CACHE_PIDR3_Typedef PIDR3;
    __I CACHE_CIDR0_Typedef CIDR0;
    __I CACHE_CIDR1_Typedef CIDR1;
    __I CACHE_CIDR2_Typedef CIDR2;
    __I CACHE_CIDR3_Typedef CIDR3;
} CACHE_TypeDef;

/******************************************************************************/
/************************************* Ethernet *******************************/
/******************************************************************************/
typedef union
{
  struct
  {
    uint32_t TRANSMIT_ENABLE: 1;
    uint32_t SYNCHRONIZED_TRANSMIT_ENABLE: 1;
    uint32_t RECEIVE_ENABLE: 1;
    uint32_t SYNCHRONIZED_RECEIVE_ENABLE: 1;
    uint32_t TRANSMIT_FLOW_CONTROL_ENABLE: 1;
    uint32_t RECEIVE_FLOW_CONTROL_ENABLE: 1;
    uint32_t RESERVED0: 2;
    uint32_t LOOP_BACK: 1;
    uint32_t RESERVED1: 7;
    uint32_t RESET_TX_FUNCTION: 1;
    uint32_t RESET_RX_FUNCTION: 1;
    uint32_t RESET_TX_MAC_CONTROL: 1;
    uint32_t RESET_RX_MAC_CONTROL: 1;
    uint32_t RESERVED2: 10;
    uint32_t SIMULATION_RESET: 1;
    uint32_t SOFT_RESET: 1;
  };
  uint32_t Word;
} ETH_MAC_CONF1_Typedef;

typedef union
{
  struct
  {
    uint32_t FULL_DUPLEX: 1;
    uint32_t CRC_ENABLE: 1;
    uint32_t PAD_CRC_ENABLE: 1;
    uint32_t RESERVED0: 1;
    uint32_t LENGTH_FIELD_CHECKING: 1;
    uint32_t HUGE_FRAME_ENABLE: 1;
    uint32_t RESERVED1: 2;
    uint32_t INTERFACE_MODE: 2;
    uint32_t RESERVED3: 2;
    uint32_t PREAMBLE_LENGTH: 4;
    uint32_t RESERVED4: 16;
  };
  uint32_t Word;
} ETH_MAC_CONF2_Typedef;

typedef union
{
  struct
  {
    uint32_t BBIPG: 7;
    uint32_t RESERVED0: 1;
    uint32_t MIE: 7;
    uint32_t IPGR2: 7;
    uint32_t RESERVED1: 1;
    uint32_t IPGR1: 7;
    uint32_t RESERVED2: 2;
  };
  uint32_t Word;
} ETH_IPFG_Typedef;

typedef union
{
  struct
  {
    uint32_t COLLISION_WINDOW: 10;
    uint32_t RESERVED0: 2;
    uint32_t RETRANSMISSION_MAXIMUM: 4;
    uint32_t EXCESSIVE_DEFER: 1;
    uint32_t NO_BACKOFF: 1;
    uint32_t BACKPRESSURE_NO_BACKOFF: 1;
    uint32_t ABEBE: 1;
    uint32_t ABEBT: 4;
    uint32_t RESERVED1: 8;
  };
  uint32_t Word;
} ETH_HFDUP_Typedef;

typedef union
{
  struct
  {
    uint32_t MAXIMUM_FRAME_LENGTH: 16;
    uint32_t RESERVED0: 16;
  };
  uint32_t Word;
} ETH_MFLEN_Typedef;

typedef union
{
  struct
  {
    uint32_t SHORTCUT_SLOT_TIME: 1;
    uint32_t TEST_PAUSE: 1;
    uint32_t RTFE: 1;
    uint32_t AXIMUM_BACKOFF: 1; 
    uint32_t RESERVED0: 28;
  };
  uint32_t Word;
} ETH_TSTR_Typedef;

typedef union
{
  struct
  {
    uint32_t MGCLK_SELECT: 3;
    uint32_t RESERVED0: 1;
    uint32_t PREAMBLE_SUPPRESSION: 1;
    uint32_t SCAN_AUTO_INCREMENT: 1;
    uint32_t RESERVED1: 25;
    uint32_t RST_MGMT: 1;
  };
  uint32_t Word;
} ETH_MGCONF_Typedef;

typedef union
{
  struct
  {
    uint32_t READ_CYCLE: 1;
        uint32_t SCAN_CYCLE: 1;
    uint32_t RESERVED0: 30;
  };
  uint32_t Word;
} ETH_MGCMD_Typedef;

typedef union
{
  struct
  {
    uint32_t REG_ADX: 5;
    uint32_t RESERVED0: 3;
    uint32_t PHY_ADX: 5;
    uint32_t RESERVED1: 19;
  };
  uint32_t Word;
} ETH_MGADR_Typedef;

typedef union
{
  struct
  {
    uint32_t WDATA: 16;
    uint32_t RESERVED0: 16;
  };
  uint32_t Word;
} ETH_MGWDATA_Typedef;

typedef union
{
  struct
  {
    uint32_t RDATA: 16;
    uint32_t RESERVED0: 16;
  };
  uint32_t Word;
} ETH_MGRDATA_Typedef;

typedef union
{
  struct
  {
    uint32_t BUSY: 1;
    uint32_t SCANNING: 1;
    uint32_t NOT_VALID: 1;
    uint32_t RESERVED0: 29;
  };
  uint32_t Word;
} ETH_MGFIG_Typedef;

typedef union
{
  struct
  {
    uint32_t ENABLE_JABBER_PROTECTION: 1;
    uint32_t RESERVED0: 6;
    uint32_t RESET_GPSI: 1;
    uint32_t DISABLE_LINK_FAIL: 1;
    uint32_t NO_CIPHER: 1;
    uint32_t FORCE_QUIET: 1;
    uint32_t RESERVED1: 4;
    uint32_t RESET_PE100X: 1;
    uint32_t SPEED: 1;
    uint32_t RESERVED2: 6;
    uint32_t RESET_PERMII: 1;
    uint32_t PHY_MODE: 1;
    uint32_t LHDMODE: 1;
    uint32_t GHDMODE: 1;
    uint32_t TBIMODE: 1;
    uint32_t RESERVED3: 3;
    uint32_t RESET_INTERFACE_MODULE: 1; 
  };
  uint32_t Word;
} ETH_ITFCON_Typedef;

typedef union
{
  struct
  {
    uint32_t JABBER: 1;
    uint32_t SQE_ERROR: 1;
    uint32_t LOSS_OF_CARRIER: 1;
    uint32_t LINK_AIL: 1;
    uint32_t SPEED: 1;
    uint32_t FULL_DUPLEX: 1;
    uint32_t LINK_OK: 1;
    uint32_t JABBER1: 1;
    uint32_t CLASH: 1;
    uint32_t EXCESS_DEFER: 1;
    uint32_t WOLDTCTD: 1;
    uint32_t RESERVED0: 21;
  };
  uint32_t Word;
} ETH_ITFSTA_Typedef;

typedef union
{
  struct
  {
    uint32_t octet_4: 8;
    uint32_t octet_3: 8;
    uint32_t octet_2: 8;
    uint32_t octet_1: 8;
  };
  uint32_t Word;
} ETH_STAADR1_Typedef;

typedef union
{
  struct
  {
    uint32_t RESERVED0: 16;
    uint32_t octet_6: 8;
    uint32_t octet_5: 8;
  };
  uint32_t Word;
} ETH_STAADR2_Typedef;


typedef struct
{
  __IO ETH_MAC_CONF1_Typedef MAC_CONF1;
  __IO ETH_MAC_CONF2_Typedef MAC_CONF2;
  __IO ETH_IPFG_Typedef IPFG;
  __IO ETH_HFDUP_Typedef HFDUP;
  __IO ETH_MFLEN_Typedef MFLEN;
  uint32_t RESERVED0[2];
  __IO ETH_TSTR_Typedef TSTR;
  __IO ETH_MGCONF_Typedef MGCONF;
  __IO ETH_MGCMD_Typedef MGCMD;
  __IO ETH_MGADR_Typedef MGADR;
  __O ETH_MGWDATA_Typedef MGWDATA;
  __I ETH_MGRDATA_Typedef MGRDATA;
  __I ETH_MGFIG_Typedef MGFIG;
  __IO ETH_ITFCON_Typedef ITFCON;
  __I ETH_ITFSTA_Typedef ITFSTA;
  __IO ETH_STAADR1_Typedef STAADR1;
  __IO ETH_STAADR2_Typedef STAADR2;
    // Added By Aidan 2017-03-09
  __IO uint32_t FIFCONF0;    //__IO ETH_FIFCONF0_Typedef  FIFCONF0; // 0x0048
  __IO uint32_t FIFCONF1;    //__IO ETH_FIFCONF1_Typedef  FIFCONF1; // 0x004C
  __IO uint32_t FIFCONF2;    //__IO ETH_FIFCONF2_Typedef  FIFCONF2; // 0x0050
  __IO uint32_t FIFCONF3;    //__IO ETH_FIFCONF3_Typedef  FIFCONF3; // 0x0054
  __IO uint32_t FIFCONF4;    //__IO ETH_FIFCONF4_Typedef  FIFCONF4; // 0x0058
  __IO uint32_t FIFCONF5;    //__IO ETH_FIFCONF5_Typedef  FIFCONF5; // 0x005C
  __IO uint32_t FIFO0;       //__IO ETH_FIFO0_Typedef     FIFO0;    // 0x0060
  __IO uint32_t FIFO1;       //__IO ETH_FIFO1_Typedef     FIFO1;    // 0x0064
  __IO uint32_t FIFO2;       //__IO ETH_FIFO2_Typedef     FIFO2;    // 0x0068
  __IO uint32_t FIFO3;       //__IO ETH_FIFO3_Typedef     FIFO3;    // 0x006C
  __IO uint32_t FIFO4;       //__IO ETH_FIFO4_Typedef     FIFO4;    // 0x0070
  __IO uint32_t FIFO5;       //__IO ETH_FIFO5_Typedef     FIFO5;    // 0x0074
  __IO uint32_t FIFO6;       //__IO ETH_FIFO6_Typedef     FIFO6;    // 0x0078
  __IO uint32_t FIFO7;       //__IO ETH_FIFO7_Typedef     FIFO7;    // 0x007C
  uint32_t RESERVED1[64];    // 0x0080 - 0x017C
  __IO uint32_t DMATXCTL;    //__IO ETH_DMATXCTL_Typedef  DMATXCTL; // 0x180
  __IO uint32_t DMATXDES;    //__IO ETH_DMATXDES_Typedef  DMATXDES; // 0x184
  __IO uint32_t DMATXSTA;    //__IO ETH_DMATXSTA_Typedef  DMATXSTA; // 0x188
  __IO uint32_t DMARXCTL;    //__IO ETH_DMARXCTL_Typedef  DMARXCTL; // 0x18C
  __IO uint32_t DMARXDES;    //__IO ETH_DMARXDES_Typedef  DMARXDES; // 0x190
  __IO uint32_t DMARXSTA;    //__IO ETH_DMARXSTA_Typedef  DMARXSTA; // 0x194
  __IO uint32_t DMAIMR;      //__IO ETH_DMAIMR_Typedef    DMAIMR;   // 0x198
  __O  uint32_t DMAISR;      //__O  ETH_DMAISR_Typedef    DMAISR;   // 0x19C
} ETH_TypeDef;

/* Base addresses */
#define FLASH_BASE (0x00000000UL)
#define SRAM_BASE (0x20000000UL)
#define APB_BASE (0x40000000UL)
#define RAM_BASE (0x60000000UL)

/* APB peripherals */
#define SCU_BASE (APB_BASE + 0x00000)
#define GPIO_BASE (APB_BASE + 0x00400)
#define IAP_BASE (APB_BASE + 0x00800)
#define ADC_BASE (APB_BASE + 0x01000)
#define IWDT_BASE (APB_BASE + 0x01C00)
#define WWDT_BASE (APB_BASE + 0x01800)
#define T16N0_BASE (APB_BASE + 0x02000)
#define T16N1_BASE (APB_BASE + 0x02400)
#define T16N2_BASE (APB_BASE + 0x02800)
#define T16N3_BASE (APB_BASE + 0x02C00)
#define T32N0_BASE (APB_BASE + 0x04000)
#define T32N1_BASE (APB_BASE + 0x04400)
#define T32N2_BASE (APB_BASE + 0x04800)
#define T32N3_BASE (APB_BASE + 0x04C00)
#define UART0_BASE (APB_BASE + 0x06000)
#define UART1_BASE (APB_BASE + 0x06400)
#define UART2_BASE (APB_BASE + 0x06800)
#define UART3_BASE (APB_BASE + 0x06C00)
#define SPI0_BASE (APB_BASE + 0x08000)
#define SPI1_BASE (APB_BASE + 0x08400)
#define SPI2_BASE (APB_BASE + 0x08800)
#define SPI3_BASE (APB_BASE + 0x08C00)
#define I2C0_BASE (APB_BASE + 0x09000)
#define I2C1_BASE (APB_BASE + 0x09400)
#define AES_BASE (APB_BASE + 0x40000)
#define TRNG_BASE (APB_BASE + 0x0E400)
#define ECC_BASE (APB_BASE + 0x0E800)
#define CRC_BASE (APB_BASE + 0x10000)
#define DMA_BASE (APB_BASE + 0x81000)
#define Cache_BASE (APB_BASE + 0x80000)
#define ETH_BASE (APB_BASE + 0xC0400)


#define SCU ((SCU_TypeDef *) SCU_BASE )
#define GPIO ((GPIO_TypeDef *) GPIO_BASE )
#define IAP ((IAP_TypeDef *) IAP_BASE )
#define ADC ((ADC_TypeDef *) ADC_BASE )
#define IWDT ((IWDT_TypeDef *) IWDT_BASE )
#define WWDT ((WWDT_TypeDef *) WWDT_BASE )
#define T16N0 ((T16N_TypeDef *) T16N0_BASE )
#define T16N1 ((T16N_TypeDef *) T16N1_BASE )
#define T16N2 ((T16N_TypeDef *) T16N2_BASE )
#define T16N3 ((T16N_TypeDef *) T16N3_BASE )
#define T32N0 ((T32N_TypeDef *) T32N0_BASE )
#define T32N1 ((T32N_TypeDef *) T32N1_BASE )
#define T32N2 ((T32N_TypeDef *) T32N2_BASE )
#define T32N3 ((T32N_TypeDef *) T32N3_BASE )
#define UART0 ((UART_TypeDef *) UART0_BASE )
#define UART1 ((UART_TypeDef *) UART1_BASE )
#define UART2 ((UART_TypeDef *) UART2_BASE )
#define UART3 ((UART_TypeDef *) UART3_BASE )
#define SPI0 ((SPI_TypeDef *) SPI0_BASE )
#define SPI1 ((SPI_TypeDef *) SPI1_BASE )
#define SPI2 ((SPI_TypeDef *) SPI2_BASE )
#define SPI3 ((SPI_TypeDef *) SPI3_BASE )
#define I2C0 ((I2C_TypeDef *) I2C0_BASE )
#define I2C1 ((I2C_TypeDef *) I2C1_BASE )
#define AES ((AES_TypeDef *) AES_BASE )
#define TRNG ((TRNG_TypeDef *) TRNG_BASE )
#define ECC ((ECC_TypeDef *) ECC_BASE )
#define CRC ((CRC_TypeDef *) CRC_BASE )
#define DMA ((DMA_TypeDef *) DMA_BASE )
#define Cache ((CACHE_TypeDef *) Cache_BASE )
#define ETH ((ETH_TypeDef *) ETH_BASE )


#endif

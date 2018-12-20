/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#ifndef __LIBSCU_H
#define __LIBSCU_H

#include "p131.h"
#include "type.h"

typedef enum
{
    SCU_NMIIRQ_RST         = -3,
    SCU_NMIIRQ_NMI         = -2, 
    SCU_NMIIRQ_HardFault   = -1, 
    SCU_NMIIRQ_GPIO0       = 0, 
    SCU_NMIIRQ_GPIO1       = 1, 
    SCU_NMIIRQ_GPIO2       = 2, 
    SCU_NMIIRQ_GPIO3       = 3, 
    SCU_NMIIRQ_GPIO4       = 4, 
    SCU_NMIIRQ_GPIO5       = 5, 
    SCU_NMIIRQ_GPIO6       = 6, 
    SCU_NMIIRQ_GPIO7       = 7, 
    SCU_NMIIRQ_T16N0       = 8, 
    SCU_NMIIRQ_T16N1       = 9, 
    SCU_NMIIRQ_T16N2       = 10, 
    SCU_NMIIRQ_T32N0       = 12,
    SCU_NMIIRQ_T32N1       = 13, 
    SCU_NMIIRQ_T32N2       = 14, 
    SCU_NMIIRQ_T32N3       = 15,  
    SCU_NMIIRQ_IWDT        = 16, 
    SCU_NMIIRQ_WWDT        = 17, 
    SCU_NMIIRQ_SPI2        = 18, 
    SCU_NMIIRQ_SPI3        = 19,
    SCU_NMIIRQ_UART0       = 20,
    SCU_NMIIRQ_UART1       = 21, 
    SCU_NMIIRQ_UART2       = 22,
    SCU_NMIIRQ_UART3       = 23, 
    SCU_NMIIRQ_SPI0        = 24, 
    SCU_NMIIRQ_SPI1        = 25,
    SCU_NMIIRQ_I2C0        = 26,
    SCU_NMIIRQ_I2C1        = 27, 
    SCU_NMIIRQ_AES         = 28, 
    SCU_NMIIRQ_ADC         = 29, 
    SCU_NMIIRQ_KINT        = 31,
    SCU_NMIIRQ_PLCRX       = 32,
    SCU_NMIIRQ_PLCTX       = 33,
    SCU_NMIIRQ_PLCTIMER    = 34,
    SCU_NMIIRQ_DMACDONE    = 36,
    SCU_NMIIRQ_DMACRR      = 37,
    SCU_NMIIRQ_ECC         = 38,
    SCU_NMIIRQ_RNG         = 39,
    SCU_NMIIRQ_Cache       = 40,
    SCU_NMIIRQ_ETHERNETINT = 43,
    SCU_NMIIRQ_LVD         = 44,
    SCU_NMIIRQ_COMP        = 45,
} SCU_TYPE_NMICS;

typedef enum
{
    SCU_PWRC_CHIPRSTF   = (1 << 1),
    SCU_PWRC_PORF_3     = (1 << 2),
    SCU_PWRC_RC_PORF    = (1 << 3),
    SCU_PWRC_BORF       = (1 << 4),
    SCU_PWRC_IWDTRSTF   = (1 << 5),
    SCU_PWRC_MRSTF      = (1 << 6),
    SCU_PWRC_SOFTRSTF   = (1 << 7),
    SCU_PWRC_POR_LOST   = (1 << 8),
    SCU_PWRC_LOCKUP_RST = (1 << 9),
    SCU_PWRC_WWDTRSTF   = (1 << 10),
} SCU_TYPE_PWRC;

typedef enum
{
    SCU_LVDFlag_IF  = (1 << 8),
    SCU_LVDFlag_Out = (1 << 15),
} SCU_TYPE_LVDCON;

typedef enum
{
    SCU_SYSCLK_LRC   = 0x01,
    SCU_SYSCLK_HRC   = 0x02,
    SCU_SYSCLK_XTAL  = 0x03,
    SCU_SYSCLK_DPLL2 = 0x04,
    WRONG_SET        = 0X0f,
} SCU_TYPE_SYSCLK;

typedef enum
{  
    SCU_DPLL2_LOCKED = (1 << 28),
    SCU_DPLL2_FLAG1  = (1 << 29),
    SCU_DPLL2_FLAG0  = (1 << 30),
} SCU_TYPE_DPLL2;

typedef enum
{
    SCU_GPIO   = 0,
    SCU_FLASH  = 1,  
    SCU_ADC    = 2,
    SCU_IWDT   = 3,
    SCU_T16N0  = 4,
    SCU_T16N1  = 5,
    SCU_T16N2  = 6,
    SCU_T16N3  = 7,
    SCU_T32N0  = 8,
    SCU_T32N1  = 9,
    SCU_T32N2  = 10,
    SCU_T32N3  = 11,
    SCU_UART0  = 12,
    SCU_UART1  = 13,
    SCU_UART2  = 14,
    SCU_UART3  = 15,
    SCU_SPI0   = 16,
    SCU_SPI1   = 17,
    SCU_I2C0   = 18,
    SCU_I2C1   = 19,
    SCU_TENSOR = 20,
    SCU_ECC    = 23,
    SCU_CRC    = 24,
    SCU_WWDT   = 25,
    SCU_OPAMP  = 26,
    SCU_COMP   = 27,
    SCU_RNG    = 29,
    SCU_SPI2   = 30,
    SCU_SPI3   = 31,
} SCU_APB1PERIPH_Typedef;

typedef enum
{         
    SCU_AES   = 0,
    SCU_BPLC  = 1,
    SCU_150M  = 2,
    SCU_TIMER = 3,
} SCU_APB2PERIPH_Typedef;

typedef enum
{           
    SCU_DMA  = 0,
    SCU_USB  = 1,
    SCU_SRAM = 2,
    SCU_APB1 = 3,
    SCU_APB2 = 4,
    SCU_ETH  = 5,
} SCU_AHBPERIPH_Typedef;

typedef enum
{
    SCU_DPLL_HRC  = 0x0,
    SCU_DPLL_XTAL = 0x1,
} SCU_DPLL_Origin;

typedef enum
{
    SCU_DPLL2_200M = 0x0,
    SCU_DPLL2_150M = 0x1,
} SCU_DPLL2_Out;

typedef enum
{
    HCLK_Div1 = 0x00,                 
    HCLK_Div2 = 0x01,              
    HCLK_Div4 = 0x02,               
    HCLK_Div8 = 0x03,                 
} SCU_APB_PRE;

typedef struct
{
    uint32_t SYSCLK_Frequency; /*!<  SYSCLK clock frequency expressed in Hz */
    uint32_t HCLK_Frequency;   /*!<  HCLK clock frequency expressed in Hz   */
    uint32_t APB1_Frequency;  /*!<  APB1 clock frequency expressed in Hz  */
    uint32_t APB2_Frequency;  /*!<  APB2 clock frequency expressed in Hz  */
} SCU_ClocksTypeDef;


#define SCU_RegUnLock() (SCU->PROT.Word = 0x55AA6996)
#define SCU_RegLock()   (SCU->PROT.Word = 0x00000000)

#define SCU_NMI_Enable()    (SCU->NMICON.NMIEN = 0x1)
#define SCU_NMI_Disable()     (SCU->NMICON.NMIEN = 0x0)

#define SCU_LVD_Enable()    (SCU->LVDCON.EN = 0x1)
#define SCU_LVD_Disable()   (SCU->LVDCON.EN = 0x0)

#define SCU_LVDFLT_Enable()     (SCU->LVDCON.FLTEN = 0x1) 
#define SCU_LVDFLT_Disable()    (SCU->LVDCON.FLTEN = 0x0) 

#define SCU_LVDVS_1V80()    (SCU->LVDCON.VS = 0x0)
#define SCU_LVDVS_1V88()    (SCU->LVDCON.VS = 0x1)
#define SCU_LVDVS_1V96()    (SCU->LVDCON.VS = 0x2)
#define SCU_LVDVS_2V04()    (SCU->LVDCON.VS = 0x3)
#define SCU_LVDVS_2V12()    (SCU->LVDCON.VS = 0x4)
#define SCU_LVDVS_2V20()    (SCU->LVDCON.VS = 0x5)
#define SCU_LVDVS_2V28()    (SCU->LVDCON.VS = 0x6)
#define SCU_LVDVS_2V36()    (SCU->LVDCON.VS = 0x7)
#define SCU_LVDVS_2V44()    (SCU->LVDCON.VS = 0x8)
#define SCU_LVDVS_2V52()    (SCU->LVDCON.VS = 0x9)
#define SCU_LVDVS_2V60()    (SCU->LVDCON.VS = 0xA)
#define SCU_LVDVS_2V68()    (SCU->LVDCON.VS = 0xB)
#define SCU_LVDVS_2V76()    (SCU->LVDCON.VS = 0xC)
#define SCU_LVDVS_2V84()    (SCU->LVDCON.VS = 0xD)
#define SCU_LVDVS_2V92()    (SCU->LVDCON.VS = 0xE)
#define SCU_LVDVS_3V00()    (SCU->LVDCON.VS = 0xF)

#define SCU_LVDIT_Enable()  (SCU->LVDCON.IE = 0x1)
#define SCU_LVDIT_Disable() (SCU->LVDCON.IE = 0x0)

#define SCU_LVDClearIFBit() (SCU->LVDCON.IF = 1)

#define SCU_LVDIFS_Rise()   (SCU->LVDCON.IFS = 0x0)
#define SCU_LVDIFS_Fall()   (SCU->LVDCON.IFS = 0x1)
#define SCU_LVDIFS_High()   (SCU->LVDCON.IFS = 0x2)
#define SCU_LVDIFS_Low()    (SCU->LVDCON.IFS = 0x3)
#define SCU_LVDIFS_Change() (SCU->LVDCON.IFS = 0x4)

#define SCU_FlashWait_2Tclk()   (SCU->FLASHWAIT.ACCT = 0x0)
#define SCU_FlashWait_3Tclk()   (SCU->FLASHWAIT.ACCT = 0x1)
#define SCU_FlashWait_4Tclk()   (SCU->FLASHWAIT.ACCT = 0x2)
#define SCU_FlashWait_5Tclk()   (SCU->FLASHWAIT.ACCT = 0x3)
#define SCU_FlashWait_6Tclk()   (SCU->FLASHWAIT.ACCT = 0x4)
#define SCU_FlashWait_7Tclk()   (SCU->FLASHWAIT.ACCT = 0x5)
#define SCU_FlashWait_8Tclk()   (SCU->FLASHWAIT.ACCT = 0x6)
#define SCU_FlashWait_9Tclk()   (SCU->FLASHWAIT.ACCT = 0x7)
#define SCU_FlashWait_10Tclk()  (SCU->FLASHWAIT.ACCT = 0x8)
#define SCU_FlashWait_11Tclk()  (SCU->FLASHWAIT.ACCT = 0x9)
#define SCU_FlashWait_12Tclk()  (SCU->FLASHWAIT.ACCT = 0xA)
#define SCU_FlashWait_13Tclk()  (SCU->FLASHWAIT.ACCT = 0xB)
#define SCU_FlashWait_14Tclk()  (SCU->FLASHWAIT.ACCT = 0xC)
#define SCU_FlashWait_15Tclk()  (SCU->FLASHWAIT.ACCT = 0xD)
#define SCU_FlashWait_16Tclk()  (SCU->FLASHWAIT.ACCT = 0xE)
#define SCU_FlashWait_17Tclk()  (SCU->FLASHWAIT.ACCT = 0xF)

#define SCU_MCU_DIV1()  (SCU->SCLKEN0.MCU_DIV 0 = 0X00)
#define SCU_MCU_DIV2()  (SCU->SCLKEN0.MCU_DIV 0 = 0X01)
#define SCU_MCU_DIV4()  (SCU->SCLKEN0.MCU_DIV 0 = 0X11)

//#define SCU_ABP1_PRE1_DIV1()     (SCU->AHBEN.ABP1_PRE1 = 0)
#define SCU_ABP1_PRE1_DIV2()     (SCU->AHBEN.ABP1_PRE1 = 1)
#define SCU_ABP1_PRE1_DIV4()     (SCU->AHBEN.ABP1_PRE1 = 2)
#define SCU_ABP1_PRE1_DIV8()     (SCU->AHBEN.ABP1_PRE1 = 3)

#define SCU_ABP2_PRE1_DIV1()     (SCU->AHBEN.ABP2_PRE1 = 0)
#define SCU_ABP2_PRE1_DIV2()     (SCU->AHBEN.ABP2_PRE1 = 1)
#define SCU_ABP2_PRE1_DIV4()     (SCU->AHBEN.ABP2_PRE1 = 2)
#define SCU_ABP2_PRE1_DIV8()     (SCU->AHBEN.ABP2_PRE1 = 3)

#define SCU_XTAL_LP_Enable()    (SCU->SCLKEN0.XTAL_LP = 0)
#define SCU_XTAL_LP_Disable()   (SCU->SCLKEN0.XTAL_LP = 1)

#define SCU_MOSC_XTAL_Enable()      (SCU->SCLKEN0.MOSCEN = 1)
#define SCU_MOSC_XTAL_Disable()     (SCU->SCLKEN0.MOSCEN = 0)

#define SCU_MOSC_XTAL_FILT_Enable()     (SCU->SCLKEN0.CLKFLT_EN = 1)
#define SCU_MOSC_XTAL_FILT_Disable()    (SCU->SCLKEN0.CLKFLT_EN = 0)

#define SCU_TBLRemap_Enable()   (SCU->TBLREMAPEN.EN= 1)
#define SCU_TBLRemap_Disable()  (SCU->TBLREMAPEN.EN= 0)

#define SCU_TBL_Offset(x)   (SCU->TBLOFF.TBLOFF = (uint32_t)x)

void SCU_NMISelect(SCU_TYPE_NMICS NMI_Type);            
FlagStatus SCU_GetPWRCFlagStatus(SCU_TYPE_PWRC PWRC_Flag);
void SCU_ClearPWRCFlagBit(SCU_TYPE_PWRC PWRC_Flag);
FlagStatus SCU_GetLVDFlagStatus(SCU_TYPE_LVDCON LVD_Flag);


void SCU_SET_SYSCLK_LRC(void);
void SCU_SET_SYSCLK_XTAL(void);
void SCU_SET_SYSCLK_HRC(void);
void SCU_SET_SYSCLK_DPLL2(uint32_t dpll2_value);

SCU_TYPE_SYSCLK SCU_GetSysClk(void);
void SystemClockSelect(SCU_TYPE_SYSCLK CLKx);

void DeviceClockAllEnable(void);
void DeviceClockAllDisable(void);
void AHBPeriphClock_Config(SCU_AHBPERIPH_Typedef tppe_periph ,FunctionalState NewState );
void APB1PeriphClock_Config(SCU_APB1PERIPH_Typedef tppe_periph ,FunctionalState NewState );
void APB2PeriphClock_Config(SCU_APB2PERIPH_Typedef tppe_periph ,FunctionalState NewState );
void SCU_GetClocksFreq(SCU_ClocksTypeDef* SCU_Clocks);
#endif

/* ************************END OF FILE********************* */




                                                                                                                                

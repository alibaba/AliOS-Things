/*********************************************************************************************************************
 * @file     system_XMC4700.c
 * @brief    CMSIS Cortex-M4 Device Peripheral Access Layer Header File for the Infineon XMC4700 Device Series
 * @version  V1.0.5
 * @date     26. sep 2017
 *
 * @cond
 *********************************************************************************************************************
 * Copyright (c) 2015-2017, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 * products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with
 * Infineon Technologies AG dave@infineon.com).
 *********************************************************************************************************************
 *
 ********************** Version History ***************************************
 * V1.0.0, 03. Sep 2015, Initial version
 * V1.0.1, 26. Jan 2016, Disable trap generation from clock unit
 * V1.0.2, 01. Jun 2016, Fix masking of OSCHPCTRL value
 * V1.0.3, 09. Feb 2017, Fix activation of USBPLL when SDMMC clock is enabled 
 * V1.0.4, 19. Jun 2017, Rely on cmsis_compiler.h instead of defining __WEAK 
 *                       Added support for ARM Compiler 6 (armclang) 
 * V1.0.5, 26. Sep 2017, Disable FPU if FPU_USED is zero    
 ******************************************************************************
 * @endcond
 */

/*******************************************************************************
 * Default clock initialization
 * fPLL = 288MHz => fSYS = 144MHz => fCPU = 144MHz
 *                                => fPB  = 144MHz
 *                                => fCCU = 144MHz
 *                                => fETH = 72MHz
 *               => fUSB = 48MHz
 *               => fEBU = 72MHz
 *
 * fUSBPLL Disabled, only enabled if SCU_CLK_USBCLKCR_USBSEL_USBPLL is selected
 *
 * fOFI = 24MHz => fWDT = 24MHz
 *******************************************************************************/

/*******************************************************************************
 * HEADER FILES
 *******************************************************************************/
#include <string.h>

#include <XMC4700.h>
#include "system_XMC4700.h"

/*******************************************************************************
 * MACROS
 *******************************************************************************/
#define CHIPID_LOC ((uint8_t *)0x20000000UL)

#define PMU_FLASH_WS          (0x4U)

#define FOSCREF               (2500000U)

#define DELAY_CNT_50US_50MHZ  (2500UL)
#define DELAY_CNT_150US_50MHZ (7500UL)
#define DELAY_CNT_50US_48MHZ  (2400UL)
#define DELAY_CNT_50US_72MHZ  (3600UL)
#define DELAY_CNT_50US_96MHZ  (4800UL)
#define DELAY_CNT_50US_120MHZ (6000UL)
#define DELAY_CNT_50US_144MHZ (7200UL)

#define SCU_PLL_PLLSTAT_OSC_USABLE  (SCU_PLL_PLLSTAT_PLLHV_Msk | \
                                     SCU_PLL_PLLSTAT_PLLLV_Msk | \
                                     SCU_PLL_PLLSTAT_PLLSP_Msk)

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/

/*
// <h> Clock configuration
*/

/*
//    <o> External crystal frequency [Hz]
//       <8000000=> 8MHz
//       <12000000=> 12MHz
//       <16000000=> 16MHz
//    <i> Defines external crystal frequency
//    <i> Default: 8MHz
*/
#define OSCHP_FREQUENCY (12000000U)

/* USB PLL settings, fUSBPLL = 48MHz and fUSBPLLVCO = 384 MHz */
/* Note: Implicit divider of 2 and fUSBPLLVCO >= 260 MHz and fUSBPLLVCO <= 520 MHz*/
#if OSCHP_FREQUENCY == 8000000U
#define USB_PDIV (1U)
#define USB_NDIV (95U)

#elif OSCHP_FREQUENCY == 12000000U
#define USB_PDIV (1U)
#define USB_NDIV (63U)

#elif OSCHP_FREQUENCY == 16000000U
#define USB_PDIV (1U)
#define USB_NDIV (47U)

#else
#error "External crystal frequency not supported"

#endif

/*
//    <o> Backup clock calibration mode
//       <0=> Factory calibration
//       <1=> Automatic calibration
//    <i> Default: Automatic calibration
*/
#define FOFI_CALIBRATION_MODE 1
#define FOFI_CALIBRATION_MODE_FACTORY 0
#define FOFI_CALIBRATION_MODE_AUTOMATIC 1

/*
//    <o> Standby clock (fSTDBY) source selection
//       <0=> Internal slow oscillator (32768Hz)
//       <1=> External crystal (32768Hz)
//    <i> Default: Internal slow oscillator (32768Hz)
*/
#define STDBY_CLOCK_SRC 0
#define STDBY_CLOCK_SRC_OSI 0
#define STDBY_CLOCK_SRC_OSCULP 1

/*
//    <o> PLL clock source selection
//       <0=> External crystal
//       <1=> Internal fast oscillator
//    <i> Default: External crystal
*/
#define PLL_CLOCK_SRC 0
#define PLL_CLOCK_SRC_EXT_XTAL 0
#define PLL_CLOCK_SRC_OFI 1

/* PLL settings, fPLL = 288MHz */
#if PLL_CLOCK_SRC == PLL_CLOCK_SRC_EXT_XTAL
#if OSCHP_FREQUENCY == 8000000U
#define PLL_PDIV (1U)
#define PLL_NDIV (71U)
#define PLL_K2DIV (0U)

#elif OSCHP_FREQUENCY == 12000000U
#define PLL_PDIV (1U)
#define PLL_NDIV (47U)
#define PLL_K2DIV (0U)

#elif OSCHP_FREQUENCY == 16000000U
#define PLL_PDIV (1U)
#define PLL_NDIV (35U)
#define PLL_K2DIV (0U)

#else
#error "External crystal frequency not supported"

#endif

#define VCO ((OSCHP_FREQUENCY / (PLL_PDIV + 1UL)) * (PLL_NDIV + 1UL))

#else /* PLL_CLOCK_SRC == PLL_CLOCK_SRC_EXT_XTAL */
#define PLL_PDIV (1U)
#define PLL_NDIV (23U)
#define PLL_K2DIV (0U)

#define VCO ((OFI_FREQUENCY / (PLL_PDIV + 1UL)) * (PLL_NDIV + 1UL))

#endif /* PLL_CLOCK_SRC == PLL_CLOCK_SRC_OFI */

#define PLL_K2DIV_24MHZ  ((VCO / OFI_FREQUENCY) - 1UL)
#define PLL_K2DIV_48MHZ  ((VCO / 48000000U) - 1UL)
#define PLL_K2DIV_72MHZ  ((VCO / 72000000U) - 1UL)
#define PLL_K2DIV_96MHZ  ((VCO / 96000000U) - 1UL)
#define PLL_K2DIV_120MHZ ((VCO / 120000000U) - 1UL)

#define SCU_CLK_CLKCLR_ENABLE_USBCLK SCU_CLK_CLKCLR_USBCDI_Msk
#define SCU_CLK_CLKCLR_ENABLE_MMCCLK SCU_CLK_CLKCLR_MMCCDI_Msk
#define SCU_CLK_CLKCLR_ENABLE_ETHCLK SCU_CLK_CLKCLR_ETH0CDI_Msk
#define SCU_CLK_CLKCLR_ENABLE_EBUCLK SCU_CLK_CLKCLR_EBUCDI_Msk
#define SCU_CLK_CLKCLR_ENABLE_CCUCLK SCU_CLK_CLKCLR_CCUCDI_Msk
#define SCU_CLK_CLKCLR_ENABLE_WDTCLK SCU_CLK_CLKCLR_WDTCDI_Msk

#define SCU_CLK_SYSCLKCR_SYSSEL_OFI      (0U << SCU_CLK_SYSCLKCR_SYSSEL_Pos)
#define SCU_CLK_SYSCLKCR_SYSSEL_PLL      (1U << SCU_CLK_SYSCLKCR_SYSSEL_Pos)

#define SCU_CLK_USBCLKCR_USBSEL_USBPLL   (0U << SCU_CLK_USBCLKCR_USBSEL_Pos)
#define SCU_CLK_USBCLKCR_USBSEL_PLL      (1U << SCU_CLK_USBCLKCR_USBSEL_Pos)

#define SCU_CLK_WDTCLKCR_WDTSEL_OFI      (0U << SCU_CLK_WDTCLKCR_WDTSEL_Pos)
#define SCU_CLK_WDTCLKCR_WDTSEL_STANDBY  (1U << SCU_CLK_WDTCLKCR_WDTSEL_Pos)
#define SCU_CLK_WDTCLKCR_WDTSEL_PLL      (2U << SCU_CLK_WDTCLKCR_WDTSEL_Pos)

#define SCU_CLK_EXTCLKCR_ECKSEL_SYS      (0U << SCU_CLK_EXTCLKCR_ECKSEL_Pos)
#define SCU_CLK_EXTCLKCR_ECKSEL_USBPLL   (2U << SCU_CLK_EXTCLKCR_ECKSEL_Pos)
#define SCU_CLK_EXTCLKCR_ECKSEL_PLL      (3U << SCU_CLK_EXTCLKCR_ECKSEL_Pos)

#define EXTCLK_PIN_P0_8  (1)
#define EXTCLK_PIN_P1_15 (2)

/*
//    <h> Clock tree
//        <o1.16> System clock source selection
//                      <0=> fOFI
//                      <1=> fPLL
//                      <i> Default: fPLL
//        <o1.0..7> System clock divider <1-256><#-1>
//                      <i> Default: 2
//        <o2.0> CPU clock divider
//                      <0=> fCPU = fSYS
//                      <1=> fCPU = fSYS / 2
//                      <i> Default: fCPU = fSYS
//        <o3.0>  Peripheral clock divider
//                      <0=> fPB = fCPU
//                      <1=> fPB = fCPU / 2
//                      <i> Default: fPB = fCPU
//        <o4.0>  CCU clock divider
//                      <0=> fCCU = fCPU
//                      <1=> fCCU = fCPU / 2
//                      <i> Default: fCCU = fCPU
//        <e.5> Enable WDT clock
//             <o5.16..17> WDT clock source <0=> fOFI
//                                          <1=> fSTDBY
//                                          <2=> fPLL
//                      <i> Default: fOFI
//             <o5.0..7> WDT clock divider <1-256><#-1>
//                      <i> Default: 1
//        </e>
//        <e.3> Enable EBU clock
//             <o6.0..5>  EBU clock divider  <1-64><#-1>
//             <i> Default: 4
//        </e>
//        <e.2> Enable ETH clock
//        </e>
//        <e.1> Enable MMC clock
//        </e>
//        <e.0> Enable USB clock
//             <o7.16> USB clock source <0=> fUSBPLL
//                                      <1=> fPLL
//             <i> Default: fPLL
//        </e>
//        <e8> Enable external clock
//             <o8.0..1> External Clock Source Selection
//                  <0=> fSYS
//                  <2=> fUSB
//                  <3=> fPLL
//                  <i> Default: fPLL
//             <o8.16..24> External Clock divider <1-512><#-1>
//                  <i> Default: 288
//                  <i> Only valid for USB PLL and PLL clocks
//             <o9.0> External Clock Pin Selection
//                  <0=> Disabled
//                  <1=> P0.8
//                  <2=> P1.15
//                  <i> Default: Disabled
//        </e>
//    </h>
*/
#define __CLKSET    (0x00000000UL)
#define __SYSCLKCR  (0x00010001UL)
#define __CPUCLKCR  (0x00000000UL)
#define __PBCLKCR   (0x00000000UL)
#define __CCUCLKCR  (0x00000000UL)
#define __WDTCLKCR  (0x00000000UL)
#define __EBUCLKCR  (0x00000003UL)
#define __USBCLKCR  (0x00010000UL)

#define __EXTCLKCR (0x01200003UL)
#define __EXTCLKPIN (0U)

/*
// </h>
*/

/*
//-------- <<< end of configuration section >>> ------------------
*/

#define ENABLE_PLL \
    (((__SYSCLKCR & SCU_CLK_SYSCLKCR_SYSSEL_Msk) == SCU_CLK_SYSCLKCR_SYSSEL_PLL) || \
     ((__CLKSET & SCU_CLK_CLKSET_EBUCEN_Msk) != 0) || \
     (((__CLKSET & SCU_CLK_CLKSET_USBCEN_Msk) != 0) && ((__USBCLKCR & SCU_CLK_USBCLKCR_USBSEL_Msk) == SCU_CLK_USBCLKCR_USBSEL_PLL)) || \
     (((__CLKSET & SCU_CLK_CLKSET_WDTCEN_Msk) != 0) && ((__WDTCLKCR & SCU_CLK_WDTCLKCR_WDTSEL_Msk) == SCU_CLK_WDTCLKCR_WDTSEL_PLL)))

#define ENABLE_USBPLL \
     ((((__CLKSET & SCU_CLK_CLKSET_USBCEN_Msk) != 0) && ((__USBCLKCR & SCU_CLK_USBCLKCR_USBSEL_Msk) == SCU_CLK_USBCLKCR_USBSEL_USBPLL)) || \
      (((__CLKSET & SCU_CLK_CLKSET_MMCCEN_Msk) != 0) && ((__USBCLKCR & SCU_CLK_USBCLKCR_USBSEL_Msk) == SCU_CLK_USBCLKCR_USBSEL_USBPLL)))
                     
#if ((__USBCLKCR & SCU_CLK_USBCLKCR_USBSEL_Msk) == SCU_CLK_USBCLKCR_USBSEL_USBPLL)
#define USB_DIV (3U)
#else
#define USB_DIV (5U)
#endif
    
/*******************************************************************************
 * GLOBAL VARIABLES
 *******************************************************************************/
#if defined ( __CC_ARM )
#if defined(XMC4700_E196x2048) || defined(XMC4700_F144x2048) || defined(XMC4700_F100x2048)
uint32_t SystemCoreClock __attribute__((at(0x2003FFC0)));
uint8_t g_chipid[16] __attribute__((at(0x2003FFC4)));
#elif defined(XMC4700_E196x1536) || defined(XMC4700_F144x1536) || defined(XMC4700_F100x1536)
uint32_t SystemCoreClock __attribute__((at(0x2002CFC0)));
uint8_t g_chipid[16] __attribute__((at(0x2002CFC4)));
#else
#error "system_XMC4700.c: device not supported" 
#endif
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#if defined(XMC4700_E196x2048) || defined(XMC4700_F144x2048) || defined(XMC4700_F100x2048)
uint32_t SystemCoreClock __attribute__((section(".ARM.__at_0x2003FFC0")));
uint8_t g_chipid[16] __attribute__((section(".ARM.__at_0x2003FFC4")));
#elif defined(XMC4700_E196x1536) || defined(XMC4700_F144x1536) || defined(XMC4700_F100x1536)
uint32_t SystemCoreClock __attribute__((section(".ARM.__at_0x2002CFC0")));
uint8_t g_chipid[16] __attribute__((section(".ARM.__at_0x2002CFC4")));
#else
#error "system_XMC4700.c: device not supported" 
#endif   
#elif defined ( __ICCARM__ )
#if defined(XMC4700_E196x2048) || defined(XMC4700_F144x2048) || defined(XMC4700_F100x2048) || \
    defined(XMC4700_E196x1536) || defined(XMC4700_F144x1536) || defined(XMC4700_F100x1536)
__no_init uint32_t SystemCoreClock;
__no_init uint8_t g_chipid[16];
#else
#error "system_XMC4700.c: device not supported" 
#endif    
#elif defined ( __GNUC__ )
#if defined(XMC4700_E196x2048) || defined(XMC4700_F144x2048) || defined(XMC4700_F100x2048) || \
    defined(XMC4700_E196x1536) || defined(XMC4700_F144x1536) || defined(XMC4700_F100x1536)
uint32_t SystemCoreClock __attribute__((section(".no_init")));
uint8_t g_chipid[16] __attribute__((section(".no_init")));
#else
#error "system_XMC4700.c: device not supported" 
#endif    
#elif defined ( __TASKING__ )
#if defined(XMC4700_E196x2048) || defined(XMC4700_F144x2048) || defined(XMC4700_F100x2048)
uint32_t SystemCoreClock __at( 0x2003FFC0 );
uint8_t g_chipid[16] __at( 0x2003FFC4 );
#elif defined(XMC4700_E196x1536) || defined(XMC4700_F144x1536) || defined(XMC4700_F100x1536)
uint32_t SystemCoreClock __at( 0x2002CFC0 );
uint8_t g_chipid[16] __at( 0x2002CFC4 );
#else
#error "system_XMC4700.c: device not supported" 
#endif    
#else
#error "system_XMC4700.c: compiler not supported" 
#endif    

extern uint32_t __Vectors;

/*******************************************************************************
 * LOCAL FUNCTIONS
 *******************************************************************************/
static void delay(uint32_t cycles)
{
  volatile uint32_t i;

  for(i = 0UL; i < cycles ;++i)
  {
    __NOP();
  }
}

/*******************************************************************************
 * API IMPLEMENTATION
 *******************************************************************************/

__WEAK void SystemInit(void)
{
  memcpy(g_chipid, CHIPID_LOC, 16);
  
  SystemCoreSetup();
  SystemCoreClockSetup(); 
}

__WEAK void SystemCoreSetup(void)
{
  uint32_t temp;
	
  /* relocate vector table */
  __disable_irq();
  SCB->VTOR = (uint32_t)(&__Vectors);
  __DSB();
  __enable_irq();
    
  /* __FPU_PRESENT = 1 defined in device header file */
  /* __FPU_USED value depends on compiler/linker options. */
  /* __FPU_USED = 0 if -mfloat-abi=soft is selected */
  /* __FPU_USED = 1 if -mfloat-abi=softfp or –mfloat-abi=hard */

#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
  SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                 (3UL << 11*2)  );               /* set CP11 Full Access */
#else
  SCB->CPACR = 0;
#endif

  /* Enable unaligned memory access - SCB_CCR.UNALIGN_TRP = 0 */
  SCB->CCR &= ~(SCB_CCR_UNALIGN_TRP_Msk);

  temp = FLASH0->FCON;
  temp &= ~FLASH_FCON_WSPFLASH_Msk;
  temp |= PMU_FLASH_WS;
  FLASH0->FCON = temp;
}

__WEAK void SystemCoreClockSetup(void)
{
#if FOFI_CALIBRATION_MODE == FOFI_CALIBRATION_MODE_FACTORY
  /* Enable factory calibration */
  SCU_PLL->PLLCON0 |= SCU_PLL_PLLCON0_FOTR_Msk;
#else
  /* Automatic calibration uses the fSTDBY */

  /* Enable HIB domain */
  /* Power up HIB domain if and only if it is currently powered down */
  if((SCU_POWER->PWRSTAT & SCU_POWER_PWRSTAT_HIBEN_Msk) == 0)
  {
    SCU_POWER->PWRSET |= SCU_POWER_PWRSET_HIB_Msk;

    while((SCU_POWER->PWRSTAT & SCU_POWER_PWRSTAT_HIBEN_Msk) == 0)
    {
      /* wait until HIB domain is enabled */
    }
  }

  /* Remove the reset only if HIB domain were in a state of reset */
  if((SCU_RESET->RSTSTAT) & SCU_RESET_RSTSTAT_HIBRS_Msk)
  {
    SCU_RESET->RSTCLR |= SCU_RESET_RSTCLR_HIBRS_Msk;
    delay(DELAY_CNT_150US_50MHZ);
  }
  
#if STDBY_CLOCK_SRC == STDBY_CLOCK_SRC_OSCULP
  /* Enable OSC_ULP */
  if ((SCU_HIBERNATE->OSCULCTRL & SCU_HIBERNATE_OSCULCTRL_MODE_Msk) != 0UL)
  {
    /*enable OSC_ULP*/
    while (SCU_GENERAL->MIRRSTS & SCU_GENERAL_MIRRSTS_OSCULCTRL_Msk)
    {
      /* check SCU_MIRRSTS to ensure that no transfer over serial interface is pending */
    }
    SCU_HIBERNATE->OSCULCTRL &= ~SCU_HIBERNATE_OSCULCTRL_MODE_Msk;

    /* Check if the clock is OK using OSCULP Oscillator Watchdog*/
    while (SCU_GENERAL->MIRRSTS & SCU_GENERAL_MIRRSTS_HDCR_Msk)
    {
      /* check SCU_MIRRSTS to ensure that no transfer over serial interface is pending */
    }
    SCU_HIBERNATE->HDCR |= SCU_HIBERNATE_HDCR_ULPWDGEN_Msk;

    /* wait till clock is stable */
    do
    {
      while (SCU_GENERAL->MIRRSTS & SCU_GENERAL_MIRRSTS_HDCLR_Msk)
      {
        /* check SCU_MIRRSTS to ensure that no transfer over serial interface is pending */
      }
      SCU_HIBERNATE->HDCLR |= SCU_HIBERNATE_HDCLR_ULPWDG_Msk;

      delay(DELAY_CNT_50US_50MHZ);

    } while ((SCU_HIBERNATE->HDSTAT & SCU_HIBERNATE_HDSTAT_ULPWDG_Msk) != 0UL);

  }

  /* now OSC_ULP is running and can be used*/
  /* Select OSC_ULP as the clock source for RTC and STDBY*/
  while (SCU_GENERAL->MIRRSTS & SCU_GENERAL_MIRRSTS_HDCR_Msk)
  {
    /* check SCU_MIRRSTS to ensure that no transfer over serial interface is pending */
  }
  SCU_HIBERNATE->HDCR |= SCU_HIBERNATE_HDCR_RCS_Msk | SCU_HIBERNATE_HDCR_STDBYSEL_Msk;
#endif /* STDBY_CLOCK_SRC == STDBY_CLOCK_SRC_OSCULP */

  /* Enable automatic calibration of internal fast oscillator */
  SCU_PLL->PLLCON0 |= SCU_PLL_PLLCON0_AOTREN_Msk;
#endif /* FOFI_CALIBRATION_MODE == FOFI_CALIBRATION_MODE_AUTOMATIC */

  delay(DELAY_CNT_50US_50MHZ);

#if ENABLE_PLL

  /* enable PLL */
  SCU_PLL->PLLCON0 &= ~(SCU_PLL_PLLCON0_VCOPWD_Msk | SCU_PLL_PLLCON0_PLLPWD_Msk);

#if PLL_CLOCK_SRC != PLL_CLOCK_SRC_OFI
  /* enable OSC_HP */
  if ((SCU_OSC->OSCHPCTRL & SCU_OSC_OSCHPCTRL_MODE_Msk) != 0U)
  {
    SCU_OSC->OSCHPCTRL &= ~(SCU_OSC_OSCHPCTRL_MODE_Msk | SCU_OSC_OSCHPCTRL_OSCVAL_Msk);
    SCU_OSC->OSCHPCTRL |= ((OSCHP_GetFrequency() / FOSCREF) - 1UL) << SCU_OSC_OSCHPCTRL_OSCVAL_Pos;

    /* select OSC_HP clock as PLL input */
    SCU_PLL->PLLCON2 &= ~SCU_PLL_PLLCON2_PINSEL_Msk;

    /* restart OSC Watchdog */
    SCU_PLL->PLLCON0 &= ~SCU_PLL_PLLCON0_OSCRES_Msk;

    while ((SCU_PLL->PLLSTAT & SCU_PLL_PLLSTAT_OSC_USABLE) != SCU_PLL_PLLSTAT_OSC_USABLE)
    {
      /* wait till OSC_HP output frequency is usable */
    }   
  }
#else /* PLL_CLOCK_SRC != PLL_CLOCK_SRC_OFI */

  /* select backup clock as PLL input */
  SCU_PLL->PLLCON2 |= SCU_PLL_PLLCON2_PINSEL_Msk;
#endif

  /* Go to bypass the Main PLL */
  SCU_PLL->PLLCON0 |= SCU_PLL_PLLCON0_VCOBYP_Msk;

  /* disconnect Oscillator from PLL */
  SCU_PLL->PLLCON0 |= SCU_PLL_PLLCON0_FINDIS_Msk;

  /* Setup divider settings for main PLL */
  SCU_PLL->PLLCON1 = ((PLL_NDIV << SCU_PLL_PLLCON1_NDIV_Pos) |
                      (PLL_K2DIV_24MHZ << SCU_PLL_PLLCON1_K2DIV_Pos) |
                      (PLL_PDIV << SCU_PLL_PLLCON1_PDIV_Pos));

  /* Set OSCDISCDIS */
  SCU_PLL->PLLCON0 |= SCU_PLL_PLLCON0_OSCDISCDIS_Msk;

  /* connect Oscillator to PLL */
  SCU_PLL->PLLCON0 &= ~SCU_PLL_PLLCON0_FINDIS_Msk;

  /* restart PLL Lock detection */
  SCU_PLL->PLLCON0 |= SCU_PLL_PLLCON0_RESLD_Msk;

  while ((SCU_PLL->PLLSTAT & SCU_PLL_PLLSTAT_VCOLOCK_Msk) == 0U)
  {
    /* wait for PLL Lock at 24MHz*/
  }

  /* Disable bypass- put PLL clock back */
  SCU_PLL->PLLCON0 &= ~SCU_PLL_PLLCON0_VCOBYP_Msk;
  while ((SCU_PLL->PLLSTAT & SCU_PLL_PLLSTAT_VCOBYST_Msk) != 0U)
  {
    /* wait for normal mode */
  } 
#endif /* ENABLE_PLL */

  /* Before scaling to final frequency we need to setup the clock dividers */
  SCU_CLK->SYSCLKCR = __SYSCLKCR;
  SCU_CLK->PBCLKCR = __PBCLKCR;
  SCU_CLK->CPUCLKCR = __CPUCLKCR;
  SCU_CLK->CCUCLKCR = __CCUCLKCR;
  SCU_CLK->WDTCLKCR = __WDTCLKCR;
  SCU_CLK->EBUCLKCR = __EBUCLKCR;
  SCU_CLK->USBCLKCR = __USBCLKCR | USB_DIV;
  SCU_CLK->EXTCLKCR = __EXTCLKCR;

#if ENABLE_PLL
  /* PLL frequency stepping...*/
  /* Reset OSCDISCDIS */
  SCU_PLL->PLLCON0 &= ~SCU_PLL_PLLCON0_OSCDISCDIS_Msk;
  
  SCU_PLL->PLLCON1 = ((PLL_NDIV << SCU_PLL_PLLCON1_NDIV_Pos) |
	                  (PLL_K2DIV_48MHZ << SCU_PLL_PLLCON1_K2DIV_Pos) |
	                  (PLL_PDIV << SCU_PLL_PLLCON1_PDIV_Pos));

  delay(DELAY_CNT_50US_48MHZ);

  SCU_PLL->PLLCON1 = ((PLL_NDIV << SCU_PLL_PLLCON1_NDIV_Pos) |
	                  (PLL_K2DIV_72MHZ << SCU_PLL_PLLCON1_K2DIV_Pos) |
	                  (PLL_PDIV << SCU_PLL_PLLCON1_PDIV_Pos));

  delay(DELAY_CNT_50US_72MHZ);

  SCU_PLL->PLLCON1 = ((PLL_NDIV << SCU_PLL_PLLCON1_NDIV_Pos) |
	                  (PLL_K2DIV_96MHZ << SCU_PLL_PLLCON1_K2DIV_Pos) |
	                  (PLL_PDIV << SCU_PLL_PLLCON1_PDIV_Pos));

  delay(DELAY_CNT_50US_96MHZ);

  SCU_PLL->PLLCON1 = ((PLL_NDIV << SCU_PLL_PLLCON1_NDIV_Pos) |
	                  (PLL_K2DIV_120MHZ << SCU_PLL_PLLCON1_K2DIV_Pos) |
	                  (PLL_PDIV << SCU_PLL_PLLCON1_PDIV_Pos));

  delay(DELAY_CNT_50US_120MHZ);

  SCU_PLL->PLLCON1 = ((PLL_NDIV << SCU_PLL_PLLCON1_NDIV_Pos) |
	                  (PLL_K2DIV << SCU_PLL_PLLCON1_K2DIV_Pos) |
	                  (PLL_PDIV << SCU_PLL_PLLCON1_PDIV_Pos));

  delay(DELAY_CNT_50US_144MHZ);
  
#endif /* ENABLE_PLL */

#if ENABLE_USBPLL
  /* enable USB PLL first */
  SCU_PLL->USBPLLCON &= ~(SCU_PLL_USBPLLCON_VCOPWD_Msk | SCU_PLL_USBPLLCON_PLLPWD_Msk);

  /* USB PLL uses as clock input the OSC_HP */
  /* check and if not already running enable OSC_HP */
  if ((SCU_OSC->OSCHPCTRL & SCU_OSC_OSCHPCTRL_MODE_Msk) != 0U)
  {
    /* check if Main PLL is switched on for OSC WDG*/
    if ((SCU_PLL->PLLCON0 &(SCU_PLL_PLLCON0_VCOPWD_Msk | SCU_PLL_PLLCON0_PLLPWD_Msk)) != 0UL)
    {
      /* enable PLL first */
      SCU_PLL->PLLCON0 &= ~(SCU_PLL_PLLCON0_VCOPWD_Msk | SCU_PLL_PLLCON0_PLLPWD_Msk);
    }

    SCU_OSC->OSCHPCTRL &= ~(SCU_OSC_OSCHPCTRL_MODE_Msk | SCU_OSC_OSCHPCTRL_OSCVAL_Msk);
    SCU_OSC->OSCHPCTRL |= ((OSCHP_GetFrequency() / FOSCREF) - 1UL) << SCU_OSC_OSCHPCTRL_OSCVAL_Pos;

    /* restart OSC Watchdog */
    SCU_PLL->PLLCON0 &= ~SCU_PLL_PLLCON0_OSCRES_Msk;

    while ((SCU_PLL->PLLSTAT & SCU_PLL_PLLSTAT_OSC_USABLE) != SCU_PLL_PLLSTAT_OSC_USABLE)
    {
      /* wait till OSC_HP output frequency is usable */
    }
  }


  /* Setup USB PLL */
  /* Go to bypass the USB PLL */
  SCU_PLL->USBPLLCON |= SCU_PLL_USBPLLCON_VCOBYP_Msk;

  /* disconnect Oscillator from USB PLL */
  SCU_PLL->USBPLLCON |= SCU_PLL_USBPLLCON_FINDIS_Msk;

  /* Setup Divider settings for USB PLL */
  SCU_PLL->USBPLLCON = ((USB_NDIV << SCU_PLL_USBPLLCON_NDIV_Pos) |
                        (USB_PDIV << SCU_PLL_USBPLLCON_PDIV_Pos));

  /* Set OSCDISCDIS */
  SCU_PLL->USBPLLCON |= SCU_PLL_USBPLLCON_OSCDISCDIS_Msk;

  /* connect Oscillator to USB PLL */
  SCU_PLL->USBPLLCON &= ~SCU_PLL_USBPLLCON_FINDIS_Msk;

  /* restart PLL Lock detection */
  SCU_PLL->USBPLLCON |= SCU_PLL_USBPLLCON_RESLD_Msk;

  while ((SCU_PLL->USBPLLSTAT & SCU_PLL_USBPLLSTAT_VCOLOCK_Msk) == 0U)
  {
    /* wait for PLL Lock */
  }  
#endif


  /* Enable selected clocks */
  SCU_CLK->CLKSET = __CLKSET;

#if __EXTCLKPIN != 0
#if __EXTCLKPIN == EXTCLK_PIN_P1_15
  /* P1.15 */
  PORT1->PDR1 &= ~PORT1_PDR1_PD15_Msk;
  PORT1->IOCR12 = (PORT1->IOCR12 & ~PORT0_IOCR12_PC15_Msk) | (0x11U << PORT0_IOCR12_PC15_Pos);
#else
  /* P0.8 */
  PORT0->HWSEL &= ~PORT0_HWSEL_HW8_Msk;
  PORT0->PDR1 &= ~PORT0_PDR1_PD8_Msk;
  PORT0->IOCR8 = (PORT0->IOCR8 & ~PORT0_IOCR8_PC8_Msk) | (0x11U << PORT0_IOCR8_PC8_Pos);
#endif
#endif  /* ENABLE_EXTCLK == 1  */

  SystemCoreClockUpdate();
}

__WEAK void SystemCoreClockUpdate(void)
{
  uint32_t pdiv;
  uint32_t ndiv;
  uint32_t kdiv;
  uint32_t temp;

  if (SCU_CLK->SYSCLKCR & SCU_CLK_SYSCLKCR_SYSSEL_Msk)
  {
    /* fPLL is clock source for fSYS */
    if(SCU_PLL->PLLCON2 & SCU_PLL_PLLCON2_PINSEL_Msk)
    {
      /* PLL input clock is the backup clock (fOFI) */
      temp = OFI_FREQUENCY;
    }
    else
    {
      /* PLL input clock is the high performance osicllator (fOSCHP) */
      temp = OSCHP_GetFrequency();
    }

    /* check if PLL is locked */
    if (SCU_PLL->PLLSTAT & SCU_PLL_PLLSTAT_VCOLOCK_Msk)
    {
      /* PLL normal mode */
      /* read back divider settings */
      pdiv = ((SCU_PLL->PLLCON1 & SCU_PLL_PLLCON1_PDIV_Msk) >> SCU_PLL_PLLCON1_PDIV_Pos) + 1;
      ndiv = ((SCU_PLL->PLLCON1 & SCU_PLL_PLLCON1_NDIV_Msk) >> SCU_PLL_PLLCON1_NDIV_Pos) + 1;
      kdiv = ((SCU_PLL->PLLCON1 & SCU_PLL_PLLCON1_K2DIV_Msk) >> SCU_PLL_PLLCON1_K2DIV_Pos) + 1;

      temp = (temp / (pdiv * kdiv)) * ndiv;
    }
    else
    {
      /* PLL prescalar mode */
      /* read back divider settings */
      kdiv  = ((SCU_PLL->PLLCON1 & SCU_PLL_PLLCON1_K1DIV_Msk) >> SCU_PLL_PLLCON1_K1DIV_Pos) + 1;
      
      temp = (temp / kdiv);
    }
  }
  else
  {
    /* fOFI is clock source for fSYS */    
    temp = OFI_FREQUENCY;
  }

  temp = temp / ((SCU_CLK->SYSCLKCR & SCU_CLK_SYSCLKCR_SYSDIV_Msk) + 1);
  temp = temp / ((SCU_CLK->CPUCLKCR & SCU_CLK_CPUCLKCR_CPUDIV_Msk) + 1);

  SystemCoreClock = temp;
}

__WEAK uint32_t OSCHP_GetFrequency(void)
{
  return OSCHP_FREQUENCY;
}

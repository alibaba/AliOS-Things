/*********************************************************************************************************************
 * @file     system_XMC1300.c
 * @brief    Device specific initialization for the XMC1300-Series according to CMSIS
 * @version  V1.11
 * @date     19 Jun 2017
 *
 * @cond
 *********************************************************************************************************************
 * Copyright (c) 2012-2017, Infineon Technologies AG
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
 * *************************** Change history ********************************
 * V1.2, 13 Dec 2012, PKB : Created change history table
 * V1.3, 20 Dec 2012, PKB : Fixed SystemCoreClock computation
 * V1.4, 02 Feb 2013, PKB : SCU_CLOCK -> SCU_CLK
 * V1.5, 27 Nov 2013, DNE : Comments added in SystemInit function for MCLK support
 * V1.6, 19 Feb 2014, JFT : Fixed SystemCoreClock when FDIV != 0 
 * V1.7, 11 Dec 2014, JFT : SystemCoreClockSetup, SystemCoreSetup as weak functions
 * V1.8, 03 Sep 2015, JFT : Override values of CLOCK_VAL1 and CLOCK_VAL2 defined in vector table (startup.s)
 *                          MCLK = 32MHz, PCLK = 64MHz
 * V1.9, 31 Mar 2016, JFT : Fix flash wait states to 1 cycle
 * V1.10,22 Aug 2016, JFT : Update coding for fixed flash wait states using new macros in device header file
 *                          Add macro guard USE_DYNAMIC_FLASH_WS. If defined in compiler options, adaptive wait states
 *                          are used for read accesses to the flash memory. Otherwise a fixed 1 WS is used.
 * V1.11,19 Jun 2017, Rely on cmsis_compiler.h instead of defining __WEAK
 *                    Added support for ARM Compiler 6 (armclang) 
 *
 * @endcond 
 */

/*******************************************************************************
 * HEADER FILES
 *******************************************************************************/

#include <XMC1300.h>
#include "system_XMC1300.h"

/*******************************************************************************
 * MACROS
 *******************************************************************************/
#define DCO1_FREQUENCY (64000000U)

/*******************************************************************************
 * GLOBAL VARIABLES
 *******************************************************************************/

#if defined ( __CC_ARM )
uint32_t SystemCoreClock __attribute__((at(0x20003FFC)));
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
uint32_t SystemCoreClock __attribute__((section(".ARM.__at_0x20003FFC")));
#elif defined ( __ICCARM__ )
__no_init uint32_t SystemCoreClock;
#elif defined ( __GNUC__ )
uint32_t SystemCoreClock __attribute__((section(".no_init")));
#elif defined ( __TASKING__ )
uint32_t SystemCoreClock __at( 0x20003FFC );
#endif

/*******************************************************************************
 * API IMPLEMENTATION
 *******************************************************************************/

__WEAK void SystemInit(void)
{    
  SystemCoreSetup();
  SystemCoreClockSetup();
}

__WEAK void SystemCoreSetup(void)
{
#ifndef USE_DYNAMIC_FLASH_WS
  /* Fix flash wait states to 1 cycle (see DS Addendum) */
  NVM->NVMCONF |= NVM_NVMCONF_WS_Msk;
  NVM->CONFIG1 |= NVM_CONFIG1_FIXWS_Msk;
#endif
}

__WEAK void SystemCoreClockSetup(void)
{
  /* Override values of CLOCK_VAL1 and CLOCK_VAL2 defined in vector table */
  /* MCLK = 32MHz, PCLK = 64MHz */
  
  SCU_GENERAL->PASSWD = 0x000000C0UL; /* disable bit protection */
  SCU_CLK->CLKCR = 0x3FF10100UL;
  while((SCU_CLK->CLKCR & SCU_CLK_CLKCR_VDDC2LOW_Msk));
  SCU_GENERAL->PASSWD = 0x000000C3UL; /* enable bit protection */

  SystemCoreClockUpdate();
}

__WEAK void SystemCoreClockUpdate(void)
{
  static uint32_t IDIV, FDIV;

  IDIV = ((SCU_CLK->CLKCR) & SCU_CLK_CLKCR_IDIV_Msk) >> SCU_CLK_CLKCR_IDIV_Pos;
  FDIV = ((SCU_CLK->CLKCR) & SCU_CLK_CLKCR_FDIV_Msk) >> SCU_CLK_CLKCR_FDIV_Pos;
  
  if (IDIV != 0)
  {
    /* Fractional divider is enabled and used */
    SystemCoreClock = ((DCO1_FREQUENCY << 6U) / ((IDIV << 8) + FDIV)) << 1U;
  }
  else
  {
    /* Fractional divider bypassed. Simply divide DCO_DCLK by 2 */
    SystemCoreClock = DCO1_FREQUENCY >> 1U;
  }
}

/******************************************************************************
* Copyright (C) 2017, Huada Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Huada Semiconductor Co.,Ltd ("HDSC").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with HDSC
* components. This software is licensed by HDSC to be adapted only
* for use in systems utilizing HDSC components. HDSC shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. HDSC is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* Disclaimer:
* HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING    ANY ACOOMPANYING WRITTEN MATERIALS),
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
* WARRANTY OF NONINFRINGEMENT.
* HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
* SAVINGS OR PROFITS,
* EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
* FROM, THE SOFTWARE.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
/******************************************************************************/
/** \file interrupts_hc32l110.c
 **
 ** Interrupt management
 ** @link Driver Group Some description @endlink
 **
 **   - 2017-04-29  1.0  Ronnie     First version.
 **
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files                                                              
 ******************************************************************************/
#include "HC32L136.h"
#include "base_types.h"
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"


void EnableNvic(IRQn_Type enIrq,uint8_t u8Level,boolean_t bEn)
{
    NVIC_ClearPendingIRQ(enIrq);
    NVIC_SetPriority(enIrq, u8Level);
    if (TRUE == bEn)
    {
        NVIC_EnableIRQ(enIrq);
    }else{
        NVIC_DisableIRQ(enIrq);
    }
}
void SysTick_Handler(void)
{
  HAL_IncTick();
  krhino_intrpt_enter();
  krhino_tick_proc();
  krhino_intrpt_exit();
  //HAL_SYSTICK_IRQHandler();
}
void HardFault_Handler(void)
{
    volatile int a = 0;
    while( 0 == a); 
}

void PORTA_IRQHandler(void)
{
    
}

void PORTB_IRQHandler(void)
{
    
}

void PORTC_IRQHandler(void)
{
    
}

void PORTD_IRQHandler(void)
{
 
}

void UART0_IRQHandler(void)
{

}
void LPUART1_IRQHandler(void)
{

}
void TIM1_IRQHandler(void)
{
			  
}
void TIM2_IRQHandler(void)
{
 			  
}
void LPTIM_IRQHandler(void)
{
    
}

void PCA_IRQHandler(void)
{

}
//PCNT 中断服务程序
void PCNT_IRQHandler(void)
{
			
}
void RTC_IRQHandler(void)
{

}
void LVD_IRQHandler(void)
{
  if(M0P_LVD->IFR_f.INTF)
  {
	  M0P_LVD->IFR_f.INTF = 0;
  
	}
}
void CLKTRIM_IRQHandler(void)
{
   if(M0P_CLK_TRIM->IFR_f.XTL_FAULT)
   {
	    M0P_CLK_TRIM->ICLR_f.XTL_FAULT_CLR = 0;

	 }		 
}
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/

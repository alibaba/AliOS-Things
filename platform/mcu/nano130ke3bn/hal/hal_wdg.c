/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "objects.h"
#include "nu_modutil.h"
#include "nu_bitutil.h"
#include "hal_status.h"
#include <k_api.h>
#include "aos/hal/wdg.h"
#include "board.h"

static uint32_t g_bWdtInited=0;
static uint32_t g_u32WDTINTCounts=0;
static uint32_t g_u32TimeCnt=0;

/**
 * @brief       IRQ Handler for WDT Interrupt
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The WDT_IRQHandler is default IRQ of WDT, declared in startup_M480.s.
 */
void WDT_IRQHandler(void)
{
    if ( g_u32WDTINTCounts < g_u32TimeCnt )
        WDT_RESET_COUNTER();

    if(WDT_GET_TIMEOUT_INT_FLAG() == 1)
    {
        /* Clear WDT time-out interrupt flag */
        WDT_CLEAR_TIMEOUT_INT_FLAG();
				g_u32WDTINTCounts++;
    }

    if(WDT_GET_TIMEOUT_WAKEUP_FLAG() == 1)
    {
        /* Clear WDT time-out wake-up flag */
        WDT_CLEAR_TIMEOUT_WAKEUP_FLAG();
    }
}


/**
 * This function will initialize the on board CPU hardware watch dog
 *
 * @param[in]  wdg  the watch dog device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_wdg_init(wdg_dev_t *wdg)
{
	if ( !wdg )
		goto exit_hal_wdg_init;
	
	if ( !g_bWdtInited )
	{
		/* Enable IP module clock */
		CLK_EnableModuleClock(WDT_MODULE);

		/* Peripheral clock source */
		CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDTSEL_LIRC, 0);
		
		/* To check if system has been reset by WDT time-out reset or not */
		if ( WDT_GET_RESET_FLAG() == 1 )
		{
				WDT_CLEAR_RESET_FLAG();
				printf("*** System has been reset by WDT time-out event ***\n\n");
				//while(1);
		}
		/* Enable WDT NVIC */
		NVIC_EnableIRQ(WDT_IRQn);

		/* Because of all bits can be written in WDT Control Register are write-protected;
			 To program it needs to disable register protection first. */
		SYS_UnlockReg();


    /* Time-out interval is 2^14 * WDT clock */
    /* around 1.6384 ~ 1.7408 s              */
    WDT_Open ( WDT_TIMEOUT_2POW14, WDT_RESET_DELAY_18CLK, TRUE, TRUE );
	 
		/* Configure WDT settings and start WDT counting */
		g_u32TimeCnt = (wdg->config.timeout / 1.638);		/* Assume the timeout definition is in mili-second. */

		/* Enable WDT interrupt function */
		WDT_EnableInt();
	}
	
	g_bWdtInited = 1;

	return HAL_OK;

exit_hal_wdg_init:
	
	return HAL_ERROR;
}

/**
 * Reload watchdog counter.
 *
 * @param[in]  wdg  the watch dog device
 */
void hal_wdg_reload(wdg_dev_t *wdg)
{
		if ( g_bWdtInited ) 
		{
			g_u32WDTINTCounts = 0;
			WDT_RESET_COUNTER(); /* Do WDT reload */
		}
}

/**
 * This function performs any platform-specific cleanup needed for hardware watch dog.
 *
 * @param[in]  wdg  the watch dog device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
	if ( g_bWdtInited ) 
	{
		WDT_DisableInt();

		/* Enable WDT NVIC */
		NVIC_DisableIRQ(WDT_IRQn);

		/* Clear WDT time-out flag */
		WDT_CLEAR_RESET_FLAG();	

		/* Disable IP module clock */
		CLK_DisableModuleClock(WDT_MODULE);
	}
	
	g_bWdtInited = 0;

	return HAL_OK;
}

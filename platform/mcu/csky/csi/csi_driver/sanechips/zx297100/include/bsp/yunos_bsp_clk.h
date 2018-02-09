/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YUNOS_BSP_CLK_H
#define YUNOS_BSP_CLK_H

typedef enum {
	CK_TIMER0_PCLK,
	CK_TIMER1_PCLK,

	CK_TIMER0_WCLK,
	CK_TIMER1_WCLK,

	AP_WDT_PCLK,
	AP_WDT_WCLK,

	AP_IRAM1_IROM_CLK,
    AP_IRAM2_CLK,
    
	UART0_PCLK,
	UART1_PCLK,
	UART2_PCLK,
	LP_UART_PCLK,
	UART0_WCLK_L1,
	UART0_WCLK_L2,
	UART1_WCLK,
	UART2_WCLK,
	LP_UART_WCLK,

	I2C0_PCLK,
	I2C1_PCLK,
	I2C0_WCLK,
	I2C1_WCLK,

	SSP0_PCLK,
	SSP1_PCLK,
	SSP2_PCLK,
	SSP0_WCLK,
	SSP1_WCLK,
	SSP2_WCLK,

	GPIO_PD_PCLK,

	ADC_PCLK,
	ADC_WCLK,

	DAC_PCLK,
	DAC_WCLK,

	AD_TIMER0_PCLK,
	AD_TIMER1_PCLK,
	AD_TIMER0_WCLK,
	AD_TIMER1_WCLK,

	AP_DMA_PCLK,

	LP_TIMER_PCLK,
	LP_TIMER_WCLK,
} clk_name;

/*
*  bits domain definition  for clock frequency
*  base clock name   same clock number  reserved       clock selection   frequency division
*  31...28 27...24   		 23...20            19...16        15...12 11...8         7...4 3...0
*  invalid value:
*  		0xff              	0xf or 0x0       0x0 or ignore         0xff                    0xff
*/
typedef enum
{
	FIXED_FREQ = 0xffffffff,

    /*ck timer0&1/rm timer0&1/ceva timer0&1*/
    WCLK_TIMER_32K  = (CK_TIMER0_WCLK << 24) | 0x00600000,
    WCLK_TIMER_19M2 = (CK_TIMER0_WCLK << 24) | 0x06000100,    /* 1 div*/
	WCLK_TIMER_9M6  = (CK_TIMER0_WCLK << 24) | 0x00600101,     /* 2 div*/

    /*ap wdt/cp wdt*/
    WCLK_WDT_32K  	= (AP_WDT_WCLK << 24) | 0x00200000,
    WCLK_WDT_19M2  	= (AP_WDT_WCLK << 24) | 0x00200100,
    WCLK_WDT_9M6  	= (AP_WDT_WCLK << 24) | 0x00200101,

    /*i2c0&1*/
    WCLK_I2C_76M8	= (I2C0_WCLK << 24) | 0x00200100,
    WCLK_I2C_19M2 	= (I2C0_WCLK << 24) | 0x00200000,

    /*uart0&1&2&lp uart*/
    WCLK_UART_19M2  = (UART0_WCLK_L1 << 24) | 0x00400000,
    WCLK_UART_76M8 	= (UART0_WCLK_L1 << 24) | 0x00400100,
    /*only for uart0& lp uart*/
	WCLK_UART_32K 	= (UART0_WCLK_L1 << 24) | 0x00400200,

	/*ssp0&1&2*/
    WCLK_SSP_19M2 	= (SSP0_WCLK << 24) | 0x00300000,
    WCLK_SSP_76M8  	= (SSP0_WCLK << 24) | 0x00300100,
	WCLK_SSP_61m44 	= (SSP0_WCLK << 24) | 0x00300200,

	/*adc*/
	WCLK_ADC_19M2 	= (ADC_WCLK <<24) | 0x00100000,
	WCLK_ADC_38M4 	= (ADC_WCLK <<24) | 0x00100100,
	WCLK_ADC_61M44 	= (ADC_WCLK <<24) | 0x00100200,
	WCLK_ADC_32K 	= (ADC_WCLK <<24) | 0x00100300,

	/*dac*/
	WCLK_DAC_19M2 	= (DAC_WCLK <<24) | 0x001001000,
	WCLK_DAC_32K 	= (DAC_WCLK <<24) | 0x00100200,

	/*ad timer0&1*/
	WCLK_AD_TIMER_32K 	= (AD_TIMER0_WCLK <<24) | 0x002000000,
	WCLK_AD_TIMER_76M8 	= (AD_TIMER0_WCLK <<24) | 0x00200100,
	WCLK_AD_TIMER_19M2 	= (AD_TIMER0_WCLK <<24) | 0x00200200,

	/*lp timer*/
	WCLK_LP_TIMER_32K 	= (LP_TIMER_WCLK <<24) | 0x001000000,
	WCLK_LP_TIMER_76M8 	= (LP_TIMER_WCLK <<24) | 0x001000100,
	WCLK_LP_TIMER_19M2 	= (LP_TIMER_WCLK <<24) | 0x001000200,

} clk_freq_sel;

/**
 * This function is used to set software clock gate
 * @param[in]	name	the clock name to set
 * @param[in]	ctrl	 	0 enbale;     1 disable
 * @return			0 success; -1 failure
 */
int yunos_clk_sw_gate_ctrl(clk_name name, uint32_t ctrl);

/**
 * This function is used to set clock autogate
 * @param[in]	name	the clock name to set
 * @param[in]	ctrl	 	0 enbale;     1 disable
 * @return			0 success; -1 failure
 */
int yunos_clk_hw_gate_ctrl(clk_name name, uint32_t ctrl);

/**
 * This function is used to config clock reset ctrl
 * @param[in]	name	the clock name to set
 * @param[in]	ctrl	 	0 enbale;     1 disable
 * @return			0 success; -1 failure
 */
int yunos_clk_reset(clk_name name, uint32_t ctrl);

/**
 * This function is used to set clock freq
 * @param[in]	name	the clock name to set
 * @param[in]	freq		the corresponding freq to set
 * @return			0 success; -1 failure
 */
int yunos_clk_set_freq(clk_name name, clk_freq_sel freq);

#endif



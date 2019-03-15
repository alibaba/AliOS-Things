/**
 * File: reg_define.h
 * Brief: registers define of Sanechips
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author: Dongdong Zhang <zhang.dongdong16@sanechips.com.cn>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _REG_DEFINE_H
#define _REG_DEFINE_H

/****************************************************************************
* 	                         Macros
****************************************************************************/
	/*64K, soft only use 8k*/
#define IROM_BASEADDR        	(0x0)
	/*64K*/
#define IRAM_AP1_BASEADDR       (0x10000)
	
	
	/*16K*/
#define IRAM_AP2_BASEADDR       (0x20000)
	
	
	
	/*4K*/
#define EFLASH1_INFO_BASEADDR   (0xF000000)
	/*1216K*/
#define EFLASH1_BASEADDR        (0xFED0000)
	
	
	/*256K*/
#define EFLASH2_BASEADDR        (0x10000000)
	/*4K*/
#define EFLASH2_CTRL_BASEADDR  	(0x10080000)
	/*1K*/
#define EFLASH2_INFO_BASEADDR  	(0x10081000)
	
	
	/*4K*/
#define CK_TIMER0_BASEADDR     	(0x22000000)
	/*4K*/
#define CK_TIMER1_BASEADDR     	(0x22001000)
	/*4K*/
#define M0_TIMER0_BASEADDR     	(0x22002000)
	/*4K*/
#define M0_TIMER1_BASEADDR     	(0x22003000)
	/*4K*/
#define AP_WDT_BASEADDR        	(0x22004000)
	/*4K*/
#define AP_CRPM_BASEADDR        (0x22005000)
	/*4K*/
#define ICU_M0_BASEADDR        	(0x22007000)
	/*4K*/
#define ICU_CK_BASEADDR        	(0x22008000)
	/*4K*/
#define AP_DMACFG_BASEADDR    	(0x22010000)
	
	
	/*4K*/
#define GPIO_AO_BASEADDR        (0x20130000)
	/*4K*/
#define PAD_CTRL_AO_BASEADDR  	(0x20131000)
	/*4K*/
#define PCU_AO_BASEADDR        	(0x20132000)
	/*4K*/
#define LP_TIMER_BASEADDR      	(0x20133000)
	/*4K*/
#define LP_UART_BASEADDR        (0x20134000)
	/*4K*/
#define A0_CRPM_BASEADDR        (0x20136000)
	
	
	/*4K*/
#define SBY_CRPM_BASEADDR      	(0x20140000)
	/*4K*/
#define PCU_SBY_BASEADDR        (0x20141000)
	/*4K*/
#define IWDT_BASEADDR        	(0x20143000)
	/*4K*/
#define RTC_BASEADDR        	(0x20144000)
	/*4K*/
#define LPM_BASEADDR        	(0x20145000)
	/*4K*/
#define PMU_CTRL_BASEADDR     	(0x20146000)
	
	
	/*4K*/
#define EFLASH1_CTRL_BASEADDR  	(0x21301000)
	/*4K*/
#define ICP_BASEADDR        	(0x21302000)
	/*4K*/
#define PAD_CTRL_PD_BASEADDR    (0x21303000)
	/*4K*/
#define TOP_CRPM_BASEADDR     	(0x21306000)
	/*4K*/
#define SOC_SYS_BASEADDR        (0x21307000)
	
	
	/*4K*/
#define LSP_CRPM_BASEADDR    	(0x21400000)
	/*4K*/
#define UART0_BASEADDR        	(0x21401000)
	/*4K*/
#define UART1_BASEADDR        	(0x21402000)
	/*4K*/
#define SSP0_BASEADDR        	(0x21403000)
	/*4K*/
#define SSP1_BASEADDR        	(0x21404000)
	/*4K*/
#define I2C0_BASEADDR       	(0x21405000)
	/*4K*/
#define I2S_BASEADDR        	(0x21406000)
	/*4K*/
#define DAC_BASEADDR        	(0x21407000)
	/*4K*/
#define GPIO_PD_BASEADDR        (0x21408000)
	/*4K*/
#define ADC_BASEADDR        	(0x21409000)
	/*4K*/
#define I2C1_BASEADDR        	(0x2140A000)
	/*4K*/
#define AD_TIMER0_BASEADDR    	(0x2140B000)
	/*4K*/
#define AD_TIMER1_BASEADDR     	(0x2140C000)
	/*4K*/
#define SSP2_BASEADDR        	(0x2140D000)
	/*4K*/
#define UART2_BASEADDR        	(0x2140E000)
	
	
	/*4K*/
#define SECURE_CFG_BASEADDR    	(0x21420000)
	/*4K*/
#define AES_BASEADDR        	(0x21421000)
	/*4K*/
#define HASH_BASEADDR        	(0x21422000)
	/*4K*/
#define RSA_BASEADDR        	(0x21423000)
	/*4K*/
#define TRNG_BASEADDR        	(0x21424000)
	/*64M*/
#define SPIFLASH_BASEADDR       (0x1C000000)
	/*4K*/
#define SPIFLASH_CTRL_BASEADDR 	(0x20000000)
	
	
	/*128K*/
#define DTCM_BASEADDR        	(0x81000000)
	/*8K*/
#define CEVA_REG_BASEADDR       (0x81040000)
	
	
	/*4K*/
#define CP_DMACFG_BASEADDR      (0x83000000)
	/*4K*/
#define DSP_WDT_BASEADDR        (0x83002000)
	/*4K*/
#define USIM0_BASEADDR        	(0x83003000)
	/*4K*/
#define CP_CRPM_BASEADDR        (0x83004000)
	/*4K*/
#define DSP_TIMER2_BASEADDR     (0x83005000)
	/*4K*/
#define DSP_TIMER3_BASEADDR    	(0x83006000)


#endif

/**
 * File: zx297100_pmu.c
 * Brief: Implementation of Sanechips pmu
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author:
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

/*******************************************************************************
 *                              Include header files                            *
 ******************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "csi_config.h"
#include "io.h"
#include "soc.h"
#include "drv_pmu.h"
#include "drv_bmu.h"
#include "drv_clk.h"
#include "drv_intc.h"
#include "drv_rtc.h"
#include "drv_tee.h"
#include "zx29_pmu.h"
#include "zx29_bmu.h"
#include "time.h"
#include "csi_core.h"

/*******************************************************************************
 *                              Macro definitions                               *
 *******************************************************************************/
#define ERR_PMU(errno) (CSI_DRV_ERRNO_PMU_BASE | errno)
/*******************************************************************************
*                               Type definitions                                *
*******************************************************************************/
typedef struct {
    uint32_t base;
    pmu_event_cb_t cb;
    pmu_mode_e mode;
} zx29_pmu_priv_t;

/*******************************************************************************
*                               Local function declarations                     *
*******************************************************************************/

/*******************************************************************************
*                               Local variable definitions                      *
*******************************************************************************/

/*******************************************************************************
*                               Global variable definitions                     *
*******************************************************************************/
static zx29_pmu_priv_t pmu_handle[CONFIG_PMU_NUM];
static rtc_handle_t rtc_handle;
static bmu_handle_t bmu_handle;

/*******************************************************************************
*                               Inline function implementations                 *
*******************************************************************************/

/*******************************************************************************
*                            function   extern                                  *
*******************************************************************************/
extern int32_t target_get_pmu_count(void);
extern int32_t target_get_pmu(int32_t idx, uint32_t *base, uint32_t *irq);
extern int zx29_rtc_startalarm(struct tm *rtctime, rtc_event_cb_t callback, void *arg);

/*******************************************************************************
*                               Local function implementations                  *
*******************************************************************************/

/*******************************************************************************
 *							   Macro definitions							   *
 ******************************************************************************/
#define PRE_WAKEUP_THRESHOLD	(10)
#define POW_DEBUG				(1)
#define AT_OFFSET(TYPE, MEMBER) ((int) &((TYPE *)0)->MEMBER)
/*******************************************************************************
 *							   Type definitions								   *
 ******************************************************************************/
struct lpm_args_t {
	uint32_t mode;
	uint32_t gate;
	uint32_t irqid;
};

typedef enum
{
	FREQ_19M2	= 0,
	FREQ_76M8	= 1,
	MAX_AXI_FREQ
}si_ap_freq;

/*******************************************************************************
 *						  Outside function declarations						   *
 ******************************************************************************/
extern unsigned int  ck802_ns_cpu_save(void);
extern unsigned int  ck802_ns_cpu_resume(void);
extern unsigned int  arch_tw_do_cpu_resume(void);

/*******************************************************************************
 *						  Local function declarations						   *
 ******************************************************************************/

void lpm_wakeup_interrupt(void);

static void cp_sleep_bypass(void);

void pcu_ao2ap_interrupt(void);

static void lpm_reset(void);

static void pow_pcu_init(void);

static void pow_context_save(void);
static void pow_context_restore(void);

static void do_cpu_sleep(void);
static void do_cpu_sleep_tee(void);

static void chip_pdcore_save(void);
static void chip_pdcore_restore(void);

static void pow_tick_reg_save(void);
static void pow_tick_reg_restore(void);

static void pow_icu_mask_reg_save(void);
static void pow_icu_mask_reg_restore(void);

static void mpll_enable(void);
static void mpll_disable(void);

static void apll_enable(void);
static void apll_disable(void);

static void set_ap_freq(si_ap_freq ap_freq);

static void chip_int_mask(void);
static void chip_int_unmask(void);

static void chip_int_ack(void);

static void chip_enter_sleep(zx29_pmu_priv_t *pmu_priv);
static void chip_exit_sleep(zx29_pmu_priv_t *pmu_priv);

static void chip_enter_standby(zx29_pmu_priv_t *pmu_priv);

static void chip_enter_dormant(zx29_pmu_priv_t *pmu_priv);
static void chip_exit_dormant(zx29_pmu_priv_t *pmu_priv);

static void sys_sleep_enter(uint32_t sleep_time,uint32_t ap_sleep_start);
static void sys_sleep_exit(zx29_pmu_priv_t *pmu_priv);

/*******************************************************************************
 *						   Local variable definitions						   *
 ******************************************************************************/
static uint32_t pow_context[29] = {0};
static uint32_t drv_cpu_sleep_flag = 0;
static uint32_t cp_sleep_flag = 0;

static uint32_t pad_ctrl_reg_save[18]={0};
static uint32_t icp_reg_save[12]={0};
static uint32_t lsp_crpm_reg_save[20]={0};
static uint32_t soc_reg_save[65]={0};
static uint32_t top_crpm_reg_save[27]={0};
static uint32_t spi_reg_save[10]={0};
static uint32_t uart_reg_save[20]={0};
static uint32_t ad_time_reg_save[10]={0};
static uint32_t ssp_reg_save[9]={0};
static uint32_t i2c_reg_save[10]={0};
static uint32_t i2s_reg_save[5]={0};
static uint32_t dac_reg_save[10]={0};
static uint32_t gpio_reg_save[20]={0};
static uint32_t adc_reg_save[10]={0};
static volatile uint32_t *icu_mask_reg_content = NULL;
static volatile uint32_t *icu_mode_reg1_content = NULL;
static volatile uint32_t *tick_reg_save_content = NULL;
static volatile uint32_t *coretim_reg_save_content = NULL;

#ifdef POW_DEBUG
static uint32_t top_int_cnt = 0;
static uint32_t icp_isr_cnt = 0;
#endif

/*******************************************************************************
 *						Local function implementations						   *
 ******************************************************************************/

void lpm_wakeup_interrupt(void)
{
    __clear_bit(0,(volatile uint32_t *)LPM_INT_CLEAR);
	__clear_bit(0,(volatile uint32_t *)LPM_TIME_EN);
	csi_intc_ack_irq(PIC_IRQ_LPM0);
}

static void cp_sleep_bypass(void)
{
	__set_bit(31,(volatile uint32_t *)CPSS_SLEEP_TIME_CONFIG_REG);
	__set_bit(1,(volatile uint32_t *)TOP_AO_SLEEP_MODE_CONFIG_REG);
}

void pcu_ao2ap_interrupt(void)
{
    top_int_cnt++;
	csi_intc_ack_irq(ICU_IRQ_PCU_AO2AP_WAKE);
}

static void lpm_reset(void)
{
	uint32_t tmp = getreg32((volatile uint32_t *)((SBY_CRM_RTC_CLKEN)|(0x1 << 2)));
	putreg32(tmp,(volatile uint32_t *)SBY_CRM_RTC_CLKEN);
    uint32_t value = 100;
	putreg32(1,(volatile uint32_t *)LPM_RST_REG);
    while(value--);
	putreg32(0,(volatile uint32_t *)LPM_RST_REG);
}

static void pow_pcu_init(void)
{

	__clear_bit(8,(volatile uint32_t *)PCU_FSM_SEND_INT_CONFIG_REG);  //wait ack

	__write_bits(4, 8, (volatile uint32_t *)TOP_AO_SLEEP_MODE_CONFIG_REG, 0); //uart bypass

	putreg32(0x1,(volatile uint32_t *)(A0_CRPM_BASEADDR+AO_CK802_BOOTLINK));

	__clear_bit(AO_WAKE_SRC_TOP2APSS_INT,(volatile uint32_t *)AO_APSS_INT_WAKE_DIS_REG1); //set no wakeup int in top 2 apss

	__clear_bit(SBY_WAKE_SRC_RTC_ALARM_INT,(volatile uint32_t *)SBY_INT_WAKEUP_DIS_REG1);
	__clear_bit(SBY_WAKE_TYPE_RTC_ALARM_INT,(volatile uint32_t *)SBY_INT_WAKEUP_TYPE_REG1);

    csi_intc_disable_irq(ICU_IRQ_PCU_AO2AP_WAKE);
    csi_intc_set_attribute(ICU_IRQ_PCU_AO2AP_WAKE,1,INT_MODE_HIGH_LEVEL);
//    yunos_bsp_intc_attach_irq(ICU_IRQ_PCU_AO2AP_WAKE, (int)pcu_ao2ap_interrupt);
    csi_intc_enable_irq(ICU_IRQ_PCU_AO2AP_WAKE);
//    csi_vic_set_wakeup_irq(VIC_IRQ_31);//CPU VIC »½ÐÑÔ´ÅäÖÃ
    csi_vic_set_wakeup_irq(PIC_IRQ_31);//CPU VIC »½ÐÑÔ´ÅäÖÃ

    /*dsp2CK*/
	__clear_bit(AO_WAKE_SRC_ICP_DSP2CK_INT,(volatile uint32_t *)AO_APSS_INT_WAKE_DIS_REG1);

	#if 0/*´Ë´¦Ö»ÄÜÄ³ºË¸´Î»Ò»´Î£¬²»ÄÜÁ½±ßÍ¬Ê±¸´Î» ~~~~*/
	lpm_reset();
	#endif

	__clear_bit(SBY_WAKE_SRC_LPM0_INT,(volatile uint32_t *)SBY_INT_WAKEUP_DIS_REG1);
	__clear_bit(SBY_WAKE_TYPE_LPM0_INT,(volatile uint32_t *)SBY_INT_WAKEUP_TYPE_REG1);

    csi_intc_disable_irq(PIC_IRQ_LPM0);
//    yunos_bsp_intc_attach_irq(PIC_IRQ_LPM0, (int)lpm_wakeup_interrupt);
    csi_intc_enable_irq(PIC_IRQ_LPM0);

	__write_bits(6, 0, (volatile uint32_t *)IRAM_TCM_CTRL_CONNFIG_REG1, 0x3f); //men ke li du control
	__write_bits(8, 0, (volatile uint32_t *)IRAM_TCM_CTRL_CONNFIG_REG2, 0xff);

	//__clear_bit(AO_WAKE_SRC_UART0_INT,AO_APSS_INT_WAKE_DIS_REG1);
	__clear_bit(AO_WAKE_SRC_UART2_INT,(volatile uint32_t *)AO_APSS_INT_WAKE_DIS_REG1);

}

static void pow_context_save(void)
{
	memcpy((void *)pow_context,(void *)(WAKE_UP_CODE_ADDRESS) ,WAKEUP_CODE_LENGTH);//Ìø×ª²Ù×÷
	pow_context[25] = getreg32((volatile uint32_t *)PMU_IRAM_AON_BASE);
	pow_context[26] = getreg32((volatile uint32_t *)PSM_SCPU_CONTENT_ADDR_STORE);
	pow_context[27] = getreg32((volatile uint32_t *)EFLASH_POWON_ADDR_STORE);
	pow_context[28] = getreg32((volatile uint32_t *)EFLASH_RST_ADDR_STORE);
}
static void pow_context_restore(void)
{
    memcpy((void *)(WAKE_UP_CODE_ADDRESS),(void *)pow_context,WAKEUP_CODE_LENGTH);
	putreg32(pow_context[25],(volatile uint32_t *)PMU_IRAM_AON_BASE);
	putreg32(pow_context[26],(volatile uint32_t *)PSM_SCPU_CONTENT_ADDR_STORE);
	putreg32(pow_context[27],(volatile uint32_t *)EFLASH_POWON_ADDR_STORE);
	putreg32(pow_context[28],(volatile uint32_t *)EFLASH_RST_ADDR_STORE);
}

static void psm_icp_isr(void *data, uint16_t len)
{
	  icp_isr_cnt ++;
}

static void do_cpu_sleep(void)
{
    __asm__("stop");
}

static void do_cpu_sleep_tee(void)
{
    csi_tee_enter_lpm(0, 0, TEE_LPM_MODE_STOP);
}
/**
 * Save the pad ctrl relevant reg.
 *
 * @return      Null.
 */
static void pow_si_pad_ctrl_save(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	for (j = 0;j < 7;j++) {
		pad_ctrl_reg_save[num++] = getreg32((volatile uint32_t *)(PAD_CTRL_PD_BASEADDR + 0x4 + 0x4*j));
	}
	for (j = 0;j < 11;j++) {
		pad_ctrl_reg_save[num++] = getreg32((volatile uint32_t *)(PAD_CTRL_PD_BASEADDR + 0x100 + 0x4*j));
	}
}

/**
 * Restore the pad ctrl relevant reg.
 *
 * @return      Null.
 */
static void pow_si_pad_ctrl_restore(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	for (j = 0;j < 7;j++) {
		putreg32(pad_ctrl_reg_save[num++],(volatile uint32_t *)(PAD_CTRL_PD_BASEADDR + 0x4 + 0x4*j));
	}
	for (j = 0;j < 11;j++) {
		putreg32(pad_ctrl_reg_save[num++],(volatile uint32_t *)(PAD_CTRL_PD_BASEADDR + 0x100 + 0x4*j));
	}
}


/**
 * Save the icp relevant reg.
 *
 * @return      Null.
 */
static void pow_si_icp_save(void)
{
	uint32_t  num = 0;

	icp_reg_save[num++] = getreg32((volatile uint32_t *)(ICP_BASEADDR + 0x044));
	icp_reg_save[num++] = getreg32((volatile uint32_t *)(ICP_BASEADDR + 0x048));
	icp_reg_save[num++] = getreg32((volatile uint32_t *)(ICP_BASEADDR + 0x05c));
	icp_reg_save[num++] = getreg32((volatile uint32_t *)(ICP_BASEADDR + 0x060));

}

/**
 * Restore the icp relevant reg.
 *
 * @return      Null.
 */
static void pow_si_icp_restore(void)
{
	uint32_t  num = 0;

	putreg32(icp_reg_save[num++],(volatile uint32_t *)(ICP_BASEADDR + 0x044));
	putreg32(icp_reg_save[num++],(volatile uint32_t *)(ICP_BASEADDR + 0x048));
	putreg32(icp_reg_save[num++],(volatile uint32_t *)(ICP_BASEADDR + 0x05c));
	putreg32(icp_reg_save[num++],(volatile uint32_t *)(ICP_BASEADDR + 0x060));

}

/**
 * Save the lsp crpm relevant reg.
 *
 * @return      Null.
 */
static void pow_si_lsp_crpm_save(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	for (j = 0;j < 20;j++) {
		lsp_crpm_reg_save[num++] = getreg32((volatile uint32_t *)(LSP_CRPM_BASEADDR + 0x4 + 0x4*j));
	}
}

/**
 * Restore the lsp crpm relevant reg.
 *
 * @return      Null.
 */
static void pow_si_lsp_crpm_restore(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	for (j = 0;j < 20;j++) {
		putreg32(lsp_crpm_reg_save[num++],(volatile uint32_t *)(LSP_CRPM_BASEADDR + 0x4 + 0x4*j));
	}
}

/**
 * Save the soc relevant reg.
 *
 * @return      Null.
 */
static void pow_si_soc_save(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x4));
	soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x18));
	soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x1c));
	soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x30));
	soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x50));
	soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x78));
	soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x7c));
	for (j = 0;j < 29;j++) {
		soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0xa0 + 0x4*j));
	}
	for (j = 0;j < 7;j++) {
		soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x11c + 0x4*j));
	}
	for (j = 0;j < 4;j++) {
		soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x140 + 0x4*j));
	}
	soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x154));
	soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x158));
	soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x170));
	soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x178));
	soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x184));
	soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x188));
	for (j = 0;j < 4;j++) {
		soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x190 + 0x4*j));
	}
	for (j = 0;j < 8;j++) {
		soc_reg_save[num++] = getreg32((volatile uint32_t *)(SOC_SYS_BASEADDR + 0x1a8 + 0x4*j));
	}
}

/**
 * Restore the soc relevant reg.
 *
 * @return      Null.
 */
static void pow_si_soc_restore(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x4));
	putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x18));
	putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x1c));
	putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x30));
	putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x50));
	putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x78));
	putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x7c));
	for (j = 0;j < 29;j++) {
		putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0xa0 + 0x4*j));
	}
	for (j = 0;j < 7;j++) {
		putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x11c + 0x4*j));
	}
	for (j = 0;j < 4;j++) {
		putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR+0x140+0x4*j));
	}
	putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x154));
	putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x158));
	putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x170));
	putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x178));
	putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x184));
	putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x188));
	for (j = 0;j < 4;j++) {
		putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x190 + 0x4*j));
	}
	for (j = 0;j < 8;j++) {
		putreg32(soc_reg_save[num++],(volatile uint32_t *)(SOC_SYS_BASEADDR + 0x1a8 + 0x4*j));
	}
}

/**
 * Save the top crpm relevant reg.
 *
 * @return      Null.
 */
static void pow_si_top_crpm_save(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	top_crpm_reg_save[num++] = getreg32((volatile uint32_t *)TOP_CRPM_BASEADDR);
	top_crpm_reg_save[num++] = getreg32((volatile uint32_t *)(TOP_CRPM_BASEADDR + 0x4));
	top_crpm_reg_save[num++] = getreg32((volatile uint32_t *)(TOP_CRPM_BASEADDR + 0x10));
	top_crpm_reg_save[num++] = getreg32((volatile uint32_t *)(TOP_CRPM_BASEADDR + 0x24));
	for (j = 0;j < 22;j++) {
		top_crpm_reg_save[num++] = getreg32((volatile uint32_t *)(TOP_CRPM_BASEADDR + 0x30 + 0x4*j));
	}
}

/**
 * Restore the top crpm relevant reg.
 *
 * @return      Null.
 */
static void pow_si_top_crpm_restore(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	putreg32(top_crpm_reg_save[num++],(volatile uint32_t *)TOP_CRPM_BASEADDR);
	putreg32(top_crpm_reg_save[num++],(volatile uint32_t *)(TOP_CRPM_BASEADDR + 0x4));
	putreg32(top_crpm_reg_save[num++],(volatile uint32_t *)(TOP_CRPM_BASEADDR + 0x10));
	putreg32(top_crpm_reg_save[num++],(volatile uint32_t *)(TOP_CRPM_BASEADDR + 0x24));
	for (j = 0;j < 22;j++) {
		putreg32(top_crpm_reg_save[num++],(volatile uint32_t *)(TOP_CRPM_BASEADDR + 0x30 + 0x4*j));
	}
}

/**
 * Save the spi flash relevant reg.
 *
 * @return      Null.
 */
static void pow_si_spiflash_save(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	for (j = 0;j < 10;j++) {
		spi_reg_save[num++] = getreg32((volatile uint32_t *)(SPIFLASH_CTRL_BASEADDR + 0x4*j));
	}
}

/**
 * Restore the spi flash relevant reg.
 *
 * @return      Null.
 */
static void pow_si_spiflash_restore(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	for (j = 0;j < 10;j++) {
		putreg32(spi_reg_save[num++],(volatile uint32_t *)(SPIFLASH_CTRL_BASEADDR + 0x4*j));
	}
}

/**
 * Save the uart relevant reg.
 *
 * @return      Null.
 */
static void pow_si_uart_save(void)
{
	uint32_t  num = 0;
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART0_BASEADDR + 0x24));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART0_BASEADDR + 0x28));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART0_BASEADDR + 0x30));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART0_BASEADDR + 0x34));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART0_BASEADDR + 0x38));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART0_BASEADDR + 0x40));

	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART1_BASEADDR + 0x24));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART1_BASEADDR + 0x28));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART1_BASEADDR + 0x30));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART1_BASEADDR + 0x34));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART1_BASEADDR + 0x38));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART1_BASEADDR + 0x40));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART1_BASEADDR + 0x50));

	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART2_BASEADDR + 0x24));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART2_BASEADDR + 0x28));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART2_BASEADDR + 0x30));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART2_BASEADDR + 0x34));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART2_BASEADDR + 0x38));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART2_BASEADDR + 0x40));
	uart_reg_save[num++] = getreg32((volatile uint32_t *)(UART2_BASEADDR + 0x50));
}

/**
 * Restore the uart relevant reg.
 *
 * @return      Null.
 */
static void pow_si_uart_restore(void)
{
	uint32_t  num = 0;
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART0_BASEADDR + 0x24));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART0_BASEADDR + 0x28));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART0_BASEADDR + 0x30));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART0_BASEADDR + 0x34));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART0_BASEADDR + 0x38));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART0_BASEADDR + 0x40));

	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART1_BASEADDR + 0x24));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART1_BASEADDR + 0x28));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART1_BASEADDR + 0x30));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART1_BASEADDR + 0x34));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART1_BASEADDR + 0x38));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART1_BASEADDR + 0x40));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART1_BASEADDR + 0x50));

	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART2_BASEADDR + 0x24));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART2_BASEADDR + 0x28));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART2_BASEADDR + 0x30));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART2_BASEADDR + 0x34));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART2_BASEADDR + 0x38));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART2_BASEADDR + 0x40));
	putreg32(uart_reg_save[num++],(volatile uint32_t *)(UART2_BASEADDR + 0x50));
}

/**
 * Save the adtime relevant reg.
 *
 * @return      Null.
 */
static void pow_si_adtime_save(void)
{
	uint32_t  num = 0;
	ad_time_reg_save[num++] = getreg32((volatile uint32_t *)(AD_TIMER0_BASEADDR + 0x4));
	ad_time_reg_save[num++] = getreg32((volatile uint32_t *)(AD_TIMER0_BASEADDR + 0x8));
	ad_time_reg_save[num++] = getreg32((volatile uint32_t *)(AD_TIMER0_BASEADDR + 0x34));
	ad_time_reg_save[num++] = getreg32((volatile uint32_t *)(AD_TIMER0_BASEADDR + 0x38));
	ad_time_reg_save[num++] = getreg32((volatile uint32_t *)(AD_TIMER0_BASEADDR + 0x40));

	ad_time_reg_save[num++] = getreg32((volatile uint32_t *)(AD_TIMER1_BASEADDR + 0x4));
	ad_time_reg_save[num++] = getreg32((volatile uint32_t *)(AD_TIMER1_BASEADDR + 0x8));
	ad_time_reg_save[num++] = getreg32((volatile uint32_t *)(AD_TIMER1_BASEADDR + 0x34));
	ad_time_reg_save[num++] = getreg32((volatile uint32_t *)(AD_TIMER1_BASEADDR + 0x38));
	ad_time_reg_save[num++] = getreg32((volatile uint32_t *)(AD_TIMER1_BASEADDR + 0x40));
}

/**
 * Restore the adtime relevant reg.
 *
 * @return      Null.
 */
static void pow_si_adtime_restore(void)
{
	uint32_t  num = 0;
	putreg32(ad_time_reg_save[num++],(volatile uint32_t *)(AD_TIMER0_BASEADDR + 0x4));
	putreg32(ad_time_reg_save[num++],(volatile uint32_t *)(AD_TIMER0_BASEADDR + 0x8));
	putreg32(ad_time_reg_save[num++],(volatile uint32_t *)(AD_TIMER0_BASEADDR + 0x34));
	putreg32(ad_time_reg_save[num++],(volatile uint32_t *)(AD_TIMER0_BASEADDR + 0x38));
	putreg32(ad_time_reg_save[num++],(volatile uint32_t *)(AD_TIMER0_BASEADDR + 0x40));

	putreg32(ad_time_reg_save[num++],(volatile uint32_t *)(AD_TIMER1_BASEADDR + 0x4));
	putreg32(ad_time_reg_save[num++],(volatile uint32_t *)(AD_TIMER1_BASEADDR + 0x8));
	putreg32(ad_time_reg_save[num++],(volatile uint32_t *)(AD_TIMER1_BASEADDR + 0x34));
	putreg32(ad_time_reg_save[num++],(volatile uint32_t *)(AD_TIMER1_BASEADDR + 0x38));
	putreg32(ad_time_reg_save[num++],(volatile uint32_t *)(AD_TIMER1_BASEADDR + 0x40));
}

/**
 * Save the ssp relevant reg.
 *
 * @return      Null.
 */
static void pow_si_ssp_save(void)
{
	uint32_t  num = 0;
	ssp_reg_save[num++] = getreg32((volatile uint32_t *)(SSP0_BASEADDR + 0x8));
	ssp_reg_save[num++] = getreg32((volatile uint32_t *)(SSP0_BASEADDR + 0x10));
	ssp_reg_save[num++] = getreg32((volatile uint32_t *)(SSP0_BASEADDR + 0x18));

	ssp_reg_save[num++] = getreg32((volatile uint32_t *)(SSP1_BASEADDR + 0x8));
	ssp_reg_save[num++] = getreg32((volatile uint32_t *)(SSP1_BASEADDR + 0x10));
	ssp_reg_save[num++] = getreg32((volatile uint32_t *)(SSP1_BASEADDR + 0x18));

	ssp_reg_save[num++] = getreg32((volatile uint32_t *)(SSP2_BASEADDR + 0x8));
	ssp_reg_save[num++] = getreg32((volatile uint32_t *)(SSP2_BASEADDR + 0x10));
	ssp_reg_save[num++] = getreg32((volatile uint32_t *)(SSP2_BASEADDR + 0x18));
}

/**
 * Restore the ssp relevant reg.
 *
 * @return      Null.
 */
static void pow_si_ssp_restore(void)
{
	uint32_t  num = 0;
    __clear_bit(1,(volatile uint32_t *)(SSP0_BASEADDR + 0x4));
	__clear_bit(2,(volatile uint32_t *)(SSP0_BASEADDR + 0x4));
	putreg32(ssp_reg_save[num++],(volatile uint32_t *)(SSP0_BASEADDR + 0x8));
	putreg32(ssp_reg_save[num++],(volatile uint32_t *)(SSP0_BASEADDR + 0x10));
	putreg32(ssp_reg_save[num++],(volatile uint32_t *)(SSP0_BASEADDR + 0x18));
    __set_bit(1,(volatile uint32_t *)(SSP0_BASEADDR + 0x4));

    __clear_bit(1,(volatile uint32_t *)(SSP1_BASEADDR + 0x4));
	__clear_bit(2,(volatile uint32_t *)(SSP1_BASEADDR + 0x4));
	putreg32(ssp_reg_save[num++],(volatile uint32_t *)(SSP1_BASEADDR + 0x8));
	putreg32(ssp_reg_save[num++],(volatile uint32_t *)(SSP1_BASEADDR + 0x10));
	putreg32(ssp_reg_save[num++],(volatile uint32_t *)(SSP1_BASEADDR + 0x18));
	__set_bit(1,(volatile uint32_t *)(SSP1_BASEADDR + 0x4));

    __clear_bit(1,(volatile uint32_t *)(SSP2_BASEADDR + 0x4));
	__clear_bit(2,(volatile uint32_t *)(SSP2_BASEADDR + 0x4));
	putreg32(ssp_reg_save[num++],(volatile uint32_t *)(SSP2_BASEADDR + 0x8));
	putreg32(ssp_reg_save[num++],(volatile uint32_t *)(SSP2_BASEADDR + 0x10));
	putreg32(ssp_reg_save[num++],(volatile uint32_t *)(SSP2_BASEADDR + 0x18));
	__set_bit(1,(volatile uint32_t *)(SSP2_BASEADDR + 0x4));

}

/**
 * Save the i2c relevant reg.
 *
 * @return      Null.
 */
static void pow_si_i2c_save(void)
{
	uint32_t  num = 0;
	i2c_reg_save[num++] = getreg32((volatile uint32_t *)(I2C0_BASEADDR + 0x4));
	i2c_reg_save[num++] = getreg32((volatile uint32_t *)(I2C0_BASEADDR + 0xc));
	i2c_reg_save[num++] = getreg32((volatile uint32_t *)(I2C0_BASEADDR + 0x14));
	i2c_reg_save[num++] = getreg32((volatile uint32_t *)(I2C0_BASEADDR + 0x1c));
	i2c_reg_save[num++] = getreg32((volatile uint32_t *)(I2C0_BASEADDR + 0x20));

	i2c_reg_save[num++] = getreg32((volatile uint32_t *)(I2C1_BASEADDR + 0x4));
	i2c_reg_save[num++] = getreg32((volatile uint32_t *)(I2C1_BASEADDR + 0xc));
	i2c_reg_save[num++] = getreg32((volatile uint32_t *)(I2C1_BASEADDR + 0x14));
	i2c_reg_save[num++] = getreg32((volatile uint32_t *)(I2C1_BASEADDR + 0x1c));
	i2c_reg_save[num++] = getreg32((volatile uint32_t *)(I2C1_BASEADDR + 0x20));
}

/**
 * Restore the i2c relevant reg.
 *
 * @return      Null.
 */
static void pow_si_i2c_restore(void)
{
	uint32_t  num = 0;
	putreg32(i2c_reg_save[num++],(volatile uint32_t *)(I2C0_BASEADDR + 0x4));
	putreg32(i2c_reg_save[num++],(volatile uint32_t *)(I2C0_BASEADDR + 0xc));
	putreg32(i2c_reg_save[num++],(volatile uint32_t *)(I2C0_BASEADDR + 0x14));
	putreg32(i2c_reg_save[num++],(volatile uint32_t *)(I2C0_BASEADDR + 0x1c));
	putreg32(i2c_reg_save[num++],(volatile uint32_t *)(I2C0_BASEADDR + 0x20));

	putreg32(i2c_reg_save[num++],(volatile uint32_t *)(I2C1_BASEADDR + 0x4));
	putreg32(i2c_reg_save[num++],(volatile uint32_t *)(I2C1_BASEADDR + 0xc));
	putreg32(i2c_reg_save[num++],(volatile uint32_t *)(I2C1_BASEADDR + 0x14));
	putreg32(i2c_reg_save[num++],(volatile uint32_t *)(I2C1_BASEADDR + 0x1c));
	putreg32(i2c_reg_save[num++],(volatile uint32_t *)(I2C1_BASEADDR + 0x20));
}

/**
 * Save the i2s relevant reg.
 *
 * @return      Null.
 */
static void pow_si_i2s_save(void)
{
	uint32_t  num = 0;
	i2s_reg_save[num++] = getreg32((volatile uint32_t *)(I2S_BASEADDR + 0x4));
	i2s_reg_save[num++] = getreg32((volatile uint32_t *)(I2S_BASEADDR + 0x8));
	i2s_reg_save[num++] = getreg32((volatile uint32_t *)(I2S_BASEADDR + 0xc));
	i2s_reg_save[num++] = getreg32((volatile uint32_t *)(I2S_BASEADDR + 0x14));
	i2s_reg_save[num++] = getreg32((volatile uint32_t *)(I2S_BASEADDR + 0x20));
}

/**
 * Restore the i2s relevant reg.
 *
 * @return      Null.
 */
static void pow_si_i2s_restore(void)
{
	uint32_t  num = 0;
	putreg32(i2s_reg_save[num++],(volatile uint32_t *)(I2S_BASEADDR + 0x4));
	putreg32(i2s_reg_save[num++],(volatile uint32_t *)(I2S_BASEADDR + 0x8));
	putreg32(i2s_reg_save[num++],(volatile uint32_t *)(I2S_BASEADDR + 0xc));
	putreg32(i2s_reg_save[num++],(volatile uint32_t *)(I2S_BASEADDR + 0x14));
	putreg32(i2s_reg_save[num++],(volatile uint32_t *)(I2S_BASEADDR + 0x20));
}

/**
 * Save the dac relevant reg.
 *
 * @return      Null.
 */
static void pow_si_dac_save(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	for (j = 0;j < 9;j++) {
		dac_reg_save[num++] = getreg32((volatile uint32_t *)(DAC_BASEADDR + 0xc + 0x4*j));
	}
	dac_reg_save[num++] = getreg32((volatile uint32_t *)(DAC_BASEADDR + 0x40));
}

/**
 * Restore the dac relevant reg.
 *
 * @return      Null.
 */
static void pow_si_dac_restore(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	for (j = 0;j < 9;j++) {
		putreg32(dac_reg_save[num++],(volatile uint32_t *)(DAC_BASEADDR + 0xc + 0x4*j));
	}
	putreg32(dac_reg_save[num++],(volatile uint32_t *)(DAC_BASEADDR + 0x40));
}

/**
 * Save the gpio relevant reg.
 *
 * @return      Null.
 */
static void pow_si_gpio_save(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	for (j = 0;j < 4;j++) {
		gpio_reg_save[num++] = getreg32((volatile uint32_t *)(GPIO_PD_BASEADDR + 0x40*j));
		gpio_reg_save[num++] = getreg32((volatile uint32_t *)(GPIO_PD_BASEADDR + 0x14 + 0x40*j));
		gpio_reg_save[num++] = getreg32((volatile uint32_t *)(GPIO_PD_BASEADDR + 0x18 + 0x40*j));
		gpio_reg_save[num++] = getreg32((volatile uint32_t *)(GPIO_PD_BASEADDR + 0x1c + 0x40*j));
		gpio_reg_save[num++] = getreg32((volatile uint32_t *)(GPIO_PD_BASEADDR + 0x20 + 0x40*j));
	}
}

/**
 * Restore the gpio relevant reg.
 *
 * @return      Null.
 */
static void pow_si_gpio_restore(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	for (j = 0;j < 4;j++) {
		putreg32(gpio_reg_save[num++],(volatile uint32_t *)(GPIO_PD_BASEADDR + 0x40*j));
		putreg32(gpio_reg_save[num++],(volatile uint32_t *)(GPIO_PD_BASEADDR + 0x14 + 0x40*j));
		putreg32(gpio_reg_save[num++],(volatile uint32_t *)(GPIO_PD_BASEADDR + 0x18 + 0x40*j));
		putreg32(gpio_reg_save[num++],(volatile uint32_t *)(GPIO_PD_BASEADDR + 0x1c + 0x40*j));
		putreg32(gpio_reg_save[num++],(volatile uint32_t *)(GPIO_PD_BASEADDR + 0x20 + 0x40*j));
	}
}

/**
 * Save the adc relevant reg.
 *
 * @return      Null.
 */
static void pow_si_adc_save(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	adc_reg_save[num++] = getreg32((volatile uint32_t *)(ADC_BASEADDR + 0x4));
	adc_reg_save[num++] = getreg32((volatile uint32_t *)(ADC_BASEADDR + 0x8));
	adc_reg_save[num++] = getreg32((volatile uint32_t *)(ADC_BASEADDR + 0x10));
	for (j = 0;j < 6;j++) {
		adc_reg_save[num++] = getreg32((volatile uint32_t *)(ADC_BASEADDR + 0x14 + 0x4*j));
	}
	adc_reg_save[num++] = getreg32((volatile uint32_t *)(ADC_BASEADDR + 0x30));
}

/**
 * Restore the adc relevant reg.
 *
 * @return      Null.
 */
static void pow_si_adc_restore(void)
{
	uint32_t  j = 0;
	uint32_t  num = 0;
	putreg32(adc_reg_save[num++],(volatile uint32_t *)(ADC_BASEADDR + 0x4));
	putreg32(adc_reg_save[num++],(volatile uint32_t *)(ADC_BASEADDR + 0x8));
	putreg32(adc_reg_save[num++],(volatile uint32_t *)(ADC_BASEADDR + 0x10));
	for (j = 0;j < 6;j++) {
		putreg32(adc_reg_save[num++],(volatile uint32_t *)(ADC_BASEADDR + 0x14 + 0x4*j));
	}
	putreg32(adc_reg_save[num++],(volatile uint32_t *)(ADC_BASEADDR + 0x30));
}

static void chip_pdcore_save(void)
{
	pow_si_pad_ctrl_save();
	pow_si_icp_save();
	pow_si_lsp_crpm_save();
	pow_si_soc_save();
	pow_si_top_crpm_save();
	pow_si_spiflash_save();
	pow_si_uart_save();
	pow_si_adtime_save();
	pow_si_ssp_save();
	pow_si_i2c_save();
	pow_si_i2s_save();
	pow_si_dac_save();
	pow_si_gpio_save();
	pow_si_adc_save();
}

static void chip_pdcore_restore(void)
{
	pow_si_pad_ctrl_restore();

	pow_si_lsp_crpm_restore();
	pow_si_soc_restore();
	pow_si_top_crpm_restore();
	pow_si_icp_restore();
	pow_si_spiflash_restore();
	pow_si_uart_restore();
	pow_si_adtime_restore();
	pow_si_ssp_restore();
	pow_si_i2c_restore();
	pow_si_i2s_restore();
	pow_si_dac_restore();
	pow_si_gpio_restore();
	pow_si_adc_restore();
}

static void pow_tick_reg_save(void)
{
    tick_reg_save_content = (volatile uint32_t *)(TICK_REG_SAVE_ADDR);
    coretim_reg_save_content = (volatile uint32_t*)(CORETIM_REG_SAVE_ADDR);
	*(tick_reg_save_content++) = getreg32((volatile uint32_t *)(CK_TIMER_CLKEN_BASEADDR));//EN
	*(tick_reg_save_content++) = getreg32((volatile uint32_t *)(CK802_TIMER0_REG_BASE+0x4));// config
	*(tick_reg_save_content++) = getreg32((volatile uint32_t *)(CK802_TIMER0_REG_BASE+0x8));//load
	*(tick_reg_save_content++) = getreg32((volatile uint32_t *)(CK802_TIMER0_REG_BASE+0xc));//evt_ctrl
	*(tick_reg_save_content++) = getreg32((volatile uint32_t *)(CK802_TIMER0_REG_BASE+0x1C));//INT_EN
	*(tick_reg_save_content++) = getreg32((volatile uint32_t *)(CK802_TIMER0_REG_BASE+0x14));//EN

	/*coretime*/
//	*(coretim_reg_save_content++) = getreg32(CORET_RVR);
//    *(coretim_reg_save_content++) = getreg32(CORET_CSR);
	*(coretim_reg_save_content++) = CORET->LOAD;
    *(coretim_reg_save_content++) = CORET->CTRL;
}
static void pow_tick_reg_restore(void)
{
    tick_reg_save_content = (volatile uint32_t *)(TICK_REG_SAVE_ADDR);
    coretim_reg_save_content = (volatile uint32_t*)(CORETIM_REG_SAVE_ADDR);
	/*coretim*/
//	putreg32(*(coretim_reg_save_content++),CORET_RVR);
//	putreg32(0x0,CORET_CVR);
//	putreg32(*(coretim_reg_save_content++),CORET_CSR);
    CORET->LOAD = *(coretim_reg_save_content++);
    CORET->VAL = 0;
    CORET->CTRL = *(coretim_reg_save_content++);

	//int index =0;
    putreg32(*(tick_reg_save_content++),(volatile uint32_t *)CK_TIMER_CLKEN_BASEADDR);
	putreg32(*(tick_reg_save_content++),(volatile uint32_t *)(CK802_TIMER0_REG_BASE+0x4));
	putreg32(*(tick_reg_save_content++),(volatile uint32_t *)(CK802_TIMER0_REG_BASE+0x8));
	putreg32(*(tick_reg_save_content++),(volatile uint32_t *)(CK802_TIMER0_REG_BASE+0xc));
	putreg32(*(tick_reg_save_content++),(volatile uint32_t *)(CK802_TIMER0_REG_BASE+0x1c));
	putreg32(0xF,(volatile uint32_t *)(CK802_TIMER0_REG_BASE+0x10));
	while(getreg32((volatile uint32_t *)(CK802_TIMER0_REG_BASE+0x10)) != 0);/*µÈ´ýÍ¬²½Íê³É*/
	putreg32(*(tick_reg_save_content++),(volatile uint32_t *)(CK802_TIMER0_REG_BASE+0x14));
    tick_reg_save_content = NULL;
    coretim_reg_save_content = NULL;

}


static void pow_icu_mask_reg_save(void)
{
	int index = 0;
//    uint32_t value =0;
	icu_mask_reg_content = (volatile uint32_t *)(ICU_MASK_REG_SAVE_ADDR);
	icu_mode_reg1_content = (volatile uint32_t *)(ICU_MODE_REG1_SAVE_ADDR);
	*(icu_mask_reg_content++) = getreg32((volatile uint32_t *)CK802_ICU_INT_MASK0);
	*(icu_mask_reg_content++) = getreg32((volatile uint32_t *)CK802_ICU_INT_MASK1);
	*(icu_mask_reg_content++) = getreg32((volatile uint32_t *)CK802_ICU_INT_MASK2);
	*(icu_mask_reg_content++) = getreg32((volatile uint32_t *)CK802_ICU_INT_MASK3);

	for (index = 0; index < 128;index++) {
		*(icu_mode_reg1_content++) = getreg32((volatile uint32_t *)(CK802_ICU_MOD_REG1_BASE+0x4*index));
	}
}
static void pow_icu_mask_reg_restore(void)
{
    icu_mask_reg_content = (volatile uint32_t *)(ICU_MASK_REG_SAVE_ADDR);
	icu_mode_reg1_content = (volatile uint32_t *)(ICU_MODE_REG1_SAVE_ADDR);

	int index =0;
	for (index = 0; index<128;index++) {
		putreg32(*(icu_mode_reg1_content++),(volatile uint32_t *)(CK802_ICU_MOD_REG1_BASE+0x4*index));
	}

	putreg32(*(icu_mask_reg_content++),(volatile uint32_t *)CK802_ICU_INT_MASK0);
	putreg32(*(icu_mask_reg_content++),(volatile uint32_t *)CK802_ICU_INT_MASK1);
	putreg32(*(icu_mask_reg_content++),(volatile uint32_t *)CK802_ICU_INT_MASK2);
	putreg32(*(icu_mask_reg_content++),(volatile uint32_t *)CK802_ICU_INT_MASK3);
	icu_mask_reg_content = NULL;
	icu_mode_reg1_content = NULL;
}

static void mpll_enable(void)
{
	__set_bit(31,(volatile uint32_t *)TOP_CRPM_MPLL_CFG);
}

static void mpll_disable(void)
{
	__clear_bit(31,(volatile uint32_t *)TOP_CRPM_MPLL_CFG);
}

static void apll_enable(void)
{
	__set_bit(31,(volatile uint32_t *)TOP_CRPM_APLL_CFG);
}

static void apll_disable(void)
{
	__clear_bit(31,(volatile uint32_t *)TOP_CRPM_APLL_CFG);

}

static void sec_disable(void)
{
	__clear_bit(0,(volatile uint32_t *)AP_CRPM_SEC_ENABLE);

}

static void ap2cp_flag_enable(void)
{
	__set_bit(0,(volatile uint32_t *)AP_CRPM_AP_TO_CP_FLAG);
	__set_bit(1,(volatile uint32_t *)AP_CRPM_AP_TO_CP_FLAG);

}

static void ap2cp_flag_disable(void)
{
	__clear_bit(0,(volatile uint32_t *)AP_CRPM_AP_TO_CP_FLAG);
	__clear_bit(1,(volatile uint32_t *)AP_CRPM_AP_TO_CP_FLAG);
}

static void m0_clk_disable(void)
{
	__clear_bit(0,(volatile uint32_t *)AP_CRPM_M0_CLKEN);

}

static void ck_idle_en(void)
{
	__set_bit(0,(volatile uint32_t *)AP_CRPM_CK_IDLE_EN);
}

static void aon_clk_config(void)
{
	__set_bit(10,(volatile uint32_t *)AO_CRPM_AON_CLKEN);//pcu auto gate
	__set_bit(13,(volatile uint32_t *)AO_CRPM_AON_CLKEN);//pinmux auto gate
	__set_bit(14,(volatile uint32_t *)AO_CRPM_AON_CLKEN);//iram clock auto gate
	__set_bit(16,(volatile uint32_t *)AO_CRPM_AON_CLKEN);//gpio apb auto gate

}

static void top_clk_config(void)
{
	//__set_bit(0,TOP_CRPM_PD_TO_AP_CLKEN);//ap matrix l2 auto gate
	__set_bit(0,(volatile uint32_t *)TOP_CRPM_PD_AHB_CLKEN);//pd logic l2 auto gate
	__clear_bit(8,(volatile uint32_t *)TOP_CRPM_PD_TO_OUT_CLKEN);//mco disable
	__clear_bit(0,(volatile uint32_t *)TOP_CRPM_PD_TO_AON_CLKEN);//aon 76.8M off
	__set_bit(3,(volatile uint32_t *)TOP_CRPM_PD_MATRIX_CLKEN);//pd pinmux auto gate
	__write_bits(12,0,(volatile uint32_t *)TOP_CRPM_SPINOR,0);//disable spinor

}

static void ap_clk_config(void)
{
	csi_clk_hw_gate_ctrl(AP_DMA_PCLK,1);
	csi_clk_hw_gate_ctrl(CK_TIMER0_PCLK,1);
	csi_clk_hw_gate_ctrl(CK_TIMER1_PCLK,1);
	csi_clk_hw_gate_ctrl(AP_IRAM1_IROM_CLK,1);
	csi_clk_hw_gate_ctrl(AP_IRAM2_CLK,1);
}

static void lsp_clk_config(void)
{
	csi_clk_hw_gate_ctrl(UART0_PCLK,1);
	csi_clk_hw_gate_ctrl(UART1_PCLK,1);
	csi_clk_hw_gate_ctrl(UART2_PCLK,1);
	csi_clk_hw_gate_ctrl(UART0_WCLK_L1,1);
	csi_clk_hw_gate_ctrl(UART1_WCLK,1);
	csi_clk_hw_gate_ctrl(UART2_WCLK,1);
	csi_clk_hw_gate_ctrl(I2C0_PCLK,1);
	csi_clk_hw_gate_ctrl(I2C1_PCLK,1);
	csi_clk_hw_gate_ctrl(I2C0_WCLK,1);
	csi_clk_hw_gate_ctrl(I2C1_WCLK,1);
	csi_clk_hw_gate_ctrl(SSP0_PCLK,1);
	csi_clk_hw_gate_ctrl(SSP1_PCLK,1);
	csi_clk_hw_gate_ctrl(SSP2_PCLK,1);
	csi_clk_hw_gate_ctrl(SSP0_WCLK,1);
	csi_clk_hw_gate_ctrl(SSP1_WCLK,1);
	csi_clk_hw_gate_ctrl(SSP2_WCLK,1);
	csi_clk_hw_gate_ctrl(GPIO_PD_PCLK,1);
	csi_clk_hw_gate_ctrl(ADC_PCLK,1);
	csi_clk_hw_gate_ctrl(ADC_WCLK,1);
	csi_clk_hw_gate_ctrl(AD_TIMER0_PCLK,1);
	csi_clk_hw_gate_ctrl(AD_TIMER1_PCLK,1);

}

static void sys_sleep_exit(zx29_pmu_priv_t *pmu_priv)
{
    if ((getreg32((volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG) & 0x1)==1) {
        cp_sleep_flag = 0;
        chip_exit_sleep(pmu_priv);
    } else if ((getreg32((volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG)& 0x2)==2) {
        cp_sleep_flag = 0;
        chip_exit_dormant(pmu_priv);
    }
}

static void pow_clk_config(void)
{
    m0_clk_disable();
    sec_disable();
    ap_clk_config();
    lsp_clk_config();
    ck_idle_en();
    aon_clk_config();
    top_clk_config();
}


static void set_ap_freq(si_ap_freq ap_freq)
{
	__write_bits(2,0,(volatile uint32_t *)TOP_CRPM_PD_AHB_SEL,ap_freq); //TOP_CRPM
}

static void chip_int_mask(void)
{
	__set_bit(5,(volatile uint32_t *)PCU_SW_CONFIG_MASK_REG);
	__set_bit(2,(volatile uint32_t *)PCU_SW_CONFIG_MASK_REG);
}
static void chip_int_unmask(void)
{
	__clear_bit(5,(volatile uint32_t *)PCU_SW_CONFIG_MASK_REG);
	__set_bit(2,(volatile uint32_t *)PCU_SW_CONFIG_MASK_REG);
}

static void chip_int_ack(void)
{
	__set_bit(9,(volatile uint32_t *)PCU_FSM_SEND_INT_CONFIG_REG);
}

static void pmu_delay_us(uint16_t us_time)
{
    volatile uint32_t time_cnt = us_time * 20;
	while (time_cnt--)
		;
}

void set_rtc_wakeup_time(uint32_t sleep_time)
{
	struct tm rtc_base;
	struct tm rtc_alarm;
	uint32_t rtc_time;
	volatile uint32_t rtc_alarm_time;
//    csi_rtc_readtime(0,&rtc_base);
    csi_rtc_get_time(rtc_handle, &rtc_base);

	rtc_time = mktime(&rtc_base);

	rtc_alarm_time = rtc_time + sleep_time;

	gmtime_r((const time_t *)&rtc_alarm_time,&rtc_alarm);
//    csi_rtc_startalarm(&rtc_alarm,NULL,NULL);
    zx29_rtc_startalarm(&rtc_alarm, NULL, NULL);
	printf("\n >>set rtc>\n");

}


uint32_t get_rtc_sleeptime(void)
{
   //U32 RtcSleepTime =0;
   struct tm rtc_base;
   struct tm rtc_alarm;
   uint32_t rtc_time = 0;
   uint32_t rtc_alarm_time = 0;

   uint32_t val = 0;
   uint32_t buf[2] = {0, 0};
//   csi_rtc_readtime(0, &rtc_base);
   csi_rtc_get_time(rtc_handle, &rtc_base);
   /*»ñÈ¡µ±Ç°Ê±¼ä*/
//   if(csi_rtc_get_alarm(&rtc_alarm) == 0) {
   if(zx29_rtc_get_alarm(&rtc_alarm) == 0) {
	   //¼ÆËã²îÖµ
	   rtc_time = mktime(&rtc_base);
	   rtc_alarm_time = mktime(&rtc_alarm);

	   if (rtc_alarm_time >= rtc_time) {
	   		val = (rtc_alarm_time - rtc_time) * 1000;
	   } else {
			val = 0;
	   }
   	} else {
		 oss_nv_read((AP_NVRW_BASE + AT_OFFSET(ap_cfg_t, wakeupCyc)), buf, sizeof(buf)-1);
		 val = buf[0]*1000;
		 set_rtc_wakeup_time(buf[0]);
   	}
   return val;
}

void pow_set_lpm_wakeup_timer(uint64_t mtime)
{
	uint32_t sleep_cnt = (mtime*32768)/1000;

	uint32_t lpm_cnt_tmp1 = getreg32((volatile uint32_t *)LPM_CNT_SYNC);
	putreg32(lpm_cnt_tmp1,(volatile uint32_t *)LPM_CNT_SYNC);

	uint32_t lpm_cnt_tmp2 = getreg32((volatile uint32_t *)LPM_CNT_SYNC);
	putreg32(lpm_cnt_tmp2,(volatile uint32_t *)LPM_CNT_SYNC);

	//uint32_t tmp = pow_readl(IOT_LPM_TIME1_CFG);
	uint32_t tmp = getreg32((volatile uint32_t *)LPM_CNT);

	putreg32((sleep_cnt+tmp),(volatile uint32_t *)LPM_TIME1_CFG);

	__set_bit(0,(volatile uint32_t *)LPM_TIME_EN);
}

uint32_t read_cnt_by_lpm(void)
{
    uint32_t lpm_cnt_tmp1;
	uint32_t lpm_cnt_tmp2;

	lpm_cnt_tmp1 = getreg32((volatile uint32_t *)LPM_CNT_SYNC);
	putreg32(lpm_cnt_tmp1,(volatile uint32_t *)LPM_CNT_SYNC);

	lpm_cnt_tmp2 = getreg32((volatile uint32_t *)LPM_CNT_SYNC);
	putreg32(lpm_cnt_tmp2,(volatile uint32_t *)LPM_CNT_SYNC);

	return getreg32((volatile uint32_t *)LPM_CNT);
}

static void chip_enter_sleep(zx29_pmu_priv_t *pmu_priv)
{
	printf("\n@@sleep mode@@\n");
    __set_bit(8,(volatile uint32_t *)PCU_FSM_SEND_INT_CONFIG_REG);  //ack shineng P423 ACK
	__set_bit(0,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG); // PCU_SBY chose the sleep mode

	__write_bits(16,0,(volatile uint32_t *)BACKUP_REG_RESERVE1,0x1); //backup reserve reserved int

	__write_bits(8,16,(volatile uint32_t *)CHIP_SLEEP_FLAG_ADDR,0x1); //PCU_ISRAM_AO SLEEP ADDR EN

}

static void chip_enter_standby(zx29_pmu_priv_t *pmu_priv)
{
	printf("\n@@standby mode@@\n");
	__write_bits(16,0,(volatile uint32_t *)BACKUP_REG_RESERVE1,0x3);
	putreg32(0x3,(volatile uint32_t *)BACKUP_REG_RESERVE2);

	putreg32(0x1,(volatile uint32_t *)RESET_STATE_HOT_BOOT);
    __set_bit(1,(volatile uint32_t *)CPSS_SLEEP_MODE_CONFIG_REG); //EN PCU_AO SLEEP MODE
    __clear_bit(0,(volatile uint32_t *)SBY_RESTORE_INT_DEAL_CONFIG_ADDR); //SBY wait timeout to do next

	__clear_bit(1,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);
	__set_bit(2,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);

	__set_bit(5,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);
	__set_bit(4,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);

	//reg32(0x20146010)|= (0x1<<16);
    //yunos_bsp_pmu_coreldo_setvol(MODE_STANDBY,CORELDO_0_900);
}

static void chip_enter_dormant(zx29_pmu_priv_t *pmu_priv)
{
	printf("\n@@dor mode@@\n");
	putreg32(0x1,(volatile uint32_t *)CHIP_DOR_FLAG_ADDR);

	__write_bits(16,0,(volatile uint32_t *)BACKUP_REG_RESERVE1,0x2);

	__set_bit(1,(volatile uint32_t *)CPSS_SLEEP_MODE_CONFIG_REG);
	__set_bit(1,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);

	__set_bit(5,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);
	__set_bit(4,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);
    __write_bits(2,3,(volatile uint32_t *)TOP_AO_SLEEP_MODE_CONFIG_REG,0x3); // dormant type
	__set_bit(2,(volatile uint32_t *)TOP_AO_SLEEP_MODE_CONFIG_REG);
	__set_bit(8,(volatile uint32_t *)PCU_FSM_SEND_INT_CONFIG_REG); //ack

	chip_pdcore_save(); //save ip reg

    set_ap_freq(FREQ_19M2);
	mpll_disable();

//    csi_bmu_config_mainbgp(bmu_handle, PMU_MODE_DORMANT, NULL, ENABLE);
//    csi_bmu_config_simldo(bmu_handle, PMU_MODE_DORMANT, NULL, ENABLE);
//    csi_bmu_lpcoreldo_setonoff(bmu_handle, PMU_MODE_DORMANT, ENABLE);
//    csi_bmu_config_coreldo(bmu_handle, PMU_MODE_DORMANT, CORELDO_0_900, DISABLE);
//    csi_bmu_config_buckdc(bmu_handle, PMU_MODE_DORMANT, NULL,ENABLE);
//    csi_bmu_mcoreldo_setonoff(bmu_handle, PMU_MODE_DORMANT, DISABLE);
//    csi_bmu_buckls1_setonoff(bmu_handle, PMU_MODE_DORMANT, DISABLE);
//    csi_bmu_config_lvioldo(bmu_handle, PMU_MODE_DORMANT, NULL, DISABLE);
	csi_bmu_dor_setonoff(bmu_handle, MAINBGP,ENABLE);
    csi_bmu_dor_setonoff(bmu_handle, SIMLDO,ENABLE);
	csi_bmu_dor_setonoff(bmu_handle, LPCORELDO,ENABLE);
	csi_bmu_coreldo_setvol(bmu_handle, PMU_MODE_DORMANT,CORELDO_0_900);
	csi_bmu_dor_setonoff(bmu_handle, BUCKDC,ENABLE);
	csi_bmu_dor_setonoff(bmu_handle, MCORELDO,DISABLE);
	csi_bmu_dor_setonoff(bmu_handle, BUCK_LS1,DISABLE);
	csi_bmu_dor_setonoff(bmu_handle, LVIOLDO,DISABLE);
    __write_bits(8,16,(volatile uint32_t *)CHIP_SLEEP_FLAG_ADDR,0x2);

}

static void chip_exit_sleep(zx29_pmu_priv_t *pmu_priv)
{
	__clear_bit(0,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);

	chip_int_unmask();

    chip_int_ack();
	__clear_bit(8,(volatile uint32_t *)PCU_FSM_SEND_INT_CONFIG_REG);
}

static void chip_exit_dormant(zx29_pmu_priv_t *pmu_priv)
{

    mpll_enable();
	set_ap_freq(FREQ_76M8);

	chip_pdcore_restore();
    if (getreg32((volatile uint32_t *)PMU_BOOST_EN_FLAG) == 1) {
    	__clear_bit(1,(volatile uint32_t *)PMU_REG_ENN_CONFIG);
		pmu_delay_us(2);
		__clear_bit(1,(volatile uint32_t *)PMU_BOOST_CONFIG);
		pmu_delay_us(2);
		__clear_bit(2,(volatile uint32_t *)PMU_BOOST_CONFIG);
	}

	__clear_bit(1,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);

	__clear_bit(5,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);
	__clear_bit(4,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);

	__write_bits(2,3,(volatile uint32_t *)TOP_AO_SLEEP_MODE_CONFIG_REG,0);

	chip_int_unmask();

	chip_int_ack();
	__clear_bit(8,(volatile uint32_t *)PCU_FSM_SEND_INT_CONFIG_REG);

}

static void sys_sleep_enter(uint32_t sleep_time,uint32_t ap_sleep_start)
{
    zx29_pmu_priv_t *pmu_priv = &pmu_handle[0];

    if((getreg32((volatile uint32_t *)AO_RD_SIGNAL_STA_REG1) & 0x2) == 0x2 ) { //check the cp sleep flag is valid or not
        chip_int_mask();        //cp soc interrupt mask
        cp_sleep_flag = 1;
        #if 1
        uint64_t cp_end_sleep_time = getreg32((volatile uint32_t *)CP_SLEEP_TIME_REG);

        uint64_t lpm_time_cur = read_cnt_by_lpm(); //read the lpm cnt value
        if (cp_end_sleep_time < lpm_time_cur) {
            chip_int_unmask();
            return;
        }
        uint64_t cp_sleep_remain_time = (cp_end_sleep_time - lpm_time_cur)*1000/32768;
        if (MIN(cp_sleep_remain_time, sleep_time) < PRE_WAKEUP_THRESHOLD) {
            chip_int_unmask();
            return;
        }
        uint64_t sys_sleep_time = MIN(cp_sleep_remain_time, sleep_time) - PRE_WAKEUP_THRESHOLD;

        #endif
        if (sys_sleep_time > ALL_CHIP_ENTER) {
            pow_set_lpm_wakeup_timer(sys_sleep_time);
            if ((getreg32((volatile uint32_t *)APSS_CONFIG_REG) & 0x1) == 0x1) {
                chip_enter_sleep(pmu_priv);
            } else {
                if ((getreg32((volatile uint32_t *)(CPSS_SLEEP_MODE_CONFIG_REG)) & 0x1) == 0x1) {
                    chip_enter_sleep(pmu_priv);
                } else {
                    if (sys_sleep_time < ALL_CHIP_DORMANT) {
                        chip_enter_sleep(pmu_priv);
                    } else if (sys_sleep_time < ALL_CHIP_STANDBY) {
                        chip_enter_dormant(pmu_priv);
                    } else {
                         chip_enter_standby(pmu_priv);
                    }
                }
            }
        } else {
            __write_bits(8,16,(volatile uint32_t *)CHIP_SLEEP_FLAG_ADDR,0);
            chip_int_unmask();
        }

    } else
        __write_bits(8,16,(volatile uint32_t *)CHIP_SLEEP_FLAG_ADDR,0);
}

uint32_t ap_sleep_start=0;
void pmu_before_enter_lpm(void)
{
    zx29_pmu_priv_t *pmu_priv = &pmu_handle[0];
    ap_sleep_start = read_cnt_by_lpm();
    printf("\n@@sleep start at %d@@\n",ap_sleep_start);
    putreg32(ap_sleep_start,(volatile uint32_t *)(uint32_t *)CHIP_SLEEP_ENTER_ADDR);
    uint32_t plat_sleep_time = get_rtc_sleeptime();
    printf("\n@@ck power@@\n");
    printf("\n@@sleep plat %d@@\n", plat_sleep_time);

    ap2cp_flag_disable();
    __write_bits(8,0,(volatile uint32_t *)CHIP_SLEEP_FLAG_ADDR,0x2);
    putreg32(0x0,(volatile uint32_t *)CHIP_DOR_FLAG_ADDR);
    csi_vic_clear_wakeup_irq(PIC_IRQ_TIMER0);

    csi_pmu_config_wakeup_source(pmu_priv, PIC_IRQ_RTC_ALARM, PMU_WAKEUP_TYPE_PULSE, PMU_WAKEUP_POL_HIGH, 1);

    __set_bit(1,(volatile uint32_t *)APSS_CONFIG_REG);
    __set_bit(3,(volatile uint32_t *)APSS_CONFIG_REG);
    putreg32(0x7FFFFFFF,(volatile uint32_t *)APSS_SLEEP_TIME_REG);

//    sys_sleep_enter(plat_sleep_time,ap_sleep_start);

    pow_tick_reg_save();//±£´ætimer¼Ä´æÆ÷
    pow_icu_mask_reg_save();//±£´æICU¼Ä´æÆ÷
    pow_context_save();
}
uint32_t pmu_after_exit_lpm(void)
{
    zx29_pmu_priv_t *pmu_priv = &pmu_handle[0];

    pow_context_restore();

    pow_icu_mask_reg_restore();//±£´æICU¼Ä´æÆ÷
    pow_tick_reg_restore();

    __clear_bit(1,(volatile uint32_t *)APSS_CONFIG_REG);
    __clear_bit(3,(volatile uint32_t *)APSS_CONFIG_REG);
    sys_sleep_exit(pmu_priv);
    pow_clk_config();
//    csi_vic_set_wakeup_irq(PIC_IRQ_TIMER0_WAKE);

    uint32_t sleep_exit_time  = read_cnt_by_lpm();//È·ÈÏÒ»ÏÂÊ±ÖÓÔ´
    printf("\n@@sleep exit at %d@@\n",sleep_exit_time);
    putreg32(sleep_exit_time,(volatile uint32_t *)CHIP_SLEEP_EXIT_ADDR);
    printf("\n@@exit power@@\n");
    ap2cp_flag_enable();

    return (((sleep_exit_time - ap_sleep_start)*1000)/32768);
}

/*******************************************************************************
*                               Gloable function implementations                *
*******************************************************************************/
/**
 *brief      Initialize pmu Interface. 1. Initializes the resources needed for
 *              the pmu interface 2.registers event callback function
 *
 *param[in]  idx device id
 *param[in]  cb_event  Pointer to \ref pmu_event_cb_t
 *return     pointer to pmu handle
*/
pmu_handle_t csi_pmu_initialize(int32_t idx, pmu_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_PMU_NUM) {
        return NULL;
    }

    uint32_t irq = 0u;
    uint32_t base = 0u;
    /* obtain the pmu information */
    int32_t real_idx = target_get_pmu(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    zx29_pmu_priv_t *pmu_priv = &pmu_handle[idx];

    pmu_priv->base = base;

    /* initialize the pmu context */
    pmu_priv->cb = cb_event;

	__clear_bit(1,(volatile uint32_t *)CPSS_SLEEP_MODE_CONFIG_REG);
	__clear_bit(2,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);
	__clear_bit(5,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);
	__clear_bit(4,(volatile uint32_t *)TOP_SBY_SLEEP_MODE_CONFIG_REG);
    pow_pcu_init();
	pow_clk_config();
#if 0
	apll_disable();
#endif
	oss_icp_channel_register_cb(OSS_ICP_CHANNEL_PSM,psm_icp_isr);
//	csi_vic_set_wakeup_irq(VIC_WAKEUP_SOURCE);//CPU VIC »½ÐÑÔ´ÅäÖÃ
    csi_vic_set_wakeup_irq(PIC_IRQ_ICP_DSP2CK);
    csi_vic_set_wakeup_irq(PIC_IRQ_AD_TIMER0);
    csi_vic_set_wakeup_irq(PIC_IRQ_RTC_ALARM);
    csi_vic_set_wakeup_irq(PIC_IRQ_UART1);
    csi_vic_set_wakeup_irq(PIC_IRQ_31);

    rtc_handle = csi_rtc_initialize(0, NULL);

    struct tm kk;

    kk.tm_sec = 0;
    kk.tm_min = 0;
    kk.tm_hour = 0;
    kk.tm_mday = 6;
    kk.tm_mon = 0;
    kk.tm_year = 70;

    csi_rtc_set_time(rtc_handle, &kk);

    bmu_handle = csi_bmu_initialize(0, NULL);

    return (pmu_handle_t)pmu_priv;
}

/**
 * brief       De-initialize pmu Interface. stops operation and releases
 * the software resources used by the interface
 * param[in]   handle  pmu handle to operate.
 * return      error code
*/
int32_t csi_pmu_uninitialize(pmu_handle_t handle)
{
    zx29_pmu_priv_t *pmu_priv = handle;
    pmu_priv->cb = NULL;

    return 0;
}

/**
 * brief       config pmu mode.
 * param[in]handle      pmu handle to operate.
 * param[in]mode        pmu mode ,ref pmu_mode_e:ECB/CBC
 * param[in]keylen_bits length of pmu key
 * param[in]endian      endian mode of pmu,\ref pmu_endian_mode_e
 * param[in]arg         the address where iv is stored
 * return           0-success , negative-error code.
*/

int32_t csi_pmu_enter_sleep(pmu_handle_t handle, pmu_mode_e mode)
{

    zx29_pmu_priv_t *pmu_priv = handle;

    switch (mode) {
        /* sleep mode */
        case PMU_MODE_SLEEP:
            asm("stop");
            break;

        /* dormant mode */
        case PMU_MODE_DORMANT:
            chip_enter_dormant(pmu_priv);
            break;

        /* standby mode */
        case PMU_MODE_STDBY:
            if (ck802_ns_cpu_save() == 0){
                do_cpu_sleep_tee();
//            while(1);
                ck802_ns_cpu_resume();
            }
            break;

        /* run/shutdown mode */
        case PMU_MODE_RUN:
        case PMU_MODE_SHUTDOWN:
            return ERR_PMU(DRV_ERROR_UNSUPPORTED);

        default:
            return ERR_PMU(DRV_ERROR_PARAMETER);
    }

    pmu_priv->mode = mode;
    return 0;
}

/**
 * brief       set key of pmu.
 * param[in]handle      pmu handle to operate.
 * param[in/out]context caller allocated memory used as internal context
 * param[in]key         pointer where pmu key is stored
 * param[in]key_len     length of pmu key
 * param[in]enc         pmu mode, DECRYPT/ENCRYPT
 *
 * return           0-success , negative-error code.
*/
int32_t csi_pmu_exit_sleep(pmu_handle_t handle)
{

    zx29_pmu_priv_t *pmu_priv = handle;

    switch (pmu_priv->mode) {
        /* sleep mode */
        case PMU_MODE_SLEEP:
            chip_exit_sleep(pmu_priv);
            break;

        /* dormant mode */
        case PMU_MODE_DORMANT:
            chip_exit_dormant(pmu_priv);
            break;

        /* standby mode */
        case PMU_MODE_STDBY:
        /* run/shutdown mode */
        case PMU_MODE_RUN:
        case PMU_MODE_SHUTDOWN:
            return ERR_PMU(DRV_ERROR_UNSUPPORTED);

        default:
            return ERR_PMU(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       Config the wakeup source.
  \param[in]   handle  pmu handle to operate
  \param[in]   type    \ref pmu_wakeup_type
  \param[in]   pol     \ref pmu_wakeup_pol
  \param[in]   enable  flag control the wakeup source is enable or not
  \return      error code
*/
int32_t csi_pmu_config_wakeup_source(pmu_handle_t handle, uint32_t irq_num, pmu_wakeup_type_e type, pmu_wakeup_pol_e pol, uint8_t enable)
{
    zx29_pmu_priv_t *pmu_priv = handle;
    if (type == PMU_WAKEUP_TYPE_PULSE) {
        __clear_bit(13,(volatile uint32_t *)SBY_INT_WAKEUP_TYPE_REG1);
    }
    if (pol == PMU_WAKEUP_POL_LOW) {
	    __clear_bit(13, (volatile uint32_t *)SBY_INT_WAKEUP_POL_REG1);
    }
    if (enable) {
	    __clear_bit(14, (volatile uint32_t *)SBY_INT_WAKEUP_DIS_REG1);
    }
    return 0;

}

/**
* brief       Get pmu status.
* param[in]   handle  pmu handle to operate.
* return      pmu status \ref pmu_status_t
*/
//pmu_status_e csi_pmu_get_status(pmu_handle_t handle)
//{
//    zx29_pmu_priv_t *pmu_priv = handle;
//    return 0;
//}


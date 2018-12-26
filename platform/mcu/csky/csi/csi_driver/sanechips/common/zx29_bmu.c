/**
 * File: bmu_si.c
 * Brief: Implementation of bmu
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author: yangsheng <yang.sheng@sanechips.com.cn>
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
 *                           Include header files                              *
 ******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "soc.h"
#include <io.h>
#include "drv_bmu.h"
#include "zx29_bmu.h"
#include "drv_intc.h"

/*******************************************************************************
 *                             Macro definitions                               *
 ******************************************************************************/
#define ERR_BMU(errno) (CSI_DRV_ERRNO_BMU_BASE | errno)
/*******************************************************************************
 *                             Type definitions                                *
 ******************************************************************************/
typedef struct {
    uint32_t base;
    bmu_event_cb_t cb;
    pmu_mode_e mode;
} zx29_bmu_priv_t;

/*******************************************************************************
 *                        Local function declarations                          *
 ******************************************************************************/

/*******************************************************************************
 *                         Local variable definitions                          *
 ******************************************************************************/
static zx29_bmu_priv_t bmu_handle[CONFIG_BMU_NUM];
volatile struct bmu_si_regs *bmu_si_reg_base = (volatile struct bmu_si_regs *)SI_PMU_BASE;

extern int32_t target_get_bmu_count(void);
extern int32_t target_get_bmu(int32_t idx, uint32_t *base, uint32_t *irq);

/*******************************************************************************
 *                        Global variable definitions                          *
 ******************************************************************************/
uint32_t sp_num;
uint32_t lp_num;
uint32_t llp_num;

/*******************************************************************************
 *                      Inline function implementations                        *
 ******************************************************************************/

/*******************************************************************************
 *                      Local function implementations                         *
 ******************************************************************************/
void bmu_sp_isr(void)
{
	sp_num++;
	//printf("wakeup1 sp!\n");
	csi_intc_ack_irq(ICU_IRQ_WAKE1_SP);
}

void bmu_lp_isr(void)
{
	lp_num++;
	//printf("wakeup1 lp!\n");
	/*sw key for shutdown*/
	bmu_si_reg_base->normal_sw_control_key_reg2 = 0xaa;
	bmu_si_reg_base->normal_sw_control_key_reg1 = 0x55;
	csi_intc_ack_irq(ICU_IRQ_WAKE1_LP);
}

void bmu_llp_isr(void)
{
	llp_num++;
	//printf("wakeup1 llp!\n");
	csi_intc_ack_irq(ICU_IRQ_WAKEUP1_LLP);
}

void (*bmu_si_batdetect_cb)(int);

void csi_bmu_sdncmp_set_cb(void * par)
{
	bmu_si_batdetect_cb = par;
}

static int s_bat_state;
void bmu_sdncmp_isr(void)
{
	int int_val;
	int_val =  HAL_FEXT(*(volatile uint32_t*)(PCU_SBY_BASEADDR + INT_M_READOUT_OFFSET), SDNCMP_EN_FALL_INT_READ);
	if(0x1 == int_val) {
		HAL_FINS(*(volatile uint32_t*)(PCU_SBY_BASEADDR + VBAT_DET_INT_MASK_OFFSET), SDN_EN_FALL_INT_MASK, 0);
		HAL_FINS(*(volatile uint32_t*)(PCU_SBY_BASEADDR + VBAT_DET_INT_MASK_OFFSET), OUT_SDNCMP_FALL_INT_MASK, 1);
		/*bat det ok*/
		s_bat_state = 1;
		HAL_FINS(*(volatile uint32_t*)(PCU_SBY_BASEADDR + INT_WAKEUP_CLEAR2_CONFIG), SDNCMP_EN_FALL_INT_CLEAR, 1);
		HAL_FINS(*(volatile uint32_t*)(PCU_SBY_BASEADDR + INT_WAKEUP_CLEAR2_CONFIG), SDNCMP_EN_FALL_INT_CLEAR, 0);

		if(bmu_si_batdetect_cb)
			(*bmu_si_batdetect_cb)(1);
		else
			printf("bmu sdncmp battery insert!\n");
	}
	else if(0x2 == int_val) {
		/*bat remove*/
		HAL_FINS(*(volatile uint32_t*)(PCU_SBY_BASEADDR + VBAT_DET_INT_MASK_OFFSET), SDN_EN_FALL_INT_MASK, 1);
		HAL_FINS(*(volatile uint32_t*)(PCU_SBY_BASEADDR + VBAT_DET_INT_MASK_OFFSET), OUT_SDNCMP_FALL_INT_MASK, 1);
		s_bat_state = 0;
		//if detect the remove once only
		//HAL_FINS(*(volatile uint32_t*)(PCU_SBY_BASEADDR + VBAT_DET_INT_MASK_OFFSET), OUT_SDNCMP_FALL_INT_MASK, 0);

		/*clear int*/
		HAL_FINS(*(volatile uint32_t*)(PCU_SBY_BASEADDR + INT_WAKEUP_CLEAR2_CONFIG), OUT_SDNCMP_FALL_INT_CLEAR, 1);
		HAL_FINS(*(volatile uint32_t*)(PCU_SBY_BASEADDR + INT_WAKEUP_CLEAR2_CONFIG), OUT_SDNCMP_FALL_INT_CLEAR, 0);
		if(bmu_si_batdetect_cb)
			(*bmu_si_batdetect_cb)(0);
		else
		printf("bmu sdncmp battery removed!\n");

	} else {
		/*error state*/
		printf("bmu sdncmp error state int_reg = %d!\n",int_val);
	}


	//csi_intc_ack_irq(PIC_IRQ_VBAT_DETN);
//	return 0;
}

void bmu_sdnpvd_isr(void)
{
	int s_bat_v = 0;
	s_bat_v = HAL_FEXT(bmu_si_reg_base->actt_bmu_status_read_addr, SDNPVD_INTN_5V);
	if (0 == s_bat_v) {
		/*low power*/
		printf("bmu sdnpvd battery low power!\n");
	} else {
		/*nomorl*/
		printf("bmu sdnpvd battery voltage normal!\n");
	}
	HAL_FINS(*(volatile uint32_t*)(PCU_SBY_BASEADDR + INT_WAKEUP_CLEAR2_CONFIG),SDNPVD_INT_CLEAR,1);
	HAL_FINS(*(volatile uint32_t*)(PCU_SBY_BASEADDR + INT_WAKEUP_CLEAR2_CONFIG),SDNPVD_INT_CLEAR,0);
	//printf("wakeup1 llp!\n");
	//csi_intc_ack_irq(ICU_IRQ_SDN_PVD_INTN_5V);
}

static void bmu_irq_install(void)
{
	/*wake1 sp irq install*/
	csi_intc_set_attribute(ICU_IRQ_WAKE1_SP, 1, INT_MODE_RISING_EDGE);
//	(void)csi_intc_attach_irq(ICU_IRQ_WAKE1_SP, (int)bmu_sp_isr);
    csi_intc_enable_irq(ICU_IRQ_WAKE1_SP);

	/*wake1 lp irq install*/
	csi_intc_set_attribute(ICU_IRQ_WAKE1_LP, 1, INT_MODE_RISING_EDGE);
//	(void)csi_intc_attach_irq(ICU_IRQ_WAKE1_LP, (int)bmu_lp_isr);
	csi_intc_enable_irq(ICU_IRQ_WAKE1_LP);

	/*wake1 llp irq install*/
	csi_intc_set_attribute(ICU_IRQ_WAKEUP1_LLP, 1, INT_MODE_RISING_EDGE);
//	(void)csi_intc_attach_irq(ICU_IRQ_WAKEUP1_LLP, (int)bmu_llp_isr);
	csi_intc_enable_irq(ICU_IRQ_WAKEUP1_LLP);
}

/*******************************************************************************
 *                      Global function implementations						   *
 ******************************************************************************/
/**
 * Pull pshold up or down.
 *
 * @param[in]  enable  Flag of up or down.
 * @return     none
 */
int32_t csi_bmu_set_pshold(bmu_handle_t handle, bmu_pshold_status_e enable)
{
	HAL_BIT_INSR(bmu_si_reg_base->pshold_config_reg2, PS_HOLD_SHIFT, enable);
    return 0;
}

/**
 * Set key long long press to do.
 *
 * @param[in]  enable  Flag of shutdown or restart.
 * @return     none
 */
int32_t csi_bmu_set_llp_todo(bmu_handle_t handle, bmu_llp_todo_e enable)
{
	HAL_BIT_INSR(bmu_si_reg_base->enable_config_reg1, LLP_TO_DO_SHIFT, enable);
    return 0;
}

/**
 * Set mainbgp voltage.
 *
 * @param[in]  mainbgp_vol  Voltage of Regulator.
 * @return     none
 */
int32_t csi_bmu_mainbgp_setvol(bmu_handle_t handle, bmu_mainbgp_vol_e mainbgp_vol)
{
	HAL_FINS(bmu_si_reg_base->reg_bgp_config_reg, RUN_REG_MAINBGP, mainbgp_vol);
    return 0;
}

/**
 * Set sdnbgp voltage in lpmode.
 *
 * @param[in]  vol  Voltage of sdnbgp in lpmode.
 * @return     none
 */
int32_t csi_bmu_sdnbgp_lpmode_setvol(bmu_handle_t handle, bmu_sdnbgp_lpmode_vol_e vol)
{
	HAL_BIT_INSR(bmu_si_reg_base->reg_bgp_config_reg, REG_SDNBGP_MODE_SHIFT, DISABLE);
	HAL_FINS(bmu_si_reg_base->reg_bgp_config_reg, REG_SDNBGP , vol);
    return 0;
}

/**
 * Set sdnbgp voltage in hpmode.
 *
 * @param[in]  vol  Voltage of sdnbgp in hpmode.
 * @return     none
 */
int32_t csi_bmu_sdnbgp_hpmode_setvol(bmu_handle_t handle, bmu_sdnbgp_hpmode_vol_e vol)
{
	HAL_BIT_INSR(bmu_si_reg_base->reg_bgp_config_reg, REG_SDNBGP_MODE_SHIFT, ENABLE);
	HAL_FINS(bmu_si_reg_base->reg_bgp_config_reg, REG_SDNBGP, vol);
    return 0;
}

/**
 * Set sdnpvd voltage.
 *
 * @param[in]  sdnpvd_vol  Voltage of sdnpvd.
 * @return     none
 */
int32_t csi_bmu_sdnpvd_setvol(bmu_handle_t handle, bmu_sdnpvd_vol_e sdnpvd_vol)
{
	HAL_FINS(bmu_si_reg_base->actt_bmu_status_write_addr, REG_SDNPVD_5V, sdnpvd_vol);
    return 0;
}

/**
 * Set sdnpvd on or off.
 *
 * @param[in]  enable  Flag of enable or disable.
 * @return     none
 */
int32_t csi_bmu_sdnpvd_setonoff(bmu_handle_t handle, bmu_enable_e enable)
{
	HAL_BIT_INSR(bmu_si_reg_base->actt_bmu_status_write_addr, SDNPVD_EN_5V_SHIFT, enable);
    return 0;
}

/**
 * Select sdnpvd source.
 *
 * @param[in]  enable  Flag of sdnpvd source.
 * @return     none
 */
int32_t csi_bmu_sdnpvd_selsrc(bmu_handle_t handle, bmu_pvdsel_5V_e pvdsel)
{
	HAL_BIT_INSR(bmu_si_reg_base->actt_bmu_status_write_addr, PVDSEL_5V_SHIFT, pvdsel);
    return 0;
}

/**
 * Set sdnbor voltage.
 *
 * @param[in]  sdnbor_vol  Voltage of sdnbor.
 * @return     none
 */
int32_t csi_bmu_sdnbor_setvol(bmu_handle_t handle, bmu_sdnbor_vol_e sdnbor_vol)
{
	HAL_FINS(bmu_si_reg_base->actt_bmu_status_write_addr, REG_SDNBOR_5V, sdnbor_vol);
    return 0;
}

/**
 * Set sdncmp current.
 *
 * @param[in]  sdncmp_cur  Current of sdncmp.
 * @return     none
 */
int32_t csi_bmu_sdncmp_setcur(bmu_handle_t handle, bmu_sdncmp_cur_e sdncmp_cur)
{
	HAL_FINS(bmu_si_reg_base->actt_bmu_status_write_addr, REG_SDNCMP_18, sdncmp_cur);
    return 0;
}

/**
 * Set sdncmp on or off.
 *
 * @param[in]  enable  Flag of enable or disable.
 * @return     none
 */
int32_t csi_bmu_sdncmp_setonoff(bmu_handle_t handle, bmu_enable_e enable)
{
	HAL_BIT_INSR(bmu_si_reg_base->actt_bmu_status_write_addr, SDNCMP_EN_18_SHIFT, enable);
    return 0;
}

/**
 * Set hvioldo voltage in different bmu mod.
 *
 * @param[in]  pmu_mode  Pmu mode.
 * @param[in]  hvioldo   Voltage value.
 * @return     0 is 0, others is error
 */
int32_t csi_bmu_hvioldo_setvol(bmu_handle_t handle, pmu_mode_e pmu_mode, bmu_hvioldo_vol_e hvioldo_vol)
{
	switch (pmu_mode) {
	case PMU_MODE_RUN:
		HAL_FINS(bmu_si_reg_base->run_config_reg1, RUN_REG_HVIOLDO, hvioldo_vol);
    	break;

	case PMU_MODE_DORMANT:
		HAL_FINS(bmu_si_reg_base->dor_config_reg1, DOR_REG_HVIOLDO, hvioldo_vol);
    	break;

	case PMU_MODE_STDBY:
		HAL_FINS(bmu_si_reg_base->sby_config_reg1, SBY_REG_LVIOLDO, hvioldo_vol);
    	break;

	default:
		return ERR_BMU(DRV_ERROR_PARAMETER);
	}
	return 0;
}

/**
 * Set simldo voltage in different bmu mod.
 *
 * @param[in]  pmu_mode Pmu mode.
 * @param[in]  simldo   Voltage value.
 * @return     0 is 0, others is error
 */
int32_t csi_bmu_simldo_setvol(bmu_handle_t handle, pmu_mode_e pmu_mode, bmu_simldo_vol_e simldo_vol)
{
	switch (pmu_mode) {
	case PMU_MODE_RUN:
		HAL_FINS(bmu_si_reg_base->run_config_reg2, RUN_REG_SIMLDO, simldo_vol);
    	break;

	case PMU_MODE_DORMANT:
		HAL_FINS(bmu_si_reg_base->dor_config_reg2, DOR_REG_SIMLDO, simldo_vol);
    	break;

	case PMU_MODE_STDBY:
		HAL_FINS(bmu_si_reg_base->sby_config_reg2, SBY_REG_SIMLDO, simldo_vol);
    	break;

	default:
		return ERR_BMU(DRV_ERROR_PARAMETER);
	}
	return 0;
}

/**
 * Set buckdc voltage in different bmu mod.
 *
 * @param[in]  pmu_mode Pmu mode.
 * @param[in]  buckdc   Voltage value.
 * @return     0 is 0, others is error
 */
int32_t csi_bmu_buckdc_setvol(bmu_handle_t handle, pmu_mode_e pmu_mode, bmu_buckdc_vol_e buckdc_vol)
{
	switch (pmu_mode) {
	case PMU_MODE_RUN:
		HAL_FINS(bmu_si_reg_base->run_config_reg1, RUN_REG_BUCKDC, buckdc_vol);
    	break;

	case PMU_MODE_DORMANT:
		HAL_FINS(bmu_si_reg_base->dor_config_reg1, DOR_REG_BUCKDC, buckdc_vol);
    	break;

	case PMU_MODE_STDBY:
		HAL_FINS(bmu_si_reg_base->sby_config_reg1, SBY_REG_BUCKDC, buckdc_vol);
    	break;

	default:
		return ERR_BMU(DRV_ERROR_PARAMETER);
	}
	return 0;
}

/**
 * Set coreldo voltage in different bmu mod.
 *
 * @param[in]  pmu_mode Pmu mode.
 * @param[in]  coreldo  Voltage value.
 * @return     0 is 0, others is error
 */
int32_t csi_bmu_coreldo_setvol(bmu_handle_t handle, pmu_mode_e pmu_mode, bmu_coreldo_vol_e coreldo_vol)
{
	switch (pmu_mode) {
	case PMU_MODE_RUN:
		HAL_FINS(bmu_si_reg_base->run_config_reg1, RUN_CORELDO, coreldo_vol);
    	break;

	case PMU_MODE_DORMANT:
		HAL_FINS(bmu_si_reg_base->dor_config_reg1, DOR_CORELDO, coreldo_vol);
    	break;

	case PMU_MODE_STDBY:
		HAL_FINS(bmu_si_reg_base->sby_config_reg1, SBY_CORELDO, coreldo_vol);
    	break;

	default:
		return ERR_BMU(DRV_ERROR_PARAMETER);
	}
	return 0;
}

/**
 * Set lvioldo voltage in different bmu mod.
 *
 * @param[in]  pmu_mode Pmu mode.
 * @param[in]  lvioldo  Voltage value.
 * @return     0 is 0, others is error
 */
int32_t csi_bmu_lvioldo_setvol(bmu_handle_t handle, pmu_mode_e pmu_mode, bmu_lvioldo_vol_e lvioldo_vol)
{
	switch (pmu_mode) {
	case PMU_MODE_RUN:
		HAL_FINS(bmu_si_reg_base->run_config_reg1, RUN_REG_LVIOLDO, lvioldo_vol);
    	break;

	case PMU_MODE_DORMANT:
		HAL_FINS(bmu_si_reg_base->dor_config_reg1, DOR_REG_LVIOLDO, lvioldo_vol);
    	break;

	case PMU_MODE_STDBY:
		HAL_FINS(bmu_si_reg_base->sby_config_reg1, SBY_REG_LVIOLDO, lvioldo_vol);
    	break;

	default:
		return ERR_BMU(DRV_ERROR_PARAMETER);
	}
	return 0;
}

/**
 * Set analdo voltage.
 *
 * @param[in]  analdo  Voltage value.
 * @return     0 is 0, others is error
 */
int32_t csi_bmu_analdo_setvol(bmu_handle_t handle, bmu_analdo_vol_e analdo_vol)
{
	HAL_FINS(bmu_si_reg_base->actt_bmu_status_write_addr, REG_ANALDO_5V, analdo_vol);
	return 0;
}

/**
 * Set sbyldo voltage.
 *
 * @param[in]  sbyldo  Voltage value.
 * @return     0 is 0, others is error
 */
int32_t csi_bmu_sbyldo_setvol(bmu_handle_t handle, bmu_sbyldo_vol_e sbyldo_vol)
{
	HAL_FINS(bmu_si_reg_base->actt_bmu_status_write_addr, REG_SBYLDO_5V, sbyldo_vol);
	return 0;
}

/**
 * Set regulator on or off in run mode of bmu.
 *
 * @param[in]  regulator  Identify of Regulator.
 * @param[in]  enable     Flag of on or off.
 * @return     0 is 0, others is error
 */
int32_t csi_bmu_run_setonoff(bmu_handle_t handle, bmu_regulator_e regulator, bmu_enable_e enable)
{
    switch (regulator) {
	case BUCK_LS0:
		HAL_BIT_INSR(bmu_si_reg_base->run_config_reg1, RUN_BUCK_LS0_EN_SHIFT, enable);
    	break;

	case BUCK_LS1:
		HAL_BIT_INSR(bmu_si_reg_base->run_config_reg1, RUN_BUCK_LS1_EN_SHIFT, enable);
    	break;

	case HVIOLDO:
		HAL_BIT_INSR(bmu_si_reg_base->run_config_reg1, RUN_HVIOLDO_EN_SHIFT, enable);
    	break;

	case LVIOLDO:
		HAL_BIT_INSR(bmu_si_reg_base->run_config_reg1,RUN_LVIOLDO_EN_SHIFT,enable);
    	break;

	case ABB_LS:
		HAL_BIT_INSR(bmu_si_reg_base->run_config_reg1, RUN_ABB_LS_EN_SHIFT, enable);
    	break;

	case HVIO_LS:
		HAL_BIT_INSR(bmu_si_reg_base->run_config_reg1, RUN_HVIO_LS_EN_SHIFT, enable);
    	break;

	case ACORELDO:
		HAL_BIT_INSR(bmu_si_reg_base->run_config_reg1, RUN_ACORE_LDO_EN_SHIFT, enable);
    	break;

	case SIMLDO:
		HAL_BIT_INSR(bmu_si_reg_base->run_config_reg2, RUN_SIMLDO_EN_SHIFT, enable);
    	break;

	case ANALDO:
		HAL_BIT_INSR(bmu_si_reg_base->run_config_reg2, RUN_ANALDO_EN_SHIFT, enable);
    	break;

	default:
		return ERR_BMU(DRV_ERROR_PARAMETER);
	}
	return 0;
}

/**
 * Set regulator on or off in dormant mode of bmu.
 *
 * @param[in]  regulator  Identify of Regulator.
 * @param[in]  enable     Flag of on or off.
 * @return     0 is 0, others is error
 */
int32_t csi_bmu_dor_setonoff(bmu_handle_t handle, bmu_regulator_e regulator, bmu_enable_e enable)
{
    switch (regulator) {
	case BUCK_LS0:
		HAL_BIT_INSR(bmu_si_reg_base->dor_config_reg1, DOR_BUCK_LS0_EN_SHIFT, enable);
    	break;

	case BUCK_LS1:
		HAL_BIT_INSR(bmu_si_reg_base->dor_config_reg1, DOR_BUCK_LS1_EN_SHIFT, enable);
    	break;

	case HVIOLDO:
		HAL_BIT_INSR(bmu_si_reg_base->dor_config_reg1, DOR_HVIOLDO_EN_SHIFT, enable);
    	break;

	case LVIOLDO:
		HAL_BIT_INSR(bmu_si_reg_base->dor_config_reg1, DOR_LVIOLDO_EN_SHIFT, enable);
    	break;

	case ABB_LS:
		HAL_BIT_INSR(bmu_si_reg_base->dor_config_reg2, DOR_ABB_LS_EN_SHIFT, enable);
    	break;

	case HVIO_LS:
		HAL_BIT_INSR(bmu_si_reg_base->dor_config_reg2, DOR_HVIO_LS_EN_SHIFT, enable);
    	break;

	case ACORELDO:
		HAL_BIT_INSR(bmu_si_reg_base->dor_config_reg1, DOR_ACORE_LDO_EN_SHIFT, enable);
    	break;

	case SIMLDO:
		HAL_BIT_INSR(bmu_si_reg_base->dor_config_reg2, DOR_SIMLDO_EN_SHIFT, enable);
    	break;

	case ANALDO:
		HAL_BIT_INSR(bmu_si_reg_base->dor_config_reg2, DOR_ANALDO_EN_SHIFT, enable);
    	break;

	case LPCORELDO:
		HAL_BIT_INSR(bmu_si_reg_base->dor_config_reg1, DOR_LPCORELDO_EN_SHIFT, enable);
    	break;

	case MCORELDO:
		HAL_BIT_INSR(bmu_si_reg_base->dor_config_reg1, DOR_MCORELDO_EN_SHIFT, enable);
    	break;

	case MAINBGP:
		HAL_BIT_INSR(bmu_si_reg_base->dor_config_reg1, DOR_MAINBGP_EN_SHIFT, enable);
    	break;

	case BUCKDC:
		HAL_BIT_INSR(bmu_si_reg_base->dor_config_reg1, DOR_BUCKDC_EN_SHIFT, enable);
    	break;

	default:
		return ERR_BMU(DRV_ERROR_PARAMETER);
	}
	return 0;
}

/**
 * Set regulator on or off in standby mode of bmu.
 *
 * @param[in]  regulator  Identify of Regulator.
 * @param[in]  enable     Flag of on or off.
 * @return     0 is 0, others is error
 */
int32_t csi_bmu_sby_setonoff(bmu_handle_t handle, bmu_regulator_e regulator, bmu_enable_e enable)
{
    switch (regulator) {
	case BUCK_LS0:
		HAL_BIT_INSR(bmu_si_reg_base->sby_config_reg1, SBY_BUCK_LS0_EN_SHIFT, enable);
    	break;

	case BUCK_LS1:
		HAL_BIT_INSR(bmu_si_reg_base->sby_config_reg1, SBY_BUCK_LS1_EN_SHIFT, enable);
    	break;

	case HVIOLDO:
		HAL_BIT_INSR(bmu_si_reg_base->sby_config_reg1, SBY_HVIOLDO_EN_SHIFT, enable);
    	break;

	case LVIOLDO:
		HAL_BIT_INSR(bmu_si_reg_base->sby_config_reg1, SBY_LVIOLDO_EN_SHIFT, enable);
    	break;

	case ABB_LS:
		HAL_BIT_INSR(bmu_si_reg_base->sby_config_reg2, SBY_ABB_LS_EN_SHIFT, enable);
    	break;

	case HVIO_LS:
		HAL_BIT_INSR(bmu_si_reg_base->sby_config_reg2, SBY_HVIO_LS_EN_SHIFT, enable);
    	break;

	case ACORELDO:
		HAL_BIT_INSR(bmu_si_reg_base->sby_config_reg1, SBY_ACORE_LDO_EN_SHIFT, enable);
    	break;

	case SIMLDO:
		HAL_BIT_INSR(bmu_si_reg_base->sby_config_reg2, SBY_SIMLDO_EN_SHIFT, enable);
    	break;

	case ANALDO:
		HAL_BIT_INSR(bmu_si_reg_base->sby_config_reg2, SBY_ANALDO_EN_SHIFT, enable);
    	break;

	case LPCORELDO:
		HAL_BIT_INSR(bmu_si_reg_base->sby_config_reg1, SBY_LPCORELDO_EN_SHIFT, enable);
    	break;

	case MCORELDO:
		HAL_BIT_INSR(bmu_si_reg_base->sby_config_reg1, SBY_MCORELDO_EN_SHIFT, enable);
    	break;

	case MAINBGP:
		HAL_BIT_INSR(bmu_si_reg_base->sby_config_reg1, SBY_MAINBGP_EN_SHIFT, enable);
    	break;

	case BUCKDC:
		HAL_BIT_INSR(bmu_si_reg_base->sby_config_reg1, SBY_BUCKDC_EN_SHIFT, enable);
    	break;

	default:
		return ERR_BMU(DRV_ERROR_PARAMETER);
	}
	return 0;
}

int32_t csi_ext_bmu_boost_setonoff(bmu_handle_t handle, bmu_enable_e enable)
{
//	int ret;

//	ret = csi_gpio_set_mode(SI_GPIO_30,GPIO_OUT,PULLING_NONE);
//	if(ret < 0)
//		return ret;
//
//	ret = csi_gpio_set_value(SI_GPIO_30,enable);
//	if(ret < 0)
//		return ret;

	return 0;
}

int32_t csi_ext_bmu_pa_setonoff(bmu_handle_t handle, bmu_enable_e enable)
{
//	int ret;

//	ret = csi_gpio_set_mode(SI_GPIO_54,GPIO_OUT,PULLING_NONE);
//	if(ret < 0)
//		return ret;
//
//	ret = csi_gpio_set_value(SI_GPIO_54,enable);
//	if(ret < 0)
//		return ret;

	return 0;
}

void csi_bmu_sdncmp_init(zx29_bmu_priv_t *bmu_priv)
{
	/*int init*/
	HAL_FINS(*(volatile uint32_t*)(PCU_SBY_BASEADDR + VBAT_DET_INT_MASK_OFFSET), OUT_SDNCMP_FALL_INT_MASK, 1);
	HAL_FINS(*(volatile uint32_t*)(PCU_SBY_BASEADDR + VBAT_DET_INT_MASK_OFFSET), SDN_EN_FALL_INT_MASK, 0);
	/*sdncmp irq install for battery remove detect*/
	csi_intc_set_attribute(PIC_IRQ_VBAT_DETN, 1, INT_MODE_DOUBLE_EDGE);
//	(void)csi_intc_attach_irq(PIC_IRQ_VBAT_DETN, (int)bmu_sdncmp_isr);
	csi_intc_enable_irq(PIC_IRQ_VBAT_DETN);

	REG32(0x20141000 + 0x18) &= ~(1<<1);
}

void csi_bmu_sdnpvd_init(zx29_bmu_priv_t *bmu_priv)
{
	csi_bmu_sdnpvd_selsrc(bmu_priv, VBAT);
	csi_bmu_sdnpvd_setvol(bmu_priv, SDNPVD_3_400);

	/*sdnpvd irq install  for low battery detect*/
	csi_intc_set_attribute(ICU_IRQ_SDN_PVD_INTN_5V, 1, INT_MODE_RISING_EDGE);
//	(void)csi_intc_attach_irq(ICU_IRQ_SDN_PVD_INTN_5V, (int)bmu_sdnpvd_isr);
	csi_intc_enable_irq(ICU_IRQ_SDN_PVD_INTN_5V);
}

/**
 * This function will initialize bmu and install interrupt
 *
 * @return     none
 */
void csi_bmu_init(zx29_bmu_priv_t *bmu_priv)
{
	/*csi_bmu_set_pshold(PSHOLD_UP);*/

	/*open key 16hz clk, recognize wakeup1 key*/
	HAL_BIT_SET(*(unsigned int *)SI_LPM_RST_REG, SI_LPM_RST_EN_SHIFT);
	HAL_BIT_SET(*(unsigned int *)SI_SBY_CRPM_RTCCLKEN_REG, SI_SBY_CRPM_RTC_CLKEN_SHIFT);
	HAL_BIT_CLR(*(unsigned int *)SI_LPM_RST_REG, SI_LPM_RST_EN_SHIFT);

	bmu_irq_install();

	/*Keyon_counter_enable*/
	HAL_BIT_SET(bmu_si_reg_base->enable_config_reg1, KEYON_COUNTER_EN_SHIFT);

	/*enable llp ,eable force shutdown function if enable */
	//HAL_BIT_SET(bmu_si_reg_base->enable_config_reg1, LLP_EN_SHIFT);

	/*set llp to shutdown or restart*/
	csi_bmu_set_llp_todo(bmu_priv, LLP_RESTART);

	/*enable lp ,enable shudown function if enable*/
	//HAL_BIT_SET(bmu_si_reg_base->enable_config_reg1, LP_EN_SHIFT);

	/*poweroff_protect_enable,default enable*/
	HAL_BIT_CLR(bmu_si_reg_base->enable_config_reg1, POWEROFF_PROTECT_EN_SHIFT);
	csi_bmu_sdncmp_init(bmu_priv);
	csi_bmu_sdnpvd_init(bmu_priv);
}

/**
  \brief       Initialize BMU Interface. 1. Initializes the resources needed for the BMU interface 2.registers event callback function
  \param[in]   idx bmu id
  \param[in]   cb_event  Pointer to \ref bmu_event_cb_t
  \return      return bmu handle if success
*/
bmu_handle_t csi_bmu_initialize(int32_t idx, bmu_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_BMU_NUM) {
        return NULL;
    }

    uint32_t irq = 0u;
    uint32_t base = 0u;
    /* obtain the pmu information */
    int32_t real_idx = target_get_bmu(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    zx29_bmu_priv_t *bmu_priv = &bmu_handle[idx];

    bmu_priv->base = base;

    /* initialize the bmu context */
    bmu_priv->cb = cb_event;

    csi_bmu_init(bmu_priv);

    return (bmu_handle_t)bmu_priv;
}

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

#ifndef YUNOS_BSP_PMU_H
#define YUNOS_BSP_PMU_H

#include <stdint.h>

typedef enum pmu_mode_s {
    WAIT_MODE = 0,
    DOZE_MODE,
    STOP_MODE,
    SLEEP_MODE
}pmu_mode_t;

#if 1
enum pmu_regulator {
	BUCK_LS0	= 0,		/*run/dor/sby onoff*/
	BUCK_LS1	= 1,		/*run/dor/sby onoff*/
	HVIOLDO		= 2,		/*run/dor/sby onoff*/	/*run/dor/sby prog BW3*/
	LVIOLDO		= 3,		/*run/dor/sby onoff*/	/*run/dor/sby prog BW3*/
	ABB_LS		= 4,		/*run/dor/sby onoff*/
	HVIO_LS		= 5,		/*run/dor/sby onoff*/
	ACORELDO	= 6,		/*run/dor sby onoff*/
	SIMLDO		= 7,		/*run/dor/sby onoff*/	/*run/dor/sby prog BW3*/
	ANALDO		= 8,		/*run/dor/sby onoff*/	
	LPCORELDO	= 9,		/*    dor/sby onoff*/
	MCORELDO	= 10,		/*    dor/sby onoff*/
	MAINBGP		= 11,		/*    dor/sby onoff*/
	BUCKDC		= 12,    	/*    dor/sby onoff*/	/*run/dor/sby prog BW8*/
	CORELDO		= 13,								/*run/dor/sby prog BW4*/
};

enum pmu_mode {
	MODE_RUN	  = 0,
	MODE_DORMANT  = 1,
	MODE_STANDBY  = 2,
	MODE_SHUTDOWN = 3,
};

enum pmu_hvioldo_vol {
	HVIOLDO_1_800 = 0,
	HVIOLDO_2_500 = 1,
	HVIOLDO_2_700 = 2,
	HVIOLDO_2_850 = 3,/*default*/
	HVIOLDO_3_000 = 4,
	HVIOLDO_3_300 = 5,
};

enum pmu_lvioldo_vol {
	LVIOLDO_1_600 = 0,
	LVIOLDO_1_640 = 1,
	LVIOLDO_1_680 = 2,
	LVIOLDO_1_720 = 3,
	LVIOLDO_1_760 = 4,
	LVIOLDO_1_800 = 5,/*default*/
	LVIOLDO_1_840 = 6,
	LVIOLDO_1_880 = 7,
};

enum pmu_simldo_vol {
	SIMLDO_1_800 = 0,/*default*/
	SIMLDO_2_500 = 1,
	SIMLDO_2_700 = 2,
	SIMLDO_2_850 = 3,	
	SIMLDO_3_000 = 4,
	SIMLDO_3_300 = 5,
};

enum pmu_buckdc_vol {
	BUCKDC_1_500 = 0,
	BUCKDC_1_620 = 4,
	BUCKDC_1_740 = 8,
	BUCKDC_1_800 = 10,
	BUCKDC_1_830 = 11,
	BUCKDC_1_860 = 12,
	BUCKDC_1_950 = 15,
};

enum pmu_coreldo_vol {
	CORELDO_0_900 = 0,
	CORELDO_1_020 = 4,
	CORELDO_1_110 = 7,
	CORELDO_1_200 = 10,/*default*/
};

enum pmu_analdo_vol {
	ANALDO_2_000 = 0,
	ANALDO_2_500 = 1,
	ANALDO_2_700 = 2,
	ANALDO_2_850 = 3,	
	ANALDO_3_000 = 4,
	ANALDO_3_300 = 5,/*default*/
};

enum pmu_sbyldo_vol {
	SBYLDO_1_640 = 0,
	SBYLDO_1_680 = 1,
	SBYLDO_1_720 = 2,
	SBYLDO_1_760 = 3,	
	SBYLDO_1_800 = 4,
	SBYLDO_1_840 = 5,/*default*/
	SBYLDO_1_880 = 6,
	SBYLDO_1_920 = 7,
};

enum pmu_sdnpvd_vol {
	SDNPVD_2_200 = 0,/*default*/
	SDNPVD_2_400 = 1,
	SDNPVD_2_600 = 2,
	SDNPVD_2_800 = 3,	
	SDNPVD_3_000 = 4,
	SDNPVD_3_200 = 5,
	SDNPVD_3_400 = 6,
	SDNPVD_3_600 = 7,
};

enum pmu_sdnbor_vol {
	SDNBOR_1_800 = 0,
	SDNBOR_2_000 = 1,/*default*/
	SDNBOR_2_200 = 2,
	SDNBOR_2_500 = 3,	
	SDNBOR_2_800 = 4,
	SDNBOR_3_000 = 5,
	SDNBOR_3_100 = 6,
	SDNBOR_3_200 = 7,
};

enum pmu_sdncmp_cur {
	SDNCMP_9uA  = 0,/*default*/
	SDNCMP_11uA = 1,
	SDNCMP_16uA = 2,
	SDNCMP_30uA = 3,	
};

enum pmu_mainbgp_vol {
	MAINBGP_0_720 = 0,
	MAINBGP_0_730 = 1,
	MAINBGP_0_740 = 2,
	MAINBGP_0_750 = 3,	
	MAINBGP_0_760 = 4,
	MAINBGP_0_770 = 5,
	MAINBGP_0_780 = 6,
	MAINBGP_0_790 = 7,
	MAINBGP_0_800 = 8,/*default*/
	MAINBGP_0_810 = 9,
	MAINBGP_0_820 = 10,
	MAINBGP_0_830 = 11,
	MAINBGP_0_840 = 12,
	MAINBGP_0_850 = 13,
	MAINBGP_0_860 = 14,
	MAINBGP_0_870 = 15,
};

enum pmu_sdnbgp_lpmode_vol {
	SDNBGP_LPMODE_1_137 = 0,
	SDNBGP_LPMODE_1_158 = 1,
	SDNBGP_LPMODE_1_175 = 2,
	SDNBGP_LPMODE_1_194 = 3,	
	SDNBGP_LPMODE_1_214 = 4,/*default*/
	SDNBGP_LPMODE_1_233 = 5,
	SDNBGP_LPMODE_1_252 = 6,
	SDNBGP_LPMODE_1_271 = 7,
};

enum pmu_sdnbgp_hpmode_vol {
	SDNBGP_HPMODE_1_152 = 0,
	SDNBGP_HPMODE_1_171 = 1,
	SDNBGP_HPMODE_1_191 = 2,
	SDNBGP_HPMODE_1_210 = 3,	
	SDNBGP_HPMODE_1_230 = 4,/*default*/
	SDNBGP_HPMODE_1_251 = 5,
	SDNBGP_HPMODE_1_270 = 6,
	SDNBGP_HPMODE_1_289 = 7,
};

enum pmu_pvdsel_5V {
    VBAT   = 0,/*default*/
    PVD_IN = 1,
};

enum pmu_llp_todo {
    LLP_SHUTDOWN = 0,/*default*/
    LLP_RESTART  = 1,
};

enum pmu_pshold_status {
    PSHOLD_DOWN  = 0,/*default*/
    PSHOLD_UP    = 1,
};

enum pmu_enable {
    DISABLE = 0,
    ENABLE  = 1,    
};
#endif

/**
 * This function will initialize pmu and install interrupt
 * @param[in]  wakesrc 
 * @return     none
 */
void yunos_bsp_pmu_init(uint32_t wakesrc);

/**
 * Set hvioldo voltage in different pmu mod.
 * @param[in]  pmu_mode  Pmu mode.
 * @param[in]  hvioldo   Voltage value.
 * @return     0 is OK, others is error
 */
int yunos_bsp_pmu_hvioldo_setvol(enum pmu_mode pmu_mode,enum pmu_hvioldo_vol hvioldo_vol);

/**
 * Set simldo voltage in different pmu mod.
 * @param[in]  pmu_mode Pmu mode.
 * @param[in]  simldo   Voltage value.  
 * @return     0 is OK, others is error
 */
int yunos_bsp_pmu_simldo_setvol(enum pmu_mode pmu_mode,enum pmu_simldo_vol simldo_vol);

/**
 * Set buckdc voltage in different pmu mod.
 * @param[in]  pmu_mode Pmu mode.
 * @param[in]  buckdc   Voltage value.  
 * @return     0 is OK, others is error
 */
int yunos_bsp_pmu_buckdc_setvol(enum pmu_mode pmu_mode,enum pmu_buckdc_vol buckdc_vol);

/**
 * Set coreldo voltage in different pmu mod.
 * @param[in]  pmu_mode Pmu mode.
 * @param[in]  coreldo  Voltage value.  
 * @return     0 is OK, others is error
 */
int yunos_bsp_pmu_coreldo_setvol(enum pmu_mode pmu_mode,enum pmu_coreldo_vol coreldo_vol);

/**
 * Set lvioldo voltage in different pmu mod.
 * @param[in]  pmu_mode Pmu mode.
 * @param[in]  lvioldo  Voltage value.  
 * @return     0 is OK, others is error
 */
int yunos_bsp_pmu_lvioldo_setvol(enum pmu_mode pmu_mode,enum pmu_lvioldo_vol lvioldo_vol);

/**
 * Set analdo voltage.
 * @param[in]  analdo  Voltage value.  
 * @return     0 is OK, others is error
 */
int yunos_bsp_pmu_analdo_setvol(enum pmu_analdo_vol analdo_vol);

/**
 * Set sbyldo voltage.
 * @param[in]  sbyldo  Voltage value.  
 * @return     0 is OK, others is error
 */
int yunos_bsp_pmu_sbyldo_setvol(enum pmu_sbyldo_vol sbyldo_vol);

/**
 * Set regulator on or off in run mode of pmu.
 * @param[in]  regulator  Identify of Regulator.
 * @param[in]  enable     Flag of on or off.  
 * @return     0 is OK, others is error
 */
int yunos_bsp_pmu_run_setonoff(enum pmu_regulator regulator,enum pmu_enable enable);

/**
 * Set regulator on or off in dormant mode of pmu.
 * @param[in]  regulator  Identify of Regulator.
 * @param[in]  enable     Flag of on or off.  
 * @return     0 is OK, others is error
 */
int yunos_bsp_pmu_dor_setonoff(enum pmu_regulator regulator,enum pmu_enable enable);

/**
 * Set regulator on or off in standby mode of pmu.
 * @param[in]  regulator  Identify of Regulator.
 * @param[in]  enable     Flag of on or off.  
 * @return     0 is OK, others is error
 */
int yunos_bsp_pmu_sby_setonoff(enum pmu_regulator regulator,enum pmu_enable enable);

/**
 * Set mainbgp voltage.
 * @param[in]  mainbgp_vol  Voltage of Regulator.
 * @return     none
 */
void yunos_bsp_pmu_mainbgp_setvol(enum pmu_mainbgp_vol mainbgp_vol);

/**
 * Set sdnbgp voltage in lpmode.
 * @param[in]  vol  Voltage of sdnbgp in lpmode.
 * @return     none
 */
void yunos_bsp_pmu_sdnbgp_lpmode_setvol(enum pmu_sdnbgp_lpmode_vol vol);

/**
 * Set sdnbgp voltage in hpmode.
 * @param[in]  vol  Voltage of sdnbgp in hpmode.
 * @return     none
 */
void yunos_bsp_pmu_sdnbgp_hpmode_setvol(enum pmu_sdnbgp_hpmode_vol vol);

/**
 * Set sdnpvd voltage.
 * @param[in]  sdnpvd_vol  Voltage of sdnpvd.
 * @return     none
 */
void yunos_bsp_pmu_sdnpvd_setvol(enum pmu_sdnpvd_vol sdnpvd_vol);

/**
 * Set sdnpvd on or off.
 * @param[in]  enable  Flag of enable or disable.
 * @return     none
 */
void yunos_bsp_pmu_sdnpvd_setonoff(enum pmu_enable enable);

/**
 * Select sdnpvd source.
 * @param[in]  enable  Flag of sdnpvd source.
 * @return     none
 */
void yunos_bsp_pmu_sdnpvd_selsrc(enum pmu_pvdsel_5V pvdsel);

/**
 * Set sdnbor voltage.
 * @param[in]  sdnbor_vol  Voltage of sdnbor.
 * @return     none
 */
void yunos_bsp_pmu_sdnbor_setvol(enum pmu_sdnbor_vol sdnbor_vol);

/**
 * Set sdncmp current.
 * @param[in]  sdncmp_cur  Current of sdncmp.
 * @return     none
 */
void yunos_bsp_pmu_sdncmp_setcur(enum pmu_sdncmp_cur sdncmp_cur);

/**
 * Set sdncmp on or off.
 * @param[in]  enable  Flag of enable or disable.
 * @return     none
 */
void yunos_bsp_pmu_sdncmp_setonoff(enum pmu_enable enable);
void yunos_bsp_pmu_sdncmp_set_cb(void* par);
/**
 * Pull pshold up or down.
 * @param[in]  enable  Flag of up or down.
 * @return     none
 */
void yunos_bsp_pmu_set_pshold(enum pmu_pshold_status enable);

/**
 * Set key long long press to do.
 * @param[in]  enable  Flag of shutdown or restart.
 * @return     none
 */
void yunos_bsp_pmu_set_llp_todo(enum pmu_llp_todo enable);

#endif /* YUNOS_BSP_PMU_H */

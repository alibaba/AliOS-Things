/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
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
/******************************************************************************
 * @file     drv_bmu.h
 * @brief    header file for bmu driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef _CSI_BMU_H_
#define _CSI_BMU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <drv_common.h>
#include <drv_pmu.h>

/// definition for bmu handle.
typedef void *bmu_handle_t;

typedef enum {
    BUCK_LS0    = 0,        /*run/dor/sby onoff*/
    BUCK_LS1    = 1,        /*run/dor/sby onoff*/
    HVIOLDO     = 2,        /*run/dor/sby onoff*/   /*run/dor/sby prog BW3*/
    LVIOLDO     = 3,        /*run/dor/sby onoff*/   /*run/dor/sby prog BW3*/
    ABB_LS      = 4,        /*run/dor/sby onoff*/
    HVIO_LS     = 5,        /*run/dor/sby onoff*/
    ACORELDO    = 6,        /*run/dor sby onoff*/
    SIMLDO      = 7,        /*run/dor/sby onoff*/   /*run/dor/sby prog BW3*/
    ANALDO      = 8,        /*run/dor/sby onoff*/
    LPCORELDO   = 9,        /*    dor/sby onoff*/
    MCORELDO    = 10,       /*    dor/sby onoff*/
    MAINBGP     = 11,       /*    dor/sby onoff*/
    BUCKDC      = 12,       /*    dor/sby onoff*/   /*run/dor/sby prog BW8*/
    CORELDO     = 13,                               /*run/dor/sby prog BW4*/
} bmu_regulator_e;

typedef enum {
    DISABLE = 0,
    ENABLE  = 1,
}bmu_enable_e;

typedef enum {
    HVIOLDO_1_800 = 0,
    HVIOLDO_2_500 = 1,
    HVIOLDO_2_700 = 2,
    HVIOLDO_2_850 = 3,/*default*/
    HVIOLDO_3_000 = 4,
    HVIOLDO_3_300 = 5,
} bmu_hvioldo_vol_e;

typedef enum {
    LVIOLDO_1_600 = 0,
    LVIOLDO_1_640 = 1,
    LVIOLDO_1_680 = 2,
    LVIOLDO_1_720 = 3,
    LVIOLDO_1_760 = 4,
    LVIOLDO_1_800 = 5,/*default*/
    LVIOLDO_1_840 = 6,
    LVIOLDO_1_880 = 7,
} bmu_lvioldo_vol_e;

typedef enum {
    SIMLDO_1_800 = 0,/*default*/
    SIMLDO_2_500 = 1,
    SIMLDO_2_700 = 2,
    SIMLDO_2_850 = 3,
    SIMLDO_3_000 = 4,
    SIMLDO_3_300 = 5,
} bmu_simldo_vol_e;

typedef enum {
    BUCKDC_1_500 = 0,
    BUCKDC_1_620 = 4,
    BUCKDC_1_740 = 8,
    BUCKDC_1_800 = 10,
    BUCKDC_1_830 = 11,
    BUCKDC_1_860 = 12,
    BUCKDC_1_950 = 15,
} bmu_buckdc_vol_e;

typedef enum {
    CORELDO_0_900 = 0,
    CORELDO_1_020 = 4,
    CORELDO_1_110 = 7,
    CORELDO_1_200 = 10,/*default*/
} bmu_coreldo_vol_e;

typedef enum {
    ANALDO_2_000 = 0,
    ANALDO_2_500 = 1,
    ANALDO_2_700 = 2,
    ANALDO_2_850 = 3,
    ANALDO_3_000 = 4,
    ANALDO_3_300 = 5,/*default*/
} bmu_analdo_vol_e;

typedef enum {
    SBYLDO_1_640 = 0,
    SBYLDO_1_680 = 1,
    SBYLDO_1_720 = 2,
    SBYLDO_1_760 = 3,
    SBYLDO_1_800 = 4,
    SBYLDO_1_840 = 5,/*default*/
    SBYLDO_1_880 = 6,
    SBYLDO_1_920 = 7,
} bmu_sbyldo_vol_e;

typedef enum {
    SDNPVD_2_200 = 0,/*default*/
    SDNPVD_2_400 = 1,
    SDNPVD_2_600 = 2,
    SDNPVD_2_800 = 3,
    SDNPVD_3_000 = 4,
    SDNPVD_3_200 = 5,
    SDNPVD_3_400 = 6,
    SDNPVD_3_600 = 7,
} bmu_sdnpvd_vol_e;

typedef enum {
    SDNBOR_1_800 = 0,
    SDNBOR_2_000 = 1,/*default*/
    SDNBOR_2_200 = 2,
    SDNBOR_2_500 = 3,
    SDNBOR_2_800 = 4,
    SDNBOR_3_000 = 5,
    SDNBOR_3_100 = 6,
    SDNBOR_3_200 = 7,
} bmu_sdnbor_vol_e;

typedef enum {
    SDNCMP_9uA  = 0,/*default*/
    SDNCMP_11uA = 1,
    SDNCMP_16uA = 2,
    SDNCMP_30uA = 3,
} bmu_sdncmp_cur_e;

typedef enum {
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
} bmu_mainbgp_vol_e;

typedef enum {
    SDNBGP_LPMODE_1_137 = 0,
    SDNBGP_LPMODE_1_158 = 1,
    SDNBGP_LPMODE_1_175 = 2,
    SDNBGP_LPMODE_1_194 = 3,
    SDNBGP_LPMODE_1_214 = 4,/*default*/
    SDNBGP_LPMODE_1_233 = 5,
    SDNBGP_LPMODE_1_252 = 6,
    SDNBGP_LPMODE_1_271 = 7,
} bmu_sdnbgp_lpmode_vol_e;

typedef enum {
    SDNBGP_HPMODE_1_152 = 0,
    SDNBGP_HPMODE_1_171 = 1,
    SDNBGP_HPMODE_1_191 = 2,
    SDNBGP_HPMODE_1_210 = 3,
    SDNBGP_HPMODE_1_230 = 4,/*default*/
    SDNBGP_HPMODE_1_251 = 5,
    SDNBGP_HPMODE_1_270 = 6,
    SDNBGP_HPMODE_1_289 = 7,
} bmu_sdnbgp_hpmode_vol_e;

typedef enum {
    VBAT   = 0,/*default*/
    PVD_IN = 1,
} bmu_pvdsel_5V_e;

typedef enum {
    LLP_SHUTDOWN = 0,/*default*/
    LLP_RESTART  = 1,
} bmu_llp_todo_e;

typedef enum {
    PSHOLD_DOWN  = 0,/*default*/
    PSHOLD_UP    = 1,
} bmu_pshold_status_e;

/****** BMU Event *****/
typedef enum {
    BMU_EVENT_SEND_COMPLETE       = 0,  ///< Send completed; however BMU may still transmit data
    BMU_EVENT_RECEIVE_COMPLETE    = 1,  ///< Receive completed
    BMU_EVENT_RECEIVED            = 2,  ///< Receiving data
    BMU_EVENT_TRANSFER_COMPLETE   = 3,  ///< Transfer completed
    BMU_EVENT_TX_COMPLETE         = 4,  ///< Transmit completed (optional)
    BMU_EVENT_TX_UNDERFLOW        = 5,  ///< Transmit data not available (Synchronous Slave)
    BMU_EVENT_RX_OVERFLOW         = 6,  ///< Receive data overflow
    BMU_EVENT_RX_TIMEOUT          = 7,  ///< Receive character timeout (optional)
    BMU_EVENT_RX_BREAK            = 8,  ///< Break detected on receive
    BMU_EVENT_RX_FRAMING_ERROR    = 9,  ///< Framing error detected on receive
    BMU_EVENT_RX_PARITY_ERROR     = 10,  ///< Parity error detected on receive
} bmu_event_e;

typedef void (*bmu_event_cb_t)(int32_t idx, bmu_event_e event);   ///< Pointer to \ref bmu_event_cb_t : BMU Event call back.

/**
\brief BMU Driver Capabilities.
*/
typedef struct  {
    uint32_t asynchronous       : 1;      ///< supports BMU (Asynchronous) mode
    uint32_t synchronous_master : 1;      ///< supports Synchronous Master mode
    uint32_t synchronous_slave  : 1;      ///< supports Synchronous Slave mode
    uint32_t single_wire        : 1;      ///< supports BMU Single-wire mode
    uint32_t event_tx_complete  : 1;      ///< Transmit completed event
    uint32_t event_rx_timeout   : 1;      ///< Signal receive character timeout event
} bmu_capabilities_t;

/**
  \brief       Initialize BMU Interface. 1. Initializes the resources needed for the BMU interface 2.registers event callback function
  \param[in]   idx bmu id
  \param[in]   cb_event  Pointer to \ref bmu_event_cb_t
  \return      return bmu handle if success
*/
bmu_handle_t csi_bmu_initialize(int32_t idx, bmu_event_cb_t cb_event);

/**
  \brief       De-initialize BMU Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  bmu handle to operate.
  \return      error code
*/
int32_t csi_bmu_uninitialize(bmu_handle_t handle);

/**
  \brief       Get driver capabilities.
  \param[in]   handle  bmu handle to operate.
  \return      \ref bmu_capabilities_t
*/
bmu_capabilities_t csi_bmu_get_capabilities(bmu_handle_t handle);

/**
 * Pull pshold up or down.
 *
 * @param[in]  enable  Flag of up or down.
 * @return     none
 */
int32_t csi_bmu_set_pshold(bmu_handle_t handle, bmu_pshold_status_e enable);

/**
 * Set key long long press to do.
 *
 * @param[in]  enable  Flag of shutdown or restart.
 * @return     none
 */
int32_t csi_bmu_set_llp_todo(bmu_handle_t handle, bmu_llp_todo_e enable);

/**
 * Set mainbgp voltage.
 *
 * @param[in]  mainbgp_vol  Voltage of Regulator.
 * @return     none
 */
int32_t csi_bmu_mainbgp_setvol(bmu_handle_t handle, bmu_mainbgp_vol_e mainbgp_vol);

/**
 * Set sdnbgp voltage in lpmode.
 *
 * @param[in]  vol  Voltage of sdnbgp in lpmode.
 * @return     none
 */
int32_t csi_bmu_sdnbgp_lpmode_setvol(bmu_handle_t handle, bmu_sdnbgp_lpmode_vol_e vol);

/**
 * Set sdnbgp voltage in hpmode.
 *
 * @param[in]  vol  Voltage of sdnbgp in hpmode.
 * @return     none
 */
int32_t csi_bmu_sdnbgp_hpmode_setvol(bmu_handle_t handle, bmu_sdnbgp_hpmode_vol_e vol);

/**
 * Set sdnpvd voltage.
 *
 * @param[in]  sdnpvd_vol  Voltage of sdnpvd.
 * @return     none
 */
int32_t csi_bmu_sdnpvd_setvol(bmu_handle_t handle, bmu_sdnpvd_vol_e sdnpvd_vol);

/**
 * Set sdnpvd on or off.
 *
 * @param[in]  enable  Flag of enable or disable.
 * @return     none
 */
int32_t csi_bmu_sdnpvd_setonoff(bmu_handle_t handle, bmu_enable_e enable);

/**
 * Select sdnpvd source.
 *
 * @param[in]  enable  Flag of sdnpvd source.
 * @return     none
 */
int32_t csi_bmu_sdnpvd_selsrc(bmu_handle_t handle, bmu_pvdsel_5V_e pvdsel);

/**
 * Set sdnbor voltage.
 *
 * @param[in]  sdnbor_vol  Voltage of sdnbor.
 * @return     none
 */
int32_t csi_bmu_sdnbor_setvol(bmu_handle_t handle, bmu_sdnbor_vol_e sdnbor_vol);

/**
 * Set sdncmp current.
 *
 * @param[in]  sdncmp_cur  Current of sdncmp.
 * @return     none
 */
int32_t csi_bmu_sdncmp_setcur(bmu_handle_t handle, bmu_sdncmp_cur_e sdncmp_cur);

/**
 * Set sdncmp on or off.
 *
 * @param[in]  enable  Flag of enable or disable.
 * @return     none
 */
int32_t csi_bmu_sdncmp_setonoff(bmu_handle_t handle, bmu_enable_e enable);

/**
 * Set hvioldo voltage in different bmu mod.
 *
 * @param[in]  pmu_mode  Pmu mode.
 * @param[in]  hvioldo   Voltage value.
 * @return     0 is OK, others is error
 */
int32_t csi_bmu_hvioldo_setvol(bmu_handle_t handle, pmu_mode_e pmu_mode, bmu_hvioldo_vol_e hvioldo_vol);

/**
 * Set simldo voltage in different bmu mod.
 *
 * @param[in]  pmu_mode Pmu mode.
 * @param[in]  simldo   Voltage value.
 * @return     0 is OK, others is error
 */
int32_t csi_bmu_simldo_setvol(bmu_handle_t handle, pmu_mode_e pmu_mode, bmu_simldo_vol_e simldo_vol);

/**
 * Set buckdc voltage in different bmu mod.
 *
 * @param[in]  pmu_mode Pmu mode.
 * @param[in]  buckdc   Voltage value.
 * @return     0 is OK, others is error
 */
int32_t csi_bmu_buckdc_setvol(bmu_handle_t handle, pmu_mode_e pmu_mode, bmu_buckdc_vol_e buckdc_vol);

/**
 * Set coreldo voltage in different bmu mod.
 *
 * @param[in]  pmu_mode Pmu mode.
 * @param[in]  coreldo  Voltage value.
 * @return     0 is OK, others is error
 */
int32_t csi_bmu_coreldo_setvol(bmu_handle_t handle, pmu_mode_e pmu_mode, bmu_coreldo_vol_e coreldo_vol);

/**
 * Set lvioldo voltage in different bmu mod.
 *
 * @param[in]  pmu_mode Pmu mode.
 * @param[in]  lvioldo  Voltage value.
 * @return     0 is OK, others is error
 */
int32_t csi_bmu_lvioldo_setvol(bmu_handle_t handle, pmu_mode_e pmu_mode, bmu_lvioldo_vol_e lvioldo_vol);

/**
 * Set analdo voltage.
 *
 * @param[in]  analdo  Voltage value.
 * @return     0 is OK, others is error
 */
int32_t csi_bmu_analdo_setvol(bmu_handle_t handle, bmu_analdo_vol_e analdo_vol);

/**
 * Set sbyldo voltage.
 *
 * @param[in]  sbyldo  Voltage value.
 * @return     0 is OK, others is error
 */
int32_t csi_bmu_sbyldo_setvol(bmu_handle_t handle, bmu_sbyldo_vol_e sbyldo_vol);

/**
 * Set regulator on or off in run mode of bmu.
 *
 * @param[in]  regulator  Identify of Regulator.
 * @param[in]  enable     Flag of on or off.
 * @return     0 is OK, others is error
 */
int32_t csi_bmu_run_setonoff(bmu_handle_t handle, bmu_regulator_e regulator, bmu_enable_e enable);

/**
 * Set regulator on or off in dormant mode of bmu.
 *
 * @param[in]  regulator  Identify of Regulator.
 * @param[in]  enable     Flag of on or off.
 * @return     0 is OK, others is error
 */
int32_t csi_bmu_dor_setonoff(bmu_handle_t handle, bmu_regulator_e regulator, bmu_enable_e enable);

/**
 * Set regulator on or off in standby mode of bmu.
 *
 * @param[in]  regulator  Identify of Regulator.
 * @param[in]  enable     Flag of on or off.
 * @return     0 is OK, others is error
 */
int32_t csi_bmu_sby_setonoff(bmu_handle_t handle, bmu_regulator_e regulator, bmu_enable_e enable);

#ifdef __cplusplus
}
#endif

#endif /* _CSI_BMU_H_ */

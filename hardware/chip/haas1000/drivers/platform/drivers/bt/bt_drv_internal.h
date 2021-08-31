/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __BT_DRV_INTERNAL_H__
#define __BT_DRV_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

typedef uint32_t BT_CONTROLER_TRACE_TYPE;
#define BT_CONTROLER_TRACE_TYPE_INTERSYS                0x01
#define BT_CONTROLER_TRACE_TYPE_CONTROLLER              0x02
#define BT_CONTROLER_TRACE_TYPE_LMP_TRACE               0x04
#define BT_CONTROLER_TRACE_TYPE_SPUV_HCI_BUFF           0x08
#define BT_CONTROLER_FILTER_TRACE_TYPE_A2DP_STREAM      0x10
#define BT_CONTROLER_TRACE_TYPE_DUMP_BUFF               0x20

#define BT_SUB_SYS_TYPE     0
#define MCU_SYS_TYPE        1
#define BT_EM_AREA_1_TYPE   2
#define BT_EM_AREA_2_TYPE   3

uint8_t btdrv_rf_init(void);
void btdrv_test_mode_rf_txpwr_init(void);
void btdrv_ins_patch_init(void);
void btdrv_data_patch_init(void);
void btdrv_patch_en(uint8_t en);
void btdrv_config_init(void);
void btdrv_testmode_config_init(void);
void btdrv_bt_spi_rawbuf_init(void);
void btdrv_bt_spi_xtal_init(void);
void btdrv_sync_config(void);
void btdrv_rf_rx_gain_adjust_req(uint32_t user, bool lowgain);
#ifdef BT_50_FUNCTION
void btdrv_config_init_ble5(void);
void btdrv_ins_patch_init_50(void);
void btdrv_data_patch_init_50(void);
#endif
void btdrv_trace_config(BT_CONTROLER_TRACE_TYPE trace_config);
void btdrv_dump_mem(uint8_t *dump_mem_start, uint32_t dump_length, uint8_t dump_type);
void ld_util_bch_create(uint8_t *lap, uint8_t *bch);
void bt_drv_ble_sup_timeout_set(uint16_t ble_conhdl, uint16_t sup_to);
void btdrv_ecc_config(void);
#ifdef __cplusplus
}
#endif

#endif

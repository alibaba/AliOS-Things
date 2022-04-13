/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __HAL_CMD_H__
#define __HAL_CMD_H__

#include "stdint.h"

// typedef enum {
//         HAL_CMD_ERR_NONE = 0x00,
//         HAL_CMD_ERR_LEN = 0x01,
//         HAL_CMD_ERR_CHECKSUM = 0x02,
//         HAL_CMD_ERR_NOT_SYNC = 0x03,

//         HAL_CMD_ERR_CMD = 0x06,

//         HAL_CMD_ERR_BURN_OK = 0x60,
//         HAL_CMD_ERR_SECTOR_SIZE = 0x61,

//         HAL_CMD_ERR_BURN_INFO_MISSING = 0x63,
//         HAL_CMD_ERR_SECTOR_DATA_LEN = 0x64,
//         HAL_CMD_ERR_SECTOR_DATA_CRC = 0x65,
//         HAL_CMD_ERR_SECTOR_SEQ = 0x66,
//         HAL_CMD_ERR_ERASE_FLSH = 0x67,
//         HAL_CMD_ERR_BURN_FLSH = 0x68,
// } HAL_CMD_ERR_T;

// typedef enum {
//         HAL_CMD_GET_PARA = 0x00,
//         HAL_CMD_SET_PARA,
//         HAL_CMD_SAVE_PARA,
//         HAL_CMD_PUSH_PARA,

//         HAL_CMD_PLAYBACK_SWITCH = 0x08,
//         HAL_CMD_CAPTURE_SWITCH,

//         HAL_CMD_BURN_HANDSHAKE = 0x10,
//         HAL_CMD_BURN_START,
//         HAL_CMD_BURN_DATA,

//         HAL_CMD_HANDSHAKE = 0x18,
//         HAL_CMD_SHUTDOWN,
//         HAL_CMD_REBOOT,
//         HAL_CMD_NOTIFICATION,

//         HAL_CMD_INVALID = 0xff	
// } EL_CMD_E;

// typedef enum {
//         HAL_CMD_PARA_SOUND_VOLUME,
//         HAL_CMD_PARA_LC_SWITCH,
//         HAL_CMD_PARA_EQ_GAIN,
//         HAL_CMD_PARA_ANC_SWITCH,
//         HAL_CMD_PARA_ANC_GAIN,

//         HAL_CMD_PARA_INVALID
// } EL_PARA_E;

typedef enum {
    HAL_CMD_RX_START,
    HAL_CMD_RX_STOP,
    HAL_CMD_RX_DONE
} hal_cmd_rx_status_t;

typedef int (*hal_cmd_callback_t)(uint8_t *buf, uint32_t  len);
typedef void (*CMD_CALLBACK_HANDLER_T)(hal_cmd_rx_status_t status);

#ifdef __cplusplus
extern "C" {
#endif
int hal_cmd_init (void);
int hal_cmd_open (void);
int hal_cmd_close (void);

#ifdef USB_AUDIO_APP
void hal_cmd_run (hal_cmd_rx_status_t status);
#else
int hal_cmd_run (hal_cmd_rx_status_t status);
#endif
void hal_cmd_set_callback(CMD_CALLBACK_HANDLER_T handler);
int hal_cmd_register(char *name, hal_cmd_callback_t callback);

#ifdef USB_EQ_TUNING
int hal_cmd_list_process(uint8_t *buf);
void hal_cmd_tx_process (uint8_t** ppbuf, uint16_t* plen);
#endif

#ifdef AOS_COMP_IRQ
typedef enum {
    USPACE_metal_id,
    USPACE_xtal_flag,
    USPACE_temperature_25,
    USPACE_crystal_freq,
    USPACE_hw_bm,
    USPACE_pll_user_map,
    USPACE_sysfreq_bundle,
    USPACE_sys_tick_hz,
    USPACE_uart0_volt,
    USPACE_uart1_volt,
    USPACE_uart2_volt,
    USPACE_analogif_inited,
    USPACE_bbpll_dig_div,
    USPACE_vcore_high_volt,
    USPACE_top_user,
    USPACE_pmu_vcore_req,
    USPACE_pmu_dcdc_dig_comp,
    USPACE_pmu_dcdc_ana_comp,
    USPACE_clk_sel,
    USPACE_spi_cs_map,
    USPACE_spi_init_done,

    USPACE_PARAM_MAX,
} aos_vendor_param_type_t;

typedef struct {
    unsigned char *addr;
    int len;
} aos_vendor_param_t, *p_aos_vendor_param_t;

int aos_vendor_param_add(aos_vendor_param_type_t type, unsigned char *addr, int len);
int32_t _aos_get_sys_mem_info(int type, uintptr_t addr, int len);
int32_t aos_get_sys_mem_info(int type, uintptr_t addr, int len);
#endif

#ifdef __cplusplus
}
#endif
// hal_cmd_t *hal_cmd_get_ptr(void);

#endif
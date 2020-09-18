/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __BT_DRV_INTERFACE_H__
#define  __BT_DRV_INTERFACE_H__

#include "stdint.h"
#include "stdbool.h"

//LMP Opcodes
#define LMP_NAME_REQ_OPCODE                1
#define LMP_NAME_RES_OPCODE                2
#define LMP_ACCEPTED_OPCODE                3
#define LMP_NOT_ACCEPTED_OPCODE            4
#define LMP_CLK_OFF_REQ_OPCODE             5
#define LMP_CLK_OFF_RES_OPCODE             6
#define LMP_DETACH_OPCODE                  7
#define LMP_INRAND_OPCODE                  8
#define LMP_COMBKEY_OPCODE                 9
#define LMP_UNITKEY_OPCODE                10
#define LMP_AURAND_OPCODE                 11
#define LMP_SRES_OPCODE                   12
#define LMP_TEMPRAND_OPCODE               13
#define LMP_TEMPKEY_OPCODE                14
#define LMP_ENC_MODE_REQ_OPCODE           15
#define LMP_ENC_KEY_SIZE_REQ_OPCODE       16
#define LMP_START_ENC_REQ_OPCODE          17
#define LMP_STOP_ENC_REQ_OPCODE           18
#define LMP_SWITCH_REQ_OPCODE             19
#define LMP_HOLD_OPCODE                   20
#define LMP_HOLD_REQ_OPCODE               21
#define LMP_SNIFF_REQ_OPCODE              23
#define LMP_UNSNIFF_REQ_OPCODE            24
#define LMP_PARK_REQ_OPCODE               25
#define LMP_SET_BSWIN_OPCODE              27
#define LMP_MODIF_BEACON_OPCODE           28
#define LMP_UNPARK_BD_REQ_OPCODE          29
#define LMP_UNPARK_PM_REQ_OPCODE          30
#define LMP_INCR_PWR_REQ_OPCODE           31
#define LMP_DECR_PWR_REQ_OPCODE           32
#define LMP_MAX_PWR_OPCODE                33
#define LMP_MIN_PWR_OPCODE                34
#define LMP_AUTO_RATE_OPCODE              35
#define LMP_PREF_RATE_OPCODE              36
#define LMP_VER_REQ_OPCODE                37
#define LMP_VER_RES_OPCODE                38
#define LMP_FEATS_REQ_OPCODE              39
#define LMP_FEATS_RES_OPCODE              40
#define LMP_QOS_OPCODE                    41
#define LMP_QOS_REQ_OPCODE                42
#define LMP_SCO_LINK_REQ_OPCODE           43
#define LMP_RMV_SCO_LINK_REQ_OPCODE       44
#define LMP_MAX_SLOT_OPCODE               45
#define LMP_MAX_SLOT_REQ_OPCODE           46
#define LMP_TIMING_ACCU_REQ_OPCODE        47
#define LMP_TIMING_ACCU_RES_OPCODE        48
#define LMP_SETUP_CMP_OPCODE              49
#define LMP_USE_SEMI_PERM_KEY_OPCODE      50
#define LMP_HOST_CON_REQ_OPCODE           51
#define LMP_SLOT_OFF_OPCODE               52
#define LMP_PAGE_MODE_REQ_OPCODE          53
#define LMP_PAGE_SCAN_MODE_REQ_OPCODE     54
#define LMP_SUPV_TO_OPCODE                55
#define LMP_TEST_ACTIVATE_OPCODE          56
#define LMP_TEST_CTRL_OPCODE              57
#define LMP_ENC_KEY_SIZE_MASK_REQ_OPCODE  58
#define LMP_ENC_KEY_SIZE_MASK_RES_OPCODE  59
#define LMP_SET_AFH_OPCODE                60
#define LMP_ENCAPS_HDR_OPCODE             61
#define LMP_ENCAPS_PAYL_OPCODE            62
#define LMP_SP_CFM_OPCODE                 63
#define LMP_SP_NB_OPCODE                  64
#define LMP_DHKEY_CHK_OPCODE              65
#define LMP_PAUSE_ENC_AES_REQ_OPCODE      66

// CARE: round-off error
#define BT_CLK_UNIT 312.5       //us
#define BT_CLK_UNIT_2X 625       //us
#define BT_CLK_UNIT_10X 3125       //us
#define US_TO_BTCLKS(us)    ((uint64_t)(us) * 2 / BT_CLK_UNIT_2X)
#define BTCLKS_TO_US(n)     ((uint64_t)(n) * BT_CLK_UNIT_2X / 2)

#define DUT_CONNECT_STATUS_CONNECTED (1)
#define DUT_CONNECT_STATUS_DISCONNECTED (2)

#define BT_TRIG_NONE_ROLE      (0)
#define BT_TRIG_MASTER_ROLE    (1)
#define BT_TRIG_SLAVE_ROLE     (2)

#define btdrv_set_intersys_trace_enable()  beshci_set_intersys_trace_enable()
#define btdrv_set_spuv_hci_buff_trace_enable()  beshci_set_supv_hci_buff_trace_enable()
#define btdrv_set_a2dp_stream_trace_disable()  beshci_set_a2dp_stream_trace_enable(false)
#define btdrv_enable_sleep_checker(isEnable)   beshci_enable_sleep_checker(isEnable)

void btdrv_hciopen(void);
void btdrv_hcioff(void);
void bt_drv_extra_config_after_init(void);
void btdrv_rf_init_ext(void);

void btdrv_testmode_start(void);
void btdrv_hci_reset(void);
void btdrv_enable_nonsig_tx(uint8_t index);
void btdrv_enable_nonsig_rx(uint8_t index);

void bt_drv_calib_open(void);
void bt_drv_check_calib(void);
void bt_drv_calib_close(void);
int bt_drv_calib_result_porc(uint32_t *capval);
void bt_drv_calib_rxonly_porc(void);
void btdrv_write_localinfo(char *name, uint8_t len, uint8_t *addr);
void btdrv_disable_scan(void);

#ifdef __cplusplus
extern "C" {
#endif
void btdrv_start_bt(void);
void btdrv_stop_bt(void);
void btdrv_enable_dut(void);
void bt_drv_rf_reset(void);
void btdrv_sleep_config(uint8_t sleep_en);
void btdrv_feature_default(void);
void btdrv_test_mode_addr_set(void);

int btdrv_meinit_param_init(void);
int btdrv_meinit_param_remain_size_get(void);
int btdrv_meinit_param_next_entry_get(uint32_t *addr, uint32_t *val);

void btdrv_store_device_role(bool slave);
bool btdrv_device_role_is_slave(void);

uint32_t btdrv_rf_get_max_xtal_tune_ppb(void);
uint32_t btdrv_rf_get_xtal_tune_factor(void);
void btdrv_rf_init_xtal_fcap(uint32_t fcap);
uint32_t btdrv_rf_get_init_xtal_fcap(void);
uint32_t btdrv_rf_get_xtal_fcap(void);
void btdrv_rf_set_xtal_fcap(uint32_t fcap, uint8_t is_direct);
int btdrv_rf_xtal_fcap_busy(uint8_t is_direct);

void btdrv_rf_bit_offset_track_enable(bool enable);
void btdrv_rf_bit_offset_track_force_disable(void);
uint32_t btdrv_rf_bit_offset_get(void);
void btdrv_uart_bridge_loop(void);
void btdrv_testmode_data_overide(void);

void btdrv_set_bt_pcm_triggler_delay(uint8_t delay);
void btdrv_set_bt_pcm_en(uint8_t  en);

uint16_t btdrv_rf_bitoffset_get(uint8_t conidx);

void  btdrv_syn_clr_trigger(void);

uint32_t btdrv_rf_bit_offset_get(void);
uint32_t btdrv_syn_get_curr_ticks(void);
uint32_t bt_syn_get_curr_ticks(uint16_t conhdl);
void bt_syn_set_tg_ticks(uint32_t val,uint16_t conhdl, uint8_t mode);
void bt_syn_trig_checker(uint16_t conhdl);
void btdrv_syn_trigger_codec_en(uint32_t v);
uint32_t btdrv_get_syn_trigger_codec_en(void);
uint32_t btdrv_get_trigger_ticks(void);


void btdrv_rf_trig_patch_enable(bool enable);
void btdrv_tws_trig_role(uint8_t role);
void btdrv_rf_set_conidx(uint32_t conidx);



#define ACL_TRIGGLE_MODE       1
#define SCO_TRIGGLE_MODE      2
void btdrv_enable_playback_triggler(uint8_t triggle_mode);
void btdrv_set_bt_pcm_triggler_en(uint8_t  en);
void btdrv_set_bt_pcm_triggler_delay(uint8_t  delay);
void btdrv_set_bt_pcm_triggler_delay_reset(uint8_t  delay);
void btdrv_set_pcm_data_ignore_crc(void);


uint8_t btdrv_conhdl_to_linkid(uint16_t connect_hdl);
void btdrv_set_tws_role_triggler(uint8_t tws_mode);
void btdrv_enable_dual_slave_configurable_slot_mode(bool isEnable,
        uint16_t activeDevHandle, uint8_t activeDevRole,
        uint16_t idleDevHandle, uint8_t idleDevRole);
void btdrv_ins_patch_test_init(void);
void btdrv_dynamic_patch_moble_disconnect_reason_hacker(uint16_t hciHandle);
void btdrv_dynamic_patch_sco_status_clear(void);
uint32_t btdrv_dut_get_connect_status(void);
void btdrv_set_powerctrl_rssi_low(uint16_t rssi);
void btdrv_seq_bak_mode(uint8_t mode,uint8_t linkid);

void btdrv_enable_one_packet_more_head(bool enable);

#if defined(CHIP_BEST2300) || defined(CHIP_BEST1400) || defined(CHIP_BEST1402) || defined(CHIP_BEST2300P) || defined(CHIP_HAAS1000)
void btdrv_linear_format_16bit_set(void);
void btdrv_pcm_enable(void);
void btdrv_pcm_disable(void);
void btdrv_spi_trig_data_change(uint8_t spi_sel, uint8_t index, uint32_t value);
void btdrv_spi_trig_data_set(uint8_t spi_sel);
uint8_t btdrv_get_spi_trig_enable(uint8_t spi_sel);
void btdrv_set_spi_trig_enable(uint8_t spi_sel);
void btdrv_clear_spi_trig_enable(uint8_t spi_sel);
void btdrv_spi_trig_init(void);
void btdrv_spi_trig_select(uint8_t link_id, bool spi_set);
void bt_drv_reg_cs_monitor(void);
void btdrv_set_controller_trace_enable(uint8_t trace_level);
uint8_t btdrv_get_controller_trace_level(void);
bool btdrv_get_lmp_trace_enable(void);
void btdrv_set_lmp_trace_enable(void);

#endif
void bt_drv_select_hwspi1(uint8_t link_id);
bool btdrv_get_controller_trace_dump_enable(void);
void btdrv_set_controller_trace_dump_enable(void);
void beshci_enable_sleep_checker(bool isEnable);
void beshci_set_intersys_trace_enable(void);
void beshci_set_supv_hci_buff_trace_enable(void);
void beshci_set_a2dp_stream_trace_enable(bool enable);
bool btdrv_rf_rx_gain_adjust_getstatus(void);
bool btdrv_rf_blerx_gain_adjust_getstatus(void);
void btdrv_rf_blerx_gain_adjust_default(void);
void btdrv_rf_blerx_gain_adjust_lowgain(void);
void btdrv_send_cmd(uint16_t opcode,uint8_t cmdlen,const uint8_t *param);
void btdrv_write_memory(uint8_t wr_type,uint32_t address,const uint8_t *value,uint8_t length);
void bt_drv_patch_force_disconnect_ack(void);
void bt_dev_music_link_config(uint16_t active_link,uint8_t active_role,uint16_t inactive_link,uint8_t inactive_role);

void btdrv_memory_copy(uint32_t *dest,const uint32_t *src,uint16_t length);
#if defined(TX_RX_PCM_MASK)
uint8_t  btdrv_is_pcm_mask_enable(void);
#endif
void btdrv_set_lpo_times(void);

#if defined(PCM_FAST_MODE)
void btdrv_open_pcm_fast_mode_enable(void);
void btdrv_open_pcm_fast_mode_disable(void);
#endif

#if defined(CVSD_BYPASS)
void btdrv_cvsd_bypass_enable(void);
#endif
void btdrv_write_private_public_key(uint8_t* private_key,uint8_t* public_key);
void btdrv_hwagc_mode_set(uint8_t sync_mode);


#define BTCLK_STATUS_PRINT()    do{ \
    TRACE("[%s] %d: curr bt time= %d", __func__, __LINE__, bt_syn_get_curr_ticks(app_tws_get_tws_conhdl())); \
                                }while(0)

#define IS_ENABLE_BT_DRIVER_REG_DEBUG_READING   0

#define BT_DRIVER_GET_U8_REG_VAL(regAddr)       (*(uint8_t *)(regAddr))
#define BT_DRIVER_GET_U16_REG_VAL(regAddr)      (*(uint16_t *)(regAddr))
#define BT_DRIVER_GET_U32_REG_VAL(regAddr)      (*(uint32_t *)(regAddr))

#define BT_DRIVER_PUT_U8_REG_VAL(regAddr, val)      *(uint8_t *)(regAddr) = (val)
#define BT_DRIVER_PUT_U16_REG_VAL(regAddr, val)     *(uint16_t *)(regAddr) = (val)
#define BT_DRIVER_PUT_U32_REG_VAL(regAddr, val)     *(uint32_t *)(regAddr) = (val)

void bt_drv_acl_tx_silence(uint16_t connHandle, uint8_t on);

int btdrv_slave2master_clkcnt_convert(uint32_t local_clk, uint16_t local_cnt,
                                               int32_t clk_offset, uint16_t bit_offset,
                                               uint32_t *master_clk, uint16_t *master_cnt);

int btdrv_clkcnt_diff(int32_t clk1, int16_t cnt1,
                         int32_t clk2, int16_t cnt2,
                         int32_t *diff_clk, uint16_t *diff_bit);
bool bt_drv_set_ble_conn_latency(uint16_t ble_conhdl, uint16_t conn_latency);
uint16_t bt_drv_get_ble_conn_latency(uint16_t ble_conhdl);
void bt_drv_set_reconnecting_flag();
uint32_t bt_drv_get_reconnecting_flag();
void bt_drv_clear_reconnecting_flag();
bool bt_drv_is_enhanced_ibrt_rom(void);
bool bt_drv_is_bes_fa_mode_en(void);
uint16_t bt_drv_get_ibrt_sco_hdl(uint16_t acl_hdl);
void bt_drv_get_ibrt_address(uint8_t *addr);

void btdrv_enable_rf_sw(int rx_on, int tx_on);
void btdrv_trigger_coredump(void);
bool btdrv_is_ecc_enable(void);
void bt_drv_adaptive_fa_rx_gain(int8_t rssi);
void* bt_drv_get_btstack_chip_config(void);
void btdrv_spi_sel_ble_setf(int elt_idx, uint8_t spiselble);

#ifdef __SW_TRIG__
void btdrv_play_trig_mode_set(uint8_t mode);
void btdrv_sw_trig_master_set(uint32_t Tclk_M, int16_t Tbit_M_h_ori);
void btdrv_sw_trig_slave_calculate_and_set(uint16_t conhdl, uint32_t Tclk_M, int16_t Tbit_M_h_ori);
#endif

void btdrv_set_bdr_ble_txpower(uint8_t div, uint8_t power_level);
void btdrv_set_edr_txpower(uint8_t div, uint8_t power_level);
void btdrv_vco_test_start(uint8_t chnl);
void btdrv_vco_test_stop(void);
void btdrv_switch_to_i2c(void);
void write_reg32(uint32_t reg32, uint32_t value);
void read_reg32(uint32_t reg32);
void nosignal_set_max_power_level(void);
void nosignal_clear_max_power_level(void);

void write_reg16(uint8_t reg16, uint16_t value);
void read_reg16(uint8_t reg16);

void wifi_write_reg16(uint8_t reg16, uint16_t value);
void wifi_read_reg16(uint8_t reg16);
void bt_drv_set_wlan_onoff(uint8_t en);
void bt_drv_set_ble_adv_transq_on(uint8_t en);
void bt_drv_set_ble_conn_transq_on(uint8_t en);
void bt_drv_set_sniff_transq_on(uint8_t en);
void bt_drv_set_rsw_transq_on(uint8_t en);
void bt_drv_set_pscan_transq_on(uint8_t en);

#ifdef __cplusplus
}
#endif

#endif


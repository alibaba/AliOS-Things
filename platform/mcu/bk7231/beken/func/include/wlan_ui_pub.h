#ifndef _WLAN_UI_PUB_
#define _WLAN_UI_PUB_

#pragma once

#include "include.h"
#include "rtos_pub.h"
#include "rw_pub.h"
#include <hal/wifi.h>

#define ICU_BASE                                     (0x00802000)
#define ICU_INT_STATUS                               (ICU_BASE + 19 * 4)

enum {
    WLAN_ENC_OPEN,
    WLAN_ENC_WEP,
    WLAN_ENC_CCMP,
    WLAN_ENC_TKIP,
};

enum
{
    WLAN_RX_BEACON,    /* receive beacon packet */
    WLAN_RX_PROBE_REQ, /* receive probe request packet */
    WLAN_RX_PROBE_RES, /* receive probe response packet */
    WLAN_RX_ACTION,    /* receive action packet */
    WLAN_RX_MANAGEMENT,/* receive ALL management packet */
    WLAN_RX_DATA,      /* receive ALL data packet */
    WLAN_RX_MCAST_DATA,/* receive ALL multicast and broadcast packet */

    WLAN_RX_ALL,       /* receive ALL 802.11 packet */
};

OSStatus bk_wlan_start(hal_wifi_init_type_t* inNetworkInitPara);

OSStatus bk_wlan_start_sta_adv(hal_wifi_init_type_adv_t* inNetworkInitParaAdv);

OSStatus bk_wlan_get_ip_status(hal_wifi_ip_stat_t *outNetpara, hal_wifi_type_t inInterface);

OSStatus bk_wlan_get_link_status(hal_wifi_link_stat_t *outStatus);

void bk_wlan_start_scan(uint8_t adv);

int bk_wlan_monitor_rx_type(int type);

int bk_wlan_start_monitor(void);

int bk_wlan_stop_monitor(void);

int bk_wlan_set_channel_sync(int channel);

int bk_wlan_set_channel(int channel);

void bk_wlan_register_monitor_cb(monitor_data_cb_t fn);

void bk_wlan_enable_lsig(void);

void bk_wlan_disable_lsig(void);

monitor_data_cb_t bk_wlan_get_monitor_cb(void);

int bk_wlan_is_monitor_mode(void);

uint32_t bk_sta_cipher_is_open(void);

uint32_t bk_sta_cipher_is_wep(void);

int bk_sta_cipher_type(void);

int bk_wlan_stop(char mode);

OSStatus bk_wlan_start_sta(hal_wifi_init_type_t *inNetworkInitPara);

OSStatus bk_wlan_start_ap(hal_wifi_init_type_t *inNetworkInitPara);

OSStatus bk_wlan_set_ip_status(hal_wifi_ip_stat_t *inNetpara, hal_wifi_type_t inInterface);

void bk_wlan_ap_set_channel_config(uint8_t channel);

uint8_t bk_wlan_ap_get_channel_config(void);

uint32_t bk_wlan_ap_get_frequency(void);

uint32_t bk_wlan_get_INT_status(void);

extern int is_apm_bss_config_empty(void);

void bk_enter_deep_sleep(UINT32 gpio_index);

extern int bk_wlan_dtim_rf_ps_mode_enable(void );

int power_save_dtim_rf_ps_disable_send_msg(void);

int bk_wlan_dtim_rf_ps_mode_disable(void);

extern int bk_wlan_dtim_with_normal_open(void);

extern int bk_wlan_dtim_with_normal_close(void);

extern int bk_wlan_mcu_ps_mode_disable(void);

extern int bk_wlan_dtim_rf_ps_mode_do_wakeup();

extern int bk_wlan_mcu_suppress_and_sleep(UINT32);

extern int bk_wlan_mcu_ps_mode_enable(void);

extern int bk_wlan_mcu_ps_mode_disable(void);


enum
{
    RF_SLEEP        = 0,
    MCU_SLEEP,
    LOW_SLEEP,
    DEEP_PS,
} ;
#define     PS_RF_SLEEP_BIT         CO_BIT(RF_SLEEP) 
#define     PS_MCU_SLEEP_BIT        CO_BIT(MCU_SLEEP) 
#define     PS_DEEP_SLEEP_BIT       CO_BIT(DEEP_PS) 
typedef     UINT32      BK_PS_LEVEL;


void bk_wlan_connection_loss(void);

void bk_wlan_start_assign_scan(UINT8 **ssid_ary, UINT8 ssid_num);

void bk_wlan_scan_ap_reg_cb(FUNC_2PARAM_PTR ind_cb);

unsigned char bk_wlan_get_scan_ap_result_numbers(void);

void bk_wlan_get_scan_ap_result(SCAN_RST_ITEM_PTR scan_rst_table,unsigned char get_scanu_num);

#ifdef CONFIG_AOS_MESH
void wlan_register_mesh_monitor_cb(monitor_data_cb_t fn);
monitor_data_cb_t wlan_get_mesh_monitor_cb(void);
int wlan_is_mesh_monitor_mode(void);
int wlan_set_mesh_bssid(uint8_t *bssid);
uint8_t *wlan_get_mesh_bssid(void);
#endif

#endif// _WLAN_UI_PUB_


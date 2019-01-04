#ifndef _ARCH_CONFIG_H_
#define _ARCH_CONFIG_H_

#include "mac.h"

#define PARAM_CFG_DEBUG

#ifdef PARAM_CFG_DEBUG
#define PARAM_CFG_PRT      os_printf
#define PARAM_CFG_WARN     warning_prf
#define PARAM_CFG_FATAL    fatal_prf
#else
#define PARAM_CFG_PRT      null_prf
#define PARAM_CFG_WARN     null_prf
#define PARAM_CFG_FATAL    null_prf
#endif

#define CONFIG_ROLE_NULL        0
#define CONFIG_ROLE_AP          1
#define CONFIG_ROLE_STA         2
#define CONFIG_ROLE_COEXIST     3

#define DEFAULT_CHANNEL_AP   11

typedef struct fast_connect_param
{
    uint8_t bssid[6];
    uint8_t chann;
} fast_connect_param_t;

typedef struct general_param
{
    uint8_t role;
    uint8_t dhcp_enable;
    uint32_t ip_addr;
    uint32_t ip_mask;
    uint32_t ip_gw;
} general_param_t;

typedef struct ap_param
{
    struct mac_addr bssid;
    struct mac_ssid ssid;
    uint8_t chann;
    uint8_t cipher_suite;
    uint8_t key[65];
    uint8_t key_len;
} ap_param_t;

typedef struct sta_param
{
    struct mac_addr own_mac;
    struct mac_ssid ssid;
    uint8_t cipher_suite;
    uint8_t key[65];
    uint8_t key_len;
    uint8_t fast_connect_set;
    fast_connect_param_t fast_connect;
} sta_param_t;

extern general_param_t *g_wlan_general_param;
extern ap_param_t *g_ap_param_ptr;
extern sta_param_t *g_sta_param_ptr;
extern uint8_t system_mac[6];

void cfg_load_mac(u8 *mac);
uint32_t cfg_param_init(void);

void wifi_get_mac_address(char *mac, u8 type);
int wifi_set_mac_address(char *mac);
int wifi_set_mac_address_to_efuse(UINT8 *mac);
int wifi_get_mac_address_from_efuse(UINT8 *mac);

int wifi_write_efuse(UINT8 addr, UINT8 data);
UINT8 wifi_read_efuse(UINT8 addr);

#endif

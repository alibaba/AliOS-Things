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

#define CONFIG_ADDR_START   0xF0000

#define CONFIG_ROLE_NULL    0
#define CONFIG_ROLE_AP      1
#define CONFIG_ROLE_STA     2

#define CONFIG_CIPHER_OPEN   0
#define CONFIG_CIPHER_WEP    1
#define CONFIG_CIPHER_TKIP   2
#define CONFIG_CIPHER_CCMP   3
#define CONFIG_CIPHER_MIXED  4
#define CONFIG_CIPHER_AUTO   5

enum
{
    PARAM_CFG_GENERAL,
    PARAM_CFG_AP,
    PARAM_CFG_STA,
    PARAM_CFG_MISC,
};

enum
{
    GENERAL_TYPE_ROLE,
    GENERAL_TYPE_DHCP,
    GENERAL_TYPE_IP,
    GENERAL_TYPE_MASK,
    GENERAL_TYPE_GW,
};

enum
{
    AP_TYPE_BSSID,
    AP_TYPE_SSID,
    AP_TYPE_CHANNEL,
    AP_TYPE_MODE,
    AP_TYPE_PASSWD,
};

enum
{
    STA_TYPE_MAC,
    STA_TYPE_SSID,
    STA_TYPE_MODE,
    STA_TYPE_PASSWD,
};

enum
{
    MISC_TYPE_UART,
    MISC_TYPE_GPIO,
};

typedef struct head_param
{
    uint32_t type;
    uint32_t len;
} head_param_t;

typedef struct fast_connect_param
{
	uint8_t bssid[6];
	uint8_t chann;
}fast_connect_param_t;

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
    uint8_t security;
    uint8_t key[65];
	uint8_t key_len;
	uint8_t fast_connect_set;
	uint8_t psk[65];
	uint8_t psk_set;
	fast_connect_param_t fast_connect;
} sta_param_t;

extern general_param_t *g_wlan_general_param;
extern ap_param_t *g_ap_param_ptr;
extern sta_param_t *g_sta_param_ptr;

int cfg_get_general_params(void);
int cfg_get_ap_params(void);
int cfg_get_sta_params(void);
uint32_t cfg_param_init(void);

void wifi_get_mac_address(char *mac);
int wifi_set_mac_address(char *mac);

#endif

#ifndef _RDA59XX_DAEMON_H
#define _RDA59XX_DAEMON_H

#include "rda59xx_wifi_include.h"

//encrypt type
#define ENCRYPT_NONE        (0)
#define ENCRYPT_WPA_TKIP    BIT0
#define ENCRYPT_WPA_CCMP    BIT1
#define ENCRYPT_WPA2_TKIP   BIT2
#define ENCRYPT_WPA2_CCMP   BIT3
#define ENCRYPT_WEP         BIT4

typedef enum {
    DAEMON_SCAN,
    DAEMON_STA_CONNECT,
    DAEMON_STA_DISCONNECT,
    DAEMON_STA_RECONNECT,
    DAEMON_AP_ENABLE,
    DAEMON_AP_DISABLE,
    DAEMON_SNIFFER_ENABLE,
    DAEMON_SNIFFER_DISABLE,
}DAEMON_MSG;

typedef enum {
    RDA59XX_WLAND_STA_CONNECT = 0,
    RDA59XX_WLAND_STA_DISCONNECT = 1,
    RDA59XX_WLAND_AP_START = 10,
    RDA59XX_WLAND_AP_STOP = 11,
}RDA59XX_WLAND_MSG;

typedef enum {
    ERR_DEVICE = -1,
    ERR_SEND_MSG = -2,
    ERR_AUTH = -3,
    ERR_CONNECTION = -4,
    ERR_DHCP = -5,
}ERR_TYPE;

typedef enum 
{
    EVENT_STA_CONNECTTED              = 0x00,
    EVENT_STA_CONNECT_FAIL,
    EVENT_STA_DISCONNECTTED,
    EVENT_STA_GOT_IP,
    EVENT_AP_STARTED                  = 0x10,
    EVENT_AP_START_FAIL,
    EVENT_AP_STOPED,
}WIFI_EVENT;

typedef void (*wifi_event_cb_t)(WIFI_EVENT evt, void* info);

#define STATE_DSIABLE       (0)
#define STATE_INIT          BIT0
#define STATE_SNIFFER       BIT1
#define STATE_STA           BIT2
#define STATE_STA_RC        BIT3
#define STATE_AP            BIT3
#define STATE_P2P           BIT4

extern r_s32 rda59xx_wifi_init();
extern r_s32 rda59xx_wifi_set_event_cb(wifi_event_cb_t cb);
extern r_s32 rda59xx_sta_connect(rda59xx_sta_info *sta_info);
extern r_s32 rda59xx_sta_disconnect();
extern r_s32 rda59xx_sta_get_ip(r_u32 ip_addr);
extern r_s32 rda59xx_ap_enable(rda59xx_ap_info *ap_info);
extern r_s32 rda59xx_ap_disable();
extern r_s32 rda59xx_scan(rda59xx_scan_info *scan_info);
extern r_s32 rda59xx_sniffer_enable(sniffer_handler_t handler);
extern r_s32 rda59xx_sniffer_disable();
extern r_u32 rda59xx_get_module_state();
extern r_void rda59xx_sta_get_bss_info(rda59xx_bss_info *bss_info);
#endif/* _RDA59XX_DAEMON_H */


#ifndef __ANT_WIFI_H__
#define __ANT_WIFI_H__
#include "ant_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ANT_MAX_SSID_CNT    20
#define ANT_MAX_SSID_LEN    80
#define ANT_MAX_PWD_LEN     80
#define ANT_WIFI_MAC_LEN    6

#define ANT_TCP_APN_CMNET		(1)
#define ANT_TCP_APN_CMWAP		(2)
#define ANT_TCP_APN_WIFI		(3)
#define ANT_TCP_APN_CMNET_ONLY  (4)
#define ANT_TCP_APN_CMWAP_ONLY  (5)

#define ANT_WIFI_SCAN_TIMES     3

typedef enum
{
    ANT_WLAN_RESULT_SUCCESS    = 0,    /* success */
    ANT_WLAN_RESULT_FAILED,            /* general failed case */
    ANT_WLAN_RESULT_PROCESSING,        /* processing state implies Event notify */
    ANT_WLAN_RESULT_STATE_ERROR,       /* service status error */
    ANT_WLAN_RESULT_INVALID_INPUT,     /* the input parameter is invalid */
    ANT_WLAN_RESULT_ABORTED,           /* the process is aborted */
    ANT_WLAN_RESULT_TERMINATED,        /* the process is terminated */
    ANT_WLAN_RESULT_ONE_STORE_EXISTS,  /* currently only support one store */
    ANT_WLAN_RESULT_STORE_NOT_COMMIT,  /* store doesn't be config commited before */
    ANT_WLAN_RESULT_INVALID_STORE,     /* store invalid */
    ANT_WLAN_RESULT_STORE_FULL,        /* reach maximum profile storage */
    ANT_WLAN_RESULT_READ_ONLY,         /* can't delete or update read_only profile */
    ANT_WLAN_RESULT_FIRST_BOOTUP,      /* first bootup rerurn code for store open */
    ANT_WLAN_RESULT_RESET_FACTORY,     /* the data base sim profiles folder is deleted */
    ANT_WLAN_RESULT_FS_ERROR,          /* the database file system error */
    ANT_WLAN_RESULT_IGNORE,
    ANT_WLAN_RESULT_TOTAL
} ant_wlan_result_enum;

typedef enum
{
    ANT_WLAN_STATE_NULL = 0,
    ANT_WLAN_STATE_DEINIT_WAIT,
    ANT_WLAN_STATE_INIT_WAIT,
    ANT_WLAN_STATE_INIT,
    ANT_WLAN_STATE_DISCONNECT_WAIT,
    ANT_WLAN_STATE_CONNECT_WAIT,
    ANT_WLAN_STATE_CONNECTED,
    ANT_WLAN_STATE_TOTAL
} ant_wlan_state_enum;

typedef struct
{
    ant_u8             app_id;
    ant_u32            app_account_id;    // 应用(用户)账号
    ant_u32            data_account_id;   // 数据(网络)账号
} ant_wifi_stat_t;

typedef struct{
    ant_s8 wifi_ssid[ANT_MAX_SSID_LEN];
    ant_s8 wifi_pwd[ANT_MAX_PWD_LEN];
}ant_wifi_info;

extern ant_s32 ant_wlan_init();
extern ant_s32 ant_wlan_deinit();
extern ant_s32 ant_wifi_init();
extern void ant_wifi_triger_scan();
extern ant_wlan_state_enum ant_get_wifi_state();
extern ant_u32 ant_wifi_get_accountid(void);
extern ant_s8 * ant_wifi_get_ssid();
ant_u8 * ant_wifi_get_mac();
void ant_wifi_set_mac();
void ant_wifi_info_set(ant_wifi_info * wifi_info);
void ant_wifi_info_get(ant_wifi_info * wifi_info);
ant_s32 ant_wifi_factory_test();
#ifdef __cplusplus
}
#endif

#endif

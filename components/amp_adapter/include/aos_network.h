/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AMP_NETWORK_H_
#define _AMP_NETWORK_H_
#define SCANNED_WIFI_COUNT_MAX 32
#define SCANNED_LOCATOR_COUNT_MAX 32

typedef enum {
	AMP_NETWORK_WIFI,
	AMP_NETWORK_CELLULAR,
	AMP_NETWORK_ETHERNET,
	AMP_NETWORK_UNKNOW,
}AMP_NETWORK_TYPE_E;

typedef enum {
	AMP_NETWORK_IPTYPE_IPV4,
	AMP_NETWORK_IPTYPE_IPV6,
	AMP_NETWORK_IPTYPE_IPV4V6,
    AMP_NETWORK_IPTYPE_INVALID
}AMP_NETWORK_IPTYPE_E;

typedef enum {
	AMP_NETWORK_SHAREMODE_RNDIS,
	AMP_NETWORK_SHAREMODE_ECM,
	AMP_NETWORK_SHAREMODE_INVALID
}AMP_NETWORK_SHAREMODE_E;

typedef enum {
	AMP_NETWORK_SHAREMODE_AUTHTYPE_NONE,
	AMP_NETWORK_SHAREMODE_AUTHTYPE_PAP,
	AMP_NETWORK_SHAREMODE_AUTHTYPE_CHAP,
    AMP_NETWORK_SHAREMODE_AUTHTYPE_PAPCHAP,
    AMP_NETWORK_SHAREMODE_AUTHTYPE_INVALID
}AMP_NETWORK_SHAREMODE_AUTHTYPE_E;

typedef struct amp_sim_info {
    char imsi[32];
    char imei[32];
    char iccid[32];
} amp_sim_info_t;

typedef struct amp_locator_info {
    char mcc[4];
    char mnc[4];
    int cellid;
    int lac;
    int signal;
} amp_locator_info_t;

typedef struct amp_signal_info {
    int rssi;
} amp_signal_info_t;

typedef struct amp_scanned_locator_info {
    int num;
    amp_locator_info_t locator_info[SCANNED_LOCATOR_COUNT_MAX];
} amp_scanned_locator_info_t;

typedef struct amp_wifi_info {
    char ssid[128];
    char mac[6];
    char ip[4];
    int rssi;
} amp_wifi_info_t;

typedef struct scanned_wifi_info {
    int num;
    amp_wifi_info_t wifi_info[SCANNED_WIFI_COUNT_MAX];
} scanned_wifi_info_t;

typedef struct amp_sharemode_info {
    int action;
    int auto_connect;
    char apn[99];
    char username[64];
    char password[64];
    AMP_NETWORK_IPTYPE_E ip_type;
    AMP_NETWORK_SHAREMODE_E share_mode;
} amp_sharemode_info_t;

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int amp_wifi_init();

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int amp_wifi_connect(const char *ssid, const char *passwd);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int amp_wifi_disconnect();

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int amp_get_wifi_info(amp_wifi_info_t *wifi_info);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int amp_get_sim_info(amp_sim_info_t *sim_info);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int amp_get_locator_info(amp_locator_info_t *locator_info);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int amp_get_neighbor_locator_info(void (*cb)(amp_locator_info_t*, int));

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_get_network_status(void);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
AMP_NETWORK_TYPE_E amp_get_network_type();

/* ECM */
/**
 * @brief       get net share mode
 *
 * @return      mode: 0-RNDIS  1-ECM
 */
AMP_NETWORK_SHAREMODE_E amp_get_netsharemode(void);

/**
 * @brief       set net share mode
 *
 * @param[out]  share_mode:  net mode
 *
 * @return      0: success, -1: failed
 */
int amp_set_netsharemode(AMP_NETWORK_SHAREMODE_E share_mode);

/**
 * @brief       net share action
 *
 * @param[out]  action_p: net share mode: 0-close 1-open
 *
 * @return      0: success, -1: failed
 */
int amp_get_netshareconfig(amp_sharemode_info_t *share_mode_info);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int amp_set_netshareconfig(int ucid, int auth_type, amp_sharemode_info_t *share_mode_info);

int amp_location_access_wifi_info(amp_wifi_info_t *wifi_info);

int amp_location_scaned_wifi_info(scanned_wifi_info_t *scaned_wifi);

#endif /* _AMP_NETWORK_H_ */


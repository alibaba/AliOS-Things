/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AOS_NETWORK_H_
#define _AOS_NETWORK_H_
#define SCANNED_WIFI_COUNT_MAX 32
#define SCANNED_LOCATOR_COUNT_MAX 32

typedef enum {
    AOS_NETWORK_WIFI,
    AOS_NETWORK_CELLULAR,
    AOS_NETWORK_ETHERNET,
    AOS_NETWORK_UNKNOW,
} AOS_NETWORK_TYPE_E;

typedef enum {
    AOS_NETWORK_IPTYPE_IPV4,
    AOS_NETWORK_IPTYPE_IPV6,
    AOS_NETWORK_IPTYPE_IPV4V6,
    AOS_NETWORK_IPTYPE_INVALID
} AOS_NETWORK_IPTYPE_E;

typedef enum {
    AOS_NETWORK_SHAREMODE_RNDIS,
    AOS_NETWORK_SHAREMODE_ECM,
    AOS_NETWORK_SHAREMODE_INVALID
} AOS_NETWORK_SHAREMODE_E;

typedef enum {
    AOS_NETWORK_SHAREMODE_AUTHTYPE_NONE,
    AOS_NETWORK_SHAREMODE_AUTHTYPE_PAP,
    AOS_NETWORK_SHAREMODE_AUTHTYPE_CHAP,
    AOS_NETWORK_SHAREMODE_AUTHTYPE_PAPCHAP,
    AOS_NETWORK_SHAREMODE_AUTHTYPE_INVALID
} AOS_NETWORK_SHAREMODE_AUTHTYPE_E;

typedef struct aos_sim_info {
    char imsi[32];
    char imei[32];
    char iccid[32];
} aos_sim_info_t;

typedef struct aos_locator_info {
    char mcc[4];
    char mnc[4];
    int cellid;
    int lac;
    int signal;
} aos_locator_info_t;

typedef struct aos_scanned_locator_info {
    int num;
    aos_locator_info_t locator_info[SCANNED_LOCATOR_COUNT_MAX];
} aos_scanned_locator_info_t;

typedef struct aos_wifi_info {
    char ssid[128];
    char mac[6];
    char ip[16];
    int rssi;
} aos_wifi_info_t;

typedef struct aos_scanned_wifi_info {
    int num;
    aos_wifi_info_t wifi_info[SCANNED_WIFI_COUNT_MAX];
} aos_scanned_wifi_info_t;

typedef struct aos_sharemode_info {
    int action;
    int auto_connect;
    char apn[99];
    char username[64];
    char password[64];
    AOS_NETWORK_IPTYPE_E ip_type;
    AOS_NETWORK_SHAREMODE_E share_mode;
} aos_sharemode_info_t;

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_wifi_init();

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_wifi_connect(const char *ssid, const char *passwd);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_wifi_disconnect();

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_get_wifi_info(aos_wifi_info_t *wifi_info);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_get_sim_info(aos_sim_info_t *sim_info);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_get_locator_info(aos_locator_info_t *locator_info);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_get_neighbor_locator_info(void (*cb)(aos_locator_info_t*, int));

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
AOS_NETWORK_TYPE_E aos_get_network_type();

/* ECM */
/**
 * @brief       get net share mode
 *
 * @return      mode: 0-RNDIS  1-ECM
 */
AOS_NETWORK_SHAREMODE_E aos_get_netsharemode(void);

/**
 * @brief       set net share mode
 *
 * @param[out]  share_mode:  net mode
 *
 * @return      0: success, -1: failed
 */
int aos_set_netsharemode(AOS_NETWORK_SHAREMODE_E share_mode);

/**
 * @brief       net share action
 *
 * @param[out]  action_p: net share mode: 0-close 1-open
 *
 * @return      0: success, -1: failed
 */
int aos_get_netshareconfig(aos_sharemode_info_t *share_mode_info);

/**
 * @brief       file close
 *
 * @param[out]  ip: ip pointer
 *
 * @return      0: success, -1: failed
 */
int aos_set_netshareconfig(int ucid, int auth_type,
                           aos_sharemode_info_t *share_mode_info);

int aos_location_access_wifi_info(aos_wifi_info_t *wifi_info);

int aos_location_scaned_wifi_info(aos_scanned_wifi_info_t *scaned_wifi);

#endif /* _AOS_NETWORK_H_ */


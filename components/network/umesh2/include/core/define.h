
#ifndef UMESH_DEFINE_H_
#define UMESH_DEFINE_H_

#include <stdint.h>

#define UMESH_TASK_STACK_SIZE               8192
#define IEEE80211_VENDOR_SPECIFIC           127
#define IEEE80211_MAC_ADDR_LEN              6
#define IEEE80211_MAC_HEADER_LEN            24
#define UMESH_MTU_MAX_LEN                   1024
#define UMESH_RANDOM_LEN                    16
#define UMESH_AES_KEY_LEN                   16
#define UMESH_RSSI_THRESHOLD_DEFAULT        -65
#define UMESH_RSSI_GRACE_DEFAULT            -5
#define UMESH_POLL_TIMEOUOT                 (1 * 1000) /* in ms */
#define UMESH_SCAN_PEER_DURATION            (100)  /* in ms */
#define UMESH_IDENTIFY_REQUEST_DURATION     (500)  /* in ms */
#define UMESH_SCAN_BASE_CNT                  2
#define UMESH_HEART_DURATION                 (10 * 1000)  /* in ms */

#define UMESH_HEART_TIMEOUT_CNT             3
#define UMESH_PEERS_DEFAULT_CLEAN_INTERVAL  (500 * 1000) /* in ms */
#define UMESH_SCAN_MAIN_CHANNEL             6
#define UMESH_SCAN_MAIN_CHANNEL_PRIO        10
#define UMESH_MASTER_SCAN_MAX_CNT           13
#define UMESH_IDENTIFY_MAX_CNT              20
#define UMESH_FCS_LEN                       4

#define UMESH_MAX_SSID_LEN                (32 + 1)    /* ssid: 32 octets at most, include the NULL-terminated */
#define UMESH_MAX_PASSWD_LEN              (64 + 1)    /* password: 8-63 ascii */


#define BIT0                                0x01
#define BIT1                                0x02
#define BIT2                                0x04
#define BIT3                                0x08
#define BIT4                                0x10
#define BIT5                                0x20
#define BIT6                                0x40
#define BIT7                                0x80


/* User Input: 0x0100 ~ 0x01FF */
#define UMESH_ERR_BASE                       (-0x1000)

/*common err */
/* User input parameters contain unacceptable NULL pointer */
/* 用户传递给API的参数中含有不合理的空指针参数 */

#define UMESH_OK                             (0)
#define UMESH_ERR_NULL_POINTER               (UMESH_ERR_BASE - 0x0001)
#define UMESH_ERR_MALLOC_FAILED              (UMESH_ERR_BASE - 0x0002)
#define UMESH_ERR_OUT_OF_BOUNDS              (UMESH_ERR_BASE - 0x0003)
#define UMESH_NO_ACTION_REQUIRED             (UMESH_ERR_BASE - 0x0004)
#define UMESH_ERR_NOT_INIT                   (UMESH_ERR_BASE - 0x0004)

/*peer err */
#define UMESH_ERR_PEER_MISSING               (UMESH_ERR_BASE - 0x0101)
#define UMESH_PEER_ADD_FAILED                (UMESH_ERR_BASE - 0x0102)

#define UMESH_RX_CRC_FAILED                  (UMESH_ERR_BASE - 0x0102)
#define UMESH_RX_IGNORE_FROM_SELF            (UMESH_ERR_BASE - 0x0103)
#define UMESH_RX_IGNORE_TO_OTHER             (UMESH_ERR_BASE - 0x0104)
#define UMESH_RX_IGNORE_NOT_UMESH            (UMESH_ERR_BASE - 0x0105)
#define UMESH_RX_UNEXPECTED_TYPE             (UMESH_ERR_BASE - 0x0106)
#define UMESH_RX_TOO_SHORT                   (UMESH_ERR_BASE - 0x0107)
#define UMESH_PACK_FAILED                    (UMESH_ERR_BASE - 0x0108)
#define UMESH_PARSE_TLV_FAILED               (UMESH_ERR_BASE - 0x0109)
/*hal err */
#define UMESH_WIFI_RAW_SEND_FAILED           (UMESH_ERR_BASE - 0x0201)
#define UMESH_WIFI_SET_CHAN_FAILED           (UMESH_ERR_BASE - 0x0202)
#define UMESH_WIFI_NO_CHAN_LIST              (UMESH_ERR_BASE - 0x0203)
#define UMESH_WIFI_GET_MAC_FAILED            (UMESH_ERR_BASE - 0x0204)
#define UMESH_WIFI_GET_AP_INFO_FAILED        (UMESH_ERR_BASE - 0x0205)

#define UMESH_ERR_AES_INIT                   (UMESH_ERR_BASE - 0x0301)
#define UMESH_ERR_AES_DECRYPT                (UMESH_ERR_BASE - 0x0302)
#define UMESH_ERR_AES_ENCRYPT                (UMESH_ERR_BASE - 0x0303)

#define UMESH_ERR_IPSTACK_INIT               (UMESH_ERR_BASE - 0x0401)
#define UMESH_ERR_IP_ADDR                    (UMESH_ERR_BASE - 0x0402)
#endif /* UMESH_DEFINE_H_ */

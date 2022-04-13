#ifndef __UMESH_H__
#define __UMESH_H__
#include <string.h>
#include <stdint.h>

#define UMESH_RSSI_THRESHOLD                (-80)
/* User Input: 0x0100 ~ 0x01FF */
#define UMESH_ERR_BASE                       (-0x1000)

/*common err */
#define UMESH_OK                             (0)
#define UMESH_ERR_NULL_POINTER               (UMESH_ERR_BASE - 0x0001)
#define UMESH_ERR_MALLOC_FAILED              (UMESH_ERR_BASE - 0x0002)
#define UMESH_ERR_OUT_OF_BOUNDS              (UMESH_ERR_BASE - 0x0003)
#define UMESH_NO_ACTION_REQUIRED             (UMESH_ERR_BASE - 0x0004)
#define UMESH_ERR_NOT_INIT                   (UMESH_ERR_BASE - 0x0005)
#define UMESH_ERR_TIMER                      (UMESH_ERR_BASE - 0x0006)
/*peer err */
#define UMESH_ERR_PEER_MISSING               (UMESH_ERR_BASE - 0x0101)
#define UMESH_PEER_ADD_FAILED                (UMESH_ERR_BASE - 0x0102)
#define UMESH_RX_CRC_FAILED                  (UMESH_ERR_BASE - 0x0103)
#define UMESH_RX_IGNORE_FROM_SELF            (UMESH_ERR_BASE - 0x0104)
#define UMESH_RX_IGNORE_TO_OTHER             (UMESH_ERR_BASE - 0x0105)
#define UMESH_RX_IGNORE_NOT_UMESH            (UMESH_ERR_BASE - 0x0106)
#define UMESH_RX_UNEXPECTED_TYPE             (UMESH_ERR_BASE - 0x0107)
#define UMESH_RX_TOO_SHORT                   (UMESH_ERR_BASE - 0x0108)
#define UMESH_RX_DUPLICATED                  (UMESH_ERR_BASE - 0x0109)
#define UMESH_PACK_FAILED                    (UMESH_ERR_BASE - 0x010a)
#define UMESH_PARSE_TLV_FAILED               (UMESH_ERR_BASE - 0x010b)
#define UMESH_LIST_FULL                      (UMESH_ERR_BASE - 0x010c)
#define UMESH_PEER_ADD_INPROGRESS            (UMESH_ERR_BASE - 0x010d)
#define UMESH_ERR_CHANNEL                    (UMESH_ERR_BASE - 0x010e)
#define UMESH_ERR_SESSION_NO_MATCH           (UMESH_ERR_BASE - 0x010f)
#define UMESH_RX_RSSI_IGNORE                 (UMESH_ERR_BASE - 0x0110)
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


typedef enum {
    WIFI_CONFIG_FAILED = -1,
    WIFI_CONFIG_FINISH = 0
} umesh_zero_config_state_t;

typedef void (*umesh_zero_info_cb)(const char *ssid, const char *pwd, const uint8_t *bssid, const uint8_t *ext_data,
                                   uint8_t ext_len, void *context);
typedef void (*umesh_zero_finish_cb)(const uint8_t *dest_mac, umesh_zero_config_state_t result, void *context);

void *umesh_network_init();
int umesh_is_connected(void *hanlde);
int umesh_network_deinit(void *handle);

int umesh_zero_master_start_config(void *handle, const char *ssid, const char *pwd, uint8_t *bssid, uint8_t *ext,
                                   uint8_t ext_len,
                                   umesh_zero_finish_cb cb, void *context);
int umesh_zero_slave_register_receiver(void *hanlde, umesh_zero_info_cb cb, void *context);
#endif /* __UMESH_H__ */


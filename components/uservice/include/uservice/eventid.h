/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_EVENT_ID_H
#define YOC_EVENT_ID_H

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif


/* net driver event
*  from 0x100 to 0x1FF
*  netdrv.h
*/

/* Netmgr */
#define EVENT_NETMGR_BASE                       0x01000
#define EVENT_NETMGR_WIFI_DISCONNECTED          (EVENT_NETMGR_BASE + 1)  // Connection disconected
#define EVENT_NETMGR_WIFI_SCAN_STARTED          (EVENT_NETMGR_BASE + 2)  // Scan start
#define EVENT_NETMGR_WIFI_SCAN_FAILED           (EVENT_NETMGR_BASE + 3)  // Scan failed
#define EVENT_NETMGR_WIFI_SCAN_DONE             (EVENT_NETMGR_BASE + 4)  // Scan failed
#define EVENT_NETMGR_WIFI_NETWORK_NOT_FOUND     (EVENT_NETMGR_BASE + 5)  // no AP found
#define EVENT_NETMGR_WIFI_AUTHENTICATING        (EVENT_NETMGR_BASE + 6)  // Authentication start
#define EVENT_NETMGR_WIFI_AUTH_REJECT           (EVENT_NETMGR_BASE + 7)  // Authentication rejected by AP
#define EVENT_NETMGR_WIFI_AUTH_TIMEOUT          (EVENT_NETMGR_BASE + 8)  // Authentication timeout with AP
#define EVENT_NETMGR_WIFI_ASSOCIATING           (EVENT_NETMGR_BASE + 9)  // Association starts
#define EVENT_NETMGR_WIFI_ASSOC_REJECT          (EVENT_NETMGR_BASE + 10) // Association rejected by AP
#define EVENT_NETMGR_WIFI_ASSOC_TIMEOUT         (EVENT_NETMGR_BASE + 11) // Association timeout with AP
#define EVENT_NETMGR_WIFI_ASSOCIATED            (EVENT_NETMGR_BASE + 12) // Authentication succeed
#define EVENT_NETMGR_WIFI_4WAY_HANDSHAKE        (EVENT_NETMGR_BASE + 13) // 4way-handshark start
#define EVENT_NETMGR_WIFI_HANDSHAKE_FAILED      (EVENT_NETMGR_BASE + 14) // 4way-handshake fails
#define EVENT_NETMGR_WIFI_4WAY_HANDSHAKE_DONE   (EVENT_NETMGR_BASE + 15) // 4way-handshark done
#define EVENT_NETMGR_WIFI_GROUP_HANDSHAKE       (EVENT_NETMGR_BASE + 16) // group-handshark start
#define EVENT_NETMGR_WIFI_GROUP_HANDSHAKE_DONE  (EVENT_NETMGR_BASE + 17) // group-handshark done = completed
#define EVENT_NETMGR_WIFI_CONNECTED             (EVENT_NETMGR_BASE + 18) // Connection to AP done
#define EVENT_NETMGR_WIFI_CONN_TIMEOUT          (EVENT_NETMGR_BASE + 19) // Connection timeout
#define EVENT_NETMGR_WIFI_DEAUTH                (EVENT_NETMGR_BASE + 20) // Deauth received from AP
#define EVENT_NETMGR_WIFI_MAX                   (EVENT_NETMGR_WIFI_DEAUTH)

#define EVENT_NETMGR_DHCP_BASE                  (EVENT_NETMGR_WIFI_MAX)
#define EVENT_NETMGR_DHCP_START_FAILED          (EVENT_NETMGR_DHCP_BASE + 1)  // DHCP start fails
#define EVENT_NETMGR_DHCP_TIMEOUT               (EVENT_NETMGR_DHCP_BASE + 2)  // DHCP timeout
#define EVENT_NETMGR_DHCP_SUCCESS               (EVENT_NETMGR_DHCP_BASE + 3)  // DHCP success
#define EVENT_NETMGR_DHCP_MAX                   (EVENT_NETMGR_DHCP_SUCCESS)

#define EVENT_NETMGR_SNTP_BASE                  (EVENT_NETMGR_DHCP_MAX)
#define EVENT_NETMGR_SNTP_SUCCESS               (EVENT_NETMGR_SNTP_BASE + 1)  // SNTP success
#define EVENT_NETMGR_SNTP_FAILED                (EVENT_NETMGR_SNTP_BASE + 2 ) // SNTP failure
#define EVENT_NETMGR_SNTP_MAX                   (EVENT_NETMGR_SNTP_FAILED)

#define EVENT_NETMGR_CONN_BASE                  (EVENT_NETMGR_SNTP_MAX)
#define EVENT_NETMGR_CONN_RECONNECT             (EVENT_NETMGR_CONN_BASE + 1)  // Reconnect AP
#define EVENT_NETMGR_CONN_MAX                   (EVENT_NETMGR_CONN_RECONNECT)

#define EVENT_NETMGR_GOT_IP                     (EVENT_NETMGR_DHCP_SUCCESS)

#define EVENT_NETMGR_NET_BASE                   (EVENT_NETMGR_CONN_MAX)
#define EVENT_NETMGR_NET_DISCON                 (EVENT_NETMGR_WIFI_DISCONNECTED)
#define EVENT_NETMGR_NET_CONFIG                 (EVENT_NETMGR_NET_BASE + 1)
#define EVENT_NETMGR_MAX                        (EVENT_NETMGR_NET_CONFIG)

/* NET */
#define EVENT_NET_GOT_IP                        0x100

/* Wi-Fi */
#define EVENT_WIFI_BASE                         (EVENT_NETMGR_MAX)
#define EVENT_WIFI_DISCONNECTED                 (EVENT_WIFI_BASE + 1)  // Connection disconected
#define EVENT_WIFI_SCAN_STARTED                 (EVENT_WIFI_BASE + 2)  // Scan start
#define EVENT_WIFI_SCAN_FAILED                  (EVENT_WIFI_BASE + 3)  // Scan failed
#define EVENT_WIFI_SCAN_DONE                    (EVENT_WIFI_BASE + 4)  // Scan failed
#define EVENT_WIFI_NETWORK_NOT_FOUND            (EVENT_WIFI_BASE + 5)  // no AP found
#define EVENT_WIFI_AUTHENTICATING               (EVENT_WIFI_BASE + 6)  // Authentication start
#define EVENT_WIFI_AUTH_REJECT                  (EVENT_WIFI_BASE + 7)  // Authentication rejected by AP
#define EVENT_WIFI_AUTH_TIMEOUT                 (EVENT_WIFI_BASE + 8)  // Authentication timeout with AP
#define EVENT_WIFI_ASSOCIATING                  (EVENT_WIFI_BASE + 9)  // Association starts
#define EVENT_WIFI_ASSOC_REJECT                 (EVENT_WIFI_BASE + 10) // Association rejected by AP
#define EVENT_WIFI_ASSOC_TIMEOUT                (EVENT_WIFI_BASE + 11) // Association timeout with AP
#define EVENT_WIFI_ASSOCIATED                   (EVENT_WIFI_BASE + 12) // Authentication succeed
#define EVENT_WIFI_4WAY_HANDSHAKE               (EVENT_WIFI_BASE + 13) // 4way-handshark start
#define EVENT_WIFI_HANDSHAKE_FAILED             (EVENT_WIFI_BASE + 14) // 4way-handshake fails
#define EVENT_WIFI_4WAY_HANDSHAKE_DONE          (EVENT_WIFI_BASE + 15) // 4way-handshark done
#define EVENT_WIFI_GROUP_HANDSHAKE              (EVENT_WIFI_BASE + 16) // group-handshark start
#define EVENT_WIFI_GROUP_HANDSHAKE_DONE         (EVENT_WIFI_BASE + 17) // group-handshark done = completed
#define EVENT_WIFI_CONNECTED                    (EVENT_WIFI_BASE + 18) // Connection to AP done
#define EVENT_WIFI_CONN_TIMEOUT                 (EVENT_WIFI_BASE + 19) // Connection timeout
#define EVENT_WIFI_DEAUTH                       (EVENT_WIFI_BASE + 20) // Deauth received from AP
#define EVENT_WIFI_MAX                          (EVENT_WIFI_DEAUTH)

#define EVENT_DHCP_BASE                         (EVENT_WIFI_MAX)
#define EVENT_DHCP_START_FAILED                 (EVENT_DHCP_BASE + 1)  // DHCP start fails
#define EVENT_DHCP_TIMEOUT                      (EVENT_DHCP_BASE + 2)  // DHCP timeout
#define EVENT_DHCP_SUCCESS                      (EVENT_DHCP_BASE + 3)  // DHCP success
#define EVENT_DHCP_MAX                          (EVENT_DHCP_SUCCESS)

#define EVENT_SNTP_BASE                         (EVENT_DHCP_MAX)
#define EVENT_SNTP_SUCCESS                      (EVENT_SNTP_BASE + 1)  // SNTP success
#define EVENT_SNTP_FAILED                       (EVENT_SNTP_BASE + 2)  // SNTP failure
#define EVENT_SNTP_MAX                          (EVENT_SNTP_FAILED)

#define EVENT_CONN_BASE                         (EVENT_SNTP_MAX)
#define EVENT_CONN_RECONNECT                    (EVENT_CONN_BASE + 1)  // Reconnect AP
#define EVENT_CONN_MAX                          (EVENT_CONN_RECONNECT)

/* IOT */
#define EVENT_IOT_BASE                          (EVENT_CONN_MAX)
#define EVENT_IOT_CONNECT_SUCCESS               (EVENT_IOT_BASE + 1)
#define EVENT_IOT_CONNECT_FAILED                (EVENT_IOT_BASE + 2)
#define EVENT_IOT_DISCONNECTED                  (EVENT_IOT_BASE + 3)
#define EVENT_IOT_PUSH_SUCCESS                  (EVENT_IOT_BASE + 4)
#define EVENT_IOT_PUSH_FAILED                   (EVENT_IOT_BASE + 5)
#define EVENT_IOT_MAX                           (EVENT_IOT_PUSH_FAILED)

/* OS */
#define EVENT_OS_BASE                           (EVENT_IOT_MAX)
#define EVENT_OS_REBOOT                         (EVENT_OS_BASE + 1)
#define EVENT_CLOCK_ALARM                       (EVENT_OS_BASE + 2)
#define EVENT_CLOCK_TIMEOUT                     (EVENT_OS_BASE + 3)
#define EVENT_OS_MAX                            (EVENT_CLOCK_TIMEOUT)

/* FOTA */
#define EVENT_FOTA_BASE                         (EVENT_OS_MAX)
#define EVENT_FOTA_START                        (EVENT_FOTA_BASE + 1)
#define EVENT_FOTA_UPDATE                       (EVENT_FOTA_BASE + 2)
#define EVENT_FOTA_MAX                          (EVENT_FOTA_UPDATE)

/* BLE */
#define EVENT_BLE_BASE                          (EVENT_FOTA_MAX)
#define EVENT_BLE                               (EVENT_BLE_BASE + 1)
#define EVENT_BLE_MESH                          (EVENT_BLE_BASE + 2)
#define EVENT_BLE_MAX                           (EVENT_BLE_MESH)

/* USER */
#define EVENT_USER_BASE                         (EVENT_BLE_MAX)
#define EVENT_USER                              (EVENT_BLE_BASE + 1)
#define EVENT_USER_MAX                          (EVENT_USER)

#ifdef __cplusplus
}
#endif

#endif

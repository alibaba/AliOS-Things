/**
 * \file            lwesp_typedefs.h
 * \brief           List of structures and enumerations for public usage
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#ifndef LWESP_HDR_DEFS_H
#define LWESP_HDR_DEFS_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lwesp/lwesp_opt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         LWESP
 * \defgroup        LWESP_TYPEDEFS Structures and enumerations
 * \brief           List of core structures and enumerations
 * \{
 */

/**
 * \}
 */

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           Result enumeration used across application functions
 */
typedef enum {
    lwespOK = 0,                                /*!< Function succeeded */
    lwespOKIGNOREMORE,                          /*!< Function succedded, should continue as lwespOK but ignore sending more data. This result is possible on connection data receive callback */
    lwespERR,                                   /*!< General error */
    lwespPARERR,                                /*!< Wrong parameters on function call */
    lwespERRMEM,                                /*!< Memory error occurred */
    lwespTIMEOUT,                               /*!< Timeout occurred on command */
    lwespCONT,                                  /*!< There is still some command to be processed in current command */
    lwespCLOSED,                                /*!< Connection just closed */
    lwespINPROG,                                /*!< Operation is in progress */

    lwespERRNOIP,                               /*!< Station does not have IP address */
    lwespERRNOFREECONN,                         /*!< There is no free connection available to start */
    lwespERRCONNTIMEOUT,                        /*!< Timeout received when connection to access point */
    lwespERRPASS,                               /*!< Invalid password for access point */
    lwespERRNOAP,                               /*!< No access point found with specific SSID and MAC address */
    lwespERRCONNFAIL,                           /*!< Connection failed to access point */
    lwespERRWIFINOTCONNECTED,                   /*!< Wifi not connected to access point */
    lwespERRNODEVICE,                           /*!< Device is not present */
    lwespERRBLOCKING,                           /*!< Blocking mode command is not allowed */
    lwespERRCMDNOTSUPPORTED,                    /*!< Command is not supported error received by device */
    lwespEAGAIN,                    /*!< Command is not supported error received by device */
} lwespr_t;

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           List of support ESP devices by firmware
 */
typedef enum {
    LWESP_DEVICE_UNKNOWN = 0x00,                /*!< Device is unknown by default */
    LWESP_DEVICE_ESP8266,                       /*!< Device is ESP8266 */
    LWESP_DEVICE_ESP32,                         /*!< Device is ESP32 */
    LWESP_DEVICE_ESP32_C3,                      /*!< Device is ESP32-C3 */
    LWESP_DEVICE_END,                           /*!< End of the list */
} lwesp_device_t;

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           List of encryptions of access point
 */
typedef enum {
    LWESP_ECN_OPEN = 0x00,                      /*!< No encryption on access point */
    LWESP_ECN_WEP,                              /*!< WEP (Wired Equivalent Privacy) encryption */
    LWESP_ECN_WPA_PSK,                          /*!< WPA (Wifi Protected Access) encryption */
    LWESP_ECN_WPA2_PSK,                         /*!< WPA2 (Wifi Protected Access 2) encryption */
    LWESP_ECN_WPA_WPA2_PSK,                     /*!< WPA/2 (Wifi Protected Access 1/2) encryption */
    LWESP_ECN_WPA2_Enterprise,                  /*!< Enterprise encryption. \note ESP8266 is not able to connect to such device */
    LWESP_ECN_WPA3_PSK,                         /*!< WPA3 (Wifi Protected Access 3) encryption */
    LWESP_ECN_WPA2_WPA3_PSK,                    /*!< WPA2/3 (Wifi Protected Access 2/3) encryption, ESP32-C3 only mode */
    LWESP_ECN_END,
} lwesp_ecn_t;

/**
 * \brief           Access point cipher modes, used for access point scan data
 */
typedef enum {
    LWESP_AP_CIPHER_NONE = 0x00,                /*!< None value */
    LWESP_AP_CIPHER_WEP40,                      /*!< WEP40 mode */
    LWESP_AP_CIPHER_WEP104,                     /*!< WEB104 */
    LWESP_AP_CIPHER_TKIP,                       /*!< TKIP */
    LWESP_AP_CIPHER_CCMP,                       /*!< CCMP */
    LWESP_AP_CIPHER_TKIP_CCMP,                  /*!< TKIP and CCMP */
    LWESP_AP_CIPHER_AES_CMAC_128,               /*!< AES-CMAC-128 */
    LWESP_AP_CIPHER_UNKNOWN,                    /*!< Unknown value */
    LWESP_AP_CIPHER_END,
} lwesp_ap_cipher_t;

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           IP type
 */
typedef enum {
    LWESP_IPTYPE_V4 = 0x00,                     /*!< IP type is V4 */
    LWESP_IPTYPE_V6                             /*!< IP type is V6 */
} lwesp_iptype_t;

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           IPv4 address structure
 */
typedef struct {
    uint8_t addr[4];                            /*!< IP address data */
} lwesp_ip4_addr_t;

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           IPv6 address structure
 */
typedef struct {
    uint16_t addr[8];                           /*!< IP address data */
} lwesp_ip6_addr_t;

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           IP structure
 */
typedef struct {
    union {
        lwesp_ip4_addr_t ip4;                   /*!< IPv4 address */
#if LWESP_CFG_IPV6 || __DOXGEN__
        lwesp_ip6_addr_t ip6;                   /*!< IPv6 address */
#endif /* LWESP_CFG_IPV6  || __DOXGEN__ */
    } addr;                                     /*!< Actual IP address */
    lwesp_iptype_t type;                        /*!< IP type, either V4 or V6 */
} lwesp_ip_t;

/**
 * \ingroup         LWESP_UTILITIES
 * \brief           Set IPv4 address to \ref lwesp_ip_t variable
 * \param[in]       ip_str: Pointer to IP structure
 * \param[in]       i1,i2,i3,i4: IPv4 parts
 */
#define lwesp_ip_set_ip4(ip_str, i1, i2, i3, i4)   do {        \
        (ip_str)->type = LWESP_IPTYPE_V4;                       \
        (ip_str)->addr.ip4.addr[0] = (i1);                      \
        (ip_str)->addr.ip4.addr[1] = (i2);                      \
        (ip_str)->addr.ip4.addr[2] = (i3);                      \
        (ip_str)->addr.ip4.addr[3] = (i4);                      \
    } while (0)

/**
 * \ingroup         LWESP_UTILITIES
 * \brief           Set IPv6 address to \ref lwesp_ip_t variable
 * \param[in]       ip_str: Pointer to IP structure
 * \param[in]       i1,i2,i3,i4,i5,i6,i7,i8: IPv6 parts
 */
#define lwesp_ip_set_ip6(ip_str, i1, i2, i3, i4, i5, i6, i7, i8)   do {\
        (ip_str)->type = LWESP_IPTYPE_V6;                       \
        (ip_str)->addr.ip6.addr[0] = (i1);                      \
        (ip_str)->addr.ip6.addr[1] = (i2);                      \
        (ip_str)->addr.ip6.addr[2] = (i3);                      \
        (ip_str)->addr.ip6.addr[3] = (i4);                      \
        (ip_str)->addr.ip6.addr[4] = (i5);                      \
        (ip_str)->addr.ip6.addr[5] = (i6);                      \
        (ip_str)->addr.ip6.addr[6] = (i7);                      \
        (ip_str)->addr.ip6.addr[7] = (i8);                      \
    } while (0)

/**
 * \brief           Check if input IP structure holds valid IP address
 * \param[in]       ip: IP to check for valid address. It can be V4 or V6
 * \return          `1` if IP valid, `0` otherwise
 */
static inline uint8_t
lwesp_ip_is_valid(const lwesp_ip_t* ip) {
    if (ip == NULL) {
        return 0;
    }

    /* Check address validity */
    if ((ip->type == LWESP_IPTYPE_V4 && ip->addr.ip4.addr[0] > 0)
#if LWESP_CFG_IPV6
        || (ip->type == LWESP_IPTYPE_V6 && ip->addr.ip6.addr[0] > 0)
#endif /* LWESP_CF_IPV6 */
       ) {
        return 1;
    }
    return 0;
}

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           Port variable
 */
typedef uint16_t    lwesp_port_t;

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           MAC address
 */
typedef struct {
    uint8_t mac[6];                             /*!< MAC address */
} lwesp_mac_t;

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           SW version in semantic versioning format
 */
typedef struct {
    uint8_t major;                              /*!< Major version */
    uint8_t minor;                              /*!< Minor version */
    uint8_t patch;                              /*!< Patch version */
} lwesp_sw_version_t;

/**
 * \ingroup         LWESP_AP
 * \brief           Access point data structure
 */
typedef struct {
    lwesp_ecn_t ecn;                            /*!< Encryption mode */
    char ssid[LWESP_CFG_MAX_SSID_LENGTH];       /*!< Access point name */
    int16_t rssi;                               /*!< Received signal strength indicator */
    lwesp_mac_t mac;                            /*!< MAC physical address */
    uint8_t ch;                                 /*!< WiFi channel used on access point */

#if LWESP_CFG_ACCESS_POINT_STRUCT_FULL_FIELDS || __DOXYGEN__
    uint8_t scan_type;                          /*!< Scan type, 0 = active, 1 = passive */
    uint16_t scan_time_min;                     /*!< Minimum active scan time per channel in units of milliseconds */
    uint16_t scan_time_max;                     /*!< maximum active scan time per channel in units of milliseconds */
    int16_t freq_offset;                        /*!< Frequency offset */
    int16_t freq_cal;                           /*!< Frequency calibration */
    lwesp_ap_cipher_t pairwise_cipher;          /*!< Pairwise cipher mode */
    lwesp_ap_cipher_t group_cipher;             /*!< Group cipher mode */
#endif /* LWESP_CFG_ACCESS_POINT_STRUCT_FULL_FIELDS || __DOXYGEN__ */
    uint8_t bgn;                                /*!< Information about 802.11[b|g|n] support */
    uint8_t wps;                                /*!< Status if WPS function is supported */
} lwesp_ap_t;

/**
 * \ingroup         LWESP_AP
 * \brief           Access point information on which station is connected to
 */
typedef struct {
    char ssid[LWESP_CFG_MAX_SSID_LENGTH];       /*!< Access point name */
    int16_t rssi;                               /*!< RSSI */
    lwesp_mac_t mac;                            /*!< MAC address */
    uint8_t ch;                                 /*!< Channel information */
} lwesp_sta_info_ap_t;

/**
 * \ingroup         LWESP_AP
 * \brief           Soft access point data structure
 */
typedef struct {
    char ssid[LWESP_CFG_MAX_SSID_LENGTH];       /*!< Access point name */
    char pwd[LWESP_CFG_MAX_PWD_LENGTH];         /*!< Access point password/passphrase */
    uint8_t ch;                                 /*!< WiFi channel used on access point */
    lwesp_ecn_t ecn;                            /*!< Encryption mode */
    uint8_t max_cons;                           /*!< Maximum number of stations allowed connected to this AP */
    uint8_t hidden;                             /*!< broadcast the SSID, 0 -- No, 1 -- Yes */
} lwesp_ap_conf_t;

/**
 * \ingroup         LWESP_STA
 * \brief           Station data structure
 */
typedef struct {
    lwesp_ip_t ip;                              /*!< IP address of connected station */
    lwesp_mac_t mac;                            /*!< MAC address of connected station */
} lwesp_sta_t;

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           Date and time structure
 */
typedef struct {
    uint8_t date;                               /*!< Day in a month, from 1 to up to 31 */
    uint8_t month;                              /*!< Month in a year, from 1 to 12 */
    uint16_t year;                              /*!< Year */
    uint8_t day;                                /*!< Day in a week, from 1 to 7 */
    uint8_t hours;                              /*!< Hours in a day, from 0 to 23 */
    uint8_t minutes;                            /*!< Minutes in a hour, from 0 to 59 */
    uint8_t seconds;                            /*!< Seconds in a minute, from 0 to 59 */
} lwesp_datetime_t;

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           List of possible WiFi modes
 */
typedef enum {
#if LWESP_CFG_MODE_STATION || __DOXYGEN__
    LWESP_MODE_STA = 1,                         /*!< Set WiFi mode to station only */
#endif /* LWESP_CFG_MODE_STATION || __DOXYGEN__ */
#if LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
    LWESP_MODE_AP = 2,                          /*!< Set WiFi mode to access point only */
#endif /* LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */
#if LWESP_CFG_MODE_STATION_ACCESS_POINT || __DOXYGEN__
    LWESP_MODE_STA_AP = 3,                      /*!< Set WiFi mode to station and access point */
#endif /* (LWESP_CFG_MODE_STATION_ACCESS_POINT) || __DOXYGEN__ */
} lwesp_mode_t;

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           List of possible HTTP methods
 */
typedef enum {
    LWESP_HTTP_METHOD_GET,                      /*!< HTTP method GET */
    LWESP_HTTP_METHOD_HEAD,                     /*!< HTTP method HEAD */
    LWESP_HTTP_METHOD_POST,                     /*!< HTTP method POST */
    LWESP_HTTP_METHOD_PUT,                      /*!< HTTP method PUT */
    LWESP_HTTP_METHOD_DELETE,                   /*!< HTTP method DELETE */
    LWESP_HTTP_METHOD_CONNECT,                  /*!< HTTP method CONNECT */
    LWESP_HTTP_METHOD_OPTIONS,                  /*!< HTTP method OPTIONS */
    LWESP_HTTP_METHOD_TRACE,                    /*!< HTTP method TRACE */
    LWESP_HTTP_METHOD_PATCH,                    /*!< HTTP method PATCH */
    LWESP_HTTP_METHOD_END,
} lwesp_http_method_t;

/**
 * \ingroup         LWESP_CONN
 * \brief           List of possible connection types
 */
typedef enum {
    LWESP_CONN_TYPE_TCP,                        /*!< Connection type is TCP */
    LWESP_CONN_TYPE_UDP,                        /*!< Connection type is UDP */
    LWESP_CONN_TYPE_SSL,                        /*!< Connection type is SSL */
#if LWESP_CFG_IPV6 || __DOXYGEN__
    LWESP_CONN_TYPE_TCPV6,                      /*!< Connection type is TCP over IPV6 */
    LWESP_CONN_TYPE_SSLV6,                      /*!< Connection type is SSL over IPV6 */
#endif /* LWESP_CFG_IPV6 || __DOXYGEN__ */
} lwesp_conn_type_t;

/* Forward declarations */
struct lwesp_evt;
struct lwesp_conn;
struct lwesp_pbuf;

/**
 * \ingroup         LWESP_CONN
 * \brief           Pointer to \ref lwesp_conn_t structure
 */
typedef struct lwesp_conn* lwesp_conn_p;

/**
 * \ingroup         LWESP_PBUF
 * \brief           Pointer to \ref lwesp_pbuf_t structure
 */
typedef struct lwesp_pbuf* lwesp_pbuf_p;

/**
 * \ingroup         LWESP_EVT
 * \brief           Event function prototype
 * \param[in]       evt: Callback event data
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
typedef lwespr_t  (*lwesp_evt_fn)(struct lwesp_evt* evt);

/**
 * \ingroup         LWESP_EVT
 * \brief           List of possible callback types received to user
 */
typedef enum lwesp_evt_type_t {
    LWESP_EVT_INIT_FINISH,                      /*!< Initialization has been finished at this point */

    LWESP_EVT_RESET_DETECTED,                   /*!< Device reset detected */
    LWESP_EVT_RESET,                            /*!< Device reset operation finished */
    LWESP_EVT_RESTORE,                          /*!< Device restore operation finished */

    LWESP_EVT_CMD_TIMEOUT,                      /*!< Timeout on command.
                                                        When application receives this event,
                                                        it may reset system as there was (maybe) a problem in device */

    LWESP_EVT_DEVICE_PRESENT,                   /*!< Notification when device present status changes */

    LWESP_EVT_AT_VERSION_NOT_SUPPORTED,         /*!< Library does not support firmware version on ESP device. */

    LWESP_EVT_CONN_RECV,                        /*!< Connection data received */
    LWESP_EVT_CONN_SEND,                        /*!< Connection data send */
    LWESP_EVT_CONN_ACTIVE,                      /*!< Connection just became active */
    LWESP_EVT_CONN_ERROR,                       /*!< Client connection start was not successful */
    LWESP_EVT_CONN_CLOSE,                       /*!< Connection close event. Check status if successful */
    LWESP_EVT_CONN_POLL,                        /*!< Poll for connection if there are any changes */

    LWESP_EVT_SERVER,                           /*!< Server status changed */

    LWESP_EVT_KEEP_ALIVE,                       /*!< Generic keep-alive event type, used as periodic timeout.
                                                    Optionally enabled with \ref LWESP_CFG_KEEP_ALIVE */

#if LWESP_CFG_MODE_STATION || __DOXYGEN__
    LWESP_EVT_WIFI_CONNECTED,                   /*!< Station just connected to access point.
                                                    When received, station may not have yet valid IP hence new connections
                                                    cannot be started in this mode */
    LWESP_EVT_WIFI_GOT_IP,                      /*!< Station has valid IP.
                                                    When this event is received to application, ESP has got IP from access point,
                                                    but no IP has been read from device and at this moment it is still being unknown to application.
                                                    Stack will proceed with IP read from device and will later send \ref LWESP_EVT_WIFI_IP_ACQUIRED event.

                                                    Note: When IPv6 is enabled, this event may be called multiple times during single connection to access point,
                                                    as device may report "got IP" several times.
                                                    Application must take care when starting new conection from this event, not to start it multiple times */
    LWESP_EVT_WIFI_DISCONNECTED,                /*!< Station just disconnected from access point */
    LWESP_EVT_WIFI_IP_ACQUIRED,                 /*!< Station IP address acquired.
                                                    At this point, valid IP address has been received from device.
                                                    Application may use \ref lwesp_sta_copy_ip function to read it */

    LWESP_EVT_STA_LIST_AP,                      /*!< Station listed APs event */
    LWESP_EVT_STA_JOIN_AP,                      /*!< Join to access point */
    LWESP_EVT_STA_INFO_AP,                      /*!< Station AP info (name, mac, channel, rssi) */
#endif /* LWESP_CFG_MODE_STATION || __DOXYGEN__ */
#if LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
    LWESP_EVT_AP_CONNECTED_STA,                 /*!< New station just connected to ESP's access point */
    LWESP_EVT_AP_DISCONNECTED_STA,              /*!< New station just disconnected from ESP's access point */
    LWESP_EVT_AP_IP_STA,                        /*!< New station just received IP from ESP's access point */
#endif /* LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */
#if LWESP_CFG_DNS || __DOXYGEN__
    LWESP_EVT_DNS_HOSTBYNAME,                   /*!< DNS domain service finished */
#endif /* LWESP_CFG_DNS || __DOXYGEN__ */
#if LWESP_CFG_PING || __DOXYGEN__
    LWESP_EVT_PING,                             /*!< PING service finished */
#endif /* LWESP_CFG_PING || __DOXYGEN__ */
#if LWESP_CFG_WEBSERVER || __DOXYGEN__
    LWESP_EVT_WEBSERVER,                        /*!< Web server events */
#endif /* LWESP_CFG_WEBSERVER || __DOXYGEN__ */
#if LWESP_CFG_SNTP || __DOXYGEN__
    LWESP_EVT_SNTP_TIME,                        /*!< SNTP event with date and time */
#endif /* LWESP_CFG_SNTP || __DOXYGEN__ */
    LWESP_CFG_END,
} lwesp_evt_type_t;

/**
 * \ingroup         LWESP_EVT
 * \brief           Global callback structure to pass as parameter to callback function
 */
typedef struct lwesp_evt {
    lwesp_evt_type_t type;                      /*!< Callback type */
    union {
        struct {
            uint8_t forced;                     /*!< Set to `1` if reset forced by user */
        } reset_detected;                       /*!< Reset occurred. Use with \ref LWESP_EVT_RESET_DETECTED event */

        struct {
            lwespr_t res;                       /*!< Reset operation result */
        } reset;                                /*!< Reset sequence finish. Use with \ref LWESP_EVT_RESET event */
        struct {
            lwespr_t res;                       /*!< Restore operation result */
        } restore;                              /*!< Restore sequence finish. Use with \ref LWESP_EVT_RESTORE event */

        struct {
            lwesp_conn_p conn;                  /*!< Connection where data were received */
            lwesp_pbuf_p buff;                  /*!< Pointer to received data */
        } conn_data_recv;                       /*!< Network data received. Use with \ref LWESP_EVT_CONN_RECV event */
        struct {
            lwesp_conn_p conn;                  /*!< Connection where data were sent */
            size_t sent;                        /*!< Number of bytes sent on connection */
            lwespr_t res;                       /*!< Send data result */
        } conn_data_send;                       /*!< Data send. Use with \ref LWESP_EVT_CONN_SEND event */
        struct {
            const char* host;                   /*!< Host to use for connection */
            lwesp_port_t port;                  /*!< Remote port used for connection */
            lwesp_conn_type_t type;             /*!< Connection type */
            void* arg;                          /*!< Connection user argument */
            lwespr_t err;                       /*!< Error value */
        } conn_error;                           /*!< Client connection start error. Use with \ref LWESP_EVT_CONN_ERROR event */
        struct {
            lwesp_conn_p conn;                  /*!< Pointer to connection */
            uint8_t client;                     /*!< Set to 1 if connection is/was client mode */
            uint8_t forced;                     /*!< Set to 1 if connection action was forced
                                                        when active: 1 = CLIENT, 0 = SERVER
                                                        when closed, 1 = CMD, 0 = REMOTE */
            lwespr_t res;                       /*!< Result of close event.
                                                        Set to \ref lwespOK on success */
        } conn_active_close;                    /*!< Process active and closed statuses at the same time.
                                                        Use with \ref LWESP_EVT_CONN_ACTIVE or
                                                        \ref LWESP_EVT_CONN_CLOSE events */
        struct {
            lwesp_conn_p conn;                  /*!< Set connection pointer */
        } conn_poll;                            /*!< Polling active connection to check for timeouts.
                                                        Use with \ref LWESP_EVT_CONN_POLL event */

        struct {
            lwespr_t res;                       /*!< Status of command */
            uint8_t en;                         /*!< Status to enable/disable server */
            lwesp_port_t port;                  /*!< Server port number */
        } server;                               /*!< Server change event. Use with \ref LWESP_EVT_SERVER event */
#if LWESP_CFG_MODE_STATION || __DOXYGEN__
        struct {
            lwespr_t res;                       /*!< Result of command */
            lwesp_ap_t* aps;                    /*!< Pointer to access points */
            size_t len;                         /*!< Number of access points found */
        } sta_list_ap;                          /*!< Station list access points. Use with \ref LWESP_EVT_STA_LIST_AP event */
        struct {
            lwespr_t res;                       /*!< Result of command */
        } sta_join_ap;                          /*!< Join to access point. Use with \ref LWESP_EVT_STA_JOIN_AP event */
        struct {
            lwesp_sta_info_ap_t* info;          /*!< AP info of current station */
            lwespr_t res;                       /*!< Result of command */
        } sta_info_ap;                          /*!< Current AP informations. Use with \ref LWESP_EVT_STA_INFO_AP event */
#endif /* LWESP_CFG_MODE_STATION || __DOXYGEN__ */
#if LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
        struct {
            lwesp_mac_t* mac;                   /*!< Station MAC address */
        } ap_conn_disconn_sta;                  /*!< A new station connected or disconnected to
                                                        ESP's access point. Use with
                                                        \ref LWESP_EVT_AP_CONNECTED_STA or
                                                        \ref LWESP_EVT_AP_DISCONNECTED_STA events */
        struct {
            lwesp_mac_t* mac;                   /*!< Station MAC address */
            lwesp_ip_t* ip;                     /*!< Station IP address */
        } ap_ip_sta;                            /*!< Station got IP address from ESP's access point.
                                                        Use with \ref LWESP_EVT_AP_IP_STA event */
#endif /* LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */
#if LWESP_CFG_DNS || __DOXYGEN__
        struct {
            lwespr_t res;                       /*!< Result of command */
            const char* host;                   /*!< Host name for DNS lookup */
            lwesp_ip_t* ip;                     /*!< Pointer to IP result */
        } dns_hostbyname;                       /*!< DNS domain service finished.
                                                        Use with \ref LWESP_EVT_DNS_HOSTBYNAME event */
#endif /* LWESP_CFG_DNS || __DOXYGEN__ */
#if LWESP_CFG_PING || __DOXYGEN__
        struct {
            lwespr_t res;                       /*!< Result of command */
            const char* host;                   /*!< Host name for ping */
            uint32_t time;                      /*!< Time required for ping. Valid only if operation succedded */
        } ping;                                 /*!< Ping finished. Use with \ref LWESP_EVT_PING event */
#endif /* LWESP_CFG_PING || __DOXYGEN__ */
#if LWESP_CFG_PING || __DOXYGEN__
        struct {
            lwespr_t res;                       /*!< Result of command */
            const lwesp_datetime_t* dt;         /*!< Pointer to datetime structure */
        } cip_sntp_time;                        /*!< SNTP time finished. Use with \ref LWESP_EVT_SNTP_TIME event */
#endif /* LWESP_CFG_PING || __DOXYGEN__ */


#if LWESP_CFG_WEBSERVER || __DOXYGEN__
        struct {
            uint8_t code;                       /*!< Result of command */
        } ws_status;                            /*!< Ping finished. Use with \ref LWESP_EVT_PING event */
#endif /* LWESP_CFG_WEBSERVER || __DOXYGEN__ */
    } evt;                                      /*!< Callback event union */
} lwesp_evt_t;

#define LWESP_SIZET_MAX                         ((size_t)(-1))  /*!< Maximal value of size_t variable type */

/**
 * \ingroup         LWESP_LL
 * \brief           Function prototype for AT output data
 * \param[in]       data: Pointer to data to send. This parameter can be set to `NULL`
 * \param[in]       len: Number of bytes to send. This parameter can be set to `0`
 *                      to indicate that internal buffer can be flushed to stream.
 *                      This is implementation defined and feature might be ignored
 * \return          Number of bytes sent
 */
typedef size_t (*lwesp_ll_send_fn)(const void* data, size_t len);

/**
 * \ingroup         LWESP_LL
 * \brief           Function prototype for hardware reset of ESP device
 * \param[in]       state: State indicating reset. When set to `1`, reset must be active (usually pin active low),
 *                      or set to `0` when reset is cleared
 * \return          `1` on successful action, `0` otherwise
 */
typedef uint8_t (*lwesp_ll_reset_fn)(uint8_t state);

/**
 * \ingroup         LWESP_LL
 * \brief           Low level user specific functions
 */
typedef struct {
    lwesp_ll_send_fn send_fn;                   /*!< Callback function to transmit data */
    lwesp_ll_reset_fn reset_fn;                 /*!< Reset callback function */
    struct {
        uint32_t baudrate;                      /*!< UART baudrate value */
    } uart;                                     /*!< UART communication parameters */
} lwesp_ll_t;

/**
 * \ingroup         LWESP_TIMEOUT
 * \brief           Timeout callback function prototype
 * \param[in]       arg: Custom user argument
 */
typedef void (*lwesp_timeout_fn)(void* arg);

/**
 * \ingroup         LWESP_TIMEOUT
 * \brief           Timeout structure
 */
typedef struct lwesp_timeout {
    struct lwesp_timeout* next;                 /*!< Pointer to next timeout entry */
    uint32_t time;                              /*!< Time difference from previous entry */
    void* arg;                                  /*!< Argument to pass to callback function */
    lwesp_timeout_fn fn;                        /*!< Callback function for timeout */
} lwesp_timeout_t;

/**
 * \ingroup         LWESP_BUFF
 * \brief           Buffer structure
 */
typedef struct {
    uint8_t* buff;                              /*!< Pointer to buffer data.
                                                    Buffer is considered initialized when `buff != NULL` */
    size_t size;                                /*!< Size of buffer data. Size of actual buffer is
                                                        `1` byte less than this value */
    size_t r;                                   /*!< Next read pointer. Buffer is considered empty
                                                        when `r == w` and full when `w == r - 1` */
    size_t w;                                   /*!< Next write pointer. Buffer is considered empty
                                                        when `r == w` and full when `w == r - 1` */
} lwesp_buff_t;

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           Linear buffer structure
 */
typedef struct {
    uint8_t* buff;                              /*!< Pointer to buffer data array */
    size_t len;                                 /*!< Length of buffer array */
    size_t ptr;                                 /*!< Current buffer pointer */
} lwesp_linbuff_t;

/**
 * \ingroup         LWESP_TYPEDEFS
 * \brief           Function declaration for API function command event callback function
 * \param[in]       res: Operation result, member of \ref lwespr_t enumeration
 * \param[in]       arg: Custom user argument
 */
typedef void (*lwesp_api_cmd_evt_fn) (lwespr_t res, void* arg);

/**
 * \ingroup         LWESP_CONN
 * \brief           Connection start structure, used to start the connection in extended mode
 */
typedef struct {
    lwesp_conn_type_t type;                     /*!< Connection type */
    const char* remote_host;                    /*!< Host name or IP address in string format */
    lwesp_port_t remote_port;                   /*!< Remote server port */
    const char* local_ip;                       /*!< Local IP. Optional parameter, set to NULL if not used (most cases) */
    union {
        struct {
            uint16_t keep_alive;                /*!< Keep alive parameter for TCP/SSL connection in units of seconds.
                                                    Value can be between `0 - 7200` where `0` means no keep alive */
        } tcp_ssl;                              /*!< TCP/SSL specific features */
        struct {
            lwesp_port_t local_port;            /*!< Custom local port for UDP */
            uint8_t mode;                       /*!< UDP mode. Set to `0` by default. Check ESP AT commands instruction set for more info when needed */
        } udp;                                  /*!< UDP specific features */
    } ext;                                      /*!< Extended support union */
} lwesp_conn_start_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_DEFS_H */

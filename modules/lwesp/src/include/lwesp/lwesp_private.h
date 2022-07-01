/**
 * \file            lwesp_private.h
 * \brief           Private structures and enumerations
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
#ifndef LWESP_HDR_PRIV_H
#define LWESP_HDR_PRIV_H

#include "lwesp/lwesp.h"
#include "lwesp/lwesp_debug.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \addtogroup      LWESP_TYPEDEFS
 * \{
 */

/**
 * \brief           List of possible messages
 */
typedef enum {
    LWESP_CMD_IDLE = 0,                         /*!< IDLE mode */

    /* Basic AT commands */
    LWESP_CMD_RESET,                            /*!< Reset device */
    LWESP_CMD_ATE0,                             /*!< Disable ECHO mode on AT commands */
    LWESP_CMD_ATE1,                             /*!< Enable ECHO mode on AT commands */
    LWESP_CMD_GMR,                              /*!< Get AT commands version */
    LWESP_CMD_CMD,                              /*!< List support AT commands */
    LWESP_CMD_GSLP,                             /*!< Set ESP to sleep mode */
    LWESP_CMD_RESTORE,                          /*!< Restore ESP internal settings to default values */
    LWESP_CMD_UART,
    LWESP_CMD_SLEEP,
    LWESP_CMD_WAKEUPGPIO,
    LWESP_CMD_RFPOWER,
    LWESP_CMD_RFVDD,
    LWESP_CMD_RFAUTOTRACE,
    LWESP_CMD_SYSRAM,
    LWESP_CMD_SYSADC,
    LWESP_CMD_SYSMSG,
    LWESP_CMD_SYSLOG,

    /* WiFi based commands */
    LWESP_CMD_WIFI_CWMODE,                      /*!< Set wifi mode */
    LWESP_CMD_WIFI_CWMODE_GET,                  /*!< Get wifi mode */
    LWESP_CMD_WIFI_CWLAPOPT,                    /*!< Configure what is visible on CWLAP response */
#if LWESP_CFG_IPV6 || __DOXYGEN__
    LWESP_CMD_WIFI_IPV6,                        /*!< Configure IPv6 support */
#endif /* LWESP_CFG_IPV6 || __DOXYGEN__ */
#if LWESP_CFG_MODE_STATION || __DOXYGEN__
    LWESP_CMD_WIFI_CWJAP,                       /*!< Connect to access point */
    LWESP_CMD_WIFI_CWRECONNCFG,                 /*!< Setup reconnect interval and maximum tries */
    LWESP_CMD_WIFI_CWJAP_GET,                   /*!< Info of the connected access point */
    LWESP_CMD_WIFI_CWQAP,                       /*!< Disconnect from access point */
    LWESP_CMD_WIFI_CWLAP,                       /*!< List available access points */
    LWESP_CMD_WIFI_CIPSTAMAC_GET,               /*!< Get MAC address of ESP station */
    LWESP_CMD_WIFI_CIPSTAMAC_SET,               /*!< Set MAC address of ESP station */
    LWESP_CMD_WIFI_CIPSTA_GET,                  /*!< Get IP address of ESP station */
    LWESP_CMD_WIFI_CIPSTA_SET,                  /*!< Set IP address of ESP station */
    LWESP_CMD_WIFI_CWAUTOCONN,                  /*!< Configure auto connection to access point */
#endif /* LWESP_CFG_MODE_STATION || __DOXYGEN__ */
    LWESP_CMD_WIFI_CWDHCP_SET,                  /*!< Set DHCP config */
    LWESP_CMD_WIFI_CWDHCP_GET,                  /*!< Get DHCP config */
    LWESP_CMD_WIFI_CWDHCPS_SET,                 /*!< Set DHCP SoftAP IP config */
    LWESP_CMD_WIFI_CWDHCPS_GET,                 /*!< Get DHCP SoftAP IP config */
#if LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
    LWESP_CMD_WIFI_CWSAP_GET,                   /*!< Get software access point configuration */
    LWESP_CMD_WIFI_CWSAP_SET,                   /*!< Set software access point configuration */
    LWESP_CMD_WIFI_CIPAPMAC_GET,                /*!< Get MAC address of ESP access point */
    LWESP_CMD_WIFI_CIPAPMAC_SET,                /*!< Set MAC address of ESP access point */
    LWESP_CMD_WIFI_CIPAP_GET,                   /*!< Get IP address of ESP access point */
    LWESP_CMD_WIFI_CIPAP_SET,                   /*!< Set IP address of ESP access point */
    LWESP_CMD_WIFI_CWLIF,                       /*!< Get connected stations on access point */
    LWESP_CMD_WIFI_CWQIF,                       /*!< Discnnect station from SoftAP */
#endif /* LWESP_CFG_MODE_STATION || __DOXYGEN__ */
#if LWESP_CFG_WPS || __DOXYGEN__
    LWESP_CMD_WIFI_WPS,                         /*!< Set WPS option */
#endif /* LWESP_CFG_WPS || __DOXYGEN__ */
#if LWESP_CFG_MDNS || __DOXYGEN__
    LWESP_CMD_WIFI_MDNS,                        /*!< Configure MDNS function */
#endif /* LWESP_CFG_MDNS || __DOXYGEN__ */
#if LWESP_CFG_HOSTNAME || __DOXYGEN__
    LWESP_CMD_WIFI_CWHOSTNAME_SET,              /*!< Set device hostname */
    LWESP_CMD_WIFI_CWHOSTNAME_GET,              /*!< Get device hostname */
#endif /* LWESP_CFG_HOSTNAME || __DOXYGEN__ */

    /* TCP/IP related commands */
#if LWESP_CFG_DNS || __DOXYGEN__
    LWESP_CMD_TCPIP_CIPDOMAIN,                  /*!< Get IP address from domain name = DNS function */
    LWESP_CMD_TCPIP_CIPDNS_SET,                 /*!< Configure user specific DNS servers */
    LWESP_CMD_TCPIP_CIPDNS_GET,                 /*!< Get DNS configuration */
#endif /* LWESP_CFG_DNS || __DOXYGEN__ */
    LWESP_CMD_TCPIP_CIPSTATUS,                  /*!< Get status of connections (deprecated, used on ESP8266 devices) */
    LWESP_CMD_TCPIP_CIPSTATE,                   /*!< Obtain connection state and information */
    LWESP_CMD_TCPIP_CIPSTART,                   /*!< Start client connection */
    LWESP_CMD_TCPIP_CIPSEND,                    /*!< Send network data */
    LWESP_CMD_TCPIP_CIPCLOSE,                   /*!< Close active connection */
    LWESP_CMD_TCPIP_CIPSSLSIZE,                 /*!< Set SSL buffer size for SSL connection */
    LWESP_CMD_TCPIP_CIPSSLCCONF,                /*!< Set the SSL configuration */
    LWESP_CMD_TCPIP_CIFSR,                      /*!< Get local IP */
    LWESP_CMD_TCPIP_CIPMUX,                     /*!< Set single or multiple connections */
    LWESP_CMD_TCPIP_CIPSERVER,                  /*!< Enables/Disables server mode */
    LWESP_CMD_TCPIP_CIPSERVERMAXCONN,           /*!< Sets maximal number of connections allowed for server population */
    LWESP_CMD_TCPIP_CIPMODE,                    /*!< Transmission mode, either transparent or normal one */
    LWESP_CMD_TCPIP_CIPSTO,                     /*!< Sets connection timeout */
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE || __DOXYGEN__
    LWESP_CMD_TCPIP_CIPRECVMODE,                /*!< Sets mode for TCP data receive (manual or automatic) */
    LWESP_CMD_TCPIP_CIPRECVDATA,                /*!< Manually reads TCP data from device */
    LWESP_CMD_TCPIP_CIPRECVLEN,                 /*!< Gets number of available bytes in connection to be read */
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE || __DOXYGEN__ */
    LWESP_CMD_TCPIP_CIUPDATE,                   /*!< Perform self-update */
#if LWESP_CFG_SNTP || __DOXYGEN__
    LWESP_CMD_TCPIP_CIPSNTPCFG,                 /*!< Configure SNTP servers */
    LWESP_CMD_TCPIP_CIPSNTPTIME,                /*!< Get current time using SNTP */
    LWESP_CMD_TCPIP_CIPSNTPINTV,                /*!< Query/Set the SNTP time synchronization interval */
#endif /* LWESP_SNT || __DOXYGEN__ */
    LWESP_CMD_TCPIP_CIPDINFO,                   /*!< Configure what data are received on +IPD statement */
#if LWESP_CFG_PING || __DOXYGEN__
    LWESP_CMD_TCPIP_PING,                       /*!< Ping domain */
#endif /* LWESP_CFG_PING || __DOXYGEN__ */
#if LWESP_CFG_SMART || __DOXYGEN__
    LWESP_CMD_WIFI_SMART_START,                 /*!< Start smart config */
    LWESP_CMD_WIFI_SMART_STOP,                  /*!< Stop smart config */
#endif /* LWESP_CFG_SMART || __DOXYGEN__ */
#if LWESP_CFG_WEBSERVER || __DOXYGEN__
    LWESP_CMD_WEBSERVER,                        /*!< Start or Stop Web Server */
#endif /* LWESP_CFG_WEBSERVER || __DOXYGEN__ */

    /* BLE commands, ESP32 only */
#if LWESP_CFG_ESP32 || __DOXYGEN__
    LWESP_CMD_BLEINIT_GET,                      /*!< Get BLE status */
#endif /* LWESP_CFG_ESP32 || __DOXYGEN__ */
    LWESP_CMD_AT_TEST,                      /*!< AT test */
} lwesp_cmd_t;

/**
 * \brief           Connection structure
 */
typedef struct lwesp_conn {
    lwesp_conn_type_t type;                     /*!< Connection type */
    uint8_t         num;                        /*!< Connection number */
    lwesp_ip_t      remote_ip;                  /*!< Remote IP address */
    lwesp_port_t    remote_port;                /*!< Remote port number */
    lwesp_port_t    local_port;                 /*!< Local IP address */
    lwesp_evt_fn    evt_func;                   /*!< Callback function for connection */
    void*           arg;                        /*!< User custom argument */

    uint8_t         val_id;                     /*!< Validation ID number. It is increased each time
                                                        a new connection is established.
                                                        It protects sending data to wrong connection
                                                        in case we have data in send queue,
                                                        and connection was closed and active
                                                        again in between. */

    lwesp_linbuff_t buff;                       /*!< Linear buffer structure */

    size_t          total_recved;               /*!< Total number of bytes received */

#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE || __DOXYGEN__
    size_t          tcp_available_bytes;        /*!< Number of bytes in ESP ready to be read on connection.
                                                        This variable always holds last known info from ESP
                                                        device and is not decremented (or incremented) by application */
    size_t          tcp_not_ack_bytes;          /*!< Number of bytes not acknowledge by application done with processing
                                                        This variable is increased everytime new packet is
                                                        read to be sent to application and decreased
                                                        when application acknowledges it */
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE || __DOXYGEN__ */

    union {
        struct {
            uint8_t active: 1;                  /*!< Status whether connection is active */
            uint8_t client: 1;                  /*!< Status whether connection is in client mode */
            uint8_t data_received: 1;           /*!< Status whether first data were received on connection */
            uint8_t in_closing: 1;              /*!< Status if connection is in closing mode.
                                                    When in closing mode, ignore any possible
                                                    received data from function */
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE || __DOXYGEN__
            uint8_t receive_blocked: 1;         /*!< Status whether we should block manual receive for some time */
            uint8_t receive_is_command_queued: 1;   /*!< Status whether manual read command is in the queue already */
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE || __DOXYGEN__ */
        } f;                                    /*!< Connection flags */
    } status;                                   /*!< Connection status union with flag bits */
} lwesp_conn_t;

/**
 * \ingroup         LWESP_PBUF
 * \brief           Packet buffer structure
 */
typedef struct lwesp_pbuf {
    struct lwesp_pbuf* next;                    /*!< Next pbuf in chain list */
    size_t tot_len;                             /*!< Total length of pbuf chain */
    size_t len;                                 /*!< Length of payload */
    size_t ref;                                 /*!< Number of references to this structure */
    uint8_t* payload;                           /*!< Pointer to payload memory */
    lwesp_ip_t ip;                              /*!< Remote address for received IPD data */
    lwesp_port_t port;                          /*!< Remote port for received IPD data */
} lwesp_pbuf_t;

/**
 * \brief           Incoming network data read structure
 */
typedef struct {
    uint8_t             read;                   /*!< Set to 1 when we should process input
                                                        data as connection data */
    size_t              tot_len;                /*!< Total length of packet */
    size_t              rem_len;                /*!< Remaining bytes to read in current +IPD statement */
    lwesp_conn_p        conn;                   /*!< Pointer to connection for network data */
    lwesp_ip_t          ip;                     /*!< Remote IP address on from IPD data */
    lwesp_port_t        port;                   /*!< Remote port on IPD data */

    size_t              buff_ptr;               /*!< Buffer pointer to save data to.
                                                        When set to `NULL` while `read = 1`,
                                                        reading should ignore incoming data */
    lwesp_pbuf_p        buff;                   /*!< Pointer to data buffer used for receiving data */
} lwesp_ipd_t;

/**
 * \brief           Message queue structure to share between threads
 */
typedef struct lwesp_msg {
    lwesp_cmd_t       cmd_def;                  /*!< Default message type received from queue */
    lwesp_cmd_t       cmd;                      /*!< Since some commands can have different
                                                        subcommands, sub command is used here */
    uint8_t           i;                        /*!< Variable to indicate order number of subcommands */
    lwesp_sys_sem_t   sem;                      /*!< Semaphore for the message */
    uint8_t           is_blocking;              /*!< Status if command is blocking */
    uint32_t          block_time;               /*!< Maximal blocking time in units of milliseconds.
                                                        Use `0` to for non-blocking call */
    lwespr_t          res;                      /*!< Result of message operation */
    lwespr_t          res_err_code;             /*!< Result from "ERR CODE" received by AT command execution */
    lwespr_t          (*fn)(struct lwesp_msg*); /*!< Processing callback function to process packet */

#if LWESP_CFG_USE_API_FUNC_EVT
    lwesp_api_cmd_evt_fn evt_fn;                /*!< Command callback API function */
    void*           evt_arg;                    /*!< Command callback API callback parameter */
#endif /* LWESP_CFG_USE_API_FUNC_EVT */

    union {
        struct {
            uint32_t delay;                     /*!< Delay in units of milliseconds before executing first RESET command */
        } reset;                                /*!< Reset device */
        struct {
            uint32_t baudrate;                  /*!< Baudrate for AT port */
        } uart;                                 /*!< UART configuration */
        struct {
            lwesp_mode_t mode;                  /*!< Mode of operation */
            lwesp_mode_t* mode_get;             /*!< Get mode */
        } wifi_mode;                            /*!< When message type \ref LWESP_CMD_WIFI_CWMODE is used */
#if LWESP_CFG_MODE_STATION || __DOXYGEN__
        struct {
            char name[32 + 1];                   /*!< AP name */
            char pass[64 + 1];                   /*!< AP password */
            lwesp_mac_t mac_s;             /*!< Specific MAC address to use when connecting to AP */
            const lwesp_mac_t *mac;             /*!< Specific MAC address to use when connecting to AP */
            uint8_t error_num;                  /*!< Error number on connecting */
        } sta_join;                             /*!< Message for joining to access point */
        struct {
            uint16_t interval;                  /*!< Interval in units of seconds */
            uint16_t rep_cnt;                   /*!< Repetition counter */
        } sta_reconn_set;                       /*!< Reconnect setup */
        struct {
            uint8_t en;                         /*!< Status to enable/disable auto join feature */
        } sta_autojoin;                         /*!< Message for auto join procedure */
        struct {
            lwesp_sta_info_ap_t* info;          /*!< Information structure */
        } sta_info_ap;                          /*!< Message for reading the AP information */
        struct {
            const char* ssid;                   /*!< Pointer to optional filter SSID name to search */
            lwesp_ap_t* aps;                    /*!< Pointer to array to save access points */
            size_t apsl;                        /*!< Length of input array of access points */
            size_t apsi;                        /*!< Current access point array */
            size_t* apf;                        /*!< Pointer to output variable holding
                                                        number of access points found */
        } ap_list;                              /*!< List for available access points to connect to */
#endif /* LWESP_CFG_MODE_STATION || __DOXYGEN__ */
#if LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
        struct {
            const char* ssid;                   /*!< Name of access point */
            const char* pwd;                    /*!< Password of access point */
            lwesp_ecn_t ecn;                    /*!< Ecryption used */
            uint8_t ch;                         /*!< RF Channel used */
            uint8_t max_sta;                    /*!< Max allowed connected stations */
            uint8_t hid;                        /*!< Configuration if network is hidden or visible */
        } ap_conf;                              /*!< Parameters to configure access point */
        struct {
            lwesp_ap_conf_t* ap_conf;           /*!< AP configuration */
        } ap_conf_get;                          /*!< Get the soft AP configuration */
        struct {
            lwesp_sta_t* stas;                  /*!< Pointer to array to save access points */
            size_t stal;                        /*!< Length of input array of access points */
            size_t stai;                        /*!< Current access point array */
            size_t* staf;                       /*!< Pointer to output variable holding number of access points found */
        } sta_list;                             /*!< List for stations connected to SoftAP */
        struct {
            uint8_t use_mac;                    /*!< Status if specific MAC is to be used */
            lwesp_mac_t mac;                    /*!< MAC address to disconnect from access point */
        } ap_disconn_sta;                       /*!< Disconnect station from access point */
#endif /* LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */
        struct {
            lwesp_ip_t* ip;                     /*!< Pointer to IP variable */
            lwesp_ip_t* gw;                     /*!< Pointer to gateway variable */
            lwesp_ip_t* nm;                     /*!< Pointer to netmask variable */
#if LWESP_CFG_IPV6
            lwesp_ip_t* ip6_ll;                 /*!< Pointer to IPV6 variable local address */
            lwesp_ip_t* ip6_gl;                 /*!< Pointer to IPV6 variable global address */
#endif /* LWESP_CFG_IPV6 */
        } sta_ap_getip;                         /*!< Message for reading station or access point IP */
        struct {
            lwesp_mac_t* mac;                   /*!< Pointer to MAC variable */
        } sta_ap_getmac;                        /*!< Message for reading station or access point MAC address */
        struct {
            lwesp_ip_t ip;                      /*!< IP variable */
            lwesp_ip_t gw;                      /*!< Gateway variable */
            lwesp_ip_t nm;                      /*!< Netmask variable */
        } sta_ap_setip;                         /*!< Message for setting station or access point IP */
        struct {
            lwesp_mac_t mac;                    /*!< Pointer to MAC variable */
        } sta_ap_setmac;                        /*!< Message for setting station or access point MAC address */
        struct {
            uint8_t sta;                        /*!< Set station DHCP settings */
            uint8_t ap;                         /*!< Set access point DHCP settings */
            uint8_t en;                         /*!< Enable/disable DHCP settings */
        } wifi_cwdhcp;                          /*!< Set DHCP settings */

#if LWESP_CFG_HOSTNAME || __DOXYGEN__
        struct {
            const char* hostname_set;           /*!< Hostname set value */
            char* hostname_get;                 /*!< Hostname get value */
            size_t length;                      /*!< Length of buffer when reading hostname */
        } wifi_hostname;                        /*!< Set or get hostname structure */
#endif /* LWESP_CFG_HOSTNAME || __DOXYGEN__ */

        /* Connection based commands */
        struct { 
            lwesp_conn_t** conn;                /*!< Pointer to pointer to save connection used */
            const char* remote_host;            /*!< Host to use for connection */
            lwesp_port_t remote_port;           /*!< Remote port used for connection */
            lwesp_conn_type_t type;             /*!< Connection type */
            const char* local_ip;               /*!< Local IP address. Normally set to NULL */
            uint16_t tcp_ssl_keep_alive;        /*!< Keep alive parameter for TCP */
            uint8_t udp_mode;                   /*!< UDP mode */
            lwesp_port_t udp_local_port;        /*!< UDP local port */
            void* arg;                          /*!< Connection custom argument */
            lwesp_evt_fn evt_func;              /*!< Callback function to use on connection */
            uint8_t success;                    /*!< Status if connection AT+CIPSTART succedded */
            uint8_t linkid;
        } conn_start;                           /*!< Structure for starting new connection */
        struct {
            lwesp_conn_t* conn;                 /*!< Pointer to connection to close */
            uint8_t val_id;                     /*!< Connection current validation ID when command was sent to queue */
            uint8_t linkid;
        } conn_close;                           /*!< Close connection */
        struct {
            lwesp_conn_t* conn;                 /*!< Pointer to connection to send data */
            size_t btw;                         /*!< Number of remaining bytes to write */
            size_t ptr;                         /*!< Current write pointer for data */
            const uint8_t* data;                /*!< Data to send */
            size_t sent;                        /*!< Number of bytes sent in last packet */
            size_t sent_all;                    /*!< Number of bytes sent all together */
            uint8_t tries;                      /*!< Number of tries used for last packet */
            uint8_t wait_send_ok_err;           /*!< Set to 1 when we wait for SEND OK or SEND ERROR */
            const lwesp_ip_t* remote_ip;        /*!< Remote IP address for UDP connection */
            lwesp_port_t remote_port;           /*!< Remote port address for UDP connection */
            uint8_t fau;                        /*!< Free after use flag to free memory after data are sent (or not) */
            size_t* bw;                         /*!< Number of bytes written so far */
            uint8_t val_id;                     /*!< Connection current validation ID when command was sent to queue */
        } conn_send;                            /*!< Structure to send data on connection */
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
        struct {
            lwesp_conn_t* conn;                 /*!< Connection handle */
            size_t len;                         /*!< Number of bytes to read */
            lwesp_pbuf_p buff;                  /*!< Buffer handle */
            uint8_t ipd_recv;                   /*!< Status indicating `+IPD` has been received during `AT+CIPRECVLEN` command.
                                                        When this happens, we need to repeat same command */
            uint8_t is_last_check;              /*!< Status indicating check for data length is at the end of command.
                                                        Do nothing after successful command */
        } ciprecvdata;                          /*!< Structure to manually read TCP data */
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */

        /* TCP/IP based commands */
        struct {
            uint8_t en;                         /*!< Enable/Disable server status */
            lwesp_port_t port;                  /*!< Server port number */
            uint16_t max_conn;                  /*!< Maximal number of connections available for server */
            uint16_t timeout;                   /*!< Connection timeout */
            lwesp_evt_fn cb;                    /*!< Server default callback function */
        } tcpip_server;                         /*!< Server configuration */
        struct {
            size_t size;                        /*!< Size for SSL in uints of bytes */
        } tcpip_sslsize;                        /*!< TCP SSL size for SSL connections */
#if LWESP_CFG_DNS
        struct {
            const char* host;                   /*!< Hostname to resolve IP address for */
            lwesp_ip_t* ip;                     /*!< Pointer to IP address to save result */
        } dns_getbyhostname;                    /*!< DNS function */
        struct {
            uint8_t en;                         /*!< Enable/Disable status */
            const char* s1;                     /*!< DNS server 1 */
            const char* s2;                     /*!< DNS server 2 */
        } dns_setconfig;                        /*!< Set DNS config */
        struct {
            uint8_t dnsi;                       /*!< DNS server index to get */
            lwesp_ip_t* s1;                     /*!< DNS server 1 */
            lwesp_ip_t* s2;                     /*!< DNS server 2 */
        } dns_getconf;                          /*!< Get DNS config */
#endif /* LWESP_CFG_DNS */
#if LWESP_CFG_PING || __DOXYGEN__
        struct {
            const char* host;                   /*!< Hostname to ping */
            uint32_t time;                      /*!< Time used for ping */
            uint32_t* time_out;                 /*!< Pointer to time output variable */
        } tcpip_ping;                           /*!< Pinging structure */
#endif /* LWESP_CFG_PING || __DOXYGEN__ */
#if LWESP_CFG_SNTP || __DOXYGEN__
        struct {
            uint8_t en;                         /*!< Status if SNTP is enabled or not */
            int8_t tz;                          /*!< Timezone setup */
            const char* h1;                     /*!< Optional server 1 */
            const char* h2;                     /*!< Optional server 2 */
            const char* h3;                     /*!< Optional server 3 */
        } tcpip_sntp_cfg;                       /*!< SNTP configuration */
        struct {
            uint32_t interval;                  /*!< Time in units of seconds */
        } tcpip_sntp_intv;                      /*!< SNTP interval configuration */
        struct {
            lwesp_datetime_t* dt;               /*!< Pointer to datetime structure */
        } tcpip_sntp_time;                      /*!< SNTP get time */
#endif /* LWESP_CFG_SNTP || __DOXYGEN__ */
#if LWESP_CFG_WPS || __DOXYGEN__
        struct {
            uint8_t en;                         /*!< Status if WPS is enabled or not */
        } wps_cfg;                              /*!< WPS configuration */
#endif /* LWESP_CFG_WPS || __DOXYGEN__ */
#if LWESP_CFG_MDNS || __DOXYGEN__
        struct {
            uint8_t en;                         /*!< Set to 1 to enable or 0 to disable */
            const char* host;                   /*!< mDNS host name */
            const char* server;                 /*!< mDNS server */
            lwesp_port_t port;                  /*!< mDNS server port */
        } mdns;                                 /*!< mDNS configuration */
#endif /* LWESP_CFG_MDNS || __DOXYGEN__ */
#if LWESP_CFG_WEBSERVER || __DOXYGEN__
        struct {
            uint8_t en;                         /*!< Enable/Disable web server status */
            lwesp_port_t port;                  /*!< Server port number */
            uint8_t timeout;                    /*!< Connection timeout */
        } web_server;                           /*!< Web Server configuration */
#endif /* LWESP_CFG_WEBSERVER || __DOXYGEN__ */
        struct {
            uint8_t link_id;                    /*!< Link ID of connection to set SSL configuration for */
            uint8_t auth_mode;                  /*!< Timezone setup */
            uint8_t pki_number;                 /*!< The index of cert and private key, if only one cert and private key, the value should be 0. */
            uint8_t ca_number;                  /*!< The index of CA, if only one CA, the value should be 0. */
        } tcpip_ssl_cfg;                        /*!< SSl configuration for connection */
    } msg;                                      /*!< Group of different message contents */
} lwesp_msg_t;

/**
 * \brief           IP and MAC structure with netmask and gateway addresses
 */
typedef struct {
    lwesp_ip_t ip;                              /*!< IP address */
    lwesp_ip_t gw;                              /*!< Gateway address */
    lwesp_ip_t nm;                              /*!< Netmask address */
#if LWESP_CFG_IPV6
    lwesp_ip_t ip6_ll;                          /*!< Local IPV6 address */
    lwesp_ip_t ip6_gl;                          /*!< Global IPV6 address */
#endif /* LWESP_CFG_IPV6 */
    lwesp_mac_t mac;                            /*!< MAC address */
    uint8_t dhcp;                               /*!< Flag indicating DHCP is enabled */
    struct {
        uint8_t has_ip : 1;                     /*!< Flag indicating IP is available */
#if LWESP_CFG_IPV6
        uint8_t has_ipv6_ll : 1;                /*!< Flag indicating local IPv6 is available */
        uint8_t has_ipv6_gl : 1;                /*!< Flag indicating global IPv6 is available */
#endif /* LWESP_CFG_IPV6 */
        uint8_t is_connected : 1;               /*!< Flag indicating ESP is connected to wifi */
    } f;                                        /*!< Flags structure */
} lwesp_ip_mac_t;

/**
 * \brief           Link connection active info
 */
typedef struct {
    uint8_t failed;                             /*!< Status if connection successful */
    uint8_t num;                                /*!< Connection number */
    uint8_t is_server;                          /*!< Status if connection is client or server */
    lwesp_conn_type_t type;                     /*!< Connection type */
    lwesp_ip_t remote_ip;                       /*!< Remote IP address */
    lwesp_port_t remote_port;                   /*!< Remote port */
    lwesp_port_t local_port;                    /*!< Local port number */
} lwesp_link_conn_t;

/**
 * \brief           Callback function linked list prototype
 */
typedef struct lwesp_evt_func {
    struct lwesp_evt_func* next;                /*!< Next function in the list */
    lwesp_evt_fn fn;                            /*!< Function pointer itself */
} lwesp_evt_func_t;

/**
 * \brief           ESP modules structure
 */
typedef struct {
    lwesp_device_t      device;                 /*!< ESP device type */
    lwesp_sw_version_t  version_at;             /*!< Version of AT command software on ESP device */
    lwesp_sw_version_t  version_sdk;            /*!< Version of SDK used to build AT software */

    uint32_t            active_conns;           /*!< Bit field of currently active connections, @todo: In case user has more than 32 connections, single variable is not enough */
    uint32_t            active_conns_last;      /*!< The same as previous but status before last check */

    lwesp_link_conn_t   link_conn;              /*!< Link connection handle */
    lwesp_ipd_t         ipd;                    /*!< Connection incoming data structure */
    lwesp_conn_t        conns[LWESP_CFG_MAX_CONNS]; /*!< Array of all connection structures */

#if LWESP_CFG_MODE_STATION || __DOXYGEN__
    lwesp_ip_mac_t      sta;                    /*!< Station IP and MAC addressed */
#endif /* LWESP_CFG_MODE_STATION || __DOXYGEN__ */
#if LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
    lwesp_ip_mac_t      ap;                     /*!< Access point IP and MAC addressed */
#endif /* LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */
} lwesp_modules_t;

/**
 * \brief           ESP global structure
 */
typedef struct {
    int              locked_cnt;             /*!< Counter how many times (recursive) stack is currently locked */

    lwesp_sys_sem_t       sem_sync;             /*!< Synchronization semaphore between threads */
    lwesp_sys_mbox_t      mbox_producer;        /*!< Producer message queue handle */
    lwesp_sys_mbox_t      mbox_process;         /*!< Consumer message queue handle */
    lwesp_sys_thread_t    thread_produce;       /*!< Producer thread handle */
    lwesp_sys_thread_t    thread_process;       /*!< Processing thread handle */
#if !LWESP_CFG_INPUT_USE_PROCESS || __DOXYGEN__
    lwesp_buff_t          buff;                 /*!< Input processing buffer */
#endif /* !LWESP_CFG_INPUT_USE_PROCESS || __DOXYGEN__ */
    lwesp_ll_t            ll;                   /*!< Low level functions */

    lwesp_msg_t*          msg;                  /*!< Pointer to current user message being executed */

    lwesp_evt_t           evt;                  /*!< Callback processing structure */
    lwesp_evt_func_t*     evt_func;             /*!< Callback function linked list */
    lwesp_evt_fn          evt_server;           /*!< Default callback function for server connections */

    lwesp_modules_t       m;                    /*!< All modules. When resetting, reset structure */

    union {
        struct {
            uint8_t     initialized: 1;         /*!< Flag indicating ESP library is initialized */
            uint8_t     dev_present: 1;         /*!< Flag indicating if physical device is connected to host device */
        } f;                                    /*!< Flags structure */
    } status;                                   /*!< Status structure */

    uint8_t conn_val_id;                        /*!< Validation ID increased each time device
                                                        connects to wifi network or on reset.
                                                        It is used for connections */
} lwesp_t;

/**
 * \ingroup         LWESP_UNICODE
 * \brief           Unicode support structure
 */
typedef struct {
    uint8_t ch[4];                              /*!< UTF-8 max characters */
    uint8_t t;                                  /*!< Total expected length in UTF-8 sequence */
    uint8_t r;                                  /*!< Remaining bytes in UTF-8 sequence */
    lwespr_t res;                               /*!< Current result of processing */
} lwesp_unicode_t;

/**
 * \}
 */

#if !__DOXYGEN__

/**
 * \ingroup         LWESP
 * \defgroup        LWESP_PRIVATE Internal functions
 * \brief           Functions, structures and enumerations
 * \{
 */

extern lwesp_t esp;

#define LWESP_MSG_VAR_DEFINE(name)                lwesp_msg_t* name
#define LWESP_MSG_VAR_ALLOC(name, blocking)       do {\
        (name) = lwesp_mem_malloc(sizeof(*(name)));       \
        LWESP_DEBUGW(LWESP_CFG_DBG_VAR | LWESP_DBG_TYPE_TRACE, (name) != NULL, "[MSG VAR] Allocated %d bytes at %p\r\n", sizeof(*(name)), (name)); \
        LWESP_DEBUGW(LWESP_CFG_DBG_VAR | LWESP_DBG_TYPE_TRACE, (name) == NULL, "[MSG VAR] Error allocating %d bytes\r\n", sizeof(*(name))); \
        if ((name) == NULL) {                           \
            return lwespERRMEM;                           \
        }                                               \
        LWESP_MEMSET((name), 0x00, sizeof(*(name)));      \
        (name)->is_blocking = LWESP_U8((blocking) > 0);   \
    } while (0)
#define LWESP_MSG_VAR_REF(name)                   (*(name))
#define LWESP_MSG_VAR_FREE(name)                  do {\
        LWESP_DEBUGF(LWESP_CFG_DBG_VAR | LWESP_DBG_TYPE_TRACE, "[MSG VAR] Free memory: %p\r\n", (name)); \
        if (lwesp_sys_sem_isvalid(&((name)->sem))) {      \
            lwesp_sys_sem_delete(&((name)->sem));         \
            lwesp_sys_sem_invalid(&((name)->sem));        \
        }                                               \
        lwesp_mem_free_s((void **)&(name));               \
    } while (0)
#if LWESP_CFG_USE_API_FUNC_EVT
#define LWESP_MSG_VAR_SET_EVT(name, e_fn, e_arg)  do {\
        (name)->evt_fn = (e_fn);                        \
        (name)->evt_arg = (e_arg);                      \
    } while (0)
#else /* LWESP_CFG_USE_API_FUNC_EVT */
#define LWESP_MSG_VAR_SET_EVT(name, e_fn, e_arg) do { LWESP_UNUSED(e_fn); LWESP_UNUSED(e_arg); } while (0)
#endif /* !LWESP_CFG_USE_API_FUNC_EVT */

#define LWESP_CHARISNUM(x)                    ((x) >= '0' && (x) <= '9')
#define LWESP_CHARTONUM(x)                    ((x) - '0')
#define LWESP_CHARISHEXNUM(x)                 (((x) >= '0' && (x) <= '9') || ((x) >= 'a' && (x) <= 'f') || ((x) >= 'A' && (x) <= 'F'))
#define LWESP_CHARHEXTONUM(x)                 (((x) >= '0' && (x) <= '9') ? ((x) - '0') : (((x) >= 'a' && (x) <= 'f') ? ((x) - 'a' + 10) : (((x) >= 'A' && (x) <= 'F') ? ((x) - 'A' + 10) : 0)))
#define LWESP_ISVALIDASCII(x)                 (((x) >= 32 && (x) <= 126) || (x) == '\r' || (x) == '\n')

#if LWESP_CFG_IPV6
#define LWESP_RESET_STA_HAS_IP()            do { esp.m.sta.f.has_ip = 0; esp.m.sta.f.has_ipv6_ll = 0; esp.m.sta.f.has_ipv6_gl = 0; } while (0)
#else
#define LWESP_RESET_STA_HAS_IP()            esp.m.sta.f.has_ip = 0
#endif /* LWESP_CFG_IPV6 */

#define CMD_IS_CUR(c)                       (esp.msg != NULL && esp.msg->cmd == (c))
#define CMD_IS_DEF(c)                       (esp.msg != NULL && esp.msg->cmd_def == (c))
#define CMD_GET_CUR()                       ((lwesp_cmd_t)(((esp.msg != NULL) ? esp.msg->cmd : LWESP_CMD_IDLE)))
#define CMD_GET_DEF()                       ((lwesp_cmd_t)(((esp.msg != NULL) ? esp.msg->cmd_def : LWESP_CMD_IDLE)))

#define CRLF                                "\r\n"
#define CRLF_LEN                            2

#define LWESP_PORT2NUM(port)                  ((uint32_t)(port))

const char* lwespi_dbg_msg_to_string(lwesp_cmd_t cmd);
lwespr_t    lwespi_process(const void* data, size_t len);
lwespr_t    lwespi_process_buffer(void);
lwespr_t    lwespi_initiate_cmd(lwesp_msg_t* msg);
uint8_t     lwespi_is_valid_conn_ptr(lwesp_conn_p conn);
lwespr_t    lwespi_send_cb(lwesp_evt_type_t type);
lwespr_t    lwespi_send_conn_cb(lwesp_conn_t* conn, lwesp_evt_fn cb);
void        lwespi_conn_init(void);
void        lwespi_conn_start_timeout(lwesp_conn_p conn);
lwespr_t    lwespi_conn_check_available_rx_data(void);
lwespr_t    lwespi_conn_manual_tcp_try_read_data(lwesp_conn_p conn);
lwespr_t    lwespi_send_msg_to_producer_mbox(lwesp_msg_t* msg, lwespr_t (*process_fn)(lwesp_msg_t*), uint32_t max_block_time);
uint32_t    lwespi_get_from_mbox_with_timeout_checks(lwesp_sys_mbox_t* b, void** m, uint32_t timeout);

void        lwespi_reset_everything(uint8_t forced);
void        lwespi_process_events_for_timeout_or_error(lwesp_msg_t* msg, lwespr_t err);

lwesp_cmd_t lwespi_get_cipstatus_or_cipstate_cmd(void);

/**
 * \}
 */

#endif /* !__DOXYGEN__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_PRIV_H */

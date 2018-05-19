/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __ESPCONN_H__
#define __ESPCONN_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef sint8 err_t;

typedef void *espconn_handle;

/** \defgroup Espconn_APIs Network Espconn APIs
  * @brief Network espconn APIs
  *
  */

/** @addtogroup Espconn_APIs
  * @{
  */

/**
  * @brief     Connect callback.
  *
  *            Callback which will be called if successful listening (ESP8266 as TCP server)
  *            or connection (ESP8266 as TCP client) callback, register by espconn_regist_connectcb.
  *
  * @attention The pointer "void *arg" may be different in different callbacks, please don't
  *            use this pointer directly to distinguish one from another in multiple connections,
  *            use remote_ip and remote_port in espconn instead.
  *
  * @param     void *arg : pointer corresponding structure espconn.
  *
  * @return    null
  */
typedef void (* espconn_connect_callback)(void *arg);

/**
  * @brief     Reconnect callback.
  *
  *            Enter this callback when error occurred, TCP connection broke. This callback is
  *            registered by espconn_regist_reconcb.
  *
  * @attention The pointer "void *arg" may be different in different callbacks, please don't
  *            use this pointer directly to distinguish one from another in multiple connections,
  *            use remote_ip and remote_port in espconn instead.
  *
  * @param     void *arg : pointer corresponding structure espconn.
  * @param     sint8 err : error code
  *    -  ESCONN_TIMEOUT    - Timeout
  *    -  ESPCONN_ABRT      - TCP connection aborted
  *    -  ESPCONN_RST       - TCP connection abort
  *    -  ESPCONN_CLSD      - TCP connection closed
  *    -  ESPCONN_CONN      - TCP connection
  *    -  ESPCONN_HANDSHAKE - TCP SSL handshake fail
  *    -  ESPCONN_PROTO_MSG - SSL application invalid
  *
  * @return  null
  */
typedef void (* espconn_reconnect_callback)(void *arg, sint8 err);

/* Definitions for error constants. */

#define ESPCONN_OK           0  /**< No error, everything OK.   */
#define ESPCONN_MEM         -1  /**< Out of memory.             */
#define ESPCONN_TIMEOUT     -3  /**< Timeout.                   */
#define ESPCONN_RTE         -4  /**< Routing problem.           */
#define ESPCONN_INPROGRESS  -5  /**< Operation in progress.     */
#define ESPCONN_MAXNUM      -7  /**< Total number exceeds the maximum limitation. */

#define ESPCONN_ABRT        -8  /**< Connection aborted.    */
#define ESPCONN_RST         -9  /**< Connection reset.      */
#define ESPCONN_CLSD       -10  /**< Connection closed.     */
#define ESPCONN_CONN       -11  /**< Not connected.         */

#define ESPCONN_ARG        -12  /**< Illegal argument.      */
#define ESPCONN_IF         -14  /**< UDP send error.        */
#define ESPCONN_ISCONN     -15  /**< Already connected.     */

/** Protocol family and type of the espconn */
enum espconn_type {
    ESPCONN_INVALID = 0,     /**< invalid type  */
    ESPCONN_TCP     = 0x10,  /**< TCP           */
    ESPCONN_UDP     = 0x20,  /**< UDP           */
};

/** Current state of the espconn. */
enum espconn_state {
    ESPCONN_NONE,        /**< idle state, no connection */
    ESPCONN_WAIT,        /**< ESP8266 is as TCP client, and waiting for connection */
    ESPCONN_LISTEN,      /**< ESP8266 is as TCP server, and waiting for connection */
    ESPCONN_CONNECT,     /**< connected         */
    ESPCONN_WRITE,       /**< sending data      */
    ESPCONN_READ,        /**< receiving data    */
    ESPCONN_CLOSE        /**< connection closed */
};

typedef struct _esp_tcp {
    int remote_port;      /**< remote port of TCP connection            */
    int local_port;       /**< ESP8266's local port of TCP connection   */
    uint8 local_ip[4];    /**< local IP of ESP8266                      */
    uint8 remote_ip[4];   /**< remote IP of TCP connection              */
    espconn_connect_callback connect_callback;      /**< connected callback */
    espconn_reconnect_callback reconnect_callback;  /**< as error handler, the TCP connection broke unexpectedly */
    espconn_connect_callback disconnect_callback;   /**< disconnected callback */
    espconn_connect_callback write_finish_fn;       /**< data send by espconn_send has wrote into buffer waiting for sending, or has sent successfully */
} esp_tcp;

typedef struct _esp_udp {
    int remote_port;    /**< remote port of UDP transmission            */
    int local_port;     /**< ESP8266's local port for UDP transmission  */
    uint8 local_ip[4];  /**< local IP of ESP8266                        */
    uint8 remote_ip[4]; /**< remote IP of UDP transmission              */
} esp_udp;

typedef struct _remot_info {
    enum espconn_state state;   /**< state of espconn   */
    int remote_port;            /**< remote port        */
    uint8 remote_ip[4];         /**< remote IP address  */
} remot_info;

/** A callback prototype to inform about events for a espconn */
typedef void (* espconn_recv_callback)(void *arg, char *pdata, unsigned short len);
typedef void (* espconn_sent_callback)(void *arg);

/** A espconn descriptor */
struct espconn {
    enum espconn_type type;     /**< type of the espconn (TCP or UDP)   */
    enum espconn_state state;   /**< current state of the espconn       */
    union {
        esp_tcp *tcp;
        esp_udp *udp;
    } proto;
    espconn_recv_callback recv_callback;    /**< data received callback */
    espconn_sent_callback sent_callback;    /**< data sent callback     */
    uint8 link_cnt;         /**< link count             */
    void *reserve;          /**< reserved for user data */
};

enum espconn_option {
    ESPCONN_START     = 0x00,   /**< no option, start enum. */
    ESPCONN_REUSEADDR = 0x01,   /**< free memory after TCP disconnection happen, need not wait 2 minutes. */
    ESPCONN_NODELAY   = 0x02,   /**< disable nagle algorithm during TCP data transmission, quicken the data transmission. */
    ESPCONN_COPY      = 0x04,   /**< enable espconn_regist_write_finish, enter write_finish_callback means that the data espconn_send sending was written into 2920 bytes write-buffer waiting for sending or already sent. */
    ESPCONN_KEEPALIVE = 0x08,   /**< enable TCP keep alive.    */
    ESPCONN_END                 /**< no option, end enum.      */
};

enum espconn_level {
    ESPCONN_KEEPIDLE,   /**< TCP keep-alive interval, unit : second.                */
    ESPCONN_KEEPINTVL,  /**< packet interval during TCP keep-alive, unit : second.  */
    ESPCONN_KEEPCNT     /**< maximum packet retry count of TCP keep-alive.          */
};

enum {
    ESPCONN_IDLE = 0,
    ESPCONN_CLIENT,
    ESPCONN_SERVER,
    ESPCONN_BOTH,
    ESPCONN_MAX
};

/**
  * @brief     espconn initialization.
  *
  * @attention   Please call this API in user_init, if you need to use espconn functions.
  *
  * @param     null
  *
  * @return    null
  */
void espconn_init(void);

/**
  * @brief     Connect to a TCP server (ESP8266 acting as TCP client).
  *
  * @attention If espconn_connect fail, returns non-0 value, there is no connection, so it
  *            won't enter any espconn callback.
  *
  * @param     struct espconn *espconn : the network connection structure,  the espconn to
  *            listen to the connection
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_RTE    - Routing Problem
  *    -  ESPCONN_MEM    - Out of memory
  *    -  ESPCONN_ISCONN - Already connected
  *    -  ESPCONN_ARG    - illegal argument, can't find the corresponding TCP connection
  *                        according to structure espconn
  */
sint8 espconn_connect(struct espconn *espconn);

/**
  * @brief     Disconnect a TCP connection.
  *
  * @attention Don't call this API in any espconn callback. If needed, please use system
  *            task to trigger espconn_disconnect.
  *
  * @param     struct espconn *espconn : the network connection structure
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection
  *                     according to structure espconn
  */
sint8 espconn_disconnect(struct espconn *espconn);

/**
  * @brief     Delete a transmission.
  *
  * @attention Corresponding creation API :
  *    -  TCP: espconn_accept,
  *    -  UDP: espconn_create
  *
  * @param     struct espconn *espconn : the network connection structure
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding network according
  *                     to structure espconn
  *    -  ESPCONN_INPROGRESS - the connection is still in progress, please call espconn_disconnect
  *                     to disconnect before delete it.
  */
sint8 espconn_delete(struct espconn *espconn);

/**
  * @brief  Creates a TCP server (i.e. accepts connections).
  *
  * @param  struct espconn *espconn : the network connection structure
  *
  * @return 0     : succeed
  * @return Non-0 : error code
  *    -  ESPCONN_MEM    - Out of memory
  *    -  ESPCONN_ISCONN - Already connected
  *    -  ESPCONN_ARG    - illegal argument, can't find the corresponding TCP connection
  *                        according to structure espconn
  */
sint8 espconn_accept(struct espconn *espconn);

/**
  * @brief     Create UDP transmission.
  *
  * @attention Parameter remote_ip and remote_port need to be set, do not set to be 0.
  *
  * @param     struct espconn *espconn : the UDP control block structure
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_MEM    - Out of memory
  *    -  ESPCONN_ISCONN - Already connected
  *    -  ESPCONN_ARG    - illegal argument, can't find the corresponding UDP transmission
  *                        according to structure espconn
  */
sint8 espconn_create(struct espconn *espconn);

/**
  * @brief  Get maximum number of how many TCP connections are allowed.
  *
  * @param  null
  *
  * @return Maximum number of how many TCP connections are allowed.
  */
uint8 espconn_tcp_get_max_con(void);

/**
  * @brief  Set the maximum number of how many TCP connection is allowed.
  *
  * @param  uint8 num : Maximum number of how many TCP connection is allowed.
  *
  * @return 0     : succeed
  * @return Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection
  *                     according to structure espconn
  */
sint8 espconn_tcp_set_max_con(uint8 num);

/**
  * @brief  Get the maximum number of TCP clients which are allowed to connect to ESP8266 TCP server.
  *
  * @param  struct espconn *espconn : the TCP server structure
  *
  * @return 0     : succeed
  * @return Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection according
  *                     to structure espconn
  */
sint8 espconn_tcp_get_max_con_allow(struct espconn *espconn);

/**
  * @brief  Set the maximum number of TCP clients allowed to connect to ESP8266 TCP server.
  *
  * @param  struct espconn *espconn : the TCP server structure
  * @param  uint8 num : Maximum number of TCP clients which are allowed
  *
  * @return 0     : succeed
  * @return Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection according
  *                     to structure espconn
  */
sint8 espconn_tcp_set_max_con_allow(struct espconn *espconn, uint8 num);

/**
  * @brief     Register timeout interval of ESP8266 TCP server.
  *
  * @attention 1. If timeout is set to 0, timeout will be disable and ESP8266 TCP server will
  *               not disconnect TCP clients has stopped communication. This usage of timeout=0,
  *               is deprecated.
  * @attention 2. This timeout interval is not very precise, only as reference.
  *
  * @param     struct espconn *espconn : the TCP connection structure
  * @param     uint32 interval : timeout interval, unit: second, maximum: 7200 seconds
  * @param     uint8 type_flag : 0, set for all connections; 1, set for a specific connection
  *    -  If the type_flag set to be 0, please call this API after espconn_accept, before listened
  *       a TCP connection.
  *    -  If the type_flag set to be 1, the first parameter *espconn is the specific connection.
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection according
  *                     to structure espconn
  */
sint8 espconn_regist_time(struct espconn *espconn, uint32 interval, uint8 type_flag);

/**
  * @brief  Get the information about a TCP connection or UDP transmission.
  *
  * @param  struct espconn *espconn : the network connection structure
  * @param  remot_info **pcon_info : connect to client info
  * @param  uint8 typeflags : 0, regular server; 1, ssl server
  *
  * @return 0     : succeed
  * @return Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding transmission according to
  *                     structure espconn
  */
sint8 espconn_get_connection_info(struct espconn *pespconn, remot_info **pcon_info, uint8 typeflags);

/**
  * @brief  Register data sent callback which will be called back when data are successfully sent.
  *
  * @param  struct espconn *espconn : the network connection structure
  * @param  espconn_sent_callback sent_cb : registered callback function which will be called if
  *                                         the data is successfully sent
  *
  * @return 0     : succeed
  * @return Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding transmission according
  *                     to structure espconn
  */
sint8 espconn_regist_sentcb(struct espconn *espconn, espconn_sent_callback sent_cb);

/**
  * @brief  Register a callback which will be called when all sending TCP data is completely
  *         write into write-buffer or sent.
  *
  *         Need to call espconn_set_opt to enable write-buffer first.
  *
  * @attention 1. write-buffer is used to keep TCP data that waiting to be sent, queue number
  *               of the write-buffer is 8 which means that it can keep 8 packets at most.
  *               The size of write-buffer is 2920 bytes.
  * @attention 2. Users can enable it by using espconn_set_opt.
  * @attention 3. Users can call espconn_send to send the next packet in write_finish_callback
  *               instead of using espconn_sent_callback.
  *
  * @param     struct espconn *espconn : the network connection structure
  * @param     espconn_connect_callback write_finish_fn : registered callback function which will
  *                                                       be called if the data is completely write
  *                                                       into write buffer or sent.
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection according
  *                     to structure espconn
  */
sint8 espconn_regist_write_finish(struct espconn *espconn, espconn_connect_callback write_finish_fn);

/**
  * @brief     Send data through network.
  *
  * @attention 1. Please call espconn_send after espconn_sent_callback of the pre-packet.
  * @attention 2. If it is a UDP transmission, it is suggested to set espconn->proto.udp->remote_ip
  *               and remote_port before every calling of espconn_send.
  *
  * @param     struct espconn *espconn : the network connection structure
  * @param     uint8 *psent : pointer of data
  * @param     uint16 length : data length
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_MEM    - Out of memory
  *    -  ESPCONN_ARG    - illegal argument, can't find the corresponding network transmission
  *                        according to structure espconn
  *    -  ESPCONN_MAXNUM - buffer of sending data is full
  *    -  ESPCONN_IF     - send UDP data fail
  */
sint8 espconn_send(struct espconn *espconn, uint8 *psent, uint16 length);

/**
  * @brief     Send data through network.
  *
  *            This API is deprecated, please use espconn_send instead.
  *
  * @attention 1. Please call espconn_sent after espconn_sent_callback of the pre-packet.
  * @attention 2. If it is a UDP transmission, it is suggested to set espconn->proto.udp->remote_ip
  *               and remote_port before every calling of espconn_sent.
  *
  * @param     struct espconn *espconn : the network connection structure
  * @param     uint8 *psent : pointer of data
  * @param     uint16 length : data length
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_MEM    - Out of memory
  *    -  ESPCONN_ARG    - illegal argument, can't find the corresponding network transmission
  *                        according to structure espconn
  *    -  ESPCONN_MAXNUM - buffer of sending data is full
  *    -  ESPCONN_IF     - send UDP data fail
  */
sint8 espconn_sent(struct espconn *espconn, uint8 *psent, uint16 length);

/**
  * @brief  Send UDP data.
  *
  * @param  struct espconn *espconn : the UDP structure
  * @param  uint8 *psent : pointer of data
  * @param  uint16 length : data length
  *
  * @return 0     : succeed
  * @return Non-0 : error code
  *    -  ESPCONN_MEM    - Out of memory
  *    -  ESPCONN_MAXNUM - buffer of sending data is full
  *    -  ESPCONN_IF     - send UDP data fail
  */
sint16 espconn_sendto(struct espconn *espconn, uint8 *psent, uint16 length);

/**
  * @brief  Register connection function which will be called back under successful TCP connection.
  *
  * @param  struct espconn *espconn : the TCP connection structure
  * @param  espconn_connect_callback connect_cb : registered callback function
  *
  * @return 0     : succeed
  * @return Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection according
  *                     to structure espconn
  */
sint8 espconn_regist_connectcb(struct espconn *espconn, espconn_connect_callback connect_cb);

/**
  * @brief  register data receive function which will be called back when data are received.
  *
  * @param  struct espconn *espconn : the network transmission structure
  * @param  espconn_recv_callback recv_cb : registered callback function
  *
  * @return 0     : succeed
  * @return Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection according
  *                     to structure espconn
  */
sint8 espconn_regist_recvcb(struct espconn *espconn, espconn_recv_callback recv_cb);

/**
  * @brief     Register reconnect callback.
  *
  * @attention espconn_reconnect_callback is more like a network-broken error handler; it handles
  *            errors that occurs in any phase of the connection.
  *            For instance, if espconn_send fails, espconn_reconnect_callback will be called
  *            because the network is broken.
  *
  * @param  struct espconn *espconn : the TCP connection structure
  * @param  espconn_reconnect_callback recon_cb : registered callback function
  *
  * @return  0     : succeed
  * @return  Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection according
  *                     to structure espconn
  */
sint8 espconn_regist_reconcb(struct espconn *espconn, espconn_reconnect_callback recon_cb);

/**
  * @brief  Register disconnection function which will be called back under successful TCP
  *         disconnection.
  *
  * @param  struct espconn *espconn : the TCP connection structure
  * @param  espconn_connect_callback discon_cb : registered callback function
  *
  * @return 0     : succeed
  * @return Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection according
  *                     to structure espconn
  */
sint8 espconn_regist_disconcb(struct espconn *espconn, espconn_connect_callback discon_cb);

/**
  * @brief  Get an available port for network.
  *
  * @param  null
  *
  * @return Port number.
  */
uint32 espconn_port(void);

/**
  * @brief     Set option of TCP connection.
  *
  * @attention In general, we need not call this API. If call espconn_set_opt, please call
  *            it in espconn_connect_callback.
  *
  * @param     struct espconn *espconn : the TCP connection structure
  * @param     uint8 opt : option of TCP connection, refer to enum espconn_option
  *    -  bit 0: 1: free memory after TCP disconnection happen need not wait 2 minutes;
  *    -  bit 1: 1: disable nagle algorithm during TCP data transmission, quiken the data transmission.
  *    -  bit 2: 1: enable espconn_regist_write_finish, enter write finish callback means
  *                 the data espconn_send sending was written into 2920 bytes write-buffer
  *                 waiting for sending or already sent.
  *    -  bit 3: 1: enable TCP keep alive
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection according
  *                     to structure espconn
  */
sint8 espconn_set_opt(struct espconn *espconn, uint8 opt);

/**
  * @brief  Clear option of TCP connection.
  *
  * @param  struct espconn *espconn : the TCP connection structure
  * @param  uint8 opt : enum espconn_option
  *
  * @return 0     : succeed
  * @return Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection according
  *                     to structure espconn
  */
sint8 espconn_clear_opt(struct espconn *espconn, uint8 opt);

/**
  * @brief     Set configuration of TCP keep alive.
  *
  * @attention In general, we need not call this API. If needed, please call it in
  *            espconn_connect_callback and call espconn_set_opt to enable keep alive first.
  *
  * @param     struct espconn *espconn : the TCP connection structure
  * @param     uint8 level : To do TCP keep-alive detection every ESPCONN_KEEPIDLE. If there
  *                          is no response, retry ESPCONN_KEEPCNT times every ESPCONN_KEEPINTVL.
  *                          If still no response, considers it as TCP connection broke, goes
  *                          into espconn_reconnect_callback. Notice, keep alive interval is not
  *                          precise, only for reference, it depends on priority.
  * @param     void* optarg : value of parameter
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection according
  *                     to structure espconn
  */
sint8 espconn_set_keepalive(struct espconn *espconn, uint8 level, void *optarg);

/**
  * @brief  Get configuration of TCP keep alive.
  *
  * @param  struct espconn *espconn : the TCP connection structure
  * @param  uint8 level : enum espconn_level
  * @param  void* optarg : value of parameter
  *
  * @return 0     : succeed
  * @return Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection according
  *                     to structure espconn
  */
sint8 espconn_get_keepalive(struct espconn *espconn, uint8 level, void *optarg);

/**
  * @brief  Callback which is invoked when a hostname is found.
  *
  * @param  const char *name : hostname
  * @param  ip_addr_t *ipaddr : IP address of the hostname, or to be NULL if the name could
  *                             not be found (or on any other error).
  * @param  void *callback_arg : callback argument.
  *
  * @return null
  */
typedef void (*dns_found_callback)(const char *name, ip_addr_t *ipaddr, void *callback_arg);

/**
  * @brief  DNS function.
  *
  *         Parse a hostname (string) to an IP address.
  *
  * @param  struct espconn *pespconn : espconn to parse a hostname.
  * @param  const char *hostname : the hostname.
  * @param  ip_addr_t *addr : IP address.
  * @param  dns_found_callback found : callback of DNS
  *
  * @return err_t :
  *    -  ESPCONN_OK - succeed
  *    -  ESPCONN_INPROGRESS - error code : already connected
  *    -  ESPCONN_ARG - error code : illegal argument, can't find network transmission
  *                     according to structure espconn
  */
err_t espconn_gethostbyname(struct espconn *pespconn, const char *hostname, ip_addr_t *addr, dns_found_callback found);

/**
  * @brief     Join a multicast group.
  *
  * @attention This API can only be called after the ESP8266 station connects to a router.
  *
  * @param     ip_addr_t *host_ip :  IP of UDP host
  * @param     ip_addr_t *multicast_ip :  IP of multicast group
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_MEM - Out of memory
  */
sint8 espconn_igmp_join(ip_addr_t *host_ip, ip_addr_t *multicast_ip);

/**
  * @brief     Leave a multicast group.
  *
  * @attention This API can only be called after the ESP8266 station connects to a router.
  *
  * @param     ip_addr_t *host_ip :  IP of UDP host
  * @param     ip_addr_t *multicast_ip :  IP of multicast group
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_MEM - Out of memory
  */
sint8 espconn_igmp_leave(ip_addr_t *host_ip, ip_addr_t *multicast_ip);

/**
  * @brief     Puts in a request to block the TCP receive function.
  *
  * @attention The function does not act immediately; we recommend calling it while
  *            reserving 5*1460 bytes of memory. This API can be called more than once.
  *
  * @param     struct espconn *espconn : corresponding TCP connection structure
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection
  *                     according to structure espconn.
  */
sint8 espconn_recv_hold(struct espconn *pespconn);

/**
  * @brief     Unblock TCP receiving data (i.e. undo espconn_recv_hold).
  *
  * @attention This API takes effect immediately.
  *
  * @param     struct espconn *espconn : corresponding TCP connection structure
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection
  *                     according to structure espconn.
  */
sint8 espconn_recv_unhold(struct espconn *pespconn);

/**
  * @brief     Set default DNS server. Two DNS server is allowed to be set.
  *
  * @attention Only if ESP8266 DHCP client is disabled (wifi_station_dhcpc_stop),
  *            this API can be used.
  *
  * @param     char numdns : DNS server ID, 0 or 1
  * @param     ip_addr_t *dnsserver : DNS server IP
  *
  * @return    null
  */
void espconn_dns_setserver(char numdns, ip_addr_t *dnsserver);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

/**
 * \file            lwesp_int.c
 * \brief           Internal functions
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
#include "lwesp/lwesp_private.h"
#include "lwesp/lwesp.h"
#include "lwesp/lwesp_int.h"
#include "lwesp/lwesp_mem.h"
#include "lwesp/lwesp_parser.h"
#include "lwesp/lwesp_unicode.h"
#include "system/lwesp_ll.h"

#if !__DOXYGEN__
/**
 * \brief           Receive character structure to handle full line terminated with `\n` character
 */
typedef struct {
    char data[128];                             /*!< Received characters */
    size_t len;                                 /*!< Length of valid characters */
} lwesp_recv_t;

/* Receive character macros */
#define RECV_ADD(ch)                        do { if (recv_buff.len < (sizeof(recv_buff.data)) - 1) { recv_buff.data[recv_buff.len++] = ch; recv_buff.data[recv_buff.len] = 0; } } while (0)
#define RECV_RESET()                        do { recv_buff.len = 0; recv_buff.data[0] = 0; } while (0)
#define RECV_LEN()                          ((size_t)recv_buff.len)
#define RECV_IDX(index)                     recv_buff.data[index]

/* Send data over AT port */
#define AT_PORT_SEND_STR(str)               esp.ll.send_fn((const void *)(str), (size_t)strlen(str))
#define AT_PORT_SEND_CONST_STR(str)         esp.ll.send_fn((const void *)(str), (size_t)(sizeof(str) - 1))
#define AT_PORT_SEND_CHR(str)               esp.ll.send_fn((const void *)(str), (size_t)1)
#define AT_PORT_SEND_FLUSH()                esp.ll.send_fn(NULL, 0)
#define AT_PORT_SEND(d, l)                  esp.ll.send_fn((const void *)(d), (size_t)(l))
#define AT_PORT_SEND_WITH_FLUSH(d, l)       do { AT_PORT_SEND((d), (l)); AT_PORT_SEND_FLUSH(); } while (0)

/* Beginning and end of every AT command */
#define AT_PORT_SEND_BEGIN_AT()             do { AT_PORT_SEND_CONST_STR("AT"); } while (0)
#define AT_PORT_SEND_END_AT()               do { AT_PORT_SEND(CRLF, CRLF_LEN); AT_PORT_SEND_FLUSH(); } while (0)

/* Send special characters over AT port with condition */
#define AT_PORT_SEND_QUOTE_COND(q)          do { if ((q)) { AT_PORT_SEND_CONST_STR("\""); } } while (0)
#define AT_PORT_SEND_COMMA_COND(c)          do { if ((c)) { AT_PORT_SEND_CONST_STR(","); } } while (0)
#define AT_PORT_SEND_EQUAL_COND(e)          do { if ((e)) { AT_PORT_SEND_CONST_STR("="); } } while (0)
#endif /* !__DOXYGEN__ */

static lwesp_recv_t recv_buff;
static lwespr_t lwespi_process_sub_cmd(lwesp_msg_t* msg, uint8_t* is_ok, uint8_t* is_error, uint8_t* is_ready);

/**
 * \brief           Free connection send data memory
 * \param[in]       m: Send data message type
 */
#define CONN_SEND_DATA_FREE(m)      do {            \
        if ((m) != NULL && (m)->msg.conn_send.fau) {    \
            (m)->msg.conn_send.fau = 0;                 \
            if ((m)->msg.conn_send.data != NULL) {      \
                LWESP_DEBUGF(LWESP_CFG_DBG_CONN | LWESP_DBG_TYPE_TRACE,   \
                             "[LWESP CONN] Free write buffer fau: %p\r\n", (void *)(m)->msg.conn_send.data);   \
                lwesp_mem_free_s((void **)&((m)->msg.conn_send.data));\
            }                                           \
        }                                               \
    } while (0)

/**
 * \brief           Send connection callback for "data send"
 * \param[in]       m: Connection send message
 * \param[in]       err: Error of type \ref lwespr_t
 */
#define CONN_SEND_DATA_SEND_EVT(m, err)  do {       \
        CONN_SEND_DATA_FREE(m);                         \
        esp.evt.type = LWESP_EVT_CONN_SEND;               \
        esp.evt.evt.conn_data_send.res = err;           \
        esp.evt.evt.conn_data_send.conn = (m)->msg.conn_send.conn;  \
        esp.evt.evt.conn_data_send.sent = (m)->msg.conn_send.sent_all;   \
        lwespi_send_conn_cb((m)->msg.conn_send.conn, NULL);   \
    } while (0)

/**
 * \brief           Send reset sequence event
 * \param[in]       m: Command message
 * \param[in]       err: Error of type \ref lwespr_t
 */
#define RESET_SEND_EVT(m, err)  do {                \
        esp.evt.evt.reset.res = err;                    \
        lwespi_send_cb(LWESP_EVT_RESET);                    \
    } while (0)

/**
 * \brief           Send restore sequence event
 * \param[in]       m: Command message
 * \param[in]       err: Error of type \ref lwespr_t
 */
#define RESTORE_SEND_EVT(m, err)  do {              \
        esp.evt.evt.restore.res = err;                  \
        lwespi_send_cb(LWESP_EVT_RESTORE);                  \
    } while (0)

/**
* \brief           Send ping event to user
* \param[in]       m: Command message
* \param[in]       err: Error of type \ref lwespr_t
*/
#define PING_SEND_EVT(m, err)   do {                \
        esp.evt.evt.ping.res = err;  \
        esp.evt.evt.ping.host = (m)->msg.tcpip_ping.host;   \
        esp.evt.evt.ping.time = (m)->msg.tcpip_ping.time;   \
        lwespi_send_cb(LWESP_EVT_PING);                     \
    } while (0)

/**
 * \brief           Send cipdomain (DNS function) event to user
 * \param[in]       m: Command message
 * \param[in]       err: Error of type \ref lwespr_t
 */
#define CIPDOMAIN_SEND_EVT(m, err)   do {           \
        esp.evt.evt.dns_hostbyname.res = err;           \
        esp.evt.evt.dns_hostbyname.host = msg->msg.dns_getbyhostname.host;  \
        esp.evt.evt.dns_hostbyname.ip = msg->msg.dns_getbyhostname.ip;  \
        lwespi_send_cb(LWESP_EVT_DNS_HOSTBYNAME);           \
    } while (0)

/**
 * \brief           Send join AP event to user
 * \param[in]       m: Command message
 * \param[in]       err: Error of type \ref lwespr_t
 */
#define STA_JOIN_AP_SEND_EVT(m, err) do {           \
        esp.evt.evt.sta_join_ap.res = err;              \
        lwespi_send_cb(LWESP_EVT_STA_JOIN_AP);              \
    } while (0)

/**
 * \brief           Send SNTP time event to user
 * \param[in]       m: Command message
 * \param[in]       err: Error of type \ref lwespr_t
 */
#define SNTP_TIME_SEND_EVT(m, err) do {           \
        esp.evt.evt.cip_sntp_time.res = err;              \
        esp.evt.evt.cip_sntp_time.dt = (m)->msg.tcpip_sntp_time.dt;   \
        lwespi_send_cb(LWESP_EVT_SNTP_TIME);              \
    } while (0)

/**
 * \brief           Send list AP event to user
 * \param[in]       m: Command message
 * \param[in]       err: Error of type \ref lwespr_t
 */
#define STA_LIST_AP_SEND_EVT(m, err) do {           \
        esp.evt.evt.sta_list_ap.res = err;              \
        esp.evt.evt.sta_list_ap.aps = msg->msg.ap_list.aps; \
        esp.evt.evt.sta_list_ap.len = msg->msg.ap_list.apsi;\
        lwespi_send_cb(LWESP_EVT_STA_LIST_AP);              \
    } while (0)

/**
 * \brief           Send info AP event to user
 * \param[in]       m: Command message
 * \param[in]       err: Error of type \ref lwespr_t
 */
#define STA_INFO_AP_SEND_EVT(m, err) do {           \
        esp.evt.evt.sta_info_ap.res = err;              \
        esp.evt.evt.sta_info_ap.info = esp.msg->msg.sta_info_ap.info;   \
        lwespi_send_cb(LWESP_EVT_STA_INFO_AP);              \
    } while (0)

/**
 * \brief           Get command name based on used Espressif device,
 *                  used to obtain current connection status information
 * \return          Cip status or state command type
 */
lwesp_cmd_t
lwespi_get_cipstatus_or_cipstate_cmd(void) {
    /*
     * With the current minimum AT versions,
     * ESP8266 and ESP32 both require to use AT+CIPSTATUS,
     * to get connection status.
     *
     * All other new devices utilize new command AT+CIPSTATE
     */
    if (0
#if LWESP_CFG_ESP8266
        || esp.m.device == LWESP_DEVICE_ESP8266
#endif /* LWESP_CFG_ESP8266 */
#if LWESP_CFG_ESP32
        || esp.m.device == LWESP_DEVICE_ESP32
#endif /* LWESP_CFG_ESP32 */
       ) {
        return LWESP_CMD_TCPIP_CIPSTATUS;
    }
    return LWESP_CMD_TCPIP_CIPSTATE;
}

/**
 * \brief           Send IP address to AT port
 * \param[in]       ip: Pointer to IP address
 * \param[in]       q: Set to `1` to include start and ending quotes
 * \param[in]       c: Set to `1` to include comma before string
 */
void
lwespi_send_ip(const lwesp_ip_t* ip, uint8_t q, uint8_t c) {
    uint8_t ch = '.', len = 4;
    char str[LWESP_CFG_IPV6 ? 5 : 4];

    AT_PORT_SEND_COMMA_COND(c);                 /* Send comma */
    if (ip == NULL) {
        return;
    }
    AT_PORT_SEND_QUOTE_COND(q);                 /* Send quote */
#if LWESP_CFG_IPV6
    if (ip->type == LWESP_IPTYPE_V6) {
        ch = ':';
        len = 8;
    }
#endif /* LWESP_CFG_IPV6 */
    for (uint8_t i = 0; i < len; ++i) {         /* Process all elements */
        if (0) {
#if LWESP_CFG_IPV6
        } else if (ip->type == LWESP_IPTYPE_V6) {
            /* Format IPV6 */
            lwesp_u32_to_gen_str(LWESP_U32(ip->addr.ip6.addr[i]), str, 1, 4);
#endif /* LWESP_CFG_IPV6 */
        } else {
            /* Format IPV4 */
            lwesp_u8_to_str(ip->addr.ip4.addr[i], str);
        }
        AT_PORT_SEND_STR(str);                  /* Send str */
        if (i < (len - 1)) {                    /* Check end if characters */
            AT_PORT_SEND_CHR(&ch);              /* Send character */
        }
    }
    AT_PORT_SEND_QUOTE_COND(q);                 /* Send quote */
}

/**
 * \brief           Send MAC address to AT port
 * \param[in]       mac: Pointer to MAC address
 * \param[in]       q: Set to `1` to include start and ending quotes
 * \param[in]       c: Set to `1` to include comma before string
 */
void
lwespi_send_mac(const lwesp_mac_t* mac, uint8_t q, uint8_t c) {
    const uint8_t ch = ':';
    char str[3];

    AT_PORT_SEND_COMMA_COND(c);                 /* Send comma */
    if (mac == NULL) {
        return;
    }
    AT_PORT_SEND_QUOTE_COND(q);                 /* Send quote */
    for (uint8_t i = 0; i < 6; ++i) {           /* Process all elements */
        lwesp_u8_to_hex_str(mac->mac[i], str, 2);   /* ... go to HEX format */
        AT_PORT_SEND_STR(str);                  /* Send str */
        if (i < (6 - 1)) {                      /* Check end if characters */
            AT_PORT_SEND_CHR(&ch);              /* Send character */
        }
    }
    AT_PORT_SEND_QUOTE_COND(q);                 /* Send quote */
}

/**
 * \brief           Send string to AT port, either plain or escaped
 * \param[in]       str: Pointer to input string to string
 * \param[in]       e: Value to indicate string send format, escaped (`1`) or plain (`0`)
 * \param[in]       q: Value to indicate starting and ending quotes, enabled (`1`) or disabled (`0`)
 * \param[in]       c: Set to `1` to include comma before string
 */
void
lwespi_send_string(const char* str, uint8_t e, uint8_t q, uint8_t c) {
    char special = '\\';

    AT_PORT_SEND_COMMA_COND(c);                 /* Send comma */
    AT_PORT_SEND_QUOTE_COND(q);                 /* Send quote */
    if (str != NULL) {
        if (e) {                                /* Do we have to escape string? */
            while (*str) {                      /* Go through string */
                if (*str == ',' || *str == '"' || *str == '\\') {   /* Check for special character */
                    AT_PORT_SEND_CHR(&special); /* Send special character */
                }
                AT_PORT_SEND_CHR(str);          /* Send character */
                ++str;
            }
        } else {
            AT_PORT_SEND_STR(str);              /* Send plain string */
        }
    }
    AT_PORT_SEND_QUOTE_COND(q);                 /* Send quote */
}

/**
 * \brief           Send number (decimal) to AT port
 * \param[in]       num: Number to send to AT port
 * \param[in]       q: Value to indicate starting and ending quotes, enabled (`1`) or disabled (`0`)
 * \param[in]       c: Set to `1` to include comma before string
 */
void
lwespi_send_number(uint32_t num, uint8_t q, uint8_t c) {
    char str[11];

    lwesp_u32_to_str(num, str);                 /* Convert digit to decimal string */

    AT_PORT_SEND_COMMA_COND(c);                 /* Send comma */
    AT_PORT_SEND_QUOTE_COND(q);                 /* Send quote */
    AT_PORT_SEND_STR(str);                      /* Send string with number */
    AT_PORT_SEND_QUOTE_COND(q);                 /* Send quote */
}

/**
 * \brief           Send port number to AT port
 * \param[in]       port: Port number to send
 * \param[in]       q: Value to indicate starting and ending quotes, enabled (`1`) or disabled (`0`)
 * \param[in]       c: Set to `1` to include comma before string
 */
void
lwespi_send_port(lwesp_port_t port, uint8_t q, uint8_t c) {
    char str[6];

    lwesp_u16_to_str(LWESP_PORT2NUM(port), str);/* Convert digit to decimal string */

    AT_PORT_SEND_COMMA_COND(c);                 /* Send comma */
    AT_PORT_SEND_QUOTE_COND(q);                 /* Send quote */
    AT_PORT_SEND_STR(str);                      /* Send string with number */
    AT_PORT_SEND_QUOTE_COND(q);                 /* Send quote */
}

/**
 * \brief           Send signed number to AT port
 * \param[in]       num: Number to send to AT port
 * \param[in]       q: Value to indicate starting and ending quotes, enabled (`1`) or disabled (`0`)
 * \param[in]       c: Set to `1` to include comma before string
 */
void
lwespi_send_signed_number(int32_t num, uint8_t q, uint8_t c) {
    char str[11];

    lwesp_i32_to_str(num, str);                 /* Convert digit to decimal string */

    AT_PORT_SEND_COMMA_COND(c);                 /* Send comma */
    AT_PORT_SEND_QUOTE_COND(q);                 /* Send quote */
    AT_PORT_SEND_STR(str);                      /* Send string with number */
    AT_PORT_SEND_QUOTE_COND(q);                 /* Send quote */
}

/**
 * \brief           Reset all connections
 * \note            Used to notify upper layer stack to close everything and reset the memory if necessary
 * \param[in]       forced: Flag indicating reset was forced by command
 */
static void
reset_connections(uint8_t forced) {
    esp.evt.type = LWESP_EVT_CONN_CLOSE;
    esp.evt.evt.conn_active_close.forced = forced;
    esp.evt.evt.conn_active_close.res = lwespOK;

    for (size_t i = 0; i < LWESP_CFG_MAX_CONNS; ++i) {  /* Check all connections */
        if (esp.m.conns[i].status.f.active) {
            esp.m.conns[i].status.f.active = 0;

            esp.evt.evt.conn_active_close.conn = &esp.m.conns[i];
            esp.evt.evt.conn_active_close.client = esp.m.conns[i].status.f.client;
            lwespi_send_conn_cb(&esp.m.conns[i], NULL); /* Send callback function */
        }
    }
}

/**
 * \brief           Reset everything after reset was detected
 * \param[in]       forced: Set to `1` if reset forced by user
 */
void
lwespi_reset_everything(uint8_t forced) {
    /**
     * \todo: Put stack to default state:
     *          - Close all the connections in memory
     *          - Clear entire data memory
     *          - Reset esp structure with IP
     *          - Start over init procedure
     */

    /* Step 1: Close all connections in memory */
    reset_connections(forced);

#if LWESP_CFG_MODE_STATION
    LWESP_RESET_STA_HAS_IP();
    if (esp.m.sta.f.is_connected) {
        lwespi_send_cb(LWESP_EVT_WIFI_DISCONNECTED);
    }
    esp.m.sta.f.is_connected = 0;
#endif /* LWESP_CFG_MODE_STATION */

    /* Check if IPD active */
    if (esp.m.ipd.buff != NULL) {
        lwesp_pbuf_free(esp.m.ipd.buff);
        esp.m.ipd.buff = NULL;
    }

    /* Invalid ESP modules */
    LWESP_MEMSET(&esp.m, 0x00, sizeof(esp.m));

    /* Set default device */
    esp.m.device = LWESP_DEVICE_UNKNOWN;

    /* Reset baudrate to default */
    esp.ll.uart.baudrate = LWESP_CFG_AT_PORT_BAUDRATE;
    lwesp_ll_init(&esp.ll);

    /* If reset was not forced by user, repeat with manual reset */
    if (!forced) {
        lwesp_reset(NULL, NULL, 0);
    }
}

/**
 * \brief           Process callback function to user with specific type
 * \param[in]       type: Callback event type
 * \return          Member of \ref lwespr_t enumeration
 */
lwespr_t
lwespi_send_cb(lwesp_evt_type_t type) {
    esp.evt.type = type;                        /* Set callback type to process */

    /* Call callback function for all registered functions */
    for (lwesp_evt_func_t* link = esp.evt_func; link != NULL; link = link->next) {
        link->fn(&esp.evt);
    }
    return lwespOK;
}

/**
 * \brief           Process connection callback
 * \note            Before calling function, callback structure must be prepared
 * \param[in]       conn: Pointer to connection to use as callback
 * \param[in]       evt: Event callback function for connection
 * \return          Member of \ref lwespr_t enumeration
 */
lwespr_t
lwespi_send_conn_cb(lwesp_conn_t* conn, lwesp_evt_fn evt) {
    if (conn->status.f.in_closing && esp.evt.type != LWESP_EVT_CONN_CLOSE) {/* Do not continue if in closing mode */
        /* return lwespOK; */
    }

    if (evt != NULL) {                          /* Try with user connection */
        return evt(&esp.evt);                   /* Call temporary function */
    } else if (conn != NULL && conn->evt_func != NULL) {/* Connection custom callback? */
        return conn->evt_func(&esp.evt);        /* Process callback function */
    } else if (conn == NULL) {
        return lwespOK;
    }

    /*
     * On normal API operation,
     * we should never enter to this part of code
     */

    /*
     * If connection doesn't have callback function
     * automatically close the connection?
     *
     * Since function call is non-blocking,
     * it will set active connection to closing mode
     * and further callback events should not be executed anymore
     */
    return lwesp_conn_close(conn, 0);
}

/**
 * \brief           Process and send data from device buffer
 * \return          Member of \ref lwespr_t enumeration
 */
static lwespr_t
lwespi_tcpip_process_send_data(void) {
    lwesp_conn_t* c = esp.msg->msg.conn_send.conn;
    if (!lwesp_conn_is_active(c) ||             /* Is the connection already closed? */
        esp.msg->msg.conn_send.val_id != c->val_id  /* Did validation ID change after we set parameter? */
       ) {
        /* Send event to user about failed send event */
        CONN_SEND_DATA_SEND_EVT(esp.msg, lwespCLOSED);
        return lwespERR;
    }

    /*
     * Get maximal length of data to transmit in single run
     *
     * For UDP packets, fragmentation may not be allowed.
     * Check for UDP is done before starting command to send data, in LWESP_CONN module
     */
    esp.msg->msg.conn_send.sent = LWESP_MIN(esp.msg->msg.conn_send.btw, LWESP_CFG_CONN_MAX_DATA_LEN);

    AT_PORT_SEND_BEGIN_AT();
    AT_PORT_SEND_CONST_STR("+CIPSEND=");
    lwespi_send_number(LWESP_U32(c->num), 0, 0);/* Send connection number */
    lwespi_send_number(LWESP_U32(esp.msg->msg.conn_send.sent), 0, 1);   /* Send length number */

    /* On UDP connections, IP address and port may be included */
    if (c->type == LWESP_CONN_TYPE_UDP
        && esp.msg->msg.conn_send.remote_ip != NULL && esp.msg->msg.conn_send.remote_port) {
        lwespi_send_ip(esp.msg->msg.conn_send.remote_ip, 1, 1); /* Send IP address including quotes */
        lwespi_send_port(esp.msg->msg.conn_send.remote_port, 0, 1); /* Send length number */
    }
    AT_PORT_SEND_END_AT();
    return lwespOK;
}

/**
 * \brief           Process data sent and send remaining
 * \param[in]       sent: Status whether data were sent or not,
 *                      info received from ESP with `SEND OK` or `SEND FAIL`
 * \return          `1` in case we should stop sending or `0` if we still have data to process
 */
static uint8_t
lwespi_tcpip_process_data_sent(uint8_t sent) {
    if (sent) {                                 /* Data were successfully sent */
        esp.msg->msg.conn_send.sent_all += esp.msg->msg.conn_send.sent;
        esp.msg->msg.conn_send.btw -= esp.msg->msg.conn_send.sent;
        esp.msg->msg.conn_send.ptr += esp.msg->msg.conn_send.sent;
        if (esp.msg->msg.conn_send.bw != NULL) {
            *esp.msg->msg.conn_send.bw += esp.msg->msg.conn_send.sent;
        }
        esp.msg->msg.conn_send.tries = 0;
    } else {                                    /* We were not successful */
        ++esp.msg->msg.conn_send.tries;         /* Increase number of tries */
        if (esp.msg->msg.conn_send.tries == LWESP_CFG_MAX_SEND_RETRIES) {   /* In case we reached max number of retransmissions */
            return 1;                           /* Return 1 and indicate error */
        }
    }
    if (esp.msg->msg.conn_send.btw > 0) {       /* Do we still have data to send? */
        if (lwespi_tcpip_process_send_data() != lwespOK) {  /* Check if we can continue */
            return 1;                           /* Finish at this point */
        }
        return 0;                               /* We still have data to send */
    }
    return 1;                                   /* Everything was sent, we can stop execution */
}

/**
 * \brief           Send error event to application layer
 * \param[in]       msg: Message from user with connection start
 * \param[in]       error: Value indicating cause of error
 */
static void
lwespi_send_conn_error_cb(lwesp_msg_t* msg, lwespr_t error) {
    esp.evt.type = LWESP_EVT_CONN_ERROR;        /* Connection error */
    esp.evt.evt.conn_error.host = esp.msg->msg.conn_start.remote_host;
    esp.evt.evt.conn_error.port = esp.msg->msg.conn_start.remote_port;
    esp.evt.evt.conn_error.type = esp.msg->msg.conn_start.type;
    esp.evt.evt.conn_error.arg = esp.msg->msg.conn_start.arg;
    esp.evt.evt.conn_error.err = error;

    /* Call callback specified by user on connection startup */
    esp.msg->msg.conn_start.evt_func(&esp.evt);
    LWESP_UNUSED(msg);
}

/**
 * \brief           Process received string from ESP
 * \param[in]       rcv: Pointer to \ref lwesp_recv_t structure with input string
 */
static void
lwespi_parse_received(lwesp_recv_t* rcv) {
    uint8_t is_ok = 0, is_error = 0, is_ready = 0;
    const char* s;
    LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
                     "recv at:%s\r\n", rcv->data);

    /* Try to remove non-parsable strings */
    if ((rcv->len == 2 && rcv->data[0] == '\r' && rcv->data[1] == '\n')
        /*
         * Condition below can only be used if AT echo is disabled
         * otherwise it may happen that message is inserted in between AT command echo, such as:
         *
         * AT+CIPCLOSE=0+LINK_CONN:0,2,"TCP",1,"192.168.0.14",57551,80\r\n\r\n
         *
         * Instead of:
         * AT+CIPCLOSE=0\r\n
         * +LINK_CONN:0,2,"TCP",1,"192.168.0.14",57551,80\r\n
         */
        /* || (rcv->len > 3 && rcv->data[0] == 'A' && rcv->data[1] == 'T' && rcv->data[2] == '+') */) {
        return;
    }

    /* Detect most common responses from device */
    is_ok = !strcmp(rcv->data, "OK" CRLF);      /* Check if received string is OK */
    if (!is_ok) {
        is_error = !strcmp(rcv->data, "ERROR" CRLF) || !strcmp(rcv->data, "FAIL" CRLF); /* Check if received string is error */
        if (!is_error) {
            is_ready = !strcmp(rcv->data, "ready" CRLF);/* Check if received string is ready */
        }
    }

    /*
     * In case ready was received, there was a reset on device,
     * either forced by command or problem on device itself
     */
    if (is_ready) {
        if (CMD_IS_CUR(LWESP_CMD_RESET) || CMD_IS_CUR(LWESP_CMD_RESTORE)) { /* Did we force reset? */
            esp.evt.evt.reset_detected.forced = 1;
        } else {                                /* Reset due unknown error */
            esp.evt.evt.reset_detected.forced = 0;
            if (esp.msg != NULL) {
                is_ok = 0;
                is_error = 1;
                is_ready = 0;
            }
        }
        lwespi_reset_everything(esp.evt.evt.reset_detected.forced); /* Put everything to default state */
        lwespi_send_cb(LWESP_EVT_RESET_DETECTED);   /* Call user callback function */
    }

    /* Read and process statements starting with '+' character */
    if (rcv->data[0] == '+') {
        if (!strncmp("+IPD", rcv->data, 4)) {   /* Check received network data */
            lwespi_parse_ipd(rcv->data);        /* Parse IPD statement and start receiving network data */
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
            if (CMD_IS_DEF(LWESP_CMD_TCPIP_CIPRECVDATA) && CMD_IS_CUR(LWESP_CMD_TCPIP_CIPRECVLEN)) {
                esp.msg->msg.ciprecvdata.ipd_recv = 1;  /* Command repeat, try again later */
            }
            /* IPD message notification? */
            lwespi_conn_manual_tcp_try_read_data(esp.m.ipd.conn);
        } else if (!strncmp("+CIPRECVDATA", rcv->data, 12)) {
            lwespi_parse_ciprecvdata(rcv->data);/* Parse CIPRECVDATA statement and start receiving network data */
        } else if (!strncmp("+CIPRECVLEN", rcv->data, 11)) {
            lwespi_parse_ciprecvlen(rcv->data); /* Parse CIPRECVLEN statement */
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */
#if LWESP_CFG_MODE_ACCESS_POINT
        } else if (!strncmp(rcv->data, "+STA_CONNECTED", 14)) {
            lwespi_parse_ap_conn_disconn_sta(&rcv->data[15], 1);/* Parse string and send to user layer */
        } else if (!strncmp(rcv->data, "+STA_DISCONNECTED", 17)) {
            lwespi_parse_ap_conn_disconn_sta(&rcv->data[18], 0);/* Parse string and send to user layer */
        } else if (!strncmp(rcv->data, "+DIST_STA_IP", 12)) {
            lwespi_parse_ap_ip_sta(&rcv->data[13]); /* Parse string and send to user layer */
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
#if LWESP_CFG_WEBSERVER
        } else if (!strncmp(rcv->data, "+WEBSERVERRSP", 13)) {
            lwespi_parse_webserver(&rcv->data[14]);/* Parse string and send to user layer */
#endif /* LWESP_CFG_WEBSERVER */
        } else if (esp.msg != NULL) {
            if (0
#if LWESP_CFG_MODE_STATION
                || (CMD_IS_CUR(LWESP_CMD_WIFI_CIPSTAMAC_GET) && !strncmp(rcv->data, "+CIPSTAMAC", 10))
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
                || (CMD_IS_CUR(LWESP_CMD_WIFI_CIPAPMAC_GET) && !strncmp(rcv->data, "+CIPAPMAC", 9))
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
               ) {
                const char* tmp;
                lwesp_mac_t mac;

                if (rcv->data[9] == ':') {
                    tmp = &rcv->data[10];
                } else if (rcv->data[10] == ':') {
                    tmp = &rcv->data[11];
                }

                lwespi_parse_mac(&tmp, &mac);   /* Save as current MAC address */
#if LWESP_CFG_MODE_STATION
                if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPSTAMAC_GET)) {
                    LWESP_MEMCPY(&esp.m.sta.mac, &mac, 6);  /* Copy to current setup */
                }
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
                if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPAPMAC_GET)) {
                    LWESP_MEMCPY(&esp.m.ap.mac, &mac, 6);   /* Copy to current setup */
                }
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
                if (esp.msg->msg.sta_ap_getmac.mac != NULL && CMD_IS_CUR(CMD_GET_DEF())) {
                    LWESP_MEMCPY(esp.msg->msg.sta_ap_getmac.mac, &mac, sizeof(mac));/* Copy to current setup */
                }
            } else if (0
#if LWESP_CFG_MODE_STATION
                       || (CMD_IS_CUR(LWESP_CMD_WIFI_CIPSTA_GET) && !strncmp(rcv->data, "+CIPSTA", 7))
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
                       || (CMD_IS_CUR(LWESP_CMD_WIFI_CIPAP_GET) && !strncmp(rcv->data, "+CIPAP", 6))
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
                      ) {
                const char* tmp = NULL, *ch_p2;
                lwesp_ip_t ip, *a = NULL, *b = NULL;
                lwesp_ip_mac_t* im;
                uint8_t ch = 0;

#if LWESP_CFG_MODE_STATION
                if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPSTA_GET)) {
                    im = &esp.m.sta;            /* Get IP and MAC structure first */
                }
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
                if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPAP_GET)) {
                    im = &esp.m.ap;             /* Get IP and MAC structure first */
                }
#endif /* LWESP_CFG_MODE_ACCESS_POINT */

                if (im != NULL) {
                    /* We expect "+CIPSTA:" or "+CIPAP:" ... */
                    if (rcv->data[6] == ':') {
                        ch = rcv->data[7];
                        ch_p2 = &rcv->data[9];
                    } else if (rcv->data[7] == ':') {
                        ch = rcv->data[8];
                        ch_p2 = &rcv->data[10];
                    }
                    switch (ch) {
                        case 'i':
                            if (0) {
#if LWESP_CFG_IPV6
                                /* Check local IPv6 address */
                            } else if (*ch_p2 == '6' && *(ch_p2 + 1) == 'l') {
                                tmp = &rcv->data[13];
                                a = &im->ip6_ll;
                                b = esp.msg->msg.sta_ap_getip.ip6_ll;
                                /* Check global IPv6 address */
                            } else if (*ch_p2 == '6' && *(ch_p2 + 1) == 'g') {
                                tmp = &rcv->data[13];
                                a = &im->ip6_gl;
                                b = esp.msg->msg.sta_ap_getip.ip6_gl;
#endif /* LWESP_CFG_IPV6 */
                            } else {
                                LWESP_UNUSED(ch_p2);
                                tmp = &rcv->data[10];
                                a = &im->ip;
                                b = esp.msg->msg.sta_ap_getip.ip;
                            }
                            break;
                        case 'g':
                            tmp = &rcv->data[15];
                            a = &im->gw;
                            b = esp.msg->msg.sta_ap_getip.gw;
                            break;
                        case 'n':
                            tmp = &rcv->data[15];
                            a = &im->nm;
                            b = esp.msg->msg.sta_ap_getip.nm;
                            break;
                        default:
                            tmp = NULL;
                            a = NULL;
                            b = NULL;
                            break;
                    }
                    if (tmp != NULL) {          /* Do we have temporary string? */
                        if (*tmp == ':' || *tmp == ',') {
                            ++tmp;
                        }
                        lwespi_parse_ip(&tmp, &ip); /* Parse IP address */
                        LWESP_MEMCPY(a, &ip, sizeof(ip));   /* Copy to current setup */
                        if (b != NULL && CMD_IS_CUR(CMD_GET_DEF())) {   /* Is current command the same as default one? */
                            LWESP_MEMCPY(b, &ip, sizeof(ip));   /* Copy to user variable */
                        }
                    }
                }
#if LWESP_CFG_MODE_STATION
            } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CWLAP) && !strncmp(rcv->data, "+CWLAP", 6)) {
                lwespi_parse_cwlap(rcv->data, esp.msg); /* Parse CWLAP entry */
            } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CWJAP) && !strncmp(rcv->data, "+CWJAP", 6)) {
                const char* tmp = &rcv->data[7];/* Go to the number position */
                esp.msg->msg.sta_join.error_num = (uint8_t)lwespi_parse_number(&tmp);
            } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CWJAP_GET) && !strncmp(rcv->data, "+CWJAP", 6)) {
                lwespi_parse_cwjap(rcv->data, esp.msg); /* Parse CWJAP */
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
            } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CWLIF) && !strncmp(rcv->data, "+CWLIF", 6)) {
                lwespi_parse_cwlif(rcv->data, esp.msg); /* Parse CWLIF entry */
            } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CWSAP_GET) && !strncmp(rcv->data, "+CWSAP", 6)) {
                lwespi_parse_cwsap(rcv->data, esp.msg);
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
#if LWESP_CFG_DNS
            } else if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPDOMAIN) && !strncmp(rcv->data, "+CIPDOMAIN", 10)) {
                lwespi_parse_cipdomain(rcv->data, esp.msg); /* Parse CIPDOMAIN entry */
            } else if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPDNS_GET) && !strncmp(rcv->data, "+CIPDNS", 7)) {
                const char* tmp = &rcv->data[8];/* Go to the ip position */
                lwesp_ip_t ip;
                uint8_t index = lwespi_parse_number(&tmp);
                esp.msg->msg.dns_getconf.dnsi = index;
                lwespi_parse_ip(&tmp, &ip);     /* Parse DNS address */
                if (esp.msg->msg.dns_getconf.s1 != NULL) {
                    *esp.msg->msg.dns_getconf.s1 = ip;
                }
                if (esp.msg->msg.dns_getconf.s2 != NULL && lwespi_parse_ip(&tmp, &ip)) {
                    *esp.msg->msg.dns_getconf.s2 = ip;
                }
#endif /* LWESP_CFG_DNS */
#if LWESP_CFG_PING
            } else if (CMD_IS_CUR(LWESP_CMD_TCPIP_PING) && !strncmp(rcv->data, "+PING", 5)) {
                lwespi_parse_ping_time(rcv->data, esp.msg); /* Parse ping time */
#endif /* LWESP_CFG_PING */
#if LWESP_CFG_SNTP
            } else if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSNTPTIME) && !strncmp(rcv->data, "+CIPSNTPTIME", 12)) {
                lwespi_parse_cipsntptime(rcv->data, esp.msg);   /* Parse CIPSNTPTIME entry */
#endif /* LWESP_CFG_SNTP */
#if LWESP_CFG_HOSTNAME
            } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CWHOSTNAME_GET) && !strncmp(rcv->data, "+CWHOSTNAME", 11)) {
                lwespi_parse_hostname(rcv->data, esp.msg);  /* Parse HOSTNAME entry */
#endif /* LWESP_CFG_HOSTNAME */
            } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CWDHCP_GET) && !strncmp(rcv->data, "+CWDHCP", 7)) {
                lwespi_parse_cwdhcp(rcv->data); /* Parse CWDHCP state */
            } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CWMODE_GET) && !strncmp(rcv->data, "+CWMODE", 7)) {
                const char* tmp = &rcv->data[8];/* Go to the number position */
                *esp.msg->msg.wifi_mode.mode_get = (uint8_t)lwespi_parse_number(&tmp);
            }
        }
#if LWESP_CFG_MODE_STATION
    } else if (strlen(rcv->data) > 4 && !strncmp(rcv->data, "WIFI", 4)) {
        if (!strncmp(&rcv->data[5], "CONNECTED", 9)) {
            esp.m.sta.f.is_connected = 1;       /* Wifi is connected */
            lwespi_send_cb(LWESP_EVT_WIFI_CONNECTED);   /* Call user callback function */
            if (!CMD_IS_CUR(LWESP_CMD_WIFI_CWJAP)) {/* In case of auto connection */
                lwesp_sta_getip(NULL, NULL, NULL, NULL, NULL, 0);   /* Get new IP address */
            }
        } else if (!strncmp(&rcv->data[5], "DISCONNECT", 10)) {
            esp.m.sta.f.is_connected = 0;       /* Wifi is disconnected */
            LWESP_RESET_STA_HAS_IP();           /* There is no valid IP */
            lwespi_send_cb(LWESP_EVT_WIFI_DISCONNECTED);/* Call user callback function */
        } else if (!strncmp(&rcv->data[5], "GOT IP", 6)) {
            if (0) {
#if LWESP_CFG_IPV6
                /* Check if IPv6 IP received */
            } else if (!strncmp(&rcv->data[11], "v6 LL", 5)) {
                esp.m.sta.f.has_ipv6_ll = 1;
            } else if (!strncmp(&rcv->data[11], "v6 GL", 5)) {
                esp.m.sta.f.has_ipv6_gl = 0;
#endif /* LWESP_CFG_IPV6 */
            } else {
                /* IP is for V4 (\todo: Add specific status) */
            }
            esp.m.sta.f.has_ip = 1;             /* Wifi got IP address */
            lwespi_send_cb(LWESP_EVT_WIFI_GOT_IP);  /* Call user callback function */
            if (!CMD_IS_CUR(LWESP_CMD_WIFI_CWJAP)) {/* In case of auto connection */
                lwesp_sta_getip(NULL, NULL, NULL, NULL, NULL, 0);   /* Get new IP address */
            }
        }
    } else if (CMD_IS_CUR(LWESP_CMD_GMR)) {
        if (!strncmp(rcv->data, "AT version", 10)) {
            uint8_t ok = 1, major = 99, minor = 99, patch = 99;
            lwespi_parse_at_sdk_version(&rcv->data[11], &esp.m.version_at);

            /*
             * AT version example string looks like (AT+GMR):
             *
             * ESP8266: "AT version:2.2.1.0(46d6c26 - ESP8266 - Aug  6 2021 06:50:15)"
             * ESP32C3: "AT version:2.3.0.0(e98993f - ESP32C3 - Dec 23 2021 09:03:35)""
             * ESP32: "AT version:2.2.0.0(c6fa6bf - ESP32 - Jul  2 2021 06:44:05)
             */
            if (0) {
#if LWESP_CFG_ESP8266
            } else if (strstr(rcv->data, "- ESP8266 -") != NULL) {
                esp.m.device = LWESP_DEVICE_ESP8266;
                major = LWESP_MIN_AT_VERSION_MAJOR_ESP8266;
                minor = LWESP_MIN_AT_VERSION_MINOR_ESP8266;
                patch = LWESP_MIN_AT_VERSION_PATCH_ESP8266;
                LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_TYPE_TRACE,
                             "[LWESP GMR] Detected Espressif device is %s\r\n", "ESP8266");
#endif /* LWESP_CFG_ESP8266 */
#if LWESP_CFG_ESP32
            } else if (strstr(rcv->data, "- ESP32 -") != NULL) {
                esp.m.device = LWESP_DEVICE_ESP32;
                major = LWESP_MIN_AT_VERSION_MAJOR_ESP32;
                minor = LWESP_MIN_AT_VERSION_MINOR_ESP32;
                patch = LWESP_MIN_AT_VERSION_PATCH_ESP32;
                LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_TYPE_TRACE,
                             "[LWESP GMR] Detected Espressif device is %s\r\n", "ESP32");
#endif /* LWESP_CFG_ESP32 */
#if LWESP_CFG_ESP32_C3
            } else if (strstr(rcv->data, "- ESP32C3 -") != NULL || strstr(rcv->data, "- ESP32-C3 -") != NULL) {
                esp.m.device = LWESP_DEVICE_ESP32_C3;
                major = LWESP_MIN_AT_VERSION_MAJOR_ESP32_C3;
                minor = LWESP_MIN_AT_VERSION_MINOR_ESP32_C3;
                patch = LWESP_MIN_AT_VERSION_PATCH_ESP32_C3;
                LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_TYPE_TRACE,
                             "[LWESP GMR] Detected Espressif device is %s\r\n", "ESP32-C3");
#endif /* LWESP_CFG_ESP32_C3 */
            } else {
                esp.m.device = LWESP_DEVICE_ESP8266;
                major = LWESP_MIN_AT_VERSION_MAJOR_ESP8266;
                minor = LWESP_MIN_AT_VERSION_MINOR_ESP8266;
                patch = LWESP_MIN_AT_VERSION_PATCH_ESP8266;
                LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_TYPE_TRACE,
                             "[LWESP GMR] Default Espressif device is %s\r\n", "ESP8266");

            }
            LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_TYPE_TRACE,
                         "[LWESP GMR] AT version minimum required: %d.%d.%d\r\n",
                         (int)major, (int)minor, (int)patch);
            LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_TYPE_TRACE,
                         "[LWESP GMR] AT version detected on device: %d.%d.%d\r\n",
                         (int)esp.m.version_at.major, (int)esp.m.version_at.minor, (int)esp.m.version_at.patch);

            /* Compare versions, but only if device is well detected */
            if (ok) {
                ok = 0;
                if (esp.m.version_at.major > major) {
                    ok = 1;
                } else if (esp.m.version_at.major == major) {
                    if (esp.m.version_at.minor > minor) {
                        ok = 1;
                    } else if (esp.m.version_at.minor == minor) {
                        if ((int8_t)esp.m.version_at.patch >= (int8_t)patch) {
                            ok = 1;
                        } else {
                            LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_SEVERE,
                                         "[LWESP GMR] AT version comparison failed with patch version\r\n");
                        }
                    } else {
                        LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_SEVERE,
                                     "[LWESP GMR] AT version comparison failed with minor version\r\n");
                    }
                } else {
                    LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_SEVERE,
                                 "[LWESP GMR] AT version comparison failed with major version\r\n");
                }
            }

            /* Send out version not supported information to system */
            if (!ok) {
                lwespi_send_cb(LWESP_EVT_AT_VERSION_NOT_SUPPORTED);
            }
        } else if (!strncmp(rcv->data, "SDK version", 11)) {
            lwespi_parse_at_sdk_version(&rcv->data[12], &esp.m.version_sdk);
        }
#endif /* LWESP_CFG_MODE_STATION */
    }

    /* Start processing received data */
    if (esp.msg != NULL) {                      /* Do we have valid message? */
        /* Start with received error code */
        if (strncmp(rcv->data, "ERR CODE:", 9) == 0) {
            /* Check for command not supported message */
            if (strncmp(&rcv->data[9], "0x01090000", 10) == 0) {
                esp.msg->res_err_code = lwespERRCMDNOTSUPPORTED;
            }
        } else if ((CMD_IS_CUR(LWESP_CMD_RESET) || CMD_IS_CUR(LWESP_CMD_RESTORE)) && is_ok) {  /* Check for reset/restore command */
            is_ok = 0;                          /* We must wait for "ready", not only "OK" */
            esp.ll.uart.baudrate = LWESP_CFG_AT_PORT_BAUDRATE;  /* Save user baudrate */
            lwesp_ll_init(&esp.ll);             /* Set new baudrate */
        } else if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSTATUS) || CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSTATE)) {
            size_t offset = 0;

            if (0
#if LWESP_CFG_ESP8266 || LWESP_CFG_ESP32
                || (!strncmp(rcv->data, "+CIPSTATUS", 10) && (offset = 11) > 0)   /* This is to check string and get offset in one shot */
#endif /* LWESP_CFG_ESP8266 || LWESP_CFG_ESP32 */
                || (!strncmp(rcv->data, "+CIPSTATE", 9) && (offset = 10) > 0)
               ) {
                lwespi_parse_cipstatus_cipstate(rcv->data + offset);/* Parse +CIPSTATUS or +CIPSTATE response */
            } else if (is_ok) {
                for (size_t i = 0; i < LWESP_CFG_MAX_CONNS; ++i) {  /* Set current connection statuses */
                    esp.m.conns[i].status.f.active = !!(esp.m.active_conns & (1 << i));
                }
            }
        } else if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSTART)) {
             if (is_ok) {
                // esp.msg->msg.conn_start.success = 1; 
             }
            /* Do nothing, it is either OK or not OK */
        } else if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSEND)) {
            if (is_ok) {                        /* Check for OK and clear as we have to check for "> " statement after OK */
                is_ok = 0;                      /* Do not reach on OK */
            }
            if (esp.msg->msg.conn_send.wait_send_ok_err) {
                if (!strncmp("SEND OK", rcv->data, 7)) {/* Data were sent successfully */
                    esp.msg->msg.conn_send.wait_send_ok_err = 0;
                    is_ok = lwespi_tcpip_process_data_sent(1);  /* Process as data were sent */
                    if (is_ok && esp.msg->msg.conn_send.conn->status.f.active) {
                        CONN_SEND_DATA_SEND_EVT(esp.msg, lwespOK);
                    }
                } else if (is_error || !strncmp("SEND FAIL", rcv->data, 9)) {
                    esp.msg->msg.conn_send.wait_send_ok_err = 0;
                    is_error = lwespi_tcpip_process_data_sent(0);   /* Data were not sent due to SEND FAIL or command didn't even start */
                    if (is_error && esp.msg->msg.conn_send.conn->status.f.active) {
                        CONN_SEND_DATA_SEND_EVT(esp.msg, lwespERR);
                    }
                }
            } else if (is_error) {
                CONN_SEND_DATA_SEND_EVT(esp.msg, lwespERR);
            }
        } else if (CMD_IS_CUR(LWESP_CMD_UART)) {/* In case of UART command */
            if (is_ok) {                        /* We have valid OK result */
                esp.ll.uart.baudrate = esp.msg->msg.uart.baudrate;  /* Save user baudrate */
                lwesp_ll_init(&esp.ll);         /* Set new baudrate */
            }
        }
    }

    /*
     * Check if connection is just active (or closed).
     *
     * Check +LINK_CONN messages
     */
    if (rcv->len > 20 && (s = strstr(rcv->data, "+LINK_CONN:")) != NULL) {
        /* Parse only valid connections, discard others */
        if (lwespi_parse_link_conn(s) && esp.m.link_conn.num < LWESP_CFG_MAX_CONNS) {
            uint8_t id;
            lwesp_conn_t* conn = &esp.m.conns[esp.m.link_conn.num]; /* Get connection pointer */
            if (esp.m.link_conn.failed && conn->status.f.active) {  /* Connection failed and now closed? */
                conn->status.f.active = 0;      /* Connection was just closed */

                esp.evt.type = LWESP_EVT_CONN_CLOSE;
                esp.evt.evt.conn_active_close.conn = conn;
                esp.evt.evt.conn_active_close.client = conn->status.f.client;   /* Set if it is client or not */
                /** @todo: Check if we really tried to close connection which was just closed */
                esp.evt.evt.conn_active_close.forced = CMD_IS_CUR(LWESP_CMD_TCPIP_CIPCLOSE);/* Set if action was forced = current action = close connection */
                esp.evt.evt.conn_active_close.client = lwespOK;
                lwespi_send_conn_cb(conn, NULL);/* Send event */

                /* Check if write buffer is set */
                if (conn->buff.buff != NULL) {
                    LWESP_DEBUGF(LWESP_CFG_DBG_CONN | LWESP_DBG_TYPE_TRACE,
                                 "[LWESP CONN] Free write buffer: %p\r\n", conn->buff.buff);
                    lwesp_mem_free_s((void**)&conn->buff.buff);
                }
            } else if (!esp.m.link_conn.failed && !conn->status.f.active) {
                id = conn->val_id;
                LWESP_MEMSET(conn, 0x00, sizeof(*conn));/* Reset connection parameters */
                conn->num = esp.m.link_conn.num;/* Set connection number */
                conn->status.f.active = !esp.m.link_conn.failed;/* Check if connection active */
                conn->val_id = ++id;            /* Set new validation ID */

                conn->type = esp.m.link_conn.type;  /* Set connection type */
                LWESP_MEMCPY(&conn->remote_ip, &esp.m.link_conn.remote_ip, sizeof(conn->remote_ip));
                conn->remote_port = esp.m.link_conn.remote_port;
                conn->local_port = esp.m.link_conn.local_port;
                conn->status.f.client = !esp.m.link_conn.is_server;

                /* Connection started as client? */
                if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSTART) && conn->status.f.client ) {
                    /* Use connection for user */
                    if (esp.msg->msg.conn_start.conn != NULL) {
                        *esp.msg->msg.conn_start.conn = conn;
                    }
                    conn->evt_func = esp.msg->msg.conn_start.evt_func;  /* Set callback function */
                    conn->arg = esp.msg->msg.conn_start.arg;/* Set argument for function */
                    esp.msg->msg.conn_start.success = 1;
                } else {                        /* Server connection start */
                    conn->evt_func = esp.evt_server;/* Set server default callback */
                    conn->arg = NULL;
                    conn->type = LWESP_CONN_TYPE_TCP;   /* Set connection type to TCP. @todo: Wait for ESP team to upgrade AT commands to set other type */
                }

                esp.evt.type = LWESP_EVT_CONN_ACTIVE;   /* Connection just active */
                esp.evt.evt.conn_active_close.conn = conn;  /* Set connection */
                esp.evt.evt.conn_active_close.client = conn->status.f.client;   /* Set if it is client or not */
                esp.evt.evt.conn_active_close.forced = conn->status.f.client;   /* Set if action was forced = if client mode */
                lwespi_send_conn_cb(conn, NULL);/* Send event */
                //lwespi_conn_start_timeout(conn);/* Start connection timeout timer */
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
                lwespi_conn_check_available_rx_data();
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */
            }
        }
        /*
        } else if (!strncmp(",CLOSED", &rcv->data[1], 7)) {
            const char* tmp = rcv->data; */
    } else if ( (rcv->len > 10  && (s = strstr(rcv->data, ",CONNECT" CRLF)) != NULL)) {
        const char* tmp = s;
        uint32_t num = 0;
        uint8_t id;
        while (tmp > rcv->data && LWESP_CHARISNUM(tmp[-1])) {
            --tmp;
        }
        num = lwespi_parse_number(&tmp);        /* Parse connection number */
        if (num < LWESP_CFG_MAX_CONNS) {
            lwesp_conn_t* conn = &esp.m.conns[num]; /* Parse received data */
            id = conn->val_id;
            LWESP_MEMSET(conn, 0x00, sizeof(*conn));/* Reset connection parameters */
            conn->num = num;                    /* Set connection number */
            conn->val_id = ++id;            /* Set new validation ID */
            conn->status.f.active = 1;
            if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSTART)) {
                conn->status.f.client = 1;
            } else {
                conn->status.f.client = 0;
            }
            if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSTART) && conn->status.f.client ) {
                /* Use connection for user */
                if (esp.msg->msg.conn_start.conn != NULL) {
                        *esp.msg->msg.conn_start.conn = conn;
                }
                conn->evt_func = esp.msg->msg.conn_start.evt_func;  /* Set callback function */
                conn->arg = esp.msg->msg.conn_start.arg;/* Set argument for function */
                esp.msg->msg.conn_start.success = 1;
            } else {                        /* Server connection start */
                conn->evt_func = esp.evt_server;/* Set server default callback */
                conn->arg = NULL;
                conn->type = LWESP_CONN_TYPE_TCP;   /* Set connection type to TCP. @todo: Wait for ESP team to upgrade AT commands to set other type */
            }

            esp.evt.type = LWESP_EVT_CONN_ACTIVE;   /* Connection just active */
            esp.evt.evt.conn_active_close.conn = conn;  /* Set connection */
            esp.evt.evt.conn_active_close.client = conn->status.f.client;   /* Set if it is client or not */
            esp.evt.evt.conn_active_close.forced = conn->status.f.client;   /* Set if action was forced = if client mode */
            lwespi_send_conn_cb(conn, NULL);/* Send event */
            //lwespi_conn_start_timeout(conn);/* Start connection timeout timer */
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
            lwespi_conn_check_available_rx_data();
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */

        }
    } else if ( (rcv->len > 9  && (s = strstr(rcv->data, ",CLOSED" CRLF)) != NULL) ||
                (rcv->len > 15 && (s = strstr(rcv->data, ",CONNECT FAIL" CRLF)) != NULL)) {
        const char* tmp = s;
        uint32_t num = 0;
        while (tmp > rcv->data && LWESP_CHARISNUM(tmp[-1])) {
            --tmp;
        }
        num = lwespi_parse_number(&tmp);        /* Parse connection number */
        if (num < LWESP_CFG_MAX_CONNS) {
            lwesp_conn_t* conn = &esp.m.conns[num]; /* Parse received data */
            conn->num = num;                    /* Set connection number */
            if (conn->status.f.active) {        /* Is connection actually active? */
                conn->status.f.active = 0;      /* Connection was just closed */

                esp.evt.type = LWESP_EVT_CONN_CLOSE;
                esp.evt.evt.conn_active_close.conn = conn;
                esp.evt.evt.conn_active_close.client = conn->status.f.client;   /* Set if it is client or not */
                esp.evt.evt.conn_active_close.forced = CMD_IS_CUR(LWESP_CMD_TCPIP_CIPCLOSE);/* Set if action was forced = current action = close connection */
                esp.evt.evt.conn_active_close.res = lwespOK;
                lwespi_send_conn_cb(conn, NULL);/* Send event */

                /*
                 * In case we received x,CLOSED on connection we are currently sending data,
                 * terminate sending of connection with failure
                 */
                if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSEND)) {
                    if (esp.msg->msg.conn_send.conn == conn) {
                        /** \todo: Find better idea to handle what to do in this case */
                        //is_error = 1;         /* Set as error to stop processing or waiting for connection */
                    }
                }
            }

            /* Check if write buffer is set */
            if (conn->buff.buff != NULL) {
                LWESP_DEBUGF(LWESP_CFG_DBG_CONN | LWESP_DBG_TYPE_TRACE,
                             "[LWESP CONN] Free write buffer: %p\r\n", conn->buff.buff);
                lwesp_mem_free_s((void**)&conn->buff.buff);
            }
        }
    } else if (is_error && CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSTART)) {
        /*
         * Notify user about failed connection,
         * but only if connection callback is known.
         *
         * This will prevent notifying wrong connection
         * in case connection is already active for some reason
         * but new callback is not set by user
         */
        if (esp.msg->msg.conn_start.evt_func != NULL) { /* Connection must be closed */
            lwespi_send_conn_error_cb(esp.msg, lwespERRCONNFAIL);
        }
    }

    /*
     * In case of any of these events, simply release semaphore
     * and proceed with next command
     */
    if (is_ok || is_error || is_ready) {
        lwespr_t res = lwespOK;
        if (esp.msg != NULL) {                  /* Do we have active message? */
            res = lwespi_process_sub_cmd(esp.msg, &is_ok, &is_error, &is_ready);
            if (res != lwespCONT) {             /* Shall we continue with next subcommand under this one? */
                if (is_ok || is_ready) {        /* Check ready or ok status */
                    res = esp.msg->res = lwespOK;
                } else {                        /* Or error status */
                    res = esp.msg->res = res;   /* Set the error status */
                }
            } else {
                ++esp.msg->i;                   /* Number of continue calls */
            }

            /*
             * When the command is finished,
             * release synchronization semaphore
             * from user thread and start with next command
             */
            if (res != lwespCONT) {             /* Do we have to continue to wait for command? */
                lwesp_sys_sem_release(&esp.sem_sync);   /* Release semaphore */
            }
        }
    }
}

#if !LWESP_CFG_INPUT_USE_PROCESS || __DOXYGEN__
/**
 * \brief           Process data from input buffer
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwespi_process_buffer(void) {
    void* data;
    size_t len;

    do {
        /*
         * Get length of linear memory in buffer
         * we can process directly as memory
         */
        len = lwesp_buff_get_linear_block_read_length(&esp.buff);
        if (len > 0) {
            /*
             * Get memory address of first element
             * in linear block of data to process
             */
            data = lwesp_buff_get_linear_block_read_address(&esp.buff);

            /* Process actual received data */
            lwespi_process(data, len);

            /*
             * Once data is processed, simply skip
             * the buffer memory and start over
             */
            lwesp_buff_skip(&esp.buff, len);
        }
    } while (len);
    return lwespOK;
}
#endif /* !LWESP_CFG_INPUT_USE_PROCESS || __DOXYGEN__ */

/**
 * \brief           Process input data received from ESP device
 * \param[in]       data: Pointer to data to process
 * \param[in]       data_len: Length of data to process in units of bytes
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwespi_process(const void* data, size_t data_len) {
    uint8_t ch;
    const uint8_t* d = data;
    size_t d_len = data_len;
    static uint8_t ch_prev1, ch_prev2;
    static lwesp_unicode_t unicode;

    /* Check status if device is available */
    if (!esp.status.f.dev_present) {
        return lwespERRNODEVICE;
    }

    while (d_len > 0) {                         /* Read entire set of characters from buffer */
        ch = *d;                                /* Get next character */
        ++d;                                    /* Go to next character, must be here as it is used later on */
        --d_len;                                /* Decrease remaining length, must be here as it is decreased later too */

        /*
         * First check if we are in IPD mode and process plain data
         * without checking for valid ASCII or unicode format
         */
        if (esp.m.ipd.read) {                   /* Do we have to read incoming IPD data? */
            size_t len;

            if (esp.m.ipd.buff != NULL) {       /* Do we have active buffer? */
                esp.m.ipd.buff->payload[esp.m.ipd.buff_ptr] = ch;   /* Save data character */
            }
            ++esp.m.ipd.buff_ptr;
            --esp.m.ipd.rem_len;

            /* Try to read more data directly from buffer */
            len = LWESP_MIN(d_len, LWESP_MIN(esp.m.ipd.rem_len, esp.m.ipd.buff != NULL ? (esp.m.ipd.buff->len - esp.m.ipd.buff_ptr) : esp.m.ipd.rem_len));
            LWESP_DEBUGF(LWESP_CFG_DBG_IPD | LWESP_DBG_TYPE_TRACE,
                         "[LWESP IPD] New length to read: %d bytes\r\n", (int)len);
            if (len > 0) {
                if (esp.m.ipd.buff != NULL) {   /* Is buffer valid? */
                    LWESP_MEMCPY(&esp.m.ipd.buff->payload[esp.m.ipd.buff_ptr], d, len);
                    LWESP_DEBUGF(LWESP_CFG_DBG_IPD | LWESP_DBG_TYPE_TRACE,
                                 "[LWESP IPD] Bytes read: %d\r\n", (int)len);
                } else {                        /* Simply skip the data in buffer */
                    LWESP_DEBUGF(LWESP_CFG_DBG_IPD | LWESP_DBG_TYPE_TRACE,
                                 "[LWESP IPD] Bytes skipped: %d\r\n", (int)len);
                }
                d_len -= len;                   /* Decrease effective length */
                d += len;                       /* Skip remaining length */
                esp.m.ipd.buff_ptr += len;      /* Forward buffer pointer */
                esp.m.ipd.rem_len -= len;       /* Decrease remaining length */
            }

            /* Did we reach end of buffer or no more data? */
            if (esp.m.ipd.rem_len == 0 || (esp.m.ipd.buff != NULL && esp.m.ipd.buff_ptr == esp.m.ipd.buff->len)) {
                lwespr_t res = lwespOK;

                /* Call user callback function with received data */
                if (esp.m.ipd.buff != NULL) {   /* Do we have valid buffer? */
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
                    size_t pbuf_len;

                    pbuf_len = lwesp_pbuf_length(esp.m.ipd.buff, 1);
                    esp.m.ipd.conn->tcp_not_ack_bytes += pbuf_len;
                    if (esp.m.ipd.conn->tcp_available_bytes >= pbuf_len) {
                        esp.m.ipd.conn->tcp_available_bytes -= pbuf_len;
                    }
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */

                    esp.m.ipd.conn->total_recved += esp.m.ipd.buff->tot_len;/* Increase number of bytes received */

                    /*
                     * Send data buffer to upper layer
                     *
                     * From this moment, user is responsible for packet
                     * buffer and must free it manually
                     */
                    esp.evt.type = LWESP_EVT_CONN_RECV;
                    esp.evt.evt.conn_data_recv.buff = esp.m.ipd.buff;
                    esp.evt.evt.conn_data_recv.conn = esp.m.ipd.conn;
                    res = lwespi_send_conn_cb(esp.m.ipd.conn, NULL);

                    lwesp_pbuf_free(esp.m.ipd.buff);/* Free packet buffer at this point */
                    LWESP_DEBUGF(LWESP_CFG_DBG_IPD | LWESP_DBG_TYPE_TRACE,
                                 "[LWESP IPD] Free packet buffer\r\n");
                    if (res == lwespOKIGNOREMORE) { /* We should ignore more data */
                        LWESP_DEBUGF(LWESP_CFG_DBG_IPD | LWESP_DBG_TYPE_TRACE,
                                     "[LWESP IPD] Ignoring more data from this IPD if available\r\n");
                        esp.m.ipd.buff = NULL;  /* Set to NULL to ignore more data if possibly available */
                    }

                    /*
                     * Create new data packet if case if:
                     *
                     *  - Previous one was successful and more data to read and
                     *  - Connection is not in closing state
                     */
                    if (esp.m.ipd.buff != NULL && esp.m.ipd.rem_len > 0 && !esp.m.ipd.conn->status.f.in_closing) {
                        size_t new_len = LWESP_MIN(esp.m.ipd.rem_len, LWESP_CFG_CONN_MAX_RECV_BUFF_SIZE);   /* Calculate new buffer length */

                        LWESP_DEBUGF(LWESP_CFG_DBG_IPD | LWESP_DBG_TYPE_TRACE,
                                     "[LWESP IPD] Allocating new packet buffer of size: %d bytes\r\n", (int)new_len);
                        esp.m.ipd.buff = lwesp_pbuf_new(new_len);   /* Allocate new packet buffer */

                        LWESP_DEBUGW(LWESP_CFG_DBG_IPD | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_WARNING,
                                     esp.m.ipd.buff == NULL, "[LWESP IPD] Buffer allocation failed for %d bytes\r\n", (int)new_len);

                        if (esp.m.ipd.buff != NULL) {
                            lwesp_pbuf_set_ip(esp.m.ipd.buff, &esp.m.ipd.ip, esp.m.ipd.port);   /* Set IP and port for received data */
                        }
                    } else {
                        esp.m.ipd.buff = NULL;  /* Reset it */
                    }
                }
                if (esp.m.ipd.rem_len == 0) {   /* Check if we read everything */
                    esp.m.ipd.buff = NULL;      /* Reset buffer pointer */
                    esp.m.ipd.read = 0;         /* Stop reading data */
                }
                esp.m.ipd.buff_ptr = 0;         /* Reset input buffer pointer */
                RECV_RESET();                   /* Reset receive data */
            }

            /*
             * We are in command mode where we have to process byte by byte
             * Simply check for ASCII and unicode format and process data accordingly
             */
        } else {
            lwespr_t res = lwespERR;
            if (LWESP_ISVALIDASCII(ch)) {       /* Manually check if valid ASCII character */
                res = lwespOK;
                unicode.t = 1;                  /* Manually set total to 1 */
                unicode.r = 0;                  /* Reset remaining bytes */
            } else if (ch >= 0x80) {            /* Process only if more than ASCII can hold */
                res = lwespi_unicode_decode(&unicode, ch);  /* Try to decode unicode format */
            }

            if (res == lwespERR) {              /* In case of an ERROR */
                unicode.r = 0;
            }
            if (res == lwespOK) {               /* Can we process the character(s) */
                if (unicode.t == 1) {           /* Totally 1 character? */
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
                    char* tmp_ptr;
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */
                    switch (ch) {
                        case '\n':
                            RECV_ADD(ch);       /* Add character to input buffer */
                            lwespi_parse_received(&recv_buff);  /* Parse received string */
                            RECV_RESET();       /* Reset received string */
                            break;
                        default:
                            RECV_ADD(ch);       /* Any ASCII valid character */
                            break;
                    }

                    /* If we are waiting for "\n> " sequence when CIPSEND command is active */
                    if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSEND)) {
                        if (ch_prev2 == '\r' && ch_prev1 == '\n' && ch == '>') {
                            RECV_RESET();       /* Reset received object */

                            /* Now actually send the data prepared before */
                            AT_PORT_SEND_WITH_FLUSH(&esp.msg->msg.conn_send.data[esp.msg->msg.conn_send.ptr], esp.msg->msg.conn_send.sent);
                            esp.msg->msg.conn_send.wait_send_ok_err = 1;/* Now we are waiting for "SEND OK" or "SEND ERROR" */
                        }
                    }

#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
                    /*
                     * Check if "+CIPRECVDATA" statement is in array and now we received colon,
                     * indicating end of +CIPRECVDATA statement and start of actual data
                     *
                     * Final conclusion for +CIPRECVDATA is:
                     * +CIPRECVDATA:<len>,<IP>,<port>,data...
                     *
                     * We must wait for 3 colon characters, before conclusion of valid data
                     */
                    if (ch == ',' && RECV_LEN() > 13 && RECV_IDX(0) == '+' && !strncmp(recv_buff.data, "+CIPRECVDATA", 12)
                        && (tmp_ptr = strchr(recv_buff.data, ',')) != NULL  /* Search for first comma */
                        && (tmp_ptr = strchr(tmp_ptr + 1, ',')) != NULL /* Search for second comma */
                        && (tmp_ptr = strchr(tmp_ptr + 1, ',')) != NULL) {  /* Search for third comma */
                        lwespi_parse_received(&recv_buff);  /* Parse received string */
                        if (esp.m.ipd.read) {   /* Shall we start read procedure? */
                            /*
                             * We should have already allocated pbuf memory at this stage.
                             * For manual receive, it is done prior command gets executed to be sure storage is ready to accept stream of data
                             *
                             * Pbuf length should not be bigger than number of received bytes
                             */
                            esp.m.ipd.buff = esp.msg->msg.ciprecvdata.buff;
                            esp.m.ipd.conn = esp.msg->msg.ciprecvdata.conn;
                            lwesp_pbuf_set_length(esp.m.ipd.buff, esp.m.ipd.tot_len);   /* Set new length of buffer */
                            esp.msg->msg.ciprecvdata.buff = NULL;   /* Clear reference for this pbuf */
                            LWESP_DEBUGF(LWESP_CFG_DBG_IPD | LWESP_DBG_TYPE_TRACE,
                                         "[LWESP IPD] Data on connection %d with total size %d byte(s)\r\n",
                                         (int)esp.m.ipd.conn->num, (int)esp.m.ipd.tot_len);
                        } else {
                            /* ERROR handling */
                        }
                    } else
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */

                        /*
                         * Check if "+IPD" statement is in array and now we received colon,
                         * indicating end of +IPD and start of actual data
                         */
                        if (ch == ':' && RECV_LEN() > 4 && RECV_IDX(0) == '+' && !strncmp(recv_buff.data, "+IPD", 4)) {
                            lwespi_parse_received(&recv_buff);  /* Parse received string */
                            if (esp.m.ipd.read) {   /* Shall we start read procedure? */
                                size_t len;
                                LWESP_DEBUGF(LWESP_CFG_DBG_IPD | LWESP_DBG_TYPE_TRACE,
                                             "[LWESP IPD] Data on connection %d with total size %d byte(s)\r\n",
                                             (int)esp.m.ipd.conn->num, (int)esp.m.ipd.tot_len);

                                len = LWESP_MIN(esp.m.ipd.rem_len, LWESP_CFG_CONN_MAX_RECV_BUFF_SIZE);

                                /*
                                 * Read received data in case of:
                                 *
                                 *  - Connection is active and
                                 *  - Connection is not in closing mode
                                 */
                                if (esp.m.ipd.conn->status.f.active && !esp.m.ipd.conn->status.f.in_closing) {
                                    esp.m.ipd.buff = lwesp_pbuf_new(len);   /* Allocate new packet buffer */
                                    if (esp.m.ipd.buff != NULL) {
                                        lwesp_pbuf_set_ip(esp.m.ipd.buff, &esp.m.ipd.ip, esp.m.ipd.port);   /* Set IP and port for received data */
                                    }
                                    LWESP_DEBUGW(LWESP_CFG_DBG_IPD | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_WARNING, esp.m.ipd.buff == NULL,
                                                 "[LWESP IPD] Buffer allocation failed for %d byte(s)\r\n", (int)len);
                                } else {
                                    esp.m.ipd.buff = NULL;  /* Ignore reading on closed connection */
                                    LWESP_DEBUGF(LWESP_CFG_DBG_IPD | LWESP_DBG_TYPE_TRACE,
                                                 "[LWESP IPD] Connection %d closed or in closing, skipping %d byte(s)\r\n",
                                                 (int)esp.m.ipd.conn->num, (int)len);
                                }
                                esp.m.ipd.conn->status.f.data_received = 1; /* We have first received data */
                                esp.m.ipd.buff_ptr = 0; /* Reset buffer write pointer */
                            }
                            RECV_RESET();       /* Reset received buffer */
                        }
                } else {                        /* We have sequence of unicode characters */
                    /*
                     * Unicode sequence characters are not "meta" characters
                     * so it is safe to just add them to receive array without checking
                     * what are the actual values
                     */
                    for (uint8_t i = 0; i < unicode.t; ++i) {
                        RECV_ADD(unicode.ch[i]);/* Add character to receive array */
                    }
                }
            } else if (res != lwespINPROG) {    /* Not in progress? */
                RECV_RESET();                   /* Invalid character in sequence */
            }
        }

        ch_prev2 = ch_prev1;                    /* Save previous character as previous previous */
        ch_prev1 = ch;                          /* Set current as previous */
    }
    return lwespOK;
}

/* Temporary macros, only available for inside gsmi_process_sub_cmd function */
/* Set new command, but first check for error on previous */
#define SET_NEW_CMD_CHECK_ERROR(new_cmd) do {   \
        if (!*(is_error)) {                         \
            n_cmd = (new_cmd);                      \
        }                                           \
    } while (0)

/* Set new command, ignore result of previous */
#define SET_NEW_CMD(new_cmd) do {               \
        n_cmd = (new_cmd);                          \
    } while (0)

/* Set new cmd if condition passes */
#define SET_NEW_CMD_COND(new_cmd, cond)  do {   \
        if ((cond)) {                               \
            n_cmd = (new_cmd);                      \
        }                                           \
    } while (0)

/**
 * \brief           Get next sub command for reset or restore sequence
 * \param[in]       msg: Pointer to current message
 * \param[in]       is_ok: Status whether last command result was OK
 * \param[in]       is_error: Status whether last command result was ERROR
 * \param[in]       is_ready: Status whether last command result was ready
 * \return          Next command to execute
 */
static lwesp_cmd_t
lwespi_get_reset_sub_cmd(lwesp_msg_t* msg, uint8_t* is_ok, uint8_t* is_error, uint8_t* is_ready) {
    lwesp_cmd_t n_cmd = LWESP_CMD_IDLE;
    switch (CMD_GET_CUR()) {
        case LWESP_CMD_RESET:
        case LWESP_CMD_RESTORE:
            SET_NEW_CMD(LWESP_CFG_AT_ECHO ? LWESP_CMD_ATE1 : LWESP_CMD_ATE0);
            break;
        case LWESP_CMD_ATE0:
        case LWESP_CMD_ATE1:
            SET_NEW_CMD(LWESP_CMD_GMR);
            break;
        case LWESP_CMD_GMR:
#if LWESP_CFG_LIST_CMD
            SET_NEW_CMD(LWESP_CMD_CMD);
            break;
        case LWESP_CMD_CMD:
#endif /* LWESP_CFG_LIST_CMD */
            SET_NEW_CMD(LWESP_CMD_SYSMSG);
            break;
        case LWESP_CMD_SYSMSG:
            SET_NEW_CMD(LWESP_CMD_SYSLOG);
            break;
        case LWESP_CMD_SYSLOG:
            SET_NEW_CMD(LWESP_CMD_WIFI_CWMODE);
            break;
        case LWESP_CMD_WIFI_CWMODE:
            SET_NEW_CMD(LWESP_CMD_WIFI_CWDHCP_GET);
            break;
        case LWESP_CMD_WIFI_CWDHCP_GET:
            SET_NEW_CMD(LWESP_CMD_TCPIP_CIPMUX);
            break;
        case LWESP_CMD_TCPIP_CIPMUX:
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
            SET_NEW_CMD(LWESP_CMD_TCPIP_CIPRECVMODE);
            break;
        case LWESP_CMD_TCPIP_CIPRECVMODE:
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */
#if LWESP_CFG_IPV6
            SET_NEW_CMD(LWESP_CMD_WIFI_IPV6);
            break;
        case LWESP_CMD_WIFI_IPV6:
#endif /* LWESP_CFG_IPV6 */
#if LWESP_CFG_MODE_STATION
            SET_NEW_CMD(LWESP_CMD_WIFI_CWLAPOPT);
            break;                              /* Set visible data for CWLAP command */
        case LWESP_CMD_WIFI_CWLAPOPT:
            SET_NEW_CMD(LWESP_CMD_TCPIP_CIPSTATUS);
            break;                              /* Get connection status */
        case LWESP_CMD_TCPIP_CIPSTATUS:
            SET_NEW_CMD(LWESP_CMD_TCPIP_CIPSTATE);
            break;                              /* Get connection status */
        case LWESP_CMD_TCPIP_CIPSTATE:
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
            SET_NEW_CMD(LWESP_CMD_WIFI_CIPAP_GET);
            break;                              /* Get access point IP */
        case LWESP_CMD_WIFI_CIPAP_GET:
            SET_NEW_CMD(LWESP_CMD_WIFI_CIPAPMAC_GET);
            break;                              /* Get access point MAC */
        case LWESP_CMD_WIFI_CIPAPMAC_GET:
#endif /* LWESP_CFG_MODE_STATION */
            SET_NEW_CMD(LWESP_CMD_TCPIP_CIPDINFO);
            break;                              /* Set visible data on +IPD */
        default:
            break;
    }
    LWESP_UNUSED(msg);
    LWESP_UNUSED(is_error);
    LWESP_UNUSED(is_ok);
    LWESP_UNUSED(is_ready);
    return n_cmd;
}

/**
 * \brief           Process current command with known execution status and start another if necessary
 * \param[in]       msg: Pointer to current message
 * \param[in]       is_ok: Status whether last command result was OK
 * \param[in]       is_error: Status whether last command result was ERROR
 * \param[in]       is_ready: Status whether last command result was ready
 * \return          lwespCONT if you sent more data and we need to process more data, or lwespOK on success, or lwespERR on error
 */
static lwespr_t
lwespi_process_sub_cmd(lwesp_msg_t* msg, uint8_t* is_ok, uint8_t* is_error, uint8_t* is_ready) {
    lwesp_cmd_t n_cmd = LWESP_CMD_IDLE;
    if (CMD_IS_DEF(LWESP_CMD_RESET)) {          /* Device is in reset mode */
        n_cmd = lwespi_get_reset_sub_cmd(msg, is_ok, is_error, is_ready);
        if (n_cmd == LWESP_CMD_IDLE) {          /* Last command? */
            RESET_SEND_EVT(msg, *is_ok ? lwespOK : lwespERR);
        }
    } else if (CMD_IS_DEF(LWESP_CMD_RESTORE)) {
        if ((CMD_IS_CUR(LWESP_CMD_RESET)) && *is_ready) {
            SET_NEW_CMD(LWESP_CMD_RESTORE);
        } else if ((CMD_IS_CUR(LWESP_CMD_RESTORE) && *is_ready)
                   || !CMD_IS_CUR(LWESP_CMD_RESTORE)) {
            SET_NEW_CMD(lwespi_get_reset_sub_cmd(msg, is_ok, is_error, is_ready));
        }
        if (n_cmd == LWESP_CMD_IDLE) {
            RESTORE_SEND_EVT(msg, *is_ok ? lwespOK : lwespERR);
        }
#if LWESP_CFG_MODE_STATION
    } else if (CMD_IS_DEF(LWESP_CMD_WIFI_CWJAP)) {  /* Is our intention to join to access point? */
        if (CMD_IS_CUR(LWESP_CMD_WIFI_CWJAP)) { /* Is the current command join? */
            if (*is_ok) {                       /* Did we join successfully? */
                SET_NEW_CMD(LWESP_CMD_WIFI_CWDHCP_GET); /* Check IP address status */
            } else {
                esp.m.sta.f.is_connected = 0;   /* Force disconnected status */
                /*
                 * Parse received error message,
                 * if final result was error, decide what type
                 * of error should be returned for user
                 */
                switch (msg->msg.sta_join.error_num) {
                    case 1:
                        esp.evt.evt.sta_join_ap.res = lwespERRCONNTIMEOUT;
                        break;
                    case 2:
                        esp.evt.evt.sta_join_ap.res = lwespERRPASS;
                        break;
                    case 3:
                        esp.evt.evt.sta_join_ap.res = lwespERRNOAP;
                        break;
                    case 4:
                        esp.evt.evt.sta_join_ap.res = lwespERRCONNFAIL;
                        break;
                    default:
                        esp.evt.evt.sta_join_ap.res = lwespERR;
                }
            }
        } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CWDHCP_GET)) {
            SET_NEW_CMD(LWESP_CMD_WIFI_CIPSTA_GET); /* Get IP address */
        } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPSTA_GET)) {
            lwespi_send_cb(LWESP_EVT_WIFI_IP_ACQUIRED); /* Notify upper layer */
            SET_NEW_CMD(LWESP_CMD_WIFI_CIPSTAMAC_GET);  /* Go to next command to get MAC address */
        } else {
            esp.evt.evt.sta_join_ap.res = lwespOK;  /* Connected ok */
        }

        /* Check command finish */
        if (n_cmd == LWESP_CMD_IDLE) {
            STA_JOIN_AP_SEND_EVT(msg, esp.evt.evt.sta_join_ap.res);
        }
    } else if (CMD_IS_DEF(LWESP_CMD_WIFI_CWLAP)) {
        STA_LIST_AP_SEND_EVT(msg, *is_ok ? lwespOK : lwespERR);
    } else if (CMD_IS_DEF(LWESP_CMD_WIFI_CWJAP_GET)) {
        STA_INFO_AP_SEND_EVT(msg, *is_ok ? lwespOK : lwespERR);
    } else if (CMD_IS_DEF(LWESP_CMD_WIFI_CIPSTA_SET)) {
        if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPSTA_SET)) {
            SET_NEW_CMD(LWESP_CMD_WIFI_CWDHCP_GET);
        } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CWDHCP_GET)) {
            SET_NEW_CMD(LWESP_CMD_WIFI_CIPSTA_GET);
        } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPSTA_GET)) {
            lwespi_send_cb(LWESP_EVT_WIFI_IP_ACQUIRED); /* Notify upper layer */
        }
    } else if (CMD_IS_DEF(LWESP_CMD_WIFI_CIPSTA_GET)) {
        if (CMD_IS_CUR(LWESP_CMD_WIFI_CWDHCP_GET)) {
            SET_NEW_CMD(LWESP_CMD_WIFI_CIPSTA_GET);
        } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPSTA_GET)) {
            lwespi_send_cb(LWESP_EVT_WIFI_IP_ACQUIRED);
        }
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
    } else if (CMD_IS_DEF(LWESP_CMD_WIFI_CWMODE) &&
               (msg->msg.wifi_mode.mode == LWESP_MODE_AP
#if LWESP_CFG_MODE_STATION
                || msg->msg.wifi_mode.mode == LWESP_MODE_STA_AP
#endif /* LWESP_CFG_MODE_STATION */
               )) {
        if (CMD_IS_CUR(LWESP_CMD_WIFI_CWMODE)) {
            SET_NEW_CMD_COND(LWESP_CMD_WIFI_CIPAP_GET, *is_ok);
        } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPAP_GET)) {
            SET_NEW_CMD_COND(LWESP_CMD_WIFI_CWDHCP_GET, *is_ok);
        } else if (CMD_IS_CUR(LWESP_CMD_WIFI_CWDHCP_GET)) {
            SET_NEW_CMD_COND(LWESP_CMD_WIFI_CIPAPMAC_GET, *is_ok);
        }
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
#if LWESP_CFG_DNS
    } else if (CMD_IS_DEF(LWESP_CMD_TCPIP_CIPDOMAIN)) {
        CIPDOMAIN_SEND_EVT(esp.msg, *is_ok ? lwespOK : lwespERR);
#endif /* LWESP_CFG_DNS */
#if LWESP_CFG_PING
    } else if (CMD_IS_DEF(LWESP_CMD_TCPIP_PING)) {
        PING_SEND_EVT(esp.msg, *is_ok ? lwespOK : lwespERR);
#endif /* LWESP_CFG_PING */
#if LWESP_CFG_SNTP
    } else if (CMD_IS_DEF(LWESP_CMD_TCPIP_CIPSNTPTIME)) {
        SNTP_TIME_SEND_EVT(esp.msg, *is_ok ? lwespOK : lwespERR);
#endif /* LWESP_CFG_SNTP */
    } else if (CMD_IS_DEF(LWESP_CMD_TCPIP_CIPSTART)) {  /* Is our intention to join to access point? */
        uint8_t is_status_check;

        /*
         * Check if current command is to get device connection status information.
         *
         * ESP8266 & ESP32 uses CIPSTATUS command
         * ESP32-C3 has CIPSTATE already implemented (CIPSTATUS is deprecated)
         */
        is_status_check = CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSTATUS) || CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSTATE);

        if (msg->i == 0 && is_status_check) { /* Was the current command status info? */
            SET_NEW_CMD_COND(LWESP_CMD_TCPIP_CIPSTART, *is_ok); /* Now actually start connection */
        } else if (msg->i == 1 && CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSTART)) {
            SET_NEW_CMD(lwespi_get_cipstatus_or_cipstate_cmd());
        } else if (msg->i == 2 && is_status_check) {
            /* Check if connect actually succeeded */
            if (!msg->msg.conn_start.success) {
                *is_ok = 0;
                *is_error = 1;
            }
        }
    } else if (CMD_IS_DEF(LWESP_CMD_TCPIP_CIPCLOSE)) {
        if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPCLOSE) && *is_error) {
            /* Notify upper layer about failed close event */
            esp.evt.type = LWESP_EVT_CONN_CLOSE;
            esp.evt.evt.conn_active_close.conn = msg->msg.conn_close.conn;
            esp.evt.evt.conn_active_close.forced = 1;
            esp.evt.evt.conn_active_close.res = lwespERR;
            esp.evt.evt.conn_active_close.client = msg->msg.conn_close.conn->status.f.active && msg->msg.conn_close.conn->status.f.client;
            lwespi_send_conn_cb(msg->msg.conn_close.conn, NULL);
        }
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
    } else if (CMD_IS_DEF(LWESP_CMD_TCPIP_CIPRECVDATA)) {
        if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPRECVLEN) && msg->msg.ciprecvdata.is_last_check == 0) {
            uint8_t set_error = 0;
            LWESP_DEBUGW(LWESP_CFG_DBG_CONN | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_SEVERE, *is_error,
                         "[LWESP CONN] CIPRECVLEN returned ERROR\r\n");

            /* If command is OK */
            if (*is_ok) {
                size_t len;

                /* Check if `+IPD` received during data length check */
                if (esp.msg->msg.ciprecvdata.ipd_recv) {
                    esp.msg->msg.ciprecvdata.ipd_recv = 0;
                    SET_NEW_CMD(LWESP_CMD_TCPIP_CIPRECVLEN);
                } else {
                    /* Number of bytes to read */
                    len = LWESP_MIN(LWESP_CFG_CONN_MAX_DATA_LEN, msg->msg.ciprecvdata.conn->tcp_available_bytes);
                    if (len > 0) {
                        lwesp_pbuf_p p = NULL;

                        /* Try to allocate packet buffer */
                        while (p == NULL) {
                            p = lwesp_pbuf_new(len);/* Try to allocate buffer */
                            if (p == NULL) {    /* In case of failure */
                                len /= 2;       /* Try with half of value on next try */
                                if (len < 10) { /* If not possible to allocate at least 10 bytes from first try, stop immediately */
                                    break;
                                }
                            }
                        }

                        /* Start reading procedure */
                        if (p != NULL) {
                            msg->msg.ciprecvdata.buff = p;
                            msg->msg.ciprecvdata.len = len;
                            SET_NEW_CMD(LWESP_CMD_TCPIP_CIPRECVDATA);
                        } else {
                            set_error = 1;
                        }
                    } else {
                        /* No error if buffer empty */
                    }
                }
            } else {
                set_error = 1;
            }
            if (set_error) {
                *is_ok = 0;
                *is_error = 1;
            }
        } else if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPRECVDATA)) {
            /* Read failed? Handle queue len */
            if (*is_error) {
                if (msg->msg.ciprecvdata.buff != NULL) {
                    lwesp_pbuf_free(msg->msg.ciprecvdata.buff);
                    msg->msg.ciprecvdata.buff = NULL;
                }
            }

            /* This one is optional, to check for more data just at the end */
            SET_NEW_CMD(LWESP_CMD_TCPIP_CIPRECVLEN);/* Inquiry for latest status on data */
            msg->msg.ciprecvdata.is_last_check = 1;
        } else if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPRECVLEN) && msg->msg.ciprecvdata.is_last_check == 0) {
            /* Do nothing */
            if (*is_error) {
                *is_error = 0;
                *is_ok = 1;
            }
        }
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */
    } else if (CMD_IS_DEF(LWESP_CMD_WIFI_CWDHCP_SET)) {
        if (CMD_IS_CUR(LWESP_CMD_WIFI_CWDHCP_SET)) {
            SET_NEW_CMD(LWESP_CMD_WIFI_CWDHCP_GET);
        }
    }

    /* Are we enabling server mode for some reason? */
    if (CMD_IS_DEF(LWESP_CMD_TCPIP_CIPSERVER)) {
        if (msg->msg.tcpip_server.en) {
            if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSERVERMAXCONN)) {
                /* Since not all AT versions support CIPSERVERMAXCONN command, ignore result */
                SET_NEW_CMD(LWESP_CMD_TCPIP_CIPSERVER);
            } else if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSERVER)) {
                if (*is_ok) {
                    esp.evt_server = msg->msg.tcpip_server.cb;  /* Set server callback function */
                    SET_NEW_CMD(LWESP_CMD_TCPIP_CIPSTO);
                }
            } else if (CMD_IS_CUR(LWESP_CMD_TCPIP_CIPSTO)) {
                *is_ok = 1;                     /* Force to 1 */
            }
        }
        if (n_cmd == LWESP_CMD_IDLE) {          /* Do we still have execution? */
            esp.evt.evt.server.res = *is_ok ? lwespOK : lwespERR;
            esp.evt.evt.server.en = msg->msg.tcpip_server.en;
            esp.evt.evt.server.port = msg->msg.tcpip_server.port;
            lwespi_send_cb(LWESP_EVT_SERVER);   /* Send to upper layer */
        }
    }

    /* Check and start a new command */
    if (n_cmd != LWESP_CMD_IDLE) {
        lwespr_t res;
        msg->cmd = n_cmd;
        if ((res = msg->fn(msg)) == lwespOK) {
            return lwespCONT;
        } else {
            *is_ok = 0;
            *is_error = 1;
            return res;
        }
    } else {
        msg->cmd = LWESP_CMD_IDLE;
    }
    return *is_ok || *is_ready ? lwespOK : (msg->res_err_code != lwespOK ? msg->res_err_code : lwespERR);
}

/**
 * \brief           Function to initialize every AT command
 * \note            Never call this function directly. Set as initialization function for command and use `msg->fn(msg)`
 * \param[in]       msg: Pointer to \ref lwesp_msg_t with data
 * \return          Member of \ref lwespr_t enumeration
 */
lwespr_t
lwespi_initiate_cmd(lwesp_msg_t* msg) {
    switch (CMD_GET_CUR()) {                    /* Check current message we want to send over AT */
        case LWESP_CMD_RESET: {                 /* Reset MCU with AT commands */
            /* Try hardware reset first */
            if (esp.ll.reset_fn != NULL && esp.ll.reset_fn(1)) {

                /* Set baudrate to default one */
                esp.ll.uart.baudrate = LWESP_CFG_AT_PORT_BAUDRATE;
                lwesp_ll_init(&esp.ll);         /* Set new baudrate */

                lwesp_delay(10);                /* Wait some time */
                esp.ll.reset_fn(0);             /* Release reset */
            } else {
                AT_PORT_SEND_BEGIN_AT();
                AT_PORT_SEND_CONST_STR("+RST");
                AT_PORT_SEND_END_AT();
                /* Do not modify baudrate yet as we need "OK" or "ERROR" response first */
            }
            break;
        }
        case LWESP_CMD_AT_TEST: {               /* connect test */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_RESTORE: {               /* Reset MCU with AT commands */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+RESTORE");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_ATE0: {                  /* Disable AT echo mode */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("E0");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_ATE1: {                  /* Enable AT echo mode */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("E1");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_GMR: {                   /* Get AT version */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+GMR");
            AT_PORT_SEND_END_AT();
            break;
        }
#if LWESP_CFG_LIST_CMD
        case LWESP_CMD_CMD: {                   /* Get CMD list */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CMD?");
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_LIST_CMD */
        case LWESP_CMD_SYSMSG: {                /* Enable system messages */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+SYSMSG_CUR=<3>");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_SYSLOG: {                /* Enable system error logs */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+SYSLOG=1");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_UART: {                  /* Change UART parameters for AT port */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+UART_CUR=");
            lwespi_send_number(LWESP_U32(msg->msg.uart.baudrate), 0, 0);
            AT_PORT_SEND_CONST_STR(",8,1,0,0");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_WIFI_CWLAPOPT: {         /* Set visible data on CWLAP command */
            AT_PORT_SEND_BEGIN_AT();
            /*
             * Ignore remaining parameters, use default value
             * providing us maximum result possible
             */
            AT_PORT_SEND_CONST_STR("+CWLAPOPT=1,0x7FF");
            AT_PORT_SEND_END_AT();
            break;
        }

            /* WiFi related commands */
#if LWESP_CFG_IPV6
        case LWESP_CMD_WIFI_IPV6: {             /* Enable IPv6 */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPV6=1");
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_IPV6 */
#if LWESP_CFG_MODE_STATION
        case LWESP_CMD_WIFI_CWJAP: {            /* Try to join to access point */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWJAP=");
            lwespi_send_string(msg->msg.sta_join.name, 1, 1, 0);
            lwespi_send_string(msg->msg.sta_join.pass, 1, 1, 1);
            if (msg->msg.sta_join.mac != NULL) {
                lwespi_send_mac(msg->msg.sta_join.mac, 1, 1);
            }
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_WIFI_CWRECONNCFG: {      /* Set reconnect interval */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWRECONNCFG=");
            lwespi_send_number(msg->msg.sta_reconn_set.interval, 0, 0);
            lwespi_send_number(msg->msg.sta_reconn_set.rep_cnt, 0, 1);
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_WIFI_CWJAP_GET: {        /* Get the info of the connected access point */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWJAP?");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_WIFI_CWQAP: {            /* Quit from access point */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWQAP");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_WIFI_CWLAP: {            /* List access points */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWLAP");
            if (msg->msg.ap_list.ssid != NULL) {/* Do we want to filter by SSID? */
                AT_PORT_SEND_CONST_STR("=");
                lwespi_send_string(msg->msg.ap_list.ssid, 1, 1, 0);
            }
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_WIFI_CWAUTOCONN: {       /* Set autoconnect feature */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWAUTOCONN=");
            lwespi_send_number(LWESP_U32(!!msg->msg.sta_autojoin.en), 0, 0);
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIUPDATE: {        /* Update ESP software remotely */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIUPDATE");
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_MODE_STATION */
        case LWESP_CMD_WIFI_CWMODE: {           /* Set WIFI mode */
            lwesp_mode_t m;

            if (!CMD_IS_DEF(LWESP_CMD_WIFI_CWMODE)) {   /* Is this command part of reset sequence? */
#if LWESP_CFG_MODE_STATION_ACCESS_POINT
                m = LWESP_MODE_STA_AP;          /* Set station and access point mode */
#elif LWESP_CFG_MODE_STATION
                m = LWESP_MODE_STA;             /* Set station mode */
#else /* LWESP_CFG_MODE_STATION */
                m = LWESP_MODE_AP;              /* Set access point mode */
#endif /* !LWESP_CFG_MODE_STATION_ACCESS_POINT */
            } else {
                /* Use user setup */
                m = msg->msg.wifi_mode.mode;
            }

            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWMODE=");
            lwespi_send_number(LWESP_U32(m), 0, 0);
            //lwespi_send_number(1, 0, 1);
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_WIFI_CWMODE_GET: {       /* Get WIFI mode */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWMODE?");
            AT_PORT_SEND_END_AT();
            break;
        }
#if LWESP_CFG_MODE_STATION
        case LWESP_CMD_WIFI_CIPSTA_GET:         /* Get station IP address */
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
        case LWESP_CMD_WIFI_CIPAP_GET:          /* Get access point IP address */
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
            {
                AT_PORT_SEND_BEGIN_AT();
                AT_PORT_SEND_CONST_STR("+CIP");
#if LWESP_CFG_MODE_STATION
                if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPSTA_GET)) {
                    AT_PORT_SEND_CONST_STR("STA");
                }
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
                if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPAP_GET)) {
                    AT_PORT_SEND_CONST_STR("AP");
                }
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
                AT_PORT_SEND_CONST_STR("?");
                AT_PORT_SEND_END_AT();
                break;
            }
#if LWESP_CFG_MODE_STATION
        case LWESP_CMD_WIFI_CIPSTAMAC_GET:      /* Get station MAC address */
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
        case LWESP_CMD_WIFI_CIPAPMAC_GET:       /* Get access point MAC address */
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
            {
                AT_PORT_SEND_BEGIN_AT();
                AT_PORT_SEND_CONST_STR("+CIP");
#if LWESP_CFG_MODE_STATION
                if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPSTAMAC_GET)) {
                    AT_PORT_SEND_CONST_STR("STA");
                }
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
                if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPAPMAC_GET)) {
                    AT_PORT_SEND_CONST_STR("AP");
                }
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
                AT_PORT_SEND_CONST_STR("MAC?");
                AT_PORT_SEND_END_AT();
                break;
            }
#if LWESP_CFG_MODE_STATION
        case LWESP_CMD_WIFI_CIPSTA_SET:         /* Set station IP address */
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
        case LWESP_CMD_WIFI_CIPAP_SET:          /* Set access point IP address */
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
            {
                AT_PORT_SEND_BEGIN_AT();
                AT_PORT_SEND_CONST_STR("+CIP");
#if LWESP_CFG_MODE_STATION
                if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPSTA_SET)) {
                    AT_PORT_SEND_CONST_STR("STA");
                }
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
                if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPAP_SET)) {
                    AT_PORT_SEND_CONST_STR("AP");
                }
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
                AT_PORT_SEND_CONST_STR("=");
                lwespi_send_ip(&msg->msg.sta_ap_setip.ip, 1, 0);/* Send IP address */
                if (lwesp_ip_is_valid(&msg->msg.sta_ap_setip.gw) > 0) { /* Is gateway set? */
                    lwespi_send_ip(&msg->msg.sta_ap_setip.gw, 1, 1);/* Send gateway address */
                    if (lwesp_ip_is_valid(&msg->msg.sta_ap_setip.nm) > 0) { /* Is netmask set ? */
                        lwespi_send_ip(&msg->msg.sta_ap_setip.nm, 1, 1);/* Send netmask address */
                    }
                }
                AT_PORT_SEND_END_AT();
                break;
            }
#if LWESP_CFG_MODE_STATION
        case LWESP_CMD_WIFI_CIPSTAMAC_SET:      /* Set station MAC address */
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
        case LWESP_CMD_WIFI_CIPAPMAC_SET:       /* Set access point MAC address */
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
            {
                AT_PORT_SEND_BEGIN_AT();
                AT_PORT_SEND_CONST_STR("+CIP");
#if LWESP_CFG_MODE_STATION
                if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPSTAMAC_SET)) {
                    AT_PORT_SEND_CONST_STR("STA");
                }
#endif /* LWESP_CFG_MODE_STATION */
#if LWESP_CFG_MODE_ACCESS_POINT
                if (CMD_IS_CUR(LWESP_CMD_WIFI_CIPAPMAC_SET)) {
                    AT_PORT_SEND_CONST_STR("AP");
                }
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
                AT_PORT_SEND_CONST_STR("MAC=");
                lwespi_send_mac(&msg->msg.sta_ap_setmac.mac, 1, 0);
                AT_PORT_SEND_END_AT();
                break;
            }
        case LWESP_CMD_WIFI_CWDHCP_GET: {
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWDHCP?");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_WIFI_CWDHCP_SET: {
            uint32_t num = 0;

            /* Configure DHCP setup */
            num |= (msg->msg.wifi_cwdhcp.sta > 0 ? 0x01 : 0x00);
            num |= (msg->msg.wifi_cwdhcp.ap > 0 ? 0x02 : 0x00);

            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWDHCP=");
            lwespi_send_number(LWESP_U32(msg->msg.wifi_cwdhcp.en > 0), 0, 0);
            lwespi_send_number(num, 0, 1);
            AT_PORT_SEND_END_AT();
            break;
        }
#if LWESP_CFG_MODE_ACCESS_POINT
        case LWESP_CMD_WIFI_CWSAP_SET: {        /* Set soft-access point parameters */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWSAP=");
            lwespi_send_string(msg->msg.ap_conf.ssid, 1, 1, 0);
            lwespi_send_string(msg->msg.ap_conf.pwd, 1, 1, 1);
            lwespi_send_number(LWESP_U32(msg->msg.ap_conf.ch), 0, 1);
            lwespi_send_number(LWESP_U32(msg->msg.ap_conf.ecn), 0, 1);
            lwespi_send_number(LWESP_U32(msg->msg.ap_conf.max_sta), 0, 1);
            lwespi_send_number(LWESP_U32(!!msg->msg.ap_conf.hid), 0, 1);
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_WIFI_CWSAP_GET: {
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWSAP?");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_WIFI_CWLIF: {            /* List stations connected on soft-access point */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWLIF");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_WIFI_CWQIF: {            /* Disconnect station from soft-access point */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWQIF");
            if (msg->msg.ap_disconn_sta.use_mac) {
                AT_PORT_SEND_CONST_STR("=");
                lwespi_send_mac(&msg->msg.ap_disconn_sta.mac, 1, 0);
            }
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_MODE_ACCESS_POINT */
#if LWESP_CFG_WPS
        case LWESP_CMD_WIFI_WPS: {              /* Enable WPS function */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+WPS=");
            lwespi_send_number(LWESP_U32(!!msg->msg.wps_cfg.en), 0, 0);
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_WPS */
#if LWESP_CFG_HOSTNAME
        case LWESP_CMD_WIFI_CWHOSTNAME_SET: {   /* List stations connected on access point */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWHOSTNAME=");
            lwespi_send_string(msg->msg.wifi_hostname.hostname_set, 1, 1, 0);
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_WIFI_CWHOSTNAME_GET: {   /* List stations connected on access point */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWHOSTNAME?");
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_HOSTNAME */
#if LWESP_CFG_MDNS
        case LWESP_CMD_WIFI_MDNS: {             /* Set mDNS parameters */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+MDNS=");
            if (msg->msg.mdns.en) {             /* Send the rest only in case mDNS should be enabled */
                AT_PORT_SEND_CONST_STR("1");
                lwespi_send_string(msg->msg.mdns.host, 0, 1, 1);
                lwespi_send_string(msg->msg.mdns.server, 0, 1, 1);
                lwespi_send_port(msg->msg.mdns.port, 0, 1);
            } else {
                AT_PORT_SEND_CONST_STR("0");
            }
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_MDNS */

        /* TCP/IP related commands */
        case LWESP_CMD_TCPIP_CIPSERVER: {       /* Enable or disable server */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPSERVER=");
            if (CMD_IS_DEF(LWESP_CMD_TCPIP_CIPSERVER) && msg->msg.tcpip_server.en) {
                AT_PORT_SEND_CONST_STR("1");
                lwespi_send_port(msg->msg.tcpip_server.port, 0, 1);
            } else {                            /* Disable server */
                AT_PORT_SEND_CONST_STR("0");
            }
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPSERVERMAXCONN: {/* Maximal number of connections */
            uint16_t max_conn;
            if (CMD_IS_DEF(LWESP_CMD_TCPIP_CIPSERVER)) {
                max_conn = LWESP_MIN(msg->msg.tcpip_server.max_conn, LWESP_CFG_MAX_CONNS);
            } else {
                max_conn = LWESP_CFG_MAX_CONNS;
            }
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPSERVERMAXCONN=");
            lwespi_send_number(LWESP_U32(max_conn), 0, 0);
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPSTO: {          /* Set server connection timeout */
            uint16_t timeout;
            if (CMD_IS_DEF(LWESP_CMD_TCPIP_CIPSERVER)) {
                timeout = msg->msg.tcpip_server.timeout;
            } else {
                timeout = 100;
            }
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPSTO=");
            lwespi_send_number(LWESP_U32(timeout), 0, 0);
            AT_PORT_SEND_END_AT();
            break;
        }
#if LWESP_CFG_MODE_STATION
        case LWESP_CMD_TCPIP_CIPSTART: {        /* Start a new connection */
            const char* conn_type_str;

            /* Do we have wifi connection? */
            if (!lwesp_sta_has_ip()) {
                lwespi_send_conn_error_cb(msg, lwespERRNOIP);
                return lwespERRNOIP;
            }

            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPSTART=");
            lwespi_send_number(msg->msg.conn_start.linkid, 0, 0);
            AT_PORT_SEND_CONST_STR(",");

            if (msg->msg.conn_start.type == LWESP_CONN_TYPE_SSL) {
                conn_type_str = "SSL";
            } else if (msg->msg.conn_start.type == LWESP_CONN_TYPE_TCP) {
                conn_type_str = "TCP";
            } else if (msg->msg.conn_start.type == LWESP_CONN_TYPE_UDP) {
                conn_type_str = "UDP";
#if LWESP_CFG_IPV6
            } else if (msg->msg.conn_start.type == LWESP_CONN_TYPE_TCPV6) {
                conn_type_str = "TCPV6";
            } else if (msg->msg.conn_start.type == LWESP_CONN_TYPE_SSLV6) {
                conn_type_str = "SSLV6";
#endif /* LWESP_CFG_IPV6 */
            } else {
                conn_type_str = "unknonw";
            }
            lwespi_send_string(conn_type_str, 0, 1, 0);
            lwespi_send_string(msg->msg.conn_start.remote_host, 0, 1, 1);
            lwespi_send_port(msg->msg.conn_start.remote_port, 0, 1);

            /* Connection-type specific features */
            if (msg->msg.conn_start.type != LWESP_CONN_TYPE_UDP) {
                lwespi_send_number(LWESP_U32(msg->msg.conn_start.tcp_ssl_keep_alive), 0, 1);
            } else {
                if (msg->msg.conn_start.udp_local_port > 0) {
                    lwespi_send_port(msg->msg.conn_start.udp_local_port, 0, 1);
                } else {
                    AT_PORT_SEND_CONST_STR(",");
                }
                lwespi_send_number(msg->msg.conn_start.udp_mode, 0, 1);
            }
            if (msg->msg.conn_start.local_ip != NULL) {
                lwespi_send_string(msg->msg.conn_start.local_ip, 0, 1, 1);
            }
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_MODE_STATION */
        case LWESP_CMD_TCPIP_CIPCLOSE: {        /* Close the connection */
            lwesp_conn_p c = msg->msg.conn_close.conn;
            if (c != NULL &&
                /*
                 * Is connection already closed or command
                 * for this connection is not valid anymore?
                 */
                (!lwesp_conn_is_active(c) || c->val_id != msg->msg.conn_close.val_id)) {
                return lwespERR;
            }
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPCLOSE=");
            lwespi_send_number(LWESP_U32(msg->msg.conn_close.conn ? msg->msg.conn_close.conn->num : LWESP_CFG_MAX_CONNS), 0, 0);
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPSEND: {         /* Send data to connection */
            return lwespi_tcpip_process_send_data();/* Process send data */
        }
        case LWESP_CMD_TCPIP_CIPSTATUS: {       /* Get status of device and all connections */
            esp.m.active_conns_last = esp.m.active_conns;   /* Save as last status */
            esp.m.active_conns = 0;             /* Reset new status before parsing starts */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPSTATUS");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPSTATE: {        /* Get status of all connections */
            esp.m.active_conns_last = esp.m.active_conns;   /* Save as last status */
            esp.m.active_conns = 0;             /* Reset new status before parsing starts */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPSTATE?");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPDINFO: {        /* Set info data on +IPD command */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPDINFO=1");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPMUX: {          /* Set multiple connections */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPMUX=1");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPSSLSIZE: {      /* Set SSL size */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPSSLSIZE=");
            lwespi_send_number(LWESP_U32(msg->msg.tcpip_sslsize.size), 0, 0);
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPSSLCCONF: {     /* Set SSL Configuration */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPSSLCCONF=");
            lwespi_send_number(LWESP_U32(msg->msg.tcpip_ssl_cfg.link_id), 0, 0);
            lwespi_send_number(LWESP_U32(msg->msg.tcpip_ssl_cfg.auth_mode), 0, 1);
            lwespi_send_number(LWESP_U32(msg->msg.tcpip_ssl_cfg.pki_number), 0, 1);
            lwespi_send_number(LWESP_U32(msg->msg.tcpip_ssl_cfg.ca_number), 0, 1);
            AT_PORT_SEND_END_AT();
            break;
        }
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
        case LWESP_CMD_TCPIP_CIPRECVMODE: {     /* Set TCP data receive mode */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPRECVMODE=1");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPRECVDATA: {     /* Manually read data */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPRECVDATA=");
            lwespi_send_number(LWESP_U32(msg->msg.ciprecvdata.conn->num), 0, 0);
            lwespi_send_number(LWESP_U32(msg->msg.ciprecvdata.len), 0, 1);
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPRECVLEN: {      /* Get length to read */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPRECVLEN?");
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */
#if LWESP_CFG_DNS
        case LWESP_CMD_TCPIP_CIPDOMAIN: {       /* DNS function */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPDOMAIN=");
            lwespi_send_string(msg->msg.dns_getbyhostname.host, 1, 1, 0);
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPDNS_SET: {      /* DNS set config */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPDNS=");
            lwespi_send_number(LWESP_U32(!!msg->msg.dns_setconfig.en), 0, 0);
            if (msg->msg.dns_setconfig.en) {
                if (msg->msg.dns_setconfig.s1 != NULL) {
                    lwespi_send_string(msg->msg.dns_setconfig.s1, 0, 1, 1);
                }
                if (msg->msg.dns_setconfig.s2 != NULL) {
                    lwespi_send_string(msg->msg.dns_setconfig.s2, 0, 1, 1);
                }
            }
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPDNS_GET: {      /* DNS get config */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPDNS?");
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_DNS */
#if LWESP_CFG_PING
        case LWESP_CMD_TCPIP_PING: {            /* Ping hostname or IP address */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+PING=");
            lwespi_send_string(msg->msg.tcpip_ping.host, 1, 1, 0);
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_PING */
#if LWESP_CFG_SNTP
        case LWESP_CMD_TCPIP_CIPSNTPCFG: {      /* Configure SNTP */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPSNTPCFG=");
            lwespi_send_number(LWESP_U32(msg->msg.tcpip_sntp_cfg.en > 0 ? 1 : 0), 0, 0);
            lwespi_send_signed_number(LWESP_I32(msg->msg.tcpip_sntp_cfg.tz), 0, 1);
            if (msg->msg.tcpip_sntp_cfg.h1 != NULL && strlen(msg->msg.tcpip_sntp_cfg.h1)) {
                lwespi_send_string(msg->msg.tcpip_sntp_cfg.h1, 0, 1, 1);
            }
            if (msg->msg.tcpip_sntp_cfg.h2 != NULL && strlen(msg->msg.tcpip_sntp_cfg.h2)) {
                lwespi_send_string(msg->msg.tcpip_sntp_cfg.h2, 0, 1, 1);
            }
            if (msg->msg.tcpip_sntp_cfg.h3 != NULL && strlen(msg->msg.tcpip_sntp_cfg.h3)) {
                lwespi_send_string(msg->msg.tcpip_sntp_cfg.h3, 0, 1, 1);
            }
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPSNTPTIME: {     /* Get time over SNTP */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPSNTPTIME?");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_TCPIP_CIPSNTPINTV: {     /* Set SNTP sync interval */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CIPSNTPINTV=");
            lwespi_send_number(LWESP_U32(msg->msg.tcpip_sntp_intv.interval), 0, 0);
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_SNTP */
#if LWESP_CFG_SMART
        case LWESP_CMD_WIFI_SMART_START: {      /* Start smart config */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWSTARTSMART");
            AT_PORT_SEND_END_AT();
            break;
        }
        case LWESP_CMD_WIFI_SMART_STOP: {       /* Stop smart config */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+CWSTOPSMART");
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_SMART */
#if LWESP_CFG_WEBSERVER
        case LWESP_CMD_WEBSERVER: {             /* Start Web Server */
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+WEBSERVER=");
            if (msg->msg.web_server.en) {
                AT_PORT_SEND_CONST_STR("1");
                lwespi_send_port(msg->msg.web_server.port, 0, 1);
                lwespi_send_number(msg->msg.web_server.timeout, 0, 1);
            } else {                            /* Stop Web server */
                AT_PORT_SEND_CONST_STR("0");
            }
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_WEBSERVER */
#if LWESP_CFG_ESP32
        case LWESP_CMD_BLEINIT_GET: {
            AT_PORT_SEND_BEGIN_AT();
            AT_PORT_SEND_CONST_STR("+BLEINIT?");
            AT_PORT_SEND_END_AT();
            break;
        }
#endif /* LWESP_CFG_ESP32 */

        default:
            return lwespERR;                    /* Invalid command */
    }
    return lwespOK;                             /* Valid command */
}

/**
 * \brief           Checks if connection pointer has valid address
 * \param[in]       conn: Address to check if valid connection ptr
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwespi_is_valid_conn_ptr(lwesp_conn_p conn) {
    for (size_t i = 0; i < LWESP_ARRAYSIZE(esp.m.conns); ++i) {
        if (conn == &esp.m.conns[i]) {
            return 1;
        }
    }
    return 0;
}

/**
 * \brief           Send message from API function to producer queue for further processing
 * \param[in]       msg: New message to process
 * \param[in]       process_fn: callback function used to process message
 * \param[in]       max_block_time: Maximal time command can block in units of milliseconds
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwespi_send_msg_to_producer_mbox(lwesp_msg_t* msg, lwespr_t (*process_fn)(lwesp_msg_t*), uint32_t max_block_time) {
    lwespr_t res = msg->res = msg->res_err_code = lwespOK;

    lwesp_core_lock();

    /*
     * If locked more than 1 time, means we were called from callback or internally.
     * It is not allowed to call command in blocking mode from stack itself (would end up in dead-lock)
     */
    if (esp.locked_cnt > 1 && msg->is_blocking) {
        res = lwespERRBLOCKING;                 /* Blocking mode not allowed */
    } else if (!esp.status.f.dev_present) {     /* Check device present */
        res = lwespERRNODEVICE;                 /* No device connected */
    }
    lwesp_core_unlock();
    if (res != lwespOK) {
        LWESP_MSG_VAR_FREE(msg);
        return res;
    }

    if (msg->is_blocking) {                     /* In case message is blocking */
        if (!lwesp_sys_sem_create(&msg->sem, 0)) {  /* Create semaphore and lock it immediately */
            LWESP_MSG_VAR_FREE(msg);            /* Release memory and return */
            return lwespERRMEM;
        }
    }
    if (!msg->cmd) {                            /* Set start command if not set by user */
        msg->cmd = msg->cmd_def;                /* Set it as default */
    }
    msg->block_time = max_block_time;           /* Set blocking status if necessary */
    msg->fn = process_fn;                       /* Save processing function to be called as callback */
    if (msg->is_blocking) {
        lwesp_sys_mbox_put(&esp.mbox_producer, msg);/* Write message to producer queue and wait forever */
    } else {
        if (!lwesp_sys_mbox_putnow(&esp.mbox_producer, msg)) {  /* Write message to producer queue immediately */
            LWESP_MSG_VAR_FREE(msg);            /* Release message */
            return lwespERRMEM;
        }
    }
    if (res == lwespOK && msg->is_blocking) {   /* In case we have blocking request */
        uint32_t time;
        time = lwesp_sys_sem_wait(&msg->sem, 0);/* Wait forever for semaphore */
        if (time == LWESP_SYS_TIMEOUT) {        /* If semaphore was not accessed within given time */
            res = lwespTIMEOUT;                 /* Semaphore not released in time */
        } else {
            res = msg->res;                     /* Set response status from message response */
        }

        LWESP_MSG_VAR_FREE(msg);                /* Release message */
    }
    return res;
}

/**
 * \brief           Process events in case of timeout on command or invalid message (if device is not present)
 *
 *                  Function is called from processing thread:
 *
 *                      - On command timeout error
 *                      - If command was sent to queue and before processed, device present status changed
 *
 * \param[in]       msg: Current message
 * \param[in]       err: Error message to send
 */
void
lwespi_process_events_for_timeout_or_error(lwesp_msg_t* msg, lwespr_t err) {
    switch (msg->cmd_def) {
        case LWESP_CMD_RESET: {
            /* Error on reset */
            RESET_SEND_EVT(msg, err);
            break;
        }

        case LWESP_CMD_RESTORE: {
            /* Error on restore */
            RESTORE_SEND_EVT(msg, err);
            break;
        }

        case LWESP_CMD_TCPIP_CIPSTART: {
            /* Start connection error */
            lwespi_send_conn_error_cb(msg, err);
            break;
        }

        case LWESP_CMD_TCPIP_CIPSEND: {
            /* Send data error */
            CONN_SEND_DATA_SEND_EVT(msg, err);
            break;
        }

#if LWESP_CFG_MODE_STATION
        case LWESP_CMD_WIFI_CWJAP: {
            /* Join access point error */
            STA_JOIN_AP_SEND_EVT(msg, err);
            break;
        }

        case LWESP_CMD_WIFI_CWLAP: {
            /* List failed event */
            STA_LIST_AP_SEND_EVT(msg, err);
            break;
        }

        case LWESP_CMD_WIFI_CWJAP_GET: {
            /* Info failed event */
            STA_INFO_AP_SEND_EVT(msg, err);
            break;
        }
#endif /* LWESP_CFG_MODE_STATION */

#if LWESP_CFG_PING
        case LWESP_CMD_TCPIP_PING: {
            /* Ping error */
            PING_SEND_EVT(msg, err);
            break;
        }
#endif /* LWESP_CFG_PING */

#if LWESP_CFG_DNS
        case LWESP_CMD_TCPIP_CIPDOMAIN: {
            /* DNS error */
            CIPDOMAIN_SEND_EVT(msg, err);
            break;
        }
#endif /* LWESP_CFG_DNS */

#if LWESP_CFG_SNTP
        case LWESP_CMD_TCPIP_CIPSNTPTIME: {
            /* SNTP error */
            SNTP_TIME_SEND_EVT(msg, err);
            break;
        }
#endif /* LWESP_CFG_SNTP */

        default:
            break;
    }
}

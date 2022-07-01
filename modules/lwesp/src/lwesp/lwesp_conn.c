/**
 * \file            lwesp_conn.c
 * \brief           Connection API
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
#include "lwesp/lwesp_conn.h"
#include "lwesp/lwesp_mem.h"
#include "lwesp/lwesp_timeout.h"

/**
 * \brief           Check if connection is closed or in closing state
 * \param[in]       conn: Connection handle
 * \hideinitializer
 */
#define CONN_CHECK_CLOSED_IN_CLOSING(conn) do { \
        lwespr_t r = lwespOK;                           \
        lwesp_core_lock();                            \
        if (conn->status.f.in_closing || !conn->status.f.active) {  \
            r = lwespCLOSED;                          \
        }                                           \
        lwesp_core_unlock();                          \
        if (r != lwespOK) {                           \
            return r;                               \
        }                                           \
    } while (0)

/**
 * \brief           Timeout callback for connection
 * \param[in]       arg: Timeout callback custom argument
 */
static void
conn_timeout_cb(void* arg) {
    lwesp_conn_p conn = arg;                    /* Argument is actual connection */

    if (conn->status.f.active) {                /* Handle only active connections */
        esp.evt.type = LWESP_EVT_CONN_POLL;     /* Poll connection event */
        esp.evt.evt.conn_poll.conn = conn;      /* Set connection pointer */
        lwespi_send_conn_cb(conn, NULL);        /* Send connection callback */

        lwespi_conn_start_timeout(conn);        /* Schedule new timeout */
       // LWESP_DEBUGF(LWESP_CFG_DBG_CONN | LWESP_DBG_TYPE_TRACE,
       //              "[LWESP CONN] Poll event: %p\r\n", conn);
    }

#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
    lwespi_conn_manual_tcp_try_read_data(conn); /* Try to read data manually */
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */
}

/**
 * \brief           Start timeout function for connection
 * \param[in]       conn: Connection handle as user argument
 */
void
lwespi_conn_start_timeout(lwesp_conn_p conn) {
    lwesp_timeout_add(LWESP_CFG_CONN_POLL_INTERVAL, conn_timeout_cb, conn); /* Add connection timeout */
}

#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE

/**
 * \brief           Callback function when manual TCP receive finishes
 * \param[in]       res: Result of reading
 * \param[in]       arg: Custom user argument
 */
static void
manual_tcp_read_data_evt_fn(lwespr_t res, void* arg) {
    lwesp_conn_p conn = arg;

    conn->status.f.receive_is_command_queued = 0;
    lwespi_conn_manual_tcp_try_read_data(conn);
}

/**
 * \brief           Manually start data read operation with desired length on specific connection
 * \param[in]       conn: Connection handle
 * \param[in]       len: Number of bytes to read
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwespi_conn_manual_tcp_try_read_data(lwesp_conn_p conn) {
    uint32_t blocking = 0;
    lwespr_t res = lwespOK;
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("conn != NULL", conn != NULL);

    /* Receive must not be blocked and other command must not be in queue to read data */
    if (conn->status.f.receive_blocked
        || conn->status.f.receive_is_command_queued) {
        return lwespINPROG;
    }

    /* Any available data to process? */
    if (conn->tcp_available_bytes == 0
        || !conn->status.f.active) {
        return lwespERR;
    }

    LWESP_MSG_VAR_ALLOC(msg, blocking);         /* Allocate first, will return on failure */
    LWESP_MSG_VAR_SET_EVT(msg, manual_tcp_read_data_evt_fn, conn);  /* Set event callback function */
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPRECVDATA;
    LWESP_MSG_VAR_REF(msg).cmd = LWESP_CMD_TCPIP_CIPRECVLEN;
    LWESP_MSG_VAR_REF(msg).msg.ciprecvdata.len = 0; /* Filled after RECVLEN received */
    LWESP_MSG_VAR_REF(msg).msg.ciprecvdata.buff = NULL; /* Filled after RECVLEN received */
    LWESP_MSG_VAR_REF(msg).msg.ciprecvdata.conn = conn;

    /* Try to start command */
    if ((res = lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 60000)) == lwespOK) {
        conn->status.f.receive_is_command_queued = 1;   /* Command queued */
    }
    return res;
}

/**
 * \brief           Callback function for checking receive length in manual TCP receive buffer
 * \param[in]       res: Result of reading
 * \param[in]       arg: Custom user argument
 */
static void
check_available_rx_data_evt_fn(lwespr_t res, void* arg) {
    /* Try to read data if possible */
    if (res != lwespOK) {
        LWESP_DEBUGF(LWESP_CFG_DBG_CONN | LWESP_DBG_TYPE_TRACE,
                         "check_available_rx_data_evt_fn res: %d\r\n", res);
        return;
    }
    for (size_t i = 0; i < LWESP_CFG_MAX_CONNS; ++i) {
        lwespi_conn_manual_tcp_try_read_data(&esp.m.conns[i]);
    }
}

/**
 * \brief           Manually check for received buffer status for connections
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
lwespr_t
lwespi_conn_check_available_rx_data(void) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, 0);                /* Allocate first, will return on failure */
    LWESP_MSG_VAR_SET_EVT(msg, check_available_rx_data_evt_fn, NULL);   /* Set event callback function */
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPRECVLEN;
    LWESP_DEBUGF(LWESP_CFG_DBG_CONN | LWESP_DBG_TYPE_TRACE,
                         "lwespi_conn_check_available_rx_data msg: %p\r\n", (void*)msg);

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */

/**
 * \brief           Get connection validation ID
 * \param[in]       conn: Connection handle
 * \return          Connection current validation ID
 */
uint8_t
lwespi_conn_get_val_id(lwesp_conn_p conn) {
    uint8_t val_id;
    lwesp_core_lock();
    val_id = conn->val_id;
    lwesp_core_unlock();

    return val_id;
}

/**
 * \brief           Send data on already active connection of type UDP to specific remote IP and port
 * \note            In case IP and port values are not set, it will behave as normal send function (suitable for TCP too)
 * \param[in]       conn: Pointer to connection to send data
 * \param[in]       ip: Remote IP address for UDP connection
 * \param[in]       port: Remote port connection
 * \param[in]       data: Pointer to data to send
 * \param[in]       btw: Number of bytes to send
 * \param[out]      bw: Pointer to output variable to save number of sent data when successfully sent
 * \param[in]       fau: "Free After Use" flag. Set to `1` if stack should free the memory after data sent
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
static lwespr_t
conn_send(lwesp_conn_p conn, const lwesp_ip_t* const ip, lwesp_port_t port, const void* data,
          size_t btw, size_t* const bw, uint8_t fau, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("conn != NULL", conn != NULL);
    LWESP_ASSERT("data != NULL", data != NULL);
    LWESP_ASSERT("btw > 0", btw > 0);

    CONN_CHECK_CLOSED_IN_CLOSING(conn);         /* Check if we can continue */

#if !LWESP_CFG_CONN_ALLOW_FRAGMENTED_UDP_SEND
    /*
     * For UDP connections, do not allow fragmented data send.
     *
     * Limit up to maximum buffer allowed by ESP
     */
    if (conn->type == LWESP_CONN_TYPE_UDP) {
        LWESP_ASSERT("USD: len < max_len", btw <= LWESP_CFG_CONN_MAX_DATA_LEN);
    }
#endif /* !LWESP_CFG_CONN_ALLOW_FRAGMENTED_UDP_SEND */

    if (bw != NULL) {
        *bw = 0;
    }
    LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
        "conn_send btw:%d\r\n", btw);


    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPSEND;

    LWESP_MSG_VAR_REF(msg).msg.conn_send.conn = conn;
    LWESP_MSG_VAR_REF(msg).msg.conn_send.data = data;
    LWESP_MSG_VAR_REF(msg).msg.conn_send.btw = btw;
    LWESP_MSG_VAR_REF(msg).msg.conn_send.bw = bw;
    LWESP_MSG_VAR_REF(msg).msg.conn_send.remote_ip = ip;
    LWESP_MSG_VAR_REF(msg).msg.conn_send.remote_port = port;
    LWESP_MSG_VAR_REF(msg).msg.conn_send.fau = fau;
    LWESP_MSG_VAR_REF(msg).msg.conn_send.val_id = lwespi_conn_get_val_id(conn);

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 60000);
}

/**
 * \brief           Flush buffer on connection
 * \param[in]       conn: Connection to flush buffer on
 * \return          \ref lwespOK if data flushed and put to queue, member of \ref lwespr_t otherwise
 */
static lwespr_t
flush_buff(lwesp_conn_p conn) {
    lwespr_t res = lwespOK;
    lwesp_core_lock();
    if (conn != NULL && conn->buff.buff != NULL) {  /* Do we have something ready? */
        /*
         * If there is nothing to write or if write was not successful,
         * simply free the memory and stop execution
         */
        if (conn->buff.ptr > 0) {               /* Anything to send at the moment? */
            res = conn_send(conn, NULL, 0, conn->buff.buff, conn->buff.ptr, NULL, 1, 0);
        } else {
            res = lwespERR;
        }
        if (res != lwespOK) {
            LWESP_DEBUGF(LWESP_CFG_DBG_CONN | LWESP_DBG_TYPE_TRACE,
                         "[LWESP CONN] Free write buffer: %p\r\n", (void*)conn->buff.buff);
            lwesp_mem_free_s((void**)&conn->buff.buff);
        }
        conn->buff.buff = NULL;
    }
    lwesp_core_unlock();
    return res;
}

static lwespr_t
flush_buff_restrict(lwesp_conn_p conn, size_t *sent) {
    lwespr_t res = lwespOK;
    lwesp_core_lock();
    if (conn != NULL && conn->buff.buff != NULL) {  /* Do we have something ready? */
        /*
         * If there is nothing to write or if write was not successful,
         * simply free the memory and stop execution
         */
        if (conn->buff.ptr > 0) {               /* Anything to send at the moment? */
            res = conn_send(conn, NULL, 0, conn->buff.buff, conn->buff.ptr, sent, 1, 1);
            if (res == lwespOK)
                conn->buff.buff = NULL;
        }          
    }
    lwesp_core_unlock();
    return res;
}


/**
 * \brief           Initialize connection module
 */
void
lwespi_conn_init(void) {

}

/**
 * \brief           Start a new connection of specific type
 * \param[out]      conn: Pointer to connection handle to set new connection reference in case of successfully connected
 * \param[in]       type: Connection type. This parameter can be a value of \ref lwesp_conn_type_t enumeration
 * \param[in]       remote_host: Connection host. In case of IP, write it as string, ex. "192.168.1.1"
 * \param[in]       remote_port: Connection port
 * \param[in]       arg: Pointer to user argument passed to connection if successfully connected
 * \param[in]       conn_evt_fn: Callback function for this connection
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_conn_start(lwesp_conn_p* conn, lwesp_conn_type_t type, uint8_t linkid, const char* const remote_host, lwesp_port_t remote_port,
                 void* const arg, lwesp_evt_fn conn_evt_fn, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("remote_host != NULL", remote_host != NULL);
    LWESP_ASSERT("remote_port > 0", remote_port > 0);
    LWESP_ASSERT("conn_evt_fn != NULL", conn_evt_fn != NULL);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPSTART;
    LWESP_MSG_VAR_REF(msg).cmd = lwespi_get_cipstatus_or_cipstate_cmd();
    LWESP_MSG_VAR_REF(msg).msg.conn_start.conn = conn;
    LWESP_MSG_VAR_REF(msg).msg.conn_start.type = type;
    LWESP_MSG_VAR_REF(msg).msg.conn_start.remote_host = remote_host;
    LWESP_MSG_VAR_REF(msg).msg.conn_start.remote_port = remote_port;
    LWESP_MSG_VAR_REF(msg).msg.conn_start.evt_func = conn_evt_fn;
    LWESP_MSG_VAR_REF(msg).msg.conn_start.arg = arg;
    LWESP_MSG_VAR_REF(msg).msg.conn_start.linkid = linkid;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 60000);
}

/**
 * \brief           Start a new connection of specific type in extended mode
 * \param[out]      conn: Pointer to connection handle to set new connection reference in case of successfully connected
 * \param[in]       start_struct: Connection information are handled by one giant structure
 * \param[in]       arg: Pointer to user argument passed to connection if successfully connected
 * \param[in]       conn_evt_fn: Callback function for this connection
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_conn_startex(lwesp_conn_p* conn, lwesp_conn_start_t* start_struct,
                   void* const arg, lwesp_evt_fn conn_evt_fn, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("start_struct != NULL", start_struct != NULL);
    LWESP_ASSERT("start_struct->remote_host != NULL", start_struct->remote_host != NULL);
    LWESP_ASSERT("start_struct->remote_port > 0", start_struct->remote_port > 0);
    LWESP_ASSERT("conn_evt_fn != NULL", conn_evt_fn != NULL);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPSTART;
    LWESP_MSG_VAR_REF(msg).cmd = lwespi_get_cipstatus_or_cipstate_cmd();
    LWESP_MSG_VAR_REF(msg).msg.conn_start.conn = conn;
    LWESP_MSG_VAR_REF(msg).msg.conn_start.type = start_struct->type;
    LWESP_MSG_VAR_REF(msg).msg.conn_start.remote_host = start_struct->remote_host;
    LWESP_MSG_VAR_REF(msg).msg.conn_start.remote_port = start_struct->remote_port;
    LWESP_MSG_VAR_REF(msg).msg.conn_start.local_ip = start_struct->local_ip;
    LWESP_MSG_VAR_REF(msg).msg.conn_start.evt_func = conn_evt_fn;
    LWESP_MSG_VAR_REF(msg).msg.conn_start.arg = arg;

    /* Add connection type specific features */
    if (start_struct->type != LWESP_CONN_TYPE_UDP) {
        LWESP_MSG_VAR_REF(msg).msg.conn_start.tcp_ssl_keep_alive = start_struct->ext.tcp_ssl.keep_alive;
    } else {
        LWESP_MSG_VAR_REF(msg).msg.conn_start.udp_local_port = start_struct->ext.udp.local_port;
        LWESP_MSG_VAR_REF(msg).msg.conn_start.udp_mode = start_struct->ext.udp.mode;
    }
    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 60000);
}

/**
 * \brief           Close specific or all connections
 * \param[in]       conn: Connection handle to close. Set to NULL if you want to close all connections.
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_conn_close(lwesp_conn_p conn, const uint32_t blocking) {
    lwespr_t res;
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("conn != NULL", conn != NULL);

    CONN_CHECK_CLOSED_IN_CLOSING(conn);         /* Check if we can continue */

    /* Proceed with close event at this point! */
    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPCLOSE;
    LWESP_MSG_VAR_REF(msg).msg.conn_close.conn = conn;
    LWESP_MSG_VAR_REF(msg).msg.conn_close.val_id = lwespi_conn_get_val_id(conn);

    flush_buff(conn);                           /* First flush buffer */
    res = lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
    if (res == lwespOK && !blocking) {          /* Function succedded in non-blocking mode */
        lwesp_core_lock();
        LWESP_DEBUGF(LWESP_CFG_DBG_CONN | LWESP_DBG_TYPE_TRACE,
                     "[LWESP CONN] Connection %d set to closing state\r\n", (int)conn->num);
        conn->status.f.in_closing = 1;          /* Connection is in closing mode but not yet closed */
        lwesp_core_unlock();
    }
    return res;
}

/**
 * \brief           Send data on active connection of type UDP to specific remote IP and port
 * \note            In case IP and port values are not set, it will behave as normal send function (suitable for TCP too)
 * \param[in]       conn: Connection handle to send data
 * \param[in]       ip: Remote IP address for UDP connection
 * \param[in]       port: Remote port connection
 * \param[in]       data: Pointer to data to send
 * \param[in]       btw: Number of bytes to send
 * \param[out]      bw: Pointer to output variable to save number of sent data when successfully sent
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_conn_sendto(lwesp_conn_p conn, const lwesp_ip_t* const ip, lwesp_port_t port, const void* data,
                  size_t btw, size_t* bw, const uint32_t blocking) {
    LWESP_ASSERT("conn != NULL", conn != NULL);

    flush_buff(conn);                           /* Flush currently written memory if exists */
    return conn_send(conn, ip, port, data, btw, bw, 0, blocking);
}

/**
 * \brief           Send data on already active connection either as client or server
 * \param[in]       conn: Connection handle to send data
 * \param[in]       data: Data to send
 * \param[in]       btw: Number of bytes to send
 * \param[out]      bw: Pointer to output variable to save number of sent data when successfully sent.
 *                      Parameter value might not be accurate if you combine \ref lwesp_conn_write and \ref lwesp_conn_send functions
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_conn_send(lwesp_conn_p conn, const void* data, size_t btw, size_t* const bw,
                const uint32_t blocking) {
    lwespr_t res;
    const uint8_t* d = data;
    size_t sent = 0;
    LWESP_ASSERT("conn != NULL", conn != NULL);
    LWESP_ASSERT("data != NULL", data != NULL);
    LWESP_ASSERT("btw > 0", btw > 0);
    if (bw != NULL)
        *bw = 0;
    lwesp_core_lock();
    if (conn->buff.buff != NULL) {              /* Check if memory available */
        size_t to_copy;
        to_copy = LWESP_MIN(btw, conn->buff.len - conn->buff.ptr);
        if (to_copy > 0) {
            LWESP_MEMCPY(&conn->buff.buff[conn->buff.ptr], d, to_copy);
            conn->buff.ptr += to_copy;
            d += to_copy;
            btw -= to_copy;
        }
    }
    lwesp_core_unlock();
    res = flush_buff_restrict(conn, &sent);                     /* Flush currently written memory if exists */
    if (res == lwespOK && bw != NULL) {
        *bw += sent;
    } else if (res != lwespOK) {
        return res;
    }
    LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
    "lwesp_conn_send btw:%d\r\n", btw);


    if (btw > 0) {                              /* Check for remaining data */
        res = conn_send(conn, NULL, 0, d, btw, &sent, 0, blocking);
        if (res == lwespOK && bw != NULL)
            *bw += sent;
    }
    return res;
}

/**
 * \brief           Notify connection about received data which means connection is ready to accept more data
 *
 * Once data reception is confirmed, stack will try to send more data to user.
 *
 * \note            Since this feature is not supported yet by AT commands, function is only prototype
 *                  and should be used in connection callback when data are received
 *
 * \note            Function is not thread safe and may only be called from connection event function
 *
 * \param[in]       conn: Connection handle
 * \param[in]       pbuf: Packet buffer received on connection
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_conn_recved(lwesp_conn_p conn, lwesp_pbuf_p pbuf) {
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
    size_t len;
    len = lwesp_pbuf_length(pbuf, 1);           /* Get length of pbuf */
    if (conn->tcp_not_ack_bytes >= len) {       /* Check length of not-acknowledged bytes */
        conn->tcp_not_ack_bytes -= len;
    } else {
        /* Warning here, de-sync happened somewhere! */
    }
    lwespi_conn_manual_tcp_try_read_data(conn); /* Try to read more connection data */
#else /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */
    LWESP_UNUSED(conn);
    LWESP_UNUSED(pbuf);
#endif /* !LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */
    return lwespOK;
}

/**
 * \brief           Set argument variable for connection
 * \param[in]       conn: Connection handle to set argument
 * \param[in]       arg: Pointer to argument
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 * \sa              lwesp_conn_get_arg
 */
lwespr_t
lwesp_conn_set_arg(lwesp_conn_p conn, void* const arg) {
    lwesp_core_lock();
    conn->arg = arg;                            /* Set argument for connection */
    lwesp_core_unlock();
    return lwespOK;
}

/**
 * \brief           Get user defined connection argument
 * \param[in]       conn: Connection handle to get argument
 * \return          User argument
 * \sa              lwesp_conn_set_arg
 */
void*
lwesp_conn_get_arg(lwesp_conn_p conn) {
    void* arg;
    lwesp_core_lock();
    arg = conn->arg;                            /* Set argument for connection */
    lwesp_core_unlock();
    return arg;
}

/**
 * \brief           Gets connections status
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_get_conns_status(const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPSTATUS;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Check if connection type is client
 * \param[in]       conn: Pointer to connection to check for status
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_conn_is_client(lwesp_conn_p conn) {
    uint8_t res = 0;
    if (conn != NULL && lwespi_is_valid_conn_ptr(conn)) {
        lwesp_core_lock();
        res = conn->status.f.active && conn->status.f.client;
        lwesp_core_unlock();
    }
    return res;
}

/**
 * \brief           Check if connection type is server
 * \param[in]       conn: Pointer to connection to check for status
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_conn_is_server(lwesp_conn_p conn) {
    uint8_t res = 0;
    if (conn != NULL && lwespi_is_valid_conn_ptr(conn)) {
        lwesp_core_lock();
        res = conn->status.f.active && !conn->status.f.client;
        lwesp_core_unlock();
    }
    return res;
}

/**
 * \brief           Check if connection is active
 * \param[in]       conn: Pointer to connection to check for status
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_conn_is_active(lwesp_conn_p conn) {
    uint8_t res = 0;
    if (conn != NULL && lwespi_is_valid_conn_ptr(conn)) {
        lwesp_core_lock();
        res = conn->status.f.active;
        lwesp_core_unlock();
    }
    return res;
}

/**
 * \brief           Check if connection is closed
 * \param[in]       conn: Pointer to connection to check for status
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_conn_is_closed(lwesp_conn_p conn) {
    uint8_t res = 0;
    if (conn != NULL && lwespi_is_valid_conn_ptr(conn)) {
        lwesp_core_lock();
        res = !conn->status.f.active;
        lwesp_core_unlock();
    }
    return res;
}

/**
 * \brief           Get the number from connection
 * \param[in]       conn: Connection pointer
 * \return          Connection number in case of success or -1 on failure
 */
int8_t
lwesp_conn_getnum(lwesp_conn_p conn) {
    int8_t res = -1;
    if (conn != NULL && lwespi_is_valid_conn_ptr(conn)) {
        /* Protection not needed as every connection has always the same number */
        res = conn->num;                        /* Get number */
    }
    return res;
}

/**
 * \brief           Set internal buffer size for SSL connection on ESP device
 * \note            Use this function before you start first SSL connection
 * \param[in]       size: Size of buffer in units of bytes. Valid range is between 2048 and 4096 bytes
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_conn_set_ssl_buffersize(size_t size, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPSSLSIZE;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_sslsize.size = size;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Get connection from connection based event
 * \param[in]       evt: Event which happened for connection
 * \return          Connection pointer on success, `NULL` otherwise
 */
lwesp_conn_p
lwesp_conn_get_from_evt(lwesp_evt_t* evt) {
    switch (evt->type) {
        case LWESP_EVT_CONN_ACTIVE:
            return lwesp_evt_conn_active_get_conn(evt);
        case LWESP_EVT_CONN_CLOSE:
            return lwesp_evt_conn_close_get_conn(evt);
        case LWESP_EVT_CONN_RECV:
            return lwesp_evt_conn_recv_get_conn(evt);
        case LWESP_EVT_CONN_SEND:
            return lwesp_evt_conn_send_get_conn(evt);
        case LWESP_EVT_CONN_POLL:
            return lwesp_evt_conn_poll_get_conn(evt);
        default:
            return NULL;
    }
}

/**
 * \brief           Write data to connection buffer and if it is full, send it non-blocking way
 * \note            This function may only be called from core (connection callbacks)
 * \param[in]       conn: Connection to write
 * \param[in]       data: Data to copy to write buffer
 * \param[in]       btw: Number of bytes to write
 * \param[in]       flush: Flush flag. Set to `1` if you want to send data immediately after copying
 * \param[out]      mem_available: Available memory size available in current write buffer.
 *                  When the buffer length is reached, current one is sent and a new one is automatically created.
 *                  If function returns \ref lwespOK and `*mem_available = 0`, there was a problem
 *                  allocating a new buffer for next operation
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_conn_write(lwesp_conn_p conn, const void* data, size_t btw, uint8_t flush,
                 size_t* const mem_available) {
    size_t len;

    const uint8_t* d = data;

    LWESP_ASSERT("conn != NULL", conn != NULL);

    /*
     * Steps during write process:
     *
     * 1. Check if we have buffer already allocated and
     *      write data to the tail of buffer
     *   1.1. In case buffer is full, send it non-blocking,
     *      and enable freeing after it is sent
     * 2. Check how many bytes we can copy as single buffer directly and send
     * 3. Create last buffer and copy remaining data to it even if no remaining data
     *      This is useful when calling function with no parameters (len = 0)
     * 4. Flush (send) current buffer if necessary
     */

    /* Step 1 */
    if (conn->buff.buff != NULL) {
        len = LWESP_MIN(conn->buff.len - conn->buff.ptr, btw);
        LWESP_MEMCPY(&conn->buff.buff[conn->buff.ptr], d, len);

        d += len;
        btw -= len;
        conn->buff.ptr += len;

        /* Step 1.1 */
        if (conn->buff.ptr == conn->buff.len || flush) {
            /* Try to send to processing queue in non-blocking way */
            if (conn_send(conn, NULL, 0, conn->buff.buff, conn->buff.ptr, NULL, 1, 0) != lwespOK) {
                LWESP_DEBUGF(LWESP_CFG_DBG_CONN | LWESP_DBG_TYPE_TRACE,
                             "[LWESP CONN] Free write buffer: %p\r\n", conn->buff.buff);
                lwesp_mem_free_s((void**)&conn->buff.buff);
            }
            conn->buff.buff = NULL;
        }
    }

    /* Step 2 */
    while (btw >= LWESP_CFG_CONN_MAX_DATA_LEN) {
        uint8_t* buff;
        buff = lwesp_mem_malloc(sizeof(*buff) * LWESP_CFG_CONN_MAX_DATA_LEN);
        if (buff != NULL) {
            LWESP_MEMCPY(buff, d, LWESP_CFG_CONN_MAX_DATA_LEN); /* Copy data to buffer */
            if (conn_send(conn, NULL, 0, buff, LWESP_CFG_CONN_MAX_DATA_LEN, NULL, 1, 0) != lwespOK) {
                LWESP_DEBUGF(LWESP_CFG_DBG_CONN | LWESP_DBG_TYPE_TRACE,
                             "[LWESP CONN] Free write buffer: %p\r\n", (void*)buff);
                lwesp_mem_free_s((void**)&buff);
                return lwespERRMEM;
            }
        } else {
            return lwespERRMEM;
        }

        btw -= LWESP_CFG_CONN_MAX_DATA_LEN;     /* Decrease remaining length */
        d += LWESP_CFG_CONN_MAX_DATA_LEN;       /* Advance data pointer */
    }

    /* Step 3 */
    if (conn->buff.buff == NULL) {
        conn->buff.buff = lwesp_mem_malloc(sizeof(*conn->buff.buff) * LWESP_CFG_CONN_MAX_DATA_LEN);
        conn->buff.len = LWESP_CFG_CONN_MAX_DATA_LEN;
        conn->buff.ptr = 0;

        LWESP_DEBUGW(LWESP_CFG_DBG_CONN | LWESP_DBG_TYPE_TRACE, conn->buff.buff != NULL,
                     "[LWESP CONN] New write buffer allocated, addr = %p\r\n", conn->buff.buff);
        LWESP_DEBUGW(LWESP_CFG_DBG_CONN | LWESP_DBG_TYPE_TRACE, conn->buff.buff == NULL,
                     "[LWESP CONN] Cannot allocate new write buffer\r\n");
    }
    if (btw > 0) {
        if (conn->buff.buff != NULL) {
            LWESP_MEMCPY(conn->buff.buff, d, btw);  /* Copy data to memory */
            conn->buff.ptr = btw;
        } else {
            return lwespERRMEM;
        }
    }

    /* Step 4 */
    if (flush && conn->buff.buff != NULL) {
        flush_buff(conn);
    }

    /* Calculate number of available memory after write operation */
    if (mem_available != NULL) {
        if (conn->buff.buff != NULL) {
            *mem_available = conn->buff.len - conn->buff.ptr;
        } else {
            *mem_available = 0;
        }
    }
    return lwespOK;
}

/**
 * \brief           Get total number of bytes ever received on connection and sent to user
 * \param[in]       conn: Connection handle
 * \return          Total number of received bytes on connection
 */
size_t
lwesp_conn_get_total_recved_count(lwesp_conn_p conn) {
    size_t tot = 0;

    if (conn != NULL) {
        lwesp_core_lock();
        tot = conn->total_recved;               /* Get total received bytes */
        lwesp_core_unlock();
    }
    return tot;
}

/**
 * \brief           Get connection remote IP address
 * \param[in]       conn: Connection handle
 * \param[out]      ip: Pointer to IP output handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_conn_get_remote_ip(lwesp_conn_p conn, lwesp_ip_t* ip) {
    if (conn != NULL && ip != NULL) {
        lwesp_core_lock();
        LWESP_MEMCPY(ip, &conn->remote_ip, sizeof(*ip));/* Copy data */
        lwesp_core_unlock();
        return 1;
    }
    return 0;
}

/**
 * \brief           Get connection remote port number
 * \param[in]       conn: Connection handle
 * \return          Port number on success, `0` otherwise
 */
lwesp_port_t
lwesp_conn_get_remote_port(lwesp_conn_p conn) {
    lwesp_port_t port = 0;
    if (conn != NULL) {
        lwesp_core_lock();
        port = conn->remote_port;
        lwesp_core_unlock();
    }
    return port;
}

/**
 * \brief           Get connection local port number
 * \param[in]       conn: Connection handle
 * \return          Port number on success, `0` otherwise
 */
lwesp_port_t
lwesp_conn_get_local_port(lwesp_conn_p conn) {
    lwesp_port_t port = 0;
    if (conn != NULL) {
        lwesp_core_lock();
        port = conn->local_port;
        lwesp_core_unlock();
    }
    return port;
}

/**
 * \brief           Configure SSL parameters
 * \param[in]       link_id: ID of the connection (0~max), for multiple connections, if the value is max, it means all connections.
 *                      By default, max is \ref LWESP_CFG_MAX_CONNS.
 * \param[in]       auth_mode: Authentication mode
 *                      `0`: no authorization
 *                      `1`: load cert and private key for server authorization
 *                      `2`: load CA for client authorize server cert and private key
 *                      `3`: both authorization
 * \param[in]       pki_number:  The index of cert and private key, if only one cert and private key, the value should be `0`.
 * \param[in]       ca_number: The index of CA, if only one CA, the value should be 0.
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_conn_ssl_set_config(uint8_t link_id, uint8_t auth_mode, uint8_t pki_number, uint8_t ca_number,
                          const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPSSLCCONF;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_ssl_cfg.link_id = LWESP_MIN(link_id, LWESP_CFG_MAX_CONNS);
    LWESP_MSG_VAR_REF(msg).msg.tcpip_ssl_cfg.auth_mode = LWESP_MIN(auth_mode, 3);
    LWESP_MSG_VAR_REF(msg).msg.tcpip_ssl_cfg.pki_number = pki_number;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_ssl_cfg.ca_number = ca_number;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

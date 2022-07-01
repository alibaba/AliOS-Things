/**
 * \file            lwesp_netconn.c
 * \brief           API functions for sequential calls
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
#include "lwesp/lwesp_netconn.h"
#include "lwesp/lwesp_private.h"
#include "lwesp/lwesp_conn.h"
#include "lwesp/lwesp_mem.h"

#if LWESP_CFG_NETCONN || __DOXYGEN__

/* Check conditions */
#if LWESP_CFG_NETCONN_RECEIVE_QUEUE_LEN < 2
#error "LWESP_CFG_NETCONN_RECEIVE_QUEUE_LEN must be greater or equal to 2"
#endif /* LWESP_CFG_NETCONN_RECEIVE_QUEUE_LEN < 2 */

#if LWESP_CFG_NETCONN_ACCEPT_QUEUE_LEN < 2
#error "LWESP_CFG_NETCONN_ACCEPT_QUEUE_LEN must be greater or equal to 2"
#endif /* LWESP_CFG_NETCONN_ACCEPT_QUEUE_LEN < 2 */

/* Check for IP status */
#if LWESP_CFG_IPV6
#define NETCONN_IS_TCP(nc)          (nc->type == LWESP_NETCONN_TYPE_TCP || nc->type == LWESP_NETCONN_TYPE_TCPV6)
#define NETCONN_IS_SSL(nc)          (nc->type == LWESP_NETCONN_TYPE_SSL || nc->type == LWESP_NETCONN_TYPE_SSLV6)
#else
#define NETCONN_IS_TCP(nc)          (nc->type == LWESP_NETCONN_TYPE_TCP)
#define NETCONN_IS_SSL(nc)          (nc->type == LWESP_NETCONN_TYPE_SSL)
#endif /* LWESP_CFG_IPV6 */

/**
 * \brief           Sequential API structure
 */
typedef struct lwesp_netconn {
    struct lwesp_netconn* next;                 /*!< Linked list entry */

    lwesp_netconn_type_t type;                  /*!< Netconn type */
    lwesp_port_t listen_port;                   /*!< Port on which we are listening */

    size_t rcv_packets;                         /*!< Number of received packets so far on this connection */
    lwesp_conn_p conn;                          /*!< Pointer to actual connection */

    lwesp_sys_mbox_t mbox_accept;               /*!< List of active connections waiting to be processed */
    lwesp_sys_mbox_t mbox_receive;              /*!< Message queue for receive mbox */
    size_t mbox_receive_entries;                /*!< Number of entries written to receive mbox */

    lwesp_linbuff_t buff;                       /*!< Linear buffer structure */

    uint16_t conn_timeout;                      /*!< Connection timeout in units of seconds when
                                                    netconn is in server (listen) mode.
                                                    Connection will be automatically closed if there is no
                                                    data exchange in time. Set to `0` when timeout feature is disabled. */
    uint8_t linkid;
#if LWESP_CFG_NETCONN_RECEIVE_TIMEOUT || __DOXYGEN__
    uint32_t rcv_timeout;                       /*!< Receive timeout in unit of milliseconds */
#endif
    uint32_t accept_timeout;
    uint32_t connect_timeout; /*client connect timeout*/
} lwesp_netconn_t;

static uint8_t recv_closed = 0xFF, recv_not_present = 0xFF;
static lwesp_netconn_t* listen_api;             /*!< Main connection in listening mode */
static lwesp_netconn_t* netconn_list;           /*!< Linked list of netconn entries */

#define MAX_NETCONN_NUM (LWESP_CFG_MAX_CONNS)
struct lwesp_netconn_table {
    lwesp_netconn_t table[MAX_NETCONN_NUM];
    uint32_t occupy_status; 
};
struct lwesp_netconn_table netconn_table = {0};

static inline int local_ffs(int x)
{
	int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}
lwesp_netconn_t * alloc_lwesp_netconn(lwesp_netconn_type_t type) 
{
    (void)type;
    lwesp_netconn_t *netconn;
    
    lwesp_core_lock();
    uint32_t status = netconn_table.occupy_status;
    int bit_position;
    bit_position = local_ffs(~status) - 1;
    if (bit_position < 0 || bit_position >= MAX_NETCONN_NUM) {
        lwesp_core_unlock();
        return NULL;
    }
    status = status | 1 << bit_position;
    
    netconn_table.occupy_status = status;
    netconn = &(netconn_table.table[bit_position]);
    memset(netconn, 0, sizeof(lwesp_netconn_t));
    netconn->linkid = bit_position; 
    lwesp_core_unlock();
   
    return netconn;

}
void free_lwesp_netconn(lwesp_netconn_t *netconn)
{
    lwesp_core_lock();
    uint32_t status = netconn_table.occupy_status;
    int bit_position = netconn->linkid;
    if (bit_position < 0 || bit_position >= MAX_NETCONN_NUM) {
        lwesp_core_unlock();
        return;
    }
    status = status & ~(1 << bit_position);
    
    netconn_table.occupy_status = status;
    lwesp_core_unlock();
   
    return;
}
/**
 * \brief           Flush all mboxes and clear possible used memories
 * \param[in]       nc: Pointer to netconn to flush
 * \param[in]       protect: Set to 1 to protect against multi-thread access
 */
static void
flush_mboxes(lwesp_netconn_t* nc, uint8_t protect) {
    lwesp_pbuf_p pbuf;
    lwesp_netconn_t* new_nc;
    if (protect) {
        lwesp_core_lock();
    }
    if (lwesp_sys_mbox_isvalid(&nc->mbox_receive)) {
        while (lwesp_sys_mbox_getnow(&nc->mbox_receive, (void**)&pbuf)) {
            if (nc->mbox_receive_entries > 0) {
                --nc->mbox_receive_entries;
            }
            if (pbuf != NULL && (uint8_t*)pbuf != (uint8_t*)&recv_closed) {
                lwesp_pbuf_free(pbuf);          /* Free received data buffers */
            }
        }
        lwesp_sys_mbox_delete(&nc->mbox_receive);   /* Delete message queue */
        lwesp_sys_mbox_invalid(&nc->mbox_receive);  /* Invalid handle */
    }
    if (lwesp_sys_mbox_isvalid(&nc->mbox_accept)) {
        while (lwesp_sys_mbox_getnow(&nc->mbox_accept, (void**)&new_nc)) {
            if (new_nc != NULL
                && (uint8_t*)new_nc != (uint8_t*)&recv_closed
                && (uint8_t*)new_nc != (uint8_t*)&recv_not_present) {
                lwesp_netconn_close(new_nc);    /* Close netconn connection */
            }
        }
        lwesp_sys_mbox_delete(&nc->mbox_accept);/* Delete message queue */
        lwesp_sys_mbox_invalid(&nc->mbox_accept);   /* Invalid handle */
    }
    if (protect) {
        lwesp_core_unlock();
    }
}

/**
 * \brief           Callback function for every server connection
 * \param[in]       evt: Pointer to callback structure
 * \return          Member of \ref lwespr_t enumeration
 */
static lwespr_t
netconn_evt(lwesp_evt_t* evt) {
    lwesp_conn_p conn;
    lwesp_netconn_t* nc = NULL;
    uint8_t close = 0;

    conn = lwesp_conn_get_from_evt(evt);        /* Get connection from event */
    switch (lwesp_evt_get_type(evt)) {
        /*
         * A new connection has been active
         * and should be handled by netconn API
         */
        case LWESP_EVT_CONN_ACTIVE: {           /* A new connection active is active */
            if (lwesp_conn_is_client(conn)) {   /* Was connection started by us? */
                nc = lwesp_conn_get_arg(conn);  /* Argument should be already set */
                if (nc != NULL) {
                    nc->conn = conn;            /* Save actual connection */
                    conn->type = nc->type;
                } else {
                    close = 1;                  /* Close this connection, invalid netconn */
                }

                /* Is the connection server type and we have known listening API? */
            } else if (lwesp_conn_is_server(conn) && listen_api != NULL) {
                /*
                 * Create a new netconn structure
                 * and set it as connection argument.
                 */
                nc = lwesp_netconn_new(LWESP_NETCONN_TYPE_TCP); /* Create new API */
                LWESP_DEBUGW(LWESP_CFG_DBG_NETCONN | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_WARNING,
                             nc == NULL, "[LWESP NETCONN] Cannot create new structure for incoming server connection!\r\n");

                if (nc != NULL) {
                    nc->conn = conn;            /* Set connection handle */
                    lwesp_conn_set_arg(conn, nc);   /* Set argument for connection */

                    /*
                     * In case there is no listening connection,
                     * simply close the connection
                     */
                    if (!lwesp_sys_mbox_isvalid(&listen_api->mbox_accept)
                        || !lwesp_sys_mbox_putnow(&listen_api->mbox_accept, nc)) {
                        close = 1;
                    }
                } else {
                    close = 1;
                }
            } else {
                LWESP_DEBUGW(LWESP_CFG_DBG_NETCONN | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_WARNING, listen_api == NULL,
                             "[LWESP NETCONN] Closing connection as there is no listening API in netconn!\r\n");
                close = 1;                      /* Close the connection at this point */
            }

            /* Decide if some events want to close the connection */
            if (close) {
                if (nc != NULL) {
                    lwesp_conn_set_arg(conn, NULL); /* Reset argument */
                    lwesp_netconn_delete(nc);   /* Free memory for API */
                }
                lwesp_conn_close(conn, 0);      /* Close the connection */
                close = 0;
            }
            break;
        }

        /*
         * We have a new data received which
         * should have netconn structure as argument
         */
        case LWESP_EVT_CONN_RECV: {
            lwesp_pbuf_p pbuf;

            nc = lwesp_conn_get_arg(conn);      /* Get API from connection */
            pbuf = lwesp_evt_conn_recv_get_buff(evt);   /* Get received buff */

#if !LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
            lwesp_conn_recved(conn, pbuf);      /* Notify stack about received data */
#endif /* !LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */

            lwesp_pbuf_ref(pbuf);               /* Increase reference counter */
            if (nc == NULL || !lwesp_sys_mbox_isvalid(&nc->mbox_receive)
                || !lwesp_sys_mbox_putnow(&nc->mbox_receive, pbuf)) {
                LWESP_DEBUGF(LWESP_CFG_DBG_NETCONN,
                             "[LWESP NETCONN] Ignoring more data for receive!\r\n");
                lwesp_pbuf_free(pbuf);          /* Free pbuf */
                return lwespOKIGNOREMORE;       /* Return OK to free the memory and ignore further data */
            }
            ++nc->mbox_receive_entries;         /* Increase number of packets in receive mbox */
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
            /* Check against 1 less to still allow potential close event to be written to queue */
            if (nc->mbox_receive_entries >= (LWESP_CFG_NETCONN_RECEIVE_QUEUE_LEN - 1)) {
                conn->status.f.receive_blocked = 1; /* Block reading more data */
            }
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */

            ++nc->rcv_packets;                  /* Increase number of packets received */
            LWESP_DEBUGF(LWESP_CFG_DBG_NETCONN | LWESP_DBG_TYPE_TRACE,
                         "[LWESP NETCONN] Received pbuf contains %d bytes. Handle written to receive mbox\r\n",
                         (int)lwesp_pbuf_length(pbuf, 0));
            break;
        }

        /* Connection was just closed */
        case LWESP_EVT_CONN_CLOSE: {
            nc = lwesp_conn_get_arg(conn);      /* Get API from connection */

            /*
             * In case we have a netconn available,
             * simply write pointer to received variable to indicate closed state
             */
            if (nc != NULL && lwesp_sys_mbox_isvalid(&nc->mbox_receive)) {
                if (lwesp_sys_mbox_putnow(&nc->mbox_receive, (void*)&recv_closed)) {
                    ++nc->mbox_receive_entries;
                }
            }

            break;
        }
        default:
            return lwespERR;
    }
    return lwespOK;
}

/**
 * \brief           Global event callback function
 * \param[in]       evt: Callback information and data
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
static lwespr_t
lwesp_evt(lwesp_evt_t* evt) {
    switch (lwesp_evt_get_type(evt)) {
        case LWESP_EVT_WIFI_DISCONNECTED: {     /* Wifi disconnected event */
            if (listen_api != NULL) {           /* Check if listen API active */
                lwesp_sys_mbox_putnow(&listen_api->mbox_accept, &recv_closed);
            }
            break;
        }
        case LWESP_EVT_DEVICE_PRESENT: {        /* Device present event */
            if (listen_api != NULL && !lwesp_device_is_present()) { /* Check if device present */
                lwesp_sys_mbox_putnow(&listen_api->mbox_accept, &recv_not_present);
            }
        }
        default:
            break;
    }
    return lwespOK;
}

/**
 * \brief           Create new netconn connection
 * \param[in]       type: Netconn connection type
 * \return          New netconn connection on success, `NULL` otherwise
 */
lwesp_netconn_p
lwesp_netconn_new(lwesp_netconn_type_t type) {
    lwesp_netconn_t* a;
    static uint8_t first = 1;

    /* Register only once! */
    lwesp_core_lock();
    if (first) {
        first = 0;
        lwesp_evt_register(lwesp_evt);          /* Register global event function */
    }
    lwesp_core_unlock();
    a = alloc_lwesp_netconn(type);        /* Allocate memory for core object */
    if (a != NULL) {
        a->type = type;                         /* Save netconn type */
        a->conn_timeout = 0;                    /* Default connection timeout */
        if (!lwesp_sys_mbox_create(&a->mbox_accept, LWESP_CFG_NETCONN_ACCEPT_QUEUE_LEN)) {  /* Allocate memory for accepting message box */
            LWESP_DEBUGF(LWESP_CFG_DBG_NETCONN | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_DANGER,
                         "[LWESP NETCONN] Cannot create accept MBOX\r\n");
            goto free_ret;
        }
        if (!lwesp_sys_mbox_create(&a->mbox_receive, LWESP_CFG_NETCONN_RECEIVE_QUEUE_LEN)) {/* Allocate memory for receiving message box */
            LWESP_DEBUGF(LWESP_CFG_DBG_NETCONN | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_DANGER,
                         "[LWESP NETCONN] Cannot create receive MBOX\r\n");
            goto free_ret;
        }
        lwesp_core_lock();
        if (netconn_list == NULL) {             /* Add new netconn to the existing list */
            netconn_list = a;
        } else {
            a->next = netconn_list;             /* Add it to beginning of the list */
            netconn_list = a;
        }
        lwesp_core_unlock();
    }
    return a;
free_ret:
    if (lwesp_sys_mbox_isvalid(&a->mbox_accept)) {
        lwesp_sys_mbox_delete(&a->mbox_accept);
        lwesp_sys_mbox_invalid(&a->mbox_accept);
    }
    if (lwesp_sys_mbox_isvalid(&a->mbox_receive)) {
        lwesp_sys_mbox_delete(&a->mbox_receive);
        lwesp_sys_mbox_invalid(&a->mbox_receive);
    }
    if (a != NULL) {
        lwesp_mem_free_s((void**)&a);
    }
    return NULL;
}

/**
 * \brief           Delete netconn connection
 * \param[in]       nc: Netconn handle
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_netconn_delete(lwesp_netconn_p nc) {
    LWESP_ASSERT("netconn != NULL", nc != NULL);

    lwesp_core_lock();
    flush_mboxes(nc, 0);                        /* Clear mboxes */

    /* Stop listening on netconn */
    if (nc == listen_api) {
        listen_api = NULL;
        lwesp_core_unlock();
        lwesp_set_server(0, nc->listen_port, 0, 0, NULL, NULL, NULL, 1);
        lwesp_core_lock();
    }

    /* Remove netconn from linkedlist */
    if (nc == netconn_list) {
        netconn_list = netconn_list->next;      /* Remove first from linked list */
    } else if (netconn_list != NULL) {
        lwesp_netconn_p tmp, prev;
        /* Find element on the list */
        for (prev = netconn_list, tmp = netconn_list->next;
             tmp != NULL; prev = tmp, tmp = tmp->next) {
            if (nc == tmp) {
                prev->next = tmp->next;         /* Remove tmp from linked list */
                break;
            }
        }
    }
    lwesp_core_unlock();

    free_lwesp_netconn(nc);
    return lwespOK;
}

/**
 * \brief           Connect to server as client
 * \param[in]       nc: Netconn handle
 * \param[in]       host: Pointer to host, such as domain name or IP address in string format
 * \param[in]       port: Target port to use
 * \return          \ref lwespOK if successfully connected, member of \ref lwespr_t otherwise
 */
lwespr_t
lwesp_netconn_connect(lwesp_netconn_p nc, const char* host, lwesp_port_t port) {
    lwespr_t res;

    LWESP_ASSERT("nc != NULL", nc != NULL);
    LWESP_ASSERT("host != NULL", host != NULL);
    LWESP_ASSERT("port > 0", port > 0);

    /*
     * Start a new connection as client and:
     *
     *  - Set current netconn structure as argument
     *  - Set netconn callback function for connection management
     *  - Start connection in blocking mode
     */
    if (nc->connect_timeout == LWESP_NETCONN_CONNECT_NO_WAIT) {
        res = lwesp_conn_start(NULL, (lwesp_conn_type_t)nc->type, (uint8_t)nc->linkid, host, port, nc, netconn_evt, 0);
        if (res == lwespOK) {
            res = lwespINPROG;
        }
    } else {
        res = lwesp_conn_start(NULL, (lwesp_conn_type_t)nc->type, (uint8_t)nc->linkid, host, port, nc, netconn_evt, 1);
    }
    return res;
}

/**
 * \brief           Connect to server as client, allow keep-alive option
 * \param[in]       nc: Netconn handle
 * \param[in]       host: Pointer to host, such as domain name or IP address in string format
 * \param[in]       port: Target port to use
 * \param[in]       keep_alive: Keep alive period seconds
 * \param[in]       local_ip: Local ip in connected command
 * \param[in]       local_port: Local port address
 * \param[in]       mode: UDP mode
 * \return          \ref lwespOK if successfully connected, member of \ref lwespr_t otherwise
 */
lwespr_t
lwesp_netconn_connect_ex(lwesp_netconn_p nc, const char* host, lwesp_port_t port,
                         uint16_t keep_alive, const char* local_ip, lwesp_port_t local_port, uint8_t mode) {
    lwesp_conn_start_t cs = {0};
    lwespr_t res;

    LWESP_ASSERT("nc != NULL", nc != NULL);
    LWESP_ASSERT("host != NULL", host != NULL);
    LWESP_ASSERT("port > 0", port > 0);

    /*
     * Start a new connection as client and:
     *
     *  - Set current netconn structure as argument
     *  - Set netconn callback function for connection management
     *  - Start connection in blocking mode
     */
    cs.type = (lwesp_conn_type_t)nc->type;
    cs.remote_host = host;
    cs.remote_port = port;
    cs.local_ip = local_ip;
    if (NETCONN_IS_TCP(nc) || NETCONN_IS_SSL(nc)) {
        cs.ext.tcp_ssl.keep_alive = keep_alive;
    } else {
        cs.ext.udp.local_port = local_port;
        cs.ext.udp.mode = mode;
    }
    res = lwesp_conn_startex(NULL, &cs, nc, netconn_evt, 1);
    return res;
}

/**
 * \brief           Bind a connection to specific port, can be only used for server connections
 * \param[in]       nc: Netconn handle
 * \param[in]       port: Port used to bind a connection to
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_netconn_bind(lwesp_netconn_p nc, lwesp_port_t port) {
    lwespr_t res = lwespOK;

    LWESP_ASSERT("nc != NULL", nc != NULL);

    /*
     * Protection is not needed as it is expected
     * that this function is called only from single
     * thread for single netconn connection,
     * thus it is considered reentrant
     */

    nc->listen_port = port;

    return res;
}

/**
 * \brief           Set timeout value in units of seconds when connection is in listening mode
 *                  If new connection is accepted, it will be automatically closed after `seconds` elapsed
 *                  without any data exchange.
 * \note            Call this function before you put connection to listen mode with \ref lwesp_netconn_listen
 * \param[in]       nc: Netconn handle used for listen mode
 * \param[in]       timeout: Time in units of seconds. Set to `0` to disable timeout feature
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
lwespr_t
lwesp_netconn_set_listen_conn_timeout(lwesp_netconn_p nc, uint16_t timeout) {
    lwespr_t res = lwespOK;
    LWESP_ASSERT("nc != NULL", nc != NULL);

    /*
     * Protection is not needed as it is expected
     * that this function is called only from single
     * thread for single netconn connection,
     * thus it is reentrant in this case
     */

    nc->conn_timeout = timeout;

    return res;
}

/**
 * \brief           Listen on previously binded connection
 * \param[in]       nc: Netconn handle used to listen for new connections
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_netconn_listen(lwesp_netconn_p nc) {
    return lwesp_netconn_listen_with_max_conn(nc, LWESP_CFG_MAX_CONNS);
}

/**
 * \brief           Listen on previously binded connection with max allowed connections at a time
 * \param[in]       nc: Netconn handle used to listen for new connections
 * \param[in]       max_connections: Maximal number of connections server can accept at a time
 *                      This parameter may not be larger than \ref LWESP_CFG_MAX_CONNS
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
lwespr_t
lwesp_netconn_listen_with_max_conn(lwesp_netconn_p nc, uint16_t max_connections) {
    lwespr_t res;

    LWESP_ASSERT("nc != NULL", nc != NULL);
    LWESP_ASSERT("nc->type must be TCP", NETCONN_IS_TCP(nc));

    /* Enable server on port and set default netconn callback */
    if ((res = lwesp_set_server(1, nc->listen_port,
                                LWESP_U16(LWESP_MIN(max_connections, LWESP_CFG_MAX_CONNS)),
                                nc->conn_timeout, netconn_evt, NULL, NULL, 1)) == lwespOK) {
        lwesp_core_lock();
        listen_api = nc;                        /* Set current main API in listening state */
        lwesp_core_unlock();
    }
    return res;
}

/**
 * \brief           Accept a new connection
 * \param[in]       nc: Netconn handle used as base connection to accept new clients
 * \param[out]      client: Pointer to netconn handle to save new connection to
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_netconn_accept(lwesp_netconn_p nc, lwesp_netconn_p* client) {
    lwesp_netconn_t* tmp;
    uint32_t time;

    LWESP_ASSERT("nc != NULL", nc != NULL);
    LWESP_ASSERT("client != NULL", client != NULL);
    LWESP_ASSERT("nc->type must be TCP", NETCONN_IS_TCP(nc));
    LWESP_ASSERT("nc == listen_api", nc == listen_api);

    *client = NULL;
    if (nc->accept_timeout == LWESP_NETCONN_ACCEPT_NO_WAIT ) {
        time = lwesp_sys_mbox_getnow(&nc->mbox_accept, (void**)&tmp);
    } else {
        time = lwesp_sys_mbox_get(&nc->mbox_accept, (void**)&tmp, nc->accept_timeout);
    }
    if (time == LWESP_SYS_TIMEOUT && nc->accept_timeout == LWESP_NETCONN_ACCEPT_NO_WAIT) {
        return lwespEAGAIN; 
    } else if (time == LWESP_SYS_TIMEOUT) {
        return lwespTIMEOUT;
    }
    if ((uint8_t*)tmp == (uint8_t*)&recv_closed) {
        lwesp_core_lock();
        listen_api = NULL;                      /* Disable listening at this point */
        lwesp_core_unlock();
        return lwespERRWIFINOTCONNECTED;        /* Wifi disconnected */
    } else if ((uint8_t*)tmp == (uint8_t*)&recv_not_present) {
        lwesp_core_lock();
        listen_api = NULL;                      /* Disable listening at this point */
        lwesp_core_unlock();
        return lwespERRNODEVICE;                /* Device not present */
    }
    *client = tmp;                              /* Set new pointer */
    return lwespOK;                             /* We have a new connection */
}

/**
 * \brief           Write data to connection output buffers
 * \note            This function may only be used on TCP or SSL connections
 * \param[in]       nc: Netconn handle used to write data to
 * \param[in]       data: Pointer to data to write
 * \param[in]       btw: Number of bytes to write
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_netconn_write(lwesp_netconn_p nc, const void* data, size_t btw) {
    size_t len, sent;
    const uint8_t* d = data;
    lwespr_t res;
//    printf("%s %d %d %d\r\n", __func__, nc != NULL, NETCONN_IS_TCP(nc) || NETCONN_IS_SSL(nc), lwesp_conn_is_active(nc->conn));


    LWESP_ASSERT("nc != NULL", nc != NULL);
    LWESP_ASSERT("nc->type must be TCP or SSL", NETCONN_IS_TCP(nc) || NETCONN_IS_SSL(nc));
    LWESP_ASSERT("nc->conn must be active", lwesp_conn_is_active(nc->conn));

    /*
     * Several steps are done in write process
     *
     * 1. Check if buffer is set and check if there is something to write to it.
     *    1. In case buffer will be full after copy, send it and free memory.
     * 2. Check how many bytes we can write directly without need to copy
     * 3. Try to allocate a new buffer and copy remaining input data to it
     * 4. In case buffer allocation fails, send data directly (may have impact on speed and effectivenes)
     */

    /* Step 1 */
    if (nc->buff.buff != NULL) {                /* Is there a write buffer ready to accept more data? */
        len = LWESP_MIN(nc->buff.len - nc->buff.ptr, btw);  /* Get number of bytes we can write to buffer */
        if (len > 0) {
            LWESP_MEMCPY(&nc->buff.buff[nc->buff.ptr], data, len);  /* Copy memory to temporary write buffer */
            d += len;
            nc->buff.ptr += len;
            btw -= len;
        }

        /* Step 1.1 */
        if (nc->buff.ptr == nc->buff.len) {
            res = lwesp_conn_send(nc->conn, nc->buff.buff, nc->buff.len, &sent, 1);
            if (res != lwespOK) {
                return res;
            }
            lwesp_mem_free_s((void**)&nc->buff.buff);
        } else {
            return lwespOK;                     /* Buffer is not full yet */
        }
    }

    /* Step 2 */
    while (btw > LWESP_CFG_CONN_MAX_DATA_LEN) {
        size_t rem;
        rem = btw % LWESP_CFG_CONN_MAX_DATA_LEN;/* Get remaining bytes for max data length */
        if (rem == 0) {
            rem = LWESP_CFG_CONN_MAX_DATA_LEN;
        }
         
        res = lwesp_conn_send(nc->conn, d, btw - rem, &sent, 1);/* Write data directly */
        if (res != lwespOK) {
            return res;
        }
        d += sent;                              /* Advance in data pointer */
        btw -= sent;                            /* Decrease remaining data to send */
    }

    if (btw == 0) {                             /* Sent everything? */
        return lwespOK;
    }

    /* Step 3 */
    if (nc->buff.buff == NULL) {                /* Check if we should allocate a new buffer */
        nc->buff.buff = lwesp_mem_malloc(sizeof(*nc->buff.buff) * LWESP_CFG_CONN_MAX_DATA_LEN);
        nc->buff.len = LWESP_CFG_CONN_MAX_DATA_LEN; /* Save buffer length */
        nc->buff.ptr = 0;                       /* Save buffer pointer */
    }
    LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
        "lwesp_netconn_write btw:%d\r\n", btw);

    /* Step 4 */
    if (nc->buff.buff != NULL) {                /* Memory available? */
        LWESP_MEMCPY(&nc->buff.buff[nc->buff.ptr], d, btw); /* Copy data to buffer */
        nc->buff.ptr += btw;
    } else {                                    /* Still no memory available? */
        return lwesp_conn_send(nc->conn, data, btw, NULL, 1);   /* Simply send directly blocking */
    }
    return lwespOK;
}

/**
 * \brief           Flush buffered data on netconn TCP/SSL connection
 * \note            This function may only be used on TCP/SSL connection
 * \param[in]       nc: Netconn handle to flush data
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_netconn_flush(lwesp_netconn_p nc) {
 //   printf("%s %d %d %d\r\n", __func__, nc != NULL, NETCONN_IS_TCP(nc) || NETCONN_IS_SSL(nc), lwesp_conn_is_active(nc->conn));

    LWESP_ASSERT("nc != NULL", nc != NULL);
    LWESP_ASSERT("nc->type must be TCP or SSL", NETCONN_IS_TCP(nc) || NETCONN_IS_SSL(nc));
    LWESP_ASSERT("nc->conn must be active", lwesp_conn_is_active(nc->conn));
    lwespr_t ret = lwespOK;  
        /*
     * In case we have data in write buffer,
     * flush them out to network
     */
    if (nc->buff.buff != NULL) {                /* Check remaining data */
        if (nc->buff.ptr > 0) {                 /* Do we have data in current buffer? */
            LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE,
    "lwesp_netconn_flush buff.ptr:%d\r\n", nc->buff.ptr);
            ret = lwesp_conn_send(nc->conn, nc->buff.buff, nc->buff.ptr, NULL, 1);/* Send data */
        }
        lwesp_mem_free_s((void**)&nc->buff.buff);
    }
    return ret;
}

/**
 * \brief           Send data on UDP connection to default IP and port
 * \param[in]       nc: Netconn handle used to send
 * \param[in]       data: Pointer to data to write
 * \param[in]       btw: Number of bytes to write
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_netconn_send(lwesp_netconn_p nc, const void* data, size_t btw) {
    LWESP_ASSERT("nc != NULL", nc != NULL);
    LWESP_ASSERT("nc->type must be UDP", nc->type == LWESP_NETCONN_TYPE_UDP);
    LWESP_ASSERT("nc->conn must be active", lwesp_conn_is_active(nc->conn));

    return lwesp_conn_send(nc->conn, data, btw, NULL, 1);
}

/**
 * \brief           Send data on UDP connection to specific IP and port
 * \note            Use this function in case of UDP type netconn
 * \param[in]       nc: Netconn handle used to send
 * \param[in]       ip: Pointer to IP address
 * \param[in]       port: Port number used to send data
 * \param[in]       data: Pointer to data to write
 * \param[in]       btw: Number of bytes to write
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_netconn_sendto(lwesp_netconn_p nc, const lwesp_ip_t* ip, lwesp_port_t port, const void* data, size_t btw) {
    LWESP_ASSERT("nc != NULL", nc != NULL);
    LWESP_ASSERT("nc->type must be UDP", nc->type == LWESP_NETCONN_TYPE_UDP);
    LWESP_ASSERT("nc->conn must be active", lwesp_conn_is_active(nc->conn));

    return lwesp_conn_sendto(nc->conn, ip, port, data, btw, NULL, 1);
}

/**
 * \brief           Receive data from connection
 * \param[in]       nc: Netconn handle used to receive from
 * \param[in]       pbuf: Pointer to pointer to save new receive buffer to.
 *                     When function returns, user must check for valid pbuf value `pbuf != NULL`
 * \return          \ref lwespOK when new data ready
 * \return          \ref lwespCLOSED when connection closed by remote side
 * \return          \ref lwespTIMEOUT when receive timeout occurs
 * \return          Any other member of \ref lwespr_t otherwise
 */
lwespr_t
lwesp_netconn_receive(lwesp_netconn_p nc, lwesp_pbuf_p* pbuf) {
    LWESP_ASSERT("nc != NULL", nc != NULL);
    LWESP_ASSERT("pbuf != NULL", pbuf != NULL);
    uint32_t timeout;
    *pbuf = NULL;
#if LWESP_CFG_NETCONN_RECEIVE_TIMEOUT
    /*
     * Wait for new received data for up to specific timeout
     * or throw error for timeout notification
     */
    if (nc->rcv_timeout == LWESP_NETCONN_RECEIVE_NO_WAIT) {
        timeout = lwesp_sys_mbox_getnow(&nc->mbox_receive, (void**)pbuf);
    } else {
        timeout = lwesp_sys_mbox_get(&nc->mbox_receive, (void**)pbuf, nc->rcv_timeout);
    }
    if (timeout == LWESP_SYS_TIMEOUT && nc->rcv_timeout == LWESP_NETCONN_ACCEPT_NO_WAIT) {
        return lwespEAGAIN; 
    } else if (timeout == LWESP_SYS_TIMEOUT) {
        return lwespTIMEOUT;
    }

#else /* LWESP_CFG_NETCONN_RECEIVE_TIMEOUT */
    /* Forever wait for new receive packet */
    lwesp_sys_mbox_get(&nc->mbox_receive, (void**)pbuf, 0);
#endif /* !LWESP_CFG_NETCONN_RECEIVE_TIMEOUT */

    lwesp_core_lock();
    if (nc->mbox_receive_entries > 0) {
        --nc->mbox_receive_entries;
    }
    lwesp_core_unlock();

    /* Check if connection closed */
    if ((uint8_t*)(*pbuf) == (uint8_t*)&recv_closed) {
        *pbuf = NULL;                           /* Reset pbuf */
        return lwespCLOSED;
    }
#if LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
    else {
        lwesp_core_lock();
        nc->conn->status.f.receive_blocked = 0; /* Resume reading more data */
        lwesp_conn_recved(nc->conn, *pbuf);     /* Notify stack about received data */
        lwesp_core_unlock();
    }
#endif /* LWESP_CFG_CONN_MANUAL_TCP_RECEIVE */
    return lwespOK;                             /* We have data available */
}

/**
 * \brief           Close a netconn connection
 * \param[in]       nc: Netconn handle to close
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_netconn_close(lwesp_netconn_p nc) {
    lwesp_conn_p conn;

    LWESP_ASSERT("nc != NULL", nc != NULL);
    LWESP_ASSERT("nc->conn != NULL", nc->conn != NULL);
    LWESP_ASSERT("nc->conn must be active", lwesp_conn_is_active(nc->conn));

    lwesp_netconn_flush(nc);                    /* Flush data and ignore result */
    conn = nc->conn;
    nc->conn = NULL;

    lwesp_conn_set_arg(conn, NULL);             /* Reset argument */
    lwesp_conn_close(conn, 1);                  /* Close the connection */
    flush_mboxes(nc, 1);                        /* Flush message queues */
    return lwespOK;
}

/**
 * \brief           Get connection number used for netconn
 * \param[in]       nc: Netconn handle
 * \return          `-1` on failure, connection number between `0` and \ref LWESP_CFG_MAX_CONNS otherwise
 */
int8_t
lwesp_netconn_get_connnum(lwesp_netconn_p nc) {
    if (nc != NULL && nc->conn != NULL) {
        return lwesp_conn_getnum(nc->conn);
    }
    return -1;
}

#if LWESP_CFG_NETCONN_RECEIVE_TIMEOUT || __DOXYGEN__

/**
 * \brief           Set timeout value for receiving data.
 *
 * When enabled, \ref lwesp_netconn_receive will only block for up to
 * `timeout` value and will return if no new data within this time
 *
 * \param[in]       nc: Netconn handle
 * \param[in]       timeout: Timeout in units of milliseconds.
 *                      Set to `0` to disable timeout feature
 *                      Set to `> 0` to set maximum milliseconds to wait before timeout
 *                      Set to \ref LWESP_NETCONN_RECEIVE_NO_WAIT to enable non-blocking receive
 */
void
lwesp_netconn_set_receive_timeout(lwesp_netconn_p nc, uint32_t timeout) {
    nc->rcv_timeout = timeout;
}

void
lwesp_netconn_set_accept_timeout(lwesp_netconn_p nc, uint32_t timeout) {
    nc->accept_timeout = timeout;
}

void
lwesp_netconn_set_connect_timeout(lwesp_netconn_p nc, uint32_t timeout) {
    nc->connect_timeout = timeout;
}
/**
 * \brief           Get netconn receive timeout value
 * \param[in]       nc: Netconn handle
 * \return          Timeout in units of milliseconds.
 *                  If value is `0`, timeout is disabled (wait forever)
 */
uint32_t
lwesp_netconn_get_receive_timeout(lwesp_netconn_p nc) {
    return nc->rcv_timeout;
}

#endif /* LWESP_CFG_NETCONN_RECEIVE_TIMEOUT || __DOXYGEN__ */

/**
 * \brief           Get netconn connection handle
 * \param[in]       nc: Netconn handle
 * \return          ESP connection handle
 */
lwesp_conn_p
lwesp_netconn_get_conn(lwesp_netconn_p nc) {
    return nc->conn;
}

/**
 * \brief           Get netconn connection type
 * \param[in]       nc: Netconn handle
 * \return          ESP connection type
 */
lwesp_netconn_type_t
lwesp_netconn_get_type(lwesp_netconn_p nc) {
    return nc->type;
}

lwespr_t
lwesp_netconn_receive_ready(lwesp_netconn_p nc) {
    if (nc == NULL) {
        return 0;
    }
    return lwesp_sys_mbox_has_entry(&nc->mbox_receive);
}

lwespr_t
lwesp_netconn_accept_ready(lwesp_netconn_p nc) {
    LWESP_ASSERT("nc != NULL", nc != NULL);
    LWESP_ASSERT("nc->type must be TCP", NETCONN_IS_TCP(nc));

    return lwesp_sys_mbox_has_entry(&nc->mbox_accept);
}



#endif /* LWESP_CFG_NETCONN || __DOXYGEN__ */

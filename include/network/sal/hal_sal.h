/*!
 * @file hal_sal.h
 *
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SAL_IMPORT_H_
#define _SAL_IMPORT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_sal API
 *  Socket Adapter Layer, which provides socket API service
 *  implemented on top of AT modules.
 *
 *  @{
 */

typedef enum {
    /* WiFi */
    TCP_SERVER,
    TCP_CLIENT,
    SSL_CLIENT,
    UDP_BROADCAST,
    UDP_UNICAST,
    /*WiFi end */
    /* Add others hereafter */
} CONN_TYPE;

/* Fill necessary fileds according to the socket type. */
typedef struct {
    int fd; //<! fd that are used in socket level
    CONN_TYPE type;
    char *addr; //<! remote ip or domain
    int32_t r_port; //<! remote port (set to -1 if not used)
    int32_t l_port; //<! local port (set to -1 if not used)
    uint32_t tcp_keep_alive; //<! tcp keep alive value (set to 0 if not used)
} sal_conn_t;

/* Socket data state indicator. */
typedef enum netconn_evt {
    NETCONN_EVT_RCVPLUS,
    NETCONN_EVT_RCVMINUS,
    NETCONN_EVT_SENDPLUS,
    NETCONN_EVT_SENDMINUS,
    NETCONN_EVT_ERROR
} netconn_evt_t;

typedef int (*netconn_data_input_cb_t)(int fd, void *data, size_t len, char remote_ip[16], uint16_t remote_port);

typedef struct sal_op_s {
    struct sal_op_s * next;  //<! Next sal_op_t structure

    char *version; //<! Reserved for furture use.

    char *name; //<! Drvier name

    /* Add sal device */
    int (*add_dev)(void*);

    /**
     * Module low level init so that it's ready to setup socket connection.
     *
     * @return  0 - success, -1 - failure
     */
    int (*init)(void);

    /**
     * Start a socket connection via module.
     *
     * @param[in]  c - connect parameters which are used to setup
     *                 the socket connection.
     *
     * @return  0 - success, -1 - failure
     */
    int (*start)(sal_conn_t *c);

    /**
     * Send data via module.
     * This function does not return until all data sent.
     *
     * @param[in]  fd - the file descripter to operate on.
     * @param[in]  data - pointer to data to send.
     * @param[in]  len - length of the data.
     * @param[in]  remote_ip - remote ip address (optional).
     * @param[in]  remote_port - remote port number (optional).
     * @param[in]  timeout - packet send timeout (ms)
     * @return  0 - success, -1 - failure
     */
    int (*send_data)(int fd, uint8_t *data, uint32_t len,
                char remote_ip[16], int32_t remote_port, int32_t timeout);

    int (*recv_data)(int fd, uint8_t *data, uint32_t len,
                char remote_ip[16], int32_t remote_port);

    /**
     * Get IP information of the corresponding domain.
     * Currently only one IP string is returned (even when the domain
     * coresponses to mutliple IPs). Note: only IPv4 is supported.
     *
     * @param[in]   domain - the domain string.
     * @param[out]  ip - the place to hold the dot-formatted ip string.
     *
     * @return  0 - success, -1 - failure
     */
    int (*domain_to_ip)(char *domain, char ip[16]);

    /**
     * Close the socket connection.
     *
     * @param[in]  fd - the file descripter to operate on.
     * @param[in]  remote_port - remote port number (optional).
     *
     * @return  0 - success, -1 - failure
     */
    int (*finish)(int fd, int32_t remote_port);

    /**
     * Destroy SAL or exit low level state if necessary.
     *
     * @return  0 - success, -1 - failure
     */
    int (*deinit)(void);

    /**
     * Register network connection data input function
     * Input data from module.
     * This callback should be called when the data is received from the module
     * It should tell the sal where the data comes from.
     * @param[in]  fd - the file descripter to operate on.
     * @param[in]  data - the received data.
     * @param[in]  len - expected length of the data when IN,
     *                    and real read len when OUT.
     * @param[in]  addr - remote ip address. Caller manages the
                                memory (optional).
     * @param[in]  port - remote port number (optional).
     *
     * @return  0 - success, -1 - failure
     */
    int (*register_netconn_data_input_cb)(netconn_data_input_cb_t cb);
} sal_op_t;

/** @} */ /* end of group aos_sal */

#ifdef __cplusplus
}
#endif

#endif

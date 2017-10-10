/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "wsf.h"
#include "wsf_log.h"
#include "wsf_msg.h"
#include "wsf_network.h"
#include "connectivity.h"
#include "os.h"

wsf_connection_t *wsf_conn = NULL;

void *sess_lock = NULL;
void *device_lock = NULL;

static char *wsf_rx_buffer;

static void wsf_regist_device();

typedef void (*connection_callback)();
extern void cb_recv(int fd, void *arg);

extern cb_network g_wsf_cb;

static wsf_code wsf_open_connection0(wsf_connection_t *conn, const char *host,
                                     int port, int enable_ssl, connection_callback callback)
{
    if (!conn) {
        return WSF_CONNECTION_ERROR;
    }

    wsf_reset_connection(conn, 0);

    os_netaddr_t netaddr = {(char *)host, port};

    conn->tcp = os_tcp_client_connect(&netaddr);
    if (conn->tcp == OS_INVALID_FD) {
        LOGE(MODULE_NAME, "can't estatblish tcp connection.");
        return WSF_CONNECTION_ERROR;
    }

    if (enable_ssl) {
        //Note: ca str is not a string in qcom platform, so can not use strlen here
        conn->ssl = os_ssl_connect(conn->tcp, wsf_get_config()->global_ca_str,
                                   wsf_get_config()->ca_str_len);
        if (!conn->ssl) {
            wsf_reset_connection(conn, 0);
            LOGE(MODULE_NAME, "can't open ssl connection");
            return WSF_CONNECTION_ERROR;
        } else {
            g_wsf_cb.sock = (long)wsf_conn->tcp;
            aos_poll_read_fd(g_wsf_cb.sock, cb_recv, &g_wsf_cb);
            LOGI(MODULE_NAME, "add new tcp socket fd to poll list.\n");
        }
    }

    conn->conn_state = CONN_OPEN;
    if (callback) {
        callback();
    }
    return WSF_SUCCESS;
}


wsf_code wsf_open_connection(wsf_config_t *config)
{
    if (!wsf_conn) {
        wsf_conn = (wsf_connection_t *)os_malloc(sizeof(wsf_connection_t));
        if (wsf_conn) {
            memset(wsf_conn, 0, sizeof(wsf_connection_t));
            wsf_conn->tcp = OS_INVALID_FD;
            wsf_conn->mutex = os_mutex_init();

            if (!wsf_rx_buffer) {
                wsf_rx_buffer = os_malloc(wsf_get_config()->max_msg_recv_length);
                OS_CHECK_MALLOC(wsf_rx_buffer);
            }

            wsf_conn->recv_buf = wsf_rx_buffer;
            wsf_conn->recv_buf_pos = 0;
            wsf_conn->recv_buf_len = wsf_get_config()->max_msg_recv_length;
            memset(wsf_conn->recv_buf, 0, wsf_conn->recv_buf_len);
        } else {
            LOGE(MODULE_NAME, "out of memory");
            return WSF_FAIL;
        }
    }

    //establish connection and register device.
    return wsf_open_connection0(wsf_conn, config->server_name,
                                config->server_port, config->enable_ssl, wsf_regist_device);
}


wsf_code wsf_reset_connection(wsf_connection_t *conn, int clear_session)
{

    LOGI(MODULE_NAME, "wsf: reset connection");
    if (conn) {
        os_mutex_lock(conn->mutex);
        if (conn->ssl) {
            os_ssl_close(conn->ssl);
            conn->ssl = NULL;
        }
        if (conn->tcp != OS_INVALID_FD) {
            aos_cancel_poll_read_fd((int)conn->tcp, NULL, NULL);
            os_tcp_close(conn->tcp);
            conn->tcp = OS_INVALID_FD;
        }

        conn->recv_buf_pos = 0;

        //FIXME: reuse session unstable, better to clear session now
        //if (conn->session_id && clear_session) {
        if (conn->session_id) {
            os_free(conn->session_id);
            conn->session_id = NULL;
        }

        conn->heartbeat_unack_counter = WSF_MAX_UNACK_HEARTBEART;
        conn->conn_state = CONN_CLOSED;
        os_mutex_unlock(conn->mutex);

        wsf_set_state(CONNECT_STATE_CLOSE);
    }

    return WSF_SUCCESS;
}

wsf_code wsf_destroy_connections()
{
    if (wsf_conn) {
        LOGI(MODULE_NAME, "wsf: destroy connection");
        wsf_reset_connection(wsf_conn, 1);
        if (wsf_conn->device_id) {
            os_free(wsf_conn->device_id);
            wsf_conn->device_id =  NULL;
        }
        os_mutex_destroy(wsf_conn->mutex);
        os_free(wsf_conn);
        wsf_conn = NULL;

        if (wsf_rx_buffer) {
            os_free(wsf_rx_buffer);
            wsf_rx_buffer = NULL;
        }
    }
    return WSF_SUCCESS;

}


wsf_code wsf_send_msg(wsf_connection_t *wsf_conn, const char *data, int length)
{
    uint32_t bytes = 0;
    int count = 0;
    /* =============  debug ================ */
    /*
    int i = 0;
    printf("send msg: \n");
    for (i = 0; i < length; i++) {
        printf(" %02X ", data[i]);
    }
    printf("\n");
    */
    /* =============  debug ================ */
    os_mutex_lock(wsf_conn->mutex);
    if (wsf_conn->ssl) {
        int ret = os_ssl_send(wsf_conn->ssl, data + count, length - count);
        if (ret == -1) {
            os_mutex_unlock(wsf_conn->mutex);
            LOGE(MODULE_NAME, "ssl send error, len:%d", length - count);
            return WSF_SEND_ERROR;
        }
        LOGD(MODULE_NAME, "wsf send succeed->  %d\n", ret);
        os_mutex_unlock(wsf_conn->mutex);
        return WSF_SUCCESS;
    }

    while (count < length) {
        bytes = os_tcp_send(wsf_conn->tcp, data + count, length - count);

        if (bytes == -1 || bytes == 0) {
            os_mutex_unlock(wsf_conn->mutex);
            return WSF_SEND_ERROR;
        }

        count += bytes;
    }
    os_mutex_unlock(wsf_conn->mutex);

    return WSF_SUCCESS;
}

char *wsf_session_id_get()
{
    char *ret = NULL;
    if (!wsf_conn) {
        return ret;
    }

    os_mutex_lock(sess_lock);
    if (wsf_conn->session_id) {
        size_t len = strlen(wsf_conn->session_id);
        ret = (char *)os_malloc(len + 1);
        if (ret) {
            strcpy(ret, wsf_conn->session_id);
        } else {
            LOGE(MODULE_NAME, "return session id null, out of memory");
        }

    }

    os_mutex_unlock(sess_lock);
    return ret;
}

void wsf_session_id_set(const char *session_id)
{
    if (!wsf_conn) {
        return;
    }

    os_mutex_lock(sess_lock);
    if (!session_id) {
        if (wsf_conn->session_id) {
            os_free(wsf_conn->session_id);
        }
        wsf_conn->session_id = NULL;
    } else {
        size_t len = strlen(session_id);
        char *tmp = (char *)os_malloc(len + 1);
        if (tmp) {
            strcpy(tmp, session_id);
            if (wsf_conn->session_id) {
                os_free(wsf_conn->session_id);
            }
            wsf_conn->session_id = tmp;
        } else {
            LOGE(MODULE_NAME, "failed to copy session id, out of memory");
        }
    }

    os_mutex_unlock(sess_lock);
}

char *wsf_device_id_get()
{
    char *ret = NULL;
    if (!wsf_conn) {
        return ret;
    }

    os_mutex_lock(device_lock);
    if (wsf_conn->device_id) {
        size_t len = strlen(wsf_conn->device_id);
        ret = (char *)os_malloc(len + 1);
        if (ret) {
            strcpy(ret, wsf_conn->device_id);
        } else {
            LOGE(MODULE_NAME, "failed to copy device id, out of memory");
        }

    }

    os_mutex_unlock(device_lock);
    return ret;
}

void wsf_device_id_set(const char *device_id)
{
    if (!wsf_conn) {
        return;
    }

    os_mutex_lock(device_lock);
    if (!device_id) {
        if (wsf_conn->device_id) {
            os_free(wsf_conn->device_id);
        }
        wsf_conn->device_id = NULL;
    } else {
        size_t len = strlen(device_id);
        char *tmp = (char *)os_malloc(len + 1);
        if (tmp) {
            strcpy(tmp, device_id);
            if (wsf_conn->device_id) {
                os_free(wsf_conn->device_id);
            }
            wsf_conn->device_id = tmp;
        } else {
            LOGE(MODULE_NAME, "failed to copy session id, out of memory");
        }
    }

    os_mutex_unlock(device_lock);

}

wsf_code wsf_recvbuff_check(wsf_connection_t *conn)
{
    if (!conn) {
        return WSF_FAIL;
    }

    if (conn->recv_buf_pos ==  conn->recv_buf_len)
        LOGE(MODULE_NAME, "msg length exceed max_msg_recv_length:%d",
             wsf_get_config()->max_msg_recv_length);

    return WSF_SUCCESS;
}

wsf_code wsf_recvbuff_clear()
{
    if (wsf_conn) {
        wsf_conn->recv_buf_pos = 0;
    }

    return WSF_SUCCESS;
}

static void wsf_regist_device()
{
    if (!wsf_conn) {
        return;
    }
    wsf_msg_t *register_req = wsf_msg_register_request_create();
    int i = 0;
    uint32_t len;
    memcpy(&len, register_req->header.msg_length, sizeof(len));
    for (i = 0; i < 3; i++) {
        wsf_msg_header_encode((void *)register_req, len);
        if (wsf_send_msg(wsf_conn, (char *)register_req, len)
            == WSF_SUCCESS) {
            break;
        }
    }

    if (i == 3) {
        LOGE(MODULE_NAME, "register device failed");
    }

    os_free(register_req);

}

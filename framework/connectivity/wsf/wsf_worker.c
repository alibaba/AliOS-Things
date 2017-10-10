/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "wsf_client.h"
#include "wsf_log.h"
#include "wsf_worker.h"
#include "wsf_msg.h"
#include "wsf_config.h"
#include "wsf_msg_queue.h"
#include "wsf_network.h"
#include "wsf.h"
#include "aos/aos.h"
#include "aos/network.h"
#include "os.h"

wsf_request_queue_t *global_request_queue;

extern wsf_connection_t *wsf_conn;

extern wsf_push_callback push_callback;

extern wsf_error_callback error_callback;

static volatile int network_up = 1;

static dlist_t g_list;
static void *g_req_mutex;

static int wsf_running;
static int total_req_nodes = 0;

typedef void (*msg_handler)(wsf_msg_t *msg, int length);

static void process_msg_response(wsf_msg_t *msg, int length);
static void process_msg_request(wsf_msg_t *msg, int length);
static void process_heartbeat_response(wsf_msg_t *msg, int length);
static void process_register_response(wsf_msg_t *msg, int length);
static void process_deregister_connection(wsf_msg_t *msg, int length);
static void __process_msg_request(wsf_msg_t *msg, int length);
static void process_received_buf(wsf_connection_t *conn);
static void wsf_keep_connection(wsf_config_t *config);
static void __copy_session(wsf_msg_t *p_rsp);
void request_msg_handle(void *arg);

static msg_handler msg_handlers[MSG_TYPE_END] = {
    process_msg_request,                    //0
    process_msg_response,                   //1
    NULL,                                   //2
    process_heartbeat_response,             //3
    NULL,                                   //4
    process_register_response,              //5
    process_deregister_connection,          //6
};


static int __cb_wsf_recv(int fd, void *arg);
static void __cb_wsf_close(int fd, void *arg);
static void __cb_wsf_timeout(void *arg);
cb_network g_wsf_cb;

void cb_recv(int fd, void *arg)
{
    cb_network *cb = (cb_network *)arg;
    if (!cb) {
        return;
    }
    //the connection is closed.
    if (0 == cb->cb_recv(fd, cb)) {
        cb->cb_close(fd, cb);
    }

    aos_cancel_delayed_action(cb->timeout, cb->cb_timeout, cb);
    aos_post_delayed_action(cb->timeout, cb->cb_timeout, cb);
}

static void _receive_worker(void *arg)
{
    wsf_config_t *config = (wsf_config_t *)arg;

    wsf_msg_t hb_req;
    uint32_t msg_len; //hb_req.header.msg_length;

    int i, retval, count;
    uint32_t timeout;
    void *fd_read[OS_SOCKET_MAXNUMS];

    for (i = 0; i < OS_SOCKET_MAXNUMS; i++) {
        fd_read[i] = OS_INVALID_FD;
    }

    wsf_msg_heartbeat_request_init(&hb_req);
    memcpy(&msg_len, hb_req.header.msg_length, sizeof(uint32_t));
    wsf_msg_header_encode((char *)&hb_req, msg_len);

    while (wsf_running) {
        timeout = config->heartbeat_interval;

        /* only fd_read[0] is used */
        if (wsf_conn && wsf_conn->ssl) {
            fd_read[0] = wsf_conn->tcp;
            retval = os_select(fd_read, NULL, timeout * 1000);
            printf("select(%d) retval:%d", timeout, retval);
        } else {
            os_sleep(timeout);
            wsf_keep_connection(config);
            continue;
        }

        if (retval < 0) {
            printf("wsf: select ret -1, reconnect");
            wsf_reset_connection(wsf_conn, 0);
        } else if (retval == 0) {
            //TIMEOUT to send heartbeat when connection ready

            if (wsf_is_heartbeat_timeout(wsf_conn)) {
                //close the connection
                printf("wsf: heartbeat timeout, reconnect");

                wsf_reset_connection(wsf_conn, 0);
            }

            if (wsf_conn && wsf_conn->ssl) {
                wsf_send_msg(wsf_conn, (const char *)&hb_req, msg_len);
                printf("send heartbeat");
                wsf_dec_heartbeat_counter(wsf_conn);
            }

            /* NOTE: wsf_reset_connection() only called in this thread */
            wsf_keep_connection(config);
        } else {
            if (wsf_conn && (OS_INVALID_FD != fd_read[0])) {
                char *buf = wsf_conn->recv_buf + wsf_conn->recv_buf_pos;
                int len = wsf_conn->recv_buf_len - wsf_conn->recv_buf_pos;
                if (NULL != wsf_conn->ssl) {
                    count = os_ssl_recv(wsf_conn->ssl, buf, len);
                } else {
                    count = os_tcp_recv(wsf_conn->tcp, buf, len);
                }
                if (count <= 0) {
                    if (!len) {
                        printf("wsf recv buffer full!");
                    } else {
                        printf("closing socket for tcp/ssl read error.");
                    }
                    wsf_reset_connection(wsf_conn, 0);
                    continue;
                } else {
                    wsf_conn->recv_buf_pos += count;
                }
                process_received_buf(wsf_conn);
            }
        }
    }

}


static void receive_worker(void *arg)
{
    wsf_config_t *config = (wsf_config_t *)arg;
    if (!config) {
        return;
    }

    memset(&g_wsf_cb, 0, sizeof(cb_network));
    if (wsf_conn && wsf_conn->ssl) {
        g_wsf_cb.sock = (long)wsf_conn->tcp;
    }
    g_wsf_cb.timeout = 1000 * config->heartbeat_interval;
    g_wsf_cb.cb_recv = __cb_wsf_recv;
    g_wsf_cb.cb_close = __cb_wsf_close;
    g_wsf_cb.cb_timeout = __cb_wsf_timeout;
    g_wsf_cb.extra = arg;
    aos_post_delayed_action(g_wsf_cb.timeout, g_wsf_cb.cb_timeout, &g_wsf_cb);
}

static void stop_receive_worker()
{
    aos_cancel_poll_read_fd(g_wsf_cb.sock, cb_recv, &g_wsf_cb);
    aos_cancel_delayed_action(g_wsf_cb.timeout, g_wsf_cb.cb_timeout, &g_wsf_cb);
}


static void __cb_wsf_close(int fd, void *arg)
{
    LOGE(MODULE_NAME, "wsf: select ret -1, reconnect");
    wsf_msg_queue_flush(global_request_queue);
    wsf_reset_connection(wsf_conn, 0);
}

static int  __cb_wsf_recv(int fd, void *arg)
{
    int count = 0;

    if (wsf_conn && (-1 != fd)) {
        char *buf = wsf_conn->recv_buf + wsf_conn->recv_buf_pos;
        int len = wsf_conn->recv_buf_len - wsf_conn->recv_buf_pos;
        if (len <= 0) {
            return 1;
        }
        if (NULL != wsf_conn->ssl) {
            count = os_ssl_recv(wsf_conn->ssl, buf, len);
        } else {
            count = os_tcp_recv(wsf_conn->tcp, buf, len);
        }
        LOGD(MODULE_NAME, "wsf recv : %s,cnt: %d, len: %d\n", buf, count, len);
        if (count < 0 && (count == -EAGAIN || errno == EAGAIN)) {
            LOGD(MODULE_NAME, "wsf recv eagain");
            return 1;
        }
        if (count <= 0) {
            if (!len) {
                LOGE(MODULE_NAME, "wsf recv buffer full!");
            } else {
                LOGE(MODULE_NAME, "closing socket for tcp/ssl read error %d.", errno);
            }
            return 0;
        } else {
            wsf_conn->recv_buf_pos += count;
        }
        /* receive packets, reset heartbeat counter */
        wsf_reset_heartbeat_counter(wsf_conn);
        process_received_buf(wsf_conn);
    }
    return 1;
}

static void __cb_wsf_timeout(void *arg)
{
    cb_network *cb = (cb_network *)arg;
    wsf_config_t *config = NULL;
    if (!cb) {
        return;
    }
    config = (wsf_config_t *)cb->extra;

    //side effect: send heartbeat before connection ready(before wsf session established)
    if (wsf_conn && wsf_conn->ssl) {
        wsf_msg_t hb_req;
        uint32_t len = 0;
        //construct heartbeat package
        memset(&hb_req, 0, sizeof(hb_req));
        wsf_msg_heartbeat_request_init(&hb_req);
        memcpy(&len, hb_req.header.msg_length, sizeof(uint32_t));
        wsf_msg_header_encode((char *)&hb_req, len);

        wsf_send_msg(wsf_conn, (const char *)&hb_req, len);
        LOGI(MODULE_NAME, "send heartbeat");
        wsf_dec_heartbeat_counter(wsf_conn);
    }

    //TIMEOUT to send heartbeat when connection ready
    if (wsf_is_heartbeat_timeout(wsf_conn)) {
        //close the connection
        LOGE(MODULE_NAME, "wsf: heartbeat timeout, reconnect");
        wsf_reset_connection(wsf_conn, 0);
    }

    wsf_keep_connection(config);

    //kick the timer
    aos_post_delayed_action(cb->timeout, cb->cb_timeout, cb);
}

static void process_msg_response(wsf_msg_t *msg, int length)
{
    wsf_msg_t *rsp = (wsf_msg_t *) os_malloc(length + 1);
    wsf_request_node_t *node = NULL;
    if (rsp) {
        memset(rsp, 0, length + 1);
        memcpy(rsp, msg, length);
        node = wsf_request_queue_trigger(global_request_queue, rsp);
        if (node) {
            //LOGW(MODULE_NAME,"process_msg_response, send signal");
            __copy_session(rsp);
            if (node->session.cb) {
                node->session.cb(rsp, node->session.extra);
                wsf_request_queue_pop(global_request_queue, node);
                wsf_msg_session_destroy(&node->session);
                os_free(node);
            } else {
                wsf_msg_session_signal(&node->session);
            }
        } else {
            LOGW(MODULE_NAME, "unknown response msg");
            os_free(rsp);
        }
    } else {
        LOGE(MODULE_NAME, "failed to os_malloc memory");
    }
}

/*
 * put the msg in the queue, and process in the callback_worker
 * msg will be os_freed after this function, copy the msg for the queue
 */
static void process_msg_request(wsf_msg_t *msg, int length)
{
    struct request_msg_node *node;
    int was_empty;

    if (total_req_nodes >= CONFIG_REQMSG_LENGTH) {
        LOGW(MODULE_NAME, "request queue has too nodes to handle");
        return;
    }

    /* attention: +1 for json parser need to append '\0' */
    node = (struct request_msg_node *)os_malloc(sizeof(struct request_msg_node) +
                                                length + 1);
    OS_CHECK_MALLOC(node);

    memcpy(node->msg, (uint8_t *)msg, length);
    node->length = length;
    os_mutex_lock(g_req_mutex);
    was_empty = dlist_empty(&g_list);
    dlist_add_tail(&node->list_head, &g_list);
    total_req_nodes ++;
    os_mutex_unlock(g_req_mutex);

    /* already process, no need to schedule again */
    if (!was_empty) {
        return;
    }

    request_msg_handle(NULL);
}

void init_req_glist(void)
{
    total_req_nodes = 0;
    g_req_mutex = os_mutex_init();
    dlist_init(&g_list);
}

void deinit_req_glist(void)
{
    struct request_msg_node *node;
    dlist_t *tmp = NULL;
    //FIXME: add delete interface.
    //stop_request_work();

    os_mutex_lock(g_req_mutex);
    dlist_for_each_entry_safe(&g_list, tmp, node, struct request_msg_node,
                              list_head) {
        dlist_del(&(node->list_head));
        os_free(node);
    }
    os_mutex_unlock(g_req_mutex);

    total_req_nodes = 0;
    if (g_req_mutex) {
        os_mutex_destroy(g_req_mutex);
        g_req_mutex = NULL;
    }
}

void request_msg_handle(void *arg)
{
    struct request_msg_node *node;
    LOGD(MODULE_NAME, "request_msg_handle");
    os_mutex_lock(g_req_mutex);
    while (!dlist_empty(&g_list)) {
        node = dlist_first_entry(&g_list, struct request_msg_node, list_head);
        total_req_nodes --;
        dlist_del(&(node->list_head));
        os_mutex_unlock(g_req_mutex);

        __process_msg_request((wsf_msg_t *)node->msg, node->length);

        os_mutex_lock(g_req_mutex);
        os_free(node);
    }
    os_mutex_unlock(g_req_mutex);

    if (!dlist_empty(&g_list)) {
        aos_loop_schedule_work(0, request_msg_handle, NULL, NULL, NULL);
    }
}

static void __process_msg_request(wsf_msg_t *msg, int length)
{
    uint32_t msg_id;
    uint32_t start_time, end_time;

    start_time = os_get_time_ms();
    memcpy(&msg_id, msg->header.msg_id, sizeof(uint32_t));
    wsf_response_t *callback_rsp = NULL;

    if (push_callback) {
        uint8_t *pp = msg->payload;
        pp += (*pp + 1);                        //servicename len &value
        pp += (*pp + 1);                        //sessionid len &value
        pp += (*pp + 1);                        //deviceid len &value
        pp += (*pp + 1);                        //secret len &value
        int param_count = *pp;                  //param count
        if (param_count > 0) {
            uint32_t param1_len = os_get_unaligned_be32(++pp);
            pp += 4; //param1 value
            LOGD(MODULE_NAME, "start push cmd to device, %d\n", time(NULL));
            callback_rsp = push_callback(pp, param1_len);
        } else {
            callback_rsp = push_callback(NULL, 0);
        }
    }
    wsf_msg_t *rsp = NULL;
    if (!callback_rsp) {
        rsp = wsf_msg_response_create(msg_id, INVOKE_RIGHT, NULL, 0);
    } else {
        rsp = wsf_msg_response_create(msg_id, callback_rsp->result, callback_rsp->data,
                                      callback_rsp->length);
    }

    if (!rsp) {
        LOGE(MODULE_NAME, "create response failed");
        return;
    }

    end_time = os_get_time_ms();
    if ((end_time - start_time) > wsf_get_config()->request_timeout * 1000) {
        LOGE(MODULE_NAME, "user callback block %d ms !!!", (end_time - start_time));
    }

    uint32_t msg_len;

    memcpy(&msg_len, rsp->header.msg_length, sizeof(uint32_t));
    wsf_msg_header_encode((char *)rsp, msg_len);

    LOGD(MODULE_NAME, "before send response to service");
    wsf_send_msg(wsf_conn, (const char *)rsp, msg_len);
    LOGD(MODULE_NAME, "send response to service");

    if (callback_rsp) {
        wsf_response_destroy(callback_rsp, 0);
    }

    os_free(rsp);

    return;
}

static void process_heartbeat_response(wsf_msg_t *msg, int length)
{
    //reset the counter
    wsf_reset_heartbeat_counter(wsf_conn);
}

static void process_register_response(wsf_msg_t *msg, int length)
{
    uint8_t *pp = msg->payload;
    invoke_result_code result_code = (invoke_result_code)(*pp); //result code
    //LOGW(MODULE_NAME,"result: %d\n",result_code);
    if (result_code == INVOKE_RIGHT) {
        pp++;
        int8_t len = *pp; //device id len
        if (len) {
            char *device_id = os_malloc(len + 1);
            OS_CHECK_MALLOC(device_id);

            pp++;
            memcpy(device_id, pp, len);
            device_id[len] = '\0';
            wsf_device_id_set(device_id);
            os_free(device_id);
        }
        //LOGW(MODULE_NAME,"len: %d\n",len);
        wsf_conn->conn_state = CONN_READY;
        wsf_set_state(CONNECT_STATE_READY);
    } else if (result_code == INVOKE_SESSION_EXPIRE_ERROR) {
        LOGE(MODULE_NAME, "wsf register: INVOKE_SESSION_EXPIRE_ERROR");
        wsf_reset_connection(wsf_conn, 1);
    } else {
        LOGE(MODULE_NAME, "wsf register: other error");
        wsf_reset_connection(wsf_conn, 0);
    }

}

static void process_deregister_connection(wsf_msg_t *msg, int length)
{

    if (wsf_conn && wsf_conn->ssl) {
        //close the connection
        wsf_reset_connection(wsf_conn, 1);
    }

    LOGE(MODULE_NAME, "connection deregister received");

    if (error_callback) {
        error_callback(WSF_CONNECTION_DEREGISTERED);
    }

}

/*
 * clear the whole wsf message, but remain the next wsf message if exist.
 * return: 1, more pkg; 0, no more pkg.
 */
static int wsf_recvbuff_clear_first_msg()
{
    wsf_msg_header_t *msg_header = (wsf_msg_header_t *)wsf_conn->recv_buf;
    int buf_len = wsf_conn->recv_buf_pos;
    int pkg_len = 0;

    /* after wsf_msg_header_decode(), no need to ntohl trans */
    memcpy(&pkg_len, msg_header->msg_length, sizeof(int));

    if (buf_len == pkg_len) {
        wsf_conn->recv_buf_pos = 0;
    } else if (buf_len > pkg_len) {
        LOGE(MODULE_NAME, "ssl recv more than 1 pkg! buf_len:%d, pkg_len:%d",
             buf_len, pkg_len);
        memmove(wsf_conn->recv_buf, wsf_conn->recv_buf + pkg_len,
                wsf_conn->recv_buf_pos - pkg_len);
        wsf_conn->recv_buf_pos = wsf_conn->recv_buf_pos - pkg_len;
        return 1;
    } else {
        /*
         * after deregister, wsf_reset_connection was called,
         * and wsf_conn->recv_buf_pos clear to 0.
         */
        if (buf_len) {
            OS_ASSERT(0, "should never reach here! %d < %d", buf_len, pkg_len);
        }
        wsf_conn->recv_buf_pos = 0;
    }

    return 0;
}

static void process_received_buf(wsf_connection_t *conn)
{
    wsf_code ret = wsf_recvbuff_check(conn);

    if (ret != WSF_SUCCESS) {
        wsf_recvbuff_clear(conn);
        LOGE(MODULE_NAME, "recv buffer error, drop the msg");
        return;
    }

reprocess:
    if (!wsf_msg_accept(conn->recv_buf, conn->recv_buf_pos)) {
        wsf_recvbuff_clear(conn);
        LOGE(MODULE_NAME, "unknown message received");
        return;
    }

    if (wsf_msg_complete(conn->recv_buf, conn->recv_buf_pos)) {

        wsf_msg_header_t *msg_header = wsf_msg_header_decode(conn->recv_buf,
                                                             conn->recv_buf_pos);

        msg_type mtype = get_msg_type(msg_header);
        //LOG("-->type: %d\n",mtype);
        if (mtype < MSG_TYPE_END) {
            msg_handler handler = msg_handlers[mtype];
            if (handler) {
                uint32_t length;
                memcpy(&length, msg_header->msg_length, sizeof(length));
                handler((wsf_msg_t *)msg_header, length);
            }
        }

        if (wsf_recvbuff_clear_first_msg(conn)) {
            goto reprocess;    //more package in conn->recv_buf, so process again.
        }
    }
}

static wsf_request_node_t *__send_msg(wsf_msg_t *req, wsf_async_cb_t cb,
                                      void *arg)
{
    uint32_t msg_id, msg_length;
    if (!req || !wsf_running) {
        return NULL;
    }

    if (!wsf_conn || !wsf_conn->ssl ||
        wsf_conn->conn_state != CONN_READY) {
        LOGE(MODULE_NAME, "wsf connect not ready");
        return NULL;
    }
    memcpy(&msg_id, req->header.msg_id, sizeof(uint32_t));
    memcpy(&msg_length, req->header.msg_length, sizeof(uint32_t));

    wsf_msg_header_encode((char *)req, msg_length);

    wsf_request_node_t *node = (wsf_request_node_t *) os_malloc(sizeof(
                                                                    wsf_request_node_t));
    if (!node) {
        return NULL;
    }
    //we should destroy the session after the response arrived.
    wsf_msg_session_init(&node->session);
    node->session.id = msg_id;
    if (cb && arg) {
        node->session.cb = cb;
        node->session.extra = arg;
    }

    if (!wsf_request_queue_push(global_request_queue, node)) {
        wsf_code ret = wsf_send_msg(wsf_conn,
                                    (const char *)req, msg_length);
        if (ret == WSF_SUCCESS) {
            LOGD(MODULE_NAME, "wsf msg send succeed.id=%d", msg_id);
            return node;
        }

        wsf_request_queue_pop(global_request_queue, node);
    }

    wsf_msg_session_destroy(&node->session);
    os_free(node);
    return NULL;
}

static void __copy_session(wsf_msg_t *p_rsp)
{
    if (!p_rsp) {
        return;
    }

    char *pp = (char *)p_rsp + sizeof(wsf_msg_header_t);
    //skip result code
    pp++;
    //session id len
    uint8_t len = *pp;
    //session id
    pp++;

    if (len > 0) {
        char *session_id = (char *)os_malloc(len + 1);
        if (session_id) {
            memcpy(session_id, pp, len);
            session_id[len] = '\0';
            pp += len;
            if (!wsf_conn->session_id || strcmp(session_id, wsf_conn->session_id) != 0) {
                wsf_session_id_set(session_id);
            }
            os_free(session_id);
        }
    }
    if (!wsf_conn->device_id) {
        //device id len
        len = *pp;
        pp++;
        if (len > 0) {
            char *device_id = (char *)os_malloc(len + 1);
            if (device_id) {
                memcpy(device_id, pp, len);

                device_id[len] = '\0';
                wsf_device_id_set(device_id);
                os_free(device_id);
            }
        }
    }
}

wsf_msg_t *__wsf_invoke_sync(wsf_msg_t *req)
{
    wsf_msg_t *p_rsp = NULL;
    wsf_request_node_t *node = __send_msg(req, NULL, NULL);

    if (!node) {
        return NULL;
    }

    if (!wsf_msg_session_timewait(&node->session,
                                  wsf_get_config()->request_timeout)) {
        LOGW(MODULE_NAME, "get reponse, id=%d", node->session.id);
        p_rsp = node->session.response;
    } else {
        LOGW(MODULE_NAME, "waiting response id=%d timeout",
             node->session.id);
    }

    wsf_request_queue_pop(global_request_queue, node);
    wsf_msg_session_destroy(&node->session);
    os_free(node);
    return p_rsp;
}


int __wsf_invoke_async(wsf_msg_t *req, wsf_async_cb_t cb, void *arg)
{
    wsf_msg_t *p_rsp = NULL;
    wsf_request_node_t *node = __send_msg(req, cb, arg);

    return (NULL == node);
}

wsf_code wsf_start_worker(wsf_config_t *config)
{
    wsf_running = 1;
    receive_worker(config);

    return WSF_SUCCESS;
}

void wsf_wait_worker(void)
{
    stop_receive_worker();
}

void wsf_stop_worker(void)
{
    wsf_running = 0;

    wsf_wait_worker();
}

static void wsf_keep_connection(wsf_config_t *config)
{
    if (!wsf_conn) {
        return;
    }

#if 0
    /* if not ready, just retry next turn */
    os_sys_net_wait_ready();
#endif

    if (!wsf_conn->ssl && network_up) {
        LOGI(MODULE_NAME, "try to reconnect");
        wsf_code result = wsf_open_connection(config);
        if (result != WSF_SUCCESS) {
            LOGE(MODULE_NAME, "failed to setup the link");
        }
        return;
    }
}

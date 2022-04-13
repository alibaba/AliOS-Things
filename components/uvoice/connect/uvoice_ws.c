
/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#include "uvoice_os.h"
#include "uvoice_ws.h"

#include "nopoll.h"
#include "lwip/netif.h"


#ifdef UVOICE_ON_BK7251
#define NOPOLL_CLIENT_TASK_STACK_SIZE	8192
#else
#define NOPOLL_CLIENT_TASK_STACK_SIZE	4096
#endif
#define NOPOLL_CLIENT_TASK_PRIORITY		UVOICE_TASK_PRI_LOWER

#define WAIT_MS_MAX						200
#define WAIT_MS_MIN						20
#define PING_INTERVAL_COUNT				100 /* max: WAIT_MS_MAX * PING_INTERVAL_COUNT < 45 seconds */

#define NOPOLL_PING_PERIOD_SEC			20
#define NOPOLL_PONG_TIMEOUT_MSEC		10000 /* keep NOPOLL_PONG_TIMEOUT_MSEC < NOPOLL_PING_PERIOD_SEC */

#define MERGE_TEXT_FRAME_SUPPORT		1

enum {
	NOPOLL_CMD_EXIT,
	NOPOLL_CMD_OPEN,
	NOPOLL_CMD_SEND_TEXT,
	NOPOLL_CMD_SEND_BINARY,
	NOPOLL_CMD_CLOSE
};

typedef struct {
	int type;
	void *data;
} nopoll_msg_t;

typedef struct {
	uint8_t *buff;
	uint32_t size;
	ws_bin_type_t type;
} nopoll_xfer_t;

typedef struct {
	char server[128];
	char port[8];
	char schema[8];
	char *cacert;
	char *path;
} nopoll_conn_info_t;

typedef struct {
	noPollCtx *ctx;
	noPollConn *conn;
	noPollConnOpts *conn_opts;
	nopoll_conn_info_t conn_info;
	char *msg_buffer;
	int msg_count;
	char *prev_text;
	int prev_text_size;
	uint8_t recv_disable:1;
	uint8_t pong_recv:1;
	int heartbeat_type;
	int prev_recv_type;
	os_mutex_t lock;
	os_sem_t exit_sem;
	os_task_t task;
	os_queue_t queue;
	ws_cb_ops_t callback;
	ws_conn_state_t ws_conn_state;
	long long conn_active_time;
} ws_handler_t;

static ws_handler_t *g_ws_handler;

static int nopoll_client_close(ws_handler_t *handler);

static void active_time_update(ws_handler_t *handler)
{
	handler->conn_active_time = os_current_time();
}

static int active_time_get(ws_handler_t *handler)
{
	return handler->conn_active_time / 1000;
}

static void inline msg_count_add(ws_handler_t *handler)
{
	os_mutex_lock(handler->lock, OS_WAIT_FOREVER);
	handler->msg_count++;
	os_mutex_unlock(handler->lock);
}

static void inline msg_count_dec(ws_handler_t *handler)
{
	os_mutex_lock(handler->lock, OS_WAIT_FOREVER);
	handler->msg_count--;
	os_mutex_unlock(handler->lock);
}

static void nopoll_message_handle(ws_handler_t *handler, noPollMsg *msg)
{
	char *content;
	noPollOpCode type;
	int size;
	bool final;

	size = nopoll_msg_get_payload_size(msg);
	type = nopoll_msg_opcode(msg);
	final = nopoll_msg_is_final(msg);
	content = (char *)nopoll_msg_get_payload(msg);

#if MERGE_TEXT_FRAME_SUPPORT
	if (handler->prev_text_size > 0 && handler->prev_text) {
		if (size > 0 && type == NOPOLL_TEXT_FRAME) {
			handler->prev_text = snd_realloc(handler->prev_text,
				handler->prev_text_size + size, AFM_EXTN);
			if (!handler->prev_text) {
				M_LOGE("realloc prev text failed !\n");
				handler->prev_text_size = 0;
				return;
			}
			memcpy(handler->prev_text + handler->prev_text_size,
				content, size);
			handler->prev_text_size += size;
			if (final) {
				handler->callback.recv_text_cb(handler->prev_text,
					handler->prev_text_size);
				snd_free(handler->prev_text);
				handler->prev_text = NULL;
				handler->prev_text_size = 0;
			}
			return;
		} else {
			M_LOGD("expect text frame, receive type %d\n", type);
			handler->callback.recv_text_cb(handler->prev_text,
				handler->prev_text_size);
			snd_free(handler->prev_text);
			handler->prev_text = NULL;
			handler->prev_text_size = 0;
		}
	} else if (size > 0 && type == NOPOLL_TEXT_FRAME && !final) {
		if (handler->prev_text) {
			snd_free(handler->prev_text);
			handler->prev_text_size = 0;
		}

		handler->prev_text = snd_zalloc(size, AFM_EXTN);
		if (!handler->prev_text) {
			M_LOGE("alloc prev text buffer failed !\n");
			return;
		}
		handler->prev_text_size = size;
		memcpy(handler->prev_text, content, size);
		return;
	}
#endif

	switch (type) {
	case NOPOLL_TEXT_FRAME:
		handler->prev_recv_type = NOPOLL_TEXT_FRAME;
		handler->callback.recv_text_cb(content, size);
		break;
	case NOPOLL_BINARY_FRAME:
		handler->prev_recv_type = NOPOLL_BINARY_FRAME;
		if (final) {
			handler->callback.recv_binary_cb(content,
				size, WS_BIN_DATA_FINISH);
		} else {
			handler->callback.recv_binary_cb(content,
				size, WS_BIN_DATA_START);
		}
		break;
	case NOPOLL_CONTINUATION_FRAME:
		if (handler->prev_recv_type == NOPOLL_TEXT_FRAME) {
			handler->callback.recv_text_cb(content, size);
		} else {
			if (final)
				handler->callback.recv_binary_cb(content,
					size, WS_BIN_DATA_FINISH);
			else
				handler->callback.recv_binary_cb(content,
					size, WS_BIN_DATA_CONTINUE);
		}
		break;
	case NOPOLL_PONG_FRAME:
		handler->pong_recv = 1;
		break;
	default:
		break;
	}
}

static void __nopoll_conn_open(ws_handler_t *handler,
	nopoll_xfer_t *data)
{
	nopoll_conn_info_t *conn_info;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return;
	}

	handler->conn_opts = nopoll_conn_opts_new();
	if (!handler->conn_opts) {
		M_LOGE("create connect opts failed !\n");
		goto __err_exit;
	}

	conn_info = &handler->conn_info;

	if (!conn_info->cacert) {
		if (conn_info->path)
			handler->conn = nopoll_conn_new_opts(handler->ctx,
				handler->conn_opts,
				conn_info->server,
				conn_info->port,
				NULL,
				conn_info->path,
				NULL, NULL);
		else
			handler->conn = nopoll_conn_new(handler->ctx,
				conn_info->server,
				conn_info->port,
				conn_info->server,
				NULL, NULL, NULL);
	} else {
		if (!nopoll_conn_opts_set_ssl_certs(handler->conn_opts,
			NULL, 0,
			NULL, 0,
			NULL, 0,
			conn_info->cacert,
			strlen(conn_info->cacert) + 1)) {
			M_LOGE("set ssl certs failed !\n");
			goto __err_exit;
		}

		nopoll_conn_opts_ssl_peer_verify(handler->conn_opts,
			nopoll_false);
		handler->conn = nopoll_conn_tls_new(handler->ctx,
			handler->conn_opts, conn_info->server,
			conn_info->port,
			NULL,
			conn_info->path,
			NULL, NULL);
	}

	if (!handler->conn) {
		M_LOGE("create nopoll connection failed !\n");
		handler->conn_opts = NULL;
		goto __err_exit;
	}

	if (!nopoll_conn_wait_until_connection_ready(
		handler->conn, 3)) {
		M_LOGE("connection timeout !\n");
		goto __err_exit;
	}

	handler->ws_conn_state = WS_CONN_STAT_CONNECTED;
	handler->callback.connect_cb();
	return;

__err_exit:
	nopoll_conn_unref(handler->conn);
	nopoll_conn_opts_free(handler->conn_opts);
	handler->conn = NULL;
	handler->conn_opts = NULL;
	handler->ws_conn_state = WS_CONN_STAT_DISCONNECTED;
}

static void __nopoll_conn_close(ws_handler_t *handler, void *data)
{
	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	nopoll_conn_close(handler->conn);
	handler->conn = NULL;
	handler->conn_opts = NULL;

	handler->ws_conn_state = WS_CONN_STAT_DISCONNECTED;
	handler->callback.disconnect_cb();
}

static int __nopoll_conn_send_continue(noPollConn * conn,
	const char *content, long length)
{
	return __nopoll_conn_send_common(conn,
		content,
		length,
		nopoll_true,
		0,
		NOPOLL_CONTINUATION_FRAME);
}

static int __nopoll_conn_send_continue_fragment(noPollConn * conn,
	const char *content, long length)
{
	return __nopoll_conn_send_common(conn,
		content,
		length,
		nopoll_false,
		0,
		NOPOLL_CONTINUATION_FRAME);
}

static int __nopoll_conn_complete_pending_write(noPollConn *conn)
{
	int try_times = 0;
	while (try_times < 5 && errno == NOPOLL_EWOULDBLOCK &&
		nopoll_conn_pending_write_bytes(conn) > 0) {
		os_msleep(50);
		if (nopoll_conn_complete_pending_write(conn) == 0)
			return 0;
		try_times++;
	}
	return 1;
}

static void __nopoll_conn_send_text(ws_handler_t *handler,
	nopoll_xfer_t *data)
{
	int ret;

	ret = nopoll_conn_send_text(handler->conn,
		(char *)data->buff, data->size);
	if (ret != data->size) {
		if (__nopoll_conn_complete_pending_write(handler->conn))
			M_LOGE("size %u ret %d\n", data->size, ret);
	}

	snd_free(data->buff);
	snd_free(data);
}

static void __nopoll_conn_send_binary(ws_handler_t *handler,
	nopoll_xfer_t *data)
{
	int ret = 0;

	switch (data->type) {
	case WS_BIN_DATA_START:
		ret = nopoll_conn_send_binary_fragment(handler->conn,
			(char *)data->buff, data->size);
		break;
	case WS_BIN_DATA_CONTINUE:
		ret = __nopoll_conn_send_continue_fragment(handler->conn,
			(char *)data->buff, data->size);
		break;
	case WS_BIN_DATA_FINISH:
		ret = __nopoll_conn_send_continue(handler->conn,
			(char *)data->buff, data->size);
		break;
	default:
		M_LOGE("unknown data type\n");
		break;
	}

	if (ret != data->size) {
		if (__nopoll_conn_complete_pending_write(handler->conn))
			M_LOGE("size %u, ret %d\n", data->size, ret);
	}

	snd_free(data->buff);
	snd_free(data);
}

static int __nopoll_conn_try_get_msg(ws_handler_t *handler)
{
	noPollMsg *msg;

	if (handler->recv_disable)
		return -1;

	if (!nopoll_conn_is_ok(handler->conn)) {
		if (handler->ws_conn_state == WS_CONN_STAT_CONNECTED) {
			M_LOGE("connect down, close nopoll client\n");
			__nopoll_conn_close(handler, NULL);
		}
		return -1;
	}

	msg = nopoll_conn_get_msg(handler->conn);
	if (!msg)
		return -1;

	active_time_update(handler);

	nopoll_message_handle(handler, msg);

	nopoll_msg_unref(msg);
	return 0;
}

static int __nopoll_conn_ping_test(ws_handler_t *handler)
{
	long long time_active;
	long long time_now;

	if (!nopoll_conn_is_ok(handler->conn)) {
		if (handler->ws_conn_state == WS_CONN_STAT_CONNECTED) {
			M_LOGE("connection is closed\n");
			__nopoll_conn_close(handler, NULL);
		}
		return -1;
	}

	time_now = os_current_time();
	time_now /= 1000;

	time_active = active_time_get(handler);

	if (time_now <= (time_active + NOPOLL_PING_PERIOD_SEC))
		return -1;

	nopoll_conn_send_ping(handler->conn);
	active_time_update(handler);
	return 0;
}

static void __nopoll_conn_pong_check(ws_handler_t *handler)
{
	if (handler->pong_recv)
		return;
	M_LOGE("recv pong frame timeout !\n");
	__nopoll_conn_close(handler, NULL);
}

static void nopoll_client_task(void *arg)
{
	ws_handler_t *handler = (ws_handler_t *)arg;
	struct netif *netif;
	int wait_time = WAIT_MS_MIN;
	int ping_count = 0;
	bool ping_sent = false;
	int ping_time = 0;
	nopoll_msg_t msg;
	unsigned int msg_size = 0;
	bool task_exit = false;
	int ret;

	netif = netif_find("en1");
	if (!netif) {
		M_LOGE("find netif failed !\n");
		return;
	}

	while (!task_exit) {
		ret = os_queue_recv(&handler->queue, 30, &msg, &msg_size);
		if (ret || msg_size != sizeof(nopoll_msg_t)) {
			if (!netif_is_up(netif) || !netif_is_link_up(netif)) {
				if (handler->ws_conn_state == WS_CONN_STAT_CONNECTED) {
					M_LOGE("netif down ! close connection\n");
					__nopoll_conn_close(handler, NULL);
				}
			}

			if (__nopoll_conn_try_get_msg(handler)) {
				wait_time <<= 1;
				if (wait_time > WAIT_MS_MAX)
					wait_time = WAIT_MS_MAX;
			} else {
				wait_time = WAIT_MS_MIN;
			}

			if (handler->heartbeat_type != 0) {
				if (ping_count++ > PING_INTERVAL_COUNT) {
					ping_count = 0;
					if (!__nopoll_conn_ping_test(handler)) {
						if (handler->heartbeat_type == 2)
							ping_sent = true;

						handler->pong_recv = 0;
						ping_time = 0;
					}
				}

				if (ping_sent) {
					if (!handler->recv_disable) {
						if (ping_time > NOPOLL_PONG_TIMEOUT_MSEC) {
							__nopoll_conn_pong_check(handler);
							ping_sent = false;
							ping_time = 0;
						} else {
							ping_time += wait_time;
						}
					} else {
						ping_sent = false;
						ping_time = 0;
					}
				}
			}

			os_msleep(wait_time);
			continue;
		}

		msg_count_dec(handler);

		switch (msg.type) {
		case NOPOLL_CMD_EXIT:
			task_exit = true;
			break;
		case NOPOLL_CMD_OPEN:
			__nopoll_conn_open(handler, msg.data);
			break;
		case NOPOLL_CMD_SEND_TEXT:
			active_time_update(handler);
			__nopoll_conn_send_text(handler, msg.data);
			break;
		case NOPOLL_CMD_SEND_BINARY:
			active_time_update(handler);
			__nopoll_conn_send_binary(handler, msg.data);
			break;
		case NOPOLL_CMD_CLOSE:
			__nopoll_conn_close(handler, msg.data);
			break;
		default:
			M_LOGW("unknown msg\n");
			break;
		}
	}
	os_sem_signal(handler->exit_sem);
}

static int nopoll_client_open(ws_handler_t *handler,
	ws_conn_info_t *info)
{
	nopoll_conn_info_t *conn_info;
	nopoll_msg_t msg;
	int ret;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	if (!handler->ctx) {
		M_LOGE("nopoll ctx null !\n");
		return -1;
	}

	if (handler->conn) {
		M_LOGW("conn already open, close it\n");
		nopoll_client_close(handler);
	}

	conn_info = &handler->conn_info;

	if (conn_info->cacert) {
		snd_free(conn_info->cacert);
		conn_info->cacert = NULL;
	}
	if (conn_info->path) {
		snd_free(conn_info->path);
		conn_info->path = NULL;
	}

	memcpy(conn_info->server, info->server,
		strlen(info->server) + 1);
	snprintf(conn_info->port, sizeof(conn_info->port),
		"%u", info->port);
	memcpy(conn_info->schema, info->schema,
		strlen(info->schema) + 1);

	if (info->cacert) {
		conn_info->cacert = snd_zalloc(strlen(info->cacert) + 1,
			AFM_EXTN);
		if (!conn_info->cacert) {
			M_LOGE("alloc cacert failed !\n");
			return -1;
		}
		memcpy(conn_info->cacert, info->cacert,
			strlen(info->cacert) + 1);
	}

	if (info->path) {
		conn_info->path = snd_zalloc(strlen(info->path) + 1,
			AFM_EXTN);
		if (!conn_info->path) {
			M_LOGE("alloc path failed !\n");
			if (conn_info->cacert) {
				snd_free(conn_info->cacert);
				conn_info->cacert = NULL;
			}
			return -1;
		}
		memcpy(conn_info->path, info->path,
			strlen(info->path) + 1);
	}

	memcpy(&handler->callback, &info->callback,
		sizeof(ws_cb_ops_t));

	msg.type = NOPOLL_CMD_OPEN;
	msg.data = NULL;

	ret = os_queue_send(&handler->queue, &msg,
		sizeof(nopoll_msg_t));
	if (ret) {
		M_LOGE("send msg failed %d ! msg count %d\n",
			ret, handler->msg_count);
		return -1;
	}

	msg_count_add(handler);
	return 0;
}

static int nopoll_client_close(ws_handler_t *handler)
{
	nopoll_conn_info_t *info;
	nopoll_msg_t msg;
	int ret;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	info = &handler->conn_info;
	if (info->cacert) {
		snd_free(info->cacert);
		info->cacert = NULL;
	}

	if (info->path) {
		snd_free(info->path);
		info->path = NULL;
	}

	if (!handler->conn) {
		M_LOGW("nopoll not open, ignore\n");
		return -1;
	}

	msg.type = NOPOLL_CMD_CLOSE;
	msg.data = NULL;

	ret = os_queue_send(&handler->queue, &msg,
		sizeof(nopoll_msg_t));
	if (ret) {
		M_LOGE("send msg failed %d ! msg count %d\n",
			ret, handler->msg_count);
		return -1;
	}

	msg_count_add(handler);
	return 0;
}

static int nopoll_client_send_text(ws_handler_t *handler,
	char *text, int len)
{
	nopoll_xfer_t *xfer;
	nopoll_msg_t msg;
	int ret;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	if (len <= 0) {
		M_LOGE("text len %d invalid !\n", len);
		return -1;
	}

	xfer = snd_zalloc(sizeof(nopoll_xfer_t), AFM_EXTN);
	if (!xfer) {
		M_LOGE("alloc nopoll xfer failed !\n");
		return -1;
	}

	xfer->buff = snd_zalloc(len, AFM_MAIN);
	if (!xfer->buff) {
		M_LOGE("alloc buffer failed !\n");
		snd_free(xfer);
		return -1;
	}

	memcpy(xfer->buff, text, len);
	xfer->size = len;

	msg.type = NOPOLL_CMD_SEND_TEXT;
	msg.data = xfer;

	ret = os_queue_send(&handler->queue, &msg,
		sizeof(nopoll_msg_t));
	if (ret) {
		M_LOGE("send msg failed %d ! msg count %d\n",
			ret, handler->msg_count);
		snd_free(xfer->buff);
		snd_free(xfer);
		return -1;
	}

	msg_count_add(handler);
	return 0;
}

static int nopoll_client_send_binary(ws_handler_t *handler,
	void *pdata, int len, ws_bin_type_t type)
{
	nopoll_xfer_t *xfer;
	nopoll_msg_t msg;
	int ret;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	if (len < 0) {
		M_LOGE("bin len %d invalid !\n", len);
		return -1;
	}

	xfer = snd_zalloc(sizeof(nopoll_xfer_t), AFM_EXTN);
	if (!xfer) {
		M_LOGE("alloc nopoll send data failed !\n");
		return -1;
	}

	if (len == 0)
		len = 32;

	xfer->buff = snd_zalloc(len, AFM_MAIN);
	if (!xfer->buff) {
		M_LOGE("alloc buffer failed !\n");
		snd_free(xfer);
		return -1;
	}
	memcpy(xfer->buff, pdata, len);

	xfer->size = len;
	xfer->type = type;

	msg.type = NOPOLL_CMD_SEND_BINARY;
	msg.data = xfer;

	ret = os_queue_send(&handler->queue, &msg,
		sizeof(nopoll_msg_t));
	if (ret) {
		M_LOGE("send msg failed %d ! msg count %d\n",
			ret, handler->msg_count);
		snd_free(xfer->buff);
		snd_free(xfer);
		return -1;
	}

	msg_count_add(handler);
	return 0;
}

static int nopoll_client_create(ws_handler_t *handler)
{
	if (!handler)
		return -1;

	if (handler->ctx) {
		M_LOGW("nopoll client exist, ignore\n");
		goto __exit;
	}

	handler->ctx = nopoll_ctx_new();
	if (!handler->ctx) {
		M_LOGE("create nopoll ctx failed !\n");
		return -1;
	}

	if (os_task_create(&handler->task,
		"nopoll_client_task",
		nopoll_client_task,
		handler,
		NOPOLL_CLIENT_TASK_STACK_SIZE,
		NOPOLL_CLIENT_TASK_PRIORITY)) {
		M_LOGE("create nopoll client task failed !\n");
		return -1;
	}

	M_LOGI("nopoll client create\n");

__exit:
	return 0;
}

static int nopoll_client_release(ws_handler_t *handler)
{
	nopoll_msg_t msg;
	int ret;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	nopoll_ctx_unref(handler->ctx);
	handler->ctx = NULL;

	msg.type = NOPOLL_CMD_EXIT;
	msg.data = NULL;

	ret = os_queue_send(&handler->queue, &msg,
		sizeof(nopoll_msg_t));
	if (ret) {
		M_LOGE("send msg failed %d ! msg count %d\n",
			ret, handler->msg_count);
		return -1;
	}

	msg_count_add(handler);

	if (os_sem_wait(handler->exit_sem, 5000)) {
		M_LOGE("wait exit sem timeout !\n");
		return -1;
	}

	M_LOGI("nopoll client release\n");
	return 0;
}

int uvoice_ws_connect(ws_conn_info_t *info)
{
	ws_handler_t *handler = g_ws_handler;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	if (!info) {
		M_LOGE("info null !\n");
		return -1;
	}

	if (!info->server) {
		M_LOGE("server info null !\n");
		return -1;
	}

	if (handler->ws_conn_state != WS_CONN_STAT_DISCONNECTED) {
		M_LOGW("ws connected or connecting, ignore\n");
		goto __exit;
	}

	handler->ws_conn_state = WS_CONN_STAT_CONNECTING;

	M_LOGD("server:%s;port:%u;schema:%s;path:%s\n",
		info->server,
		info->port,
		info->schema ? info->schema : "null",
		info->path ? info->path : "null");

	if (nopoll_client_create(handler)) {
		M_LOGE("create nopoll client failed !\n");
		handler->ws_conn_state = WS_CONN_STAT_DISCONNECTED;
		return -1;
	}

	if (nopoll_client_open(handler, info)) {
		M_LOGE("open nopoll client failed !\n");
		nopoll_client_release(handler);
		handler->ws_conn_state = WS_CONN_STAT_DISCONNECTED;
		return -1;
	}

__exit:
	return 0;
}

int uvoice_ws_disconnect(void)
{
	ws_handler_t *handler = g_ws_handler;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	if (handler->ws_conn_state != WS_CONN_STAT_CONNECTED) {
		M_LOGE("ws not connected !\n");
		return -1;
	}

	M_LOGD("disconnect ws\n");

	nopoll_client_close(handler);

	nopoll_client_release(handler);

	return 0;
}

int uvoice_ws_conn_state(ws_conn_state_t *state)
{
	ws_handler_t *handler = g_ws_handler;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	if (!state) {
		M_LOGE("arg null !\n");
		return -1;
	}

	*state = handler->ws_conn_state;
	return 0;
}

int uvoice_ws_send_text(char *text, int len)
{
	ws_handler_t *handler = g_ws_handler;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	if (handler->ws_conn_state != WS_CONN_STAT_CONNECTED) {
		M_LOGE("ws not connected !\n");
		return -1;
	}

	return nopoll_client_send_text(handler, text, len);
}

int uvoice_ws_send_binary(char *data, int len, ws_bin_type_t type)
{
	ws_handler_t *handler = g_ws_handler;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	if (handler->ws_conn_state != WS_CONN_STAT_CONNECTED) {
		M_LOGE("ws not connected !\n");
		return -1;
	}

	return nopoll_client_send_binary(handler, data, len, type);
}

int uvoice_ws_recv_disable(int disable)
{
	ws_handler_t *handler = g_ws_handler;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	if (disable)
		handler->recv_disable = 1;
	else
		handler->recv_disable = 0;

	M_LOGD("set recv_disable %d\n", handler->recv_disable);
	return 0;
}

/*
 * ping-pong heartbeat type:
 * 0 - disable
 * 1 - send ping frame but don't check pong frame
 * 2 - send ping frame and check pong frame
 */
int uvoice_ws_heartbeat_set(int type)
{
	ws_handler_t *handler = g_ws_handler;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	if (type < 0 || type > 2) {
		M_LOGE("type %d invalid !\n", type);
		return -1;
	}

	if (handler->heartbeat_type != type) {
		handler->heartbeat_type = type;
		M_LOGD("set heartbeat_type %d\n",
			handler->heartbeat_type);
	}
	return 0;
}

int uvoice_ws_init(void)
{
	ws_handler_t *handler = g_ws_handler;
	int ret;

	if (handler)
		return 0;

	handler = snd_zalloc(sizeof(ws_handler_t), AFM_EXTN);
	if (!handler) {
		M_LOGE("alloc nopoll handler failed !\n");
		return -1;
	}

	handler->msg_buffer = snd_zalloc(sizeof(nopoll_msg_t) * 8,
		AFM_MAIN);
	if (!handler->msg_buffer) {
		M_LOGE("alloc msg buffer failed !\n");
		snd_free(handler);
		return -1;
	}

	ret = os_queue_new(&handler->queue, handler->msg_buffer,
		sizeof(nopoll_msg_t) * 8,
		sizeof(nopoll_msg_t));
	if (ret) {
		M_LOGE("create event queue failed %d!\n", ret);
		snd_free(handler->msg_buffer);
		snd_free(handler);
		return -1;
	}

	handler->exit_sem = os_sem_new(0);
	handler->lock = os_mutex_new();

	handler->ws_conn_state = WS_CONN_STAT_DISCONNECTED;

	g_ws_handler = handler;

	M_LOGI("ws init\n");
	return 0;
}

int uvoice_ws_deinit(void)
{
	ws_handler_t *handler = g_ws_handler;

	if (!handler) {
		M_LOGE("ws handler null !\n");
		return -1;
	}

	if (handler->ws_conn_state != WS_CONN_STAT_DISCONNECTED)
		uvoice_ws_disconnect();

	os_mutex_free(handler->lock);
	os_sem_free(handler->exit_sem);
	os_queue_free(&handler->queue);
	snd_free(handler->msg_buffer);
	snd_free(handler);
	g_ws_handler = NULL;

	M_LOGI("ws free\n");
	return 0;
}


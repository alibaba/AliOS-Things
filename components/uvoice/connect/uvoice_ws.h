/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_WS_H__
#define __UVOICE_WS_H__


typedef enum {
    WS_BIN_DATA_START,
    WS_BIN_DATA_CONTINUE,
    WS_BIN_DATA_FINISH,
} ws_bin_type_t;

typedef struct {
	void (*connect_cb)(void);
	void (*disconnect_cb)(void);
	void (*recv_text_cb)(char *text, int size);
	void (*recv_binary_cb)(char *data, int size, ws_bin_type_t type);
} ws_cb_ops_t;

typedef struct {
	uint16_t port;
    char *server;
    char *schema;
    char *cacert;
    char *path;
    ws_cb_ops_t callback;
} ws_conn_info_t;

typedef enum {
	WS_CONN_STAT_DISCONNECTED = 0,
	WS_CONN_STAT_CONNECTING,
	WS_CONN_STAT_CONNECTED,
} ws_conn_state_t;

#if (UVOICE_WS_ENABLE == 1)
int uvoice_ws_connect(ws_conn_info_t *info);
int uvoice_ws_disconnect(void);
int uvoice_ws_conn_state(ws_conn_state_t *state);
int uvoice_ws_send_text(char *text, int len);
int uvoice_ws_send_binary(char *data, int len, ws_bin_type_t type);
int uvoice_ws_recv_pause(int pause);
int uvoice_ws_heartbeat_set(int type);
int uvoice_ws_init(void);
int uvoice_ws_deinit(void);
#endif


#endif /*__UVOICE_WS_H__*/


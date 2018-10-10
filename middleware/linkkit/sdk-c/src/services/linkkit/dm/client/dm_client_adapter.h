#ifndef _DM_CLIENT_ADAPTER_H_
#define _DM_CLIENT_ADAPTER_H_

#define DM_CLIENT_REQUEST_TIMEOUT_MS    (2000)
#define DM_CLIENT_KEEPALIVE_INTERVAL_MS (60000)
#define DM_CLIENT_CONNECT_TIMEOUT_MS    (10000)

typedef struct {
    int fd;
    iotx_conn_info_t *conn_info;
    void *callback;
    char read_buff[CONFIG_MQTT_RX_MAXLEN];
    char write_buff[CONFIG_MQTT_TX_MAXLEN];
} dm_client_ctx_t;

int dm_client_open(void);
int dm_client_connect(void);
int dm_client_close(void);
int dm_client_subscribe(char *uri, iotx_cm_data_handle_cb callback, void *context);
int dm_client_unsubscribe(char *uri);
int dm_client_publish(char *uri, unsigned char *payload, int payload_len);
int dm_client_yield(unsigned int timeout);

#endif
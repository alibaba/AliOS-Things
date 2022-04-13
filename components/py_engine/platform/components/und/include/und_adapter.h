/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UND_ADAPTER_H
#define UND_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

int und_conn_is_connected();

int und_conn_unregister_cb(char *topic);

int und_conn_register_cb(char *topic, void *cb);

/*
 * send und packet, to isolate the detail of protocol (MQTT/HTTP/COAP)
 */
int und_conn_send(char *topic, void *data, int len, int qos);

#ifdef __cplusplus
}
#endif

#endif  /* end of UND_ADAPTER_H */

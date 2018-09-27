/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AT_MQTT_CLIENT_H_
#define _AT_MQTT_CLIENT_H_

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif


typedef struct at_mqtt_client_op_s {
    int (*conn)(char *proKey, char *devName, char *devSecret, uint8_t tlsEnable);
    int (*disconn)(void);
    int (*subscribe)(char *topic, uint8_t qos, int *mqtt_packet_id, int *mqtt_status);
    int (*unsubscribe)(char *topic, int *mqtt_packet_id, int *mqtt_status);
    int (*publish)(char *topic, uint8_t qos, int *message);
    int (*state)(void);
    int (*settings)(void);
} at_mqtt_client_op_t;


int at_mqtt_register(at_mqtt_client_op_t *client_ops);
int at_mqtt_unregister(void);
int at_mqtt_connect(char *proKey, char *devName, char *devSecret, uint8_t tlsEnable);
int at_mqtt_disconnect(void);
int at_mqtt_subscribe(char *topic, uint8_t qos, int *mqtt_packet_id, int *mqtt_status);
int at_mqtt_unsubscribe(char *topic, int *mqtt_packet_id, int *mqtt_status);
int at_mqtt_publish(char *topic, uint8_t qos, int *message);
int at_mqtt_yield(void);
int at_mqtt_state(void);
int at_mqtt_settings(void);


#ifdef __cplusplus
}
#endif

#endif


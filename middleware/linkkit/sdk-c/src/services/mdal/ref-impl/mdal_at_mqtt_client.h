/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _MDAL_AT_MQTT_CLIENT_H_
#define _MDAL_AT_MQTT_CLIENT_H_

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

typedef struct at_mqtt_client_op_s {
    int (*init)(void);
    int (*deinit)(void);
    int (*conn)(char *proKey, char *devName, char *devSecret, uint8_t tlsEnable);
    int (*disconn)(void);
    int (*subscribe)(char *topic, uint8_t qos, int *mqtt_packet_id, int *mqtt_status, int timeout_ms);
    int (*unsubscribe)(char *topic, int *mqtt_packet_id, int *mqtt_status);
    int (*publish)(char *topic, uint8_t qos, char *message);
    int (*state)(void);
    int (*sendat)(char *at_cmd, int timeout_ms);
} at_mqtt_client_op_t;


int HAL_AT_MQTT_Connect(char *proKey, char *devName, char *devSecret, uint8_t tlsEnable);
int HAL_AT_MQTT_Disconnect(void);
int HAL_AT_MQTT_Subscribe(char *topic, uint8_t qos, int *mqtt_packet_id, int *mqtt_status, int timeout_ms);
int HAL_AT_MQTT_Unsubscribe(char *topic, int *mqtt_packet_id, int *mqtt_status);
int HAL_AT_MQTT_Publish(char *topic, uint8_t qos, char *message);
int HAL_AT_MQTT_State(void);

int HAL_AT_MQTT_Savemsg(char *topic, char *message);
int HAL_AT_MQTT_Readmsg(char *topic, char *message);

#ifdef __cplusplus
}
#endif

#endif


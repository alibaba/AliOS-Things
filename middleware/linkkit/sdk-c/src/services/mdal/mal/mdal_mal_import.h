/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _MDAL_MAL_IMPORT_H_
#define _MDAL_MAL_IMPORT_H_

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

typedef int (*recv_cb)(char* topic, char* message);

int HAL_MDAL_MAL_Connect(char *proKey, char *devName, char *devSecret);
int HAL_MDAL_MAL_Disconnect(void);
int HAL_MDAL_MAL_Subscribe(const char *topic, int qos, unsigned int *mqtt_packet_id, int *mqtt_status, int timeout_ms);
int HAL_MDAL_MAL_Unsubscribe(const char *topic, unsigned int *mqtt_packet_id, int *mqtt_status);
int HAL_MDAL_MAL_Publish(const char *topic, int qos, const char *message);
int HAL_MDAL_MAL_State(void);

void HAL_MDAL_MAL_RegRecvCb(recv_cb);

#ifdef MAL_ICA_ENABLED
typedef void (*mal_ica_cb)(void *arg, char *buf, int buflen);

int HAL_MDAL_MAL_ICA_Init();
int HAL_MDAL_MAL_ICA_InputCb(const char *prefix, const char *postfix, int maxlen,
                  mal_ica_cb cb, void *arg);
int HAL_MDAL_MAL_ICA_Write(const char* at_cmd);
#endif

#ifdef __cplusplus
}
#endif

#endif


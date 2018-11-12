/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_PLATFORM_OS_H_
#define OTA_PLATFORM_OS_H_
#include "ota_service.h"

#define OTA_SSL_TIMEOUT 5000
#define ota_snprintf snprintf

/*memory*/
void *ota_malloc(int size);
void *ota_realloc(void *ptr, int size);
void *ota_calloc(int n, int size);
void ota_free(void *ptr);

/*Semaphore*/
void *ota_semaphore_create(void);
int ota_semaphore_wait(void *sem, int ms);
void ota_semaphore_post(void *sem);
void ota_semaphore_destroy(void *sem);

/*Thread*/
int ota_thread_create(
    void **thread_handle,
    void *(*work_routine)(void *),
    void *arg,
    void *param,
    int *stack_used);
void ota_thread_exit(void *thread);
void ota_msleep(int ms);

/*KV store*/
int ota_kv_set(const char *key, const void *val, int len, int sync);
int ota_kv_get(const char *key, void *buffer, int *len);

/*Reboot*/
void ota_reboot(void);

/*System version*/
const char *ota_get_sys_version(void);

/*Socket API*/
void* ota_socket_connect(char *host, int port);
int ota_socket_send(void* fd,  char *buf, int len);
int ota_socket_recv(void* fd,  char *buf, int len);
void ota_socket_close(void* fd);

/*SSL*/
void* ota_ssl_connect(const char *host, unsigned short port, const char *ca, int len);
int ota_ssl_send(void* ssl,  char *buf, int len);
int ota_ssl_recv(void* ssl,  char *buf, int len);

/*MQTT*/
int ota_hal_mqtt_publish(char *topic, int qos, void *data, int len);
int ota_hal_mqtt_subscribe(char *topic, void* cb, void *ctx);
int ota_hal_mqtt_deinit(void);
int ota_hal_mqtt_init(void);

/*COAP*/
int ota_coap_send(void *p_context, char *p_path, void *p_message);
int ota_coap_send_block(void *p_context, char *p_path, void *p_message,
                                  int block_type, int num, int more, int size);
int ota_coap_parse_block(void *p_message, int type, int *num,int *more, int *size);
int ota_coap_get_payload(void *p_message, const char **pp_payload, int *p_len);
int ota_coap_get_code(void *p_message, void *p_resp_code);
int ota_coap_init(void);
int ota_coap_deinit(void);
#endif

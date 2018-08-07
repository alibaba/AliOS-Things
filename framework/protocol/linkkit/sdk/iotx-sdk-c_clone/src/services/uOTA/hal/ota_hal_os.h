/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_PLATFORM_OS_H_
#define OTA_PLATFORM_OS_H_
#include <stdint.h>
#include "ota_service.h"
#include "iot_import.h"


#ifdef _IS_LINKKIT_
#define ota_snprintf HAL_Snprintf
#define ota_malloc HAL_Malloc
#define ota_free   HAL_Free
#endif

void *ota_malloc(uint32_t size);

void *ota_zalloc(uint32_t size);

void *ota_realloc(void *ptr, uint32_t size);

void ota_free(void *ptr);

void *ota_mutex_init(void);

int ota_mutex_lock(void *mutex);

int ota_mutex_unlock(void *mutex);

void ota_mutex_destroy(void *mutex);

void *ota_semaphore_init(void);

int ota_semaphore_wait(void *sem, int32_t timeout_ms);

void ota_semaphore_post(void *sem);

void ota_semaphore_destroy(void *sem);

uint32_t ota_now_ms(void);

void ota_msleep(uint32_t ms);

int ota_thread_create(
    void **thread_handle,
    void *(*work_routine)(void *),
    void *arg,
    void* param,
    int *stack_used);

void ota_thread_exit(void *thread);

void *ota_timer_create(const char *name, void (*func)(void *), void *user_data);

int ota_timer_start(void *t, int ms);

void ota_reboot(void);

int ota_kv_set(const char *key, const void *val, int len, int sync);

int ota_kv_get(const char *key, void *buffer, int *buffer_len);

int ota_kv_del(const char *key);

int ota_kv_erase_all(void);

int ota_version_report(void);

int ota_HAL_GetProductKey(char pk[PRODUCT_KEY_MAXLEN]);

int ota_HAL_GetDeviceName(char dn[DEVICE_NAME_MAXLEN]);

int ota_HAL_GetDeviceSecret(char ds[DEVICE_SECRET_MAXLEN]);

void* ota_ssl_connect(int socketfd, const char *ca_crt, uint32_t ca_crt_len);

int32_t ota_ssl_send(void* ssl,  char *buf, uint32_t len);

int32_t ota_ssl_recv(void* ssl,  char *buf, uint32_t len);

const char *ota_iotx_ca_get(void);
/*MQTT & COAP HAL*/
int ota_hal_mqtt_publish(char *topic, int qos, void *data, int len);
int ota_hal_mqtt_subscribe(char *topic, void (*cb)(char *topic, int topic_len, void *payload, int payload_len, void *ctx), void *ctx);
int ota_hal_mqtt_deinit_instance(void);
int ota_hal_mqtt_init_instance(char *productKey, char *deviceName, char *deviceSecret, int maxMsgSize);
void* ota_hal_iot_mqtt_field(void* ctx);

int ota_IOT_CoAP_SendMessage(void *p_context,   char *p_path, void *p_message);
int ota_IOT_CoAP_SendMessage_block(void *p_context, char *p_path, void *p_message,
                               unsigned int block_type, unsigned int num, unsigned int more, unsigned int size);
int ota_IOT_CoAP_ParseOption_block(void *p_message, int type, unsigned int *num,
                                unsigned int *more, unsigned int *size);
int ota_IOT_CoAP_GetMessagePayload(void *p_message, unsigned char **pp_payload, int *p_len);
int ota_IOT_CoAP_GetMessageCode(void *p_message, void *p_resp_code);
void *ota_IOT_CoAP_Init(void *p_config);
int ota_IOT_CoAP_DeviceNameAuth(void *p_context);
int ota_IOT_CoAP_Deinit(void **pp_context);
#endif

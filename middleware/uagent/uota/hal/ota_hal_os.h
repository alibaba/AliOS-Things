/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_PLATFORM_OS_H_
#define OTA_PLATFORM_OS_H_
#include <stdint.h>
#include "ota_service.h"
#include "iot_import.h"

#define OTA_SSL_TIMEOUT 5000

#ifndef OTA_WITH_LINKKIT
#define ota_snprintf snprintf
#else
#define ota_snprintf      HAL_Snprintf
#define ota_malloc        HAL_Malloc
#define ota_free          HAL_Free
#define ota_calloc        HAL_Calloc
#define ota_realloc       HAL_Realloc
#define ota_mutex_init    HAL_MutexCreate
#define ota_mutex_lock    HAL_MutexLock
#define ota_mutex_unlock  HAL_MutexUnlock
#define ota_mutex_destroy HAL_MutexDestroy
#define ota_semaphore_init HAL_SemaphoreCreate
#define ota_semaphore_wait HAL_SemaphoreWait
#define ota_semaphore_post HAL_SemaphorePost
#define ota_semaphore_destroy HAL_SemaphoreDestroy
#define ota_msleep        HAL_SleepMs
#define ota_thread_create HAL_ThreadCreate
#define ota_thread_exit   HAL_ThreadDelete
#define ota_kv_set        HAL_Kv_Set
#define ota_kv_get        HAL_Kv_Get
#define ota_timer_create  HAL_Timer_Create
#define ota_timer_start   HAL_Timer_Start
#define ota_reboot        HAL_Reboot
#endif

/*memory*/
void *ota_malloc(uint32_t size);
void *ota_realloc(void *ptr, uint32_t size);
void *ota_calloc(uint32_t n, uint32_t size);
void ota_free(void *ptr);

/*Mutex*/
void *ota_mutex_init(void);
void ota_mutex_lock(void *mutex);
void ota_mutex_unlock(void *mutex);
void ota_mutex_destroy(void *mutex);

/*Semaphore*/
void *ota_semaphore_init(void);
int ota_semaphore_wait(void *sem, uint32_t timeout_ms);
void ota_semaphore_post(void *sem);
void ota_semaphore_destroy(void *sem);

/*Thread*/
int ota_thread_create(
    void **thread_handle,
    void *(*work_routine)(void *),
    void *arg,
    hal_os_thread_param_t *param,
    int *stack_used);
void ota_thread_exit(void *thread);

/*Timer*/
void *ota_timer_create(const char *name, void (*func)(void *), void *user_data);
int ota_timer_start(void *t, int ms);
void ota_msleep(uint32_t ms);

/*KV store*/
int ota_kv_set(const char *key, const void *val, int len, int sync);
int ota_kv_get(const char *key, void *buffer, int *buffer_len);

void ota_reboot(void);
/*PK DN*/
int ota_HAL_GetProductKey(char pk[PRODUCT_KEY_MAXLEN]);
int ota_HAL_GetProductSecret(char ps[PRODUCT_SECRET_MAXLEN]);
int ota_HAL_GetDeviceName(char dn[DEVICE_NAME_MAXLEN]);
int ota_HAL_GetDeviceSecret(char ds[DEVICE_SECRET_MAXLEN]);

/*Socket API*/
int ota_socket_connect(char *host, int port);
int ota_socket_send(int fd,  char *buf, size_t len);
int ota_socket_recv(int fd,  char *buf, size_t len);
void ota_socket_close(int fd);

/*SSL*/
void* ota_ssl_connect(const char *host, uint16_t port, const char *ca_crt,uint32_t ca_crt_len);
int32_t ota_ssl_send(void* ssl,  char *buf, uint32_t len);
int32_t ota_ssl_recv(void* ssl,  char *buf, uint32_t len);

/*MQTT*/
int ota_hal_mqtt_publish(char *topic, int qos, void *data, int len);
int ota_hal_mqtt_subscribe(char *topic, void (*cb)(char *topic, int topic_len, 
                             void *payload, int payload_len, void *ctx), void *ctx);
int ota_hal_mqtt_deinit_instance(void);
int ota_hal_mqtt_init_instance(char *productKey, char *deviceName, char *deviceSecret, int maxMsgSize);

/*COAP*/
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

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _WRAPPERS_ATM_H_
#define _WRAPPERS_ATM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>

#include "infra_config.h"
#include "wrappers_defs.h"
#include "wrappers.h"

#ifndef NULL
#define NULL (void *)0
#endif

#define HAL_WAIT_FOREVER 0xFFFFFFFFU


#if defined(AT_PARSER_ENABLED)


/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_AT_Uart_Init(uart_dev_t *uart);

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_AT_Uart_Deinit(uart_dev_t *uart);

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart     the UART interface
 * @param[in]  data     pointer to the start of data
 * @param[in]  size     number of bytes to transmit
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_AT_Uart_Send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout);

/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[out]  recv_size    number of bytes received
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_AT_Uart_Recv(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout);
#endif

#if defined(AT_TCP_ENABLED)


/**
 * Module low level init so that it's ready to setup socket connection.
 *
 * @return  0 - success, -1 - failure
 */
int HAL_AT_CONN_Init(void);


/**
 * Start a socket connection via module.
 *
 * @param[in]  conn - connect parameters which are used to setup
 *                 the socket connection.
 *
 * @return  0 - success, -1 - failure
 */
int HAL_AT_CONN_Start(at_conn_t *conn);


/**
 * Send data via module.
 * This function does not return until all data sent.
 *
 * @param[in]  fd - the file descripter to operate on.
 * @param[in]  data - pointer to data to send.
 * @param[in]  len - length of the data.
 * @param[in]  remote_ip - remote port number (optional).
 * @param[in]  remote_port - remote port number (optional).
 *
 * @return  0 - success, -1 - failure
 */
int HAL_AT_CONN_Send(int fd, uint8_t *data, uint32_t len, char remote_ip[16],
                     int32_t remote_port, int32_t timeout);


/**
 * Get IP information of the corresponding domain.
 * Currently only one IP string is returned (even when the domain
 * coresponses to mutliple IPs). Note: only IPv4 is supported.
 *
 * @param[in]   domain - the domain string.
 * @param[out]  ip - the place to hold the dot-formatted ip string.
 *
 * @return  0 - success, -1 - failure
 */
int HAL_AT_CONN_DomainToIp(char *domain, char ip[16]);


/**
 * Close the socket connection.
 *
 * @param[in]  fd - the file descripter to operate on.
 * @param[in]  remote_port - remote port number (optional).
 *
 * @return  0 - success, -1 - failure
 */
int HAL_AT_CONN_Close(int fd, int32_t remote_port);


/**
 * Destroy SAL or exit low level state if necessary.
 *
 * @return  0 - success, -1 - failure
 */
int HAL_AT_CONN_Deinit(void);

#elif defined(AT_MQTT_ENABLED)
#include "linkkit/mqtt_api.h"

struct at_mqtt_input {
    char     *topic;
    uint32_t  topic_len;
    char     *message;
    uint32_t  msg_len;
};

int HAL_AT_MQTT_Init(iotx_mqtt_param_t *pInitParams);
int HAL_AT_MQTT_Deinit(void);
int HAL_AT_MQTT_Connect(char *proKey, char *devName, char *devSecret);
int HAL_AT_MQTT_Disconnect(void);
int HAL_AT_MQTT_Subscribe(const char *topic, int qos, unsigned int *mqtt_packet_id, int *mqtt_status, int timeout_ms);
int HAL_AT_MQTT_Unsubscribe(const char *topic, unsigned int *mqtt_packet_id, int *mqtt_status);
int HAL_AT_MQTT_Publish(const char *topic, int qos, const char *message, unsigned int msg_len);
int HAL_AT_MQTT_State(void);

#endif

#ifdef __cplusplus
}
#endif

#endif



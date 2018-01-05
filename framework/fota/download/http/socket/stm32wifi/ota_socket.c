/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/network.h>

#include "stm32_wifi.h"
#include "ota_log.h"
#include "ota_socket.h"

#define WIFI_WRITE_TIMEOUT   200
#define WIFI_READ_TIMEOUT    200
#define MAX_RCV_RETRY_TIME   5


int ota_socket_connect(int port, char *host_addr)
{
    if (host_addr == NULL || strlen(host_addr) == 0 || port <= 0) {
        OTA_LOG_E("ota_socket_connect parms   error\n ");
        return -1;
    }
    WIFI_Status_t ret;
    uint8_t ip_addr[4];
    int fake_socket = 1;

    ret = WIFI_GetHostAddress((char *)host_addr, ip_addr);
    if (ret != WIFI_STATUS_OK) {
        OTA_LOG_E("ota_socket_connect: get host addr fail - %d\n", ret);
        return -1;
    }

    ret = WIFI_OpenClientConnection(fake_socket, WIFI_TCP_PROTOCOL, "", ip_addr, port, 1);
    if (ret != WIFI_STATUS_OK) {
        OTA_LOG_E("ota_socket_connect: open client fail - %d\n", ret);
        return -1;
    }

    return fake_socket;
}

int ota_socket_send(int socket, const char *buf, size_t len)
{
    WIFI_Status_t ret;
    uint16_t send_size;

    if (socket < 0) {
        OTA_LOG_E("ota_socket_send: invalid socket fd\n");
        return -1;
    }
    OTA_LOG_I("ota_socket_send: len = %d\n", len);
    ret = WIFI_SendData((uint8_t)socket,
                        (uint8_t *)buf, len,
                        &send_size, WIFI_WRITE_TIMEOUT);
    if (ret != WIFI_STATUS_OK) {
        OTA_LOG_E("ota_socket_send: send data fail - %d\n", ret);
        return -1;
    }

    return send_size;
}

int ota_socket_recv(int socket,  char *buf, size_t len)
{
    WIFI_Status_t ret;
    uint16_t recv_size;

    if (socket < 0) {
        OTA_LOG_E("ota_socket_recv: invalid socket fd\n");
        return -1;
    }
    if (buf == NULL) {
        OTA_LOG_E("ota_socket_recv: buf is NULL\n");
        return -1;
    }

    if (len > ES_WIFI_PAYLOAD_SIZE) {
        len = ES_WIFI_PAYLOAD_SIZE;
    }
    int err_count = 0;
    do {
        ret = WIFI_ReceiveData((uint8_t)socket,
                               (uint8_t*)buf, (uint16_t)len,
                               &recv_size, WIFI_READ_TIMEOUT);
        if (ret != WIFI_STATUS_OK) {
            OTA_LOG_E("ota_socket_recv: receive data fail - %d\n", ret);
            return -1;
        }

        //TODO, how to identify the connection is shutdown?
        if (recv_size == 0) {
            if (err_count == MAX_RCV_RETRY_TIME) {
                OTA_LOG_E("ota_socket_recv: retry WIFI_ReceiveData %d times failed\n", err_count);
                return -1;
            } else {
                err_count++;
                OTA_LOG_E("ota_socket_recv: retry WIFI_ReceiveData time %d\n", err_count);
            }
        }
    } while (ret == WIFI_STATUS_OK && recv_size == 0);

    return recv_size;
}

void ota_socket_close(int socket)
{
    WIFI_Status_t ret;

    if (socket == -1) {
        return;
    }

    ret = WIFI_CloseClientConnection((uint32_t)socket);
    if (ret != WIFI_STATUS_OK) {
        OTA_LOG_E("ota_socket_free: close client fail - %d\n", ret);
        return;
    }
}

int ota_socket_check_conn(int sock)
{
    return sock;
}

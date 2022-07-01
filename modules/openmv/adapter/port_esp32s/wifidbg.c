/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * WiFi debugger.
 */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include "py/nlr.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/lexer.h"
#include "py/parse.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "usbdbg.h"
#include "sensor.h"
#include "framebuffer.h"
#include "omv_boardconfig.h"
#include "lib/utils/pyexec.h"


#if OMV_ENABLE_WIFIDBG

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

#define OPENMVCAM_BROADCAST_ADDR ((uint8_t [5]){255, 255, 255, 255})
#define OPENMVCAM_BROADCAST_PORT (0xABD1)
#define SERVER_ADDR              ((uint8_t [5]){192, 168, 1, 1})
#define SERVER_PORT              (9000)
#define BUFFER_SIZE              (512)

#define close_all_sockets()             \
    do {                                \
        close(client_fd);   \
        close(server_fd);   \
        close(udpbcast_fd); \
        client_fd = -1;                 \
        server_fd = -1;                 \
        udpbcast_fd = -1;               \
    } while (0)

#define close_server_socket()           \
    do {                                \
        close(server_fd);   \
        server_fd = -1;                 \
    } while (0)

#define close_udpbcast_socket()         \
    do {                                \
        close(udpbcast_fd); \
        udpbcast_fd = -1;               \
    } while (0)
#define MAKE_SOCKADDR(addr, ip, port) \
          struct sockaddr addr; \
          addr.sa_family = AF_INET; \
          addr.sa_data[0] = (uint8_t)(port >> 8); \
          addr.sa_data[1] = (uint8_t)(port); \
          addr.sa_data[2] = ip[0]; \
          addr.sa_data[3] = ip[1]; \
          addr.sa_data[4] = ip[2]; \
          addr.sa_data[5] = ip[3];

#define WIFI_IPV4_ADDR_LEN      (4)

static int client_fd = -1;
static int server_fd = -1;
static int udpbcast_fd = -1;
static int udpbcast_time = 0;
static uint8_t ip_addr[WIFI_IPV4_ADDR_LEN] = {};


#include "esp32_wifidbg.c"
#include "wifidbg.h"

#define UDPCAST_STRING           "%d.%d.%d.%d:%d:%s"
#define UDPCAST_STRING_SIZE      4+4+4+4+6+WIFI_MAX_BOARD_NAME_LEN+1
static char udpbcast_string[UDPCAST_STRING_SIZE] = {};


int wifidbg_init(wifidbg_config_t *config)
{
    client_fd = -1;
    server_fd = -1;
    udpbcast_fd = -1;

    if(!config->mode) { // STA Mode

        wifi_init_sta();

    } else { // AP Mode

        // Initialize WiFi in AP mode.
        wifi_init_softap();
        memcpy(ip_addr, SERVER_ADDR, WIFI_IPV4_ADDR_LEN);
    }

    snprintf(udpbcast_string, UDPCAST_STRING_SIZE, UDPCAST_STRING,
             ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3],
             SERVER_PORT, config->board_name);

    return 0;
}

void wifidbg_dispatch()
{
    int ret;
    uint8_t buf[BUFFER_SIZE];
    struct sockaddr client_sockaddr;

    if (client_fd < 0 && udpbcast_fd < 0) {
        // Create broadcast socket.
        uint8_t addr[5] = OPENMVCAM_BROADCAST_PORT;
        MAKE_SOCKADDR(udpbcast_sockaddr, addr, OPENMVCAM_BROADCAST_PORT)

        if ((udpbcast_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            return;
        }

        if ((ret = bind(udpbcast_fd, &udpbcast_sockaddr, sizeof(udpbcast_sockaddr))) < 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            close_udpbcast_socket();
            return;
        }

        return;
    }

    if (client_fd < 0 && (udpbcast_fd >= 0) && (!(udpbcast_time++ % 100))) {
        // Broadcast message to the IDE.
        uint8_t addr[5] = OPENMVCAM_BROADCAST_PORT;
        MAKE_SOCKADDR(udpbcast_sockaddr, OPENMVCAM_BROADCAST_ADDR, OPENMVCAM_BROADCAST_PORT)

        if ((ret = sendto(udpbcast_fd, (uint8_t *) udpbcast_string,
                        strlen(udpbcast_string) + 1, 0, &udpbcast_sockaddr, sizeof(udpbcast_sockaddr))) < 0) {
            ESP_LOGE(TAG, "Socket sendto failed: errno %d", errno);
            close_udpbcast_socket();
            return;
        }

        return;
    }

    if (server_fd < 0) {
        // Create server socket
        MAKE_SOCKADDR(server_sockaddr, ip_addr, SERVER_PORT)

        if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            return;
        }

        if ((ret = bind(server_fd, &server_sockaddr, sizeof(server_sockaddr))) < 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            close_all_sockets();
            return;
        }

        if ((ret = listen(server_fd, 1)) < 0) {
            ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
            close_all_sockets();
            return;
        }

        return;
    }

    if (client_fd < 0) {
        // Call accept.
        if ((client_fd = accept(server_fd,
                        &client_sockaddr, sizeof(client_sockaddr))) < 0) {
            if (ret < 0) {
                ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
                client_fd = -1;
            } else {
                close_all_sockets();
            }
            return;
        }

        return;
    }

    if ((ret = recv(client_fd, buf, 6, 0)) < 0) {
        if (ret < 0) {
            ESP_LOGE(TAG, "recv failed: errno %d", errno);
            return;
        } else {
            close_all_sockets();
            return;
        }
    }

    if (ret != 6 || buf[0] != 0x30) {
        return;
    }

    uint8_t request = buf[1];
    uint32_t xfer_length = *((uint32_t*)(buf+2));
    usbdbg_control(buf+6, request, xfer_length);

    while (xfer_length) {
        if (request & 0x80) {
            // Device-to-host data phase
            int bytes = MIN(xfer_length, BUFFER_SIZE);
            xfer_length -= bytes;
            usbdbg_data_in(buf, bytes);
            if ((ret = send(client_fd, buf, bytes, 500)) < 0) {
                ESP_LOGE(TAG, "send failed: errno %d", errno);
                close_all_sockets();
                return;
            }
        } else {
            // Host-to-device data phase
            int bytes = MIN(xfer_length, BUFFER_SIZE);
            if ((ret = recv(client_fd, buf, bytes, 0)) < 0) {
                ESP_LOGE(TAG, "recv failed stage2: errno %d", errno);
                close_all_sockets();
                return;
            }
            xfer_length -= ret;
            usbdbg_data_out(buf, ret);
        }
    }
}
#else
void wifidbg_dispatch(){};
#endif // OMV_ENABLE_WIFIDBG && MICROPY_PY_WINC1500

/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _YUNOS_BSP_SKTPT_H_
#define _YUNOS_BSP_SKTPT_H_

#include <stdint.h>

typedef enum {
    SKTPS_CONN_TCP,
    SKTPS_CONN_UDP
} sktps_conn_e;

/**
 * This function will init socket pass througth device
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_sktpt_init(void);

/**
 * This function will set default ap and save
 * @param[in]   ssid          ap ssid name
 * @param[in]   psw           ap password
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_sktpt_set_default_ap(const char *ssid, const char *psw);

/**
 * This function will get link status
 * @return      Zero on link success, -1 on failed
 */
int yunos_bsp_sktpt_get_link_status(void);


/**
 * This function will connect a server.
 * @param[in]   type        connect type udp or tcp
 * @param[in]   srvname     server name
 * @param[in]   port        socket port
 * @return      >=0 hanlde , -1 on failed
 */
int yunos_bsp_sktpt_connect(sktps_conn_e type, char *srvname, uint16_t port);

/**
 * This function disconnect a handle.
 * @param[in]   handle      connect handle
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_sktpt_disconnect(int handle);

/**
 * This function will send bytes to device.
 * @param[in]   handle      connect handle
 * @param[in]   pdata       data will be send
 * @param[in]   len         the byte length of data
 * @return      >=0 real sent len, -1 on failed
 */
int yunos_bsp_sktpt_send(int handle, const uint8_t *pdata, uint32_t len);

/**
 * This function will recv data from device.
 * @param[in]   handle      connect handle
 * @param[in]       pdata
 * @param[in]       data        A pointer to a buffer in which to receive data
 * @param[in]       len          in buffer len
 * @return          real recv len, -1 on failed
 */
int yunos_bsp_sktpt_recv(int handle, uint8_t *pdata, uint32_t len);

/**
 * This function get recv buffer status.
 * @param[in]   handle      connect handle
 * @return      > 0 recv buffer have data
 */
int yunos_bsp_sktpt_can_recv(int handle);

/**
 * This function get send ready status
 * @param[in]   handle      connect handle
 * @return      > 0 can send data
 */
int yunos_bsp_sktpt_can_send(int handle);

/**
 * This function will set wifi mode
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_sktpt_set_mode(uint8_t mode);


#endif

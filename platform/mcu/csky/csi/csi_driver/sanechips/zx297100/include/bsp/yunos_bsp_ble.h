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

#ifndef YUNOS_BSP_BLE_H
#define YUNOS_BSP_BLE_H

#include <stdint.h>

#define BLE_DATA                0
#define BLE_EVENT_CONNECT       1
#define BLE_EVENT_DISCONNECT    2

/** the ops struct of ble */
typedef struct _ble_ops_t
{
    int (*open) (void);               ///< The open of ble
    int (*close)(void);               ///< The close of ble
    int (*read) (uint8_t *, int);     ///< The read of ble
    int (*write)(uint8_t *, int);     ///< The write of ble
    void (*mdelay)(int);              ///< The mdelay of ble
} ble_ops_t;

/**
 * Open ble device
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_open(void);

/**
 * Close ble device
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_close(void);

/**
 * Read data from ble
 * @param[out]  buf         recv buffer
 * @param       len         input buffer len, output recv buffer len
 * @return      0 is BLE_DATA, 1 is BLE_EVENT_CONNECT, 2 is BLE_EVENT_DISCONNECT
 */
int yunos_bsp_ble_read_data(uint8_t * buf, int *len);

/**
 * Write data to ble
 *
 * @param[in]   buf         the buffer to be send
 * @param[in]   len         the send length 
 * @return      real write len
 */
int yunos_bsp_ble_write_data(uint8_t * buf, int len);

/**
 * Ctrl ble module rename
 *
 * @param[in]   name        name set to ble module to be found by host
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_ctrl_rename(char *name);

/**
 * Ctrl ble connect buadrate with host
 *
 * @param[in]   rateid      the id of rate
 * @return      0 on success; < 0 on failure
 * @note        the rateid, 0 - 2400, 1 - 9600, 2 - 1440, 3 - 19200,\
                            4 - 38400, 5 - 57600, 6 - 115200, 7 - 256000
 */
int yunos_bsp_ble_ctrl_baudrate(uint32_t rateid);


/**
 * Set bt mac address
 * @param[in]   btaddr      bt mac address, 6 byte array
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_ctrl_btaddr(uint8_t * btaddr);

/**
 * Ctrl ble module reset advertizing
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_ctrl_rstadv(void);

/**
 * Ctrl ble module adv inter value set
 * @param[in]   advintv     adv inter value set to ble module
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_ctrl_advintv(uint32_t advintv);

/**
 * Ctrl ble module to set adv data
 * @param[in]   advdata     adv data set to ble module
 * @param[in]   advlen      adv data length
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_ctrl_advdata(uint8_t *advdata, uint8_t advlen);

/**
 * Ctrl ble module to set connect device addr to filter the wrong device
 * @param[in]   conndaddr   connect device address set to pair with the right device\
                            bt mac address, 6 byte array
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_ctrl_conndaddr(uint8_t * conndaddr);

/**
 * Ctrl ble module to set the adv work or not
 * @param[in]   txlevel     tx power level
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_ctrl_txpower(uint8_t txlevel);

/**
 * Ctrl ble module to set the adv work or not
 * @param[in]   conndaddr   the adv status set to ble module, 1 to start, 0 to stop
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_ctrl_advwk(uint8_t state);

/* read ctrl info implemented */
/**
 * Read inter value of ble module
 * @param[out]  info        info buf
 * @param       len         input info buf len, return real data len
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_read_intv(uint8_t *info, int *len);

/**
 * Read the name of ble module
 * @param[out]  info        info buf
 * @param       len         input info buf len, return real data len
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_read_name(uint8_t *info, int *len);

/**
 * Read the baudrate of ble module
 * @param[out]  info        info buf
 * @param       len         input info buf len, return real data len
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_read_baudrt(uint8_t *info, int *len);

/**
 * Read the mac address of ble module
 * @param[out]  info        info buf
 * @param       len         input info buf len, return real data len
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_read_btaddr(uint8_t *info, int *len);

/**
 * Read the adv inter value of ble module
 * @param[out]  info        info buf
 * @param       len         input info buf len, return real data len
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_read_advintv(uint8_t *info, int *len);

/**
 * Read the adv data of ble module
 * @param[out]  info        info buf
 * @param       len         input info buf len, return real data len
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_read_advdata(uint8_t *info, int *len);

/**
 * Read the adv status of ble module, working or not
 * @param[out]  info        info buf
 * @param       len         input info buf len, return real data len
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_read_advwk(uint8_t *info, int *len);

/**
 * Init the ops for ble communicate
 * @param[in]   ops         communicate ops from hal
 * @return      0 on success; < 0 on failure
 */
int yunos_bsp_ble_init(ble_ops_t *ops);

#endif /* YUNOS_BSP_BLE_H */


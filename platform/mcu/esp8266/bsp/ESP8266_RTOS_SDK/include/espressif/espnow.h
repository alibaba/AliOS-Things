/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __ESPNOW_H__
#define __ESPNOW_H__

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup ESPNow_APIs ESP-NOW APIs
  * @brief ESP-NOW APIs
  *
  * @attention 1. ESP-NOW do not support broadcast and multicast.
  * @attention 2. ESP-NOW is targeted to Smart-Light project, so it is suggested
  *               that slave role corresponding to soft-AP or soft-AP+station mode,
  *               controller role corresponding to station mode.
  * @attention 3. When ESP8266 is in soft-AP+station mode, it will communicate through
  *               station interface if it is in slave role, and communicate through
  *               soft-AP interface if it is in controller role.
  * @attention 4. ESP-NOW can not wake ESP8266 up from sleep, so if the target ESP8266
  *               station is in sleep, ESP-NOW communication will fail.
  * @attention 5. In station mode, ESP8266 supports 10 encrypt ESP-NOW peers at most,
  *               with the unencrypted peers, it can be 20 peers in total at most.
  * @attention 6. In the soft-AP mode or soft-AP + station mode, the ESP8266 supports
  *               6 encrypt ESP-NOW peers at most, with the unencrypted peers, it can
  *               be 20 peers in total at most.
  *
  */

/** @addtogroup ESPNow_APIs
  * @{
  */

enum esp_now_role {
    ESP_NOW_ROLE_IDLE = 0,
    ESP_NOW_ROLE_CONTROLLER,
    ESP_NOW_ROLE_SLAVE,
    ESP_NOW_ROLE_MAX,
};

/**
  * @brief     ESP-NOW send callback.
  *
  * @attention The status will be OK, if ESP-NOW send packet successfully. But users
  *            need to make sure by themselves that key of communication is correct.
  *
  * @param     uint8 *mac_addr : MAC address of target device
  * @param     uint8 *data     : data received
  * @param     uint8 len       : data length
  *
  * @return null
  */
typedef void (*esp_now_recv_cb_t)(uint8 *mac_addr, uint8 *data, uint8 len);

/**
  * @brief     ESP-NOW send callback.
  *
  * @attention The status will be OK, if ESP-NOW send packet successfully. But users
  *            need to make sure by themselves that key of communication is correct.
  *
  * @param     uint8 *mac_addr : MAC address of target device
  * @param     uint8 status    : status of ESP-NOW sending packet, 0, OK; 1, fail.
  *
  * @return    null
  */
typedef void (*esp_now_send_cb_t)(uint8 *mac_addr, uint8 status);

/**
  * @brief  ESP-NOW initialization.
  *
  * @param  null
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_init(void);

/**
  * @brief  Deinitialize ESP-NOW.
  *
  * @param  null
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_deinit(void);

/**
  * @brief  Register ESP-NOW send callback.
  *
  * @param  esp_now_send_cb_t cb : send callback
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_register_send_cb(esp_now_send_cb_t cb);

/**
  * @brief  Unregister ESP-NOW send callback.
  *
  * @param  null
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_unregister_send_cb(void);

/**
  * @brief  Register ESP-NOW receive callback.
  *
  * @param  esp_now_recv_cb_t cb : receive callback
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_register_recv_cb(esp_now_recv_cb_t cb);

/**
  * @brief  Unregister ESP-NOW receive callback.
  *
  * @param  null
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_unregister_recv_cb(void);

/**
  * @brief  Send ESP-NOW packet.
  *
  * @param  uint8 *da   : destination MAC address.
  *                       If it's NULL, send packet to all MAC addresses recorded
  *                       by ESP-NOW; otherwise, send packet to target MAC address.
  * @param  uint8 *data : data need to send
  * @param  uint8 len   : data length
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_send(uint8 *da, uint8 *data, uint8 len);

/**
  * @brief  Add an ESP-NOW peer, store MAC address of target device into ESP-NOW MAC list.
  *
  * @param  uint8 *mac_addr : MAC address of device
  * @param  uint8 role      : role type of device, enum esp_now_role
  * @param  uint8 channel   : channel of device
  * @param  uint8 *key      : 16 bytes key which is needed for ESP-NOW communication
  * @param  uint8 key_len   : length of key, has to be 16 bytes now
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_add_peer(uint8 *mac_addr, uint8 role, uint8 channel, uint8 *key, uint8 key_len);

/**
  * @brief  Delete an ESP-NOW peer, delete MAC address of the device from ESP-NOW MAC list.
  *
  * @param  u8 *mac_addr : MAC address of device
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_del_peer(uint8 *mac_addr);

/**
  * @brief  Set ESP-NOW role of device itself.
  *
  * @param  uint8 role : role type of device, enum esp_now_role.
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_set_self_role(uint8 role);

/**
  * @brief  Get ESP-NOW role of device itself.
  *
  * @param  uint8 role : role type of device, enum esp_now_role.
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_get_self_role(void);

/**
  * @brief  Set ESP-NOW role for a target device. If it is set multiple times,
  *         new role will cover the old one.
  *
  * @param  uint8 *mac_addr : MAC address of device.
  * @param  uint8 role      : role type, enum esp_now_role.
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_set_peer_role(uint8 *mac_addr, uint8 role);

/**
  * @brief  Get ESP-NOW role of a target device.
  *
  * @param  uint8 *mac_addr : MAC address of device.
  *
  * @return  ESP_NOW_ROLE_CONTROLLER, role type : controller
  * @return  ESP_NOW_ROLE_SLAVE, role type : slave
  * @return  otherwise : fail
  */
sint32 esp_now_get_peer_role(uint8 *mac_addr);

/**
  * @brief  Record channel information of a ESP-NOW device.
  *
  * When communicate with this device,
  *    -  call esp_now_get_peer_channel to get its channel first,
  *    -  then call wifi_set_channel to be in the same channel and do communication.
  *
  * @param  uint8 *mac_addr : MAC address of target device.
  * @param  uint8 channel   : channel, usually to be 1 ~ 13, some area may use channel 14.
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_set_peer_channel(uint8 *mac_addr, uint8 channel);

/**
  * @brief  Get channel information of a ESP-NOW device.
  *
  * @attention ESP-NOW communication needs to be at the same channel.
  *
  * @param  uint8 *mac_addr : MAC address of target device.
  *
  * @return 1 ~ 13 (some area may get 14) : channel number
  * @return Non-0 : fail
  */
sint32 esp_now_get_peer_channel(uint8 *mac_addr);

/**
  * @brief  Set ESP-NOW key for a target device.
  *
  * If it is set multiple times, new key will cover the old one.
  *
  * @param  uint8 *mac_addr : MAC address of target device.
  * @param  uint8 *key      : 16 bytes key which is needed for ESP-NOW communication,
  *                           if it is NULL, current key will be reset to be none.
  * @param  uint8 key_len   : key length, has to be 16 bytes now
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_set_peer_key(uint8 *mac_addr, uint8 *key, uint8 key_len);

/**
  * @brief  Get ESP-NOW key of a target device.
  *
  * If it is set multiple times, new key will cover the old one.
  *
  * @param  uint8 *mac_addr : MAC address of target device.
  * @param  uint8 *key      : pointer of key, buffer size has to be 16 bytes at least
  * @param  uint8 key_len   : key length
  *
  * @return 0   : succeed
  * @return > 0 : find target device but can't get key
  * @return < 0 : fail
  */
sint32 esp_now_get_peer_key(uint8 *mac_addr, uint8 *key, uint8 *key_len);

/**
  * @brief      Get MAC address of ESP-NOW device.
  *
  *            Get MAC address of ESP-NOW device which is pointed now, and move
  *            the pointer to next one in ESP-NOW MAC list or move the pointer to
  *            the first one in ESP-NOW MAC list.
  *
  * @attention 1. This API can not re-entry
  * @attention 2. Parameter has to be true when you call it the first time.
  *
  * @param  bool restart : true, move pointer to the first one in ESP-NOW MAC list;
  *                        false, move pointer to the next one in ESP-NOW MAC list
  *
  * @return NULL, no ESP-NOW devices exist
  * @return Otherwise, MAC address of ESP-NOW device which is pointed now
  */
uint8 *esp_now_fetch_peer(bool restart);

/**
  * @brief  Check if target device exists or not.
  *
  * @param  uint8 *mac_addr : MAC address of target device.
  *
  * @return 0   : device does not exist
  * @return < 0 : error occur, check fail
  * @return > 0 : device exists
  */
sint32 esp_now_is_peer_exist(uint8 *mac_addr);

/**
  * @brief  Get the total number of ESP-NOW devices which are associated, and the
  *         number count of encrypted devices.
  *
  * @param  uint8 *all_cnt    : total number of ESP-NOW devices which are associated.
  * @param  uint8 *encryp_cnt : number count of encrypted devices
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_get_cnt_info(uint8 *all_cnt, uint8 *encrypt_cnt);

/**
  * @brief  Set the encrypt key of communication key.
  *
  *         All ESP-NOW devices share the same encrypt key. If users do not set
  *         the encrypt key, ESP-NOW communication key will be encrypted by a default key.
  *
  * @param  uint8 *key : pointer of encrypt key.
  * @param  uint8 len  : key length, has to be 16 bytes now.
  *
  * @return 0     : succeed
  * @return Non-0 : fail
  */
sint32 esp_now_set_kok(uint8 *key, uint8 len);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

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

#ifndef __SMARTCONFIG_H__
#define __SMARTCONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup Smartconfig_APIs Smartconfig APIs
  * @brief SmartConfig APIs
  *
  * SmartConfig can only be enabled in station only mode.
  * Please make sure the target AP is enabled before enable SmartConfig.
  *
  */

/** @addtogroup Smartconfig_APIs
  * @{
  */

typedef enum {
    SC_STATUS_WAIT = 0,             /**< waiting, do not start connection in this phase */
    SC_STATUS_FIND_CHANNEL,         /**< find target channel, start connection by APP in this phase */
    SC_STATUS_GETTING_SSID_PSWD,    /**< getting SSID and password of target AP */
    SC_STATUS_LINK,                 /**< connecting to target AP */
    SC_STATUS_LINK_OVER,            /**< got IP, connect to AP successfully */
} sc_status;

typedef enum {
    SC_TYPE_ESPTOUCH = 0,       /**< protocol: ESPTouch */
    SC_TYPE_AIRKISS,            /**< protocol: AirKiss */
    SC_TYPE_ESPTOUCH_AIRKISS,   /**< protocol: ESPTouch and AirKiss */
} sc_type;

/**
  * @brief  The callback of SmartConfig, executed when smart-config status changed.
  *
  * @param  sc_status status : status of SmartConfig:
  *    -  if status == SC_STATUS_GETTING_SSID_PSWD, parameter void *pdata is a pointer
                       of sc_type, means SmartConfig type: AirKiss or ESP-TOUCH.
  *    -  if status == SC_STATUS_LINK, parameter void *pdata is a pointer of struct station_config;
  *    -  if status == SC_STATUS_LINK_OVER, parameter void *pdata is a pointer of mobile
  *                    phone's IP address, 4 bytes. This is only available in ESPTOUCH, otherwise,
  *                    it is NULL.
  *    -  otherwise, parameter void *pdata is NULL.
  * @param  void *pdata : data of SmartConfig
  *
  * @return null
  */
typedef void (*sc_callback_t)(sc_status status, void *pdata);

/**
  * @brief  Get the version of SmartConfig.
  *
  * @param  null
  *
  * @return SmartConfig version
  */
const char *smartconfig_get_version(void);

/**
  * @brief     Start SmartConfig mode.
  *
  *            Start SmartConfig mode, to connect ESP8266 station to AP, by sniffing
  *            for special packets from the air, containing SSID and password of desired AP.
  *            You need to broadcast the SSID and password (e.g. from mobile device or computer)
  *            with the SSID and password encoded.
  *
  * @attention 1. This api can only be called in station mode.
  * @attention 2. During SmartConfig, ESP8266 station and soft-AP are disabled.
  * @attention 3. Can not call smartconfig_start twice before it finish, please call
  *               smartconfig_stop first.
  * @attention 4. Don't call any other APIs during SmartConfig, please call smartconfig_stop first.
  *
  * @param     sc_callback_t cb : SmartConfig callback; executed when SmartConfig status changed;
  * @param     uint8 log : 1, UART output logs; otherwise, UART only outputs the result.
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool smartconfig_start(sc_callback_t cb, ...);

/**
  * @brief     Stop SmartConfig, free the buffer taken by smartconfig_start.
  *
  * @attention Whether connect to AP succeed or not, this API should be called to free
  *            memory taken by smartconfig_start.
  *
  * @param     null
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool smartconfig_stop(void);

/**
  * @brief     Set timeout of SmartConfig.
  *
  * @attention SmartConfig timeout start at SC_STATUS_FIND_CHANNEL, SmartConfig will
  *            restart if timeout.
  *
  * @param     uint8 time_s : range 15s~255s, offset:45s.
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool esptouch_set_timeout(uint8 time_s);

/**
  * @brief     Set protocol type of SmartConfig.
  *
  * @attention If users need to set the SmartConfig type, please set it before calling
  *            smartconfig_start.
  *
  * @param     sc_type type : AirKiss, ESP-TOUCH or both.
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool smartconfig_set_type(sc_type type);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

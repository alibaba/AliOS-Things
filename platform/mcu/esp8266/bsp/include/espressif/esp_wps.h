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

#ifndef __ESPWPS_H__
#define __ESPWPS_H__

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup WPS_APIs  WPS APIs
  * @brief ESP8266 WPS APIs
  *
  * WPS can only be used when ESP8266 station is enabled.
  *
  */

/** @addtogroup WPS_APIs
  * @{
  */

typedef enum wps_type {
    WPS_TYPE_DISABLE = 0,
    WPS_TYPE_PBC,
    WPS_TYPE_PIN,
    WPS_TYPE_DISPLAY,
    WPS_TYPE_MAX,
} WPS_TYPE_t;

enum wps_cb_status {
    WPS_CB_ST_SUCCESS = 0,     /**< WPS succeed */
    WPS_CB_ST_FAILED,          /**< WPS fail */
    WPS_CB_ST_TIMEOUT,         /**< WPS timeout, fail */
    WPS_CB_ST_WEP,             /**< WPS failed because that WEP is not supported */
    WPS_CB_ST_SCAN_ERR,        /**< can not find the target WPS AP */
};

/**
  * @brief     Enable Wi-Fi WPS function.
  *
  * @attention WPS can only be used when ESP8266 station is enabled.
  *
  * @param     WPS_TYPE_t wps_type : WPS type, so far only WPS_TYPE_PBC is supported
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool wifi_wps_enable(WPS_TYPE_t wps_type);

/**
  * @brief  Disable Wi-Fi WPS function and release resource it taken.
  *
  * @param  null
  *
  * @return true  : succeed
  * @return false : fail
  */
bool wifi_wps_disable(void);

/**
  * @brief     WPS starts to work.
  *
  * @attention WPS can only be used when ESP8266 station is enabled.
  *
  * @param     null
  *
  * @return    true  : WPS starts to work successfully, but does not mean WPS succeed.
  * @return    false : fail
  */
bool wifi_wps_start(void);

/**
  * @brief  WPS callback.
  *
  * @param  int status : status of WPS, enum wps_cb_status.
  *    -  If parameter status == WPS_CB_ST_SUCCESS in WPS callback, it means WPS got AP's
  *       information, user can call wifi_wps_disable to disable WPS and release resource,
  *       then call wifi_station_connect to connect to target AP.
  *    -  Otherwise, it means that WPS fail, user can create a timer to retry WPS by
  *       wifi_wps_start after a while, or call wifi_wps_disable to disable WPS and release resource.
  *
  * @return null
  */
typedef void (*wps_st_cb_t)(int status);

/**
  * @brief     Set WPS callback.
  *
  * @attention WPS can only be used when ESP8266 station is enabled.
  *
  * @param     wps_st_cb_t cb : callback.
  *
  * @return    true  : WPS starts to work successfully, but does not mean WPS succeed.
  * @return    false : fail
  */
bool wifi_set_wps_cb(wps_st_cb_t cb);

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

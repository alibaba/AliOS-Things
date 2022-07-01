/**
 * \file            lwesp.h
 * \brief           LwESP
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#ifndef LWESP_HDR_H
#define LWESP_HDR_H

#include "lwesp/lwesp_includes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup        LWESP Lightweight ESP-AT parser library
 * \brief           Lightweight ESP-AT parser
 * \{
 */

lwespr_t    lwesp_init(lwesp_evt_fn cb_func, const uint32_t blocking);
lwespr_t    lwesp_reset(const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_reset_with_delay(uint32_t delay, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);

lwespr_t    lwesp_at_test(const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_restore(const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_set_at_baudrate(uint32_t baud, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_set_wifi_mode(lwesp_mode_t mode, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_get_wifi_mode(lwesp_mode_t* mode, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);

lwespr_t    lwesp_sysmsg_def(const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_update_sw(const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);

lwespr_t    lwesp_core_lock(void);
lwespr_t    lwesp_core_unlock(void);

lwespr_t    lwesp_device_set_present(uint8_t present, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
uint8_t     lwesp_device_is_present(void);

uint8_t     lwesp_delay(const uint32_t ms);

lwespr_t    lwesp_get_current_at_fw_version(lwesp_sw_version_t* const version);
lwespr_t    lwesp_get_min_at_fw_version(lwesp_sw_version_t* const version);

/**
 * \brief           Set and format major, minor and patch values to firmware version
 * \param[in]       v: Version output, pointer to \ref lwesp_sw_version_t structure
 * \param[in]       major_: Major version
 * \param[in]       minor_: Minor version
 * \param[in]       patch_: Patch version
 * \hideinitializer
 */
#define     lwesp_set_fw_version(v, major_, minor_, patch_)          do { (v)->major = (major_); (v)->minor = (minor_); (v)->patch = (patch_); } while (0)

#if LWESP_CFG_ESP8266 || __DOXYGEN__
uint8_t     lwesp_device_is_esp8266(void);
#else
#define      lwesp_device_is_esp8266()                       ((uint8_t)0)
#endif /* LWESP_CFG_ESP8266 || __DOXYGEN__ */
#if LWESP_CFG_ESP32 || __DOXYGEN__
uint8_t     lwesp_device_is_esp32(void);
#else
#define      lwesp_device_is_esp32()                         ((uint8_t)0)
#endif /* LWESP_CFG_ESP32 || __DOXYGEN__ */
#if LWESP_CFG_ESP32_C3 || __DOXYGEN__
uint8_t     lwesp_device_is_esp32_c3(void);
#else
#define      lwesp_device_is_esp32_c3()                      ((uint8_t)0)
#endif /* LWESP_CFG_ESP32_C3 || __DOXYGEN__ */
lwesp_device_t lwesp_device_get_device(void);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_H */

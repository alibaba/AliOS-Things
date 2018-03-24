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

#ifndef __UPGRADE_H__
#define __UPGRADE_H__

#include "lwip/sockets.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup System_APIs System APIs
  * @brief System APIs
  */

/** @addtogroup System_APIs
  * @{
  */

/** \defgroup Upgrade_APIs Upgrade APIs
  * @brief Firmware upgrade (FOTA) APIs
  */

/** @addtogroup Upgrade_APIs
  * @{
  */

#define SPI_FLASH_SEC_SIZE  4096    /**< SPI Flash sector size */

#define USER_BIN1           0x00    /**< firmware, user1.bin */
#define USER_BIN2           0x01    /**< firmware, user2.bin */

#define UPGRADE_FLAG_IDLE   0x00    /**< flag of upgrading firmware, idle */
#define UPGRADE_FLAG_START  0x01    /**< flag of upgrading firmware, start upgrade */
#define UPGRADE_FLAG_FINISH 0x02    /**< flag of upgrading firmware, finish upgrading */

#define UPGRADE_FW_BIN1     0x00    /**< firmware, user1.bin */
#define UPGRADE_FW_BIN2     0x01    /**< firmware, user2.bin */

/**
  * @brief  Callback of upgrading firmware through WiFi.
  *
  * @param  void * arg : information about upgrading server
  *
  * @return null
  */
typedef void (*upgrade_states_check_callback)(void *arg);

//#define UPGRADE_SSL_ENABLE

struct upgrade_server_info {
    struct sockaddr_in sockaddrin;          /**< socket of upgrading */
    upgrade_states_check_callback check_cb; /**< callback of upgrading */
    uint32 check_times;                     /**< time out of upgrading, unit : ms */
    uint8 pre_version[16];                  /**< previous version of firmware */
    uint8 upgrade_version[16];              /**< the new version of firmware */
    uint8 *url;                             /**< the url of upgrading server */
    void *pclient_param;
    uint8 upgrade_flag;                     /**< true, upgrade succeed; false, upgrade fail */
};

/**
  * @brief  Upgrade function initialization.
  *
  * @param  null
  *
  * @return null
  */
void system_upgrade_init();

/**
  * @brief  Upgrade function de-initialization.
  *
  * @param  null
  *
  * @return null
  */
void system_upgrade_deinit();

/**
 * @brief  Upgrade function de-initialization.
 *
 * @param  uint8 *data : segment of the firmware bin data
 * @param  uint32 len  : length of the segment bin data
 *
 * @return null
 */
bool system_upgrade(uint8 *data, uint32 len);

#ifdef UPGRADE_SSL_ENABLE

/**
 * @brief  Start upgrade firmware through WiFi with SSL connection.
 *
 * @param  struct upgrade_server_info *server : the firmware upgrade server info
 *
 * @return true  : succeed
 * @return false : fail
 */
bool system_upgrade_start_ssl(struct upgrade_server_info *server);
#else

/**
 * @brief  Start upgrade firmware through WiFi with normal connection.
 *
 * @param  struct upgrade_server_info *server : the firmware upgrade server info
 *
 * @return true  : succeed
 * @return false : fail
 */

bool system_upgrade_start(struct upgrade_server_info *server);
#endif

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

/*
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Use in source and binary forms, redistribution in binary form only, with
 * or without modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 2. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 3. This software, with or without modification, must only be used with a Nordic
 *    Semiconductor ASA integrated circuit.
 * 
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**@file
 *
 * @defgroup ali_cmd Alibaba IOT commands.
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    Alibaba IOT commands.
 */

#ifndef ALI_CMD_H__
#define ALI_CMD_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


#define ALI_CMD_TYPE_MASK           0xF0                            /**< Mask used to extract the command type. */

#define ALI_CMD_TYPE_GENERIC        0x00                            /**< Command type: generic */
#define ALI_CMD_CTRL                (ALI_CMD_TYPE_GENERIC | 0x0)    /**< Control command from the central. */
#define ALI_CMD_STATUS              (ALI_CMD_TYPE_GENERIC | 0x1)    /**< Status notification to the central. */
#define ALI_CMD_QUERY               (ALI_CMD_TYPE_GENERIC | 0x2)    /**< Query request, to be handled by application. */
#define ALI_CMD_REPLY               (ALI_CMD_TYPE_GENERIC | 0x3)    /**< Response to @ref ALI_CMD_QUERY, from the application. */
#define ALI_CMD_ERROR               (ALI_CMD_TYPE_GENERIC | 0xF)    /**< Error notification from peripheral to central. */

#define ALI_CMD_TYPE_AUTH           0x10                            /**< Command type: authentication */
#define ALI_CMD_AUTH_RAND           (ALI_CMD_TYPE_AUTH | 0x1)       /**< 16-byte random number. */
#define ALI_CMD_AUTH_REQ            (ALI_CMD_TYPE_AUTH | 0x2)       /**< Authentication request from central, with payload "Hi,Server". */
#define ALI_CMD_AUTH_RSP            (ALI_CMD_TYPE_AUTH | 0x3)       /**< Authentication response from peripheral, with payload "Hi,Client". */
#define ALI_CMD_AUTH_CFM            (ALI_CMD_TYPE_AUTH | 0x4)       /**< Authentication confirmation from central, with payload "OK". */

#define ALI_CMD_TYPE_FW_UPGRADE     0x20                            /**< Command type: firmware upgrade */
#define ALI_CMD_FW_VERSION_REQ      (ALI_CMD_TYPE_FW_UPGRADE | 0x0) /**< Query on firmware version, sent from central. */
#define ALI_CMD_FW_VERSION_RSP      (ALI_CMD_TYPE_FW_UPGRADE | 0x1) /**< Response to @ref ALI_CMD_FW_VERSION_REQ. */
#define ALI_CMD_FW_UPGRADE_REQ      (ALI_CMD_TYPE_FW_UPGRADE | 0x2) /**< Request to start firmware upgrade, sent from central. */
#define ALI_CMD_FW_UPGRADE_RSP      (ALI_CMD_TYPE_FW_UPGRADE | 0x3) /**< Response to @ref ALI_CMD_FW_UPGRADE_REQ. */
#define ALI_CMD_FW_BYTES_RECEIVED   (ALI_CMD_TYPE_FW_UPGRADE | 0x4) /**< Response to @ref ALI_CMD_FW_GET_INITIAL_SIZE or @ref ALI_CMD_FW_DATA on the progress of firmware download, conveying the last known valid frame sequence and data offset. */
#define ALI_CMD_FW_CHECK_RESULT     (ALI_CMD_TYPE_FW_UPGRADE | 0x5) /**< Result of firmware check after the whole firmware image has been received. */
#define ALI_CMD_FW_UPDATE_PROCESS   (ALI_CMD_TYPE_FW_UPGRADE | 0x6) /**< Notification from peripheral on the progress of firmware upgrade. */
#define ALI_CMD_FW_GET_INIT_FW_SIZE (ALI_CMD_TYPE_FW_UPGRADE | 0x7) /**< Request to get the desired offset of firmware image. */
#define ALI_CMD_FW_XFER_FINISH      (ALI_CMD_TYPE_FW_UPGRADE | 0x8) /**< Message from the central to indicate the end of firmware transfer. */
#define ALI_CMD_FW_DATA             (ALI_CMD_TYPE_FW_UPGRADE | 0xF) /**< Speciial packet for firmware data. */


#ifdef __cplusplus
}
#endif

#endif // ALI_CMD_H__

/** @} */

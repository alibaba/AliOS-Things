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
 * @defgroup ali_error_src Alibaba IOT error source.
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    Alibaba IOT error source.
 */

#ifndef ALI_ERROR_SRC_H__
#define ALI_ERROR_SRC_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ALI_ERROR_SRC_TYPE_CORE                 0x00010000                          /**< Error source group: Core */
#define ALI_ERROR_SRC_CORE_CANNOT_SET_KEY       (ALI_ERROR_SRC_TYPE_CORE | 0)       /**< Core: New key has been generated but refused by transport layer. */
#define ALI_ERROR_SRC_CORE_SEND_ERR             (ALI_ERROR_SRC_TYPE_CORE | 1)       /**< Core: when sending error (cmd=0x0F) to peer. */

#define ALI_ERROR_SRC_TYPE_TRANSPORT            0x00020000                          /**< Error source group: Transport */
#define ALI_ERROR_SRC_TRANSPORT_TX_TIMER        (ALI_ERROR_SRC_TYPE_TRANSPORT | 0)  /**< Transport layer: Tx timer. */
#define ALI_ERROR_SRC_TRANSPORT_RX_TIMER        (ALI_ERROR_SRC_TYPE_TRANSPORT | 1)  /**< Transport layer: Rx timer. */
#define ALI_ERROR_SRC_TRANSPORT_1ST_FRAME       (ALI_ERROR_SRC_TYPE_TRANSPORT | 2)  /**< Transport layer: 1st frame. */
#define ALI_ERROR_SRC_TRANSPORT_OTHER_FRAMES    (ALI_ERROR_SRC_TYPE_TRANSPORT | 3)  /**< Transport layer: 2nd frame, and beyond. */
#define ALI_ERROR_SRC_TRANSPORT_ENCRYPTED       (ALI_ERROR_SRC_TYPE_TRANSPORT | 4)  /**< Transport layer: encrypted packet, but either length is not aligned to 16 (NRF_ERROR_INVALID_DATA) or key is absent (NRF_ERROR_FORBIDDEN). */
#define ALI_ERROR_SRC_TRANSPORT_RX_BUFF_SIZE    (ALI_ERROR_SRC_TYPE_TRANSPORT | 5)  /**< Transport layer: Received packet larger than buffer size. */
#define ALI_ERROR_SRC_TRANSPORT_PKT_CFM_SENT    (ALI_ERROR_SRC_TYPE_TRANSPORT | 6)  /**< Transport layer: Number of packets sent more than expected. */
#define ALI_ERROR_SRC_TRANSPORT_FW_DATA_DISC    (ALI_ERROR_SRC_TYPE_TRANSPORT | 7)  /**< Transport layer: 2nd frame, and beyond. Sequence number discontinuous. */

#define ALI_ERROR_SRC_TYPE_AUTH                 0x00030000                          /**< Error source group: Authentication */
#define ALI_ERROR_SRC_AUTH_SEND_RSP             (ALI_ERROR_SRC_TYPE_AUTH | 0)       /**< Authentication: when sending response. */
#define ALI_ERROR_SRC_AUTH_PROC_TIMER_0         (ALI_ERROR_SRC_TYPE_AUTH | 1)       /**< Authentication: Procedure timer after connection. */
#define ALI_ERROR_SRC_AUTH_PROC_TIMER_1         (ALI_ERROR_SRC_TYPE_AUTH | 2)       /**< Authentication: Procedure timer after receiving authetication request. */
#define ALI_ERROR_SRC_AUTH_PROC_TIMER_2         (ALI_ERROR_SRC_TYPE_AUTH | 3)       /**< Authentication: Procedure timer timeout. */
#define ALI_ERROR_SRC_AUTH_SVC_ENABLED          (ALI_ERROR_SRC_TYPE_AUTH | 4)       /**< Authentication: when service has just been enabled. */
#define ALI_ERROR_SRC_AUTH_SEND_ERROR           (ALI_ERROR_SRC_TYPE_AUTH | 5)       /**< Authentication: when sending error (cmd=0x0F). */

#define ALI_ERROR_SRC_TYPE_GAP                  0x00040000                          /**< Error source group: Generic Access */
#define ALI_ERROR_SRC_GAP_CMD_RECEIVED          (ALI_ERROR_SRC_TYPE_GAP | 0)        /**< GAP: Message has arrived. */
#define ALI_ERROR_SRC_GAP_TX_DONE               (ALI_ERROR_SRC_TYPE_GAP | 1)        /**< GAP: Message has been sent. */

#define ALI_ERROR_SRC_TYPE_OTA                  0x00050000                          /**< Error source group: OTA firmware upgrade */
#define ALI_ERROR_SRC_OTA_SEND_VER_RSP          (ALI_ERROR_SRC_TYPE_OTA | 0)        /**< OTA: when sending version response (cmd=0x21). */
#define ALI_ERROR_SRC_OTA_SEND_UPGRADE_RSP      (ALI_ERROR_SRC_TYPE_OTA | 1)        /**< OTA: when sending upgrade response (cmd=0x23). */
#define ALI_ERROR_SRC_OTA_SEND_BYTES_RECVD      (ALI_ERROR_SRC_TYPE_OTA | 2)        /**< OTA: when sending bytes-received message (cmd=0x24). */
#define ALI_ERROR_SRC_OTA_SEND_CRC_RESULT       (ALI_ERROR_SRC_TYPE_OTA | 3)        /**< OTA: when sending CRC result (cmd=0x25). */
#define ALI_ERROR_SRC_OTA_SEND_ERROR            (ALI_ERROR_SRC_TYPE_OTA | 4)        /**< OTA: when sending error (cmd=0x0F). */
#define ALI_ERROR_SRC_OTA_SETTINGS_WRITE        (ALI_ERROR_SRC_TYPE_OTA | 5)        /**< OTA: when writing bootloader settings on boot. */
#define ALI_ERROR_SRC_OTA_FLASH_ERASE           (ALI_ERROR_SRC_TYPE_OTA | 6)        /**< OTA: when erasing flash on receive of firmware upgrade request (cmd=0x22). */
#define ALI_ERROR_SRC_OTA_FLASH_WRITE           (ALI_ERROR_SRC_TYPE_OTA | 7)        /**< OTA: when writing flash on receive of firmware data (cmd=0x2F). */
#define ALI_ERROR_SRC_OTA_FWUP_REQ              (ALI_ERROR_SRC_TYPE_OTA | 8)        /**< OTA: when erasing flash on receive of firmware upgrade request (cmd=0x22). */
#define ALI_ERROR_SRC_OTA_FW_DATA               (ALI_ERROR_SRC_TYPE_OTA | 9)        /**< OTA: when erasing flash on receive of firmware data (cmd=0x2F). */
#define ALI_ERROR_SRC_OTA_XFER_FINISHED         (ALI_ERROR_SRC_TYPE_OTA | 10)       /**< OTA: when writing bootloader settings on receive of firmware check (cmd=0x28). */
#define ALI_ERROR_SRC_OTA_NOTIFY_NEW_FW         (ALI_ERROR_SRC_TYPE_OTA | 11)       /**< OTA: when writing bootloader settings on new firmware is swapped. */
#define ALI_ERROR_SRC_OTA_SEND_FWUP_SUCCESS     (ALI_ERROR_SRC_TYPE_OTA | 12)       /**< OTA: when sending firmware upgrade process after firmware update succeed (cmd=0x26). */
#define ALI_ERROR_SRC_OTA_AUTH_TIMING           (ALI_ERROR_SRC_TYPE_OTA | 13)       /**< OTA: something has happened before authentication. */
#define ALI_ERROR_SRC_OTA_FW_RX_WRITE_SETTINGS  (ALI_ERROR_SRC_TYPE_OTA | 14)       /**< OTA: when writing bootloader settings after writing firmware data. */
#define ALI_ERROR_SRC_OTA_FW_RX_WR_SETTINGS_CB  (ALI_ERROR_SRC_TYPE_OTA | 15)       /**< OTA: in callback when writing bootloader settings after writing firmware data. */

#ifdef __cplusplus
}
#endif

#endif // ALI_ERROR_SRC_H__

/** @} */

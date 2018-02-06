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
 * @defgroup ali_ota Alibaba IOT Service
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    Alibaba IOT generic access profile (OTA) module implementation.
 *
 * @details The Alibaba IOT OTA module hands Tx and Rx messages for command
 *          type @ref ALI_CMD_TYPE_GENERIC.
 */

#ifndef ALI_OTA_H__
#define ALI_OTA_H__

#include <stdint.h>
//#include "sdk_errors.h"
#include "ali_cmd.h"
#include "ali_common.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ALI_OTA_MAX_FW_VERSION_LEN  8       /**< Maximum 32 bytes of firmware version. */


/**
 * @brief State machine.
 */
typedef enum
{
    ALI_OTA_STATE_OFF,              /**< Off, expects authentication results. */
    ALI_OTA_STATE_IDLE,             /**< Idle, expects commands 0x20 (version query) or 0x22 (update request) */
    ALI_OTA_STATE_RECEIVE,          /**< Receive firmware, expects commands 0x27 (progress query) or 0x2F (Firmware data). */
    ALI_OTA_STATE_RECEIVE_ERR,      /**< Receive firmware, but frame discontinuity happened, waiting for error (cmd=0x0F) sent. */
    ALI_OTA_STATE_WRITE,            /**< Flash write, expects flash completion event. */
    ALI_OTA_STATE_WRITE_SETTINGS,   /**< Settings write after flash write, expects bootloader_settings completion event. */
    ALI_OTA_STATE_FW_CHECK,         /**< Flash check, expects command 0x28 (Firmware check) to make sure the peer will not send anything in between. */
    ALI_OTA_STATE_RESET_PREPARE,    /**< Reset prepare, expects Tx-done event to make sure commands 0x25 (firmware check result) is sent. */
    ALI_OTA_STATE_UPGRADE_REPORT,   /**< Upgrade report, expects auth-done event to trigger the sending of command 0x26 (update result). */
} ali_ota_state_t;


/**
 * @brief Types of events.
 */
typedef enum
{
    ALI_OTA_EVT_NEW_FW,     /**< New firmware has been received, so notify application to prepare for system reset. */
    ALI_OTA_EVT_ERROR,      /**< Error reported by referred modules. */
} ali_ota_evt_type_t;


/**@brief Structure for error event. */
typedef struct
{
    uint32_t source;        /**< The location which caused the error. */
    uint32_t err_code;      /**< Error code which has been raised. */
} ali_ota_error_evt_t;


/**@brief Structure for otaentication module event. */
typedef struct
{
    ali_ota_evt_type_t type;            /**< Event type. */
    union
    {
        ali_ota_error_evt_t   error;    /**< Data provided for error event. */
    } data;
} ali_ota_event_t;


// Forward declaration of the ali_ota_t type.
typedef struct ali_ota_s ali_ota_t;


/**
 * @brief Transport layer Tx function.
 *
 * @param[in] p_context   Context passed to interrupt handler, set on initialization.
 * @param[in] cmd         Commands.
 * @param[in] p_data      Pointer to Tx data.
 * @param[in] length      Length of Tx data.
 */
typedef uint32_t (*ali_ota_tx_func_t)(void * p_context, uint8_t cmd, uint8_t * p_data, uint16_t length);


/**
 * @brief Event handler.
 *
 * @param[in] p_context   Context passed to interrupt handler, set on initialization.
 * @param[in] p_event     Pointer to event structure. Event is allocated on the stack so it is available
 *                        only within the context of the event handler.
 */
typedef void (*ali_ota_event_handler_t)(void * p_context, ali_ota_event_t * p_event);


/**@brief Structure for OTA module configuration. */
typedef struct
{
    bool                    feature_enable;     /**< Whether feature is enabled or not. */
    ali_ota_event_handler_t event_handler;      /**< Pointer to event handler. */
    void                  * p_evt_context;      /**< Pointer to context which will be passed as a parameter of event_handler. */
    ali_ota_tx_func_t       tx_func;            /**< Pointer to Tx function (notification). */
    void                  * p_tx_func_context;  /**< Pointer to context which will be passed as a parameter of tx_func. */
    uint8_t               * p_fw_version;       /**< Pointer to firmware version. */
    uint8_t                 fw_version_len;     /**< Length of firmware version, excluding the terminating '\0'. */
} ali_ota_init_t;


/**@brief OTA module structure. This contains various status information for the module. */
struct ali_ota_s
{
    bool                    feature_enable;                         /**< Whether feature is enabled or not. */
    ali_ota_state_t         state;                                  /**< State machine. */
    ali_ota_event_handler_t event_handler;                          /**< Pointer to event handler. */
    void                  * p_evt_context;                          /**< Pointer to context which will be passed as a parameter of event_handler. */
    ali_ota_tx_func_t       tx_func;                                /**< Pointer to Tx function (notification). */
    void                  * p_tx_func_context;                      /**< Pointer to context which will be passed as a parameter of tx_func. */
    uint8_t                 fw_ver[ALI_OTA_MAX_FW_VERSION_LEN];     /**< Firmware version string. */
    uint8_t                 fw_ver_len;                             /**< Length of firmware version. */
    uint32_t                bank_1_addr;                            /**< Address of bank_1. */
    uint32_t                rx_fw_size;                             /**< Size of firmware to be received. */
    uint32_t                bytes_recvd;                            /**< Size of firmware received. */
    uint16_t                frames_recvd;                           /**< Number of frames of firmware received. */
    uint16_t                crc;                                    /**< CRC of firmware received. */
    uint8_t                 tx_buff[ALI_OTA_MAX_FW_VERSION_LEN];    /**< Buffer for compiling message to be sent. */
};


/**
 * @brief Function for initializing the OTA module.
 *
 * This function configures and enables the OTA module.
 *
 * @param[in] p_ota     OTA module structure.
 * @param[in] p_init    Initial configuration.
 *
 * @retval    NRF_SUCCESS            If initialization was successful.
 * @retval    NRF_ERROR_NULL         If NULL pointers are provided.
 */
ret_code_t ali_ota_init(ali_ota_t * p_ota, ali_ota_init_t const * p_init);


/**
 * @brief Function for resetting the state machine of OTA module.
 *
 * @param[in] p_ota OTA structure.
 */
void ali_ota_reset(ali_ota_t * p_ota);


/**@brief Function for handling the commands from central.
 *
 * @param[in] p_ota      OTA module structure.
 * @param[in] cmd        Command.
 * @param[in] p_data     Pointer to data.
 * @param[in] length     Length of data.
 * @param[in] num_frames Number of frame received.
 */
void ali_ota_on_command(ali_ota_t * p_ota, uint8_t cmd, uint8_t * p_data, uint16_t length, uint8_t num_frames);


/**@brief Function for handling the event @ref ALI_TRANSPORT_EVT_TX_DONE from Transport layer.
 *
 * @param[in] p_ota     OTA module structure.
 * @param[in] cmd       Command sent.
 */
void ali_ota_on_tx_done(ali_ota_t * p_ota, uint8_t cmd);


/**@brief Function for handling authentication result
 *
 * @param[in] p_ota             OTA module structure.
 * @param[in] is_authenticated  Authentication status.
 */
void ali_ota_on_auth(ali_ota_t * p_ota, bool is_authenticated);


/**@brief Function for handling frame discontinuity
 *
 * @param[in] p_ota OTA  module structure.
 */
void ali_ota_on_discontinuous_frame(ali_ota_t * p_ota);


#ifdef __cplusplus
}
#endif

#endif // ALI_OTA_H__

/** @} */

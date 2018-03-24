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
 * @defgroup ali_core Alibaba IOT Service
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    Alibaba IOT core management module implementation.
 *
 * @details The Alibaba IOT core module is the public interface of the Ali-SDK.
 */

#ifndef ALI_CORE_H__
#define ALI_CORE_H__

#include <stdint.h>
//#include "ble_srv_common.h"
//#include "sdk_errors.h"
#include "ali_common.h"
#include <stdbool.h>
#include "ble_ais.h"
#include "ali_transport.h"
#include "ali_auth.h"
#include "ali_gap.h"
#include "ali_ota.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ALI_COMPANY_ID              0x01A8      /**< Bluetooth company ID of Alibaba (see spec. v1.0.4 ch. 2.2). */
#define ALI_PROTOCOL_ID             0x03        /**< Protocol ID (see spec. v1.0.4 ch. 2.2). */
//#define ALI_PROTOCOL_ID             0x04        /**< Protocol ID (see spec. v1.0.4 ch. 2.2). */

/**
 * @brief Types of commands for control interface.
 */
typedef enum
{
    ALI_CTRL_SET_MTU = 0,   /**< Set MTU, uint16_t * p_data, obtained from @ref nrf_ble_gatt_evt_t. */
} ali_ctrl_t;


/**
 * @brief Types of events.
 */
typedef enum
{
    ALI_EVT_CONNECTED,      /**< Connection established. */
    ALI_EVT_DISCONNECTED,   /**< Disconnected from peer. */
    ALI_EVT_AUTHENTICATED,  /**< Authentication has been successful. */
    ALI_EVT_TX_DONE,        /**< Command has been sent to central successfully. */
    ALI_EVT_CTRL_RECEIVED,  /**< Command ALI_CMD_CTRL has been received from the central. */
    ALI_EVT_QUERY_RECEIVED, /**< Command ALI_CMD_QUERY has been received from the central. */
    ALI_EVT_NEW_FIRMWARE,   /**< New firmware has been received and written to flash, need reboot to update. */
    ALI_EVT_ERROR,          /**< Error reported by lower layers. */
} ali_evt_type_t;



/**@brief Structure for Rx data event @ref ALI_EVT_CTRL_RECEIVED and @ref ALI_EVT_QUERY_RECEIVED.
 */
typedef struct
{
    uint8_t * p_data;       /**< Pointer to data, which be invalidated on function exit. */
    uint16_t  length;       /**< Length of data, which be invalidated on function exit. */
} ali_rx_data_evt_t;


/**@brief Structure for error event. */
typedef struct
{
    uint32_t source;        /**< The location which caused the error. */
    uint32_t err_code;      /**< Error code which has been raised. */
} ali_error_evt_t;


/**@brief Structure for core module event. */
typedef struct
{
    ali_evt_type_t type;            /**< Event type. */
    union
    {
        ali_rx_data_evt_t rx_data;  /**< Data provided for rx-data event. */
        ali_error_evt_t   error;    /**< Data provided for error event. */
    } data;
} ali_event_t;


/**
 * @brief Event handler.
 *
 * @param[in] p_context   Context passed to event handler, set on initialization.
 * @param[in] p_event     Pointer to event structure. Event is allocated on the stack so it is available
 *                        only within the context of the event handler.
 */
typedef void (*ali_event_handler_t)(void * p_context, ali_event_t * p_event);


/**@brief Structure for core module configuration. */
typedef struct
{
    uint16_t            context_size;           /**< Size of context. */
    ali_event_handler_t event_handler;          /**< Pointer to event handler. */
    void              * p_evt_context;          /**< Pointer to context which will be passed as a parameter of event_handler. */
    uint16_t            model_id;               /**< Model ID. */
    uint8_t           * p_mac;                  /**< Pointer to MAC, format: "xx:xx:xx:xx:xx:xx". */
    uint8_t             mac_len;                /**< Length of MAC, excluding '\0' at the end. */
    uint8_t           * p_secret;               /**< Pointer to secret (ASCII, typical 40-byte, max. 64 bytes). */
    uint8_t             secret_len;             /**< Length of secret. */
    uint8_t           * p_sw_ver;               /**< Software version. */
    uint8_t             sw_ver_len;             /**< Length of software version. */
    uint32_t            timer_prescaler;        /**< Prescaler of timers. */
    uint32_t            transport_timeout;      /**< Timeout of Tx/Rx, in number of ms. Fill 0 if not used. */
    bool                enable_auth;            /**< Enable authentication. */
    bool                enable_ota;             /**< Enable OTA firmware upgrade. */
    uint16_t            max_mtu;                /**< Maximum MTU. */
} ali_init_t;


#define MAX_ADV_DATA_LEN        16                                          /**< Maximum length of Manufacturer specific advertising data (12 bytes excluding type and length in specification v1.0.4)  */
#define TX_BUFF_LEN             (ALI_MAX_SUPPORTED_MTU - 3)                 /**< Transport layer: Tx buffer size (see specification v1.0.4, ch.5.9). */
#define RX_BUFF_LEN             ALI_TRANSPORT_MAX_RX_DATA_LEN               /**< Transport layer: Rx buffer size (see specification v1.0.4, ch.5.9). */


/**@brief Core module structure. */
typedef struct
{
    ble_ais_t           ais;                                    /**< ble_ais structure. */
    ali_transport_t     transport;                              /**< Transport layer structure. */
    ali_auth_t          auth;                                   /**< Authentication module structure. */
    ali_gap_t           gap;                                    /**< GAP module structure */
    ali_ota_t           ota;                                    /**< OTA DFU module structure */
    bool                is_initialized;                         /**< Whether module has been initialized successfully. */
    bool                is_auth_enabled;                        /**< Whether ali_auth module has been enabled. */
    ali_event_handler_t event_handler;                          /**< Pointer to event handler. */
    void              * p_evt_context;                          /**< Pointer to context which will be passed as a parameter of event_handler. */
    bool                is_authenticated;                       /**< flag which tells whether authentication has been successful. */
    uint16_t            conn_handle;                            /**< Connection handle. */
    uint8_t             manuf_spec_adv_data[MAX_ADV_DATA_LEN];  /**< Payload of manufacturer specific advertising data. */
    uint16_t            manuf_spec_adv_data_len;                /**< Length of manufacturer specific advertising data. */
    uint8_t             tx_buff[TX_BUFF_LEN];                   /**< Tx buffer for transport layer. */
    uint32_t            rx_buff[RX_BUFF_LEN/sizeof(uint32_t)];  /**< Rx buffer for transport layer. */
} ali_t;

extern ali_t *g_ali;

/**
 * @brief Function for initializing the core module.
 *
 * This function configures and enables the core module.
 *
 * @param[in] p_ali     Pointer to core module context.
 * @param[in] p_init    Initial configuration.
 *
 * @retval    NRF_SUCCESS            If initialization was successful.
 * @retval    NRF_ERROR_NULL         If NULL pointers are provided.
 */
ret_code_t ali_init(void * p_ali, ali_init_t const * p_init);


/**
 * @brief Function for resetting the core module.
 *
 * @param[in] p_ali     Pointer to core module context.
 */
void ali_reset(void * p_ali);


/**@brief Function for sending commands to central through notification.
 *
 * @param[in] p_ali     Core module structure.
 * @param[in] p_data    Pointer to data.
 * @param[in] length    Length of data.
 *
 * @retval    NRF_SUCCESS            If the process was successful.
 * @retval    NRF_ERROR_NULL         If NULL pointers are provided.
 * @retval    NRF_ERROR_BUSY         If Tx process is already on-going.
 * @retval    NRF_ERROR_DATA_SIZE    If data size is zero.
 * @retval    NRF_ERROR_INVALID_DATA If invalid parameters have been provided.
 */
ret_code_t ali_send_notify(void * p_ali, uint8_t * p_data, uint16_t length);


/**@brief Function for sending commands to central through indication.
 *
 * @param[in] p_ali     Core module structure.
 * @param[in] p_data    Pointer to data.
 * @param[in] length    Length of data.
 *
 * @retval    NRF_SUCCESS            If the process was successful.
 * @retval    NRF_ERROR_NULL         If NULL pointers are provided.
 * @retval    NRF_ERROR_BUSY         If Tx process is already on-going.
 * @retval    NRF_ERROR_DATA_SIZE    If data size is zero.
 * @retval    NRF_ERROR_INVALID_DATA If invalid parameters have been provided.
 */
ret_code_t ali_send_indicate(void * p_ali, uint8_t * p_data, uint16_t length);


/**@brief Function for getting manufacturer-specific advertising data.
 *
 * @param[in]    p_ali      Core module structure.
 * @param[in]    p_data     Pointer to data.
 * @param[inout] length     Length of data.
 *
 * @retval    NRF_SUCCESS            If the process was successful.
 * @retval    NRF_ERROR_NULL         If NULL pointers are provided.
 * @retval    NRF_ERROR_BUSY         If Tx process is already on-going.
 * @retval    NRF_ERROR_DATA_SIZE    If data size is zero.
 * @retval    NRF_ERROR_INVALID_DATA If invalid parameters have been provided.
 */
ret_code_t ali_get_manuf_spec_adv_data(void * p_ali, uint8_t * p_data, uint16_t * length);


/**@brief Function for handling the Alibaba IOT Service's BLE events.
 *
 * @details The Alibaba IOT Service expects the application to call this function each time an
 * event is received from the SoftDevice. This function processes the event if it
 * is relevant and calls the Alibaba IOT Service event handler of the
 * application if necessary.
 *
 * @param[in] p_ali       Core module structure.
 * @param[in] p_ble_evt   Event received from the SoftDevice.
 */
//void ali_on_ble_evt(void * p_ali, ble_evt_t * p_ble_evt);


/**@brief Control interface.
 *
 * @param[in] p_ali       Core module structure.
 * @param[in] ctrl_word   Control word.
 * @param[in] p_data      Pointer to data provided, dependent on ctrl_word.
 *
 * @retval    NRF_SUCCESS If the process was successful.
 */
ret_code_t ali_ctrl(void * p_ali, ali_ctrl_t ctrl_word, void * p_data);

void notify_evt_no_data (ali_t * p_ali, ali_evt_type_t evt_type);

#ifdef __cplusplus
}
#endif

#endif // ALI_CORE_H__

/** @} */

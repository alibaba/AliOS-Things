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
 * @defgroup ali_ext Alibaba IOT Service
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    Alibaba IOT extended command module implementation.
 *
 * @details The Alibaba IOT extend module handles Tx and Rx messages for extended command.
 */

#ifndef ALI_EXT_H__
#define ALI_EXT_H__

#include <stdint.h>
#include <stdbool.h>
#include "ali_cmd.h"
#include "ali_common.h"
#include "ali_ext.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ALI_EXT_MAX_TLV_01_RSP_LEN  26      /**< Maximum size of TLV type 01 response data. */

#define ALI_EXT_TX_BUFF_LEN         255     /**< Tx buffer size. */


/**
 * @brief Types of events.
 */
typedef enum
{
    ALI_EXT_EVT_ERROR,      /**< Error reported by referred modules. */
} ali_ext_evt_type_t;


/**@brief Structure for error event. */
typedef struct
{
    uint32_t source;        /**< The location which caused the error. */
    uint32_t err_code;      /**< Error code which has been raised. */
} ali_ext_error_evt_t;


/**@brief Structure for extend module event. */
typedef struct
{
    ali_ext_evt_type_t type;       /**< Event type. */
    union
    {
        ali_ext_error_evt_t   error;    /**< Data provided for error event. */
    } data;
} ali_ext_event_t;


// Forward declaration of the ali_ext_t type.
typedef struct ali_ext_s ali_ext_t;


/**
 * @brief Transport layer Tx function.
 *
 * @param[in] p_context   Context passed to interrupt handler, set on initialization.
 * @param[in] cmd         Commands.
 * @param[in] p_data      Pointer to Tx data.
 * @param[in] length      Length of Tx data.
 */
typedef uint32_t (*ali_ext_tx_func_t)(void * p_context, uint8_t cmd, uint8_t * p_data, uint16_t length);


/**
 * @brief Event handler.
 *
 * @param[in] p_context   Context passed to interrupt handler, set on initialization.
 * @param[in] p_event     Pointer to event structure. Event is allocated on the stack so it is available
 *                        only within the context of the event handler.
 */
typedef void (*ali_ext_event_handler_t)(void * p_context, ali_ext_event_t * p_event);


/**@brief Structure for extend module configuration. */
typedef struct
{
    ali_ext_event_handler_t event_handler;      /**< Pointer to event handler. */
    void                  * p_evt_context;      /**< Pointer to context which will be passed as a parameter of event_handler. */
    ali_ext_tx_func_t       tx_func;            /**< Pointer to Tx function (notification). */
    void                  * p_tx_func_context;  /**< Pointer to context which will be passed as a parameter of tx_func. */
    uint8_t               * p_fw_version;       /**< Pointer to firmware version. */
    uint8_t                 fw_version_len;     /**< Length of firmware version, excluding the terminating '\0'. */
    uint32_t                model_id;
    uint8_t               * p_product_key;      /**< Pointer to product key string. */
    uint8_t                 product_key_len;    /**< Length of product key. */
    uint8_t               * p_secret;           /**< Pointer to secret string. */
    uint8_t                 secret_len;         /**< Length of secret. */
    uint8_t               * p_device_name;      /**< Pointer to device name string. */
    uint8_t                 device_name_len;    /**< Length of device name. */
    uint8_t               * p_v2_signature;     /**< Pointer to V2 network signature. */
    uint8_t                 v2_signature_len;   /**< Length of V2 network signature. */
} ali_ext_init_t;


/**@brief Extend module structure. This contains various status information for the module. */
struct ali_ext_s
{
    ali_ext_event_handler_t event_handler;                          /**< Pointer to event handler. */
    void                  * p_evt_context;                          /**< Pointer to context which will be passed as a parameter of event_handler. */
    ali_ext_tx_func_t       tx_func;                                /**< Pointer to Tx function (notification). */
    void                  * p_tx_func_context;                      /**< Pointer to context which will be passed as a parameter of tx_func. */
    bool                    is_authenticated;                       /**< Flag to indicate if authentication has been passed. */
    uint8_t                 tlv_01_rsp[ALI_EXT_MAX_TLV_01_RSP_LEN]; /**< TLV type 01 response data. */
    uint8_t                 tlv_01_rsp_len;                         /**< Length of TLV type 01 response data. */
    uint32_t                model_id;
    uint8_t               * p_product_key;                          /**< Pointer to product key string. */
    uint8_t                 product_key_len;                        /**< Length of product key. */
    uint8_t               * p_secret;                               /**< Pointer to secret string. */
    uint8_t                 secret_len;                             /**< Length of secret. */
    uint8_t               * p_device_name;                          /**< Pointer to device name string. */
    uint8_t                 device_name_len;                        /**< Length of device name. */
    uint8_t               * p_v2_signature;                         /**< Pointer to V2 network signature. */
    uint8_t                 p_random[16];                               /**< Pointer to secret string. */
    uint8_t                 random_len;                             /**< Length of secret. */
    uint8_t                 v2_signature_len;                       /**< Length of V2 network signature. */
    uint8_t                 tx_buff[ALI_EXT_TX_BUFF_LEN];           /**< Tx buffer. */
};


/**
 * @brief Function for initializing the extend module.
 *
 * This function configures and enables the extend module.
 *
 * @param[in] p_ext         Extend module structure.
 * @param[in] p_init        Initial configuration. Default configuration used if NULL.
 *
 * @retval    NRF_SUCCESS             If initialization was successful.
 * @retval    NRF_ERROR_INVALID_PARAM If invalid parameters have been provided.
 * @retval    NRF_ERROR_NULL          If NULL pointers are provided.
 */
ret_code_t ali_ext_init(ali_ext_t * p_ext, ali_ext_init_t const * p_init);


/**
 * @brief Function for resetting the state machine of extend module.
 *
 * @param[in] p_ext     Extend module structure.
 */
void ali_ext_reset(ali_ext_t * p_ext);


/**@brief Function for handling the extended commands from central.
 *
 * @param[in] p_ext     Extend module structure.
 * @param[in] cmd       Command.
 * @param[in] p_data    Pointer to data.
 * @param[in] length    Length of data.
 */
void ali_ext_on_command(ali_ext_t * p_ext, uint8_t cmd, uint8_t * p_data, uint16_t length);


/**@brief Function for handling authentication result
 *
 * @param[in] p_ext             Extend module structure.
 * @param[in] is_authenticated  Authentication status.
 */
void ali_ext_on_auth(ali_ext_t * p_ext, bool is_authenticated);


#ifdef __cplusplus
}
#endif

#endif // ALI_EXT_H__

/** @} */

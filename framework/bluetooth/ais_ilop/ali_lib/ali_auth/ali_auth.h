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
 * @defgroup ali_auth Alibaba IOT Service
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    Alibaba IOT authentication module implementation.
 *
 * @details The Alibaba IOT authentication module performs random number generation,
 *          key regeneration, and manages the state machine for initial cipher
 *          verification.
 */

#ifndef ALI_AUTH_H__
#define ALI_AUTH_H__

#include <stdint.h>
#include "ali_cmd.h"
#include <aos/aos.h>
#include "ali_common.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ALI_AUTH_SECRET_LEN_MAX 64      /**< Maximum length of secret. */
#define ALI_AUTH_DKEY_LEN_MAX   32      /**< Maximum length of device key. */
#define ALI_AUTH_PKEY_LEN_MAX   20      /**< Maximum length of product key. */
#define ALI_AUTH_MAC_LEN        6       /**< Length of "MAC". */
#define ALI_AUTH_PRS_LEN        16      /**< Length of random sequence. */
#define ALI_AUTH_SEC_KEY_LEN    16      /**< Length of secret key (AES-128-ECB). */
#define ALI_AUTH_IKM_MAX_LEN    (ALI_AUTH_SECRET_LEN_MAX + ALI_AUTH_MAC_LEN + ALI_AUTH_PRS_LEN + 2) /**< Length of IKM. */

#define ALI_AUTH_PKEY_V1_LEN    20      /**< Length of product key, V1 network. */
#define ALI_AUTH_DKEY_V1_LEN    20      /**< Length of device key, V1 network. */
#define ALI_AUTH_SECRET_V1P_LEN 40      /**< Length of secret, V1 network, secret per product. */
#define ALI_AUTH_SECRET_V1D_LEN 32      /**< Length of secret, V1 network, secret per device. */

#define ALI_AUTH_PKEY_V2_LEN    11      /**< Length of product key, V2 network. */
#define ALI_AUTH_SECRET_V2P_LEN 16      /**< Length of secret, V2 network, secret per product. */
#define ALI_AUTH_SECRET_V2D_LEN 32      /**< Length of secret, V2 network, secret per device. */

#define ALI_AUTH_V2_SECRET_LEN          32  /**< V2 network: Maximum length of secret */
#define ALI_AUTH_V2_DEV_NAME_LEN_MAX    32  /**< V2 network: Maximum length of device name. */
#define ALI_AUTH_V2_SIGNATURE_LEN       32  /**< V2 network: Length of the signature used for authentication. */


/**
 * @brief Finite state machine.
 */
typedef enum
{
    ALI_AUTH_STATE_IDLE,        /**< Idle. */
    ALI_AUTH_STATE_CONNECTED,   /**< Connected. */
    ALI_AUTH_STATE_SVC_ENABLED, /**< Both indication and notification have been enabled. */
    ALI_AUTH_STATE_RAND_SENT,   /**< Random sequence sent. */
    ALI_AUTH_STATE_REQ_RECVD,   /**< Authentication request has been received. */
    ALI_AUTH_STATE_DONE,        /**< Authentication process finished. */
    ALI_AUTH_STATE_FAILED,      /**< Authentication process finished. */
} ali_auth_state_t;


/**
 * @brief Types of events.
 */
typedef enum
{
    ALI_AUTH_EVT_DONE,          /**< Authentication process finished. */
    ALI_AUTH_EVT_KEY_UPDATE,    /**< New key has been generated. */
    ALI_AUTH_EVT_ERROR,         /**< Error reported by referred modules. */
} ali_auth_evt_type_t;


/**@brief Structure for authentication completion event @ref ALI_AUTH_EVT_DONE.
 */
typedef struct
{
    bool result;                    /**< Authentication result. */
} ali_auth_done_evt_t;


/**@brief Structure for key update event @ref ALI_AUTH_EVT_KEY_UPDATE.
 */
typedef struct
{
    uint8_t * p_sec_key;            /**< Pointer to new secret key. */
} ali_auth_key_update_evt_t;


/**@brief Structure for error event. */
typedef struct
{
    uint32_t source;                /**< The location which caused the error. */
    uint32_t err_code;              /**< Error code which has been raised. */
} ali_auth_error_evt_t;


/**@brief Structure for authentication module event. */
typedef struct
{
    ali_auth_evt_type_t type;       /**< Event type. */
    union
    {
        ali_auth_done_evt_t       auth_done;    /**< Data provided for authentication completion event. */
        ali_auth_key_update_evt_t new_key;      /**< Data provided for key update event. */
        ali_auth_error_evt_t      error;        /**< Data provided for error event. */
    } data;
} ali_auth_event_t;


// Forward declaration of the ali_auth_t type.
typedef struct ali_auth_s ali_auth_t;


/**
 * @brief Authentication module Tx function.
 *
 * @param[in] p_context   Context passed to interrupt handler, set on initialization.
 * @param[in] cmd         Commands.
 * @param[in] p_data      Pointer to Tx data.
 * @param[in] length      Length of Tx data.
 */
typedef uint32_t (*ali_auth_tx_func_t)(void * p_context, uint8_t cmd, uint8_t * p_data, uint16_t length);


/**
 * @brief Authentication module event handler.
 *
 * @param[in] p_context   Context passed to interrupt handler, set on initialization.
 * @param[in] p_event     Pointer to event structure. Event is allocated on the stack so it is available
 *                        only within the context of the event handler.
 */
typedef void (*ali_auth_event_handler_t)(void * p_context, ali_auth_event_t * p_event);


/**@brief Structure for authentication module configuration. */
typedef struct
{
    bool                     feature_enable;    /**< Whether authentication sequence is enabled. */
    uint32_t                 timeout;           /**< Timeout, in number of ticks. */
    ali_auth_event_handler_t event_handler;     /**< Pointer to event handler. */
    void                   * p_evt_context;     /**< Pointer to context which will be passed as a parameter of event_handler. */
    ali_auth_tx_func_t       tx_func;           /**< Pointer to Tx function. */
    void                   * p_tx_func_context; /**< Pointer to context which will be passed as a parameter of tx_func. */
    uint8_t                * p_mac;             /**< Pointer to MAC (binary, 6-byte). */
    uint8_t                * p_secret;          /**< Pointer to secret (ASCII, Maximum 64 bytes). */
    uint8_t                  secret_len;        /**< Length of secret. (Typical 40-byte, max. 64 bytes.)*/
    uint8_t                * p_dkey;            /**< Pointer to device key (ASCII, Maximum 20 bytes). For V2 network, this is device name. */
    uint8_t                  dkey_len;          /**< Length of device key. (Min. 11 bytes, max. 20 bytes.) */
    uint8_t                * p_pkey;            /**< Pointer to product key (ASCII, Maximum 20 bytes). */
    uint8_t                  pkey_len;          /**< Length of product key. (Min. 11 bytes, max. 20 bytes.) */
    uint32_t                 model_id;          /**< Model ID, for precomputation of V2 network signature. */
} ali_auth_init_t;


/**@brief Authentication module structure. This contains various status information for the module. */
struct ali_auth_s
{
    bool                     feature_enable;            /**< Whether authentication sequence is enabled. */
    ali_auth_state_t         state;                     /**< Finite state machine. */
    ali_auth_event_handler_t event_handler;             /**< Pointer to event handler. */
    void                   * p_evt_context;             /**< Pointer to context which will be passed as a parameter of event_handler. */
    ali_auth_tx_func_t       tx_func;                   /**< Pointer to Tx function. */
    void                   * p_tx_func_context;         /**< Pointer to context which will be passed as a parameter of tx_func. */
    uint32_t                 timeout;                   /**< Timeout of procedures, in number of ticks. */
    aos_timer_t              timer;                     /**< Timer for procedure timeout. */
    uint16_t                 ikm_len;                   /**< Derived length of IKM. */
    uint8_t                  ikm[ALI_AUTH_IKM_MAX_LEN]; /**< Input key material. */
    uint8_t                  okm[ALI_AUTH_SEC_KEY_LEN]; /**< Output key material. */
    uint8_t                  key[ALI_AUTH_DKEY_LEN_MAX];/**< Device/Product key. */
    uint8_t                  key_len;                   /**< Device/Product key length. */
	
    struct
    {
        uint8_t product_key[ALI_AUTH_PKEY_V2_LEN];          /**< Product key, which comes from p_pkey. */
        uint8_t device_name[ALI_AUTH_V2_DEV_NAME_LEN_MAX];  /**< Device name, which comes from p_dkey. */
        uint8_t device_name_len;                            /**< Device name length. */
        uint8_t secret[ALI_AUTH_V2_SECRET_LEN];             /**< Device name, which comes from p_secret. */
        uint8_t v2_signature[ALI_AUTH_V2_SIGNATURE_LEN];    /**< Signature precomputed for V2 network authentication. */
    } v2_network;
};


/**
 * @brief Function for initializing the authentication module.
 *
 * This function configures and enables the authentication module.
 *
 * @param[in] p_auth        Authentication module structure.
 * @param[in] p_init        Initial configuration. Default configuration used if NULL.
 *
 * @retval    NRF_SUCCESS             If initialization was successful.
 * @retval    NRF_ERROR_INVALID_PARAM If invalid parameters have been provided.
 * @retval    NRF_ERROR_NULL          If NULL pointers are provided.
 */
ret_code_t ali_auth_init(ali_auth_t * p_auth, ali_auth_init_t const * p_init);


/**
 * @brief Function for resetting the state machine of authentication module.
 *
 * @param[in] p_auth Authentication module structure.
 */
void ali_auth_reset(ali_auth_t * p_auth);


/**@brief Function for handling the commands from central.
 *
 * @param[in]   p_auth      Authentication module structure.
 * @param[in]   cmd         Command.
 * @param[in]   p_data      Pointer to data.
 * @param[in]   length      Length of data.
 */
void ali_auth_on_command(ali_auth_t * p_auth, uint8_t cmd, uint8_t * p_data, uint16_t length);


/**@brief Function for handling the event @ref BLE_GAP_EVT_CONNECTED from BLE Stack.
 *
 * @param[in]   p_auth      Authentication module structure.
 */
void ali_auth_on_connected(ali_auth_t * p_auth);


/**@brief Function for handling the event when the notification of "Notify Characteristics" 
 *        and the indications of "Indicate Characteristics" are enabled.
 *
 * @param[in]   p_auth      Authentication module structure.
 */
void ali_auth_on_enable_service(ali_auth_t * p_auth);


/**@brief Function for handling the event when TX has completed.
 *
 * @param[in]   p_auth      Authentication module structure.
 */
void ali_auth_on_tx_complete(ali_auth_t * p_auth);


/**@brief Function for getting device name (V2 Network only).
 *
 * @param[in]   p_auth          Authentication module structure.
 * @param[out]  pp_device_name  Pointer to pointer of device name.
 * @param[out]  p_length        Length of data copied to p_buff.
 *
 * @retval    NRF_SUCCESS             If initialization was successful.
 * @retval    NRF_ERROR_NOT_SUPPORTED If V2 network is not supported.
 * @retval    NRF_ERROR_NULL          If NULL pointers are provided.
 */
ret_code_t ali_auth_get_device_name(ali_auth_t * p_auth, uint8_t ** pp_device_name, uint8_t * p_length);


/**@brief Function for getting product key (V2 Network only).
 *
 * @param[in]   p_auth          Authentication module structure.
 * @param[out]  pp_prod_key     Pointer to pointer of product key.
 * @param[out]  p_length        Length of data copied to p_buff.
 *
 * @retval    NRF_SUCCESS             If initialization was successful.
 * @retval    NRF_ERROR_NOT_SUPPORTED If V2 network is not supported.
 * @retval    NRF_ERROR_NULL          If NULL pointers are provided.
 */
ret_code_t ali_auth_get_product_key(ali_auth_t * p_auth, uint8_t ** pp_prod_key, uint8_t * p_length);

/**@brief Function for getting secret (V2 Network only).
 *
 * @param[in]   p_auth          Authentication module structure.
 * @param[out]  pp_secret       Pointer to pointer of pp_secret.
 * @param[out]  p_length        Length of data copied to p_buff.
 *
 * @retval    NRF_SUCCESS             If initialization was successful.
 * @retval    NRF_ERROR_NOT_SUPPORTED If V2 network is not supported.
 * @retval    NRF_ERROR_NULL          If NULL pointers are provided.
 */
ret_code_t ali_auth_get_secret(ali_auth_t * p_auth, uint8_t ** pp_secret, uint8_t * p_length);

/**@brief Function for getting v2 network signature (V2 Network only).
 *
 * @param[in]   p_auth          Authentication module structure.
 * @param[out]  pp_signature    Pointer to pointer of signature.
 * @param[out]  p_length        Length of data copied to p_buff.
 *
 * @retval    NRF_SUCCESS             If initialization was successful.
 * @retval    NRF_ERROR_NOT_SUPPORTED If V2 network is not supported.
 * @retval    NRF_ERROR_NULL          If NULL pointers are provided.
 */
ret_code_t ali_auth_get_v2_signature(ali_auth_t * p_auth, uint8_t ** pp_signature, uint8_t * p_length);


#ifdef __cplusplus
}
#endif

#endif // ALI_AUTH_H__

/** @} */

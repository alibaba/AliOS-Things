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
 * @defgroup ali_transport Alibaba IOT Service
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    Alibaba IOT transport layer implementation.
 *
 * @details The Alibaba IOT transport layer performs fragmentation & reassembly,
 *          and encryption & decryption, over packets to and from ble_ais module.
 *          This module also supports two types of packets: generi packets and
 *          firmware data packets, which have different schemes of fragmentation
 *          and reassembly.
 */

#ifndef ALI_TRANSPORT_H__
#define ALI_TRANSPORT_H__

#include <stdint.h>
//#include "sdk_errors.h"
//#include "ble.h"
//#include "ble_srv_common.h"
//#include "app_timer.h"
#include "ali_cmd.h"
//#include "nrf_soc.h"
#include "ali_common.h"
#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ALI_TRANSPORT_MAX_TX_DATA_LEN  1024     /**< Maximum length of data (in bytes) that can be transmitted to the peer. */
#define ALI_TRANSPORT_MAX_RX_DATA_LEN  1024     /**< Maximum length of data (in bytes) that can be received from the peer. */
#define ALI_TRANSPORT_VERSION          0        /**< Packet header version, conveyed in the version field of the packet header. */


/**
 * @brief Types of Transport layer events.
 */
typedef enum
{
    ALI_TRANSPORT_EVT_TX_DONE,      /**< Requested TX transfer completed. */
    ALI_TRANSPORT_EVT_RX_DONE,      /**< Requested RX transfer completed. */
    ALI_TRANSPORT_EVT_TX_TIMEOUT,   /**< Requested TX transfer timed out. */
    ALI_TRANSPORT_EVT_RX_TIMEOUT,   /**< Requested RX transfer timed out. */
    ALI_TRANSPORT_EVT_ERROR,        /**< Error reported by referred modules. */
} ali_transport_evt_type_t;


/**
 * @brief Types of Tx.
 */
typedef enum
{
    ALI_TRANSPORT_TX_TYPE_NOTIFY,       /**< Use notification for Tx. */
    ALI_TRANSPORT_TX_TYPE_INDICATE,     /**< Use indication for Tx. */
} ali_transport_tx_type_t;


/**@brief Structure for Tx/Rx completion event.
 * @note  Includes @ref ALI_TRANSPORT_EVT_TX_DONE and @ref ALI_TRANSPORT_EVT_RX_DONE.
 */
typedef struct
{
    uint8_t * p_data;               /**< Pointer to memory used for transfer. */
    uint16_t  length;               /**< Number of bytes transfered. */
    uint8_t   cmd;                  /**< Command. */
    uint8_t   num_frames;           /**< Number of frames involved in Tx / Rx. */
} ali_transport_xfer_evt_t;


/**@brief Structure for transport layer error event. */
typedef struct
{
    uint32_t source;                /**< The location which caused the error. */
    uint32_t err_code;              /**< Error code which has been raised. */
} ali_transport_error_evt_t;


/**@brief Structure for transport layer event. */
typedef struct
{
    ali_transport_evt_type_t type;  /**< Event type. */
    union
    {
        ali_transport_xfer_evt_t  rxtx;     /**< Data provided for transfer completion events. */
        ali_transport_error_evt_t error;    /**< Data provided for error event. */
    } data;
} ali_transport_event_t;


// Forward declaration of the ali_transport_t type.
typedef struct ali_transport_s ali_transport_t;


/**
 * @brief Transport layer Tx function.
 *
 * @param[in] p_context   Context passed to interrupt handler, set on initialization.
 * @param[in] p_data      Pointer to Tx data.
 * @param[in] length      Length of Tx data.
 */
typedef uint32_t (*ali_transport_tx_func_t)(void * p_context, uint8_t * p_data, uint16_t length);

/**
 * @brief Transport layer event handler.
 *
 * @param[in] p_context   Context passed to interrupt handler, set on initialization.
 * @param[in] p_event     Pointer to event structure. Event is allocated on the stack so it is available
 *                        only within the context of the event handler.
 */
typedef void (*ali_transport_event_handler_t)(void * p_context, ali_transport_event_t * p_event);


/**@brief Structure for transport layer configuration. */
typedef struct
{
    uint8_t                     * tx_buffer;            /**< Tx buffer provided by the application. */
    uint16_t                      tx_buffer_len;        /**< Size of Tx buffer provided. */
    uint8_t                     * rx_buffer;            /**< Rx buffer provided by the application. */
    uint16_t                      rx_buffer_len;        /**< Size of Rx buffer provided. */
    uint32_t                      timeout;              /**< Timeout of Tx/Rx, in number of ticks. */
    ali_transport_event_handler_t event_handler;        /**< Pointer to event handler. */
    void                        * p_evt_context;        /**< Pointer to context which will be passed as a parameter of event_handler. */
    ali_transport_tx_func_t       tx_func_notify;       /**< Pointer to Tx function (notify). */
    ali_transport_tx_func_t       tx_func_indicate;     /**< Pointer to Tx function (indicate). */
    void                        * p_tx_func_context;    /**< Pointer to context which will be passed as a parameter of tx_func. */
    uint8_t                     * p_key;                /**< Key for AES-128 encryption. */
} ali_transport_init_t;


typedef struct ecb_hal_data_s {
    uint8_t key[16];
    uint8_t data[256];
} ecb_hal_data_t;

/**@brief Transport layer structure. This contains various status information for the layer. */
struct ali_transport_s
{
    struct
    {
        uint8_t               * buff;           /**< Tx buffer. */
        uint16_t                buff_size;      /**< Size of Tx buffer. */
        uint8_t               * data;           /**< Pointer to tx data. */
        uint16_t                len;            /**< Size of Tx data. */
        uint16_t                bytes_sent;     /**< Number of bytes sent. */
        uint8_t                 encrypted;      /**< Flag telling whether encryption is active (chapter 5.2.1) */
        uint8_t                 msg_id;         /**< Tx: Message ID (chapter 5.2.1). */
        uint8_t                 cmd;            /**< Tx: Command (chapter 5.2.1). */
        uint8_t                 total_frame;    /**< Tx: Total frame (chapter 5.2.1). */
        uint8_t                 frame_seq;      /**< Tx: Frame sequence (chapter 5.2.1). */
        uint8_t                 zeroes_padded;  /**< Tx: Number of zeroes padded. */
        uint16_t                pkt_req;        /**< Number of packets requested for Tx. */
        uint16_t                pkt_cfm;        /**< Number of packets confirmed for Tx. */
        aos_timer_t             timer;          /**< Timer for Tx timeout. */
        void                  * p_context;      /**< Pointer to context which will be passed as a parameter of tx_func. */
        ali_transport_tx_func_t active_func;    /**< Pointer to the active Tx function. */
        ali_transport_tx_func_t notify_func;    /**< Pointer to Tx function (notify). */
        ali_transport_tx_func_t indicate_func;  /**< Pointer to Tx function (indicate). */
        ecb_hal_data_t          ecb_context;    /**< ECB context from softdevice. */
    } tx;
    struct
    {
        uint8_t   * buff;                 /**< Rx buffer. */
        uint16_t    buff_size;            /**< Size of Rx buffer. */
        uint16_t    bytes_received;       /**< Number of bytes received. */
        uint8_t     msg_id;               /**< Rx: Message ID (chapter 5.2.1). */
        uint8_t     cmd;                  /**< Rx: Command (chapter 5.2.1). */
        uint8_t     total_frame;          /**< Rx: Total frame (chapter 5.2.1). */
        uint8_t     frame_seq;            /**< Rx: Frame sequence (chapter 5.2.1). */
        aos_timer_t timer;                /**< Timer for Rx timeout. */
    } rx;
    uint16_t                      max_pkt_size;         /**< MTU - 3 */
    ali_transport_event_handler_t event_handler;        /**< Pointer to event handler. */
    void                        * p_evt_context;        /**< Pointer to context which will be passed as a parameter of event_handler. */
    void                        * p_key;                /**< Pointer to encryption key. */
    uint32_t                      timeout;              /**< Timeout of Tx/Rx state machine, in number of ticks. */
};


/**
 * @brief Function for initializing the Transport layer.
 *
 * This function configures and enables the transport layer. Buffers should be 
 * provided by the application and assigned in the @ref ali_transport_init_t 
 * structure.
 *
 * @param[in] p_transport   Transport layer structure.
 * @param[in] p_init        Initial configuration. Default configuration used if NULL.
 *
 * @retval    NRF_SUCCESS             If initialization was successful.
 * @retval    NRF_ERROR_INVALID_PARAM If invalid parameters have been provided.
 * @retval    NRF_ERROR_NULL          If NULL pointers are provided.
 */
ret_code_t ali_transport_init(ali_transport_t * p_transport, ali_transport_init_t const * p_init);


/**
 * @brief Function for resetting the state machine of transport layer.
 *
 * @param[in] p_transport Transport layer structure.
 */
void ali_transport_reset(ali_transport_t * p_transport);


/**
 * @brief Function for sending data over Bluetooth LE.
 *
 * @param[in] p_transport Transport layer structure.
 * @param[in] tx_type     Type of Tx.
 * @param[in] cmd         Command.
 * @param[in] p_data      Pointer to data.
 * @param[in] length      Number of bytes to send.
 *
 * @retval    NRF_SUCCESS            If data has been queued successfully.
 * @retval    NRF_ERROR_BUSY         If Tx is on-going.
 * @retval    NRF_ERROR_NULL         If NULL pointer has been provided.
 */
ret_code_t ali_transport_send(ali_transport_t * p_transport, ali_transport_tx_type_t tx_type,
                              uint8_t cmd, uint8_t const * const p_data, uint16_t length);


/**@brief Function for handling the Rx data from lower layers.
 *
 * @param[in]   p_transport Transport layer structure.
 * @param[in]   p_data      Pointer to data.
 * @param[in]   length      Length of data.
 */
void ali_transport_on_rx_data(ali_transport_t * p_transport, uint8_t * p_data, uint16_t length);


/**@brief Function for handling the event @ref BLE_EVT_TX_COMPLETE from BLE Stack.
 *
 * @param[in]   p_transport Transport layer structure.
 * @param[in]   pkt_sent    Number of packets sent.
 */
void ali_transport_on_tx_complete(ali_transport_t * p_transport, uint16_t pkt_sent);


/**@brief Function for setting MTU.
 *
 * @param[in]   p_transport Transport layer structure.
 * @param[in]   mtu         MTU.
 *
 * @retval    NRF_SUCCESS               If data has been queued successfully.
 * @retval    NRF_ERROR_NULL            If NULL pointer is provided.
 * @retval    NRF_ERROR_INVALID_PARAM   If MTU cannot be accepted.
 */
uint32_t ali_transport_set_mtu(ali_transport_t * p_transport, uint16_t mtu);


/**@brief Function for setting encryption key.
 *
 * @param[in]   p_transport Transport layer structure.
 * @param[in]   p_key       Pointer to 16-byte key..
 *
 * @retval    NRF_SUCCESS               If data has been queued successfully.
 * @retval    NRF_ERROR_NULL            If NULL pointer is provided.
 */
uint32_t ali_transport_set_key(ali_transport_t * p_transport, uint8_t * p_key);


#ifdef __cplusplus
}
#endif

#endif // ALI_TRANSPORT_H__

/** @} */

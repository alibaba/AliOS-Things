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
 * @defgroup ble_ais Alibaba IOT Service
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    Alibaba IOT Service implementation.
 *
 * @details The Alibaba IOT Service is a simple GATT-based service with TX and RX characteristics.
 *          Data received from the peer is passed to the application, and the data received
 *          from the application of this service is sent to the peer as Handle Value
 *          Notifications. This module demonstrates how to implement a custom GATT-based
 *          service and characteristics using the SoftDevice. The service
 *          is used by the application to send and receive ASCII text strings to and from the
 *          peer.
 *
 * @note The application must propagate SoftDevice events to the Alibaba IOT Service module
 *       by calling the ble_ais_on_ble_evt() function from the ble_stack_handler callback.
 */

#ifndef BLE_AIS_H__
#define BLE_AIS_H__

//#include "ble.h"
//#include "ble_srv_common.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_UUID_AIS_SERVICE 0xFEB3                      /**< The UUID of the Alibaba IOT Service. */
#define BLE_AIS_MAX_DATA_LEN (GATT_MTU_SIZE_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Alibaba IOT service module. */


/**
 * @brief Types of events.
 */
typedef enum
{
    BLE_AIS_EVT_RX_DATA,        /**< Data has been received. */
    BLE_AIS_EVT_TX_DONE,        /**< Data has been sent. */
    BLE_AIS_EVT_SVC_ENABLED,    /**< Notification of "Notify Characteristics" characteristic and indication of "Indicate Characteristics" characteristic have been enabled. */
} ble_ais_evt_type_t;

/**@brief Structure for Rx completion event @ref BLE_AIS_EVT_RX_DATA.  */
typedef struct
{
    uint8_t * p_data;               /**< Pointer to memory used for transfer. */
    uint8_t   length;               /**< Number of bytes transfered. */
} ble_ais_rx_data_evt_t;

/**@brief Structure for Tx completion event. */
typedef struct
{
    uint8_t pkt_sent;              /**< Number of packets sent. */
} ble_ais_tx_done_evt_t;

/* Forward declaration of the ble_ais_t type. */
typedef struct ble_ais_s ble_ais_t;

/**@brief Structure for event. */
typedef struct
{
    ble_ais_evt_type_t type;  /**< Event type. */
    union
    {
        ble_ais_rx_data_evt_t rx_data;  /**< Data provided for Rx data event @ref BLE_AIS_EVT_RX_DATA. */
        ble_ais_tx_done_evt_t tx_done;  /**< Data provided for Tx-done event @ref BLE_AIS_EVT_TX_DONE. */
    } data;
} ble_ais_event_t;

/**
 * @brief Alibaba IOT Service event handler type.
 *
 * @param[in] p_context   Context passed to interrupt handler, set on initialization.
 * @param[in] p_event     Pointer to event structure. Event is allocated on the stack so it is available
 *                        only within the context of the event handler.
 */
typedef void (*ble_ais_event_handler_t)(void * p_context, ble_ais_event_t * p_event);

/**@brief Alibaba IOT Service initialization structure.
 *
 * @details This structure contains the initialization information for the service. The application
 * must fill this structure and pass it to the service using the @ref ble_ais_init
 *          function.
 */
typedef struct
{
    ble_ais_event_handler_t   event_handler;    /**< Event handler.  */
    void                    * p_context;        /**< Pointer to context passed to event_handler. */
    uint16_t                  mtu;              /**< MTU. */
} ble_ais_init_t;

typedef struct ble_gatts_char_handles_s {
    uint16_t chrc_handle;
    uint16_t value_handle;
    uint16_t user_desc_handle;
    uint16_t cccd_handle;
    uint16_t sccd_handle;
} ble_gatts_char_handles_t;

/**@brief Alibaba IOT Service structure.
 *
 * @details This structure contains status information related to the service.
 */
struct ble_ais_s
{
    uint16_t                  service_handle;           /**< Handle of Alibaba IOT Service (as provided by the SoftDevice). */
    ble_gatts_char_handles_t  rc_handles;               /**< Handles related to the "Read Characteristics" characteristic (as provided by the SoftDevice). */
    ble_gatts_char_handles_t  wc_handles;               /**< Handles related to the "Write Characteristics" characteristic (as provided by the SoftDevice). */
    ble_gatts_char_handles_t  ic_handles;               /**< Handles related to the "Indicate Characteristics" characteristic (as provided by the SoftDevice). */
    ble_gatts_char_handles_t  wwnrc_handles;            /**< Handles related to the "Write WithNoRsp Characteristics" characteristic (as provided by the SoftDevice). */
    ble_gatts_char_handles_t  nc_handles;               /**< Handles related to the "Notify Characteristics" characteristic (as provided by the SoftDevice). */
    uint16_t                  conn_handle;              /**< Handle of the current connection (as provided by the SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection. */
    bool                      is_indication_enabled;    /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
    bool                      is_notification_enabled;  /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
    bool                      is_authenticated;         /**< Flag to indicate if authentication passed. */
    ble_ais_event_handler_t   event_handler;            /**< Event handler.  */
    void                    * p_context;                /**< Pointer to context passed to event_handler. */
    uint16_t                  max_pkt_size;             /**< Maximum packet size, dervice from MTU. */
};

/**@brief Function for initializing the Alibaba IOT Service.
 *
 * @param[out] p_ais      Alibaba IOT Service structure. This structure must be supplied
 *                        by the application. It is initialized by this function and will
 *                        later be used to identify this particular service instance.
 * @param[in] p_ais_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was successfully initialized. Otherwise, an error code is returned.
 * @retval NRF_ERROR_NULL If either of the pointers p_ais or p_ais_init is NULL.
 */
uint32_t ble_ais_init(ble_ais_t * p_ais, const ble_ais_init_t * p_ais_init);

/**@brief Function for setting authentication state.
 *
 * @param[in] p_ais            Alibaba IOT Service structure.
 * @param[in] is_authenticated Authentication state provided by application.
 *
 * @retval NRF_SUCCESS    If the authentication state has been updated successfully.
 * @retval NRF_ERROR_NULL If either of the pointers p_ais or p_ais_init is NULL.
 */
uint32_t ble_ais_set_auth(ble_ais_t * p_ais, bool is_authenticated);


/**@brief Function for setting MTU.
 *
 * @param[in] p_ais       Alibaba IOT Service structure.
 * @param[in] mtu         MTU.
 *
 * @retval NRF_SUCCESS             If the authentication state has been updated successfully.
 * @retval NRF_ERROR_NULL          If either of the pointers p_ais or p_ais_init is NULL.
 * @retval NRF_ERROR_INVALID_PARAM If the provided MTU is too large.
 */
uint32_t ble_ais_set_mtu(ble_ais_t * p_ais, uint16_t mtu);


/**@brief Function for handling the Alibaba IOT Service's BLE events.
 *
 * @details The Alibaba IOT Service expects the application to call this function each time an
 * event is received from the SoftDevice. This function processes the event if it
 * is relevant and calls the Alibaba IOT Service event handler of the
 * application if necessary.
 *
 * @param[in] p_ais       Alibaba IOT Service structure.
 * @param[in] p_ble_evt   Event received from the SoftDevice.
 */
//void ble_ais_on_ble_evt(ble_ais_t * p_ais, ble_evt_t * p_ble_evt);

/**@brief Function for sending data to the peer by notification.
 *
 * @details This function sends the input data as an RX characteristic notification to the
 *          peer.
 *
 * @param[in] p_ais       Pointer to the Alibaba IOT Service structure.
 * @param[in] p_data      Data to be sent.
 * @param[in] length      Length of the data.
 *
 * @retval NRF_SUCCESS If the data was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_ais_send_notification(ble_ais_t * p_ais, uint8_t * p_data, uint16_t length);


/**@brief Function for sending data to the peer by indication.
 *
 * @details This function sends the input data as an RX characteristic indication to the
 *          peer.
 *
 * @param[in] p_ais       Pointer to the Alibaba IOT Service structure.
 * @param[in] p_data      Data to be sent.
 * @param[in] length      Length of the data.
 *
 * @retval NRF_SUCCESS If the data was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_ais_send_indication(ble_ais_t * p_ais, uint8_t * p_data, uint16_t length);


#ifdef __cplusplus
}
#endif

#endif // BLE_AIS_H__

/** @} */

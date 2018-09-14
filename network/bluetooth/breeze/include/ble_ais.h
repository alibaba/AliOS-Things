/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BLE_AIS_H__
#define BLE_AIS_H__

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
 * @retval BREEZE_SUCCESS If the service was successfully initialized. Otherwise, an error code is returned.
 * @retval BREEZE_ERROR_NULL If either of the pointers p_ais or p_ais_init is NULL.
 */
uint32_t ble_ais_init(ble_ais_t * p_ais, const ble_ais_init_t * p_ais_init);

/**@brief Function for setting authentication state.
 *
 * @param[in] p_ais            Alibaba IOT Service structure.
 * @param[in] is_authenticated Authentication state provided by application.
 *
 * @retval BREEZE_SUCCESS    If the authentication state has been updated successfully.
 * @retval BREEZE_ERROR_NULL If either of the pointers p_ais or p_ais_init is NULL.
 */
uint32_t ble_ais_set_auth(ble_ais_t * p_ais, bool is_authenticated);


/**@brief Function for setting MTU.
 *
 * @param[in] p_ais       Alibaba IOT Service structure.
 * @param[in] mtu         MTU.
 *
 * @retval BREEZE_SUCCESS             If the authentication state has been updated successfully.
 * @retval BREEZE_ERROR_NULL          If either of the pointers p_ais or p_ais_init is NULL.
 * @retval BREEZE_ERROR_INVALID_PARAM If the provided MTU is too large.
 */
uint32_t ble_ais_set_mtu(ble_ais_t * p_ais, uint16_t mtu);

/**@brief Function for sending data to the peer by notification.
 *
 * @details This function sends the input data as an RX characteristic notification to the
 *          peer.
 *
 * @param[in] p_ais       Pointer to the Alibaba IOT Service structure.
 * @param[in] p_data      Data to be sent.
 * @param[in] length      Length of the data.
 *
 * @retval BREEZE_SUCCESS If the data was sent successfully. Otherwise, an error code is returned.
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
 * @retval BREEZE_SUCCESS If the data was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_ais_send_indication(ble_ais_t * p_ais, uint8_t * p_data, uint16_t length);


#ifdef __cplusplus
}
#endif

#endif // BLE_AIS_H__

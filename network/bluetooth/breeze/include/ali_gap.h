/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef ALI_GAP_H__
#define ALI_GAP_H__

#include <stdint.h>
#include "ali_common.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Types of events.
 */
typedef enum
{
    ALI_GAP_EVT_TX_DONE,    /**< Message from application has been transmitted successfully. */
    ALI_GAP_EVT_ON_CTRL,    /**< Command ALI_CMD_CTRL has been received from the central. */
    ALI_GAP_EVT_ON_QUERY,   /**< Command ALI_CMD_QUERY has been received from the central. */
    ALI_GAP_EVT_ERROR,      /**< Error reported by referred modules. */
} ali_gap_evt_type_t;


/**@brief Structure for Rx data event @ref ALI_GAP_EVT_ON_CTRL and @ref ALI_GAP_EVT_ON_QUERY.
 */
typedef struct
{
    uint8_t * p_data;       /**< Pointer to data, which will become invalid on function exit. */
    uint16_t  length;       /**< Length of data, which will become invalid on function exit. */
} ali_gap_rx_data_evt_t;


/**@brief Structure for error event. */
typedef struct
{
    uint32_t source;        /**< The location which caused the error. */
    uint32_t err_code;      /**< Error code which has been raised. */
} ali_gap_error_evt_t;


/**@brief Structure for gapentication module event. */
typedef struct
{
    ali_gap_evt_type_t type;       /**< Event type. */
    union
    {
        ali_gap_rx_data_evt_t rx_data;  /**< Data provided for rx-data event. */
        ali_gap_error_evt_t   error;    /**< Data provided for error event. */
    } data;
} ali_gap_event_t;


// Forward declaration of the ali_gap_t type.
typedef struct ali_gap_s ali_gap_t;


/**
 * @brief Transport layer Tx function.
 *
 * @param[in] p_context   Context passed to interrupt handler, set on initialization.
 * @param[in] cmd         Commands.
 * @param[in] p_data      Pointer to Tx data.
 * @param[in] length      Length of Tx data.
 */
typedef uint32_t (*ali_gap_tx_func_t)(void * p_context, uint8_t cmd, uint8_t * p_data, uint16_t length);


/**
 * @brief Event handler.
 *
 * @param[in] p_context   Context passed to interrupt handler, set on initialization.
 * @param[in] p_event     Pointer to event structure. Event is allocated on the stack so it is available
 *                        only within the context of the event handler.
 */
typedef void (*ali_gap_event_handler_t)(void * p_context, ali_gap_event_t * p_event);


/**@brief Structure for GAP module configuration. */
typedef struct
{
    ali_gap_event_handler_t event_handler;      /**< Pointer to event handler. */
    void                  * p_evt_context;      /**< Pointer to context which will be passed as a parameter of event_handler. */
    ali_gap_tx_func_t       tx_func_notify;     /**< Pointer to Tx function (notification). */
    ali_gap_tx_func_t       tx_func_indicate;   /**< Pointer to Tx function (indication). */
    void                  * p_tx_func_context;  /**< Pointer to context which will be passed as a parameter of tx_func. */
} ali_gap_init_t;


/**@brief GAP module structure. This contains various status information for the module. */
struct ali_gap_s
{
    bool                    tx_active;          /**< Tx state. */
    ali_gap_event_handler_t event_handler;      /**< Pointer to event handler. */
    void                  * p_evt_context;      /**< Pointer to context which will be passed as a parameter of event_handler. */
    ali_gap_tx_func_t       tx_func_notify;     /**< Pointer to Tx function (notification). */
    ali_gap_tx_func_t       tx_func_indicate;   /**< Pointer to Tx function (indication). */
    void                  * p_tx_func_context;  /**< Pointer to context which will be passed as a parameter of tx_func. */
    uint8_t                 tx_cmd;             /**< Command requested for Tx. */
};


/**
 * @brief Function for initializing the GAP module.
 *
 * This function configures and enables the GAP module.
 *
 * @param[in] p_gap     GAP module structure.
 * @param[in] p_init    Initial configuration.
 *
 * @retval    BREEZE_SUCCESS            If initialization was successful.
 * @retval    BREEZE_ERROR_NULL         If NULL pointers are provided.
 */
ret_code_t ali_gap_init(ali_gap_t * p_gap, ali_gap_init_t const * p_init);


/**
 * @brief Function for resetting the state machine of GAP module.
 *
 * @param[in] p_gap GAP structure.
 */
void ali_gap_reset(ali_gap_t * p_gap);


/**@brief Function for sending commands to central through notification.
 *
 * @param[in] p_gap     GAP module structure.
 * @param[in] p_data    Pointer to data.
 * @param[in] length    Length of data.
 *
 * @retval    BREEZE_SUCCESS            If initialization was successful.
 * @retval    BREEZE_ERROR_NULL         If NULL pointers are provided.
 * @retval    BREEZE_ERROR_BUSY         If Tx process is already on-going.
 * @retval    BREEZE_ERROR_DATA_SIZE    If data size is zero.
 * @retval    BREEZE_ERROR_INVALID_DATA If invalid parameters have been provided.
 */
ret_code_t ali_gap_send_notify(ali_gap_t * p_gap, uint8_t * p_data, uint16_t length);


/**@brief Function for sending commands to central through indication.
 *
 * @param[in] p_gap     GAP module structure.
 * @param[in] p_data    Pointer to data.
 * @param[in] length    Length of data.
 *
 * @retval    BREEZE_SUCCESS            If initialization was successful.
 * @retval    BREEZE_ERROR_NULL         If NULL pointers are provided.
 * @retval    BREEZE_ERROR_BUSY         If Tx process is already on-going.
 * @retval    BREEZE_ERROR_DATA_SIZE    If data size is zero.
 * @retval    BREEZE_ERROR_INVALID_DATA If invalid parameters have been provided.
 */
ret_code_t ali_gap_send_indicate(ali_gap_t * p_gap, uint8_t * p_data, uint16_t length);


/**@brief Function for handling the commands from central.
 *
 * @param[in] p_gap     GAP module structure.
 * @param[in] cmd       Command.
 * @param[in] p_data    Pointer to data.
 * @param[in] length    Length of data.
 */
void ali_gap_on_command(ali_gap_t * p_gap, uint8_t cmd, uint8_t * p_data, uint16_t length);


/**@brief Function for handling the event @ref ALI_TRANSPORT_EVT_TX_DONE from Transport layer.
 *
 * @param[in] p_gap     GAP module structure.
 * @param[in] cmd       Command sent.
 */
void ali_gap_on_tx_done(ali_gap_t * p_gap, uint8_t cmd);


#ifdef __cplusplus
}
#endif

#endif // ALI_GAP_H__

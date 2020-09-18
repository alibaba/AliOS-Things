/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef APP_MESH_PROXY_H_
#define APP_MESH_PROXY_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Mesh Proxy Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_MESH_PROXY)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"

#define BLE_INVALID_CONNECTION_INDEX    0xFF

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


extern struct app_env_tag app_env;

/// health thermometer application environment structure
struct app_mesh_proxy_env_tag
{
    uint8_t connectionIndex;
    uint8_t isNotificationEnabled;
    uint16_t mtu;
};

typedef void(*app_mesh_proxy_tx_done_t)(void);

typedef void(*app_mesh_proxy_activity_stopped_t)(void);

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// Health Thermomter Application environment
extern struct app_mesh_proxy_env_tag app_mesh_proxy_env;

/// Table of message handlers
extern const struct ke_state_handler app_mesh_proxy_table_handler;

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize DataPath Server Application
 ****************************************************************************************
 */
void app_mesh_proxy_init(void);

/**
 ****************************************************************************************
 * @brief Add a DataPath Server instance in the DB
 ****************************************************************************************
 */
void app_mesh_proxy_add(void);

void app_mesh_proxy_connected_evt_handler(uint8_t conidx);

void app_mesh_proxy_disconnected_evt_handler(uint8_t conidx, uint8_t reason);

void app_mesh_proxy_send_data_via_notification(uint8_t conidx, uint8_t* ptrData, uint32_t length);

void app_mesh_proxy_send_data_via_write_command(uint8_t* ptrData, uint32_t length);

void app_mesh_proxy_send_data_via_write_request(uint8_t* ptrData, uint32_t length);

void app_mesh_proxy_register_tx_done(app_mesh_proxy_tx_done_t callback);

void app_mesh_proxy_update_conn_parameter(uint8_t conidx, uint32_t min_interval_in_ms, uint32_t max_interval_in_ms,
        uint32_t supervision_timeout_in_ms, uint8_t slaveLantency);

void app_mesh_proxy_control_notification(bool isEnable);

void app_mesh_proxy_mtu_exchanged_handler(uint8_t conidx, uint16_t mtu);

uint16_t app_mesh_proxy_get_mtu(void);

void app_mesh_proxy_read_ccc(uint8_t conidx);

#endif //(BLE_APP_MESH_PROXY)

/// @} APP

#endif // APP_MESH_PROXY_H_

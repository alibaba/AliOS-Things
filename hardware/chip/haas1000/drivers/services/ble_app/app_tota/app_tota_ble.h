/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */


#ifndef APP_TOTA_H_
#define APP_TOTA_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief TOTA Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_TOTA)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
#ifdef __cplusplus
extern "C" {
#endif

#define TOTA_HIGH_SPEED_BLE_CONNECTION_INTERVAL_MIN_IN_MS		25
#define TOTA_HIGH_SPEED_BLE_CONNECTION_INTERVAL_MAX_IN_MS		30
#define TOTA_HIGH_SPEED_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS	5000

#define TOTA_LOW_SPEED_BLE_CONNECTION_INTERVAL_MIN_IN_MS			400
#define TOTA_LOW_SPEED_BLE_CONNECTION_INTERVAL_MAX_IN_MS			500
#define TOTA_LOW_SPEED_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS	5000


extern struct app_env_tag app_env;

/// health thermometer application environment structure
struct app_tota_env_tag
{
    uint8_t connectionIndex;
	uint8_t	isNotificationEnabled;
	uint16_t mtu[BLE_CONNECTION_MAX];
};

typedef void(*app_tota_tx_done_t)(void);

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// Health Thermomter Application environment
extern struct app_tota_env_tag app_tota_env;

/// Table of message handlers
extern const struct ke_state_handler app_tota_table_handler;

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize DataPath Server Application
 ****************************************************************************************
 */
void app_tota_ble_init(void);

void app_tota_add_tota(void);

void app_tota_connected_evt_handler(uint8_t conidx);

void app_tota_disconnected_evt_handler(uint8_t conidx);

void app_tota_send_notification(uint8_t* ptrData, uint32_t length);

void app_tota_send_indication(uint8_t* ptrData, uint32_t length);

void app_tota_register_tx_done(app_tota_tx_done_t callback);

void app_tota_update_conn_parameter(uint8_t conidx, uint32_t min_interval_in_ms, uint32_t max_interval_in_ms,
		uint32_t supervision_timeout_in_ms, uint8_t slaveLantency);

void app_tota_mtu_exchanged_handler(uint8_t conidx, uint16_t MTU);

#ifdef __cplusplus
	}
#endif


#endif //(BLE_APP_TOTA)

/// @} APP

#endif // APP_TOTA_H_
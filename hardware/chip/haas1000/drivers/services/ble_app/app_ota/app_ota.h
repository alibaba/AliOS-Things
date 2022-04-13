/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */


#ifndef APP_OTA_H_
#define APP_OTA_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief OTA Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_OTA)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
#ifdef __cplusplus
extern "C" {
#endif

#define OTA_HIGH_SPEED_BLE_CONNECTION_INTERVAL_MIN_IN_MS		25
#define OTA_HIGH_SPEED_BLE_CONNECTION_INTERVAL_MAX_IN_MS		30
#define OTA_HIGH_SPEED_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS	5000

#define OTA_LOW_SPEED_BLE_CONNECTION_INTERVAL_MIN_IN_MS			400
#define OTA_LOW_SPEED_BLE_CONNECTION_INTERVAL_MAX_IN_MS			500
#define OTA_LOW_SPEED_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS	5000


extern struct app_env_tag app_env;

/// health thermometer application environment structure
struct app_ota_env_tag
{
    uint8_t connectionIndex;
	uint8_t	isNotificationEnabled;
	uint16_t mtu[BLE_CONNECTION_MAX];
};

typedef void(*app_ota_tx_done_t)(void);

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// Health Thermomter Application environment
extern struct app_ota_env_tag app_ota_env;

/// Table of message handlers
extern const struct ke_state_handler app_ota_table_handler;

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize DataPath Server Application
 ****************************************************************************************
 */
void app_ota_init(void);

void app_ota_add_ota(void);

void app_ota_connected_evt_handler(uint8_t conidx);

void app_ota_disconnected_evt_handler(uint8_t conidx);

void app_ota_send_notification(uint8_t* ptrData, uint32_t length);

void app_ota_send_indication(uint8_t* ptrData, uint32_t length);

void app_ota_register_tx_done(app_ota_tx_done_t callback);

void app_ota_update_conn_parameter(uint8_t conidx, uint32_t min_interval_in_ms, uint32_t max_interval_in_ms,
		uint32_t supervision_timeout_in_ms, uint8_t slaveLantency);

void app_ota_mtu_exchanged_handler(uint8_t conidx, uint16_t MTU);

#ifdef __cplusplus
	}
#endif


#endif //(BLE_APP_OTA)

/// @} APP

#endif // APP_OTA_H_

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_HRP_H
#define __APP_HRP_H

#if (BLE_APP_HR)

#include "hrps.h"
#include "hrps_task.h"

#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include "string.h"

#define INVALID_CONNECTION_INDEX	0xFF

void app_hrps_add_profile(void);

extern const struct ke_state_handler app_hrps_table_handler;

void app_hrps_init(void);

void app_hrps_connected_evt_handler(uint8_t conidx);

void app_hrps_disconnected_evt_handler(void);

/// Messages for HRP Server Profile 
enum hrp_msg_id
{
	HRP_MEASUREMENT_CCC_CHANGED = TASK_FIRST_MSG(TASK_ID_HRPS),

    HRP_MEASUREMENT_DATA_SENT,

    HRP_CTRL_POINT_RECEIVED,
	
};

struct ble_hrp_measurement_notif_config_t
{
	bool 		isNotificationEnabled;
};

struct app_hrps_env_tag
{
    uint8_t connectionIndex;
	uint8_t	isNotificationEnabled;
    struct hrps_meas_send_req meas;
};


#endif

#endif

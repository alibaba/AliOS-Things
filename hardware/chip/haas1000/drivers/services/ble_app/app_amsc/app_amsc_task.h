/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */


#include "rwip_config.h"

#ifndef APP_AMSC_TASK_H_
#define APP_AMSC_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 *
 * @brief 
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */


#if BLE_AMS_CLIENT

#include "amsc_task.h"

/// Table of message handlers
extern const struct ke_state_handler app_amsc_table_handler;

#endif //BLE_AMS_CLIENT

/// @} APP

#endif // APP_AMSC_TASK_H_

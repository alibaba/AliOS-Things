/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */


#include "rwip_config.h"

#ifndef APP_ANCC_TASK_H_
#define APP_ANCC_TASK_H_

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


#if BLE_ANC_CLIENT

#include "ancc_task.h"

/// Table of message handlers
extern const struct ke_state_handler app_ancc_table_handler;

#endif //BLE_ANC_CLIENT

/// @} APP

#endif // APP_ANCC_TASK_H_

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */


#ifndef APP_ANCC_H_
#define APP_ANCC_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 *
 * @brief ANCC Application entry point.
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if BLE_ANC_CLIENT
#include "ancc.h"

#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include "string.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 *
 * ANCC Application Functions
 *
 ****************************************************************************************
 */

void app_ancc_add_ancc(void);

/**
 ****************************************************************************************
 * @brief Inialize application and enable ANCC profile.
 *
 ****************************************************************************************
 */
void app_ancc_enable(uint8_t conidx);

#endif //BLE_ANC_CLIENT

/// @} APP

#endif // APP_ANCC_H_

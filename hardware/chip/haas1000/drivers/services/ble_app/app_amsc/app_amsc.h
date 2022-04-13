/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */


#ifndef APP_AMSC_H_
#define APP_AMSC_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 *
 * @brief AMSC Application entry point.
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if BLE_AMS_CLIENT
#include "amsc.h"

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
 * AMSC Application Functions
 *
 ****************************************************************************************
 */

void app_amsc_add_amsc(void);

/**
 ****************************************************************************************
 * @brief Inialize application and enable AMSC profile.
 *
 ****************************************************************************************
 */
void app_amsc_enable(uint8_t conidx);

#endif //BLE_AMS_CLIENT

/// @} APP

#endif // APP_AMSC_H_

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
/**
 ****************************************************************************************
 * @addtogroup APP_SEC
 * @{
 ****************************************************************************************
 */

#ifndef APP_SEC_H_
#define APP_SEC_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_APP_SEC)

#include <stdint.h>          // Standard Integer Definition

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * STRUCTURES DEFINITIONS
 ****************************************************************************************
 */

struct app_sec_env_tag
{
    // Bond status
    bool bonded;
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/// Application Security Environment
extern struct app_sec_env_tag app_sec_env;

/// Table of message handlers
extern const struct ke_state_handler app_sec_table_handler;

/*
 * GLOBAL FUNCTIONS DECLARATIONS
 ****************************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief Initialize the Application Security Module
 ****************************************************************************************
 */
void app_sec_init(void);


#if (NVDS_SUPPORT)
/**
 ****************************************************************************************
 * @brief Remove all bond data stored in NVDS
 ****************************************************************************************
 */
void app_sec_remove_bond(void);
#endif //(NVDS_SUPPORT)

/**
 ****************************************************************************************
 * @brief Send a security request to the peer device. This function is used to require the
 * central to start the encryption with a LTK that would have shared during a previous
 * bond procedure.
 *
 * @param[in]   - conidx: Connection Index
 ****************************************************************************************
 */
void app_sec_send_security_req(uint8_t conidx, enum gap_auth authority);

void app_sec_reset_env_on_connection(void);

#ifdef __cplusplus
}
#endif

#endif //(BLE_APP_SEC)

#endif // APP_SEC_H_

/// @} APP_SEC

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef APP_HT_H_
#define APP_HT_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Health Thermometer Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_HT)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

extern struct app_env_tag app_env;

/// health thermometer application environment structure
struct app_ht_env_tag
{
    /// Measurement interval
    uint16_t htpt_meas_intv;

    /// Temperature value
    uint32_t temp_value;
    /// Temperature step
    uint32_t temp_step;
    /// Temperature type
    uint8_t temp_meas_type;
    /// Temperature type string
    char temp_type_string[16];

    /// Measurement interval timer enable
    bool timer_enable;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// Health Thermomter Application environment
extern struct app_ht_env_tag app_ht_env;

/// Table of message handlers
extern const struct ke_state_handler app_ht_table_handler;

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 *
 * Health Thermometer Application Functions
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Health Thermometer Application
 ****************************************************************************************
 */
void app_ht_init(void);

/**
 ****************************************************************************************
 * @brief Add a Health Thermometer instance in the DB
 ****************************************************************************************
 */
void app_ht_add_hts(void);

/**
 ****************************************************************************************
 * @brief Stop the interval measurement timer if used
 ****************************************************************************************
 */
void app_stop_timer (void);

/**
 ****************************************************************************************
 * @brief Enable the health thermometer profile
 ****************************************************************************************
 */
void app_ht_enable_prf(uint8_t);

/**
 ****************************************************************************************
 * @brief Increase temp_value by temp_step
 ****************************************************************************************
 */
void app_ht_temp_inc(void);

/**
 ****************************************************************************************
 * @brief Decrease temp_value by temp_step
 ****************************************************************************************
 */
void app_ht_temp_dec(void);

/**
 ****************************************************************************************
 * @brief Decrease temp_type by 1
 ****************************************************************************************
 */
void app_ht_temp_type_dec(void);

/**
 ****************************************************************************************
 * @brief Increase temp_type by 1
 ****************************************************************************************
 */
void app_ht_temp_type_inc(void);

#endif //(BLE_APP_HT)

/// @} APP

#endif // APP_HT_H_

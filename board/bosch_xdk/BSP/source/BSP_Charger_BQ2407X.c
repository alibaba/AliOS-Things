/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH. 
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @file
 * @brief  Implementation of Charger BQ2407X BSP function.
 *
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */
#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_CHARGER_BQ2407X

#include "BCDS_HAL.h"
#if BCDS_FEATURE_BSP_CHARGER_BQ2407X

#include "BSP_BoardType.h"
#include "BSP_BoardSettings.h"
#include "BSP_BoardShared.h"
#include "BCDS_BSP_Charger_BQ2407X.h"
#include "BCDS_Assert.h"

static bool chargerEnabled = false;
/*
 * Refer to interface header for description
 */
Retcode_T BSP_Charger_BQ2407X_Connect(void)
{
    GPIO_PinModeSet(CHARGING_SELECT_PORT, CHARGING_SELECT_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(VOUT_SENSE_ADC_PORT, VOUT_SENSE_ADC_PIN, gpioModeDisabled, 0);
    return RETCODE_OK;
}

/*
 * refer to interface for description
 */
Retcode_T BSP_Charger_BQ2407X_Enable(uint32_t control)
{
    Retcode_T retcode = RETCODE_OK;

    if ((uint32_t) BSP_XDK_CHARGING_SPEED_2 == control)
    {
        GPIO_PinOutSet(CHARGING_SELECT_PORT, CHARGING_SELECT_PIN);

    }
    else if ((uint32_t) BSP_XDK_CHARGING_SPEED_1 == control)
    {
        GPIO_PinOutClear(CHARGING_SELECT_PORT, CHARGING_SELECT_PIN);
        chargerEnabled = true;
    }
    else
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        chargerEnabled = true;

    }
    return retcode;
}
/*
 * Refer interface header for description.
 */
Retcode_T BSP_Charger_BQ2407X_Disable(void)
{
    chargerEnabled = false;
    GPIO_PinOutClear(CHARGING_SELECT_PORT, CHARGING_SELECT_PIN);
    return RETCODE_OK;
}

/*
 * Refer interface header for description.
 */
Retcode_T BSP_Charger_BQ2407X_MeasureSignal(uint32_t signal, uint32_t* output)
{
    BCDS_UNUSED(signal);
    BCDS_UNUSED(output);

    return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED));
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Charger_BQ2407X_GetStatus(BSP_ChargeState_T* state)
{
    BCDS_UNUSED(state);
    return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED));
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Charger_BQ2407X_CheckStatus(BSP_ChargeState_T* state, uint32_t BatteryVoltage)
{
    Retcode_T retval = RETCODE_OK;
    *state = BSP_XDK_CHARGE_STATUS_UNDEFINED;
    if (NULL == state)
    {
        retval = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    if (RETCODE_OK == retval)
    {
        if ((BatteryVoltage >= CHARGE_CRITICAL_LOWER_LIMIT) && (BatteryVoltage <= CHARGE_CRITICAL_UPPER_LIMIT)) //refer to charger and battery specification for threshold information
        {
            *state = BSP_XDK_CHARGE_STATUS_CRITICAL;
        }
        else if ((BatteryVoltage > CHARGE_LOW_LOWER_LIMIT) && (BatteryVoltage < CHARGE_LOW_UPPER_LIMIT)) //refer to charger and battery specification for threshold information
        {
            *state = BSP_XDK_CHARGE_STATUS_LOW;
        }
        else if ((BatteryVoltage >= CHARGE_NORMAL_LOWER_LIMIT) && (BatteryVoltage <= CHARGE_NORMAL_UPPER_LIMIT)) //refer to charger and battery specification for threshold information
        {
            *state = BSP_XDK_CHARGE_STATUS_NORMAL;
        }
        else if ((BatteryVoltage > CHARGE_HIGH_LOWER_LIMIT) && (BatteryVoltage < CHARGE_HIGH_UPPER_LIMIT)) //refer to charger and battery specification for threshold information
        {
            *state = BSP_XDK_CHARGE_STATUS_FULL;
        }
        else if ((BatteryVoltage >= CHARGE_ON_LOWER_LIMIT) && (BatteryVoltage <= CHARGE_ON_UPPER_LIMIT))
        {
            *state = BSP_XDK_CHARGE_STATUS_ON;
        }
        else
        {
            ;/* state = BSP_XDK_CHARGE_STATUS_UNDEFINED; */
        }
    }
    return retval;
}
/*
 * Refer interface header for description.
 */
Retcode_T BSP_Charger_BQ2407X_Disconnect(void)
{
    GPIO_PinModeSet(CHARGING_SELECT_PORT, CHARGING_SELECT_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(VOUT_SENSE_ADC_PORT, VOUT_SENSE_ADC_PIN, gpioModeDisabled, 0);
    return RETCODE_OK;
}

/*
 * Refer interface header for description.
 */
Retcode_T BSP_Charger_BQ2407X_SetSysoffLow(void)
{
    return RETCODE(RETCODE_SEVERITY_INFO, RETCODE_NOT_SUPPORTED);
}

/*
 * Refer interface header for description.
 */
Retcode_T BSP_Charger_BQ2407X_SetSysoffHigh(void)
{
    return RETCODE(RETCODE_SEVERITY_INFO, RETCODE_NOT_SUPPORTED);
}

#endif


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
 * @brief  Implementation of Charger BQ2405X  BSP function.
 *
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */

#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_CHARGER_BQ2405X

#include "BCDS_HAL.h"
#if BCDS_FEATURE_BSP_CHARGER_BQ2405X

#include "BSP_BoardType.h"
#include "BSP_BoardExtSettings.h"
#include "BSP_BoardSettings.h"
#include "BSP_BoardShared.h"
#include "BCDS_BSP_Charger_BQ2405X.h"
#include "BSP_BoardShared.h"
#include "BCDS_Assert.h"

static bool chargerEnabled = false;
/*
 * Refer to interface header for description
 */
Retcode_T BSP_Charger_BQ2405X_Connect(void)
{
    GPIO_PinModeSet(PGOOD_N_PORT, PGOOD_N_PIN, PGOOD_N_MODE, 1);
    GPIO_PinModeSet(CHARGING_N_PORT, CHARGING_N_PIN, CHARGING_N_MODE, 1);
    GPIO_PinModeSet(VBAT_SENSE_PORT, VBAT_SENSE_PIN, VBAT_SENSE_MODE, 0);
    return RETCODE_OK;
}

/*
 * refer to interface for description
 */
Retcode_T BSP_Charger_BQ2405X_Enable(uint32_t control)
{
    BCDS_UNUSED(control);
    chargerEnabled = true;
    return RETCODE_OK;

}
/*
 * Refer interface header for description.
 */
Retcode_T BSP_Charger_BQ2405X_Disable(void)
{
    chargerEnabled = false;
    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
uint32_t BSP_Charger_BQ2405X_GetStatus(void)
{
    BSP_ChargeState_T status = BSP_XDK_CHARGE_STATUS_UNDEFINED;
    int32_t sample = 0;
    int32_t voltage = 0;
    if (chargerEnabled)
    {
        sample = BSP_Charger_BQ2405X_MeasureSignal((uint32_t)BSP_XDK_EXT_SENSE_VBAT_ADC);
        voltage = (2 * sample * 2500) / 4096; // the voltage value equals the sample integer value times the bit resolution in Volts/bit

        if ((BSP_IO_PIN_LOW == GPIO_PinInGet(CHARGING_N_PORT, CHARGING_N_PIN)) && (BSP_IO_PIN_HIGH == GPIO_PinInGet(PGOOD_N_PORT, PGOOD_N_PIN)))
        {
            status = BSP_XDK_CHARGE_STATUS_ERROR; // if CHG_N is low when PGOOD_N is high there must be an hardware error: impossible combination
        }
        else if ((BSP_IO_PIN_LOW == GPIO_PinInGet(CHARGING_N_PORT, CHARGING_N_PIN)) && (BSP_IO_PIN_LOW == GPIO_PinInGet(PGOOD_N_PORT, PGOOD_N_PIN)))
        {
            status = BSP_XDK_CHARGE_STATUS_ON;
        }
        else if ((BSP_IO_PIN_HIGH == GPIO_PinInGet(CHARGING_N_PORT, CHARGING_N_PIN)) && (BSP_IO_PIN_LOW == GPIO_PinInGet(PGOOD_N_PORT, PGOOD_N_PIN)))
        { //charge completed battery full or battery disconnected
            if (voltage < EXT_CHARGE_VREG_NO_BATTERY)
            {
                status = BSP_XDK_CHARGE_STATUS_NO_BATTERY;
            }
            else
            {
                status = BSP_XDK_CHARGE_STATUS_FULL;
            }
        }
        else
        {
            if ((voltage >= EXT_CHARGE_CRITICAL_LOWER_LIMIT) && (voltage <= EXT_CHARGE_CRITICAL_UPPER_LIMIT)) //refer to charger and battery specification for threshold information
            {
                status = BSP_XDK_CHARGE_STATUS_CRITICAL;
            }
            else if ((voltage > EXT_CHARGE_LOW_LOWER_LIMIT) && (voltage < EXT_CHARGE_LOW_UPPER_LIMIT)) //refer to charger and battery specification for threshold information
            {
                status = BSP_XDK_CHARGE_STATUS_LOW;
            }
            else if ((voltage >= EXT_CHARGE_NORMAL_LOWER_LIMIT) && (voltage <= EXT_CHARGE_NORMAL_UPPER_LIMIT)) //refer to charger and battery specification for threshold information
            {
                status = BSP_XDK_CHARGE_STATUS_NORMAL;
            }
            else if ((voltage <= EXT_CHARGE_NO_BATTERY_HIGH_LIMIT))
            {
                status = BSP_XDK_CHARGE_STATUS_NO_BATTERY;
            }
            else
            {
                /*lint -e506 assert needed to prevent holes*/
                assert(0);
            }
        }
    }
    return (uint32_t) status;
}

/*
 * Refer interface header for description.
 */
uint32_t BSP_Charger_BQ2405X_GetIOState(uint32_t outPin)
{
    BSP_IOState_T state = BSP_XDK_IO_STATE_ERROR;
    if (chargerEnabled)
    {
        state = BSP_XDK_IO_STATE_HIGH;
        if (outPin == (uint32_t)BSP_XDK_IO_EXT_CHG_N)
        {
            if (BSP_IO_PIN_LOW == GPIO_PinInGet(CHARGING_N_PORT, CHARGING_N_PIN))
                state = BSP_XDK_IO_STATE_LOW;
        }
        else if (outPin == (uint32_t)BSP_XDK_IO_EXT_PGOOD_N)
        {
            if (BSP_IO_PIN_LOW == GPIO_PinInGet(PGOOD_N_PORT, PGOOD_N_PIN))
                state = BSP_XDK_IO_STATE_LOW;
        }
        else
        {
            state = BSP_XDK_IO_STATE_ERROR;
        }
    }
    return (uint32_t) state;
}

/*
 * Refer interface header for description.
 */
int32_t BSP_Charger_BQ2405X_MeasureSignal(uint32_t signal)
{
    BCDS_UNUSED(signal);
    ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;
    int32_t sample = 0;
    int32_t voltage = 0;
    if (chargerEnabled)
    {
        /* Init for single conversion use, measure VDD/3 with 1.25 reference. */
        singleInit.reference = adcRef2V5;
        singleInit.input = adcSingleInpCh5;
        singleInit.resolution = adcRes12Bit;

        singleInit.acqTime = adcAcqTime64;
        while ((ADC0->STATUS & (ADC_STATUS_SINGLEACT | ADC_STATUS_SCANACT)) && (BSP_UNLOCKED == ADCLock));
        __disable_irq();
        ADCLock = BSP_LOCKED;
        __enable_irq();
        ADC_InitSingle(ADC0, &singleInit);
        ADC_Start(ADC0, adcStartSingle);

        /* Wait while conversion is active */
        while (ADC0->STATUS & (ADC_STATUS_SINGLEACT | ADC_STATUS_SCANACT));
        sample = ADC_DataSingleGet(ADC0);
        __disable_irq();
        ADCLock = BSP_UNLOCKED;
        __enable_irq();
        voltage = (2 * sample * 2500) / 4096; // the voltage value equals the sample integer value times the bit resolution in Volts/bit

    }
    return voltage;
}

/*
 * Refer interface header for description.
 */
Retcode_T BSP_Charger_BQ2405X_Disconnect(void)
{
    GPIO_PinModeSet(PGOOD_N_PORT, PGOOD_N_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(CHARGING_N_PORT, CHARGING_N_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(VBAT_SENSE_PORT, VBAT_SENSE_PIN, gpioModeDisabled, 0);
    return RETCODE_OK;
}

#endif


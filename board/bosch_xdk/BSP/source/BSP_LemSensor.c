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
 * @brief  Implementation of LEM Sensor BSP functions
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */
#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_LEM_SENSOR

#include "BCDS_HAL.h"
#if BCDS_FEATURE_BSP_LEM_SENSOR

#include "BSP_BoardType.h"
#include "BSP_BoardExtSettings.h"
#include "BSP_BoardSettings.h"
#include "BSP_BoardShared.h"
#include "BSP_ExtensionPort.h"
#include "BSP_LemSensor.h"
/*
 * Refer to interface header for description
 */
Retcode_T BSP_LemSensor_Connect(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_ConnectGpio(LEM_SENSOR_CONNECT_STATUS_PIN);
    if (RETCODE_OK == retVal)
    {
    	retVal = BSP_ExtensionPort_ConnectAdc(BSP_EXTENSIONPORT_GPIO_PD5);
    }
    if (RETCODE_OK == retVal)
    {
    	retVal = BSP_ExtensionPort_ConnectAdc(BSP_EXTENSIONPORT_GPIO_PD6);
    }
	return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_LemSensor_Disconnect(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_DisconnectGpio(LEM_SENSOR_CONNECT_STATUS_PIN);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisconnectAdc(BSP_EXTENSIONPORT_GPIO_PD5);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisconnectAdc(BSP_EXTENSIONPORT_GPIO_PD6);
    }
	return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_LemSensor_Enable(void)
{
    Retcode_T retVal = RETCODE_OK;
    Board_WakeupPowerSupply2V5(EXTENSION_BOARD);
    Board_EnablePowerSupply3V3(EXTENSION_BOARD);
    retVal = BSP_ExtensionPort_SetGpioConfig(LEM_SENSOR_CONNECT_STATUS_PIN, BSP_EXTENSIONPORT_GPIO_PINMODE, BSP_EXTENSIONPORT_INPUT_NOPULL, NULL);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(LEM_SENSOR_CONNECT_STATUS_PIN, BSP_EXTENSIONPORT_GPIO_PINVALUE, BSP_EXTENSIONPORT_GPIO_PIN_HIGH, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_EnableGpio(LEM_SENSOR_CONNECT_STATUS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_EnableAdc(BSP_EXTENSIONPORT_GPIO_PD5);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_EnableAdc(BSP_EXTENSIONPORT_GPIO_PD6);
    }
	return retVal;
}
/*
 * Refer to interface header for description
 */
Retcode_T BSP_LemSensor_Disable(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_DisableGpio(LEM_SENSOR_CONNECT_STATUS_PIN);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisableAdc(BSP_EXTENSIONPORT_GPIO_PD5);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisableAdc(BSP_EXTENSIONPORT_GPIO_PD6);
    }
    if (RETCODE_OK == retVal)
    {
        Board_SnoozePowerSupply2V5(EXTENSION_BOARD);
        Board_DisablePowerSupply3V3(EXTENSION_BOARD);
    }
	return retVal;
}


/*
 * Refer to interface header for description
 */
Retcode_T BSP_LemSensor_GetStatus(uint8_t *isLemConnected)
{
    Retcode_T retVal = RETCODE_OK;
	uint8_t pinVal;
	*isLemConnected = false;
	retVal = BSP_ExtensionPort_ReadGpio(LEM_SENSOR_CONNECT_STATUS_PIN,&pinVal);
	if(RETCODE_OK == retVal)
	{
		if(BSP_EXTENSIONPORT_GPIO_PIN_LOW == pinVal)
		{
			*isLemConnected = true;
		}
	}
	return retVal;
}

#endif /* BCDS_FEATURE_BSP_LEM_SENSORNODE */

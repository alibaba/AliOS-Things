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
 * @brief  Implementation of I2C IRSensor Node BSP functions
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */
#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_SENSOR_NODE

#include "BCDS_HAL.h"
#if BCDS_FEATURE_BSP_IRSENSOR_NODE

#include "BSP_BoardType.h"
#include "BSP_BoardSettings.h"
#include "BSP_BoardExtSettings.h"
#include "BSP_BoardShared.h"
#include "BCDS_BSP_IRSensorNode.h"
#include "BCDS_MCU_I2C_Handle.h"
#include "BSP_ExtensionPort.h"

struct MCU_I2C_Handle_S BSP_IRSensorNode_I2C_Handle;

/*
 * Refer to interface header for description
 */
Retcode_T BSP_IRSensorNode_Connect(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_ConnectI2c();

    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_IRSensorNode_Enable(void)
{
    Retcode_T retcode = RETCODE_OK;
	Board_WakeupPowerSupply2V5(EXTENSION_BOARD);
    Board_EnablePowerSupply3V3(EXTENSION_BOARD);
    retcode = BSP_ExtensionPort_SetI2cConfig(BSP_EXTENSIONPORT_I2C_MODE, BSP_EXTENSIONPORT_I2C_FAST_MODE, NULL);
    if (RETCODE_OK == retcode)
    {
        retcode = BSP_ExtensionPort_EnableI2c();
    }
    return retcode;
}
/*
 * Refer to interface header for description
 */
Retcode_T BSP_IRSensorNode_Disable(void)
{
    Retcode_T retcode = RETCODE_OK;
    retcode = BSP_ExtensionPort_DisableI2c();
	if (RETCODE_OK == retcode)
	{
		Board_SnoozePowerSupply2V5(EXTENSION_BOARD);
		Board_DisablePowerSupply3V3(EXTENSION_BOARD);
	}
    return retcode;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_IRSensorNode_Disconnect(void)
{
    Retcode_T retcode = RETCODE_OK;
    retcode = BSP_ExtensionPort_DisonnectI2c();
    return retcode;
}

/*
 * Refer to interface header for description
 */
HWHandle_T BSP_IRSensorNode_GetI2CHandle(void)
{
    HWHandle_T Handle = BSP_ExtensionPort_GetI2cHandle();
    return Handle;
}

#endif /* BCDS_FEATURE_BSP_IRSENSOR_NODE */

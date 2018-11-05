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
 * @brief  Implementation of Microphone AKU340 BSP functions
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */

#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_MIC_AKU340

#include "BCDS_HAL.h"
#if BCDS_FEATURE_BSP_MIC_AKU340

#include "BCDS_BSP_Mic_AKU340.h"
#include "BSP_BoardType.h"
#include "BSP_Mic_AKU340.h"
#include "BSP_BoardSettings.h"
#include "BSP_BoardShared.h"

static bool micEnabled = false;
/*
 * Refer to interface header for description
 */
Retcode_T BSP_Mic_AKU340_Connect(void)
{

    /**
     * Due to electrical issues noted,
     * the AKU VOUT port has to be permanently drained using
     * a pull-down to ground level in order to avoid charge accumulation
     */
	GPIO_PinModeSet(VDD_AKU_PORT, VDD_AKU_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(AKU_OUT_PORT, AKU_OUT_PIN, gpioModeInputPull, 0);
    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Mic_AKU340_Enable(void)
{
    micEnabled = true;

	Board_WakeupPowerSupply2V5(MIC_AKU340);

    GPIO_PinOutSet(VDD_AKU_PORT, VDD_AKU_PIN);
    GPIO_PinOutClear(AKU_OUT_PORT, AKU_OUT_PIN);
    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Mic_AKU340_Disable(void)
{
    micEnabled = false;

    GPIO_PinOutClear(VDD_AKU_PORT, VDD_AKU_PIN);
    GPIO_PinOutClear(AKU_OUT_PORT, AKU_OUT_PIN);

	Board_SnoozePowerSupply2V5(MIC_AKU340);

    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Mic_AKU340_Disconnect(void)
{
    GPIO_PinModeSet(VDD_AKU_PORT, VDD_AKU_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(AKU_OUT_PORT, AKU_OUT_PIN, gpioModeDisabled, 0);

    return RETCODE_OK;
}

/*
 * Deprecated */
int32_t BSP_Mic_AKU340_Sense(void)
{
    return INT32_MIN;
}
#endif /* BCDS_FEATURE_BSP_MIC_AKU340 */



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
 * @brief  Implementation of test interface BSP functions
 */

#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_TEST_INTERFACE

#include "BCDS_HAL.h"

#if BCDS_FEATURE_BSP_TEST_INTERFACE

#include "BSP_TestInterface.h"
#include "BSP_BoardShared.h"
#include "BCDS_USB.h"

static USB_lineCoding_tp BSP_TestIF_USB_Handle = NULL;

static USB_rxCallback TestIF_USB_ISRCallback = NULL;

/*
 * Refer to interface header for description
 */
Retcode_T BSP_TestInterface_Connect(void)
{
    BSP_TestIF_USB_Handle = &USB_lineCoding_mds;
    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_TestInterface_Enable(void)
{
    /* Dont enable if the callback has not been registered */
    if ( NULL == BSP_TestIF_USB_Handle || NULL == BSP_TestIF_USB_Handle->usbRxCallback)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED);
    }

    TestIF_USB_ISRCallback = BSP_TestIF_USB_Handle->usbRxCallback;
    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_TestInterface_Disable(void)
{
    return (Retcode_T)RETCODE_NOT_SUPPORTED;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_TestInterface_Disconnect(void)
{
    return (Retcode_T)RETCODE_NOT_SUPPORTED;
}
/*
 * Refer to interface header for description
 */
HWHandle_T BSP_TestInterface_GetUSBHandle(void)
{
    if (NULL == BSP_TestIF_USB_Handle)
    {
        return (HWHandle_T) NULL;
    }

    return (HWHandle_T) BSP_TestIF_USB_Handle;
}

#endif /* BCDS_FEATURE_BSP_TEST_INTERFACE */



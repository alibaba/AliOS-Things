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
 *
 * @defgroup BCDS_WLAN Wlan
 *
 * @{
 *
 * @brief WLAN module
 *
 * @details Put here the documentation of this header file. Explain the interface exposed
 * by this header, e.g. what is the purpose of use, how to use it, etc.
 *
 * @file
 *
 **/

#ifndef BCDS_Wlan_H
#define BCDS_Wlan_H

/* Include all headers which are needed by this file. */
#include "BCDS_Retcode.h"
/* Put the type and macro definitions here */
/*
 * enumeration of custom return codes
 */
/* No Includes here to avoid dependencies */

enum BCDS_Wlan_ModuleID_E
{
    BCDS_WLAN_MODULE_ID_WLAN_DRIVER         = 1,
    BCDS_WLAN_MODULE_ID_WLAN_CONNECT        = 2,
    BCDS_WLAN_MODULE_ID_NETWORK_CONFIG      = 3
};

/* Put the function declarations here */

enum Retcode_Wlan_E
{
    RETCODE_WLANDRIVER_SPI_RX_ERROR = RETCODE_FIRST_CUSTOM_CODE,
    RETCODE_WLANDRIVER_SPI_TX_ERROR,
    RETCODE_WLANDRIVER_SPI_DATALOSS,
    RETCODE_WLANDRIVER_NULL_HANDLE
};
/* Put the function declarations here */


#endif /* BCDS_Wlan_H */

/** @} */

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
 */

/* header definition ******************************************************** */
#ifndef NETWORKCONFIG_H_
#define NETWORKCONFIG_H_

/* local interface declaration ********************************************** */

/* local type and macro definitions */
#define NETWORKCONFIG_FAILURE                 INT32_C(-1) /**< Macro for defining -1*/
#define NETWORKCONFIG_SUCCESS                 INT32_C(0) /**< Macro for defining 0*/
#define NETWORKCONFIG_ZERO                    INT32_C(0) /**< Macro for defining 0*/
#define NETWORKCONFIG_ONE                     UINT8_C(1) /**< Macro for defining 1*/
#define NETWORKCONFIG_TIMEOUT                 UINT16_C(0xFF) /**< Macro for defining timeout*/
/* local function prototype declarations */
/**
 * @brief         Function to set the IP status.
 *
 * @param [in]    Status of IP that needs to be set
 */
void NetworkConfig_SetIpStatus(NetworkConfig_IpStatus_T ipStatus);

/* local module global variable declarations */

/* public global variable declarations */

/* local inline function definitions */

#endif /* NETWORKCONFIG_H_ */

/** ************************************************************************* */

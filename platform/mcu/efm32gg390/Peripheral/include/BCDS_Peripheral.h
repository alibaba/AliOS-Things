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
 * @defgroup PERIPHERAL Peripherals
 * @{
 *
 * @brief Peripheral drivers
 *
 * @details
 *
 * @file
 **/

#ifndef BCDS_PERIPHERAL_H_
#define BCDS_PERIPHERAL_H_

/* interface dependency checks */
#include "BCDS_Retcode.h"

/**
 * @brief Enumeration to represent the return codes of Utils module.
 */
enum Peripherals_Retcode_E
{
    RETCODE_DOUBLE_INITIALIZATION = RETCODE_FIRST_CUSTOM_CODE,
    RETCODE_DEINITIALIZE_FAILED,
    RETCODE_MAX_ERROR,
};

/* Put the type and macro definitions here */
/**
 * @brief BCDS_MODULE_ID for each Utils C module
 * @details Usage:
 * @code{.c}
 * #undef BCDS_MODULE_ID
 * #define BCDS_MODULE_ID BCDS_UTILS_MODULE_ID_xxx
 * @endcode
 */
enum BCDS_Peripherals_ModuleID_E
{
    BCDS_UTILS_MODULE_ID_PERIPHERALDRIVERALIOS = 1,
};

#endif /* BCDS_PERIPHERAL_H_ */
/**@} */

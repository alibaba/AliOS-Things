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
 * @ingroup BCDS_HAL_MCU_IF
 * @defgroup BCDS_HAL_MCU_SLEEP  MCU Sleep API
 *
 * @{
 * @brief Micro-Controller Sleep API
 *
 * @details This is the interface to common MCU sleep management functionality
 *
 * The invoking application shall be able to block the system from entering
 * into certain energy modes.
 *
 * Sleep modes can be blocked or unblocked by a nested counting semaphore.
 * A block operation increases the counter by one element,
 * unblocking will decrease this.
 *
 * The MCU_Sleep_GoSleep will go into the lowest/deepest acceptable sleep mode.
 *
 * Pre-sleep and post-sleep callbacks can be registered in order to
 * do the last preparations from the application/BSP perspective before and after sleeping
 *
 * Such callbacks might include pin-settings configuration, turning interrupts on/off, etc.
 *
 * @warning Only power-saving modes which retain CPU and RAM are
 *      considered here as a sleep mode. This excludes some shutdown-modes!
 *
 * @file
 *
 */

#ifndef BCDS_MCU_SLEEP_H
#define BCDS_MCU_SLEEP_H

#include "BCDS_HAL.h"
#include "BCDS_HALConfig.h"

#if( BCDS_FEATURE_SLEEP )

/**
 * @brief Initialize MCU Sleep Management
 *
 * This function initializes the micro-controller sleep management
 *
 * @retval RETCODE_OK : Successful Execution
 *
 */
Retcode_T MCU_Sleep_Initialize(void);

/**
 * @brief Block Sleep Mode
 *
 * Blocks the sleep management from entering into the specified sleep mode
 *
 * @param[in] mode : Mode to block
 *
 * @retval RETCODE_OK : Successful Execution
 * @retval RETCODE_INVALID_PARAM : Tried to block an unsupported mode
 *
 */
Retcode_T MCU_Sleep_BlockSleepMode(uint32_t mode);

/**
 * @brief Unblock a sleep mode
 *
 * Removes one blockade in the sleep management from entering into the specified sleep mode
 *
 * @param[in] mode : Mode to unblock
 *
 * @retval RETCODE_OK : Successful Execution
 * @retval RETCODE_INVALID_PARAM : Tried to unblock an unsupported mode
 *
 */
Retcode_T MCU_Sleep_UnblockSleepMode(uint32_t mode);

/**
 * @brief Get Lowest Sleep Mode
 *
 * Returns the lowest possible sleep mode to use.
 * Mind that only power-saving modes which retain CPU and RAM are considered here as sleep modes.
 *
 * @returns Lowest Sleep Mode Available
 *
 */
uint32_t MCU_Sleep_GetLowestSleepMode(void);

/**
 * @brief MCU Sleep
 *
 * Enters the micro-controller into the lowest possible sleep mode.
 * This function returns after the MCU is awake again.
 *
 * @param[in] mode : Sleep mode to be entered
 *
 */
void MCU_Sleep_EnterSleep(uint32_t mode);

#endif /* BCDS_FEATURE_MCU_SLEEP */
#endif /* BCDS_MCU_SLEEP_H */

/** @} */

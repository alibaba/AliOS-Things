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
 *  @ingroup  BCDS_HAL_BSP_IF
 *
 *  @defgroup BCDS_HAL_BSP_Board Board
 *
 *  @{
 *  @brief Generic interface to the Board API
 *
 * @details The following taxonomy has been adopted:
 *
 *   - Board     : A board is an electrical system equipped with processing
 *                 capabilities and containing different interrelated electrical
 *                 components to serve for a defined mission.
 *
 *   - MCU       : Main/Micro Controller Unit is the component inside a board
 *                 responsible for controlling and correlating the different
 *                 board components.
 *
 *   - Peripheral: Any board component that is not the MCU and that is meant to
 *                 establish a connection  to the MCU in order to serve for the
 *                 board mission.
 *
 * After the MCU has executed its startup sequence and pointed to the main
 * function the application shall call directly BSP_Board_Initialize(param1, param2).
 *
 *  @file
 */

#ifndef BCDS_BSP_Board_H
#define BCDS_BSP_Board_H

#include "BCDS_HAL.h"



/**
 * Type definition for the SysTick callback
 * @note If an operating system is in use it may needs to perform some
 * operations each system clock tick, this callback has to be provided by the
 * application through BSP_Board_OSTickInitialize() function.
 */
typedef void (*BSP_Systick_Callback_T)(void);

/**
 * Type definition for the delay function which is passed from the OS to the BSP in order to
 * replace the blocking delay function.
 *
 * @param uint32_t expected delay parameter type.
 */
typedef void (*OStoBSPDelayFunc_T)(uint32_t);


/**
 * @brief Initializes the entire board.
 *
 * @details This function is responsible of setting the initial configuration
 * at the MCU and board level in order to establish a running state and perform
 * basic operations.
 *
 * The MCU resources are at this time configured according to their use cases
 * and shut-down in order to maintain a low power consumption. As an example
 * for a UART interface the BSP_Board_Initialize(param1, param2) will set the baudrate, the
 * parity, the number of data bits, the number of stop bits, etc. and then
 * disables the UART and turns off its clock.
 *
 * @param param1 : Initialization parameter depending on the implementation, can be used to indicate type of param2.
 * @param param2 : Initialization parameter depending on the implementation.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Board_Initialize(uint32_t param1, void* param2);

/**
 * @brief Switches between different power modes.
 *
 * @details This function is responsible of preparing switching between the
 * available MCU power modes if the transition from the current mode to the next
 * mode is allowed.
 * Depending on the selected mode this function implements the recovery routines
 * in order to bring the system to the state before it switched to that power
 * mode.
 * @param powerMode: 32 bit value designating the desired power mode according to what is
 *  specified in the implementation under BSP_BoardType.h
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Board_SwitchPowerMode(uint32_t powerMode);

/**
 * @brief Returns the last wake-up source
 *
 * @details This function will return the last event that resulted in the wake-up of the
 * MCU from a sleep/deep-sleep state.
 *
 * @return The latest wake-up event source.
 */
uint32_t BSP_Board_GetWakeupSource(void);

/**
 * @brief Returns the power-supply mode
 *
 * @details This function detects whether the MCU is being supplied using a
 * battery or using an external power supply.
 *
 * @return the current power supply type.
 */
uint32_t BSP_Board_DetectPowerSupply(void);

/**
 * @brief Returns the BSP Revision.
 *
 * @return   BSP Revision.
 */
uint32_t BSP_Board_GetBoardRevision(void);

/**
 * @brief Controls the on-board power rails.
 * @details: provided that there are controllable power rails on the board,
 * the BSP will provide the necessary commands that are possible to have in
 * order to control them safely.
 *
 * @param control: the desired power rail command implemented by the BSP.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Board_ControlPowerRail(uint32_t control);

/**
 * @brief Initializes pre/post tick increment processing in OS based applications.
 *
 * @details If an operating system needs to perform some operations before and/or after
 * the system tick count is incremented the application needs to call this
 * function and provide the callbacks which are called before and after the tick
 * increment.
 * @param preCallback: A pointer to the function which the system needs to
 * execute before incrementing the tick count of the system tick.
 * @note If no pre-processing is needed provide NULL.
 * @param postCallback: A pointer that points to the function which the system
 * needs to execute after incrementing the tick count of the system tick.
 * @note If no post-processing is needed provide NULL.
 */
void BSP_Board_OSTickInitialize(BSP_Systick_Callback_T preCallback, BSP_Systick_Callback_T postCallback);

/**
 * @brief Provide accurate delay  based on variable incremented.
 *
 * @param delay: specifies the delay time
 *
 * @note Refer to your implementation for time granularity details.
 */
void BSP_Board_Delay(uint32_t delay);



/**
 * @brief Provide accurate delay  based on variable incremented in mikro seconds.
 *
 * @param[in] delayInUs: specifies the delay time in us
 *
 * @note Refer to your implementation for time granularity details.
 */
void BSP_Board_DelayUs(uint32_t delayInUs);


/**
 * @brief Provide a timestamp based on variable incremented in milli seconds.
 *
 * @note Refer to your implementation for time granularity details.
 */
uint64_t BSP_Board_GetTick(void);

/**
 * @brief Passes  delay function from OS to BSP.
 * @details If the application needs to replace the blocking delays greater than
 * 50ms with non blocking delays it has to assign the delay function to this
 * function pointer.
 *
 * In general the delays implemented have 1 ms granularity.
 *
 * @param delayFunc: delay function.
 *
 */
void BSP_Board_SetOStoBSPDelayFunc(OStoBSPDelayFunc_T delayFunc);

/**
 * @brief Enable the Core SysTick
 *
 * @retval RETCODE_OK if success
 * @retval RETCODE_FAILURE if failure
 */
Retcode_T BSP_Board_EnableSysTick(void);

/**
 * @brief Disable the Core SysTick
 */
void BSP_Board_DisableSysTick(void);

/**
 * @brief Invoke Board Soft Reset. This puts the MCU registers to appropriate state applicable for soft reset.
 *        Please note this is not equivalent to hard reset.
 */
void BSP_Board_SoftReset(void);

#endif /* BCDS_BSP_Board_H */

/**  @} */

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
 * @{
 * @brief Provide functions for different delay times.
 * @details
 * By default the Delay Functions is implemented with a aktive blocking delay .
 *
 * The us Delay is always a active waiting function, who needs
 * a BSP_Board_DelayUs function who must implemented in the BSP.
 * The implemented function for Delay ms will use this Function so
 *
 * Using the function HAL_Delay_SetMsHook(), an RTOS Delay for ms
 * implementation can be hooked in if needed to replace the HAL implementation.
 *
 * @file BCDS_HAL_Delay.h
 *
 */

#ifndef BCDS_HAL_DELAY_H_
#define BCDS_HAL_DELAY_H_


#include "BCDS_Retcode.h"

/** Function data type for to handle different os Functions by setting a new Function*/
typedef Retcode_T (*HAL_Delay_MsHook_T)(uint32_t timeInMs);



/**
 * @brief Set Delay ms Mode for use in NoOs or Rtos Context
 *
 * @param[in] hook : Pointer to new Delay ms Function
 * @retval #RETCODE_OK when successful
 * @retval #RETCODE_NULL_POINTER when any one of the param has been NULL
 */
Retcode_T HAL_Delay_SetMsHook( HAL_Delay_MsHook_T hook);


/**
 * @brief  Delay/wait in us
 *
 * This is just a wrapper for the BSP_Board_DelayUs() function.
 *
 * @param[in] timeInUs : value in us to aktive wait (in noOs and in Rtos Mode )
 * @retval RETCODE_OK when successful
 */
Retcode_T HAL_Delay_WaitUs(uint32_t timeInUs);

/**
 * @brief  Delay/wait in ms
 *
 * By default this function is wrapper for the BSP_Board_Delay() function. It is implements
 * a busy wait. Using the function HAL_Delay_SetMsHook(), an RTOS delay function can be
 * hooked in if needed to replace the busy wait with a proper RTOS delay mechanism.
 *
 * @param[in] timeInMs : value in ms to wait (active wait in noOs and task delay in Rtos Mode )
 * @retval #RETCODE_OK when successful
 * @retval #RETCODE_NULL_POINTER when HAL_Delay_SetMsHook() is not initialized.
 */

Retcode_T HAL_Delay_WaitMs(uint32_t timeInMs);


#endif /* BCDS_HAL_DELAY_H_ */
/** @} */

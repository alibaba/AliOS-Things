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
 * @ingroup BCDS_HAL_MCU_IF
 * @defgroup BCDS_HAL_MCU_TIMER Timer

 * @{
 *  @brief Unified HAL API for Timer hardware
 *
 * @details Use this component to control the timer hardware peripheral
 * The timer is being set in the BSP including clock setup
 * The application will get a driver handle by the BSP in order to control, start, stop the timer
 *
 * Before using the timer it has to be initialized by a call to MCU_TIMER_Initialize
 *
 * @code{.c}
 * Retcode_T retcode = MCU_TIMER_Initialize(timer0, MyTimer0EventCallback);
 * if(RETCODE_OK != retcode)
 * {
 *      // handle the error
 * }
 * else
 * {
 *     // use the timer
 * }
 * @endcode
 *
 * Once the timer is successfully initialized, it can be used
 *
 * @note It is possible to disable the entire TIMER feature in the HAL-BSP
 * configuration file. i.e. the TIMER code will only be compiled and linked if
 * the feature <b>BCDS_FEATURE_TIMER</b> is set to 1 in the configuration
 * file.
 *
 * @file
 *
 * ****************************************************************************/

#ifndef BCDS_MCU_Timer_H
#define BCDS_MCU_Timer_H

#include "BCDS_Essentials.h"
#include "BCDS_HAL.h"

#if( BCDS_FEATURE_TIMER )

/**
 * Timer Status
 */
enum MCU_Timer_Status_E
{
    MCU_TIMER_STATUS_INVALID, /**< Invalid state */
    MCU_TIMER_STATUS_BSP_INITIALIZED, /**< Timer has been initialized by the BSP */
    MCU_TIMER_STATUS_INITIALIZED, /**< Timer was initialized by the driver */
    MCU_TIMER_STATUS_ACTIVE, /**< Timer is running */
    MCU_TIMER_STATUS_IDLE, /**< Timer is idle */
    MCU_TIMER_STATUS_DEINITIALIZED, /**<  Timer has been deinitialized */
    MCU_TIMER_STATUS_ERROR, /**< Timer driver is in error state */
};

typedef enum MCU_Timer_Status_E MCU_Timer_Status_T;

/* Put the type and macro definitions here */
/**
 * @brief Typedef for the Timer handler type which is used to identify the
 * Timer component to work with.
 */
typedef HWHandle_T Timer_T;

/**
 * @brief Structure to represent the events that can be received from the Timer
 * in the callback function.
 */
struct MCU_Timer_Event_S
{

    uint8_t CompareMatch :1; /**< A compare match has been detected on the compare channel. */
    uint8_t Overflow :1; /**< An overflow has been detected */
    uint8_t Underflow :1; /**< Underflow was detected */
    uint8_t reserved :5;
    uint32_t CompareChannel :8; /**< Which compare channel the event is related to. */
    uint32_t ErrorCode :16; /**< an error has been detected */
};

/**
 * @brief This data type represents a function pointer which is used for timers as
 * a callback whenever an event is to be notified to the timer consumer
 *
 * @details Upon initialization of the timer module the user should pass a function pointer
 * of this type whenever a event based callback is expected (timer not used in polling mode)
 *
 * @param [in] event : Structure containing current event information
 * @see MCU_Timer_Initialize()
 */
typedef void ( * MCU_TIMER_Callback_T )(Timer_T timer, struct MCU_Timer_Event_S event);

/* Put the function declarations here */

/**
 * @brief This function initializes the timer and registers an event callback
 *
 * @details The timer's clock base and settings are configure by the BSP
 * After calling this function the timer is not yet active
 *
 * @param[in] timer : the timer reference/handle (provided by the BSP)
 * @retval RETCODE_OK when successful and a custom error code otherwise
 */
Retcode_T MCU_Timer_Initialize(Timer_T timer, MCU_TIMER_Callback_T callback);

/**
 * @brief This function de-initializes the timer and de-registers the event callback
 *
 * @note after having de-initialized the timer all calls to the timer API will return an error
 * @retval RETCODE_OK when successful and a custom error code otherwise
 */
Retcode_T MCU_Timer_Deinitialize(Timer_T timer);

/**
 * @brief This function is a convenience function to set the compare value for a specific channel
 *
 * @retval RETCODE_OK when successful and a custom error code otherwise
 */
Retcode_T MCU_Timer_SetCompareValue(Timer_T timer, uint32_t channel, uint32_t value);

/**
 * @brief This function is a convenience function to get the compare value for a specific channel
 *
 * @retval RETCODE_OK when successful and a custom error code otherwise
 */
Retcode_T MCU_Timer_GetCompareValue(Timer_T timer, uint32_t channel, uint32_t *value);

/**
 * @brief This function is a convenience function to get the current count value of the timer
 *
 * @param[in] timer : the timer reference
 *
 * @param[out] countValue : Current count value of timer
 *
 * @retval RETCODE_OK when successful and a custom error code otherwise
 */
Retcode_T MCU_Timer_GetCountValue(Timer_T timer, uint32_t * countValue);

/**
 * @brief This function returns the maximum count value possible by the HW
 *
 * @param[in] timer : the timer reference
 *
 * @param[out] maxCount : Maximum count value supported by the timer
 */
Retcode_T MCU_Timer_GetMaxCount(Timer_T timer, uint32_t * maxCount);

/**
 * @brief This function will start the timer (set to active state)
 *
 * @param[in] timer: the timer handle to be started
 *
 * @retval RETCODE_OK when successful and a custom error code otherwise
 */
Retcode_T MCU_Timer_Start(Timer_T timer);

/**
 * @brief This function is a generic function to get a specific property of the timer
 *
 * @param[in] timer: the timer handle to be stopped
 *
 * @retval RETCODE_OK when successful and a custom error code otherwise
 */
Retcode_T MCU_Timer_Stop(Timer_T timer);

/**
 * @brief This function returns the status of the timer driver
 *
 * @param[in] timer: the timer handle to be queried
 *
 * @returns a state code indicating current driver state
 *
 */
MCU_Timer_Status_T MCU_Timer_GetStatus(Timer_T timer);

/**
 * @brief This function is called by the Interrupt handler
 *
 * @param[in] timer: the timer handle
 *
 */
void MCU_Timer_Callback(Timer_T timer);

/**
 * @brief Get the Timer Clock Tick frequency
 *
 * @param[in] timer : the timer reference
 *
 * @param[out] frequency : Current value of clock frequency in Hz
 *
 * @retval RETCODE_OK when successful and a custom error code otherwise
 */
Retcode_T MCU_Timer_GetClockFrequency(Timer_T timer, uint32_t * frequency);

#endif /* BCDS_MCU_Timer_H */

#endif /* BCDS_FEATURE_TIMER */
/** @} */


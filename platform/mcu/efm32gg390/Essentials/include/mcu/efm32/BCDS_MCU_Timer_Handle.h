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
 *
 * @brief Timer Peripheral handle definitions for EFM32
 *
 */

#ifndef BCDS_MCU_TIMER_HANDLE_H
#define BCDS_MCU_TIMER_HANDLE_H

#if(BCDS_FEATURE_TIMER)

#include "em_device.h"

#include "em_cmu.h"

#if defined(RTC_PRESENT) && (RTC_COUNT > 0)
#ifndef BCDS_FEATURE_EFM32_RTC
#define BCDS_FEATURE_EFM32_RTC 1
#endif
#endif

#if defined(BURTC_PRESENT) && (BURTC_COUNT > 0)
#ifndef BCDS_FEATURE_EFM32_BURTC
#define BCDS_FEATURE_EFM32_BURTC 1
#endif
#endif

#if defined(TIMER_PRESENT) && (TIMER_COUNT > 0)
#ifndef BCDS_FEATURE_EFM32_TIMER
#define BCDS_FEATURE_EFM32_TIMER 1
#endif
#endif

#if defined(LETIMER_PRESENT) && (LETIMER_COUNT > 0)
#ifndef BCDS_FEATURE_EFM32_LETIMER
#define BCDS_FEATURE_EFM32_LETIMER 1
#endif
#endif

#if (BCDS_FEATURE_EFM32_RTC)
#include "em_rtc.h"
#endif

#if (BCDS_FEATURE_EFM32_BURTC)
#include "em_burtc.h"
#endif

#if (BCDS_FEATURE_EFM32_TIMER)
#include "em_timer.h"
#endif

#if (BCDS_FEATURE_EFM32_LETIMER)
#include "em_letimer.h"
#endif

/**
 * Timer Module types supported by EFM32
 */
enum Hal_MCU_Timer_E
{
    EM_RTC, /**< Real Time Clock  */
    EM_BURTC, /**< Back-up Real Time Clock  */
    EM_TIMER, /**< Standard Timer */
    EM_LETIMER, /**< Low Energy Timer */
};

/**
 * Timer Error Status
 */
enum MCU_Timer_ErrorStatus_E
{
    TIMER_NO_ERROR,
    TIMER_ERROR_NOT_AVAILABLE,
    TIMER_LFXCO_FAILURE,
    TIMER_INPUT_BUFFER_OVERFLOW,
};

/**
 * Timer Initialization Handle
 */
union HAL_MCU_initHandle_U
{
    const void * None; /**< Generic Handle */

#if (BCDS_FEATURE_EFM32_RTC)
    const RTC_Init_TypeDef * Rtc; /**< Initialization Structure Handle for RTC */
#endif

#if (BCDS_FEATURE_EFM32_BURTC)
    const BURTC_Init_TypeDef * Burtc; /**< Initialization Structure Handle for BURTC */
#endif

#if (BCDS_FEATURE_EFM32_TIMER)
    const TIMER_Init_TypeDef * Timer; /**< Initialization Structure Handle for TIMER */
#endif

#if (BCDS_FEATURE_EFM32_LETIMER)
    const LETIMER_Init_TypeDef * Letimer; /**< Initialization Structure Handle for LETIMER */
#endif
};

/**
 * Timer HW Low Level Handle
 * Constant Pointer to HW
 */
union HAL_MCU_hwHandle_U
{
    void * const None; /**< Generic Timer Handle */

#if (BCDS_FEATURE_EFM32_RTC)
    RTC_TypeDef * const Rtc; /**< RTC Timer Handle  */
#endif

#if (BCDS_FEATURE_EFM32_BURTC)
    BURTC_TypeDef * const Burtc; /* BURTC Timer Handle */
#endif

#if (BCDS_FEATURE_EFM32_TIMER)
    TIMER_TypeDef * const Timer; /**< Timer Low Level Handle  */
#endif

#if (BCDS_FEATURE_EFM32_LETIMER)
    LETIMER_TypeDef * const Letimer; /**< Low-Energy Timer Low Level Handle*/
#endif

};

/**
 * EFM32 Specific Timer Handle
 */
struct MCU_Timer_Handle_S
{
    union HAL_MCU_hwHandle_U HwHandle; /**< HW Handle pointer */

    const enum Hal_MCU_Timer_E HwType; /**< Type of Timer, instance ID  */

    union HAL_MCU_initHandle_U InitHandle; /**< HW Initialization Handle */
    union HAL_MCU_initHandle_U DeInitHandle; /**< HW DeInitialization Handle */

    enum MCU_Timer_Status_E Status; /**< Current state of the timer as tracked by the SW */

    MCU_TIMER_Callback_T Callback; /**< Callback for timer */
};

#if(BCDS_FEATURE_EFM32_BURTC)

/**
 * Returns the Handle to the BURTC
 */
HWHandle_T MCU_Timer_GetBurtcHandle(void);

/**
 * Handle the BURTC peripheral IRQ
 */
void MCU_Timer_HandleBurtcIrq(void);

#endif

#if(BCDS_FEATURE_EFM32_RTC)

/**
 * Returns the Handle to the RTC
 */
HWHandle_T MCU_Timer_GetRtcHandle(void);

/**
 * Handle the RTC peripheral IRQ
 */
void MCU_Timer_HandleRtcIrq(void);
#endif

#endif /* BCDS_FEATURE_TIMER */
#endif /* BCDS_MCU_TIMER_HANDLE_H */


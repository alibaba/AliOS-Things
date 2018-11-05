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
 *  @ingroup BCDS_HAL_MCU_IF
 *
 *  @defgroup BCDS_HAL_MCU_Watchdog Watchdog

 * @{
 * @brief Unified watchdog API for CoSP (Connected Software Platform)
 *
 * @details Application must initialize the watchdog timer with timeout.
 * After which application must enable and feed the watchdog.
 * The watchdog component also provides a function to determine
 * the reset cause after a program reset.
 *
 * @code
 * #include "BCDS_MCU_Watchdog.h
 *
 * MCU_WatchdogInit_T wdgInitStruct;
 * wdgInitStruct.WdgCallback = NULL;
 * wdgInitStruct.ResetMode = WATCHDOG_RESET_ON;
 * wdgInitStruct.RunOnCpuHalt = true;
 * wdgInitStruct.Timeout = 6002;
 *
 * wdgHandle = (WdgHandle_T)&wdgInitStruct;
 * retcode = MCU_Watchdog_Init(wdgHandle);
 * if(RETCODE_OK == retcode)
 * {
 *      // enable the watchdog
 *      retcode = MCU_Watchdog_Enable();
 * }
 * if(RETCODE_OK == retcode)
 * {
 *      // Feed the watchdog
 *      retcode = MCU_Watchdog_Feed();
 * }
 * @endcode
 *
 * @note It is possible to disable the entire watchdog feature in the HAL-BSP
 * configuration file. I.e. the watchdog code will only be compiled and linked if
 * the feature <b>BCDS_FEATURE_WATCHDOG</b> is set to 1 in the configuration
 * file.
 *
 *  @file
 *
 * ****************************************************************************/
#ifndef BCDS_MCU_WATCHDOG_H
#define BCDS_MCU_WATCHDOG_H

#include "BCDS_HAL.h"

/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_WATCHDOG

#include "BCDS_Basics.h"

/**
 * @brief enumeration of watchdog timer error return codes
 */
enum RETCODE_ESSENTIALS_WATCHDOG_E
{
    RETCODE_OPTION_NOT_AVAILABLE = RETCODE_FIRST_CUSTOM_CODE,
};

/**
 *  @brief      Watchdog callback pointer
 *
 *  This is the typedef that will allow a callback function to be specified in the MCU_WatchdogInit_S structure
 *  which provides interrupt to the application.
 */
typedef void (*Watchdog_Callback)(uint32_t, void *);

/**
 *  @brief      Watchdog reset mode settings
 *
 *  This enumeration defines the reset modes for the Watchdog. The Watchdog can
 *  be configured to either generate a reset upon timeout or simply produce a
 *  periodic interrupt.
 */
typedef enum Watchdog_ResetMode_E {
    WATCHDOG_RESET_ON,   /**< Generates reset after timeout */
    WATCHDOG_RESET_OFF   /**< Timeouts generate interrupts only */
} Watchdog_ResetMode_T;

/**
 *  @brief    Watchdog timer init structure
 *  @details  Watchdog timer structure is passed as argument to MCU_Watchdog_Init() function.
 *
 *  @param[in] Callback             Watchdog_Callback
 *
 *  @param[in] ResetMode            Watchdog_ResetMode_E. By default watchdog will trigger a reset
 *
 *  @param[in] RunOnCpuHalt         On some targets, the Watchdog timer will continue to count down while
 *                                  a debugging session is halted. To avoid unwanted resets, the Watchdog
 *                                  can be set to stall while the processor is stopped by the debugger.
 *                                  This option might be not available for some targets.
 *                                  By default the watchdog will be stopped on cpu halt.
 *
 *  @param[in,out] Timeout          Watchdog timeout is configured in milliseconds.
 *                                  Refer respective mcu manuals for watchdog timeout range supported.
 *                                  Internal 1 kHz RC oscillator will be used for efm32 mcu.
 */
typedef struct MCU_WatchdogInit_S {
    Watchdog_Callback WdgCallback;        /**< Pointer to callback upon watchdog interrupt */
    Watchdog_ResetMode_T  ResetMode;      /**< Mode to enable resets */
    bool RunOnCpuHalt;                    /**< If set to false when in debug mode, watchdog timer will be frozen whenever the cpu core is halted e.g for breakpoint. */
    uint32_t Timeout;                     /**< Watchdog timeout in milliseconds */
}MCU_WatchdogInit_T;


/**
 * @brief   Data type for the WATCHdOG handler type which is used to identify the
 * WATCHdOG component to work with.
 * @note: This data type is kept for backward compatibility purposes with versions 1.x.x of essentials
 */
typedef HWHandle_T WdgHandle_T;

/**
 * @brief Initialize watchdog timer
 *
 * @details Initialize the watchdog timer with MCU_WatchdogInit_S structure
 * @param[in] wdgHandle The implementation of a watchdog component shall rely on typecasting a reference to MCU_WatchdogInit_T variable to
 *  WdgHandle_T.
 * @note Some targets have fixed values for watchdog timeout therefore the application shall write the Timeout member of the MCU_WatchdogInit_S
 * and read it back after calling MCU_Watchdog_Init() in order to get the real Timeout value.
 *
 * @retval  RETCODE_OK upon successful init of the watchdog
 * @retval  RETCODE_UNINITIALIZED in case wdgInitStruct is NULL
 * @retval  RETCODE_INVALID_PARAM in case watchdog Timeout is out of range supported by mcu
 */
Retcode_T MCU_Watchdog_Init(WdgHandle_T wdgHandle);

/**
 * @brief Enable watchdog timer
 *
 * @details After watchdog initialization,watchdog can be enabled using this function.
 * @retval  RETCODE_OK upon successful enabling of the watchdog.
 * @retval  RETCODE_UNINITIALIZED in case of failure
 */
Retcode_T MCU_Watchdog_Enable(void);

/**
 * @brief Feed watchdog timer
 *
 * @details This function is used to feed the watchdog within a certain
 * interval to prevent the watchdog from firing.
 * @retval  RETCODE_OK upon successful feeding
 * @retval  RETCODE_UNINITIALIZED in case of failure
 */
Retcode_T MCU_Watchdog_Feed(void);

/**
 * @brief Provides reset cause.
 *
 * @details This function checks whether the last system reset was caused by the watchdog.
 * If reset was due to watchdog, reset cause is cleared.
 * @retval true if the last system reset was caused by the watchdog.
 * @retval false if the last system reset was not caused by the watchdog.
 */
bool MCU_Watchdog_IsResetCausedByWatchdog(void);


#endif /* BCDS_FEATURE_WATCHDOG */
#endif /* BCDS_MCU_WATCHDOG_H */
/**  @} */

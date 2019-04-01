#ifndef _HAL_TM_H_
#define _HAL_TM_H_

#include <stdint.h>
#include <stddef.h>

#include "sys/intc.h"   // components/sys/

typedef enum _TM_ID_E {
    TM_TU0_US   = 0,                ///< ID for microsecond timer 0.
    TM_TU1_US   = 1,                ///< ID for microsecond timer 1.
    TM_TU2_US   = 2,                ///< ID for microsecond timer 2.
    TM_TU3_US   = 3,                ///< ID for microsecond timer 3.
    TM_TM0_MS   = 4,                ///< ID for millisecond timer 4.
    TM_TM1_MS   = 5,                ///< ID for millisecond timer 5.
    TM_TM2_MS   = 6,                ///< ID for millisecond timer 6.
    TM_TM3_MS   = 7,                ///< ID for millisecond timer 7.
    TM_MAX      = 8                 ///< Maximum total ID of timers.
} TM_ID_E;

typedef enum _TM_MODE_E {
    TM_MODE_ONE_SHOT    = 0,        ///< Countdown to 0 then stop.
    TM_MODE_AUTO_RELOAD = 1         ///< Countdown to 0 then auto reload the timer value and continue counting.
} TM_MODE_E;

/**
 * Registers an interrupt handler to be called when a given timer interrupt occurs.
 *
 * If tm_irq_handler is NULL, then the timer interrupt is disabled.
 *
 * @param   tm_id                   Select a microsecond or millisecond timer ID.
 * @param   tm_irq_handler          A pointer to an interrupt handler that is called when a timer interrupt occurs.
 *                                  NULL to disable interrupt.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @retval   0                      The handler for the timer interrupt was successfully installed.
 */
int32_t
hal_tm_register_irq_handler (
    TM_ID_E     tm_id,
    isr_func    tm_irq_handler
    );

/**
 * This service clears the status that indicates the timer interrupt.
 *
 * @param   tm_id                   Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @retval   0                      The operation completed successfully.
 */
int32_t
hal_tm_clear_int_sts (
    TM_ID_E     tm_id
    );

/**
 * Initialize the timer hardware.
 *
 * @param   tm_id                   Select a microsecond or millisecond timer ID.
 *
 * @retval  -17                     The clock source is no mapping.
 * @retval   -2                     The timer ID was incorrect.
 * @retval    0                     The operation completed successfully.
 */
int32_t
hal_tm_init (
    TM_ID_E     tm_id
    );

/**
 * Deinitialize the timer hardware.
 *
 * @param   tm_id                   Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @retval   0                      The operation completed successfully.
 */
int32_t
hal_tm_deinit (
    TM_ID_E     tm_id
    );

/**
 * This service begins the timer countdown.
 *
 * If tm_init_value is 0, then the timer is disabled.
 *
 * @param   tm_id                   Select a microsecond or millisecond timer ID.
 * @param   tm_mode                 Countdown to 0 then stop or auto reload the timer value.
 * @param   tm_init_value           The timer initial value.
 *                                  If tm_init_value is 0, then the timer is disabled.
 *                                  Contains at least the [0, 65535] range.
 *
 * @retval  -20                     The timer has already been started.
 * @retval   -2                     The timer ID was incorrect.
 * @retval    0                     The operation completed successfully.
 */
int32_t
hal_tm_enable (
    TM_ID_E     tm_id,
    TM_MODE_E   tm_mode,
    uint32_t    tm_init_value
    );

/**
 * This service disables the timer countdown.
 *
 * @param   tm_id                   Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @retval   0                      The operation completed successfully.
 */
int32_t
hal_tm_disable (
    TM_ID_E     tm_id
    );

/**
 * The timer software reset.
 *
 * Write 1 to trigger a pulse to reset the whole timer.
 *
 * @param   tm_id                   Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @retval   0                      The operation completed successfully.
 */
int32_t
hal_tm_sw_rst (
    TM_ID_E     tm_id
    );

/**
 * This service reads the timer begin countdown value.
 *
 * @param   tm_id                   Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @return  Returns the timer begin countdown value.
 */
int32_t
hal_tm_get_init_value (
    TM_ID_E     tm_id
    );

/**
 * This service reads the timer mode value.
 *
 * @param   tm_id                   Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @retval   1                      The timer mode was periodic.
 * @retval   0                      The timer mode was one-shot.
 */
int32_t
hal_tm_get_mode (
    TM_ID_E     tm_id
    );

/**
 * This service reads the timer interrupt status value.
 *
 * @param   tm_id                   Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @retval   1                      The timer interrupt is pending.
 * @retval   0                      The timer interrupt is not pending.
 */
int32_t
hal_tm_get_int_sts (
    TM_ID_E     tm_id
    );

/**
 * This service reads the timer interrupt mask value.
 *
 * @param   tm_id                   Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @return  Returns the timer interrupt mask value.
 */
int32_t
hal_tm_get_int_mask (
    TM_ID_E     tm_id
    );

/**
 * This service reads the timer current countdown value.
 *
 * @param   tm_id                   Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @return  Returns the timer current countdown value.
 */
int32_t
hal_tm_get_cur_value (
    TM_ID_E     tm_id
    );

/**
 * This service reads the timer prescale value.
 *
 * @param   tm_id                   Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @return  Returns the timer prescale value.
 */
int32_t
hal_tm_get_prescale (
    TM_ID_E     tm_id
    );

/**
 * Delay for at least the request number of microseconds.
 *
 * @param   microseconds            The number of microseconds to delay.
 *
 * @retval  -14                     No timer that can be used to delay.
 * @retval    0                     The operation completed successfully.
 */
int32_t
hal_tm_us_delay (
    uint16_t    microseconds
    );

/**
 * Delay for at least the request number of milliseconds.
 *
 * @param   milliseconds            The number of milliseconds to delay.
 *
 * @retval  -14                     No timer that can be used to delay.
 * @retval    0                     The operation completed successfully.
 */
int32_t
hal_tm_ms_delay (
    uint16_t    milliseconds
    );

#endif  // #ifndef _HAL_TM_H_

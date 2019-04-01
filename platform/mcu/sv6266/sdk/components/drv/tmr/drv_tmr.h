#ifndef _DRV_TMR_H_
#define _DRV_TMR_H_

#include "hal_tm.h"

/**
 * Registers an interrupt handler to be called when a given timer interrupt occurs.
 *
 * If tmr_irq_handler is NULL, then the timer interrupt is disabled.
 *
 * @param   tmr_id                  Select a microsecond or millisecond timer ID.
 * @param   tmr_irq_handler         A pointer to an interrupt handler that is called when a timer interrupt occurs.
 *                                  NULL to disable interrupt.
 *
 * @retval  -3                      The timer ID is not supported.
 * @retval  -2                      The timer ID was incorrect.
 * @retval   0                      The handler for the timer interrupt was successfully installed.
 */
int32_t
drv_tmr_register_irq_handler (
    TM_ID_E     tmr_id,
    isr_func    tmr_irq_handler
    );

/**
 * This service clears the status that indicates the timer interrupt.
 *
 * @param   tmr_id                  Select a microsecond or millisecond timer ID.
 *
 * @retval  -3                      The timer ID is not supported.
 * @retval  -2                      The timer ID was incorrect.
 * @retval   0                      The operation completed successfully.
 */
int32_t
drv_tmr_clear_interrupt_status (
    TM_ID_E     tmr_id
    );

/**
 * Initialize the timer hardware.
 *
 * @param   tmr_id                  Select a microsecond or millisecond timer ID.
 *
 * @retval  -17                     The clock source is no mapping.
 * @retval   -3                     The timer ID is not supported.
 * @retval   -2                     The timer ID was incorrect.
 * @retval    0                     The operation completed successfully.
 */
int32_t
drv_tmr_init (
    TM_ID_E     tmr_id
    );

/**
 * Deinitialize the timer hardware.
 *
 * @param   tmr_id                  Select a microsecond or millisecond timer ID.
 *
 * @retval  -3                      The timer ID is not supported.
 * @retval  -2                      The timer ID was incorrect.
 * @retval   0                      The operation completed successfully.
 */
int32_t
drv_tmr_deinit (
    TM_ID_E     tmr_id
    );

/**
 * This service begins the timer countdown.
 *
 * If tm_init_value is 0, then the timer is disabled.
 *
 * @param   tmr_id                  Select a microsecond or millisecond timer ID.
 * @param   tmr_mode                Countdown to 0 then stop or auto reload the timer value.
 * @param   tmr_init_count          The timer initial count.
 *                                  If tmr_init_count is 0, then the timer is disabled.
 *                                  Contains at least the [0, 65535] range.
 *
 * @retval  -20                     The timer has already been started.
 * @retval   -3                     The timer ID is not supported.
 * @retval   -2                     The timer ID was incorrect.
 * @retval    0                     The operation completed successfully.
 */
int32_t
drv_tmr_enable (
    TM_ID_E     tmr_id,
    TM_MODE_E   tmr_mode,
    uint16_t    tmr_init_count
    );

/**
 * This service disables the timer countdown.
 *
 * @param   tmr_id                  Select a microsecond or millisecond timer ID.
 *
 * @retval  -3                      The timer ID is not supported.
 * @retval  -2                      The timer ID was incorrect.
 * @retval   0                      The operation completed successfully.
 */
int32_t
drv_tmr_disable (
    TM_ID_E     tmr_id
    );

/**
 * The timer software reset.
 *
 * Write 1 to trigger a pulse to reset the whole timer.
 *
 * @param   tmr_id                  Select a microsecond or millisecond timer ID.
 *
 * @retval  -3                      The timer ID is not supported.
 * @retval  -2                      The timer ID was incorrect.
 * @retval   0                      The operation completed successfully.
 */
int32_t
drv_tmr_sw_rst (
    TM_ID_E     tmr_id
    );

/**
 * This service reads the timer begin countdown value.
 *
 * @param   tmr_id                  Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @return  Returns the timer begin countdown value.
 */
int32_t
drv_tmr_get_init_count (
    TM_ID_E     tmr_id
    );

/**
 * This service reads the timer mode value.
 *
 * @param   tmr_id                  Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @retval   1                      The timer mode was periodic.
 * @retval   0                      The timer mode was one-shot.
 */
int32_t
drv_tmr_get_mode (
    TM_ID_E     tmr_id
    );

/**
 * This service reads the timer interrupt status value.
 *
 * @param   tmr_id                  Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @retval   1                      The timer interrupt is pending.
 * @retval   0                      The timer interrupt is not pending.
 */
int32_t
drv_tmr_get_interrupt_status (
    TM_ID_E     tmr_id
    );

/**
 * This service reads the timer current countdown value.
 *
 * @param   tmr_id                  Select a microsecond or millisecond timer ID.
 *
 * @retval  -2                      The timer ID was incorrect.
 * @return  Returns the timer current countdown value.
 */
int32_t
drv_tmr_get_current_count (
    TM_ID_E     tmr_id
    );

#endif  // #ifndef _DRV_TMR_H_

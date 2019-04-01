#ifndef _HAL_WDT_H_
#define _HAL_WDT_H_

#include <stdint.h>
#include <stddef.h>

typedef enum _WDT_ID_E {
    MCU_WDT = 0,                    ///< ID for MCU watchdog timer.
    SYS_WDT = 1,                    ///< ID for SYS watchdog timer.
    MAX_WDT = 2                     ///< Maximum total ID of watchdog timers.
} WDT_ID_E;

/**
 * Prototype for the watchdog timer interrupt service routine.
 */
typedef
void
(*WDT_ISR) (
    void
    );

/**
 * Registers an interrupt service routine to be called from the watchdog timer interrupt handler.
 *
 * If wdt_int_cnt_ofs is 0, then the watchdog timer interrupt is disabled.
 * If wdt_isr is NULL, then the watchdog timer interrupt is disabled.
 *
 * @param   wdt_id                  Select a MCU or SYS watchdog timer ID.
 * @param   wdt_int_cnt_ofs         The watchdog interrupt time offset (in unit of ms).
 *                                  Contains at least the [0, 255] range.
 * @param   wdt_isr                 A pointer to an ISR function that is called when a watchdog timer interrupt handler occurs.
 *                                  NULL to disable interrupt.
 *
 * @retval  -2                      The watchdog timer ID was incorrect.
 * @retval   0                      The interrupt service routine was successfully installed.
 */
int32_t
hal_wdt_register_isr (
    WDT_ID_E    wdt_id,
    uint32_t    wdt_int_cnt_ofs,
    WDT_ISR     wdt_isr
    );

/**
 * Initialize the watchdog timer hardware.
 */
void
hal_wdt_init (
    void
    );

/**
 * Deinitialize the watchdog timer hardware.
 */
void
hal_wdt_deinit (
    void
    );

/**
 * This service begins the watchdog timer countdown.
 *
 * If the countdown completes prior to stop timer or restart timer the system will reset.
 * If wdt_time_cnt is 0, then the watchdog timer is disabled.
 *
 * @param   wdt_id                  Select a MCU or SYS watchdog timer ID.
 * @param   wdt_time_cnt            The amount of time in ms units to wait before the watchdog timer is fired.
 *                                  If wdt_time_cnt is 0, then the watchdog timer is disabled.
 *                                  Contains at least the [0, 65535] range.
 *
 * @retval  -2                      The watchdog timer ID was incorrect.
 * @retval   0                      The watchdog timer has been programmed to fire in time ms units.
 */
int32_t
hal_wdt_enable (
    WDT_ID_E    wdt_id,
    uint32_t    wdt_time_cnt
    );

/**
 * This service disables the watchdog timer countdown.
 *
 * @param   wdt_id                  Select a MCU or SYS watchdog timer ID.
 *
 * @retval  -2                      The watchdog timer ID was incorrect.
 * @retval   0                      The operation completed successfully.
 */
int32_t
hal_wdt_disable (
    WDT_ID_E    wdt_id
    );

/**
 * This service restarts the watchdog timer countdown and should only be called after the watchdog timer enable function.
 *
 * @param   wdt_id                  Select a MCU or SYS watchdog timer ID.
 *
 * @retval  -2                      The watchdog timer ID was incorrect.
 * @retval   0                      The operation completed successfully.
 */
int32_t
hal_wdt_kick (
    WDT_ID_E    wdt_id
    );

/**
 * This service sets the watchdog timer period.
 *
 * @param   wdt_id                  Select a MCU or SYS watchdog timer ID.
 * @param   wdt_time_cnt            Contains at least the [0, 65535] range.
 *
 * @retval  -2                      The watchdog timer ID was incorrect.
 * @retval   0                      The watchdog timer has been programmed to fire in time ms units.
 */
int32_t
hal_wdt_set_time_cnt (
    WDT_ID_E    wdt_id,
    uint32_t    wdt_time_cnt
    );

/**
 * The watchdog timer software reset.
 *
 * Write 1 to trigger a pulse to reset the whole watchdog timer.
 */
void
hal_wdt_sw_rst (
    void
    );

/**
 * This service reads the watchdog timer enable or disable value.
 *
 * @param   wdt_id                  Select a MCU or SYS watchdog timer ID.
 *
 * @retval  -2                      The watchdog timer ID was incorrect.
 * @return  Returns the watchdog enable/disable vaule.
 */
int32_t
hal_wdt_get_enable (
    WDT_ID_E    wdt_id
    );

/**
 * This service reads the watchdog interrupt time offset value.
 *
 * @param   wdt_id                  Select a MCU or SYS watchdog timer ID.
 *
 * @retval  -2                      The watchdog timer ID was incorrect.
 * @return  Returns the watchdog interrupt time offset value (in unit of ms).
 */
int32_t
hal_wdt_get_int_cnt_ofs (
    WDT_ID_E    wdt_id
    );

/**
 * This service reads the watchdog timer begin countdown value.
 *
 * @param   wdt_id                  Select a MCU or SYS watchdog timer ID.
 *
 * @retval  -2                      The watchdog timer ID was incorrect.
 * @return  Returns the watchdog timer begin countdown value (in unit of ms).
 */
int32_t
hal_wdt_get_time_cnt (
    WDT_ID_E    wdt_id
    );

/**
 * This service reads the watchdog timer reset status for the boot information.
 *
 * @param   wdt_id                  Select a MCU or SYS watchdog timer ID.
 *
 * @retval  -2                      The watchdog timer ID was incorrect.
 * @retval   1                      The watchdog timer reset occured.
 * @retval   0                      The watchdog timer no reset occured.
 */
int32_t
hal_wdt_get_reset (
    WDT_ID_E    wdt_id
    );

#endif  // #ifndef _HAL_WDT_H_

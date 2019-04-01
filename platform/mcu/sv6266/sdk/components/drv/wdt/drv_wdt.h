#ifndef _DRV_WDT_H_
#define _DRV_WDT_H_

#include "hal_wdt.h"

/**
 * Registers an interrupt service routine to be called from the watchdog timer interrupt handler.
 *
 * If wdt_int_count_offset is 0, then the watchdog timer interrupt is disabled.
 * If wdt_isr is NULL, then the watchdog timer interrupt is disabled.
 *
 * @param   wdt_id                  Select a MCU or SYS watchdog timer ID.
 * @param   wdt_int_count_offset    The watchdog interrupt time offset (in unit of ms).
 *                                  Contains at least the [0, 255] range.
 * @param   wdt_isr                 A pointer to an ISR function that is called when a watchdog timer interrupt handler occurs.
 *                                  NULL to disable interrupt.
 *
 * @retval  -2                      The watchdog timer ID was incorrect.
 * @retval   0                      The interrupt service routine was successfully installed.
 */
int32_t
drv_wdt_register_isr (
    WDT_ID_E    wdt_id,
    uint8_t     wdt_int_count_offset,
    WDT_ISR     wdt_isr
    );

/**
 * Initialize the watchdog timer hardware.
 */
void
drv_wdt_init (
    void
    );

/**
 * Deinitialize the watchdog timer hardware.
 */
void
drv_wdt_deinit (
    void
    );

/**
 * This service begins the watchdog timer countdown.
 *
 * If the countdown completes prior to stop timer or restart timer the system will reset.
 * If wdt_init_count is 0, then the watchdog timer is disabled.
 *
 * @param   wdt_id                  Select a MCU or SYS watchdog timer ID.
 * @param   wdt_init_count          The amount of time in ms units to wait before the watchdog timer is fired.
 *                                  If wdt_init_count is 0, then the watchdog timer is disabled.
 *                                  Contains at least the [0, 65535] range.
 *
 * @retval  -2                      The watchdog timer ID was incorrect.
 * @retval   0                      The watchdog timer has been programmed to fire in time ms units.
 */
int32_t
drv_wdt_enable (
    WDT_ID_E    wdt_id,
    uint16_t    wdt_init_count
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
drv_wdt_disable (
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
drv_wdt_kick (
    WDT_ID_E    wdt_id
    );

/**
 * This service sets the watchdog timer period.
 *
 * @param   wdt_id                  Select a MCU or SYS watchdog timer ID.
 * @param   wdt_init_count          Contains at least the [0, 65535] range.
 *
 * @retval  -2                      The watchdog timer ID was incorrect.
 * @retval   0                      The watchdog timer has been programmed to fire in time ms units.
 */
int32_t
drv_wdt_set_init_count (
    WDT_ID_E    wdt_id,
    uint16_t    wdt_init_count
    );

/**
 * The watchdog timer software reset.
 *
 * Write 1 to trigger a pulse to reset the whole watchdog timer.
 */
void
drv_wdt_sw_rst (
    void
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
drv_wdt_get_reset_info (
    WDT_ID_E    wdt_id
    );

#endif  // #ifndef _DRV_WDT_H_

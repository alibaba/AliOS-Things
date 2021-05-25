/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_PM_H
#define AOS_PM_H

typedef enum {
    AOS_CHARGER_STAT_SHUTDOWN = 0,
    AOS_CHARGER_STAT_CHECK,
    AOS_CHARGER_STAT_TRICKLE,
    AOS_CHARGER_STAT_PRE,
    AOS_CHARGER_STAT_CC,
    AOS_CHARGER_STAT_CV,
    AOS_CHARGER_STAT_TERMINAL,
    AOS_CHARGER_STAT_FAULT
} aos_charger_state_t;

/**
 * System enter sleep
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_system_sleep(void);

/**
 * Enable system autosleep interface
 *
 * @param[in]  mode  1 - autosleep enable, 0 - autosleep disable
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_system_autosleep(int mode);

/**
 * Accquire wakelock
 *
 * @param[in]  wakelock  wakelock instance
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_wakelock_lock(void *wakelock);

/**
 * Release wakelock
 *
 * @param[in]  wakelock  wakelock instance
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_wakelock_unlock(void *wakelock);

/**
 * Accquire wakelock within given time
 *
 * @param[in]  wakelock  wakelock instance
 * @param[in]  msec  wakelock keep time in ms
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_wakelock_timedlock(void *wakelock, unsigned int msec);

/**
 * Create wakelock
 *
 * @param[in]  name  wakelock name
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
void *aos_wakelock_create(const char *name);

/**
 * Destroy wakelock
 *
 * @param[in]  wakelock  wakelock instance
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
void aos_wakelock_release(void *wakelock);

/**
 * Register power key state notifier
 *
 * @param[in]  cb  power key notifier callback (argment: 1 - key down, 0 - key up)
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_pwrkey_notify_register(void (*cb)(int));

/**
 * Device power down
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_power_down(void);

/**
 * Device power reset
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_power_reset(void);

/**
 * Get battery connection state
 *
 * @param[in]  state (1 - connected, 0 - disconnected)
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_battery_connect_state_get(int *state);

/**
 * Get battery connection state
 *
 * @param[in] store voltage in mV
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_battery_voltage_get(int *voltage);

/**
 * Get battery level
 *
 * @param[in] store battery level (0 - 100)
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_battery_level_get(int *level);

/**
 * Get battery temperature
 *
 * @param[in]  store temperature
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_battery_temperature_get(int *temperature);

/**
 * Get charger connection state
 *
 * @param[in] store connection state (1 - connected, 0 - disconnected)
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_charger_connect_state_get(int *state);

/**
 * Get charger state
 *
 * @param[in] store charger state
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_charger_state_get(aos_charger_state_t *state);

/**
 * Get charger current
 *
 * @param[in] store charger current in mA
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_charger_current_get(int *current);

/**
 * Set charger switch (1 - ON, 0 - OFF)
 *
 * @param[in] charger switch onoff
 *
 * @return  0 : on success, negative number : if an error occurred with any step
 */
int aos_charger_switch_set(int enable);

#endif /* AOS_PM_H */


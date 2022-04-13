/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AMP_PM_H
#define AMP_PM_H



/**
 * Enable system autosleep interface
 *
 * @param[in]  mode  1 - autosleep enable, 0 - autosleep disable
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_System_Autosleep(int mode);

/**
 * Accquire wakelock
 *
 * @param[in]  wakelock  wakelock instance
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_Wakelock_Lock(void *wakelock);

/**
 * Release wakelock
 *
 * @param[in]  wakelock  wakelock instance
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_Wakelock_Unlock(void *wakelock);

/**
 * Accquire wakelock within given time
 *
 * @param[in]  wakelock  wakelock instance
 * @param[in]  msec  wakelock keep time in ms
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_Wakelock_Timedlock(void *wakelock, unsigned int msec);

/**
 * Create wakelock
 *
 * @param[in]  name  wakelock name
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
void *HAL_Wakelock_Create(const char *name);

/**
 * Destroy wakelock
 *
 * @param[in]  wakelock  wakelock instance
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
void HAL_Wakelock_Release(void *wakelock);

/**
 * Register power key state notifier
 *
 * @param[in]  cb  power key notifier callback (argment: 1 - key down, 0 - key up)
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int HAL_Pwrkey_Notify_Register(void (*cb)(int));

/**
 * Get battery connection state
 *
 * @param[in]  state (1 - connected, 0 - disconnected)
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int HAL_Battery_Connect_State_Get(int *state);

/**
 * Get battery connection state
 *
 * @param[in] store voltage in mV
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int HAL_Battery_Voltage_Get(int *voltage);

/**
 * Get battery level
 *
 * @param[in] store battery level (0 - 100)
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int HAL_Battery_Level_Get(int *level);

/**
 * Get battery temperature
 *
 * @param[in]  store temperature
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int HAL_Battery_Temperature_Get(int *temperature);

/**
 * Get charger connection state
 *
 * @param[in] store connection state (1 - connected, 0 - disconnected)
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int HAL_Charger_Connect_State_Get(int *state);

/**
 * Get charger current
 *
 * @param[in] store charger current in mA
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int HAL_Charger_Current_Get(int *current);

/**
 * Set charger switch (1 - ON, 0 - OFF)
 *
 * @param[in] charger switch onoff
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int HAL_Charger_Switch_Set(int enable);


#endif /* AMP_PM_H */

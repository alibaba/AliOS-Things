/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ql_data_call.h"
#include "ql_rtos.h"
#include "ql_power.h"

#include "amp_pm.h"

int32_t HAL_System_Sleep(void)
{
    return 0;
}

int32_t HAL_System_Autosleep(int mode)
{
    HAL_Printf("set autosleep %d\r\n", mode);
    ql_autosleep_enable(mode ? QL_ALLOW_SLEEP : QL_NOT_ALLOW_SLEEP);
    return 0;
}

int32_t HAL_Wakelock_Lock(void *wakelock)
{
    if (!wakelock)
        return -1;
    return ql_lpm_wakelock_lock(*(int *)wakelock);
}

int32_t HAL_Wakelock_Unlock(void *wakelock)
{
    if (!wakelock)
        return -1;
    return ql_lpm_wakelock_unlock(*(int *)wakelock);
}

int32_t HAL_Wakelock_Timedlock(void *wakelock, unsigned int msec)
{
    if (!wakelock)
        return -1;
    return 0;
}

void *HAL_Wakelock_Create(const char *name)
{
    int *w;

    if (!name)
        return NULL;

    w = HAL_Malloc(sizeof(int));
    if (!w)
        return NULL;

    *w = ql_lpm_wakelock_create(name, strlen(name));
    if (*w <= 0) {
        HAL_Printf("create wakelock fail %d\n", *w);
        HAL_Free(w);
        return NULL;
    }

    return w;
}

void HAL_Wakelock_Release(void *wakelock)
{
    if (!wakelock)
        return;
    ql_lpm_wakelock_delete(*(int *)wakelock);
    HAL_Free(wakelock);
}

static void (*user_pwrkey_cb)(int);

static void ql_pwrkey_state_cb(void)
{
    if (ql_get_pwrkey_status() == 1) {
        user_pwrkey_cb(1);
        return;
    }
    user_pwrkey_cb(0);
}

int HAL_Pwrkey_Notify_Register(void (*cb)(int))
{
    if (!cb)
        return -1;
    user_pwrkey_cb = cb;
    ql_pwrkey_register_irq(ql_pwrkey_state_cb);
    ql_pwrkey_intc_enable(1);
    return 0;
}

int HAL_Battery_Connect_State_Get(int *state)
{
	if (ql_get_battery_state()) {
		*state = 1;
	} else {
		*state = 0;
	}
	return 0;
}

int HAL_Battery_Voltage_Get(int *voltage)
{
	*voltage = ql_get_battery_vol();
	return 0;
}

int HAL_Battery_ChargeVoltage_Get(int *voltage)
{
	*voltage = ql_get_charging_battery_vol();
	return 0;
}

int HAL_Battery_Level_Get(int *level)
{
	*level = ql_get_battery_level();
	return 0;
}

int HAL_Battery_Temperature_Get(int *temperature)
{
	*temperature = ql_get_battemp_vol_mv();
	return 0;
}

int HAL_Charger_Battery_ChargeVoltage_Get(int *voltage)
{
	*voltage = ql_get_charging_battery_vol();
	return 0;
}

int HAL_Charger_Connect_State_Get(int *state)
{
	*state = ql_get_vbus_state();
	return 0;
}

int HAL_Charger_Current_Get(int *current)
{
	*current = ql_get_charge_cur();
	return 0;
}

int HAL_Charger_Switch_Set(int enable)
{
	ql_charger_switch(enable);
	return 0;
}


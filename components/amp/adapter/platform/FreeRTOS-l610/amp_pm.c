/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "amp_pm.h"

int32_t HAL_System_Sleep(void)
{
    return -1;
}

int32_t HAL_System_Autosleep(int mode)
{
    return -1;
}

int32_t HAL_Wakelock_Lock(void *wakelock)
{
    return -1;
}

int32_t HAL_Wakelock_Unlock(void *wakelock)
{
    return -1;
}

int32_t HAL_Wakelock_Timedlock(void *wakelock, unsigned int msec)
{
    return -1;
}

void *HAL_Wakelock_Create(const char *name)
{
    return NULL;
}

void HAL_Wakelock_Release(void *wakelock)
{
}

int HAL_Pwrkey_Notify_Register(void (*cb)(int))
{
    return -1;
}

int HAL_Battery_Connect_State_Get(int *state)
{
    return -1;
}

int HAL_Battery_Voltage_Get(int *voltage)
{
    return -1;
}

int HAL_Battery_ChargeVoltage_Get(int *voltage)
{
    return -1;
}

int HAL_Battery_Level_Get(int *level)
{
    return -1;
}

int HAL_Battery_Temperature_Get(int *temperature)
{
    return -1;
}

int HAL_Charger_Battery_ChargeVoltage_Get(int *voltage)
{
    return -1;
}

int HAL_Charger_Connect_State_Get(int *state)
{
    return -1;
}

int HAL_Charger_Current_Get(int *current)
{
    return -1;
}

int HAL_Charger_Switch_Set(int enable)
{
    return -1;
}


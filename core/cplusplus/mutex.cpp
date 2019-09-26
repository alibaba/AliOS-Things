/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "mutex.h"

using namespace AliOS;

/**
 * This function will create a mutex
 * @param[in] name   name of the mutex
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Mutex::create(const name_t *name)
{
    return krhino_mutex_create(&_mutex_def, name);
}

/**
 * This function will delete a mutex
 * @param[in] NULL
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Mutex::destroy(void)
{
    return krhino_mutex_del(&_mutex_def);
}

/**
 * This function will lock mutex
 * @param[in]  millisec  millisec to be wait for before lock
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Mutex::lock(uint32_t millisec)
{
    tick_t ticks = 0;

    if (millisec == 0) {
        ticks = RHINO_NO_WAIT;
    } else if (millisec == Mutex_WAIT_FOREVER){
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = krhino_ms_to_ticks(millisec);
    }

    return krhino_mutex_lock(&_mutex_def, ticks);
}

/**
 * This function will unlock a mutex
 * @param[in] NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Mutex::unlock(void)
{
    return krhino_mutex_unlock(&_mutex_def);
}

/**
 * This function will get a Mutex struct pointer
 * @param[in]  none
 * @return  Mutex struct pointer
 */
kmutex_t *Mutex::self(void)
{
    return &_mutex_def;
}

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "Semaphore.h"

using namespace AliOS;

/**
 * This function will create a semaphore
 * @param[in]  name   name of the semaphore
 * @param[in]  count  the init count of the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Semaphore::create(const name_t *name, sem_count_t count)
{
    return krhino_sem_create(&_sem_def, name, count);
}

/**
 * This function will delete a semaphore
 * @param[in]  NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Semaphore::destroy(void)
{
    return krhino_sem_del(&_sem_def);
}

/**
 * This function will wait a semaphore
 * @param[in]  millisec  millisec to wait before take
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Semaphore::wait(uint32_t millisec)
{
    tick_t ticks = 0;

    if (millisec == 0) {
        ticks = RHINO_NO_WAIT;
    } else if (millisec == Semaphore_WAIT_FOREVER){
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = krhino_ms_to_ticks(millisec);
    }

    return krhino_sem_take(&_sem_def, ticks);
}

/**
 * This function will release a semaphore
 * @param[in]  NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Semaphore::release(void)
{
    return krhino_sem_give(&_sem_def);
}

/**
 * This function will release all semaphore
 * @param[in]  NULL
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Semaphore::release_all(void)
{
    return krhino_sem_give_all(&_sem_def);
}

/**
 * This function will set the count of a semaphore
 * @param[in]  sem_count  count of the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Semaphore::count_set(sem_count_t  count)
{
    return krhino_sem_count_set(&_sem_def, count);
}

/**
 * This function will get count of a semaphore
 * @param[out]  count  count of the semaphore
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t Semaphore::count_get(sem_count_t *count)
{
    return krhino_sem_count_get(&_sem_def, count);
}

/**
 * This function will get a Semaphore struct pointer
 * @param[in]  none
 * @return  Semaphore struct pointer
 */
ksem_t *Semaphore::self(void)
{
    return &_sem_def;
}

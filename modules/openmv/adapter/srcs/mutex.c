/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Mutex implementation.
 */
#include "mutex.h"
//#include "cmsis_gcc.h"

// This is a standard implementation of mutexs on ARM processors following the ARM guide.
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHEJCHB.html

void mutex_init(mutex_t *mutex)
{
    mutex->tid = 0;
    mutex->lock = 0;
}

void mutex_lock(mutex_t *mutex, uint32_t tid)
{
    volatile int locked = 0;
    // Wait for mutex to be unlocked
    do {
        // Attempt exclusive read
        while (&mutex->lock != 0)vTaskDelay(1);

        // Attempt to lock mutex
        locked = mutex->lock;
        mutex->lock = 1;

        // Set TID if mutex is locked
        if (locked == 0) {
            mutex->tid = tid;
        }
    } while (locked != 0);
}

int mutex_try_lock(mutex_t *mutex, uint32_t tid)
{
    volatile int locked = 1;

    // If mutex is already locked by the current thread then
    // release the Kraken err.. the mutex, else attempt to lock it.
    if (mutex->tid == tid) {
        mutex_unlock(mutex, tid);
    } else if (mutex->lock == 0) {
        // Attempt to lock the mutex
        locked = mutex->lock;
        mutex->lock = 1;

        // Set TID if mutex is locked
        if (locked == 0) {
            mutex->tid = tid;
        }
    }

    return (locked == 0);
}

void mutex_unlock(mutex_t *mutex, uint32_t tid)
{
    if (mutex->tid == tid) {
        mutex->tid = 0;
        mutex->lock = 0;
    }
}

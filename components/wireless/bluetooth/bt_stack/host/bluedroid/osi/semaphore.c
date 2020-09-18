/******************************************************************************
 *
 *  Copyright (C) 2015 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/


#include "osi/semaphore.h"

/*-----------------------------------------------------------------------------------*/
//  Creates and returns a new semaphore. The "init_count" argument specifies
//  the initial state of the semaphore, "max_count" specifies the maximum value
//  that can be reached.
int osi_sem_new(osi_sem_t *sem, uint32_t max_count, uint32_t init_count)
{
    int ret = -1;

    if (sem) {
        ret = hal_os_sem_new((hal_sem_t*)sem, max_count, init_count);
    }

    return ret;
}

/*-----------------------------------------------------------------------------------*/
// Give a semaphore
void osi_sem_give(osi_sem_t *sem)
{
    hal_os_sem_give((hal_sem_t*)sem);
}

/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds).

*/
int
osi_sem_take(osi_sem_t *sem, uint32_t timeout)
{
    return hal_os_sem_take((hal_sem_t*)sem, timeout);
}

// Deallocates a semaphore
void osi_sem_free(osi_sem_t *sem)
{
    hal_os_sem_free((hal_sem_t*)sem);
}

int osi_sem_is_valid(osi_sem_t *sem)
{
    return hal_os_sem_is_valid((hal_sem_t*)sem);
}

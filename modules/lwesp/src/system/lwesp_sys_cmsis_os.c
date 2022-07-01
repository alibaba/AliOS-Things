/**
 * \file            lwesp_sys_cmsis_os.c
 * \brief           System dependent functions for CMSIS based operating system
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#include "system/lwesp_sys.h"
#include "cmsis_os.h"

#if !__DOXYGEN__

static osMutexId_t sys_mutex;

uint8_t
lwesp_sys_init(void) {
    lwesp_sys_mutex_create(&sys_mutex);
    return 1;
}

uint32_t
lwesp_sys_now(void) {
    return osKernelGetTickCount();
}

uint8_t
lwesp_sys_protect(void) {
    lwesp_sys_mutex_lock(&sys_mutex);
    return 1;
}

uint8_t
lwesp_sys_unprotect(void) {
    lwesp_sys_mutex_unlock(&sys_mutex);
    return 1;
}

uint8_t
lwesp_sys_mutex_create(lwesp_sys_mutex_t* p) {
    const osMutexAttr_t attr = {
        .attr_bits = osMutexRecursive,
        .name = "lwesp_mutex",
    };
    return (*p = osMutexNew(&attr)) != NULL;
}

uint8_t
lwesp_sys_mutex_delete(lwesp_sys_mutex_t* p) {
    return osMutexDelete(*p) == osOK;
}

uint8_t
lwesp_sys_mutex_lock(lwesp_sys_mutex_t* p) {
    return osMutexAcquire(*p, osWaitForever) == osOK;
}

uint8_t
lwesp_sys_mutex_unlock(lwesp_sys_mutex_t* p) {
    return osMutexRelease(*p) == osOK;
}

uint8_t
lwesp_sys_mutex_isvalid(lwesp_sys_mutex_t* p) {
    return p != NULL && *p != NULL;
}

uint8_t
lwesp_sys_mutex_invalid(lwesp_sys_mutex_t* p) {
    *p = LWESP_SYS_MUTEX_NULL;
    return 1;
}

uint8_t
lwesp_sys_sem_create(lwesp_sys_sem_t* p, uint8_t cnt) {
    const osSemaphoreAttr_t attr = {
        .name = "lwesp_sem",
    };
    return (*p = osSemaphoreNew(1, cnt > 0 ? 1 : 0, &attr)) != NULL;
}

uint8_t
lwesp_sys_sem_delete(lwesp_sys_sem_t* p) {
    return osSemaphoreDelete(*p) == osOK;
}

uint32_t
lwesp_sys_sem_wait(lwesp_sys_sem_t* p, uint32_t timeout) {
    uint32_t tick = osKernelSysTick();
    return (osSemaphoreAcquire(*p, timeout == 0 ? osWaitForever : timeout) == osOK) ? (osKernelSysTick() - tick) : LWESP_SYS_TIMEOUT;
}

uint8_t
lwesp_sys_sem_release(lwesp_sys_sem_t* p) {
    return osSemaphoreRelease(*p) == osOK;
}

uint8_t
lwesp_sys_sem_isvalid(lwesp_sys_sem_t* p) {
    return p != NULL && *p != NULL;
}

uint8_t
lwesp_sys_sem_invalid(lwesp_sys_sem_t* p) {
    *p = LWESP_SYS_SEM_NULL;
    return 1;
}

uint8_t
lwesp_sys_mbox_create(lwesp_sys_mbox_t* b, size_t size) {
    const osMessageQueueAttr_t attr = {
        .name = "lwesp_mbox",
    };
    return (*b = osMessageQueueNew(size, sizeof(void*), &attr)) != NULL;
}

uint8_t
lwesp_sys_mbox_delete(lwesp_sys_mbox_t* b) {
    if (osMessageQueueGetCount(*b) > 0) {
        return 0;
    }
    return osMessageQueueDelete(*b) == osOK;
}

uint32_t
lwesp_sys_mbox_put(lwesp_sys_mbox_t* b, void* m) {
    uint32_t tick = osKernelSysTick();
    return osMessageQueuePut(*b, &m, 0, osWaitForever) == osOK ? (osKernelSysTick() - tick) : LWESP_SYS_TIMEOUT;
}

uint32_t
lwesp_sys_mbox_get(lwesp_sys_mbox_t* b, void** m, uint32_t timeout) {
    uint32_t tick = osKernelSysTick();
    return (osMessageQueueGet(*b, m, NULL, timeout == 0 ? osWaitForever : timeout) == osOK) ? (osKernelSysTick() - tick) : LWESP_SYS_TIMEOUT;
}

uint8_t
lwesp_sys_mbox_putnow(lwesp_sys_mbox_t* b, void* m) {
    return osMessageQueuePut(*b, &m, 0, 0) == osOK;
}

uint8_t
lwesp_sys_mbox_getnow(lwesp_sys_mbox_t* b, void** m) {
    return osMessageQueueGet(*b, m, NULL, 0) == osOK;
}

uint8_t
lwesp_sys_mbox_isvalid(lwesp_sys_mbox_t* b) {
    return b != NULL && *b != NULL;
}

uint8_t
lwesp_sys_mbox_invalid(lwesp_sys_mbox_t* b) {
    *b = LWESP_SYS_MBOX_NULL;
    return 1;
}

uint8_t
lwesp_sys_thread_create(lwesp_sys_thread_t* t, const char* name, lwesp_sys_thread_fn thread_func, void* const arg, size_t stack_size, lwesp_sys_thread_prio_t prio) {
    lwesp_sys_thread_t id;
    const osThreadAttr_t thread_attr = {
        .name = (char*)name,
        .priority = (osPriority)prio,
        .stack_size = stack_size > 0 ? stack_size : LWESP_SYS_THREAD_SS
    };

    id = osThreadNew(thread_func, arg, &thread_attr);
    if (t != NULL) {
        *t = id;
    }
    return id != NULL;
}

uint8_t
lwesp_sys_thread_terminate(lwesp_sys_thread_t* t) {
    if (t != NULL) {
        osThreadTerminate(*t);
    } else {
        osThreadExit();
    }
    return 1;
}

uint8_t
lwesp_sys_thread_yield(void) {
    osThreadYield();
    return 1;
}

#endif /* !__DOXYGEN__ */

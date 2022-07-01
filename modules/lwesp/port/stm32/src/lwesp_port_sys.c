/**
 * \file            lwesp_sys_freertos.c
 * \brief           System dependant functions for FreeRTOS
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
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Author:          Adrian Carpenter (FreeRTOS port)
 * Version:         v1.1.2-dev
 */
#include "system/lwesp_sys.h"
#include "lwesp_sys_port.h"
#include "lwesp/lwesp.h"
#include "lwesp/lwesp_mem.h"
#if !__DOXYGEN__

static pyb_mutex_t sys_mutex;

uint8_t
lwesp_sys_init(void) {
    pyb_mutex_init(&sys_mutex);
    return 1;
}

uint32_t
lwesp_sys_now(void) {
    return mp_hal_ticks_ms();
}
#define RAISE_IRQ_PRI() disable_irq() 
#define RESTORE_IRQ_PRI(state) enable_irq(state)

#define DEBUG 0
uint8_t
lwesp_sys_protect(void) {
#if DEBUG
    uint32_t irq_state;
    irq_state = RAISE_IRQ_PRI();
    printf("%s[%s] 1 owner_nested = %d\r\n", __func__, pyb_thread_cur->name, sys_mutex.owner_nested);
    RESTORE_IRQ_PRI(irq_state); 
#endif    
    pyb_mutex_lock(&sys_mutex, 1);
#if DEBUG
    irq_state = RAISE_IRQ_PRI();
    printf("%s[%s] 2 owner_nested = %d\r\n", __func__, pyb_thread_cur->name, sys_mutex.owner_nested);
    RESTORE_IRQ_PRI(irq_state); 
#endif    
    return 1;
}

extern pyb_thread_t *volatile pyb_thread_cur;
uint8_t
lwesp_sys_unprotect(void) {
#if DEBUG
    uint32_t irq_state;
    irq_state = RAISE_IRQ_PRI();
    printf("%s[%s] 1 owner_nested = %d\r\n", __func__, pyb_thread_cur->name, sys_mutex.owner_nested);
    RESTORE_IRQ_PRI(irq_state); 
#endif    
    pyb_mutex_unlock(&sys_mutex);
#if DEBUG
    irq_state = RAISE_IRQ_PRI();
    printf("%s[%s] 2 owner_nested = %d\r\n", __func__, pyb_thread_cur->name, sys_mutex.owner_nested);
    RESTORE_IRQ_PRI(irq_state); 
#endif    
    return 1;
}

uint8_t
lwesp_sys_mutex_create(lwesp_sys_mutex_t* p) {
    void *mutex = lwesp_mem_malloc(sizeof(pyb_mutex_t));
    if (mutex == NULL) {
        return 0;
    }
    pyb_mutex_init(mutex);
    *p = mutex;
    return 1;
}

uint8_t
lwesp_sys_mutex_delete(lwesp_sys_mutex_t* p) {
    lwesp_mem_free(*p);
    return 1;
}

uint8_t
lwesp_sys_mutex_lock(lwesp_sys_mutex_t* p) {
    pyb_mutex_lock(*p, 1);
    return 1;
}

uint8_t
lwesp_sys_mutex_unlock(lwesp_sys_mutex_t* p) {
    pyb_mutex_unlock(*p);
    return 1;
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
    void *sem = lwesp_mem_malloc(sizeof(pyb_sem_t));
    if (sem == NULL) {
        return 0;
    }
    pyb_sem_init(sem, cnt);
    *p = sem;
    
    return *p != NULL;
}

uint8_t
lwesp_sys_sem_delete(lwesp_sys_sem_t* p) {
    lwesp_mem_free(*p);
    return 1;
}

uint32_t
lwesp_sys_sem_wait(lwesp_sys_sem_t* p, uint32_t timeout) {
    if (!timeout) {
        pyb_sem_take(*p, 1);  
    } else {
        const uint32_t count_1ms = HAL_RCC_GetSysClockFreq() / (4 * 4000);
        for (volatile int i = 0; i < timeout; i++) {
            for (volatile uint32_t count = 0; ++count <= count_1ms;) {
                
            }
            if (0 == pyb_sem_take(*p, 0)) {
                    return 1;
            }
        }
#if 0
        mp_uint_t start = mp_hal_ticks_ms();
        while (mp_hal_ticks_ms() - start <= timeout) { 
            if (0 == pyb_sem_take(*p, 0)) {
                return 1;
            }
        }
#endif
        return 0;

    }
    return 1;
}

uint8_t
lwesp_sys_sem_release(lwesp_sys_sem_t* p) {
    pyb_sem_give(*p);
    return 1;
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
    void *mbox = lwesp_mem_malloc(sizeof(pyb_mbox_t));
    if (mbox == NULL) {
        return 0;
    }
    pyb_mbox_init(mbox, size);
    *b = mbox;
    return *b != NULL;
}

uint8_t
lwesp_sys_mbox_delete(lwesp_sys_mbox_t* b) {
    lwesp_mem_free(*b);
    return 1;
}

uint32_t
lwesp_sys_mbox_put(lwesp_sys_mbox_t* b, void* m) {
    pyb_mbox_post(*b, m);
    return 1;
}

uint32_t
lwesp_sys_mbox_has_entry(lwesp_sys_mbox_t* b) {
    return pyb_mbox_has_entry(*b);
}

uint32_t
lwesp_sys_mbox_get(lwesp_sys_mbox_t* b, void** m, uint32_t timeout) {
     if (!timeout) { 
        pyb_mbox_fetch(*b, m, 0);
     } else {
        const uint32_t count_1ms = HAL_RCC_GetSysClockFreq() / (4 * 4000);
        for (volatile int i = 0; i < timeout; i++) {
            for (volatile uint32_t count = 0; ++count <= count_1ms;) {
                
            }
            if (0 == pyb_mbox_tryfetch(*b, m)) {
                    return 1;
            }
        }

            
 #if 0
        mp_uint_t start = mp_hal_ticks_ms();
        while (mp_hal_ticks_ms() - start <= timeout) { 
            if (0 == pyb_mbox_tryfetch(*b, m)) {
                    return 1;
            }
        }
#endif
        return 0;
     }
     return 1;
}

uint8_t
lwesp_sys_mbox_putnow(lwesp_sys_mbox_t* b, void* m) {
    pyb_mbox_post( *b, m);
    return 1;
}

uint8_t
lwesp_sys_mbox_getnow(lwesp_sys_mbox_t* b, void** m) {
    if (1 == pyb_mbox_tryfetch(*b, m)) {
        return 0;
    }
    return 1;
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

void mp_ctask_entry(void *arg)
{
    mp_ctask_struct_t *task_struct = (mp_ctask_struct_t *)arg;
    mp_state_thread_t ts = {0};
    mp_thread_set_state(&ts);
    lwesp_sys_thread_fn entry = task_struct->entry;
    void *task_param = task_struct->arg;
   
    mp_stack_set_top(&ts + 1);  // need to include ts in root-pointer scan
    mp_stack_set_limit(task_struct->stack_size);

#if MICROPY_ENABLE_PYSTACK
    // TODO threading and pystack is not fully supported, for now just make a small stack
    mp_obj_t mini_pystack[128];
    mp_pystack_init(mini_pystack, &mini_pystack[128]);
#endif

    ts.gc_lock_depth = 0;
    ts.mp_pending_exception = MP_OBJ_NULL;
    // set locals and globals from the calling context
    mp_locals_set(task_struct->dict_locals);
    mp_globals_set(task_struct->dict_globals);
    // signal that we are set up and running
    if (entry) {
        entry(task_param);
    }
}

int create_mp_ctask(lwesp_sys_thread_t* t, const char* name, lwesp_sys_thread_fn thread_func, void* const arg, size_t stack_size, lwesp_sys_thread_prio_t prio)
{
    mp_ctask_struct_t *task_struct;

    task_struct = (mp_ctask_struct_t *)lwesp_mem_malloc(sizeof(mp_ctask_struct_t));
    if (task_struct == NULL) {
        return 0;
    }
    void *stack = lwesp_mem_malloc(stack_size);
    if (stack == NULL) {
        lwesp_mem_free(task_struct);
        return 0;
    }
    
    memset(task_struct, 0, sizeof(mp_ctask_struct_t));
    task_struct->entry = thread_func;
    task_struct->arg = arg;
    task_struct->dict_locals = mp_locals_get();
    task_struct->dict_globals = mp_globals_get();
    task_struct->stack_size =  stack_size - 1024;
    task_struct->stack = stack; 
    memcpy(task_struct->pyb_thread.name, name, strlen(name) + 1);
    pyb_thread_new(&(task_struct->pyb_thread), stack, stack_size / sizeof(uint32_t), mp_ctask_entry, task_struct);
    *t = task_struct;
    return 1;
}

uint8_t
lwesp_sys_thread_create(lwesp_sys_thread_t* t, const char* name, lwesp_sys_thread_fn thread_func, void* const arg, size_t stack_size, lwesp_sys_thread_prio_t prio) 
{
    return create_mp_ctask(t, name, thread_func, arg, stack_size, prio);
}
uint8_t
lwesp_sys_thread_terminate(lwesp_sys_thread_t* t) {
    mp_ctask_struct_t *task_struct = NULL;
    if (t) {
        task_struct = *t;
    }
    if (task_struct) {
        pyb_thread_terminate_thread(&(task_struct->pyb_thread));
    }
    if (task_struct && task_struct->stack) {
        lwesp_mem_free(task_struct->stack);
    }
    if (task_struct) {
        lwesp_mem_free(task_struct);
    }
    return 1;
}

uint8_t
lwesp_sys_thread_yield(void) {
    pyb_thread_yield();
    return 1;
}



#endif /* !__DOXYGEN__ */

/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>
#include <stdio.h>

#include "py/obj.h"
#include "gccollect.h"
#include "irq.h"
#include "pybthread.h"

#if MICROPY_PY_THREAD

#define PYB_MUTEX_UNLOCKED ((void *)0)
#define PYB_MUTEX_LOCKED ((void *)1)

// These macros are used when we only need to protect against a thread
// switch; other interrupts are still allowed to proceed.
#define RAISE_IRQ_PRI() disable_irq() 
#define RESTORE_IRQ_PRI(state) enable_irq(state)

extern void __fatal_error(const char *);

volatile int pyb_thread_enabled;
pyb_thread_t *volatile pyb_thread_all;
pyb_thread_t *volatile pyb_thread_cur;

static inline void pyb_thread_add_to_runable(pyb_thread_t *thread) {
    thread->run_prev = pyb_thread_cur->run_prev;
    thread->run_next = pyb_thread_cur;
    pyb_thread_cur->run_prev->run_next = thread;
    pyb_thread_cur->run_prev = thread;
}

static inline void pyb_thread_remove_from_runable(pyb_thread_t *thread) {
    if (thread->run_next == thread) {
        pyb_thread_dump();
        printf("%s curr:%p next:%p prev:%p\r\n",__func__, thread, thread->run_next, thread->run_prev );
        __fatal_error("deadlock");
    }
    thread->run_prev->run_next = thread->run_next;
    thread->run_next->run_prev = thread->run_prev;
}

static inline void pyb_thread_remove_from_runable_sem(pyb_thread_t *thread) {
    if (thread->run_next == thread) {
        pyb_thread_dump();
        printf("%s curr:%p next:%p prev:%p\r\n",__func__, thread, thread->run_next, thread->run_prev );
        __fatal_error("deadlock");
    }
    thread->run_prev->run_next = thread->run_next;
    thread->run_next->run_prev = thread->run_prev;
}
static inline void pyb_thread_remove_from_runable_mutex(pyb_thread_t *thread) {
    if (thread->run_next == thread) {
        pyb_thread_dump();
        printf("%s curr:%p next:%p prev:%p\r\n",__func__, thread, thread->run_next, thread->run_prev );
        __fatal_error("deadlock");
    }
    thread->run_prev->run_next = thread->run_next;
    thread->run_next->run_prev = thread->run_prev;
}
void pyb_thread_init(pyb_thread_t *thread) {
    pyb_thread_enabled = 0;
    pyb_thread_all = thread;
    pyb_thread_cur = thread;
    thread->sp = NULL; // will be set when this thread switches out
    thread->local_state = 0; // will be set by mp_thread_init
    thread->arg = NULL;
    thread->stack = &_sstack;
    thread->stack_len = ((uint32_t)&_estack - (uint32_t)&_sstack) / sizeof(uint32_t);
    thread->all_next = NULL;
    thread->run_prev = thread;
    thread->run_next = thread;
    thread->queue_next = NULL;
    memcpy(thread->name, "repl", strlen("repl")+1);
}

void pyb_thread_deinit() {
    uint32_t irq_state = disable_irq();
    pyb_thread_enabled = 0;
    pyb_thread_all = pyb_thread_cur;
    pyb_thread_cur->all_next = NULL;
    pyb_thread_cur->run_prev = pyb_thread_cur;
    pyb_thread_cur->run_next = pyb_thread_cur;
    enable_irq(irq_state);
}

void pyb_thread_yield(void) {
if (pyb_thread_cur->run_next == pyb_thread_cur) {
    __WFI();
} else {
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}
}

void pyb_thread_terminate_thread(pyb_thread_t *thread) {
    uint32_t irq_state = disable_irq();
    // take current thread off the run list
    pyb_thread_remove_from_runable(thread);
    // take current thread off the list of all threads
    for (pyb_thread_t **n = (pyb_thread_t **)&pyb_thread_all;; n = &(*n)->all_next) {
        if (*n == thread) {
            *n = thread->all_next;
            break;
        }
    }
    // clean pointers as much as possible to help GC
    thread->all_next = NULL;
    thread->queue_next = NULL;
    thread->stack = NULL;
    if (pyb_thread_all->all_next == NULL) {
        // only 1 thread left
        pyb_thread_enabled = 0;
    }
    // thread switch will occur after we enable irqs
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    enable_irq(irq_state);
}
STATIC void pyb_thread_terminate(void) {
    uint32_t irq_state = disable_irq();
    pyb_thread_t *thread = pyb_thread_cur;
    // take current thread off the run list
    pyb_thread_remove_from_runable(thread);
    // take current thread off the list of all threads
    for (pyb_thread_t **n = (pyb_thread_t **)&pyb_thread_all;; n = &(*n)->all_next) {
        if (*n == thread) {
            *n = thread->all_next;
            break;
        }
    }
    // clean pointers as much as possible to help GC
    thread->all_next = NULL;
    thread->queue_next = NULL;
    thread->stack = NULL;
    if (pyb_thread_all->all_next == NULL) {
        // only 1 thread left
        pyb_thread_enabled = 0;
    }
    // thread switch will occur after we enable irqs
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    enable_irq(irq_state);
    // should not return
    __fatal_error("could not terminate");
}

uint32_t pyb_thread_new(pyb_thread_t *thread, void *stack, size_t stack_len, void *entry, void *arg) {
    uint32_t *stack_top = (uint32_t *)stack + stack_len; // stack is full descending
    *--stack_top = 0x01000000; // xPSR (thumb bit set)
    *--stack_top = (uint32_t)entry & 0xfffffffe; // pc (must have bit 0 cleared, even for thumb code)
    *--stack_top = (uint32_t)pyb_thread_terminate; // lr
    *--stack_top = 0; // r12
    *--stack_top = 0; // r3
    *--stack_top = 0; // r2
    *--stack_top = 0; // r1
    *--stack_top = (uint32_t)arg; // r0
    *--stack_top = 0xfffffff9; // lr (return to thread mode, non-FP, use MSP)
    stack_top -= 8; // r4-r11
    stack_top -= 16; // s16-s31 (we assume all threads use FP registers)
    thread->sp = stack_top;
    thread->local_state = 0;
    thread->arg = arg;
    thread->stack = stack;
    thread->stack_len = stack_len;
    thread->queue_next = NULL;
    uint32_t irq_state = disable_irq();
    pyb_thread_enabled = 1;
    thread->all_next = pyb_thread_all;
    pyb_thread_all = thread;
    pyb_thread_add_to_runable(thread);
    enable_irq(irq_state);
    return (uint32_t)thread; // success
}

void pyb_thread_dump(void) {
    if (!pyb_thread_enabled) {
        printf("THREAD: only main thread\r\n");
    } else {
        printf("THREAD:\r\n");
        for (pyb_thread_t *th = pyb_thread_all; th != NULL; th = th->all_next) {
            bool runable = false;
            for (pyb_thread_t *th2 = pyb_thread_cur;; th2 = th2->run_next) {
                if (th == th2) {
                    runable = true;
                    break;
                }
                if (th2->run_next == pyb_thread_cur) {
                    break;
                }
            }
            printf(" name=%s   id=%p sp=%p sz=%u", th->name, th, th->stack, th->stack_len);
            if (runable) {
                printf(" (runable)");
            }
            printf("\r\n");
        }
    }
}

// should only be called from pendsv_isr_handler
void *pyb_thread_next(void *sp) {
    pyb_thread_cur->sp = sp;
    pyb_thread_cur = pyb_thread_cur->run_next;
    pyb_thread_cur->timeslice = 4; // in milliseconds
    return pyb_thread_cur->sp;
}

void pyb_sem_init(pyb_sem_t *m, uint32_t count) {
    m->count = count;
    m->thread  = NULL;
}
/*
    1:fail;0:ok
 */
int pyb_sem_take(pyb_sem_t *m, int wait) {
    uint32_t irq_state = RAISE_IRQ_PRI();
    if (m->count > 0) {
        // sem is available
        m->count -= 1;
        RESTORE_IRQ_PRI(irq_state);
    } else {
        // sem is locked
        if (!wait) {
            RESTORE_IRQ_PRI(irq_state);
            return 1; // failed to lock sem
        }
        
        if (m->thread == NULL) {
            m->thread = pyb_thread_cur; // first thread to take
        } else {
            for (pyb_thread_t *n = m->thread;; n = n->queue_next) {
                if (n->queue_next == NULL) {
                    n->queue_next = pyb_thread_cur;
                    break;
                }
            }
        }
        pyb_thread_cur->queue_next = NULL;
        // take current thread off the run list
        pyb_thread_remove_from_runable_sem(pyb_thread_cur);
        // thread switch will occur after we enable irqs
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
        RESTORE_IRQ_PRI(irq_state);
        // when we come back we have the mutex
    }
    return 0; // have mutex
}

void pyb_sem_give(pyb_sem_t *m) {
    uint32_t irq_state = RAISE_IRQ_PRI();
    if ((m->count > 0) || (m->count == 0 && m->thread == NULL)) {
        // no threads are blocked on the sem.
        m->count += 1;
    } else {
        // at least one thread is blocked on this mutex
        pyb_thread_t *th = m->thread;
        if (th->queue_next == NULL) {
            // no other threads are blocked
            m->thread = NULL;
        } else {
            // at least one other thread is still blocked
            m->thread = th->queue_next;
        }
        // put unblocked thread on runable list
        pyb_thread_add_to_runable(th);
    }
    RESTORE_IRQ_PRI(irq_state);
}

int pyb_mbox_init(pyb_mbox_t *mbox, int size)
{
    (void)size;
    memset(mbox, 0, sizeof(pyb_mbox_t));

    mbox->write = mbox->read = mbox->entry =0;
    mbox->size = SYS_MBOX_SIZE; 
    pyb_sem_init(&mbox->have_space, 0);
    pyb_sem_init(&mbox->have_data, 0);
    pyb_mutex_init(&mbox->mutex);
    mbox->wait_send = 0;
    mbox->wait_recv = 0;
    return 0;
}

/*-----------------------------------------------------------------------------------*/
/*
    Posts the "msg" to the mailbox. This function have to block until  the "msg" is really posted.
*/
void pyb_mbox_post(pyb_mbox_t *mbox, void *msg)
{
    
    pyb_mutex_lock(&mbox->mutex, 1);

    while (mbox->entry == mbox->size) {
        mbox->wait_send++;
        pyb_mutex_unlock(&mbox->mutex);
        pyb_sem_take(&mbox->have_space, 1);
        pyb_mutex_lock(&mbox->mutex, 1);
        mbox->wait_send--;
    }

    mbox->msgs[mbox->write] = msg;
    ++mbox->write;
    if (mbox->write >= mbox->size)
        mbox->write = 0;
    mbox->entry++;
    
    if (mbox->wait_recv) {
        pyb_sem_give(&mbox->have_data);
    }

    pyb_mutex_unlock(&mbox->mutex);
}

/*
    err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)

    Try to post the "msg" to the mailbox. Returns ERR_MEM if this one  is full, else, ERR_OK if the "msg" is posted.
*/
int pyb_mbox_trypost(pyb_mbox_t *mbox, void *msg)
{
    uint8_t first;

    pyb_mutex_lock(&mbox->mutex, 1);


    if (mbox->entry == mbox->size) {
        pyb_mutex_unlock(&mbox->mutex);
        return 1;
    }
    mbox->msgs[mbox->write] = msg;
    ++mbox->write;
    if (mbox->write >= mbox->size)
        mbox->write = 0;
    mbox->entry++;
    
    if (mbox->wait_recv) {
        pyb_sem_give(&mbox->have_data);
    }

    pyb_mutex_unlock(&mbox->mutex);
    return 0;
}

/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped.

  The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP.
*/
unsigned int pyb_mbox_fetch(pyb_mbox_t *mbox, void **msg, unsigned int timeout)
{
    unsigned int time_needed = 0;

    /* The mutex lock is quick so we don't bother with the timeout
     stuff here. */
    pyb_mutex_lock(&mbox->mutex, 1);

    while (mbox->entry == 0) {
        mbox->wait_recv++;
        pyb_mutex_unlock(&mbox->mutex);

        /* We block while waiting for a mail to arrive in the mailbox. We
           must be prepared to timeout. */
        pyb_sem_take(&mbox->have_data, 1);
        pyb_mutex_lock(&mbox->mutex, 1);
        mbox->wait_recv--;
    }

    if (msg != NULL) {
        *msg = mbox->msgs[mbox->read];
    } 
    ++ mbox->read;
    if (mbox->read >= mbox->size)
        mbox->read  =0;
    mbox->entry--;
    
    if (mbox->wait_send) {
        pyb_sem_give(&mbox->have_space);
    }

    pyb_mutex_unlock(&mbox->mutex);

    return 0;
}

unsigned int pyb_mbox_has_entry(pyb_mbox_t *mbox)
{
    unsigned int has_entry;
    
    pyb_mutex_lock(&mbox->mutex, 1);
    has_entry = (mbox->entry > 0) ? 1 : 0;
    pyb_mutex_unlock(&mbox->mutex);

    return has_entry;
}
/*
    similar to sys_arch_mbox_fetch, however if a message is not  present in the mailbox,
    it immediately returns with the code  SYS_MBOX_EMPTY.
*/
unsigned int pyb_mbox_tryfetch(pyb_mbox_t *mbox, void **msg)
{
    pyb_mutex_lock(&mbox->mutex, 1);

    if (mbox->entry == 0) {
        pyb_mutex_unlock(&mbox->mutex);
        return 1;
    }

    if (msg != NULL) {
        *msg = mbox->msgs[mbox->read];
    } 
    ++ mbox->read;
    if (mbox->read >= mbox->size)
        mbox->read  =0;
    mbox->entry--;
    
    if (mbox->wait_send) {
        pyb_sem_give(&mbox->have_space);
    }

    pyb_mutex_unlock(&mbox->mutex);

    return 0;

}
void pyb_mutex_init(pyb_mutex_t *m) {
    m->self = PYB_MUTEX_UNLOCKED;
    m->mutex_task = NULL;
    m->owner_nested = 0;
}

#if 1
int pyb_mutex_lock(pyb_mutex_t *m, int wait) {
    uint32_t irq_state = RAISE_IRQ_PRI();
    
    if (pyb_thread_cur == m->mutex_task) {
        m->owner_nested += 1;       
        RESTORE_IRQ_PRI(irq_state);
        return 1;
    }
    if (m->self == PYB_MUTEX_UNLOCKED) {
        // mutex is available
        m->self = PYB_MUTEX_LOCKED;
        m->mutex_task = pyb_thread_cur;
        m->owner_nested = 1;
        RESTORE_IRQ_PRI(irq_state);
        return 1;
    } else {
        // mutex is locked
        if (!wait) {
            RESTORE_IRQ_PRI(irq_state);
            return 0; // failed to lock mutex
        }
        if (m->self == PYB_MUTEX_LOCKED) {
            m->self = pyb_thread_cur;
        } else {
            for (pyb_thread_t *n = m->self;; n = n->queue_next) {
                if (n->queue_next == NULL) {
                    n->queue_next = pyb_thread_cur;
                    break;
                }
            }
        }
        pyb_thread_cur->queue_next = NULL;
        // take current thread off the run list
        pyb_thread_remove_from_runable_mutex(pyb_thread_cur);
        // thread switch will occur after we enable irqs
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
        RESTORE_IRQ_PRI(irq_state);
        // when we come back we have the mutex
    }
    return 1; // have mutex
}

void pyb_mutex_unlock(pyb_mutex_t *m) {
    uint32_t irq_state = RAISE_IRQ_PRI();
    
    if (pyb_thread_cur != m->mutex_task) {
        printf("unlock from different thread\r\n");
        RESTORE_IRQ_PRI(irq_state);
        return;
    }
    
    m->owner_nested -= 1;

    if (m->owner_nested < 0) {
        printf("unlock from same thread[%s] m:%p m->owner_nested = %d\r\n", pyb_thread_cur->name, m, m->owner_nested);
        RESTORE_IRQ_PRI(irq_state);
        return;
    }
    if (m->owner_nested > 0) {
        RESTORE_IRQ_PRI(irq_state);
        return;
    }
    if (m->self == PYB_MUTEX_LOCKED) {
        // no threads are blocked on the mutex
        m->self = PYB_MUTEX_UNLOCKED;
        m->mutex_task = NULL;
    } else {
        // at least one thread is blocked on this mutex
        pyb_thread_t *th = m->self;
        if (th->queue_next == NULL) {
            // no other threads are blocked
            m->self = PYB_MUTEX_LOCKED;
        } else {
            // at least one other thread is still blocked
            m->self = th->queue_next;
        }
        m->mutex_task = th;
        m->owner_nested = 1;
        // put unblocked thread on runable list
        pyb_thread_add_to_runable(th);
    }
    RESTORE_IRQ_PRI(irq_state);
}
#else
int pyb_mutex_lock(pyb_mutex_t *m, int wait) {
    uint32_t irq_state = RAISE_IRQ_PRI();
    if (m->self == PYB_MUTEX_UNLOCKED) {
        // mutex is available
        m->self = PYB_MUTEX_LOCKED;
        RESTORE_IRQ_PRI(irq_state);
    } else {
        // mutex is locked
        if (!wait) {
            RESTORE_IRQ_PRI(irq_state);
            return 0; // failed to lock mutex
        }
        if (m->self == PYB_MUTEX_LOCKED) {
            m->self = pyb_thread_cur;
        } else {
            for (pyb_thread_t *n = m->self;; n = n->queue_next) {
                if (n->queue_next == NULL) {
                    n->queue_next = pyb_thread_cur;
                    break;
                }
            }
        }
        pyb_thread_cur->queue_next = NULL;
        // take current thread off the run list
        pyb_thread_remove_from_runable(pyb_thread_cur);
        // thread switch will occur after we enable irqs
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
        RESTORE_IRQ_PRI(irq_state);
        // when we come back we have the mutex
    }
    return 1; // have mutex
}

void pyb_mutex_unlock(pyb_mutex_t *m) {
    uint32_t irq_state = RAISE_IRQ_PRI();
    if (m->self == PYB_MUTEX_LOCKED) {
        // no threads are blocked on the mutex
        m->self = PYB_MUTEX_UNLOCKED;
    } else {
        // at least one thread is blocked on this mutex
        pyb_thread_t *th = m->self;
        if (th->queue_next == NULL) {
            // no other threads are blocked
            m->self = PYB_MUTEX_LOCKED;
        } else {
            // at least one other thread is still blocked
            m->self = th->queue_next;
        }
        // put unblocked thread on runable list
        pyb_thread_add_to_runable(th);
    }
    RESTORE_IRQ_PRI(irq_state);
}


#endif
#endif // MICROPY_PY_THREAD

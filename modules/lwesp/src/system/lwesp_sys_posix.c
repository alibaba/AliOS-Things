/**
 * \file            lwesp_sys_posix.c
 * \brief           System dependant functions for WIN32
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
 * Author:          imi415 <imi415.public@gmail.com>
 * Version:         v1.1.2-dev
 */
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include "system/lwesp_sys.h"

#if !__DOXYGEN__

typedef void* (*lwesp_sys_posix_thread_fn) (void*);

/**
 * \brief           Custom message queue implementation for WIN32
 */
typedef struct {
    lwesp_sys_sem_t sem_not_empty;              /*!< Semaphore indicates not empty */
    lwesp_sys_sem_t sem_not_full;               /*!< Semaphore indicates not full */
    lwesp_sys_sem_t sem;                        /*!< Semaphore to lock access */
    size_t in, out, size;
    void* entries[1];
} posix_mbox_t;

/* Mutex ID for main protection */
static pthread_mutex_t sys_mutex;
static pthread_mutexattr_t sys_mutex_attr;

/**
 * \brief           Check if message box is full
 * \param[in]       m: Message box handle
 * \return          1 if full, 0 otherwise
 */
static uint8_t
mbox_is_full(posix_mbox_t* m) {
    size_t size = 0;
    if (m->in > m->out) {
        size = (m->in - m->out);
    } else if (m->out > m->in) {
        size = m->size - m->out + m->in;
    }
    return size == m->size - 1;
}

/**
 * \brief           Check if message box is empty
 * \param[in]       m: Message box handle
 * \return          1 if empty, 0 otherwise
 */
static uint8_t
mbox_is_empty(posix_mbox_t* m) {
    return m->in == m->out;
}

uint8_t
lwesp_sys_init(void) {
    if (pthread_mutexattr_init(&sys_mutex_attr) != 0) {
        return 0;
    }

    if (pthread_mutexattr_settype(&sys_mutex_attr, PTHREAD_MUTEX_RECURSIVE) != 0) {
        return 0;
    }

    /* pthread_mutex_init return 0 on success */
    if (pthread_mutex_init(&sys_mutex, &sys_mutex_attr) != 0) {
        return 0;
    }

    return 1;
}

uint32_t
lwesp_sys_now(void) {
    struct timespec tp;
    if (clock_gettime(CLOCK_MONOTONIC, &tp) != 0) {
        return 0;
    }
    uint32_t msec = (tp.tv_sec * 1000 + tp.tv_nsec / 1000000) \
                    & 0xFFFFFFFFU;

    return msec;
}

#if LWESP_CFG_OS

uint8_t
lwesp_sys_protect(void) {
    pthread_mutex_lock(&sys_mutex);
    return 1;
}

uint8_t
lwesp_sys_unprotect(void) {
    pthread_mutex_unlock(&sys_mutex);
    return 1;
}

uint8_t
lwesp_sys_mutex_create(lwesp_sys_mutex_t* p) {
    *p = malloc(sizeof(pthread_mutex_t));
    if (*p == NULL) {
        return 0;
    }

    if (pthread_mutex_init(*p, NULL) != 0) {
        free(*p);
        return 0;
    }
    return 1;
}

uint8_t
lwesp_sys_mutex_delete(lwesp_sys_mutex_t* p) {
    if (pthread_mutex_destroy(*p) != 0) {
        return 0;
    }

    free(*p);

    return 1;
}

uint8_t
lwesp_sys_mutex_lock(lwesp_sys_mutex_t* p) {
    if (pthread_mutex_lock(*p) != 0) {
        return 0;
    }
    return 1;
}

uint8_t
lwesp_sys_mutex_unlock(lwesp_sys_mutex_t* p) {
    if (pthread_mutex_unlock(*p) != 0) {
        return 0;
    }
    return 1;
}

uint8_t
lwesp_sys_mutex_isvalid(lwesp_sys_mutex_t* p) {
    if (p == NULL || *p == NULL) {
        return 0;
    }
    return 1;
}

uint8_t
lwesp_sys_mutex_invalid(lwesp_sys_mutex_t* p) {
    *p = NULL;
    return 1;
}

uint8_t
lwesp_sys_sem_create(lwesp_sys_sem_t* p, uint8_t cnt) {
    *p = malloc(sizeof(sem_t));
    if (*p == NULL) {
        return 0;
    }

    /* sem_init returns 0 on success
    * This function assumes a binary semaphore
    * should be created in some ports.
    */
    if (sem_init(*p, 0, !!cnt) != 0) {
        free(*p);
        return 0;
    }
    return 1;
}

uint8_t
lwesp_sys_sem_delete(lwesp_sys_sem_t* p) {
    if (sem_destroy(*p) != 0) {
        return 0;
    }

    free(*p);

    return 1;
}

uint32_t
lwesp_sys_sem_wait(lwesp_sys_sem_t* p, uint32_t timeout) {
    struct timespec ts;
    int ret;

    uint32_t t_start = lwesp_sys_now();

    /* Note that timedwait requires CLOCK_REALTIME, not CLOCK_MONOTONIC. */
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        return 0;
    }

    if (timeout == 0) {
        ret = sem_wait(*p);
    } else {
        /* Calculate new timespec values based on timeout. */
        time_t timeout_sec = timeout / 1000;
        time_t timeout_nsec = (timeout % 1000) * 1000000; /* 1E6 */
        ts.tv_sec += timeout_sec;
        ts.tv_nsec += timeout_nsec;
        if (ts.tv_nsec > 1000000000) { /* 1E9 */
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000;
        }

        ret = sem_timedwait(*p, &ts);
    }

    if (ret != 0) {
        return LWESP_SYS_TIMEOUT;
    }
    return lwesp_sys_now() - t_start;
}

uint8_t
lwesp_sys_sem_release(lwesp_sys_sem_t* p) {
    if (sem_post(*p) != 0) {
        return 0;
    }
    return 1;
}

uint8_t
lwesp_sys_sem_isvalid(lwesp_sys_sem_t* p) {
    if (p == NULL || *p == NULL) {
        return 0;
    }
    return 1;
}

uint8_t
lwesp_sys_sem_invalid(lwesp_sys_sem_t* p) {
    *p = NULL;

    return 1;
}

uint8_t
lwesp_sys_mbox_create(lwesp_sys_mbox_t* b, size_t size) {
    posix_mbox_t* mbox;

    *b = 0;

    mbox = malloc(sizeof(*mbox) + size * sizeof(void*));
    if (mbox != NULL) {
        memset(mbox, 0x00, sizeof(*mbox));
        mbox->size = size + 1;                  /* Set it to 1 more as cyclic buffer has only one less than size */
        lwesp_sys_sem_create(&mbox->sem, 1);
        lwesp_sys_sem_create(&mbox->sem_not_empty, 0);
        lwesp_sys_sem_create(&mbox->sem_not_full, 0);
        *b = mbox;
    }
    return *b != NULL;
}

uint8_t
lwesp_sys_mbox_delete(lwesp_sys_mbox_t* b) {
    posix_mbox_t* mbox = *b;
    lwesp_sys_sem_delete(&mbox->sem);
    lwesp_sys_sem_delete(&mbox->sem_not_full);
    lwesp_sys_sem_delete(&mbox->sem_not_empty);
    free(mbox);
    return 1;
}

uint32_t
lwesp_sys_mbox_put(lwesp_sys_mbox_t* b, void* m) {
    posix_mbox_t* mbox = *b;
    uint32_t time = lwesp_sys_now();            /* Get start time */

    lwesp_sys_sem_wait(&mbox->sem, 0);          /* Wait for access */

    /*
     * Since function is blocking until ready to write something to queue,
     * wait and release the semaphores to allow other threads
     * to process the queue before we can write new value.
     */
    while (mbox_is_full(mbox)) {
        lwesp_sys_sem_release(&mbox->sem);      /* Release semaphore */
        lwesp_sys_sem_wait(&mbox->sem_not_full, 0); /* Wait for semaphore indicating not full */
        lwesp_sys_sem_wait(&mbox->sem, 0);      /* Wait availability again */
    }
    mbox->entries[mbox->in] = m;
    if (++mbox->in >= mbox->size) {
        mbox->in = 0;
    }
    lwesp_sys_sem_release(&mbox->sem_not_empty);/* Signal non-empty state */
    lwesp_sys_sem_release(&mbox->sem);          /* Release access for other threads */
    return lwesp_sys_now() - time;
}

uint32_t
lwesp_sys_mbox_get(lwesp_sys_mbox_t* b, void** m, uint32_t timeout) {
    posix_mbox_t* mbox = *b;
    uint32_t time;

    time = lwesp_sys_now();

    /* Get exclusive access to message queue */
    if (lwesp_sys_sem_wait(&mbox->sem, timeout) == LWESP_SYS_TIMEOUT) {
        return LWESP_SYS_TIMEOUT;
    }
    while (mbox_is_empty(mbox)) {
        lwesp_sys_sem_release(&mbox->sem);
        if (lwesp_sys_sem_wait(&mbox->sem_not_empty, timeout) == LWESP_SYS_TIMEOUT) {
            return LWESP_SYS_TIMEOUT;
        }
        lwesp_sys_sem_wait(&mbox->sem, timeout);
    }
    *m = mbox->entries[mbox->out];
    if (++mbox->out >= mbox->size) {
        mbox->out = 0;
    }
    lwesp_sys_sem_release(&mbox->sem_not_full);
    lwesp_sys_sem_release(&mbox->sem);

    return lwesp_sys_now() - time;
}

uint8_t
lwesp_sys_mbox_putnow(lwesp_sys_mbox_t* b, void* m) {
    posix_mbox_t* mbox = *b;

    lwesp_sys_sem_wait(&mbox->sem, 0);
    if (mbox_is_full(mbox)) {
        lwesp_sys_sem_release(&mbox->sem);
        return 0;
    }
    mbox->entries[mbox->in] = m;
    if (mbox->in == mbox->out) {
        lwesp_sys_sem_release(&mbox->sem_not_empty);
    }
    if (++mbox->in >= mbox->size) {
        mbox->in = 0;
    }
    lwesp_sys_sem_release(&mbox->sem);
    return 1;
}

uint8_t
lwesp_sys_mbox_getnow(lwesp_sys_mbox_t* b, void** m) {
    posix_mbox_t* mbox = *b;

    lwesp_sys_sem_wait(&mbox->sem, 0);          /* Wait exclusive access */
    if (mbox->in == mbox->out) {
        lwesp_sys_sem_release(&mbox->sem);      /* Release access */
        return 0;
    }

    *m = mbox->entries[mbox->out];
    if (++mbox->out >= mbox->size) {
        mbox->out = 0;
    }
    lwesp_sys_sem_release(&mbox->sem_not_full); /* Queue not full anymore */
    lwesp_sys_sem_release(&mbox->sem);          /* Release semaphore */
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

uint8_t
lwesp_sys_thread_create(lwesp_sys_thread_t* t, const char* name,
                        lwesp_sys_thread_fn thread_func, void* const arg,
                        size_t stack_size, lwesp_sys_thread_prio_t prio) {

    pthread_t* new_thread = malloc(sizeof(pthread_t));
    if (new_thread == NULL) {
        return 0;
    }

    if (pthread_create(new_thread, NULL, (lwesp_sys_posix_thread_fn)thread_func, arg) != 0) {
        free(*t);
        return 0;
    };

    if (t != NULL) {
        *t = new_thread;
    } else {
        free(new_thread);
    }

    return 1;
}

uint8_t
lwesp_sys_thread_terminate(lwesp_sys_thread_t* t) {
    if (t != NULL && *t != NULL) {
        if (pthread_cancel(**t) != 0) {
            return 0;
        }

        free(*t);
    }

    return 1;
}

uint8_t
lwesp_sys_thread_yield(void) {
    /* Not implemented. */
    return 1;
}

#endif /* LWESP_CFG_OS */
#endif /* !__DOXYGEN__ */

/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/
#include "config.h"


#if defined(ALI_ALGO_THREADING_C)

#include "threading.h"

#if defined(ALI_ALGO_THREADING_PTHREAD)
static void  __mutex_init (osa_mutex_t *mutex)
{
    if (mutex == NULL || mutex->is_valid)
        return;

    mutex->is_valid = pthread_mutex_init(&mutex->mutex, NULL) == 0;
}

static void  __mutex_free (osa_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid)
        return;

    (void)pthread_mutex_destroy(&mutex->mutex);
    mutex->is_valid = 0;
}

static int  __mutex_lock(osa_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid)
        return (OSA_ERR_THREADING_BAD_INPUT_DATA);

    if (pthread_mutex_lock(&mutex->mutex) != 0)
        return (OSA_ERR_THREADING_MUTEX_ERROR);

    return (0);
}

static int __mutex_unlock(osa_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid)
        return (OSA_ERR_THREADING_BAD_INPUT_DATA);

    if (pthread_mutex_unlock(&mutex->mutex) != 0)
        return (OSA_ERR_THREADING_MUTEX_ERROR);

    return (0);
}

/*
 * With phtreads we can statically initialize mutexes
 */
#define MUTEX_INIT = { PTHREAD_MUTEX_INITIALIZER, 1 }

#endif /* ALI_ALGO_THREADING_PTHREAD */

#if defined(ALI_ALGO_THREADING_AOS)

extern int  aos_mutex_new(aos_mutex_t *mutex);
extern void aos_mutex_free(aos_mutex_t *mutex);
extern int  aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout);
extern int  aos_mutex_unlock(aos_mutex_t *mutex);

FORCE_COMPILE void __mutex_init(osa_mutex_t *mutex)
{
    if (mutex == NULL || mutex->is_valid) {
        return;
    }

    mutex->is_valid = (aos_mutex_new(&mutex->mutex) == 0);
}

FORCE_COMPILE void __mutex_free(osa_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid) {
        return;
    }

    aos_mutex_free(&mutex->mutex);
    mutex->is_valid = 0;
}

FORCE_COMPILE int __mutex_lock(osa_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid) {
        return (OSA_ERR_THREADING_BAD_INPUT_DATA);
    }

    if (aos_mutex_lock(&mutex->mutex, AOS_WAIT_FOREVER) != 0) {
        return (OSA_ERR_THREADING_MUTEX_ERROR);
    }

    return 0;
}

FORCE_COMPILE int __mutex_unlock(osa_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid) {
        return (OSA_ERR_THREADING_BAD_INPUT_DATA);
    }

    if (aos_mutex_unlock(&mutex->mutex) != 0) {
        return (OSA_ERR_THREADING_MUTEX_ERROR);
    }

    return (0);
}


#endif /* ALI_ALGO_THREADING_ALT */

void (*osa_mutex_init)(osa_mutex_t *) = __mutex_init;
void (*osa_mutex_free)(osa_mutex_t *) = __mutex_free;
int (*osa_mutex_lock)(osa_mutex_t *)  = __mutex_lock;
int (*osa_mutex_unlock)(osa_mutex_t *) = __mutex_unlock;


/*
 * Define global mutexes
 */
#ifndef MUTEX_INIT
#define MUTEX_INIT
#endif
osa_mutex_t osa_readdir_mutex   MUTEX_INIT;
osa_mutex_t osa_gmtime_mutex    MUTEX_INIT;

#endif /* THREADING_C */

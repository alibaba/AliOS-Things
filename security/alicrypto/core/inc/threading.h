/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/

#ifndef ALI_CRYPTO_THREADING_H
#define ALI_CRYPTO_THREADING_H

#include <stdlib.h>
#include "../config/config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OSA_ERR_THREADING_FEATURE_UNAVAILABLE         -0x001A  /**< The selected feature is not available. */
#define OSA_ERR_THREADING_BAD_INPUT_DATA              -0x001C  /**< Bad input parameters to function. */
#define OSA_ERR_THREADING_MUTEX_ERROR                 -0x001E  /**< Locking / unlocking / free failed with error code. */

#if defined(ALI_ALGO_THREADING_PTHREAD)
#include <pthread.h>
typedef struct
{
    pthread_mutex_t mutex;
    char is_valid;
} osa_mutex_t;
#endif

#if defined(ALI_ALGO_THREADING_AOS)
#include "aos/kernel.h"
typedef struct
{
    aos_mutex_t mutex;
    char is_valid;
} osa_mutex_t;

#endif /* ALI_ALGO_THREADING_AOS */

#if defined(ALI_ALGO_THREADING_C)
/*
 * The function pointers for mutex_init, mutex_free, mutex_ and mutex_unlock
 *
 * All these functions are expected to work or the result will be undefined.
 */
extern void (*osa_mutex_init)( osa_mutex_t *mutex );
extern void (*osa_mutex_free)( osa_mutex_t *mutex );
extern int (*osa_mutex_lock)( osa_mutex_t *mutex );
extern int (*osa_mutex_unlock)( osa_mutex_t *mutex );

/*
 * Global mutexes
 */
extern osa_mutex_t osa_readdir_mutex;
extern osa_mutex_t osa_gmtime_mutex;
#endif /* ALI_ALGO_THREADING_C */

#ifdef __cplusplus
}
#endif

#endif /* ALI_CRYPTO_THREADING_H*/

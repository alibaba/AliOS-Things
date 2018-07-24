/**
 * Copyright (C) 2017 The YunOS IoT Project. All rights reserved.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(MBEDTLS_DEBUG_C)
#include "mbedtls/debug.h"
#endif

#if defined(MBEDTLS_THREADING_ALT)
#include "mbedtls/threading.h"
#endif

#if defined(_WIN32)
#define FORCE_COMPILE
#else
#define FORCE_COMPILE __attribute__((__used__))
#endif


#define MBEDTLS_ALT_PRINT(_f, ...) \
    printf("%s %d: "_f, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#if defined(MBEDTLS_THREADING_ALT)

extern int  aos_mutex_new(aos_mutex_t *mutex);
extern void aos_mutex_free(aos_mutex_t *mutex);
extern int  aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout);
extern int  aos_mutex_unlock(aos_mutex_t *mutex);

FORCE_COMPILE void OSA_mutex_init(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || mutex->is_valid) {
        return;
    }

    mutex->is_valid = (aos_mutex_new(&mutex->mutex) == 0);
}

FORCE_COMPILE void OSA_mutex_free(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid) {
        return;
    }

    aos_mutex_free(&mutex->mutex);
    mutex->is_valid = 0;
}

FORCE_COMPILE int OSA_mutex_lock(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid) {
        return (MBEDTLS_ERR_THREADING_BAD_INPUT_DATA);
    }

    if (aos_mutex_lock(&mutex->mutex, AOS_WAIT_FOREVER) != 0) {
        return (MBEDTLS_ERR_THREADING_MUTEX_ERROR);
    }

    return 0;
}

FORCE_COMPILE int OSA_mutex_unlock(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid) {
        return (MBEDTLS_ERR_THREADING_BAD_INPUT_DATA);
    }

    if (aos_mutex_unlock(&mutex->mutex) != 0) {
        return (MBEDTLS_ERR_THREADING_MUTEX_ERROR);
    }

    return (0);
}

#endif /* MBEDTLS_THREADING_ALT */

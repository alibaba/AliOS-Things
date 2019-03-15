/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/* This file provides threading mutex implementation for AliOS Things. */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_THREADING_ALT)
#include <aos/kernel.h>

#include "mbedtls/threading.h"

void threading_mutex_init( mbedtls_threading_mutex_t *mutex )
{
    if( mutex == NULL || mutex->is_valid )
        return;

    mutex->is_valid = ( aos_mutex_new( &mutex->mutex ) == 0 );
}

void threading_mutex_free( mbedtls_threading_mutex_t *mutex )
{
    if( mutex == NULL || !mutex->is_valid )
        return;

    aos_mutex_free( &mutex->mutex );
    mutex->is_valid = 0;
}

int threading_mutex_lock( mbedtls_threading_mutex_t *mutex )
{
    if( mutex == NULL || !mutex->is_valid )
        return( MBEDTLS_ERR_THREADING_BAD_INPUT_DATA );

    if( aos_mutex_lock( &mutex->mutex, AOS_WAIT_FOREVER ) != 0 )
        return( MBEDTLS_ERR_THREADING_MUTEX_ERROR );

    return( 0 );
}

int threading_mutex_unlock( mbedtls_threading_mutex_t *mutex )
{
    if( mutex == NULL || ! mutex->is_valid )
        return( MBEDTLS_ERR_THREADING_BAD_INPUT_DATA );

    if( aos_mutex_unlock( &mutex->mutex ) != 0 )
        return( MBEDTLS_ERR_THREADING_MUTEX_ERROR );

    return( 0 );
}
#endif /* MBEDTLS_THREADING_ALT */


/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/* This file provides implementation of common (libc) functions that is defined
 * in platform abstraction layer for AliOS Things.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>

#if defined(MBEDTLS_PLATFORM_MEMORY)
#if defined(XTENSE_MALLOC_IRAM)
extern void *iram_heap_malloc( size_t xWantedSize );
extern void  iram_heap_free( void *pv );
extern int   iram_heap_check_addr( void *addr );

void * aos_mbedtls_calloc( size_t n, size_t size )
{
    void *buf = NULL;

    if( ( n == 0 ) || ( size == 0 ) )
        return( NULL );

    buf = iram_heap_malloc( n * size );
    if( buf == NULL )
        buf = malloc( n * size );

    if( buf == NULL )
        return( NULL );
    else
        memset( buf, 0, n * size );

    return( buf );
}

void aos_mbedtls_free( void *ptr )
{
    if( ptr == NULL )
        return;

    if( iram_heap_check_addr( ptr ) == 1 )
        iram_heap_free( ptr );
    else
        free( ptr );
}

#else /*XTENSE_MALLOC_IRAM*/

void * aos_mbedtls_calloc( size_t n, size_t size )
{
    void *buf = NULL;

    buf = aos_malloc(n * size);
    if( buf == NULL )
        return( NULL );
    else
        memset(buf, 0, n * size);

    return( buf );
}

void aos_mbedtls_free( void *ptr )
{
    if( ptr == NULL )
        return;

    aos_free( ptr );
}
#endif /*XTENSE_MALLOC_IRAM*/
#endif /*MBEDTLS_PLATFORM_MEMORY*/
#endif /*MBEDTLS_PLATFORM_C*/

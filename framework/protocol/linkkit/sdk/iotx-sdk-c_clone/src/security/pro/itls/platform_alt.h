/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef MBEDTLS_PLATFORM_ALT_H
#define MBEDTLS_PLATFORM_ALT_H

#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define mbedtls_time       time
#define mbedtls_snprintf   snprintf

typedef time_t mbedtls_time_t;

void *mbedtls_calloc( size_t n, size_t size );
void mbedtls_free( void *ptr );

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_PLATFORM_ALT_H */

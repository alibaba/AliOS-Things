/*
 *  Portable interface to the CPU cycle counter
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SELF_TEST) && defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf     printf
#endif

#if defined(MBEDTLS_TIMING_C)

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

struct _hr_time
{
    struct timeval start;
};
#include "mbedtls/timing.h"
//
//extern uint64_t krhino_sys_time_get(void);
//int gettimeofday(struct timeval *tv, void *tzp)
//{
//    uint64_t t = krhino_sys_time_get();
//
//    printf("%s: enter!\n", __FUNCTION__);
//    tv->tv_sec = t / 1000;
//    tv->tv_usec = (t % 1000) * 1000;
//    return 0;
//}

static int hardclock_init = 0;
static struct timeval tv_init;

unsigned long mbedtls_timing_hardclock( void )
{
    struct timeval tv_cur;

    printf("%s: enter!\n", __FUNCTION__);
    if( hardclock_init == 0 )
    {
        gettimeofday(&tv_init, NULL);
        hardclock_init = 1;
    }

    gettimeofday( &tv_cur, NULL );
    return( ( tv_cur.tv_sec  - tv_init.tv_sec  ) * 1000000
          + ( tv_cur.tv_usec - tv_init.tv_usec ) );
}

unsigned long mbedtls_timing_get_timer( struct mbedtls_timing_hr_time *val, int reset )
{
    struct _hr_time *t = (struct _hr_time *)val;

    printf("%s: enter!\n", __FUNCTION__);
    if( reset )
    {
        gettimeofday( &t->start, NULL );
        return( 0 );
    }
    else
    {
        unsigned long delta;
        struct timeval now;
        gettimeofday( &now, NULL );
        delta = ( now.tv_sec  - t->start.tv_sec  ) * 1000ul
              + ( now.tv_usec - t->start.tv_usec ) / 1000;
        return( delta );
    }
}

void mbedtls_timing_set_delay( void *data, uint32_t int_ms, uint32_t fin_ms )
{
    mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;

    printf("%s: enter!\n", __FUNCTION__);

    ctx->int_ms = int_ms;
    ctx->fin_ms = fin_ms;

    if( fin_ms != 0 )
        (void) mbedtls_timing_get_timer( &ctx->timer, 1 );
}

/*
 * Get number of delays expired
 */
int mbedtls_timing_get_delay( void *data )
{
    mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;
    unsigned long elapsed_ms;

    if( ctx->fin_ms == 0 )
        return( -1 );

    elapsed_ms = mbedtls_timing_get_timer( &ctx->timer, 0 );

    if( elapsed_ms >= ctx->fin_ms )
        return( 2 );

    if( elapsed_ms >= ctx->int_ms )
        return( 1 );

    return( 0 );
}
#endif /* MBEDTLS_TIMING_C */


/**
 * Copyright (C) 2017 The YunOS IoT Project. All rights reserved.
 */

#ifndef _THREADING_ALT_H_
#define _THREADING_ALT_H_

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stdlib.h>

#if CONFIG_NO_ALIOS

typedef struct {                                                                
     void *hdl;                                                                  
} aos_hdl_t;

typedef aos_hdl_t aos_mutex_t; 
#define AOS_WAIT_FOREVER    0xffffffffu

#else

#include "aos/kernel.h"

#endif

typedef struct
{
    aos_mutex_t mutex;
    char is_valid;
} mbedtls_threading_mutex_t;

void OSA_mutex_init(mbedtls_threading_mutex_t *mutex);
void OSA_mutex_free(mbedtls_threading_mutex_t *mutex);
int OSA_mutex_lock(mbedtls_threading_mutex_t *mutex);
int OSA_mutex_unlock(mbedtls_threading_mutex_t *mutex);

#endif /* _THREADING_ALT_H_*/

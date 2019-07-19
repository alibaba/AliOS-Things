/*******************************************************************************
 *
 * Copyright (c) 2013, 2014, 2015 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *******************************************************************************/

#include <network/lwm2m.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include "ulog/ulog.h"
#ifdef OSAL_RHINO
#include "aos/kernel.h"
#endif
#define TAG "lwm2m"

#ifndef LWM2M_MEMORY_TRACE

void * lwm2m_malloc(size_t s)
{
#ifdef OSAL_RHINO
    return aos_malloc(s);
#else
    return malloc(s);
#endif
}

void lwm2m_free(void * p)
{
#ifdef OSAL_RHINO
    aos_free(p);
#else
    free(p);
#endif
}

char * lwm2m_strdup(const char * str)
{
    int len = strlen(str) + 1;
    void * new = lwm2m_malloc(len);

    if(new == NULL)
        return NULL;
    return (char*) memcpy(new, str, len);
}

#endif

int lwm2m_strncmp(const char * s1,
                     const char * s2,
                     size_t n)
{
    return strncmp(s1, s2, n);
}

time_t lwm2m_gettime(void)
{
#ifdef OSAL_RHINO
    return aos_now_ms()/1000;
#else
    struct timeval tv;

    if (0 != gettimeofday(&tv, NULL))
    {
        return -1;
    }

    return tv.tv_sec;
#endif
}

void lwm2m_printf(const char * format, ...)
{
#ifdef OSAL_RHINO
    va_list ap;

    va_start(ap, format);

    LOGD(TAG, format, ap);

    va_end(ap);
#else
    va_list ap;

    va_start(ap, format);

    vfprintf(stderr, format, ap);

    va_end(ap);
#endif
}

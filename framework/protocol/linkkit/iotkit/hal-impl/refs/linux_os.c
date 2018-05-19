/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */

#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "platform.h"


#define handle_error_en(en, msg) \
        do { printf(msg); printf("%d\n", en); goto do_exit; } while (0)

void platform_printf(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	fflush(stdout);
}

/************************ memory manage ************************/


void *platform_malloc(uint32_t size)
{
	return malloc(size);
}


void platform_free(void *ptr)
{
	free(ptr);
}



/************************ mutex manage ************************/

void *platform_mutex_init(void)
{
	pthread_mutex_t *mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if (NULL == mutex)
	{
		return NULL;
	}

	if (0 != pthread_mutex_init(mutex, NULL))
	{
        free(mutex);
        return NULL;
	}

	return mutex;
}


void platform_mutex_lock(void *mutex)
{
	pthread_mutex_lock((pthread_mutex_t *)mutex);
}


void platform_mutex_unlock(void *mutex)
{
	pthread_mutex_unlock((pthread_mutex_t *)mutex);
}


void platform_mutex_destroy(void *mutex)
{
	pthread_mutex_destroy((pthread_mutex_t *)mutex);
	free(mutex);
}


/************************ semaphore manage ************************/

void *platform_semaphore_init(void)
{
	sem_t *sem = (sem_t *)malloc(sizeof(sem_t));
	if (NULL == sem)
	{
		return NULL;
	}

	if (0 != sem_init(sem, 0, 0))
	{
        free(sem);
        return NULL;
	}

	return sem;
}

int platform_semaphore_wait(_IN_ void *sem, _IN_ uint32_t timeout_ms)
{
	if (PLATFORM_WAIT_INFINITE == timeout_ms)
	{
		sem_wait(sem);
		return 0;
	}
	else
	{
		struct timespec ts;
		int s;
		/* Restart if interrupted by handler */
		do
		{
	        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
	        {
	            return -1;
	        }

	        s = 0;
	        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
	        if (ts.tv_nsec >= 1000000000)
	        {
	            ts.tv_nsec -= 1000000000;
	            s = 1;
	        }

	        ts.tv_sec += timeout_ms / 1000 + s;

		} while (((s = sem_timedwait(sem, &ts)) != 0) && errno == EINTR);

		return (s == 0) ? 0 : -1;
	}
}


void platform_semaphore_post(void *sem)
{
	sem_post((sem_t *)sem);
}


void platform_semaphore_destroy(void *sem)
{
	sem_destroy((sem_t *)sem);
	free(sem);
}

/****************************************************************************/


void platform_msleep(_IN_ uint32_t ms)
{
	usleep( 1000 * ms );
}


uint32_t platform_get_time_ms(void)
{
	struct timeval tv = { 0 };
	uint32_t time_ms;

	gettimeofday(&tv, NULL);

	time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;

	return time_ms;
}


uint64_t platform_get_utc_time(_INOUT_ uint64_t *p_utc)
{
	return (uint64_t)time((time_t *)p_utc);
}

os_time_struct *platform_local_time_r(const _IN_ uint64_t *p_utc,
        _OUT_ os_time_struct *p_result)
{
	return (os_time_struct *)localtime_r((time_t *)p_utc,(struct tm *)p_result);
}

int platform_thread_get_stack_size(_IN_ const char *thread_name)
{
    /* Note: not used by linux platform, rtos tune stack size here */
    if (0 == strcmp(thread_name, "wsf_receive_worker")) 
    {
        printf("get wsf receive worker\n");
        return 10240;
    }
    else if (0 == strcmp(thread_name, "alcs_thread"))
    {
        printf("get alcs thread\n");
        return 10240;
    } 
    else if (0 == strcmp(thread_name, "work queue"))
    {
        printf("get work queue thread\n");
        return 10240;
    } 	
		else if (0 == strcmp(thread_name, "ifttt_update_system_utc"))
    {
        printf("get ifttt_update_system_utc thread\n");
        return 10240;
    }
	else if (0 == strcmp(thread_name, "asr_websocket_thread"))
    {
        printf("get asr_websocket_thread thread\n");
        return 10240;
    }

    assert(0);
}

int platform_thread_create(
		_OUT_ void **thread,
		_IN_ const char *name,
		_IN_ void *(*start_routine) (void *),
		_IN_ void *arg,
		_IN_ void *stack,
		_IN_ uint32_t stack_size,
		_OUT_ int *stack_used)
{
 	int ret = -1;
    *stack_used = 0;

	ret = pthread_create((pthread_t *)thread, NULL, start_routine, arg);

	return ret;
}

void platform_thread_exit(void *thread)
{
	pthread_exit(0);
}

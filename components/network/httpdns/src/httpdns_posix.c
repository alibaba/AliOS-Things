/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include "aos/posix/pthread.h"
#include "aos/posix/timer.h"

#include "httpdns_opts.h"
#include "httpdns_internal.h"

void httpdns_thread_msleep(const unsigned int millisec) {
	usleep(millisec * 1000);
}

httpdns_mutex_t httpdns_mutex_create_recursive(void) {
	pthread_mutex_t * mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	int res = -1;
	pthread_mutexattr_t attr;

	if (pthread_mutexattr_init(&attr) == 0) {
		if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) == 0) {
			res = pthread_mutex_init(mutex, &attr);
		}
		pthread_mutexattr_destroy(&attr);
	}

	assert(res == 0);

	return mutex;
}

void httpdns_mutex_lock(httpdns_mutex_t mutex) {
	if (mutex) {
		pthread_mutex_lock((pthread_mutex_t *)mutex);
	}
}

void httpdns_mutex_unlock(httpdns_mutex_t mutex) {
	if (mutex) {
		pthread_mutex_unlock((pthread_mutex_t *)mutex);
	}
}

void httpdns_mutex_delete(httpdns_mutex_t mutex) {
	if (mutex) {
		pthread_mutex_destroy((pthread_mutex_t *)mutex);
		free(mutex);
	}
}

long long timespec_to_seconds(struct timespec* ts)
{
    return (ts->tv_sec + ts->tv_nsec / 1000000000);
}

void get_clock_time(struct timespec *time)
{
	clock_gettime(CLOCK_MONOTONIC, time);
}

void httpdns_resolv_thread(httpdns_connection_t *conn, httpdns_routine_t routine, int async)
{
    int ret;
    pthread_t tid;
    pthread_attr_t attr;

    if (pthread_attr_init(&attr) == 0) {
        pthread_attr_setstacksize(&attr, HTTP_DNS_TASK_STACK);
        struct sched_param sched;
        sched.sched_priority = HTTP_DNS_TASK_PRIO;
        pthread_attr_setschedparam(&attr, &sched);
        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) == 0) {
            ret = pthread_create(&tid, &attr, routine, conn);
            pthread_setname_np(tid, "httpdns");
        }
        pthread_attr_destroy(&attr);
    }

    if(0 != ret)
    {
        return;
    }

    if(async) {
        pthread_detach(tid);
    } else {
        pthread_join(tid, NULL);
    }
}

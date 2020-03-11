/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "aos/kernel.h"
#include "httpdns_internal.h"

#include "httpdns_opts.h"
#include "httpdns_internal.h"

void httpdns_thread_msleep(const unsigned int millisec) {
	aos_msleep(millisec);
}

httpdns_mutex_t httpdns_mutex_create_recursive(void) {
	aos_mutex_t mutex;
    if (0 != aos_mutex_new(&mutex)) {
        return NULL;
    }

    return mutex.hdl;
}

void httpdns_mutex_lock(httpdns_mutex_t mutex) {
	if (NULL != mutex) {
        aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
    }
}

void httpdns_mutex_unlock(httpdns_mutex_t mutex) {
	if (NULL != mutex) {
        aos_mutex_unlock((aos_mutex_t *)&mutex);
    }
}

void httpdns_mutex_delete(httpdns_mutex_t mutex) {
    if (NULL != mutex) {
        aos_mutex_free((aos_mutex_t *)&mutex);
    }
}

void get_clock_time(struct timespec *time)
{
	long long current;
	current = aos_now_ms();
    
    if (NULL != time) {
	    time->tv_sec = current / 1000;
        time->tv_nsec = current % 1000 * 1000000;
    }
}

void httpdns_resolv_thread(httpdns_connection_t *conn, httpdns_routine_t routine, int async)
{
	int res = -1;
	int stack_size, prio;
	char *name;
	aos_task_t task;

    stack_size = HTTP_DNS_TASK_STACK;
    name = "httpdns";
    prio = HTTP_DNS_TASK_PRIO;

    res = aos_task_new_ext(&task, name, routine, conn, stack_size, prio);
	assert(res == 0);
}

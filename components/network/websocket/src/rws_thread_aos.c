/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "websoc/librws.h"
#include "rws_memory.h"
#include "rws_opts.h"

#include <assert.h>
#include <unistd.h>

#include "aos/kernel.h"

struct rws_thread_struct {
	rws_thread_funct thread_function;
	void * user_object;
	void * task;
};

typedef struct _rws_threads_joiner_struct {
	rws_thread thread;
	rws_mutex mutex;
} _rws_threads_joiner;

static _rws_threads_joiner * _threads_joiner = NULL;
static void rws_threads_joiner_clean(void) { // private
	rws_thread t = _threads_joiner->thread;
	void * r = NULL;

	if (!t) {
		return;
	}
	_threads_joiner->thread = NULL;

	rws_free(t);
}

static void rws_threads_joiner_add(rws_thread thread) { // public
	rws_mutex_lock(_threads_joiner->mutex);
	rws_threads_joiner_clean();
	_threads_joiner->thread = thread;
	rws_mutex_unlock(_threads_joiner->mutex);
}

static void rws_threads_joiner_create_ifneed(void) {
	if (_threads_joiner) {
		return;
	}
	_threads_joiner = (_rws_threads_joiner *)rws_malloc_zero(sizeof(_rws_threads_joiner));
	_threads_joiner->mutex = rws_mutex_create_recursive();
}

static void * rws_thread_func_priv(void * some_pointer) {
	rws_thread t = (rws_thread)some_pointer;
	t->thread_function(t->user_object);
	rws_threads_joiner_add(t);

	return NULL;
}

rws_thread rws_thread_create(rws_thread_funct thread_function, void * user_object)
{
	rws_thread t = NULL;
	int res = -1;
	int stack_size, prio;
	char *name;

	if (!thread_function) {
		return NULL;
	}

	rws_threads_joiner_create_ifneed();
	t = (rws_thread)rws_malloc_zero(sizeof(struct rws_thread_struct));
	t->user_object = user_object;
	t->thread_function = thread_function;
    
    stack_size = WEBSOC_TASK_STACK;
    name = WEBSOC_TASK_NAME;
    prio = WEBSOC_TASK_PRIO;

    res = aos_task_new_ext((aos_task_t *)&t->task, name, &rws_thread_func_priv, (void *)t, stack_size,
                            prio);
	assert(res == 0);

	return t;
}

void rws_thread_sleep(const unsigned int millisec) {
	aos_msleep(millisec);
}

rws_mutex rws_mutex_create_recursive(void) {
	aos_mutex_t mutex;
    if (0 != aos_mutex_new(&mutex)) {
        return NULL;
    }

    return mutex.hdl;
}

void rws_mutex_lock(rws_mutex mutex) {
	if (NULL != mutex) {
        aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
    }
}

void rws_mutex_unlock(rws_mutex mutex) {
	if (NULL != mutex) {
        aos_mutex_unlock((aos_mutex_t *)&mutex);
    }
}

void rws_mutex_delete(rws_mutex mutex) {
    if (NULL != mutex) {
        aos_mutex_free((aos_mutex_t *)&mutex);
    }
}

rws_sem rws_sem_create(void)
{
    aos_sem_t sem;

    if (0 != aos_sem_new(&sem, 0)) {
        return NULL;
    }

    return sem.hdl;
}

void rws_sem_delete(rws_sem sem)
{
	aos_sem_free((aos_sem_t *)&sem);
}

void rws_sem_signal(rws_sem sem)
{
    aos_sem_signal((aos_sem_t *)&sem);
}

int rws_sem_wait(rws_sem sem, unsigned int timeout_ms)
{
	return aos_sem_wait((aos_sem_t *)&sem, timeout_ms);
}

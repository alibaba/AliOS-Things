/**
 * @file lock/lock.c  Pthread mutex locking
 *
 * Copyright (C) 2010 Creytiv.com
 */
#define _DEFAULT_SOURCE 1
#define __USE_UNIX98 1
#include <posix/pthread.h>
#include <re_types.h>
#include <re_mem.h>
#include <re_lock.h>


#define DEBUG_MODULE "lock"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


#ifndef RELEASE
#define LOCK_DEBUG 0
#endif


/** Defines a lock */
struct lock {
	pthread_mutex_t m;
};


static void lock_destructor(void *data)
{
	struct lock *l = data;

	int err = pthread_mutex_destroy(&l->m);
	if (err) {
		DEBUG_WARNING("pthread_mutex_destroy: %m\n", err);
	}
}


/**
 * Allocate a new lock
 *
 * @param lp Pointer to allocated lock object
 *
 * @return 0 if success, otherwise errorcode
 */
int lock_alloc(struct lock **lp)
{
	pthread_mutexattr_t attr;
	struct lock *l;

	if (!lp)
		return EINVAL;

	l = mem_zalloc(sizeof(*l), lock_destructor);
	if (!l)
		return ENOMEM;

	(void)pthread_mutex_init(&l->m, NULL);

	pthread_mutexattr_init(&attr);

#if LOCK_DEBUG
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	DEBUG_NOTICE("init debug lock\n");
#else
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
#endif
	pthread_mutex_init(&l->m, &attr);

	*lp = l;
	return 0;
}


/**
 * Get the lock for reading
 *
 * @param l Lock object
 */
void lock_read_get(struct lock *l)
{
	const int err = pthread_mutex_lock(&l->m);
	if (err) {
		DEBUG_WARNING("lock_read_get: %m\n", err);
	}
}


/**
 * Get the lock for writing
 *
 * @param l Lock object
 */
void lock_write_get(struct lock *l)
{
	const int err = pthread_mutex_lock(&l->m);
	if (err) {
		DEBUG_WARNING("lock_write_get: %m\n", err);
	}
}


/**
 * Attempt to get a lock for reading
 *
 * @param l Lock object
 *
 * @return 0 if success, otherwise errorcode
 */
int lock_read_try(struct lock *l)
{
	return pthread_mutex_trylock(&l->m);
}


/**
 * Attempt to get a lock for writing
 *
 * @param l Lock object
 *
 * @return 0 if success, otherwise errorcode
 */
int lock_write_try(struct lock *l)
{
	return pthread_mutex_trylock(&l->m);
}


/**
 * Release a lock
 *
 * @param l Lock object
 */
void lock_rel(struct lock *l)
{
	const int err = pthread_mutex_unlock(&l->m);
	if (err) {
		DEBUG_WARNING("lock_rel: %m\n", err);
	}
}

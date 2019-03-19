/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>
#include <mutex.h>
#include <errnox.h>

int mutex_init(mutex_t *mutex)
{
	kstat_t stat;

	if (mutex == NULL) {
		return -EINVAL;
	}

	stat = krhino_mutex_dyn_create((kmutex_t**)mutex, "app_mutex");
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}

int mutex_lock(mutex_t *mutex)
{
	kstat_t stat;

	if ((mutex == NULL) || (*mutex == NULL)) {
		return -EINVAL;
	}

	stat = krhino_mutex_lock((kmutex_t*)(*mutex), RHINO_WAIT_FOREVER);
	if (stat == RHINO_SUCCESS) {
		return 0;
	} else if (stat == RHINO_BLK_TIMEOUT) {
		return -EAGAIN;
	}

	return 0;
}

int mutex_timedlock(mutex_t *mutex, unsigned long ms)
{
	kstat_t stat;
	tick_t ticks;

	if ((mutex == NULL) || (*mutex == NULL)) {
		return -EINVAL;
	}

    ticks = krhino_ms_to_ticks(ms);
	stat = krhino_mutex_lock((kmutex_t*)(*mutex), ticks);
	if (stat == RHINO_SUCCESS) {
		return 0;
	} else if (stat == RHINO_BLK_TIMEOUT) {
		return -EAGAIN;
	}

	return -TOERRNO(stat);

}

int mutex_trylock(mutex_t *mutex)
{
	kstat_t stat;

	if ((mutex == NULL) || (*mutex == NULL)) {
		return -EINVAL;
	}

	stat = krhino_mutex_lock((kmutex_t*)(*mutex), RHINO_NO_WAIT);
	if (stat == RHINO_SUCCESS) {
		return 0;
	} else if (stat == RHINO_NO_PEND_WAIT) {
		return -EAGAIN;
	}

	return -TOERRNO(stat);
}

int mutex_unlock(mutex_t *mutex)
{
	kstat_t stat;

	if ((mutex == NULL) || (*mutex == NULL)) {
		return -EINVAL;
	}

	stat = krhino_mutex_unlock((kmutex_t*)(*mutex));
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}

int mutex_destory(mutex_t *mutex)
{
	kstat_t stat;

	if ((mutex == NULL) || (*mutex == NULL)) {
		return -EINVAL;
	}

	stat = krhino_mutex_dyn_del((kmutex_t*)(*mutex));
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}

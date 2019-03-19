/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>
#include <semaphore.h>
#include <systime.h>
#include <errnox.h>


int sem_init(sem_t *sem, unsigned int val)
{
	kstat_t stat;

	if (sem == NULL) {
		return -EINVAL;
	}

	stat = krhino_sem_dyn_create((ksem_t**)sem, "app_sem", 0);
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}

/*
 *sem_destroy() destroys the unnamed semaphore at the address pointed
 *     to by sem
 */
int sem_destory(sem_t *sem)
{
	kstat_t stat;

	if ((sem == NULL) || (*sem == NULL)) {
		return -EINVAL;
	}

	stat = krhino_sem_dyn_del((ksem_t*)(*sem));
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}

int sem_wait(sem_t *sem)
{
	kstat_t stat;

	if ((sem == NULL) || (*sem == NULL)) {
		return -EINVAL;
	}

	stat = krhino_sem_take((ksem_t*)(*sem), RHINO_WAIT_FOREVER);
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}

/* this API is not implemented */
int sem_trywait(sem_t *sem)
{
	kstat_t stat;

	if ((sem == NULL) || (*sem == NULL)) {
		return -EINVAL;
	}

	stat = krhino_sem_take((ksem_t*)(*sem), RHINO_NO_WAIT);
	if (stat == RHINO_SUCCESS) {
		return 0;
	} else if (stat == RHINO_NO_PEND_WAIT) {
		return -EAGAIN;
	}

	return -TOERRNO(stat);
}

int sem_timedwait(sem_t *sem, unsigned long ms)
{
	kstat_t stat;
	systick_t ticks;

	if ((sem == NULL) || (*sem == NULL)) {
		return -EINVAL;
	}

    ticks = krhino_ms_to_ticks(ms);

	stat = krhino_sem_take((ksem_t*)(*sem), ticks);
	if (stat == RHINO_SUCCESS) {
		return 0;
	} else if (stat == RHINO_BLK_TIMEOUT) {
		return -EAGAIN;
	}

	return -TOERRNO(stat);
}

int sem_post(sem_t *sem)
{
	kstat_t stat;

	if ((sem == NULL) || (*sem == NULL)) {
		return -EINVAL;
	}

	stat = krhino_sem_give((ksem_t*)(*sem));
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>
#include <timer.h>
#include <errnox.h>

int timer_create(timer_t *timer, timer_func func, void *arg,
                 unsigned long delay, unsigned long period, int autorun)
{
	kstat_t stat;

	if (timer == NULL) {
		return -EINVAL;
	}

	stat = krhino_timer_dyn_create((ktimer_t**)timer, "app_timer", (timer_cb_t)func,
	                               (sys_time_t)delay, (sys_time_t)period, arg, autorun);
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}

int timer_destory(timer_t *timer)
{
	kstat_t stat;
	int ret;

	if ((timer == NULL) || (*timer == NULL)) {
		return -EINVAL;
	}

	ret = timer_stop(timer);
	if (ret != 0) {
            return ret;
	}

	stat = krhino_timer_dyn_del((ktimer_t*)(*timer));
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}

int timer_start(timer_t *timer)
{
	kstat_t stat;

	if ((timer == NULL) || (*timer == NULL)) {
		return -EINVAL;
	}

	stat = krhino_timer_start((ktimer_t*)(*timer));
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}

int timer_stop(timer_t *timer)
{
	kstat_t stat;

	if ((timer == NULL) || (*timer == NULL)) {
		return -EINVAL;
	}

	stat = krhino_timer_stop((ktimer_t*)(*timer));
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}

int timer_change(timer_t *timer, unsigned long delay, unsigned long period)
{
	kstat_t stat;

	if ((timer == NULL) || (*timer == NULL)) {
		return -EINVAL;
	}

	stat = krhino_timer_change((ktimer_t*)(*timer), (sys_time_t)delay, (sys_time_t)period);
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}

int timer_arg_change(timer_t *timer, void *arg)
{
	kstat_t stat;

	if ((timer == NULL) || (*timer == NULL)) {
		return -EINVAL;
	}

	stat = krhino_timer_arg_change((ktimer_t*)(*timer), (void*)arg);
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>
#include <systime.h>
#include <errnox.h>

systick_t sys_tick(void)
{
	return krhino_sys_tick_get(); 
}

systime_t sys_time(void)
{
	return krhino_sys_time_get(); 
}

systime_t tick_to_time(systick_t ticks)
{
	return krhino_ticks_to_ms(ticks);
}

systick_t time_to_tick(systime_t ms)
{
	return krhino_ms_to_ticks(ms);
}

int msleep(systime_t ms)
{
	kstat_t stat;

	stat = krhino_task_sleep(time_to_tick(ms));
	if (stat != RHINO_SUCCESS) {
		return -TOERRNO(stat);
	}

	return 0;
}

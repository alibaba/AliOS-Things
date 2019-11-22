#include "os_common.h"
#include "os_mem.h"
#include "os_timer.h"
#include "typedef.h"
os_timer_handle_t os_timer_create( char *timer_name,u32_t period,u32_t auto_reload,void * const pvTimerID,timer_callback_t func)
{
	int ret = 0;
	os_timer__t *timer;
	
	//timer = nos_malloc(sizeof(os_timer__t*));
	//NULL_PARA_CHECK(timer,NULL);
	if(auto_reload>0)auto_reload=1;
	if(0 == auto_reload)
	{
		ret = krhino_timer_dyn_create(&timer, timer_name,func,krhino_ms_to_ticks(period), 0, pvTimerID, 0);
	}
	else
	{
		ret = krhino_timer_dyn_create(&timer, timer_name,func,period, krhino_ms_to_ticks(period), pvTimerID, 0);
	}
	if(0 != ret)
	{
		nos_free(timer);
		timer_debug_error("create timer fail,name(%s),reason:%d\r\n",(timer_name == NULL)?"NULL":timer_name,ret);
		return NULL;
	}
	else
	{
		timer_debug_info("create timer ,handle:%x name:%s period:%d\r\n",timer,(timer_name == NULL)?"NULL":timer_name,period);
		return timer;
	}
}

int os_timer_start(os_timer_handle_t timer_handle)
{
	NULL_PARA_CHECK(timer_handle,os_false);
	int ret = 0;
	char *timer_name = ((ktimer_t*)timer_handle)->name;
	ret = krhino_timer_start((ktimer_t*)timer_handle);
	if(0 == ret)
	{
		return os_true;
	}
	else
	{
		timer_debug_error("timer start fail reason:%d,handle:%x,name:%s\r\n",ret,timer_handle,(timer_name == NULL)?"NULL":timer_name);
		return os_false;
	}
}

int os_timer_stop(os_timer_handle_t timer_handle)
{
	NULL_PARA_CHECK(timer_handle,os_false);
	int ret = 0;
	char *timer_name = ((ktimer_t*)timer_handle)->name;
	ret = krhino_timer_stop((ktimer_t*)timer_handle);
	if(0 == ret)
	{
		return os_true;
	}
	else
	{
		timer_debug_error("timer stop fail reason:%d,handle:%x,name:%s\r\n",ret,timer_handle,(timer_name == NULL)?"NULL":timer_name);
		return os_false;
	}
}

int os_timer_delete(os_timer_handle_t timer_handle)
{
	NULL_PARA_CHECK(timer_handle,os_false);
	int ret = 0;
	char *timer_name = ((ktimer_t*)timer_handle)->name;
	krhino_timer_stop(timer_handle);
	ret = krhino_timer_dyn_del(timer_handle);
	if(0 == ret)
	{
		return os_true;
	}
	else
	{
		timer_debug_error("timer delete fail,%d,handle:%x,name:%s\r\n",ret,timer_handle,(timer_name == NULL)?"NULL":timer_name);
		return os_false;
	}
}

int os_timer_period_change(os_timer_handle_t timer_handle,u32_t new_period,u32_t waite_time)
{
	NULL_PARA_CHECK(timer_handle,os_false);
	int ret = 0;
	char *timer_name = ((ktimer_t*)timer_handle)->name;
	krhino_timer_stop(timer_handle);
	ret = krhino_timer_change(timer_handle,krhino_ms_to_ticks(new_period),((ktimer_t*)timer_handle)->round_ticks);
	krhino_timer_start(timer_handle);
	if(0 == ret)
	{
		return os_true;
	}
	else
	{
		timer_debug_error("timer period change fail,%d,handle:%x,name:%s\r\n",ret,timer_handle,(timer_name == NULL)?"NULL":timer_name);
		return os_false;
	}
}
int os_timer_reset(os_timer_handle_t timer_handle)
{
	NULL_PARA_CHECK(timer_handle,os_false);
	int ret = 0;
	char *timer_name = ((ktimer_t*)timer_handle)->name;
	ret = krhino_timer_stop(timer_handle);
	ret = krhino_timer_start(timer_handle);
	if(0 == ret)
	{
		return os_true;
	}
	else
	{
		timer_debug_error("timer reset fail,%d,handle:%x,name:%s\r\n",ret,timer_handle,(timer_name == NULL)?"NULL":timer_name);
		return os_false;
	}
}
void os_delay(u32_t ticks)
{
	krhino_task_sleep(krhino_ms_to_ticks(ticks));
}
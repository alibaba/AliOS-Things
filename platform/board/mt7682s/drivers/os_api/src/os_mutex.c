#include "os_common.h"
#include "os_mutex.h"
#include "typedef.h"
os_mutex_handle_t os_mutex_create(void)
{
	kmutex_t *mutex;
	int ret = 0;
	//mutex = nos_malloc(sizeof(kmutex_t*));
	//NULL_PARA_CHECK(mutex,NULL);
	ret = krhino_mutex_dyn_create(&mutex,"AOS-MUTEX");
	if(0 == ret)
	{
		return mutex;
	}
	else
	{
		mutex_debug_error("mutex create fail,reason:%d\r\n",ret);
		//nos_free(mutex);
		return NULL;
	}
}

int os_mutex_get_lock(os_mutex_handle_t mutex_handle,os_tick_type_t wait_time)
{
	NULL_PARA_CHECK(mutex_handle,os_false);
	int ret =0;
	if(OS_WAIT_FOREVER == wait_time)
	{
		ret = krhino_mutex_lock((kmutex_t*)mutex_handle,RHINO_WAIT_FOREVER);
	}
	else
	{
		ret = krhino_mutex_lock((kmutex_t*)mutex_handle,krhino_ms_to_ticks(wait_time));
	}
	if(0 != ret)
	{
		mutex_debug_error("mutex take fail,reason:%d\r\n",ret);
		return os_false;
	}
	else
	{
		return os_true;
	}
}

int os_mutex_release_lock(os_mutex_handle_t mutex_handle)
{
	NULL_PARA_CHECK(mutex_handle,os_false);
	int ret = 0;
	ret = krhino_mutex_unlock((kmutex_t*)mutex_handle);
	if(0 != ret)
	{
		mutex_debug_error("mutex take fail,reason:%d\r\n",ret);
		return os_false;
	}
	else
	{
		return os_true;
	}
}
int os_mutex_delete(os_mutex_handle_t mutex_handle)
{
	NULL_PARA_CHECK(mutex_handle,os_false);
	int ret = 0;
	ret = krhino_mutex_dyn_del((kmutex_t*)mutex_handle);
		if(0 != ret)
	{
		mutex_debug_error("mutex delete fail,reason:%d\r\n",ret);
		return os_false;
	}
	else
	{
		return os_true;
	}
}

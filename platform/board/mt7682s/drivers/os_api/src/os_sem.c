#include "os_common.h"
#include "os_sem.h"
#include "typedef.h"
os_sem_handle_t os_sem_create_counting(u32_t max_count,u32_t init_num)
{
	os_sem__t *sem=NULL;
	int ret = 0;
	sem = nos_malloc(sizeof(os_sem__t));
	NULL_PARA_CHECK(sem,NULL);
	ret = krhino_sem_create(sem, "AOS-SEM", max_count);
	if(0 != ret)
	{
		sem_debug_error("sem create fail,reason:%d\r\n",ret);
		nos_free(sem);
		return NULL;
	}
	else
	{
		krhino_sem_count_set(sem,init_num);
		return sem;
	}
}

os_sem_handle_t os_sem_create_binary(void)
{
	os_sem__t *sem=NULL;
	int ret = 0;
	sem = nos_malloc(sizeof(os_sem__t));
	NULL_PARA_CHECK(sem,NULL);
	ret = krhino_sem_create(sem, "AOS-SEM", 1);
	if(0 != ret)
	{
		sem_debug_error("sem binary create fail,reason:%d\r\n",ret);
		nos_free(sem);
		return NULL;
	}
	else
	{
		krhino_sem_count_set(sem,0);
		return sem;
	}
}

int os_take_sem(os_sem_handle_t sem_handle,os_tick_type_t waite_time)
{
	NULL_PARA_CHECK(sem_handle,os_false);
	int ret = 0;
	if(OS_WAIT_FOREVER == waite_time)
	{
		ret = krhino_sem_take((ksem_t*)sem_handle,RHINO_WAIT_FOREVER);
	}
	else
	{
		ret = krhino_sem_take((ksem_t*)sem_handle,krhino_ms_to_ticks(waite_time));
	}
	if(0 == ret)
	{
		sem_debug_info("sem take sucess,rest sem count %d\r\n",((ksem_t*)sem_handle)->count);
		return os_true;
	}
	else
	{
		sem_debug_error("sem take error reason:%d,block time:%d\r\n",ret,waite_time);
		return os_false;
	}
		
}

int os_sem_give(os_sem_handle_t sem_handle)
{
	NULL_PARA_CHECK(sem_handle,os_false);
	int ret = 0;
	if(((ksem_t*)sem_handle)->peak_count > ((ksem_t*)sem_handle)->count)
	{
		ret = krhino_sem_give((ksem_t*)sem_handle);
	}
	else
	{
		ret = -1;
	}
	if(0 == ret)
	{
		//sem_debug_info("sem give sucess,rest sem count %d\r\n",((ksem_t*)sem_handle)->count);
		return os_true;
	}
	else
	{
		sem_debug_error("sem give error reason:%d\r\n",ret);
		return os_false;
	}
	
}

int os_sem_delete(os_sem_handle_t sem_handle)
{
	NULL_PARA_CHECK(sem_handle,os_false);
	krhino_sem_del(sem_handle);
	if(sem_handle)
	{
		nos_free(sem_handle);
	}
	sem_handle = NULL;
}
#include "os_mutex.h"
#include "os_common.h"
#define test_mtuex_debug printf

static int g_count = 0;
static os_mutex_handle_t mutex_hdl=NULL;
void mutex_entry_1(void *param)
{
	while(1)
	{
		g_count++;
		os_mutex_get_lock(mutex_hdl,RHINO_WAIT_FOREVER);
		test_mtuex_debug("task 1 get lock\r\n");
		os_delay(3000);
		test_mtuex_debug("task 1 release lock\r\n");
		os_mutex_release_lock(mutex_hdl);
		if(g_count == 5)
		{
			test_mtuex_debug("test fnish\r\n");
			int ret = os_mutex_delete(mutex_hdl);
			if(os_true != ret)
			{
				test_mtuex_debug("mutex delete error\r\n");
			}
			os_task_delete(NULL);
			
		}
	}
}
void mutex_entry_2(void *param)
{
	while(1)
	{
		os_mutex_get_lock(mutex_hdl,RHINO_WAIT_FOREVER);
		test_mtuex_debug("task 2 get lock\r\n");
		os_delay(3000);
		test_mtuex_debug("task 2 release lock\r\n");
		os_mutex_release_lock(mutex_hdl);
		if(g_count == 5)
		{
			os_task_delete(NULL);
			
		}
	}
}

void os_mutex_test(void *param)
{
	int ret = 0;
	mutex_hdl = os_mutex_create();
	if(NULL == mutex_hdl)
	{
		test_mtuex_debug("create mutex fail\r\n");
	}
	ret = os_task_create(mutex_entry_1,"test1",256,"task 1 test",8,NULL);
	
	if(os_true != ret)
	{
		test_mtuex_debug("task1 create fail,reason:%d\r\n",ret);
	}
	ret = os_task_create(mutex_entry_2,"test2",256,"task 2 test",8,NULL);
	if(os_true != ret)
	{
		test_mtuex_debug("task2 create fail,reason:%d\r\n",ret);
	}
	os_task_delete(NULL);
}


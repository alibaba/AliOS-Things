#include "os_event.h"
#include "os_sem.h"
#include "os_common.h"

#define test_event_debug printf
#define BIT_0 1<<0
#define BIT_1 1<<1
#define BIT_2 1<<2
#define BIT_3 1<<3



static os_event_handle_t event_hdl = NULL;
static os_sem_handle_t sem_hdl = NULL;
void event_entry_1(void *param)
{
	int ret = 0;
	unsigned int set_bits = 0;
	while(1)
	{
		set_bits  = BIT_0|BIT_1|BIT_2|BIT_3;
		ret = os_event_set(event_hdl,set_bits);//设置所有位，接收所有位,接收成功
		if(os_true != ret)
		{
			test_event_debug("event set fail,%d\r\n",ret);
		}
	#if 1
		if(os_true != os_take_sem(sem_hdl,OS_WAIT_FOREVER))
		{
			test_event_debug("sem take error\r\n");
			os_task_delete(NULL);
		}
		set_bits = 0;
		set_bits = BIT_0|BIT_1|BIT_2;
		ret = os_event_set(event_hdl,set_bits);//设置三位，接收4位，接收失败
		if(os_true != ret)
		{
			test_event_debug("event set fail,%d\r\n",ret);
		}
		
		if(os_true != os_take_sem(sem_hdl,OS_WAIT_FOREVER))
		{
			test_event_debug("sem take error\r\n");
			os_task_delete(NULL);
		}
		set_bits = 0;
		set_bits = BIT_0|BIT_1;
		ret = os_event_set(event_hdl,set_bits);//设置四位，接收其中之一，接收成功
		if(os_true != ret)
		{
			test_event_debug("event set fail,%d\r\n",ret);
		}
		#endif
		os_task_delete(NULL);
	}
}
void event_entry_2(void *param)
{
	int ret = 0;
	unsigned int wait_bits = 0;
	unsigned int get_bits = 0;
	while(1)
	{
		wait_bits = BIT_0|BIT_1|BIT_2|BIT_3;
		ret = os_event_wait_bits(event_hdl,wait_bits,1,1,&get_bits,OS_WAIT_FOREVER);
		if(os_true != ret)
		{
			test_event_debug("get bits error,test fail\r\n");
		}
		else
		{
			test_event_debug("test recv wait all bits sucess,wait_bits:%d,get_bits:%d\r\n",wait_bits,get_bits);
		}
	#if 1
		os_sem_give(sem_hdl);
		get_bits = 0;
		printf("3333333333\r\n");
		wait_bits = (BIT_0|BIT_1|BIT_2|BIT_3);
		//u64_t time = 1000;
		ret = os_event_wait_bits(event_hdl,wait_bits,1,1,&get_bits,3000);
		if(os_true != ret)
		{
			test_event_debug("get bits out of time,test sucess,get_bits:%d\r\n",get_bits);
		}
		else
		{
			test_event_debug("test fail\r\n");
		}
		os_sem_give(sem_hdl);
		get_bits = 0;
		ret = os_event_wait_bits(event_hdl,BIT_0|BIT_1|BIT_2|BIT_3,0,1,&get_bits,3000);
		if(os_true == ret)
		{
			test_event_debug("get bits sucess,get_bits:%x\r\n",get_bits);
		}
		else
		{
			test_event_debug("test fail\r\n");
		}
	#endif
		os_task_delete(NULL);
	}
}


void os_event_test(void *param)
{
	int ret = 0;
	event_hdl = os_event_group_create();
	sem_hdl = os_sem_create_binary(); 
	if(NULL == event_hdl || NULL == sem_hdl)
	{
		test_event_debug("create event fail\r\n");
	}
	ret = os_task_create(event_entry_1,"test1",256,"task 1 test",8,NULL);
	
	if(os_true != ret)
	{
		test_event_debug("task1 create fail,reason:%d\r\n",ret);
	}
	ret = os_task_create(event_entry_2,"test2",256,"task 2 test",8,NULL);
	if(os_true != ret)
	{
		test_event_debug("task2 create fail,reason:%d\r\n",ret);
	}
	os_task_delete(NULL);
}


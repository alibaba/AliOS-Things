#include "os_sem.h"
#include "os_common.h"
#define test_sem_debug printf
static os_sem_handle_t g_sem_hdl=NULL;
static int send_flag = 0;
static int send_count = 5;
static int sem_count = 1;
static void sem_send_task(void*param)
{
	int count = 0;
	int ret = 0;
	while(1)
	{
		os_delay(1000);
		send_flag = 1;
		ret = os_sem_give(g_sem_hdl);
		if(os_true != ret)
		{
			test_sem_debug("sem give error,reason:%d\r\n",ret);
		}
		os_delay(500);
		ret = os_sem_give(g_sem_hdl);
		if(os_true != ret)
		{
			test_sem_debug("sem give error2,reason:%d\r\n",ret);
		}
		for(int i=0;i<send_count;i++)
		{
			ret = os_sem_give(g_sem_hdl);
			if(ret == os_true)
			{
				count++;
			}
			if(count>sem_count)
			{
				test_sem_debug("test sem fail,count %d\r\n",count);
			}
		}
		os_task_delete(NULL);
	}
}
static void sem_recv_task(void *param)
{
	int ret = 0;
	while(1)
	{
		ret = os_take_sem(g_sem_hdl,RHINO_WAIT_FOREVER);
		if(os_true == ret)
		{
			if(send_flag == 1)
			{
				test_sem_debug("sem take sucess,test 1 sucess\r\n");
			}
			else
			{
				test_sem_debug("sem test fail,init value is not zero\r\n");
			}
		}
		ret = os_take_sem(g_sem_hdl,2000);
		if(os_true == ret)
		{
			test_sem_debug("sem take sucess,test 1 sucess\r\n");
		}
		os_task_delete(NULL);
		
	}
}
void sem_binary_test(void*param)
{
	int ret = 0;
	g_sem_hdl = os_sem_create_binary();
	ret = os_task_create(sem_send_task,"test1",256,"task 1 test",8,NULL);
	if(os_true != ret)
	{
		test_sem_debug("task1 create fail,reason:%d\r\n",ret);
	}
	ret = os_task_create(sem_recv_task,"test2",256,"task 2 test",8,NULL);
	if(os_true != ret)
	{
		test_sem_debug("task2 create fail,reason:%d\r\n",ret);
	}
	os_task_delete(NULL);

}

#include "os_task.h"
#include "os_common.h"
#define test_task_debug printf
static os_task_handle_t g_task_handle_1=NULL;
static os_task_handle_t g_task_handle_2=NULL;
static int g_count_1=0;
static int g_count_2=0;

static void task_entry_1(void*param)
{
	char *name;
	while(1)
	{
		g_count_1++;
		name=os_get_task_name(g_task_handle_2);
		if(NULL != name)
			test_task_debug("%s,task 2 name:%s %d\r\n",param,name,g_count_1);
		else
			test_task_debug("get task 2 name fail\r\n");
		os_delay(1000);
		if(g_count_1==10)
		{
			os_task_pri_change(g_task_handle_2,6);
			test_task_debug("delete task 1\r\n");
			os_task_delete(NULL);
		}
	}
}
static void task_entry_2(void*param)
{
	char *name;
		while(1)
	{
		g_count_2++;
		name=os_get_task_name(NULL);
		test_task_debug("%s,current task name:%s,pri:%d\r\n",param,name,os_task_pri_get(NULL));
		os_delay(1000);
		if(g_count_2 == 20)
		{
			os_task_handle_t hdl = os_cur_task_handle();
			if(NULL == hdl)
			{
				test_task_debug("get current task hdl fail\r\n");
			}
			else
			{
				test_task_debug("delete task 2\r\n");
				os_task_delete(hdl);
			}
		}
	}
}

void os_test_task(void *param)
{
	int ret = 0;
	ret = os_task_create(task_entry_1,"task1",256,"task 1 running",8,&g_task_handle_1);
	if(os_true != ret || NULL == g_task_handle_1)
	{
		test_task_debug("task1 create fail,reason:%d\r\n",ret);
	}
	ret = os_task_create(task_entry_2,"task2",256,"task 2 running",8,&g_task_handle_2);
	if(os_true != ret || NULL == g_task_handle_2)
	{
		test_task_debug("task2 create fail,reason:%d\r\n",ret);
	}
	os_task_delete(NULL);
}

//临界区测试
static os_task_handle_t g_hdl_1;
static os_task_handle_t g_hdl_2;
int g_total = 100;
static void test1(void* param)
{
        int isRun = true;
        while(isRun)
        {
                os_enter_critical();
                if(g_total>0)
                {
                        printf("test1 working,rest %d tickets\r\n",g_total);
                        g_total--;
						os_delay(500);
                        printf("one tickets sale,%d rest\r\n",g_total);
                }
                else
                {
                        isRun = false;
                }
               os_exit_critical();
        }

        return 0;
}



static void test2(void* param)
{
        int isRun = true;
		int flag;
        while(isRun)
        {        
				 os_enter_critical();

                if(g_total>0)
                {
                        printf("test2 working,rest %d tickets\r\n",g_total);
                        g_total--;
						os_delay(500);
                        printf("one-- tickets sale,%d rest\r\n",g_total);
                }
                else
                {
                        isRun = false;
                }
				os_exit_critical();
        }
        return 0;
}
void task_critical_test(void *param)
{
	int ret = 0;
	ret = os_task_create(test1,"test1",256,"task 1 test",8,NULL);
	if(os_true != ret)
	{
		test_task_debug("task1 create fail,reason:%d\r\n",ret);
	}
	ret = os_task_create(test2,"test2",256,"task 2 test",8,NULL);
	if(os_true != ret)
	{
		test_task_debug("task2 create fail,reason:%d\r\n",ret);
	}
	os_task_delete(NULL);
}

//任务调度测试
void yield_entry_1(void *param)
{
	while(1)
	{
		printf("before task yield in task1\r\n");
		os_task_yield_isr();
		printf("after task yield in task1\r\n");
	}
}
void yield_entry_2(void *param)
{
	while(1)
	{
		printf("before task yield in task2\r\n");
		printf("after task yield in task2\r\n");
	}
}

void os_task_yield_test(void *param)
{
	int ret = 0;
	ret = os_task_create(yield_entry_1,"test1",256,"task 1 test",8,NULL);
	if(os_true != ret)
	{
		test_task_debug("task1 create fail,reason:%d\r\n",ret);
	}
	ret = os_task_create(yield_entry_2,"test2",256,"task 2 test",8,NULL);
	if(os_true != ret)
	{
		test_task_debug("task2 create fail,reason:%d\r\n",ret);
	}
	os_task_delete(NULL);
}


#include "os_queue.h"
#include "os_common.h"

#define test_queue_debug printf
static os_queue_handle_t g_queue_hdl=NULL;

#define QUEUE_LEN 20
static int g_send_finish=0;
typedef struct message_{
	char str[100];
	int falg;
}message_t;


void queue_entry_1(void *param)
{
	message_t message;
	int ret = 0;
	char str[100]="this is a queue test";
	message.falg = 11;
	memcpy(message.str,str,strlen(str)+1);
	while(1)
	{
		for(int i=0;i<QUEUE_LEN+1;i++)
		{
			ret = os_queue_send(g_queue_hdl,&message,1000);
			if(os_true != ret)
			{
				if(i != QUEUE_LEN)
					test_queue_debug("queue send fail,%d\r\n",i);
				else
					test_queue_debug("queue send fail because of queu full,test sucess\r\n");
			}
			os_delay(100);
		}
		g_send_finish = 1;
		os_task_delete(NULL);
	}
}
void queue_entry_2(void *param)
{
	int ret = 0;
	message_t message;
	int count = 0;
	while(1)
	{
		if(g_send_finish)
		{
			ret = os_queue_recive(g_queue_hdl,&message,RHINO_WAIT_FOREVER);
			if(os_true == ret)
			{
				count++;
				test_queue_debug("recv message:%s,%d,count:%d\r\n",message.str,message.falg,count);
			}
			else
			{
				test_queue_debug("queue recv fail\r\n");
			}
			if(count == QUEUE_LEN)
			{
				test_queue_debug("queu test sucess\r\n");
			}
		}
		os_delay(500);
	}
}


void os_queue_test(void *param)
{
	int ret = 0;
	g_queue_hdl = os_queue_create(QUEUE_LEN,sizeof(message_t));
	if(NULL == g_queue_hdl)
	{
		test_queue_debug("create mutex fail\r\n");
	}
	ret = os_task_create(queue_entry_1,"test1",256,"task 1 test",8,NULL);
	
	if(os_true != ret)
	{
		test_queue_debug("task1 create fail,reason:%d\r\n",ret);
	}
	ret = os_task_create(queue_entry_2,"test2",256,"task 2 test",8,NULL);
	if(os_true != ret)
	{
		test_queue_debug("task2 create fail,reason:%d\r\n",ret);
	}
	os_task_delete(NULL);
}



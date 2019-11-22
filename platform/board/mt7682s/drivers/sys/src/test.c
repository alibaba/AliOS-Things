#include "freertos_to_aos.h"

/**************************task api**********************************/
static TaskHandle_t task_handle1=NULL;
static TaskHandle_t task_handle2=NULL;
static void task_entry1(void *param)
{
	size_t free;
	int count = 0;
	char *task_name=NULL;
	TaskHandle_t *cur_hdl=NULL;
	cur_hdl = xTaskGetCurrentTaskHandle();
	while(1)
	{
		count++;
		if(NULL != cur_hdl)
		{
			task_name = pcTaskGetTaskName(cur_hdl);
			if(NULL != task_name)
			{
				printf("task1 name:%s\r\n",task_name);
			}
			krhino_task_stack_min_free(cur_hdl,&free);
			printf("Task1 Minmum size:%d\r\n",free);
		}
		vTaskDelay(1000);
		if(count > 10)
			vTaskDelete(task_handle2);
	}
}
static void task_entry2(void *param)
{
	int count=0;
	size_t free;
	char *task_name=NULL;
	TaskHandle_t *cur_hdl=NULL;
	cur_hdl = xTaskGetCurrentTaskHandle();
	while(1)
	{
		count++;
		if(NULL != cur_hdl)
		{
			task_name = pcTaskGetTaskName(cur_hdl);
			if(NULL != task_name)
			{
				printf("task2 name:%s\r\n",task_name);
			}
			krhino_task_stack_min_free(cur_hdl,&free);
			printf("Task2 Minmum size:%d\r\n",free);
		}
		vTaskDelay(1000);
		if(count > 20)
			vTaskDelete(task_handle1);
	}
}
void freertos_task_api_test()
{
	int count = 0;
	if(pdTRUE != xTaskCreate(task_entry1,"task_entry1",256,"param1",13,task_handle1))
	{
		printf("task_entry1 create fail\r\n");
	}
	if(pdTRUE != xTaskCreate(task_entry2,"task_entry2",256,"param2",14,task_handle2))
	{
		printf("task_entry2 create fail\r\n");
	}
	while(1)
	{
		count ++;
		if(0)
		{
			printf("delete task1\r\n");
			vTaskDelete(task_handle1);
			vTaskDelay(1000);
			printf("delete task2\r\n");
			vTaskDelete(task_handle2);
			vTaskDelay(1000);
			printf("delete NULL\r\n");
			vTaskDelete(NULL);
		}
		printf("task running\r\n");
		vTaskDelete(NULL);
		vTaskDelay(1000);
	}
}
/**************************end**********************************/

/**************************queue api**********************************/
static TaskHandle_t queue_task1=NULL;
static TaskHandle_t queue_task2=NULL;
static QueueHandle_t queue1=NULL;
static QueueHandle_t queue2=NULL;
static QueueHandle_t queue3=NULL;
#define USE_KERNEL 0
typedef struct message_{
	char str[100];
}message_t;
static void queue_task_1(void *param)
{
	char str1[50]="send queue1 sucess";
	char str2[50]="send queue2 sucess";
	char str3[50]="send queue3 sucess";
	message_t message1,message2,message3;
	strcpy(message1.str,str1);
	strcpy(message2.str,str2);
	strcpy(message3.str,str3);
	int count=0;
	int aw=55;
	int items=0;
	int ret =0;
	while(1)
	{
		count++;
		if(NULL != queue1)
		{
			//items = uxQueueMessagesWaiting(queue1);
			//printf(" queue items(%d)\r\n",items);
		#if USE_KERNEL
			ret = krhino_buf_queue_send(queue1,str1,8);
			printf("send queue %d\r\n",ret);
		#else
			if(pdTRUE != xQueueSendFromISR(queue1,&message1,sizeof(message1)))
			{
				printf("send queue1 fail\r\n");
			}
			//printf("%d items in queue,%u free\r\n",uxQueueMessagesWaiting(queue1),uxQueueSpacesAvailable(queue1));
			if(pdTRUE != xQueueSendFromISR(queue1,&message1,sizeof(message1)))
			{
				printf("send queue1 fail\r\n");
			}
			//printf("%d items in queue,%u free\r\n",uxQueueMessagesWaiting(queue1),uxQueueSpacesAvailable(queue1));
		#endif
			vTaskDelay(1000);
			
		}
		 if(NULL != queue2)
		 {
			 printf("before send queue2\r\n");
			 if(pdTRUE != xQueueSend(queue2,&message2,0))
			 {
				 printf("send queue2 fail\r\n");
			 }
			 vTaskDelay(1000);
		 }
		 if(NULL != queue3)
		 {
			 printf("before send queue3\r\n");
			 if(pdTRUE != xQueueSendToBackFromISR(queue3,&message3,0))
			 {
				 printf("send queue3 fail\r\n");
			 }
			// vTaskDelay(3000);
		 }
		vTaskDelay(1000);
		//if(count >10)
		//{
		//	vQueueDelete(queue1);
		//	vQueueDelete(queue2);
		//	vTaskDelete(NULL);
		//}
	}
}
static void recv_task_1(void *param)
{
	char str[100];
	message_t message;
	int sw=0;
	memset(str,0,sizeof(str));
	int count = 0;
	int items=0;
	while(1)
	{
		if(NULL != queue1)
		{
			//vTaskDelay(5000);
			printf("ready to recv queue\r\n");
			if(pdTRUE == xQueueReceive(queue1,&message,4000))
			{
				printf("recv queue1(%s)\r\n",message.str);
			}
			//printf("%d items in queue,%u free\r\n",uxQueueMessagesWaiting(queue1),uxQueueSpacesAvailable(queue1));
			//vTaskDelay(3000);
		}
		//vTaskDelay(1000);
	}
}
static void recv_task_2(void *param)
{
		int sw=0;
		message_t message;
	while(1)
	{
		if(NULL != queue2)
		{
			if(pdTRUE == xQueueReceive(queue2,&message,4000))
			{
				printf("recv queue2(%s)\r\n",message.str);
			}
		}
		vTaskDelay(1000);
	}
}
static void recv_task_3(void *param)
{
			char str[100];
			message_t message;
	while(1)
	{
		if(NULL != queue3)
		{
			if(pdTRUE == xQueueReceive(queue3,&message,4000))
			{
				printf("recv queue3(%s)\r\n",message.str);
			}
		}
		vTaskDelay(1000);
	}
}
void freertos_queue_api_test(void *param)
{
#if USE_KERNEL
		queue1 = aos_malloc(sizeof(kbuf_queue_t));
	queue1 = aos_malloc(sizeof(kbuf_queue_t));
	krhino_buf_queue_dyn_create(&queue1,"aos",100*sizeof(char)+1,100*sizeof(char)+1);
	krhino_buf_queue_dyn_create(&queue2,"aos",10*sizeof(int),sizeof(int));
#else
	queue1 = xQueueCreate(100,sizeof(message_t));
	queue2 = xQueueCreate(10,sizeof(message_t));
	queue3 = xQueueCreate(110,sizeof(message_t));
#endif
	if(NULL == queue1)
	{
		printf("queue1 create fail");
	}
	if(NULL == queue2)
	{
		printf("queue2 create fail");
	}
	if(NULL == queue3)
	{
		printf("queue2 create fail");
	}
	if(pdTRUE != xTaskCreate(queue_task_1,"queue_task2",256,"param1",13,queue_task1))
	{
		printf("task_entry1 create fail\r\n");
	}
	if(pdTRUE != xTaskCreate(recv_task_1,"queue_task2",256,"param2",14,queue_task2))
	{
		printf("task_entry2 create fail\r\n");
	}
		 if(pdTRUE != xTaskCreate(recv_task_2,"queue_task2",256,"param2",14,queue_task2))
	 {
		 printf("task_entry2 create fail\r\n");
	 }
	 if(pdTRUE != xTaskCreate(recv_task_3,"queue_task2",256,"param2",14,queue_task2))
	 {
		 printf("task_entry2 create fail\r\n");
	 }
	vTaskDelete(NULL);
}
/**************************end**********************************/

/**************************binary sem test**********************************/
SemaphoreHandle_t xBinarySemaphore;
SemaphoreHandle_t semphore;
void sem_send_task(void *param)
{
	while(1)
	{
	taskENTER_CRITICAL();
		vTaskDelay(100);
			int ret = xSemaphoreGive(xBinarySemaphore);
	taskEXIT_CRITICAL();
			if(ret != pdTRUE)
			{
				printf("sem send error\r\n");
			}
		xSemaphoreGive(xBinarySemaphore);
		vTaskDelay(5000);
		xSemaphoreGive(semphore);
		vTaskDelay(5000);
		//vSemaphoreDelete(semphore);
		//vSemaphoreDelete(xBinarySemaphore);
		//if(semphore)printf("-------%x------\r\n",semphore);
		 vTaskDelete(NULL);
	}
}
void sem_recv_task(void *param)
{
	int ret = 0;
	while(1)
	{
	taskENTER_CRITICAL();
			taskEXIT_CRITICAL();
		//vTaskDelay(1000);
		if(xBinarySemaphore)
		{
			ret = xSemaphoreTake( xBinarySemaphore, 9000);
			if(pdTRUE == ret)
			{
				printf("binary sem take--\r\n");
			}
		}
	}
}
void sem_recv_task_low(void *param)
{
	int ret = 0;
	while(1)
	{
		//vTaskDelay(1000);
		if(semphore)
		{
			taskENTER_CRITICAL();
			taskEXIT_CRITICAL();
			ret = xSemaphoreTake( semphore, 9000 );
			if(pdTRUE == ret)
			{
				printf("binary sem take low pri\r\n");
			}
		}

	}
}
void freertos_binary_sem_test(void*param)
{
	 xBinarySemaphore = xSemaphoreCreateBinary();
	 semphore = xSemaphoreCreateBinary();
	 if(NULL == xBinarySemaphore)
	 {
		 printf("binary sem create fail\r\n");
		 return;
	 }
	  if(pdTRUE != xTaskCreate(sem_send_task,"send-task",256,NULL,16,NULL))
	 {
		 printf("task_entry1 create fail\r\n");
	 }
	if(pdTRUE != xTaskCreate(sem_recv_task,"recv-task",256,NULL,16,NULL))
	{
		printf("task_entry2 create fail\r\n");
	}
		if(pdTRUE != xTaskCreate(sem_recv_task_low,"recv-task-low",256,NULL,16,NULL))
	{
		printf("task_entry3 create fail\r\n");
	}
	vTaskDelete(NULL);
}
/**************************end**********************************/


/**************************mutex test**********************************/

EventGroupHandle_t xmutex;
void lok_task(void *param)
{
	int ret;
	int count=0;
	while(1)
	{
		count++;
		vTaskDelay(1000);
		//xSemaphoreGive(xBinarySemaphore);
		ret = xSemaphoreTake( xmutex, portMAX_DELAY);
		if(pdTRUE == ret)
		{
			printf("task1 take lock sucess\r\n\r\n");
		}
		vTaskDelay(3000);
		if(pdTRUE != xSemaphoreGive(xmutex))
		{
			printf("sem give error\r\n");
		}
		if(count>10)
		{
			vSemaphoreDelete(xmutex);
			vTaskDelete(NULL);
		}
	}
}
void unlock_task(void *param)
{
	int ret = 0;
	while(1)
	{
		if(xmutex)
		{
			//vTaskDelay(1000);
			ret = xSemaphoreTake( xmutex, portMAX_DELAY);
			if(pdTRUE == ret)
			{
				printf("task2 take lock sucess\r\n\r\n");
			}
			vTaskDelay(3000);
			xSemaphoreGive(xmutex);
		}
		vTaskDelay(100);
	}
}
void freertos_mutex_test(void*param)
{
	 xmutex = xSemaphoreCreateMutex();
	 if(NULL == xmutex)
	 {
		 printf("binary sem create fail\r\n");
		 return;
	 }
	 if(pdTRUE != xTaskCreate(lok_task,"lock-task",256,NULL,13,NULL))
	{
		printf("task_entry1 create fail\r\n");
	}
	if(pdTRUE != xTaskCreate(unlock_task,"lock-task",256,NULL,13,NULL))
	{
		printf("task_entry2 create fail\r\n");
	}

	vTaskDelete(NULL);
}
/**************************end**********************************/

/**************************event test**********************************/
static QueueHandle_t event_handle;
EventBits_t event_bits;
#define EVENT_BIT0 (1<<0)
#define EVENT_BIT1 (1<<1)
#define EVENT_BIT2 (1<<2)
#define EVENT_BIT3 (1<<3)
void event_task1(void *param)
{
	int ret;
	while(1)
	{
		vTaskDelay(1000);
		//xSemaphoreGive(xBinarySemaphore);
		ret = xEventGroupSetBits(event_handle, EVENT_BIT0|EVENT_BIT1|EVENT_BIT2|EVENT_BIT3);
		if(pdTRUE == ret)
		{
			printf("task1 event set sucess\r\n\r\n");
		}
		printf("send event\r\n");
		vTaskDelay(3000);
		//xSemaphoreGive(event_handle);
	}
}
void event_task2(void *param)
{
	int ret = 0;
	printf("**************************\r\n");
	while(1)
	{
		//vTaskDelay(1000);
		ret = xEventGroupWaitBits(event_handle,EVENT_BIT0|EVENT_BIT1,pdTRUE,pdFALSE,portMAX_DELAY);
		if(0 != ret)
		{
			printf("task2 event get sucess(%x)\r\n\r\n",ret);
		}
		ret = xEventGroupClearBits(event_handle,EVENT_BIT0|EVENT_BIT1);
		printf("after clear bits(%x)\r\n",ret);
		//vTaskDelay(3000);
	}
}
void freertos_event_test(void*param)
{
	 event_handle = xEventGroupCreate();

	 if(NULL == event_handle)
	 {
		 printf("event group create fail\r\n");
		 return;
	 }
	 if(pdTRUE != xTaskCreate(event_task1,"event_task1",256,NULL,13,NULL))
	{
		printf("task_entry1 create fail\r\n");
	}
	if(pdTRUE != xTaskCreate(event_task2,"event_task2",256,NULL,13,NULL))
	{
		printf("task_entry2 create fail\r\n");
	}

	vTaskDelete(NULL);
}
/**************************end**********************************/

/**************************set test**********************************/
QueueHandle_t set_handle;
static QueueHandle_t queue_handle1=NULL;
static QueueHandle_t queue_handle2=NULL;
void send_task(void *param)
{
	int ret;
	char queue[]="this is a test\r\n";
	int queue2[]={11,22,33,44,55};
	while(1)
	{
		vTaskDelay(1000);
		//xSemaphoreGive(xBinarySemaphore);
		ret = xQueueSend(queue_handle1, queue,0);
		if(pdTRUE != ret)
		{
			printf("task1 queue1 send fail\r\n\r\n");
		}
		vTaskDelay(3000);
		ret = xQueueSend(queue_handle2, queue2,0);
		if(pdTRUE != ret)
		{
			printf("task1 queue2 send fail\r\n\r\n");
		}
	    // ret = xSemaphoreGive(xBinarySemaphore, 0);
		// if(pdTRUE != ret)
		// {
			// printf("task1 sem send fail\r\n\r\n");
		// }
		//xSemaphoreGive(event_handle);
	}
}
void recv_task(void *param)
{
	QueueHandle_t recv_handle;
	char recv_str[100];
	int recv_int[100];
	int ret = 0;
	while(1)
	{
		//vTaskDelay(1000);
		recv_handle = xQueueSelectFromSet(set_handle,portMAX_DELAY);
		
		if(queue_handle1 == recv_handle)
		{
			if(pdTRUE == xQueueReceive(recv_handle,recv_str,0))
			{
				printf("recv queue(%s)\r\n",recv_str);
			}
			else
			{
				printf("recv queue error(%x)\r\n",recv_handle);
			}
		}
		else if(queue_handle2 == recv_handle)
		{
			if(pdTRUE == xQueueReceive(recv_handle,recv_int,0))
			{
				printf("recv queue %d %d %d\r\n\r\n",recv_int[0],recv_int[1],recv_int[2]);
			}
		}
		else
		{
			printf("recv handle error\r\n");
		}
		//vTaskDelay(3000);
	}
}
void freertos_set_test(void *param)
{
	 set_handle = xQueueCreateSet(5);
	 queue_handle1 = xQueueCreate(100,sizeof(char));
	 queue_handle2 = xQueueCreate(100,sizeof(int));
	 if(queue_handle1 == NULL)
	 {
		 printf("queue1 create fail\r\n");
	 }
	 printf("queue addr %x\r\n",queue_handle1);
	 //xBinarySemaphore = xSemaphoreCreateBinary();
	 xQueueAddToSet(queue_handle1,set_handle);
	 xQueueAddToSet(queue_handle2,set_handle);
	
	 if(NULL == set_handle)
	 {
		 printf("binary sem create fail\r\n");
		 return;
	 }
	 if(pdTRUE != xTaskCreate(send_task,"event_task1",256,NULL,13,NULL))
	{
		printf("task_entry1 create fail\r\n");
	}
	if(pdTRUE != xTaskCreate(recv_task,"event_task2",256,NULL,13,NULL))
	{
		printf("task_entry2 create fail\r\n");
	}

	vTaskDelete(NULL);
}
/**************************end**********************************/

/**************************timer api**********************************/
static TimerHandle_t timer_handle=NULL;
static TimerHandle_t timer_handle1=NULL;
static void timer_isr(void *param,void *param1)
{
	if(param)
	printf("enter timer handler,param%x\r\n",param);
		if(param1)
	printf("enter timer handler,param1 %s\r\n",param1);
	printf("get timer id %s\r\n",pvTimerGetTimerID(param));
}
static void timer_isr1(void *param,void *param1)
{
	if(param)
	printf("enter timer2 handler,%s\r\n",param);
	if(param1)
	printf("enter timer handler,param1 %s\r\n",param1);
}
void freertos_timer_test(void *param)
{
	timer_handle = xTimerCreate("timer1",1000,pdTRUE,"TIMER1",timer_isr);
	timer_handle1 = xTimerCreate("timer2",1000,pdFALSE,"TIMER2",timer_isr1);
	printf("time handle %x,%x",timer_handle,timer_handle1);
	if(NULL != timer_handle || NULL != timer_handle1)
	{
		xTimerStart(timer_handle1,5000);
		if(pdTRUE != xTimerStart(timer_handle,5000))
		{
			printf("timer start fail\r\n");
		}
		else
		{
			printf("timer start sucess\r\n");
		}
	}
	else 
	{
		printf("timer create fail\r\n");
	}
	vTaskDelay(5000);
	if(pdFALSE == xTimerChangePeriod(timer_handle,3000,0))
	{
		printf("timer period change fail\r\n");
	}
	//vTaskDelay(5000);
	xTimerDelete(timer_handle,0);
	printf("timer init\r\n");
	vTaskDelete(NULL);
}
/**************************end**********************************/
void print_heap()
{
    extern k_mm_head *g_kmm_head;
    int               free = g_kmm_head->free_size;
    printf("============free heap size =%d==========\r\n", free);
}
void printf_task(void *param)
{
	while(1)
	{
		printf("task running\r\n");
		vTaskDelay(1000);
	}
}
int total = 100;
int cs;
#include "nvic.h"
void test1(void* param)
{
        int isRun = true;
        while(isRun)
        {
                //taskENTER_CRITICAL();
                if(total>0)
                {
                        printf("test1 working,rest %d tickets\r\n",total);
                        total--;
						vTaskDelay(5000);
                        printf("one tickets sale,%d rest\r\n",total);
                }
                else
                {
                        isRun = false;
                }
               // taskEXIT_CRITICAL();
        }

        return 0;
}



void test2(void* param)
{
        int isRun = true;
		int flag;
        while(isRun)
        {        
                 //taskENTER_CRITICAL();
                 //local_irq_save(flag)
                if(total>0)
                {
                        printf("test2 working,rest %d tickets\r\n",total);
						vTaskDelay(5000);
                        //total--;
                        printf("one-- tickets sale,%d rest\r\n",total);
                }
                else
                {
                        isRun = false;
                }
				//local_irq_restore(flag)
                //taskEXIT_CRITICAL();
        }
        return 0;
}

#include "wifi_os_api.h"
void freertos_api_test(void *param)
{
// printf("create event task\r\n");
// char *p=aos_malloc(4000);
// print_heap();
// aos_free(p);
// if(p)printf("ppppppppppp%x\r\n",p);
// print_heap();
	 // if(pdTRUE != xTaskCreate(freertos_queue_api_test,"test_task",256,"param2",8,NULL))
	 // {
		 // printf("task_entry2 create fail\r\n");
 // }
		 // if(pdTRUE != xTaskCreate(freertos_mutex_test,"test_task",256,"param2",8,NULL))
	 // {
		 // printf("task_entry2 create fail\r\n");
	 // }
	if(pdTRUE != xTaskCreate(freertos_binary_sem_test,"test_task",256,"param2",8,NULL))
	{
		printf("task_entry2 create fail\r\n");
	}
	 // if(pdTRUE != xTaskCreate(test1,"test_task0",256,"param2",8,NULL))
	  // {
		  // printf("task_entry2 create fail\r\n");
	  // }
	 // if(pdTRUE != xTaskCreate(test2,"test_task1",256,"param2",8,NULL))
	  // {
		  // printf("task_entry2 create fail\r\n");
	  // }
	 // printf("delete task\r\n");
	 // vTaskDelete(NULL);
	// printf("111111111111111111\r\n");
	while(1)
	{
		print_heap();
		//printf("state %d\r\n",wifi_os_task_get_scheduler_state());
		vTaskDelay(5000);
	}
}

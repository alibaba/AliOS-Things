#include "os_common.h"
#include "typedef.h"
#include "os_assert.h"
#include "os_task.h"
#include "os_sem.h"
#include "os_queue.h"
#include "os_timer.h"
#include "os_event.h"

#include "freertos_to_aos.h"

#define debug_error //printf

BaseType_t xTaskCreate(task_entry_t entry,const name_t *name,size_t stack_size,void *arg,uint8_t pri,TaskHandle_t *task_handle)
{
	if(pri<=RHINO_CONFIG_USER_PRI_MAX)
	{
		//if(0 == strcmp(name,"wfw"))
		//   pri = 7;
		//else
		   pri=RHINO_CONFIG_USER_PRI_MAX-pri;
	}
	else 
	{
		fos_debug_error("task max pri out of range\r\n");
		return pdFALSE;
	}
	os_task__t *task_hdl;
	if(os_true == os_task_create(entry,name,(stack_size*4),arg,pri,&task_hdl))
	{
		*task_handle = task_hdl;
		return pdTRUE;
	}
	else
	{
		fos_debug_error("task create fail\r\n");
		return pdFALSE;
	}
}
void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority )
{
	UBaseType_t pri;
	if(uxNewPriority<=RHINO_CONFIG_USER_PRI_MAX)
	{
		pri=RHINO_CONFIG_USER_PRI_MAX-uxNewPriority;
		//printf("new pri:%d,%d\r\n",uxNewPriority,pri);
	}
	else 
	{
		fos_debug_error("task max pri out of range\r\n");
		return pdFALSE;
	}
	if(os_true != os_task_pri_change(xTask,pri))
	{
		fos_debug_error("pri change fail\r\n");
	}
}

TaskHandle_t xTaskGetCurrentTaskHandle( void )
{
	return os_cur_task_handle();
}
char *pcTaskGetTaskName( TaskHandle_t xTaskToQuery )
{
	return os_get_task_name(xTaskToQuery);
}
void vTaskDelete( TaskHandle_t xTaskToDelete )
{
	if(os_true != os_task_delete(xTaskToDelete))
	{
		fos_debug_error("fail to delete task\r\n");
	}
}
void taskYIELD() 
{
	os_task_yield();
	//task_debug("task yield in (%s)\r\n",pcTaskGetTaskName(NULL));
	//cpu_intrpt_switch();
}
void portYIELD_FROM_ISR(x) 
{
	task_debug("task yield isr ----------------in (%s)\r\n",pcTaskGetTaskName(NULL));
	krhino_intrpt_enter();
    os_task_yield();
	krhino_intrpt_exit();
	//krhino_task_yield();
}

#if 1
QueueHandle_t xQueueCreate(const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize)
{
	return os_queue_create(uxQueueLength,uxItemSize);
}
#endif

BaseType_t xQueueSend(QueueHandle_t xQueue, void *pvItemToQueue,TickType_t xTicksToWait )
{
	char *name = pcTaskGetTaskName(NULL);

	if(os_true != os_queue_send(xQueue,pvItemToQueue,xTicksToWait))
	{
		fos_debug_error("queue send error in task:%s,handle:%x\r\n",(NULL == name)?"NULL":name,xQueue);
		return pdFALSE;
	}
	fos_debug_info("send queue in task:%s,handle:%x,free space:%d\r\n",(NULL == name)?"NULL":name,xQueue,uxQueueSpacesAvailable(xQueue));
	return pdTRUE;
}

BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, void *pvItemToQueue,BaseType_t * const pxHigherPriorityTaskWoken )
{
	char *name = pcTaskGetTaskName(NULL);

	if(os_true != os_queue_send(xQueue,pvItemToQueue,0))
	{
		fos_debug_error("queue send error in isr task:%s,handle:%x\r\n",(NULL == name)?"NULL":name,xQueue);
		return pdFALSE;
	}
	fos_debug_info("send queue in isr task:%s,handle:%x,free space:%d\r\n",(NULL == name)?"NULL":name,xQueue,uxQueueSpacesAvailable(xQueue));
	return pdTRUE;
}

BaseType_t xQueueSendToBack(QueueHandle_t xQueue,void * pvItemToQueue, TickType_t xTicksToWait)
 {
	 int ret = 0;
	 char *name = pcTaskGetTaskName(NULL);
	 if(os_true != os_queue_send(xQueue,pvItemToQueue,xTicksToWait))
	 {
		 fos_debug_error("queue send to back error in task:%s,handle:%x\r\n",(NULL == name)?"NULL":name,xQueue);
		 return pdFALSE;
	 }
	 fos_debug_info("send queue in isr task:%s,handle:%x:free space:%d\r\n",(NULL == name)?"NULL":name,xQueue,uxQueueSpacesAvailable(xQueue));
	 return pdTRUE;
 }
BaseType_t xQueueSendToBackFromISR(QueueHandle_t xQueue, void *pvItemToQueue,  BaseType_t * pxHigherPriorityTaskWoken )
{
	if(os_true != os_queue_send(xQueue,pvItemToQueue,0))
	{
		char *name = pcTaskGetTaskName(NULL);
		fos_debug_error("queue send isr error in task:%s,handle:%x,free space:%d\r\n",(NULL == name)?"NULL":name,xQueue,uxQueueSpacesAvailable(xQueue));
		return pdFALSE;
	}
	return pdTRUE;
}
BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvItemToQueue,TickType_t xTicksToWait )
{
	char *name=pcTaskGetTaskName(NULL);

	if(os_true != os_queue_recive(xQueue,pvItemToQueue,(TickType_t)xTicksToWait))
	{
		fos_debug_error("queue recive error in task:%s,handle:%x,block time:%d\r\n",(NULL == name)?"NULL":name,xQueue,xTicksToWait);
		return pdFALSE;
	}
	fos_debug_info("recv queue in task:%s,handle:%x,free space:%d\r\n",(NULL == name)?"NULL":name,xQueue,uxQueueSpacesAvailable(xQueue));
	return pdTRUE;
}
BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue, void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken)
{
	char *name=pcTaskGetTaskName(NULL);

	if(os_true != os_queue_recive(xQueue,pvItemToQueue,0))
	{
		fos_debug_error("queue recive error in isr task:%s,handle:%x\r\n",(NULL == name)?"NULL":name,xQueue);
		return pdFALSE;
	}
	fos_debug_info("recv queue in isr task:%s,handle:%x,free space:%d\r\n",(NULL == name)?"NULL":name,xQueue,uxQueueSpacesAvailable(xQueue));
	return pdTRUE;

}
BaseType_t xQueueReset( QueueHandle_t xQueue)
{
	int ret = 0;
	fos_debug_info("reset queue\r\n");
	ret = krhino_buf_queue_flush((kbuf_queue_t*)(((rtos_queue_t*)xQueue)->aos_queue));
	if(0 == ret)
		return pdTRUE;
	else
		return pdFALSE;
}
void vQueueDelete( QueueHandle_t xQueue )
{
	os_queue_delete(xQueue);
}

UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue )
{
	return os_queue_used_nums(xQueue);
}
UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue )
{
	return os_queue_free_nums(xQueue);	
}

static int MAX_SET_ITEMS;
static rtos_set_t *g_queue_set;
static int rtos_set_items=0;
QueueSetHandle_t xQueueCreateSet( const UBaseType_t uxEventQueueLength )
{
	g_queue_set = pvPortMalloc(uxEventQueueLength*sizeof(rtos_set_t));
	NULL_PARA_CHECK(g_queue_set,NULL);
	memset(0,g_queue_set,sizeof(g_queue_set));
	MAX_SET_ITEMS = uxEventQueueLength;
	return g_queue_set;
}

void vQueueAddToRegistry( QueueHandle_t xQueue, const char *pcQueueName )
{
	//FA_DEBUG("queue add to registry\r\n");
}
BaseType_t xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet )
{
	int i=0;
	int type = 0;
	NULL_PARA_CHECK(xQueueOrSemaphore,pdFALSE);
	NULL_PARA_CHECK(xQueueSet,pdFALSE);
	if(RHINO_EVENT_OBJ_TYPE == (((kevent_t*)xQueueOrSemaphore)->blk_obj).obj_type)
	{
		type = EVENT;
	}
	else if(RHINO_BUF_QUEUE_OBJ_TYPE == (((kevent_t *)xQueueOrSemaphore)->blk_obj).obj_type)
	{
		type = QUEUE;
	}
	else if(RHINO_SEM_OBJ_TYPE == (((kevent_t *)xQueueOrSemaphore)->blk_obj).obj_type)
	{
		type = SEM;
	}
	else 
	{
		printf("queue add to set type error(%d)\r\n",(((ksem_t*)xQueueOrSemaphore)->blk_obj).obj_type);
		return pdFALSE;
	}
	for(i=0;i<MAX_SET_ITEMS;i++)
	{
		if(0 == (((rtos_set_t*)xQueueSet)[i]).type)
		{
			(((rtos_set_t*)xQueueSet)[i]).type = type;
			(((rtos_set_t*)xQueueSet)[i]).handle = xQueueOrSemaphore;
			rtos_set_items++;
			//FA_DEBUG("queue type(%d)(%d) %x\r\n",i,(((rtos_set_t*)xQueueSet)[i]).type,(((rtos_set_t*)xQueueSet)[i]).handle);
			return pdTRUE;
		}
	}
	fos_debug_error("queue add to set fail\r\n");
	return pdFALSE;
	
}

QueueSetMemberHandle_t xQueueSelectFromSet( QueueSetHandle_t xQueueSet, TickType_t const xTicksToWait )
{
	NULL_PARA_CHECK(xQueueSet,pdFALSE);
	int i=0,wait_count=0;
	while(1)
	{
		for(i=0;i<rtos_set_items;i++)
		{
			if(QUEUE == (((rtos_set_t*)xQueueSet)[i]).type)
			{
				if(0 < os_queue_used_nums((((rtos_set_t*)xQueueSet)[i]).handle))
				{
					//printf("find select type queue\r\n");
					return (((rtos_set_t*)xQueueSet)[i]).handle;
				}
			}
			else if(EVENT == (((rtos_set_t*)xQueueSet)[i]).type)
			{
				
			}
			else if(SEM == (((rtos_set_t*)xQueueSet)[i]).type)
			{
				if(((ksem_t*)(((rtos_set_t*)xQueueSet)[i]).handle)->count > 0)
				{
					//printf("find sem type(%d)(%d) %x\r\n",i,(((rtos_set_t*)xQueueSet)[i]).type,(((rtos_set_t*)xQueueSet)[i]).handle);
					return (((rtos_set_t*)xQueueSet)[i]).handle;
				}
			}
		}
		aos_msleep(10);
		wait_count+=10;
		if(wait_count>=xTicksToWait)
			break;
	}
	//FA_DEBUG("no handle to select\r\n");
	return NULL;
}



#if 1
static unsigned char g_sem_binary = 1;
QueueHandle_t xSemaphoreCreateBinary()
{
	return os_sem_create_binary();	
}
#endif
#define SEM_BIT0_SET (1<<0)
static int g_mutex_flag=0;
BaseType_t xSemaphoreTake(QueueHandle_t xSemaphore,TickType_t xBlockTime )
{
	int ret = -1;
	int wait_bits = 0;
	if(RHINO_MUTEX_OBJ_TYPE == (((kmutex_t*)xSemaphore)->blk_obj).obj_type)
	{
		//ret = os_mutex_get_lock(xSemaphore,xBlockTime);
		ret = os_true;
		
	}
	else if(RHINO_SEM_OBJ_TYPE == (((ksem_t*)xSemaphore)->blk_obj).obj_type)
	{
		ret = os_take_sem(xSemaphore,xBlockTime);
	}
	else
	{
		fos_debug_error("sem take type error(%d)\r\n",(((kmutex_t*)xSemaphore)->blk_obj).obj_type);
		return pdFALSE;
	}
	if(os_true != ret)
	{
		fos_debug_error("sem take error\r\n");
		return pdFALSE;
	}
	return pdTRUE;
		
}

BaseType_t xSemaphoreGive(QueueHandle_t xSemaphore )
{
	int ret = 0;
	if(RHINO_MUTEX_OBJ_TYPE == (((kmutex_t*)xSemaphore)->blk_obj).obj_type)
	{
		//ret = os_mutex_release_lock((kmutex_t*)xSemaphore);
		ret = os_true;
	}
	else if(RHINO_SEM_OBJ_TYPE == (((ksem_t*)xSemaphore)->blk_obj).obj_type)
	{
		ret = os_sem_give((ksem_t*)xSemaphore);
	}
	else
	{
		fos_debug_error("sem give type error(%d)\r\n",(((kmutex_t*)xSemaphore)->blk_obj).obj_type);
	}
	if(os_true != ret)
	{
		char *name = os_get_task_name(NULL);
		fos_debug_error("sem give error in task: %s reason:%d\r\n",(name==NULL)?"NULL":name,ret);
		return pdFALSE;
	}
	else
	{
		return pdTRUE;
	}
}
BaseType_t xSemaphoreGiveFromISR(QueueHandle_t xSemaphore,BaseType_t *pxHigherPriorityTaskWoken)
{
	int ret = 0;
	if(RHINO_MUTEX_OBJ_TYPE == (((kmutex_t*)xSemaphore)->blk_obj).obj_type)
	{
		//ret = os_mutex_release_lock((kmutex_t*)xSemaphore);
		ret = os_true;
	}
	else if(RHINO_SEM_OBJ_TYPE == (((ksem_t*)xSemaphore)->blk_obj).obj_type)
	{
		ret = os_sem_give((ksem_t*)xSemaphore);
	}
	else
	{
		fos_debug_error("sem give type error(%d)\r\n",(((kmutex_t*)xSemaphore)->blk_obj).obj_type);
	}
	if(os_true != ret)
	{
		fos_debug_error("sem give error %d\r\n",ret);
		return pdFALSE;
	}
	else
	{
		return pdTRUE;
	}

}
void vSemaphoreDelete( QueueHandle_t xSemaphore )
{
	int ret = 0;
	if(RHINO_MUTEX_OBJ_TYPE == (((kmutex_t*)xSemaphore)->blk_obj).obj_type)
	{
		os_mutex_delete(xSemaphore);
	}
	else if(RHINO_SEM_OBJ_TYPE == (((kmutex_t*)xSemaphore)->blk_obj).obj_type)
	{
		os_sem_delete(xSemaphore);
	}
	else 
	{
		fos_debug_error("free sem type error %d\r\n",(((kmutex_t*)xSemaphore)->blk_obj).obj_type);
		return;
	}
}
#if 1
QueueHandle_t xSemaphoreCreateMutex(  )
{
	return os_mutex_create();
}
#endif

EventGroupHandle_t xEventGroupCreate( void )
{
	return os_event_group_create();
}
EventBits_t xEventGroupWaitBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait )
{
	int ret = 0;
	os_event_bits_t event_bits = 0;
	ret = os_event_wait_bits(xEventGroup,uxBitsToWaitFor,xWaitForAllBits,xClearOnExit,&event_bits,xTicksToWait);
	if(os_true != ret)
	{
		fos_debug_error("event_bits get error\r\n");
	}
	printf("event bits %d\r\n",event_bits);
	return event_bits;
}
EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet )
{
	os_event_set(xEventGroup,uxBitsToSet);
	return ((kevent_t*)xEventGroup)->flags;
}
EventBits_t xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear )
{
	os_event_set(xEventGroup,0);
	return ((kevent_t*)xEventGroup)->flags;
}
void *pvPortMalloc( size_t xWantedSize )
{
	char *p;
	p = krhino_mm_alloc(xWantedSize);
	return p;
}
static size_t xBlockAllocatedBit = 0;

void *pvPortMallocNC( size_t xWantedSize )
{
#ifdef HAL_CACHE_WITH_REMAP_FEATURE

/*
      head        res            xBlockAlignWantedSize         res
    |_____|________|______________________|________|
    p1     p2     p3     p4

    res is a const value: portCacheline_ALIGNMENT - portBYTE_ALIGNMENT, 
    the first res is to confirm this non-cacheable block is located at the different cache line compared with the front heap block
    the second res is to confirm this non-cacheable block is located at the differet cache line compared with the next heap block

    p1: block begin address
    p2: return address of pvPortMalloc
    p3: cache line align address, which is the begin of the cache line invalidate operation
    p4: user address,which is equal to p2 + res(portCacheline_ALIGNMENT - portBYTE_ALIGNMENT)
*/
    const size_t xResSize =  portCacheline_ALIGNMENT - portBYTE_ALIGNMENT; /* res */
    size_t xBlockAlignWantedSize = 0;
    void *pvReturn = NULL;          /* p2*/
    uint32_t xCacheAlignAddr;       /* p3 */
    uint32_t xUserAddr;             /* p4 */
    uint32_t xInvalidLength;
    if( ( xWantedSize & xBlockAllocatedBit ) == 0 )
    {
        /* The wanted size is increased so it can contain a BlockLink_t
        structure in addition to the requested amount of bytes. */
        if( xWantedSize > 0 )
        {
            xBlockAlignWantedSize = xWantedSize;
        	/* Ensure that blocks are always aligned to the required number of bytes. */
        	if( ( xBlockAlignWantedSize & portBYTE_ALIGNMENT_MASK ) != 0x00 )
        	{
        		/* Byte alignment required. */
        		xBlockAlignWantedSize += ( portBYTE_ALIGNMENT - ( xBlockAlignWantedSize & portBYTE_ALIGNMENT_MASK ) );
        		configASSERT( ( xBlockAlignWantedSize & portBYTE_ALIGNMENT_MASK ) == 0 );
        	}
        	else
        	{
        		mtCOVERAGE_TEST_MARKER();
        	}
            /* Allocate a block from heap memory */
            pvReturn = pvPortMalloc(xBlockAlignWantedSize + xResSize * 2);
        }
        else
        {
        	mtCOVERAGE_TEST_MARKER();
        }   
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }
    
    /* directly return if allocate fail */
    if(pvReturn == NULL) 
    {
        return pvReturn;
    }
    /* round up to cache line align size for invalidation */    
    xCacheAlignAddr = ((uint32_t)pvReturn + portCacheline_ALIGNMENT - 1) & ~(portCacheline_ALIGNMENT - 1); /* p3 */
    xUserAddr = (uint32_t)pvReturn + xResSize;      /* p4 = p2 + res */
    configASSERT(xCacheAlignAddr <= xUserAddr);     /* p3 <= p4 */
    
    xInvalidLength = (xUserAddr - xCacheAlignAddr + xBlockAlignWantedSize + portCacheline_ALIGNMENT - 1) & ~(portCacheline_ALIGNMENT - 1); /* (p4 - p3 + xBlockAlignWantedSize) round up to cache line aligne size */
    configASSERT((xCacheAlignAddr + xInvalidLength) <= (xUserAddr + xBlockAlignWantedSize + xResSize)); /* (p3 + xInvalidLength) <= (p4 + xBlockAlignWantedSize + res) */

    /* do invalidation*/
    if(HAL_CACHE_STATUS_OK != hal_cache_invalidate_multiple_cache_lines(xCacheAlignAddr, xInvalidLength))
    {
        configASSERT(0);
    }

    /* change to non-cacheable address */
	xUserAddr = HAL_CACHE_VIRTUAL_TO_PHYSICAL(xUserAddr);

    return (void*)xUserAddr;
#else
    return pvPortMalloc(xWantedSize);
#endif /* HAL_CACHE_WITH_REMAP_FEATURE */
}


void vPortFreeNC( void *pv )
{
#ifdef HAL_CACHE_WITH_REMAP_FEATURE
/*
      head        res        xBlockAlignWantedSize         res
    |_____|________|______________________|________|
    p1     p2     p3     p4
    
    p2 = p4 - res
*/
    const uint32_t xResSize =  portCacheline_ALIGNMENT - portBYTE_ALIGNMENT; /* res */
    uint32_t xAddr;

    if(pv != NULL)
    {
        xAddr = (uint32_t)pv - xResSize; /* p2 */
        
        /* check address is cacheable or not, if yes, then assert */
		configASSERT(pdFALSE == hal_cache_is_cacheable(xAddr));

        /* change to virtual address */
		xAddr = HAL_CACHE_PHYSICAL_TO_VIRTUAL(xAddr);

        /* free */  
        vPortFree((void*)xAddr);
    }

#else
    vPortFree(pv);
#endif /* HAL_CACHE_WITH_REMAP_FEATURE*/
}

void *pvPortCalloc( size_t nmemb, size_t size )
{
	return krhino_mm_alloc(size*nmemb);
}
void vPortFree( void *pv )
{
	if(NULL!=pv)
	{
		krhino_mm_free(pv);
	}
}
#if 1
size_t xPortGetFreeHeapSize( void )
{
	return g_kmm_head->free_size;
}
#endif
BaseType_t xTaskGetSchedulerState( void )
{
    if (g_sys_stat == RHINO_RUNNING) {
        return taskSCHEDULER_RUNNING;
    }
	else if(g_sys_stat == RHINO_STOPPED)
	{
		return taskSCHEDULER_NOT_STARTED;
	}
	else 
	{
		return taskSCHEDULER_SUSPENDED;
	}

}

TimerHandle_t xTimerCreate( char * pcTimerName, uint32_t xTimerPeriodInTicks, UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction )
{
#if 0
	if(0 ==strcmp(pcTimerName,"contextSwitch"))
	{
		printf("skip contextSwitch,auto reload:%d,period:%d\r\n",uxAutoReload,xTimerPeriodInTicks);
		return;
	}
#endif
	return os_timer_create(pcTimerName,xTimerPeriodInTicks,uxAutoReload,pvTimerID,pxCallbackFunction);
}
BaseType_t xTimerStart(TimerHandle_t xTimer,TickType_t xTicksToWait )
{
	int ret;
	ret = os_timer_start((ktimer_t*)xTimer);
	//printf("start timer:%s\r\n",((ktimer_t*)xTimer)->name);
	if(os_true == ret)
	{
		return pdTRUE;
	}
	else
	{
		fos_debug_error("start timer error,name:%s\r\n",((ktimer_t*)xTimer)->name);
		return pdFALSE;
	}
}
BaseType_t xTimerStop(TimerHandle_t xTimer,TickType_t xTicksToWait )
{
	int ret;
	ret = os_timer_stop(xTimer);
	//printf("stop timer:%s\r\n",((ktimer_t*)xTimer)->name);
	if(os_true == ret)
		return pdTRUE;
	else
		return pdFALSE;
}
BaseType_t xTimerDelete(TimerHandle_t xTimer, TickType_t xTicksToWait )
{
	int ret;
	ret = os_timer_delete(xTimer);
	//printf("delete timer:%s\r\n",((ktimer_t*)xTimer)->name);
	if(os_true == ret)
	{
		return pdTRUE;
	}
	else
	{
		fos_debug_error("delete timer error,name:%s\r\n",((ktimer_t*)xTimer)->name);
		return pdFALSE;
	}
}
BaseType_t xTimerReset( TimerHandle_t xTimer, const TickType_t xTicksToWait )
{
	int ret = 0;
	NULL_PARA_CHECK(xTimer,pdFALSE);
	//printf("reset timer:%s\r\n",((ktimer_t*)xTimer)->name);
	ret = krhino_timer_stop(xTimer);
	ret = krhino_timer_start(xTimer);
	if(0 == ret)
		return pdTRUE;
	else
		return pdFALSE;
}
BaseType_t xTimerChangePeriod(TimerHandle_t xTimer,TickType_t xNewPeriod,TickType_t xTicksToWait )
{
	int ret;
	ret = os_timer_period_change(xTimer,xNewPeriod,xTicksToWait);
	//printf("change timer period:%s\r\n",((ktimer_t*)xTimer)->name);
	if(os_true == ret)
		return pdTRUE;
	else
		return pdFALSE;
}
void *pvTimerGetTimerID( const TimerHandle_t xTimer )
{
	//printf("get timer id:%s,%s\r\n",((ktimer_t*)xTimer)->name);

	if(NULL != xTimer)
		return ((ktimer_t*)xTimer)->timer_cb_arg;
}
void vTaskDelay(int ms)
{
	os_delay(ms);
}

void vPortEnterCritical( void )
{
	os_enter_critical();
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	os_exit_critical();
}

void portSET_INTERRUPT_MASK_FROM_ISR()
{
	krhino_intrpt_enter();
}
void portCLEAR_INTERRUPT_MASK_FROM_ISR()
{
	krhino_intrpt_exit();
}



#include "aos_common.h"
#include "typedef.h"
#include "aos_assert.h"
#include "aos_task.h"
#include "aos_sem.h"
#include "aos_queue.h"
#include "aos_timer.h"
#include "aos_event.h"
#include "aos_mutex.h"
#include "k_api.h"
#include "freertos_to_aos.h"
#include <stdlib.h>

#define debug_error //printf

#ifndef os_true
#define os_true 1
#endif

#define OS_TASK_PRI_ADAPTER     (RHINO_CONFIG_USER_PRI_MAX - 25)   //according to configMAX_PRIORITIES

extern char *_os_get_task_name(os_task_handle_t task_handle);

int FreeRTOS_errno = 0;
extern volatile int xSchedulerRunning[RHINO_CONFIG_CPU_NUM];

BaseType_t xTaskCreate(task_entry_t entry,const name_t *name,size_t stack_size,void *arg,uint8_t pri,TaskHandle_t *task_handle)
{
    os_task_t *task_hdl;
    uint16_t name_len;
    uint8_t *task_name = NULL;

    if(pri<=OS_TASK_PRI_ADAPTER)
    {
        pri=OS_TASK_PRI_ADAPTER -pri;
    }
    else
    {
        fos_debug_error("task max pri out of range\r\n");
        return pdFALSE;
    }

    if (name != NULL) {
        name_len = strlen(name) + 1;
        if (name_len > configMAX_TASK_NAME_LEN) {
            name_len = configMAX_TASK_NAME_LEN;
        }
        task_name = malloc(name_len);
        if (task_name != NULL) {
            memset(task_name, 0, name_len);
            strncpy(task_name, name, name_len - 1);
        }
        name = task_name;
    }

    if(os_true == _os_task_create(entry,name,stack_size,arg,pri,&task_hdl))
    {
        if(task_handle){
            *task_handle = task_hdl;
        }
        return pdTRUE;
    }
    else
    {
        fos_debug_error("task create fail\r\n");
        if (task_name != NULL) {
            free(task_name);
        }
        return pdFALSE;
    }
}
void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority )
{
    UBaseType_t pri;
    if(uxNewPriority<=OS_TASK_PRI_ADAPTER)
    {
        pri=OS_TASK_PRI_ADAPTER-uxNewPriority;
    }
    else
    {
        fos_debug_error("task max pri out of range\r\n");
        return;
    }
    if(os_true != _os_task_pri_change(xTask,pri))
    {
        fos_debug_error("pri change fail\r\n");
    }
}

TaskHandle_t xTaskGetCurrentTaskHandle( void )
{
    return (TaskHandle_t)_os_cur_task_handle();
}
char *pcTaskGetTaskName( TaskHandle_t xTaskToQuery )
{
    return _os_get_task_name(xTaskToQuery);
}
void vTaskDelete( TaskHandle_t xTaskToDelete )
{
    ktask_t * ptask;

    ptask = xTaskToDelete;
    if (ptask == NULL) {
        ptask = (ktask_t *)_os_cur_task_handle();
    }

    if ((ptask != NULL) && ((ptask->task_name) != NULL)) {
        free((void*)ptask->task_name);
        ptask->task_name = NULL;
    }

    if(os_true != _os_task_delete(xTaskToDelete))
    {
        fos_debug_error("fail to delete task\r\n");
    }
}

void taskYIELD(void)
{
    _os_task_yield();
}

void portYIELD_FROM_ISR(BaseType_t xHigherPriorityTaskWoken)
{
    if (xHigherPriorityTaskWoken != pdFALSE) {
        core_sched();
    }
}

#if 1
QueueHandle_t xQueueCreate(const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize)
{
	return (QueueHandle_t)_os_queue_create(uxQueueLength,uxItemSize);
}
#endif

BaseType_t xQueueSend(QueueHandle_t xQueue, void *pvItemToQueue,TickType_t xTicksToWait )
{
    char *name = pcTaskGetTaskName(NULL);

    if(os_true != _os_queue_send(xQueue,pvItemToQueue,xTicksToWait))
    {
        fos_debug_error("queue send error in task:%s,handle:%p\r\n",(NULL == name)?"NULL":name,xQueue);
        return pdFALSE;
    }
    fos_debug_info("send queue in task:%s,handle:%p,free space:%d\r\n",(NULL == name)?"NULL":name,xQueue,uxQueueSpacesAvailable(xQueue));
    return pdTRUE;
}

BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, void *pvItemToQueue,BaseType_t * const pxHigherPriorityTaskWoken )
{
    char *name = pcTaskGetTaskName(NULL);

    if(os_true != _os_queue_send(xQueue,pvItemToQueue,0))
    {
        fos_debug_error("queue send error in isr task:%s,handle:%p\r\n",(NULL == name)?"NULL":name,xQueue);
        return pdFALSE;
    }
    fos_debug_info("send queue in isr task:%s,handle:%p,free space:%d\r\n",(NULL == name)?"NULL":name,xQueue,uxQueueSpacesAvailable(xQueue));
    return pdTRUE;
}

BaseType_t xQueueSendToBack(QueueHandle_t xQueue,void * pvItemToQueue, TickType_t xTicksToWait)
 {
    int ret = 0;
    char *name = pcTaskGetTaskName(NULL);
    if(os_true != _os_queue_send(xQueue,pvItemToQueue,xTicksToWait))
    {
        fos_debug_error("queue send to back error in task:%s,handle:%p\r\n",(NULL == name)?"NULL":name,xQueue);
        return pdFALSE;
    }
    fos_debug_info("send queue in isr task:%s,handle:%p:free space:%d\r\n",(NULL == name)?"NULL":name,xQueue,uxQueueSpacesAvailable(xQueue));
    return pdTRUE;
 }
BaseType_t xQueueSendToBackFromISR(QueueHandle_t xQueue, void *pvItemToQueue,  BaseType_t * pxHigherPriorityTaskWoken )
{
    if(os_true != _os_queue_send(xQueue,pvItemToQueue,0))
    {
        char *name = pcTaskGetTaskName(NULL);
        fos_debug_error("queue send isr error in task:%s,handle:%p,free space:%ld\r\n",(NULL == name)?"NULL":name,xQueue,uxQueueSpacesAvailable(xQueue));
        return pdFALSE;
    }
    return pdTRUE;
}
BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvItemToQueue,TickType_t xTicksToWait )
{
    char *name=pcTaskGetTaskName(NULL);
    int ret = _os_queue_recive(xQueue,pvItemToQueue,(TickType_t)xTicksToWait);

    if(RHINO_NO_PEND_WAIT == ret)
    {
        return pdFALSE;
    }
    else if(os_true != ret)
    {
        /*  fos_debug_error("queue recive error in task:%s,handle:%x,block time:%d\r\n",(NULL == name)?"NULL":name,xQueue,xTicksToWait);*/
        return pdFALSE;
    }
    fos_debug_info("recv queue in task:%s,handle:%p,free space:%d\r\n",(NULL == name)?"NULL":name,xQueue,uxQueueSpacesAvailable(xQueue));
    return pdTRUE;
}
BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue, void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken)
{
    printf("Error! queue recive error in isr handle:%p\r\n",xQueue);
    return pdFALSE;
}

void vQueueDelete( QueueHandle_t xQueue )
{
    _os_queue_delete(xQueue);
}

UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue )
{
    return _os_queue_used_nums(xQueue);
}
UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue )
{
    return _os_queue_free_nums(xQueue);
}

static int MAX_SET_ITEMS;
static rtos_set_t *g_queue_set;
static int rtos_set_items=0;
QueueSetHandle_t xQueueCreateSet( const UBaseType_t uxEventQueueLength )
{
    g_queue_set = pvPortMalloc(uxEventQueueLength*sizeof(rtos_set_t));
    NULL_PARA_CHECK(g_queue_set,NULL);
    memset(g_queue_set, 0, sizeof(*g_queue_set));
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
                if(0 < _os_queue_used_nums((((rtos_set_t*)xQueueSet)[i]).handle))
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
    return (QueueHandle_t)_os_sem_create_binary();
}
#endif
#define SEM_BIT0_SET (1<<0)
static int g_mutex_flag=0;

QueueHandle_t xQueueCreateCountingSemaphore( const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount )
{
    kstat_t ret;
    ksem_t *s;
    (void)uxMaxCount;

    s = aos_malloc(sizeof(ksem_t));
    if (s == NULL) {
        return NULL;
    }

    ret = krhino_sem_create(s, "AOS", uxInitialCount);
    if (ret != RHINO_SUCCESS) {
        aos_free(s);
        return NULL;
    }

    return s;
}

BaseType_t xSemaphoreTake(QueueHandle_t xSemaphore,TickType_t xBlockTime )
{
    int ret = -1;
    int wait_bits = 0;
    if(RHINO_MUTEX_OBJ_TYPE == (((kmutex_t*)xSemaphore)->blk_obj).obj_type)
    {
        ret = _os_mutex_get_lock(xSemaphore,xBlockTime);
    }
    else if(RHINO_SEM_OBJ_TYPE == (((ksem_t*)xSemaphore)->blk_obj).obj_type)
    {
        ret = _os_take_sem(xSemaphore,xBlockTime);
    }
    else
    {
        fos_debug_error("sem take type error(%d)\r\n",(((kmutex_t*)xSemaphore)->blk_obj).obj_type);
        return pdFALSE;
    }
    if(RHINO_NO_PEND_WAIT == ret)
    {
        return pdFALSE;
    }
    else if(os_true != ret)
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
        ret = _os_mutex_release_lock((kmutex_t*)xSemaphore);
    }
    else if(RHINO_SEM_OBJ_TYPE == (((ksem_t*)xSemaphore)->blk_obj).obj_type)
    {
        ret = _os_sem_give((ksem_t*)xSemaphore);
    }
    else
    {
        fos_debug_error("sem give type error(%d)\r\n",(((kmutex_t*)xSemaphore)->blk_obj).obj_type);
    }
    if(os_true != ret)
    {
        char *name = _os_get_task_name(NULL);
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
        ret = _os_mutex_release_lock((kmutex_t*)xSemaphore);
    }
    else if(RHINO_SEM_OBJ_TYPE == (((ksem_t*)xSemaphore)->blk_obj).obj_type)
    {
        ret = _os_sem_give((ksem_t*)xSemaphore);
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

BaseType_t xSemaphoreTakeFromISR(QueueHandle_t xSemaphore,BaseType_t *pxHigherPriorityTaskWoken)
{
    if(RHINO_MUTEX_OBJ_TYPE == (((kmutex_t*)xSemaphore)->blk_obj).obj_type)
    {
        kmutex_t* mutex = (kmutex_t*)xSemaphore;
        if(mutex->owner_nested > 0 ){
            mutex->owner_nested--;
            return pdTRUE;
        }else{
            return pdFALSE;
        }
    }
    else if(RHINO_SEM_OBJ_TYPE == (((ksem_t*)xSemaphore)->blk_obj).obj_type)
    {
        ksem_t* sem = (ksem_t*)xSemaphore;
        if(sem->count > 0 ){
            sem->count--;
            return pdTRUE;
        }else{
            return pdFALSE;
        }
    }
    else
    {
        return pdFALSE;
    }

}

void vSemaphoreDelete( QueueHandle_t xSemaphore )
{
    int ret = 0;
    if(RHINO_MUTEX_OBJ_TYPE == (((kmutex_t*)xSemaphore)->blk_obj).obj_type)
    {
        _os_mutex_delete(xSemaphore);
    }
    else if(RHINO_SEM_OBJ_TYPE == (((kmutex_t*)xSemaphore)->blk_obj).obj_type)
    {
        _os_sem_delete(xSemaphore);
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
    return _os_mutex_create();
}
#endif

EventGroupHandle_t xEventGroupCreate( void )
{
    return (EventGroupHandle_t)_os_event_group_create();
}

void vEventGroupDelete( EventGroupHandle_t xEventGroup )
{
    _os_event_group_delete(xEventGroup);
}

EventBits_t xEventGroupWaitBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait )
{
    int ret = 0;
    os_event_bits_t event_bits = 0;
    ret = _os_event_wait_bits(xEventGroup,uxBitsToWaitFor,xWaitForAllBits,xClearOnExit,&event_bits,xTicksToWait);
    if(RHINO_NO_PEND_WAIT == ret)
    {
        //return 0;
    }
    else if(os_true != ret)
    {
        fos_debug_error("event_bits get error\r\n");
    }
    return event_bits;
}
EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet )
{
    _os_event_set(xEventGroup,uxBitsToSet);
    return ((kevent_t*)xEventGroup)->flags;
}
EventBits_t xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear )
{
    _os_event_clear(xEventGroup,uxBitsToClear);
    return ((kevent_t*)xEventGroup)->flags;
}
void *pvPortMalloc( size_t xWantedSize )
{
    char *p;
    p = krhino_mm_alloc(xWantedSize);
    return p;
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
#if 0
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

typedef struct {
    ktimer_t ptmr;
    void * pvTimerID;
    UBaseType_t uxAutoReload;
} faos_ktimer_t;

TimerHandle_t xTimerCreate( char * pcTimerName, uint32_t xTimerPeriodInTicks, UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction )
{
    faos_ktimer_t * pfostmr = (faos_ktimer_t*)malloc(sizeof(faos_ktimer_t));
    if(NULL == pfostmr){
        return NULL;
    }
    memset(pfostmr,0,sizeof(faos_ktimer_t));
    pfostmr->pvTimerID = pvTimerID;
    pfostmr->uxAutoReload = uxAutoReload;
    return (TimerHandle_t)espos_timer_create_static_adapter(&(pfostmr->ptmr),(const char *)pcTimerName,pxCallbackFunction,pfostmr,xTimerPeriodInTicks,uxAutoReload);

    //turn espos_timer_create_obj_adapter(pcTimerName,pxCallbackFunction,pvTimerID,xTimerPeriodInTicks,uxAutoReload);
}
BaseType_t xTimerStart(TimerHandle_t xTimer,TickType_t xTicksToWait )
{
    int ret;
    ktimer_t* ptmr = &(((faos_ktimer_t *)xTimer)->ptmr);
    ret = espos_timer_start((ktimer_t*)ptmr);
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
    ktimer_t* ptmr = &(((faos_ktimer_t *)xTimer)->ptmr);
    ret = espos_timer_stop(ptmr);
    if(os_true == ret)
        return pdTRUE;
    else
        return pdFALSE;
}
BaseType_t xTimerDelete(TimerHandle_t xTimer, TickType_t xTicksToWait )
{
    int ret;

    ktimer_t* ptmr = &(((faos_ktimer_t *)xTimer)->ptmr);
    ret = espos_timer_delete(ptmr);
    if(os_true == ret)
    {
        free(xTimer);
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

    ktimer_t* ptmr = &(((faos_ktimer_t *)xTimer)->ptmr);
    NULL_PARA_CHECK(ptmr,pdFALSE);
    ret = krhino_timer_stop(ptmr);
    ret |= krhino_timer_start(ptmr);
    if(0 == ret)
        return pdTRUE;
    else
        return pdFALSE;
}
BaseType_t xTimerChangePeriod(TimerHandle_t xTimer,TickType_t xNewPeriod,TickType_t xTicksToWait )
{
    int ret;

    ktimer_t* ptmr = &(((faos_ktimer_t *)xTimer)->ptmr);
    ret = os_timer_period_change(ptmr,xNewPeriod,xTicksToWait,((faos_ktimer_t *)xTimer)->uxAutoReload);
    if(os_true == ret)
        return pdTRUE;
    else
        return pdFALSE;
}

BaseType_t xTimerChangePeriodFromISR( TimerHandle_t xTimer,TickType_t xNewPeriod,BaseType_t * const pxHigherPriorityTaskWoken )
{
    (void)pxHigherPriorityTaskWoken;

    ktimer_t* ptmr = &(((faos_ktimer_t *)xTimer)->ptmr);
    return xTimerChangePeriod(ptmr,xNewPeriod,0);
}


BaseType_t xTimerIsTimerActive( TimerHandle_t xTimer )
{
    ktimer_t* ptimer = &(((faos_ktimer_t *)xTimer)->ptmr);

    //ktimer_t* ptimer = (ktimer_t*)xTimer;
    BaseType_t xReturn;
    //Timer_t *pxTimer = xTimer;
    uint32_t cpu_sr;

    configASSERT( xTimer );

    /* Is the timer in the list of active timers? */
    taskENTER_CRITICAL(cpu_sr);
    {
        if( ptimer->timer_state != TIMER_ACTIVE )
        {
            xReturn = pdFALSE;
        }
        else
        {
            xReturn = pdTRUE;
        }
    }
    taskEXIT_CRITICAL(cpu_sr);

    return xReturn;
}

BaseType_t xTimerResetFromISR( TimerHandle_t xTimer, BaseType_t * const pxHigherPriorityTaskWoken )
{
    ktimer_t* ptimer = &(((faos_ktimer_t *)xTimer)->ptmr);

    (void)pxHigherPriorityTaskWoken;
    xTimerReset((ktimer_t *)ptimer, 0);

    return pdPASS;
}

void *pvTimerGetTimerID( const TimerHandle_t xTimer )
{
    espos_adapter_timer *parg;

    //ktimer_t* ptimer = &(((faos_ktimer_t *)xTimer)->ptmr);

    if(NULL != xTimer){
        return ((faos_ktimer_t*)xTimer)->pvTimerID;
    }
    return NULL;
        //return ((ktimer_t*)xTimer)->timer_cb_arg;
}

void vTaskDelay(int xTicksToDelay)
{
    _os_delay_tick(xTicksToDelay);
}

uint32_t vPortEnterCritical(void)
{
    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();
    return cpsr;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( uint32_t cpsr )
{
    RHINO_CRITICAL_EXIT();
    return;
}

int portSET_INTERRUPT_MASK_FROM_ISR()
{
    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();
    return cpsr;
}
void portCLEAR_INTERRUPT_MASK_FROM_ISR(uint32_t cpsr )
{
    RHINO_CRITICAL_EXIT();
    return;
}

TickType_t xTaskGetTickCount( void )
{
    return (TickType_t)krhino_sys_tick_get();
}

QueueHandle_t xQueueCreateMutex( const uint8_t ucQueueType )
{
    return (QueueHandle_t)_os_mutex_create();
}

BaseType_t xSemCreateRecursiveMutex(void * mutex)
{
    int ret;
    ret =  _os_mutex_sta_create(mutex);
    if(ret == 0){
        return (BaseType_t)mutex;
    }
    return (BaseType_t)NULL;
}

TaskHandle_t xQueueGetMutexHolder( QueueHandle_t xSemaphore )
{
    kmutex_t * mutex = (kmutex_t *)xSemaphore;
    if(NULL == mutex) return NULL;

    return (TaskHandle_t)(mutex->mutex_task);
}

BaseType_t xQueueTakeMutexRecursive( QueueHandle_t xMutex, TickType_t xTicksToWait )
{
    int ret;
    ret = _os_mutex_get_lock(xMutex,xTicksToWait);
    return ret;
}

BaseType_t xQueueGiveMutexRecursive( QueueHandle_t xMutex )
{
    int ret;

    ret = _os_mutex_release_lock(xMutex);
    return ret;
}


UBaseType_t uxQueueMessagesWaitingFromISR( const QueueHandle_t xQueue )
{
    int ret = -1;
    if(RHINO_MUTEX_OBJ_TYPE == (((kmutex_t*)xQueue)->blk_obj).obj_type)
    {
        return ((kmutex_t*)xQueue)->owner_nested;
    }
    else if(RHINO_SEM_OBJ_TYPE == (((ksem_t*)xQueue)->blk_obj).obj_type)
    {
        return ((ksem_t*)xQueue)->count;
    }
    else
    {
        fos_debug_error("QueueMessagesWaiting error(%d)\r\n",(((kmutex_t*)xQueue)->blk_obj).obj_type);
        return 0;
    }
}



BaseType_t xQueueSemaphoreTake( QueueHandle_t xQueue, TickType_t xTicksToWait )
{
    int            ret =0;
    os_tick_type_t wait_tick;
    if (portMAX_DELAY == xTicksToWait) {
        wait_tick = OS_WAIT_FOREVER;
    } else {
        wait_tick = xTicksToWait;
    }

    ret = _os_mutex_get_lock((os_mutex_handle_t)xQueue, wait_tick);
    if(os_true == ret)
        return pdTRUE;
    else
        return pdFALSE;
}

BaseType_t xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition )
{
    int ret = 0;
    if(RHINO_MUTEX_OBJ_TYPE == (((kmutex_t*)xQueue)->blk_obj).obj_type)
    {
        ret = _os_mutex_release_lock((kmutex_t*)xQueue);
    }
    else if(RHINO_SEM_OBJ_TYPE == (((ksem_t*)xQueue)->blk_obj).obj_type)
    {
        ret = _os_sem_give((ksem_t*)xQueue);
    }
    else if (RHINO_BUF_QUEUE_OBJ_TYPE == (((ksem_t*)xQueue)->blk_obj).obj_type) {
        if(pvItemToQueue == 0){
            return pdFALSE;
        }
        return xQueueSend(xQueue, (void*)pvItemToQueue, xTicksToWait);
    }
    else
    {
        fos_debug_error("xQueueGenericSend type error(%d)\r\n",(((kmutex_t*)xQueue)->blk_obj).obj_type);
    }
    if(os_true != ret)
    {
        char *name = _os_get_task_name(NULL);
        fos_debug_error("xQueueGenericSend error in task: %s reason:%d\r\n",(name==NULL)?"NULL":name,ret);
        return pdFALSE;
    }
    else
    {
        return pdTRUE;
    }
}

TickType_t xTaskGetTickCountFromISR( void )
{
    return (TickType_t)krhino_sys_tick_get();
}

void vTaskEndScheduler( void )
{
    CPSR_ALLOC();
    cpsr = cpu_intrpt_save();
    g_sched_lock[cpu_cur_get()] = 1;
    //vPortEndScheduler();
}

void vTaskSuspendAll( void )
{
    if(xSchedulerRunning[cpu_cur_get()] == 0){
        return;
    }
    krhino_sched_disable();
}

BaseType_t xTaskResumeAll( void )
{
    if(xSchedulerRunning[cpu_cur_get()] == 0){
        return 0;
    }
    return krhino_sched_enable();
}

void vTaskSuspend( TaskHandle_t xTaskToSuspend )
{
    (void)krhino_task_suspend((ktask_t *)xTaskToSuspend);
}

void vTaskResume( TaskHandle_t xTaskToResume )
{
    (void)krhino_task_resume((ktask_t *)xTaskToResume);
}

UBaseType_t uxTaskPriorityGet( const TaskHandle_t xTask )
{
    uint8_t basepri = ((ktask_t *)xTask)->prio;
    return (UBaseType_t)(OS_TASK_PRI_ADAPTER - basepri);
}

BaseType_t xTaskNotifyStateClear( TaskHandle_t xTask )
{
    return !krhino_task_sem_del((ktask_t *)xTask);
}


BaseType_t xQueuePeek(QueueHandle_t xQueue, void *pvItemToQueue,TickType_t xTicksToWait )
{
    char *name=pcTaskGetTaskName(NULL);
    int ret = _os_queue_peek(xQueue,pvItemToQueue,(TickType_t)xTicksToWait);
    if(RHINO_NO_PEND_WAIT == ret)
    {
        return pdFALSE;
    }
    else if(os_true != ret)
    {
        fos_debug_error("queue recive error in task:%s,handle:%p,block time:%ld\r\n",(NULL == name)?"NULL":name,xQueue,xTicksToWait);
        return pdFALSE;
    }
    fos_debug_info("recv queue Peek in task:%s,handle:%p,free space:%ld\r\n",(NULL == name)?"NULL":name,xQueue,uxQueueSpacesAvailable(xQueue));
    return pdTRUE;
}

BaseType_t xQueuePeekFromISR(QueueHandle_t xQueue, void *pvItemToQueue)
{
    printf("Error! queue peek error in isr handle:%p\r\n",xQueue);
    return pdFALSE;
}

uint32_t ulPortSetInterruptMask( void )
{
    uint32_t sr_level;
    sr_level = cpu_intrpt_save();
    return sr_level;
}

void vPortClearInterruptMask( uint32_t sr_level )
{
    cpu_intrpt_restore(sr_level);
    return ;
}

BaseType_t xQueueGiveFromISR( QueueHandle_t xQueue, BaseType_t * const pxHigherPriorityTaskWoken )
{
    int ret = 0;
    (void)pxHigherPriorityTaskWoken;
    if(RHINO_MUTEX_OBJ_TYPE == (((kmutex_t*)xQueue)->blk_obj).obj_type)
    {
        ret = _os_mutex_release_lock((kmutex_t*)xQueue);
    }
    else if(RHINO_SEM_OBJ_TYPE == (((ksem_t*)xQueue)->blk_obj).obj_type)
    {
        ret = _os_sem_give((ksem_t*)xQueue);
    }
    else
    {
        fos_debug_error("sem give type error(%d)\r\n",(((kmutex_t*)xQueue)->blk_obj).obj_type);
    }
    if(os_true != ret)
    {
        char *name = _os_get_task_name(NULL);
        fos_debug_error("sem give error in task: %s reason:%d\r\n",(name==NULL)?"NULL":name,ret);
        return pdFALSE;
    }
    else
    {
        return pdTRUE;
    }
}

void vTaskSetApplicationTaskTag( TaskHandle_t xTask, TaskHookFunction_t pxHookFunction )
{
    ktask_t * ptask;

    //TCB_t *xTCB;
    uint32_t cpu_sr;

    /* If xTask is NULL then it is the task hook of the calling task that is
    getting set. */
    if( xTask == NULL )
    {
        ptask = (ktask_t *)_os_cur_task_handle();
    }
    else
    {
        ptask = xTask;
    }

    /* Save the hook function in the TCB.  A critical section is required as
    the value can be accessed from an interrupt. */
    taskENTER_CRITICAL(cpu_sr);
    {
        ptask->user_info[RHINO_TASK_HOOK_USER_INFO_POS] = pxHookFunction;
    }
    taskEXIT_CRITICAL(cpu_sr);


}

TaskHookFunction_t xTaskGetApplicationTaskTag( TaskHandle_t xTask )
{
    ktask_t * ptask;
    TaskHookFunction_t xReturn;
    uint32_t cpu_sr;

    /* If xTask is NULL then set the calling task's hook. */
    if( xTask == NULL )
    {
        ptask = (ktask_t *)_os_cur_task_handle();
    }
    else
    {
        ptask = xTask;
    }

    /* Save the hook function in the TCB.  A critical section is required as
    the value can be accessed from an interrupt. */
    taskENTER_CRITICAL(cpu_sr);
    {
        xReturn = ptask->user_info[RHINO_TASK_HOOK_USER_INFO_POS];
    }
    taskEXIT_CRITICAL(cpu_sr);

    return xReturn;
}

void vTaskSetName( TaskHandle_t xTaskToQuery, const char * name )
{
    ktask_t * ptask;
    uint16_t name_len = (strlen(name) + 1);

    if( xTaskToQuery == NULL )
    {
        ptask = (ktask_t *)_os_cur_task_handle();
    }
    else
    {
        ptask = xTaskToQuery;
    }

    /* If null is passed in here then the name of the calling task is being
    queried. */
    //memcpy(&( pxTCB->pcTaskName[0] ), name, configMAX_TASK_NAME_LEN - 1);
    if (name_len > configMAX_TASK_NAME_LEN) {
        name_len = configMAX_TASK_NAME_LEN;
    }

    if (ptask->task_name != NULL) {
        free((void*)ptask->task_name);
        ptask->task_name = NULL;
    }
    ptask->task_name = malloc(name_len);
    if (ptask->task_name != NULL) {
        memset((void*)ptask->task_name, 0, name_len);
        strncpy((void*)ptask->task_name, name, name_len - 1);
    }
}

void vTaskList( char * pcWriteBuffer )
{
    printf("warning,vTaskList,file:%s,line:%d\r\n",__FILE__,__LINE__);
}

QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType )
{
    switch (ucQueueType) {
        case queueQUEUE_TYPE_MUTEX:
            return xQueueCreateMutex(ucQueueType);
            break;

        case queueQUEUE_TYPE_BASE:
            return xQueueCreate(uxQueueLength, uxItemSize);
            break;

        case queueQUEUE_TYPE_COUNTING_SEMAPHORE:
            return xQueueCreateCountingSemaphore(0,0);
            break;

        case queueQUEUE_TYPE_BINARY_SEMAPHORE:
            return xSemaphoreCreateBinary();
            break;

        default:
            break;
    }
    return NULL;
}

BaseType_t xTaskNotifyGive( TaskHandle_t xTaskToNotify)
{
    int ret;
    ktask_t *task_handle = (ktask_t *)xTaskToNotify;
    if(NULL == task_handle){
        return false;
    }

    if(NULL == task_handle->task_sem_obj){
        /*free in krhino_task_del_hook*/
        ksem_t *sem = (ksem_t *)malloc(sizeof(ksem_t));
        if(NULL == sem){
            return false;
        }
        ret = krhino_task_sem_create(task_handle, sem, "osif_signal",0);
        if (RHINO_SUCCESS != ret)
        {
            printf("ERROR:xTaskNotifyGive fail\r\n");
            free(sem);
            return false;
        }
    }

    ret = krhino_task_sem_give(task_handle);
    if(0 == ret)
    {
        return os_true;
    }
    else
    {
        printf("xTaskNotifyGive error reason:%d\r\n",ret);
        return os_false;
    }
}

void vTaskNotifyGiveFromISR( TaskHandle_t xTaskToNotify, BaseType_t *pxHigherPriorityTaskWoken )
{
    int ret;
    ktask_t *task_handle = (ktask_t *)xTaskToNotify;
    (void)pxHigherPriorityTaskWoken;

    if(NULL == task_handle){
        return;
    }

    if(NULL == task_handle->task_sem_obj){
        /*free in krhino_task_del_hook*/
        ksem_t *sem = (ksem_t *)malloc(sizeof(ksem_t));
        if(NULL == sem){
            return;
        }
        ret = krhino_task_sem_create(task_handle, sem, "osif_signal",0);
        if (RHINO_SUCCESS != ret)
        {
            printf("ERROR:vTaskNotifyGiveFromISR fail\r\n");
            free(sem);
            return;
        }
    }

    ret = krhino_task_sem_give(task_handle);
    if(0 != ret)
    {
        printf("vTaskNotifyGiveFromISR error reason:%d\r\n",ret);
    }
    return;
}

uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait )
{
    int ret;
    tick_t ticks;
    ktask_t * task_handle = krhino_cur_task_get();

    if(OS_WAIT_FOREVER == xTicksToWait){
        ticks = RHINO_WAIT_FOREVER;
    }else{
        ticks = xTicksToWait;
    }

    if(NULL == task_handle){
        return false;
    }

    if(NULL == task_handle->task_sem_obj){
        /*free in krhino_task_del_hook*/
        ksem_t *sem = (ksem_t *)malloc(sizeof(ksem_t));
        if(NULL == sem){
            return false;
        }
        ret = krhino_task_sem_create(task_handle, sem, "osif_signal",0);
        if (RHINO_SUCCESS != ret)
        {
            printf("ERROR:ulTaskNotifyTake fail\r\n");
            free(sem);
            return false;
        }
    }

    ret = krhino_task_sem_take(ticks);
    if(0 == ret)
    {
        if(xClearCountOnExit){
            krhino_task_sem_count_set(krhino_cur_task_get(),0);
        }

        return os_true;
    }
    else
    {
        printf("ulTaskNotifyTake error reason:%d\r\n",ret);
        return os_false;
    }
}



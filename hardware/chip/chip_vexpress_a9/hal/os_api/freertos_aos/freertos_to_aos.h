#ifndef _FREERTOE_TO_AOS
#define _FREERTOE_TO_AOS

#include "k_api.h"
#include "aos/kernel.h"

#define MAX_QUEUE_NUM 20

/* Type definitions. */
#define portCHAR    char
#define portFLOAT   float
#define portDOUBLE  double
#define portLONG    long
#define portSHORT   short
#define portSTACK_TYPE  uint32_t

#define portBASE_TYPE   long
#define pdTRUE    1
#define pdFALSE   0
#define pdPASS  ( pdTRUE )
#define pdFAIL  ( pdFALSE )
#define portMAX_DELAY 0xfffffffful

#define configMAX_API_CALL_INTERRUPT_PRIORITY	31
#define configAPPLICATION_NORMAL_PRIORITY	15

#define queueQUEUE_TYPE_BASE                    ( ( uint8_t ) 0U )
#define queueQUEUE_TYPE_SET                     ( ( uint8_t ) 0U )
#define queueQUEUE_TYPE_MUTEX                   ( ( uint8_t ) 1U )
#define queueQUEUE_TYPE_COUNTING_SEMAPHORE      ( ( uint8_t ) 2U )
#define queueQUEUE_TYPE_BINARY_SEMAPHORE        ( ( uint8_t ) 3U )
#define queueQUEUE_TYPE_RECURSIVE_MUTEX         ( ( uint8_t ) 4U )

#define configUSE_POSIX_ERRNO 1
#define configCPU_CLOCK_HZ						400000000UL
#define configUSE_PORT_OPTIMISED_TASK_SELECTION	1
#define configSUPPORT_DYNAMIC_ALLOCATION		1
#define configSUPPORT_STATIC_ALLOCATION 1
#define configUSE_TICKLESS_IDLE					0
//#define configTICK_RATE_HZ						( ( TickType_t ) 100 )
#define configUSE_PREEMPTION					1
#define configMAX_PRIORITIES					( 32 )
#define configMINIMAL_STACK_SIZE				( ( unsigned short ) 1024 )
#define configTOTAL_HEAP_SIZE					( ( size_t ) (0x800000))
#define configMAX_TASK_NAME_LEN					( 32 )
#define configUSE_TRACE_FACILITY				1
#define configUSE_16_BIT_TICKS					0
#define configIDLE_SHOULD_YIELD					1
#define configUSE_MUTEXES						1
#define configQUEUE_REGISTRY_SIZE				8
#define configCHECK_FOR_STACK_OVERFLOW			2
#define configUSE_RECURSIVE_MUTEXES				1
#define configUSE_APPLICATION_TASK_TAG			1
#define configUSE_COUNTING_SEMAPHORES			1
/* Hook/Callback related definitions. */
#define configUSE_MALLOC_FAILED_HOOK			0
#define configUSE_IDLE_HOOK						1
#define configUSE_TICK_HOOK						1


#define taskSCHEDULER_RUNNING 2//RHINO_RUNNING
#define taskSCHEDULER_SUSPENDED 0//RHINO_STOPPED
#define taskSCHEDULER_NOT_STARTED 1//RHINO_STOPPED

#define portTICK_RATE_MS     portTICK_PERIOD_MS
#define configTICK_RATE_HZ   RHINO_CONFIG_TICKS_PER_SECOND
#define portTICK_PERIOD_MS   ((TickType_t)1000/RHINO_CONFIG_TICKS_PER_SECOND)

#define errQUEUE_EMPTY  RHINO_BUF_QUEUE_SIZE_ZERO
#define errQUEUE_FULL   RHINO_BUF_QUEUE_FULL

#define portEND_SWITCHING_ISR(a)
//#define xTimerChangePeriodFromISR   xTimerChangePeriod

#define ulPortInterruptNesting   g_intrpt_nested_level
#define xTimerStartFromISR       xTimerStart
#define xTimerStopFromISR        xTimerStop

#define taskENTER_CRITICAL_FROM_ISR() portSET_INTERRUPT_MASK_FROM_ISR()
#define taskEXIT_CRITICAL_FROM_ISR( x ) portCLEAR_INTERRUPT_MASK_FROM_ISR( x )
#define tskIDLE_PRIORITY			( 0 )

#define xSemaphoreGetMutexHolder( xSemaphore ) xQueueGetMutexHolder( ( xSemaphore ) )

#define xSemaphoreTakeRecursive( xMutex, xBlockTime )	xQueueTakeMutexRecursive( ( xMutex ), ( xBlockTime ) )

#define xSemaphoreGiveRecursive( xMutex )   xQueueGiveMutexRecursive( ( xMutex ) )

#define xSemaphoreCreateRecursiveMutex() xQueueCreateMutex( 0 )

#define __enable_irq()               do { asm volatile ("cpsie i"); } while (0);
#define __disable_irq()              do { asm volatile ("cpsid i"); } while (0);

#define soft_break(...)     	do { __asm__ __volatile__("bkpt #0" ::: "memory", "cc"); } while(0)

#define cur_cpu_id  cpu_cur_get
#define SMP_DBG(fmt, ...) do { printf("cpu%d: %s line %d, "fmt, cpu_cur_get(), __func__, __LINE__, ##__VA_ARGS__); } while(0)

#ifndef pdMS_TO_TICKS
    #define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) RHINO_CONFIG_TICKS_PER_SECOND ) / ( TickType_t ) 1000 ) )
#endif


#define configTIMER_TASK_PRIORITY       ( configMAX_PRIORITIES - 1 )

#define portBYTE_ALIGNMENT 8

#define xSemaphoreCreateCounting xQueueCreateCountingSemaphore

typedef enum {
    SEM=1,
    QUEUE,
    EVENT
}os_queue_type_e;

typedef struct rtos_set_
{
    os_queue_type_e type;
    void *handle;
}rtos_set_t;

typedef void * QueueHandle_t;
typedef QueueHandle_t SemaphoreHandle_t;
typedef void * TimerHandle_t;
typedef void * TaskHandle_t;
typedef void * EventGroupHandle_t;
typedef void * QueueSetMemberHandle_t;
typedef void * QueueSetHandle_t;

typedef void (*TaskFunction_t)( void * );
typedef portSTACK_TYPE StackType_t;

#define xTaskHandle TaskHandle_t
#define xQueueHandle QueueHandle_t
#define xSemaphoreHandle SemaphoreHandle_t
#define portSTACK_TYPE	uint32_t
#define xTimerHandle TimerHandle_t

typedef uint32_t TickType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;
typedef TickType_t EventBits_t;
typedef void (*TimerCallbackFunction_t)( void* xTimer );
typedef TickType_t portTickType;

extern uint32_t vPortEnterCritical(void);
extern void vPortExitCritical( uint32_t cpsr );

#define portDISABLE_INTERRUPTS(cpu_sr)	do {  cpu_sr = ulPortSetInterruptMask(); } while(0)
#define portENABLE_INTERRUPTS(cpu_sr)	do {  vPortClearInterruptMask( cpu_sr ); } while(0)

#define taskDISABLE_INTERRUPTS(cpu_sr)	do { portDISABLE_INTERRUPTS(cpu_sr); } while(0)

#define portENTER_CRITICAL(cpu_sr)  do {  cpu_sr=vPortEnterCritical(); } while(0)
#define portEXIT_CRITICAL(cpu_sr)   do {  vPortExitCritical(cpu_sr); } while(0)

#define taskENTER_CRITICAL(cpu_sr)    portENTER_CRITICAL(cpu_sr) //krhino_intrpt_enter();
#define taskEXIT_CRITICAL(cpu_sr)    portEXIT_CRITICAL(cpu_sr) //krhino_intrpt_exit();

typedef BaseType_t (*TaskHookFunction_t)( void * );

extern void platform_assert(const char *, const char *, int);
#define configASSERT( x ) if( (x) == 0 ) { printf("Error:%s,file:%s,line:%d\r\n",#x, __FILE__, __LINE__); }

#define FOS_DEBUG_INFO 0
#define FOS_DEBUG_ERROR 1
#if FOS_DEBUG_ERROR
#define fos_debug_error(fmt,arg...) \
    do{ \
        printf("[error F:%s L:%d]",__FUNCTION__,__LINE__);  \
        printf(fmt,##arg);  \
    }while(0);
#else
#define fos_debug_error(fmt,arg...)
#endif

#if FOS_DEBUG_INFO
#define fos_debug_info(fmt,arg...)	\
    do{ \
        printf("[info F:%s L:%d]",__FUNCTION__,__LINE__);   \
        printf(fmt,##arg);  \
    }while(0);
#else
#define fos_debug_info(fmt,arg...)
#endif

BaseType_t xTaskCreate(task_entry_t entry,const name_t *name,size_t stack_size,void *arg,uint8_t pri,TaskHandle_t *task_handle);

void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority );

TaskHandle_t xTaskGetCurrentTaskHandle( void );

char *pcTaskGetTaskName( TaskHandle_t xTaskToQuery );

void vTaskDelete( TaskHandle_t xTaskToDelete );

void taskYIELD(void);

void portYIELD_FROM_ISR(BaseType_t xHigherPriorityTaskWoken);

QueueHandle_t xQueueCreate(const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize);

BaseType_t xQueueSend(QueueHandle_t xQueue, void *pvItemToQueue,TickType_t xTicksToWait );

BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, void *pvItemToQueue,BaseType_t * const pxHigherPriorityTaskWoken );

BaseType_t xQueueSendToBack(QueueHandle_t xQueue,void * pvItemToQueue, TickType_t xTicksToWait);

BaseType_t xQueueSendToBackFromISR(QueueHandle_t xQueue, void *pvItemToQueue,  BaseType_t * pxHigherPriorityTaskWoken );

BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvItemToQueue,TickType_t xTicksToWait );

BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue, void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken);

void vQueueDelete( QueueHandle_t xQueue );

UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue );

UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue );

QueueSetHandle_t xQueueCreateSet( const UBaseType_t uxEventQueueLength );

void vQueueAddToRegistry( QueueHandle_t xQueue, const char *pcQueueName );

BaseType_t xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet );

QueueSetMemberHandle_t xQueueSelectFromSet( QueueSetHandle_t xQueueSet, TickType_t const xTicksToWait );

QueueHandle_t xSemaphoreCreateBinary();

QueueHandle_t xQueueCreateCountingSemaphore( const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount );

BaseType_t xSemaphoreTake(QueueHandle_t xSemaphore,TickType_t xBlockTime );

BaseType_t xSemaphoreGive(QueueHandle_t xSemaphore );

BaseType_t xSemaphoreGiveFromISR(QueueHandle_t xSemaphore,BaseType_t *pxHigherPriorityTaskWoken);

BaseType_t xSemaphoreTakeFromISR(QueueHandle_t xSemaphore,BaseType_t *pxHigherPriorityTaskWoken);

void vSemaphoreDelete( QueueHandle_t xSemaphore );

QueueHandle_t xSemaphoreCreateMutex(  );

EventGroupHandle_t xEventGroupCreate( void );

void vEventGroupDelete( EventGroupHandle_t xEventGroup );

EventBits_t xEventGroupWaitBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait );

EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet );

EventBits_t xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear );

void *pvPortMalloc( size_t xWantedSize );

void *pvPortCalloc( size_t nmemb, size_t size );

void vPortFree( void *pv );

size_t xPortGetFreeHeapSize( void );

BaseType_t xTaskGetSchedulerState( void );

TimerHandle_t xTimerCreate( char * pcTimerName, uint32_t xTimerPeriodInTicks, UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction );

BaseType_t xTimerStart(TimerHandle_t xTimer,TickType_t xTicksToWait );

BaseType_t xTimerStop(TimerHandle_t xTimer,TickType_t xTicksToWait );

BaseType_t xTimerDelete(TimerHandle_t xTimer, TickType_t xTicksToWait );

BaseType_t xTimerReset( TimerHandle_t xTimer, const TickType_t xTicksToWait );

BaseType_t xTimerChangePeriod(TimerHandle_t xTimer,TickType_t xNewPeriod,TickType_t xTicksToWait );

BaseType_t xTimerChangePeriodFromISR( TimerHandle_t xTimer,TickType_t xNewPeriod,BaseType_t * const pxHigherPriorityTaskWoken );

BaseType_t xTimerIsTimerActive( TimerHandle_t xTimer );

BaseType_t xTimerResetFromISR( TimerHandle_t xTimer, BaseType_t * const pxHigherPriorityTaskWoken );

void *pvTimerGetTimerID( const TimerHandle_t xTimer );

void vTaskDelay(int xTicksToDelay);

uint32_t vPortEnterCritical(void);

void vPortExitCritical( uint32_t cpsr );

int portSET_INTERRUPT_MASK_FROM_ISR();

void portCLEAR_INTERRUPT_MASK_FROM_ISR(uint32_t cpsr );

TickType_t xTaskGetTickCount( void );

QueueHandle_t xQueueCreateMutex( const uint8_t ucQueueType );

BaseType_t xSemCreateRecursiveMutex(void * mutex);

TaskHandle_t xQueueGetMutexHolder( QueueHandle_t xSemaphore );

BaseType_t xQueueTakeMutexRecursive( QueueHandle_t xMutex, TickType_t xTicksToWait );

BaseType_t xQueueGiveMutexRecursive( QueueHandle_t xMutex );

UBaseType_t uxQueueMessagesWaitingFromISR( const QueueHandle_t xQueue );

BaseType_t xQueueSemaphoreTake( QueueHandle_t xQueue, TickType_t xTicksToWait );

BaseType_t xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition );

TickType_t xTaskGetTickCountFromISR( void );

void vTaskEndScheduler( void );

void vTaskSuspendAll( void );

BaseType_t xTaskResumeAll( void );

void vTaskSuspend( TaskHandle_t xTaskToSuspend );

void vTaskResume( TaskHandle_t xTaskToResume );

UBaseType_t uxTaskPriorityGet( const TaskHandle_t xTask );

BaseType_t xTaskNotifyStateClear( TaskHandle_t xTask );

BaseType_t xQueuePeek(QueueHandle_t xQueue, void *pvItemToQueue,TickType_t xTicksToWait );

BaseType_t xQueuePeekFromISR(QueueHandle_t xQueue, void *pvItemToQueue);

uint32_t ulPortSetInterruptMask( void );

void vPortClearInterruptMask( uint32_t sr_level );

BaseType_t xQueueGiveFromISR( QueueHandle_t xQueue, BaseType_t * const pxHigherPriorityTaskWoken );

void vTaskSetApplicationTaskTag( TaskHandle_t xTask, TaskHookFunction_t pxHookFunction );

TaskHookFunction_t xTaskGetApplicationTaskTag( TaskHandle_t xTask );

void vTaskSetName( TaskHandle_t xTaskToQuery, const char * name );

void vTaskList( char * pcWriteBuffer );

QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType );

void vTaskNotifyGiveFromISR( TaskHandle_t xTaskToNotify, BaseType_t *pxHigherPriorityTaskWoken );

uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait );

#endif

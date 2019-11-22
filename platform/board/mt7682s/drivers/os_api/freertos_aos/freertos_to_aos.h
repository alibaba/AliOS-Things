#ifndef _FREERTOE_TO_AOS
#define _FREERTOE_TO_AOS

#include "k_api.h"
#include "aos/kernel.h"


#define MAX_QUEUE_NUM 20

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	uint32_t

#define portBASE_TYPE	long
#define pdTRUE    1
#define pdFALSE   0
#define pdPASS			( pdTRUE )
#define pdFAIL			( pdFALSE )
#define portMAX_DELAY 0xfffffffful

#define portNVIC_INT_CTRL_REG 0

#define taskSCHEDULER_RUNNING 2//RHINO_RUNNING
#define taskSCHEDULER_SUSPENDED 0//RHINO_STOPPED
#define taskSCHEDULER_NOT_STARTED 1//RHINO_STOPPED

#define configMAX_PRIORITIES RHINO_CONFIG_PRI_MAX
#define portTICK_PERIOD_MS ((TickType_t)1000/RHINO_CONFIG_TICKS_PER_SECOND)

#define errQUEUE_EMPTY	RHINO_BUF_QUEUE_SIZE_ZERO
#define errQUEUE_FULL	RHINO_BUF_QUEUE_FULL
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
typedef struct x_queue_{
	kbuf_queue_t *aos_queue;
	size_t item_size;
	size_t item_num;
	size_t queue_size;
	char flag;
}rtos_queue_t;
typedef void * QueueHandle_t;
typedef QueueHandle_t SemaphoreHandle_t;
typedef void * TimerHandle_t;
typedef void * TaskHandle_t;
typedef void * EventGroupHandle_t;
typedef void * QueueSetMemberHandle_t;
typedef void * QueueSetHandle_t;

#define xTaskHandle TaskHandle_t
#define xQueueHandle QueueHandle_t
#define xSemaphoreHandle SemaphoreHandle_t
#define portSTACK_TYPE	uint32_t

#define portBYTE_ALIGNMENT			8

#ifdef HAL_CACHE_WITH_REMAP_FEATURE
#define portCacheline_ALIGNMENT HAL_CACHE_LINE_SIZE
#endif

#if portBYTE_ALIGNMENT == 8
	#define portBYTE_ALIGNMENT_MASK ( 0x0007U )
#endif


typedef uint32_t TickType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;
typedef TickType_t EventBits_t;
//typedef void (*TimerCallbackFunction_t)( TimerHandle_t xTimer );
typedef void (*TimerCallbackFunction_t)( void* xTimer ,void*arg);
#define taskENTER_CRITICAL()		vPortEnterCritical(); //krhino_intrpt_enter();
#define taskEXIT_CRITICAL()			vPortExitCritical();//krhino_intrpt_exit();
//#define portSET_INTERRUPT_MASK_FROM_ISR() //krhino_intrpt_enter();
//#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) //krhino_intrpt_exit();

#ifndef mtCOVERAGE_TEST_MARKER
	#define mtCOVERAGE_TEST_MARKER()
#endif
//#define pvPortMallocNC(size) pvPortMalloc(size)
//#define vPortFreeNC(buf) vPortFree(buf)

extern void platform_assert(const char *, const char *, int);
#define configASSERT( x ) if( (x) == 0 ) { platform_assert(#x, __FILE__, __LINE__); }

#define FOS_DEBUG_INFO 0
#define FOS_DEBUG_ERROR 0
#if FOS_DEBUG_ERROR
#define fos_debug_error(fmt,arg...)	\
	do{					\
		printf("[error F:%s L:%d]",__FUNCTION__,__LINE__);	\
		printf(fmt,##arg);	\
	  }while(0);
#else
#define fos_debug_error(fmt,arg...)
#endif

#if FOS_DEBUG_INFO
#define fos_debug_info(fmt,arg...)	\
	do{					\
		printf("[info F:%s L:%d]",__FUNCTION__,__LINE__);	\
		printf(fmt,##arg);	\
	  }while(0);
#else
#define fos_debug_info(fmt,arg...)
#endif

void rtos_api_test(void *param);
void freertos_api_test(void *param);
UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue );
void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );
#endif
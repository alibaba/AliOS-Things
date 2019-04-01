#ifndef _OSAL_H_
#define _OSAL_H_

// include header
#include <stdlib.h>
#include "soc_defs.h"
#include "soc_types.h"
#ifdef CONFIG_OS_RHINO
	#include <k_api.h>
	#include "ulist.h"
#else
	#include "FreeRTOS.h"
	#include "semphr.h"
	#include "event_groups.h"
	#include "timers.h"
	#include "ulist.h"
#endif

// basic type
#ifdef CONFIG_OS_RHINO
	#define pdFALSE			( ( uint32_t ) 0 )
	#define pdTRUE			( ( uint32_t ) 1 )

	#define pdPASS			( pdTRUE )
	#define pdFAIL			( pdFALSE )
	
	#define portCHAR		char
	#define portLONG		long
	#define portBASE_TYPE	portLONG
	#define portSTACK_TYPE	unsigned portLONG
	#define portMAX_DELAY	RHINO_WAIT_FOREVER

	#define portBYTE_ALIGNMENT		8
	#define portBYTE_ALIGNMENT_MASK ( 0x0007U )

	#define	OS_MAX_WAITTIME	RHINO_WAIT_FOREVER

    typedef uint64_t OS_TICKTYPE;

	extern void soc_asserted(const char *func, int line);
	#define configASSERT( x ) \
		do { \
		  if ( /*lint -save -e731 */ ( x ) == 0 /*lint -restore */) \
			soc_asserted(__func__, __LINE__); \
		} while (0)
#else
	#define	OS_MAX_WAITTIME	portMAX_DELAY

    typedef uint32_t OS_TICKTYPE;
#endif
typedef void *                       OsTaskHandle;
typedef void *                       OsMutex;
typedef void *                       OsSemaphore;
typedef void *                       OsEvent;
typedef void *                       OsMsgQ;
typedef void *						 OsBufQ;
typedef void *                       OsTimer;
typedef void (*OsTimerHandler)(OsTimer);
typedef void (*OsTask)(void *);

// task priority
#ifdef CONFIG_OS_RHINO
	#define OS_TASK_PRIOBASE 32
#else
	#define OS_TASK_PRIOBASE 0
#endif
#if 1
	#define OS_TASK_PRIO0               0
	#define OS_TASK_PRIO1               1
	#define OS_TASK_PRIO2               2
	#define OS_TASK_PRIO3               3
	#define OS_TASK_PRIO4               4
	#define OS_TASK_PRIO5               5
	#define OS_TASK_PRIO6               6
	#define OS_TASK_PRIO7               7
#endif

// error value
typedef enum _OS_STATUS
{
	OS_SUCCESS = 0,
	OS_FAILED,
	OS_TIMEOUT
} OS_STATUS;

// tick macro
#ifdef CONFIG_OS_RHINO
	#define OS_MS2TICK(_ms) krhino_ms_to_ticks(_ms)
	#define os_tick2ms(_tick) (((_tick)*1000)/RHINO_CONFIG_TICKS_PER_SECOND)
	#define OS_TICK_RATE_HZ RHINO_CONFIG_TICKS_PER_SECOND
	#define OS_TICK_RATE_MS (1000/RHINO_CONFIG_TICKS_PER_SECOND)
#else
	#define OS_MS2TICK(_ms)             ( (_ms)/portTICK_RATE_MS )
	#define os_tick2ms(_tick) ((_tick)*portTICK_RATE_MS)
	#define OS_TICK_RATE_HZ configTICK_RATE_HZ
	#define OS_TICK_RATE_MS portTICK_PERIOD_MS
#endif

/**
 *  OsMsgQEntry only used in OsMsgQ API
 */
typedef struct OsMsgQEntry_st
{
    u32         MsgCmd;
    void        *MsgData;
} OsMsgQEntry, *POsMsgQEntry;

typedef void (*TASK_FUNC)(void *);
typedef struct task_info_st
{
    const char   *task_name;
    OsMsgQ       qevt;
    u32          qlength;
    u32          prio;
    u32          stack_size;  /* unit: 16 */
    void       *args;
    TASK_FUNC    task_func;
} task_info;

/**
 *  Flag to indicate whether ISR handler is running or not.
 */
extern volatile u8 gOsFromISR;

/**
 * check whether current context is in ISR
 */
u8 OS_FromISR( void );
/**
 * init os, must be called at begin of APP_init
 */
void OS_Init( void );
/**
 * init cpu usage statistic
 */
void OS_StatInit( void );
void OS_TaskLogSwitch (void);
/**
 * get a random number
 *
 * @retval the random number
 */
unsigned long OS_Random(void);
/**
 * get current system tick count
 *
 * @retval current system tick count
 */
OS_TICKTYPE OS_GetSysTick(void);

/* Monitor: */
void OS_SysInfo(void);

/* Task: */
#ifdef CONFIG_OS_RHINO
	//#warning "ALIOS do not need static task table"
	typedef struct HeapRegion
	{
		uint8_t *pucStartAddress;
		size_t xSizeInBytes;
	} HeapRegion_t;
#else
	typedef struct table_entry
	{
		OsTaskHandle task_handle;
		u32 task_stack_size;
		u32 min_ever_remain_stack_size;
		u32 curr_runtime_in_sec;
		u32 prev_runtime_in_sec;
		struct table_entry* next;
	} table_entry;
#endif
/**
 * create task
 *
 * @param   task                    	task function
 * @param   name                    	task name
 * @param   stackSize                   task stack size in WORDS (ex: task stack size 512 bytes = 128 WORDS)
 * @param   param                       task parameter
 * @param   pri                       	task priority, (min:OS_TASK_PRIO0, max:OS_TASK_PRIO3)
 * @param   taskHandle                  task handle
 *
 * @retval  0                       	task create failed
 * @retval  1                         	task create success
 */
u8  OS_TaskCreate( OsTask task, const char *name, u16 stackSize, void *param, u32 pri, OsTaskHandle *taskHandle );
/**
 * get current task's handle
 *
 * @retval current task's handle
 */
OsTaskHandle OS_TaskGetCurrHandle(void);
u32 OS_TaskGetPriority(OsTaskHandle taskHandle);
void OS_TaskSetPriority(OsTaskHandle taskHandle, u32 prio);
/**
 * delete task
 *
 * @param   taskHandle                    	NULL: delete current task
 */
void OS_TaskDelete( OsTaskHandle taskHandle );
/**
 * get current task's name
 *
 * @retval current task's name
 */
const char * OS_TaskGetName (OsTaskHandle taskHandle);
/**
 * suspend task
 *
 * @param   taskHandle                    	NULL: suspend current task, other: task to suspend
 */
void OS_TaskSuspend( OsTaskHandle taskHandle );
/**
 * resume task
 *
 * @param   taskHandle                    	task to resume
 */
void OS_TaskResume( OsTaskHandle taskHandle );
/**
 * start scheduler, must be called at end of APP_init
 */
void OS_StartScheduler( void );
/**
 * stop os, never used
 */
void OS_StopScheduler( void );
/**
 * force scheduler to do context switch
 */
void OS_ForceScheduler( void );
/**
 * suspend scheduler
 */
void OS_SuspendScheduler( void );
/**
 * resume scheduler
 */
void OS_ResumeScheduler( void );

/* Mutex: */
/**
 * create mutex
 *
 * @param   mutex                    	pointer of mutex handle
 *
 * @retval  OS_SUCCESS                  create mutex success
 * @retval  OS_FAILED                   create mutex failed
 */
OS_STATUS OS_MutexInit( OsMutex *mutex );
/**
 * lock mutex
 *
 * @param   mutex                    	mutex handle
 *
 * @retval  OS_SUCCESS                  lock mutex success
 * @retval	OS_FAILED					lock mutex invalid handle or error mutex holder
 * @retval  OS_TIMEOUT                  lock mutex timeout
 */
OS_STATUS OS_MutexLock( OsMutex mutex );
/**
 * unlock mutex
 *
 * @param   mutex                    	mutex handle
 *
 * @retval  OS_SUCCESS                  unlock mutex success
 * @retval  OS_FAILED                   unlock mutex failed
 */
OS_STATUS OS_MutexUnLock( OsMutex mutex );
/**
 * delete mutex
 *
 * @param   mutex                    	mutex handle
 *
 * @retval	OS_SUCCESS					delete mutex success
 * @retval	OS_FAILED					delete mutex failed
 */
OS_STATUS OS_MutexDelete( OsMutex mutex );

/*semaphore*/
/**
 * create semaphore
 *
 * @param   Sem                    	pointer of semaphore handle
 * @param   maxcnt                  semaphore max cnt
 * @param   cnt                    	semaphore init cnt
 *
 * @retval  OS_SUCCESS              create semaphore success
 * @retval  OS_FAILED               create semaphore failed
 */
OS_STATUS OS_SemInit( OsSemaphore* Sem , u32 maxcnt , u32 cnt);
/**
 * wait a semaphore
 *
 * @param   Sem                    		semaphore handle
 * @param   timeout                    	system tick count to wait
 *
 * @retval  OS_SUCCESS                  get a message success
 * @retval  OS_TIMEOUT                  get a message timeout
 */
OS_STATUS OS_SemWait( OsSemaphore Sem , OS_TICKTYPE timeout);
/**
 * get count from semaphore
 *
 * @param   Sem                    		semaphore handle
 *
 * @retval  count in semaphore
 */
u32 OS_SemGetCount( OsSemaphore Sem );
/**
 * signal a semaphore in normal context
 * 
 * @retval  OS_SUCCESS                  signal a semaphore success
 * @retval  OS_FAILED                   signal a semaphore failed
 */
OS_STATUS OS_SemSignal( OsSemaphore Sem);
/**
 * delete a semaphore
 *
 * @param   Sem                    		semaphore handle
 *
 * @retval  OS_SUCCESS                  delete a semaphore success
 * @retval  OS_FAILED                   delete a semaphore failed
 *
 */
OS_STATUS OS_SemDelete(OsSemaphore Sem);

/* Event */
OS_STATUS OS_EventCreate(OsEvent* event);
OS_STATUS OS_EventWait(OsEvent event, OS_TICKTYPE ticks_to_wait);
OS_STATUS OS_EventSet(OsEvent event);
u32	OS_EventGetStatus(OsEvent event);
OS_STATUS OS_EventDelete(OsEvent event);

/* Delay: */
/**
 * delay ms
 *
 * @param   ms                    		ms count
 * 
 */
void OS_MsDelay(u32 ms);
/**
 * delay us
 *
 * @param   us                    		us count
 * 
 */
void OS_UsDelay(u32 us);
/**
 * delay tick
 *
 * @param   ticks                    	tick count
 * 
 */
void OS_TickDelay(OS_TICKTYPE ticks);

/* Timer: */
/**
 * create softtimer
 *
 * @param   timer                    	pointer to timer handle
 * @param   ms                    		timer period in ms
 * @param	autoReload					0: one-shot 1:autoreload
 * @param	args						pointer to timer id, timer id is an arbitrary value
 * @param	timHandler					timer callback
 *
 * @retval  OS_SUCCESS                  create softtimer success
 * @retval  OS_FAILED                  	create softtimer failed
 */
OS_STATUS OS_TimerCreate( OsTimer *timer, u32 ms, u8 autoReload, void *args, OsTimerHandler timHandler );
/**
 * change softtimer
 *
 * @param   timer                    	timer handle
 * @param   ms                    		timer period in ms
 * @param	autoReload					0: one-shot 1:autoreload
 * @param	args						pointer to timer id, timer id is an arbitrary value
 *
 * @retval  1                  			change softtimer success
 * @retval  0                  			change softtimer failed
 */
u8 OS_TimerSet( OsTimer timer, u32 ms, u8 autoReload, void *args );
u8 OS_TimerCheckAutoReload(OsTimer timer);
/**
 * start softtimer
 *
 * @param   timer                    	timer handle
 *
 * @retval	0:failed 1:success
 */
u8 OS_TimerStart( OsTimer timer );
/**
 * stop softtimer
 *
 * @param   timer                    	timer handle
 *
 * @retval	0:failed 1:success
 */
u8 OS_TimerStop( OsTimer timer );
/**
 * reset softtimer
 *
 * @param   timer                    	timer handle
 *
 * @retval	0:failed 1:success
 */
u8 OS_TimerReset( OsTimer timer );
/**
 * delete softtimer
 *
 * @param   timer                    	timer handle
 *
 * @retval	0:failed 1:success
 */
u8 OS_TimerDelete( OsTimer timer );
/**
 * check softtimer active
 *
 * @param   timer                    	timer handle
 *
 * @retval	0:inactive 1:active
 */
u8 OS_TimerIsActive( OsTimer timer);
/**
 * get softtimer id
 *
 * @param   timer                    	timer handle
 *
 * @retval	pointer to timer id
 */
void *OS_TimerGetData( OsTimer timer );

/* Message Queue: */
OS_STATUS OS_MsgQCreate( OsMsgQ *MsgQ, u32 QLen );
void OS_MsgQDelete( OsMsgQ MsgQ);
OS_STATUS OS_MsgQEnqueue( OsMsgQ MsgQ, OsMsgQEntry *MsgItem );
/**
 * get a message from MsgQ
 *
 * @param   MsgQ                    	message queue handle
 * @param   MsgItem                    	pointer of OsMsgQEntry
 * @param   TickToWait                  system tick count to wait
 *
 * @retval  OS_SUCCESS                  get a message success
 * @retval  OS_TIMEOUT                  get a message timeout
 */
OS_STATUS OS_MsgQDequeue( OsMsgQ MsgQ, OsMsgQEntry *MsgItem, OS_TICKTYPE TickToWait );
u32 OS_MsgQWaitingSize( OsMsgQ MsgQ );

/* Buffer Queue: */
OS_STATUS OS_BufQCreate(OsBufQ *bufq, u32 qlen, u32 item_size);
void OS_BufQDelete(OsBufQ bufq);
OS_STATUS OS_BufQPush( OsBufQ bufq, void* item);
OS_STATUS OS_BufQPushEx( OsBufQ bufq, void* item, OS_TICKTYPE tick_to_wait);
OS_STATUS OS_BufQPushFront( OsBufQ bufq, void* item);
OS_STATUS OS_BufQPop( OsBufQ bufq, void* item, OS_TICKTYPE tick_to_wait);
u32 OS_BufQWaitingSize( OsBufQ bufq );

/* Memory: */
/**
 * print total size/remain size of system heap
 * print every free block
 * print kernel used heap size & user used heap size
 * print allocated memory in every task
 *
 */
void OS_MemInfo(void);
/**
 * init system heap
 *
 * @retval  0							system heap init failed
 * @retval  1                  			system heap init success
 */
u8 OS_MemInit(void);
u32 OS_MemTotalSize(void);
u32 OS_MemRemainSize(void);
u32 OS_MemMaxFreeBlockSize(void);
u32 OS_MemUsedByKernel(void);
u32 OS_MemUsedByUser(void);
void *OS_MemAlloc( u32 size );
void OS_MemFree( void *m );
void *OS_MemRealloc( void* m, u32 newsize );
void *OS_MemCalloc(u32 count, u32 size);
void *OS_MemZalloc(u32 size);

/* PSRAM: */
u8 OS_PsramInit(void);
u32 OS_PsramTotalSize(void);
u32 OS_PsramRemainSize(void);
u32 OS_PsramMaxFreeBlockSize(void);
u32 OS_PsramUsedByKernel(void);
u32 OS_PsramUsedByUser(void);
void *OS_PsramAlloc( u32 size );
void OS_PsramFree( void *m );
void *OS_PsramRealloc( void* m, u32 newsize );
void *OS_PsramCalloc(u32 count, u32 size);
void *OS_PsramZalloc(u32 size);

/* Critical Sections */
u32 OS_IntSave(void);
void OS_IntRestore(u32);

#ifdef CONFIG_OS_RHINO
    #define OS_DeclareCritical()        CPSR_ALLOC()
    #define OS_EnterCritical()          RHINO_CRITICAL_ENTER()
    #define OS_ExitCritical()           RHINO_CRITICAL_EXIT()
    #define OS_EnterISR()               krhino_intrpt_enter()
    #define OS_ExitISR()                krhino_intrpt_exit()
#else
    extern void vPortEnterCritical();
    extern void vPortExitCritical();
    #define OS_DeclareCritical()
    #define OS_EnterCritical()          vPortEnterCritical()
    #define OS_ExitCritical()           vPortExitCritical()
    #define OS_EnterISR()
    #define OS_ExitISR()
#endif

#endif /* _RTOS_H_ */


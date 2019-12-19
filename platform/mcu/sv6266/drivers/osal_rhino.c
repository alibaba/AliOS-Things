#include <stdlib.h>
#include <string.h>
#include "soc_defs.h"
#include "osal.h"


void OS_TaskSwitchInfo(void)
{
    // aMH do nothing.
}

u8 OS_FromISR( void )
{
    return (gOsFromISR>0) ? true : false;
}

u32 OS_Random(void)
{
    return rand();
}

u32 OS_IntSave(void)
{
    size_t psw = __nds32__mfsr(NDS32_SR_PSW);
    __nds32__gie_dis();
    return psw;
}

void OS_IntRestore(u32 psw)
{
    __nds32__mtsr(psw, NDS32_SR_PSW);
}

/* Task: */
void  OS_Init( void )
{
    krhino_init();
}

void OS_StatInit( void )
{
}

u8 OS_TaskCreate( OsTask task, const char *name, u16 stackSize, void *param, u32 pri, OsTaskHandle *taskHandle )
{
    kstat_t ret;
    ktask_t *task_tmp;
    u32 real_pri = (OS_TASK_PRIOBASE - pri);

    ret = krhino_task_dyn_create(&task_tmp, name, param, real_pri, 0, stackSize, (task_entry_t)task, 1);

    if (ret == RHINO_SUCCESS) {
    if(taskHandle)
    {
        *taskHandle = task_tmp;
    }
        return true;
    }

    return false;
}

const char * OS_TaskGetName (OsTaskHandle taskHandle)
{
    ktask_t* handle;
    if(!taskHandle)
    {
        handle = krhino_cur_task_get();
    }
    else
    {
        handle = (ktask_t*)taskHandle;
    }
    return handle->task_name;
}

OsTaskHandle OS_TaskGetCurrHandle(void)
{
    return ((OsTaskHandle)krhino_cur_task_get());
}

u32 OS_TaskGetPriority(OsTaskHandle taskHandle)
{
    return ( OS_TASK_PRIOBASE - (((ktask_t*)taskHandle)->prio) );
}

void OS_TaskSetPriority(OsTaskHandle taskHandle, u32 prio)
{
    u8 old_pri;
    krhino_task_pri_change((ktask_t*)taskHandle, (OS_TASK_PRIOBASE-prio), &old_pri);
}

void OS_TaskDelete(OsTaskHandle taskHandle)
{
    if (taskHandle == NULL){
        krhino_task_dyn_del(NULL);
    } else {
        krhino_task_dyn_del((ktask_t *)taskHandle);
    }
}

void OS_TaskSuspend( OsTaskHandle taskHandle )
{
    ktask_t* handle;
    if(!taskHandle)
    {
        handle = krhino_cur_task_get();
    }
    else
    {
        handle = (ktask_t*)taskHandle;
    }
    krhino_task_suspend(handle);
}

void OS_TaskResume( OsTaskHandle taskHandle )
{
    krhino_task_resume((ktask_t*)taskHandle);
}

void OS_SysInfo(void)
{
}

void OS_StartScheduler( void )
{
    krhino_start();
}

void OS_StopScheduler( void )
{
}

void OS_ForceScheduler( void )
{
    /* trigger swi*/
    __nds32__mtsr(0x10000,NDS32_SR_INT_PEND);
    __nds32__isb();
}

void OS_SuspendScheduler( void )
{
    krhino_sched_disable();
}

void OS_ResumeScheduler( void )
{
    krhino_sched_enable();
}

OS_TICKTYPE OS_GetSysTick(void)
{
    return (u32)krhino_sys_tick_get();
}


/* Mutex APIs: */
OS_STATUS OS_MutexInit( OsMutex *mutex )
{
    kstat_t ret;

    ret = krhino_mutex_dyn_create((kmutex_t**)mutex, "mutex");

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }

    return OS_FAILED;
}


OS_STATUS OS_MutexDelete( OsMutex mutex )
{
    krhino_mutex_dyn_del((kmutex_t*)mutex);
    return OS_SUCCESS;
}

OS_STATUS OS_MutexLock( OsMutex mutex )
{
    kstat_t ret;

    ret = krhino_mutex_lock((kmutex_t*)mutex, RHINO_WAIT_FOREVER);

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }

    return OS_FAILED;
}

OS_STATUS OS_MutexUnLock( OsMutex mutex )
{
    kstat_t ret;

    ret = krhino_mutex_unlock((kmutex_t*)mutex);

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }

    return OS_FAILED;
}

void OS_MsDelay(u32 ms)
{
    OS_TICKTYPE ticks;

    ticks = krhino_ms_to_ticks(ms);
    if (ticks == 0) {
        ticks = 1;
    }

    krhino_task_sleep(ticks);
}

void OS_UsDelay(u32 us)
{
    OS_TICKTYPE ticks;

    ticks = krhino_ms_to_ticks(us/1000);
    if (ticks == 0) {
        ticks = 1;
    }

    krhino_task_sleep(ticks);
}

void OS_TickDelay(OS_TICKTYPE ticks)
{
    krhino_task_sleep(ticks);
}


/* Message Queue: */
OS_STATUS OS_MsgQCreate( OsMsgQ *MsgQ, u32 QLen )
{
    kstat_t ret;

    ret = krhino_buf_queue_dyn_create((kbuf_queue_t**)MsgQ, "default_queue", (QLen*sizeof(OsMsgQEntry)), sizeof(OsMsgQEntry));

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }

    return OS_FAILED;
}


OS_STATUS OS_MsgQEnqueue( OsMsgQ MsgQ, OsMsgQEntry *MsgItem )
{
    kstat_t ret;
    kbuf_queue_t *q = (kbuf_queue_t*)MsgQ;

    ret = krhino_buf_queue_send(q, MsgItem, q->max_msg_size);

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }

    return OS_FAILED;
}

OS_STATUS OS_MsgQDequeue( OsMsgQ MsgQ, OsMsgQEntry *MsgItem, OS_TICKTYPE TickToWait )
{
    kstat_t ret;
    size_t msg_len;

    ret = krhino_buf_queue_recv((kbuf_queue_t*)MsgQ, TickToWait, MsgItem, &msg_len);

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }

    return OS_FAILED;
}

u32 OS_MsgQWaitingSize( OsMsgQ MsgQ )
{
    u32 num;
    CPSR_ALLOC();

    kbuf_queue_t *q = (kbuf_queue_t*)MsgQ;

    RHINO_CRITICAL_ENTER();

    num = q->cur_num;

    RHINO_CRITICAL_EXIT();

    return num;
}

void OS_MsgQDelete( OsMsgQ MsgQ)
{
    krhino_buf_queue_dyn_del((kbuf_queue_t*)MsgQ);
}

/* Buffer Queue: */
OS_STATUS OS_BufQCreate(OsBufQ *bufq, u32 qlen, u32 item_size)
{
    kstat_t ret;

    ret = krhino_buf_queue_dyn_create((kbuf_queue_t**)bufq, "default_queue", (qlen*item_size), item_size);

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }

    return OS_FAILED;
}

void OS_BufQDelete(OsBufQ bufq)
{
    krhino_buf_queue_dyn_del((kbuf_queue_t*)bufq);
}

OS_STATUS OS_BufQPush( OsBufQ bufq, void* item)
{
    kstat_t ret;
    kbuf_queue_t *q = (kbuf_queue_t*)bufq;

    ret = krhino_buf_queue_send(q, item, q->max_msg_size);

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }

    return OS_FAILED;
}

OS_STATUS OS_BufQPushEx( OsBufQ bufq, void* item, OS_TICKTYPE tick_to_wait)
{
    // aMH: todo review this pushEx implement is right?
    return OS_BufQPush(bufq, item);
}

OS_STATUS OS_BufQPushFront( OsBufQ bufq, void* item)
{
    kstat_t ret;
    kbuf_queue_t *q = (kbuf_queue_t*)bufq;

    ret = krhino_buf_queue_send_front(q, item, q->max_msg_size);

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }

    return OS_FAILED;
}

OS_STATUS OS_BufQPop( OsBufQ bufq, void* item, OS_TICKTYPE tick_to_wait)
{
    kstat_t ret;
    size_t msg_len;

    ret = krhino_buf_queue_recv((kbuf_queue_t*)bufq, tick_to_wait, item, &msg_len);

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }

    return OS_FAILED;
}

u32 OS_BufQWaitingSize( OsBufQ bufq )
{
    u32 num;
    CPSR_ALLOC();

    kbuf_queue_t *q = (kbuf_queue_t*)bufq;

    RHINO_CRITICAL_ENTER();

    num = q->cur_num;

    RHINO_CRITICAL_EXIT();

    return num;
}

/* Event */
OS_STATUS OS_EventCreate(OsEvent* event)
{
    kstat_t ret;

    ret = krhino_event_dyn_create((kevent_t **)event, "event", 0x00000000);

    if (ret == RHINO_SUCCESS)
    {
        return OS_SUCCESS;
    }
    else
    {
        return OS_FAILED;
    }
}

OS_STATUS OS_EventWait(OsEvent event, OS_TICKTYPE ticks_to_wait)
{
    u32 real_flag;
    if(!event)
    {
        return OS_FAILED;
    }

    krhino_event_get((kevent_t*)event, 0x00000001, RHINO_OR_CLEAR, &real_flag, ticks_to_wait);

    if(real_flag & 0x00000001)
    {
        return OS_SUCCESS;
    }
    else
    {
        return OS_FAILED;
    }
}

OS_STATUS OS_EventSet(OsEvent event)
{
    kstat_t ret;

    if(!event)
    {
        return OS_FAILED;
    }

    ret = krhino_event_set((kevent_t*)event, 0x00000001, RHINO_OR);

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }
    else
    {
        return OS_FAILED;
    }
}

u32	OS_EventGetStatus(OsEvent event)
{
    u32 real_flag;
    if(!event)
    {
        return 0;
    }

    krhino_event_get((kevent_t*)event, 0x00000001, RHINO_OR_CLEAR, &real_flag, RHINO_NO_WAIT);

    return real_flag;
}

OS_STATUS OS_EventDelete(OsEvent event)
{
    if(!event)
    {
        return OS_FAILED;
    }

    krhino_event_del((kevent_t*)event);
    return OS_SUCCESS;
}

/* Timer: */
OS_STATUS OS_TimerCreate( OsTimer *timer, u32 ms, u8 autoReload, void *args, OsTimerHandler timHandler )
{
    kstat_t ret;

    if(autoReload)
    {
        ret = krhino_timer_dyn_create(
            (ktimer_t**)timer,
            "timer",
            timHandler,
            krhino_ms_to_ticks(ms),
            krhino_ms_to_ticks(ms),
            args,
            0);
    }
    else
    {
        ret = krhino_timer_dyn_create(
            (ktimer_t**)timer,
            "timer",
            timHandler,
            krhino_ms_to_ticks(ms),
            0,
            args,
            0);
    }

    if (ret == RHINO_SUCCESS)
    {
        return OS_SUCCESS;
    }
    else
    {
        return OS_FAILED;
    }

}

u8 OS_TimerSet( OsTimer timer, u32 ms, u8 autoReload, void *args )
{
    kstat_t ret;

    if(autoReload)
    {
        ret = krhino_timer_arg_change((ktimer_t*)timer, args);
        if(ret != RHINO_SUCCESS)
        {
            return OS_FAILED;
        }

        ret = krhino_timer_change((ktimer_t*)timer, krhino_ms_to_ticks(ms), krhino_ms_to_ticks(ms));
        if(ret != RHINO_SUCCESS)
        {
            return OS_FAILED;
        }
    }
    else
    {
        ret = krhino_timer_arg_change((ktimer_t*)timer, args);
        if(ret != RHINO_SUCCESS)
        {
            return OS_FAILED;
        }

        ret = krhino_timer_change((ktimer_t*)timer, krhino_ms_to_ticks(ms), 0);
        if(ret != RHINO_SUCCESS)
        {
            return OS_FAILED;
        }
    }

    return OS_SUCCESS;
}

u8 OS_TimerCheckAutoReload(OsTimer timer)
{
    ktimer_t* handle = (ktimer_t*)timer;

    if(handle->round_ticks)
    {
        return true;
    }
    else
    {
        return false;
    }
}

u8 OS_TimerStart( OsTimer timer )
{
    if(!timer)
    {
        return false;
    }

    if( RHINO_SUCCESS == krhino_timer_start((ktimer_t*)timer) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

u8 OS_TimerStop( OsTimer timer )
{
    if(!timer)
    {
        return false;
    }

    if( RHINO_SUCCESS == krhino_timer_stop((ktimer_t*)timer) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

u8 OS_TimerReset( OsTimer timer )
{
    OS_TICKTYPE ticks;

    if(!timer)
    {
        return false;
    }

    if( RHINO_SUCCESS == krhino_timer_stop((ktimer_t*)timer) )
    {
        if( RHINO_SUCCESS == krhino_timer_start((ktimer_t*)timer) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

u8 OS_TimerDelete( OsTimer timer )
{
    kstat_t ret;

    if(!timer)
    {
        return false;
    }

    if(RHINO_SUCCESS == krhino_timer_dyn_del((ktimer_t*)timer))
    {
        return true;
    }
    else
    {
        return false;
    }
}

u8 OS_TimerIsActive( OsTimer timer)
{
    if( TIMER_ACTIVE == ((ktimer_t*)timer)->timer_state )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void *OS_TimerGetData( OsTimer timer )
{
    return ((ktimer_t*)timer)->timer_cb_arg;
}

/* Semaphore: */
OS_STATUS OS_SemInit( OsSemaphore* Sem , u32 maxcnt , u32 cnt)
{
    kstat_t ret;

    ret = krhino_sem_dyn_create((ksem_t**)Sem, "sema", cnt);

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }

    return OS_FAILED;
}

OS_STATUS OS_SemWait( OsSemaphore Sem , OS_TICKTYPE timeout)
{
    kstat_t ret;

    ret =  krhino_sem_take((ksem_t*)Sem, timeout);

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }

    return OS_TIMEOUT;
}

u32 OS_SemGetCount( OsSemaphore Sem )
{
    u32 count;
    krhino_sem_count_get((ksem_t*)Sem, &count);
    return count;
}

OS_STATUS OS_SemSignal( OsSemaphore Sem )
{
    kstat_t ret;

    ret = krhino_sem_give((ksem_t*)Sem);

    if (ret == RHINO_SUCCESS) {
        return OS_SUCCESS;
    }

    return OS_FAILED;
}

OS_STATUS OS_SemDelete(OsSemaphore Sem)
{
    krhino_sem_dyn_del((ksem_t*)Sem);
}

/* Memory: */
//+++ OSAL API
extern k_mm_head  *g_kmm_head;

u8 OS_MemInit(void)
{
    // need to move
    return true;
}

void OS_MemInfo(void)
{
    // need to implement
}

u32 OS_MemTotalSize(void)
{
    return ( g_kmm_head->used_size + g_kmm_head->free_size );
}

u32 OS_MemRemainSize(void)
{
    return g_kmm_head->free_size;
}

void *OS_MemAlloc( u32 size )
{
    void *tmp = NULL;

    if (size == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        tmp = krhino_mm_alloc(size | AOS_UNSIGNED_INT_MSB);

    #ifndef AOS_BINS
        krhino_owner_attach(tmp, (size_t)__builtin_return_address(0));
    #endif
    } else {
        tmp = krhino_mm_alloc(size);
    }

#else
    tmp = krhino_mm_alloc(size);
#endif

    return tmp;
}

void OS_MemFree( void *mem )
{
    if (mem == NULL) {
        return;
    }

    krhino_mm_free(mem);
}

void* OS_MemCalloc(u32 count, u32 size)
{
    void *p;

    /* allocate 'count' objects of size 'size' */
    p = OS_MemAlloc(count * size);
    if (p)
    {
        /* zero the memory */
        memset(p, 0, count * size);
    }
    return p;
}

void* OS_MemZalloc(u32 size)
{
     return OS_MemCalloc(1, size);
}

void* OS_MemRealloc(void *mem, u32 size)
{
    void *tmp = NULL;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        tmp = krhino_mm_realloc(mem, size | AOS_UNSIGNED_INT_MSB);

#ifndef AOS_BINS
        krhino_owner_attach(tmp, (size_t)__builtin_return_address(0));
#endif
    } else {
        tmp = krhino_mm_realloc(mem, size);
    }

#else
    tmp = krhino_mm_realloc(mem, size);
#endif

    return tmp;
}
//---


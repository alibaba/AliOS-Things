#include "k_api.h"
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_RTT.h"

#ifdef AOS_COMP_TRACE

void SendTaskInfo(ktask_t *task)
{
  SEGGER_SYSVIEW_TASKINFO TaskInfo;

  memset(&TaskInfo, 0, sizeof(TaskInfo)); // Fill all elements with 0 to allow extending the structure in future version without breaking the code
  TaskInfo.TaskID     = (U32)task;
  TaskInfo.sName      = task->task_name;
  TaskInfo.Prio       = task->prio;
  TaskInfo.StackBase  = (U32)task->task_stack_base;
  TaskInfo.StackSize  = task->stack_size;
  SEGGER_SYSVIEW_SendTaskInfo(&TaskInfo);
}

void SEGGER_SYSVIEW_RecordObject(unsigned EventID, blk_obj_t *object)
{
    U8  aPacket[SEGGER_SYSVIEW_INFO_SIZE + 4 * SEGGER_SYSVIEW_QUANTA_U32];
    U8 *pPayload;

    pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);                      // Prepare the packet for SystemView
    pPayload = SEGGER_SYSVIEW_EncodeString(pPayload, object->name, SEGGER_SYSVIEW_MAX_STRING_LEN);    // Add object name

    SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, EventID);              // Send the packet
}

void SEGGER_SYSVIEW_RecordObject_Name(unsigned EventID, const name_t *name)
{
    U8  aPacket[SEGGER_SYSVIEW_INFO_SIZE + 4 * SEGGER_SYSVIEW_QUANTA_U32];
    U8 *pPayload;

    pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);                      // Prepare the packet for SystemView
    pPayload = SEGGER_SYSVIEW_EncodeString(pPayload, name, 128);    // Add object name

    SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, EventID);              // Send the packet
}

void trace_task_start_ready(ktask_t *task)
{
    SEGGER_SYSVIEW_OnTaskStartReady((unsigned)task);
}

void trace_task_stop_ready(ktask_t *task)
{
    SEGGER_SYSVIEW_OnTaskStopReady((unsigned)task, 0);
}

void trace_task_resume(ktask_t *task, ktask_t *task_resumed)
{
    // SEGGER_SYSVIEW_OnTaskStartReady((unsigned)task_resumed);
}

void trace_task_suspend(ktask_t *task, ktask_t *task_suspended)
{
    // SEGGER_SYSVIEW_OnTaskStopExec();
}

void trace_task_create(ktask_t *task)
{
    SEGGER_SYSVIEW_OnTaskCreate((unsigned)task);
    SendTaskInfo(task);

    SEGGER_SYSVIEW_OnTaskStartReady((unsigned)task);
}

void trace_task_wait_abort(ktask_t *task, ktask_t *task_abort)
{
    SEGGER_SYSVIEW_OnTaskTerminate((unsigned)task_abort);
}

void trace_task_switch(ktask_t *from, ktask_t *to)
{
    SEGGER_SYSVIEW_OnTaskStopReady((unsigned)from, 0);

    SEGGER_SYSVIEW_OnTaskStartExec((unsigned)to);
}

void trace_intrpt_task_switch(ktask_t *from, ktask_t *to)
{
    SEGGER_SYSVIEW_RecordExitISRToScheduler();
    trace_task_switch(from, to);
}

void trace_intrpt_exit(void)
{
    SEGGER_SYSVIEW_RecordExitISR();
}

void trace_intrpt_enter(void)
{
    SEGGER_SYSVIEW_RecordEnterISR();
}

void trace_init(void)
{
    SEGGER_SYSVIEW_Conf();
    printf("_SEGGER_RTT:%p\n", &_SEGGER_RTT);
#ifdef TRACE_BY_UART
    uart_port_init();
#endif
}

void trace_sem_create(ktask_t *task, ksem_t *sem)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_SEM_CREATE, &sem->blk_obj);
}

void trace_sem_overflow(ktask_t *task, ksem_t *sem)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_SEM_OVERFLOW, &sem->blk_obj);
}

void trace_sem_cnt_increase(ktask_t *task, ksem_t *sem)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_SEM_CNT_INCREASE, &sem->blk_obj);
}

void trace_sem_get_success(ktask_t *task, ksem_t *sem)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_SEM_GET_SUCCESS, &sem->blk_obj);
}

void trace_sem_get_blk(ktask_t *task, ksem_t *sem)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_SEM_GET_BLK, &sem->blk_obj);
}

void trace_sem_task_wake(ktask_t *task, ktask_t *task_waked_up, ksem_t *sem, uint8_t opt_wake_all)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_SEM_TASK_WAKE, &sem->blk_obj);
}

void trace_sem_del(ktask_t *task, ksem_t *sem)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_SEM_DEL, &sem->blk_obj);
}

void trace_sem_give(ksem_t *sem, uint8_t opt_wake_all)
{
    SEGGER_SYSVIEW_Record_StringU32(SYSVIEW_TRACE_ID_SEM_GIVE, sem->blk_obj.name, opt_wake_all);
}

void trace_mutex_create(ktask_t *task, kmutex_t *mutex, const name_t *name)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_MUTEX_CREATE, &mutex->blk_obj);
}

void trace_mutex_release(ktask_t *task, ktask_t *task_release, uint8_t new_pri)
{
    // SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_MUTEX_RELEASE,  );
}

void trace_mutex_get(ktask_t *task, kmutex_t *mutex, tick_t wait_option)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_MUTEX_GET, &mutex->blk_obj);
}

void trace_task_pri_inv(ktask_t *task, ktask_t *mtxtsk)
{
    // SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_TASK_PRI_INV,  );
}

void trace_mutex_get_blk(ktask_t *task, kmutex_t *mutex, tick_t wait_option)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_MUTEX_GET_BLK, &mutex->blk_obj);
}

void trace_mutex_release_success(ktask_t *task, kmutex_t *mutex)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_MUTEX_RELEASE_SUCCESS, &mutex->blk_obj);
}

void trace_mutex_task_wake(ktask_t *task, ktask_t *task_waked_up, kmutex_t *mutex)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_MUTEX_TASK_WAKE, &mutex->blk_obj);
}

void trace_mutex_del(ktask_t *task, kmutex_t *mutex)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_MUTEX_DEL, &mutex->blk_obj);
}

void trace_event_create(ktask_t *task, kevent_t *event, const name_t *name, uint32_t flags_init)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_EVENT_CREATE, &event->blk_obj);
}

void trace_event_get(ktask_t *task, kevent_t *event)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_EVENT_GET, &event->blk_obj);
}

void trace_event_get_blk(ktask_t *task, kevent_t *event, tick_t wait_option)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_EVENT_GET_BLK, &event->blk_obj);
}

void trace_event_task_wake(ktask_t *task, ktask_t *task_waked_up, kevent_t *event)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_EVENT_TASK_WAKE, &event->blk_obj);
}

void trace_event_del(ktask_t *task, kevent_t *event)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_EVENT_DEL, &event->blk_obj);
}

void trace_buf_queue_create(ktask_t *task, kqueue_t *buf_queue)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_BUF_QUEUE_CREATE, &buf_queue->blk_obj);
}

void trace_buf_queue_max(ktask_t *task, kqueue_t *buf_queue, void *msg, size_t msg_size)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_BUF_QUEUE_MAX, &buf_queue->blk_obj);
}

void trace_buf_queue_post(ktask_t *task, kqueue_t *buf_queue, void *msg, size_t msg_size)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_BUF_QUEUE_POST, &buf_queue->blk_obj);
}

void trace_buf_queue_task_wake(ktask_t *task, ktask_t *task_waked_up, kqueue_t *buf_queue)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_BUF_QUEUE_TASK_WAKE, &buf_queue->blk_obj);
}

void trace_buf_queue_get_blk(ktask_t *task, kqueue_t *buf_queue, tick_t wait_option)
{
    SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_BUF_QUEUE_GET_BLK, &buf_queue->blk_obj);
}

void trace_workqueue_init(ktask_t *task, kwork_t *work)
{
    // SEGGER_SYSVIEW_RecordObject(SYSVIEW_TRACE_ID_WORKQUEUE_INIT, );
}

void trace_workqueue_create(ktask_t *task, kworkqueue_t *workqueue)
{
    SEGGER_SYSVIEW_RecordObject_Name(SYSVIEW_TRACE_ID_WORKQUEUE_CREATE, workqueue->name);
}

void trace_workqueue_del(ktask_t *task, kworkqueue_t *workqueue)
{
    SEGGER_SYSVIEW_RecordObject_Name(SYSVIEW_TRACE_ID_WORKQUEUE_DEL, workqueue->name);
}

#endif
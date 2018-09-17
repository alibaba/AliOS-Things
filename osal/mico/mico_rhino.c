/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <string.h>
#include <stdlib.h>
#include "mico_rtos.h"
#include "mico_rtos_common.h"
#include "common.h"

typedef struct
{
    event_handler_t function;
    void* arg;
} mico_event_message_t;

OSStatus mico_rtos_create_thread( mico_thread_t* thread, uint8_t priority, const char* name, mico_thread_function_t function, uint32_t stack_size, mico_thread_arg_t arg )
{
    kstat_t ret;
    ktask_t *task_tmp;

    if (thread == NULL) {
        ret = krhino_task_dyn_create(&task_tmp, name, (void *)arg, priority, 0, stack_size/4, (task_entry_t)function, 1);
    } else {
        ret = krhino_task_dyn_create((ktask_t **)thread, name, (void *)arg, priority, 0, stack_size/4, (task_entry_t)function, 1);
    }

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_delete_thread( mico_thread_t* thread )
{
    kstat_t ret;

    if (thread == NULL) {
        ret = krhino_task_dyn_del(NULL);
    } else {
        ret = krhino_task_dyn_del(*((ktask_t **)thread));
    }

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

void mico_rtos_suspend_thread(mico_thread_t* thread)
{
    if (thread == NULL) {
        krhino_task_suspend(krhino_cur_task_get());        
    }
    else {
        krhino_task_suspend(*((ktask_t **)thread));
    }
}

void mico_rtos_suspend_all_thread(void)
{
    krhino_sched_disable();
}

long mico_rtos_resume_all_thread(void)
{
    kstat_t ret;

    ret = krhino_sched_enable();

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_thread_join( mico_thread_t* thread )
{
    return kNoErr;
}


OSStatus mico_rtos_thread_force_awake( mico_thread_t* thread )
{
    kstat_t ret;

    ret = krhino_task_wait_abort(*((ktask_t **)thread));

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}


bool mico_rtos_is_current_thread( mico_thread_t* thread )
{
    ktask_t *t;

    t = *((ktask_t **)thread);

    if (t == krhino_cur_task_get()) {
        return true;
    }

    return false;
}

OSStatus mico_rtos_delay_milliseconds( uint32_t num_ms )
{
    uint32_t ticks;

    ticks = krhino_ms_to_ticks(num_ms);
    if (ticks == 0) {
        ticks = 1;
    }

    krhino_task_sleep(ticks);

    return kNoErr;
}

OSStatus mico_rtos_print_thread_status( char* pcWriteBuffer, int xWriteBufferLen )
{
    pcWriteBuffer[0] = 'n';
    pcWriteBuffer[1] = '\0';

    (void)xWriteBufferLen;

    return kNoErr;
}

OSStatus mico_rtos_init_semaphore( mico_semaphore_t* semaphore, int count )
{
    kstat_t ret;

    ret = krhino_sem_dyn_create((ksem_t **)semaphore, "sema", 0);

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_set_semaphore( mico_semaphore_t* semaphore )
{
    kstat_t ret;

    ret = krhino_sem_give(*((ksem_t **)semaphore));

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_get_semaphore( mico_semaphore_t* semaphore, uint32_t timeout_ms )
{
    kstat_t ret;
    tick_t ticks;

    if (timeout_ms == MICO_NEVER_TIMEOUT) {
        ret =  krhino_sem_take(*((ksem_t **)semaphore), RHINO_WAIT_FOREVER);
    }
    else {
        ticks = krhino_ms_to_ticks(timeout_ms);
        ret =  krhino_sem_take(*((ksem_t **)semaphore), ticks);
    }

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_deinit_semaphore( mico_semaphore_t* semaphore )
{
    kstat_t ret;

    ret =  krhino_sem_dyn_del(*((ksem_t **)semaphore));

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_init_mutex( mico_mutex_t* mutex )
{
    kstat_t ret;

    ret = krhino_mutex_dyn_create((kmutex_t **)mutex, "mutex");

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_lock_mutex( mico_mutex_t* mutex )
{
    kstat_t ret;

    ret = krhino_mutex_lock(*((kmutex_t **)mutex), RHINO_WAIT_FOREVER);

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_unlock_mutex( mico_mutex_t* mutex )
{
    kstat_t ret;
	
    ret = krhino_mutex_unlock(*((kmutex_t **)mutex));

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_deinit_mutex( mico_mutex_t* mutex )
{
    kstat_t ret;

    ret = krhino_mutex_dyn_del(*((kmutex_t **)mutex));

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_init_queue( mico_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages )
{
    kstat_t ret;
    
    if (name == NULL) {
        name = "default_queue";
    }

    ret = krhino_buf_queue_dyn_create((kbuf_queue_t **)queue, name, number_of_messages * (message_size + COMPRESS_LEN(message_size)), message_size);

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}


OSStatus mico_rtos_push_to_queue( mico_queue_t* queue, void* message, uint32_t timeout_ms )
{
    kstat_t ret;
    kbuf_queue_t *q = *((kbuf_queue_t **)queue);

    timeout_ms = timeout_ms;

    ret = krhino_buf_queue_send(q, message, q->max_msg_size);

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_pop_from_queue( mico_queue_t* queue, void* message, uint32_t timeout_ms )
{
    kstat_t ret;
    size_t msg_len;

    ret = krhino_buf_queue_recv(*((kbuf_queue_t **)queue), krhino_ms_to_ticks(timeout_ms), message, &msg_len);

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_deinit_queue( mico_queue_t* queue )
{
    kstat_t ret;

    ret = krhino_buf_queue_dyn_del(*((kbuf_queue_t **)queue));

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}


bool mico_rtos_is_queue_empty( mico_queue_t* queue )
{
    bool ret;
    CPSR_ALLOC();

    kbuf_queue_t *q = *((kbuf_queue_t **)queue);

    RHINO_CRITICAL_ENTER();

    if (q->cur_num == 0) {
        ret = true;
    }
    else {
        ret = false;;
    }

    RHINO_CRITICAL_EXIT();

    return ret;
}

bool mico_rtos_is_queue_full( mico_queue_t* queue )
{
    bool ret;
    CPSR_ALLOC();

    kbuf_queue_t *q = *((kbuf_queue_t **)queue);
    uint32_t max_msg_num;

    RHINO_CRITICAL_ENTER();

    max_msg_num = (q->ringbuf.end - q->ringbuf.buf) / (q->max_msg_size + COMPRESS_LEN(q->max_msg_size));

    if (q->cur_num == max_msg_num) {
        ret =  true;
    }
    else {
        ret = false;
    }

    RHINO_CRITICAL_EXIT();

    return ret;
}

mico_time_t mico_rtos_get_time( void )
{
    return krhino_ticks_to_ms(krhino_sys_tick_get());
}

static void timmer_wrapper(void *timer, void *arg)
{
    (void)timer;

    mico_timer_t *timer_arg = arg;

    if (timer_arg->function != 0) {
        timer_arg->function(timer_arg->arg);
    }
}

OSStatus mico_rtos_init_timer( mico_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg )
{
    kstat_t ret;

    timer->function = function;
    timer->arg      = arg;

    ret = krhino_timer_dyn_create((ktimer_t **)(&timer->handle),"timer", timmer_wrapper, 
                                  krhino_ms_to_ticks(time_ms), krhino_ms_to_ticks(time_ms), timer, 0);

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_init_oneshot_timer( mico_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg )
{
    kstat_t ret;

    timer->function = function;
    timer->arg      = arg;

    ret = krhino_timer_dyn_create((ktimer_t **)(&timer->handle),"timer", timmer_wrapper, 
                                  krhino_ms_to_ticks(time_ms), 0, timer, 0);

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}


OSStatus mico_rtos_start_timer( mico_timer_t* timer )
{
    kstat_t ret;

    ret = krhino_timer_start((ktimer_t *)(timer->handle));

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_stop_timer( mico_timer_t* timer )
{
    kstat_t ret;

    ret = krhino_timer_stop((ktimer_t *)(timer->handle));


    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}


OSStatus mico_rtos_reload_timer( mico_timer_t* timer )
{
    kstat_t ret;


    krhino_timer_stop((ktimer_t *)(timer->handle));

    ret = krhino_timer_start((ktimer_t *)(timer->handle));

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

OSStatus mico_rtos_deinit_timer( mico_timer_t* timer )
{
    kstat_t ret;


    krhino_timer_stop((ktimer_t *)(timer->handle));
    ret = krhino_timer_dyn_del((ktimer_t *)(timer->handle));

    if (ret == RHINO_SUCCESS) {
        return kNoErr;
    }

    return kGeneralErr;
}

bool mico_rtos_is_timer_running( mico_timer_t* timer )
{
    ktimer_t *t;

    t = (ktimer_t *)timer->handle;

    if (t->timer_state == TIMER_ACTIVE) {

        return true;
    }

    return false;
}

bool mico_rtos_is_timer_init( mico_timer_t* timer )
{
	if (timer == NULL)
		return false;
	if (timer->handle == NULL)
		return false;

    return true;
}

static void worker_thread_main( uint32_t arg )
{
    mico_worker_thread_t* worker_thread = (mico_worker_thread_t*) arg;

    while ( 1 )
    {
        mico_event_message_t message;

        if ( mico_rtos_pop_from_queue( &worker_thread->event_queue, &message, MICO_WAIT_FOREVER ) == kNoErr )
        {
            message.function( message.arg );
        }
    }
}

OSStatus mico_rtos_create_worker_thread( mico_worker_thread_t* worker_thread, uint8_t priority, uint32_t stack_size, uint32_t event_queue_size )
{
    memset( worker_thread, 0, sizeof( *worker_thread ) );

    if ( mico_rtos_init_queue( &worker_thread->event_queue, "worker queue", sizeof(mico_event_message_t), event_queue_size ) != kNoErr )
    {
        return kGeneralErr;
    }

    if ( mico_rtos_create_thread( &worker_thread->thread, priority , "worker thread", worker_thread_main, stack_size, (mico_thread_arg_t) worker_thread ) != kNoErr )
    {
        mico_rtos_deinit_queue( &worker_thread->event_queue );
        return kGeneralErr;
    }

    return kNoErr;
}

OSStatus mico_rtos_delete_worker_thread( mico_worker_thread_t* worker_thread )
{
    if ( mico_rtos_delete_thread( &worker_thread->thread ) != kNoErr )
    {
        return kGeneralErr;
    }

    if ( mico_rtos_deinit_queue( &worker_thread->event_queue ) != kNoErr )
    {
        return kGeneralErr;
    }

    return kNoErr;
}

OSStatus mico_rtos_send_asynchronous_event( mico_worker_thread_t* worker_thread, event_handler_t function, void* arg )
{
    mico_event_message_t message;

    if( worker_thread->thread == NULL )
        return kNotInitializedErr;

    message.function = function;
    message.arg = arg;

    return mico_rtos_push_to_queue( &worker_thread->event_queue, &message, MICO_NO_WAIT );
}

static void timed_event_handler( void* arg )
{
    mico_timed_event_t* event_object = (mico_timed_event_t*) arg;
    mico_event_message_t message;

    message.function = event_object->function;
    message.arg = event_object->arg;

    mico_rtos_push_to_queue( &event_object->thread->event_queue, &message, MICO_NO_WAIT );
}

OSStatus mico_rtos_register_timed_event( mico_timed_event_t* event_object, mico_worker_thread_t* worker_thread, event_handler_t function, uint32_t time_ms, void* arg )
{
    if( worker_thread->thread == NULL )
        return kNotInitializedErr;

    if ( mico_rtos_init_timer( &event_object->timer, time_ms, timed_event_handler, (void*) event_object ) != kNoErr )
    {
        return kGeneralErr;
    }

    event_object->function = function;
    event_object->thread = worker_thread;
    event_object->arg = arg;

    if ( mico_rtos_start_timer( &event_object->timer ) != kNoErr )
    {
        mico_rtos_deinit_timer( &event_object->timer );
        return kGeneralErr;
    }

    return kNoErr;
}

OSStatus mico_rtos_deregister_timed_event( mico_timed_event_t* event_object )
{
    if ( mico_rtos_deinit_timer( &event_object->timer ) != kNoErr )
    {
        return kGeneralErr;
    }


    return kNoErr;
}


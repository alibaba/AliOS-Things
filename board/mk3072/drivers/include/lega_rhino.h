/*
 * Copyright (C) 2015-2018 ASR Group Holding Limited
 */
#ifndef __LEGARTOS_H__
#define __LEGARTOS_H__
#include <k_api.h>
#include <ctype.h>
#include "port.h"

#define LEGA_NEVER_TIMEOUT   (0xFFFFFFFF)
#define LEGA_WAIT_FOREVER    (0xFFFFFFFF)
#define LEGA_NO_WAIT         (0)

typedef enum
{
    kNoErr=0,
    kGeneralErr,
    kTimeoutErr,
}OSStatus;

typedef enum
{
  FALSE=0,
  TRUE=1,
}OSBool;

typedef void * lega_semaphore_t;
typedef void * lega_mutex_t;
typedef void * lega_thread_t;
typedef void * lega_queue_t;
//typedef void * lega_event_t;// LEGA OS event: lega_semaphore_t, lega_mutex_t or lega_queue_t
typedef void (*timer_handler_t)( void* arg );
//typedef OSStatus (*event_handler_t)( void* arg );

typedef struct
{
    void *          handle;
    timer_handler_t function;
    void *          arg;
}lega_timer_t;

typedef uint32_t lega_thread_arg_t;
typedef void (*lega_thread_function_t)( lega_thread_arg_t arg );


/** @defgroup LEGA_RTOS_Thread LEGA RTOS Thread Management Functions
 *  @brief Provide thread creation, delete, suspend, resume, and other RTOS management API
 *  @verbatim
 *   LEGA thread priority table
 *
 * +----------+-----------------+
 * | Priority |      Thread     |
 * |----------|-----------------|
 * |     0    |      LEGA       |   Highest priority
 * |     1    |     Network     |
 * |     2    |                 |
 * |     3    | Network worker  |
 * |     4    |                 |
 * |     5    | Default Library |
 * |          | Default worker  |
 * |     6    |                 |
 * |     7    |   Application   |
 * |     8    |                 |
 * |     9    |      Idle       |   Lowest priority
 * +----------+-----------------+
 *  @endverbatim
 * @{
 */
 OSBool lega_rtos_is_in_interrupt_context(void);


/** @brief Creates and starts a new thread
  *
  * @param thread     : Pointer to variable that will receive the thread handle (can be null)
  * @param priority   : A priority number.
  * @param name       : a text name for the thread (can be null)
  * @param function   : the main thread function
  * @param stack_size : stack size for this thread
  * @param arg        : argument which will be passed to thread function
  *
  * @return    kNoErr          : on success.
  * @return    kGeneralErr     : if an error occurred
  */
OSStatus lega_rtos_create_thread( lega_thread_t* thread, uint8_t priority, const char* name, lega_thread_function_t function, uint32_t stack_size, lega_thread_arg_t arg );

/** @brief   Deletes a terminated thread
  *
  * @param   thread     : the handle of the thread to delete, , NULL is the current thread
  *
  * @return  kNoErr        : on success.
  * @return  kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_delete_thread( lega_thread_t* thread );




/** @defgroup LEGA_RTOS_SEM LEGA RTOS Semaphore Functions
  * @brief Provide management APIs for semaphore such as init,set,get and dinit.
  * @{
  */

/** @brief    Initialises a counting semaphore and set count to 0
  *
  * @param    semaphore : a pointer to the semaphore handle to be initialised
  * @param    count     : the max count number of this semaphore
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_init_semaphore( lega_semaphore_t* semaphore, int count );


/** @brief    Set (post/put/increment) a semaphore
  *
  * @param    semaphore : a pointer to the semaphore handle to be set
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_set_semaphore( lega_semaphore_t* semaphore );


/** @brief    Get (wait/decrement) a semaphore
  *
  * @Details  Attempts to get (wait/decrement) a semaphore. If semaphore is at zero already,
  *           then the calling thread will be suspended until another thread sets the
  *           semaphore with @ref lega_rtos_set_semaphore
  *
  * @param    semaphore : a pointer to the semaphore handle
  * @param    timeout_ms: the number of milliseconds to wait before returning
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_get_semaphore( lega_semaphore_t* semaphore, uint32_t timeout_ms );


/** @brief    De-initialise a semaphore
  *
  * @Details  Deletes a semaphore created with @ref lega_rtos_init_semaphore
  *
  * @param    semaphore : a pointer to the semaphore handle
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_deinit_semaphore( lega_semaphore_t* semaphore );
/**
  * @}
  */

#define lega_rtos_declare_critical() CPSR_ALLOC()
/** @brief Enter a critical session, all interrupts are disabled
  *
  * @return    none
  */
#define lega_rtos_enter_critical() RHINO_CRITICAL_ENTER()
//void lega_rtos_enter_critical( void );

/** @brief Exit a critical session, all interrupts are enabled
  *
  * @return    none
  */
#define lega_rtos_exit_critical() RHINO_CRITICAL_EXIT()
//void lega_rtos_exit_critical( void );

/** @defgroup LEGA_RTOS_MUTEX LEGA RTOS Mutex Functions
  * @brief Provide management APIs for Mutex such as init,lock,unlock and dinit.
  * @{
  */

/** @brief    Initialises a mutex
  *
  * @Details  A mutex is different to a semaphore in that a thread that already holds
  *           the lock on the mutex can request the lock again (nested) without causing
  *           it to be suspended.
  *
  * @param    mutex : a pointer to the mutex handle to be initialised
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_init_mutex( lega_mutex_t* mutex );


/** @brief    Obtains the lock on a mutex
  *
  * @Details  Attempts to obtain the lock on a mutex. If the lock is already held
  *           by another thead, the calling thread will be suspended until the mutex
  *           lock is released by the other thread.
  *
  * @param    mutex : a pointer to the mutex handle to be locked
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_lock_mutex( lega_mutex_t* mutex, uint32_t timeout_ms );


/** @brief    Releases the lock on a mutex
  *
  * @Details  Releases a currently held lock on a mutex. If another thread
  *           is waiting on the mutex lock, then it will be resumed.
  *
  * @param    mutex : a pointer to the mutex handle to be unlocked
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_unlock_mutex( lega_mutex_t* mutex );


/** @brief    De-initialise a mutex
  *
  * @Details  Deletes a mutex created with @ref lega_rtos_init_mutex
  *
  * @param    mutex : a pointer to the mutex handle
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_deinit_mutex( lega_mutex_t* mutex );
/**
  * @}
  */

/** @defgroup LEGA_RTOS_QUEUE LEGA RTOS FIFO Queue Functions
  * @brief Provide management APIs for FIFO such as init,push,pop and dinit.
  * @{
  */

/** @brief    Initialises a FIFO queue
  *
  * @param    queue : a pointer to the queue handle to be initialised
  * @param    name  : a text string name for the queue (NULL is allowed)
  * @param    message_size : size in bytes of objects that will be held in the queue
  * @param    number_of_messages : depth of the queue - i.e. max number of objects in the queue
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_init_queue( lega_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages );


/** @brief    Pushes an object onto a queue
  *
  * @param    queue : a pointer to the queue handle
  * @param    message : the object to be added to the queue. Size is assumed to be
  *                  the size specified in @ref lega_rtos_init_queue
  * @param    timeout_ms: the number of milliseconds to wait before returning
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error or timeout occurred
  */
OSStatus lega_rtos_push_to_queue( lega_queue_t* queue, void* message, uint32_t timeout_ms );


/** @brief    Pops an object off a queue
  *
  * @param    queue : a pointer to the queue handle
  * @param    message : pointer to a buffer that will receive the object being
  *                     popped off the queue. Size is assumed to be
  *                     the size specified in @ref lega_rtos_init_queue , hence
  *                     you must ensure the buffer is long enough or memory
  *                     corruption will result
  * @param    timeout_ms: the number of milliseconds to wait before returning
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error or timeout occurred
  */
OSStatus lega_rtos_pop_from_queue( lega_queue_t* queue, void* message, uint32_t timeout_ms );


/** @brief    De-initialise a queue created with @ref lega_rtos_init_queue
  *
  * @param    queue : a pointer to the queue handle
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_deinit_queue( lega_queue_t* queue );


/** @brief    Check if a queue is empty
  *
  * @param    queue : a pointer to the queue handle
  *
  * @return   true  : queue is empty.
  * @return   false : queue is not empty.
  */
OSBool lega_rtos_is_queue_empty( lega_queue_t* queue );


/** @brief    Check if a queue is full
  *
  * @param    queue : a pointer to the queue handle
  *
  * @return   true  : queue is empty.
  * @return   false : queue is not empty.
  */
OSBool lega_rtos_is_queue_full( lega_queue_t* queue );

/**
  * @}
  */

/** @defgroup LEGA_RTOS_TIMER LEGA RTOS Timer Functions
  * @brief Provide management APIs for timer such as init,start,stop,reload and dinit.
  * @{
  */


/**
  * @brief     Initialize a RTOS timer
  *
  * @note      Timer does not start running until @ref lega_start_timer is called
  *
  * @param     timer    : a pointer to the timer handle to be initialised
  * @param     time_ms  : Timer period in milliseconds
  * @param     function : the callback handler function that is called each time the
  *                       timer expires
  * @param     arg      : an argument that will be passed to the callback function
  *
  * @return    kNoErr        : on success.
  * @return    kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_init_timer( lega_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg );


/** @brief    Starts a RTOS timer running
  *
  * @note     Timer must have been previously initialised with @ref lega_rtos_init_timer
  *
  * @param    timer    : a pointer to the timer handle to start
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_start_timer( lega_timer_t* timer );


/** @brief    Stops a running RTOS timer
  *
  * @note     Timer must have been previously started with @ref lega_rtos_init_timer
  *
  * @param    timer    : a pointer to the timer handle to stop
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_stop_timer( lega_timer_t* timer );


/** @brief    Reloads a RTOS timer that has expired
  *
  * @note     This is usually called in the timer callback handler, to
  *           reschedule the timer for the next period.
  *
  * @param    timer    : a pointer to the timer handle to reload
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_reload_timer( lega_timer_t* timer );


/** @brief    De-initialise a RTOS timer
  *
  * @note     Deletes a RTOS timer created with @ref lega_rtos_init_timer
  *
  * @param    timer : a pointer to the RTOS timer handle
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_deinit_timer( lega_timer_t* timer );


/** @brief    Check if an RTOS timer is running
  *
  * @param    timer : a pointer to the RTOS timer handle
  *
  * @return   true        : if running.
  * @return   false       : if not running
  */
OSBool lega_rtos_is_timer_running( lega_timer_t* timer );

/**
  * @brief    Gets time in miiliseconds since RTOS start
  *
  * @note:    Since this is only 32 bits, it will roll over every 49 days, 17 hours.
  *
  * @returns  Time in milliseconds since RTOS started.
  */
uint32_t lega_rtos_get_time(void);

/**
  * @}
  */

/** @brief    Suspend current thread for a specific time
 *
 * @param     num_ms : A time interval (Unit: millisecond)
 *
 * @return    kNoErr.
 */
OSStatus lega_rtos_delay_milliseconds( uint32_t num_ms );

#define lega_rtos_malloc(s) _lega_rtos_malloc(s,__FUNCTION__,__LINE__)
void *_lega_rtos_malloc(uint32_t xWantedSize,const char * function,uint32_t line);
void lega_rtos_free(void *mem);

void lega_system_reset();

#if 0
/** @brief   Creates a worker thread
 *
 * Creates a worker thread
 * A worker thread is a thread in whose context timed and asynchronous events
 * execute.
 *
 * @param worker_thread    : a pointer to the worker thread to be created
 * @param priority         : thread priority
 * @param stack_size       : thread's stack size in number of bytes
 * @param event_queue_size : number of events can be pushed into the queue
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred
 */
OSStatus lega_rtos_create_worker_thread( lega_worker_thread_t* worker_thread, uint8_t priority, uint32_t stack_size, uint32_t event_queue_size );


/** @brief   Deletes a worker thread
 *
 * @param worker_thread : a pointer to the worker thread to be created
 *
 * @return    kNoErr : on success.
 * @return    kGeneralErr   : if an error occurred
 */
OSStatus lega_rtos_delete_worker_thread( lega_worker_thread_t* worker_thread );


/** @brief    Suspend a thread
  *
  * @param    thread     : the handle of the thread to suspend, NULL is the current thread
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
void lega_rtos_suspend_thread(lega_thread_t* thread);



/** @brief    Suspend all other thread
  *
  * @param    none
  *
  * @return   none
  */
void lega_rtos_suspend_all_thread(void);


/** @brief    Rresume all other thread
  *
  * @param    none
  *
  * @return   none
  */
long lega_rtos_resume_all_thread(void);


/** @brief    Sleeps until another thread has terminated
  *
  * @Details  Causes the current thread to sleep until the specified other thread
  *           has terminated. If the processor is heavily loaded with higher priority
  *           tasks, this thread may not wake until significantly after the thread termination.
  *
  * @param    thread : the handle of the other thread which will terminate
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_thread_join( lega_thread_t* thread );


/** @brief    Forcibly wakes another thread
  *
  * @Details  Causes the specified thread to wake from suspension. This will usually
  *           cause an error or timeout in that thread, since the task it was waiting on
  *           is not complete.
  *
  * @param    thread : the handle of the other thread which will be woken
  *
  * @return   kNoErr        : on success.
  * @return   kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_thread_force_awake( lega_thread_t* thread );


/** @brief    Checks if a thread is the current thread
  *
  * @Details  Checks if a specified thread is the currently running thread
  *
  * @param    thread : the handle of the other thread against which the current thread
  *                    will be compared
  *
  * @return   true   : specified thread is the current thread
  * @return   false  : specified thread is not currently running
  */
OSBool lega_rtos_is_current_thread( lega_thread_t* thread );






/** @brief    Print Thread status into buffer
  *
  * @param    buffer, point to buffer to store thread status
  * @param    length, length of the buffer
  *
  * @return   none
  */
OSStatus lega_rtos_print_thread_status( char* buffer, int length );

/**
  * @}
  */







/** @defgroup LEGA_RTOS_EVENT LEGA RTOS Event Functions
  * @{
  */

/**
  * @brief    Sends an asynchronous event to the associated worker thread
  *
  * @param worker_thread :the worker thread in which context the callback should execute from
  * @param function      : the callback function to be called from the worker thread
  * @param arg           : the argument to be passed to the callback function
  *
  * @return    kNoErr        : on success.
  * @return    kGeneralErr   : if an error occurred
  */
OSStatus lega_rtos_send_asynchronous_event( lega_worker_thread_t* worker_thread, event_handler_t function, void* arg );

/** Requests a function be called at a regular interval
 *
 * This function registers a function that will be called at a regular
 * interval. Since this is based on the RTOS time-slice scheduling, the
 * accuracy is not high, and is affected by processor load.
 *
 * @param event_object  : pointer to a event handle which will be initialised
 * @param worker_thread : pointer to the worker thread in whose context the
 *                        callback function runs on
 * @param function      : the callback function that is to be called regularly
 * @param time_ms       : the time period between function calls in milliseconds
 * @param arg           : an argument that will be supplied to the function when
 *                        it is called
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred
 */
OSStatus lega_rtos_register_timed_event( lega_timed_event_t* event_object, lega_worker_thread_t* worker_thread, event_handler_t function, uint32_t time_ms, void* arg );


/** Removes a request for a regular function execution
 *
 * This function de-registers a function that has previously been set-up
 * with @ref lega_rtos_register_timed_event.
 *
 * @param event_object : the event handle used with @ref lega_rtos_register_timed_event
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred
 */
OSStatus lega_rtos_deregister_timed_event( lega_timed_event_t* event_object );


/**
  * @}
  */


int SetTimer(unsigned long ms, void (*psysTimerHandler)(void));
int SetTimer_uniq(unsigned long ms, void (*psysTimerHandler)(void));
int UnSetTimer(void (*psysTimerHandler)(void));

/** @brief    Initialize an endpoint for a RTOS event, a file descriptor
  *           will be created, can be used for select
  *
  * @param    event_handle : lega_semaphore_t, lega_mutex_t or lega_queue_t
  *
  * @retval   On success, a file descriptor for RTOS event is returned.
  *           On error, -1 is returned.
  */
int lega_rtos_init_event_fd(lega_event_t event_handle);

/** @brief    De-initialise an endpoint created from a RTOS event
  *
  * @param    fd : file descriptor for RTOS event
  *
  * @retval   0 for success. On error, -1 is returned.
  */
int lega_rtos_deinit_event_fd(int fd);

/**
  * @}
  */
#endif
#endif //__LEGARTOS_H__
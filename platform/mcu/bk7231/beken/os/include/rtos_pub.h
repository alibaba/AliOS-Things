#ifndef __RTOS_PUB__
#define __RTOS_PUB__

#include "include.h"
#include "typedef.h"

#pragma once

#define RTOS_SUCCESS                       (1)
#define RTOS_FAILURE                       (0)

#define BEKEN_DEFAULT_WORKER_PRIORITY      (6)
#define BEKEN_APPLICATION_PRIORITY         (7)

#define kNanosecondsPerSecond              1000000000UUL
#define kMicrosecondsPerSecond             1000000UL
#define kMillisecondsPerSecond             1000

#define BEKEN_NEVER_TIMEOUT                (0xFFFFFFFF)
#define BEKEN_WAIT_FOREVER                 (0xFFFFFFFF)
#define BEKEN_NO_WAIT                      (0)

#define NANOSECONDS                        1000000UL
#define MICROSECONDS                       1000
#define MILLISECONDS                       (1)
#define SECONDS                            (1000)
#define MINUTES                            (60 * SECONDS)
#define HOURS                              (60 * MINUTES)
#define DAYS                               (24 * HOURS)

#if FreeRTOS_VERSION_MAJOR == 7
#define _xTaskCreate( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask ) xTaskCreate( pvTaskCode, (signed char*)pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask )
#define _xTimerCreate( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction ) xTimerCreate( (signed char*)pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction )
#else
#define _xTaskCreate( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask ) xTaskCreate( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask )
#define _xTimerCreate( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction ) xTimerCreate( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction )
#endif

typedef int             OSStatus;
typedef void (*timer_handler_t)( void*);
typedef OSStatus (*event_handler_t)( void* arg );

typedef void *          beken_thread_arg_t;
typedef uint8_t         beken_bool_t;
typedef uint32_t        beken_time_t;        /**< Time value in milliseconds */
typedef uint32_t        beken_utc_time_t;    /**< UTC Time in seconds        */
typedef uint64_t        beken_utc_time_ms_t; /**< UTC Time in milliseconds   */
typedef uint32_t        beken_event_flags_t;
typedef void *          beken_semaphore_t;
typedef void *          beken_mutex_t;
typedef void *          beken_thread_t;
typedef void *          beken_queue_t;
typedef void *          beken_event_t;        //  OS event: beken_semaphore_t, beken_mutex_t or beken_queue_t

typedef enum
{
    WAIT_FOR_ANY_EVENT,
    WAIT_FOR_ALL_EVENTS,
} beken_event_flags_wait_option_t;

typedef struct
{
    void *          handle;
    timer_handler_t function;
    void *          arg;
}beken_timer_t;

typedef struct
{
    beken_thread_t thread;
    beken_queue_t  event_queue;
} beken_worker_thread_t;

typedef struct
{
    event_handler_t        function;
    void*                  arg;
    beken_timer_t           timer;
    beken_worker_thread_t*  thread;
} beken_timed_event_t;

typedef void (*timer_2handler_t)( void* Larg, void* Rarg);

#define BEKEN_MAGIC_WORD       (0xBABA7231)
typedef struct
{
    void *          handle;
    timer_2handler_t function;
    void *          left_arg;
    void *          right_arg;
	uint32_t        beken_magic;
}beken2_timer_t;

typedef void (*beken_thread_function_t)( beken_thread_arg_t arg );

extern beken_worker_thread_t beken_hardware_io_worker_thread;
extern beken_worker_thread_t beken_worker_thread;


void rtos_enter_critical( void );


void rtos_exit_critical( void );


OSStatus rtos_create_thread( beken_thread_t* thread, uint8_t priority, const char* name, beken_thread_function_t function, uint32_t stack_size, beken_thread_arg_t arg );

OSStatus rtos_delete_thread( beken_thread_t* thread );


OSStatus rtos_create_worker_thread( beken_worker_thread_t* worker_thread, uint8_t priority, uint32_t stack_size, uint32_t event_queue_size );



OSStatus rtos_delete_worker_thread( beken_worker_thread_t* worker_thread );


void rtos_suspend_thread(beken_thread_t* thread);


void rtos_suspend_all_thread(void);


long rtos_resume_all_thread(void);


OSStatus rtos_thread_join( beken_thread_t* thread );


OSStatus rtos_thread_force_awake( beken_thread_t* thread );


bool rtos_is_current_thread( beken_thread_t* thread );


beken_thread_t* rtos_get_current_thread( void );


void rtos_thread_sleep(uint32_t seconds);


void rtos_thread_msleep(uint32_t milliseconds);


OSStatus rtos_delay_milliseconds( uint32_t num_ms );


OSStatus rtos_print_thread_status( char* buffer, int length );


OSStatus rtos_init_semaphore( beken_semaphore_t* semaphore, int maxCount );


OSStatus rtos_set_semaphore( beken_semaphore_t* semaphore );


OSStatus rtos_get_semaphore( beken_semaphore_t* semaphore, uint32_t timeout_ms );
int rtos_get_sema_count( beken_semaphore_t* semaphore );


OSStatus rtos_deinit_semaphore( beken_semaphore_t* semaphore );

OSStatus rtos_init_mutex( beken_mutex_t* mutex );


OSStatus rtos_lock_mutex( beken_mutex_t* mutex );


OSStatus rtos_unlock_mutex( beken_mutex_t* mutex );


OSStatus rtos_deinit_mutex( beken_mutex_t* mutex );



OSStatus rtos_init_queue( beken_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages );


OSStatus rtos_push_to_queue( beken_queue_t* queue, void* message, uint32_t timeout_ms );


OSStatus rtos_pop_from_queue( beken_queue_t* queue, void* message, uint32_t timeout_ms );


OSStatus rtos_deinit_queue( beken_queue_t* queue );


bool rtos_is_queue_empty( beken_queue_t* queue );


bool rtos_is_queue_full( beken_queue_t* queue );


OSStatus rtos_send_asynchronous_event( beken_worker_thread_t* worker_thread, event_handler_t function, void* arg );


OSStatus rtos_register_timed_event( beken_timed_event_t* event_object, beken_worker_thread_t* worker_thread, event_handler_t function, uint32_t time_ms, void* arg );


OSStatus rtos_deregister_timed_event( beken_timed_event_t* event_object );


uint32_t rtos_get_time(void);


OSStatus rtos_init_timer( beken_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg );
OSStatus rtos_init_oneshot_timer( beken_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg );


OSStatus rtos_start_timer( beken_timer_t* timer );


OSStatus rtos_stop_timer( beken_timer_t* timer );


OSStatus rtos_reload_timer( beken_timer_t* timer );


OSStatus rtos_deinit_timer( beken_timer_t* timer );


bool rtos_is_timer_init( beken_timer_t* timer );
bool rtos_is_timer_running( beken_timer_t* timer );


int rtos_init_event_fd(beken_event_t event_handle);


int rtos_deinit_event_fd(int fd);


#endif // __RTOS_PUB__
// EOF


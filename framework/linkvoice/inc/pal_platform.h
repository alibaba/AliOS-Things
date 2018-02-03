//
//  pal_platform.h
//  pal_sdk
//

#ifndef pal_platform_h
#define pal_platform_h

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif
    
/* Comment out this line if stdint.h is not defined */
#define HAVE_STDINT_H
    
#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
    typedef signed char        int8_t;
    typedef signed short       int16_t;
    typedef signed int         int32_t;
    typedef unsigned char      uint8_t;
    typedef unsigned short     uint16_t;
    typedef unsigned int       uint32_t;
    typedef unsigned long long uint64_t;
#endif
    
    extern int gmalloc_count;
    extern int gfree_count;
/*
 * Create a thread.
 * params:
 *     thread: the new thread handle;
 *     name: thread name;
 *     start_routine: user-defined function to be executed by the thread;
 *     arg: parameter to be passed to start_routine;
 *     stack_size: thread stack size in bytes;
 * return:
 *     0: success.
 *    -1: failed.
 */
int pal_thread_create(void **thread, const char *name, void *(*start_routine)(void *), void *arg, unsigned int stack_size);
    

/*
 * Terminate calling thread
 * param:
 *     thread: thread handle.
 */
void pal_thread_exit(void *thread);
    
/*
 * Join with a terminated thread.
 * return:
 *      0: success.
 *     -1: failed.
 */
int pal_thread_join(void *thread);
    
/*
 * Destroy the thread.
 *
 */
void pal_thread_destroy(void *thread);
    
/*
 * Initialize a mutex.
 * return:
 *      0: success.
 *     -1: failed.
 */
int pal_mutex_init(void **mutex);
    
/*
 * Destroy the mutex.
 * return:
 *      0: success.
 *     -1: failed.
 */
int pal_mutex_destroy(void *mutex);
    
/*
 * Lock the mutex.
 * return:
 *      0: success.
 *     -1: failed.
 */
int pal_mutex_lock(void *mutex);
    
/*
 * Unlock the mutex.
 * return:
 *      0: success.
 *     -1: failed.
 */
int pal_mutex_unlock(void *mutex);
   
/*
 * Initialize an semaphore.
 * return:
 *       the semaphore handle.
 */
void *pal_semaphore_init();
    
/*
 * Destroy the semaphore.
 */
void pal_semaphore_destroy(void *sem);
    
/*
 * Decrements (locks) the semaphore pointed to by sem.  If
 * the semaphore's value is greater than zero , then the decrement
 * proceeds, and the function returns, immediately. Otherwise, wait until timeout_ms in millisecond reached.
 * return:
 *      0: success.
 *     -1: failed.
 */
int pal_semaphore_wait(void *sem, int timeout_ms);
    
/*
 * Increments (unlocks) the semaphore pointed to by sem.
 */
void pal_semaphore_post(void *sem);
    
/*
 * Suspend execution for millisecond intervals.
 */
void pal_msleep(int millisecond);

/*
 * Allocate dynamic memory of `size' bytes.
 */
void* pal_malloc(int size);
    
/*
 * Free dynamic memory.
 */
void pal_free(void *mem);
  
/*
 * Get UTC time in millisecond.
 */
uint64_t pal_get_utc_time_ms();
    
#ifdef __cplusplus
}
#endif

#endif /* pal_platform_h */

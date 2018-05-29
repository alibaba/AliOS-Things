#ifndef _RDA_SYS_WRAPPER_H_
#define _RDA_SYS_WRAPPER_H_

#include "rda_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RDA_NO_WAIT         AOS_NO_WAIT
#define RDA_WAIT_FOREVER    AOS_WAIT_FOREVER    

/* Alarm */
/**
 * @brief     : get current time in units of micro second
 * @param[in] :
 * @param[out]:
 * @return    : return time value with uint32 type
 */
extern r_u64 rda_get_cur_time_ms(r_void);

/**
 * @brief     : create an alarm with given function, return timer handle
 * @param[in] : func(callback)/data(pass to func)/timeout_ms(ms of the normal timer triger)
 * @param[out]:
 * @return    : return timer handle, a pointer to the timer structure, non-zero is valid
 */
//extern void * rda_alarm_create_v2(void *func, unsigned int data, unsigned int mode);
extern r_void * rda_alarm_create(r_void *func, r_u32 data);

/**
 * @brief     : delete an alarm with given handle, then reset the handle
 * @param[in] : *handle(pointer to the timer structure)
 * @param[out]: *handle(address of the handle variable)
 * @return    :
 */
extern r_s32 rda_alarm_delete(r_void *handle);

/**
 * @brief     : start an alarm, raise a function call after given timeout delay
 * @param[in] : handle(pointer to the timer structure)/timeout(micro second)
 * @param[out]:
 * @return    :
 */

extern r_s32 rda_alarm_delete_fw(r_void **rdaalarm);
extern r_s32 rda_alarm_start(r_void *handle, r_u32 timeout_ms);

/**
 * @brief     : stop an alarm, will not raise a function call any more
 * @param[in] : handle(pointer to the timer structure)
 * @param[out]:
 * @return    :
 */
extern r_s32 rda_alarm_stop(r_void *handle);


/* Semaphore */
/**
 * @brief     : create a semaphore
 * @param[in] : name and initial valve of semaphore
 * @param[out]:
 * @return    : return ERR or NO_ERR
 */
extern r_void* rda_sem_create(r_u32 count);

/**
 * @brief     : wait a semaphore
 * @param[in] : name of semaphore
 * @param[out]:
 * @return    : return ERR or NO_ERR
 */
extern r_s32 rda_sem_wait(r_void *sem, r_u32 millisec);

/**
 * @brief     : release a semaphore
 * @param[in] : name of semaphore
 * @param[out]:
 * @return    : return ERR or NO_ERR
 */
extern r_s32 rda_sem_release(r_void *sem);

/**
 * @brief     : delete a semaphore
 * @param[in] : name of semaphore
 * @param[out]:
 * @return    : return ERR or NO_ERR
 */
extern r_s32 rda_sem_delete(r_void *sem);


/* Queue */
/**
 * @brief     : create a queue
 * @param[in] : queue size/count
 * @param[out]: 
 * @return    : return queue handler
 */
extern r_void* rda_queue_create(r_u32 msg_size, r_u32 max_msg);


/**
 * @brief     : receive a msg from queue
 * @param[in] : handler of queue/msg/wait time
 * @param[out]:
 * @return    : return ERR or NO_ERR
 */
extern r_s32 rda_queue_recv(r_void *handler, r_u32 msg, r_u32 millisec);

/**
 * @brief     : send a msg to queue
 * @param[in] : handler of queue/msg/wait time
 * @param[out]:
 * @return    : return ERR or NO_ERR
 */
extern r_s32 rda_queue_send(r_void *handler, r_u32 msg, r_u32 millisec);

/**
 * @brief     : delete a queue
 * @param[in] : handler of queue
 * @param[out]:
 * @return    : return ERR or NO_ERR
 */
extern r_s32 rda_queue_free(r_void *handler);

/* Mutex */
/**
 * @brief     : create a mutex
 * @param[in] :
 * @param[out]:
 * @return    : return ERR or NO_ERR
 */
extern r_void* rda_mutex_create(r_void);

/**
 * @brief     : wait a mutex
 * @param[in] : id of mutex and wait time
 * @param[out]:
 * @return    : return ERR or NO_ERR
 */
extern r_s32 rda_mutex_wait(r_void *rdamutex, r_u32 millisec);

/**
 * @brief     : release a mutex
 * @param[in] : id of mutex
 * @param[out]:
 * @return    : return ERR or NO_ERR
 */
extern r_s32 rda_mutex_realease(r_void *rdamutex);

/**
 * @brief     : delete a mutex
 * @param[in] : id of mutex
 * @param[out]:
 * @return    : return ERR or NO_ERR
 */
extern r_s32 rda_mutex_delete(r_void *rdamutex);

/* Thread */
/**
 * @brief     : creat a thread
 * @param[in] : thread name/thread function/thread fuction argument/stacksize/thread priority
 * @param[out]:
 * @return    : return thread id
 */
r_void* rda_thread_new(const r_u8 *pcName, r_void (*thread)(r_void *arg), r_void *arg, r_u32 stacksize, r_s32 priority);

/**
 * @brief     : delete a thread
 * @param[in] : thread id
 * @param[out]:
 * @return    : return ERR or NO_ERR
 */
extern r_s32 rda_thread_delete(r_void* id);

/**
 * @brief     : get current thread id
 * @param[in] :
 * @param[out]:
 * @return    : return thread id
 */
extern r_void* rda_thread_get_id(r_void);

/**
 * @brief     : set thread to sleep 
 * @param[in] :
 * @param[out]:
 * @return    : return thread id
 */
extern r_void rda_msleep(r_u32 ms);

extern r_void rda_critical_sec_start(r_void);
extern r_void rda_critical_sec_end(r_void);
extern r_void * rda_create_interrupt(r_u32 vec, r_u32 pri, r_void *isr);
extern r_void rda_delete_interrupt(r_u32 vec);
extern r_void rda_enable_interrupt(r_u32 vec);
extern r_void rda_disable_interrupt(r_u32 vec);

#ifdef __cplusplus
}
#endif

#endif /* _RDA_SYS_WRAPPER_H_ */


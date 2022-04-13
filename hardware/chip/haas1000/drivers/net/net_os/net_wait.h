#ifndef __NET_WAIT_H__
#define __NET_WAIT_H__
#include "net_mutex.h"
#include "jiffies.h"
#include "net_debug.h"
#include "cmsis_os.h"

struct __wait_queue_head {
    osSemaphoreId sem_id;
#if defined(CMSIS_OS_RTX) && (osCMSIS < 0x20000U)
	uint32_t os_semaphore_data[2];
#endif
    osSemaphoreDef_t os_semaphore_name;
};
typedef struct __wait_queue_head wait_queue_head_t;

extern void init_waitqueue_head(wait_queue_head_t *wq);
extern void uinit_waitqueue(wait_queue_head_t *wq);
extern uint32_t wait_sem_timeout(wait_queue_head_t *wq,
                             uint32_t timeout);

//should be careful the macro condition.
//there be valure set operation, you call condition will change the vaule once.
/**
 * wait_event_timeout - sleep until a condition gets true or a timeout elapses
 * @wq: the waitqueue to wait on
 * @condition: a C expression for the event to wait for
 * @timeout: timeout, in jiffies
 *
 * The process is put to sleep (TASK_UNINTERRUPTIBLE) until the
 * @condition evaluates to true. The @condition is checked each time
 * the waitqueue @wq is woken up.
 *
 * wake_up() has to be called after changing any variable that could
 * change the result of the wait condition.
 *
 * Returns:
 * 0 if the @condition evaluated to %false after the @timeout elapsed,
 * 1 if the @condition evaluated to %true after the @timeout elapsed,
 * or the remaining jiffies (at least 1) if the @condition evaluated
 * to %true before the @timeout elapsed.
 * note: *******return only 0 or 1. no remaining jiffies*******
 */
#define wait_event_timeout(wq, condition, timeout) (\
		{	\
			uint32_t ret = 0;\
			uint32_t timer_need = 0, timer_start = 0 , timer_now = 0;\
			uint32_t timer_cost = 0;\
			timer_need = jiffies_to_msecs(timeout);\
			if (condition) \
				ret = 1; \
			else if (!timeout) { \
				ret = 0; \
			} else { \
				do{ \
					timer_start = jiffies;\
					if(condition) {\
						ret = 1;\
						break; \
					}\
					wait_sem_timeout(&wq, timer_need); \
					if(condition) {\
						ret = 1;\
						break;\
					}\
					timer_now = jiffies;\
					if(timer_now >= timer_start)\
						timer_cost += timer_now - timer_start;\
					else\
						timer_cost += timer_now + (UINT_MAX - timer_start);\
					if(timer_cost < timeout)\
						timer_need = jiffies_to_msecs(timeout - timer_cost);\
					else{\
						break; \
					}\
				}while(1);\
			} \
			ret;\
		} \
	)

#define wait_event_interruptible(wq, condition) \
     wait_event_timeout(wq, condition, osWaitForever)

#define wait_event_interruptible_timeout(wq, condition, timeout)\
     wait_event_timeout(wq, condition, timeout)

static inline void wake_up(wait_queue_head_t *wq)
{
	//osStatus ret;
	//printf("wake up%p\n", wq);
    //ret = 
    osSemaphoreRelease(wq->sem_id);
    //if (ret) {
        //printf("wake_up fail.ret,%d. wq:%p, sem_id:%p\n", ret, wq, wq->sem_id);
    //}
}

#endif


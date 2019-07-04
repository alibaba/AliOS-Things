#include <stdio.h>
#include <string.h>
#include <k_api.h>
#include <aos/kernel.h>

#include <errno.h>
#include "rda5981.h"
#include "rda_def.h"
#include "core_cm4.h"

#define NO_ERR  0
#define ERR     -1

//#define RDA_SYS_DEBUG
#ifdef RDA_SYS_DEBUG
#define RDA_SYS_PRINT(fmt, ...) do {\
            printf(fmt, ##__VA_ARGS__);\
    } while (0)
#else
#define RDA_SYS_PRINT(fmt, ...)
#endif

/**
 * @brief     : get current time in units of micro second
 * @param[in] :
 * @param[out]:
 * @return    : return time value with uint32 type
 */
r_u64 rda_get_cur_time_ms(r_void)
{
    return aos_now_ms();
}


r_void * rda_alarm_create(r_void *func, r_u32 data)
{
    aos_timer_t *timer = (aos_timer_t *)r_malloc(sizeof(aos_timer_t));
    if (timer == NULL) {
        return NULL;
    }

    r_s32 ret = aos_timer_new_ext(timer, func, (void *)data, 1000, 0, 0);
    if (!ret) {
        return timer;
    } else {
        return NULL;
    }
}

/**
 * @brief     : delete an alarm with given handle, then reset the handle
 * @param[in] : *handle(pointer to the timer structure)
 * @param[out]: **handle(address of the handle variable)
 * @return    :
 */
r_s32 rda_alarm_delete(r_void *handle)
{
    if (NULL != handle) {
        aos_timer_stop((aos_timer_t *)handle);
        aos_timer_free((aos_timer_t *)handle);
        r_free(handle);
        handle = NULL;
        return  NO_ERR;
    } else {
        return ERR;
    }
}

/* timer for fw */
r_s32 rda_alarm_delete_fw(r_void **rdaalarm)
{
    r_s32 res = rda_alarm_delete((r_void *)*rdaalarm);
    *rdaalarm = NULL;
    return res;
}

/**
 * @brief     : start an alarm, raise a function call after given timeout delay
 * @param[in] : handle(pointer to the timer structure)/timeout(micro second)
 * @param[out]:
 * @return    :
 */
r_s32 rda_alarm_start(r_void *handle, r_u32 timeout_ms)
{
    r_s32 ret;
    if(NULL != handle) {
        //ret = aos_timer_change((aos_timer_t *)handle, timeout_ms);
        ret = krhino_timer_change(((aos_timer_t *)handle)->hdl, krhino_ms_to_ticks(timeout_ms), 0);
        if(ret != 0)
            return ERR;
        return aos_timer_start((aos_timer_t *)handle);
    } else {
        return ERR;
    }
}

/**
 * @brief     : stop an alarm, will not raise a function call any more
 * @param[in] : handle(pointer to the timer structure)
 * @param[out]:
 * @return    :
 */
r_s32 rda_alarm_stop(r_void *handle)
{
    if(NULL != handle) {
        return aos_timer_stop((aos_timer_t *)handle);
    } else {
        return ERR;
    }
}



/* Semaphore */
r_void* rda_sem_create(r_u32 count)
{
    aos_sem_t *sem = (aos_sem_t *)r_malloc(sizeof(aos_sem_t));

    if (NULL == sem) {
        return NULL;
    }

    if (0 != aos_sem_new(sem, count)) {
        r_free(sem);
        return NULL;
    }
    return sem;
}

r_s32 rda_sem_wait(r_void* sem, r_u32 timeout_ms)
{
    return aos_sem_wait((aos_sem_t *)sem, timeout_ms);
}

r_s32 rda_sem_release(r_void *sem)
{
    if (sem != NULL) {
        aos_sem_signal((aos_sem_t *)sem);
        return NO_ERR;
    } else {
        return ERR;
    }
}

r_s32 rda_sem_delete(r_void *sem)
{
    if ((sem != NULL)) {
        aos_sem_free((aos_sem_t *)sem);
        r_free(sem);
        sem = NULL;
        return NO_ERR;
    } else {
        return ERR;
    }
}

/* Queue */
typedef struct{
    r_void *queue;
    r_void *buf;
    r_u32 msg_size;
    r_u32 max_msg;
}rda_queue_handle;

/* Queue */
r_void* rda_queue_create(r_u32 max_msg, r_u32 msg_size)
{
    rda_queue_handle *r_handler = NULL;
    aos_queue_t *queue = NULL;
    r_void *buf = NULL;
    r_s32 res;

    r_handler = (rda_queue_handle *)r_malloc(sizeof(rda_queue_handle));
    if (r_handler == NULL) {
        return NULL;
    }

    queue = (aos_queue_t*)r_malloc(sizeof(aos_queue_t));
    if (queue == NULL) {
        r_free(r_handler);
        return NULL;
    }

    buf = r_malloc(msg_size * max_msg);
    if(buf == NULL){
        r_free(r_handler);
        r_free(queue);
        return NULL;
    }

    res = aos_queue_new(queue, buf, msg_size * max_msg, msg_size);

    if (res == 0) {
        r_handler->queue = (r_void*)queue;
        r_handler->buf = buf;
        r_handler->msg_size = msg_size;
        r_handler->max_msg = max_msg;
        return (r_void*)r_handler;
    } else {
        return NULL;
    }

}

r_s32 rda_queue_send(r_void *handler, r_u32 msg, r_u32 millisec)
{
    rda_queue_handle *r_handler = (rda_queue_handle*)handler;
    if (aos_queue_send((aos_queue_t *)(r_handler->queue), (r_void*)msg, r_handler->msg_size) != 0)
        return ERR;
    else
        return NO_ERR;
}

r_s32 rda_queue_recv(r_void *handler, r_u32 msg, r_u32 millisec)
{
    rda_queue_handle *r_handler = (rda_queue_handle*)handler;
    r_s32 size;
    if(aos_queue_recv((aos_queue_t *)(r_handler->queue), millisec, (r_void*)msg, &size) != 0)
        return ERR;
    else
        return NO_ERR;

}

r_s32 rda_queue_free(r_void *handler)
{
    rda_queue_handle *r_handler = (rda_queue_handle*)handler;
    aos_queue_free(r_handler->queue);
    r_free((r_void*)(r_handler->queue));
    r_free(r_handler->buf);
    r_free((r_void*)r_handler);

    return NO_ERR;
}

/* Mutex */
r_void* rda_mutex_create(r_void)
{
    aos_mutex_t *rdamutex = (aos_mutex_t *)r_malloc(sizeof(aos_mutex_t));

    if (NULL == rdamutex) {
        return NULL;
    }

    if (0 != aos_mutex_new(rdamutex)) {
        r_free(rdamutex);
        return NULL;
    }

    return rdamutex;
}

r_s32 rda_mutex_wait(r_void *rdamutex, r_u32 millisec)
{
    return aos_mutex_lock((aos_mutex_t *)rdamutex, millisec);
}

r_s32 rda_mutex_realease(r_void *rdamutex)
{
    return aos_mutex_unlock((aos_mutex_t *)rdamutex);
}

r_s32 rda_mutex_delete(r_void *rdamutex)
{
    if (rdamutex == NULL) {
        return ERR;
    }
    aos_mutex_free((aos_mutex_t *)rdamutex);
    r_free(rdamutex);
    return NO_ERR;
}

/* Thread */
r_void* rda_thread_new(const r_u8 *pcName,
                            r_void (*thread)(r_void *arg),
                            r_void *arg, r_u32 stacksize, r_s32 priority)
{
    ktask_t * rdaktask;
    krhino_task_dyn_create(&rdaktask, pcName, arg, priority, 0,
                                      stacksize / sizeof(cpu_stack_t), thread, 1u);

    return rdaktask;
}

r_s32 rda_thread_delete(r_void* id)
{
    kstat_t    ret;
    ret = krhino_task_dyn_del((ktask_t *)id);
    if (ret != RHINO_SUCCESS)
        return ERR;
    return NO_ERR;
}

r_void* rda_thread_get_id(r_void)
{
    ktask_t *task;
    task = krhino_cur_task_get();
    return (r_void*)task;
}

r_s32 rda_thread_yeild(r_void)
{
    kstat_t ret;
    ret = krhino_task_yield();
    if(ret == RHINO_SUCCESS)
        return NO_ERR;
    else
        return ERR;
}

r_void rda_msleep(r_u32 ms)
{
    aos_msleep(ms);
}

#define CONFIG_DISABLE_ALL_INT
#define CRI_SEC_START_PRI_LEVEL 0xF8
#define CRI_SEC_END_PRI_LEVEL   0x00
static r_u32 g_critical_sec_counter = 0U;
#if defined(CONFIG_DISABLE_ALL_INT)
cpu_cpsr_t cpu_intrpt_save(void);
void   cpu_intrpt_restore(cpu_cpsr_t cpsr);
static cpu_cpsr_t g_critical_ctxt_saved;
#endif /* CONFIG_DISABLE_ALL_INT */


r_void rda_critical_sec_start(r_void)
{
    if(__get_IPSR() == 0U) {
        if(0U == g_critical_sec_counter) {
#if defined(CONFIG_DISABLE_ALL_INT)
            g_critical_ctxt_saved = cpu_intrpt_save();
#else  /* CONFIG_DISABLE_ALL_INT */
            __set_BASEPRI(CRI_SEC_START_PRI_LEVEL);
#endif /* CONFIG_DISABLE_ALL_INT */
        }
        g_critical_sec_counter++;
    }
}

r_void rda_critical_sec_end(r_void)
{
    if(__get_IPSR() == 0U) {
        g_critical_sec_counter--;
        if(0U == g_critical_sec_counter) {
#if defined(CONFIG_DISABLE_ALL_INT)
            cpu_intrpt_restore(g_critical_ctxt_saved);
#else  /* CONFIG_DISABLE_ALL_INT */
            __set_BASEPRI(CRI_SEC_END_PRI_LEVEL);
#endif /* CONFIG_DISABLE_ALL_INT */
        }
    }
}

r_void * rda_create_interrupt(r_u32 vec, r_u32 pri, r_void *isr)
{
    NVIC_SetPriority((IRQn_Type)vec, (uint32_t) pri);
    NVIC_SetVector((IRQn_Type)vec, (uint32_t) isr);

    return NULL;
}

r_void rda_delete_interrupt(r_u32 vec)
{
    NVIC_SetVector((IRQn_Type)vec, 0);
}

r_void rda_enable_interrupt(r_u32 vec)
{
    NVIC_EnableIRQ((IRQn_Type)vec);
}

r_void rda_disable_interrupt(r_u32 vec)
{
    NVIC_DisableIRQ((IRQn_Type)vec);
}



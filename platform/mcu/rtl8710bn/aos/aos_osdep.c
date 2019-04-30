/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

//#include <autoconf.h>
#include <osdep_service.h>
#include <stdio.h>
#include <freertos/wrapper.h>
#include "aos/kernel.h"
#include <aos/errno.h>
#include <freertos_pmu.h>
/********************* os depended utilities ********************/

#ifndef USE_MUTEX_FOR_SPINLOCK
#define USE_MUTEX_FOR_SPINLOCK 1
#endif

//----- ------------------------------------------------------------------
// Misc Function
//----- ------------------------------------------------------------------
#if 0
void save_and_cli()
{
    RHINO_CRITICAL_ENTER();
}

void restore_flags()
{
    RHINO_CRITICAL_EXIT();
}
#endif
void cli()
{
    //RHINO_CPU_INTRPT_DISABLE();
}

/* Not needed on 64bit architectures */
static unsigned int __div64_32(u64 *n, unsigned int base)
{
	u64 rem = *n;
	u64 b = base;
	u64 res, d = 1;
	unsigned int high = rem >> 32;

	/* Reduce the thing a bit first */
	res = 0;
	if (high >= base) {
		high /= base;
		res = (u64) high << 32;
		rem -= (u64) (high * base) << 32;
	}

	while ((u64)b > 0 && b < rem) {
		b = b+b;
		d = d+d;
	}

	do {
		if (rem >= b) {
			rem -= b;
			res += d;
		}
		b >>= 1;
		d >>= 1;
	} while (d);

	*n = res;
	return rem;
}

/********************* os depended service ********************/

u8* _aos_malloc(u32 sz)
{
    //DBG_8195A("  realsil malloc %d\r\n", sz);
    return aos_malloc(sz);
}

u8* _aos_zmalloc(u32 sz)
{
    u8 *pbuf = _aos_malloc(sz);

    if (pbuf != NULL)
        memset(pbuf, 0, sz);

    return pbuf;	
}

void _aos_mfree(u8 *pbuf, u32 sz)
{
    aos_free(pbuf);
}

static void _aos_memcpy(void* dst, void* src, u32 sz)
{
    _memcpy(dst, src, sz);
}

static int _aos_memcmp(void *dst, void *src, u32 sz)
{
//under Linux/GNU/GLibc, the return value of memcmp for two same mem. chunk is 0
    if (!(_memcmp(dst, src, sz)))
        return 1;

    return 0;
}

static void _aos_memset(void *pbuf, int c, u32 sz)
{
    _memset(pbuf, c, sz);
}

static void _aos_init_sema(_sema *sema, int init_val)
{
    aos_sem_new(sema, init_val);	//Set max count 0xffffffff
}

static void _aos_free_sema(_sema *sema)
{
    if(sema != NULL)
        aos_sem_free(sema);
}

static void _aos_up_sema(_sema *sema)
{
    aos_sem_signal(sema);
}

static void _aos_up_sema_from_isr(_sema *sema)
{
    aos_sem_signal(sema);
}

static u32 _aos_down_sema(_sema *sema, u32 timeout)
{
    //DBG_8195A("_aos_down_sema\r\n");
    if(timeout == RTW_MAX_DELAY) {
        timeout = AOS_WAIT_FOREVER;
    } else {
        timeout = rtw_ms_to_systime(timeout);
    }
    //DBG_8195A("_aos_down_sema, timeout %d\r\n", timeout);
	if(aos_sem_wait(sema, timeout) != 0) {
                printf("_aos_down_sema 0x%x, timeout %d", sema, timeout);
		return FALSE;
	}

	return TRUE;
    //return aos_sem_wait(sema, timeout);
}

static void _aos_mutex_init(_mutex *pmutex)
{
    aos_mutex_new(pmutex);
    return;
}

static void _aos_mutex_free(_mutex *pmutex)
{
    if(pmutex != NULL)
        aos_mutex_free(pmutex);

    //pmutex = NULL;
}

static void _aos_mutex_get(_lock *plock)
{
    while(aos_mutex_lock(plock, 60 * 1000) != 0)
        DBG_8195A("[%s] %s(%p) failed, retry\n", aos_task_name(), __FUNCTION__, plock);
}

static int _aos_mutex_get_timeout(_lock *plock, u32 timeout_ms)
{
    if(aos_mutex_lock(plock, timeout_ms ) != 0){
        DBG_8195A("[%s] %s(%p) failed, retry\n", aos_task_name(), __FUNCTION__, plock);
        return -1;
    }
    return 0;
}

static void _aos_mutex_put(_lock *plock)
{
    aos_mutex_unlock(plock);
}
#if 1
static void _aos_enter_critical(_lock *plock, _irqL *pirqL)
{
	*pirqL = cpu_intrpt_save( );
}

static void _aos_exit_critical(_lock *plock, _irqL *pirqL)
{
	cpu_intrpt_restore(*pirqL);
}

static void _aos_enter_critical_from_isr(_lock *plock, _irqL *pirqL)
{
	*pirqL = cpu_intrpt_save( );
}

static void _aos_exit_critical_from_isr(_lock *plock, _irqL *pirqL)
{
	cpu_intrpt_restore(*pirqL);
}
#endif
static int _aos_enter_critical_mutex(_mutex *pmutex, _irqL *pirqL)
{
    int ret = 0;

    while(aos_mutex_lock(pmutex, 60 * 1000) != 0)
        DBG_8195A("[%s] %s(%p) failed, retry\n", aos_task_name(), __FUNCTION__, pmutex);

    return ret;
}

static void _aos_exit_critical_mutex(_mutex *pmutex, _irqL *pirqL)
{
    aos_mutex_unlock(pmutex);
}

static void _aos_spinlock_init(_lock *plock)
{
#if USE_MUTEX_FOR_SPINLOCK
    aos_mutex_new(plock);
#endif
}

static void _aos_spinlock_free(_lock *plock)
{
#if USE_MUTEX_FOR_SPINLOCK
    if(plock != NULL)
        aos_mutex_free(plock);

    //*plock = NULL;
#endif
}

static void _aos_spinlock(_lock *plock)
{
#if USE_MUTEX_FOR_SPINLOCK
    while(aos_mutex_lock(plock, 60 * 1000) != 0)
        DBG_8195A("[%s] %s(%p) failed, retry\n", aos_task_name(), __FUNCTION__, plock);
#endif
}

static void _aos_spinunlock(_lock *plock)
{
#if USE_MUTEX_FOR_SPINLOCK
    aos_mutex_unlock(plock);
#endif
}
#if 0
static void _aos_spinlock_irqsave(_lock *plock, _irqL *irqL)
{
    RHINO_CRITICAL_ENTER();
#if USE_MUTEX_FOR_SPINLOCK
    while(aos_mutex_lock(plock, 60 * 1000) != 0)
        DBG_8195A("[%s] %s(%p) failed, retry\n", aos_task_name(), __FUNCTION__, plock);
#endif
}

static void _aos_spinunlock_irqsave(_lock *plock, _irqL *irqL)
{
#if USE_MUTEX_FOR_SPINLOCK
    aos_mutex_unlock(plock);
#endif
    RHINO_CRITICAL_EXIT();
}
}
#endif
static int _aos_init_xqueue( _xqueue* queue, const char* name, u32 message_size, u32 number_of_messages )
{
    int ret = 0;
    ret = krhino_buf_queue_dyn_create(&queue, name, message_size, number_of_messages);
    return ret;
}

static int _aos_push_to_xqueue( _xqueue* queue, void* message, u32 timeout_ms )
{
    krhino_buf_queue_send(queue, message, sizeof(void*));

    return 0;
}

static int _aos_pop_from_xqueue( _xqueue* queue, void* message, u32 timeout_ms )
{
    int size = 0;
    krhino_buf_queue_recv(queue, timeout_ms, message, &size);

    return 0;
}

static int _aos_deinit_xqueue( _xqueue* queue )
{
    krhino_buf_queue_dyn_del(queue);
    return 0;
}

static u32 _aos_get_current_time(void)
{
    return aos_now_ms();	//The count of ticks since vTaskStartScheduler was called.
}

static u32 _aos_systime_to_ms(u32 systime)
{
    return  krhino_ticks_to_ms(systime);
}

static u32 _aos_systime_to_sec(u32 systime)
{
    return krhino_ticks_to_ms(systime)/1000;
}

static u32 _aos_ms_to_systime(u32 ms)
{
    return krhino_ms_to_ticks(ms);
}

static u32 _aos_sec_to_systime(u32 sec)
{
    return krhino_ms_to_ticks(sec) * 1000;
}

static void _aos_msleep_os(int ms)
{
#if defined(CONFIG_PLATFORM_8195A)
	aos_msleep(ms);
#elif defined(CONFIG_PLATFORM_8711B)
	if (pmu_yield_os_check()) {
		aos_msleep(ms);
	} else {
		DelayMs(ms);
	}
#endif
}

static void _aos_usleep_os(int us)
{
#if defined(STM32F2XX) || defined(STM32F4XX) || defined(STM32F10X_XL)
	// FreeRTOS does not provide us level delay. Use busy wait
	WLAN_BSP_UsLoop(us);
#elif defined(CONFIG_PLATFORM_8195A)
	//DBG_ERR("%s: Please Implement micro-second delay\n", __FUNCTION__);
#elif defined(CONFIG_PLATFORM_8711B)
	DelayUs(us);
#else
	#error "Please implement hardware dependent micro second level sleep here"
#endif
}

static void _aos_mdelay_os(int ms)
{
#if defined(CONFIG_PLATFORM_8195A)
	aos_msleep(ms);
#elif defined(CONFIG_PLATFORM_8711B)
	if (pmu_yield_os_check()) {
		aos_msleep(ms);
	} else {
		DelayMs(ms);
	}
#endif
}

static void _aos_udelay_os(int us)
{
#if defined(STM32F2XX)	|| defined(STM32F4XX) || defined(STM32F10X_XL)
	// FreeRTOS does not provide us level delay. Use busy wait
	WLAN_BSP_UsLoop(us);
#elif defined(CONFIG_PLATFORM_8195A)
	HalDelayUs(us);
#elif defined(CONFIG_PLATFORM_8711B)
	DelayUs(us);
#else
	#error "Please implement hardware dependent micro second level sleep here"
#endif
}

static void _aos_yield_os(void)
{
#if defined(CONFIG_PLATFORM_8195A)
	taskYIELD();
#elif defined(CONFIG_PLATFORM_8711B)
	krhino_task_yield();
#endif
}

static void _aos_ATOMIC_SET(ATOMIC_T *v, int i)
{
	atomic_set(v,i);
}

static int _aos_ATOMIC_READ(ATOMIC_T *v)
{
	return atomic_read(v);
}

static void _aos_ATOMIC_ADD(ATOMIC_T *v, int i)
{
        CPSR_ALLOC();
	save_and_cli();
	v->counter += i;
	restore_flags();
}

static void _aos_ATOMIC_SUB(ATOMIC_T *v, int i)
{ 
	CPSR_ALLOC();
	save_and_cli();
	v->counter -= i;
	restore_flags();
}

static void _aos_ATOMIC_INC(ATOMIC_T *v)
{
	_aos_ATOMIC_ADD(v, 1);
}

static void _aos_ATOMIC_DEC(ATOMIC_T *v)
{
	_aos_ATOMIC_SUB(v, 1);
}

static int _aos_ATOMIC_ADD_RETURN(ATOMIC_T *v, int i)
{
	int temp;
	CPSR_ALLOC();
	save_and_cli();
	temp = v->counter;
	temp += i;
	v->counter = temp;
	restore_flags();

	return temp;
}

static int _aos_ATOMIC_SUB_RETURN(ATOMIC_T *v, int i)
{
	int temp;
	CPSR_ALLOC();
	save_and_cli();
	temp = v->counter;
	temp -= i;
	v->counter = temp;
	restore_flags();

	return temp;
}

static int _aos_ATOMIC_INC_RETURN(ATOMIC_T *v)
{
	return _aos_ATOMIC_ADD_RETURN(v, 1);
}

static int _aos_ATOMIC_DEC_RETURN(ATOMIC_T *v)
{
	return _aos_ATOMIC_SUB_RETURN(v, 1);
}

static u64 _aos_modular64(u64 n, u64 base)
{
	unsigned int __base = (base);
	unsigned int __rem;

	if (((n) >> 32) == 0) {
		__rem = (unsigned int)(n) % __base;
		(n) = (unsigned int)(n) / __base;
	}
	else
		__rem = __div64_32(&(n), __base);
	
	return __rem;
}

/* Refer to ecos bsd tcpip codes */
static int _aos_arc4random(void)
{
	u32 res = aos_now_ms();
	static unsigned long seed = 0xDEADB00B;

#if CONFIG_PLATFORM_8711B
	if(random_seed){
		seed = random_seed;
		random_seed = 0;
	}
#endif
	
	seed = ((seed & 0x007F00FF) << 7) ^
	    ((seed & 0x0F80FF00) >> 8) ^ // be sure to stir those low bits
	    (res << 13) ^ (res >> 9);    // using the clock too!
	return (int)seed;
}

static int _aos_get_random_bytes(void *buf, size_t len)
{
#if 1 //becuase of 4-byte align, we use the follow code style.
	unsigned int ranbuf;
	unsigned int *lp;
	int i, count;
	count = len / sizeof(unsigned int);
	lp = (unsigned int *) buf;

	for(i = 0; i < count; i ++) {
		lp[i] = _aos_arc4random();  
		len -= sizeof(unsigned int);
	}

	if(len > 0) {
		ranbuf = _aos_arc4random();
		_aos_memcpy(&lp[i], &ranbuf, len);
	}
	return 0;
#else
	unsigned long ranbuf, *lp;
	lp = (unsigned long *)buf;
	while (len > 0) {
		ranbuf = _freertos_arc4random();
		*lp++ = ranbuf; //this op need the pointer is 4Byte-align!
		len -= sizeof(ranbuf);
	}
	return 0;
#endif
}

static u32 _aos_GetFreeHeapSize(void)
{
	//return (u32)xPortGetFreeHeapSize();
	return g_kmm_head->free_size;
}

static int _aos_create_task(struct task_struct *ptask, const char *name,
	u32  stack_size, u32 priority, thread_func_t func, void *thctx)
{
	thread_func_t task_func = NULL;
	void *task_ctx = NULL;
	int ret = 0;

        //DBG_8195A("_aos_create_task  start\r\n");
	ptask->task_name = name;
	ptask->blocked = 0;
	ptask->callback_running = 0;
	
	_aos_init_sema(&ptask->wakeup_sema, 0);
	_aos_init_sema(&ptask->terminate_sema, 0);
	//rtw_init_queue(&wq->work_queue);
       // DBG_8195A("_aos_create_task  init sema done\r\n");

	if(func){
		task_func = func;
		task_ctx = thctx;
	}
	//else{
	//	task_func = freertos_wq_thread_handler;
	//	task_ctx = wq;
	//}

	priority = 11-priority;

	ret = aos_task_new_ext(&ptask->task,
				name,
				func,
				task_ctx,
				stack_size*sizeof(cpu_stack_t),
				priority);


	if(ret != 0){
		DBG_8195A("Create Task \"%s\" Failed! ret=%d\n", ptask->task_name, ret);
	}
	
	//DBG_8195A("Create Task \"%s\"\n", ptask->task_name);
	return 1;
}
	
static void _aos_delete_task(struct task_struct *ptask)
{
	if (!ptask->task.hdl){
		DBG_8195A("_aos_delete_task(): ptask is NULL!\n");
		return;
	}

	ptask->blocked = 1;

	_aos_up_sema(&ptask->wakeup_sema);
	_aos_down_sema(&ptask->terminate_sema, RTW_MAX_DELAY);
	
	//rtw_deinit_queue(&wq->work_queue);
	_aos_free_sema(&ptask->wakeup_sema);
	_aos_free_sema(&ptask->terminate_sema);

	//ptask->task = 0;

	//DBG_8195A("Delete Task \"%s\"\n", ptask->task_name);
}

void _aos_wakeup_task(struct task_struct *ptask)
{
    _aos_up_sema(&ptask->wakeup_sema);
}

static void _aos_thread_enter(char *name)
{
    //DBG_8195A("\n\rRTKTHREAD %s\n", name);
}

static void _aos_thread_exit(void)
{
    //DBG_8195A("\n\rRTKTHREAD exit %s\n", __FUNCTION__); 
    aos_task_exit(0);
}

_timerHandle _aos_timerCreate( const signed char *pcTimerName, 
							  osdepTickType xTimerPeriodInTicks, 
							  u32 uxAutoReload, 
							  void * pvTimerID, 
							  TIMER_FUN pxCallbackFunction )
{
    _timerHandle t_handler= _aos_zmalloc(sizeof(struct timerHandle));

    if(xTimerPeriodInTicks>= 0xffffffff)
        xTimerPeriodInTicks = 0x7ffffffe;

    aos_timer_new_ext(&t_handler->timer, pxCallbackFunction,(void *)t_handler, krhino_ticks_to_ms(xTimerPeriodInTicks), 0, 0);

    return t_handler;
}

u32 _aos_timerDelete( _timerHandle xTimer, 
							   osdepTickType xBlockTime )
{
    aos_timer_stop(&xTimer->timer);
    aos_timer_free(&xTimer->timer);
    _aos_mfree(xTimer, sizeof(xTimer));
    return 0;
}

u32 _aos_timerIsTimerActive( _timerHandle xTimer )
{
	return 1;	
}

u32  _aos_timerStop( _timerHandle xTimer, 
							   osdepTickType xBlockTime )
{
	if(!aos_timer_stop(&xTimer->timer))	
		return 1;
	return 0;
}

#define MS2TICK(ms) krhino_ms_to_ticks(ms)

int _aos_timer_change_no_repeat(aos_timer_t *timer, int ms)
{
    int ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    ret = krhino_timer_stop(timer->hdl);
    if (ret != RHINO_SUCCESS) {
        return ret;
    }

    ret = krhino_timer_change(timer->hdl, MS2TICK(ms), 0);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

u32  _aos_timerChangePeriod( _timerHandle xTimer, 
							   osdepTickType xNewPeriod, 
							   osdepTickType xBlockTime )
{
	if(xNewPeriod == 0)
		xNewPeriod += 1;
	//if(!aos_timer_change(&xTimer->timer, xNewPeriod))
	if(!_aos_timer_change_no_repeat(&xTimer->timer, xNewPeriod))
		return 1;
	return 0;
}
void *_aos_timerGetID( _timerHandle xTimer ){

	return 0;
}

u32  _aos_timerStart( _timerHandle xTimer, 
							   osdepTickType xBlockTime )
{
	return !aos_timer_start(&xTimer->timer);	
}

u32  _aos_timerStartFromISR( _timerHandle xTimer, 
							   osdepBASE_TYPE *pxHigherPriorityTaskWoken )
{
    return !aos_timer_start(&xTimer->timer);	
}

u32  _aos_timerStopFromISR( _timerHandle xTimer, 
							   osdepBASE_TYPE *pxHigherPriorityTaskWoken )
{
    return !aos_timer_stop(&xTimer->timer);	
}

u32  _aos_timerResetFromISR( _timerHandle xTimer, 
							   osdepBASE_TYPE *pxHigherPriorityTaskWoken )
{
    (u32)aos_timer_stop(&xTimer->timer);	
    return !aos_timer_start(&xTimer->timer);	
}

u32  _aos_timerChangePeriodFromISR( _timerHandle xTimer, 
							   osdepTickType xNewPeriod, 
							   osdepBASE_TYPE *pxHigherPriorityTaskWoken )
{
    if(xNewPeriod == 0)
        xNewPeriod += 1;

    (u32)aos_timer_stop(&xTimer->timer);  

    return !aos_timer_change(&xTimer->timer, xNewPeriod);
}

u32  _aos_timerReset( _timerHandle xTimer, 
							   osdepTickType xBlockTime )
{
    (u32)aos_timer_stop(&xTimer->timer);	
    return !aos_timer_start(&xTimer->timer);	
}

void _aos_acquire_wakelock()
{
#if defined(CONFIG_PLATFORM_8195A)

#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
   	pmu_acquire_wakelock(PMU_WLAN_DEVICE);
#endif
	
#elif defined(CONFIG_PLATFORM_8711B)

#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
	if (pmu_yield_os_check()) 
   		 pmu_acquire_wakelock(PMU_WLAN_DEVICE);
#endif

#endif
}

void _aos_release_wakelock()
{

#if defined(CONFIG_PLATFORM_8195A)

#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
    pmu_release_wakelock(PMU_WLAN_DEVICE);
#endif
	
#elif defined(CONFIG_PLATFORM_8711B)

#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
	if (pmu_yield_os_check()) 
		pmu_release_wakelock(PMU_WLAN_DEVICE);
#endif

#endif
}

void _aos_wakelock_timeout(uint32_t timeout)
{
#if defined(CONFIG_PLATFORM_8195A)
	
#elif defined(CONFIG_PLATFORM_8711B)
	if (pmu_yield_os_check()) 
		pmu_set_sysactive_time(timeout);
	else
		DBG_INFO("can't aquire wake during suspend flow!!\n");
#endif
}

u8 _aos_get_scheduler_state(void)
{
	return 0;
}


const struct osdep_service_ops osdep_service = {
	_aos_malloc,			//rtw_vmalloc
	_aos_zmalloc,			//rtw_zvmalloc
	_aos_mfree,			//rtw_vmfree
	_aos_malloc,			//rtw_malloc
	_aos_zmalloc,			//rtw_zmalloc
	_aos_mfree,			//rtw_mfree
	_aos_memcpy,			//rtw_memcpy
	_aos_memcmp,			//rtw_memcmp
	_aos_memset,			//rtw_memset
	_aos_init_sema,		//rtw_init_sema
	_aos_free_sema,		//rtw_free_sema
	_aos_up_sema,			//rtw_up_sema
	_aos_up_sema_from_isr,	//rtw_up_sema_from_isr
	_aos_down_sema,		//rtw_down_sema
	_aos_mutex_init,		//rtw_mutex_init
	_aos_mutex_free,		//rtw_mutex_free
	_aos_mutex_get,		//rtw_mutex_get
	_aos_mutex_get_timeout,//rtw_mutex_get_timeout
	_aos_mutex_put,		//rtw_mutex_put
	_aos_enter_critical,	//rtw_enter_critical
	_aos_exit_critical,	//rtw_exit_critical
	_aos_enter_critical_from_isr,	//rtw_enter_critical_from_isr
	_aos_exit_critical_from_isr,	//rtw_exit_critical_from_isr
	NULL,		//rtw_enter_critical_bh
	NULL,		//rtw_exit_critical_bh
	_aos_enter_critical_mutex,	//rtw_enter_critical_mutex
	_aos_exit_critical_mutex,	//rtw_exit_critical_mutex
	_aos_spinlock_init,		//rtw_spinlock_init
	_aos_spinlock_free,		//rtw_spinlock_free
	_aos_spinlock,				//rtw_spin_lock
	_aos_spinunlock,			//rtw_spin_unlock
	NULL,		//rtw_spinlock_irqsave
	NULL,	//rtw_spinunlock_irqsave
	_aos_init_xqueue,			//rtw_init_xqueue
	_aos_push_to_xqueue,		//rtw_push_to_xqueue
	_aos_pop_from_xqueue,		//rtw_pop_from_xqueue
	_aos_deinit_xqueue,		//rtw_deinit_xqueue
	_aos_get_current_time,		//rtw_get_current_time
	_aos_systime_to_ms,		//rtw_systime_to_ms
	_aos_systime_to_sec,		//rtw_systime_to_sec
	_aos_ms_to_systime,		//rtw_ms_to_systime
	_aos_sec_to_systime,		//rtw_sec_to_systime
	_aos_msleep_os,	//rtw_msleep_os
	_aos_usleep_os,	//rtw_usleep_os
	_aos_mdelay_os,	//rtw_mdelay_os
	_aos_udelay_os,	//rtw_udelay_os
	_aos_yield_os,		//rtw_yield_os
	
	_aos_ATOMIC_SET,	//ATOMIC_SET
	_aos_ATOMIC_READ,	//ATOMIC_READ
	_aos_ATOMIC_ADD,	//ATOMIC_ADD
	_aos_ATOMIC_SUB,	//ATOMIC_SUB
	_aos_ATOMIC_INC,	//ATOMIC_INC
	_aos_ATOMIC_DEC,	//ATOMIC_DEC
	_aos_ATOMIC_ADD_RETURN,	//ATOMIC_ADD_RETURN
	_aos_ATOMIC_SUB_RETURN,	//ATOMIC_SUB_RETURN
	_aos_ATOMIC_INC_RETURN,	//ATOMIC_INC_RETURN
	_aos_ATOMIC_DEC_RETURN,	//ATOMIC_DEC_RETURN

	_aos_modular64,			//rtw_modular64
	_aos_get_random_bytes,		//rtw_get_random_bytes
	_aos_GetFreeHeapSize,		//rtw_getFreeHeapSize

	_aos_create_task,			//rtw_create_task
	_aos_delete_task,			//rtw_delete_task
	_aos_wakeup_task,			//rtw_wakeup_task

	_aos_thread_enter,			//rtw_thread_enter
	_aos_thread_exit,			//rtw_thread_exit

	_aos_timerCreate,			//rtw_timerCreate,       
	_aos_timerDelete,			//rtw_timerDelete,       
	_aos_timerIsTimerActive,	//rtw_timerIsTimerActive,
	_aos_timerStop,			//rtw_timerStop,         
	_aos_timerChangePeriod,	//rtw_timerChangePeriod 
	_aos_timerGetID,			//rtw_timerGetID
	_aos_timerStart,			//rtw_timerStart
	_aos_timerStartFromISR,	//rtw_timerStartFromISR
	_aos_timerStopFromISR,		//rtw_timerStopFromISR
	_aos_timerResetFromISR,	//rtw_timerResetFromISR
	_aos_timerChangePeriodFromISR,	//rtw_timerChangePeriodFromISR
	_aos_timerReset,			//rtw_timerReset

	_aos_acquire_wakelock,		//rtw_acquire_wakelock
	_aos_release_wakelock,		//rtw_release_wakelock
	_aos_wakelock_timeout,		//rtw_wakelock_timeout
	_aos_get_scheduler_state	//rtw_get_scheduler_state
};


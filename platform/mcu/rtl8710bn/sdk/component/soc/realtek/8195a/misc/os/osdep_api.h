/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#ifndef __OSDEP_API_H_
#define __OSDEP_API_H_

#include "os_timer.h"
#include "os_support.h"
#include "osdep_service.h"


#define MAX_SEMA_COUNT                      32	/* the maximum count of a semaphore */

typedef _sema					_Sema;
typedef _mutex					_Mutex;
typedef	u32						_Lock;
typedef struct TIMER_LIST		_Timer;
typedef unsigned long			_IRQL;
typedef _thread_hdl_			_THREAD_HDL_;
typedef VOID					THREAD_RETURN;
typedef VOID					THREAD_CONTEXT;


#ifndef mdelay
#define mdelay(t)					((t/portTICK_RATE_MS)>0)?(vTaskDelay(t/portTICK_RATE_MS)):(vTaskDelay(1))
#endif

#ifndef udelay
#define udelay(t)					((t/(portTICK_RATE_MS*1000))>0)?vTaskDelay(t/(portTICK_RATE_MS*1000)):(vTaskDelay(1))
#endif

/* to delete/start/stop a timer it will send a message to the timer task through a message queue,
    so we define the max wait time for message sending */
#define RTL_TIMER_API_MAX_BLOCK_TIME    1000    // unit is ms
#define RTL_TIMER_API_MAX_BLOCK_TICKS   (RTL_TIMER_API_MAX_BLOCK_TIME/portTICK_RATE_MS)

typedef VOID (*RTL_TIMER_CALL_BACK)(void    *pContext);

typedef struct _RTL_TIMER{
#ifdef PLATFORM_FREERTOS
    _timerHandle        TimerHandle;	// The timer handle of created FreeRTOS soft-timer
#endif    
	RTL_TIMER_CALL_BACK	CallBackFunc;	// Callback function of this timer
	u32                 msPeriod;		// The period of this timer
	void                *Context;		// Timer specific context.
	u8                  isPeriodical;	// Is a periodical timer
	u8                  TimerName[35];	// The Name of timer
}RTL_TIMER, *PRTL_TIMER;

__inline static VOID RtlEnterCritical(VOID)
{
	rtw_enter_critical(NULL, NULL);
}

__inline static VOID RtlExitCritical(VOID)
{
	rtw_exit_critical(NULL, NULL);
}

__inline static VOID RtlEnterCriticalBh(IN  _Lock *plock, IN  _IRQL *pirqL)
{
	rtw_enter_critical_bh((_lock *)plock, pirqL);
}

__inline static VOID RtlExitCriticalBh(IN  _Lock *plock, IN  _IRQL *pirqL)
{
	rtw_exit_critical_bh((_lock *)plock, pirqL);
}

__inline static u32 RtlEnterCriticalMutex(IN  _Mutex *pmutex, IN  _IRQL *pirqL)
{
	return rtw_enter_critical_mutex(pmutex, pirqL);
}

__inline static VOID RtlExitCriticalMutex(IN  _Mutex *pmutex,IN  _IRQL *pirqL)
{
    rtw_exit_critical_mutex(pmutex, pirqL);
}

__inline static VOID RtlInitTimer(
    IN  _Timer *ptimer,
    IN  VOID *Data,
    IN  VOID (*pfunc)(VOID *),
    IN  VOID* cntx
)
{
	ptimer->Function = pfunc;
	ptimer->Data = (unsigned long)cntx;
	InitTimer(ptimer);
}

__inline static VOID RtlSetTimer(
    IN  _Timer *ptimer,
    IN  u32 delay_time
)
{	
	ModTimer(ptimer , (JIFFIES+(delay_time*RTL_HZ/1000)));	
}

__inline static VOID RtlCancelTimer(
    IN  _Timer *ptimer,
    IN  u8 *bcancelled
)
{
	DelTimerSync(ptimer); 	
	*bcancelled=  _TRUE;//TRUE ==1; FALSE==0
}

__inline static u32 RtlSystime2Ms(IN  u32 systime)
{
	return rtw_systime_to_ms(systime);
}

__inline static u32 RtlMs2Systime(IN  u32 ms)
{
	return rtw_ms_to_systime(ms);
}

extern u8*	RtlZmalloc(u32 sz);
extern u8*	RtlMalloc(u32 sz);
extern VOID	RtlMfree(u8 *pbuf, u32 sz);

extern VOID* RtlMalloc2d(u32 h, u32 w, u32 size);
extern VOID	RtlMfree2d(VOID *pbuf, u32 h, u32 w, u32 size);

extern VOID	RtlInitSema(_Sema *sema, u32 init_val);
extern VOID	RtlFreeSema(_Sema	*sema);
extern VOID	RtlUpSema(_Sema	*sema);
extern VOID	RtlUpSemaFromISR(_Sema	*sema);
extern u32	RtlDownSema(_Sema *sema);
extern u32	RtlDownSemaWithTimeout(_Sema *sema, u32 ms);

extern VOID	RtlMutexInit(_Mutex *pmutex);
extern VOID	RtlMutexFree(_Mutex *pmutex);

extern VOID	RtlSpinlockInit(_Lock *plock);
extern VOID	RtlSpinlockFree(_Lock *plock);
extern VOID	RtlSpinlock(_Lock	*plock);
extern VOID	RtlSpinunlock(_Lock	*plock);
extern VOID	RtlSpinlockEx(_Lock	*plock);
extern VOID	RtlSpinunlockEx(_Lock	*plock);

extern VOID	RtlSleepSchedulable(u32 ms);

extern VOID	RtlMsleepOS(u32 ms);
extern VOID	RtlUsleepOS(u32 us);
extern VOID RtlMdelayOS(u32 ms);
extern VOID RtlUdelayOS(u32 us);
extern VOID RtlYieldOS(VOID);

#define RtlUpMutex(mutex)		RtlUpSema(mutex)
#define RtlDownMutex(mutex)		RtlDownSema(mutex)

__inline static u8 RtlCancelTimerEx(IN  _Timer *ptimer)
{
	DelTimerSync(ptimer);
	return 0; 
}


static __inline VOID ThreadEnter(IN  char *name)
{
	DBG_8195A("\rRTKTHREAD_enter %s\n", name);
}

#define ThreadExit() do{DBG_8195A("\rRTKTHREAD_exit %s\n", __FUNCTION__);}while(0)

__inline static VOID FlushSignalsThread(VOID) 
{
}


#define RTL_RND(sz, r) ((((sz)+((r)-1))/(r))*(r))
#define RTL_RND4(x)	(((x >> 2) + (((x & 3) == 0) ?  0: 1)) << 2)

__inline static u32 RtlRnd4(IN  u32 sz)
{

	u32	val;

	val = ((sz >> 2) + ((sz & 3) ? 1: 0)) << 2;
	
	return val;

}

__inline static u32 RtlRnd8(IN  u32 sz)
{

	u32	val;

	val = ((sz >> 3) + ((sz & 7) ? 1: 0)) << 3;
	
	return val;

}

__inline static u32 RtlRnd128(IN  u32 sz)
{

	u32	val;

	val = ((sz >> 7) + ((sz & 127) ? 1: 0)) << 7;
	
	return val;

}

__inline static u32 RtlRnd256(IN  u32 sz)
{

	u32	val;

	val = ((sz >> 8) + ((sz & 255) ? 1: 0)) << 8;
	
	return val;

}

__inline static u32 RtlRnd512(IN  u32 sz)
{

	u32	val;

	val = ((sz >> 9) + ((sz & 511) ? 1: 0)) << 9;
	
	return val;

}

__inline static u32 BitShift(IN  u32 BitMask)
{
	u32 i;

	for (i = 0; i <= 31; i++)
		if (((BitMask>>i) &  0x1) == 1) break;

	return i;
}


//#ifdef __GNUC__
#ifdef PLATFORM_LINUX
#define STRUCT_PACKED __attribute__ ((packed))
#else
#define STRUCT_PACKED
#endif


//Atomic integer operations
#define RTL_ATOMIC_T atomic_t

static inline VOID RTL_ATOMIC_SET(IN  RTL_ATOMIC_T *v, IN  u32 i)
{
	ATOMIC_SET(v,i);
}

static inline uint32_t RTL_ATOMIC_READ(IN  RTL_ATOMIC_T *v)
{
	return ATOMIC_READ(v);
}

static inline VOID RTL_ATOMIC_ADD(IN  RTL_ATOMIC_T *v, IN  u32 i)
{
	ATOMIC_ADD(v,i);
}

static inline VOID RTL_ATOMIC_SUB(IN  RTL_ATOMIC_T *v, IN  u32 i)
{
	ATOMIC_SUB(v,i);
}

static inline VOID RTL_ATOMIC_INC(IN  RTL_ATOMIC_T *v)
{
	ATOMIC_INC(v);
}

static inline VOID RTL_ATOMIC_DEC(IN  RTL_ATOMIC_T *v)
{
	ATOMIC_DEC(v);
}

static inline u32 RTL_ATOMIC_ADD_RETURN(IN  RTL_ATOMIC_T *v, IN  u32 i)
{
	return ATOMIC_ADD_RETURN(v, i);
}

static inline u32 RTL_ATOMIC_SUB_RETURN(IN  RTL_ATOMIC_T *v, IN  u32 i)
{
	return ATOMIC_SUB_RETURN(v, i);
}

static inline u32 RTL_ATOMIC_INC_RETURN(IN  RTL_ATOMIC_T *v)
{
	return ATOMIC_INC_RETURN(v);
}

static inline u32 RTL_ATOMIC_DEC_RETURN(IN  RTL_ATOMIC_T *v)
{
	return ATOMIC_DEC_RETURN(v);
}

extern u64 RtlModular64(u64 x, u64 y);

extern PRTL_TIMER
RtlTimerCreate(
    IN char *pTimerName,
    IN u32 TimerPeriodMS,
    IN RTL_TIMER_CALL_BACK CallbckFunc,
    IN void *pContext,
    IN u8 isPeriodical
);

extern VOID
RtlTimerDelete(
    IN PRTL_TIMER pTimerHdl
);

extern u8
RtlTimerStart(
    IN PRTL_TIMER pTimerHdl,
    IN u8 isFromISR
);

extern u8
RtlTimerStop(
    IN PRTL_TIMER pTimerHdl,
    IN u8 isFromISR
);

extern u8
RtlTimerReset(
    IN PRTL_TIMER pTimerHdl,
    IN u8 isFromISR
);

extern u8
RtlTimerChangePeriod(
    IN PRTL_TIMER pTimerHdl,
    IN u32 NewPeriodMS,
    IN u8 isFromISR
);

#endif	//#ifndef __OSDEP_API_H_



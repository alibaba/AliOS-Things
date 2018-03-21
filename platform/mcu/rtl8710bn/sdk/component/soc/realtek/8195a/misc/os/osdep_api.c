/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
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


#define _OSDEP_API_C_

#include <osdep_api.h>
#include "osdep_service.h"

extern _LONG_CALL_ char *_strcpy(char *dest, const char *src);
extern _LONG_CALL_ VOID *_memset(void *dst0, int Val,SIZE_T length);

u8* RtlMalloc(IN  u32 sz)
{
	u8 	*pbuf=NULL;

	pbuf = rtw_malloc((u32)sz);

	return pbuf;		
}


u8* RtlZmalloc(IN  u32 sz)
{
    u8  *pbuf;
 
    pbuf= rtw_malloc((u32)sz);

    if (pbuf != NULL) {
        _memset(pbuf, 0, sz);
    }

    return pbuf;    
}

VOID RtlMfree(IN  u8 *pbuf, IN  u32 sz)
{
    rtw_mfree(pbuf, sz);	
}

VOID* RtlMalloc2d(IN  u32 h, IN  u32 w, IN  u32 size)
{
	u32 j;

	VOID **a = (VOID **) rtw_malloc2d(h, w, size);
	if(a == NULL)
	{
		DBG_ERROR_LOG("%s: alloc memory fail!\n", __FUNCTION__);
		return NULL;
	}

	return a;
}

VOID RtlMfree2d(IN  VOID *pbuf, IN  u32 h, IN  u32 w, IN  u32 size)
{
	rtw_mfree2d(pbuf, h, w, size);
}

VOID RtlInitSema(IN  _Sema *sema, IN  u32 init_val)
{
	rtw_init_sema(sema, init_val);
}

VOID RtlFreeSema(IN  _Sema *sema)
{
	rtw_free_sema(sema);
}

VOID RtlUpSema(IN  _Sema *sema)
{
	rtw_up_sema(sema);
}

VOID RtlUpSemaFromISR(IN  _Sema *sema)
{
	rtw_up_sema_from_isr(sema);
}

u32 RtlDownSema(IN   _Sema *sema)
{
	rtw_down_sema(sema);
	return  _SUCCESS;
}

u32 RtlDownSemaWithTimeout(IN   _Sema *sema,IN   u32 ms)
{
	return rtw_down_timeout_sema(sema, ms);
}

VOID	RtlMutexInit(IN _Mutex *pmutex)
{
	rtw_mutex_init(pmutex);
}

VOID RtlMutexFree(IN _Mutex *pmutex)
{
	rtw_mutex_free(pmutex);
}

VOID RtlSpinlockInit(IN  _Lock *plock)
{
	rtw_spinlock_init((_lock *)plock);
}

VOID RtlSpinlockFree(IN  _Lock *plock)
{
	rtw_spinlock_free((_lock *)plock);
}

VOID RtlSpinlock(IN  _Lock *plock)
{
    rtw_spin_lock((_lock *)plock);
}

VOID RtlSpinunlock(IN  _Lock *plock)
{
	rtw_spin_unlock((_lock *)plock);
}

VOID RtlSpinlockEx(IN  _Lock *plock)
{
}

VOID RtlSpinunlockEx(IN  _Lock *plock)
{
}

u32	 RtlGetCurrentTime(VOID)
{
    return rtw_get_current_time();
}

VOID RtlSleepSchedulable(IN  u32 ms) 
{
}

VOID RtlMsleepOS(IN u32 ms)
{
	rtw_msleep_os(ms);
}

VOID RtlUsleepOS(IN  u32 us)
{
	rtw_usleep_os(us);
}

VOID RtlMdelayOS(IN   u32 ms)
{
   	rtw_mdelay_os(ms); 
}

VOID RtlUdelayOS(IN  u32 us)
{
	rtw_udelay_os(us);
}

VOID RtlYieldOS(VOID)
{
	rtw_yield_os();
}


#if defined(__ICCARM__)
u64 RtlModular64(IN  u64 n, IN  u64 base)
{
	unsigned int __base = (base);	
	unsigned int __rem;	
	//(void)(((typeof((n)) *)0) == ((__uint64_t *)0));
	if (((n) >> 32) == 0) {	
		__rem = (unsigned int)(n) % __base;
		(n) = (unsigned int)(n) / __base;
	} else 					
		__rem = __Div64_32(&(n), __base);
	return __rem;

}
#else
u64 RtlModular64(IN  u64 x, IN  u64 y)
{
	return rtw_modular64(x, y);
}
#endif

/******************************************************************************
 * Function: RtlTimerCallbckEntry
 * Desc: This function is a timer callback wrapper. All OS timer callback 
 *      will call this function and then call the real callback function inside
 *      this function.
 *
 * Para:
 * 	 pxTimer: The FreeRTOS timer handle which is expired and call this callback.
 *
 * Return: None
 *
 ******************************************************************************/
#ifdef PLATFORM_FREERTOS
void RtlTimerCallbckEntry (IN _timerHandle pxTimer)
{
    PRTL_TIMER pTimer;

    if (NULL == pxTimer) {
        MSG_TIMER_ERR("RtlTimerCallbckEntry: NULL Timer Handle Err!\n");
        return;
    }
    
    pTimer = (PRTL_TIMER) rtw_timerGetID( pxTimer );
    pTimer->CallBackFunc(pTimer->Context);
}
#endif  // end of "#ifdef PLATFORM_FREERTOS"

/******************************************************************************
 * Function: RtlTimerCreate
 * Desc: To create a software timer.
 *
 * Para:
 * 	 pTimerName: A string for the timer name.
 * 	 TimerPeriodMS: The timer period, the unit is milli-second.
 *   CallbckFunc: The callback function of this timer.
 *   pContext: A pointer will be used as the parameter to call the timer 
 *              callback function.
 *   isPeriodical: Is this timer periodical ? (Auto reload after expired)
 * Return: The created timer handle, a pointer. It can be used to delete the 
 *          timer. If timer createion failed, return NULL.
 *
 ******************************************************************************/
PRTL_TIMER RtlTimerCreate(
    IN char *pTimerName,
    IN u32 TimerPeriodMS,
    IN RTL_TIMER_CALL_BACK CallbckFunc,
    IN void *pContext,
    IN u8 isPeriodical)
{
    PRTL_TIMER pTimer;
    u32 timer_ticks;
    int i;

    pTimer = (PRTL_TIMER)RtlZmalloc(sizeof(RTL_TIMER));
    if (NULL == pTimer) {
        MSG_TIMER_ERR("RtlTimerCreate: Alloc Mem Err!\n");
        return NULL;
    }

    if (portTICK_RATE_MS >= TimerPeriodMS) {
        timer_ticks = 1;    // at least 1 system tick
    }
    else {
        timer_ticks = TimerPeriodMS/portTICK_RATE_MS;
    }

    pTimer->TimerHandle = rtw_timerCreate ((const char*)(pTimer->TimerName), timer_ticks, 
            (portBASE_TYPE)isPeriodical, (void *) pTimer, RtlTimerCallbckEntry);

#ifdef PLATFORM_FREERTOS    // if any RTOS is used
    if (pTimer->TimerHandle) {
        pTimer->msPeriod = TimerPeriodMS;
        pTimer->CallBackFunc = CallbckFunc;
        pTimer->Context = pContext;
        pTimer->isPeriodical = isPeriodical;
        // copy the timer name
        if (NULL != pTimerName) {
            for(i = 0; i < sizeof(pTimer->TimerName); i++)
            {
                pTimer->TimerName[i] = pTimerName[i]; 
                if(pTimerName[i] == '\0')
                {
                    break;
                }
            }
        }
        else {
            _strcpy((char*)(pTimer->TimerName), "None");
        }
        MSG_TIMER_INFO("RtlTimerCreate: SW Timer Created: Name=%s Period=%d isPeriodical=%d\n", \
            pTimer->TimerName, pTimer->msPeriod, pTimer->isPeriodical);
    }
    else 
#endif
    {
        RtlMfree((u8 *)pTimer, sizeof(RTL_TIMER));
        pTimer = NULL;
        MSG_TIMER_ERR("RtlTimerCreate: OS Create Timer Failed!\n");
    }

    return (pTimer);    
}

/******************************************************************************
 * Function: RtlTimerDelete
 * Desc: To delete a created software timer.
 *
 * Para:
 * 	 pTimerHdl: The timer to be deleted
 *
 * Return: None
 *
 ******************************************************************************/
VOID RtlTimerDelete(IN PRTL_TIMER pTimerHdl)
{
#ifdef PLATFORM_FREERTOS
    portBASE_TYPE ret;
#endif

    if (NULL == pTimerHdl) {
        MSG_TIMER_ERR("RtlTimerDelete: NULL Timer Handle!\n");
        return;
    }

    MSG_TIMER_INFO("RtlTimerDelete: Name=%s\n", pTimerHdl->TimerName);
#ifdef PLATFORM_FREERTOS
    /* try to delete the soft timer and wait max RTL_TIMER_API_MAX_BLOCK_TICKS 
        to send the delete command to the timer command queue */
	ret = rtw_timerDelete(pTimerHdl->TimerHandle, RTL_TIMER_API_MAX_BLOCK_TICKS);
    if (pdPASS != ret) {
        MSG_TIMER_ERR("RtlTimerDelete: Delete OS Timer Failed!\n");
    }
#endif    
    RtlMfree((u8 *)pTimerHdl, sizeof(RTL_TIMER));

}

/******************************************************************************
 * Function: RtlTimerStart
 * Desc: To start a created timer..
 *
 * Para:
 * 	 pTimerHdl: The timer to be started.
 *	isFromISR: The flag to indicate that is this function is called from an ISR.
 *
 * Return: _SUCCESS or _FAIL
 *
 ******************************************************************************/
u8 RtlTimerStart(IN PRTL_TIMER pTimerHdl, IN u8 isFromISR)
{
#ifdef PLATFORM_FREERTOS
    u8 ret=_FAIL;
    portBASE_TYPE HigherPriorityTaskWoken=pdFALSE;

    if (isFromISR) {
        if (pdPASS == rtw_timerStartFromISR(pTimerHdl->TimerHandle,&HigherPriorityTaskWoken))
        {
            // start OS timer successful
            if (pdFALSE != HigherPriorityTaskWoken) {
                rtw_yield_os();
            }
            ret = _SUCCESS;
        }
        else {
            MSG_TIMER_ERR("RtlTimerStart: Start Timer(%s) from ISR failed\n", pTimerHdl->TimerName);
        }
    }
    else {
        if (pdPASS == rtw_timerStart(pTimerHdl->TimerHandle, RTL_TIMER_API_MAX_BLOCK_TICKS)) {
            ret = _SUCCESS;
        }
        else {
            MSG_TIMER_ERR("RtlTimerStart: Start Timer(%s) failed\n", pTimerHdl->TimerName);
        }
    }

    MSG_TIMER_INFO("RtlTimerStart: SW Timer %s Started\n", pTimerHdl->TimerName);

    return ret;
#endif
}

/******************************************************************************
 * Function: RtlTimerStop
 * Desc: To stop a running timer..
 *
 * Para:
 * 	 pTimerHdl: The timer to be stoped.
 *	isFromISR: The flag to indicate that is this function is called from an ISR.
 *
 * Return: _SUCCESS or _FAIL
 *
 ******************************************************************************/
u8 RtlTimerStop(IN PRTL_TIMER pTimerHdl, IN u8 isFromISR)
{
#ifdef PLATFORM_FREERTOS
    u8 ret=_FAIL;
    portBASE_TYPE HigherPriorityTaskWoken=pdFALSE;

    if (isFromISR) {
        if (pdPASS == rtw_timerStopFromISR(pTimerHdl->TimerHandle,&HigherPriorityTaskWoken))
        {
            // start OS timer successful
            if (pdFALSE != HigherPriorityTaskWoken) {
                rtw_yield_os();
            }
            ret = _SUCCESS;
        }
    }
    else {
        if (pdPASS == rtw_timerStop(pTimerHdl->TimerHandle, RTL_TIMER_API_MAX_BLOCK_TICKS)) {
            ret = _SUCCESS;
        }
    }

    if (_FAIL == ret) {
        MSG_TIMER_ERR("RtlTimerStop: Stop Timer(%s) Failed, IsFromISR=%d\n", pTimerHdl->TimerName, isFromISR);
    }

    MSG_TIMER_INFO("RtlTimerStop: SW Timer %s Stoped\n", pTimerHdl->TimerName);

    return ret;
#endif
}

/******************************************************************************
 * Function: RtlTimerReset
 * Desc: To reset a timer. A reset will get a re-start and reset
 *          the timer ticks counting. A running timer expired time is relative 
 *          to the time when Reset function be called. Please ensure the timer
 *          is in active state (Started). A stopped timer also will be started
 *          when this function is called.
 *
 * Para:
 * 	 pTimerHdl: The timer to be reset.
 *	isFromISR: The flag to indicate that is this function is called from an ISR.
 *
 * Return: _SUCCESS or _FAIL
 *
 ******************************************************************************/
u8
RtlTimerReset(
    IN PRTL_TIMER pTimerHdl,
    IN u8 isFromISR
)
{
#ifdef PLATFORM_FREERTOS
    u8 ret=_FAIL;
    portBASE_TYPE HigherPriorityTaskWoken=pdFALSE;

    if (isFromISR) {
        if (pdPASS == rtw_timerResetFromISR(pTimerHdl->TimerHandle,&HigherPriorityTaskWoken))
        {
            // start OS timer successful
            if (pdFALSE != HigherPriorityTaskWoken) {
                rtw_yield_os();
            }
            ret = _SUCCESS;
        }
    }
    else {
        if (pdPASS == rtw_timerReset(pTimerHdl->TimerHandle, RTL_TIMER_API_MAX_BLOCK_TICKS)) {
            ret = _SUCCESS;
        }
    }

    if (_FAIL == ret) {
        MSG_TIMER_ERR("RtlTimerReset: Reset Timer(%s) Failed, IsFromISR=%d\n", pTimerHdl->TimerName, isFromISR);
    }

    MSG_TIMER_INFO("RtlTimerReset: SW Timer %s Reset\n", pTimerHdl->TimerName);

    return ret;
#endif
}

/******************************************************************************
 * Function: RtlTimerChangePeriod
 * Desc: To change the period of a timer that was created previously.
 *
 * Para:
 * 	 pTimerHdl: The timer handle to be changed the priod.
 *   NewPeriodMS: The new timer period, in milli-second.
 *	 isFromISR: The flag to indicate that is this function is called from an ISR.
 *
 * Return: _SUCCESS or _FAIL
 *
 ******************************************************************************/
u8 RtlTimerChangePeriod(
    IN PRTL_TIMER pTimerHdl,
    IN u32 NewPeriodMS,
    IN u8 isFromISR)
{
#ifdef PLATFORM_FREERTOS
    u32 timer_ticks;
    u8 ret=_FAIL;
    portBASE_TYPE HigherPriorityTaskWoken=pdFALSE;

    if (portTICK_RATE_MS >= NewPeriodMS) {
        timer_ticks = 1;    // at least 1 system tick
    }
    else {
        timer_ticks = NewPeriodMS/portTICK_RATE_MS;
    }

    if (isFromISR) {
        if (pdPASS == rtw_timerChangePeriodFromISR(pTimerHdl->TimerHandle, timer_ticks, &HigherPriorityTaskWoken))
        {
            // start OS timer successful
            if (pdFALSE != HigherPriorityTaskWoken) {
                taskYIELD();
            }
            ret = _SUCCESS;
        }
    }
    else {
        if (pdPASS == rtw_timerChangePeriod(pTimerHdl->TimerHandle, timer_ticks, RTL_TIMER_API_MAX_BLOCK_TICKS)) {
            ret = _SUCCESS;
        }
    }

    if (_FAIL == ret) {
        MSG_TIMER_ERR("RtlTimerChangePeriod: Change Timer(%s) Period Failed, IsFromISR=%d\n", pTimerHdl->TimerName, isFromISR);
    }
    else {
        pTimerHdl->msPeriod = NewPeriodMS;
        MSG_TIMER_INFO("RtlTimerChangePeriod: SW Timer %s change period to %d\n", pTimerHdl->TimerName, pTimerHdl->msPeriod);
    }

    
    return ret;
#endif
}


/******************************************************************************
  *
  * Name: sys-support.h - System type support for Linux
  *       $Revision: 1.1.1.1 $
  *
  *****************************************************************************/

#ifndef __OS_TIMER_H__
#define __OS_TIMER_H__

#include "diag.h"
#include "os_support.h"
#include "osdep_service.h"


#define JIFFIES	        xTaskGetTickCount()

enum {
	TIMER_NO_INIT = 0,
	TIMER_INIT = 1,
	TIMER_START = 2,
	TIMER_DISABLE = 3
};

struct TIMER_LIST {
	_timerHandle    TimeHdl;
	u32             Flag;
	unsigned long	Data;
	VOID            (*Function)(void *);
	u32             TimerID;
};

static inline VOID InitTimer(IN  struct TIMER_LIST *Timer)
{
	u32 TimerID = Timer->TimerID;
	VOID (*Function)(VOID *) = Timer->Function;
	CPSR_ALLOC();
	save_and_cli();

	if (Timer->Flag != TIMER_DISABLE) {
		if (Timer->Flag == TIMER_NO_INIT) {
            Timer->TimeHdl = rtw_timerCreate( (const char *)"Timer",	// Just a test name, not used by the kernel.
                                            ( 100 ),					// The timer period in ticks.
                                            _FALSE,					// The timers will auto-reload themselves when they expire.
                                            ( void * ) TimerID,			// Assign each timer a unique id equal to its array index.
                                            Function
                                            #ifdef RTK_MODE_TIMER                 
                                            ,data				// Each timer calls the same callback when it expires.                                            
                                            #endif
                                          );
            if (NULL == Timer->TimeHdl) {
                DBG_ERROR_LOG("\rInitial Timer fail !!!!!!!!!\n");
            }
            else {
                TimerID++;
            }

			Timer->Flag = TIMER_INIT;
		}
		else if (Timer->Flag == TIMER_START) {
            rtw_timerStop(Timer->TimeHdl,0);
			Timer->Flag = TIMER_DISABLE;
		}
	}
    
	restore_flags();
}

static inline void ModTimer(IN  struct TIMER_LIST *Timer, IN  u32 TimeoutTicks)
{
#ifndef PLATFORM_FREERTOS
	u32 Flags;
#endif

  	void (*Function)(void *) = Timer->Function;
	CPSR_ALLOC();
	save_and_cli();

	if (Timer->Flag == TIMER_NO_INIT) {
		if (Timer->Function) {
            Timer->TimeHdl = rtw_timerCreate((const char *)"Timer",                    // Just a text name, not used by the kernel.
                                            ( 100 ),                    // The timer period in ticks.
                                            _FALSE,                    // The timers will auto-reload themselves when they expire.
                                            ( void * ) Timer->TimerID,  // Assign each timer a unique id equal to its array index.
                                            Function
                                            #ifdef RTK_MODE_TIMER                                                              
                                            ,Timer->Data	// Each timer calls the same callback when it expires.
                                            #endif
                                      );
            if (NULL == Timer->TimeHdl) {
                DBG_ERROR_LOG("\rInitial Timer fail !!!!!!!!!\n");
            }
            else {
                Timer->TimerID++;
            }

			Timer->Flag = TIMER_INIT;
		}
		else {
			restore_flags();
			return;
		}
	}
	else if (Timer->Flag == TIMER_START) {
        rtw_timerStop(Timer->TimeHdl,0);
	 	Timer->Flag = TIMER_DISABLE;
	}
	
	TimeoutTicks -= rtw_get_current_time();
	if (TimeoutTicks <= 0)
		TimeoutTicks = 2;

	if (xTimerStart(Timer->TimeHdl, TimeoutTicks ))
		Timer->Flag = TIMER_START;
	else
		DBG_ERROR_LOG("\r###mod_timer() - no slots available###\n");
	restore_flags();
}

  
static inline int TimerPending (IN  const struct TIMER_LIST *Timer)
{
    if (Timer->TimeHdl && Timer->Flag != TIMER_NO_INIT)
		return 1;
	else
		return 0;
}
  
static inline void  DelTimerSync(IN  struct TIMER_LIST *Timer)
{	CPSR_ALLOC();
	save_and_cli();
	
    if (Timer->TimeHdl && Timer->Flag != TIMER_INIT) {
        if (Timer->Flag == TIMER_START)
            rtw_timerStop(Timer->TimeHdl, 0);
        
        rtw_timerDelete(Timer->TimeHdl, 0);
        Timer->Flag = TIMER_NO_INIT;
    }

	restore_flags();
}

 /*
  *      These inlines deal with timer wrapping correctly. You are 
  *      strongly encouraged to use them
  *      1. Because people otherwise forget
  *      2. Because if the timer wrap changes in future you wont have to
  *         alter your driver code.
  *
  * time_after(a,b) returns true if the time a is after time b.
  *
  * Do this with "<0" and ">=0" to only test the sign of the result. A
  * good compiler would generate better code (and a really good compiler
  * wouldn't care). Gcc is currently neither.
  */
 #define TIME_AFTER(a,b)		((long)(b) - (long)(a) < 0)
 #define TIMER_BEFORE(a,b)		TIME_AFTER(b,a)
  
 #define TIME_AFTER_EQ(a,b)		((long)(a) - (long)(b) >= 0)
 #define TIMER_BEFORE_EQ(a,b)		TIME_AFTER_EQ(b,a)
  


#endif //__OS_TIMER_H__
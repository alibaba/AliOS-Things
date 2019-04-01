#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
/* The definition of the timers themselves. */
typedef struct tmrTimerControl
{
	const char				*pcTimerName;		/*<< Text name.  This is not used by the kernel, it is included simply to make debugging easier. */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
	ListItem_t				xTimerListItem;		/*<< Standard linked list item as used by all kernel features for event management. */
	TickType_t				xTimerPeriodInTicks;/*<< How quickly and often the timer expires. */
	UBaseType_t				uxAutoReload;		/*<< Set to pdTRUE if the timer should be automatically restarted once expired.  Set to pdFALSE if the timer is, in effect, a one-shot timer. */
	void 					*pvTimerID;			/*<< An ID to identify the timer.  This allows the timer to be identified when the same callback is used for multiple timers. */
	TimerCallbackFunction_t	pxCallbackFunction;	/*<< The function that will be called when the timer expires. */
	#if( configUSE_TRACE_FACILITY == 1 )
		UBaseType_t			uxTimerNumber;		/*<< An ID assigned by trace tools such as FreeRTOS+Trace */
	#endif
} xTIMER;

typedef xTIMER Timer_t;
/* Added by Felix: 
 *   
 *  This API only can be used to change the auto-reload state of a dormant 
 *  timer. 
 */
BaseType_t xTimerChangeSetting (TimerHandle_t xTimer, TickType_t ticks, BaseType_t autoReload, void *args)
{
    if (pdFALSE == xTimerIsTimerActive(xTimer))
    {
        Timer_t *pxTimer = (Timer_t *)xTimer;
        pxTimer->pvTimerID = args;
        pxTimer->uxAutoReload = autoReload;  
        pxTimer->xTimerPeriodInTicks = ticks;
        return pdPASS;
    }
    return pdFAIL;
}

#include "k_api.h"
#include "k_err.h"
#include "k_sys.h"
#include "sxs_io.h"
#include "sxr_mem.h"
#include "sxr_csp.h"
#include "sxs_rmt.h"
#include "sxr_tim.h"
#include "sxr_jbsd.h"
#include "sxr_tksd.h"
#include "aos_adapter_timeout.h"


#ifndef RHINO_ONLY

/*can be used in interrupt*/
void sxr_wakeup_rhino_task(void *param)
{
    /*wake up*/

    UINT8 mbx = (UINT32)param & 0xff;
    UINT8 task = ((UINT32)param >> 8) & 0xff;
    
    sxr_WakeupTaskFromQueue(task, mbx);
   
}


BOOL sxr_rhino_SemaTryTake(sxr_rhino_sema *sema, UINT32 ms)
{
    if (sema->mbx == 0 || sema->mbx == 0xff)
        return FALSE;

    UINT32 sc = hal_SysEnterCriticalSection();
    if (sema->count > 0)
        goto passed;
    if (ms == 0)
        goto failed;

    UINT32 param = (sxr_GetCurrentTaskId() << 8) | sema->mbx;
    sxr_StartFunctionTimer((ms * HAL_TICK1S) / 1000, sxr_wakeup_rhino_task, (void *)param, 0);
    sxr_TaskWaitToQueue(sema->mbx);
    sxr_StopFunctionTimer2(sxr_wakeup_rhino_task, (void *)param);

    if (sema->count > 0)
        goto passed;

failed:
    hal_SysExitCriticalSection(sc);
    return FALSE;

passed:
    sema->count--;
    hal_SysExitCriticalSection(sc);
    return TRUE;
}



VOID sxr_rhino_SemaRelease(sxr_rhino_sema *sema)
{
    if (sema->mbx == 0 || sema->mbx == 0xff)
        return;

    UINT32 sc = hal_SysEnterCriticalSection();
    sema->count++;
    sxr_WakeupFirstTaskFromQueue(sema->mbx);
    hal_SysExitCriticalSection(sc);
}


/*see COS_SemaInit*/
VOID sxr_rhino_SemaInit(sxr_rhino_sema *sema, UINT32 init)
{
    COS_SemaDestroy(sema);

    sema->count = init;
    sema->mbx = sxr_NewMailBox();
}



#endif

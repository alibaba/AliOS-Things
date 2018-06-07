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
#include <hal_mem_map.h>

#include <hal_debug.h>
#include "aos_adapter_timeout.h"
// Alios kernel main entry.

extern void test_certificate(void);
extern void aos_Timer_Initialise(void);

ktask_t demo1;
cpu_stack_t      g_demo1_task_stack[1024];

#define   SXR_RHINO_TICK_MS       20

static void sxs_TimerDelivery (UINT32 *Id)
{
    sxs_Raise (_SXR, "Timer not handled %x\n", Id);
}

static void sxs_Init(void)
{
    UINT32 i;
// -----------------------
// Create Heaps
// -----------------------
    for (i=0; i<PAL_NB_RAM_AREA; i++)
    {
        if (i == 0)
            _sxr_NewHeap ( (void *)(((UINT8 *)PAL_HEAP_START(0)) + sizeof (sxs_Boot_t)), PAL_HEAP_SIZE(0) - sizeof (sxs_Boot_t), 0);
        else
            _sxr_NewHeap (PAL_HEAP_START(i), PAL_HEAP_SIZE(i), i);
    }

    if (sxs_Boot -> Pattern != SXS_BOOT_PATTERN)
    {
        memset (sxs_Boot, 0, sizeof (sxs_Boot_t));
        sxs_Boot -> Pattern = SXS_BOOT_PATTERN;
    }

    sxs_SetFnStamp( (UINT32*)(((UINT32)_sxs_SetFnStamp) & ~3) );

//------------------------
// Init OS timer
//------------------------

//    sxr_InitTimer (sxs_TimerDelivery, pal_TimerSetCnt, pal_TimerGetCnt, SXR_REGULAR_TIMER);
//------------------------
// Init OS Tasks
//------------------------
    //sxr_TkSdInit ();
//------------------------
// Init OS Jobs
//------------------------
    sxr_JbSdInit ();
//------------------------
// Init OS Mailboxes
//------------------------
    //sxr_SbxInit ();
//------------------------
// Init OS Mutex
//------------------------
// TODO: Re-enable this init when we get the latest BASE/SX.
// sxr_MutexInit ();
//------------------------
// Init OS Queues
//------------------------
    //sxr_InitQueue ();

//------------------------
// Init OS Trace
//------------------------
 //   sxs_IoInit ();

//-----------------------
// Start Idle Task
// ----------------------
    //sxr_StartTask (sxr_NewTask (&sxs_TaskIdleDesc), NIL);

}

#ifndef RHINO_ONLY
sxr_rhino_sema g_rhino_sxs_sem;
extern void sxr_rhino_tick_proc(void *param);
#endif

#include <aos/kernel.h>

/*
static aos_sem_t    g_sem;

void test_sem_timeout(void)
{
    int ret;
    ret = aos_sem_new(&g_sem, 0);

    hal_HstSendEvent(0x0001a244);
    aos_sem_wait(&g_sem, 20);
}*/

void demo1_task(void *pa)
{
    int  ev = 1;
    #ifdef RHINO_ONLY
    // Set up timings for SRAM and FLASH
    memd_FlashOpen(tgt_GetMemdFlashConfig());
    sxs_Init();


    #else
    
    UINT32 param;
    /*create sem for wakeup rhino task g_rhino_sxs_taskid
      it should be  wakeup by  sxr_rhino_SemaRelease(&g_rhino_sxs_sem)
    */
    sxr_rhino_SemaInit(&g_rhino_sxs_sem, 0);

    /*create a timer to simulator tick*/
    param = (sxr_GetCurrentTaskId() << 8) | g_rhino_sxs_sem.mbx;
    sxr_StartFunctionTimer ((SXR_RHINO_TICK_MS * HAL_TICK1S) / 1000, sxr_rhino_tick_proc,(void *)param, _SXR);
    #endif

    test_certificate();

    while(1)
    {
        hal_DbgTrace((1 << 21), 0, "alios hello world. count is %d\n", ev);
        ev++;
        //krhino_task_sleep(1024);
        //rhino_to_sxr_sleep(100 * 16384 / 1000);
        aos_msleep(100);
    }
    
    //krhino_task_dyn_del(NULL);
}

/*modify alios run in sxr task*/
#ifdef RHINO_ONLY
int main (int argc, char *argv[])
{
    sxr_EnterScSchedule(); /* Disable sxr task schedule */

    krhino_init();

    krhino_task_create(&demo1, "demo1_task", NULL,
                       4, 0u, g_demo1_task_stack,
                       200, demo1_task, 1u);

    aos_Timer_Initialise();
   
    //hal_TimSetExpirationTime(16);

    krhino_start();
}
#else
#ifdef SXR_RHINO_DUAL_OS
int rhino_sxs_task()
{
    krhino_init();
    
    krhino_task_create(&demo1, "demo1_task", NULL,
                   4, 0u, g_demo1_task_stack,
                   200, demo1_task, 1u);
    aos_Timer_Initialise();
    
    krhino_start();
}
#else
int rhino_sxs_task()
{
    int count = 0;
    aos_Timer_Initialise();

    test_certificate();
    while (1) {
        hal_DbgTrace((1 << 21), 0, "helloworld! count is %d.\n", count++);
        sxr_Sleep(1000);
    }
}

#endif

#define RHINO_TASK_STACK_SIZE 20*1024
#define RHINO_TASK_PRIORITY   254    //IDLE_TASK_PRIORITY = 255

u8 g_rhino_sxs_taskid;

void sxr_rhino_tick_proc(void *param)
{
    UINT8 mbx = (UINT32)param & 0xff;
    UINT8 task = ((UINT32)param >> 8) & 0xff;

    tick_list_update(1);

    sxr_WakeupTaskFromQueue(task, mbx);
    
    sxr_StartFunctionTimer ((SXR_RHINO_TICK_MS * HAL_TICK1S) / 1000, sxr_rhino_tick_proc,(void *)param, _SXR);
}


const sxr_TaskDesc_t sxs_TaskRhinoDesc =
{
    (void(*)(void *))rhino_sxs_task,
    0,
    "Task rhino.",
    RHINO_TASK_STACK_SIZE,
    RHINO_TASK_PRIORITY
};

int rhino_start()
{
    
    hal_HstSendEvent(0x9999a244);
    g_rhino_sxs_taskid = sxr_NewTask (&sxs_TaskRhinoDesc);
    
    sxr_StartTask (g_rhino_sxs_taskid, NIL);
}

#endif

void soc_err_proc(kstat_t err)
{
    hal_DbgAssert("kernel panic,err %d!\n",err);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

void rhino_to_sxr_sleep(u32 Period)
{
    u32 Status;
    uint8_t cur_cpu_num;

    Status = sxr_EnterSc ();

#ifndef CT_NO_DEBUG
    u32 k1;
    HAL_SYS_GET_K1(k1);
    if (k1 != 0)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1), TSTR("sxr_Sleep() must be run in task context! (k1=0x%08x)\n",0x06ac0004),
                    k1));
    }

    cur_cpu_num = cpu_cur_get();

    /* system is locked so task can not be blocked just return immediately */
    if (g_sched_lock[cur_cpu_num] > 0u) {
        sxr_ExitSc (Status);
        //return RHINO_SCHED_DISABLE;
        hal_DbgAssert("system is locked!\n");
    }

#endif // ! CT_NO_DEBUG

    if (Period != 0)
    {
        // The parameter passed to sxr_ResummeTask() should is a u8. But it is passed
        // through the SXR function timer mechanism, which only passes a void* parameter
        // to the called function. This is why we need the double casting.
        //sxr_StartFunctionTimer (Period, sxr_ResumeTask, (void*)(u32)sxr_Task.Active, _SXR);
        #ifdef RHINO_ONLY
        sxr_StartFunctionTimer (Period, krhino_task_resume, (void*)g_active_task[cur_cpu_num], _SXR);
        #else
        extern kstat_t task_resume_sxr(ktask_t *task);
        sxr_StartFunctionTimer (Period, task_resume_sxr, (void*)g_active_task[cur_cpu_num], _SXR);
        #endif
        
        //sxr_SuspendTask (sxr_Task.Active);
        krhino_task_suspend(g_active_task[cur_cpu_num]);
    }

    sxr_ExitSc (Status);
}

//extern void hal_DbgTrace(uint32_t level, uint32_t tsmap, const char *fmt, ...);

/*
int csp_printf(const char *fmt, ...)
{
    //CPSR_ALLOC();

    va_list args;
    int ret;

    //RHINO_CRITICAL_ENTER();

    va_start(args, fmt);
    hal_DbgTrace((1 << 21), 0, fmt, args);
    //ret = vprintf(fmt, args);
    va_end(args);
    //fflush(stdout);

    //RHINO_CRITICAL_EXIT();

    return ret;
}*/


int printf(const char *fmt, ...)
{
    UINT32 id;

    UINT32 level = 1 << 21;
    UINT32 tsmap = 0;
    
    va_list args;
    id = level | TSMAP(tsmap);
    va_start(args, fmt);
    sxs_vprintf(id, fmt, args);
    va_end(args);
    
    return 0;
}


int csp_printf(const char *fmt, ...)
{
    UINT32 id;

    UINT32 level = 1 << 21;
    UINT32 tsmap = 0;
    
    va_list args;
    id = level | TSMAP(tsmap);
    va_start(args, fmt);
    sxs_vprintf(id, fmt, args);
    va_end(args);
    return 0;
}

int puts(const char *str)
{
    printf("%s",str);
}




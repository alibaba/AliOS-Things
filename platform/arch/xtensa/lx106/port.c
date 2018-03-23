
/* Scheduler includes. */

#include <k_api.h>

#include <xtensa/config/core.h>
#include <xtensa/tie/xt_interrupt.h>
#include <xtensa/tie/xt_timer.h>

#include "frxt/xtensa_rtos.h"
#include "frxt/xtensa_api.h"

#include "c_types.h"
#include "ets_sys.h"

#undef XT_RTOS_INT_EXIT
#define XT_RTOS_INT_EXIT _xt_int_exit

#define XT_STK_EXIT             0x00    /* (offset 0) exit point for dispatch */
#define XT_STK_PC               0x04    /* return address */
#define XT_STK_PS               0x08    /* at level 1 PS.EXCM is set here */
#define XT_STK_A0               0x0C
#define XT_STK_A1               0x10    /* stack ptr before interrupt */
#define XT_STK_A2               0x14
#define XT_STK_A3               0x18
#define XT_STK_A4               0x1C
#define XT_STK_A5               0x20
#define XT_STK_A6               0x24
#define XT_STK_A7               0x28
#define XT_STK_A8               0x2C
#define XT_STK_A9               0x30
#define XT_STK_A10              0x34
#define XT_STK_A11              0x38
#define XT_STK_A12              0x3C    /* Call0 callee-save */
#define XT_STK_A13              0x40    /* Call0 callee-save */
#define XT_STK_A14              0x44    /* Call0 callee-save */
#define XT_STK_A15              0x48    /* Call0 callee-save */
#define XT_STK_SAR              0x4C

extern char NMIIrqIsOn;
static char HdlMacSig = 0;
static char SWReq = 0;
static char PendSvIsPosted = 0;

unsigned cpu_sr;

/* Each task maintains its own interrupt status in the critical nesting
variable. */
static unsigned int uxCriticalNesting = 0;

void vPortEnterCritical( void );
void vPortExitCritical( void );
void debug_stack(int *p);

extern void _xt_user_exit(void);

/*
 * See header file for description.
 */
// int *
void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size,
                          void *arg, task_entry_t entry)
{
	#define SET_STKREG(r,v) sp[(r) >> 2] = (int)(v)
    int *sp, *tp;
    int *pxTopOfStack = (int *)(stack_base + stack_size -1);

    /* Create interrupt stack frame aligned to 16 byte boundary */
    sp = (int*) (((int32_t)(pxTopOfStack+1) - XT_CP_SIZE - XT_STK_FRMSZ) & ~0xf);

    /* Clear the entire frame (do not use memset() because we don't depend on C library) */
    for (tp = sp; tp <= pxTopOfStack; ++tp)
        *tp = 0;

    /* Explicitly initialize certain saved registers */
    SET_STKREG( XT_STK_PC,    entry                 );  /* task entrypoint                  */
    SET_STKREG( XT_STK_A0,    krhino_task_deathbed  );  /* to terminate GDB backtrace       */
    SET_STKREG( XT_STK_A1,    (int32_t)sp + XT_STK_FRMSZ );  /* physical top of stack frame      */
    SET_STKREG( XT_STK_A2,    arg                   );  /* parameters                       */
    SET_STKREG( XT_STK_EXIT,  _xt_user_exit         );  /* user exception exit dispatcher   */

    /* Set initial PS to int level 0, EXCM disabled ('rfe' will enable), user mode. */
    #ifdef __XTENSA_CALL0_ABI__
    SET_STKREG( XT_STK_PS,    PS_UM | PS_EXCM       );
    #else
    /* + for windowed ABI also set WOE and CALLINC (pretend task was 'call4'd). */
    SET_STKREG( XT_STK_PS,    PS_UM | PS_EXCM | PS_WOE | PS_CALLINC(1) );
    #endif

	return sp;
}

void PendSV( char req )
{
	char tmp=0;
//ETS_INTR_LOCK();

	if( NMIIrqIsOn == 0 )
	{
		vPortEnterCritical();
		//PortDisableInt_NoNest();
		tmp = 1;
	}

	if(req ==1)
	{
		SWReq = 1;
	}
	else if(req ==2)
		HdlMacSig= 1;
#if 0
	GPIO_REG_WRITE(GPIO_STATUS_W1TS_ADDRESS, 0x40);	
#else
	if(PendSvIsPosted == 0)
	{
		PendSvIsPosted = 1;
		xthal_set_intset(1<<ETS_SOFT_INUM);
	}
#endif
	if(tmp == 1)
		vPortExitCritical();
}

void cpu_task_switch(void)
{
	PendSV(1);
}

void cpu_intrpt_switch(void)
{
    PendSV(1);
}

void SoftIsrHdl(void *arg)
{
	extern int MacIsrSigPostDefHdl(void);

	PendSvIsPosted = 0;
	int xHigherPriorityTaskWoken=false ;
	if(HdlMacSig == 1)
	{
		xHigherPriorityTaskWoken = MacIsrSigPostDefHdl();
		HdlMacSig = 0;
	}
	if( xHigherPriorityTaskWoken || (SWReq==1)) 
	{
		_xt_timer_int1();
		SWReq = 0;
	}
}

void vTaskSwitchContext(void)
{
	g_active_task[cpu_cur_get()] = g_preferred_ready_task[cpu_cur_get()];
}

void xPortSysTickHandle (void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}

/*
 * See header file for description.
 */
void 
cpu_first_task_start( void )
{
	//set pendsv and systemtick as lowest priority ISR.
	//pendsv setting

	/*******software isr*********/
   	_xt_isr_attach(ETS_SOFT_INUM, SoftIsrHdl, NULL);
    _xt_isr_unmask(1<<ETS_SOFT_INUM);

    /* Initialize system tick timer interrupt and schedule the first tick. */
    _xt_tick_timer_init();

	g_active_task[cpu_cur_get()] = g_preferred_ready_task[cpu_cur_get()];

//    REG_SET_BIT(0x3ff2006c, BIT(4));
	/* Restore the context of the first task that is going to run. */

	XT_RTOS_INT_EXIT();

	/* Should not get here as the tasks are now running! */
}

void 
vPortEndScheduler( void )
{
	/* It is unlikely that the CM3 port will require this function as there
	is nothing to return to.  */
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

static unsigned int tick_lock=0;
static char ClosedLv1Isr = 0;

void vPortEnterCritical( void )
{
	if(NMIIrqIsOn == 0)
	{	
		_espos_enter_critical(NULL);
	}
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	if(NMIIrqIsOn == 0)
	{
		_espos_exit_critical(NULL, 0);
	}
}

void ShowCritical(void)
{
	ets_printf("ShowCritical:%d\n",uxCriticalNesting);
    ets_delay_us(50000);
}

extern uint32 WDEV_INTEREST_EVENT;
#define INT_ENA_WDEV        0x3ff20c18
#define WDEV_TSF0_REACH_INT (BIT(27))
static uint32_t s_nmilock_cnt;
static uint32_t s_nmilock_;

#define ETS_INTR_LOCK_() do {       \
        if (NMIIrqIsOn == 0) {      \
            REG_WRITE(INT_ENA_WDEV, 0); \
            __asm__ __volatile__("rsync":::"memory");       \
            REG_WRITE(INT_ENA_WDEV, WDEV_TSF0_REACH_INT);   \
            __asm__ __volatile__("rsync":::"memory");       \
            vPortEnterCritical();       \
            s_nmilock_cnt++;            \
            __asm__ __volatile__("":::"memory");            \
        }   \
    } while(0)

#define ETS_INTR_UNLOCK_()   do {   \
        if (NMIIrqIsOn == 0) {      \
            __asm__ __volatile__("":::"memory");            \
            if (s_nmilock_cnt > 0) {s_nmilock_cnt--;}       \
            vPortExitCritical();        \
            if  ( s_nmilock_cnt == 0 )  \
            {                           \
                __asm__ __volatile__("rsync":::"memory");       \
                REG_WRITE(INT_ENA_WDEV, WDEV_INTEREST_EVENT);   \
                __asm__ __volatile__("rsync":::"memory");       \
            }                           \
        }   \
    } while(0)

void vPortETSIntrLock( void )
{
    ETS_INTR_LOCK_();
}

void vPortETSIntrUnlock( void )
{
    ETS_INTR_UNLOCK_();
}

void 
PortDisableInt_NoNest( void )
{
//	printf("ERRRRRRR\n");
	if(NMIIrqIsOn == 0)	
	{
		if( ClosedLv1Isr !=1 )
		{
			portDISABLE_INTERRUPTS();
			ClosedLv1Isr = 1;
		}
	}
}

void 
PortEnableInt_NoNest( void )
{
//	printf("ERRRRR\n");

	if(NMIIrqIsOn == 0)
	{		
		if( ClosedLv1Isr ==1 )
		{
			ClosedLv1Isr = 0;
			portENABLE_INTERRUPTS();
		}
	}
}

/*-----------------------------------------------------------*/
void  ResetCcountVal( unsigned int cnt_val )
{
//  XT_WSR_CCOUNT(cnt_val);
    asm volatile("wsr a2, ccount");
}

_xt_isr_entry isr[16];
char _xt_isr_status = 0;

void 
_xt_isr_attach(uint8_t i, _xt_isr func, void *arg)
{
    isr[i].handler = func;
    isr[i].arg = arg;
}

uint16_t _xt_isr_handler(uint16_t i)
{
	uint8_t index;

	if (i & (1 << ETS_WDT_INUM)) {
//		printf("i %x %u\n", i, REG_READ(0x3ff20c00));
		index = ETS_WDT_INUM;
	} 
	else if (i & (1 << ETS_GPIO_INUM)) {
		index = ETS_GPIO_INUM;
	}else {
		index = __builtin_ffs(i) - 1;

		if (index == ETS_MAX_INUM) {
			i &= ~(1 << ETS_MAX_INUM);
			index = __builtin_ffs(i) - 1;
		}
	}

    _xt_clear_ints(1<<index);

	// if (index != ETS_WDT_INUM && index != ETS_SOFT_INUM && index != 10)
	// 	ets_printf("i:%d\n", index);

    espos_isr_enter();

    _xt_isr_status = 1;
    isr[index].handler(isr[index].arg);
    _xt_isr_status = 0;

    espos_isr_exit();

    return i & ~(1 << index);
}

#define PANIC_DUMP_STACK

uint32_t g_double_exc;
char g_panic_title[] = 
    "Register dump, frame at 0x        \r\n";
char g_panic_info[]  = 
    "PC       0x         \n"
    "PS       0x         \n"
    "A0       0x         \n"
    "A1       0x         \n"
    "A2       0x         \n"
    "A3       0x         \n"
    "A4       0x         \n"
    "A5       0x         \n"
    "A6       0x         \n"
    "A7       0x         \n"
    "A8       0x         \n"
    "A9       0x         \n"
    "A10      0x         \n"
    "A11      0x         \n"
    "A12      0x         \n"
    "A13      0x         \n"
    "A14      0x         \n"
    "A15      0x         \n"
    "SAR      0x         \n"
    "EXCCAUSE 0x         \n"
    "EXCVADDR 0x         \n"
    ;
#ifdef PANIC_DUMP_STACK
char g_panic_stack[]  = 
    "stack:   0x         \n"
    "stack:   0x         \n"
    "stack:   0x         \n"
    "stack:   0x         \n"
    ;
#endif

/* itoa, int to ascii */ 
char *itoa_(int num,char *str,int radix) 
{  
	char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
	unsigned unum; 
	int i=0,j,k; 

	if(radix==10&&num<0) 
	{ 
		unum=(unsigned)-num; 
		str[i++]='-'; 
	} 
	else unum=(unsigned)num; 

	do  
	{ 
		str[i++]=index[unum%(unsigned)radix]; 
		unum/=radix; 
	}while(unum); 
	
	//str[i]='\0'; 	

	if(str[0]=='-') k=1;
	else k=0; 
	char temp; 
	for(j=k;j<=(i-k-1)/2.0;j++) 
	{ 
		temp=str[j]; 
		str[j]=str[i-j-1]; 
		str[i-j-1]=temp; 
	} 
	return str; 
} 

void panicHandler(XtExcFrame *frame)
{
    int *regs = (int *)frame;
    int x;

    /* avoid to use printf, because printf depend on malloc, 
       malloc may trig another exception */

    /* NMI can interrupt exception. */
    
    vPortETSIntrLock();
    
    g_double_exc++;    

    if ( g_double_exc == 1 )
    {
        puts("exception occur: ");
        itoa_((uintptr_t)frame, &g_panic_title[26], 16);
        puts(g_panic_title);
        
        for (x = 0; x < 21; x++)
        {
            itoa_(regs[x + 1], &g_panic_info[21*x + 11], 16);
        }
        puts(g_panic_info);
        
#ifdef PANIC_DUMP_STACK
        int y;
        int *sp = (int *)regs[4];
        
        for ( y = 0 ; y < 16 ; y++ )
        {
            for (x = 0; x < 4; x++)
            {
                itoa_(sp[y*4 + x], &g_panic_stack[21*x + 11], 16);
            }
            puts(g_panic_stack);
        }
#endif
    }
    else if  ( g_double_exc == 2 )
    {
        puts("double exception occur!\r\n");
    }

    while (1);
}

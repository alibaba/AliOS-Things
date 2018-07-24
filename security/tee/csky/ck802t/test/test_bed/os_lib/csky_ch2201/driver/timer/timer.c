/* ****************************************************************************
 *                                                                          *
 * C-Sky Microsystems Confidential                                          *
 * -------------------------------                                          *
 * This file and all its contents are properties of C-Sky Microsystems. The *
 * information contained herein is confidential and proprietary and is not  *
 * to be disclosed outside of C-Sky Microsystems except under a             *
 * Non-Disclosured Agreement (NDA).                                         *
 *                                                                          *
 ****************************************************************************
 FILE_NAME           : powm.c
 MODIFY              : lin lin
 FUNCTION            : This file contains the functions support timer operations
 DATE                : 2014/12/11
 ******************************************************************************/
#include "config.h"
#include "datatype.h"
#include "cktimer.h"
#include "timer.h"
#include "intc.h"
#include "powm.h"


/*********************************************CKTimer**********************************/

static CKStruct_TimerInfo CK_Timer_Table[] = {
    { 0, (PCKPStruct_TIMER)CK_TIMER0_BASSADDR, CK_TIMER_IRQ1, FALSE, 0 },
    { 1, (PCKPStruct_TIMER)CK_TIMER1_BASSADDR, CK_TIMER_IRQ2, FALSE, 0 },
#ifdef CK_TIMER2_BASSADDR
    { 2, (PCKPStruct_TIMER)CK_TIMER2_BASSADDR, CK_TIMER_IRQ3, FALSE, 0 },
#endif
#ifdef CK_TIMER3_BASSADDR
    { 3, (PCKPStruct_TIMER)CK_TIMER3_BASSADDR, CK_TIMER_IRQ4, FALSE, 0 }
#endif
};

#define TIMERID_MAX (sizeof(CK_Timer_Table) / sizeof(CKStruct_TimerInfo) - 1)


/*
 * Make all the timers in the idle state;
 * this function should be called before
 * INTC module working;
 */

void CK_Deactive_TimerModule()
{
    int              i;
    PCKPStruct_TIMER ptimer;
    for (i = 0; i < TIMERID_MAX; i++) {
        ptimer = CK_Timer_Table[i].addr;
        /* stop the corresponding timer */
        ptimer->TxControl &= ~CK_TIMER_TXCONTROL_ENABLE;
        /* Disable interrupt. */
        ptimer->TxControl |= CK_TIMER_TXCONTROL_INTMASK;
    }
}

/****************************************************************************/

// static CK_INT32 CK_Timer_PowerManager(
//                IN CK_UINT32 Mode,
//                IN CK_UINT32 ahbfreq,
//                IN CK_UINT32 apbfreq
//                )
//{
//    int i;
//    PCKPStruct_TIMER ptimer;
//    PCKStruct_TIMER_REG timerreg;
//    CK_UINT32 load, timeout;
//    CK_REG *addr;
//    CK_UINT32 k;
//    addr = CK_TIMER0_BASSADDR;
//    for    (i = 0; i < TIMERID_MAX; i++)
//    {
//        ptimer = CK_Timer_Table[i].addr;
//        if (Mode == CK_POWM_PreSTANDBY || Mode == CK_POWM_PreSTOP)
//        {
//            for (k = 0 ; k< sizeof(timerreg) / 4 ; k++)
//                *((CK_UINT32 *)timerreg + k) = *(addr + k);
//        }
//        else if (Mode == CK_POWM_STOP || Mode == CK_POWM_STANDBY)
//        {
//            for (k = 0 ; k< sizeof(timerreg) / 4 ; k++)
//                *(addr + k) = *((CK_UINT32 *)timerreg + k);
//        }
//        else if (Mode == CK_POWM_CLOCK)
//        {
//            load = ptimer->TxLoadCount;
//            timeout = (CK_UINT32)((load * 1000000) / apbfreq);
//            CK_Timer_Start(i, timeout, apbfreq);
//        }
//        else
//            break;
//    }
//
//    return SUCCESS;
//}

/*
 * intilize the timer driver.
 */
void CK_Timer_Init(void)
{
    CK_Deactive_TimerModule();
    //    CK_POWM_RegisterModule(CK_Timer_PowerManager);
}

/*
 * open the timer, register the interrupt.
 *
 * timerid: the mumber of the corresponding timer port;
 * handler: the interrupt service function of the corresponding timer;
 * bfast: indicate whether the fast interrupt ornot;
 * bopened: indicate the state whether be opened ornot
 */
CK_INT32 CK_Timer_Open(CKEnum_Timer_Device timerid,
                       IN void (*handler)(CK_UINT32 irqid),
                       IN CK_UINT16 priority, IN BOOL bfast)
{
    PCKStruct_TimerInfo info;
    PCKPStruct_TIMER    ptimer;

    if ((timerid < 0) || (timerid > TIMERID_MAX)) {
        return FAILURE;
    }
    info   = &(CK_Timer_Table[timerid]);
    ptimer = info->addr;
    if (info->bopened) {
        return FAILURE;
    }
    /* intilize irqhandler */
    if (NULL != handler) {
        info->irqhandler.devname  = "TIMER";
        info->irqhandler.irqid    = info->irq;
        info->irqhandler.priority = priority % 4;
        info->irqhandler.handler  = handler;
        /* register timer isr */
        CK_INTC_RequestIrq(&(info->irqhandler));
    }
    info->bopened = TRUE;
    /* Enable Timer interrupt. */
    ptimer->TxControl &= ~(CK_TIMER_TXCONTROL_INTMASK);
    return SUCCESS;
}

/*
 * close the timer, free interrupt.
 *
 * timerid: the mumber of the corresponding timer port;
 * bopened: indicate the state whether be opened ornot
 */
CK_INT32 CK_Timer_Close(CKEnum_Timer_Device timerid)
{
    PCKStruct_TimerInfo info;
    PCKPStruct_TIMER    ptimer;

    if ((timerid < 0) || (timerid > TIMERID_MAX)) {
        return FAILURE;
    }
    info   = &(CK_Timer_Table[timerid]);
    ptimer = info->addr;
    if (!(info->bopened)) {
        return FAILURE;
    }

    /* stop the corresponding timer */
    ptimer->TxControl &= ~CK_TIMER_TXCONTROL_ENABLE;
    /* Disable interrupt. */
    ptimer->TxControl |= CK_TIMER_TXCONTROL_INTMASK;
    /*clear the backcall function*/
    CK_INTC_FreeIrq(&(info->irqhandler));
    info->bopened = FALSE;
    return SUCCESS;
}


/*
 * start the corresponding timer
 *
 * timerid: the mumber of the corresponding timer port;
 * timeout: the set time (uS)
 */
CK_INT32 CK_Timer_Start(CKEnum_Timer_Device timerid, IN CK_UINT32 timeout,
                        CK_UINT32 apbfreq)
{
    CK_UINT32           load;
    PCKStruct_TimerInfo info;
    PCKPStruct_TIMER    ptimer;

    if ((timerid < 0) || (timerid > TIMERID_MAX)) {
        return FAILURE;
    }

    info   = &(CK_Timer_Table[timerid]);
    ptimer = info->addr;
    if (!(info->bopened)) {
        return FAILURE;
    }

    load = (CK_UINT32)((apbfreq / 1000000) * timeout);
    /*  load time(us)  */
    ptimer->TxLoadCount = load;
    info->timeout       = timeout;
    /*in user-defined running mode, if use design ware timer, you can dynamic
     * cha     nge the user mode and free mode when you enable the timer*/
    ptimer->TxControl |= CK_TIMER_TXCONTROL_MODE;
    /* enable the corresponding timer */
    ptimer->TxControl |= CK_TIMER_TXCONTROL_ENABLE;
    /*in user-defined running mode*/
    //  ptimer->TxControl |= CK_TIMER_TXCONTROL_MODE;
    return SUCCESS;
}

/*
 * stop a designated timer
 *
 * timerid: the mumber of the corresponding timer port;
 * stop_val: the count value when the timer stops
 */
CK_UINT32 CK_Timer_Stop(CKEnum_Timer_Device timerid)
{
    CK_UINT32           stop_val;
    PCKStruct_TimerInfo info;
    PCKPStruct_TIMER    ptimer;

    /* if the timer does not open,return failure */
    info = &(CK_Timer_Table[timerid]);
    if (!(info->bopened)) {
        return FAILURE;
    }

    ptimer = info->addr;
    /* disable the timer*/
    ptimer->TxControl &= ~CK_TIMER_TXCONTROL_ENABLE;
    stop_val = ptimer->TxCurrentValue;
    return stop_val;
}

/*
 * clear a timer interrupt
 * by reading its End of Interrupt register(EOI)
 */
void CK_Timer_ClearIrqFlag(CKEnum_Timer_Device timerid)
{
    PCKStruct_TimerInfo info;
    PCKPStruct_TIMER    ptimer;

    info   = &(CK_Timer_Table[timerid]);
    ptimer = info->addr;
    *((volatile CK_UINT32 *)(&(ptimer->TxEOI)));
}

/*
 * read the current value of the timer
 *
 * timerid: the mumber of the corresponding timer;
 * current_val: the current count-value
 */
CK_UINT32 CK_Timer_CurrentValue(CKEnum_Timer_Device timerid)
{
    CK_UINT32           current_val;
    PCKStruct_TimerInfo info;
    PCKPStruct_TIMER    ptimer;

    info        = &(CK_Timer_Table[timerid]);
    ptimer      = info->addr;
    current_val = ptimer->TxCurrentValue;
    return current_val;
}

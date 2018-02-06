/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef CKTIMER_H_
#define CKTIMER_H_

#include "config.h"
#include "datatype.h"
#include "intc.h"

/*
 * define number of the timer interrupt
 */
typedef struct CKS_TIMER
{
        CK_REG    TxLoadCount;     /*  Timer Value to be loaded into Timer */
        CK_REG    TxCurrentValue;  /*  Timer Current Value of Timer */
        CK_REG    TxControl;       /* Timer Control Register for Timer */
        CK_REG    TxEOI;       /* Timer Clears the interrupt from Timer */
        CK_REG    TxIntStatus;     /* Timer0 Contains the interrupt status for Timer*/

} CKStruct_TIMER,* PCKPStruct_TIMER;
typedef struct {
        CK_UINT32 id;              /* the number of timer */
        PCKPStruct_TIMER  addr;    /* the base-address of timer */
        CK_UINT32 irq;             /* the interruput mumber of timer */
        BOOL      bopened;         /* indicate whether have been opened or not */
        CK_UINT32 timeout;         /* the set time (us) */
        CKStruct_IRQHandler irqhandler;

} CKStruct_TimerInfo, * PCKStruct_TimerInfo;

/*
 *  define the bits for TxControl
 */
#define CK_TIMER_TXCONTROL_ENABLE      (1UL << 0)
#define CK_TIMER_TXCONTROL_MODE        (1UL << 1)
#define CK_TIMER_TXCONTROL_INTMASK     (1UL << 2)

#define  PCK_TIMER_CONTROL  ((PCKStruct_TIMER_CON)CK_TIMER_CONTROL_BASSADDR)

typedef struct CKS_TIMER_CON
{
        CK_REG    IntStatus;       /*  Contains the interrupt status of all
                                *  timers in the component.
                                */
        CK_REG    EOI;           /*  Returns all zeroes (0) and clears all
                                *  active interrupts.
                                */
        CK_REG    RawIntStatus;       /*  Contains the unmasked interrupt status
                                *  of all timers in the component.
                                */

} CKStruct_TIMER_CON,* PCKStruct_TIMER_CON;

typedef struct CKS_TIMER_REG
{
        CK_REG    T1LoadCount;     /*  Timer Value to be loaded into Timer */
        CK_REG    T1CurrentValue;  /*  Timer Current Value of Timer */
        CK_REG    T1Control;       /* Timer Control Register for Timer */
        CK_REG    T1EOI;       /* Timer Clears the interrupt from Timer */
        CK_REG    T1IntStatus;     /* Timer0 Contains the interrupt status for Timer*/
        CK_REG    T2LoadCount;     /*  Timer Value to be loaded into Timer */
        CK_REG    T2CurrentValue;  /*  Timer Current Value of Timer */
        CK_REG    T2Control;       /* Timer Control Register for Timer */
        CK_REG    T2EOI;       /* Timer Clears the interrupt from Timer */
        CK_REG    T2IntStatus;     /* Timer0 Contains the interrupt status for Timer*/
        CK_REG    REV[30];
        CK_REG    IntStatus;       /*  Contains the interrupt status of all
                                *  timers in the component.
                                */
        CK_REG    EOI;           /*  Returns all zeroes (0) and clears all
                                *  active interrupts.
                                */
        CK_REG    RawIntStatus;       /*  Contains the unmasked interrupt status
                                *  of all timers in the component.
                                */
} CKStruct_TIMER_REG,* PCKStruct_TIMER_REG;




#endif /* CKTIMER_H_ */

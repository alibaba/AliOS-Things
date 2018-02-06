/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "cksmart.h"

#include "datatype.h"
#include "intc.h"
#include "ckintc.h"

/*
 * NR_IRQS: The number of member in normal_irq_list and fast_irq_list
 *          respective.It is equal to the number of priority levels.
 */
#define NR_IRQS  32

/*
 * normal_irq_list[NR_IRQS]: All the normal interrupt service routines should
 *                           been registered in this list.
 *
 * fast_irq_list[NR_IRQS]: All the fast interrupt service routines should been
 *                         registered in this list.
 */
static PCKStruct_IRQHandler normal_irq_list[NR_IRQS];

/* PCK_INTC: The base address of interrupt controller registers */
volatile CKStruct_INTC *icrp = PCK_INTC;

#if 0
static char *exception_names[] =
{
    "RESET PC", "BUS ERROR", "ADDRESS ERROR", "ZERO DIVIDE",
    "ILLEGAL INSTRUCTION",  "PRIVILEGE VIOLATION",
    "TRACE", "BREAK POIOT ERROR", "FATAL ERROR" , "Idly4 ERROR",
    "" , "" ,  "UNASSIGNED RESERVED HAI", "UNASSIGNED RESERVED FP",
    "" , "" , "TRAP #0" , "TRAP #1", "TRAP #2", "TRAP #3",
    "UNASSIGNED RESERVED 20", "UNASSIGNED RESERVED 21",
    "UNASSIGNED RESERVED 22", "UNASSIGNED RESERVED 23",
    "UNASSIGNED RESERVED 24", "UNASSIGNED RESERVED 25",
    "UNASSIGNED RESERVED 26", "UNASSIGNED RESERVED 27",
    "UNASSIGNED RESERVED 28", "UNASSIGNED RESERVED 29",
    "UNASSIGNED RESERVED 30",  "SYSTEM DESCRIPTION POINT",
};
#endif

/*
 * CKCORE_SAVED_CONTEXT -- Saved by a normal interrput or exception
 */
typedef struct
{
    CK_UINT32 pc;
    CK_UINT32 psr;
    CK_UINT32 r1;
    CK_UINT32 r2;
    CK_UINT32 r3;
    CK_UINT32 r4;
    CK_UINT32 r5;
    CK_UINT32 r6;
    CK_UINT32 r7;
    CK_UINT32 r8;
    CK_UINT32 r9;
    CK_UINT32 r10;
    CK_UINT32 r11;
    CK_UINT32 r12;
    CK_UINT32 r13;
    CK_UINT32 r14;
    CK_UINT32 r15;
} __attribute__ ((aligned, packed)) Ckcore_SavedRegisters;

extern void hw_vsr_default_exception();
extern void hw_vsr_autovec();

/***************************************************************************
This function initializes normal_irq_list[NR_IRQS], fast_irq_list[NR_IRQS]
and PR0-PR31.

***************************************************************************/
void CK_INTC_Init(void) {
    int i;
    for (i = 0; i < NR_IRQS; i++) {
        normal_irq_list[i] = NULL;
    }

    /* initialize PR0-PR31, big endian */
    icrp->IPR[0] = 0x00000000;
    icrp->IPR[1] = 0x00000000;
    icrp->IPR[2] = 0x00000000;
    icrp->IPR[3] = 0x00000000;
    icrp->IPR[4] = 0x00000000;
    icrp->IPR[5] = 0x00000000;
    icrp->IPR[6] = 0x00000000;
    icrp->IPR[7] = 0x00000000;
}

/******************************************************************
This function enables a priority level of normal interrupt.

priotity: A priority of normal interrupt which between 0 to 31.

*******************************************************************/
void CK_INTC_EnNormalIrq( IN CK_UINT32 irq_num )
{
  CK_UINT32 psrbk;

  CK_CPU_EnterCritical(&psrbk);
  icrp->ISER |= (1 << irq_num);
  CK_CPU_ExitCritical(psrbk);
}

/************************************************************************
This function disables a priority level of normal interrupt.

priotity: A priority of normal interrupt which between 0 to 31.

**********************************************************************/
void CK_INTC_DisNormalIrq(IN CK_UINT32 irq_num)
{
  CK_UINT32 psrbk;

  CK_CPU_EnterCritical(&psrbk);
  icrp->ISER &= ~(1 << irq_num);
  CK_CPU_ExitCritical(psrbk);
}


/**********************************************************************
This function is used for registering interrupt functions

pirqhandler: The pointer pointing the interrupt data struct which would be
             registerd.

********************************************************************/
CK_INT32 CK_INTC_RequestIrq(PCKStruct_IRQHandler pirqhandler) {
    CK_UINT32 pr_index;
    CK_UINT32 shift;
    CK_UINT32 psrbk;

    /* Judge the validity of pirqhandler */
    /* priority: 0~3,  irq number: 0~31*/
    if ((NULL == pirqhandler) || (pirqhandler->handler == NULL)
            || (pirqhandler->priority < 0 || pirqhandler->priority > (CK_INTC_PRIO_LEVEL - 1))
            || ((pirqhandler->irqid < 0) || pirqhandler->irqid > (CK_INTC_COUNT - 1))) {
        return FAILURE;
    }

    /* Assigns pirqhandler->priority to corresponding interrupt source */
    pr_index = (pirqhandler->irqid) / 4;
    shift = (3 - (pirqhandler->irqid) % 4) * 8;

    CK_CPU_EnterCritical(&psrbk);
    icrp->IPR[pr_index] &= ~(0x000000ff << shift);

    /* priority: 0~3, bit 7~6 of IPR is valid */
    icrp->IPR[pr_index] |= (((pirqhandler->priority) << 6) << shift);

    /* If is normal interrupt */
    /* If the list of this priority is empty */
    if (NULL == (normal_irq_list[pirqhandler->irqid])) {
        normal_irq_list[pirqhandler->irqid] = pirqhandler;
        CK_INTC_EnNormalIrq(pirqhandler->irqid);
    } else {
        /* re register */
        return FAILURE;
    }

    CK_CPU_ExitCritical(psrbk);
    return SUCCESS;
}



/***************************************************************************
This function is used for freeing those functions which have been register

pirqhandler: The pointer pointing to the interrupt service function which
             has been register.

**************************************************************************/
CK_INT32 CK_INTC_FreeIrq(INOUT PCKStruct_IRQHandler pirqhandler) {
    /* Judge the validity of pirqhandler */
    if ((pirqhandler == NULL) || (pirqhandler->handler == NULL)
            || ((pirqhandler->irqid < 0) || (pirqhandler->irqid > 31))) {
        return FAILURE;
    }

    if (NULL == normal_irq_list[pirqhandler->irqid]) {
        return FAILURE;
    }
    normal_irq_list[pirqhandler->irqid] = NULL;

    CK_INTC_DisNormalIrq(pirqhandler->irqid);

    return SUCCESS;
}

/******************************************************************
CK_Default_Exception_Handler -- The default exception handler

INPUT:
vector : the exception vector number

regs : the registers that have restored

RETURN VALUE: None

******************************************************************/

void CK_Default_Exception_Handler(int vector, Ckcore_SavedRegisters *regs)
{
    //printk("Exception: %s\n", exception_names[vector]);
}

/*********************************************************
CK_Exception_Init -- Initialize the vector table

INPUT: None

RETURN VALUE: None

*********************************************************/

void CK_Exception_Init(void) {
    int i;

    // set all exception vector table as hw_vsr_default_exception
    for (i = 0; i < CKCORE_VECTOR_SYS; i++) {
        ckcpu_vsr_table[i] = (CK_UINT32) hw_vsr_default_exception;
    }

    for (i = 0; i < CK_INTC_COUNT; i++) {
        ckcpu_vsr_table[i + CKCORE_VECTOR_SYS] =
                (CK_UINT32) hw_vsr_autovec;
    }

    CK_CPU_EnAllNormalIrq();
}

/*********************************************************
CK_INTC_InterruptService -- Execute the interrupt service,
called by "Inthandler"

INPUT:

offset : the offset in  the array normal_irq_list

RETURN VALUE: None

*********************************************************/

void CK_INTC_InterruptService(int offset) {
    PCKStruct_IRQHandler phandler;

    phandler = normal_irq_list[offset];
    if (phandler != NULL) {
        /* phandler -> handler must not be NULL */
        phandler -> handler(offset);
    }
}


/*********************************************************
CK_CPU_EnAllNormalIrq -- Config cpu to enable all normal
interrupt

INPUT: None

RETURN VALUE: None

*********************************************************/

void CK_CPU_EnAllNormalIrq(void)
{
  asm  ("psrset ee,ie");
}

/*********************************************************
CK_CPU_DisAllNormalIrq -- Config cpu to disable all normal
interrupt

INPUT: None

RETURN VALUE: None

*********************************************************/

void CK_CPU_DisAllNormalIrq(void)
{
 asm  ("psrclr ie");
}


/*********************************************************
CK_CPU_EnterCritical -- This function should be called
before executing critical code which should not be
interrupted by other interrupts


INPUT: psr, to store the value of psr

RETURN VALUE: None

*********************************************************/

void CK_CPU_EnterCritical(CK_UINT32 *psr)
{
  asm volatile ("mfcr    %0, psr\n\r"
                "psrclr  ie, fe"
                 : "=r" (*psr) );
}

/*********************************************************
CK_CPU_ExitCritical -- This function should be called
after exiting critical area.

INPUT: psr, contain the backup value of psr

RETURN VALUE: None

*********************************************************/

void CK_CPU_ExitCritical(CK_UINT32 psr)
{
  asm volatile ("mtcr   %0, psr"
                 :
                 :"r"(psr));
}

/*************************************************************
* @name:   NVIC_PowerWakeUp_Enable
* @brief:  enable the bit for Power wake up
* @param:  irqn    the irqnumber,eg:INTC_CORETIM_IRQn
*/
__INLINE void VIC_PowerWakeUp_Enable(IRQn_Type irqn)
{
    icrp->IWER |= (1 << irqn);
}

/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _INTR_H_
#define _INTR_H_

#include "datatype.h"
#include "intc.h"

#define CKCORE_VECTOR_SYS  32
#define CK_INTC_COUNT      32
#define CK_INTC_PRIO_LEVEL 4

#define CKCORE_VECTOR_WSC   	12
// VSR table
extern  volatile unsigned int ckcpu_vsr_table[64];

/* define the data structure of interrupt description */
typedef struct CKS_IRQ_Handler{
      char        *devname;
      CK_UINT32   irqid;
      CK_UINT32    priority;
      void        (*handler)(CK_UINT32 irqid);
      BOOL        bfast;
      struct CKS_IRQ_Handler  *next;

}CKStruct_IRQHandler, *PCKStruct_IRQHandler;

/* Statement of those functions which are used in intc.c*/
void CK_CPU_EnterCritical(CK_UINT32 *psr);
void CK_CPU_ExitCritical(CK_UINT32 psr);
void CK_INTC_EnNormalIrq(IN CK_UINT32 priority);
void CK_INTC_DisNormalIrq(IN CK_UINT32 priority);
CK_INT32 CK_INTC_RequestIrq(PCKStruct_IRQHandler priqhandler);
CK_INT32 CK_INTC_FreeIrq(INOUT PCKStruct_IRQHandler priqhandler);
void CK_CPU_EnAllNormalIrq(void);
void CK_CPU_DisAllNormalIrq(void);

//CK_INT32 CK_INTC_RequestIrq(PCKStruct_IRQHandler priqhandler);
//CK_INT32 CK_INTC_FreeIrq(INOUT PCKStruct_IRQHandler priqhandler);

/* save context */
#define IRQ_HANDLER_START()  \
asm (  \
    "subi    sp, 28\n\t"   \
    "subi    sp, 8\n\t"   \
    "stw     a0, (sp, 0)\n\t"   \
    "stw     a1, (sp, 4)\n\t"   \
    "stw     a2, (sp, 8)\n\t"   \
    "stw     a3, (sp, 12)\n\t"   \
    "stw     t0, (sp, 16)\n\t"   \
    "stw     t1, (sp, 20)\n\t"   \
    "stw     l0, (sp, 24)\n\t"   \
    "stw     lr, (sp, 28)\n\t"   \
\
    "subi    sp, 8\n\t"   \
    "mfcr    a3, epsr\n\t"   \
    "stw     a3, (sp, 4)\n\t"   \
    "mfcr    a2, epc\n\t"   \
    "stw     a2, (sp, 0)\n\t"   \
\
    "subi    sp, 8\n\t"   \
    "mov     l0, sp\n\t"   \
)

/*   Restore the psr and pc     */
#define IRQ_HANDLER_END()   \
asm (   \
    "addi    sp, 8\n\t"      \
    "ldw     a3, (sp, 0)\n\t"  \
    "mtcr    a3, epc\n\t"  \
    "ldw     a2, (sp, 4)\n\t"   \
    "mtcr    a2, epsr\n\t"   \
    "addi    sp, 8\n\t"   \
\
    "ldw     a0, (sp, 0)\n\t"   \
    "ldw     a1, (sp, 4)\n\t"   \
    "ldw     a2, (sp, 8)\n\t"   \
    "ldw     a3, (sp, 12)\n\t"   \
    "ldw     t0, (sp, 16)\n\t"   \
    "ldw     t1, (sp, 20)\n\t"   \
    "ldw     l0, (sp, 24)\n\t"   \
    "ldw     lr, (sp, 28)\n\t"   \
    "addi    sp, 28\n\t"   \
    "addi    sp, 8\n\t"      \
\
    "rte"   \
)


/*
 * return the irq ID
 */
#define IRQ_ID()  \
({  \
    int  irqid;   \
    asm ( "mfcr        %0, psr\n\t"   \
          "lsri        %0, 16\n\t"   \
          "sextb    %0\n\t"   \
          : "=&r" (irqid)  \
          :   \
    ); \
    irqid; \
})

#endif

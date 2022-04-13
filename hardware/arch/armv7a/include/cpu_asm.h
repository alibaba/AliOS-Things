#ifndef __CPU_ASM_H
#define __CPU_ASM_H

#define ARM_EXCEPT_IRQ   0x06

/* When I bit is set, IRQ is disabled */
#define I_Bit       0x80

/* When F bit is set, FIQ is disabled */
#define F_Bit       0x40

/* Define core program mode */
#define MODE_MSK          0x1f
#define ARM_MODE_USR      0x10
#define ARM_MODE_FIQ      0x11
#define ARM_MODE_IRQ      0x12
#define ARM_MODE_SVC      0x13
#define ARM_MODE_ABT      0x17
#define ARM_MODE_UND      0x1b
#define ARM_MODE_SYS      0x1f
#define ARM_MODE_MON      0x16

/* Define bits of CPSR */
#define  ARMV7_THUMB_MASK     (1<<5)
#define  ARMV7_FIQ_MASK       (1<<6)
#define  ARMV7_IRQ_MASK       (1<<7)
#define  ARMV7_ABT_MASK       (1<<8)
#define  ARMV7_END_MASK       (1<<9)

#define  ARMV7_IT_MASK        ((0x3f<<10) || (0x03<<25))
#define  ARMV7_GE_MASK        (0x0f<<16)
#define  ARMV7_JAVA_MASK      (1<<24)

#define  ARMV7_QFLAG_BIT      (1 << 27)
#define  ARMV7_CC_V_BIT       (1 << 28)
#define  ARMV7_CC_C_BIT       (1 << 29)
#define  ARMV7_CC_Z_BIT       (1 << 30)
#define  ARMV7_CC_N_BIT       (1 << 31)
#define  NS_BIT               (0x01)

/* Config stack address for every program mode */
#define user_stack_len      (0x100)
#define fiq_stack_len       (0x100)
#define mon_stack_len       (0x100)
#define abt_stack_len       (0x200)
#define und_stack_len       (0x200)
#define svc_stack_len       (0x1000)
#define irq_stack_len       (0x1000)

#endif



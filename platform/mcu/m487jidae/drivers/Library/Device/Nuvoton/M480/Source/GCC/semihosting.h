/**************************************************************************//**
 * @file     semihosting.h
 * @version  V1.00
 * @brief    semihosting header file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#ifndef __SEMIHOSTING_H__
#define __SEMIHOSTING_H__

#define SEMIHOSTING_SYS_OPEN  0x01

// SWI numbers and reason codes for RDI (Angel) monitors.
#define AngelSWI_ARM                    0x123456
#ifdef __thumb__
#define AngelSWI                        0xAB
#else
#define AngelSWI                        AngelSWI_ARM
#endif

#define AngelSWIInsn                    "swi"
#define AngelSWIAsm                     swi

// Testing the local semihosting handler cannot use another BKPT, since this
// configuration cannot trigger HaedFault exceptions while the debugger is
// connected, so we use an illegal op code, that will trigger an
// UsageFault exception.
#define AngelSWITestFault       "setend be"
#define AngelSWITestFaultOpCode (0xB658)

static inline int
__attribute__ ((always_inline))
call_host (int reason, void* arg)
{
    int value;
    asm volatile (

        " mov r0, %[rsn]  \n"
        " mov r1, %[arg]  \n"
        " " AngelSWITestFault " \n"
        " mov %[val], r0"

        : [val] "=r" (value) /* Outputs */
        : [rsn] "r" (reason), [arg] "r" (arg), [swi] "i" (AngelSWI) /* Inputs */
        : "r0", "r1", "r2", "r3", "ip", "lr", "memory", "cc"
        // Clobbers r0 and r1, and lr if in supervisor mode
    );

    // Accordingly to page 13-77 of ARM DUI 0040D other registers
    // can also be clobbered. Some memory positions may also be
    // changed by a system call, so they should not be kept in
    // registers. Note: we are assuming the manual is right and
    // Angel is respecting the APCS.
    return value;
}


#endif // __SEMIHOSTING_H__

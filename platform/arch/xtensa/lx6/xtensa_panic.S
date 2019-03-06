#include "xtensa_rtos.h"

    .global panicHandler, port_IntStackTop

    .global     _xt_ext_panic
    .type       _xt_ext_panic, @function
    .align      4
    .literal_position
_xt_ext_panic:
    /* Allocate exception frame and save minimal context. */
    movi    a0, port_IntStackTop
    sub     a0, a0, sp
    srli    a0, a0, 8
    bgeui   a0, (configISR_STACK_SIZE*portNUM_PROCESSORS)>>8, _switch_sp
    mov     a0, sp
    j       _dump
_switch_sp:
    mov     a0, sp
    movi    sp, port_IntStackTop
_dump:
    addi    sp, sp, -XT_STK_FRMSZ
    s32i    a0, sp, XT_STK_A1
    rsr     a0, PS                          /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_1                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    s32i    a12, sp, XT_STK_A12             /* _xt_context_save requires A12- */
    s32i    a13, sp, XT_STK_A13             /* A13 to have already been saved */
    call0   _xt_context_save

    /* Save exc cause and vaddr into exception frame */
    rsr     a0, EXCCAUSE
    s32i    a0, sp, XT_STK_EXCCAUSE
    rsr     a0, EXCVADDR
    s32i    a0, sp, XT_STK_EXCVADDR

    /* _xt_context_save seems to save the current a0, but we need the interuptees a0. Fix this. */
    rsr     a0, EXCSAVE_1                   /* save interruptee's a0 */

    s32i    a0, sp, XT_STK_A0

    /* Set up PS for C, disable all interrupts except NMI and debug, and clear EXCM. */
    movi    a0, PS_INTLEVEL(5) | PS_UM | PS_WOE
    wsr     a0, PS

    //Call panic handler
    mov     a6, sp
    call4   xtensaPanic

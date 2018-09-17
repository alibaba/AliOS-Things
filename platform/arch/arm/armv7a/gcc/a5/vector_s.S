#include <k_config.h>
#if (RHINO_CONFIG_SYSTEM_STACK_SIZE == 0)
#error "RHINO_CONFIG_SYSTEM_STACK_SIZE must be set in k_config.h!"
#endif

@******************************************************************************
@                            EXTERN SYMBOLS
@******************************************************************************
.extern __bss_start @ defined in *.ld 
.extern __bss_end   @ defined in *.ld 
.extern sys_start
.extern _interrupt_handler

@******************************************************************************
@                            EXPORT SYMBOLS
@******************************************************************************
.global _vector_table
.global mode_stack_base
.global mode_stack_top
.global sys_stack_base
.global sys_stack_top

@******************************************************************************
@                               EQUATES
@******************************************************************************
@ Bits in CPSR (Current Program Status Register) 
.equ CPSR_Mode_USR,             0x10
.equ CPSR_Mode_FIQ,             0x11
.equ CPSR_Mode_IRQ,             0x12
.equ CPSR_Mode_SVC,             0x13
.equ CPSR_Mode_ABT,             0x17
.equ CPSR_Mode_UND,             0x1B
.equ CPSR_Mode_SYS,             0x1F

.equ CPSR_FIQ_DIS,              0x40           @ Disable FIQ. 
.equ CPSR_IRQ_DIS,              0x80           @ Disable IRQ. 
.equ CPSR_INT_DIS,              CPSR_FIQ_DIS | CPSR_IRQ_DIS

@ Stack size for all modes 
.equ UND_Stack_Size,            0x20
.equ ABT_Stack_Size,            0x20
.equ FIQ_Stack_Size,            0x20
.equ IRQ_Stack_Size,            0x20
.equ MODE_Stack_Size,           UND_Stack_Size + ABT_Stack_Size + FIQ_Stack_Size + IRQ_Stack_Size

@ Stack size for ISR & Fault & Reset
.equ SYS_Stack_Size,            RHINO_CONFIG_SYSTEM_STACK_SIZE

@ bits in SCTLR (System Control Register) 
.equ SCTLR_M,                   0x0001
.equ SCTLR_C,                   0x0004
.equ SCTLR_I,                   0x1000

@ Exception type 
.equ ARM_EXCEPT_RESET,          0x00
.equ ARM_EXCEPT_UNDEF_INSTR,    0x01
.equ ARM_EXCEPT_SWI,            0x02
.equ ARM_EXCEPT_PREFETCH_ABORT, 0x03
.equ ARM_EXCEPT_DATA_ABORT,     0x04
.equ ARM_EXCEPT_RESERVED,       0x05
.equ ARM_EXCEPT_IRQ,            0x06
.equ ARM_EXCEPT_FIQ,            0x07

@******************************************************************************
@                          STACK REGION DEFINATION
@******************************************************************************
.section .bss
.align 3
mode_stack_base:
.fill MODE_Stack_Size
mode_stack_top:

.section .bss.isr
.align 3
sys_stack_base:
.fill SYS_Stack_Size
sys_stack_top:

@******************************************************************************
@                           vector table
@******************************************************************************
.section ".vectors", "ax"
.arm

_vector_table:
    ldr pc, _reset
    ldr pc, _undef
    ldr pc, _swi
    ldr pc, _pabt
    ldr pc, _dabt
    ldr pc, _resv
    ldr pc, _irq
    ldr pc, _fiq

_reset: .word vector_reset
_undef: .word vector_undef
_swi:   .word vector_swi
_pabt:  .word vector_pabt
_dabt:  .word vector_dabt
_resv:  .word vector_resv
_irq:   .word vector_irq
_fiq:   .word vector_fiq

@ magicword 
.balignl 16, 0xe7f000f0

@******************************************************************************
@                           vectors function
@******************************************************************************
.section .text.isr, "ax"
.arm
@ reset entry 
vector_reset:
    @ Disable MMU & Cache 
    MRC     P15, 0, R0, C1, C0, 0               @ Read SCTRL 
    BIC	    R0, R0, #SCTLR_M                    @ Disable MMU 
    BIC	    R0, R0, #SCTLR_C                    @ Disable data cache 
    BIC	    R0, R0, #SCTLR_I                    @ Disable instruction cache 
    MCR     P15, 0, R0, C1, C0, 0               @ Write SCTRL 

    @ Invalidate all cache 
    MOV     R0, #0X0
    MCR     P15, 0, R0, C7, C5, 6               @ Invalidate entire branch predictor array. 
    MOV     R0, #0X0
    MCR     P15, 0, R0, C8, C7, 0               @ Invalidate entire Unified TLB 
    MOV     R0, #0
    MCR     P15, 0, R0, C7, C5, 0               @ Invalidate all instruction caches to PoU.
                                                @ Also flushes branch target cache. 
    @ Setup Stack for each mode 
    LDR     R0, = mode_stack_top
    MSR     CPSR_c, #CPSR_Mode_UND | CPSR_INT_DIS
    MOV     SP, R0
    SUB     R0, R0, #UND_Stack_Size

    MSR     CPSR_c, #CPSR_Mode_ABT | CPSR_INT_DIS
    MOV     SP, R0
    SUB     R0, R0, #ABT_Stack_Size

    MSR     CPSR_c, #CPSR_Mode_FIQ | CPSR_INT_DIS
    MOV     SP, R0
    SUB     R0, R0, #FIQ_Stack_Size

    MSR     CPSR_c, #CPSR_Mode_IRQ | CPSR_INT_DIS
    MOV     SP, R0
    SUB     R0, R0, #IRQ_Stack_Size

    @ Setup Stack for SVC
    LDR     R0, = sys_stack_top
    MSR     CPSR_c, #CPSR_Mode_SVC | CPSR_INT_DIS
    MOV     SP, R0

    @ clear .bss
    MOV     R0,#0                               @ get a zero
    LDR     R1,=__bss_start                     @ bss start
    LDR     R2,=__bss_end                       @ bss end
bss_loop:
    CMP     R1,R2                               @ check if data to clear
    STRLO   R0,[R1],#4                          @ clear 4 bytes
    BLO     bss_loop                            @ loop until done

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    @ Enabling VFP support
    MRC     P15, 0, R0, C1, C1, 2               @ Read NSACR, Non-Secure Access Control Register
    ORR     R0, R0, #3<<10                      @ enable Non-secure access to fpu 
    MCR     P15, 0, R0, C1, C1, 2               @ Write NSACR

    MRC     P15, 0, R0, C1, C0, 2               @ Read CPACR, Non-Secure Access Control Register
    ORR     R0, R0, #0XF<<20                    @ Set access permission for VFP
    MCR     P15, 0, R0, C1, C0, 2               @ Write CPACR
    ISB

    MOV     R0, #0X40000000
    VMSR    FPEXC, R0                           @ Set the FPEXC.EN bit to enable the FPU 
    #endif

    @ start Kernel 
    LDR     PC, entry_main
    B       .                                   @ never reach here
entry_main:
    .word   sys_start

@ exception handlers: undef, swi, padt, dabt, resv, irq, fiq 
vector_undef:
    STMFD   SP!, {R0-R3}                        @ Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_UNDEF_INSTR         @ Set exception type to ARM_EXCEPT_UNDEF_INSTR.
    MRS     R1, SPSR                            @ Save CPSR
    SUB     R2, LR, #4                          @ Save LR(PC) register: -4.
    MOV     R3, SP                              @ Save SP register.
    ADD     SP, SP, #(4 * 4)                    @ set SP to undef stack top.
    B       _panic_handler

vector_swi:
    UDF     0                                   @ reserved for Supervisor Call

vector_pabt:
    STMFD   SP!, {R0-R3}                        @ Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_PREFETCH_ABORT      @ Set exception type to ARM_EXCEPT_PREFETCH_ABORT.
    MRS     R1, SPSR                            @ Save CPSR.
    SUB     R2, LR, #4                          @ Save LR(PC) register: -4.
    MOV     R3, SP                              @ Save SP register.
    ADD     SP, SP, #(4 * 4)                    @ set SP to padt stack top.
    B       _panic_handler

vector_dabt:
    STMFD   SP!, {R0-R3}                        @ Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_DATA_ABORT          @ Set exception type to ARM_EXCEPT_DATA_ABORT.
    MRS     R1, SPSR                            @ Save CPSR.
    SUB     R2, LR, #8                          @ Save LR(PC) register: -8.
    MOV     R3, SP                              @ Save SP register.
    ADD     SP, SP, #(4 * 4)                    @ set SP to dabt stack top.
    B       _panic_handler

vector_resv:
    UDF     0                                   @ reserved for Hyp Trap

vector_irq:
    STMFD   SP!, {R0-R3}                        @ Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_IRQ                 @ Set exception type to ARM_EXCEPT_IRQ.
    MRS     R1, SPSR                            @ Save CPSR.
    SUB     R2, LR, #4                          @ Save LR(PC) register: -4.
    MOV     R3, SP                              @ Save SP register.
    ADD     SP, SP, #(4 * 4)                    @ set SP to irq stack top.
    B       _interrupt_handler                  @ bl to common_except_handler.

vector_fiq:
    STMFD   SP!, {R0-R3}                        @ Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_FIQ                 @ Set exception type to ARM_EXCEPT_FIQ.
    MRS     R1, SPSR                            @ Save CPSR.
    SUB     R2, LR, #4                          @ Save LR(PC) register: -4.
    MOV     R3, SP                              @ Save SP register.
    ADD     SP, SP, #(4 * 4)                    @ set SP to fiq stack top.
    B       _interrupt_handler                  @ bl to common_except_handler.


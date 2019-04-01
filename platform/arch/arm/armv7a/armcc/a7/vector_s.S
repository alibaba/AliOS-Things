#include <k_default_config.h>
#include <k_config.h>

;******************************************************************************
;                            EXTERN SYMBOLS
;******************************************************************************
;    IMPORT ||$$bss_start$$||
;    IMPORT __bss_end
    ;IMPORT sys_start
    IMPORT _interrupt_handler
    IMPORT SVC_Handler
    IMPORT _panic_handler
    IMPORT svc_mode5_handle
    IMPORT svc_mode6_handle

;******************************************************************************
;                            EXPORT SYMBOLS
;******************************************************************************
    export  mode_stack_base
    export  mode_stack_top
    export  sys_stack_base
    export  sys_stack_top
    export  _vector_table

;******************************************************************************
;                               EQUATES
;******************************************************************************
; Bits in CPSR (Current Program Status Register) 
CPSR_Mode_USR EQU            0x10
CPSR_Mode_FIQ EQU            0x11
CPSR_Mode_IRQ EQU            0x12
CPSR_Mode_SVC EQU            0x13
CPSR_Mode_ABT EQU            0x17
CPSR_Mode_UND EQU            0x1B
CPSR_Mode_SYS EQU            0x1F

CPSR_FIQ_DIS EQU             0x40           ; Disable FIQ. 
CPSR_IRQ_DIS EQU             0x80           ; Disable IRQ. 
CPSR_INT_DIS EQU             0xC0

; Stack size for all modes 
UND_Stack_Size EQU           0x20
ABT_Stack_Size EQU           0x20
FIQ_Stack_Size EQU           0x20
IRQ_Stack_Size EQU           0x20
MODE_Stack_Each_Size EQU     (UND_Stack_Size + ABT_Stack_Size + FIQ_Stack_Size + IRQ_Stack_Size)
MODE_Stack_Size EQU          (MODE_Stack_Each_Size*RHINO_CONFIG_CPU_NUM)

; Stack size for ISR & Fault & Reset
SYS_Stack_Size EQU           (RHINO_CONFIG_SYSTEM_STACK_SIZE*RHINO_CONFIG_CPU_NUM)

; bits in SCTLR (System Control Register) 
SCTLR_M EQU                  0x0001
SCTLR_C EQU                  0x0004
SCTLR_I EQU                  0x1000

; Exception type 
ARM_EXCEPT_RESET           EQU          0x00
ARM_EXCEPT_UNDEF_INSTR     EQU          0x01
ARM_EXCEPT_SWI             EQU          0x02
ARM_EXCEPT_PREFETCH_ABORT  EQU          0x03
ARM_EXCEPT_DATA_ABORT      EQU          0x04
ARM_EXCEPT_RESERVED        EQU          0x05
ARM_EXCEPT_IRQ             EQU          0x06
ARM_EXCEPT_FIQ             EQU          0x07

;******************************************************************************
;                          STACK REGION DEFINATION
;******************************************************************************
    AREA ||.data||, DATA, READWRITE, ALIGN = 3
mode_stack_base SPACE MODE_Stack_Size
mode_stack_top

;AREA ||.data||, DATA, READWRITE, ALIGN = 3
sys_stack_base SPACE SYS_Stack_Size
sys_stack_top

;******************************************************************************
;                           vector table
;******************************************************************************
    AREA ||.text||, CODE, READONLY
    CODE32

    export vector_swi
    export vector_irq
    export vector_fiq
    export vector_pabt
    export vector_dabt
    export vector_undef
_vector_table
    ldr pc, vector_reset
    ldr pc, vector_undef
    ldr pc, vector_swi
    ldr pc, vector_pabt
    ldr pc, vector_dabt
    ldr pc, vector_resv
    ldr pc, vector_irq
    ldr pc, vector_fiq

; magicword 
;.balignl 16, 0xe7f000f0

;******************************************************************************
;                           vectors function
;******************************************************************************
;    AREA ||.text||, CODE, READONLY
;    CODE32
; reset entry 
vector_reset
    ; save r0 for cores 1-3, r0 arg field passed by ROM
    ; r0 is a function pointer for secondary cpus
    mov     r4, r0

    ; Disable MMU & Cache 
    MRC     p15, 0, r0, c1, c0, 0               ; Read SCTRL 
    BIC     r0, r0, #SCTLR_M                    ; Disable MMU 
    BIC     r0, r0, #SCTLR_C                    ; Disable data cache 
    BIC     r0, r0, #SCTLR_I                    ; Disable instruction cache 
    MCR     p15, 0, r0, c1, c0, 0               ; Write SCTRL 

    ; Invalidate all cache 
    MOV     r0, #0x0
    MCR     p15, 0, r0, c7, c5, 6               ; Invalidate entire branch predictor array. 
    MOV     r0, #0x0
    MCR     p15, 0, r0, c8, c7, 0               ; Invalidate entire Unified TLB 
    MOV     r0, #0
    MCR     p15, 0, r0, c7, c5, 0               ; Invalidate all instruction caches to PoU.
                                                ; Also flushes branch target cache.     

    IF {FPU} != "SoftVFP"
    ; Enabling VFP support
    MRC     p15, 0, r0, c1, c1, 2                ; Read NSACR, Non-Secure Access Control Register
    ORR     r0, r0, #3<<10                        ; enable Non-secure access to fpu 
    MCR     p15, 0, r0, c1, c1, 2                ; Write NSACR
    
    MRC     p15, 0, r0, c1, c0, 2                ; Read CPACR, Non-Secure Access Control Register
    ORR     r0, r0, #0xF<<20                    ; Set access permission for VFP
    MCR     p15, 0, r0, c1, c0, 2                ; Write CPACR
    ISB
    
    MOV     r0, #0x40000000
    VMSR    FPEXC, r0                            ; Set the FPEXC.EN bit to enable the FPU 
    ENDIF

    ; get cpu id, and subtract the offset from the stacks base address
    mrc     p15, 0, r2, c0, c0, 5                      ; read multiprocessor affinity register
    and     r2, r2, #3                            ; mask off, leaving CPU ID field
    mov     r5, r2                                  ; save cpu id for later

    
    ; Setup Stack for each mode 
    LDR     R0, = mode_stack_top
    MOV     R1, #MODE_Stack_Each_Size
    MUL     R1, R1, R5 
    SUB     R0, R0, R1                            ; get mode stack for current core

    MSR     CPSR_c, #(CPSR_Mode_UND :OR: CPSR_INT_DIS)
    MOV     SP, R0
    SUB     R0, R0, #UND_Stack_Size

    MSR     CPSR_c, #(CPSR_Mode_ABT :OR: CPSR_INT_DIS)
    MOV     SP, R0
    SUB     R0, R0, #ABT_Stack_Size

    MSR     CPSR_c, #(CPSR_Mode_FIQ :OR: CPSR_INT_DIS)
    MOV     SP, R0
    SUB     R0, R0, #FIQ_Stack_Size

    MSR     CPSR_c, #(CPSR_Mode_IRQ :OR: CPSR_INT_DIS)
    MOV     SP, R0
    SUB     R0, R0, #IRQ_Stack_Size

    ; Setup Stack for SVC
    LDR     R0, = sys_stack_top
    MOV     R1, #RHINO_EACHCORE_SYSTEM_STACK_SIZE
    MUL     R1, R1, R5 
    SUB     R0, R0, R1      ; get svc stack for current core

    MSR     CPSR_c, #(CPSR_Mode_SVC :OR: CPSR_INT_DIS)
    MOV     SP, R0

    ; check cpu id - cpu0 is primary cpu
    CMP     r5, #0
    BEQ     primary_cpu_init
    BX      r4     ; for secondary cpus, jump to argument function pointer passed in by ROM


primary_cpu_init
;    ; clear .bss
;    MOV     R0,#0                               ; get a zero
;    LDR     R1,=__bss_start                     ; bss start
;    LDR     R2,=__bss_end                       ; bss end
;bss_loop:
;    CMP     R1,R2                               ; check if data to clear
;    STRLO   R0,[R1],#4                          ; clear 4 bytes
;    BLO     bss_loop                            ; loop until done

;verify below
    ; start Kernel 
;    LDR     PC, entry_main
;    B       .                                   ; never reach here

;entry_main DCD   sys_start

; exception handlers: undef, swi, padt, dabt, resv, irq, fiq 
vector_undef
    STMFD   SP!, {R0-R3}                        ; Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_UNDEF_INSTR         ; Set exception type to ARM_EXCEPT_UNDEF_INSTR.
    MRS     R1, SPSR                            ; Save CPSR
    SUB     R2, LR, #4                          ; Save LR(PC) register: -4.
    MOV     R3, SP                              ; Save SP register.
    ADD     SP, SP, #(4 * 4)                    ; set SP to undef stack top.
    B       _panic_handler

vector_swi
    STMFD   SP!, {R0-R3}
    LDR     R0, [LR, #-4]
    BIC     R0, R0, #0xFF000000
    CMP     R0, #5
    BEQ     svc_mode5_handle
    CMP     R0, #6
    BEQ     svc_mode6_handle
    LDMFD   SP!, {R0-R3}
    B       SVC_Handler

vector_pabt
    STMFD   SP!, {R0-R3}                        ; Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_PREFETCH_ABORT      ; Set exception type to ARM_EXCEPT_PREFETCH_ABORT.
    MRS     R1, SPSR                            ; Save CPSR.
    SUB     R2, LR, #4                          ; Save LR(PC) register: -4.
    MOV     R3, SP                              ; Save SP register.
    ADD     SP, SP, #(4 * 4)                    ; set SP to padt stack top.
    B       _panic_handler

vector_dabt
    STMFD   SP!, {R0-R3}                        ; Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_DATA_ABORT          ; Set exception type to ARM_EXCEPT_DATA_ABORT.
    MRS     R1, SPSR                            ; Save CPSR.
    SUB     R2, LR, #8                          ; Save LR(PC) register: -8.
    MOV     R3, SP                              ; Save SP register.
    ADD     SP, SP, #(4 * 4)                    ; set SP to dabt stack top.
    B       _panic_handler

vector_resv
    DCD     0                                   ; reserved for Hyp Trap

vector_irq
    STMFD   SP!, {R0-R3}                        ; Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_IRQ                 ; Set exception type to ARM_EXCEPT_IRQ.
    MRS     R1, SPSR                            ; Save CPSR.
    SUB     R2, LR, #4                          ; Save LR(PC) register: -4.
    MOV     R3, SP                              ; Save SP register.
    ADD     SP, SP, #(4 * 4)                    ; set SP to irq stack top.
    B       _interrupt_handler                  ; bl to common_except_handler.

vector_fiq
    STMFD   SP!, {R0-R3}                        ; Push R0-R3 registers.
    MOV     R0, #ARM_EXCEPT_FIQ                 ; Set exception type to ARM_EXCEPT_FIQ.
    MRS     R1, SPSR                            ; Save CPSR.
    SUB     R2, LR, #4                          ; Save LR(PC) register: -4.
    MOV     R3, SP                              ; Save SP register.
    ADD     SP, SP, #(4 * 4)                    ; set SP to fiq stack top.
    B       _interrupt_handler                  ; bl to common_except_handler.

    END

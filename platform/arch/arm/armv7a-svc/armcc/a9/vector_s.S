#include <k_config.h>
#if (RHINO_CONFIG_SYSTEM_STACK_SIZE == 0)
#error "RHINO_CONFIG_SYSTEM_STACK_SIZE must be set in k_config.h!"
#endif

;******************************************************************************
;                            EXTERN SYMBOLS
;******************************************************************************
    EXTERN __bss_start ; defined in *.ld
    EXTERN __bss_end   ; defined in *.ld
    EXTERN sys_start
    EXTERN _interrupt_handler
    EXTERN _panic_handler
    EXTERN SVC_Handler

;******************************************************************************
;                            EXPORT SYMBOLS
;******************************************************************************
    EXPORT _vector_table
    EXPORT mode_stack_base
    EXPORT mode_stack_top
    EXPORT sys_stack_base
    EXPORT sys_stack_top
    EXPORT _vector_table

;******************************************************************************
;                               EQUATES
;******************************************************************************
; Bits in CPSR (Current Program Status Register)
CPSR_Mode_USR    EQU       0x10
CPSR_Mode_FIQ    EQU       0x11
CPSR_Mode_IRQ    EQU       0x12
CPSR_Mode_SVC    EQU       0x13
CPSR_Mode_ABT    EQU       0x17
CPSR_Mode_UND    EQU       0x1B
CPSR_Mode_SYS    EQU       0x1F

CPSR_FIQ_DIS     EQU       0x40           ; Disable FIQ.
CPSR_IRQ_DIS     EQU       0x80           ; Disable IRQ.
CPSR_INT_DIS     EQU       CPSR_FIQ_DIS:OR:CPSR_IRQ_DIS

; Stack size for all modes
UND_Stack_Size   EQU       0x20
ABT_Stack_Size   EQU       0x20
FIQ_Stack_Size   EQU       0x20
IRQ_Stack_Size   EQU       0x20
MODE_Stack_Size  EQU       UND_Stack_Size + ABT_Stack_Size + FIQ_Stack_Size + IRQ_Stack_Size

; Stack size for ISR & Fault & Reset
SYS_Stack_Size   EQU       RHINO_CONFIG_SYSTEM_STACK_SIZE

; bits in SCTLR (System Control Register)
SCTLR_M         EQU        0x0001
SCTLR_A         EQU        0x0002
SCTLR_C         EQU        0x0004
SCTLR_I         EQU        0x1000

; Exception type
ARM_EXCEPT_RESET            EQU     0x00
ARM_EXCEPT_UNDEF_INSTR      EQU     0x01
ARM_EXCEPT_SWI              EQU     0x02
ARM_EXCEPT_PREFETCH_ABORT   EQU     0x03
ARM_EXCEPT_DATA_ABORT       EQU     0x04
ARM_EXCEPT_RESERVED         EQU     0x05
ARM_EXCEPT_IRQ              EQU     0x06
ARM_EXCEPT_FIQ              EQU     0x07

;******************************************************************************
;                          STACK REGION DEFINATION
;******************************************************************************
    AREA |.bss|, DATA, READWRITE, ALIGN=3
mode_stack_base
    SPACE MODE_Stack_Size*RHINO_CONFIG_CPU_NUM
mode_stack_top
    ARM

    AREA |.bss.isr|, DATA, READWRITE, ALIGN=3
sys_stack_base
    SPACE SYS_Stack_Size*RHINO_CONFIG_CPU_NUM
sys_stack_top

;******************************************************************************
;                           vector table
;******************************************************************************
    AREA |.vectors|, CODE, READONLY, ALIGN=2
    ARM

_vector_table
    ldr pc, _reset
    ldr pc, _undef
    ldr pc, _swi
    ldr pc, _pabt
    ldr pc, _dabt
    ldr pc, _resv
    ldr pc, _irq
    ldr pc, _fiq

_reset  DCD vector_reset
_undef  DCD vector_undef
_swi    DCD vector_swi
_pabt   DCD vector_pabt
_dabt   DCD vector_dabt
_resv   DCD vector_resv
_irq    DCD vector_irq
_fiq    DCD vector_fiq

;******************************************************************************
;                           vectors function
;******************************************************************************
    AREA |.text.isr|, CODE, READONLY, ALIGN=2
    ARM

; reset entry
vector_reset
    ; save R0 for secondary cores, R0 arg field passed by ROM
    ; r0 is a function pointer for secondary cpus
    MOV     R4, R0

    ; Disable MMU & Cache
    MRC     p15, 0, R0, c1, c0, 0               ; Read SCTLR
    BIC     R0, R0, #SCTLR_M                    ; Disable MMU
    BIC     R0, R0, #SCTLR_C                    ; Disable data cache
    BIC     R0, R0, #SCTLR_I                    ; Disable instruction cache
    MCR     p15, 0, R0, c1, c0, 0               ; Write SCTLR

    ; Invalidate all cache
    MOV     R0, #0X0
    MCR     p15, 0, R0, c7, c5, 6               ; Invalidate entire branch predictor array.
    MOV     R0, #0X0
    MCR     p15, 0, R0, c8, c7, 0               ; Invalidate entire Unified TLB
    MOV     R0, #0
    MCR     p15, 0, R0, c7, c5, 0               ; Invalidate all instruction caches to PoU.
                                                ; Also flushes branch target cache.

    ; Alignment check disable
    MRC     p15, 0, R0, c1, c0, 0               ; Read SCTLR
    BIC	    R0, R0, #SCTLR_A                    ; Disable Alignment fault checking
    MCR     p15, 0, R0, c1, c0, 0               ; Write SCTLR

  IF {FPU} != "SoftVFP"
    ; Enabling VFP support
    MRC     p15, 0, R0, c1, c1, 2               ; Read NSACR, Non-Secure Access Control Register
    ORR     R0, R0, #3<<10                      ; enable Non-secure access to fpu
  IF :DEF:TARGET_FEATURE_NEON
    BIC     R0, R0, #3<<14                      ; clear nsasedis/nsd32dis
  ENDIF
    MCR     p15, 0, R0, c1, c1, 2               ; Write NSACR

    MRC     p15, 0, R0, c1, c0, 2               ; Read CPACR, Non-Secure Access Control Register
    ORR     R0, R0, #0XF<<20                    ; Set access permission for VFP
    MCR     p15, 0, R0, c1, c0, 2               ; Write CPACR
    ISB

    MOV     R0, #0X40000000
    VMSR    FPEXC, R0                           ; Set the FPEXC.EN bit to enable the FPU
  ENDIF
    ; get cpu id, and subtract the offset from the stacks base address
    MRC     p15, 0, R5, c0, c0, 5               ; read multiprocessor affinity register
    AND     R5, R5, #3                          ; mask off, leaving CPU ID field

    ; Setup Stack for each mode
    LDR     R0, = mode_stack_top
    MOV     R1, #MODE_Stack_Size
    MUL     R1, R1, R5
    SUB     R0, R0, R1                          ; get mode stack for current core

    MSR     CPSR_c, #CPSR_Mode_UND:OR:CPSR_INT_DIS
    MOV     SP, R0
    SUB     R0, R0, #UND_Stack_Size

    MSR     CPSR_c, #CPSR_Mode_ABT:OR:CPSR_INT_DIS
    MOV     SP, R0
    SUB     R0, R0, #ABT_Stack_Size

    MSR     CPSR_c, #CPSR_Mode_FIQ:OR:CPSR_INT_DIS
    MOV     SP, R0
    SUB     R0, R0, #FIQ_Stack_Size

    MSR     CPSR_c, #CPSR_Mode_IRQ:OR:CPSR_INT_DIS
    MOV     SP, R0
    SUB     R0, R0, #IRQ_Stack_Size

    ; Setup Stack for SVC
    LDR     R0, = sys_stack_top
    MOV     R1, #SYS_Stack_Size
    MUL     R1, R1, R5
    SUB     R0, R0, R1                          ; get svc stack for current core

    MSR     CPSR_c, #CPSR_Mode_SVC:OR:CPSR_INT_DIS
    MOV     SP, R0

    ; check cpu id - cpu0 is primary cpu
    CMP     R5, #0
    BEQ     primary_cpu_init
    BX      R4                                  ; for secondary cores, jump to argument function pointer passed in by ROM

primary_cpu_init
    ; clear .bss
    MOV     R0,#0                               ; get a zero
    LDR     R1,=__bss_start                     ; bss start
    LDR     R2,=__bss_end                       ; bss end
bss_loop
    CMP     R1,R2                               ; check if data to clear
    STRLO   R0,[R1],#4                          ; clear 4 bytes
    BLO     bss_loop                            ; loop until done


    ; start Kernel
    LDR     PC, entry_main
    B       .                                   ; never reach here
entry_main
    DCD     sys_start

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
    UND     0                                   ; reserved for Hyp Trap

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


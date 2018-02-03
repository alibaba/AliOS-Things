
    .EXTERN     _g_active_task
    .EXTERN     _krhino_intrpt_enter
    .EXTERN     _krhino_intrpt_exit

	.SECTION .text, TEXT

PUSHALL  .MACRO
         PUSH   RP0
         PUSH   RP1
         PUSH   RP2
         PUSH   RP3
         MOV    A, CS
         MOV    X, A
         MOV    A, ES
         PUSH   RP0
         .ENDM

POPALL   .MACRO
         POP    RP0
         MOV    ES, A
         MOV    A,  X
         MOV    CS, A
         POP    RP3
         POP    RP2
         POP    RP1
         POP    RP0
         .ENDM

sys_isr_enter  .MACRO
               PUSHALL
               MOVW    RP2, !_g_active_task
               MOVW    RP0, SP
               MOVW    [RP2], RP0
               CALL    !!_krhino_intrpt_enter
               .ENDM

sys_isr_exit  .MACRO
              CALL    !!_krhino_intrpt_exit
              POPALL
              RETI
              .ENDM


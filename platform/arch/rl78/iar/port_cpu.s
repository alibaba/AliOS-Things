#include  "port_cpu.inc"

        NAME    ?cpu_port

        PUBLIC  ___interrupt_0x7E
        PUBLIC  _cpu_first_task_start
        PUBLIC  _task_switch
        PUBLIC  _cpu_intrpt_switch

        EXTERN  _g_preferred_ready_task
        EXTERN  _g_active_task

        SECTION .text:CODE:ROOT(1)

_cpu_first_task_start:
        MOVW    RP1, _g_preferred_ready_task
        MOVW    RP0, 0x0000[RP1]
        MOVW    SP, RP0
        POPALL
        RETI

___interrupt_0x7E:
_task_switch:
        PUSHALL
        MOVW    RP2, _g_active_task
        MOVW    RP0, SP
        MOVW    [RP2], RP0
        MOVW    RP0, _g_preferred_ready_task
        MOVW    _g_active_task, RP0

        MOVW    RP1, _g_preferred_ready_task
        MOVW    RP0, 0x0000[RP1]
        MOVW    SP, RP0

        POPALL

        RETI

_cpu_intrpt_switch:
        MOVW    RP0, _g_preferred_ready_task
        MOVW    _g_active_task, RP0
        MOVW    RP1, _g_preferred_ready_task
        MOVW    RP0, 0x0000[RP1]
        MOVW    SP, RP0
        POPALL
        RETI

        END


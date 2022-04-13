#include <k_config.h>

@******************************************************************************
@                            EXTERN SYMBOLS
@******************************************************************************


@******************************************************************************
@                            EXPORT SYMBOLS
@******************************************************************************
.global k_cache_scu_enable
.global k_cache_scu_invalidate
.global k_cpu_smp_enable
.global k_cpu_smp_disable
@******************************************************************************
@                                 EQUATES
@******************************************************************************
@ Bits in MIDR Primary part number; Base value for V8
.equ ARM_CPU_PART_FOUNDATION,             0xD00
@ bits in ACTLR (Auxiliary Control Register)
.equ ACTLR_SMP,                           0x40
.equ ACTLR_FW,                            0x01
.equ ACTLR_PE,                            0x04

@******************************************************************************
@                        CODE GENERATION DIRECTIVES
@******************************************************************************
.section .text.smp, "ax"
.arm

@ get arch mode: v7 or v8 aarch32
.macro GET_MIDR_PPNUM reg tmp
    MRC     P15, 0, \reg, C0, C0, 0
    MOV     \reg, \reg, lsr #4
    MOV     \tmp, #0xFFFUL
    AND     \reg, \reg, \tmp
.endm

@******************************************************************************
@ Functions:
@     void k_cache_scu_enable(void);
@******************************************************************************
.type k_cache_scu_enable, #function
k_cache_scu_enable:
    @filter v8 aarch32 mode
    GET_MIDR_PPNUM R1 R0
    LDR R0, =ARM_CPU_PART_FOUNDATION
    CMP R1, R0
    BXGE LR

    MRC P15, 4, R0, C15, C0, 0    @ Read periph base address
    LDR R1, [R0, #0x0]            @ Read the SCU Control Register
    ORR R1, R1, #0x1              @ Set bit 0 (The Enable bit)
    STR R1, [R0, #0x0]            @ Write back modifed value
    BX  LR

@******************************************************************************
@ Functions:
@     void k_cache_scu_invalidate(unsigned int cpu, unsigned int ways);
@ Pass 0xf to invalidate all ways
@******************************************************************************
.type k_cache_scu_invalidate, #function
k_cache_scu_invalidate:
    @filter v8 aarch32 mode
    GET_MIDR_PPNUM R3 R2
    LDR R2, =ARM_CPU_PART_FOUNDATION
    CMP R3, R2
    BXGE LR

    AND R0, R0, #0x03             @ Mask off unused bits of CPU ID
    MOV R0, R0, LSL #2            @ Convert into bit offset (four bits per core)
    AND R1, R1, #0x0F             @ Mask off unused bits of ways
    MOV R1, R1, LSL r0            @ Shift ways into the correct CPU field
    MRC P15, 4, R2, C15, C0, 0    @ Read periph base address
    STR R1, [R2, #0x0C]           @ Write to SCU Invalidate All in Secure State
    BX  LR


@******************************************************************************
@ Functions:
@     void k_cpu_smp_enable(void);
@******************************************************************************
.type k_cpu_smp_enable, #function
k_cpu_smp_enable:
    @ Set SMP Mode
    GET_MIDR_PPNUM R1 R0
    LDR R0, =ARM_CPU_PART_FOUNDATION
    CMP R1, R0
    BGE aarch32_smp_set

a9_smp_set:
    MRC     P15, 0, R0, C1, C0, 1               @ Read ACTLR, Auxiliary Control Register
    ORR     R0, R0, #ACTLR_SMP                  @ Set ACTLR.SMP
    MCR     P15, 0, R0, C1, C0, 1               @ Write ACTLR, Auxiliary Control Register

    @ Enable Maintenance operations broadcasting and Dside prefetch.
    MRC     P15, 0, R0, C1, C0, 1               @ Read ACTLR, Auxiliary Control Register
    ORR     R0, R0, #ACTLR_FW                   @ Set ACTLR.FW
    ORR     R0, R0, #ACTLR_PE                   @ Set ACTLR.PE
    MCR     P15, 0, R0, C1, C0, 1               @ Write ACTLR, Auxiliary Control Register
    DSB
    DMB
    ISB
    BX LR

aarch32_smp_set:
    MRRC     P15, 1, R0, R1, C15                @ Read CPUECTLR, CPU Extended Control Register
    ORR      R0, R0, #ACTLR_SMP                 @ Set CPUECTLR.SMP
    MCRR     P15, 1, R0, R1, C15                @ Write CPUECTLR, CPU Extended Control Register
    DSB
    DMB
    ISB
    BX LR


@******************************************************************************
@ Functions:
@     void k_cpu_smp_disable(void);
@******************************************************************************
.type k_cpu_smp_disable, #function
k_cpu_smp_disable:
    @ Clear SMP Mode
    GET_MIDR_PPNUM R1 R0
    LDR R0, =ARM_CPU_PART_FOUNDATION
    CMP R1, R0
    BGE aarch32_smp_clear

a9_smp_clear:
    MRC     P15, 0, R0, C1, C0, 1               @ Read ACTLR, Auxiliary Control Register
    BIC     R0, R0, #ACTLR_SMP                  @ Set ACTLR.SMP
    MCR     P15, 0, R0, C1, C0, 1               @ Write ACTLR, Auxiliary Control Register

    @ Disable Maintenance operations broadcasting and Dside prefetch.
    MRC     P15, 0, R0, C1, C0, 1               @ Read ACTLR, Auxiliary Control Register
    BIC     R0, R0, #ACTLR_FW                   @ Set ACTLR.FW
    BIC     R0, R0, #ACTLR_PE                   @ Set ACTLR.PE
    MCR     P15, 0, R0, C1, C0, 1               @ Write ACTLR, Auxiliary Control Register
    DSB
    DMB
    ISB
    BX LR

aarch32_smp_clear:
    MRRC     P15, 1, R0, R1, C15                @ Read CPUECTLR, CPU Extended Control Register
    BIC      R0, R0, #ACTLR_SMP                 @ Set CPUECTLR.SMP
    MCRR     P15, 1, R0, R1, C15                @ Write CPUECTLR, CPU Extended Control Register
    DSB
    DMB
    ISB
    BX LR


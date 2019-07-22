/*
*
* @file port_s.S
*
*/

#******************************************************************************
#                            EXTERN PARAMETERS
#******************************************************************************
    
.extern _enter_irq

#******************************************************************************
#                            EXPORT FUNCTIONS
#******************************************************************************
    
.globl  cpu_intrpt_save
.globl  cpu_intrpt_restore
.global cpu_first_task_start
.global cpu_intrpt_switch
.global SVC_TASK_SWITCH
.global _interpt

#******************************************************************************
#                                 EQUATES
#******************************************************************************
 
.equ    MIPS_OFFSET_SR,      4 + 16
.equ    MIPS_OFFSET_EPC,     MIPS_OFFSET_SR    + 4
.equ    MIPS_OFFSET_LO,      MIPS_OFFSET_EPC   + 4
.equ    MIPS_OFFSET_HI,      MIPS_OFFSET_LO    + 4
.equ    MIPS_OFFSET_GPR1,    MIPS_OFFSET_HI    + 4
.equ    MIPS_OFFSET_GPR2,    MIPS_OFFSET_GPR1  + 4
.equ    MIPS_OFFSET_GPR3,    MIPS_OFFSET_GPR2  + 4
.equ    MIPS_OFFSET_GPR4,    MIPS_OFFSET_GPR3  + 4
.equ    MIPS_OFFSET_GPR5,    MIPS_OFFSET_GPR4  + 4
.equ    MIPS_OFFSET_GPR6,    MIPS_OFFSET_GPR5  + 4
.equ    MIPS_OFFSET_GPR7,    MIPS_OFFSET_GPR6  + 4
.equ    MIPS_OFFSET_GPR8,    MIPS_OFFSET_GPR7  + 4
.equ    MIPS_OFFSET_GPR9,    MIPS_OFFSET_GPR8  + 4
.equ    MIPS_OFFSET_GPR10,   MIPS_OFFSET_GPR9  + 4
.equ    MIPS_OFFSET_GPR11,   MIPS_OFFSET_GPR10 + 4
.equ    MIPS_OFFSET_GPR12,   MIPS_OFFSET_GPR11 + 4
.equ    MIPS_OFFSET_GPR13,   MIPS_OFFSET_GPR12 + 4
.equ    MIPS_OFFSET_GPR14,   MIPS_OFFSET_GPR13 + 4
.equ    MIPS_OFFSET_GPR15,   MIPS_OFFSET_GPR14 + 4
.equ    MIPS_OFFSET_GPR16,   MIPS_OFFSET_GPR15 + 4
.equ    MIPS_OFFSET_GPR17,   MIPS_OFFSET_GPR16 + 4
.equ    MIPS_OFFSET_GPR18,   MIPS_OFFSET_GPR17 + 4
.equ    MIPS_OFFSET_GPR19,   MIPS_OFFSET_GPR18 + 4
.equ    MIPS_OFFSET_GPR20,   MIPS_OFFSET_GPR19 + 4
.equ    MIPS_OFFSET_GPR21,   MIPS_OFFSET_GPR20 + 4
.equ    MIPS_OFFSET_GPR22,   MIPS_OFFSET_GPR21 + 4
.equ    MIPS_OFFSET_GPR23,   MIPS_OFFSET_GPR22 + 4
.equ    MIPS_OFFSET_GPR24,   MIPS_OFFSET_GPR23 + 4
.equ    MIPS_OFFSET_GPR25,   MIPS_OFFSET_GPR24 + 4
.equ    MIPS_OFFSET_GPR26,   MIPS_OFFSET_GPR25 + 4
.equ    MIPS_OFFSET_GPR27,   MIPS_OFFSET_GPR26 + 4
.equ    MIPS_OFFSET_GPR28,   MIPS_OFFSET_GPR27 + 4
.equ    MIPS_OFFSET_GPR30,   MIPS_OFFSET_GPR28 + 4
.equ    MIPS_OFFSET_GPR31,   MIPS_OFFSET_GPR30 + 4 
.equ    MIPS_CTX_SIZE,       MIPS_OFFSET_GPR31 + 4


#******************************************************************************
#                        CODE GENERATION DIRECTIVES
#******************************************************************************

.section .text, "ax", @progbits
.set noreorder
.set noat



    .ent cpu_intrpt_save
cpu_intrpt_save:

    jr    $31
    di    $2                  

    .end cpu_intrpt_save

    .ent cpu_intrpt_restore
cpu_intrpt_restore:

    jr    $31
    mtc0  $4, $12, 0
    .end cpu_intrpt_restore

 .ent cpu_first_task_start
cpu_first_task_start:
    # sp = g_active_task->task_stack
    la    $8,  g_active_task
    lw    $9,  0($8)
    lw    $29, 0($9)

    lw    $8,  MIPS_OFFSET_SR($29)
    mtc0  $8,  $12, 0

    lw    $8,  MIPS_OFFSET_EPC($29)
    mtc0  $8,  $14, 0

    lw    $8,  MIPS_OFFSET_LO($29)
    lw    $9,  MIPS_OFFSET_HI($29)
    mtlo  $8
    mthi  $9

    lw    $31, MIPS_OFFSET_GPR31($29)
    lw    $30, MIPS_OFFSET_GPR30($29) 
    lw    $28, MIPS_OFFSET_GPR28($29)
    lw    $27, MIPS_OFFSET_GPR27($29) 
    lw    $26, MIPS_OFFSET_GPR26($29) 
    lw    $25, MIPS_OFFSET_GPR25($29) 
    lw    $24, MIPS_OFFSET_GPR24($29) 
    lw    $23, MIPS_OFFSET_GPR23($29) 
    lw    $22, MIPS_OFFSET_GPR22($29) 
    lw    $21, MIPS_OFFSET_GPR21($29) 
    lw    $20, MIPS_OFFSET_GPR20($29) 
    lw    $19, MIPS_OFFSET_GPR19($29) 
    lw    $18, MIPS_OFFSET_GPR18($29) 
    lw    $17, MIPS_OFFSET_GPR17($29) 
    lw    $16, MIPS_OFFSET_GPR16($29) 
    lw    $15, MIPS_OFFSET_GPR15($29) 
    lw    $14, MIPS_OFFSET_GPR14($29) 
    lw    $13, MIPS_OFFSET_GPR13($29) 
    lw    $12, MIPS_OFFSET_GPR12($29) 
    lw    $11, MIPS_OFFSET_GPR11($29) 
    lw    $10, MIPS_OFFSET_GPR10($29) 
    lw    $9,  MIPS_OFFSET_GPR9($29)  
    lw    $8,  MIPS_OFFSET_GPR8($29)  
    lw    $7,  MIPS_OFFSET_GPR7($29)  
    lw    $6,  MIPS_OFFSET_GPR6($29)  
    lw    $5,  MIPS_OFFSET_GPR5($29)  
    lw    $4,  MIPS_OFFSET_GPR4($29)  
    lw    $3,  MIPS_OFFSET_GPR3($29) 
    lw    $2,  MIPS_OFFSET_GPR2($29)  
    lw    $1,  MIPS_OFFSET_GPR1($29) 
    
    addi  $29, $29, MIPS_CTX_SIZE  

    eret

    .end cpu_first_task_start



     .ent cpu_intrpt_switch
cpu_intrpt_switch:
    # g_active_task = g_preferred_ready_task
    la    $8,  g_preferred_ready_task
    lw    $9,  0($8)
    la    $10, g_active_task
    sw    $9,  0($10)

    # sp = g_active_task->task_stack
    lw    $29, 0($9)

    lw    $8,  MIPS_OFFSET_SR($29)
    mtc0  $8,  $12, 0 

    lw    $8,  MIPS_OFFSET_EPC($29)
    mtc0  $8,  $14, 0 

    lw    $8,  MIPS_OFFSET_LO($29)
    lw    $9,  MIPS_OFFSET_HI($29)
    mtlo  $8
    mthi  $9

    lw    $31, MIPS_OFFSET_GPR31($29)
    lw    $30, MIPS_OFFSET_GPR30($29) 
    lw    $28, MIPS_OFFSET_GPR28($29)
    lw    $27, MIPS_OFFSET_GPR27($29) 
    lw    $26, MIPS_OFFSET_GPR26($29) 
    lw    $25, MIPS_OFFSET_GPR25($29) 
    lw    $24, MIPS_OFFSET_GPR24($29) 
    lw    $23, MIPS_OFFSET_GPR23($29) 
    lw    $22, MIPS_OFFSET_GPR22($29) 
    lw    $21, MIPS_OFFSET_GPR21($29) 
    lw    $20, MIPS_OFFSET_GPR20($29) 
    lw    $19, MIPS_OFFSET_GPR19($29) 
    lw    $18, MIPS_OFFSET_GPR18($29) 
    lw    $17, MIPS_OFFSET_GPR17($29) 
    lw    $16, MIPS_OFFSET_GPR16($29) 
    lw    $15, MIPS_OFFSET_GPR15($29) 
    lw    $14, MIPS_OFFSET_GPR14($29) 
    lw    $13, MIPS_OFFSET_GPR13($29) 
    lw    $12, MIPS_OFFSET_GPR12($29) 
    lw    $11, MIPS_OFFSET_GPR11($29) 
    lw    $10, MIPS_OFFSET_GPR10($29) 
    lw    $9,  MIPS_OFFSET_GPR9($29)  
    lw    $8,  MIPS_OFFSET_GPR8($29)  
    lw    $7,  MIPS_OFFSET_GPR7($29)  
    lw    $6,  MIPS_OFFSET_GPR6($29)  
    lw    $5,  MIPS_OFFSET_GPR5($29)  
    lw    $4,  MIPS_OFFSET_GPR4($29)  
    lw    $3,  MIPS_OFFSET_GPR3($29) 
    lw    $2,  MIPS_OFFSET_GPR2($29)  
    lw    $1,  MIPS_OFFSET_GPR1($29) 

    addi  $29, $29, MIPS_CTX_SIZE  

    eret

    .end cpu_intrpt_switch


     .ent SVC_TASK_SWITCH
SVC_TASK_SWITCH:
    addi  $29, $29, -MIPS_CTX_SIZE

    sw    $1,  MIPS_OFFSET_GPR1($29)
    sw    $2,  MIPS_OFFSET_GPR2($29)
    sw    $3,  MIPS_OFFSET_GPR3($29)
    sw    $4,  MIPS_OFFSET_GPR4($29)
    sw    $5,  MIPS_OFFSET_GPR5($29)
    sw    $6,  MIPS_OFFSET_GPR6($29)
    sw    $7,  MIPS_OFFSET_GPR7($29)
    sw    $8,  MIPS_OFFSET_GPR8($29)
    sw    $9,  MIPS_OFFSET_GPR9($29)
    sw    $10, MIPS_OFFSET_GPR10($29)
    sw    $11, MIPS_OFFSET_GPR11($29)
    sw    $12, MIPS_OFFSET_GPR12($29)
    sw    $13, MIPS_OFFSET_GPR13($29)
    sw    $14, MIPS_OFFSET_GPR14($29)
    sw    $15, MIPS_OFFSET_GPR15($29)
    sw    $16, MIPS_OFFSET_GPR16($29)
    sw    $17, MIPS_OFFSET_GPR17($29)
    sw    $18, MIPS_OFFSET_GPR18($29)
    sw    $19, MIPS_OFFSET_GPR19($29)
    sw    $20, MIPS_OFFSET_GPR20($29)
    sw    $21, MIPS_OFFSET_GPR21($29)
    sw    $22, MIPS_OFFSET_GPR22($29)
    sw    $23, MIPS_OFFSET_GPR23($29)
    sw    $24, MIPS_OFFSET_GPR24($29)
    sw    $25, MIPS_OFFSET_GPR25($29)
    sw    $26, MIPS_OFFSET_GPR26($29)
    sw    $27, MIPS_OFFSET_GPR27($29)
    sw    $28, MIPS_OFFSET_GPR28($29)
    sw    $30, MIPS_OFFSET_GPR30($29)
    sw    $31, MIPS_OFFSET_GPR31($29)

    mflo  $8
    mfhi  $9
    sw    $8,  MIPS_OFFSET_LO($29)
    sw    $9,  MIPS_OFFSET_HI($29)

    mfc0  $8,  $14, 0
    addi  $8,  $8, 4
    sw    $8,  MIPS_OFFSET_EPC($29)
  
    mfc0  $8,  $12, 0
    sw    $8,  MIPS_OFFSET_SR($29)

    # g_active_task->task_stack = current sp
    la    $8, g_active_task
    lw    $9, 0($8)
    sw    $29, 0($9)

    # g_active_task = g_preferred_ready_task
    la    $8,  g_preferred_ready_task
    lw    $9,  0($8)
    la    $10, g_active_task
    sw    $9,  0($10)

    # sp = g_active_task->task_stack
    lw    $29, 0($9)

    lw    $8,  MIPS_OFFSET_SR($29)
    mtc0  $8,  $12, 0

    lw    $8,  MIPS_OFFSET_EPC($29)
    mtc0  $8,  $14, 0

    lw    $8,  MIPS_OFFSET_LO($29)
    lw    $9,  MIPS_OFFSET_HI($29)
    mtlo  $8
    mthi  $9

    lw    $31, MIPS_OFFSET_GPR31($29)
    lw    $30, MIPS_OFFSET_GPR30($29) 
    lw    $28, MIPS_OFFSET_GPR28($29)
    lw    $27, MIPS_OFFSET_GPR27($29) 
    lw    $26, MIPS_OFFSET_GPR26($29) 
    lw    $25, MIPS_OFFSET_GPR25($29) 
    lw    $24, MIPS_OFFSET_GPR24($29) 
    lw    $23, MIPS_OFFSET_GPR23($29) 
    lw    $22, MIPS_OFFSET_GPR22($29) 
    lw    $21, MIPS_OFFSET_GPR21($29) 
    lw    $20, MIPS_OFFSET_GPR20($29) 
    lw    $19, MIPS_OFFSET_GPR19($29) 
    lw    $18, MIPS_OFFSET_GPR18($29) 
    lw    $17, MIPS_OFFSET_GPR17($29) 
    lw    $16, MIPS_OFFSET_GPR16($29) 
    lw    $15, MIPS_OFFSET_GPR15($29) 
    lw    $14, MIPS_OFFSET_GPR14($29) 
    lw    $13, MIPS_OFFSET_GPR13($29) 
    lw    $12, MIPS_OFFSET_GPR12($29) 
    lw    $11, MIPS_OFFSET_GPR11($29) 
    lw    $10, MIPS_OFFSET_GPR10($29) 
    lw    $9,  MIPS_OFFSET_GPR9($29)  
    lw    $8,  MIPS_OFFSET_GPR8($29)  
    lw    $7,  MIPS_OFFSET_GPR7($29)  
    lw    $6,  MIPS_OFFSET_GPR6($29)  
    lw    $5,  MIPS_OFFSET_GPR5($29)  
    lw    $4,  MIPS_OFFSET_GPR4($29)  
    lw    $3,  MIPS_OFFSET_GPR3($29) 
    lw    $2,  MIPS_OFFSET_GPR2($29)  
    lw    $1,  MIPS_OFFSET_GPR1($29) 

    addi  $29, $29, MIPS_CTX_SIZE

    eret                                   

    .end SVC_TASK_SWITCH

    .ent _interpt
    _interpt:
    
        addi  $29, $29, -MIPS_CTX_SIZE
    
        sw    $1,  MIPS_OFFSET_GPR1($29)
        sw    $2,  MIPS_OFFSET_GPR2($29)
        sw    $3,  MIPS_OFFSET_GPR3($29)
        sw    $4,  MIPS_OFFSET_GPR4($29)
        sw    $5,  MIPS_OFFSET_GPR5($29)
        sw    $6,  MIPS_OFFSET_GPR6($29)
        sw    $7,  MIPS_OFFSET_GPR7($29)
        sw    $8,  MIPS_OFFSET_GPR8($29)
        sw    $9,  MIPS_OFFSET_GPR9($29)
        sw    $10, MIPS_OFFSET_GPR10($29)
        sw    $11, MIPS_OFFSET_GPR11($29)
        sw    $12, MIPS_OFFSET_GPR12($29)
        sw    $13, MIPS_OFFSET_GPR13($29)
        sw    $14, MIPS_OFFSET_GPR14($29)
        sw    $15, MIPS_OFFSET_GPR15($29)
        sw    $16, MIPS_OFFSET_GPR16($29)
        sw    $17, MIPS_OFFSET_GPR17($29)
        sw    $18, MIPS_OFFSET_GPR18($29)
        sw    $19, MIPS_OFFSET_GPR19($29)
        sw    $20, MIPS_OFFSET_GPR20($29)
        sw    $21, MIPS_OFFSET_GPR21($29)
        sw    $22, MIPS_OFFSET_GPR22($29)
        sw    $23, MIPS_OFFSET_GPR23($29)
        sw    $24, MIPS_OFFSET_GPR24($29)
        sw    $25, MIPS_OFFSET_GPR25($29)
        sw    $26, MIPS_OFFSET_GPR26($29)
        sw    $27, MIPS_OFFSET_GPR27($29)
        sw    $28, MIPS_OFFSET_GPR28($29)
        sw    $30, MIPS_OFFSET_GPR30($29)
        sw    $31, MIPS_OFFSET_GPR31($29)

        mflo  $8
        mfhi  $9
        sw    $8,  MIPS_OFFSET_LO($29)
        sw    $9,  MIPS_OFFSET_HI($29)
    
        mfc0  $8,  $14, 0
        sw    $8,  MIPS_OFFSET_EPC($29)
      
        mfc0  $8,  $12, 0
        sw    $8,  MIPS_OFFSET_SR($29)

        la    $10, g_active_task
        lw    $11, 0($10)
        sw    $29, 0($11)

        jal 	_enter_irq
        nop

        lw    $8,  MIPS_OFFSET_SR($29)
        mtc0  $8,  $12, 0
        ehb
    
        lw    $8,  MIPS_OFFSET_EPC($29)
        mtc0  $8,  $14, 0
    
        lw    $8,  MIPS_OFFSET_LO($29)
        lw    $9,  MIPS_OFFSET_HI($29)
        mtlo  $8
        mthi  $9
    
        lw    $31, MIPS_OFFSET_GPR31($29)
        lw    $30, MIPS_OFFSET_GPR30($29)
        lw    $28, MIPS_OFFSET_GPR28($29)
        lw    $27, MIPS_OFFSET_GPR27($29)
        lw    $26, MIPS_OFFSET_GPR26($29)
        lw    $25, MIPS_OFFSET_GPR25($29)
        lw    $24, MIPS_OFFSET_GPR24($29)
        lw    $23, MIPS_OFFSET_GPR23($29)
        lw    $22, MIPS_OFFSET_GPR22($29)
        lw    $21, MIPS_OFFSET_GPR21($29)
        lw    $20, MIPS_OFFSET_GPR20($29)
        lw    $19, MIPS_OFFSET_GPR19($29)
        lw    $18, MIPS_OFFSET_GPR18($29)
        lw    $17, MIPS_OFFSET_GPR17($29)
        lw    $16, MIPS_OFFSET_GPR16($29)
        lw    $15, MIPS_OFFSET_GPR15($29)
        lw    $14, MIPS_OFFSET_GPR14($29)
        lw    $13, MIPS_OFFSET_GPR13($29)
        lw    $12, MIPS_OFFSET_GPR12($29)
        lw    $11, MIPS_OFFSET_GPR11($29)
        lw    $10, MIPS_OFFSET_GPR10($29)
        lw    $9,  MIPS_OFFSET_GPR9($29)
        lw    $8,  MIPS_OFFSET_GPR8($29)
        lw    $7,  MIPS_OFFSET_GPR7($29)
        lw    $6,  MIPS_OFFSET_GPR6($29)
        lw    $5,  MIPS_OFFSET_GPR5($29)
        lw    $4,  MIPS_OFFSET_GPR4($29)
        lw    $3,  MIPS_OFFSET_GPR3($29)
        lw    $2,  MIPS_OFFSET_GPR2($29)
        lw    $1,  MIPS_OFFSET_GPR1($29)
    
        addi  $29, $29, MIPS_CTX_SIZE
    
        eret

        .end _interpt


;@******************************************************************************
;@                            EXTERN PARAMETERS
;@******************************************************************************

	IMPORT g_active_task
	IMPORT g_preferred_ready_task

;@******************************************************************************
;@                            EXPORT FUNCTIONS
;@******************************************************************************

	EXPORT cpu_intrpt_save
	EXPORT cpu_intrpt_restore
	EXPORT cpu_task_switch
	EXPORT cpu_intrpt_switch
	EXPORT cpu_first_task_start
	IMPORT krhino_stack_ovf_check
		
	PRESERVE8
		
	CODE32
	
	AREA |.text|, CODE, READONLY
		
	GLOBAL cpu_intrpt_save
cpu_intrpt_save
    MRS     R0, CPSR                ;@Set IRQ and FIQ bits in CPSR to disable all interrupts
    ORR     R1, R0, #0xC0
    MSR     CPSR_c, R1
    MRS     R1, CPSR                ;@Confirm that CPSR contains the proper interrupt disable flags
    AND     R1, R1, #0xC0
    CMP     R1, #0xC0
    BNE     cpu_intrpt_save         ;@Not properly disabled (try again)
    BX      LR                      ;@Disabled, return the original CPSR contents in R0

	GLOBAL cpu_intrpt_restore
cpu_intrpt_restore
    MSR     CPSR_c, R0
    BX      LR
	
cpu_first_task_start
    LDR R0, =g_active_task
    LDR R0, [R0]         
    LDR SP, [R0]
    
    LDMFD   SP!, {R0}  
    LDR     R1, [SP, #0x38]
    TST     R1, #1
    ORRNE   R0, #0x20          ;@ if PC is thumb mode, set SPSR to thumb
    MSR     SPSR_cxsf, R0
    LDMFD   SP!, {R0-R12, LR, PC}^
	
	GLOBAL cpu_task_switch
cpu_task_switch
    STMFD SP!,{LR}             
    STMFD SP!,{R0-R12,LR}       
    MRS   R0,CPSR
    STMFD SP!,{R0}                 ;@push current cpsr

    ;/* g_active_task->task_stack = SP */
    LDR R0, =g_active_task
    LDR R0, [R0]
    STR SP, [R0]

    BL krhino_stack_ovf_check
	
	GLOBAL cpu_intrpt_switch
cpu_intrpt_switch
    LDR     R0, =g_preferred_ready_task
    LDR     R1, =g_active_task
    LDR     R0, [R0]
    STR     R0, [R1]

    LDR     R0, =g_active_task
    LDR     R0, [R0]
    LDR     SP, [R0]

    ;@----------------------------------------------------------------------------------	
    ;@ Restore New Task context
    ;@----------------------------------------------------------------------------------
	
    LDMFD   SP!, {R0}       
    LDR     R1, [SP, #38]
    TST     R1, #1
    ORRNE   R0, #0x20          ;@ if PC is thumb mode, set SPSR to thumb
    MSR     SPSR_cxsf, R0
    LDMFD   SP!, {R0-R12, LR, PC}^  

    END


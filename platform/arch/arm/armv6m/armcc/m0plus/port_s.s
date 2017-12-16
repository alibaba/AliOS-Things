 EXTERN  g_active_task
 EXTERN  g_preferred_ready_task
 IMPORT  krhino_stack_ovf_check
 EXPORT  cpu_intrpt_save    
 EXPORT  cpu_intrpt_restore
 EXPORT  cpu_first_task_start
 EXPORT  cpu_task_switch
 EXPORT  cpu_intrpt_switch
 EXPORT  PendSV_Handler
    

NVIC_INT_CTRL   EQU     0xE000ED04
NVIC_SHPR3      EQU     0xE000ED20
NVIC_PENDSV_PRI EQU     0x00C00000
NVIC_PENDSVSET  EQU     0x10000000

    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

cpu_intrpt_save
    MRS     R0, PRIMASK     
    CPSID   I
    BX      LR

cpu_intrpt_restore
    MSR     PRIMASK, R0
    BX      LR

cpu_first_task_start
    ;set PendSV prority to the lowest
    LDR     R0, =NVIC_SHPR3                   
    LDR     R2, [R0]
    LDR     R1, =NVIC_PENDSV_PRI
    ORRS    R2, R1
    STR     R2, [R0]

    ;indicate PendSV_Handler branch to _pendsv_handler_nosave
    MOVS    R0, #0                                       
    MSR     PSP, R0

    ;align MSP to 8 byte
    MRS     R0, MSP
    LSRS    R0, R0, #3
    LSLS    R0, R0, #3
    MSR     MSP, R0

    ;make PendSV exception pending
    LDR     R0, =NVIC_INT_CTRL                                  
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]

    ;goto PendSV_Handler
    CPSIE   I                                                  

cpu_task_switch
    LDR     R0, =NVIC_INT_CTRL                             
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

cpu_intrpt_switch
    LDR     R0, =NVIC_INT_CTRL                                  
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

PendSV_Handler
    CPSID   I                                  
    MRS     R0, PSP 
    CMP     R0, #0
    ;branch if cpu_first_task_start
    BEQ     _pendsv_handler_nosave

    ;hardware saved R0~R3,R12,LR,PC,xPSR
    
    ;save context
    SUBS    R0, R0, #0x20
    STMIA   R0!, {R4 - R7}        
    MOV     R4, R8                
    MOV     R5, R9
    MOV     R6, R10
    MOV     R7, R11
    STMIA   R0!, {R4 - R7}        

    ;g_active_task->task_stack = context region
    SUBS    R0, R0, #0x20
    LDR     R1, =g_active_task              
    LDR     R1, [R1]
    STR     R0, [R1]                      

    ;save and restore LR
	MOV 	R4, LR
    bl      krhino_stack_ovf_check
    MOV 	LR, R4    

_pendsv_handler_nosave
    LDR     R0, =g_active_task         
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1]
    STR     R2, [R0]
    ;R0 = g_active_task->task_stack = context region
    LDR     R0, [R2]  

    ;restore context
	LDMIA   R0!, {R4 - R7}          
    LDMIA   R0!, {R2 - R3}          
    MOV     R8,  R2                 
    MOV     R9,  R3                 
    LDMIA   R0!, {R2 - R3}          
    MOV     R10,  R2                 
    MOV     R11,  R3                 

    MSR     PSP, R0

    ;after exception return: stack = PSP
    MOV     R0, LR
    MOVS    R1, #0x04
    ORRS    R0, R1
    MOV     LR, R0

    CPSIE   I
    
    ;hardware restore R0~R3,R12,LR,PC,xPSR
    BX      LR 

    ALIGN
    END


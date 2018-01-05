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
NVIC_SYSPRI14   EQU     0xE000ED22
NVIC_PENDSV_PRI EQU     0xFF
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
    LDR     R0, =NVIC_SYSPRI14                   
    LDR     R1, =NVIC_PENDSV_PRI
    STRB    R1, [R0]

    MOVS    R0, #0                                       
    MSR     PSP, R0

    ;align msp to 8 byte
    MRS     R0, MSP
    LSRS    R0, R0, #3
    LSLS    R0, R0, #3
    MSR     MSP, R0

    LDR     R0, =NVIC_INT_CTRL                                  
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]

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
    CBZ     R0, _pendsv_handler_nosave

    IF      {FPU} != "SoftVFP"
    VSTMFD  r0!, {d8 - d15}
    ENDIF

    SUBS    R0, R0, #0x24                   
    STM     R0, {R4-R11, LR}

    LDR     R1, =g_active_task              
    LDR     R1, [R1]
    STR     R0, [R1]                      

    bl      krhino_stack_ovf_check

_pendsv_handler_nosave
    LDR     R0, =g_active_task         
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]                         
    LDM     R0, {R4-R11, LR}                  
    ADDS    R0, R0, #0x24

    IF	    {FPU} != "SoftVFP"
    VLDMFD  r0!, {d8 - d15}
    ENDIF

    ;return stack = PSP
    MSR     PSP, R0               
    ORR     LR, LR, #0x04
    
    CPSIE   I
    BX      LR 

    ALIGN   ;align to avoid warning A1581W
    END


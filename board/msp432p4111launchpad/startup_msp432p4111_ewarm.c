//*****************************************************************************
//
// Copyright (C) 2012 - 2016 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the
//  distribution.
//
//  Neither the name of Texas Instruments Incorporated nor the names of
//  its contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// MSP432 Startup Code and interrupt vectors for IAR Embedded Workbench for ARM
//
//*****************************************************************************

#include <stdint.h>

//*****************************************************************************
//
// Enable the IAR extensions for this source file.
//
//*****************************************************************************
#pragma language=extended

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
extern void Reset_Handler       (void);
static void Default_Handler     (void);

extern void NMI_Handler         (void);
extern void HardFault_Handler   (void);
extern void MemManage_Handler   (void);
extern void BusFault_Handler    (void);
extern void UsageFault_Handler  (void);
extern void SVC_Handler         (void);
extern void DebugMon_Handler    (void);
extern void PendSV_Handler      (void);


/* device specific interrupt handler */
extern void SysTick_Handler     (void);
extern void PSS_IRQHandler      (void);
extern void CS_IRQHandler       (void);
extern void PCM_IRQHandler      (void);
extern void WDT_A_IRQHandler    (void);
extern void FPU_IRQHandler      (void);
extern void FLCTL_A_IRQHandler  (void);
extern void COMP_E0_IRQHandler  (void);
extern void COMP_E1_IRQHandler  (void);
extern void TA0_0_IRQHandler    (void);
extern void TA0_N_IRQHandler    (void);
extern void TA1_0_IRQHandler    (void);
extern void TA1_N_IRQHandler    (void);
extern void TA2_0_IRQHandler    (void);
extern void TA2_N_IRQHandler    (void);
extern void TA3_0_IRQHandler    (void);
extern void TA3_N_IRQHandler    (void);
extern void EUSCIA0_IRQHandler  (void);
extern void EUSCIA1_IRQHandler  (void);
extern void EUSCIA2_IRQHandler  (void);
extern void EUSCIA3_IRQHandler  (void);
extern void EUSCIB0_IRQHandler  (void);
extern void EUSCIB1_IRQHandler  (void);
extern void EUSCIB2_IRQHandler  (void);
extern void EUSCIB3_IRQHandler  (void);
extern void ADC14_IRQHandler    (void);
extern void T32_INT1_IRQHandler (void);
extern void T32_INT2_IRQHandler (void);
extern void T32_INTC_IRQHandler (void);
extern void AES256_IRQHandler   (void);
extern void RTC_C_IRQHandler    (void);
extern void DMA_ERR_IRQHandler  (void);
extern void DMA_INT3_IRQHandler (void);
extern void DMA_INT2_IRQHandler (void);
extern void DMA_INT1_IRQHandler (void);
extern void DMA_INT0_IRQHandler (void);
extern void PORT1_IRQHandler    (void);
extern void PORT2_IRQHandler    (void);
extern void PORT3_IRQHandler    (void);
extern void PORT4_IRQHandler    (void);
extern void PORT5_IRQHandler    (void);
extern void PORT6_IRQHandler    (void);
extern void LCD_F_IRQHandler    (void);

/* Cortex-M4 Processor Exceptions */
#pragma weak NMI_Handler=Default_Handler
#pragma weak HardFault_Handler=Default_Handler
#pragma weak MemManage_Handler=Default_Handler
#pragma weak BusFault_Handler=Default_Handler
#pragma weak UsageFault_Handler=Default_Handler
#pragma weak SVC_Handler=Default_Handler
#pragma weak DebugMon_Handler=Default_Handler   
#pragma weak PendSV_Handler=Default_Handler      

/* device specific interrupt handler */
#pragma weak SysTick_Handler=Default_Handler
#pragma weak PSS_IRQHandler=Default_Handler
#pragma weak CS_IRQHandler=Default_Handler
#pragma weak PCM_IRQHandler=Default_Handler
#pragma weak WDT_A_IRQHandler=Default_Handler
#pragma weak FPU_IRQHandler=Default_Handler
#pragma weak FLCTL_A_IRQHandler=Default_Handler
#pragma weak COMP_E0_IRQHandler=Default_Handler
#pragma weak COMP_E1_IRQHandler=Default_Handler
#pragma weak TA0_0_IRQHandler=Default_Handler
#pragma weak TA0_N_IRQHandler=Default_Handler
#pragma weak TA1_0_IRQHandler=Default_Handler
#pragma weak TA1_N_IRQHandler=Default_Handler
#pragma weak TA2_0_IRQHandler=Default_Handler
#pragma weak TA2_N_IRQHandler=Default_Handler
#pragma weak TA3_0_IRQHandler=Default_Handler
#pragma weak TA3_N_IRQHandler=Default_Handler
#pragma weak EUSCIA0_IRQHandler=Default_Handler
#pragma weak EUSCIA1_IRQHandler=Default_Handler
#pragma weak EUSCIA2_IRQHandler=Default_Handler
#pragma weak EUSCIA3_IRQHandler=Default_Handler
#pragma weak EUSCIB0_IRQHandler=Default_Handler
#pragma weak EUSCIB1_IRQHandler=Default_Handler
#pragma weak EUSCIB2_IRQHandler=Default_Handler
#pragma weak EUSCIB3_IRQHandler=Default_Handler
#pragma weak ADC14_IRQHandler=Default_Handler
#pragma weak T32_INT1_IRQHandler=Default_Handler
#pragma weak T32_INT2_IRQHandler=Default_Handler
#pragma weak T32_INTC_IRQHandler=Default_Handler
#pragma weak AES256_IRQHandler=Default_Handler
#pragma weak RTC_C_IRQHandler=Default_Handler
#pragma weak DMA_ERR_IRQHandler=Default_Handler
#pragma weak DMA_INT3_IRQHandler=Default_Handler
#pragma weak DMA_INT2_IRQHandler=Default_Handler
#pragma weak DMA_INT1_IRQHandler=Default_Handler
#pragma weak DMA_INT0_IRQHandler=Default_Handler
#pragma weak PORT1_IRQHandler=Default_Handler
#pragma weak PORT2_IRQHandler=Default_Handler
#pragma weak PORT3_IRQHandler=Default_Handler
#pragma weak PORT4_IRQHandler=Default_Handler
#pragma weak PORT5_IRQHandler=Default_Handler
#pragma weak PORT6_IRQHandler=Default_Handler
#pragma weak LCD_F_IRQHandler=Default_Handler

//*****************************************************************************
//
// The reference for the device initialization.
//
//*****************************************************************************
extern void SystemInit(void);

//*****************************************************************************
//
// The entry point for the application startup code.
//
//*****************************************************************************
extern void main(void);

//*****************************************************************************
//
// Reserve space for the system stack.
//
//*****************************************************************************
static uint32_t systemStack[128] @ ".noinit";

//*****************************************************************************
//
// A union that describes the entries of the vector table.  The union is needed
// since the first entry is the stack pointer and the remainder are function
// pointers.
//
//*****************************************************************************
typedef union
{
    void (*handler)(void);
    uint32_t ptr;
}
uVectorEntry;

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
__root const uVectorEntry __vector_table[] @ ".intvec" =
{
    { .ptr = (uint32_t)systemStack + sizeof(systemStack) },
                                           // The initial stack pointer
    Reset_Handler,                         /* The reset handler         */
    NMI_Handler,                           /* The NMI handler           */
    HardFault_Handler,                     /* The hard fault handler    */
    MemManage_Handler,                     /* The MPU fault handler     */
    BusFault_Handler,                      /* The bus fault handler     */
    UsageFault_Handler,                    /* The usage fault handler   */
    0,                                     /* Reserved                  */
    0,                                     /* Reserved                  */
    0,                                     /* Reserved                  */
    0,                                     /* Reserved                  */
    SVC_Handler,                           /* SVCall handler            */
    DebugMon_Handler,                      /* Debug monitor handler     */
    0,                                     /* Reserved                  */
    PendSV_Handler,                        /* The PendSV handler        */
    SysTick_Handler,                       /* The SysTick handler       */
    PSS_IRQHandler,                        /* PSS Interrupt             */
    CS_IRQHandler,                         /* CS Interrupt              */
    PCM_IRQHandler,                        /* PCM Interrupt             */
    WDT_A_IRQHandler,                      /* WDT_A Interrupt           */
    FPU_IRQHandler,                        /* FPU Interrupt             */
    FLCTL_A_IRQHandler,                    /* Flash Controller Interrupt*/
    COMP_E0_IRQHandler,                    /* COMP_E0 Interrupt         */
    COMP_E1_IRQHandler,                    /* COMP_E1 Interrupt         */
    TA0_0_IRQHandler,                      /* TA0_0 Interrupt           */
    TA0_N_IRQHandler,                      /* TA0_N Interrupt           */
    TA1_0_IRQHandler,                      /* TA1_0 Interrupt           */
    TA1_N_IRQHandler,                      /* TA1_N Interrupt           */
    TA2_0_IRQHandler,                      /* TA2_0 Interrupt           */
    TA2_N_IRQHandler,                      /* TA2_N Interrupt           */
    TA3_0_IRQHandler,                      /* TA3_0 Interrupt           */
    TA3_N_IRQHandler,                      /* TA3_N Interrupt           */
    EUSCIA0_IRQHandler,                    /* EUSCIA0 Interrupt         */
    EUSCIA1_IRQHandler,                    /* EUSCIA1 Interrupt         */
    EUSCIA2_IRQHandler,                    /* EUSCIA2 Interrupt         */
    EUSCIA3_IRQHandler,                    /* EUSCIA3 Interrupt         */
    EUSCIB0_IRQHandler,                    /* EUSCIB0 Interrupt         */
    EUSCIB1_IRQHandler,                    /* EUSCIB1 Interrupt         */
    EUSCIB2_IRQHandler,                    /* EUSCIB2 Interrupt         */
    EUSCIB3_IRQHandler,                    /* EUSCIB3 Interrupt         */
    ADC14_IRQHandler,                      /* ADC14 Interrupt           */
    T32_INT1_IRQHandler,                   /* T32_INT1 Interrupt        */
    T32_INT2_IRQHandler,                   /* T32_INT2 Interrupt        */
    T32_INTC_IRQHandler,                   /* T32_INTC Interrupt        */
    AES256_IRQHandler,                     /* AES256 Interrupt          */
    RTC_C_IRQHandler,                      /* RTC_C Interrupt           */
    DMA_ERR_IRQHandler,                    /* DMA_ERR Interrupt         */
    DMA_INT3_IRQHandler,                   /* DMA_INT3 Interrupt        */
    DMA_INT2_IRQHandler,                   /* DMA_INT2 Interrupt        */
    DMA_INT1_IRQHandler,                   /* DMA_INT1 Interrupt        */
    DMA_INT0_IRQHandler,                   /* DMA_INT0 Interrupt        */
    PORT1_IRQHandler,                      /* Port1 Interrupt           */
    PORT2_IRQHandler,                      /* Port2 Interrupt           */
    PORT3_IRQHandler,                      /* Port3 Interrupt           */
    PORT4_IRQHandler,                      /* Port4 Interrupt           */
    PORT5_IRQHandler,                      /* Port5 Interrupt           */
    PORT6_IRQHandler,                      /* Port6 Interrupt           */
    LCD_F_IRQHandler                       /* LCD_F Interrupt           */
};

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
__weak void Reset_Handler(void)
{
     
    //
    // Initialize the device
    //
    SystemInit();

    //
    // Call the application's entry point.
    //
    __iar_program_start();
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void Default_Handler(void)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}


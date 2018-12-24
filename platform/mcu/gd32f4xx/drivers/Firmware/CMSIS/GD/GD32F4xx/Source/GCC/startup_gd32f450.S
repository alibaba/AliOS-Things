/**
  ******************************************************************************
  * @file    startup_gd32f450.s
  * @author  MCU SD
  * @version V1.0.0
  * @date    26-Dec-2014
  * @brief   GD32F30X startup code.
  ******************************************************************************
  */

  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb
  
.global  g_pfnVectors
.global  Default_Handler
     
/* start address of the static initialization data */
.word  _sidata
/* start address of the data section */ 
.word  _sdata
/* end address of the data section */
.word  _edata
/* start address of the bss section */
.word  _sbss
/* end address of the bss section */
.word  _ebss


    .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function

Reset_Handler:
  ldr   sp, =_estack    /* Atollic update: set stack pointer */
  
/* copy the data segment into ram */  
  movs r1, #0
  b DataInit

CopyData:
  ldr r3, =_sidata
  ldr r3, [r3, r1]
  str r3, [r0, r1]
  adds r1, r1, #4
    
DataInit:
  ldr r0, =_sdata
  ldr r3, =_edata
  adds r2, r0, r1
  cmp r2, r3
  bcc CopyData
  ldr r2, =_sbss
  b Zerobss
/* zero the bss segment */ 
FillZerobss:
  movs r3, #0
  str r3, [r2], #4
    
Zerobss:
  ldr r3, = _ebss
  cmp r2, r3
  bcc FillZerobss
/* configure the clock */
  bl  SystemInit
/* start execution of the program */
/* Call static constructors */
#    bl __libc_init_array
  bl main
  bx lr
.size Reset_Handler, .-Reset_Handler



    .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler


  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
  .size g_pfnVectors, .-g_pfnVectors


g_pfnVectors:
                    .word _estack
                    .word Reset_Handler              /* Vector Number 1,Reset Handler */
                    .word NMI_Handler                /* Vector Number 2,NMI Handler */
                    .word HardFault_Handler          /* Vector Number 3,Hard Fault Handler */
                    .word MemManage_Handler          /* Vector Number 4,MPU Fault Handler */
                    .word BusFault_Handler           /* Vector Number 5,Bus Fault Handler */
                    .word UsageFault_Handler         /* Vector Number 6,Usage Fault Handler */
                    .word 0                          /* Reserved  */
                    .word 0                          /* Reserved */
                    .word 0                          /* Reserved */
                    .word 0                          /* Reserved */
                    .word SVC_Handler                /* Vector Number 11,SVCall Handler */
                    .word DebugMon_Handler           /* Vector Number 12,Debug Monitor Handler */
                    .word 0                          /* Reserved */
                    .word PendSV_Handler             /* Vector Number 14,PendSV Handler */
                    .word SysTick_Handler            /* Vector Number 15,SysTick Handler */

                    /* External Interrupts */
                    .word WWDGT_IRQHandler               /* Vector Number 16,Window Watchdog */
                    .word LVD_IRQHandler                 /* Vector Number 17,LVD through EXTI Line detect */
                    .word TAMPER_STAMP_IRQHandler              /* Vector Number 18,Tamper Interrupt */
                    .word RTC_WKUP_IRQHandler                 /* Vector Number 19,RTC through EXTI Line */
                    .word FMC_IRQHandler                 /* Vector Number 20,FMC */
                    .word RCU_CTC_IRQHandler             /* Vector Number 21,RCC */
                    .word EXTI0_IRQHandler               /* Vector Number 22,EXTI Line 0 */
                    .word EXTI1_IRQHandler               /* Vector Number 23,EXTI Line 1 */
                    .word EXTI2_IRQHandler               /* Vector Number 24,EXTI Line 2 */
                    .word EXTI3_IRQHandler               /* Vector Number 25,EXTI Line 3 */
                    .word EXTI4_IRQHandler               /* Vector Number 26,EXTI Line 4 */
                    .word DMA0_Channel0_IRQHandler       /* Vector Number 27,DMA1 Channel 1 */
                    .word DMA0_Channel1_IRQHandler       /* Vector Number 28,DMA1 Channel 2 */
                    .word DMA0_Channel2_IRQHandler       /* Vector Number 29,DMA1 Channel 3 */
                    .word DMA0_Channel3_IRQHandler       /* Vector Number 30,DMA1 Channel 4 */
                    .word DMA0_Channel4_IRQHandler       /* Vector Number 31,DMA1 Channel 5 */
                    .word DMA0_Channel5_IRQHandler       /* Vector Number 32,DMA1 Channel 6 */
                    .word DMA0_Channel6_IRQHandler       /* Vector Number 33,DMA1 Channel 7 */
                    .word ADC_IRQHandler              /* Vector Number 34,ADC1 and ADC2  */
                    .word CAN0_TX_IRQHandler             /* Vector Number 35,CAN1 TX  */
                    .word CAN0_RX0_IRQHandler            /* Vector Number 36,CAN1 RX0  */
                    .word CAN0_RX1_IRQHandler            /* Vector Number 37,CAN1 RX1  */
                    .word CAN0_EWMC_IRQHandler           /* Vector Number 38,CAN1 SCE  */
                    .word EXTI5_9_IRQHandler             /* Vector Number 39,EXTI Line 9..5  */
                    .word TIMER0_BRK_TIMER8_IRQHandler          /* Vector Number 40,TIMER1 Break  */
                    .word TIMER0_UP_TIMER9_IRQHandler           /* Vector Number 41,TIMER1 Update  */
                    .word TIMER0_TRG_CMT_TIMER10_IRQHandler     /* Vector Number 42,TIMER1 Break, Update, Trigger and Commutation  */
                    .word TIMER0_CC_IRQHandler             /* Vector Number 43,TIMER1 Capture Compare */
                    .word TIMER1_IRQHandler              /* Vector Number 44,TIMER2 */
                    .word TIMER2_IRQHandler              /* Vector Number 45,TIMER3 */
                    .word TIMER3_IRQHandler              /* Vector Number 46,TIMER4 */
                    .word I2C0_EV_IRQHandler             /* Vector Number 47,I2C1 Event */
                    .word I2C0_ER_IRQHandler             /* Vector Number 48,I2C1 Error */
                    .word I2C1_EV_IRQHandler             /* Vector Number 49,I2C2 Event */
                    .word I2C1_ER_IRQHandler             /* Vector Number 50,I2C1 Error */
                    .word SPI0_IRQHandler                /* Vector Number 51,SPI1 */
                    .word SPI1_IRQHandler                /* Vector Number 52,SPI2 */
                    .word USART0_IRQHandler              /* Vector Number 53,USART1 */
                    .word USART1_IRQHandler              /* Vector Number 54,USART2 */
                    .word USART2_IRQHandler              /* Vector Number 55,USART3 */
                    .word EXTI10_15_IRQHandler           /* Vector Number 56,External Line[15:10] */
                    .word RTC_Alarm_IRQHandler           /* Vector Number 57,RTC Alarm through EXTI Line */
                    .word USBFS_WKUP_IRQHandler          /* Vector Number 58,USB OTG FS WakeUp from suspend through EXTI Line */
                    .word TIMER7_BRK_TIMER11_IRQHandler  /* Vector Number 59,TIMER8 Break Interrupt and TIMER12 global */
                    .word TIMER7_UP_TIMER12_IRQHandler   /* Vector Number 60,TIMER8 Update Interrupt and TIMER13 global */
                    .word TIMER7_TRG_CMT_TIMER13_IRQHandler /* Vector Number 61,TIMER8 Trigger and Commutation Interrupt and TIMER14 */
                    .word TIMER7_CC_IRQHandler      /* Vector Number 62,TIMER8 Capture Compare */ 
                    .word DMA0_Channel7_IRQHandler                              /* Vector Number 63,ADC3 */
                    .word EXMC_IRQHandler                /* Vector Number 64,EXMC */
                    .word SDIO_IRQHandler                              /* Reserved */
                    .word TIMER4_IRQHandler              /* Vector Number 66,TIMER5 */
                    .word SPI2_IRQHandler                /* Vector Number 67,SPI3 */
                    .word UART3_IRQHandler               /* Vector Number 68,UART4 */
                    .word UART4_IRQHandler               /* Vector Number 69,UART5 */
                    .word TIMER5_DAC_IRQHandler              /* Vector Number 70,TIMER6 */
                    .word TIMER6_IRQHandler              /* Vector Number 71,TIMER7 */
                    .word DMA1_Channel0_IRQHandler       /* Vector Number 72,DMA2 Channel1 */
                    .word DMA1_Channel1_IRQHandler       /* Vector Number 73,DMA2 Channel2 */
                    .word DMA1_Channel2_IRQHandler       /* Vector Number 74,DMA2 Channel3 */
                    .word DMA1_Channel3_IRQHandler       /* Vector Number 75,DMA2 Channel4 */
                .word     DMA1_Channel4_IRQHandler         /* 76:DMA1 Channel4 */
                .word     ENET_IRQHandler                  /* 77:Ethernet */
                .word     ENET_WKUP_IRQHandler             /* 78:Ethernet Wakeup through EXTI Line */
                .word     CAN1_TX_IRQHandler               /* 79:CAN1 TX */
                .word     CAN1_RX0_IRQHandler              /* 80:CAN1 RX0 */
                .word     CAN1_RX1_IRQHandler              /* 81:CAN1 RX1 */
                .word     CAN1_EWMC_IRQHandler             /* 82:CAN1 EWMC */
                .word     USBFS_IRQHandler                 /* 83:USBFS */
                .word     DMA1_Channel5_IRQHandler         /* 84:DMA1 Channel5 */
                .word     DMA1_Channel6_IRQHandler         /* 85:DMA1 Channel6 */
                .word     DMA1_Channel7_IRQHandler         /* 86:DMA1 Channel7 */
                .word     USART5_IRQHandler                /* 87:USART5 */
                .word     I2C2_EV_IRQHandler               /* 88:I2C2 Event */
                .word     I2C2_ER_IRQHandler               /* 89:I2C2 Error */
                .word     USBHS_EP1_Out_IRQHandler         /* 90:USBHS Endpoint 1 Out  */
                .word     USBHS_EP1_In_IRQHandler          /* 91:USBHS Endpoint 1 in */
                .word     USBHS_WKUP_IRQHandler            /* 92:USBHS Wakeup through EXTI Line */
                .word     USBHS_IRQHandler                 /* 93:USBHS */
                .word     DCI_IRQHandler                   /* 94:DCI */
                .word     0                                /* 95:Reserved */
                .word     TRNG_IRQHandler                  /* 96:TRNG */
                .word     FPU_IRQHandler                   /* 97:FPU */
                .word     UART6_IRQHandler                 /* 98:UART6 */
                .word     UART7_IRQHandler                 /* 99:UART7 */
                .word     SPI3_IRQHandler                  /* 100:SPI3 */
                .word     SPI4_IRQHandler                  /* 101:SPI4 */
                .word     SPI5_IRQHandler                  /* 102:SPI5 */
                .word     0                                /* 103:Reserved */
                .word     TLI_IRQHandler                   /* 104:TLI */
                .word     TLI_ER_IRQHandler                /* 105:TLI Error */
                .word     IPA_IRQHandler                   /* 106:IPA */




  .weak NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak MemManage_Handler
  .thumb_set MemManage_Handler,Default_Handler

  .weak BusFault_Handler
  .thumb_set BusFault_Handler,Default_Handler
  
  .weak UsageFault_Handler
  .thumb_set UsageFault_Handler,Default_Handler
  
  .weak SVC_Handler
  .thumb_set SVC_Handler,Default_Handler
  
  .weak DebugMon_Handler
  .thumb_set DebugMon_Handler,Default_Handler
  
  .weak PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler

  .weak SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler

  .weak WWDGT_IRQHandler
  .thumb_set WWDG_IRQHandler,Default_Handler

  .weak LVD_IRQHandler
  .thumb_set LVD_IRQHandler,Default_Handler

  .weak TAMPER_STAMP_IRQHandler
  .thumb_set TAMPER_STAMP_IRQHandler,Default_Handler
  
  .weak RTC_WKUP_IRQHandler
  .thumb_set RTC_WKUP_IRQHandler,Default_Handler
  
  .weak FMC_IRQHandler
  .thumb_set FMC_IRQHandler,Default_Handler

  .weak RCU_CTC_IRQHandler
  .thumb_set RCU_CTC_IRQHandler,Default_Handler
  
  .weak EXTI0_IRQHandler
  .thumb_set EXTI0_IRQHandler,Default_Handler

  .weak EXTI1_IRQHandler
  .thumb_set EXTI1_IRQHandler,Default_Handler

  .weak EXTI2_IRQHandler
  .thumb_set EXTI2_IRQHandler,Default_Handler

  .weak EXTI3_IRQHandler
  .thumb_set EXTI3_IRQHandler,Default_Handler

  .weak EXTI4_IRQHandler
  .thumb_set EXTI4_IRQHandler,Default_Handler

  .weak DMA0_Channel0_IRQHandler
  .thumb_set DMA0_Channel0_IRQHandler,Default_Handler

  .weak DMA0_Channel1_IRQHandler
  .thumb_set DMA0_Channel1_IRQHandler,Default_Handler

  .weak DMA0_Channel2_IRQHandler
  .thumb_set DMA0_Channel2_IRQHandler,Default_Handler

  .weak DMA0_Channel3_IRQHandler
  .thumb_set DMA0_Channel3_IRQHandler,Default_Handler

  .weak DMA0_Channel4_IRQHandler
  .thumb_set DMA0_Channel4_IRQHandler,Default_Handler

  .weak DMA0_Channel5_IRQHandler
  .thumb_set DMA0_Channel5_IRQHandler,Default_Handler

  .weak DMA0_Channel6_IRQHandler
  .thumb_set DMA0_Channel6_IRQHandler,Default_Handler
 
  .weak ADC_IRQHandler
  .thumb_set ADC_IRQHandler,Default_Handler

  .weak CAN0_TX_IRQHandler
  .thumb_set CAN0_TX_IRQHandler,Default_Handler

  .weak CAN0_RX0_IRQHandler
  .thumb_set CAN0_RX0_IRQHandler,Default_Handler

  .weak CAN0_RX1_IRQHandler
  .thumb_set CAN0_RX1_IRQHandler,Default_Handler

  .weak CAN0_EWMC_IRQHandler
  .thumb_set CAN0_EWMC_IRQHandler,Default_Handler

  .weak EXTI5_9_IRQHandler
  .thumb_set EXTI5_9_IRQHandler,Default_Handler

  .weak TIMER0_BRK_TIMER8_IRQHandler
  .thumb_set TIMER0_BRK_TIMER8_IRQHandler,Default_Handler

  .weak TIMER0_UP_TIMER9_IRQHandler
  .thumb_set TIMER0_UP_TIMER9_IRQHandler,Default_Handler

  .weak TIMER0_TRG_CMT_TIMER10_IRQHandler
  .thumb_set TIMER0_TRG_CMT_TIMER10_IRQHandler,Default_Handler

  .weak TIMER0_CC_IRQHandler
  .thumb_set TIMER0_CC_IRQHandler,Default_Handler
  
  .weak TIMER1_IRQHandler
  .thumb_set TIMER1_IRQHandler,Default_Handler
  
  .weak TIMER2_IRQHandler
  .thumb_set TIMER2_IRQHandler,Default_Handler

  .weak TIMER3_IRQHandler
  .thumb_set TIMER3_IRQHandler,Default_Handler
  
  .weak I2C0_EV_IRQHandler
  .thumb_set I2C0_EV_IRQHandler,Default_Handler

  .weak I2C0_ER_IRQHandler
  .thumb_set I2C0_ER_IRQHandler,Default_Handler
  
  .weak I2C1_EV_IRQHandler
  .thumb_set I2C1_EV_IRQHandler,Default_Handler

  .weak I2C1_ER_IRQHandler
  .thumb_set I2C1_ER_IRQHandler,Default_Handler
  
  .weak SPI0_IRQHandler
  .thumb_set SPI0_IRQHandler,Default_Handler
  
  .weak SPI1_IRQHandler
  .thumb_set SPI1_IRQHandler,Default_Handler
  
  .weak USART0_IRQHandler
  .thumb_set USART0_IRQHandler,Default_Handler
  
  .weak USART1_IRQHandler
  .thumb_set USART1_IRQHandler,Default_Handler

  .weak USART2_IRQHandler
  .thumb_set USART2_IRQHandler,Default_Handler

  .weak EXTI10_15_IRQHandler
  .thumb_set EXTI10_15_IRQHandler,Default_Handler
  
  .weak RTC_Alarm_IRQHandler
  .thumb_set RTC_Alarm_IRQHandler,Default_Handler

  .weak USBFS_WKUP_IRQHandler 
  .thumb_set USBFS_WKUP_IRQHandler,Default_Handler
  
  .weak TIMER7_BRK_TIMER11_IRQHandler
  .thumb_set TIMER7_BRK_TIMER11_IRQHandler,Default_Handler
  
  .weak TIMER7_UP_TIMER12_IRQHandler
  .thumb_set TIMER7_UP_TIMER12_IRQHandler,Default_Handler
  
  .weak TIMER7_TRG_CMT_TIMER13_IRQHandler
  .thumb_set TIMER7_TRG_CMT_TIMER13_IRQHandler,Default_Handler
  
  .weak TIMER7_CC_IRQHandler
  .thumb_set TIMER7_CC_IRQHandler,Default_Handler

  .weak DMA0_Channel7_IRQHandler
  .thumb_set DMA0_Channel7_IRQHandler,Default_Handler
  
  .weak EXMC_IRQHandler
  .thumb_set EXMC_IRQHandler,Default_Handler

  .weak SDIO_IRQHandler
  .thumb_set SDIO_IRQHandler,Default_Handler
  
  .weak TIMER4_IRQHandler
  .thumb_set TIMER4_IRQHandler,Default_Handler

  .weak SPI2_IRQHandler
  .thumb_set SPI2_IRQHandler,Default_Handler

  .weak UART3_IRQHandler
  .thumb_set UART3_IRQHandler,Default_Handler
  
  .weak UART4_IRQHandler
  .thumb_set UART4_IRQHandler,Default_Handler

  .weak TIMER5_DAC_IRQHandler
  .thumb_set TIMER5_DAC_IRQHandler,Default_Handler

  .weak TIMER6_IRQHandler
  .thumb_set TIMER6_IRQHandler,Default_Handler

  .weak DMA1_Channel0_IRQHandler
  .thumb_set DMA1_Channel0_IRQHandler,Default_Handler

  .weak DMA1_Channel1_IRQHandler
  .thumb_set DMA1_Channel1_IRQHandler,Default_Handler

  .weak DMA1_Channel2_IRQHandler
  .thumb_set DMA1_Channel2_IRQHandler,Default_Handler

  .weak DMA1_Channel3_IRQHandler
  .thumb_set DMA1_Channel3_IRQHandler,Default_Handler

  .weak DMA1_Channel4_IRQHandler
  .thumb_set DMA1_Channel4_IRQHandler,Default_Handler
  
  .weak ENET_IRQHandler
  .thumb_set ENET_IRQHandler,Default_Handler
  
  .weak ENET_WKUP_IRQHandler
  .thumb_set ENET_WKUP_IRQHandler,Default_Handler
  
  .weak CAN1_TX_IRQHandler
  .thumb_set CAN1_TX_IRQHandler,Default_Handler
  
  .weak CAN1_RX0_IRQHandler
  .thumb_set CAN1_RX0_IRQHandler,Default_Handler
  
  .weak CAN1_RX1_IRQHandler
  .thumb_set CAN1_RX1_IRQHandler,Default_Handler
  
  .weak CAN1_EWMC_IRQHandler
  .thumb_set CAN1_EWMC_IRQHandler,Default_Handler
  
  .weak USBFS_IRQHandler
  .thumb_set USBFS_IRQHandler,Default_Handler  

  .weak DMA1_Channel5_IRQHandler
  .thumb_set DMA1_Channel5_IRQHandler,Default_Handler  
  
  .weak DMA1_Channel6_IRQHandler
  .thumb_set DMA1_Channel6_IRQHandler,Default_Handler  
  
  .weak DMA1_Channel7_IRQHandler
  .thumb_set DMA1_Channel7_IRQHandler,Default_Handler  
  
  .weak USART5_IRQHandler
  .thumb_set USART5_IRQHandler,Default_Handler  
  
  .weak I2C2_EV_IRQHandler
  .thumb_set I2C2_EV_IRQHandler,Default_Handler  
  
  .weak I2C2_ER_IRQHandler
  .thumb_set I2C2_ER_IRQHandler,Default_Handler  
  
  .weak USBHS_EP1_Out_IRQHandler
  .thumb_set USBHS_EP1_Out_IRQHandler,Default_Handler  
  
  .weak USBHS_EP1_In_IRQHandler
  .thumb_set USBHS_EP1_In_IRQHandler,Default_Handler  
  
  .weak USBHS_WKUP_IRQHandler
  .thumb_set USBHS_WKUP_IRQHandler,Default_Handler  
  
  .weak USBHS_IRQHandler
  .thumb_set USBHS_IRQHandler,Default_Handler  
  
  .weak DCI_IRQHandler
  .thumb_set DCI_IRQHandler,Default_Handler  
  
  .weak TRNG_IRQHandler
  .thumb_set TRNG_IRQHandler,Default_Handler  
  
  .weak FPU_IRQHandler
  .thumb_set FPU_IRQHandler,Default_Handler  
  
  .weak UART6_IRQHandler
  .thumb_set UART6_IRQHandler,Default_Handler  
  
  .weak UART7_IRQHandler
  .thumb_set UART7_IRQHandler,Default_Handler  
  
  .weak SPI3_IRQHandler
  .thumb_set SPI3_IRQHandler,Default_Handler  
  
  .weak SPI4_IRQHandler
  .thumb_set SPI4_IRQHandler,Default_Handler  
  
  .weak SPI5_IRQHandler
  .thumb_set SPI5_IRQHandler,Default_Handler  
  
  .weak TLI_IRQHandler
  .thumb_set TLI_IRQHandler,Default_Handler  
  
  .weak TLI_ER_IRQHandler
  .thumb_set TLI_ER_IRQHandler,Default_Handler  
  
  .weak IPA_IRQHandler
  .thumb_set IPA_IRQHandler,Default_Handler  


/******************* (C) COPYRIGHT 2014 GIGADEVICE *****END OF FILE****/

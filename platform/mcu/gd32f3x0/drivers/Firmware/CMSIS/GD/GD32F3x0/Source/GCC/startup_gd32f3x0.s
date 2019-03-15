/**
  ******************************************************************************
  * @file    startup_gd32f3x0.s
  * @author  MCU SD
  * @version V1.0.0
  * @date    26-Dec-2014
  * @brief   GD32F3X0 startup code.
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
                    .word Reset_Handler                     /* Vector Number 1,Reset Handler */
                    .word NMI_Handler                       /* Vector Number 2,NMI Handler */
                    .word HardFault_Handler                 /* Vector Number 3,Hard Fault Handler */
                    .word MemManage_Handler                 /* Vector Number 4,MPU Fault Handler */
                    .word BusFault_Handler                  /* Vector Number 5,Bus Fault Handler */
                    .word UsageFault_Handler                /* Vector Number 6,Usage Fault Handler */
                    .word 0                                 /* Reserved  */
                    .word 0                                 /* Reserved */
                    .word 0                                 /* Reserved */
                    .word 0                                 /* Reserved */
                    .word SVC_Handler                       /* Vector Number 11,SVCall Handler */
                    .word DebugMon_Handler                  /* Vector Number 12,Debug Monitor Handler */
                    .word 0                                 /* Reserved */
                    .word PendSV_Handler                    /* Vector Number 14,PendSV Handler */
                    .word SysTick_Handler                   /* Vector Number 15,SysTick Handler */

                    /* External Interrupts */
                    .word WWDGT_IRQHandler                  /* Vector Number 16,Window Watchdog */
                    .word LVD_IRQHandler                    /* Vector Number 17,LVD through EXTI Line detect */
                    .word RTC_IRQHandler                    /* Vector Number 18,RTC through EXTI Line */
                    .word FMC_IRQHandler                    /* Vector Number 19,FMC */
                    .word RCU_IRQHandler                    /* Vector Number 20,RCU */
                    .word EXTI0_1_IRQHandler                /* Vector Number 21,EXTI Line 0 and EXTI Line 1 */
                    .word EXTI2_3_IRQHandler                /* Vector Number 22,EXTI Line 2 and EXTI Line 3 */
                    .word EXTI4_15_IRQHandler               /* Vector Number 23,EXTI Line 4 to EXTI Line 15 */
                    .word TSI_IRQHandler                    /* Vector Number 24,TSI */
                    .word DMA_Channel0_IRQHandler           /* Vector Number 25,DMA Channel 0  */
                    .word DMA_Channel1_2_IRQHandler         /* Vector Number 26,DMA Channel 1 and DMA Channel 2 */
                    .word DMA_Channel3_4_IRQHandler         /* Vector Number 27,DMA Channel 3 and DMA Channel 4 */
                    .word ADC_CMP_IRQHandler                /* Vector Number 28,ADC and Comparator 0-1  */
                    .word TIMER0_BRK_UP_TRG_COM_IRQHandler  /* Vector Number 29,TIMER0 Break,Update,Trigger and Commutation */
                    .word TIMER0_CC_IRQHandler              /* Vector Number 30,TIMER0 Capture Compare  */
                    .word TIMER1_IRQHandler                 /* Vector Number 31,TIMER1  */
                    .word TIMER2_IRQHandler                 /* Vector Number 32,TIMER2  */
                    .word TIMER5_DAC_IRQHandler             /* Vector Number 33,TIMER5 and DAC */
                    .word 0                                 /* Vector Number 34,Reserved  */
                    .word TIMER13_IRQHandler                /* Vector Number 35,TIMER13  */
                    .word TIMER14_IRQHandler                /* Vector Number 36,TIMER14 */
                    .word TIMER15_IRQHandler                /* Vector Number 37,TIMER15 */
                    .word TIMER16_IRQHandler                /* Vector Number 38,TIMER16 */
                    .word I2C0_EV_IRQHandler                /* Vector Number 39,I2C0 Event */
                    .word I2C1_EV_IRQHandler                /* Vector Number 40,I2C1 Event */
                    .word SPI0_IRQHandler                   /* Vector Number 41,SPI0 */
                    .word SPI1_IRQHandler                   /* Vector Number 42,SPI1 */
                    .word USART0_IRQHandler                 /* Vector Number 43,USART0 */
                    .word USART1_IRQHandler                 /* Vector Number 44,USART1 */
                    .word 0                                 /* Vector Number 45,Reserved */
                    .word CEC_IRQHandler                    /* Vector Number 46,CEC */
                    .word 0                                 /* Vector Number 47,Reserved */
                    .word I2C0_ER_IRQHandler                /* Vector Number 48,I2C0 Error */
                    .word 0                                 /* Vector Number 49,Reserved */
                    .word I2C1_ER_IRQHandler                /* Vector Number 50,I2C1 Error */
                    .word I2C2_EV_IRQHandler                /* Vector Number 51,I2C2 Event */
                    .word I2C2_ER_IRQHandler                /* Vector Number 52,I2C2 Error */
                    .word 0                                 /* Vector Number 53,Reserved */
                    .word 0                                 /* Vector Number 54,Reserved */
                    .word 0                                 /* Vector Number 55,Reserved */
                    .word 0                                 /* Vector Number 56,Reserved */
                    .word 0                                 /* Vector Number 57,Reserved */
                    .word USBFS_WKUP_IRQHandler             /* Vector Number 58,USBFS Wakeup */
                    .word 0                                 /* Vector Number 59,Reserved */
                    .word 0                                 /* Vector Number 60,Reserved */ 
                    .word 0                                 /* Vector Number 61,Reserved */
                    .word 0                                 /* Vector Number 62,Reserved */
                    .word 0                                 /* Vector Number 63,Reserved */
                    .word DMA_Channel5_6_IRQHandler         /* Vector Number 64,DMA Channel5 and Channel6  */
                    .word 0                                 /* Vector Number 65,Reserved */
                    .word 0                                 /* Vector Number 66,Reserved */
                    .word SPI2_IRQHandler                   /* Vector Number 67,SPI2 */
                    .word 0                                 /* Vector Number 68,Reserved */
                    .word 0                                 /* Vector Number 69,Reserved */
                    .word 0                                 /* Vector Number 70,Reserved */
                    .word 0                                 /* Vector Number 71,Reserved */
                    .word 0                                 /* Vector Number 72,Reserved */
                    .word 0                                 /* Vector Number 73,Reserved */
                    .word 0                                 /* Vector Number 74,Reserved */
                    .word 0                                 /* Vector Number 75,Reserved */
                    .word 0                                 /* Vector Number 76,Reserved */
                    .word 0                                 /* Vector Number 77,Reserved */
                    .word 0                                 /* Vector Number 78,Reserved */
                    .word 0                                 /* Vector Number 79,Reserved */
                    .word 0                                 /* Vector Number 80,Reserved */
                    .word 0                                 /* Vector Number 81,Reserved */
                    .word 0                                 /* Vector Number 82,Reserved */
                    .word USBFS_IRQHandler                  /* Vector Number 83,USBFS */



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
  
  .weak RTC_IRQHandler
  .thumb_set RTC_IRQHandler,Default_Handler
  
  .weak FMC_IRQHandler
  .thumb_set FMC_IRQHandler,Default_Handler

  .weak RCU_IRQHandler
  .thumb_set RCU_IRQHandler,Default_Handler
  
  .weak EXTI0_1_IRQHandler
  .thumb_set EXTI0_1_IRQHandler,Default_Handler

  .weak EXTI2_3_IRQHandler
  .thumb_set EXTI2_3_IRQHandler,Default_Handler

  .weak EXTI4_15_IRQHandler
  .thumb_set EXTI4_15_IRQHandler,Default_Handler

  .weak TSI_IRQHandler
  .thumb_set TSI_IRQHandler,Default_Handler

  .weak DMA_Channel0_IRQHandler
  .thumb_set DMA_Channel0_IRQHandler,Default_Handler

  .weak DMA_Channel1_2_IRQHandler
  .thumb_set DMA_Channel1_2_IRQHandler,Default_Handler

  .weak DMA_Channel3_4_IRQHandler
  .thumb_set DMA_Channel3_4_IRQHandler,Default_Handler

  .weak ADC_CMP_IRQHandler
  .thumb_set ADC_CMP_IRQHandler,Default_Handler

  .weak TIMER0_BRK_UP_TRG_COM_IRQHandler
  .thumb_set TIMER0_BRK_UP_TRG_COM_IRQHandler,Default_Handler

  .weak TIMER0_CC_IRQHandler
  .thumb_set TIMER0_CC_IRQHandler,Default_Handler

  .weak TIMER1_IRQHandler
  .thumb_set TIMER1_IRQHandler,Default_Handler

  .weak TIMER2_IRQHandler
  .thumb_set TIMER2_IRQHandler,Default_Handler
 
  .weak TIMER5_DAC_IRQHandler
  .thumb_set TIMER5_DAC_IRQHandler,Default_Handler

  .weak TIMER13_IRQHandler
  .thumb_set TIMER13_IRQHandler,Default_Handler

  .weak TIMER14_IRQHandler
  .thumb_set TIMER14_IRQHandler,Default_Handler

  .weak TIMER15_IRQHandler
  .thumb_set TIMER15_IRQHandler,Default_Handler

  .weak TIMER16_IRQHandler
  .thumb_set TIMER16_IRQHandler,Default_Handler

  .weak I2C0_EV_IRQHandler
  .thumb_set I2C0_EV_IRQHandler,Default_Handler

  .weak I2C1_EV_IRQHandler
  .thumb_set I2C1_EV_IRQHandler,Default_Handler

  .weak SPI0_IRQHandler
  .thumb_set SPI0_IRQHandler,Default_Handler

  .weak SPI1_IRQHandler
  .thumb_set SPI1_IRQHandler,Default_Handler

  .weak USART0_IRQHandler
  .thumb_set USART0_IRQHandler,Default_Handler
  
  .weak USART1_IRQHandler
  .thumb_set USART1_IRQHandler,Default_Handler
  
  .weak CEC_IRQHandler
  .thumb_set CEC_IRQHandler,Default_Handler

  .weak I2C0_ER_IRQHandler
  .thumb_set I2C0_ER_IRQHandler,Default_Handler
  
  .weak I2C1_ER_IRQHandler
  .thumb_set I2C1_ER_IRQHandler,Default_Handler

  .weak I2C2_EV_IRQHandler
  .thumb_set I2C2_EV_IRQHandler,Default_Handler
  
  .weak I2C2_ER_IRQHandler
  .thumb_set I2C2_ER_IRQHandler,Default_Handler

  .weak USBFS_WKUP_IRQHandler
  .thumb_set USBFS_WKUP_IRQHandler,Default_Handler
  
  .weak DMA_Channel5_6_IRQHandler
  .thumb_set DMA_Channel5_6_IRQHandler,Default_Handler
  
  .weak SPI2_IRQHandler
  .thumb_set SPI2_IRQHandler,Default_Handler
  
  .weak USBFS_IRQHandler
  .thumb_set USBFS_IRQHandler,Default_Handler

/******************* (C) COPYRIGHT 2014 GIGADEVICE *****END OF FILE****/

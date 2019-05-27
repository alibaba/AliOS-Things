
/**************************  startup_cm4.s   **********************************************/
  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb

.global  g_pfnVectors
.global  Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss
/* stack used for SystemInit_ExtMemCtl; always internal RAM used */

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

  .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:
  ldr   sp, =_estack     /* set stack pointer */

/* Copy the data segment initializers from flash to SRAM */
  movs  r1, #0
  b  LoopCopyDataInit

CopyDataInit:
  ldr  r3, =_sidata
  ldr  r3, [r3, r1]
  str  r3, [r0, r1]
  adds  r1, r1, #4

LoopCopyDataInit:
  ldr  r0, =_sdata
  ldr  r3, =_edata
  adds  r2, r0, r1
  cmp  r2, r3
  bcc  CopyDataInit
  ldr  r2, =_sbss
  b  LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
  movs  r3, #0
  str  r3, [r2], #4

LoopFillZerobss:
  ldr  r3, = _ebss
  cmp  r2, r3
  bcc  FillZerobss

/* Call the clock system intitialization function.*/
  bl  SystemInit
/* Call static constructors */
 /*   bl __libc_init_array */
/* Call the application's entry point.*/
  bl  main
  bx  lr
.size  Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 * @param  None
 * @retval None
*/
    .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b  Infinite_Loop
  .size  Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex M3. Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
*******************************************************************************/
   .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors


g_pfnVectors:
  .word  _estack
  .word  Reset_Handler
  .word  NMI_Handler
  .word  HardFault_Handler
  .word  MemManage_Handler
  .word  BusFault_Handler
  .word  UsageFault_Handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  SVC_Handler
  .word  DebugMon_Handler
  .word  0
  .word  PendSV_Handler
  .word  SysTick_Handler

  /* External Interrupts */
  .word     intc_irq             /* intc_irq CEVA RW IP Interrupt         */
  .word     SLEEP_IRQHandler                  /* Sleep Wake-Up Interrupt      */
  .word     WDG_IRQHandler                    /* Window WatchDog              */
  .word     FLASH_IRQHandler                  /* FLASH                        */
  .word     GPIO_IRQHandler                   /* GPIO                         */
  .word     TIMER_IRQHandler                  /* Timer interrupt              */
  .word     CRYPTOCELL310_IRQHandler          /* CryptoCell 310 Interrupt     */
  .word     DMA_CTRL_IRQHandler               /* Generic DMA Ctrl Interrupt   */
  .word     UART0_IRQHandler                  /* UART0 Interrupt              */
  .word     UART1_IRQHandler                  /* UART1 Interrupt              */
  .word     UART2_IRQHandler                  /* UART2 Interrupt              */
  .word     SPI0_IRQHandler                   /* SPI0                         */
  .word     SPI1_IRQHandler                   /* SPI1                         */
  .word     SPI2_IRQHandler                   /* SPI2                         */
  .word     I2C0_IRQHandler                   /* I2C0                         */
  .word     I2C1_IRQHandler                   /* I2C1                         */
  .word     SDIO_IRQHandler                   /* SDIO Combined Interrupt      */
  .word     D_APLL_UNLOCK_IRQHandler          /* RF added: D_APLL_UNLOCK      */
  .word     D_SX_UNLOCK_IRQHandler            /* RF added: D_SX_UNLOCK        */
  .word     0x00000000
  .word     0x00000000
  .word     0x00000000
  .word     0x00000000
  .word     PLATFORM_WAKEUP_IRQHandler             /*!< WiFi SOC Wake-Up Interrupt */
  .word     HCLK_SW_IRQHandler


/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/
   .weak      NMI_Handler
   .thumb_set NMI_Handler,Default_Handler

   .weak      HardFault_Handler
   .thumb_set HardFault_Handler,Default_Handler

   .weak      MemManage_Handler
   .thumb_set MemManage_Handler,Default_Handler

   .weak      BusFault_Handler
   .thumb_set BusFault_Handler,Default_Handler

   .weak      UsageFault_Handler
   .thumb_set UsageFault_Handler,Default_Handler

   .weak      SVC_Handler
   .thumb_set SVC_Handler,Default_Handler

   .weak      DebugMon_Handler
   .thumb_set DebugMon_Handler,Default_Handler

   .weak      PendSV_Handler
   .thumb_set PendSV_Handler,Default_Handler

   .weak      SysTick_Handler
   .thumb_set SysTick_Handler,Default_Handler

   .weak      intc_irq
   .thumb_set intc_irq,Default_Handler

   .weak      SLEEP_IRQHandler
   .thumb_set SLEEP_IRQHandler,Default_Handler

   .weak      WDG_IRQHandler
   .thumb_set WDG_IRQHandler,Default_Handler

   .weak      FLASH_IRQHandler
   .thumb_set FLASH_IRQHandler,Default_Handler

   .weak      GPIO_IRQHandler
   .thumb_set GPIO_IRQHandler,Default_Handler

   .weak      TIMER_IRQHandler
   .thumb_set TIMER_IRQHandler,Default_Handler

   .weak      CRYPTOCELL310_IRQHandler
   .thumb_set CRYPTOCELL310_IRQHandler,Default_Handler

   .weak      DMA_CTRL_IRQHandler
   .thumb_set DMA_CTRL_IRQHandler,Default_Handler

   .weak      UART0_IRQHandler
   .thumb_set UART0_IRQHandler,Default_Handler

   .weak      UART1_IRQHandler
   .thumb_set UART1_IRQHandler,Default_Handler

   .weak      UART2_IRQHandler
   .thumb_set UART2_IRQHandler,Default_Handler

   .weak      SPI0_IRQHandler
   .thumb_set SPI0_IRQHandler,Default_Handler

   .weak      SPI1_IRQHandler
   .thumb_set SPI1_IRQHandler,Default_Handler

   .weak      SPI2_IRQHandler
   .thumb_set SPI2_IRQHandler,Default_Handler

   .weak      I2C0_IRQHandler
   .thumb_set I2C0_IRQHandler,Default_Handler

   .weak      I2C1_IRQHandler
   .thumb_set I2C1_IRQHandler,Default_Handler

   .weak      SDIO_IRQHandler
   .thumb_set SDIO_IRQHandler,Default_Handler

   .weak      D_APLL_UNLOCK_IRQHandler
   .thumb_set D_APLL_UNLOCK_IRQHandler,Default_Handler

   .weak      D_SX_UNLOCK_IRQHandler
   .thumb_set D_SX_UNLOCK_IRQHandler,Default_Handler

   .weak      PLATFORM_WAKEUP_IRQHandler
   .thumb_set PLATFORM_WAKEUP_IRQHandler,Default_Handler

   .weak      HCLK_SW_IRQHandler
   .thumb_set HCLK_SW_IRQHandler,Default_Handler


/*****************END OF FILE*********************/

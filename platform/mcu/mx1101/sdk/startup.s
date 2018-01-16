  .syntax unified
  .cpu cortex-m3
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
  ldr   sp, =0x20020000    		 /* set stack pointer */

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
    .word     _estack
    .word     Reset_Handler
    .word     NMI_Handler
    .word     HardFault_Handler 
    .word     MemManage_Handler
    .word     BusFault_Handler 
    .word     UsageFault_Handler 
    .word     0       
    .word     0       
    .word     0       
    .word     0       
    .word     SVC_Handler   
    .word     0                    
    .word     0       
    .word     PendSV_Handler
    .word     SysTick_Handler

    .word     GpioInterrupt          
    .word     RtcInterrupt           
    .word     IrInterrupt            
    .word     FuartInterrupt         
    .word     BuartInterrupt  
    .word     PwcInterrupt    
    .word     Timer0Interrupt        
    .word     UsbInterrupt      
    .word     DmaCh0Interrupt        
    .word     DmaCh1Interrupt       
    .word     AudioDecoderInterrupt
    .word     SpisInterrupt
    .word     SdInterrupt 
    .word     SpimInterrupt    
    .word     Timer1Interrupt
    .word     WatchDogInterrupt 
                    
/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler. 
* As they are weak aliases, any function with the same name will override 
* this definition.
* 
*******************************************************************************/
   .weak      NMI_Handler
   NMI_Handler:
   B .
  
   .weak      HardFault_Handler
   HardFault_Handler:
   B .
  
   .weak      MemManage_Handler
   MemManage_Handler:
   B .
  
   .weak      BusFault_Handler
   BusFault_Handler:
   B .

   .weak      UsageFault_Handler
   UsageFault_Handler:
   B .

   .weak      SVC_Handler
   .thumb_set SVC_Handler,Default_Handler

   .weak      DebugMon_Handler
   .thumb_set DebugMon_Handler,Default_Handler

   .weak      PendSV_Handler
   .thumb_set PendSV_Handler,Default_Handler

   .weak      SysTick_Handler
   .thumb_set SysTick_Handler,Default_Handler              
   
   .weak      GpioInterrupt
   .thumb_set GpioInterrupt,Default_Handler    
   
   .weak      RtcInterrupt   
   .thumb_set RtcInterrupt,Default_Handler   
   
   .weak      IrInterrupt  
   .thumb_set IrInterrupt,Default_Handler   
   
   .weak      FuartInterrupt     
   .thumb_set FuartInterrupt,Default_Handler    
   
   .weak      BuartInterrupt   
   .thumb_set BuartInterrupt,Default_Handler
   
   .weak      PwcInterrupt  
   .thumb_set PwcInterrupt,Default_Handler
   
   .weak      Timer0Interrupt  
   .thumb_set Timer0Interrupt,Default_Handler  
   
   .weak      UsbInterrupt   
   .thumb_set UsbInterrupt,Default_Handler
   
   .weak      DmaCh0Interrupt  
   .thumb_set DmaCh0Interrupt,Default_Handler 
   
   .weak      DmaCh1Interrupt  
   .thumb_set DmaCh1Interrupt,Default_Handler
   
   .weak      AudioDecoderInterrupt  
   .thumb_set AudioDecoderInterrupt,Default_Handler
   
   .weak      SpisInterrupt  
   .thumb_set SpisInterrupt,Default_Handler
   
   .weak      SdInterrupt  
   .thumb_set SdInterrupt,Default_Handler
   
   .weak      SpimInterrupt  
   .thumb_set SpimInterrupt,Default_Handler
   
   .weak      Timer1Interrupt  
   .thumb_set Timer1Interrupt,Default_Handler
   
   .weak      WatchDogInterrupt  
   .thumb_set WatchDogInterrupt,Default_Handler 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


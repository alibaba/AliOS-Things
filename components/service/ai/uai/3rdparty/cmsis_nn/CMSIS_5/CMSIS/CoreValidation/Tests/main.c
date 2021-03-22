/*----------------------------------------------------------------------------
 * Name:    main.c
 *----------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "RTE_Components.h"
#include  CMSIS_device_header
 
#ifdef RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#endif

#include "cmsis_cv.h"
#include "CV_Report.h"

//lint -e970 allow using int for main

/* Private functions ---------------------------------------------------------*/
int main (void);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main (void)
{  

  // System Initialization
  SystemCoreClockUpdate();
#ifdef RTE_Compiler_EventRecorder
  // Initialize and start Event Recorder
  (void)EventRecorderInitialize(EventRecordError, 1U);
  (void)EventRecorderEnable    (EventRecordAll, 0xFEU, 0xFEU);
#endif
  
  cmsis_cv();
  
  #ifdef __MICROLIB
  for(;;) {}
  #else
  exit(0);
  #endif
}

#if defined(__CORTEX_A)
#include "irq_ctrl.h"

#if (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) || \
  (defined ( __GNUC__ ))
#define __IRQ __attribute__((interrupt("IRQ")))
#elif defined ( __CC_ARM )
#define __IRQ __irq
#elif defined ( __ICCARM__ )
#define __IRQ __irq __arm
#else
#error "Unsupported compiler!"
#endif

__IRQ
void IRQ_Handler(void) {
  const IRQn_ID_t irqn = IRQ_GetActiveIRQ();
  IRQHandler_t const handler = IRQ_GetHandler(irqn);
  if (handler != NULL) {
    __enable_irq();
    handler();
    __disable_irq();
  }
  IRQ_EndOfInterrupt(irqn);
}

__IRQ
void Undef_Handler (void) {
  cmsis_cv_abort(__FILENAME__, __LINE__, "Undefined Instruction!");
  exit(0);
}

__IRQ
void SVC_Handler   (void) {
}

__IRQ
void PAbt_Handler  (void) {
  cmsis_cv_abort(__FILENAME__, __LINE__, "Prefetch Abort!");
  exit(0);
}

__IRQ
void DAbt_Handler  (void) {
  cmsis_cv_abort(__FILENAME__, __LINE__, "Data Abort!");
  exit(0);
}

__IRQ
void FIQ_Handler   (void) {
}
#endif

#if defined(__CORTEX_M)
__NO_RETURN
extern void HardFault_Handler(void);
void HardFault_Handler(void) {
  cmsis_cv_abort(__FILENAME__, __LINE__, "HardFault!");
  #ifdef __MICROLIB
  for(;;) {}
  #else
  exit(0);
  #endif
}
#endif

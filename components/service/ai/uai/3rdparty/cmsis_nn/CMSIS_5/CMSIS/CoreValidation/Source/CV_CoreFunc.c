/*-----------------------------------------------------------------------------
 *      Name:         CV_CoreFunc.c
 *      Purpose:      CMSIS CORE validation tests implementation
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2017 - 2019 Arm Limited. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "CV_Framework.h"
#include "cmsis_cv.h"

/*-----------------------------------------------------------------------------
 *      Test implementation
 *----------------------------------------------------------------------------*/

static volatile uint32_t irqTaken = 0U;
#if defined(__CORTEX_M) && (__CORTEX_M > 0)
static volatile uint32_t irqActive = 0U;
#endif

static void TC_CoreFunc_EnDisIRQIRQHandler(void) {
  ++irqTaken;
#if defined(__CORTEX_M) && (__CORTEX_M > 0)
  irqActive = NVIC_GetActive(Interrupt0_IRQn);
#endif
}

static volatile uint32_t irqIPSR = 0U;
static volatile uint32_t irqXPSR = 0U;

static void TC_CoreFunc_IPSR_IRQHandler(void) {
  irqIPSR = __get_IPSR();
  irqXPSR = __get_xPSR();
}

/*-----------------------------------------------------------------------------
 *      Test cases
 *----------------------------------------------------------------------------*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_EnDisIRQ
\details
Check expected behavior of interrupt related control functions:
- __disable_irq() and __enable_irq()
- NVIC_EnableIRQ, NVIC_DisableIRQ,  and NVIC_GetEnableIRQ
- NVIC_SetPendingIRQ, NVIC_ClearPendingIRQ, and NVIC_GetPendingIRQ
- NVIC_GetActive (not on Cortex-M0/M0+)
*/
void TC_CoreFunc_EnDisIRQ (void)
{
  // Globally disable all interrupt servicing
  __disable_irq();

  // Enable the interrupt
  NVIC_EnableIRQ(Interrupt0_IRQn);
  ASSERT_TRUE(NVIC_GetEnableIRQ(Interrupt0_IRQn) != 0U);

  // Clear its pending state
  NVIC_ClearPendingIRQ(Interrupt0_IRQn);
  ASSERT_TRUE(NVIC_GetPendingIRQ(Interrupt0_IRQn) == 0U);

  // Register test interrupt handler.
  TST_IRQHandler = TC_CoreFunc_EnDisIRQIRQHandler;
  irqTaken = 0U;
#if defined(__CORTEX_M) && (__CORTEX_M > 0)
  irqActive = UINT32_MAX;
#endif

  // Set the interrupt pending state
  NVIC_SetPendingIRQ(Interrupt0_IRQn);
  for(uint32_t i = 10U; i > 0U; --i) {}

  // Interrupt is not taken
  ASSERT_TRUE(irqTaken == 0U);
  ASSERT_TRUE(NVIC_GetPendingIRQ(Interrupt0_IRQn) != 0U);
#if defined(__CORTEX_M) && (__CORTEX_M > 0)
  ASSERT_TRUE(NVIC_GetActive(Interrupt0_IRQn) == 0U);
#endif

  // Globally enable interrupt servicing
  __enable_irq();

  for(uint32_t i = 10U; i > 0U; --i) {}

  // Interrupt was taken
  ASSERT_TRUE(irqTaken == 1U);
#if defined(__CORTEX_M) && (__CORTEX_M > 0)
  ASSERT_TRUE(irqActive != 0U);
  ASSERT_TRUE(NVIC_GetActive(Interrupt0_IRQn) == 0U);
#endif

  // Interrupt it not pending anymore.
  ASSERT_TRUE(NVIC_GetPendingIRQ(Interrupt0_IRQn) == 0U);

  // Disable interrupt
  NVIC_DisableIRQ(Interrupt0_IRQn);
  ASSERT_TRUE(NVIC_GetEnableIRQ(Interrupt0_IRQn) == 0U);

  // Set interrupt pending
  NVIC_SetPendingIRQ(Interrupt0_IRQn);
  for(uint32_t i = 10U; i > 0U; --i) {}

  // Interrupt is not taken again
  ASSERT_TRUE(irqTaken == 1U);
  ASSERT_TRUE(NVIC_GetPendingIRQ(Interrupt0_IRQn) != 0U);

  // Clear interrupt pending
  NVIC_ClearPendingIRQ(Interrupt0_IRQn);
  for(uint32_t i = 10U; i > 0U; --i) {}

  // Interrupt it not pending anymore.
  ASSERT_TRUE(NVIC_GetPendingIRQ(Interrupt0_IRQn) == 0U);

  // Globally disable interrupt servicing
  __disable_irq();
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_IRQPrio
\details
Check expected behavior of interrupt priority control functions:
- NVIC_SetPriority, NVIC_GetPriority
*/
void TC_CoreFunc_IRQPrio (void)
{
  /* Test Exception Priority */
  uint32_t orig = NVIC_GetPriority(SVCall_IRQn);

  NVIC_SetPriority(SVCall_IRQn, orig+1U);
  uint32_t prio = NVIC_GetPriority(SVCall_IRQn);

  ASSERT_TRUE(prio == orig+1U);

  NVIC_SetPriority(SVCall_IRQn, orig);

  /* Test Interrupt Priority */
  orig = NVIC_GetPriority(Interrupt0_IRQn);

  NVIC_SetPriority(Interrupt0_IRQn, orig+1U);
  prio = NVIC_GetPriority(Interrupt0_IRQn);

  ASSERT_TRUE(prio == orig+1U);

  NVIC_SetPriority(Interrupt0_IRQn, orig);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/** Helper function for TC_CoreFunc_EncDecIRQPrio
\details
The helper encodes and decodes the given priority configuration.
\param[in] prigroup The PRIGROUP setting to be considered for encoding/decoding.
\param[in] pre The preempt priority value.
\param[in] sub The subpriority value.
*/
static void TC_CoreFunc_EncDecIRQPrio_Step(uint32_t prigroup, uint32_t pre, uint32_t sub) {
  uint32_t prio = NVIC_EncodePriority(prigroup, pre, sub);

  uint32_t ret_pre = UINT32_MAX;
  uint32_t ret_sub = UINT32_MAX;

  NVIC_DecodePriority(prio, prigroup, &ret_pre, &ret_sub);

  ASSERT_TRUE(ret_pre == pre);
  ASSERT_TRUE(ret_sub == sub);
}

/**
\brief Test case: TC_CoreFunc_EncDecIRQPrio
\details
Check expected behavior of interrupt priority encoding/decoding functions:
- NVIC_EncodePriority, NVIC_DecodePriority
*/
void TC_CoreFunc_EncDecIRQPrio (void)
{
  /* Check only the valid range of PRIGROUP and preempt-/sub-priority values. */
  static const uint32_t priobits = (__NVIC_PRIO_BITS > 7U) ? 7U : __NVIC_PRIO_BITS;
  for(uint32_t prigroup = 7U-priobits; prigroup<7U; prigroup++) {
    for(uint32_t pre = 0U; pre<(128U>>prigroup); pre++) {
      for(uint32_t sub = 0U; sub<(256U>>(8U-__NVIC_PRIO_BITS+7U-prigroup)); sub++) {
        TC_CoreFunc_EncDecIRQPrio_Step(prigroup, pre, sub);
      }
    }
  }
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_IRQVect
\details
Check expected behavior of interrupt vector relocation functions:
- NVIC_SetVector, NVIC_GetVector
*/
void TC_CoreFunc_IRQVect(void) {
#if defined(__VTOR_PRESENT) && __VTOR_PRESENT
  /* relocate vector table */
  extern uint32_t __VECTOR_TABLE[];
  static uint32_t vectors[32] __ALIGNED(512);

  for(uint32_t i=0U; i<32U; i++) {
    vectors[i] = __VECTOR_TABLE[i];
  }

  const uint32_t orig_vtor = SCB->VTOR;
  const uint32_t vtor = ((uint32_t)vectors) & SCB_VTOR_TBLOFF_Msk;
  SCB->VTOR = vtor;

  ASSERT_TRUE(vtor == SCB->VTOR);

  /* check exception vectors */
  extern void HardFault_Handler(void);
  extern void SVC_Handler(void);
  extern void PendSV_Handler(void);
  extern void SysTick_Handler(void);

  ASSERT_TRUE(NVIC_GetVector(HardFault_IRQn) == (uint32_t)HardFault_Handler);
  ASSERT_TRUE(NVIC_GetVector(SVCall_IRQn) == (uint32_t)SVC_Handler);
  ASSERT_TRUE(NVIC_GetVector(PendSV_IRQn) == (uint32_t)PendSV_Handler);
  ASSERT_TRUE(NVIC_GetVector(SysTick_IRQn) == (uint32_t)SysTick_Handler);

  /* reconfigure WDT IRQ vector */
  extern void Interrupt0_Handler(void);

  const uint32_t wdtvec = NVIC_GetVector(Interrupt0_IRQn);
  ASSERT_TRUE(wdtvec == (uint32_t)Interrupt0_Handler);

  NVIC_SetVector(Interrupt0_IRQn, wdtvec + 32U);

  ASSERT_TRUE(NVIC_GetVector(Interrupt0_IRQn) == (wdtvec + 32U));

  /* restore vector table */
  SCB->VTOR = orig_vtor;
#endif
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_GetCtrl
\details
- Check if __set_CONTROL and __get_CONTROL() sets/gets control register
*/
void TC_CoreFunc_Control (void) {
  // don't use stack for this variables
  static uint32_t orig;
  static uint32_t ctrl;
  static uint32_t result;

  orig = __get_CONTROL();
  ctrl = orig;
  result = UINT32_MAX;

#ifdef CONTROL_SPSEL_Msk
  // SPSEL set to 0 (MSP)
  ASSERT_TRUE((ctrl & CONTROL_SPSEL_Msk) == 0U);

  // SPSEL set to 1 (PSP)
  ctrl |= CONTROL_SPSEL_Msk;

  // Move MSP to PSP
  __set_PSP(__get_MSP());
#endif

  __set_CONTROL(ctrl);
  __ISB();

  result = __get_CONTROL();

  __set_CONTROL(orig);
  __ISB();

  ASSERT_TRUE(result == ctrl);
  ASSERT_TRUE(__get_CONTROL() == orig);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_IPSR
\details
- Check if __get_IPSR intrinsic is available
- Check if __get_xPSR intrinsic is available
- Result differentiates between thread and exception modes
*/
void TC_CoreFunc_IPSR (void) {
  uint32_t result = __get_IPSR();
  ASSERT_TRUE(result == 0U); // Thread Mode

  result = __get_xPSR();
  ASSERT_TRUE((result & xPSR_ISR_Msk) == 0U); // Thread Mode

  TST_IRQHandler = TC_CoreFunc_IPSR_IRQHandler;
  irqIPSR = 0U;
  irqXPSR = 0U;

  NVIC_ClearPendingIRQ(Interrupt0_IRQn);
  NVIC_EnableIRQ(Interrupt0_IRQn);
  __enable_irq();

  NVIC_SetPendingIRQ(Interrupt0_IRQn);
  for(uint32_t i = 10U; i > 0U; --i) {}

  __disable_irq();
  NVIC_DisableIRQ(Interrupt0_IRQn);

  ASSERT_TRUE(irqIPSR != 0U); // Exception Mode
  ASSERT_TRUE((irqXPSR & xPSR_ISR_Msk) != 0U); // Exception Mode
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

#if defined(__CC_ARM)
#define SUBS(Rd, Rm, Rn) __ASM volatile("SUBS " # Rd ", " # Rm ", " # Rn)
#define ADDS(Rd, Rm, Rn) __ASM volatile("ADDS " # Rd ", " # Rm ", " # Rn)
#elif defined( __GNUC__ )  && (!defined(__ARMCC_VERSION))  && (defined(__ARM_ARCH_6M__) || defined(__ARM_ARCH_8M_BASE__))
#define SUBS(Rd, Rm, Rn) __ASM volatile("SUB %0, %1, %2" : "=r"(Rd) : "r"(Rm), "r"(Rn) : "cc")
#define ADDS(Rd, Rm, Rn) __ASM volatile("ADD %0, %1, %2" : "=r"(Rd) : "r"(Rm), "r"(Rn) : "cc")
#elif defined(_lint)
//lint -save -e(9026) allow function-like macro
#define SUBS(Rd, Rm, Rn) ((Rd) = (Rm) - (Rn))
#define ADDS(Rd, Rm, Rn) ((Rd) = (Rm) + (Rn))
//lint -restore
#else
#define SUBS(Rd, Rm, Rn) __ASM volatile("SUBS %0, %1, %2" : "=r"(Rd) : "r"(Rm), "r"(Rn) : "cc")
#define ADDS(Rd, Rm, Rn) __ASM volatile("ADDS %0, %1, %2" : "=r"(Rd) : "r"(Rm), "r"(Rn) : "cc")
#endif

/**
\brief Test case: TC_CoreFunc_APSR
\details
- Check if __get_APSR intrinsic is available
- Check if __get_xPSR intrinsic is available
- Check negative, zero and overflow flags
*/
void TC_CoreFunc_APSR (void) {
  volatile uint32_t result;
  //lint -esym(838, Rm) unused values
  //lint -esym(438, Rm) unused values

  // Check negative flag
  volatile int32_t Rm = 5;
  volatile int32_t Rn = 7;
  SUBS(Rm, Rm, Rn);
  result  = __get_APSR();
  ASSERT_TRUE((result & APSR_N_Msk) == APSR_N_Msk);

  Rm = 5;
  Rn = 7;
  SUBS(Rm, Rm, Rn);
  result  = __get_xPSR();
  ASSERT_TRUE((result & xPSR_N_Msk) == xPSR_N_Msk);

  // Check zero and compare flag
  Rm = 5;
  SUBS(Rm, Rm, Rm);
  result  = __get_APSR();
  ASSERT_TRUE((result & APSR_Z_Msk) == APSR_Z_Msk);
  ASSERT_TRUE((result & APSR_C_Msk) == APSR_C_Msk);

  Rm = 5;
  SUBS(Rm, Rm, Rm);
  result  = __get_xPSR();
  ASSERT_TRUE((result & xPSR_Z_Msk) == xPSR_Z_Msk);
  ASSERT_TRUE((result & APSR_C_Msk) == APSR_C_Msk);

  // Check overflow flag
  Rm = 5;
  Rn = INT32_MAX;
  ADDS(Rm, Rm, Rn);
  result  = __get_APSR();
  ASSERT_TRUE((result & APSR_V_Msk) == APSR_V_Msk);

  Rm = 5;
  Rn = INT32_MAX;
  ADDS(Rm, Rm, Rn);
  result  = __get_xPSR();
  ASSERT_TRUE((result & xPSR_V_Msk) == xPSR_V_Msk);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_PSP
\details
- Check if __get_PSP and __set_PSP intrinsic can be used to manipulate process stack pointer.
*/
void TC_CoreFunc_PSP (void) {
  // don't use stack for this variables
  static uint32_t orig;
  static uint32_t psp;
  static uint32_t result;

  orig = __get_PSP();

  psp = orig + 0x12345678U;
  __set_PSP(psp);

  result = __get_PSP();

  __set_PSP(orig);

  ASSERT_TRUE(result == psp);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_MSP
\details
- Check if __get_MSP and __set_MSP intrinsic can be used to manipulate main stack pointer.
*/
void TC_CoreFunc_MSP (void) {
  // don't use stack for this variables
  static uint32_t orig;
  static uint32_t msp;
  static uint32_t result;
  static uint32_t ctrl;

  ctrl = __get_CONTROL();
  orig = __get_MSP();

  __set_PSP(orig);
  __set_CONTROL(ctrl | CONTROL_SPSEL_Msk); // switch to PSP

  msp = orig + 0x12345678U;
  __set_MSP(msp);

  result = __get_MSP();

  __set_MSP(orig);

  __set_CONTROL(ctrl);

  ASSERT_TRUE(result == msp);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_PSPLIM
\details
- Check if __get_PSPLIM and __set_PSPLIM intrinsic can be used to manipulate process stack pointer limit.
*/
void TC_CoreFunc_PSPLIM (void) {
#if ((defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) || \
     (defined (__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1))    )
  // don't use stack for this variables
  static uint32_t orig;
  static uint32_t psplim;
  static uint32_t result;

  orig = __get_PSPLIM();

  psplim = orig + 0x12345678U;
  __set_PSPLIM(psplim);

  result = __get_PSPLIM();

  __set_PSPLIM(orig);

#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     (!defined (__ARM_FEATURE_CMSE  ) || (__ARM_FEATURE_CMSE   < 3)))
  // without main extensions, the non-secure PSPLIM is RAZ/WI
  ASSERT_TRUE(result == 0U);
#else
  ASSERT_TRUE(result == psplim);
#endif

#endif
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_PSPLIM_NS
\details
- Check if __TZ_get_PSPLIM_NS and __TZ_set_PSPLIM_NS intrinsic can be used to manipulate process stack pointer limit.
*/
void TC_CoreFunc_PSPLIM_NS (void) {
#if ((defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) || \
     (defined (__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1))    )

#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3))
  uint32_t orig;
  uint32_t psplim;
  uint32_t result;

  orig = __TZ_get_PSPLIM_NS();

  psplim = orig + 0x12345678U;
  __TZ_set_PSPLIM_NS(psplim);

  result = __TZ_get_PSPLIM_NS();

  __TZ_set_PSPLIM_NS(orig);

#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)))
  // without main extensions, the non-secure PSPLIM is RAZ/WI
  ASSERT_TRUE(result == 0U);
#else
  ASSERT_TRUE(result == psplim);
#endif
#endif

#endif
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_MSPLIM
\details
- Check if __get_MSPLIM and __set_MSPLIM intrinsic can be used to manipulate main stack pointer limit.
*/
void TC_CoreFunc_MSPLIM (void) {
#if ((defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) || \
     (defined (__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1))    )
  // don't use stack for this variables
  static uint32_t orig;
  static uint32_t msplim;
  static uint32_t result;
  static uint32_t ctrl;

  ctrl = __get_CONTROL();
  __set_CONTROL(ctrl | CONTROL_SPSEL_Msk); // switch to PSP

  orig = __get_MSPLIM();

  msplim = orig + 0x12345678U;
  __set_MSPLIM(msplim);

  result = __get_MSPLIM();

  __set_MSPLIM(orig);

  __set_CONTROL(ctrl);

#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
     (!defined (__ARM_FEATURE_CMSE  ) || (__ARM_FEATURE_CMSE   < 3)))
  // without main extensions, the non-secure MSPLIM is RAZ/WI
  ASSERT_TRUE(result == 0U);
#else
  ASSERT_TRUE(result == msplim);
#endif

#endif
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_MSPLIM_NS
\details
- Check if __TZ_get_MSPLIM_NS and __TZ_set_MSPLIM_NS intrinsic can be used to manipulate process stack pointer limit.
*/
void TC_CoreFunc_MSPLIM_NS (void) {
#if ((defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) || \
     (defined (__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1))    )

#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3))
  uint32_t orig;
  uint32_t msplim;
  uint32_t result;

  orig = __TZ_get_MSPLIM_NS();

  msplim = orig + 0x12345678U;
  __TZ_set_MSPLIM_NS(msplim);

  result = __TZ_get_MSPLIM_NS();

  __TZ_set_MSPLIM_NS(orig);

#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)))
  // without main extensions, the non-secure MSPLIM is RAZ/WI
  ASSERT_TRUE(result == 0U);
#else
  ASSERT_TRUE(result == msplim);
#endif
#endif

#endif
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_PRIMASK
\details
- Check if __get_PRIMASK and __set_PRIMASK intrinsic can be used to manipulate PRIMASK.
- Check if __enable_irq and __disable_irq are reflected in PRIMASK.
*/
void TC_CoreFunc_PRIMASK (void) {
  uint32_t orig = __get_PRIMASK();

  // toggle primask
  uint32_t primask = (orig & ~0x01U) | (~orig & 0x01U);

  __set_PRIMASK(primask);
  uint32_t result = __get_PRIMASK();

  ASSERT_TRUE(result == primask);

  __disable_irq();
  result = __get_PRIMASK();
  ASSERT_TRUE((result & 0x01U) == 1U);

  __enable_irq();
  result = __get_PRIMASK();
  ASSERT_TRUE((result & 0x01U) == 0U);

  __disable_irq();
  result = __get_PRIMASK();
  ASSERT_TRUE((result & 0x01U) == 1U);

  __set_PRIMASK(orig);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_FAULTMASK
\details
- Check if __get_FAULTMASK and __set_FAULTMASK intrinsic can be used to manipulate FAULTMASK.
- Check if __enable_fault_irq and __disable_fault_irq are reflected in FAULTMASK.
*/
void TC_CoreFunc_FAULTMASK (void) {
#if ((defined (__ARM_ARCH_7M__      ) && (__ARM_ARCH_7M__      == 1)) || \
     (defined (__ARM_ARCH_7EM__     ) && (__ARM_ARCH_7EM__     == 1)) || \
     (defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1))    )

  uint32_t orig = __get_FAULTMASK();

  // toggle faultmask
  uint32_t faultmask = (orig & ~0x01U) | (~orig & 0x01U);

  __set_FAULTMASK(faultmask);
  uint32_t result = __get_FAULTMASK();

  ASSERT_TRUE(result == faultmask);

  __disable_fault_irq();
  result = __get_FAULTMASK();
  ASSERT_TRUE((result & 0x01U) == 1U);

  __enable_fault_irq();
  result = __get_FAULTMASK();
  ASSERT_TRUE((result & 0x01U) == 0U);

  __disable_fault_irq();
  result = __get_FAULTMASK();
  ASSERT_TRUE((result & 0x01U) == 1U);

  __set_FAULTMASK(orig);

#endif
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_BASEPRI
\details
- Check if __get_BASEPRI and __set_BASEPRI intrinsic can be used to manipulate BASEPRI.
- Check if __set_BASEPRI_MAX intrinsic can be used to manipulate BASEPRI.
*/
void TC_CoreFunc_BASEPRI(void) {
#if ((defined (__ARM_ARCH_7M__      ) && (__ARM_ARCH_7M__      == 1)) || \
     (defined (__ARM_ARCH_7EM__     ) && (__ARM_ARCH_7EM__     == 1)) || \
     (defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1))    )

  uint32_t orig = __get_BASEPRI();

  uint32_t basepri = ~orig & 0x80U;
  __set_BASEPRI(basepri);
  uint32_t result = __get_BASEPRI();

  ASSERT_TRUE(result == basepri);

  __set_BASEPRI(orig);

  __set_BASEPRI_MAX(basepri);
  result = __get_BASEPRI();

  ASSERT_TRUE(result == basepri);

#endif
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_FPUType
\details
Check SCB_GetFPUType returns information.
*/
void TC_CoreFunc_FPUType(void) {
  uint32_t fpuType = SCB_GetFPUType();
#if defined(__FPU_PRESENT) && (__FPU_PRESENT != 0)
  ASSERT_TRUE(fpuType > 0U);
#else
  ASSERT_TRUE(fpuType  == 0U);
#endif
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreFunc_FPSCR
\details
- Check if __get_FPSCR and __set_FPSCR intrinsics can be used
*/
void TC_CoreFunc_FPSCR(void) {
  uint32_t fpscr = __get_FPSCR();
  __ISB();
  __DSB();

  __set_FPSCR(~fpscr);
  __ISB();
  __DSB();

  uint32_t result = __get_FPSCR();

  __set_FPSCR(fpscr);

#if (defined (__FPU_USED   ) && (__FPU_USED    == 1U))
  ASSERT_TRUE(result != fpscr);
#else
  ASSERT_TRUE(result == 0U);
#endif
}

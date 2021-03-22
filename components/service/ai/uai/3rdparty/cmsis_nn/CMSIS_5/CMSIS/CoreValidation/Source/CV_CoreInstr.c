/*-----------------------------------------------------------------------------
 *      Name:         CV_CoreInstr.c
 *      Purpose:      CMSIS CORE validation tests implementation
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2017 - 2018 Arm Limited. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "CV_Framework.h"
#include "cmsis_cv.h"

#if defined(__CORTEX_M)
#elif defined(__CORTEX_A)
#include "irq_ctrl.h"
#else
#error __CORTEX_M or __CORTEX_A must be defined!
#endif

/*-----------------------------------------------------------------------------
 *      Test implementation
 *----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 *      Test cases
 *----------------------------------------------------------------------------*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_NOP
\details
- Check if __NOP instrinsic is available
- No real assertion is deployed, just a compile time check.
*/
void TC_CoreInstr_NOP (void) {
  __NOP();
  ASSERT_TRUE(1U == 1U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_SEV
\details
- Check if __SEV instrinsic is available
- No real assertion is deployed, just a compile time check.
*/
void TC_CoreInstr_SEV (void) {
  __SEV();
  ASSERT_TRUE(1U == 1U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_BKPT
\details
- Check if __BKPT instrinsic is available
- No real assertion is deployed, just a compile time check.
*/
void TC_CoreInstr_BKPT (void) {
  __BKPT(0xABU);
  ASSERT_TRUE(1U == 1U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_ISB
\details
- Check if __ISB instrinsic is available
- No real assertion is deployed, just a compile time check.
*/
void TC_CoreInstr_ISB (void) {
  __ISB();
  ASSERT_TRUE(1U == 1U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_DSB
\details
- Check if __DSB instrinsic is available
- No real assertion is deployed, just a compile time check.
*/
void TC_CoreInstr_DSB (void) {
  __DSB();
  ASSERT_TRUE(1U == 1U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_DMB
\details
- Check if __DNB instrinsic is available
- No real assertion is deployed, just a compile time check.
*/
void TC_CoreInstr_DMB (void) {
  __DMB();
  ASSERT_TRUE(1U == 1U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_WFI
\details
- Check if __WFI instrinsic is available
- No real assertion is deployed, just a compile time check.
*/
void TC_CoreInstr_WFI (void) {
  __WFI();
  ASSERT_TRUE(1U == 1U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_WFE
\details
- Check if __WFE instrinsic is available
- No real assertion is deployed, just a compile time check.
*/
void TC_CoreInstr_WFE (void) {
  __WFE();
  ASSERT_TRUE(1U == 1U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_REV
\details
- Check if __REV instrinsic swaps all bytes in a word.
*/
void TC_CoreInstr_REV (void) {
  volatile uint32_t op1_u32;
  volatile uint32_t res_u32;

  op1_u32 = 0x47110815U;
  res_u32 = __REV(op1_u32);
  ASSERT_TRUE(res_u32 == 0x15081147U);

  op1_u32 = 0x80000000U;
  res_u32 = __REV(op1_u32);
  ASSERT_TRUE(res_u32 == 0x00000080U);

  op1_u32 = 0x00000080U;
  res_u32 = __REV(op1_u32);
  ASSERT_TRUE(res_u32 == 0x80000000U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_REV16
\details
- Check if __REV16 instrinsic swaps the bytes in both halfwords independendly.
*/
void TC_CoreInstr_REV16(void) {
  volatile uint32_t op1_u32;
  volatile uint32_t res_u32;

  op1_u32 = 0x47110815U;
  res_u32 = __REV16(op1_u32);
  ASSERT_TRUE(res_u32 == 0x11471508U);

  op1_u32 = 0x00001234U;
  res_u32 = __REV16(op1_u32);
  ASSERT_TRUE(res_u32 == 0x00003412U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_REVSH
\details
- Check if __REVSH instrinsic swaps bytes in a signed halfword keeping the sign.
*/
void TC_CoreInstr_REVSH(void) {
  volatile int16_t value  = 0U;
           int16_t result = 0U;

  value = 0x4711;
  result = __REVSH(value);
  ASSERT_TRUE(result == 0x1147);

  value = (int16_t)0x8000;
  result = __REVSH(value);
  ASSERT_TRUE(result == 0x0080);

  value = 0x0080;
  result = __REVSH(value);
  ASSERT_TRUE(result == (int16_t)0x8000);

  value = -0x1234;
  result = __REVSH(value);
  ASSERT_TRUE(result == (int16_t)0xcced);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_RBIT
\details
- Check if __RBIT instrinsic revserses the bit order of arbitrary words.
*/
void TC_CoreInstr_RBIT (void) {
  volatile uint32_t value  = 0U;
           uint32_t result = 0U;

  value = 0xAAAAAAAAU;
  result = __RBIT(value);
  ASSERT_TRUE(result == 0x55555555U);

  value = 0x55555555U;
  result = __RBIT(value);
  ASSERT_TRUE(result == 0xAAAAAAAAU);

  value = 0x00000001U;
  result = __RBIT(value);
  ASSERT_TRUE(result == 0x80000000U);

  value = 0x80000000U;
  result = __RBIT(value);
  ASSERT_TRUE(result == 0x00000001U);

  value = 0xDEADBEEFU;
  result = __RBIT(value);
  ASSERT_TRUE(result == 0xF77DB57BU);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_ROR
\details
- Check if __ROR instrinsic moves all bits as expected.
*/
void TC_CoreInstr_ROR(void) {
  volatile uint32_t value  = 0U;
           uint32_t result = 0U;

  value = 0x00000001U;
  result = __ROR(value, 1U);
  ASSERT_TRUE(result == 0x80000000U);

  value = 0x80000000U;
  result = __ROR(value, 1U);
  ASSERT_TRUE(result == 0x40000000U);

  value = 0x40000000U;
  result = __ROR(value, 30U);
  ASSERT_TRUE(result == 0x00000001U);

  value = 0x00000001U;
  result = __ROR(value, 32U);
  ASSERT_TRUE(result == 0x00000001U);

  value = 0x08154711U;
  result = __ROR(value, 8U);
  ASSERT_TRUE(result == 0x11081547U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_CLZ
\details
- Check if __CLZ instrinsic counts leading zeros.
*/
void TC_CoreInstr_CLZ (void) {
  volatile uint32_t value  = 0U;
           uint32_t result = 0U;

  value = 0x00000000U;
  result = __CLZ(value);
  ASSERT_TRUE(result == 32);

  value = 0x00000001U;
  result = __CLZ(value);
  ASSERT_TRUE(result == 31);

  value = 0x40000000U;
  result = __CLZ(value);
  ASSERT_TRUE(result == 1);

  value = 0x80000000U;
  result = __CLZ(value);
  ASSERT_TRUE(result == 0);

  value = 0xFFFFFFFFU;
  result = __CLZ(value);
  ASSERT_TRUE(result == 0);

  value = 0x80000001U;
  result = __CLZ(value);
  ASSERT_TRUE(result == 0);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_SSAT
\details
- Check if __SSAT instrinsic saturates signed integer values.
*/
void TC_CoreInstr_SSAT (void) {
  volatile int32_t value  = 0;
           int32_t result = 0;

  value = INT32_MAX;
  result = __SSAT(value, 32U);
  ASSERT_TRUE(result == INT32_MAX);

  value = INT32_MAX;
  result = __SSAT(value, 16U);
  ASSERT_TRUE(result == INT16_MAX);

  value = INT32_MAX;
  result = __SSAT(value, 8U);
  ASSERT_TRUE(result == INT8_MAX);

  value = INT32_MAX;
  result = __SSAT(value, 1U);
  ASSERT_TRUE(result == 0);

  value = INT32_MIN;
  result = __SSAT(value, 32U);
  ASSERT_TRUE(result == INT32_MIN);

  value = INT32_MIN;
  result = __SSAT(value, 16U);
  ASSERT_TRUE(result == INT16_MIN);

  value = INT32_MIN;
  result = __SSAT(value, 8U);
  ASSERT_TRUE(result == INT8_MIN);

  value = INT32_MIN;
  result = __SSAT(value, 1U);
  ASSERT_TRUE(result == -1);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_USAT
\details
- Check if __USAT instrinsic saturates unsigned integer values.
*/
void TC_CoreInstr_USAT (void) {
  volatile  int32_t value  = 0U;
           uint32_t result = 0U;

  value = INT32_MAX;
  result = __USAT(value, 31U);
  ASSERT_TRUE(result == (UINT32_MAX >> 1U));

  value = INT32_MAX;
  result = __USAT(value, 16U);
  ASSERT_TRUE(result == UINT16_MAX);

  value = INT32_MAX;
  result = __USAT(value, 8U);
  ASSERT_TRUE(result == UINT8_MAX);

  value = INT32_MAX;
  result = __USAT(value, 0U);
  ASSERT_TRUE(result == 0U);

  value = INT32_MIN;
  result = __USAT(value, 31U);
  ASSERT_TRUE(result == 0U);

  value = INT32_MIN;
  result = __USAT(value, 16U);
  ASSERT_TRUE(result == 0U);

  value = INT32_MIN;
  result = __USAT(value, 8U);
  ASSERT_TRUE(result == 0U);

  value = INT32_MIN;
  result = __USAT(value, 0U);
  ASSERT_TRUE(result == 0U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_CoreInstr_RRX
\details
- Check if __USAT instrinsic saturates unsigned integer values.
*/
void TC_CoreInstr_RRX (void) {
#if ((defined (__ARM_ARCH_7M__      ) && (__ARM_ARCH_7M__      == 1)) || \
     (defined (__ARM_ARCH_7EM__     ) && (__ARM_ARCH_7EM__     == 1)) || \
     (defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1))    )

  volatile uint32_t  value  = 0U;
  volatile uint32_t  result = 0U;
  volatile xPSR_Type xPSR;

  value = 0x80000002;
  xPSR.w = __get_xPSR();
  result = __RRX(value);
  ASSERT_TRUE(result == (0x40000001 | (uint32_t)(xPSR.b.C << 31)));
#endif
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
#if ((defined (__ARM_ARCH_7M__      ) && (__ARM_ARCH_7M__      == 1)) || \
     (defined (__ARM_ARCH_7EM__     ) && (__ARM_ARCH_7EM__     == 1)) || \
     (defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) || \
     (defined (__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1)) || \
     (defined(__CORTEX_A)                                           )    )

/// Exclusive byte value
static volatile uint8_t TC_CoreInstr_LoadStoreExclusive_byte = 0x47U;

/// Exclusive halfword value
static volatile uint16_t TC_CoreInstr_LoadStoreExclusive_hword = 0x0815U;

/// Exclusive word value
static volatile uint32_t TC_CoreInstr_LoadStoreExclusive_word = 0x08154711U;

/**
\brief Interrupt function for TC_CoreInstr_LoadStoreExclusive
\details
The interrupt manipulates all the global data
which disrupts the exclusive sequences in the test
*/
static void TC_CoreInstr_LoadStoreExclusive_IRQHandler(void) {

  const uint8_t b = __LDREXB(&TC_CoreInstr_LoadStoreExclusive_byte);
  __STREXB((uint8_t)~b, &TC_CoreInstr_LoadStoreExclusive_byte);

  const uint16_t hw = __LDREXH(&TC_CoreInstr_LoadStoreExclusive_hword);
  __STREXH((uint16_t)~hw, &TC_CoreInstr_LoadStoreExclusive_hword);

  const uint32_t w = __LDREXW(&TC_CoreInstr_LoadStoreExclusive_word);
  __STREXW((uint32_t)~w, &TC_CoreInstr_LoadStoreExclusive_word);
}

/**
\brief Helper function for TC_CoreInstr_LoadStoreExclusive to enable test interrupt.
\details
This helper function implements interrupt enabling according to target
architecture, i.e. Cortex-A or Cortex-M.
*/
static void TC_CoreInstr_LoadStoreExclusive_IRQEnable(void) {
#if defined(__CORTEX_M)
  TST_IRQHandler = TC_CoreInstr_LoadStoreExclusive_IRQHandler;
  NVIC_EnableIRQ(Interrupt0_IRQn);
#elif defined(__CORTEX_A)
  IRQ_SetHandler(SGI0_IRQn, TC_CoreInstr_LoadStoreExclusive_IRQHandler);
  IRQ_Enable(SGI0_IRQn);
#else
  #error __CORTEX_M or __CORTEX_A must be defined!
#endif
  __enable_irq();
}

/**
\brief Helper function for TC_CoreInstr_LoadStoreExclusive to set test interrupt pending.
\details
This helper function implements set pending the test interrupt according to target
architecture, i.e. Cortex-A or Cortex-M.
*/
static void TC_CoreInstr_LoadStoreExclusive_IRQPend(void) {
#if defined(__CORTEX_M)
  NVIC_SetPendingIRQ(Interrupt0_IRQn);
#elif defined(__CORTEX_A)
  IRQ_SetPending(SGI0_IRQn);
#else
  #error __CORTEX_M or __CORTEX_A must be defined!
#endif
  for(uint32_t i = 10U; i > 0U; --i) {}
}

/**
\brief Helper function for TC_CoreInstr_LoadStoreExclusive to disable test interrupt.
\details
This helper function implements interrupt disabling according to target
architecture, i.e. Cortex-A or Cortex-M.
*/
static void TC_CoreInstr_LoadStoreExclusive_IRQDisable(void) {
  __disable_irq();
#if defined(__CORTEX_M)
  NVIC_DisableIRQ(Interrupt0_IRQn);
  TST_IRQHandler = NULL;
#elif defined(__CORTEX_A)
  IRQ_Disable(SGI0_IRQn);
  IRQ_SetHandler(SGI0_IRQn, NULL);
#else
  #error __CORTEX_M or __CORTEX_A must be defined!
#endif
}
#endif

/**
\brief Test case: TC_CoreInstr_LoadStoreExclusive
\details
Checks exclusive load and store instructions:
- LDREXB, LDREXH, LDREXW
- STREXB, STREXH, STREXW
- CLREX
*/
void TC_CoreInstr_LoadStoreExclusive (void) {
#if ((defined (__ARM_ARCH_7M__      ) && (__ARM_ARCH_7M__      == 1)) || \
     (defined (__ARM_ARCH_7EM__     ) && (__ARM_ARCH_7EM__     == 1)) || \
     (defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) || \
     (defined (__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1)) || \
     (defined(__CORTEX_A))                                               )
  uint8_t  u8,  u8Inv;
  uint16_t u16, u16Inv;
  uint32_t u32, u32Inv;
  uint32_t result;

  /* 1. Test exclusives without interruption */
  u8 = __LDREXB(&TC_CoreInstr_LoadStoreExclusive_byte);
  ASSERT_TRUE(u8 == TC_CoreInstr_LoadStoreExclusive_byte);

  result = __STREXB(u8+1U, &TC_CoreInstr_LoadStoreExclusive_byte);
  ASSERT_TRUE(result == 0U);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreExclusive_byte == u8+1U);

  u16 = __LDREXH(&TC_CoreInstr_LoadStoreExclusive_hword);
  ASSERT_TRUE(u16 == TC_CoreInstr_LoadStoreExclusive_hword);

  result = __STREXH(u16+1U, &TC_CoreInstr_LoadStoreExclusive_hword);
  ASSERT_TRUE(result == 0U);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreExclusive_hword == u16+1U);

  u32 = __LDREXW(&TC_CoreInstr_LoadStoreExclusive_word);
  ASSERT_TRUE(u32 == TC_CoreInstr_LoadStoreExclusive_word);

  result = __STREXW(u32+1U, &TC_CoreInstr_LoadStoreExclusive_word);
  ASSERT_TRUE(result == 0U);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreExclusive_word == u32+1U);

  /* 2. Test exclusives with clear */
  u8 = __LDREXB(&TC_CoreInstr_LoadStoreExclusive_byte);
  ASSERT_TRUE(u8 == TC_CoreInstr_LoadStoreExclusive_byte);

  __CLREX();

  result = __STREXB(u8+1U, &TC_CoreInstr_LoadStoreExclusive_byte);
  ASSERT_TRUE(result == 1U);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreExclusive_byte == u8);

  u16 = __LDREXH(&TC_CoreInstr_LoadStoreExclusive_hword);
  ASSERT_TRUE(u16 == TC_CoreInstr_LoadStoreExclusive_hword);

  __CLREX();

  result = __STREXH(u16+1U, &TC_CoreInstr_LoadStoreExclusive_hword);
  ASSERT_TRUE(result == 1U);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreExclusive_hword == u16);

  u32 = __LDREXW(&TC_CoreInstr_LoadStoreExclusive_word);
  ASSERT_TRUE(u32 == TC_CoreInstr_LoadStoreExclusive_word);

  __CLREX();

  result = __STREXW(u32+1U, &TC_CoreInstr_LoadStoreExclusive_word);
  ASSERT_TRUE(result == 1U);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreExclusive_word == u32);

  /* 3. Test exclusives with interruption */
  TC_CoreInstr_LoadStoreExclusive_IRQEnable();

  u8 = __LDREXB(&TC_CoreInstr_LoadStoreExclusive_byte);
  ASSERT_TRUE(u8 == TC_CoreInstr_LoadStoreExclusive_byte);

  TC_CoreInstr_LoadStoreExclusive_IRQPend();

  result = __STREXB(u8+1U, &TC_CoreInstr_LoadStoreExclusive_byte);
  ASSERT_TRUE(result == 1U);
  u8Inv = (uint8_t)~u8;
  ASSERT_TRUE(u8Inv == TC_CoreInstr_LoadStoreExclusive_byte);

  u16 = __LDREXH(&TC_CoreInstr_LoadStoreExclusive_hword);
  ASSERT_TRUE(u16 == TC_CoreInstr_LoadStoreExclusive_hword);

  TC_CoreInstr_LoadStoreExclusive_IRQPend();

  result = __STREXH(u16+1U, &TC_CoreInstr_LoadStoreExclusive_hword);
  ASSERT_TRUE(result == 1U);
  u16Inv = (uint16_t)~u16;
  ASSERT_TRUE(u16Inv == TC_CoreInstr_LoadStoreExclusive_hword);

  u32 = __LDREXW(&TC_CoreInstr_LoadStoreExclusive_word);
  ASSERT_TRUE(u32 == TC_CoreInstr_LoadStoreExclusive_word);

  TC_CoreInstr_LoadStoreExclusive_IRQPend();

  result = __STREXW(u32+1U, &TC_CoreInstr_LoadStoreExclusive_word);
  ASSERT_TRUE(result == 1U);
  u32Inv = (uint32_t)~u32;
  ASSERT_TRUE(u32Inv == TC_CoreInstr_LoadStoreExclusive_word);

  TC_CoreInstr_LoadStoreExclusive_IRQDisable();
#endif
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
#if ((defined (__ARM_ARCH_7M__      ) && (__ARM_ARCH_7M__      == 1)) || \
     (defined (__ARM_ARCH_7EM__     ) && (__ARM_ARCH_7EM__     == 1)) || \
     (defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1))    )

/// byte value unprivileged access
static volatile uint8_t TC_CoreInstr_LoadStoreUnpriv_byte = 0x47U;

/// halfword value unprivileged access
static volatile uint16_t TC_CoreInstr_LoadStoreUnpriv_hword = 0x0815U;

/// word value unprivileged access
static volatile uint32_t TC_CoreInstr_LoadStoreUnpriv_word = 0x08154711U;
#endif


/**
\brief Test case: TC_CoreInstr_LoadStoreUnpriv
\details
Checks load/store unprivileged instructions:
- LDRBT, LDRHT, LDRT
- STRBT, STRHT, STRT
*/
void TC_CoreInstr_LoadStoreUnpriv (void) {
#if ((defined (__ARM_ARCH_7M__      ) && (__ARM_ARCH_7M__      == 1)) || \
     (defined (__ARM_ARCH_7EM__     ) && (__ARM_ARCH_7EM__     == 1)) || \
     (defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1))    )
  uint8_t  u8     = 0U;
  uint16_t u16    = 0U;
  uint32_t u32    = 0U;

  /* 1. Test without interruption */
  u8 = __LDRBT(&TC_CoreInstr_LoadStoreUnpriv_byte);
  ASSERT_TRUE(u8 == TC_CoreInstr_LoadStoreUnpriv_byte);

  __STRBT(u8+1U, &TC_CoreInstr_LoadStoreUnpriv_byte);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreUnpriv_byte == u8+1U);

  u16 = __LDRHT(&TC_CoreInstr_LoadStoreUnpriv_hword);
  ASSERT_TRUE(u16 == TC_CoreInstr_LoadStoreUnpriv_hword);

  __STRHT(u16+1U, &TC_CoreInstr_LoadStoreUnpriv_hword);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreUnpriv_hword == u16+1U);

  u32 = __LDRT(&TC_CoreInstr_LoadStoreUnpriv_word);
  ASSERT_TRUE(u32 == TC_CoreInstr_LoadStoreUnpriv_word);

  __STRT(u32+1U, &TC_CoreInstr_LoadStoreUnpriv_word);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreUnpriv_word == u32+1U);
#endif
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
#if ((defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) || \
     (defined (__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1))    )

/// byte value unprivileged access
static volatile uint8_t TC_CoreInstr_LoadStoreAcquire_byte = 0x47U;

/// halfword value unprivileged access
static volatile uint16_t TC_CoreInstr_LoadStoreAcquire_hword = 0x0815U;

/// word value unprivileged access
static volatile uint32_t TC_CoreInstr_LoadStoreAcquire_word = 0x08154711U;
#endif


/**
\brief Test case: TC_CoreInstr_LoadStoreAquire
\details
Checks Load-Acquire and Store-Release instructions:
- LDAB, LDAH, LDA
- STLB, STLH, STL
*/
void TC_CoreInstr_LoadStoreAcquire (void) {
#if ((defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) || \
     (defined (__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1))    )
  uint8_t  u8     = 0U;
  uint16_t u16    = 0U;
  uint32_t u32    = 0U;

  /* 1. Test without interruption */
  u8 = __LDAB(&TC_CoreInstr_LoadStoreAcquire_byte);
  ASSERT_TRUE(u8 == TC_CoreInstr_LoadStoreAcquire_byte);

  __STLB(u8+1U, &TC_CoreInstr_LoadStoreAcquire_byte);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreAcquire_byte == u8+1U);

  u16 = __LDAH(&TC_CoreInstr_LoadStoreAcquire_hword);
  ASSERT_TRUE(u16 == TC_CoreInstr_LoadStoreAcquire_hword);

  __STLH(u16+1U, &TC_CoreInstr_LoadStoreAcquire_hword);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreAcquire_hword == u16+1U);

  u32 = __LDA(&TC_CoreInstr_LoadStoreAcquire_word);
  ASSERT_TRUE(u32 == TC_CoreInstr_LoadStoreAcquire_word);

  __STL(u32+1U, &TC_CoreInstr_LoadStoreAcquire_word);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreAcquire_word == u32+1U);
#endif
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
#if ((defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) || \
     (defined (__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1))    )

/// byte value unprivileged access
static volatile uint8_t TC_CoreInstr_LoadStoreAcquireExclusive_byte = 0x47U;

/// halfword value unprivileged access
static volatile uint16_t TC_CoreInstr_LoadStoreAcquireExclusive_hword = 0x0815U;

/// word value unprivileged access
static volatile uint32_t TC_CoreInstr_LoadStoreAcquireExclusive_word = 0x08154711U;
#endif


/**
\brief Test case: TC_CoreInstr_LoadStoreAquire
\details
Checks Load-Acquire and Store-Release exclusive instructions:
- LDAEXB, LDAEXH, LDAEX
- STLEXB, STLEXH, STLEX
*/
void TC_CoreInstr_LoadStoreAcquireExclusive (void) {
#if ((defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) || \
     (defined (__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1))    )
  uint8_t  u8     = 0U;
  uint16_t u16    = 0U;
  uint32_t u32    = 0U;
  uint32_t result = 0U;

  /* 1. Test without interruption */
  u8 = __LDAEXB(&TC_CoreInstr_LoadStoreAcquireExclusive_byte);
  ASSERT_TRUE(u8 == TC_CoreInstr_LoadStoreAcquireExclusive_byte);

  result = __STLEXB(u8+1U, &TC_CoreInstr_LoadStoreAcquireExclusive_byte);
  ASSERT_TRUE(result == 0U);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreAcquireExclusive_byte == u8+1U);

  u16 = __LDAEXH(&TC_CoreInstr_LoadStoreAcquireExclusive_hword);
  ASSERT_TRUE(u16 == TC_CoreInstr_LoadStoreAcquireExclusive_hword);

  result = __STLEXH(u16+1U, &TC_CoreInstr_LoadStoreAcquireExclusive_hword);
  ASSERT_TRUE(result == 0U);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreAcquireExclusive_hword == u16+1U);

  u32 = __LDAEX(&TC_CoreInstr_LoadStoreAcquireExclusive_word);
  ASSERT_TRUE(u32 == TC_CoreInstr_LoadStoreAcquireExclusive_word);

  result = __STLEX(u32+1U, &TC_CoreInstr_LoadStoreAcquireExclusive_word);
  ASSERT_TRUE(result == 0U);
  ASSERT_TRUE(TC_CoreInstr_LoadStoreAcquireExclusive_word == u32+1U);
#endif
}


/**
\brief Test case: TC_CoreInstr_UnalignedUint16
\details
Checks macro functions to access unaligned uint16_t values:
- __UNALIGNED_UINT16_READ
- __UNALIGNED_UINT16_WRITE
*/
void TC_CoreInstr_UnalignedUint16(void) {
  uint8_t buffer[3] = { 0U, 0U, 0U };
  uint16_t val;
  
  for(int i=0; i<2; i++) {
    __UNALIGNED_UINT16_WRITE(&(buffer[i]), 0x4711U);
    ASSERT_TRUE(buffer[i]       == 0x11U);
    ASSERT_TRUE(buffer[i+1]     == 0x47U);
    ASSERT_TRUE(buffer[(i+2)%3] == 0x00U);
  
    buffer[i] = 0x12U;
    buffer[i+1] = 0x46U;
  
    val = __UNALIGNED_UINT16_READ(&(buffer[i]));
    ASSERT_TRUE(val == 0x4612U);
  
    buffer[i]   = 0x00U;
    buffer[i+1] = 0x00U;
  }
}


/**
\brief Test case: TC_CoreInstr_UnalignedUint32
\details
Checks macro functions to access unaligned uint32_t values:
- __UNALIGNED_UINT32_READ
- __UNALIGNED_UINT32_WRITE
*/
void TC_CoreInstr_UnalignedUint32(void) {
  uint8_t buffer[7] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
  uint32_t val;
  
  for(int i=0; i<4; i++) {
    __UNALIGNED_UINT32_WRITE(&(buffer[i]), 0x08154711UL);
    ASSERT_TRUE(buffer[i+0]     == 0x11U);
    ASSERT_TRUE(buffer[i+1]     == 0x47U);
    ASSERT_TRUE(buffer[i+2]     == 0x15U);
    ASSERT_TRUE(buffer[i+3]     == 0x08U);
    ASSERT_TRUE(buffer[(i+4)%7] == 0x00U);
    ASSERT_TRUE(buffer[(i+5)%7] == 0x00U);
    ASSERT_TRUE(buffer[(i+6)%7] == 0x00U);
  
    buffer[i+0] = 0x12U;
    buffer[i+1] = 0x46U;
    buffer[i+2] = 0x14U;
    buffer[i+3] = 0x09U;
  
    val = __UNALIGNED_UINT32_READ(&(buffer[i]));
    ASSERT_TRUE(val == 0x09144612UL);
  
    buffer[i+0] = 0x00U;
    buffer[i+1] = 0x00U;
    buffer[i+2] = 0x00U;
    buffer[i+3] = 0x00U;
  }
}

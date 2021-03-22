/*-----------------------------------------------------------------------------
 *      Name:         CV_L1Cache.c 
 *      Purpose:      CMSIS CORE validation tests implementation
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2017 ARM Limited. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "CV_Framework.h"
#include "cmsis_cv.h"

/*-----------------------------------------------------------------------------
 *      Test implementation
 *----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 *      Test cases
 *----------------------------------------------------------------------------*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_L1Cache_EnDisable(void) {
  
  uint32_t orig = __get_SCTLR();
  
  L1C_EnableCaches();
  
  uint32_t sctlr = __get_SCTLR();
  ASSERT_TRUE((sctlr  & SCTLR_I_Msk) == SCTLR_I_Msk);
  ASSERT_TRUE((sctlr  & SCTLR_C_Msk) == SCTLR_C_Msk);
  
  L1C_CleanDCacheAll();
  L1C_DisableCaches();
  
  sctlr = __get_SCTLR();
  ASSERT_TRUE((sctlr & SCTLR_I_Msk) == 0U);
  ASSERT_TRUE((sctlr & SCTLR_C_Msk) == 0U);
  
  __set_SCTLR(orig);
  __ISB();
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_L1Cache_EnDisableBTAC(void) {
  uint32_t orig = __get_SCTLR();
  
  L1C_EnableBTAC();
  
  uint32_t sctlr = __get_SCTLR();
  ASSERT_TRUE((sctlr & SCTLR_Z_Msk) == SCTLR_Z_Msk);
  
  L1C_DisableBTAC();
  
  sctlr = __get_SCTLR();
#if __CORTEX_A == 7
  // On Cortex-A7 SCTLR_Z is RAO/WI.
  ASSERT_TRUE((sctlr & SCTLR_Z_Msk) == SCTLR_Z_Msk);
#else
  ASSERT_TRUE((sctlr & SCTLR_Z_Msk) == 0U);
#endif

  __set_SCTLR(orig);
  __ISB();
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_L1Cache_log2_up(void) {
  uint8_t log2 = __log2_up(0U);
  ASSERT_TRUE(log2 == 0U);
  
  log2 = __log2_up(1U);
  ASSERT_TRUE(log2 == 0U);

  log2 = __log2_up(2U);
  ASSERT_TRUE(log2 == 1U);

  log2 = __log2_up(3U);
  ASSERT_TRUE(log2 == 2U);

  log2 = __log2_up(4U);
  ASSERT_TRUE(log2 == 2U);

  log2 = __log2_up(0x80000000U);
  ASSERT_TRUE(log2 == 31U);

  log2 = __log2_up(0x80000001U);
  ASSERT_TRUE(log2 == 32U);
  
  log2 = __log2_up(0xFFFFFFFFU);
  ASSERT_TRUE(log2 == 32U);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_L1Cache_InvalidateDCacheAll(void) {
  
  /* setup */
  uint32_t orig = __get_SCTLR();
  volatile uint32_t value = 0x0815U;

  L1C_EnableCaches();

  L1C_CleanDCacheAll();

  /* test cached value gets lost */
  
  // WHEN a value is written
  value = 0x4711U;
  
  // ... and the cache is invalidated
  L1C_InvalidateDCacheAll();

  // ... and the cache is disabled
  L1C_DisableCaches();

  // THEN the new value has been lost
  ASSERT_TRUE(value == 0x0815U);
  
  /* tear down */
  L1C_InvalidateDCacheAll();
  __set_SCTLR(orig);
  __ISB();
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_L1Cache_CleanDCacheAll(void) {
  /* setup */
  uint32_t orig = __get_SCTLR();
  uint32_t value = 0x0815U;

  L1C_EnableCaches();

  L1C_CleanDCacheAll();
  
  /* test cached value is preserved */
  
  // WHEN a value is written
  value = 0x4711U;
  
  // ... and the cache is cleaned
  L1C_CleanDCacheAll();
  
  // ... and the cache is disabled
  L1C_DisableCaches();
  
  // THEN the new value is preserved
  ASSERT_TRUE(value == 0x4711U);
  
  /* tear down */
  L1C_InvalidateDCacheAll();
  __set_SCTLR(orig);
  __ISB();
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_L1Cache_CleanInvalidateDCacheAll(void) {
  /* setup */
  uint32_t orig = __get_SCTLR();
  uint32_t value = 0x0815U;

  L1C_EnableCaches();

  L1C_CleanDCacheAll();
  
  /* test cached value is preserved */
  
  // WHEN a value is written
  value = 0x4711U;
  
  // ... and the cache is cleaned/invalidated
  L1C_CleanInvalidateDCacheAll();
  
  // ... and the cache is disabled
  L1C_DisableCaches();

  // THEN the new value is preserved
  ASSERT_TRUE(value == 0x4711U);
  
  /* tear down */
  L1C_InvalidateDCacheAll();
  __set_SCTLR(orig);
  __ISB();
}


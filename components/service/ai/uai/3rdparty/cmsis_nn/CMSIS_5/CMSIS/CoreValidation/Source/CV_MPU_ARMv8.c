/*-----------------------------------------------------------------------------
 *      Name:         CV_MPU_ARMv7.c 
 *      Purpose:      CMSIS CORE validation tests implementation
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2017 ARM Limited. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "CV_Framework.h"
#include "cmsis_cv.h"

/*-----------------------------------------------------------------------------
 *      Test implementation
 *----------------------------------------------------------------------------*/

#if defined(__MPU_PRESENT) && __MPU_PRESENT
static void ClearMpu(void) {
  for(uint32_t i = 0U; i < 8U; ++i) {
    MPU->RNR = i;
    MPU->RBAR = 0U;
    MPU->RLAR = 0U;
  }
}
#endif

/*-----------------------------------------------------------------------------
 *      Test cases
 *----------------------------------------------------------------------------*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_MPU_SetClear
\details
- Check if ARM_MPU_Load correctly loads MPU table to registers.
*/
void TC_MPU_SetClear(void)
{
#if defined(__MPU_PRESENT) && __MPU_PRESENT
  static const ARM_MPU_Region_t table[] = {
    { .RBAR = 0U, .RLAR = 0U },
    { .RBAR = ARM_MPU_RBAR(0x30000000U, 0U, 1U, 1U, 1U), .RLAR = ARM_MPU_RLAR(0x38000000U, 0U) }
  };
  
  #define ASSERT_MPU_REGION(rnr, region) \
    MPU->RNR = rnr; \
    ASSERT_TRUE(MPU->RBAR == region.RBAR); \
    ASSERT_TRUE(MPU->RLAR == region.RLAR)
  
  ClearMpu();
    
  ARM_MPU_SetRegion(2U, table[1].RBAR, table[1].RLAR);
  
  ASSERT_MPU_REGION(1U, table[0]);
  ASSERT_MPU_REGION(2U, table[1]);
  ASSERT_MPU_REGION(3U, table[0]);
  
  ARM_MPU_ClrRegion(2U);
  
  MPU->RNR = 2U;
  ASSERT_TRUE((MPU->RLAR & MPU_RLAR_EN_Msk) == 0U);
  
  #undef ASSERT_MPU_REGION
#endif
}
  
/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: TC_MPU_Load
\details
- Check if ARM_MPU_Load correctly loads MPU table to registers.
*/
void TC_MPU_Load(void)
{
#if defined(__MPU_PRESENT) && __MPU_PRESENT
  static const ARM_MPU_Region_t table[] = {
    { .RBAR = ARM_MPU_RBAR(0x10000000U, 0U, 1U, 1U, 1U), .RLAR = ARM_MPU_RLAR(0x18000000U, 0U) },
    { .RBAR = ARM_MPU_RBAR(0x20000000U, 0U, 1U, 1U, 1U), .RLAR = ARM_MPU_RLAR(0x27000000U, 0U) },
    { .RBAR = ARM_MPU_RBAR(0x30000000U, 0U, 1U, 1U, 1U), .RLAR = ARM_MPU_RLAR(0x36000000U, 0U) },
    { .RBAR = ARM_MPU_RBAR(0x40000000U, 0U, 1U, 1U, 1U), .RLAR = ARM_MPU_RLAR(0x45000000U, 0U) },
    { .RBAR = ARM_MPU_RBAR(0x50000000U, 0U, 1U, 1U, 1U), .RLAR = ARM_MPU_RLAR(0x54000000U, 0U) },
    { .RBAR = ARM_MPU_RBAR(0x60000000U, 0U, 1U, 1U, 1U), .RLAR = ARM_MPU_RLAR(0x63000000U, 0U) },
    { .RBAR = ARM_MPU_RBAR(0x70000000U, 0U, 1U, 1U, 1U), .RLAR = ARM_MPU_RLAR(0x72000000U, 0U) },
    { .RBAR = ARM_MPU_RBAR(0x80000000U, 0U, 1U, 1U, 1U), .RLAR = ARM_MPU_RLAR(0x31000000U, 0U) }
  };
  
  #define ASSERT_MPU_REGION(rnr, table) \
    MPU->RNR = rnr; \
    ASSERT_TRUE(MPU->RBAR == table[rnr].RBAR); \
    ASSERT_TRUE(MPU->RLAR == table[rnr].RLAR)

  ClearMpu();
  
  ARM_MPU_Load(0U, &(table[0]), 1U);
  
  ASSERT_MPU_REGION(0U, table);
  
  ARM_MPU_Load(1U, &(table[1]), 5U);

  ASSERT_MPU_REGION(0U, table);
  ASSERT_MPU_REGION(1U, table);
  ASSERT_MPU_REGION(2U, table);
  ASSERT_MPU_REGION(3U, table);
  ASSERT_MPU_REGION(4U, table);
  ASSERT_MPU_REGION(5U, table);

  ARM_MPU_Load(6U, &(table[6]), 2U);

  ASSERT_MPU_REGION(5U, table);
  ASSERT_MPU_REGION(6U, table);
  ASSERT_MPU_REGION(7U, table);

  #undef ASSERT_MPU_REGION
#endif 
}

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
    MPU->RASR = 0U;
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
    { .RBAR = 0U, .RASR = 0U },
    { .RBAR = ARM_MPU_RBAR(2U, 0x30000000U), .RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, ARM_MPU_REGION_SIZE_128MB) },
    { .RBAR = 0x50000000U, .RASR = ARM_MPU_RASR(0U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, ARM_MPU_REGION_SIZE_64MB) }
  };
  
  #define ASSERT_MPU_REGION(rnr, region) \
    MPU->RNR = rnr; \
    ASSERT_TRUE((MPU->RBAR & MPU_RBAR_ADDR_Msk) == (region.RBAR & MPU_RBAR_ADDR_Msk)); \
    ASSERT_TRUE(MPU->RASR == region.RASR)
  
  ClearMpu();
    
  ARM_MPU_SetRegion(table[1].RBAR, table[1].RASR);
  
  ASSERT_MPU_REGION(1U, table[0]);
  ASSERT_MPU_REGION(2U, table[1]);
  ASSERT_MPU_REGION(3U, table[0]);
  
  ARM_MPU_SetRegionEx(5U, table[2].RBAR, table[2].RASR);
  
  ASSERT_MPU_REGION(4U, table[0]);
  ASSERT_MPU_REGION(5U, table[2]);
  ASSERT_MPU_REGION(6U, table[0]);
  
  ARM_MPU_ClrRegion(5U);
  
  MPU->RNR = 5U;
  ASSERT_TRUE((MPU->RASR & MPU_RASR_ENABLE_Msk) == 0U);
  
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
    { .RBAR = ARM_MPU_RBAR(0U, 0x10000000U), .RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, ARM_MPU_REGION_SIZE_32MB)  },
    { .RBAR = ARM_MPU_RBAR(1U, 0x20000000U), .RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, ARM_MPU_REGION_SIZE_64MB)  },
    { .RBAR = ARM_MPU_RBAR(2U, 0x30000000U), .RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, ARM_MPU_REGION_SIZE_128MB) },
    { .RBAR = ARM_MPU_RBAR(3U, 0x40000000U), .RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, ARM_MPU_REGION_SIZE_256MB) },
    { .RBAR = ARM_MPU_RBAR(4U, 0x50000000U), .RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, ARM_MPU_REGION_SIZE_512MB) },
    { .RBAR = ARM_MPU_RBAR(5U, 0x60000000U), .RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, ARM_MPU_REGION_SIZE_16MB)  },
    { .RBAR = ARM_MPU_RBAR(6U, 0x70000000U), .RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, ARM_MPU_REGION_SIZE_8MB)   },
    { .RBAR = ARM_MPU_RBAR(7U, 0x80000000U), .RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, ARM_MPU_REGION_SIZE_4MB)   }
  };
  
  #define ASSERT_MPU_REGION(rnr, table) \
    MPU->RNR = rnr; \
    ASSERT_TRUE((MPU->RBAR & MPU_RBAR_ADDR_Msk) == (table[rnr].RBAR & MPU_RBAR_ADDR_Msk)); \
    ASSERT_TRUE(MPU->RASR == table[rnr].RASR)

  ClearMpu();
  
  ARM_MPU_Load(&(table[0]), 1U);
  
  ASSERT_MPU_REGION(0U, table);
  
  ARM_MPU_Load(&(table[1]), 5U);

  ASSERT_MPU_REGION(0U, table);
  ASSERT_MPU_REGION(1U, table);
  ASSERT_MPU_REGION(2U, table);
  ASSERT_MPU_REGION(3U, table);
  ASSERT_MPU_REGION(4U, table);
  ASSERT_MPU_REGION(5U, table);

  ARM_MPU_Load(&(table[6]), 2U);

  ASSERT_MPU_REGION(5U, table);
  ASSERT_MPU_REGION(6U, table);
  ASSERT_MPU_REGION(7U, table);

  #undef ASSERT_MPU_REGION
#endif
}


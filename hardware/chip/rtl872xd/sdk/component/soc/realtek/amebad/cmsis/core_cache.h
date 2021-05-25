/**************************************************************************//**
 * @file     core_cache.h
 * @brief    Realtek TM4 TM0 CMSIS Core Peripheral Access Layer Header File
 * @version  V5.0.1
 * @date     25. November 2016
 ******************************************************************************/
/*
 * Copyright (c) 2009-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CORE_CACHE_H
#define __CORE_CACHE_H

#ifdef __cplusplus
 extern "C" {
#endif

#define __ICACHE_PRESENT          1U
#define __DCACHE_PRESENT          1U

/* check device defines and use defaults */
#if defined __CHECK_DEVICE_DEFINES
  #ifndef __ICACHE_PRESENT
    #define __ICACHE_PRESENT          0U
    #warning "__ICACHE_PRESENT not defined in device header file; using default!"
  #endif

  #ifndef __DCACHE_PRESENT
    #define __DCACHE_PRESENT          0U
    #warning "__DCACHE_PRESENT not defined in device header file; using default!"
  #endif
#endif


/* ##########################  Cache functions  #################################### */
/**
  \ingroup  CMSIS_Core_FunctionInterface
  \defgroup CMSIS_Core_CacheFunctions Cache Functions
  \brief    Functions that configure Instruction and Data cache.
  @{
 */

/* Cache Size ID Register Macros */
#define CCSIDR_WAYS(x)         (((x) & SCB_CCSIDR_ASSOCIATIVITY_Msk) >> SCB_CCSIDR_ASSOCIATIVITY_Pos)
#define CCSIDR_SETS(x)         (((x) & SCB_CCSIDR_NUMSETS_Msk      ) >> SCB_CCSIDR_NUMSETS_Pos      )


/**
  \brief   Enable I-Cache
  \details Turns on I-Cache
  */
__STATIC_INLINE void SCB_EnableICache (void)
{
  #if defined (__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    __DSB();
    __ISB();
    SCB->ICIALLU = 0UL;                     /* invalidate I-Cache */
    __DSB();
    __ISB();
    SCB->CCR |=  (uint32_t)SCB_CCR_IC_Msk;  /* enable I-Cache */
    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Disable I-Cache
  \details Turns off I-Cache
  */
__STATIC_INLINE void SCB_DisableICache (void)
{
  #if defined (__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    __DSB();
    __ISB();
    SCB->CCR &= ~(uint32_t)SCB_CCR_IC_Msk;  /* disable I-Cache */
    SCB->ICIALLU = 0UL;                     /* invalidate I-Cache */
    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Invalidate I-Cache
  \details Invalidates I-Cache
  */
__STATIC_INLINE void SCB_InvalidateICache (void)
{
  #if defined (__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    __DSB();
    __ISB();
    SCB->ICIALLU = 0UL;
    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Enable D-Cache
  \details Turns on D-Cache
  */
__STATIC_INLINE void SCB_EnableDCache (void)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    SCB->CSSELR = 0U; /*(0U << 1U) | 0U;*/  /* Level 1 data cache */
    __DSB();

    ccsidr = SCB->CCSIDR;

                                            /* invalidate D-Cache */
    sets = (uint32_t)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
      do {
        SCB->DCISW = (((sets << SCB_DCISW_SET_Pos) & SCB_DCISW_SET_Msk) |
                      ((ways << SCB_DCISW_WAY_Pos) & SCB_DCISW_WAY_Msk)  );
        #if defined ( __CC_ARM )
          __schedule_barrier();
        #endif
      } while (ways-- != 0U);
    } while(sets-- != 0U);
    __DSB();

    SCB->CCR |=  (uint32_t)SCB_CCR_DC_Msk;  /* enable D-Cache */

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Disable D-Cache
  \details Turns off D-Cache
  */
__STATIC_INLINE void SCB_DisableDCache (void)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    register uint32_t ccsidr;
    register uint32_t sets;
    register uint32_t ways;

    SCB->CSSELR = 0U; /*(0U << 1U) | 0U;*/  /* Level 1 data cache */
    __DSB();

    SCB->CCR &= ~(uint32_t)SCB_CCR_DC_Msk;  /* disable D-Cache */
    __DSB();

    ccsidr = SCB->CCSIDR;

                                            /* clean & invalidate D-Cache */
    sets = (uint32_t)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
      do {
        SCB->DCCISW = (((sets << SCB_DCCISW_SET_Pos) & SCB_DCCISW_SET_Msk) |
                       ((ways << SCB_DCCISW_WAY_Pos) & SCB_DCCISW_WAY_Msk)  );
        #if defined ( __CC_ARM )
          __schedule_barrier();
        #endif
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Invalidate D-Cache
  \details Invalidates D-Cache
  */
__STATIC_INLINE void SCB_InvalidateDCache (void)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    SCB->CSSELR = 0U; /*(0U << 1U) | 0U;*/  /* Level 1 data cache */
    __DSB();

    ccsidr = SCB->CCSIDR;

                                            /* invalidate D-Cache */
    sets = (uint32_t)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
      do {
        SCB->DCISW = (((sets << SCB_DCISW_SET_Pos) & SCB_DCISW_SET_Msk) |
                      ((ways << SCB_DCISW_WAY_Pos) & SCB_DCISW_WAY_Msk)  );
        #if defined ( __CC_ARM )
          __schedule_barrier();
        #endif
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Clean D-Cache
  \details Cleans D-Cache
  */
__STATIC_INLINE void SCB_CleanDCache (void)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

     SCB->CSSELR = 0U; /*(0U << 1U) | 0U;*/  /* Level 1 data cache */
   __DSB();

    ccsidr = SCB->CCSIDR;

                                            /* clean D-Cache */
    sets = (uint32_t)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
      do {
        SCB->DCCSW = (((sets << SCB_DCCSW_SET_Pos) & SCB_DCCSW_SET_Msk) |
                      ((ways << SCB_DCCSW_WAY_Pos) & SCB_DCCSW_WAY_Msk)  );
        #if defined ( __CC_ARM )
          __schedule_barrier();
        #endif
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Clean & Invalidate D-Cache
  \details Cleans and Invalidates D-Cache
  */
__STATIC_INLINE void SCB_CleanInvalidateDCache (void)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    SCB->CSSELR = 0U; /*(0U << 1U) | 0U;*/  /* Level 1 data cache */
    __DSB();

    ccsidr = SCB->CCSIDR;

                                            /* clean & invalidate D-Cache */
    sets = (uint32_t)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
      do {
        SCB->DCCISW = (((sets << SCB_DCCISW_SET_Pos) & SCB_DCCISW_SET_Msk) |
                       ((ways << SCB_DCCISW_WAY_Pos) & SCB_DCCISW_WAY_Msk)  );
        #if defined ( __CC_ARM )
          __schedule_barrier();
        #endif
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   D-Cache Invalidate by address
  \details Invalidates D-Cache for the given address
  \param[in]   addr    address (aligned to 32-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
__STATIC_INLINE void SCB_InvalidateDCache_by_Addr (uint32_t *addr, int32_t dsize)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
     int32_t op_size = dsize;
    uint32_t op_addr = (uint32_t)addr;
     int32_t linesize = 32;                /* in Cortex-M7 size of cache line is fixed to 8 words (32 bytes) */

    __DSB();

    while (op_size > 0) {
      SCB->DCIMVAC = op_addr;
      op_addr += (uint32_t)linesize;
      op_size -=           linesize;
    }

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   D-Cache Clean by address
  \details Cleans D-Cache for the given address
  \param[in]   addr    address (aligned to 32-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
__STATIC_INLINE void SCB_CleanDCache_by_Addr (uint32_t *addr, int32_t dsize)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
     int32_t op_size = dsize;
    uint32_t op_addr = (uint32_t) addr;
     int32_t linesize = 32;                /* in Cortex-M7 size of cache line is fixed to 8 words (32 bytes) */

    __DSB();

    while (op_size > 0) {
      SCB->DCCMVAC = op_addr;
      op_addr += (uint32_t)linesize;
      op_size -=           linesize;
    }

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   D-Cache Clean and Invalidate by address
  \details Cleans and invalidates D_Cache for the given address
  \param[in]   addr    address (aligned to 32-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
__STATIC_INLINE void SCB_CleanInvalidateDCache_by_Addr (uint32_t *addr, int32_t dsize)
{
  #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
     int32_t op_size = dsize;
    uint32_t op_addr = (uint32_t) addr;
     int32_t linesize = 32;                /* in Cortex-M7 size of cache line is fixed to 8 words (32 bytes) */

    __DSB();

    while (op_size > 0) {
      SCB->DCCIMVAC = op_addr;
      op_addr += (uint32_t)linesize;
      op_size -=           linesize;
    }

    __DSB();
    __ISB();
  #endif
}


/*@} end of CMSIS_Core_CacheFunctions */

#ifdef __cplusplus
}
#endif

#endif /* __CORE_CACHE_H */


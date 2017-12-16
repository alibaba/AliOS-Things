/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     core_ck610.c
 * @brief    CSI CK610 Core Peripheral Access Layer File
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdint.h>
#include <core_ck610.h>
#include <config.h>

/*******************************************************************************
 *                Hardware Abstraction Layer
  Core Function Interface contains:
  - Core VIC Functions
  - Core CORET Functions
  - Core Register Access Functions
 ******************************************************************************/
/* ##########################  Cache functions  #################################### */
/**
  \ingroup  CSI_Core_FunctionInterface
  \defgroup CSI_Core_CacheFunctions Cache Functions
  \brief    Functions that configure Instruction and Data cache.
  @{
 */

/**
  \brief   Enable I-Cache
  \details Turns on I-Cache
  */
void drv_icache_enable (void)
{
    uint32_t tmp=0;
    __ASM volatile("mfcr %0, cr18\n\t"
                   "bseti %0, 2\n\t"
                   "mtcr %0, cr18\n\t"
                ::"r"(tmp));
}


/**
  \brief   Disable I-Cache
  \details Turns off I-Cache
  */
void drv_icache_disable (void)
{
    uint32_t tmp=0;
    __ASM volatile("mfcr %0, cr18\n\t"
                   "bclri %0, 2\n\t"
                   "mtcr %0, cr18\n\t"
                ::"r"(tmp));
}


/**
  \brief   Invalidate I-Cache
  \details Invalidates I-Cache
  */
void drv_icache_invalid (void)
{
    set_cr17(0x11);
    set_cr17(INS_CACHE | CACHE_INV);
}


/**
  \brief   Enable D-Cache
  \details Turns on D-Cache
  \note    I-Cache also turns on.
  */
void drv_dcache_enable (void)
{
    uint32_t tmp=0;
    __ASM volatile("mfcr %0, cr18\n\t"
                   "bseti %0, 3\n\t"
                   "mtcr %0, cr18\n\t"
                ::"r"(tmp));
}


/**
  \brief   Disable D-Cache
  \details Turns off D-Cache
  \note    I-Cache also turns off.
  */
void drv_dcache_disable (void)
{
    uint32_t tmp=0;
    __ASM volatile("mfcr %0, cr18\n\t"
                   "bclri %0, 3\n\t"
                   "mtcr %0, cr18\n\t"
                ::"r"(tmp));
}


/**
  \brief   Invalidate D-Cache
  \details Invalidates D-Cache
  \note    I-Cache also invalid
  */
void drv_dcache_invalid (void)
{
    set_cr17(DATA_CACHE | CACHE_INV);
}


/**
  \brief   Clean D-Cache
  \details Cleans D-Cache
  \note    I-Cache also cleans
  */
void drv_dcache_clean (void)
{
    set_cr17(DATA_CACHE | CACHE_CLR);
}


/**
  \brief   Clean & Invalidate D-Cache
  \details Cleans and Invalidates D-Cache
  \note    I-Cache also flush.
  */
void drv_dcache_clean_invalid (void)
{
    set_cr17(DATA_CACHE | CACHE_CLR | CACHE_INV);
}

static inline void set_cache_range (uint32_t start, uint32_t end, uint32_t value)
{
    if (!(start & SSEG0_BASE_ADDR) || (end - start) &~(CACHE_RANGE_MAX_SIZE - 1)) {
        set_cr17(value);
    }

    if (value & INS_CACHE) {
        drv_icache_disable();
    }
    uint32_t i;
    for (i = start; i < end; i += L1_CACHE_BYTES) {
        set_cr22(i);
        set_cr17(CACHE_OMS | value);
    }

    if (end & (L1_CACHE_BYTES-1)) {
        set_cr22(end);
        set_cr17(CACHE_OMS | value);
    }

    if (value & INS_CACHE) {
        drv_icache_enable();
    }

}

/**
  \brief   D-Cache Invalidate by address
  \details Invalidates D-Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
void drv_dcache_invalid_range (uint32_t *addr, int32_t dsize)
{
    set_cache_range((uint32_t)addr, (uint32_t)addr + dsize, (DATA_CACHE | CACHE_INV));
}


/**
  \brief   D-Cache Clean by address
  \details Cleans D-Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
void drv_dcache_clean_range (uint32_t *addr, int32_t dsize)
{
    set_cache_range((uint32_t)addr, (uint32_t)addr + dsize, (DATA_CACHE | CACHE_CLR));
}


/**
  \brief   D-Cache Clean and Invalidate by address
  \details Cleans and invalidates D_Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
void drv_dcache_clean_invalid_range (uint32_t *addr, int32_t dsize)
{
    set_cache_range((uint32_t)addr, (uint32_t)addr + dsize, (DATA_CACHE | CACHE_CLR | CACHE_INV));
}

/*@} end of CSI_Core_CacheFunctions */

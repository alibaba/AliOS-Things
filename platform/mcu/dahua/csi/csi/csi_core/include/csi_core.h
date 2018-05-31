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
 * @file     csi_core.h
 * @brief    CSI Core Layer Header File
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef _CORE_H_
#define _CORE_H_

#include <stdint.h>
#include <csi_gcc/csi_gcc.h>

#if defined(__CK801__)
#include <core/core_ck801.h>
#elif defined(__CK802__)
#include <core/core_ck802.h>
#elif defined(__CK803S__) || defined(__CK803__)
#include <core/core_ck803S.h>
#elif defined(__CK802__)
#include <core/core_ck610.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* ##################################    VIC function  ############################################ */

/**
  \brief   initialize the VIC interrupt controller
  \param [in]      prio_bits  the priority bits of VIC interrupt controller.
 */
void csi_vic_init(uint32_t prio_bits);

/**
  \brief   Enable External Interrupt
  \details Enables a device-specific interrupt in the VIC interrupt controller.
  \param [in]      irq_num  External interrupt number. Value cannot be negative.
 */
void csi_vic_enable_irq(int32_t irq_num);
/**
  \brief   Disable External Interrupt
  \details Disables a device-specific interrupt in the VIC interrupt controller.
  \param [in]      irq_num  External interrupt number. Value cannot be negative.
 */
void csi_vic_disable_irq(int32_t irq_num);

/**
  \brief   Get Enabled Interrupt irqnums
  \details Reads the enabled interrupt register in the NVIC interrupt controller.
  \return  Interrput status is enabled.
 */
uint32_t csi_vic_get_enabled_irq(void);

/**
  \brief   Get Pending Interrupt
  \details Reads the pending register in the VIC and returns the pending bit for the specified interrupt.
  \param [in]      irq_num  Interrupt number.
  \return             0  Interrupt status is not pending.
  \return             1  Interrupt status is pending.
 */
uint32_t csi_vic_get_pending_irq(int32_t irq_num);

/**
  \brief   Set Pending Interrupt
  \details Sets the pending bit of an external interrupt.
  \param [in]      irq_num  Interrupt number. Value cannot be negative.
 */
void csi_vic_set_pending_irq(int32_t irq_num);

/**
  \brief   Clear Pending Interrupt
  \details Clears the pending bit of an external interrupt.
  \param [in]      irq_num  External interrupt number. Value cannot be negative.
 */
void csi_vic_clear_pending_irq(int32_t irq_num);

/**
  \brief   Get Active Interrupt
  \details Reads the active register in the VIC and returns the active bit for the device specific interrupt.
  \param [in]      irq_num  Device specific interrupt number.
  \return             0  Interrupt status is not active.
  \return             1  Interrupt status is active.
  \note    irq_num must not be negative.
 */
uint32_t csi_vic_get_active(int32_t irq_num);

/**
  \brief   Set Interrupt Priority
  \details Sets the priority of an interrupt.
  \note    The priority cannot be set for every core interrupt.
  \param [in]      irq_num  Interrupt number.
  \param [in]  priority  Priority to set.
*/
void csi_vic_set_prio(int32_t irq_num, uint32_t priority);
/**
  \brief   Get Interrupt Priority
  \details Reads the priority of an interrupt.
           The interrupt number can be positive to specify an external (device specific) interrupt,
           or negative to specify an internal (core) interrupt.
  \param [in]   irq_num  Interrupt number.
  \return             Interrupt Priority.
                      Value is aligned automatically to the implemented priority bits of the microcontroller.
 */
uint32_t csi_vic_get_prio(int32_t irq_num);

/**
  \brief   Set interrupt handler
  \details Set the interrupt handler according to the interrupt num, the handler will be filled in __Vectors[].
  \param [in]      IRQn  Interrupt number.
  \param [in]   handler  Interrupt handler.
 */
void csi_vic_set_vector(int32_t irq_num, uint32_t handler);

/**
  \brief   Get interrupt handler
  \details Get the address of interrupt handler function.
  \param [in]      IRQn  Interrupt number.
 */
uint32_t csi_vic_get_vector(int32_t irq_num);

/*@} end of CSI_Core_VICFunctions */


/* ##########################  Cache functions  #################################### */

/**
  \brief   Enable I-Cache
  \details Turns on I-Cache
  */
void csi_icache_enable(void);

/**
  \brief   Disable I-Cache
  \details Turns off I-Cache
  */
void csi_icache_disable(void);

/**
  \brief   Invalidate I-Cache
  \details Invalidates I-Cache
  */
void csi_icache_invalid(void);

/**
  \brief   Enable D-Cache
  \details Turns on D-Cache
  \note    I-Cache also turns on.
  */
void csi_dcache_enable(void);

/**
  \brief   Disable D-Cache
  \details Turns off D-Cache
  \note    I-Cache also turns off.
  */
void csi_dcache_disable(void);

/**
  \brief   Invalidate D-Cache
  \details Invalidates D-Cache
  \note    I-Cache also invalid
  */
void csi_dcache_invalid(void);

/**
  \brief   Clean D-Cache
  \details Cleans D-Cache
  \note    I-Cache also cleans
  */
void csi_dcache_clean(void);

/**
  \brief   Clean & Invalidate D-Cache
  \details Cleans and Invalidates D-Cache
  \note    I-Cache also flush.
  */
void csi_dcache_clean_invalid(void);


/**
  \brief   D-Cache Invalidate by address
  \details Invalidates D-Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
void csi_dcache_invalid_range(uint32_t *addr, int32_t dsize);

/**
  \brief   D-Cache Clean by address
  \details Cleans D-Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
void csi_dcache_clean_range(uint32_t *addr, int32_t dsize);

/**
  \brief   D-Cache Clean and Invalidate by address
  \details Cleans and invalidates D_Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
void csi_dcache_clean_invalid_range(uint32_t *addr, int32_t dsize);

/**
  \brief   setup cacheable range Cache
  \details setup Cache range
  */
void csi_cache_set_range(uint32_t index, uint32_t baseAddr, uint32_t size, uint32_t enable);

/**
  \brief   Enable cache profile
  \details Turns on Cache profile
  */
void csi_cache_enable_profile(void);

/**
  \brief   Disable cache profile
  \details Turns off Cache profile
  */
void csi_cache_disable_profile(void);
/**
  \brief   Reset cache profile
  \details Reset Cache profile
  */
void csi_cache_reset_profile(void);

/**
  \brief   cache access times
  \details Cache access times
  \note    every 256 access add 1.
  */
uint32_t csi_cache_get_access_time(void);

/**
  \brief   cache miss times
  \details Cache miss times
  \note    every 256 miss add 1.
  */
uint32_t csi_cache_get_miss_time(void);

/* ##################################    SysTick function  ############################################ */

/**
  \brief   CORE timer Configuration
  \details Initializes the System Timer and its interrupt, and starts the System Tick Timer.
           Counter is in free running mode to generate periodic interrupts.
  \param [in]  ticks  Number of ticks between two interrupts.
  \param [in]  irq_num   core timer Interrupt number.
  \return          0  Function succeeded.
  \return          1  Function failed.
  \note    When the variable <b>__Vendor_SysTickConfig</b> is set to 1, then the
           function <b>SysTick_Config</b> is not included. In this case, the file <b><i>device</i>.h</b>
           must contain a vendor-specific implementation of this function.
 */
uint32_t csi_coret_config(uint32_t ticks, int32_t irq_num);

/**
  \brief   get CORE timer reload value
  \return          CORE timer counter value.
 */
uint32_t csi_coret_get_load(void);

/**
  \brief   get CORE timer counter value
  \return          CORE timer counter value.
 */
uint32_t csi_coret_get_value(void);

/**
  \brief   Save the Irq context
  \details save the psr result before disable irq.
  \param [in]      irq_num  External interrupt number. Value cannot be negative.
 */
__ALWAYS_INLINE uint32_t csi_irq_save(void)
{
    uint32_t result;
    result = __get_PSR();
    __disable_irq();
    return(result);
}

/**
  \brief   Restore the Irq context
  \details restore saved primask state.
  \param [in]      irq_state  psr irq state.
 */
__ALWAYS_INLINE void csi_irq_restore(uint32_t irq_state)
{
    __set_PSR(irq_state);
}


#ifdef __cplusplus
}
#endif

#endif /* _CORE_H_ */

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
 * @file     core_ck803S.c
 * @brief    CSI CK803S Core Peripheral Access Layer File
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdint.h>
#include <core/core_ck803S.h>
#include <config.h>

/*******************************************************************************
 *                Hardware Abstraction Layer
  Core Function Interface contains:
  - Core VIC Functions
  - Core CORET Functions
  - Core Register Access Functions
 ******************************************************************************/
/**
  \defgroup CSI_Core_FunctionInterface Functions and Instructions Reference
*/

/* ##########################   VIC functions  #################################### */
/**
  \ingroup  CSI_Core_FunctionInterface
  \defgroup CSI_Core_VICFunctions VIC Functions
  \brief    Functions that manage interrupts and exceptions via the VIC.
  @{
 */

/* The following MACROS handle generation of the register offset and byte masks */
#define _BIT_SHIFT(IRQn)         (  ((((uint32_t)(int32_t)(IRQn))         )      &  0x03UL) * 8UL)
#define _IP_IDX(IRQn)            (   (((uint32_t)(int32_t)(IRQn))                >>    2UL)      )

extern void (*__Vectors[])(void);

static uint32_t s_vic_prio_bits = __VIC_PRIO_BITS;

/**
  \brief   initialize the VIC interrupt controller
  \param [in]      prio_bits  the priority bits of VIC interrupt controller.
 */
void csi_vic_init(uint32_t prio_bits)
{
    if (s_vic_prio_bits >= 8U) {
        return;
    }

    s_vic_prio_bits = prio_bits;
}

/**
  \brief   Enable External Interrupt
  \details Enables a device-specific interrupt in the VIC interrupt controller.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void csi_vic_enable_irq(int32_t IRQn)
{
    VIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
#ifdef CONFIG_SYSTEM_SECURE
    VIC->ISSR[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
#endif
}

/**
  \brief   Disable External Interrupt
  \details Disables a device-specific interrupt in the VIC interrupt controller.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void csi_vic_disable_irq(int32_t IRQn)
{
    VIC->ICER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

/**
  \brief   Enable External Secure Interrupt
  \details Enables a secure device-specific interrupt in the VIC interrupt controller.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void csi_vic_enable_sirq(int32_t IRQn)
{
    VIC->ISSR[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

/**
  \brief   Get Enabled Interrupt irqnums
  \details Reads the enabled interrupt register in the NVIC interrupt controller.
  \return  Interrput status is enabled.
 */
uint32_t csi_vic_get_enabled_irq(void)
{
    return (uint32_t)(VIC->ISER[0U]);
}

/**
  \brief   Get Pending Interrupt
  \details Reads the pending register in the VIC and returns the pending bit for the specified interrupt.
  \param [in]      IRQn  Interrupt number.
  \return             0  Interrupt status is not pending.
  \return             1  Interrupt status is pending.
 */
uint32_t csi_vic_get_pending_irq(int32_t IRQn)
{
    return ((uint32_t)(((VIC->ISPR[0U] & (1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
}

/**
  \brief   Set Pending Interrupt
  \details Sets the pending bit of an external interrupt.
  \param [in]      IRQn  Interrupt number. Value cannot be negative.
 */
void csi_vic_set_pending_irq(int32_t IRQn)
{
    VIC->ISPR[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

/**
  \brief   Clear Pending Interrupt
  \details Clears the pending bit of an external interrupt.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void csi_vic_clear_pending_irq(int32_t IRQn)
{
    VIC->ICPR[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

/**
  \brief   Get Wake up Interrupt
  \details Reads the wake up register in the VIC and returns the pending bit for the specified interrupt.
  \param [in]      IRQn  Interrupt number.
  \return             0  Interrupt is not set as wake up interrupt.
  \return             1  Interrupt is set as wake up interrupt.
 */
uint32_t csi_vic_get_wakeup_irq(int32_t IRQn)
{
    return ((uint32_t)(((VIC->IWER[0U] & (1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
}

/**
  \brief   Set Wake up Interrupt
  \details Sets the wake up bit of an external interrupt.
  \param [in]      IRQn  Interrupt number. Value cannot be negative.
 */
void csi_vic_set_wakeup_irq(int32_t IRQn)
{
    VIC->IWER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

/**
  \brief   Clear Wake up Interrupt
  \details Clears the wake up bit of an external interrupt.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void csi_vic_clear_wakeup_irq(int32_t IRQn)
{
    VIC->IWDR[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

/**
  \brief   Get Active Interrupt
  \details Reads the active register in the VIC and returns the active bit for the device specific interrupt.
  \param [in]      IRQn  Device specific interrupt number.
  \return             0  Interrupt status is not active.
  \return             1  Interrupt status is active.
  \note    IRQn must not be negative.
 */
uint32_t csi_vic_get_active(int32_t IRQn)
{
    return ((uint32_t)(((VIC->IABR[0] & (1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
}

/**
  \brief   Set Threshold register
  \details set the threshold register in the VIC.
  \param [in]      VectThreshold  specific vecter threshold.
  \param [in]      PrioThreshold  specific priority threshold.
 */
void csi_vic_set_threshold(uint32_t VectThreshold, uint32_t PrioThreshold)
{
    VIC->IPTR = 0x80000000 | (((VectThreshold + 32) & 0xFF) << 8) | ((PrioThreshold & 0x3) << 6);
}

/**
  \brief   Set Interrupt Priority
  \details Sets the priority of an interrupt.
  \note    The priority cannot be set for every core interrupt.
  \param [in]      IRQn  Interrupt number.
  \param [in]  priority  Priority to set.
 */
void csi_vic_set_prio(int32_t IRQn, uint32_t priority)
{
    VIC->IPR[_IP_IDX(IRQn)]  = ((uint32_t)(VIC->IPR[_IP_IDX(IRQn)]  & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
                                 (((priority << (8U - s_vic_prio_bits)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
}

/**
  \brief   Get Interrupt Priority
  \details Reads the priority of an interrupt.
           The interrupt number can be positive to specify an external (device specific) interrupt,
           or negative to specify an internal (core) interrupt.
  \param [in]   IRQn  Interrupt number.
  \return             Interrupt Priority.
                      Value is aligned automatically to the implemented priority bits of the microcontroller.
 */
uint32_t csi_vic_get_prio(int32_t IRQn)
{
    return ((uint32_t)(((VIC->IPR[ _IP_IDX(IRQn)] >> _BIT_SHIFT(IRQn)) & (uint32_t)0xFFUL) >> (8U - s_vic_prio_bits)));
}

/**
  \brief   Set interrupt handler
  \details Set the interrupt handler according to the interrupt num, the handler will be filled in __Vectors[].
  \param [in]      IRQn  Interrupt number.
  \param [in]   handler  Interrupt handler.
 */
void csi_vic_set_vector(int32_t IRQn, uint32_t handler)
{
    if (IRQn >= 0 && IRQn < 32) {
        __Vectors[IRQn] = (void *)handler;
    }
}

/**
  \brief   Get interrupt handler
  \details Get the address of interrupt handler function.
  \param [in]      IRQn  Interrupt number.
 */
uint32_t csi_vic_get_vector(int32_t IRQn)
{
    if (IRQn >= 0 && IRQn < 32) {
        return (uint32_t)__Vectors[IRQn];
    }

    return 0;
}

/*@} end of CSI_Core_VICFunctions */

/* ##################################    SysTick function  ############################################ */
/**
  \ingroup  CSI_Core_FunctionInterface
  \defgroup CSI_Core_SysTickFunctions SysTick Functions
  \brief    Functions that configure the System.
  @{
 */


/**
  \brief   CORE timer Configuration
  \details Initializes the System Timer and its interrupt, and starts the System Tick Timer.
           Counter is in free running mode to generate periodic interrupts.
  \param [in]  ticks  Number of ticks between two interrupts.
  \param [in]  IRQn   core timer Interrupt number.
  \return          0  Function succeeded.
  \return          1  Function failed.
  \note    When the variable <b>__Vendor_SysTickConfig</b> is set to 1, then the
           function <b>SysTick_Config</b> is not included. In this case, the file <b><i>device</i>.h</b>
           must contain a vendor-specific implementation of this function.
 */
uint32_t csi_coret_config(uint32_t ticks, int32_t IRQn)
{
    if ((ticks - 1UL) > CORET_LOAD_RELOAD_Msk) {
        return (1UL);                                                   /* Reload value impossible */
    }

    CORET->LOAD  = (uint32_t)(ticks - 1UL);                           /* set reload register */
    csi_vic_set_prio(IRQn, (1UL << s_vic_prio_bits) - 1UL);    /* set Priority for Systick Interrupt */
    CORET->VAL   = 0UL;                                               /* Load the CORET Counter Value */
    CORET->CTRL  = CORET_CTRL_CLKSOURCE_Msk |
                   CORET_CTRL_TICKINT_Msk |
                   CORET_CTRL_ENABLE_Msk;                           /* Enable CORET IRQ and CORET Timer */
    return (0UL);                                                     /* Function successful */
}

/**
  \brief   get CORE timer reload value
  \return          CORE timer counter value.
 */
uint32_t csi_coret_get_load(void)
{
    return CORET->LOAD;
}

/**
  \brief   get CORE timer counter value
  \return          CORE timer counter value.
 */
uint32_t csi_coret_get_value(void)
{
    return CORET->VAL;
}

/*@} end of CSI_Core_SysTickFunctions */

#if 0
/* ##################################### DCC function ########################################### */
/**
  \ingroup  CSI_Core_FunctionInterface
  \defgroup CSI_core_DebugFunctions HAD Functions
  \brief    Functions that access the HAD debug interface.
  @{
 */

/**
  \brief   HAD Send Character
  \details Transmits a character via the HAD channel 0, and
           \li Just returns when no debugger is connected that has booked the output.
           \li Is blocking when a debugger is connected, but the previous character sent has not been transmitted.
  \param [in]     ch  Character to transmit.
  \returns            Character to transmit.
 */
uint32_t HAD_SendChar(uint32_t ch)
{
    DCC->DERJR = (uint8_t)ch;

    return (ch);
}


/**
  \brief   HAD Receive Character
  \details Inputs a character via the external variable \ref HAD_RxBuffer.
  \return             Received character.
  \return         -1  No character pending.
 */
int32_t HAD_ReceiveChar(void)
{
    int32_t ch = -1;                           /* no character available */

    if (_FLD2VAL(DCC_EHSR_JW, DCC->EHSR)) {
        ch = DCC->DERJW;
    }

    return (ch);
}


/**
  \brief   HAD Check Character
  \details Checks whether a character is pending for reading in the variable \ref HAD_RxBuffer.
  \return          0  No character available.
  \return          1  Character available.
 */
int32_t HAD_CheckChar(void)
{
    return _FLD2VAL(DCC_EHSR_JW, DCC->EHSR);                              /* no character available */
}

#endif

/*@} end of CSI_core_DebugFunctions */
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
void csi_icache_enable (void)
{
  #if (__ICACHE_PRESENT == 1U)
    __DSB();
    __ISB();
    CACHE->CIR = CACHE_CIR_INV_ALL_Msk;         /* invalidate all Cache */
    CACHE->CER |=  (uint32_t)(CACHE_CER_EN_Msk | CACHE_CER_CFIG_Msk);  /* enable all Cache */
    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Disable I-Cache
  \details Turns off I-Cache
  */
void csi_icache_disable (void)
{
  #if (__ICACHE_PRESENT == 1U)
    __DSB();
    __ISB();
    CACHE->CER &=  ~(uint32_t)(CACHE_CER_EN_Msk | CACHE_CER_CFIG_Msk);  /* disable all Cache */
    CACHE->CIR = CACHE_CIR_INV_ALL_Msk;          /* invalidate all Cache */
    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Invalidate I-Cache
  \details Invalidates I-Cache
  */
void csi_icache_invalid (void)
{
  #if (__ICACHE_PRESENT == 1U)
    __DSB();
    __ISB();
    CACHE->CIR = CACHE_CIR_INV_ALL_Msk;         /* invalidate all Cache */
    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Enable D-Cache
  \details Turns on D-Cache
  \note    I-Cache also turns on.
  */
void csi_dcache_enable (void)
{
  #if (__DCACHE_PRESENT == 1U)
    __DSB();
    __ISB();
    CACHE->CIR = CACHE_CIR_INV_ALL_Msk;         /* invalidate all Cache */
    CACHE->CER =  (uint32_t)(CACHE_CER_EN_Msk | CACHE_CER_WB_Msk | CACHE_CER_DCW_Msk);  /* enable all Cache */
    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Disable D-Cache
  \details Turns off D-Cache
  \note    I-Cache also turns off.
  */
void csi_dcache_disable (void)
{
  #if (__DCACHE_PRESENT == 1U)
    __DSB();
    __ISB();
    CACHE->CER &=  ~(uint32_t)CACHE_CER_EN_Msk;  /* disable all Cache */
    CACHE->CIR = CACHE_CIR_INV_ALL_Msk;          /* invalidate all Cache */
    __DSB();
    __ISB();

  #endif
}


/**
  \brief   Invalidate D-Cache
  \details Invalidates D-Cache
  \note    I-Cache also invalid
  */
void csi_dcache_invalid (void)
{
  #if (__DCACHE_PRESENT == 1U)
    __DSB();
    __ISB();
    CACHE->CIR = CACHE_CIR_INV_ALL_Msk;         /* invalidate all Cache */
    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Clean D-Cache
  \details Cleans D-Cache
  \note    I-Cache also cleans
  */
void csi_dcache_clean (void)
{
  #if (__DCACHE_PRESENT == 1U)
    __DSB();
    __ISB();
    CACHE->CIR = _VAL2FLD(CACHE_CIR_CLR_ALL, 1);         /* clean all Cache */
    __DSB();
    __ISB();
  #endif
}


/**
  \brief   Clean & Invalidate D-Cache
  \details Cleans and Invalidates D-Cache
  \note    I-Cache also flush.
  */
void csi_dcache_clean_invalid (void)
{
  #if (__DCACHE_PRESENT == 1U)
    __DSB();
    __ISB();
    CACHE->CIR = _VAL2FLD(CACHE_CIR_INV_ALL, 1) | _VAL2FLD(CACHE_CIR_CLR_ALL, 1);         /* clean and inv all Cache */
    __DSB();
    __ISB();
  #endif
}


/**
  \brief   D-Cache Invalidate by address
  \details Invalidates D-Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
void csi_dcache_invalid_range (uint32_t *addr, int32_t dsize)
{
  #if (__DCACHE_PRESENT == 1U)
     int32_t op_size = dsize;
    uint32_t op_addr = (uint32_t)addr;
     int32_t linesize = 16;

    __DSB();

    while (op_size > 0) {
      CACHE->CIR = op_addr | _VAL2FLD(CACHE_CIR_INV_ONE, 1);
      op_addr += linesize;
      op_size -= linesize;
    }

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   D-Cache Clean by address
  \details Cleans D-Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
void csi_dcache_clean_range (uint32_t *addr, int32_t dsize)
{
  #if (__DCACHE_PRESENT == 1)
     int32_t op_size = dsize;
    uint32_t op_addr = (uint32_t) addr & CACHE_CIR_INV_ADDR_Msk;
     int32_t linesize = 16;

    __DSB();

    while (op_size > 0) {
      CACHE->CIR = op_addr | _VAL2FLD(CACHE_CIR_CLR_ONE, 1);
      op_addr += linesize;
      op_size -= linesize;
    }

    __DSB();
    __ISB();
  #endif
}


/**
  \brief   D-Cache Clean and Invalidate by address
  \details Cleans and invalidates D_Cache for the given address
  \param[in]   addr    address (aligned to 16-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
void csi_dcache_clean_invalid_range (uint32_t *addr, int32_t dsize)
{
  #if (__DCACHE_PRESENT == 1U)
     int32_t op_size = dsize;
    uint32_t op_addr = (uint32_t) addr;
     int32_t linesize = 16;

    __DSB();

    while (op_size > 0) {
      CACHE->CIR = op_addr | _VAL2FLD(CACHE_CIR_CLR_ONE, 1) | _VAL2FLD(CACHE_CIR_INV_ONE, 1);
      op_addr += linesize;
      op_size -= linesize;
    }

    __DSB();
    __ISB();
  #endif
}

/**
  \brief   setup cacheable range Cache
  \details setup Cache range
  */
void csi_cache_set_range (uint32_t index, uint32_t baseAddr, uint32_t size, uint32_t enable)
{
  CACHE->CRCR[index] =  ((baseAddr & CACHE_CRCR_BASE_ADDR_Msk) |
                         (_VAL2FLD(CACHE_CRCR_SIZE, size)) |
                         (_VAL2FLD(CACHE_CRCR_EN, enable)));
}

/**
  \brief   Enable cache profile
  \details Turns on Cache profile
  */
void csi_cache_enable_profile (void)
{
  CACHE->CPFCR |=  (uint32_t)CACHE_CPFCR_PFEN_Msk;
}

/**
  \brief   Disable cache profile
  \details Turns off Cache profile
  */
void csi_cache_disable_profile (void)
{
  CACHE->CPFCR &=  ~(uint32_t)CACHE_CPFCR_PFEN_Msk;
}

/**
  \brief   Reset cache profile
  \details Reset Cache profile
  */
void csi_cache_reset_profile (void)
{
  CACHE->CPFCR |=  (uint32_t)CACHE_CPFCR_PFRST_Msk;
}

/**
  \brief   cache access times
  \details Cache access times
  \note    every 256 access add 1.
  */
uint32_t csi_cache_get_access_time (void)
{
  return CACHE->CPFATR;
}

/**
  \brief   cache miss times
  \details Cache miss times
  \note    every 256 miss add 1.
  */
uint32_t csi_cache_get_miss_time (void)
{
  return CACHE->CPFMTR;
}

/*@} end of CSI_Core_CacheFunctions */

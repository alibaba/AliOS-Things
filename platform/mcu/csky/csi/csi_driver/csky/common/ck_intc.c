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
 * @file     ck_intc.c
 * @brief    CSI Source File for INTC Driver
 * @version  V1.0
 * @date     02. Jan 2018
 ******************************************************************************/

#include <stdint.h>
#include <csi_config.h>
#include <drv_intc.h>
#include <csi_core.h>
#include <ck_intc.h>
#include <io.h>

#define ERR_INTC(errno) (CSI_DRV_ERRNO_INTC_BASE | errno)

#define INTC_NULL_PARAM_CHK(para)                  \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_INTC(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

extern void *g_irqvector[];
static int find_bit(unsigned long irq)
{
    int i;

    for (i = 0; i < 32; i++) {
        if (irq & (1 << i)) {
            return i;
        }
    }

    return -1;
}
/**
  \brief   initialize the INTC interrupt controller
  \param [in]      prio_bits  the priority bits of INTC interrupt controller.
 */
void csi_intc_init(void)
{
    *(uint32_t *)(INTC_BASE) = (*(uint32_t *)(INTC_BASE)) & (~(1 << AVE_BIT));;
    *(uint32_t *)(CSKY_INTC_PRIO_BASE + 0) = 0x00010203;
    *(uint32_t *)(CSKY_INTC_PRIO_BASE + 0x4) = 0x04050607;
    *(uint32_t *)(CSKY_INTC_PRIO_BASE + 0x8) = 0x08090a0b;
    *(uint32_t *)(CSKY_INTC_PRIO_BASE + 0xc) = 0x0c0d0e0f;
    *(uint32_t *)(CSKY_INTC_PRIO_BASE + 0x10) = 0x10111213;
    *(uint32_t *)(CSKY_INTC_PRIO_BASE + 0x14) = 0x14151617;
    *(uint32_t *)(CSKY_INTC_PRIO_BASE + 0x18) = 0x18191a1b;
    *(uint32_t *)(CSKY_INTC_PRIO_BASE + 0x1c) = 0x1c1d1e1f;
    __enable_irq();
}

/**
  \brief   Enable External Interrupt
  \details Enables a device-specific interrupt in the INTC interrupt controller.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void csi_intc_enable_irq(int32_t IRQn)
{
    //default: enable normal interrupt.
    if (IRQn < 0 || IRQn >= LAST_RECORD) {
        return;
    }

    *(uint32_t *)(CSKY_INTC_NIER_BASE) = (*(uint32_t *)(CSKY_INTC_NIER_BASE)) | ((uint32_t)(1UL << IRQn));
}

/**
  \brief   Disable External Interrupt
  \details Disables a device-specific interrupt in the INTC interrupt controller.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void csi_intc_disable_irq(int32_t IRQn)
{
    if (IRQn < 0 || IRQn >= LAST_RECORD) {
        return;
    }

    *(uint32_t *)(CSKY_INTC_NIER_BASE) = (*(uint32_t *)(CSKY_INTC_NIER_BASE)) & (~(uint32_t)(1UL << IRQn));
}

/**
  \brief   Get Pending Interrupt
  \details Reads the pending register in the INTC and returns the pending bit for the specified interrupt.
  \param [in]      IRQn  Interrupt number.
  \return             0  Interrupt status is not pending.
  \return             1  Interrupt status is pending.
 */
uint32_t csi_intc_get_pending_irq(int32_t IRQn)
{
    if (IRQn < 0 || IRQn >= LAST_RECORD) {
        return 0;
    }

    volatile unsigned long irq_status = INTC->NIPR;
    return find_bit(irq_status);
}

/**
  \brief   Set Pending Interrupt
  \details Sets the pending bit of an external interrupt.
  \param [in]      IRQn  Interrupt number. Value cannot be negative.
 */
void csi_intc_set_pending_irq(int32_t IRQn)
{
    ;
}

/**
  \brief   Clear Pending Interrupt
  \details Clears the pending bit of an external interrupt.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void csi_intc_clear_pending_irq(int32_t IRQn)
{
    ;
}

/**
  \brief   Get Wake up Interrupt
  \details Reads the wake up register in the INTC and returns the pending bit for the specified interrupt.
  \param [in]      IRQn  Interrupt number.
  \return             0  Interrupt is not set as wake up interrupt.
  \return             1  Interrupt is set as wake up interrupt.
 */
uint32_t csi_intc_get_wakeup_irq(int32_t IRQn)
{
    return ERR_INTC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief   Set Wake up Interrupt
  \details Sets the wake up bit of an external interrupt.
  \param [in]      IRQn  Interrupt number. Value cannot be negative.
 */
void csi_intc_set_wakeup_irq(int32_t IRQn)
{
    ;
}

/**
  \brief   Clear Wake up Interrupt
  \details Clears the wake up bit of an external interrupt.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void csi_intc_clear_wakeup_irq(int32_t IRQn)
{
    ;
}

/**
  \brief   Get Active Interrupt
  \details Reads the active register in the INTC and returns the active bit for the device specific interrupt.
  \param [in]      IRQn  Device specific interrupt number.
  \return             0  Interrupt status is not active.
  \return             1  Interrupt status is active.
  \note    IRQn must not be negative.
 */
uint32_t csi_intc_get_active(int32_t IRQn)
{
    return ERR_INTC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief   Set Threshold register
  \details set the threshold register in the INTC.
  \param [in]      VectThreshold  specific vecter threshold.
  \param [in]      PrioThreshold  specific priority threshold.
 */
void csi_intc_set_threshold(uint32_t VectThreshold, uint32_t PrioThreshold)
{
    ;
}

/**
  \brief   Set Interrupt Priority
  \details Sets the priority of an interrupt.
  \note    The priority cannot be set for every core interrupt.
  \param [in]      IRQn  Interrupt number.
  \param [in]  priority  Priority to set.
 */
void csi_intc_set_prio(int32_t IRQn, uint32_t priority)
{
    if (IRQn < 0 || IRQn >= LAST_RECORD) {
        return;
    }

    uint32_t intergal = IRQn / 4;
    uint32_t decimal = IRQn % 4;
    write_bits(PRIO_REG_WIDTH, (4 - (decimal + 1)) * 4, (uint32_t *)(INTC_BASE + 0x40 + intergal * 4), priority);
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
uint32_t csi_intc_get_prio(int32_t IRQn)
{
    if (IRQn < 0 || IRQn >= LAST_RECORD) {
        return 0;
    }

    uint32_t intergal = IRQn / 4;
    uint32_t decimal = IRQn % 4;
    uint32_t ret = read_bits(PRIO_REG_WIDTH, (4 - (decimal + 1)) * 4, (uint32_t *)(INTC_BASE + 0x40 + intergal * 4));
    return ret;
}

/**
  \brief   Set interrupt handler
  \details Set the interrupt handler according to the interrupt num, the handler will be filled in g_irqvector[].
  \param [in]      IRQn  Interrupt number.
  \param [in]   handler  Interrupt handler.
 */
void csi_intc_set_vector(int32_t IRQn, uint32_t handler)
{
    if (IRQn >= 0) {
        g_irqvector[IRQn] = (void *)handler;
    }
}

/**
  \brief   Get interrupt handler
  \details Get the address of interrupt handler function.
  \param [in]      IRQn  Interrupt number.
 */
uint32_t csi_intc_get_vector(int32_t IRQn)
{
    if (IRQn >= 0) {
        return (uint32_t)g_irqvector[IRQn];
    }

    return 0;
}

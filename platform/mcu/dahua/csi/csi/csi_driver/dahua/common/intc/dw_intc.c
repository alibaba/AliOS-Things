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
 * @file     silan_intc.c
 * @brief    CSI CK802 Core Peripheral Access Layer File
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdint.h>
#include <config.h>
#include <drv_intc.h>
#include <soc.h>
//#include <core_ck803S.h>
#include "dw_intc.h"

#ifdef CONFIG_CHIP_DH5010M
extern void (*g_irqvector[64])(void);
#else
extern const unsigned char g_irqrelocation[64];
extern void (*g_irqvector_intc[22])(void);
#endif

#define ERR_INTC(errno) (CSI_DRV_ERRNO_INTC_BASE | errno)

#define INTC_NULL_PARAM_CHK(para)                  \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_INTC(EDRV_PARAMETER);   \
        }                                       \
    } while (0)


/**
  \brief   initialize the INTC interrupt controller
  \param [in]      prio_bits  the priority bits of INTC interrupt controller.
 */
void csi_intc_init(void)
{
    *(uint32_t *)(INTC_BASE) &= ~(1 << 31);
//    __enable_irq();
}

/**
  \brief   Enable External Interrupt
  \details Enables a device-specific interrupt in the INTC interrupt controller.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void csi_intc_enable_irq(int32_t IRQn)
{
#ifdef CONFIG_CHIP_DH5010M
  	if(IRQn < 32)
  	{
  	    *(PIC_MASK) &= ~(1 << IRQn);
  	}
	else
	{
		*(PIC_MASK_1) &= ~(1 << (IRQn - 32));
	}
#else
    if (IRQn < 32) {
        INTC->IRQ_INTEN_L |= (uint32_t)(1UL << IRQn);
        INTC->IRQ_INTMASK_L &= ~(uint32_t)(1UL << IRQn);
    } else if (IRQn >= 32 && IRQn < 64) {
        INTC->IRQ_INTEN_H |= (uint32_t)(1UL << (IRQn - 32));
        INTC->IRQ_INTMASK_H &= ~(uint32_t)(1UL << (IRQn - 32));
    }
#endif
}

/**
  \brief   Disable External Interrupt
  \details Disables a device-specific interrupt in the INTC interrupt controller.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void csi_intc_disable_irq(int32_t IRQn)
{
#ifdef CONFIG_CHIP_DH5010M
  	if(IRQn < 32)
  	{
  	    *(PIC_MASK) |= (1 << IRQn);
  	}
    else
    {
	    *(PIC_MASK_1) |= (1 << (IRQn - 32));
    }
#else
    if (IRQn < 32) {
        INTC->IRQ_INTEN_L &= ~(uint32_t)(1UL << IRQn);
        INTC->IRQ_INTMASK_L |= (uint32_t)(1UL << IRQn);
    } else if (IRQn >= 32 && IRQn < 64) {
        INTC->IRQ_INTEN_H &= ~(uint32_t)(1UL << (IRQn - 32));
        INTC->IRQ_INTMASK_H |= (uint32_t)(1UL << (IRQn - 32));
    }
#endif
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
#ifdef CONFIG_CHIP_DH5010M
    return 0;
#else
    unsigned int irq;
	unsigned int irqno = 0;

    irq = *(volatile unsigned long *)(INTC_BASE + 0x30);
    if (irq > 0)
    {
        /*先处理串口中断*/
        if(irq & 0x00001000)
        {
            return g_irqrelocation[12];
        }
        __asm__ __volatile__ 
        (
            "ff1 %0, %1     \n\t"
            : "=r"(irqno), "=r"(irq)
            : "0"(irqno), "1"(irq)
        );

        irqno = 31 - irqno;

        return g_irqrelocation[irqno];
    }

    irq = *(volatile unsigned long *)(INTC_BASE + 0x34);
    if (irq > 0)
    {
        __asm__ __volatile__ 
        (
            "ff1 %0, %1     \n\t"
            : "=r"(irqno), "=r"(irq)
            : "0"(irqno), "1"(irq)
        );

        irqno = 63 - irqno;
        return g_irqrelocation[irqno];
    }

    return 0;    
#endif	
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
    return ERR_INTC(EDRV_UNSUPPORTED);
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
    return ERR_INTC(EDRV_UNSUPPORTED);
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
    ;
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
    return ERR_INTC(EDRV_UNSUPPORTED);
}

/**
  \brief   Set interrupt handler
  \details Set the interrupt handler according to the interrupt num, the handler will be filled in g_irqvector[].
  \param [in]      IRQn  Interrupt number.
  \param [in]   handler  Interrupt handler.
 */
void csi_intc_set_vector(int32_t IRQn, void *handler)
{
#ifdef CONFIG_CHIP_DH5010M
      g_irqvector_intc[IRQn] = (void *)handler;
#else	  
      g_irqvector_intc[g_irqrelocation[IRQn]] = (void *)handler;
#endif
}

/**
  \brief   Get interrupt handler
  \details Get the address of interrupt handler function.
  \param [in]      IRQn  Interrupt number.
 */
/*void *csi_intc_get_vector(int32_t IRQn)
{
    return (void *)g_irqvector[IRQn];
}*/




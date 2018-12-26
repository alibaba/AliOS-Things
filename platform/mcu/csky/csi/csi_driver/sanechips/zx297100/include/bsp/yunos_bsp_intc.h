/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
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

#ifndef YUNOS_BSP_INTC_H
#define YUNOS_BSP_INTC_H

#include <stdint.h>

/**
 * the trigger node
 */
typedef enum int_trigger_mode_t
{
    INT_MODE_LOW_LEVEL,
    INT_MODE_HIGH_LEVEL,
    INT_MODE_RISING_EDGE,
    INT_MODE_FALLING_EDGE,
    INT_MODE_DOUBLE_EDGE,
}int_trigger_mode_t;

/**
 * Enable the IRQ specified by 'irq'
 * @param[in]   irq             irq number to operate
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_intc_enable_irq(int irq);

/**
 * Disable the IRQ specified by 'irq'
 * @param[in]   irq             irq number to operate
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_intc_disable_irq(int irq);

/**
 * Configure the IRQ subsystem so that IRQ number 'irq' is dispatched to
 * @param[in]   isr             the intterrput service
 * @param[in]   irq             irq number to operate
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_intc_attach_irq(int irq, int isr);

/**
 * Set the attributes of an IRQ.
 * @param[in]   irq             irq number to operate
 * @param[in]   priority        interrupt priority
 * @param[in]   trigger_mode    interrupt trigger_mode
 * @return      0 on success; -1 on failure
 */
int yunos_bsp_intc_set_attribute(int irq, int priority,int_trigger_mode_t trigger_mode);

/**
 * Init irq system
 * @return      void
 */
int yunos_bsp_intc_init(void);

/**
 * Get current pending irq. this interface is internally used by irq system
 * @param[in]       irq         irq number to operate
 * @return          -1 on failure, other - the pending irq number
 */
int yunos_bsp_intc_get_pending_irq(int irq);

/**
 * Acknowledge the IRQ. this interface is internally used by irq system
 * @param[in]       irq         irq number to operate
 * @return          0 on success; -1 on failure
 */
int yunos_bsp_intc_ack_irq(int irq);

/**
 * Get interrupt status
 * @param[in]       irq         irq number to operate
 * @return          0 on success; -1 on failure
 */
int yunos_bsp_intc_get_state(int irq);

#endif /* YUNOS_BSP_INTC_H */

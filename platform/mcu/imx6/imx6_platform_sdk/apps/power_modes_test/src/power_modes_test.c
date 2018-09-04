/*
 * Copyright (c) 2011-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sdk.h"
#include "platform_init.h"
#include "timer/epit.h"
#include "registers/regsepit.h"

static void tick_timer_interrupt_routine(void);

/* epit instance for tick timer */
static uint32_t epit_instance = HW_EPIT2;

/*! 
 * EPIT interrupt handler.
 */
void tick_timer_interrupt_routine(void)
{
    /* clear the compare event flag */
    epit_get_compare_event(epit_instance);
    epit_counter_disable(epit_instance);
}

void power_modes_test(void)
{
    printf("Starting the low power modes test.\n");

    /* Initialize the EPIT timer with interrupt */
    /* By using CKIL and a prescaler of 33, the counter frequency is ~1kHz */
    epit_init(epit_instance, CLKSRC_CKIL, 33,
              SET_AND_FORGET, 5000, WAIT_MODE_EN | STOP_MODE_EN);
    epit_setup_interrupt(epit_instance, tick_timer_interrupt_routine, true);

    /* allow the EPIT2 interrupt source to wake up the processor */
    ccm_set_lpm_wakeup_source(EPIT_IRQS(epit_instance), TRUE);

    /************* WAIT MODE *************************/
    printf("Entering wait state. The EPIT will wake up the core in ~5s.\n");
    /* delay is just used to allow the complete display of the above
       message before going into low power mode */ 
    hal_delay_us(10000);
    /* enable IRQ in 5000 clock cycles of CKIL = ~5s */
    epit_counter_enable(epit_instance, 5000, IRQ_MODE);

    ccm_enter_low_power(WAIT_MODE);

    printf("Exiting wait state.\n");

    /************* STOP MODE *************************/
    printf("Entering stop state. The EPIT will wake up the core in ~5s.\n");
    /* delay is just used to allow the complete display of the above
       message before going into low power mode */ 
    hal_delay_us(10000);
    /* enable IRQ in 5000 clock cycles of CKIL = ~5s */
    epit_counter_enable(epit_instance, 5000, IRQ_MODE);

    ccm_enter_low_power(STOP_MODE);

    printf("Exiting stop state.\n");

    printf("Exiting from the low power modes test.\n");
}

void main(void)
{
    platform_init();

    // Perform the power test.
    power_modes_test();
}

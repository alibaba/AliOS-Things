/*
 * Copyright (c) 2012, Freescale Semiconductor, Inc.
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
#include "irq_numbers.h"
#include "registers/regssrc.h"
#include "cpu_utility/cpu_utility.h"
#include "core/cortex_a9.h"
#include "core/gic.h"
#include "core/interrupt.h"
#include "utility/system_util.h"
#include "timer/epit.h"
#include "print_version.h"
#include "registers/regsepit.h"

////////////////////////////////////////////////////////////////////////////////
// Externs
////////////////////////////////////////////////////////////////////////////////

extern int streammain();

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

//! @brief Timer used by the benchmark.
double mysecond()
{
//         struct timeval tp;
//         struct timezone tzp;
//         int i;
// 
//         i = gettimeofday(&tp,&tzp);
//         return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );

    // use hardware timer
    uint32_t usec = 0;
    uint32_t epit_instance = HW_EPIT2, epit_freq = 0;
    static int timer_init = 0;

    if (!timer_init) {
        epit_freq = get_main_clock(IPG_CLK);
        epit_init(epit_instance, CLKSRC_IPG_CLK, epit_freq / 1000000,
                  SET_AND_FORGET, 10000, WAIT_MODE_EN | STOP_MODE_EN);
        epit_setup_interrupt(epit_instance, NULL, FALSE);
        epit_counter_enable(epit_instance, 0xFFFFFFFF, 0);   //polling mode
        timer_init = 1;
    }

    usec = 0xFFFFFFFF - epit_get_counter_value(epit_instance);

    return ((double)usec) * 1.e-6;
}

void main(void)
{
    int i = 0;

    platform_init();

    print_version();

    while (i < CPU_WORKPOINT_OUTOFRANGE) {
        uint32_t cpu_freq = cpu_workpoint_set(i);
        printf(" CPU work point set at %dMHz\n", cpu_freq);

        // Run the benchmark.
        streammain();

        i++;
    }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

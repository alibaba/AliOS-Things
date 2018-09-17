/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
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
#include "timer/timer.h"
#include "cpu_utility/cpu_utility.h"
#include "core/cortex_a9.h"
#include "utility/spinlock.h"
#include "primes.h"

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

// Set MAX_CPUS as desired; will be forced to no greater than actual number of cpus.
#define MAX_CPUS 4;
static unsigned int num_cpus = MAX_CPUS;

static volatile unsigned int all_done;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
void configure_cpu(uint32_t cpu)
{
    const unsigned int all_ways = 0xf;

    disable_strict_align_check();

    // Enable branch prediction
    arm_branch_target_cache_invalidate();
    arm_branch_prediction_enable();

    // Enable L1 caches
    arm_dcache_enable();
    arm_dcache_invalidate();
    arm_icache_enable();
    arm_icache_invalidate();

    // Invalidate SCU copy of TAG RAMs
    scu_secure_invalidate(cpu, all_ways);

    // Join SMP
    scu_join_smp();
    scu_enable_maintenance_broadcast();
}

void smp_primes(void * arg)
{
    uint64_t start_time, done_time;
    uint32_t cpu_id = cpu_get_current();
    int i;

    if (cpu_id == 0)
    {
        int cpu_count = cpu_get_cores();

        printf("Starting the smp primes example.\n");
        printf("Found %d CPU core(s)\n", cpu_count);
        if (num_cpus > cpu_count)
        {
            num_cpus = cpu_count;
        }
        printf("Using %d CPU core(s)\n", num_cpus);

        scu_enable();
        configure_cpu(cpu_id);
        spinlock_init(&prime_lock);
        all_done = (num_cpus == 1) ? 1 : 0;

        initPrimes();

        start_time = time_get_microseconds();

        // start all cpus
        for (i = 1; i < num_cpus; i++)
        {
            cpu_start_secondary(i, &smp_primes, 0);
        }

        spinlock_lock(&prime_lock, kSpinlockWaitForever);
        printf("CPU %d: Starting calculation\n", cpu_id);
        spinlock_unlock(&prime_lock);

        calculatePrimes(cpu_id);

        spinlock_lock(&prime_lock, kSpinlockWaitForever);
        printf("CPU %d: Finished\n", cpu_id);
        spinlock_unlock(&prime_lock);

        while (!all_done)
        {
            for (i=0; i <= 100000; i++) { _ARM_NOP(); }
        }
        done_time = time_get_microseconds();
        printf("Application finished in %d usecs\n", (uint32_t)(done_time - start_time));

        // put other cores back into reset
        for (i = 1; i < num_cpus; i++)
        {
            cpu_disable(i);
        }
    }
    else
    {
        configure_cpu(cpu_id);

        spinlock_lock(&prime_lock, kSpinlockWaitForever);
        printf("CPU %d: Starting calculation\n", cpu_id);
        spinlock_unlock(&prime_lock);

        calculatePrimes(cpu_id);

        spinlock_lock(&prime_lock, kSpinlockWaitForever);
        printf("CPU %d: Finished\n", cpu_id);
        spinlock_unlock(&prime_lock);

        // if last cpu, trigger test done
        if (cpu_id == (num_cpus - 1))
        {
            all_done = 1;
        }

        // wait to be disabled
        while (1)
        {
            for (i=0; i <= 100000; i++) { _ARM_NOP(); }
        }
    }
}

void main(void)
{
    platform_init();

    smp_primes(0);
}

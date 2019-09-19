/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include "k_api.h"
#include "k_arch.h"


#include "aos/kernel.h"

extern void os_crosscore_int_init();
extern void cpu_first_task_start(void);

#define MAX_CPUS 4

#if (RHINO_CONFIG_CPU_NUM > MAX_CPUS)
#error core num error RHINO_CONFIG_CPU_NUM > MAX_CPUS
#endif

static unsigned int num_cpus = RHINO_CONFIG_CPU_NUM;


void configure_cpu(uint32_t cpu)
{
    const unsigned int all_ways = 0xf;

    // Enable branch prediction
    arm_branch_target_cache_invalidate();
    arm_branch_prediction_enable();

    // Invalidate SCU copy of TAG RAMs
    scu_secure_invalidate(cpu, all_ways);

    // Join SMP
    scu_join_smp();
    scu_enable_maintenance_broadcast();

    OS_DSB();
    OS_ISB();

}

void smp_cpu_init(void)
{
    uint32_t cpu_count;
    uint32_t cpu_id;

    cpu_id = cpu_get_current();

    if (cpu_id == 0) {
        cpu_count = cpu_get_cores();
        num_cpus = (cpu_count >= num_cpus ? num_cpus : cpu_count);
    } else {
        platform_init();

        k_vectable_set();
#if (RHINO_CONFIG_CPU_NUM > 1)
        extern kspinlock_t g_smp_printlock;
        krhino_spin_lock(&g_smp_printlock);
#endif

        printf("Starting scheduler on CPU:%d.\r\n",cpu_id);
#if (RHINO_CONFIG_CPU_NUM > 1)
        krhino_spin_unlock(&g_smp_printlock);
#endif
        cpu_first_task_start();
    }

}


void os_load_slavecpu(void)
{
    int i;

    os_crosscore_int_init();

    // start all cpus
    for (i = 1; i < num_cpus; i++)
    {
        cpu_start_secondary(i, &smp_cpu_init, 0);
    }

    k_wait_allcores();
}


/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
 * This file provides cpu_pwr_topology_show()/cpu_pwr_state_show() to show
 * information for CPU power management.
 */

#include <stdio.h>
#include <string.h>

#include "cpu_pwr_hal_lib.h"
#include "pwrmgmt_debug.h"

#if (PWRMGMT_CONFIG_SHOW > 0)
extern cpu_pwr_t *p_cpu_pwr_root_node;
extern cpu_pwr_t *p_cpu_node_array[];

static void cpu_pwr_c_support_print(cpu_pwr_t *p_cpu_node, char *prefix)
{
    uint32_t state_set;
    uint32_t latency = 0;
    int      state;
    int      ix;
    uint32_t tmp;

    (void)printf("%s C-States:\n", prefix);

    state_set = p_cpu_node->support_bitset_c;

    while (state_set != 0) {
        /* find first bit set  */
        if (state_set == 0) {
            state = 0;
        } else {
            tmp   = state_set & (-state_set);
            state = 31 - krhino_find_first_bit(&tmp);
        }

        for (ix = 0; ix < p_cpu_node->cstate_nums; ix++) {
            if (state == p_cpu_node->p_pair_latency[ix].state) {
                latency = p_cpu_node->p_pair_latency[ix].value;
                break;
            }
        }

        (void)printf("%s   C%d: %4d us latency\n", prefix, state, latency);

        state_set &= ~(1 << state);
    }
}

/**
 * cpu_pwr_info_show() will show information for all nodes of the cpu power
 * management. For each node the C-state and P-state capability
 * is provided, as applicable, depending on hardware capabilities and
 * configuration.
 *
 * For P-state capability the CPU core frequency and voltage setting is
 * displayed. For C-state capabilities the latency information (wake-up time)
 * is displayed.
 */
void cpu_pwr_info_show(void)
{
    cpu_pwr_t *p_cpu_node;
    int        cpu_idx;

    (void)printf("\n    CPU NODE     CAPABILITIES\n"
                 "---------------  ---------------\n");

    for (cpu_idx = 0; cpu_idx < RHINO_CONFIG_CPU_NUM; cpu_idx++) {
        p_cpu_node = p_cpu_node_array[cpu_idx];
        (void)printf("%s%u (CPU%d)\n", p_cpu_node->name, p_cpu_node->unit, cpu_idx);
        cpu_pwr_c_support_print(p_cpu_node, "       ");
    }
}

/**
 * cpu_pwr_state_show() display the CPU power/performance states.
 */
void cpu_pwr_state_show(void)
{
    uint32_t     cpu_idx;
    cpu_cstate_t cState;

    printf("\n    CPU     C-STATE\n"
           "----------  ----------\n");

    for (cpu_idx = 0; cpu_idx < RHINO_CONFIG_CPU_NUM; cpu_idx++) {
        cpu_pwr_c_state_get(cpu_idx, &cState);
        printf("CPU%-5u         C%-2u\n", cpu_idx, cState);
    }

    printf("\n");
}

#endif /* RHINO_CONFIG_CPU_PWR_SHOW */

#if (PWRMGMT_CONFIG_DEBUG > 0)
void pwrmgmt_log(pwrmgmt_log_level_t debug_level, const char *fmt_str, ...)
{
    va_list param;

    printf("pwrmgmt:");

    if (debug_level >= PWRMGMT_LOG_LEVEL) {
        va_start(param,fmt_str);
        vprintf(fmt_str,param);
        va_end(param);
    }
}
#endif /* RHINO_CONFIG_CPU_PWR_DEBUG > 0 */

/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
modification history
--------------------
14jan2018, init vesion
*/

#ifndef K_CPU_PWR_HAL_H
#define K_CPU_PWR_HAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <k_err.h>
#include "pwr_state.h"

#define CPU_PWR_NODE_NAME_LEN 64

#define CPU_STATE_BIT(n) (1 << (n))
#define CPU_FREQ_UNKNOW (-1)
#define CPU_VOLT_UNKNOW (-1)
#define CPU_LATENCY_UNKNOW (-1)

    typedef struct
    {
        uint32_t state;
        uint32_t value;
    } state_val_pair_t;

    struct cpu_pwr;

    typedef pwr_status_t (*cpu_pstate_set_t)(struct cpu_pwr *p_cpu_node,
                                             uint32_t        cpu_state);
    typedef pwr_status_t (*cpu_cstate_set_t)(uint32_t cpu_state, int master);
    typedef pwr_status_t (*cpu_cstate_get_t)(struct cpu_pwr *p_cpu_node,
                                             uint32_t *      pCpuState);

    typedef struct cpu_pwr
    {
        uint32_t          support_bitset_c;
        int               cstate_nums;
        state_val_pair_t *p_pair_latency;
        unsigned int      cstate_updating;
        cpu_cstate_t      current_c_state;
        cpu_cstate_t      desire_c_state;
        cpu_cstate_set_t  cpu_cstate_set_func;
        char *            name;
        uint32_t          unit;
    } cpu_pwr_t;

    extern void         cpu_pwr_hal_lib_init(void);
    extern pwr_status_t cpu_pwr_node_init_static(char *, uint32_t, cpu_pwr_t *);
    extern pwr_status_t cpu_pwr_node_init_dyn(char *, uint32_t, cpu_pwr_t **);
    extern cpu_pwr_t *  cpu_pwr_node_find_by_name(char *pName, uint32_t index);
    extern pwr_status_t cpu_pwr_node_record(cpu_pwr_t *p_cpu_node,
                                            uint32_t   cpuIndex);
    extern pwr_status_t cpu_pwr_c_state_capability_set(uint32_t cpuIndex,
                                                       uint32_t supportBitsetC);
    extern pwr_status_t cpu_pwr_c_state_latency_save(uint32_t     cpuIndex,
                                                     cpu_cstate_t cpu_state,
                                                     uint32_t     latency);
    extern uint32_t     cpu_pwr_c_state_latency_get(uint32_t     cpuIndex,
                                                    cpu_cstate_t CState);
    extern pwr_status_t cpu_pwr_c_method_set(
      uint32_t cpu_idx, cpu_cstate_set_t cpu_cstate_set_func);


#ifdef __cplusplus
}
#endif

#endif /* K_CPU_PWR_HAL_H */

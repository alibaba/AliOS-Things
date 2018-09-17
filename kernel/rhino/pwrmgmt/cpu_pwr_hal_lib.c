/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
modification history
--------------------
14jan2018, init vesion
*/

/*
This file provides Hardware Abstraction Layer of CPU power management support.
*/

#include <stdlib.h>
#include "cpu_pwr_lib.h"
#include "cpu_pwr_hal_lib.h"
#include "cpu_pwr_api.h"
#include "pwr_debug.h"

/* debug switch of this file */
#undef CPU_PWR_HAL_DBG

/* forward declarations */

/*
board_cpu_pwr_topo_create() must be provided by board or platform,
different board/platform may has different cpu topology.
*/
extern pwr_status_t board_cpu_pwr_init(void);

/* the index of this p_cpu_node_array implies the cpu logic id */
cpu_pwr_t *     p_cpu_node_array[CPUS_NUM_MAX];
static kmutex_t cpu_pwr_hal_mux;

static int num_of_bit_one_get(uint32_t n);

#define CPU_TREE_MUX_LOCK() \
    (void)krhino_mutex_lock(&cpu_pwr_hal_mux, RHINO_WAIT_FOREVER)
#define CPU_TREE_MUX_UNLOCK() (void)krhino_mutex_unlock(&cpu_pwr_hal_mux)

static kspinlock_t cpu_pwr_lock;

cpu_pwr_t *cpu_pwr_node_find_by_name(char *p_name, uint32_t index)
{
    int i = 0;

    for (i = 0; i < CPUS_NUM_MAX; i++) {
        if (p_cpu_node_array[i]->name == NULL) {
            continue;
        }

        if ((!strncmp(p_cpu_node_array[i]->name, p_name,
                      CPU_PWR_NODE_NAME_LEN)) &&
            (p_cpu_node_array[i]->unit == index)) {
            /* find it, and return */
            return p_cpu_node_array[i];
        }
    }

    return NULL;
}

pwr_status_t cpu_pwr_node_init_(char *name, uint32_t unit,
                                cpu_pwr_t *p_cpu_node)
{
    if (name == NULL || p_cpu_node == NULL) {
        return PWR_ERR;
    }

    memset(p_cpu_node, 0, sizeof(cpu_pwr_t));

    p_cpu_node->name = name;
    p_cpu_node->unit = unit;

    p_cpu_node->current_c_state  = CPU_CSTATE_NONE;
    p_cpu_node->support_bitset_c = 0;

    PWR_DBG(DBG_INFO, "init %s%d done\n", name, unit);

    return PWR_OK;
}

/**
 * cpu_pwr_node_init_static will create and initialize a cpu power management
 * node according the given level number, node name and node unit.
 *
 * This routine is normally used to create cpu power management topology tree by
 * boards/platform.
 *
 * For example, cpu_pwr_node_init_static(2, "core", 0, p_cpu_node) will init a
 * cpu power management node named "core0" in level2 of the cpu power management
 * topology tree, and return the struction point address, but it do not belong
 * to any other node and do not has any child at this time,
 * cpu_pwr_child_add(p_parent, p_child) could be used to create relationship
 * with other node in the topology tree.
 *
 * the space of p_cpu_node is provided by caller.
 */
pwr_status_t cpu_pwr_node_init_static(char *name, uint32_t unit,
                                      cpu_pwr_t *p_cpu_node)
{
    if (name == NULL || p_cpu_node == NULL) {
        return PWR_ERR;
    }

    return cpu_pwr_node_init_(name, unit, p_cpu_node);
}

/**
 * cpu_pwr_node_init_dyn() is same as cpu_pwr_node_init_static(),
 * the difference is the space of pp_cpu_node is mallocced in
 * running time.
 */
pwr_status_t cpu_pwr_node_init_dyn(char *name, uint32_t unit,
                                   cpu_pwr_t **pp_cpu_node)
{
    cpu_pwr_t *p_cpu_node;

    if (name == NULL || pp_cpu_node == NULL) {
        return PWR_ERR;
    }

    p_cpu_node = (cpu_pwr_t *)malloc(sizeof(cpu_pwr_t));
    if (p_cpu_node == NULL) {
        PWR_DBG(DBG_ERR, "malloc failed\n");
        return PWR_ERR;
    }

    *pp_cpu_node = p_cpu_node;

    return cpu_pwr_node_init_(name, unit, *pp_cpu_node);
}

/**
 *
 * cpu_pwr_node_record - record the cpu power management node.
 * The node information is setup by this way:
 * a) a array named p_cpu_node_array[32] with 32 point(type is cpu_pwr_t *) is
 *    created in cpu_pwr_hal_lib_init();
 * b) all the 32 point is set to NULL in cpu_pwr_hal_lib_init();
 * c) when BSP/platform init the cpu pwr, it knows the cpu index information of
 * that cpu, BSP/platform should call cpu_pwr_node_record(p_cpu_node, cpu_idx)
 * to save the cpu_pwr_t * information of cpu indicated by cpu_idx into
 *    p_cpu_node_array[cpu_idx];
 *
 * When there is requirement to find the cpu_pwr_t * according the cpu_idx,
 * just return the p_cpu_node_array[cpu_idx] is fine and save much time.
 *
 * @return  PWR_OK or PWR_ERR when failed.
 */
pwr_status_t cpu_pwr_node_record(cpu_pwr_t *p_cpu_node, uint32_t cpu_idx)
{
    if (p_cpu_node == NULL) {
        PWR_DBG(DBG_ERR, "p_cpu_node == NULL\n");

        return PWR_ERR;
    }

    if (cpu_idx >= CPUS_NUM_MAX) {
        PWR_DBG(DBG_ERR, "cpu_idx(%d) error, it should be 0 ~ %d\n", cpu_idx,
                CPUS_NUM_MAX - 1);

        return PWR_ERR;
    }

    if (p_cpu_node_array[cpu_idx] != NULL) {
        PWR_DBG(DBG_ERR, "p_cpu_node_array[%d] != NULL\n", cpu_idx);

        return PWR_ERR;
    }

    /* set the flag to a default value */
    p_cpu_node->cstate_updating = FALSE;

    p_cpu_node_array[cpu_idx] = p_cpu_node;

    PWR_DBG(DBG_INFO, "%s%d connect to logic id %d done\n", p_cpu_node->name,
            p_cpu_node->unit, cpu_idx);

    return PWR_OK;
}


/**
 * cpu_pwr_c_method_set() set the method of controlling C state,
 *normally it will be called by platform in board_cpu_pwr_init().
 *
 * @renturn  PWR_OK or PWR_ERR when failed.
 */
pwr_status_t cpu_pwr_c_method_set(uint32_t         cpu_idx,
                                  cpu_cstate_set_t cpu_cstate_set_func)
{
    cpu_pwr_t *  p_cpu_node;
    pwr_status_t ret = PWR_OK;

    if (cpu_idx >= CPUS_NUM_MAX) {
        PWR_DBG(DBG_ERR, "cpu_idx(%d) error, it should be 0 ~ %d\n", cpu_idx,
                CPUS_NUM_MAX - 1);

        return PWR_ERR;
    }

    /* fetch the p_cpu_node directly from p_cpu_node_array according cpu_idx */
    p_cpu_node = p_cpu_node_array[cpu_idx];

    if (p_cpu_node == NULL) {
        PWR_DBG(DBG_ERR,
                "did not find p_cpu_node in p_cpu_node_array "
                "with cpu_idx[%d]\n",
                cpu_idx);

        return PWR_ERR;
    }

    CPU_TREE_MUX_LOCK();
    /* bind P state set/get method into this node */
    p_cpu_node->cpu_cstate_set_func = cpu_cstate_set_func;

    /* set C state to C0 by default, is here the right place ?*/
    p_cpu_node->current_c_state = CPU_CSTATE_C0;

    PWR_DBG(DBG_INFO, "set CPU(%s%d in level %d) C-state-set to 0x%08x\n",
            p_cpu_node->name, p_cpu_node->unit, p_cpu_node->level,
            cpu_cstate_set_func);
    PWR_DBG(DBG_INFO, "set CPU(%s%d in level %d) current_c_state to C%d\n",
            p_cpu_node->name, p_cpu_node->unit, p_cpu_node->level,
            p_cpu_node->current_c_state);
    CPU_TREE_MUX_UNLOCK();

    return ret;
}

static pwr_status_t cpu_pwr_c_state_set_(cpu_pwr_t *  p_cpu_node,
                                         cpu_cstate_t cpu_c_state, int master,
                                         int all_cores_need_sync)
{
    pwr_status_t ret           = PWR_OK;
    uint32_t     state_c_match = 0;

    if (p_cpu_node == NULL) {
        PWR_DBG(DBG_ERR, "PWR_ERR: p_cpu_node == NULL\n");
        return PWR_ERR;
    }

    state_c_match = (CPU_STATE_BIT(cpu_c_state) & p_cpu_node->support_bitset_c);

#ifdef CPU_PWR_HAL_DBG
    if ((p_cpu_node->cpu_cstate_set_func != NULL) && (state_c_match == 0)) {
        PWR_DBG(DBG_INFO,
                "%s%d(with supportCBitset[0x%08x]) "
                "do not support C state P%d\n",
                p_cpu_node->name, p_cpu_node->unit,
                p_cpu_node->support_bitset_c, cpu_c_state, );
    }
#endif /* CPU_PWR_HAL_DBG */

    /*
     * If current p_cpu_node support cpu_cstate_set_func() and match the C state
     * which it support, use it directly and update C state of all the children
     * at same time.
     */
    if ((p_cpu_node->cpu_cstate_set_func != NULL) && (state_c_match != 0)) {

        /*
         * The new C state should be recorded before calling the real C
         * control method, the reason is when C0->C1~Cn, the CPU will stop
         * to execute the instruction.
         */

        /* update the current C state */
        p_cpu_node->current_c_state = cpu_c_state;

        /* hold input C state into desire_c_state */
        p_cpu_node->desire_c_state = cpu_c_state;

        /*
         * cpu_c_state == CPU_CSTATE_C1, C1 state must a independent
         * state that each core support, each core could enter/exit
         * C1 state independently, so each core could update it's C1
         * state independently;
         *
         * C2/C3 or higher is dependent on platfrom/arch, some platform
         * support C2/C3 per-core, but some platform do not support.
         * all_cores_need_sync is assert here to check this information,
         * 1) all_cores_need_sync == TRUE, means that this Cx state is not
         * per-core independent, it need all cores enter/exit this Cx
         * state at same time.
         * 2) all_cores_need_sync == FALSE, means that this Cx state is
         * pre-core independent, it could enter/exit this Cx state
         * independently.
         *
         * When all_cores_need_sync is TRUE, we leave the responsibility
         * to master(means the last core enter idle mode) to update
         * all cores C state of Cx(C2/C3 or higher).
         */

        /* call the real C idle method */
        ret = p_cpu_node->cpu_cstate_set_func(cpu_c_state, master);

        if (ret != PWR_OK) {
            PWR_DBG(DBG_ERR, "%s%d -> cpu_cstate_set_func(P%d) failed\n",
                    p_cpu_node->name, p_cpu_node->unit, cpu_c_state);
            return PWR_ERR;
        }

        return PWR_OK;
    }

    /*
     * if we
     * 1) - do not support cpu_cstate_set_func()
     *    or
     * 2) - the request C state is not support
     */

    PWR_DBG(DBG_INFO,
            "p_cpu_node(%s%d) do not support cpu_cstate_set_func "
            "or the request C state is not support\n",
            p_cpu_node->name, p_cpu_node->unit);

    return PWR_OK;
}

/**
 * cpu_pwr_c_state_get - get C state of CPU indicated by cpu_idx
 *
 * This routine will get C state of CPU indicated by cpu_idx, the C state will
 * return in pCpuCStatus.
 *
 * @return  PWR_OK or PWR_ERR when failed.
 */
static pwr_status_t _cpu_pwr_c_state_get(uint32_t      cpu_idx,
                                         cpu_cstate_t *p_cpu_c_state)
{
    cpu_pwr_t *p_cpu_node;

    if (cpu_idx >= CPUS_NUM_MAX) {
        PWR_DBG(DBG_ERR, "cpu_idx(%d) error, it should be 0 ~ %d\n", cpu_idx,
                CPUS_NUM_MAX - 1);

        return PWR_ERR;
    }

    /* fetch the p_cpu_node directly from p_cpu_node_array according cpu_idx */
    p_cpu_node = p_cpu_node_array[cpu_idx];

    if (p_cpu_node == NULL) {
        PWR_DBG(DBG_ERR,
                "did not find p_cpu_node in p_cpu_node_array "
                "with cpu_idx[%d]\n",
                cpu_idx);

        return PWR_ERR;
    }

    *p_cpu_c_state = p_cpu_node->current_c_state;

    return PWR_OK;
}

pwr_status_t cpu_pwr_c_state_get(uint32_t cpu_idx, cpu_cstate_t *p_cpu_c_state)
{
    pwr_status_t ret;

    krhino_spin_lock_irq_save(&cpu_pwr_lock);
    ret = _cpu_pwr_c_state_get(cpu_idx, p_cpu_c_state);
    krhino_spin_unlock_irq_restore(&cpu_pwr_lock);

    return ret;
}

/**
 *
 * cpu_pwr_c_state_set - put CPU into given C state
 *
 * This routine will put CPU into given C state, under SMP environment,
 * cpu_idx is fetched locally from cpu_index_get which implies that
 * this function is always applied to the current core.
 * This function has to be called on the CPU for which the C state is being set.
 * the caller has to ensure that the code will be executed on the same core
 * (such as called by task with CPU affinity or from ISR context).
 *
 * @return  PWR_OK or PWR_ERR when failed.
 */
static pwr_status_t _cpu_pwr_c_state_set(cpu_cstate_t target_c_state)
{
    uint32_t     cpu_idx;
    cpu_pwr_t *  p_cpu_node;
    pwr_status_t ret = PWR_OK;
    int          master;

    cpu_idx = 0;

    /* fetch the p_cpu_node directly from p_cpu_node_array according cpu_idx */
    p_cpu_node = p_cpu_node_array[cpu_idx];

    if (p_cpu_node == NULL) {
        PWR_DBG(
          DBG_ERR,
          "did not find p_cpu_node in p_cpu_node_array with cpu_idx[%d]\n",
          cpu_idx);

        return PWR_ERR;
    }

    if (p_cpu_node->current_c_state == target_c_state) {
        return PWR_OK;
    }

    /*
     * at any time for one CPU, only one C state updating process is allowed,
     * when a C state updating is going on, it set cstate_updating to 1,
     * if the other thread(a task or interrupt) try to start other C state
     * updating, it will encount cstate_updating == 1, and should
     * return immediately.
     *
     * a exception should be consider, idle task()->cpu_pwr_down()->
     * cpu_pwr_c_state_set(CPU_CSTATE_C1(C2,C3,...)) will make
     * CPU stopping execute instruction and never return, which will
     * leave cstate_updating as TRUE and never be cleaned to FALSE.
     *
     * when cpu_pwr_c_state_set(CPU_CSTATE_C0) is called again
     * to make C state changed from C1(C2,C3,...) to C0, we check
     * the value of cstate_updating, if it is TURE, which implies that
     * cstate_updating is not cleaned in previous state change(from C0
     * to C1,C2,C3,...), cstate_updating will be forced to FALSE.
     */

    if (target_c_state == CPU_CSTATE_C0 &&
        p_cpu_node->current_c_state != CPU_CSTATE_C0 &&
        p_cpu_node->cstate_updating == TRUE) {

        p_cpu_node->cstate_updating = FALSE;
    }

    if (p_cpu_node->cstate_updating == FALSE) {
        p_cpu_node->cstate_updating = TRUE;

        if (target_c_state == CPU_CSTATE_C0) {

            /*
             * cpu_c_state == CPU_CSTATE_C0, which means current call
             * stack is:
             * krhino_intrpt_enter() -> cpu_pwr_up() -> cpu_pwr_c_state_set(C0)
             * what we need to do here is set current_c_state/desire_c_state
             * to C0.
             * For the first core which wakeup there maybe need some
             * prepare work need to do, so the master(the first one
             * open door) should be distinguished here.
             */

            /*
             * for UP, we are always the master, because there is
             * only one core in system.
             */
            master = TRUE;

            /*
             * master == TRUE, means we are the first one wakeup,
             * a liite more work need to do than other cores here
             * is update all the parent to C0 state. And send IPI
             * to other cores to update it's self Cx state when
             * wake up from C2 or high Cx state(C3/C4...).
             */

            if (master == TRUE) {
                /* call the real C control method */

                if (p_cpu_node->cpu_cstate_set_func != NULL) {

                    /*
                     * Note, the master value is TRUE here,
                     * cpu_cstate_set_func() could take proper action
                     * based on this value, because we are the first
                     * core wake up.
                     */
                    ret =
                      p_cpu_node->cpu_cstate_set_func(target_c_state, master);
                    if (ret != PWR_OK) {
                        PWR_DBG(DBG_ERR,
                                "%s%d -> cpu_cstate_set_func(P%d) "
                                "failed\n",
                                p_cpu_node->name, p_cpu_node->unit,
                                target_c_state);
                        ret = PWR_ERR;
                    }
                }

                /* update C state to C0 */
                p_cpu_node->current_c_state = CPU_CSTATE_C0;
                p_cpu_node->desire_c_state  = CPU_CSTATE_C0;

            } else {
                /*
                 * If come here, we must be subsequent core wake up
                 * after first core, what we need to do is updating
                 * self state to C0 and call function if need,
                 * normmally cpu_cstate_set_func() will do nothing here.
                 */

                if (p_cpu_node->cpu_cstate_set_func != NULL) {
                    ret =
                      p_cpu_node->cpu_cstate_set_func(target_c_state, master);
                    if (ret != PWR_OK) {
                        PWR_DBG(DBG_ERR,
                                "%s%d -> cpu_cstate_set_func(P%d) "
                                "failed\n",
                                p_cpu_node->name, p_cpu_node->unit,
                                target_c_state);

                        ret = PWR_ERR;
                    }
                }

                p_cpu_node->current_c_state = CPU_CSTATE_C0;
                p_cpu_node->desire_c_state  = CPU_CSTATE_C0;
            }
        } else {

            /*
             * if come here, cpu_c_state must is not CPU_CSTATE_C0,
             * it must be C1 or high C2/C3..., it must be going
             * to idle Cx mode.
             */

            /*
             * for UP, we are always the master, because there is
             * only one core in system.
             */
            master = TRUE;

            ret =
              cpu_pwr_c_state_set_(p_cpu_node, target_c_state, master, FALSE);
        }

        /*
         * after C state change completed, it should set cstate_updating
         * to 0 to allow C state updating again.
         *
         * a exception should be consider, idletask()->cpu_pwr_down()->
         * cpu_pwr_c_state_set(CPU_CSTATE_C1(C2,C3,...)) will make
         * CPU stopping execute instruction and never return, which will
         * lead this line never be executed and leave cstate_updating as TRUE.
         *
         * when cpu_pwr_c_state_set(CPU_CSTATE_C0) is called again
         * to make C state changed from C1(C2,C3,...) to C0, we check
         * the value of cstate_updating, if it is TURE, which implies that
         * cstate_updating is not cleaned in previous state change(from C0
         * to C1,C2,C3,...), cstate_updating will be forced to FALSE.
         */
        p_cpu_node->cstate_updating = FALSE;
    }

    return ret;
}

pwr_status_t cpu_pwr_c_state_set(cpu_cstate_t target_c_state)
{
    pwr_status_t ret;

    krhino_spin_lock_irq_save(&cpu_pwr_lock);
    ret = _cpu_pwr_c_state_set(target_c_state);
    krhino_spin_unlock_irq_restore(&cpu_pwr_lock);

    return ret;
}

static pwr_status_t cpu_pwr_c_state_capability_set_(cpu_pwr_t *p_cpu_node,
                                                    int        cpu_idx,
                                                    int32_t    support_bitset_c)
{
    if (p_cpu_node == NULL) {
        PWR_DBG(DBG_ERR, "p_cpu_node == NULL\n");
        return PWR_ERR;
    }

    /* if this level support setting the P state, it implies that it
       maintain the support_bitset_p value. */
    if (p_cpu_node->cpu_cstate_set_func != NULL) {
        p_cpu_node->support_bitset_c = support_bitset_c;

        PWR_DBG(DBG_INFO,
                "level %d, p_cpu_node(%s%d) support_bitset_c = 0x%016llx\n",
                p_cpu_node->level, p_cpu_node->name, p_cpu_node->unit,
                p_cpu_node->support_bitset_c);

        return PWR_OK;
    }

    PWR_DBG(DBG_ERR,
            "PWR_ERR: p_cpu_node(%s%d) "
            "support_bitset_c = 0x%016llx\n",
            p_cpu_node->name, p_cpu_node->unit, p_cpu_node->support_bitset_c);

    return PWR_ERR;
}

static pwr_status_t cpu_pwr_c_state_capability_get_(
  cpu_pwr_t *p_cpu_node, int cpu_idx, uint32_t *p_support_bitset_c)
{
    if (p_cpu_node == NULL) {
        PWR_DBG(DBG_ERR, "p_cpu_node == NULL\n");
        return PWR_ERR;
    }

    /*
     * if this level support setting the C state, it implies that it
     * maintain the support_bitset_c value.
     */
    if (p_cpu_node->cpu_cstate_set_func != NULL) {
        *p_support_bitset_c |= p_cpu_node->support_bitset_c;

        PWR_DBG(DBG_INFO,
                "level %d, p_cpu_node(%s%d) support_bitset_c = 0x%016llx\n",
                p_cpu_node->level, p_cpu_node->name, p_cpu_node->unit,
                p_cpu_node->support_bitset_c);

        /* do not return, continue check if any support C state from parent */
    }

    return PWR_OK;
}

/**
 * cpu_pwr_c_state_capability_set() will set the C state supported of given CPU.
 * There is two way to set the C state of CPU which is supplied by this library,
 * one is according cpu index, other is according cpu level.
 *
 * If all the cpu node in same level support same C state, user can use
 * cpu_pwr_c_state_capability_set_by_level(level, support_bitset_c) to set all
 * nodes in same level at one time.
 * If different CPU support different C state, user can use this routine to set
 * support C state for each cpu individually.
 *
 * @return  PWR_OK or PWR_ERR when failed.
 */
pwr_status_t cpu_pwr_c_state_capability_set(uint32_t cpu_idx,
                                            uint32_t support_bitset_c)
{
    cpu_pwr_t *  p_cpu_node;
    pwr_status_t ret = PWR_OK;

    if (cpu_idx >= CPUS_NUM_MAX) {
        PWR_DBG(DBG_ERR, "cpu_idx(%d) error, it should be 0 ~ %d\n", cpu_idx,
                CPUS_NUM_MAX - 1);

        return PWR_ERR;
    }

    /* fetch the p_cpu_node directly from p_cpu_node_array according cpu_idx */

    p_cpu_node = p_cpu_node_array[cpu_idx];

    if (p_cpu_node == NULL) {
        PWR_DBG(DBG_ERR,
                "did not find p_cpu_node in p_cpu_node_array "
                "with cpu_idx[%d]\n",
                cpu_idx);

        return PWR_ERR;
    }

    CPU_TREE_MUX_LOCK();

    ret =
      cpu_pwr_c_state_capability_set_(p_cpu_node, cpu_idx, support_bitset_c);

    CPU_TREE_MUX_UNLOCK();

    return ret;
}

/**
 * cpu_pwr_c_state_capability_get() will return the all C state supported of
 * given CPU.
 *
 * @return  PWR_OK or PWR_ERR when failed.
 */
pwr_status_t cpu_pwr_c_state_capability_get(uint32_t  cpu_idx,
                                            uint32_t *p_support_bitset_c)
{
    cpu_pwr_t *  p_cpu_node;
    pwr_status_t ret = PWR_OK;

    if (cpu_idx >= CPUS_NUM_MAX) {
        PWR_DBG(DBG_ERR, "cpu_idx(%d) error, it should be 0 ~ %d\n", cpu_idx,
                CPUS_NUM_MAX - 1);

        return PWR_ERR;
    }

    /* fetch the p_cpu_node directly from p_cpu_node_array according cpu_idx */
    p_cpu_node = p_cpu_node_array[cpu_idx];

    if (p_cpu_node == NULL) {
        PWR_DBG(DBG_ERR,
                "did not find p_cpu_node in p_cpu_node_array "
                "with cpu_idx[%d]\n",
                cpu_idx);

        return PWR_ERR;
    }

    /* bzero return value first */
    *p_support_bitset_c = 0;

    CPU_TREE_MUX_LOCK();

    ret =
      cpu_pwr_c_state_capability_get_(p_cpu_node, cpu_idx, p_support_bitset_c);

    CPU_TREE_MUX_UNLOCK();

    PWR_DBG(DBG_INFO, "%s%d C support bit set 0x%08x\n", p_cpu_node->name,
            p_cpu_node->unit, *p_support_bitset_c);

    return ret;
}


static pwr_status_t cpu_pwr_c_state_latency_save_(cpu_pwr_t *  p_cpu_node,
                                                  cpu_cstate_t cpu_c_state,
                                                  uint32_t     latency)
{
    int    i;
    size_t spaceSize = 0;

    if (p_cpu_node == NULL) {
        PWR_DBG(DBG_ERR, "PWR_ERR: p_cpu_node == NULL\n");
        return PWR_ERR;
    }

    /*
     * Note that comparing enums like this is technically
     * not valid. There is some ambiguity because while enums are
     * intended to be represented by integers, it's up to the
     * compiler to decide whether they should be signed or unsigned.
     * In this case, we happen to know that the cpu_cstate_t is defined
     * to only use unsigned integer values, so we can temporarily
     * get away with using a cast here to silence the compiler
     * warning, but this test should probably be removed.
     */

    for (i = 0; i <= CPU_CSTATE_MAX; i++) {
        if (cpu_c_state == CPU_CSTATE_C0 + i) {
            break;
        }
    }

    if (i > CPU_CSTATE_MAX) {
        PWR_DBG(DBG_ERR, "PWR_ERR: cpu_c_state(%d) > CPU_CSTATE_MAX(%d)\n",
                cpu_c_state, CPU_PSTATE_MAX);
        return PWR_ERR;
    }

    if (p_cpu_node->cpu_cstate_set_func != NULL) {
        /* how many C state is supportted */
        if (p_cpu_node->cstate_nums == 0) {
            p_cpu_node->cstate_nums =
              num_of_bit_one_get(p_cpu_node->support_bitset_c);

            if (p_cpu_node->cstate_nums == 0) {
                PWR_DBG(DBG_ERR,
                        "%s%d->support_bitset_c(%d), cstate_nums() = 0\n",
                        p_cpu_node->name, p_cpu_node->unit,
                        p_cpu_node->support_bitset_c);

                return PWR_ERR;
            }
        }

        /* malloc the space to save frequency info */
        if (p_cpu_node->p_pair_latency == NULL) {
            spaceSize = sizeof(state_val_pair_t) * p_cpu_node->cstate_nums;
            p_cpu_node->p_pair_latency = (state_val_pair_t *)malloc(spaceSize);

            if (p_cpu_node->p_pair_latency == NULL) {
                PWR_DBG(DBG_ERR, "zalloc failed\n");

                return PWR_ERR;
            }

            /* clean space */
            memset(p_cpu_node->p_pair_latency, 0, spaceSize);

            /* init value to init value CPU_LATENCY_UNKNOW */
            for (i = 0; i < p_cpu_node->cstate_nums; i++) {
                p_cpu_node->p_pair_latency[i].value =
                  (uint32_t)CPU_LATENCY_UNKNOW;
            }
        }

        /* if we support this C state, save the corresponding latency info */
        if (CPU_STATE_BIT(cpu_c_state) & p_cpu_node->support_bitset_c) {

            /* check if info has been saved before */
            for (i = 0; i < p_cpu_node->cstate_nums; i++) {
                if ((p_cpu_node->p_pair_latency[i].state == cpu_c_state) &&
                    (p_cpu_node->p_pair_latency[i].value == latency)) {
                    return PWR_OK;
                }
            }

            for (i = 0; i < p_cpu_node->cstate_nums; i++) {
                /* search a free space on p_pair_latency to save info */
                if (p_cpu_node->p_pair_latency[i].value ==
                    (uint32_t)CPU_LATENCY_UNKNOW) {
                    p_cpu_node->p_pair_latency[i].state = cpu_c_state;
                    p_cpu_node->p_pair_latency[i].value = latency;

                    return PWR_OK;
                }
            }

            /* if search free space failed */
            if (i == p_cpu_node->cstate_nums) {
                PWR_DBG(DBG_ERR, "p_cpu_node(%s%d) search free space failed\n",
                        p_cpu_node->name, p_cpu_node->unit);
                return PWR_ERR;
            }
        }
    }

    /*
     * if come here, there is two possible case:
     * 1) this node do not support cpu_cstate_set_func
     *    or
     * 2) this node support cpu_cstate_set_func, but the given C state
     * cpu_c_state is not supported
     */
    PWR_DBG(DBG_INFO,
            "p_cpu_node(%s%d) do not support cpu_cstate_set_func "
            "or the given C state cpu_c_state is not supported\n",
            p_cpu_node->name, p_cpu_node->unit);

    return PWR_OK;
}

/**
 * cpu_pwr_c_state_latency_save() save the cpu latency information of C state,
 * it will let cpu topology system to know how long when the CPU wake up from
 * C state, normally it will be called by platform in
 * board_cpu_pwr_topo_create().
 *
 * @return  PWR_OK or PWR_ERR when failed.
 */
pwr_status_t cpu_pwr_c_state_latency_save(uint32_t     cpu_idx,
                                          cpu_cstate_t cpu_c_state,
                                          uint32_t     latency)
{
    cpu_pwr_t *  p_cpu_node;
    pwr_status_t ret = PWR_OK;

    if (cpu_idx >= CPUS_NUM_MAX) {
        PWR_DBG(DBG_ERR, "cpu_idx(%d) error, it should be 0 ~ %d\n", cpu_idx,
                CPUS_NUM_MAX - 1);
        return PWR_ERR;
    }

    /* fetch the p_cpu_node directly from p_cpu_node_array according cpu_idx */
    p_cpu_node = p_cpu_node_array[cpu_idx];

    if (p_cpu_node == NULL) {
        PWR_DBG(DBG_ERR,
                "did not find p_cpu_node in p_cpu_node_array "
                "with cpu_idx[%d]\n",
                cpu_idx);

        return PWR_ERR;
    }

    CPU_TREE_MUX_LOCK();

    ret = cpu_pwr_c_state_latency_save_(p_cpu_node, cpu_c_state, latency);

    CPU_TREE_MUX_UNLOCK();

    return ret;
}

static uint32_t cpu_pwr_c_state_latency_get_(cpu_pwr_t *  p_cpu_node,
                                             cpu_cstate_t cpu_c_state)
{
    int i;

    if (p_cpu_node == NULL) {
        PWR_DBG(DBG_ERR, "p_cpu_node is NULL\n", p_cpu_node);

        return (uint32_t)CPU_LATENCY_UNKNOW;
    }

    /* if we support cpu_cstate_set_func, then we should save the CPU latency
       in initialization stage already. */
    if (p_cpu_node->cpu_cstate_set_func != NULL) {

        /*
         * if the query C state is supported, then we must has the CPU
         * latency info, or it is a bug which is something such as
         * p_pair_latency is is not initialized in the boot stage.
         */
        if (CPU_STATE_BIT(cpu_c_state) & p_cpu_node->support_bitset_c) {
            if (p_cpu_node->p_pair_latency != NULL) {
                for (i = 0; i < p_cpu_node->cstate_nums; i++) {
                    if (cpu_c_state == p_cpu_node->p_pair_latency[i].state) {
                        return p_cpu_node->p_pair_latency[i].value;
                    }
                }
            } else {
                PWR_DBG(DBG_ERR,
                        "p_cpu_node(%s%d)->pPStateAttrArray is not "
                        "initialized\n",
                        p_cpu_node->name, p_cpu_node->unit);

                return (uint32_t)CPU_LATENCY_UNKNOW;
            }
        }
    }

    return (uint32_t)CPU_LATENCY_UNKNOW;
}

/**
 * This routine gets the Cstate latency of a logical CPU for the specified
 * Cstate. Cstate latency is the time when CPU wake up from idle/sleep state,
 * normally this time is a fixed value which is decided by hardware design and
 * electrical characteristics. The deeper idle/sleep state CPU enter, the longer
 * latency time is need when CPU wakeup.
 *
 * Tickless module will use latency info to evaluate which C state should enter,
 * for example, if latency C1 is 5 ticks, latency C2 is 10 ticks, than if there
 * is nothing to do for the system in the feature 10 ticks, system would enter
 * C2 state and wake up after 10 ticks.
 *
 * @return  Cstate latency(microsecond) of a logical CPU for specified Cstate
 *          or CPU_LATENCY_UNKNOW when failed.
 */
uint32_t cpu_pwr_c_state_latency_get(uint32_t cpu_idx, cpu_cstate_t cpu_c_state)
{
    cpu_pwr_t *p_cpu_node;

    if (cpu_idx >= CPUS_NUM_MAX) {
        PWR_DBG(DBG_ERR, "cpu_idx(%d) error, it should be 0 ~ %d\n", cpu_idx,
                CPUS_NUM_MAX - 1);

        return (uint32_t)CPU_LATENCY_UNKNOW;
    }

    p_cpu_node = p_cpu_node_array[cpu_idx];

    return cpu_pwr_c_state_latency_get_(p_cpu_node, cpu_c_state);
}


/*
 * cpu_pwr_hal_lib_init() is called to initialize the cpu hardware
 * abstraction layer infrastructure, such as constructing the semphore.
 *
 * In the end of this routine, it will execute a function named
 * board_cpu_pwr_init() which is provided by board/platform
 */
void cpu_pwr_hal_lib_init(void)
{
    /* clean the space */
    memset(p_cpu_node_array, 0, sizeof(cpu_pwr_t *) * CPUS_NUM_MAX);

    krhino_spin_lock_init(&cpu_pwr_lock);

    if (krhino_mutex_create(&cpu_pwr_hal_mux, "cpu_pwr_hal_mux") != PWR_OK) {
        PWR_DBG(DBG_ERR, "krhino_mutex_create(cpu_pwr_hal_mux) failed!\n");
        return;
    }

    /* call board function to create CPU power management topology tree */
    board_cpu_pwr_init();

    /*
     * set CPU default idle mode to sleep mode,
     * if tickless configuration is enabled,
     * the idle mode will be updated to tickless mode by tickless module.
     */
    (void)cpu_pwr_idle_mode_set(CPU_IDLE_MODE_SLEEP);

    PWR_DBG(DBG_INFO, "cpu_pwr_hal_lib_init() done!\n");
}

/**
 * num_of_bit_one_get() return the number of 1 for the given binary value.
 */
static int num_of_bit_one_get(uint32_t n)
{
    int count = 0;

    while (n != 0) {
        n = n & (n - 1);

        count++;
    }

    return count;
}

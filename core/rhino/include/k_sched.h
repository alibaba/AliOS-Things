/**
 * @file k_sched.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_SCHED_H
#define K_SCHED_H

/** @addtogroup aos_rhino sched
 *  Task schedule.
 *
 *  @{
 */

#define KSCHED_FIFO          0u
#define KSCHED_RR            1u
#define KSCHED_CFS           2u

#define SCHED_MAX_LOCK_COUNT 200u
#define NUM_WORDS            ((RHINO_CONFIG_PRI_MAX + 31) / 32)

/**
 * Ready tasks information
 * 'cur_list_item' as task ready list per priority.
 * When a task enters 'ready' from another state, it is put into the list;
 * When a task leaves 'ready' to another state, it is get out of the list;
 * The 'task_bit_map' is used to improve performance.
 * The Nth bit in the bitmap starting from msb corresponds to the priority N-1.
 */
typedef struct {
    klist_t  *cur_list_item[RHINO_CONFIG_PRI_MAX];
    uint32_t  task_bit_map[NUM_WORDS];
    uint8_t   highest_pri;
} runqueue_t;


typedef struct {
    /* to add */
    uint8_t dis_sched;
} per_cpu_t;

/**
 * Disable task schedule
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sched_disable(void);

/**
 * Enable task schedule
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_sched_enable(void);

/** @} */

#endif /* K_SCHED_H */


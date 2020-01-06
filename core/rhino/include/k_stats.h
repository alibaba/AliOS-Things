/**
 * @file k_stats.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_STATS_H
#define K_STATS_H

/** @addtogroup aos_rhino stats
 *  OS statistic
 *
 *  @{
 */

#if (RHINO_CONFIG_KOBJ_LIST > 0)
/**
 * Init statistic list.
 *
 * @param[in]  NULL
 *
 * @return  NULL
 */
void kobj_list_init(void);
#endif

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
/**
 * Check task stack overflow.
 *
 * @param[in]  NULL
 *
 * @return  NULL
 */
void krhino_stack_ovf_check(void);
#endif

#if (RHINO_CONFIG_SYS_STATS > 0)
/**
 * Reset task schedule statistic data.
 *
 * @param[in]  NULL
 *
 * @return  NULL
 */
void krhino_task_sched_stats_reset(void);

/**
 * Record task schedule statistic data.
 *
 * @param[in]  NULL
 *
 * @return  NULL
 */
void krhino_task_sched_stats_get(void);
#endif

#if (RHINO_CONFIG_HW_COUNT > 0)
/**
 * Record measurement overhead.
 *
 * @param[in]  NULL
 *
 * @return  NULL
 */
void krhino_overhead_measure(void);
#endif

/** @} */

#endif /* K_STATS_H */


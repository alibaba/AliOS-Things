/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_SOC_H
#define K_SOC_H

#if (RHINO_CONFIG_HW_COUNT > 0)
void       soc_hw_timer_init(void);
hr_timer_t soc_hr_hw_cnt_get(void);
lr_timer_t soc_lr_hw_cnt_get(void);
#define HR_COUNT_GET() soc_hr_hw_cnt_get()
#else  /* RHINO_CONFIG_HW_COUNT */
#define HR_COUNT_GET() 0u
#endif /* RHINO_CONFIG_HW_COUNT */

#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
#define LR_COUNT_GET() soc_lr_hw_cnt_get()
#else  /* RHINO_CONFIG_TASK_SCHED_STATS */
#define LR_COUNT_GET() 0u
#endif /* RHINO_CONFIG_TASK_SCHED_STATS */

#if (RHINO_CONFIG_INTRPT_GUARD > 0)
void soc_intrpt_guard(void);
#endif

#if (RHINO_CONFIG_INTRPT_STACK_REMAIN_GET > 0)
size_t soc_intrpt_stack_remain_get(void);
#endif

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void);
#endif

void soc_err_proc(kstat_t err);

size_t soc_get_cur_sp(void);

#endif /* K_SOC_H */


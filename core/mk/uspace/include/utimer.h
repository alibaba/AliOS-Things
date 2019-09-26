/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UTIMER_H
#define UTIMER_H

#ifdef __cplusplus
extern "C" {
#endif

kstat_t krhino_timer_create(ktimer_t *timer, const name_t *name, timer_cb_t cb,
                            sys_time_t first, sys_time_t round, void *arg,
                            uint8_t auto_run);

kstat_t krhino_timer_dyn_create(ktimer_t **timer, const name_t *name,
                                timer_cb_t cb, sys_time_t first, sys_time_t round,
                                void *arg, uint8_t auto_run);

kstat_t krhino_timer_dyn_del(ktimer_t *timer);

kstat_t krhino_timer_start(ktimer_t *timer);

kstat_t krhino_timer_stop(ktimer_t *timer);

kstat_t krhino_timer_del(ktimer_t *timer);

kstat_t krhino_timer_change(ktimer_t *timer, sys_time_t first, sys_time_t round);

kstat_t krhino_timer_arg_change(ktimer_t *timer, void *arg);

kstat_t krhino_timer_arg_change_auto(ktimer_t *timer, void *arg);

int timer_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio, size_t msg_num);

#ifdef __cplusplus
}
#endif

#endif /* UTIMER_H */


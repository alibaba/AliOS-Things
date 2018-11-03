
#ifndef U_TIMER_H
#define U_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

void    timer_init();

kstat_t krhino_timer_create(ktimer_t *timer, const name_t *name, timer_cb_t cb,
                            sys_time_t first, sys_time_t round, void *arg, uint8_t auto_run);

kstat_t krhino_timer_dyn_create(ktimer_t **timer, const name_t *name,
                                timer_cb_t cb,
                                sys_time_t first, sys_time_t round, void *arg, uint8_t auto_run);

kstat_t krhino_timer_dyn_del(ktimer_t *timer);

kstat_t krhino_timer_start(ktimer_t *timer);
kstat_t krhino_timer_stop(ktimer_t *timer);
kstat_t krhino_timer_del(ktimer_t *timer);
kstat_t krhino_timer_change(ktimer_t *timer, sys_time_t first, sys_time_t round);
kstat_t krhino_timer_arg_change(ktimer_t *timer, void *arg);
kstat_t krhino_timer_arg_change_auto(ktimer_t *timer, void *arg);


#ifdef __cplusplus
}
#endif

#endif



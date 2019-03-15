
#ifndef _ESPOS_AOS_VCALL_H_
#define _ESPOS_AOS_VCALL_H_

#include <stdbool.h>

#include "esp_attr.h"
#include "espos_types.h"
#include "espos_errno.h"
#include "espos_spinlock.h"

#ifdef __cplusplus
extern "C"
{
#endif

unsigned IRAM_ATTR espos_schedule_running(int coreid);

void espaos_critical_init(espos_spinlock_t *spinlock);
espos_critical_t espaos_enter_crital(espos_spinlock_t *spinlock);
void espaos_exit_crital(espos_spinlock_t *spinlock, espos_critical_t tmp);
espos_critical_t espaos_enter_crital_isr(espos_spinlock_t *spinlock);
void espaos_exit_crital_isr(espos_spinlock_t *spinlock, espos_critical_t tmp);

unsigned espaos_enter_critical_nested();
void espaos_exit_critical_nested(unsigned state);

espos_task_t espos_task_get_idle_task_for_cpu(int cpuid);
espos_task_t espos_task_get_current_for_cpu(espos_cpu_t cpu_id);

unsigned espaos_interrupted_from_isr_cxt();
int espos_queue_is_full_generic(espos_queue_t queue);
#ifdef __cplusplus
}
#endif

#endif
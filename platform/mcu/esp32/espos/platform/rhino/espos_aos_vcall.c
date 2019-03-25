
#include <stddef.h>
#include <stdint.h>

#include "k_api.h"

#include "espos_aos_vcall.h"

extern volatile uint64_t g_cpu_flag;
extern kstat_t   g_sys_stat;
extern uint8_t   g_intrpt_nested_level[];

unsigned espos_schedule_running(int coreid)
{
    return 1;
}

void espaos_critical_init(espos_spinlock_t *spinlock)
{

#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_lock_init((kspinlock_t *)spinlock);
#endif
}

espos_critical_t espaos_enter_crital(espos_spinlock_t *spinlock)
{
    espos_critical_t cpsr;
    kspinlock_t *lock = (kspinlock_t *)spinlock;
    cpsr = cpu_intrpt_save();

#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_lock(lock);
#endif
    return cpsr;
}

void espaos_exit_crital(espos_spinlock_t *spinlock, espos_critical_t tmp)
{
    kspinlock_t *lock = (kspinlock_t *)spinlock;

#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_unlock(lock);
#endif
    cpu_intrpt_restore(tmp);
}

espos_critical_t espaos_enter_crital_isr(espos_spinlock_t *spinlock)
{
    espos_critical_t cpsr;
    kspinlock_t *lock = (kspinlock_t *)spinlock;
    cpsr = cpu_intrpt_save();

#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_lock(lock);
#endif
	return cpsr;
}

void espaos_exit_crital_isr(espos_spinlock_t *spinlock, espos_critical_t tmp)
{
    kspinlock_t *lock = (kspinlock_t *)spinlock;

#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_unlock(lock);
#endif
    cpu_intrpt_restore(tmp);
}

unsigned espaos_enter_critical_nested()
{
    espos_critical_t cpsr;
    cpsr = cpu_intrpt_save();
    return cpsr;
}

void espaos_exit_critical_nested(unsigned state)
{
    cpu_intrpt_restore(state);
}

espos_task_t espos_task_get_idle_task_for_cpu(espos_cpu_t cpu_id)
{
    return (espos_task_t)(&g_idle_task[cpu_id]);
}

espos_task_t espos_task_get_current_for_cpu(espos_cpu_t cpu_id)
{
    return (espos_task_t)(g_active_task[cpu_id]);
}

unsigned espaos_interrupted_from_isr_cxt()
{
    return (g_intrpt_nested_level[cpu_cur_get()] != 0);
}

int espos_queue_is_full_generic(espos_queue_t queue)
{
    /* TBD */
    return 0;
}




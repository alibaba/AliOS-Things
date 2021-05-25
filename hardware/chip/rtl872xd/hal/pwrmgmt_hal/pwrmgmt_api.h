#if (AOS_COMP_PWRMGMT > 0)
#include "pwrmgmt.h"
#endif

#include "ameba_soc.h"

int      pwrmgmt_cpu_active_msec_set(uint32_t active_time);
uint32_t pwrmgmt_sleep_type_set(uint32_t type);
int      pwrmgmt_cpu_lowpower_suspend(uint32_t suspend_module);
int      pwrmgmt_cpu_lowpower_resume(uint32_t resume_module);
int      pwrmgmt_cpu_minisleep_msec_set(uint32_t time_ms);
uint32_t pwrmgmt_cpu_minisleep_msec_get(void);
int      pwrmgmt_wifi_powersave_resume(uint32_t resume_module);
int      pwrmgmt_wifi_powersave_suspend(uint32_t suspend_module);

void pmu_unregister_sleep_callback(u32 nDeviceId);
void pmu_register_sleep_callback(u32 nDeviceId, PSM_HOOK_FUN sleep_hook_fun, void* sleep_param_ptr, PSM_HOOK_FUN wakeup_hook_fun, void* wakeup_param_ptr);
void pmu_acquire_deepwakelock(uint32_t nDeviceId);
void pmu_release_deepwakelock(uint32_t nDeviceId);


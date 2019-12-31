/**
 * @file      pwrmgmt.h
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef PWRMGMT_H
#define PWRMGMT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "k_api.h"

#include "pwrmgmt_default_config.h"

/**
 * @addtogroup aos_pwrmgmt pwrmgmt
 * power management module
 *
 * @{
 */

/**
 * This enum define the module id which used to call functions like aos_pwrmgmt_lowpower_suspend
 */
typedef enum pwrmgmt_user_module {
    PWRMGMT_RHINO   = 0,
    PWRMGMT_OTA     = 1,
    PWRMGMT_NETMGR  = 2,
    PWRMGMT_NETWORK = 3,
    PWRMGMT_PWRMGMT = 4,
    PWRMGMT_AT      = 5,
    PWRMGMT_APP     = 16,
} PWRMGMT_USER_MODULE;

/**
 * This function will suspend the low powr
 * @param[in]  suspend_module  the module name which call this funciton
 * @return  the operation status, 0 is OK, -1 is error
 */
int32_t aos_pwrmgmt_lowpower_suspend(uint32_t suspend_module);

/**
 * This function will resume the low powr
 * @param[in]  resume_module  the module name which call this funciton
 * @return  the operation status, 0 is OK, -1 is error
 */
int32_t aos_pwrmgmt_lowpower_resume(uint32_t resume_module);

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
/**
 * This function will resume the cpu low powr
 * @param[in]  suspend_module  the module name which call this funciton
 * @return  the operation status, 0 is OK, -1 is error
 */
int32_t aos_pwrmgmt_cpu_lowpower_suspend(uint32_t suspend_module);

/**
 * This function will resume the cpu low powr
 * @param[in]  resume_module  the module name which call this funciton
 * @return  the operation status, 0 is OK, -1 is error
 */
int32_t aos_pwrmgmt_cpu_lowpower_resume(uint32_t resume_module);

#if (PWRMGMT_CONFIG_MINISLEEP > 0)
/**
 * This function will keep the CPU active for active_time ms
 * @param[in]  active_time  the time to keep the CPU active in milisecond
 * @return  the operation status, 0 is OK, -1 is error
 */
int32_t aos_pwrmgmt_cpu_minisleep_set(uint32_t time_ms);

/**
 * This function will get the minimum CPU sleep time currently set
 * @return The minimum CPU sleep time currently set
 */
uint32_t aos_pwrmgmt_cpu_minisleep_get(void);
#endif

#if (PWRMGMT_CONFIG_CPU_ACTIVE > 0)
/**
 * This function will keep the CPU active for active_time ms
 * @param[in]  active_time  the time to keep the CPU active in milisecond.
 * @return  the operation status, 0 is OK, -1 is error
 */
int32_t aos_pwrmgmt_cpu_active_set(uint32_t active_time);
#endif
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
/**
 * This function will enable the wifi power save
 * @return  the operation status, 0 is OK, -1 is error
 */
int32_t aos_pwrmgmt_wifi_powersave_enable(void);

/**
 * This function will resume the wifi power save
 * @param[in]  resume_module  the module name which call this funciton
 * @return  the operation status, 0 is OK, -1 is error
 */
int32_t aos_pwrmgmt_wifi_powersave_resume(uint32_t resume_module);

/**
 * This function will suspend the wifi power save
 * @param[in]  suspend_module  the module name which call this funciton
 * @return  the operation status, 0 is OK, -1 is error
 */
int32_t aos_pwrmgmt_wifi_powersave_suspend(uint32_t suspend_module);
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PWRMGMT_H */


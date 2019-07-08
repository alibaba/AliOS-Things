/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#ifndef PWRMGMT_DEFAULT_CONFIG_H
#define PWRMGMT_DEFAULT_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef PWRMGMT_CONFIG_SHOW
#define PWRMGMT_CONFIG_SHOW       0
#endif

#ifndef PWRMGMT_CONFIG_DEBUG
#define PWRMGMT_CONFIG_DEBUG      0
#endif

#ifndef PWRMGMT_CONFIG_CPU_ACTIVE
#define PWRMGMT_CONFIG_CPU_ACTIVE 0
#endif

#ifndef PWRMGMT_CONFIG_MINISLEEP
#define PWRMGMT_CONFIG_MINISLEEP  0
#endif

#if ((PWRMGMT_CONFIG_MINISLEEP >= 1) && (PWRMGMT_CONFIG_CPU_LOWPOWER == 0))
#error "PWRMGMT_CONFIG_CPU_LOWPOWER should be 1 when PWRMGMT_CONFIG_MINISLEEP is enabled."
#endif

#if ((PWRMGMT_CONFIG_CPU_ACTIVE >= 1) && (PWRMGMT_CONFIG_CPU_LOWPOWER == 0))
#error "PWRMGMT_CONFIG_CPU_LOWPOWER should be 1 when PWRMGMT_CONFIG_CPU_ACTIVE is enabled."
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PWRMGMT_DEFAULT_CONFIG_H */

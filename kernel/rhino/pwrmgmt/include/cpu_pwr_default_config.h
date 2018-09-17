/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#ifndef K_CPU_PWR_DEFAULT_CONFIG_H
#define K_CPU_PWR_DEFAULT_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Maximum number of CPUs */
#ifndef CPUS_NUM_MAX
#define CPUS_NUM_MAX 1
#endif

#ifndef RHINO_CONFIG_CPU_PWR_SHOW
#define RHINO_CONFIG_CPU_PWR_SHOW 0
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* K_CPU_PWR_DEFAULT_CONFIG_H */

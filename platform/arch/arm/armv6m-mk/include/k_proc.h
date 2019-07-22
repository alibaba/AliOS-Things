/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_PROC_H
#define K_PROC_H

#include <stdint.h>

#ifndef RHINO_CONFIG_MAX_APP_BINS
#define MAX_APP_BINS   (3) /**< by default, support 3 app bins */
#else
#if (RHINO_CONFIG_MAX_APP_BINS > 32)
#error "app bins count beyonds 32"
#endif
#define MAX_APP_BINS RHINO_CONFIG_MAX_APP_BINS
#endif

/**
 * @brief process init
 *
 * @return On success, return 0, else return negative error code
 */
int k_proc_init(void);

/**
 * @brief start all apps
 *
 * @return return the number of app star
 */
int k_proc_exec_all(void);

/**
 * @brief unload process
 *
 * @param[in] pid   the PID of the process to be unloaded
 */
void k_proc_unload(uint32_t pid);

/**
 * @brief switch process on current CPU
 *
 * @param[in] new   the new task to be scheded in
 * @param[in] old   the old task to be scheded out
 */
void k_proc_switch(void *new, void *old);

#endif /* K_PROC_H */


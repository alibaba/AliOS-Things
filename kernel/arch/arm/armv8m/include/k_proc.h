/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_PROC_H
#define K_PROC_H

#include <stdint.h>
#include "preamble.h"
#include "k_mpu.h"

#ifndef RHINO_CONFIG_MAX_APP_BINS
    #define MAX_APP_BINS (6) /**< by default, support app bins */
#else
    #if (RHINO_CONFIG_MAX_APP_BINS > 32)
        #error "app bins count beyonds 32"
    #endif
    #define MAX_APP_BINS RHINO_CONFIG_MAX_APP_BINS
#endif

typedef struct {
    mpu_region_t text_region;
    mpu_region_t data_region;
} app_region_t;

typedef struct mpu_io_region {
    mpu_region_t io_region;
    uint32_t    region_id;
    int32_t     memattr;
    struct mpu_io_region *next;
} io_region_t;

typedef struct {
    uint32_t      pid;
    preamble_t   *preamble;
    app_region_t  region;
    io_region_t *io_regions;
} app_t;

typedef struct {
    app_t procs[MAX_APP_BINS + 1];
} app_info_t;

/**
 * @brief process init
 *
 * @return On success, return 0, else return negative error code
 */
int k_proc_init(void *phyaddr_dummy, size_t size_dummp);

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

/**
 * @brief start user space apps
 *
 * @return On success return 0, else return negative errno
 */
int application_start(int argc, char **argv);

int32_t k_proc_load(const char *file, const char *name);

int32_t k_proc_ramload(const void *ramaddr, const char *name);

#endif /* K_PROC_H */


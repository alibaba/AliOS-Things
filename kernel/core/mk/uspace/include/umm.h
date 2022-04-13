/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMM_H
#define UMM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "k_api.h"

#define UMM_ERROR_LOCKED        1
#define UMM_ERROR_UNLOCKED      0
#define UMM_BT_SET_BY_KV        1

/*** User heap management config ***/
/**
 * enable futex for umm, better performance
 */
#ifndef RHINO_CONFIG_UMM_FUTEX
#define RHINO_CONFIG_UMM_FUTEX           1
#endif

/**
 * Enable umm debug, low performance better bug shooting
 */
#ifndef RHINO_CONFIG_UMM_DEBUG
#define RHINO_CONFIG_UMM_DEBUG           1
#endif

/************* config end **********/

void umm_init(void);

void *umm_alloc(size_t size);

void *umm_realloc(void *ptr, size_t size);

void umm_free(void *ptr);

void umm_cli_cmds_register(void);

void u_mm_critical_enter(void);

void u_mm_critical_exit(void);

#ifdef __cplusplus
}
#endif

#endif /* UMM_H */


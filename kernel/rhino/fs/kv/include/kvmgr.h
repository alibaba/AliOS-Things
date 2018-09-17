/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _KEY_VALUE_H
#define _KEY_VALUE_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief init the kv module
 *
 * @retrun 0 on success, otherwise will be failed.
 */
int aos_kv_init(void);

/**
 * @brief deinit the kv module
 *
 * @retrun none
 */
void aos_kv_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _key_value_h_
#define _key_value_h_

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

/* The totally storage size for key-value store */
#ifndef CONFIG_AOS_KV_BUFFER_SIZE
#define KV_TOTAL_SIZE   (8 * 1024)
#else
#define KV_TOTAL_SIZE   CONFIG_AOS_KV_BUFFER_SIZE
#endif

/* The physical parition for key-value store */
#ifndef CONFIG_AOS_KV_PTN
#define KV_PTN    HAL_PARTITION_PARAMETER_2
#else
#define KV_PTN    CONFIG_AOS_KV_PTN
#endif

/**
 * @brief init the kv module.
 *
 * @param[in] none.
 *
 * @note: the default KV size is @HASH_TABLE_MAX_SIZE, the path to store
 *        the kv file is @KVFILE_PATH.
 * @retval  0 on success, otherwise -1 will be returned
 */
int aos_kv_init(void);

/**
 * @brief deinit the kv module.
 *
 * @param[in] none.
 *
 * @note: all the KV in RAM will be released.
 * @retval none.
 */
void aos_kv_deinit(void);

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif



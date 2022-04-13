/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef CACHE_HAL_H
#define CACHE_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

enum HAL_CACHE_ID_T {
    HAL_CACHE_ID_I_CACHE = 0,
    HAL_CACHE_ID_D_CACHE,
    HAL_CACHE_ID_NUM,
};

uint8_t hal_cache_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_writebuffer_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_writebuffer_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_writebuffer_flush(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_writeback_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_writeback_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_wrap_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_wrap_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cache_invalidate(enum HAL_CACHE_ID_T id, uint32_t start_address, uint32_t len);
uint8_t hal_cache_sync(enum HAL_CACHE_ID_T id);

uint8_t hal_cachecp_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_writebuffer_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_writebuffer_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_writebuffer_flush(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_writeback_enable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_writeback_disable(enum HAL_CACHE_ID_T id);
uint8_t hal_cachecp_invalidate(enum HAL_CACHE_ID_T id, uint32_t start_address, uint32_t len);
uint8_t hal_cachecp_sync(enum HAL_CACHE_ID_T id);

#ifdef __cplusplus
}
#endif

#endif /* CACHE_HAL_H */

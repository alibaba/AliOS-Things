/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_TRNG_H__
#define __HAL_TRNG_H__
#ifdef __cplusplus
extern "C" {
#endif
#ifdef CHIP_HAS_TRNG
#include "plat_types.h"


enum HAL_TRNG_RET {
    HAL_TRNG_RET_OK = 0,
    HAL_TRNG_RET_FAILED,
};
typedef unsigned int HAL_TRNG_RET_ENUM;

enum HAL_TRNG_RND_SRC_SEL {
	HAL_TRNG_RND_SRC_SHORTEST = 0,
	HAL_TRNG_RND_SRC_SHORT,
	HAL_TRNG_RND_SRC_LONG,
	HAL_TRNG_RND_SRC_LONGEST,
};
typedef unsigned int HAL_TRNG_RND_SRC_SEL_ENUM;

struct HAL_TRNG_CFG_T {
    uint32_t 					sample_cntr1; // must not be less than 0x11	
    HAL_TRNG_RND_SRC_SEL_ENUM   rnd_src_sel; 
};

HAL_TRNG_RET_ENUM hal_trng_open(const struct HAL_TRNG_CFG_T *cfg);
void hal_trng_close(void);
void get_trngdata(uint32* data);

#endif // CHIP_HAS_TRNG
#ifdef __cplusplus
}
#endif
#endif //__HAL_TRNG_H__

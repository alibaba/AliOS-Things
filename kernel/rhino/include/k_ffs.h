/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_FFS_H
#define K_FFS_H

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

extern int ffs32_lsb (uint32_t i);
extern int ffs32_msb (uint32_t i);

#define FFS_LSB(i)		ffs32_lsb(i)
#define FFS_MSB(i)		ffs32_msb(i)

#ifdef __cplusplus
}
#endif

#endif /* K_FFS_H */


/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_EFLASH_H_
#define _TEE_EFLASH_H_

#include "tee_types.h"

enum eflash_cmd {
    EFLASH_ERASE        = 0,
    EFLASH_INVALID      = 1,
};

typedef struct _tee_eflash_read_args_t {
    uint32_t addr;
    uint8_t *buf;
    uint32_t size;
} tee_eflash_read_args_t;

typedef struct _tee_eflash_write_args_t {
    uint32_t addr;
    uint32_t *buf;
    uint32_t size;
} tee_eflash_write_args_t;

typedef struct _tee_eflash_erase_args_t {
    uint32_t addr;
    uint32_t sec_num;
} tee_eflash_erase_args_t;

#endif /* _TEE_ELASH_H_ */


/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_ASC_H_
#define _TEE_ASC_H_

#include "device.h"

#define TEE_DRV_ASC_NAME       "asc"

#define TEE_ASC_SET_MEM_PERM   (0x0)
#define TEE_ASC_GET_MEM_PERM   (0x1)
#define TEE_ASC_CLR_MEM_PERM   (0x2)

/* region security prop */
#define REGION_SIZE_UNSECURITY (0x0)
#define REGION_SIZE_SECURITY   (0x1)

/* region AP authority */
#define REGION_READ_WRITE      (0x0)
#define REGION_READ_ONLY       (0x1)
#define REGION_WRITE_ONLY      (0x3)
#define REGION_NO_ACCESS       (0x4)

/* region CD authority */
#define REGION_ACCESS_DENY     (0x0)
#define REGION_ONLY_DATA       (0x1)
#define REGION_ONLY_INSTRUCT   (0x2)
#define REGION_BOTH            (0x3)

typedef struct _tee_asc_perm_t {
    uint8_t sec;
    uint8_t ap;
    uint8_t cd;
} tee_asc_perm_t;

typedef struct _tee_asc_arg_t {
    size_t paddr;
    size_t size;
    tee_asc_perm_t perm;
} tee_asc_arg_t;

extern int asc_set_perm(uint32_t paddr, uint32_t size, bool is_secure, uint8_t ap_perm, uint8_t cd_perm);

#endif

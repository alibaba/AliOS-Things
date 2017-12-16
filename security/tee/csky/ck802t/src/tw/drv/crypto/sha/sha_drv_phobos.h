/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 **/

#ifndef _SHA_DRV_PHOBOS_h_
#define _SHA_DRV_PHOBOS_h_

#include "tee_types.h"

#define SHA_DRV_INITED    0x0
#define SHA_DRV_STARTED   0x1
#define SHA_DRV_CONTINUE  0x2

typedef enum _sha_control_bits_t {
    SHA_CONTROL_BITS_CALC        = 0,   /* [2:0] */
    SHA_CONTROL_BITS_ENABLE_INIT = 3,   /* [3] */
    SHA_CONTROL_BITS_INTERRUPT   = 4,   /* [4] */
    SHA_CONTROL_BITS_ENDIAN      = 5,   /* [5] */
    SHA_CONTROL_BITS_RUN         = 6,   /* [6] */
} sha_control_bits_t;

typedef enum _sha_control_calc_mode_t {
    SHA_CONTROL_CALC_MODE_INVALID    = 0x0,
    SHA_CONTROL_CALC_MODE_SHA1       = 0x1,
    SHA_CONTROL_CALC_MODE_SHA256     = 0x2,
    SHA_CONTROL_CALC_MODE_SHA224     = 0x3,
    SHA_CONTROL_CALC_MODE_SHA512     = 0x4,
    SHA_CONTROL_CALC_MODE_SHA384     = 0x5,
    SHA_CONTROL_CALC_MODE_SHA512_256 = 0x6,
    SHA_CONTROL_CALC_MODE_SHA512_224 = 0x7,
    SHA_CONTROL_CALC_MODE_MAX
} sha_control_calc_mode_t;

typedef enum _sha_control_endian_mode_t {
    SHA_CONTROL_ENDIAN_MODE_BIG_ENDIAN    = 0,
    SHA_CONTROL_ENDIAN_MODE_LITTLE_ENDIAN = 1
} sha_control_endian_mode_t;

typedef struct _sha_phobos_reg_t {
    uint32_t SHA_CON;
    uint32_t SHA_INTSTATE;
    uint32_t SHA_H0L;
    uint32_t SHA_H1L;
    uint32_t SHA_H2L;
    uint32_t SHA_H3L;
    uint32_t SHA_H4L;
    uint32_t SHA_H5L;
    uint32_t SHA_H6L;
    uint32_t SHA_H7L;
    uint32_t SHA_H0H;
    uint32_t SHA_H1H;
    uint32_t SHA_H2H;
    uint32_t SHA_H3H;
    uint32_t SHA_H4H;
    uint32_t SHA_H5H;
    uint32_t SHA_H6H;
    uint32_t SHA_H7H;
    uint32_t SHA_DATA1;
    uint32_t REV[15];
    uint32_t SHA_DATA2;
} sha_phobos_reg_t;

typedef struct _sha_drv_ctx_t {
    uint32_t state;
    uint8_t data[128];
    uint32_t data_size;
    uint32_t total_size;
    sha_phobos_reg_t sha_regs;
} sha_drv_ctx_t;

#endif

/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 **/

#ifndef _SHA_DRV_h_
#define _SHA_DRV_h_

#include "tee_types.h"

#define SHA_DRV_INITED 0x0
#define SHA_DRV_STARTED 0x1
#define SHA_DRV_CONTINUE 0x2

typedef struct _sha_reg_t
{
    uint32_t sha_ctrl;
    uint32_t sha_mode;
    uint32_t sha_intstate;
    uint32_t sha_baseaddr;
    uint32_t sha_destaddr;
    uint32_t sha_counter0;
    uint32_t sha_counter1;
    uint32_t sha_counter2;
    uint32_t sha_counter3;
    uint32_t sha_h0l;
    uint32_t sha_h1l;
    uint32_t sha_h2l;
    uint32_t sha_h3l;
    uint32_t sha_h4l;
    uint32_t sha_h5l;
    uint32_t sha_h6l;
    uint32_t sha_h7l;
    uint32_t sha_h0h;
    uint32_t sha_h1h;
    uint32_t sha_h2h;
    uint32_t sha_h3h;
    uint32_t sha_h4h;
    uint32_t sha_h5h;
    uint32_t sha_h6h;
    uint32_t sha_h7h;
} sha_reg_t;

typedef struct _sha_drv_ctx_t
{
    uint32_t  state;
    uint8_t   data[128];
    uint32_t  data_size;
    uint32_t  total_size;
    sha_reg_t sha_regs;
} sha_drv_ctx_t;

#endif

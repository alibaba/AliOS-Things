/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef RTU_H
#define RTU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../adu.h"

mb_status_t rtu_assemble(mb_handler_t *handler);
mb_status_t rtu_disassemble(mb_handler_t *handler);

#ifdef __cplusplus
}
#endif

#endif /* RTU_H */

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef LPC_PLC_API_H
#define LPC_PLC_API_H

#include <stdint.h>

struct LpcPlcState_;

typedef struct LpcPlcState_ LpcPlcState;

#ifdef __cplusplus
extern "C" {
#endif

LpcPlcState *lpc_plc_create(int32_t sample_rate);

void lpc_plc_destroy(LpcPlcState *st);

void lpc_plc_save(LpcPlcState *st, int16_t *buf);

void lpc_plc_generate(LpcPlcState *st, int16_t *buf, int16_t *encbuf);

#ifdef __cplusplus
}
#endif

#endif
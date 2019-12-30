/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_ACTIVATION_H
#define UAI_ACTIVATION_H

#include "uai_function_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

void uai_relu(int8_t *data, uint16_t size, uint16_t out_width);
void uai_relu6(int8_t *data, uint16_t size, uint16_t out_width);
void uai_sigmod(int8_t *data, uint16_t size, uint16_t out_width);
void uai_tanh(int8_t *data, uint16_t size, uint16_t out_width);

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_ACTIVATION_H */

/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_SOFTMAX_H
#define UAI_SOFTMAX_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void uai_softmax(const int8_t *in_vec, const uint16_t dim_vec, int8_t *out_vec);

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_SOFTMAX_H */

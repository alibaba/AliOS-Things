/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_POOL_H
#define UAI_POOL_H

#include "uai_function_typedef.h"
#include "uai_common.h"

#ifdef __cplusplus
extern "C" {
#endif

void uai_pool_max(uai_input_s *input, uai_weight_s *filter, uint16_t *strides, uai_pad_type_e pad_type, uai_bias_s *bias, uai_output_s *output);

void uai_pool_ave(uai_input_s *input, uai_weight_s *filter, uint16_t *strides, uai_pad_type_e pad_type, uai_bias_s *bias, uai_output_s *output);

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_POOL_H */

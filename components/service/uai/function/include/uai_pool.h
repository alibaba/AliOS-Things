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

#ifdef UAI_ODLA_SUPPORT
void uai_pool_max(uai_tensor_s *input, uai_tensor_s *weight, uint16_t *strides, const unsigned *paddings_front,
                  const unsigned *paddings_back, uai_tensor_s *output);
void uai_pool_ave(uai_tensor_s *input, uai_tensor_s *weight, uint16_t *strides, const unsigned *paddings_front,
                  const unsigned *paddings_back, uai_tensor_s *output);
#else
void uai_pool_max(uai_input_s *input, uai_weight_s *filter, uint16_t *strides, uai_pad_type_e pad_type,
                  uai_bias_s *bias, uai_output_s *output);
void uai_pool_ave(uai_input_s *input, uai_weight_s *filter, uint16_t *strides, uai_pad_type_e pad_type,
                  uai_bias_s *bias, uai_output_s *output);
#endif

#ifdef __cplusplus
}
#endif

#endif /* end of UAI_POOL_H */

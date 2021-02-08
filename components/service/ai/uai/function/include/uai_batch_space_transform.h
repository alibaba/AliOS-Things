/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_BATCH_SPACE_TRANSFORM_H
#define UAI_BATCH_SPACE_TRANSFORM_H

#include "uai_function_typedef.h"
#include "uai_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UAI_ODLA_SUPPORT
int uai_space_to_batch(uai_tensor_s *input, uai_tensor_s *padding, int block_size, uai_tensor_s*output);
int uai_space_to_batch_nd(uai_tensor_s *input, uai_tensor_s *block, uai_tensor_s *padding, uai_tensor_s*output);
int uai_batch_to_space(uai_tensor_s *input, uai_tensor_s *crops, int block_size, uai_tensor_s *output);
int uai_batch_to_space_nd(uai_tensor_s *input, uai_tensor_s *block, uai_tensor_s *crops, uai_tensor_s*output);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_BATCH_SPACE_TRANSFORM_H */

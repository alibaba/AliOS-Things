/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include "uai_add.h"
#ifdef UAI_USE_CMSIS_NN
#include "arm_nnfunctions.h"
#endif

#ifdef UAI_ODLA_SUPPORT
int uai_pad(uai_tensor_s *input, const int32_t *padding_front, const int32_t *padding_back, uai_tensor_s *output)
{
    size_t i          = 0;
    size_t out_offset = 0;
    size_t in_offset  = 0;

    output->dim_batch    = input->dim_batch;
    output->dim_channels = input->dim_channels;
    output->dim_height   = input->dim_height + padding_front[0] + padding_back[0];
    output->dim_width    = input->dim_width + padding_front[1] + padding_back[1];

    out_offset = output->dim_width * padding_front[0];

    for ( i = 0; i < input->dim_height; i++) {
        out_offset += padding_front[1];
        memcpy(output->buffer + out_offset, input->buffer + in_offset, input->dim_width);
        out_offset += input->dim_width + padding_back[1];
        in_offset  += input->dim_width;
    }

    return 0;
}
#else
#endif
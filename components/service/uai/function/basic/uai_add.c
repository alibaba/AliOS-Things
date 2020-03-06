/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "uai_add.h"
#ifdef UAI_USE_CMSIS_NN
#include "arm_math.h"
#include "arm_nnfunctions.h"
#endif

#ifdef UAI_ODLA_SUPPORT
/* ODLA will broadcasting the bias to the same dimensions as input*/
int uai_bias_add(uai_tensor_s *input, uai_tensor_s *bias, uai_quant_scale *bias_scale, uai_quant_scale *act_scale, uai_tensor_s *output)
{
    int hi          = 0;
    int wi          = 0;
    int ci          = 0;

    if((input->dim_batch != 1) || (bias->dim_batch != 1)) {
        return UAI_FAIL;
    }

    if(UAI_IS_SAME_DIMS(input, bias)) {
        memcpy(&output->dims, &input->dims, sizeof(uai_dims));
        output->buffer = uai_malloc(uai_dims_size(&output->dims));
        for(ci = 0; ci < input->dim_channels; ci++) {
            for(wi = 0; wi < input->dim_width; wi++) {
                for(hi = 0; hi < input->dim_height; hi++) {
                    output->buffer[hi*wi*ci] = (int8_t) __SSAT(((((uint32_t *)input->buffer)[hi*wi*ci] + bias->buffer[hi*wi*ci]*bias_scale->scale[ci]) >> (act_scale->shift)) / act_scale->scale[ci], 8);
                }
            }
        }
    }  else {
        return UAI_FAIL;
    }

    return UAI_SUCCESS;
}

int uai_elementwise_add(uai_tensor_s *input1, uai_tensor_s *input2, uai_tensor_s *output)
{
    return UAI_FAIL;
}
#else
#endif
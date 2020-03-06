/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include "uai_pool.h"
#ifdef UAI_USE_CMSIS_NN
#include "arm_nnfunctions.h"

extern void
arm_maxpool_q7_HWC_nonsquare(q7_t * Im_in,
                   const uint16_t dim_im_in_x,
                   const uint16_t dim_im_in_y,
                   const uint16_t ch_im_in,
                   const uint16_t dim_kernel_x,
                   const uint16_t dim_kernel_y,
                   const uint16_t padding_x,
                   const uint16_t padding_y,
                   const uint16_t stride_x,
                   const uint16_t stride_y,
                   const uint16_t dim_im_out_x,
                   const uint16_t dim_im_out_y,
                   q7_t * bufferA,
                   q7_t * Im_out);
extern void
arm_avepool_q7_HWC_nonsquare(q7_t * Im_in,
                   const uint16_t dim_im_in_x,
                   const uint16_t dim_im_in_y,
                   const uint16_t ch_im_in,
                   const uint16_t dim_kernel_x,
                   const uint16_t dim_kernel_y,
                   const uint16_t padding_x,
                   const uint16_t padding_y,
                   const uint16_t stride_x,
                   const uint16_t stride_y,
                   const uint16_t dim_im_out_x,
                   const uint16_t dim_im_out_y,
                   q7_t * bufferA,
                   q7_t * Im_out);
#endif

#ifdef UAI_ODLA_SUPPORT
void uai_pool_max(uai_tensor_s *input, uai_tensor_s *weight, uint16_t *strides, const unsigned* paddings_front,
                  const unsigned* paddings_back, uai_tensor_s *output)
{
    uint16_t padding_x = (paddings_front[0] + paddings_back[0]) / 2;
    uint16_t padding_y = (paddings_front[1] + paddings_back[1]) / 2;

#if defined(UAI_USE_CMSIS_NN)

    if (input->dim_height == input->dim_width) {
        arm_maxpool_q7_HWC(input->buffer, input->dim_height, input->dim_channels, weight->dim_height,
                                 padding_x, strides[0], output->dim_height, NULL, output->buffer);
    } else {
        arm_maxpool_q7_HWC_nonsquare(input->buffer, input->dim_height, input->dim_width, input->dim_channels,
                                            weight->dim_height, weight->dim_width, padding_x, padding_y,
                                            strides[0], strides[1], output->dim_height, output->dim_width,
                                            NULL, output->buffer);
    }
#else
#endif
    return;
}

void uai_pool_ave(uai_tensor_s *input, uai_tensor_s *weight, uint16_t *strides, const unsigned* paddings_front,
                  const unsigned* paddings_back, uai_tensor_s *output)
{
    int8_t *buffer_input = NULL;
    uint16_t padding_x   = (paddings_front[0] + paddings_back[0]) / 2;
    uint16_t padding_y   = (paddings_front[1] + paddings_back[1]) / 2;

#if defined(UAI_USE_CMSIS_NN)
    #if defined (ARM_MATH_DSP)
    #ifdef UAI_MEM_STATIC
    buffer_input = UAI_CONV_TEMP_MEM;
    #else
    buffer_input = uai_malloc(2 * input->dim_channels * weight->dim_height * weight->dim_width);
    #endif
    #endif

    if (input->dim_height == input->dim_width) {
        arm_avepool_q7_HWC(input->buffer, input->dim_height, input->dim_channels, weight->dim_height,
                           padding_x, strides[0], output->dim_height, buffer_input, output->buffer);
    } else {
        arm_avepool_q7_HWC_nonsquare(input->buffer, input->dim_height, input->dim_width, input->dim_channels,
                                     weight->dim_height, weight->dim_width, padding_x, padding_y,
                                     strides[0], strides[1], output->dim_height, output->dim_width,
                                     buffer_input, output->buffer);
    }

    #if defined (ARM_MATH_DSP)
    #ifndef UAI_MEM_STATIC
    uai_free(buffer_input);
    #endif
    #endif
#else
#endif
    return;
}

#else
void uai_pool_max(uai_input_s *input, uai_weight_s *weight, uint16_t *strides, uai_pad_type_e pad_type, uai_bias_s *bias, uai_output_s *output)
{
    uint16_t padding_x = 0;
    uint16_t padding_y = 0;
    int16_t  temp      = 0;

#if defined(UAI_USE_CMSIS_NN)
    if (pad_type == UAI_PAD_SAME) {
        output->height = (input->height + input->height % strides[0]) / strides[0] ;
        output->width  = (input->width  + input->width  % strides[1]) / strides[1] ;

        temp           = (output->height - 1) * strides[0] + weight->height - input->height;
        padding_x      = (uint16_t)UAI_MAX(temp / 2, 0);

        temp           = (output->width  - 1) * strides[1] + weight->width  - input->width;
        padding_y      = (uint16_t)UAI_MAX(temp / 2, 0);
    } else {
        output->height = (input->height - weight->height) / strides[0];
        output->width  = (input->width  - weight->width)  / strides[1];
    }

    if (input->height == input->width) {
        arm_maxpool_q7_HWC(input->buffer, input->height, input->channels, weight->height,
                           padding_x, strides[0], output->height, NULL, output->buffer);
    } else {
        arm_maxpool_q7_HWC_nonsquare(input->buffer, input->height, input->width, input->channels,
                                     weight->height, weight->width, padding_x, padding_y,
                                     strides[0], strides[1], output->height, output->width,
                                     NULL, output->buffer);
    }
#else
#endif
    return;
}

void uai_pool_ave(uai_input_s *input, uai_weight_s *weight, uint16_t *strides, uai_pad_type_e pad_type,
                  uai_bias_s *bias, uai_output_s *output)
{
    int8_t *buffer_input = NULL;
    uint16_t padding_x   = 0;
    uint16_t padding_y   = 0;
    int16_t  temp        = 0;

#if defined(UAI_USE_CMSIS_NN)
    #if defined (ARM_MATH_DSP)
    #ifdef UAI_MEM_STATIC
    buffer_input = UAI_CONV_TEMP_MEM;
    #else
    buffer_input = uai_malloc(2 * input->channels * weight->height * weight->width);
    #endif
    #endif
    if (pad_type == UAI_PAD_SAME) {
        output->height = (input->height + input->height % strides[0]) / strides[0];
        output->width  = (input->width  + input->width  % strides[1]) / strides[1];

        temp           = (output->height - 1) * strides[0] + weight->height - input->height;
        padding_x      = (uint16_t)UAI_MAX(temp / 2, 0);

        temp           = (output->width  - 1) * strides[1] + weight->width  - input->width;
        padding_y      = (uint16_t)UAI_MAX(temp / 2, 0);
    } else {
        output->height = (input->height - weight->height) / strides[0];
        output->width  = (input->width  - weight->width)  / strides[1];
    }

    if (input->height == input->width) {
        arm_avepool_q7_HWC(input->buffer, input->height, input->channels, weight->height,
                           padding_x, strides[0], output->height, buffer_input, output->buffer);
    } else {
        arm_avepool_q7_HWC_nonsquare(input->buffer, input->height, input->width, input->channels,
                                     weight->height, weight->width, padding_x, padding_y,
                                     strides[0], strides[1], output->height, output->width,
                                     buffer_input, output->buffer);
    }

    #if defined (ARM_MATH_DSP)
    #ifndef UAI_MEM_STATIC
    uai_free(buffer_input);
    #endif
    #endif
#else
#endif
    return;
}

#endif
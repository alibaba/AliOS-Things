/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include "uai_function.h"
#ifdef UAI_USE_CMSIS_NN
#include "arm_nnfunctions.h"
#endif

#ifdef UAI_ODLA_SUPPORT
#define CONV_SQUARE_PARM        input->buffer,        \
                                input->dim_height,        \
                                input->dim_channels,      \
                                weight->buffer,       \
                                output->dim_channels, \
                                weight->dim_height,       \
                                padding_x,            \
                                strides[0],   \
                                output->buffer,       \
                                output->dim_height,       \
                                buffer_input,         \
                                NULL

#define CONV_NON_SQUARE_PARM    input->buffer,        \
                                input->dim_height,        \
                                input->dim_width,        \
                                input->dim_channels,      \
                                weight->buffer,       \
                                output->dim_channels, \
                                weight->dim_height,       \
                                weight->dim_width,       \
                                padding_x,            \
                                padding_y,            \
                                strides[0],   \
                                strides[1],   \
                                output->buffer,       \
                                output->dim_height,       \
                                output->dim_width,        \
                                buffer_input,         \
                                NULL
extern arm_status arm_depthwise_separable_conv_HWC_q7_uai(const q7_t * Im_in,
                                               const uint16_t dim_im_in,
                                               const uint16_t ch_im_in,
                                               const q7_t * wt,
                                               const uint16_t ch_im_out,
                                               const uint16_t dim_kernel,
                                               const uint16_t padding,
                                               const uint16_t stride,
                                               q31_t * Im_out,
                                               const uint16_t dim_im_out,
                                               q15_t * bufferA,
                                               q7_t * bufferB);
extern arm_status arm_depthwise_separable_conv_HWC_q7_nonsquare_uai(const q7_t * Im_in,
                                                         const uint16_t dim_im_in_x,
                                                         const uint16_t dim_im_in_y,
                                                         const uint16_t ch_im_in,
                                                         const q7_t * wt,
                                                         const uint16_t ch_im_out,
                                                         const uint16_t dim_kernel_x,
                                                         const uint16_t dim_kernel_y,
                                                         const uint16_t padding_x,
                                                         const uint16_t padding_y,
                                                         const uint16_t stride_x,
                                                         const uint16_t stride_y,
                                                         q31_t * Im_out,
                                                         const uint16_t dim_im_out_x,
                                                         const uint16_t dim_im_out_y,
                                                         q15_t * bufferA,
                                                         q7_t * bufferB);
int uai_conv_depthwise_sp_2d(uai_tensor_s *input, uai_tensor_s *weight, uint16_t *strides, const unsigned* paddings_front, const unsigned* paddings_back, uai_tensor_s *output)
{
    int8_t *buffer_input = NULL;
    uint16_t padding_x   = (paddings_front[0] + paddings_back[0])/2;
    uint16_t padding_y   = (paddings_front[1] + paddings_back[1])/2;
    int32_t  ret         = -1;

#if defined(UAI_USE_CMSIS_NN)
    #if defined (ARM_MATH_DSP)
    #ifdef UAI_MEM_STATIC
    buffer_input = UAI_CONV_TEMP_MEM;
    #else
    buffer_input = uai_malloc(2 * input->dim_channels * weight->dim_height * weight->dim_width);
    #endif
    #endif

    if(input->dim_height == input->dim_width) {
        ret = arm_depthwise_separable_conv_HWC_q7_uai(CONV_SQUARE_PARM);
    } else {
        ret = arm_depthwise_separable_conv_HWC_q7_nonsquare_uai(CONV_NON_SQUARE_PARM);
    }
    #if defined (ARM_MATH_DSP)
    #ifndef UAI_MEM_STATIC
    uai_free(buffer_input);
    #endif
    #endif
#else
#endif
    return ret;
}

int uai_conv_depthwise_sp(uai_tensor_s *input, uai_tensor_s *weight, uint16_t *strides, const unsigned* paddings_front, const unsigned* paddings_back, uai_tensor_s *output)
{
    if((input->dim_height != 0) && (input->dim_width != 0)){
        return uai_conv_depthwise_sp_2d(input, weight, strides, paddings_front, paddings_back, output);
    }

    return -1;
}
#else
#define CONV_SQUARE_PARM        input->buffer,        \
                                input->height,        \
                                input->channels,      \
                                weight->buffer,       \
                                weight->out_channels, \
                                weight->height,       \
                                padding_x,            \
                                strides[0],   \
                                bias,                 \
                                bias->shift,          \
                                output->shift,        \
                                output->buffer,       \
                                output->height,       \
                                buffer_input,         \
                                NULL

#define CONV_NON_SQUARE_PARM    input->buffer,        \
                                input->height,        \
                                input->width,        \
                                input->channels,      \
                                weight->buffer,       \
                                weight->out_channels, \
                                weight->height,       \
                                weight->width,       \
                                padding_x,            \
                                padding_y,            \
                                strides[0],   \
                                strides[1],   \
                                bias,                 \
                                bias->shift,          \
                                output->shift,        \
                                output->buffer,       \
                                output->height,       \
                                output->width,        \
                                buffer_input,         \
                                NULL

int uai_conv_depthwise_sp_2d(uai_input_s *input, uai_weight_s *weight, uint16_t *strides, uai_pad_type_e pad_type, uai_bias_s *bias, uai_output_s *output)
{
    int8_t *buffer_input = NULL;
    uint16_t padding_x   = 0;
    uint16_t padding_y   = 0;
    int16_t  temp        = 0;
    int32_t  ret         = -1;

#if defined(UAI_USE_CMSIS_NN)
    #if defined (ARM_MATH_DSP)
    #ifdef UAI_MEM_STATIC
    buffer_input = UAI_CONV_TEMP_MEM;
    #else
    buffer_input = uai_malloc(2 * input->channels * weight->height * weight->width);
    #endif
    #endif
    if(pad_type == UAI_PAD_SAME) {
        output->height = (input->height + input->height % strides[0]) / strides[0] ;
        output->width  = (input->width  + input->width  % strides[1])  / strides[1] ;
        temp = (output->height - 1) * strides[0] + weight->height - input->height;
        padding_x = (uint16_t)UAI_MAX(temp/2, 0);

        temp = (output->width  - 1) * strides[1] + weight->width  - input->width;
        padding_y = (uint16_t)UAI_MAX(temp/2, 0);
    } else {
        output->height = (input->height - weight->height) / strides[0];
        output->width  = (input->width  - weight->width)  / strides[1];
    }

    if(input->height == input->width) {
        ret = arm_depthwise_separable_conv_HWC_q7(CONV_SQUARE_PARM);
    } else {
        ret = arm_depthwise_separable_conv_HWC_q7_nonsquare(CONV_NON_SQUARE_PARM);
    }
    #if defined (ARM_MATH_DSP)
    #ifndef UAI_MEM_STATIC
    uai_free(buffer_input);
    #endif
    #endif
#else
#endif
    return ret;
}

int uai_conv_depthwise_sp(uai_input_s *input, uai_weight_s *weight, uint16_t *strides, uai_pad_type_e pad_type, uai_bias_s *bias, uai_output_s *output)
{
    if((input->height != 0) && (input->width != 0) && (input->depth == 0)){
        return uai_conv_depthwise_sp_2d(input, weight, strides, pad_type, bias, output);
    }

    return -1;
}
#endif
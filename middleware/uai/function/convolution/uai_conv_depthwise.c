/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include "uai_function.h"
#ifdef UAI_USE_CMSIS_NN
#include "arm_nnfunctions.h"
#endif

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
/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include "uai_function.h"
#ifdef UAI_USE_CMSIS_NN
#include "arm_math.h"
#include "arm_nnfunctions.h"
#endif

#ifdef UAI_ODLA_SUPPORT
#ifdef UAI_USE_CMSIS_NN
#define CONV_SQUARE_PARM        input->buffer,              \
                                input->dim_height,          \
                                input->dim_channels,        \
                                kernel->buffer,             \
                                kernel->dim_kel_out_ch,     \
                                kernel->dim_kel_height,     \
                                padding_x,                  \
                                strides[0],                 \
                                (const int32_t *)kernel_scale->scale, \
                                (int32_t *)output->buffer,  \
                                output->dim_height,         \
                                buffer_input,               \
                                NULL

#define CONV_NON_SQUARE_PARM    input->buffer,              \
                                input->dim_height,          \
                                input->dim_width,           \
                                input->dim_channels,        \
                                kernel->buffer,             \
                                kernel->dim_kel_out_ch,     \
                                kernel->dim_height,         \
                                kernel->dim_width,          \
                                padding_x,                  \
                                padding_y,                  \
                                strides[0],                 \
                                strides[1],                 \
                                kernel_scale->scale,        \
                                (int32_t *)output->buffer,  \
                                output->dim_height,         \
                                output->dim_width,          \
                                buffer_input,               \
                                NULL
extern arm_status arm_convolve_HWC_q7_basic_uai(const q7_t * Im_in,
                          const uint16_t dim_im_in,
                          const uint16_t ch_im_in,
                          const q7_t * wt,
                          const uint16_t ch_im_out,
                          const uint16_t dim_kernel,
                          const uint16_t padding,
                          const uint16_t stride,
                          const int32_t *scale,
                          q31_t * Im_out,
                          const uint16_t dim_im_out,
                          q15_t * bufferA,
                          q7_t * bufferB);
extern arm_status arm_convolve_HWC_q7_fast_uai(const q7_t * Im_in,
                         const uint16_t dim_im_in,
                         const uint16_t ch_im_in,
                         const q7_t * wt,
                         const uint16_t ch_im_out,
                         const uint16_t dim_kernel,
                         const uint16_t padding,
                         const uint16_t stride,
                         const int32_t *scale,
                         q31_t * Im_out,
                         const uint16_t dim_im_out,
                         q15_t * bufferA,
                         q7_t * bufferB);

extern arm_status arm_convolve_HWC_q7_RGB_uai(const q7_t * Im_in,
                        const uint16_t dim_im_in,
                        const uint16_t ch_im_in,
                        const q7_t * wt,
                        const uint16_t ch_im_out,
                        const uint16_t dim_kernel,
                        const uint16_t padding,
                        const uint16_t stride,
                        const int32_t *scale,
                        q31_t * Im_out,
                        const uint16_t dim_im_out,
                        q15_t * bufferA, q7_t * bufferB);

extern arm_status arm_convolve_HWC_q7_basic_nonsquare_uai(const q7_t * Im_in,
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
                                               const int32_t *scale,
                                               q31_t * Im_out,
                                               const uint16_t dim_im_out_x,
                                               const uint16_t dim_im_out_y,
                                               q15_t * bufferA,
                                               q7_t * bufferB);
extern arm_status arm_convolve_HWC_q7_fast_nonsquare_uai(const q7_t * Im_in,
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
                                              const int32_t *scale,
                                              q31_t * Im_out,
                                              const uint16_t dim_im_out_x,
                                              const uint16_t dim_im_out_y,
                                              q15_t * bufferA,
                                              q7_t * bufferB);
extern arm_status arm_convolve_1x1_HWC_q7_fast_nonsquare_uai(const q7_t * Im_in,
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
                                                  const int32_t *scale,
                                                  q7_t * Im_out,
                                                  const uint16_t dim_im_out_x,
                                                  const uint16_t dim_im_out_y,
                                                  q15_t * bufferA,
                                                  q7_t * bufferB);
#endif
int uai_conv_2d(uai_tensor_s *input, uai_tensor_s *kernel, uint16_t *strides,
                const unsigned* paddings_front, const unsigned* paddings_back,
                uai_quant_scale *kernel_scale,
                uai_tensor_s *output)
{
    int8_t *buffer_input = NULL;
    uint16_t padding_x   = (paddings_front[0] + paddings_back[0])/2;
    uint16_t padding_y   = (paddings_front[1] + paddings_back[1])/2;
    int32_t  ret         = -1;

#if defined(UAI_USE_CMSIS_NN)
    arm_status arm_ret = ARM_MATH_SUCCESS;
    if(input->dim_batch != 1) {
        return UAI_FAIL;
    }

    #if defined (ARM_MATH_DSP)
    #ifdef UAI_MEM_STATIC
    buffer_input = UAI_CONV_TEMP_MEM;
    #else
    buffer_input = uai_malloc(sizeof(uint16_t) * input->size);
    #endif
    #endif

    if(input->dim_height == input->dim_width) {
        if((input->dim_channels % 4 == 0)&&(output->dim_channels % 2 == 0)) {
            arm_ret = arm_convolve_HWC_q7_fast_uai(CONV_SQUARE_PARM);
        } else if (input->dim_channels % 3 == 0) {
            arm_ret = arm_convolve_HWC_q7_RGB_uai(CONV_SQUARE_PARM);
        } else {
            arm_ret = arm_convolve_HWC_q7_basic_uai(CONV_SQUARE_PARM);
        }
    } else {
        if((input->dim_channels % 4 == 0)&&(output->dim_channels % 2 == 0)) {
            if((kernel->dim_kel_height == 1) && (kernel->dim_kel_width == 1) &&
                (strides[0] == 1) && (strides[1] == 1) &&
                (padding_x == 0) && (padding_y == 0)){
                    arm_ret = (int32_t)arm_convolve_1x1_HWC_q7_fast_nonsquare_uai(CONV_NON_SQUARE_PARM);
                } else {
                    arm_ret = (int32_t)arm_convolve_HWC_q7_fast_nonsquare_uai(CONV_NON_SQUARE_PARM);
                }
        }
        else {
            arm_ret = (int32_t)arm_convolve_HWC_q7_basic_nonsquare_uai(CONV_NON_SQUARE_PARM);
        }
    }
    #if defined (ARM_MATH_DSP)
    #ifndef UAI_MEM_STATIC
    uai_free(buffer_input);
    #endif
    #endif

    ret = (arm_ret == ARM_MATH_SUCCESS) ? UAI_SUCCESS : UAI_FAIL;
#else
#endif
    return ret;
}

int uai_conv(uai_tensor_s *input, uai_tensor_s *kernel, uint16_t *strides,
            const unsigned* paddings_front, const unsigned* paddings_back,
            uai_quant_scale *kernel_scale, uai_tensor_s *output)
{
    if((input->dim_height != 0) && (input->dim_width != 0)){
        return uai_conv_2d(input, kernel, strides, paddings_front, paddings_back, kernel_scale, output);
    }

    return UAI_FAIL;
}
#else
#define CONV_SQUARE_PARM        input->buffer,        \
                                input->height,        \
                                input->channels,      \
                                weight->buffer,       \
                                weight->out_channels, \
                                weight->height,       \
                                padding_x,            \
                                strides[0],           \
                                bias->buffer,         \
                                bias->shift,          \
                                output->shift,        \
                                output->buffer,       \
                                output->height,       \
                                buffer_input,         \
                                NULL

#define CONV_NON_SQUARE_PARM    input->buffer,        \
                                input->height,        \
                                input->width,         \
                                input->channels,      \
                                weight->buffer,       \
                                weight->out_channels, \
                                weight->height,       \
                                weight->width,        \
                                padding_x,            \
                                padding_y,            \
                                strides[0],           \
                                strides[1],           \
                                bias->buffer,         \
                                bias->shift,          \
                                output->shift,        \
                                output->buffer,       \
                                output->height,       \
                                output->width,        \
                                buffer_input,         \
                                NULL
int uai_conv_2d(uai_input_s *input, uai_weight_s *weight, uint16_t *strides, uai_pad_type_e pad_type, uai_bias_s *bias, uai_output_s *output)
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
    buffer_input = uai_malloc(sizeof(uint16_t) * input->size);
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
        if((input->channels % 4 == 0)&&(weight->out_channels % 2 == 0)) {
            ret = arm_convolve_HWC_q7_fast(CONV_SQUARE_PARM);
        } else if (input->channels % 3 == 0) {
            ret = arm_convolve_HWC_q7_RGB(CONV_SQUARE_PARM);
        } else {
            ret = arm_convolve_HWC_q7_basic(CONV_SQUARE_PARM);
        }
    } else {
        if((input->channels % 4 == 0)&&(weight->out_channels % 2 == 0)) {
            if((weight->height == 1) && (weight->width == 1) &&
                (strides[0] == 1) && (strides[1] == 1) &&
                (padding_x == 0) && (padding_y == 0)){
                    ret = arm_convolve_1x1_HWC_q7_fast_nonsquare(CONV_NON_SQUARE_PARM);
                } else {
                    ret = arm_convolve_HWC_q7_fast_nonsquare(CONV_NON_SQUARE_PARM);
                }
        }
        else {
            ret = arm_convolve_HWC_q7_basic_nonsquare(CONV_NON_SQUARE_PARM);
        }
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

int uai_conv(uai_input_s *input, uai_weight_s *weight, uint16_t *strides, uai_pad_type_e pad_type, uai_bias_s *bias, uai_output_s *output)
{
    if((input->height != 0) && (input->width != 0) && (input->depth == 0)){
        return uai_conv_2d(input, weight, strides, pad_type, bias, output);
    }

    return -1;
}
#endif
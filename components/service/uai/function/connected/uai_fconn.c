/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include "uai_fconn.h"
#ifdef UAI_USE_CMSIS_NN
#include "arm_nnfunctions.h"
#endif

#ifdef UAI_ODLA_SUPPORT
#ifdef UAI_USE_CMSIS_NN
extern arm_status
arm_fully_connected_q7_uai(const q7_t * pV,
                       const q7_t * pM,
                       const uint16_t dim_vec,
                       const uint16_t num_of_rows,
                       const int32_t *scale,
                       q31_t * pOut, q15_t * vec_buffer);
#endif
int uai_fconn(uai_tensor_s *input, uai_tensor_s *weight, uai_quant_scale *kernel_scale, uai_tensor_s *output)
{
    int16_t dtype = 0;
    int16_t *vec_buffer;
    int32_t ret = 0;

    dtype = ((input->dtype)<<4) + output->dtype;

#if defined(UAI_USE_CMSIS_NN)
    #if defined (ARM_MATH_DSP)
    #ifdef UAI_MEM_STATIC
    vec_buffer = UAI_FCONN_TEMP_MEM;
    #else
    vec_buffer = uai_malloc(sizeof(int16_t) * input->size);
    #endif
    #endif
    switch(dtype) {
        case 0x11:
            ret = arm_fully_connected_q7_uai(input->buffer, weight->buffer, input->size, weight->dims.dims[UAI_DIM_HEIGHT], kernel_scale->scale, (int32_t *)output->buffer, vec_buffer);
            break;

        default:
            break;
    }

    #if defined (ARM_MATH_DSP)
    #ifndef UAI_MEM_STATIC
    uai_free(vec_buffer);
    #endif
    #endif
    return ret;
#elif defined(UAI_USE_HARDWARE_NN)
    return uai_fully_connected_hal(input, weight, output);
#else
    /* TODO: This version does not support */
    return uai_fully_connected_software(input, weight, output);
#endif
}
#else
int uai_fconn(uai_input_s *input, uai_weight_s *weight, uai_bias_s *bias, uai_output_s *output)
{
    int16_t dtype = 0;
    int16_t *vec_buffer;
    int32_t ret = 0;

    dtype = ((input->dtype)<<4) + output->dtype;

#if defined(UAI_USE_CMSIS_NN)
    #if defined (ARM_MATH_DSP)
    #ifdef UAI_MEM_STATIC
    vec_buffer = UAI_FCONN_TEMP_MEM;
    #else
    vec_buffer = uai_malloc(sizeof(int16_t) * input->size);
    #endif
    #endif
    switch(dtype) {
        case 0x11:
            ret = arm_fully_connected_q7(input->buffer, weight->buffer, input->size, weight->height, bias->shift, output->shift, bias->buffer, output->buffer, vec_buffer);
            break;
        case 0x22:
            ret = arm_fully_connected_q15(input->buffer, weight->buffer, input->size, weight->height, bias->shift, output->shift, bias->buffer, output->buffer, vec_buffer);
            break;
        case 0x12:
            ret = arm_fully_connected_mat_q7_vec_q15(input->buffer, weight->buffer, input->size, weight->height, bias->shift, output->shift, bias->buffer, output->buffer, vec_buffer);
            break;
        default:
            break;
    }

    #if defined (ARM_MATH_DSP)
    #ifndef UAI_MEM_STATIC
    uai_free(vec_buffer);
    #endif
    #endif
    return ret;
#elif defined(UAI_USE_HARDWARE_NN)
    return uai_fully_connected_hal(input, weight, bias, output);
#else
    /* TODO: This version does not support */
    return uai_fully_connected_software(input, weight, bias, output);
#endif
}

int uai_fconn_opt(uai_input_s *input, uai_weight_s *weight, uai_bias_s *bias, uai_output_s *output)
{
    uint16_t dtype = 0;
    int16_t *vec_buffer;
    int32_t ret = 0;

    dtype = ((input->dtype)<<4) | output->dtype;

#if defined(UAI_USE_CMSIS_NN)
    #if defined (ARM_MATH_DSP)
    #ifdef UAI_MEM_STATIC
    vec_buffer = UAI_FCONN_TEMP_MEM;
    #else
    vec_buffer = uai_malloc(sizeof(int16_t) * input->size);
    #endif
    #endif
    switch(dtype) {
        case 0x11:
            ret =  arm_fully_connected_q7_opt(input->buffer, weight->buffer, input->size, weight->height, bias->shift, output->shift, bias->buffer, output->buffer, vec_buffer);
            break;
        case 0x22:
            ret = arm_fully_connected_q15_opt(input->buffer, weight->buffer, input->size, weight->height, bias->shift, output->shift, bias->buffer, output->buffer, vec_buffer);
            break;
        case 0x12:
            ret = arm_fully_connected_mat_q7_vec_q15_opt(input->buffer, weight->buffer, input->size, weight->height, bias->shift, output->shift, bias->buffer, output->buffer, vec_buffer);
            break;
        default:
            break;
    }
    #if defined (ARM_MATH_DSP)
    #ifndef UAI_MEM_STATIC
    uai_free(vec_buffer);
    #endif
    #endif
    return ret;
#elif defined(UAI_USE_HARDWARE_NN)
    return uai_fully_connected_hal_opt(input, weight, bias, output);
#else
    /* TODO: This version does not support */
    return uai_fully_connected_opt_software(input, weight, bias, output);
#endif
}
#endif

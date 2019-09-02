/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef UAI_USE_CMSIS_NN
#include "arm_nnfunctions.h"
#endif

int uai_fully_connected(const int8_t  *input_buffer,
                        const int8_t  *weights,
                        const uint16_t input_size,
                        const uint16_t weight_rows,
                        const uint16_t bias_shift,
                        const uint16_t out_shift,
                        const int8_t  *bias,
                        int8_t        *out_buffer,
                        int16_t       *vec_buffer)
{
#if defined(UAI_USE_CMSIS_NN)
    return arm_fully_connected_q7(input_buffer, weights, input_size, weight_rows, bias_shift, out_shift, bias, out_buffer, vec_buffer);
#elif defined(UAI_USE_HARDWARE_NN)
    return uai_fully_connected_hal(input_buffer, weights, input_size, weight_rows, bias_shift, out_shift, bias, out_buffer, vec_buffer);
#else
    /* TODO: This version does not support */
    return uai_fully_connected_software(input_buffer, weights, input_size, weight_rows, bias_shift, out_shift, bias, out_buffer, vec_buffer);
#endif
}

int uai_fully_connected_opt(const int8_t  *input_buffer,
                            const int8_t  *weights,
                            const uint16_t input_size,
                            const uint16_t weight_rows,
                            const uint16_t bias_shift,
                            const uint16_t out_shift,
                            const int8_t   *bias,
                            int8_t         *out_buffer,
                            int16_t        *vec_buffer)
{
#if defined(UAI_USE_CMSIS_NN)
    return arm_fully_connected_q7_opt(input_buffer, weights, input_size, weight_rows, bias_shift, out_shift, bias, out_buffer, vec_buffer);
#elif defined(UAI_USE_HARDWARE_NN)
    return uai_fully_connected_opt_hal(input_buffer, weights, input_size, weight_rows, bias_shift, out_shift, bias, out_buffer, vec_buffer);
#else
    /* TODO: This version does not support */
    return uai_fully_connected_opt_software(input_buffer, weights, input_size, weight_rows, bias_shift, out_shift, bias, out_buffer, vec_buffer);
#endif
}
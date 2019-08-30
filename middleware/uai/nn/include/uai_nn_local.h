/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_NN_LOCAL_H
#define UAI_NN_LOCAL_H

#ifdef __cplusplus
extern "C" {
#endif


typedef int (*uai_fconn_func)(const int8_t  *inut_buffer,
                                const int8_t  *weights,
                                const uint16_t input_size,
                                const uint16_t weight_rows,
                                const uint16_t bias_shift,
                                const uint16_t out_shift,
                                const int8_t   *bias,
                                int8_t         *out_buffer, 
                                int16_t        *vec_buffer);

typedef void (*uai_act_func)(int8_t * data, uint16_t size, uint16_t int_width);

typedef void (*uai_softmax_func)(const int8_t *in_vec, const uint16_t dim_vec, int8_t *out_vec);

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_NN_LOCAL_H */

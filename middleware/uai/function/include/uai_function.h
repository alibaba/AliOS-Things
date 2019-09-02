/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAI_FUNCTION_H
#define UAI_FUNCTION_H

#ifdef __cplusplus
extern "C" {
#endif

void uai_relu(int8_t *data, uint16_t size, uint16_t out_width);
void uai_relu6(int8_t *data, uint16_t size, uint16_t out_width);
void uai_sigmod(int8_t *data, uint16_t size, uint16_t out_width);
void uai_tanh(int8_t *data, uint16_t size, uint16_t out_width);

int uai_fully_connected(const int8_t  *input_buffer,
                        const int8_t  *weights,
                        const uint16_t input_size,
                        const uint16_t weight_rows,
                        const uint16_t bias_shift,
                        const uint16_t out_shift,
                        const int8_t  *bias,
                        int8_t        *out_buffer,
                        int16_t       *vec_buffer);

int uai_fully_connected_opt(const int8_t  *input_buffer,
                            const int8_t  *weights,
                            const uint16_t input_size,
                            const uint16_t weight_rows,
                            const uint16_t bias_shift,
                            const uint16_t out_shift,
                            const int8_t   *bias,
                            int8_t         *out_buffer,
                            int16_t        *vec_buffer);

void uai_softmax(const int8_t *in_vec, const uint16_t dim_vec, int8_t *out_vec);

#ifdef __cplusplus
}
#endif

#endif  /* end of UAI_FUNCTION_H */

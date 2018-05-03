#ifndef __NDS32_UTILS_MATH_H__
#define __NDS32_UTILS_MATH_H__
/***************************************************************************
 * nds32_utils_math.h                                                      *
 *                                                                         *
 * Copyright (C) 2012-2017 Andes Technology Corporation                    *
 * All rights reserved.                                                    *
 ***************************************************************************/

/**
 * @defgroup utils Utils Functions
 * This set of functions implements sine, cosine, arctanm, and square root.
 * There are separate functions for Q15, Q31, and floating-point data.
 */
#ifdef  __cplusplus
extern "C"
{
#endif

#include "nds32_math_types.h"

float32_t nds32_cos_f32(float32_t src);
q31_t nds32_cos_q31(q31_t src);
q15_t nds32_cos_q15(q15_t src);

float32_t nds32_sin_f32(float32_t src);
q31_t nds32_sin_q31(q31_t src);
q15_t nds32_sin_q15(q15_t src);

float32_t nds32_atan_f32(float32_t src);
q31_t nds32_atan_q31(q31_t src);
q15_t nds32_atan_q15(q15_t src);
float32_t nds32_atan2_f32(float32_t srcy, float32_t src2);
q15_t nds32_atan2_q15(q15_t srcy, q15_t src2);
q31_t nds32_atan2_q31(q31_t srcy, q31_t src2);

// Square Root
/**
 * @brief Square root of the floating-potint input.
 * @param[in]       src the input value.
 * @return the suqare root of input.
 */
float32_t nds32_sqrt_f32(float32_t src);

/**
 * @brief Square root of the q31 input.
 * @param[in]       src the input value.
 * @return the suqare root of input.
 */
q31_t nds32_sqrt_q31(q31_t src);

/**
 * @brief Square root of the q15 input.
 * @param[in]       src the input value.
 * @return the suqare root of input.
 */
q15_t nds32_sqrt_q15(q15_t src);

// Convert function
/**
 * @brief Convert a floating-point vector to Q15.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst yhe output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
void nds32_convert_f32_q15(float32_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Convert a floating-point vector to Q31.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vectors.
 * @return none.
 */
void nds32_convert_f32_q31(float32_t *src, q31_t *dst, uint32_t size);

/**
 * @brief Convert a floating-point vector to Q7.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vectors.
 * @return none.
 */
void nds32_convert_f32_q7(float32_t *src, q7_t *dst, uint32_t size);

/**
 * @brief Convert a Q15 vector to floating.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
void nds32_convert_q15_f32(q15_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Convert a Q15 vector to Q31.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
void nds32_convert_q15_q31(q15_t *src, q31_t *dst, uint32_t size);

/**
 * @brief Convert a Q15 vector to Q7.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
void nds32_convert_q15_q7(q15_t *src, q7_t *dst, uint32_t size);

/**
 * @brief Convert a Q31 vector to floating.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
void nds32_convert_q31_f32(q31_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Convert a Q31 vector to Q15.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
void nds32_convert_q31_q15(q31_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Convert a Q31 vector to Q7.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
void nds32_convert_q31_q7(q31_t *src, q7_t *dst, uint32_t size);

/**
 * @brief Convert a Q7 vector to floating.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
void nds32_convert_q7_f32(q7_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Convert a Q7 vector to Q15.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
void nds32_convert_q7_q15(q7_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Convert a Q7 vector to Q31.
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vector.
 * @return none.
 */
void nds32_convert_q7_q31(q7_t *src, q31_t *dst, uint32_t size);

/**
 * @brief Duplicate the floating vector
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vectors.
 * @return none.
 */
void nds32_dup_f32(float32_t *src, float32_t *dst, uint32_t size);

/**
 * @brief Duplicate the Q15 vector
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vectors.
 * @return none.
 */
void nds32_dup_q15(q15_t *src, q15_t *dst, uint32_t size);

/**
 * @brief Duplicate the Q31 vector
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vectors.
 * @return none.
 */
void nds32_dup_q31(q31_t *src, q31_t *dst, uint32_t size);

/**
 * @brief Duplicate the Q7 vector
 * @param[in]       *src the input vector point.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of vectors.
 * @return none.
 */
void nds32_dup_q7(q7_t *src, q7_t *dst, uint32_t size);

/**
 * @brief Set the floating-point vector.
 * @param[in]       val specify floating-point value.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of the vector.
 * @return none.
 */
void nds32_set_f32(float32_t val, float32_t *dst, uint32_t size);

/**
 * @brief Set the Q15 vector.
 * @param[in]       val specify Q15 value.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of the vector.
 * @return none.
 */
void nds32_set_q15(q15_t val, q15_t *dst, uint32_t size);

/**
 * @brief Set the Q31 vector.
 * @param[in]       val specify Q31 value.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of the vector.
 * @return none.
 */
void nds32_set_q31(q31_t val, q31_t *dst, uint32_t size);

/**
 * @brief Set the Q7 vector.
 * @param[in]       val specify Q7 value.
 * @param[out]      *dst the output vector point.
 * @param[in]       size size of the vector.
 * @return none.
 */
void nds32_set_q7(q7_t val, q7_t *dst, uint32_t size);

#ifdef  __cplusplus
}
#endif
#endif // __NDS32_UTILS_MATH_H__

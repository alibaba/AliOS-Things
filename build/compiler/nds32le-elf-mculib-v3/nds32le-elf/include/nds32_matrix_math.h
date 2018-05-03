#ifndef __NDS32_MATRIX_MATH_H__
#define __NDS32_MATRIX_MATH_H__
/***************************************************************************
 * nds32_matrix_math.h                                                     *
 *                                                                         *
 * Copyright (C) 2012-2017 Andes Technology Corporation                    *
 * All rights reserved.                                                    *
 ***************************************************************************/

/**
 * @defgroup matrix Matrix Functions
 *
 * This set of functions provides basic matrix math operations.
 * The funciotn specifies the size of the matrix and then points to an array.
 * For example,
 * the function definition for the floating-point is shown below:
 * <pre>
 *     void nds32_funcname_f32(const float32_t *src1,
 *                             const float32_t *src2,
 *                                   float32_t *dst,
 *                                   uint32_t row,
 *                                   uint32_t col,
 *                                   uint32_t row2,
 *                                   uint32_t col2)
 * </pre>
 * where it can be transform to the two matrix. For the matrix 1 is a
 * <code>row * col</code> matrix and the matrix 2 is a
 * <code>rol2 * col2</code> and the output matrix woild be different since
 * the math operation. There are similar definitions for Q15 and Q31 data types.
 */

#ifdef  __cplusplus
extern "C"
{
#endif

#include "nds32_math_types.h"

// Matrix Addition
/**
 * @brief Addition of two floating-potint matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
void nds32_mat_add_f32(const float32_t *src1, const float32_t *src2,
                       float32_t *dst, uint32_t row, uint32_t col);

/**
 * @brief Addition of two q15 matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 *
 * The output results will be saturated in Q15 range [0x8000 0x7FFF].
 */
void nds32_mat_add_q15(const q15_t *src1, const q15_t *src2,
                       q15_t *dst, uint32_t row, uint32_t col);

/**
 * @brief Addition of two q31 matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 *
 * Ouput results will be saturated in Q31 range [0x80000000 0x7FFFFFFF].
 */
void nds32_mat_add_q31(const q31_t *src1, const q31_t *src2,
                       q31_t *dst, uint32_t row, uint32_t col);

// Matrix Inverse
/**
 * @brief Compute the inverse matrix of the floating-potint matrix.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       size  number of the matrix row or column.
 * @return the inverse process success or not.
 */
int32_t nds32_mat_inv_f32(float32_t *src, float32_t *dst, uint32_t size);
int32_t nds32_mat_inv_f64(float64_t *src, float64_t *dst, uint32_t size);

// Matrix Multiplication
/**
 * @brief Multiplication of two floating-point matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the first input matrix rows.
 * @param[in]       col   number of the first input matrix columns.
 * @param[in]       col2  number of the second input matrix columns.
 * @return none.
 */
void nds32_mat_mul_f32(const float32_t *src1, const float32_t *src2,
                       float32_t *dst, uint32_t row, uint32_t col, uint32_t col2);

void nds32_mat_mul_f64(const float64_t *src1, const float64_t *src2,
                       float64_t *dst, uint32_t row, uint32_t col, uint32_t col2);
int32_t nds32_mat_pow2_cache_f64(const float64_t *src, float64_t *dst, uint32_t size);
#define nds32_mat_pwr2_cache_f64 nds32_mat_pow2_cache_f64
// The input is a square matrix for nds32_mat_pow2_cache_f64.

/**
 * @brief Multiplication of two q15 matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the first input matrix rows.
 * @param[in]       col   number of the first input matrix columns.
 * @param[in]       col2  number of the second input matrix columns.
 * @return none.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format input is multiplied yields a 2.30 format, and then added
 * without saturation to a 64-bit accumulator in 34.30 format. Finally,
 * the added output is truncated to 34.15 format by discarding the lower 15
 * bits, and then saturated to yield a result in 1.15 format.
 */
void nds32_mat_mul_q15(const q15_t *src1, const q15_t *src2,
                       q15_t *dst, uint32_t row, uint32_t col, uint32_t col2);

/**
 * @brief Multiplication of two q31 matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the first input matrix rows.
 * @param[in]       col   number of the first input matrix columns.
 * @param[in]       col2  number of the second input matrix columns.
 * @return none.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input is multiplied yields a 2.62 format. In order to
 * avoid overflows, the input signal must be scaled down by
 * <code>log2(col)</code> bits, Finally, the 2.62 accumulator is right
 * shifted by 31 bits to yield a 1.31 format value.
 */
void nds32_mat_mul_q31(const q31_t *src1, const q31_t *src2,
                       q31_t *dst, uint32_t row, uint32_t col, uint32_t col2);

// Matrix Scale
/**
 * @brief Multiplt a scale value of floating-potint matrix.
 * @param[in]       *src points to the input matrix.
 * @param[in]       scale is the factor to be multiplied.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
void nds32_mat_scale_f32(const float32_t *src, float32_t scale,
                         float32_t *dst, uint32_t row, uint32_t col);

/**
 * @brief Multiplt a scale value of q15 matrix.
 * @param[in]       *src      points to the input matrix.
 * @param[in]       scalefract fractional multiplication.
 * @param[in]       shift      arithmetic shift.
 * @param[out]      *dst       points to the output matrix.
 * @param[in]       row        number of the matrix rows.
 * @param[in]       col        number of the matrix columns.
 * @return none.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format inputs are multiplied to yield a 2.30 intermediate result
 * and this is shifted with saturation to 1.15 format.
 */
void nds32_mat_scale_q15(const q15_t *src, q15_t scale_fract, int32_t shift,
                         q15_t *dst, uint32_t row, uint32_t col);

/**
 * @brief Multiplt a scale value of q31 matrix.
 * @param[in]       *src      points to the input matrix.
 * @param[in]       scalefract fractional multiplication.
 * @param[in]       shift      arithmetic shift.
 * @param[out]      *dst       points to the output matrix.
 * @param[in]       row        number of the matrix rows.
 * @param[in]       col        number of the matrix columns.
 * @return none.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input are multiplied to yield a 2.62 intermediate result
 * and this is shifted with saturation to 1.31 format.
 */
void nds32_mat_scale_q31(const q31_t *src, q31_t scale_fract, int32_t shift,
                         q31_t *dst, uint32_t row, uint32_t col);

// Matrix Subtraction
/**
 * @brief Substraction of two floating-potint matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
void nds32_mat_sub_f32(const float32_t *src1, const float32_t *src2,
                       float32_t *dst, uint32_t row, uint32_t col);

/**
 * @brief Substraction of two q15 matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 *
 * The output results will be saturated in Q15 range [0x8000 0x7FFF].
 */
void nds32_mat_sub_q15(const q15_t *src1, const q15_t *src2,
                       q15_t *dst, uint32_t row, uint32_t col);

/**
 * @brief Substraction of two q31 matrices.
 * @param[in]       *src1 points to the first input matrix.
 * @param[in]       *src2 points to the second input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 *
 * Ouput results will be saturated in Q31 range [0x80000000 0x7FFFFFFF].
 */
void nds32_mat_sub_q31(const q31_t *src1, const q31_t *src2,
                       q31_t *dst, uint32_t row, uint32_t col);

// Matrix Transpose
/**
 * @brief Transpose the floating-potint matricex.
 * @param[in]       *src  points to the input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
void nds32_mat_trans_f32(const float32_t *src, float32_t *dst, uint32_t row,
                         uint32_t col);

/**
 * @brief Transpose the q15 matricex.
 * @param[in]       *src  points to the input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
void nds32_mat_trans_q15(const q15_t *src, q15_t *dst, uint32_t row,
                         uint32_t col);

/**
 * @brief Transpose the q31 matricex.
 * @param[in]       *src  points to the input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
void nds32_mat_trans_q31(const q31_t *src, q31_t *dst, uint32_t row,
                         uint32_t col);

// uint8 function
/**
 * @brief Transpose the u8 matricex.
 * @param[in]       *src  points to the input matrix.
 * @param[out]      *dst  points to the output matrix.
 * @param[in]       row   number of the matrix rows.
 * @param[in]       col   number of the matrix columns.
 * @return none.
 */
void nds32_mat_trans_u8(const uint8_t *src, uint8_t *dst, uint32_t row, uint32_t col);

#ifdef  __cplusplus
}
#endif
#endif // __NDS32_MATRIX_MATH_H__

#ifndef __NDS32_STATISTICS_MATH_H__
#define __NDS32_STATISTICS_MATH_H__
/***************************************************************************
 * nds32_statistics_math.h                                                 *
 *                                                                         *
 * Copyright (C) 2012-2017 Andes Technology Corporation                    *
 * All rights reserved.                                                    *
 ***************************************************************************/

/**
 * @defgroup statistics Statistics Functions
 */
#ifdef  __cplusplus
extern "C"
{
#endif

#include "nds32_math_types.h"

// Maximum
/**
 * @brief Maximum value of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the maximum value.
 * @return maximum value.
 */
float32_t nds32_max_f32(const float32_t *src, uint32_t size,
                        uint32_t *index);

/**
 * @brief Maximum value of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the maximum value.
 * @return maximum value.
 */
q15_t nds32_max_q15(const q15_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Maximum value of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the maximum value.
 * @return maximum value.
 */
q31_t nds32_max_q31(const q31_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Maximum value of the q7 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the maximum value.
 * @return maximum value.
 */
q7_t nds32_max_q7(const q7_t *src, uint32_t size, uint32_t *index);

// Minimum
/**
 * @brief Minimum value of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the minimum value.
 * @return minimum value.
 */
float32_t nds32_min_f32(const float32_t *src, uint32_t size,
                        uint32_t *index);

/**
 * @brief Minimum value of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the minimum value.
 * @return minimum value.
 */
q15_t nds32_min_q15(const q15_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Minimum value of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the minimum value.
 * @return minimum value.
 */
q31_t nds32_min_q31(const q31_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Minimum value of the q7 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @param[out]      *index	index of the minimum value.
 * @return minimum value.
 */
q7_t nds32_min_q7(const q7_t *src, uint32_t size, uint32_t *index);

// Mean
/**
 * @brief Mean value of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return mean value.
 */
float32_t nds32_mean_f32(const float32_t *src, uint32_t size);

/**
 * @brief Mean value of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return mean value.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format input is accumulated in a 32-bit accumulator in 17.15
 * format and then truncated to yield a result of 1.15 format.
 */
q15_t nds32_mean_q15(const q15_t *src, uint32_t size);

/**
 * @brief Mean value of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return mean value.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input is accumulated in a 64-bit accumulator in 33.31
 * format and then truncated to yield a result of 1.31 format.
 */
q31_t nds32_mean_q31(const q31_t *src, uint32_t size);

/**
 * @brief Mean value of the q7 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return mean value.
 *
 * <b>Function notes:</b>
 *
 * The 1.7 format input is accumulated in a 32-bit accumulator in 25.7
 * format and then truncated to yield a result of 1.7 format.
 */
q7_t nds32_mean_q7(const q7_t *src, uint32_t size);

// Sun of the Squares
/**
 * @brief Sum of the squares of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Sum of the squares value.
 */
float32_t nds32_sos_f32(const float32_t *src, uint32_t size);
#define nds32_pwr_f32 nds32_sos_f32

/**
 * @brief Sum of the squares of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Sum of the squares value.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format input is multiplied yields a 2.30 format, and then added
 * without saturation to a 64-bit accumulator in 34.30 format. Finally,
 * the return result is in 34.30 format.
 */
q63_t nds32_sos_q15(const q15_t *src, uint32_t size);
#define nds32_pwr_q15 nds32_sos_q15

/**
 * @brief Sum of the squares of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Sum of the squares value.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input is multiplied yields a 2.62 format and this result
 * is truncated to 2.48 format by discarding the lower 14 bits. The 2.48
 * result is then added without saturation to a 64-bit accumulator in 16.48
 * format. Finally, the return result is in 16.48 format.
 */
q63_t nds32_sos_q31(const q31_t *src, uint32_t size);
#define nds32_pwr_q31 nds32_sos_q31

/**
 * @brief Sum of the squares of the q7 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Sum of the squares value.
 *
 * <b>Function notes:</b>
 *
 * The 1.7 format input is multiplied yields a 2.14 format, and then added
 * without saturation to a 32-bit accumulator in 18.14 format. Finally,
 * the return result is in 18.14 format.
 */
q31_t nds32_sos_q7(const q7_t *src, uint32_t size);
#define nds32_pwr_q7 nds32_sos_q7

// Root Mean Square
/**
 * @brief RMS of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return RMS value.
 */
float32_t nds32_rms_f32(const float32_t *src, uint32_t size);

/**
 * @brief RMS of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return RMS value.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format input is multiplied yields a 2.30 format, and then added
 * without saturation to a 64-bit accumulator in 34.30 format. Finally,
 * the added output is truncated to 34.15 format by discarding the lower 15
 * bits, and then saturated to yield a result in 1.15 format.
 */
q15_t nds32_rms_q15(const q15_t *src, uint32_t size);

/**
 * @brief RMS of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return RMS value.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input is multiplied yields a 2.62 format. In order to
 * avoid overflows, the input signal must be scaled down by
 * <code>log2(size)</code> bits, Finally, the 2.62 accumulator is right
 * shifted  by 31 bits to yield a 1.31 format value.
 */
q31_t nds32_rms_q31(const q31_t *src, uint32_t size);

// Standard deviation
/**
 * @brief Standard deviation of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Standard deviation value.
 */
float32_t nds32_std_f32(const float32_t *src, uint32_t size);

/**
 * @brief Standard deviation of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Standard deviation value.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format input is multiplied yields a 2.30 format, and then added
 * without saturation to a 64-bit accumulator in 34.30 format. Finally,
 * the added output is truncated to 34.15 format by discarding the lower 15
 * bits, and then saturated to yield a result in 1.15 format.
 */
q15_t nds32_std_q15(const q15_t *src, uint32_t size);

/**
 * @brief Standard deviation of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Standard deviation value.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input is multiplied yields a 2.62 format. In order to
 * avoid overflows, the input signal must be scaled down by
 * <code>log2(size)</code> bits, Finally, the 2.62 accumulator is right
 * shifted  by 31 bits to yield a 1.31 format value.
 */
q31_t nds32_std_q31(const q31_t *src, uint32_t size);

// Variance
/**
 * @brief Variance of the floating-potint vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Variance value.
 */
float32_t nds32_var_f32(const float32_t *src, uint32_t size);

/**
 * @brief Variance of the q15 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Variance value.
 *
 * <b>Function notes:</b>
 *
 * The 1.15 format input is multiplied yields a 2.30 format, and then added
 * without saturation to a 64-bit accumulator in 34.30 format. Finally,
 * the added output is truncated to 34.15 format by discarding the lower 15
 * bits, and then saturated to yield a result in 1.15 format.
 */
q31_t nds32_var_q15(const q15_t *src, uint32_t size);

/**
 * @brief Variance of the q31 vector.
 * @param[in]       *src	points to the input vector.
 * @param[in]       size	size of the vectors.
 * @return Variance value.
 *
 * <b>Function notes:</b>
 *
 * The 1.31 format input is multiplied yields a 2.62 format. In order to
 * avoid overflows, the input signal must be scaled down by
 * <code>log2(size)</code> bits, Finally, the 2.62 accumulator is right
 * shifted  by 31 bits to yield a 1.31 format value.
 */
q63_t nds32_var_q31(const q31_t *src, uint32_t size);

// uint8 function
/**
 * @brief Max value of the u8 vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @param[out]      *index  index of the maximum value.
 * @return max value.
 */
uint8_t nds32_max_u8(const uint8_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Minimum value of the u8 vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @param[out]      *index  index of the minimum value.
 * @return minimum value.
 */
uint8_t nds32_min_u8(const uint8_t *src, uint32_t size, uint32_t *index);

/**
 * @brief Mean value of the u8 vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @return mean value.
 *
 * The 8-bit format input is accumulated in a 32-bit accumulator
 * and then truncated to yield a result of 8-bit format.
 */
uint8_t nds32_mean_u8(const uint8_t *src, uint32_t size);

/**
 * @brief Standard deviation of the u8 vector.
 * @param[in]       *src    points to the input vector.
 * @param[in]       size    size of the vectors.
 * @return Standard deviation value.
 *
 * <b>Function notes:</b>
 * The 8-bit format input is multiplied yields a 16-bit format, and then added
 * saturation to a 32-bit accumulator in 16.16 format. Finally,
 * the added output is truncated to 34.15 format by discarding the lower 1
 * bits, and then saturated to yield a result in 1.15 format.
 */
q15_t nds32_std_u8(const uint8_t *src, uint32_t size);

#ifdef  __cplusplus
}
#endif
#endif // __NDS32_STATISTICS_MATH_H__

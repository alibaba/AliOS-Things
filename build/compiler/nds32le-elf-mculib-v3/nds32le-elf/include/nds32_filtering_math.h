#ifndef __NDS32_FILTERING_MATH_H__
#define __NDS32_FILTERING_MATH_H__
/***************************************************************************
 * nds32_filtering_math.h                                                  *
 *                                                                         *
 * Copyright (C) 2012-2017 Andes Technology Corporation                    *
 * All rights reserved.                                                    *
 ***************************************************************************/

/**
 * @defgroup filtering Filtering Functions
 */


#ifdef  __cplusplus
extern "C"
{
#endif

#include "nds32_math_types.h"
#include "nds32_utils_math.h"

/**
 * Standard FIR filter
 */
/**
 * @brief Structure for the floating-point standard FIR filter.
 */
typedef struct
{
    uint32_t coeff_size;    // number of filter coefficients.
    float32_t *state;       // points to the state variable array of size <code>coeff_size + size - 1</code>.
    float32_t *coeff;       // points to the coefficient array of size <code>coeff_size</code>. Ex: {b[coeff_size-1], b[coeff_size-2], ..., b[1], b[0]}
} nds32_fir_f32_t;

/**
 * @brief Structure for the Q31 standard FIR filter.
 */
typedef struct
{
    uint32_t coeff_size;    // number of filter coefficients.
    q31_t *state;           // points to the state variable array of size <code>coeff_size + size - 1</code>.
    q31_t *coeff;           // points to the coefficient array of size <code>coeff_size</code>. Ex: {b[coeff_size-1], b[coeff_size-2], ..., b[1], b[0]}
} nds32_fir_q31_t;

/**
 * @brief Structure for the Q15 standard FIR filter.
 */
typedef struct
{
    uint32_t coeff_size;    // number of filter coefficients.
    q15_t *state;           // points to the state variable array of size <code>coeff_size + size - 1</code>.
    q15_t *coeff;           // points to the coefficient array of size <code>coeff_size</code>. Ex: {b[coeff_size-1], b[coeff_size-2], ..., b[1], b[0]}
} nds32_fir_q15_t;

/**
 * @brief Structure for the Q7 standard FIR Filters.
 */
typedef struct
{
    uint32_t coeff_size;    // number of filter coefficients.
    q7_t *state;            // points to the state variable array of size <code>coeff_size + size - 1</code>.
    q7_t *coeff;            // points to the coefficient array of size <code>coeff_size</code>. Ex: {b[coeff_size-1], b[coeff_size-2], ..., b[1], b[0]}
} nds32_fir_q7_t;

/**
 * @brief Function for the floating-point FIR filter.
 * @param[in]       *instance points to an instance of the FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 */
void nds32_fir_f32(const nds32_fir_f32_t *instance, float32_t *src,
                   float32_t *dst, uint32_t size);

/**
 * @brief Function for the q31 FIR filter.
 * @param[in]       *instance points to an instance of the FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * Both coefficients and state variables are represented in 1.31 format
 * and multiplications yield a 2.62 result. The 2.62 results are accumulated
 * in a 64-bit accumulator and is right shifted by 31 bits and saturated to
 * 1.31 formatthe to yield the final result. In order to avoid overflows
 * completely the input signal must be scaled down by log2(coeff_size) bits.
 */
void nds32_fir_q31(const nds32_fir_q31_t *instance, q31_t *src,
                   q31_t *dst, uint32_t size);

/**
 * @brief Function for the q15 FIR filter.
 * @param[in]       *instance points to an instance of the FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * Both coefficients and state variables are represented in 1.15 format
 * and multiplications yield a 2.30 result. The 2.30 results are accumulated
 * in a 64-bit accumulator in 34.30 format and the results is truncated
 * to 34.15 format by discarding low 15 bits. Lastly, the outputs is
 * saturated to yield a result in 1.15 format.
 */
void nds32_fir_q15(const nds32_fir_q15_t *instance, q15_t *src,
                   q15_t *dst, uint32_t size);

/**
 * @brief Function for the q31 FIR filter.
 * @param[in]       *instance points to an instance of the FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.7 format and multiplications yield a 2.14 result.
 * The 2.14 intermediate results are accumulated in a 32-bit accumulator in
 * 18.14 format. The 18.14 result is then converted to 18.7 format by
 * discarding the low 7 bits and then saturated to 1.7 format.
 */
void nds32_fir_q7(const nds32_fir_q7_t *instance, q7_t *src, q7_t *dst,
                  uint32_t size);

/**
 * Lattice FIR filter
 */
/**
 * @brief Structure for the f32 Lattice FIR filter.
 */
typedef struct
{
    uint32_t stage;    // number of filter stages.
    float32_t *state;  // points to the state variable array of size <code>stage + size - 1</code>.
    float32_t *coeff;  // points to the coefficient array of size <code>stage</code>. Ex: {b[stage-1], b[stage-2], ..., b[1], b[0]}
} nds32_lfir_f32_t;

/**
 * @brief Structure for the q15 Lattice FIR filter.
 */
typedef struct
{
    uint32_t stage;    // number of filter stages.
    q15_t *state;      // points to the state variable array of size <code>stage + size - 1</code>.
    q15_t *coeff;      // points to the coefficient array of size <code>stage</code>. Ex: {b[stage-1], b[stage-2], ..., b[1], b[0]}
} nds32_lfir_q15_t;

/**
 * @brief Structure for the q31 Lattice FIR filter.
 */
typedef struct
{
    uint32_t stage;    // number of filter stages.
    q31_t *state;      // points to the state variable array of size <code>stage + size - 1</code>.
    q31_t *coeff;      // points to the coefficient array of size <code>stage</code>. Ex: {b[stage-1], b[stage-2], ..., b[1], b[0]}
} nds32_lfir_q31_t;

/**
 * @brief Function for the floating-point lattice FIR filter.
 * @param[in]       *instance points to an instance of the lattice
 *                            FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 */
void nds32_lfir_f32(const nds32_lfir_f32_t *instance, float32_t *src,
                    float32_t *dst, uint32_t size);

/**
 * @brief Function for the q15 lattice FIR filter.
 * @param[in]       *instance points to an instance of the lattice
 *                            FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 */
void nds32_lfir_q15(const nds32_lfir_q15_t *instance, q15_t *src,
                    q15_t *dst, uint32_t size);

/**
 * @brief Function for the q31 lattice FIR filter.
 * @param[in]       *instance points to an instance of the lattice
 *                            FIR structure.
 * @param[in]       *src      points to the input block data.
 * @param[out]      *dst      points to the output block data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * In order to avoid overflows the input signal must be scaled down by
 * 2*log2(stage) bits.
 */
void nds32_lfir_q31(const nds32_lfir_q31_t *instance, q31_t *src,
                    q31_t *dst, uint32_t size);

/**
 * Decimator FIR Filters
 */
/**
 * @brief Structure for the f32 Decimator FIR Filters.
 */
typedef struct
{
    uint32_t M;             // is the decimation factor.
    uint32_t coeff_size;    // number of filter coefficients.
    float32_t *coeff;       // points to the coefficient array of size <code>coeff_size</code>. Ex: {b[coeff_size-1], b[coeff_size-2], ..., b[1], b[0]}
    float32_t *state;       // points to the state variable array of size <code>coeff_size + size - 1</code>.
} nds32_dcmfir_f32_t;

/**
 * @brief Structure for the q15 Decimator FIR Filters.
 */
typedef struct
{
    uint32_t M;             // is the decimation factor.
    uint32_t coeff_size;    // number of filter coefficients.
    q15_t *coeff;           // points to the coefficient array of size <code>coeff_size</code>. Ex: {b[coeff_size-1], b[coeff_size-2], ..., b[1], b[0]}
    q15_t *state;           // points to the state variable array of size <code>coeff_size + size - 1</code>.
} nds32_dcmfir_q15_t;

/**
 * @brief Structure for the q31 Decimator FIR Filters.
 */
typedef struct
{
    uint32_t M;             // is the decimation factor.
    uint32_t coeff_size;    // number of filter coefficients.
    q31_t *coeff;           // points to the coefficient array of size <code>coeff_size</code>. Ex: {b[coeff_size-1], b[coeff_size-2], ..., b[1], b[0]}
    q31_t *state;           // points to the state variable array of size <code>coeff_size + size - 1</code>.
} nds32_dcmfir_q31_t;

void nds32_dcmfir_f32(const nds32_dcmfir_f32_t *instance, float32_t *src,
                      float32_t *dst, uint32_t size);
void nds32_dcmfir_q15(const nds32_dcmfir_q15_t *instance, q15_t *src,
                      q15_t *dst, uint32_t size);
void nds32_dcmfir_q31(const nds32_dcmfir_q31_t *instance, q31_t *src,
                      q31_t *dst, uint32_t size);

/**
 * Upsampling FIR Filters
 */
/**
 * @brief Structure for the floating-point Upsampling FIR Filters.
 */
typedef struct
{
    uint32_t L;          // is the upsampling factor.
    uint32_t plen;       // length of the polyphase filter
    float32_t *coeff;    // points to the coefficient array of size <code>L*plen</code>.
    float32_t *state;    // points to the state variable array of size <code>plen + size - 1</code>.
} nds32_upsplfir_f32_t;

/**
 * @brief Structure for the Q15 Upsampling FIR Filters.
 */
typedef struct
{
    uint32_t L;       // is the upsampling factor.
    uint32_t plen;    // length of the polyphase filter
    q15_t *coeff;     // points to the coefficient array of size <code>L*plen</code>.
    q15_t *state;     // points to the state variable array of size <code>plen + size - 1</code>.
} nds32_upsplfir_q15_t;

/**
 * @brief Structure for the Q31 Upsampling FIR Filters.
 */
typedef struct
{
    uint32_t L;       // is the upsampling factor.
    uint32_t plen;    // length of the polyphase filter
    q31_t *coeff;     // points to the coefficient array of size <code>L*plen</code>.
    q31_t *state;     // points to the state variable array of size <code>plen + size - 1</code>.
} nds32_upsplfir_q31_t;

void nds32_upsplfir_f32(const nds32_upsplfir_f32_t *instance, float32_t *src,
                        float32_t *dst, uint32_t size);
void nds32_upsplfir_q15(const nds32_upsplfir_q15_t *instance, q15_t *src,
                        q15_t *dst, uint32_t size);
void nds32_upsplfir_q31(const nds32_upsplfir_q31_t *instance, q31_t *src,
                        q31_t *dst, uint32_t size);

/**
 * Sparse FIR Filters
 */
/**
 * @brief Structure for the flaoting-point Sparse FIR Filters.
 */
typedef struct
{
    uint16_t coeff_size;    // number of filter coefficients.
    uint16_t index;         // is the state buffer index.
    float32_t *state;       // points to the state variable array of size <code>delay + size</code>.
    float32_t *coeff;       // points to the coefficient array of size <code>coeff_size</code>.
    uint16_t delay;         // is the largest offset value that is ever used in the nezdelay array.
    int32_t *nezdelay;      // points to an array of nonzero indices and is also of length <code>coeff_size</code>.
} nds32_spafir_f32_t;

/**
 * @brief Structure for the Q15 Sparse FIR Filters.
 */
typedef struct
{
    uint16_t coeff_size;    // number of filter coefficients.
    uint16_t index;         // is the state buffer index.
    q15_t *state;           // points to the state variable array of size <code>delay + size</code>.
    q15_t *coeff;           // points to the coefficient array of size <code>coeff_size</code>.
    uint16_t delay;         // is the largest offset value that is ever used in the nezdelay array.
    int32_t *nezdelay;      // points to an array of nonzero indices and is also of length <code>coeff_size</code>.
} nds32_spafir_q15_t;

/**
 * @brief Structure for the Q31 Sparse FIR Filters.
 */
typedef struct
{
    uint16_t coeff_size;    // number of filter coefficients.
    uint16_t index;         // is the state buffer index.
    q31_t *state;           // points to the state variable array of size <code>delay + size</code>.
    q31_t *coeff;           // points to the coefficient array of size <code>coeff_size</code>.
    uint16_t delay;         // is the largest offset value that is ever used in the nezdelay array.
    int32_t *nezdelay;      // points to an array of nonzero indices and is also of length <code>coeff_size</code>.
} nds32_spafir_q31_t;

/**
 * @brief Structure for the Q7 Sparse FIR Filters.
 */
typedef struct
{
    uint16_t coeff_size;    // number of filter coefficients.
    uint16_t index;         // is the state buffer index.
    q7_t *state;            // points to the state variable array of size <code>delay + size</code>.
    q7_t *coeff;            // points to the coefficient array of size <code>coeff_size</code>.
    uint16_t delay;         // is the largest offset value that is ever used in the nezdelay array.
    int32_t *nezdelay;      // points to an array of nonzero indices and is also of length <code>coeff_size</code>.
} nds32_spafir_q7_t;

void nds32_spafir_f32(nds32_spafir_f32_t *instance, float32_t *src,
                      float32_t *dst, float32_t *buf, uint32_t size);
void nds32_spafir_q15(nds32_spafir_q15_t *instance, q15_t *src,
                      q15_t *dst, q15_t *buf1, q31_t *buf2, uint32_t size);
void nds32_spafir_q31(nds32_spafir_q31_t *instance, q31_t *src,
                      q31_t *dst, q31_t *buf, uint32_t size);
void nds32_spafir_q7(nds32_spafir_q7_t *instance, q7_t *src,
                     q7_t *dst, q7_t *buf1, q31_t *buf2, uint32_t size);

/**
 * Standard LMS filte
 */
/**
 * @brief Structure for the floatint-point standard LMS Filters.
 */
typedef struct
{
    uint32_t coeff_size; // number of filter coefficients.
    float32_t *state;    // points to the state variable array of size <code>coeff_size + size - 1</code>.
    float32_t *coeff;    // points to the coefficient array of size <code>coeff_size</code> and the coefficients are stored in time reversed order.
    float32_t mu;        // step size that controls filter coefficient updates.
} nds32_lms_f32_t;

/**
 * @brief Structure for the Q31 standard LMS Filters.
 */
typedef struct
{
    uint32_t coeff_size; // number of filter coefficients.
    q31_t *state;        // points to the state variable array of size <code>coeff_size + size - 1</code>.
    q31_t *coeff;        // points to the coefficient array of size <code>coeff_size</code> and the coefficients are stored in time reversed order.
    q31_t mu;            // step size that controls filter coefficient updates.
    q31_t shift;         // bit shift applied to coefficients.
} nds32_lms_q31_t;

/**
 * @brief Structure for the Q15 standard LMS Filters.
 */
typedef struct
{
    uint32_t coeff_size; // number of filter coefficients.
    q15_t *state;        // points to the state variable array of size <code>coeff_size + size - 1</code>.
    q15_t *coeff;        // points to the coefficient array of size <code>coeff_size</code> and the coefficients are stored in time reversed order.
    q15_t mu;            // step size that controls filter coefficient updates.
    q15_t shift;         // bit shift applied to coefficients.
} nds32_lms_q15_t;

/**
 * @brief Function for the floating-point LMS filter.
 * @param[in]       *instance points to an instance of the LMS structure.
 * @param[in]       *src      points to the input block data.
 * @param[in]       *ref      points to the reference data.
 * @param[out]      *dst      points to the output data.
 * @param[out]      *err      points to the error data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 */
void nds32_lms_f32(const nds32_lms_f32_t *instance, float32_t *src,
                   float32_t *ref, float32_t *dst, float32_t *err, uint32_t size);

/**
 * @brief Function for the q31 LMS filter.
 * @param[in]       *instance points to an instance of the LMS structure.
 * @param[in]       *src      points to the input block data.
 * @param[in]       *ref      points to the reference data.
 * @param[out]      *dst      points to the output data.
 * @param[out]      *err      points to the error data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * Both coefficients and state variables are represented in 1.31 format
 * and multiplications yield a 2.62 result. The 2.62 results are accumulated
 * in a 64-bit accumulator and is right shifted by 31 bits and saturated to
 * 1.31 formatthe to yield the final result. In order to avoid overflows
 * completely the input signal must be scaled down by log2(coeff_size) bits.
 */
void nds32_lms_q31(const nds32_lms_q31_t *instance, q31_t *src, q31_t *ref,
                   q31_t *dst, q31_t *err, uint32_t size);

/**
 * @brief Function for the q15 LMS filter.
 * @param[in]       *instance points to an instance of the LMS structure.
 * @param[in]       *src      points to the input block data.
 * @param[in]       *ref      points to the reference data.
 * @param[out]      *dst      points to the output data.
 * @param[out]      *err      points to the error data.
 * @param[in]       size      number of the blocksize.
 * @return none.
 *
 * Function notes:
 * Both coefficients and state variables are represented in 1.15 format
 * and multiplications yield a 2.30 result. The 2.30 results are accumulated
 * in a 64-bit accumulator in 34.30 format and the results is truncated
 * to 34.15 format by discarding low 15 bits. Lastly, the outputs is
 * saturated to yield a result in 1.15 format.
 */
void nds32_lms_q15(const nds32_lms_q15_t *instance, q15_t *src, q15_t *ref,
                   q15_t *dst, q15_t *err, uint32_t size);

/**
 * Normalized LNS filter
 */
#define EPSILON 0.000000119209289f

/**
 * @brief Structure for the f32 normalized LNS filter.
 */
typedef struct
{
    uint32_t coeff_size; ///< number of filter coefficients.
    float32_t *state;    ///< points to the state variable array of size <code>coeff_size + size - 1</code>.
    float32_t *coeff;    ///< points to the coefficient array of size <code>coeff_size</code> and the coefficients are stored in time reversed order.
    float32_t mu;        ///< step size that controls filter coefficient updates.
    float32_t energy;    ///< previous frame energy.
} nds32_nlms_f32_t;

void nds32_nlms_f32(nds32_nlms_f32_t *instance, float32_t *src,
                    float32_t *ref, float32_t *dst, float32_t *err, uint32_t size);


/**
 * @brief Structure for the q31 normalized LNS filter.
 */
typedef struct
{
    uint32_t coeff_size; ///< number of filter coefficients.
    q31_t *state;    ///< points to the state variable array of size <code>coeff_size + size - 1</code>.
    q31_t *coeff;    ///< points to the coefficient array of size <code>coeff_size</code> and the coefficients are stored in time reversed order.
    q31_t mu;        ///< step size that controls filter coefficient updates.
    uint8_t postshift;  //bit shift applied to coefficients.
    q31_t energy;    ///< previous frame energy.
    q31_t x0;       ///< previous input sample.
} nds32_nlms_q31_t;


void nds32_nlms_q31(nds32_nlms_q31_t *instance, q31_t *src,
                    q31_t *ref, q31_t *dst, q31_t *err, uint32_t size);


/**
 * @brief Structure for the q15 normalized LNS filter.
 */
typedef struct
{
    uint32_t coeff_size; ///< number of filter coefficients.
    q15_t *state;    ///< points to the state variable array of size <code>coeff_size + size - 1</code>.
    q15_t *coeff;    ///< points to the coefficient array of size <code>coeff_size</code> and the coefficients are stored in time reversed order.
    q15_t mu;        ///< step size that controls filter coefficient updates.
    uint8_t postshift;  //bit shift applied to coefficients.
    q15_t energy;    ///< previous frame energy.
    q15_t x0;       ///< previous input sample.
} nds32_nlms_q15_t;


void nds32_nlms_q15(nds32_nlms_q15_t *instance, q15_t *src,
                    q15_t *ref, q15_t *dst, q15_t *err, uint32_t size);


// Convolution
/**
 * @brief Convolution of the floating-point vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @return none.
 */
void nds32_conv_f32(float32_t *src1, uint32_t len1,
                    float32_t *src2, uint32_t len2, float32_t *dst);

/**
 * @brief Convolution of the q15 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.15 format and multiplications yield a 2.30 result.
 * The 2.30 intermediate results are accumulated in a 64-bit accumulator in
 * 34.30 format. The 34.30 result is then truncated to 34.15 format by
 * discarding the low 15 bits and then saturated to 1.15 format.
 */
void nds32_conv_q15(q15_t *src1, uint32_t len1,
                    q15_t *src2, uint32_t len2, q15_t *dst);

/**
 * @brief Convolution of the q31 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.31 format and the 64-bit accumulator has a 2.62
 * format and maintains full precision of the intermediate multiplication
 * results but provides only a single guard bit. The input signals should be
 * scaled down to avoid intermediate overflows. Scale down the inputs by
 * log2(min(srcALen, srcBLen)), The 2.62 accumulator is right shifted by 31
 * bits and saturated to 1.31 forma t to yield the final result.
 */
void nds32_conv_q31(q31_t *src1, uint32_t len1,
                    q31_t *src2, uint32_t len2, q31_t *dst);

/**
 * @brief Convolution of the q7 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.7 format and multiplications yield a 2.14 result.
 * The 2.14 intermediate results are accumulated in a 32-bit accumulator in
 * 18.14 format. The 18.14 result is then truncated to 18.7 format by
 * discarding the low 7 bits and then saturated to 1.7 format.
 */
void nds32_conv_q7(q7_t *src1, uint32_t len1,
                   q7_t *src2, uint32_t len2, q7_t *dst);

/**
 * @brief Convolution Partial of the floating-point vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @param[in]       startindex is the first output sample to start with.
 * @param[in]       size is the number of output points to be computed.
 * @return          Returns
 *                  0; success
 *                  -1; fail,  the input subset are not between 0 and len1+len2-2.
 */
int32_t nds32_conv_partial_f32(float32_t *src1, uint32_t len1,
                               float32_t *src2, uint32_t len2, float32_t *dst, uint32_t startindex, uint32_t size);

/**
 * @brief Convolution Partial of the q15 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @param[in]       startindex is the first output sample to start with.
 * @param[in]       size is the number of output points to be computed.
 * @return          Returns
 *                  0; success
 *                  -1; fail,  the input subset are not between 0 and len1+len2-2.
 */
int32_t nds32_conv_partial_q15(q15_t *src1, uint32_t len1,
                               q15_t *src2, uint32_t len2, q15_t *dst, uint32_t startindex, uint32_t size);

/**
 * @brief Convolution Partial of the q31 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @param[in]       startindex is the first output sample to start with.
 * @param[in]       size is the number of output points to be computed.
 * @return          Returns
 *                  0; success
 *                  -1; fail,  the input subset are not between 0 and len1+len2-2.
 */
int32_t nds32_conv_partial_q31(q31_t *src1, uint32_t len1,
                               q31_t *src2, uint32_t len2, q31_t *dst, uint32_t startindex, uint32_t size);

/**
 * @brief Convolution Partial of the q7 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @param[in]       startindex is the first output sample to start with.
 * @param[in]       size is the number of output points to be computed.
 * @return          Returns
 *                  0; success
 *                  -1; fail,  the input subset are not between 0 and len1+len2-2.
 */
int32_t nds32_conv_partial_q7(q7_t *src1, uint32_t len1,
                               q7_t *src2, uint32_t len2, q7_t *dst, uint32_t startindex, uint32_t size);


// Correlation
/**
 * @brief Correlation of the floating-point vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        2 * max(len1, len2) - 1.
 * @return none.
 */
void nds32_corr_f32(float32_t *src1, uint32_t len1,
                    float32_t *src2, uint32_t len2, float32_t *dst);

/**
 * @brief Correlation of the q15 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        2 * max(len1, len2) - 1.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.15 format and multiplications yield a 2.30 result.
 * The 2.30 intermediate results are accumulated in a 64-bit accumulator in
 * 34.30 format. The 34.30 result is then truncated to 34.15 format by
 * discarding the low 15 bits and then saturated to 1.15 format.
 */
void nds32_corr_q15(q15_t *src1, uint32_t len1,
                    q15_t *src2, uint32_t len2, q15_t *dst);

/**
 * @brief Convolution of the q31 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        len1 + len2 - 1.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.31 format and the 64-bit accumulator has a 2.62
 * format and maintains full precision of the intermediate multiplication
 * results but provides only a single guard bit. The input signals should be
 * scaled down to avoid intermediate overflows. Scale down one of the inputs
 * by <code>1/min(srcALen, srcBLen)</code> to avoid overflows since a
 * maximum of <code>min(srcALen, srcBLen)</code> number of additions is
 * carried internally. The 2.62 accumulator is right shifted by 31 bits and
 * saturated to 1.31 forma t to yield the final result.
 */
void nds32_corr_q31(q31_t *src1, uint32_t len1,
                    q31_t *src2, uint32_t len2, q31_t *dst);

/**
 * @brief Correlation of the q7 vectors.
 * @param[in]       *src1 points to the first input vector.
 * @param[in]       len1  length of the first input vector.
 * @param[in]       *src2 points to the second input vector.
 * @param[in]       len2  length of the second input vector.
 * @param[out]      *dst  points to the output vector where the length is
 *                        2 * max(len1, len2) - 1.
 * @return none.
 *
 * Function notes:
 * Both inputs are in 1.7 format and multiplications yield a 2.14 result.
 * The 2.14 intermediate results are accumulated in a 32-bit accumulator in
 * 18.14 format. The 18.14 result is then truncated to 18.7 format by
 * discarding the low 7 bits and then saturated to 1.7 format.
 */
void nds32_corr_q7(q7_t *src1, uint32_t len1,
                   q7_t *src2, uint32_t len2, q7_t *dst);

/**
 * Biquad Filter
 */
/**
 * @brief Structure for the f32 Biquad Filter.
 */

typedef struct
{
    uint32_t nstage;    // number of order stages in the filter.
    float32_t *state;   // Points to the array of state coefficients. The array is of length 4*nstage.
    float32_t *coeff;   // Points to the array of coefficients. The array is oflength 5*nstage.
} nds32_biquad_df1_f32_t;

typedef struct
{
    uint32_t nstage;    // number of order stages in the filter.
    float32_t *state;   // Points to the array of state coefficients. The array is of length 2*nstage.
    float32_t *coeff;   // Points to the array of coefficients. The array is of length 5*nstage.
} nds32_biquad_df2T_f32_t;


typedef struct
{
    uint32_t nstage;    // number of order stages in the filter.
    float32_t *state;   // Points to the array of state coefficients. The array is of length 4*nstage.
    float32_t *coeff;   // Points to the array of coefficients. The array is oflength 5*nstage.
} nds32_biquad_stereo_df2T_f32_t;


/**
 * @brief Structure for the f64 Biquad Filter.
 */
typedef struct
{
    uint32_t nstage;    // number of order stages in the filter.
    float64_t *state;   // Points to the array of state coefficients. The array is of length 2*nstage.
    float64_t *coeff;   // Points to the array of coefficients. The array is oflength 5*nstage.
} nds32_biquad_df2T_f64_t;


/**
 * @brief Structure for the q15 Biquad Filter.
 */
typedef struct
{
    uint32_t nstage;    // number of order stages in the filter.
    q15_t *state;       // Points to the array of state coefficients.The array is of length 4*nstage.
    q15_t *coeff;       // Points to the array of coefficients. The array is oflength 5*nstage.
    int8_t shift;       // Additional bits shift.
} nds32_biquad_df1_q15_t;

/**
 * @brief Structure for the q31 Biquad Filter.
 */
typedef struct
{
    uint32_t nstage;    // number of order stages in the filter.
    q31_t *state;       // Points to the array of state coefficients.The array is of length 4*nstage.
    q31_t *coeff;       // Points to the array of coefficients. The array is oflength 5*nstage.
    int8_t shift;       // Additional bits shift.
} nds32_biquad_df1_q31_t;


typedef struct
{
    uint32_t nstage;    // number of order stages in the filter.
    q63_t *state;       // Points to the array of state coefficients.The array is of length 4*nstage.
    q31_t *coeff;       // Points to the array of coefficients. The array is oflength 5*nstage.
    int8_t shift;       // Additional bits shift.
} nds32_biquad_df1_32x64_q31_t;



void nds32_biquad_df1_f32(const nds32_biquad_df1_f32_t *instance,
                          float32_t *src, float32_t *dst, uint32_t size);
void nds32_biquad_df1_q15(const nds32_biquad_df1_q15_t *instance,
                          q15_t *src, q15_t *dst, uint32_t size);
void nds32_biquad_df1_q31(const nds32_biquad_df1_q31_t *instance,
                          q31_t *src, q31_t *dst, uint32_t size);
void nds32_biquad_df1_32x64_q31(const nds32_biquad_df1_32x64_q31_t *instance,
                          q31_t *src, q31_t *dst, uint32_t size);
void nds32_biquad_df2T_f32(const nds32_biquad_df2T_f32_t *instance,
                          float32_t *src, float32_t *dst, uint32_t size);
void nds32_biquad_df2T_f64(const nds32_biquad_df2T_f64_t *instance,
                          float64_t *src, float64_t *dst, uint32_t size);
void nds32_biquad_stereo_df2T_f32(const nds32_biquad_stereo_df2T_f32_t *instance,
                          float32_t *src, float32_t *dst, uint32_t size);

#define nds32_bq_df1_f32_t nds32_biquad_df1_f32_t
#define nds32_bq_df1_q15_t nds32_biquad_df1_q15_t
#define nds32_bq_df1_q31_t nds32_biquad_df1_q31_t
#define nds32_bq_df1_32x64_q31_t nds32_biquad_df1_32x64_q31_t
#define nds32_bq_df2T_f32_t nds32_biquad_df2T_f32_t
#define nds32_bq_df2T_f64_t nds32_biquad_df2T_f64_t
#define nds32_bq_stereo_df2T_f32_t nds32_biquad_stereo_df2T_f32_t
#define nds32_bq_df1_f32 nds32_biquad_df1_f32
#define nds32_bq_df1_q15 nds32_biquad_df1_q15
#define nds32_bq_df1_q31 nds32_biquad_df1_q31
#define nds32_bq_df1_32x64_q31 nds32_biquad_df1_32x64_q31
#define nds32_bq_df2T_f32 nds32_biquad_df2T_f32
#define nds32_bq_df2T_f64 nds32_biquad_df2T_f64
#define nds32_bq_stereo_df2T_f32 nds32_biquad_stereo_df2T_f32

/**
 * Lattice IIR filter
 */
/**
 * @brief Structure for the floating-point lattice IIR filter.
 */
typedef struct
{
    uint32_t nstage;    // number of stages in the filter.
    float32_t *state;   // points to the state variable array of size <code>nstage + size</code>.
    float32_t *rcoeff;  // points to the reflection coefficient array of size <code>nstage</code> and the coefficients are stored in time reversed order.
    float32_t *lcoeff;  // points to the ladder coefficient array of size <code>nstage + 1</code>.
} nds32_liir_f32_t;

/**
 * @brief Structure for the q31 lattice IIR filter.
 */
typedef struct
{
    uint32_t nstage;    // number of stages in the filter.
    q31_t *state;       // points to the state variable array of size <code>nstage + size</code>.
    q31_t *rcoeff;      // points to the reflection coefficient array of size <code>nstage</code> and the coefficients are stored in time reversed order.
    q31_t *lcoeff;      // points to the ladder coefficient array of size <code>nstage + 1</code>.
} nds32_liir_q31_t;

/**
 * @brief Structure for the q15 lattice IIR filter.
 */
typedef struct
{
    uint32_t nstage;    // number of stages in the filter.
    q15_t *state;       // points to the state variable array of size <code>nstage + size</code>.
    q15_t *rcoeff;      // points to the reflection coefficient array of size <code>nstage</code> and the coefficients are stored in time reversed order.
    q15_t *lcoeff;      // points to the ladder coefficient array of size <code>nstage + 1</code>.
} nds32_liir_q15_t;

void nds32_liir_f32(const nds32_liir_f32_t *instance, float32_t *src,
                    float32_t *dst, uint32_t size);
void nds32_liir_q31(const nds32_liir_q31_t *instance, q31_t *src,
                    q31_t *dst, uint32_t size);
void nds32_liir_q15(const nds32_liir_q15_t *instance, q15_t *src,
                    q15_t *dst, uint32_t size);

#ifdef  __cplusplus
}
#endif
#endif // __NDS32_FILTERING_MATH_H__

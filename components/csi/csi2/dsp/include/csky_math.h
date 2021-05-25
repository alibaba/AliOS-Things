/******************************************************************************
 * @file     csky_math.h
 * @brief    Public header file for CSI DSP Library.
 * @version  V1.0
 * @date     20. Dec 2016
 ******************************************************************************/
/* ---------------------------------------------------------------------------
 * Copyright (C) 2016 CSKY Limited. All rights reserved.
 *
 * Redistribution and use of this software in source and binary forms,
 * with or without modification, are permitted provided that the following
 * conditions are met:
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *   * Neither the name of CSKY Ltd. nor the names of CSKY's contributors may
 *     be used to endorse or promote products derived from this software without
 *     specific prior written permission of CSKY Ltd.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 * -------------------------------------------------------------------------- */


/**
 * @defgroup groupMath Basic Math Functions
 */

/**
 * @defgroup groupFastMath Fast Math Functions
 * This set of functions provides a fast approximation to sine, cosine, and square root.
 * As compared to most of the other functions in the CSI math library, the fast math functions
 * operate on individual values and not arrays.
 * There are separate functions for Q15, Q31, and floating-point data.
 *
 */

/**
 * @defgroup groupCmplxMath Complex Math Functions
 * This set of functions operates on complex data vectors.
 * The data in the complex arrays is stored in an interleaved fashion
 * (real, imag, real, imag, ...).
 * In the API functions, the number of samples in a complex array refers
 * to the number of complex values; the array contains twice this number of
 * real values.
 */

/**
 * @defgroup groupFilters Filtering Functions
 */

/**
 * @defgroup groupMatrix Matrix Functions
 *
 * This set of functions provides basic matrix math operations.
 * The functions operate on matrix data structures.  For example,
 * the type
 * definition for the floating-point matrix structure is shown
 * below:
 * <pre>
 *     typedef struct
 *     {
 *       uint16_t numRows;     // number of rows of the matrix.
 *       uint16_t numCols;     // number of columns of the matrix.
 *       float32_t *pData;     // points to the data of the matrix.
 *     } csky_matrix_instance_f32;
 * </pre>
 * There are similar definitions for Q15 and Q31 data types.
 *
 * The structure specifies the size of the matrix and then points to
 * an array of data.  The array is of size <code>numRows X numCols</code>
 * and the values are arranged in row order.  That is, the
 * matrix element (i, j) is stored at:
 * <pre>
 *     pData[i*numCols + j]
 * </pre>
 *
 * \par Init Functions
 * There is an associated initialization function for each type of matrix
 * data structure.
 * The initialization function sets the values of the internal structure fields.
 * Refer to the function <code>csky_mat_init_f32()</code>, <code>csky_mat_init_q31()</code>
 * and <code>csky_mat_init_q15()</code> for floating-point, Q31 and Q15 types,  respectively.
 *
 * \par
 * Use of the initialization function is optional. However, if initialization function is used
 * then the instance structure cannot be placed into a const data section.
 * To place the instance structure in a const data
 * section, manually initialize the data structure.  For example:
 * <pre>
 * <code>csky_matrix_instance_f32 S = {nRows, nColumns, pData};</code>
 * <code>csky_matrix_instance_q31 S = {nRows, nColumns, pData};</code>
 * <code>csky_matrix_instance_q15 S = {nRows, nColumns, pData};</code>
 * </pre>
 * where <code>nRows</code> specifies the number of rows, <code>nColumns</code>
 * specifies the number of columns, and <code>pData</code> points to the
 * data array.
 *
 * \par Size Checking
 * By default all of the matrix functions perform size checking on the input and
 * output matrices.  For example, the matrix addition function verifies that the
 * two input matrices and the output matrix all have the same number of rows and
 * columns.  If the size check fails the functions return:
 * <pre>
 *     CSKY_MATH_SIZE_MISMATCH
 * </pre>
 * Otherwise the functions return
 * <pre>
 *     CSKY_MATH_SUCCESS
 * </pre>
 * There is some overhead associated with this matrix size checking.
 * The matrix size checking is enabled via the \#define
 * <pre>
 *     CSKY_MATH_MATRIX_CHECK
 * </pre>
 * within the library project settings.  By default this macro is defined
 * and size checking is enabled.  By changing the project settings and
 * undefining this macro size checking is eliminated and the functions
 * run a bit faster.  With size checking disabled the functions always
 * return <code>CSKY_MATH_SUCCESS</code>.
 */

/**
 * @defgroup groupTransforms Transform Functions
 */

/**
 * @defgroup groupController Controller Functions
 */

/**
 * @defgroup groupStats Statistics Functions
 */
/**
 * @defgroup groupSupport Support Functions
 */

/**
 * @defgroup groupInterpolation Interpolation Functions
 * These functions perform 1- and 2-dimensional interpolation of data.
 * Linear interpolation is used for 1-dimensional data and
 * bilinear interpolation is used for 2-dimensional data.
 */


/**
 * @defgroup groupYunvoice Yunvoice Functions
 * These functions are designed for Yunvoice project, which are modified
 * according to the CEVA DSP functions. So, one can porting the software
 * from CEVA to CSKY straightforwardly.
 */

/**
 * @defgroup groupExamples Examples
 */


#ifndef _CSKY_MATH_H
#define _CSKY_MATH_H

#define __CSI_GENERIC         /* disable NVIC and Systick functions */

#include "csi_core.h"

#include <float.h>
#undef  __CSI_GENERIC         /* enable NVIC and Systick functions */
#include "string.h"
#include "math.h"
#ifdef   __cplusplus
extern "C"
{
#endif


  /**
   * @brief Macros required for reciprocal calculation in Normalized LMS
   */

#define DELTA_Q31          (0x100)
#define DELTA_Q15          0x5
#define INDEX_MASK         0x0000003F
#ifndef PI
#define PI                 3.14159265358979f
#endif

  /**
   * @brief Macros required for SINE and COSINE Fast math approximations
   */

#define FAST_MATH_TABLE_SIZE  512
#define FAST_MATH_Q31_SHIFT   (32 - 10)
#define FAST_MATH_Q15_SHIFT   (16 - 10)
#define CONTROLLER_Q31_SHIFT  (32 - 9)
#define TABLE_SIZE  256
#define TABLE_SPACING_Q31     0x400000
#define TABLE_SPACING_Q15     0x80

  /**
   * @brief Macros required for SINE and COSINE Controller functions
   */
  /* 1.31(q31) Fixed value of 2/360 */
  /* -1 to +1 is divided into 360 values so total spacing is (2/360) */
#define INPUT_SPACING         0xB60B61

  /**
   * @brief Macro for Unaligned Support
   */
#ifndef UNALIGNED_SUPPORT_DISABLE
    #define ALIGN4
#else
    #define ALIGN4 __attribute__((aligned(4)))
#endif   /* #ifndef UNALIGNED_SUPPORT_DISABLE */

__ALWAYS_STATIC_INLINE int32_t __SSAT_31(int32_t x)
{
    int32_t res = x;
    if (x > 0x3fffffff) {
       res = 0x3fffffff;
    } else if (x < -1073741824) {
       res = -1073741824;
    }

    return res;
}

__ALWAYS_STATIC_INLINE int32_t __SSAT_16(int32_t x)
{
    int32_t res = x;
    if (x > 0x7fff) {
       res = 0x7fff;
    } else if (x < -32768) {
       res = -32768;
    }

    return res;
}

__ALWAYS_STATIC_INLINE int32_t __SSAT_8(int32_t x)
{
    int32_t res = x;
    if (x > 0x7f) {
       res = 0x7f;
    } else if (x < -128) {
       res = -128;
    }

    return res;
}

#ifdef CSKY_SIMD
/* SMMLAR */
__ALWAYS_STATIC_INLINE int32_t multAcc_32x32_keep32_R(int32_t a, int32_t x, int32_t y)
{
    __ASM volatile("mula.s32.rhs %0, %1, %2\n\t"
                   :"=r" (a), "=r" (x), "=r" (y) : "0" (a), "1" (x), "2" (y));
    return a;
}

/* SMMLSR */
__ALWAYS_STATIC_INLINE int32_t multSub_32x32_keep32_R(int32_t a, int32_t x, int32_t y)
{
    __ASM volatile("muls.s32.rhs %0, %1, %2\n\t"
                   :"=r" (a), "=r" (x), "=r" (y): "0" (a), "1" (x), "2" (y));
    return a;
}

/* SMMULR */
__ALWAYS_STATIC_INLINE int32_t mult_32x32_keep32_R(int32_t x, int32_t y)
{
    int32_t a;
    __ASM volatile("mul.s32.rh %0, %1, %2\n\t"
                   :"=r" (a), "=r" (x), "=r" (y): "1" (x), "2" (y));
    return a;
}

/* SMMLA */
__ALWAYS_STATIC_INLINE int32_t multAcc_32x32_keep32(int32_t a, int32_t x, int32_t y)
{
    __ASM volatile("mula.s32.hs %0, %1, %2\n\t"
                   :"=r" (a), "=r" (x), "=r" (y): "0" (a), "1" (x), "2" (y));
    return a;
}

/* SMMLS */
__ALWAYS_STATIC_INLINE int32_t multSub_32x32_keep32(int32_t a, int32_t x, int32_t y)
{
    __ASM volatile("muls.s32.hs %0, %1, %2\n\t"
                   :"=r" (a), "=r" (x), "=r" (y): "0" (a), "1" (x), "2" (y));
    return a;
}

/* SMMUL */
__ALWAYS_STATIC_INLINE int32_t mult_32x32_keep32(int32_t x, int32_t y)
{
    int32_t a;
    __ASM volatile("mul.s32.h %0, %1, %2\n\t"
                   :"=r" (a), "=r" (x), "=r" (y): "0" (a), "1" (x), "2" (y));
    return a;
}

__ALWAYS_STATIC_INLINE int32_t multAcc_16x16_keep32(int32_t a, int16_t x, int16_t y)
{
    __ASM volatile("mulall.s16 %0, %1, %2\n\t"
                   :"=r" (a), "=r" (x), "=r" (y): "0" (a), "1" (x), "2" (y));
    return a;
}

__ALWAYS_STATIC_INLINE int64_t multAcc_16x16_keep64(int64_t a, int16_t x, int16_t y)
{
    __ASM volatile("mulall.s16.e %0, %1, %2\n\t"
                   :"=r" (a), "=r" (x), "=r" (y): "0" (a), "1" (x), "2" (y));
    return a;
}

__ALWAYS_STATIC_INLINE int64_t mult_32x32_keep64(int32_t x, int32_t y)
{
    int64_t a;
    __ASM volatile("mul.s32 %0, %1, %2\n\t"
                   :"=r" (a), "=r" (x), "=r" (y): "1" (x), "2" (y));
    return a;
}

__ALWAYS_STATIC_INLINE int64_t multAcc_32x32_keep64(int64_t a, int32_t x, int32_t y)
{
    __ASM volatile("mula.s32 %0, %1, %2\n\t"
                   :"=r" (a), "=r" (x), "=r" (y): "0" (a), "1" (x), "2" (y));
    return a;
}

__ALWAYS_STATIC_INLINE int32_t mult_32x32_dext_31(int32_t x, int32_t y)
{
    int64_t tmp1;
    int32_t tmp2;
    __ASM volatile("mul.s32 %0, %1, %2\n\t"
                   "dexti %3, %0, %R0, 31"
                   :"=r" (tmp1), "=r" (x), "=r" (y), "=r" (tmp2): "1" (x), "2" (y));
    return tmp2;
}

__ALWAYS_STATIC_INLINE int32_t mult_32x32_dext_30(int32_t x, int32_t y)
{
    int64_t tmp1;
    int32_t tmp2;
    __ASM volatile("mul.s32 %0, %1, %2\n\t"
                   "dexti %3, %0, %R0, 30"
                   :"=r" (tmp1), "=r" (x), "=r" (y), "=r" (tmp2): "1" (x), "2" (y));
    return tmp2;
}

__ALWAYS_STATIC_INLINE int32_t mult_32x32_dext_4(int32_t x, int32_t y)
{
    int64_t tmp1;
    int32_t tmp2;
    __ASM volatile("mul.s32 %0, %1, %2\n\t"
                   "dexti %3, %0, %R0, 4"
                   :"=r" (tmp1), "=r" (x), "=r" (y), "=r" (tmp2): "1" (x), "2" (y));
    return tmp2;
}

__ALWAYS_STATIC_INLINE int32_t mult_32x32_dext_33(int32_t x, int32_t y)
{
    int64_t tmp1;
    int32_t tmp2;
    __ASM volatile("mul.s32 %0, %1, %2\n\t"
                   "asri %3, %R0, 1"
                   :"=r" (tmp1), "=r" (x), "=r" (y), "=r" (tmp2): "1" (x), "2" (y));
    return tmp2;
}

__ALWAYS_STATIC_INLINE int32_t dext_31(int64_t x)
{
    int32_t tmp1;
    __ASM volatile(
                   "dexti %0, %1, %R1, 31"
                   :"=r" (tmp1), "=r" (x) : "1" (x));
    return tmp1;
}

__ALWAYS_STATIC_INLINE int32_t mult_l16xl16_keep32(int32_t x, int32_t y)
{
    int32_t a;
    __ASM volatile("mulll.s16 %0, %1, %2\n\t"
                   :"=r" (a), "=r" (x), "=r" (y): "1" (x), "2" (y));
    return a;
}

__ALWAYS_STATIC_INLINE int32_t mult_h16xl16_keep32(int32_t x, int32_t y)
{
    int32_t a;
    __ASM volatile("mulhl.s16 %0, %1, %2\n\t"
                   :"=r" (a), "=r" (x), "=r" (y): "1" (x), "2" (y));
    return a;
}

__ALWAYS_STATIC_INLINE int32_t mult_h16xh16_keep32(int32_t x, int32_t y)
{
    int32_t a;
    __ASM volatile("mulhh.s16 %0, %1, %2\n\t"
                   :"=r" (a), "=r" (x), "=r" (y): "1" (x), "2" (y));
    return a;
}

#endif


  /**
   * @brief Error status returned by some functions in the library.
   */

  typedef enum
  {
    CSKY_MATH_SUCCESS = 0,                /**< No error */
    CSKY_MATH_ARGUMENT_ERROR = -1,        /**< One or more arguments are incorrect */
    CSKY_MATH_LENGTH_ERROR = -2,          /**< Length of data buffer is incorrect */
    CSKY_MATH_SIZE_MISMATCH = -3,         /**< Size of matrices is not compatible with the operation. */
    CSKY_MATH_NANINF = -4,                /**< Not-a-number (NaN) or infinity is generated */
    CSKY_MATH_SINGULAR = -5,              /**< Generated by matrix inversion if the input matrix is singular and cannot be inverted. */
    CSKY_MATH_TEST_FAILURE = -6           /**< Test Failed  */
  } csky_status;

  /**
   * @brief 8-bit fractional data type in 1.7 format.
   */
  typedef int8_t q7_t;

  /**
   * @brief 16-bit fractional data type in 1.15 format.
   */
  typedef int16_t q15_t;

  /**
   * @brief 32-bit fractional data type in 1.31 format.
   */
  typedef int32_t q31_t;

  /**
   * @brief 64-bit fractional data type in 1.63 format.
   */
  typedef int64_t q63_t;

  /**
   * @brief 32-bit floating-point type definition.
   */
  typedef float float32_t;

  /**
   * @brief 64-bit floating-point type definition.
   */
  typedef double float64_t;

   /**
   * @brief 32-bit fractional complex data type in 1.31 format.
   */
  typedef struct
  {
    q31_t re;
    q31_t im;
  } cq31_t;
  /**
   * @brief 16-bit fractional complex data type in 1.15 format.
   */
  typedef struct
  {
    q15_t re;
    q15_t im;
  } cq15_t;
   /**
   * @brief definition to read/write two 16 bit values.
   */
  #define __SIMD32_TYPE int32_t
  #define CSI_UNUSED __attribute__((unused))

#define __SIMD32(addr)        (*(__SIMD32_TYPE **) & (addr))
#define __SIMD32_CONST(addr)  ((__SIMD32_TYPE *)(addr))
#define _SIMD32_OFFSET(addr)  (*(__SIMD32_TYPE *)  (addr))
#define __SIMD64(addr)        (*(int64_t **) & (addr))

#if defined (CSKY_MATH_NO_SIMD)
  /**
   * @brief definition to pack two 16 bit values.
   */
#define __PKHBT(ARG1, ARG2, ARG3)      ( (((int32_t)(ARG1) <<  0) & (int32_t)0x0000FFFF) | \
                                         (((int32_t)(ARG2) << ARG3) & (int32_t)0xFFFF0000)  )
#define __PKHTB(ARG1, ARG2, ARG3)      ( (((int32_t)(ARG1) <<  0) & (int32_t)0xFFFF0000) | \
                                         (((int32_t)(ARG2) >> ARG3) & (int32_t)0x0000FFFF)  )

#endif


   /**
   * @brief definition to pack four 8 bit values.
   */
#ifndef CSKY_MATH_BIG_ENDIAN

#define __PACKq7(v0,v1,v2,v3) ( (((int32_t)(v0) <<  0) & (int32_t)0x000000FF) | \
                                (((int32_t)(v1) <<  8) & (int32_t)0x0000FF00) | \
                                (((int32_t)(v2) << 16) & (int32_t)0x00FF0000) | \
                                (((int32_t)(v3) << 24) & (int32_t)0xFF000000)  )
#else

#define __PACKq7(v0,v1,v2,v3) ( (((int32_t)(v3) <<  0) & (int32_t)0x000000FF) | \
                                (((int32_t)(v2) <<  8) & (int32_t)0x0000FF00) | \
                                (((int32_t)(v1) << 16) & (int32_t)0x00FF0000) | \
                                (((int32_t)(v0) << 24) & (int32_t)0xFF000000)  )

#endif

  /**
   * @brief Clips Q63 to Q31 values.
   */
  static __INLINE q31_t clip_q63_to_q31(
  q63_t x)
  {
    return ((q31_t) (x >> 32) != ((q31_t) x >> 31)) ?
      ((0x7FFFFFFF ^ ((q31_t) (x >> 63)))) : (q31_t) x;
  }

  /**
   * @brief Instance structure for the Q7 FIR filter.
   */
  typedef struct
  {
    uint16_t numTaps;        /**< number of filter coefficients in the filter. */
    q7_t *pState;            /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    q7_t *pCoeffs;           /**< points to the coefficient array. The array is of length numTaps.*/
  } csky_fir_instance_q7;

  /**
   * @brief Instance structure for the Q15 FIR filter.
   */
  typedef struct
  {
    uint16_t numTaps;         /**< number of filter coefficients in the filter. */
    q15_t *pState;            /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    q15_t *pCoeffs;           /**< points to the coefficient array. The array is of length numTaps.*/
  } csky_fir_instance_q15;

  /**
   * @brief Instance structure for the Q31 FIR filter.
   */
  typedef struct
  {
    uint16_t numTaps;         /**< number of filter coefficients in the filter. */
    q31_t *pState;            /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    q31_t *pCoeffs;           /**< points to the coefficient array. The array is of length numTaps. */
  } csky_fir_instance_q31;

  /**
   * @brief Instance structure for the floating-point FIR filter.
   */
  typedef struct
  {
    uint16_t numTaps;     /**< number of filter coefficients in the filter. */
    float32_t *pState;    /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    float32_t *pCoeffs;   /**< points to the coefficient array. The array is of length numTaps. */
  } csky_fir_instance_f32;

  void csky_fir_q7(
  const csky_fir_instance_q7 * S,
  q7_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);

  void csky_fir_init_q7(
  csky_fir_instance_q7 * S,
  uint16_t numTaps,
  q7_t * pCoeffs,
  q7_t * pState,
  uint32_t blockSize);

  void csky_fir_q15(
  const csky_fir_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_fir_fast_q15(
  const csky_fir_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  csky_status csky_fir_init_q15(
  csky_fir_instance_q15 * S,
  uint16_t numTaps,
  q15_t * pCoeffs,
  q15_t * pState,
  uint32_t blockSize);

  void csky_fir_q31(
  const csky_fir_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_fir_fast_q31(
  const csky_fir_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_fir_init_q31(
  csky_fir_instance_q31 * S,
  uint16_t numTaps,
  q31_t * pCoeffs,
  q31_t * pState,
  uint32_t blockSize);

  void csky_fir_f32(
  const csky_fir_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_fir_init_f32(
  csky_fir_instance_f32 * S,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  uint32_t blockSize);


  /**
   * @brief Instance structure for the Q15 Biquad cascade filter.
   */
  typedef struct
  {
    int8_t numStages;        /**< number of 2nd order stages in the filter.  Overall order is 2*numStages. */
    q15_t *pState;           /**< Points to the array of state coefficients.  The array is of length 4*numStages. */
    q15_t *pCoeffs;          /**< Points to the array of coefficients.  The array is of length 5*numStages. */
    int8_t postShift;        /**< Additional shift, in bits, applied to each output sample. */
  } csky_biquad_casd_df1_inst_q15;

  /**
   * @brief Instance structure for the Q31 Biquad cascade filter.
   */
  typedef struct
  {
    uint32_t numStages;      /**< number of 2nd order stages in the filter.  Overall order is 2*numStages. */
    q31_t *pState;           /**< Points to the array of state coefficients.  The array is of length 4*numStages. */
    q31_t *pCoeffs;          /**< Points to the array of coefficients.  The array is of length 5*numStages. */
    uint8_t postShift;       /**< Additional shift, in bits, applied to each output sample. */
  } csky_biquad_casd_df1_inst_q31;

 /**
   * @brief Instance structure for the Q31 Biquad cascade filter.
   */

  /**
   * @brief Instance structure for the floating-point Biquad cascade filter.
   */
  typedef struct
  {
    uint32_t numStages;      /**< number of 2nd order stages in the filter.  Overall order is 2*numStages. */
    float32_t *pState;       /**< Points to the array of state coefficients.  The array is of length 4*numStages. */
    float32_t *pCoeffs;      /**< Points to the array of coefficients.  The array is of length 5*numStages. */
  } csky_biquad_casd_df1_inst_f32;

  void csky_biquad_cascade_df1_q15(
  const csky_biquad_casd_df1_inst_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_biquad_cascade_df1_init_q15(
  csky_biquad_casd_df1_inst_q15 * S,
  uint8_t numStages,
  q15_t * pCoeffs,
  q15_t * pState,
  int8_t postShift);

  void csky_biquad_cascade_df1_fast_q15(
  const csky_biquad_casd_df1_inst_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_biquad_cascade_df1_q31(
  const csky_biquad_casd_df1_inst_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_biquad_cascade_df1_fast_q31(
  const csky_biquad_casd_df1_inst_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_biquad_cascade_df1_init_q31(
  csky_biquad_casd_df1_inst_q31 * S,
  uint8_t numStages,
  q31_t * pCoeffs,
  q31_t * pState,
  int8_t postShift);

  void csky_biquad_cascade_df1_f32(
  const csky_biquad_casd_df1_inst_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_biquad_cascade_df1_init_f32(
  csky_biquad_casd_df1_inst_f32 * S,
  uint8_t numStages,
  float32_t * pCoeffs,
  float32_t * pState);


  /**
   * @brief Instance structure for the floating-point matrix structure.
   */
  typedef struct
  {
    uint16_t numRows;     /**< number of rows of the matrix.     */
    uint16_t numCols;     /**< number of columns of the matrix.  */
    float32_t *pData;     /**< points to the data of the matrix. */
  } csky_matrix_instance_f32;


  /**
   * @brief Instance structure for the floating-point matrix structure.
   */
  typedef struct
  {
    uint16_t numRows;     /**< number of rows of the matrix.     */
    uint16_t numCols;     /**< number of columns of the matrix.  */
    float64_t *pData;     /**< points to the data of the matrix. */
  } csky_matrix_instance_f64;

  /**
   * @brief Instance structure for the Q15 matrix structure.
   */
  typedef struct
  {
    uint16_t numRows;     /**< number of rows of the matrix.     */
    uint16_t numCols;     /**< number of columns of the matrix.  */
    q15_t *pData;         /**< points to the data of the matrix. */
  } csky_matrix_instance_q15;

  /**
   * @brief Instance structure for the Q31 matrix structure.
   */
  typedef struct
  {
    uint16_t numRows;     /**< number of rows of the matrix.     */
    uint16_t numCols;     /**< number of columns of the matrix.  */
    q31_t *pData;         /**< points to the data of the matrix. */
  } csky_matrix_instance_q31;

  csky_status csky_mat_add_f32(
  const csky_matrix_instance_f32 * pSrcA,
  const csky_matrix_instance_f32 * pSrcB,
  csky_matrix_instance_f32 * pDst);

  csky_status csky_mat_add_q15(
  const csky_matrix_instance_q15 * pSrcA,
  const csky_matrix_instance_q15 * pSrcB,
  csky_matrix_instance_q15 * pDst);

  csky_status csky_mat_add_q31(
  const csky_matrix_instance_q31 * pSrcA,
  const csky_matrix_instance_q31 * pSrcB,
  csky_matrix_instance_q31 * pDst);

  csky_status csky_mat_cmplx_mult_f32(
  const csky_matrix_instance_f32 * pSrcA,
  const csky_matrix_instance_f32 * pSrcB,
  csky_matrix_instance_f32 * pDst);

  csky_status csky_mat_cmplx_mult_q15(
  const csky_matrix_instance_q15 * pSrcA,
  const csky_matrix_instance_q15 * pSrcB,
  csky_matrix_instance_q15 * pDst);

  csky_status csky_mat_cmplx_mult_q31(
  const csky_matrix_instance_q31 * pSrcA,
  const csky_matrix_instance_q31 * pSrcB,
  csky_matrix_instance_q31 * pDst);

  csky_status csky_mat_trans_f32(
  const csky_matrix_instance_f32 * pSrc,
  csky_matrix_instance_f32 * pDst);

  csky_status csky_mat_trans_q15(
  const csky_matrix_instance_q15 * pSrc,
  csky_matrix_instance_q15 * pDst);

  csky_status csky_mat_trans_q31(
  const csky_matrix_instance_q31 * pSrc,
  csky_matrix_instance_q31 * pDst);

  csky_status csky_mat_mult_f32(
  const csky_matrix_instance_f32 * pSrcA,
  const csky_matrix_instance_f32 * pSrcB,
  csky_matrix_instance_f32 * pDst);

  csky_status csky_mat_mult_q15(
  const csky_matrix_instance_q15 * pSrcA,
  const csky_matrix_instance_q15 * pSrcB,
  csky_matrix_instance_q15 * pDst);

  csky_status csky_mat_mult_fast_q15(
  const csky_matrix_instance_q15 * pSrcA,
  const csky_matrix_instance_q15 * pSrcB,
  csky_matrix_instance_q15 * pDst);

  csky_status csky_mat_mult_q31(
  const csky_matrix_instance_q31 * pSrcA,
  const csky_matrix_instance_q31 * pSrcB,
  csky_matrix_instance_q31 * pDst);

  csky_status csky_mat_mult_trans_q31(
  const csky_matrix_instance_q31 * pSrcA,
  const csky_matrix_instance_q31 * pSrcB,
  csky_matrix_instance_q31 * pDst);

  csky_status csky_mat_mult_fast_q31(
  const csky_matrix_instance_q31 * pSrcA,
  const csky_matrix_instance_q31 * pSrcB,
  csky_matrix_instance_q31 * pDst);

  csky_status csky_mat_sub_f32(
  const csky_matrix_instance_f32 * pSrcA,
  const csky_matrix_instance_f32 * pSrcB,
  csky_matrix_instance_f32 * pDst);

  csky_status csky_mat_sub_q15(
  const csky_matrix_instance_q15 * pSrcA,
  const csky_matrix_instance_q15 * pSrcB,
  csky_matrix_instance_q15 * pDst);

  csky_status csky_mat_sub_q31(
  const csky_matrix_instance_q31 * pSrcA,
  const csky_matrix_instance_q31 * pSrcB,
  csky_matrix_instance_q31 * pDst);

  csky_status csky_mat_scale_f32(
  const csky_matrix_instance_f32 * pSrc,
  float32_t scale,
  csky_matrix_instance_f32 * pDst);

  csky_status csky_mat_scale_q15(
  const csky_matrix_instance_q15 * pSrc,
  q15_t scaleFract,
  int32_t shift,
  csky_matrix_instance_q15 * pDst);

  csky_status csky_mat_scale_q31(
  const csky_matrix_instance_q31 * pSrc,
  q31_t scaleFract,
  int32_t shift,
  csky_matrix_instance_q31 * pDst);

  void csky_mat_init_q31(
  csky_matrix_instance_q31 * S,
  uint16_t nRows,
  uint16_t nColumns,
  q31_t * pData);

  void csky_mat_init_q15(
  csky_matrix_instance_q15 * S,
  uint16_t nRows,
  uint16_t nColumns,
  q15_t * pData);

  void csky_mat_init_f32(
  csky_matrix_instance_f32 * S,
  uint16_t nRows,
  uint16_t nColumns,
  float32_t * pData);

  /**
   * @brief Instance structure for the Q15 PID Control.
   */
  typedef struct
  {
    q15_t A0;           /**< The derived gain, A0 = Kp + Ki + Kd . */
    q15_t A1;
    q15_t A2;
    q15_t state[3];     /**< The state array of length 3. */
    q15_t Kp;           /**< The proportional gain. */
    q15_t Ki;           /**< The integral gain. */
    q15_t Kd;           /**< The derivative gain. */
  } csky_pid_instance_q15;

  /**
   * @brief Instance structure for the Q31 PID Control.
   */
  typedef struct
  {
    q31_t A0;            /**< The derived gain, A0 = Kp + Ki + Kd . */
    q31_t A1;            /**< The derived gain, A1 = -Kp - 2Kd. */
    q31_t A2;            /**< The derived gain, A2 = Kd . */
    q31_t state[3];      /**< The state array of length 3. */
    q31_t Kp;            /**< The proportional gain. */
    q31_t Ki;            /**< The integral gain. */
    q31_t Kd;            /**< The derivative gain. */
  } csky_pid_instance_q31;

  /**
   * @brief Instance structure for the floating-point PID Control.
   */
  typedef struct
  {
    float32_t A0;          /**< The derived gain, A0 = Kp + Ki + Kd . */
    float32_t A1;          /**< The derived gain, A1 = -Kp - 2Kd. */
    float32_t A2;          /**< The derived gain, A2 = Kd . */
    float32_t state[3];    /**< The state array of length 3. */
    float32_t Kp;          /**< The proportional gain. */
    float32_t Ki;          /**< The integral gain. */
    float32_t Kd;          /**< The derivative gain. */
  } csky_pid_instance_f32;

  void csky_pid_init_f32(
  csky_pid_instance_f32 * S,
  int32_t resetStateFlag);

  void csky_pid_reset_f32(
  csky_pid_instance_f32 * S);

  void csky_pid_init_q31(
  csky_pid_instance_q31 * S,
  int32_t resetStateFlag);

  void csky_pid_reset_q31(
  csky_pid_instance_q31 * S);

  void csky_pid_init_q15(
  csky_pid_instance_q15 * S,
  int32_t resetStateFlag);

  void csky_pid_reset_q15(
  csky_pid_instance_q15 * S);


  /**
   * @brief Instance structure for the floating-point Linear Interpolate function.
   */
  typedef struct
  {
    uint32_t nValues;           /**< nValues */
    float32_t x1;               /**< x1 */
    float32_t xSpacing;         /**< xSpacing */
    float32_t *pYData;          /**< pointer to the table of Y values */
  } csky_linear_interp_instance_f32;

  /**
   * @brief Instance structure for the floating-point bilinear interpolation function.
   */
  typedef struct
  {
    uint16_t numRows;   /**< number of rows in the data table. */
    uint16_t numCols;   /**< number of columns in the data table. */
    float32_t *pData;   /**< points to the data table. */
  } csky_bilinear_interp_instance_f32;

   /**
   * @brief Instance structure for the Q31 bilinear interpolation function.
   */
  typedef struct
  {
    uint16_t numRows;   /**< number of rows in the data table. */
    uint16_t numCols;   /**< number of columns in the data table. */
    q31_t *pData;       /**< points to the data table. */
  } csky_bilinear_interp_instance_q31;

   /**
   * @brief Instance structure for the Q15 bilinear interpolation function.
   */
  typedef struct
  {
    uint16_t numRows;   /**< number of rows in the data table. */
    uint16_t numCols;   /**< number of columns in the data table. */
    q15_t *pData;       /**< points to the data table. */
  } csky_bilinear_interp_instance_q15;

   /**
   * @brief Instance structure for the Q15 bilinear interpolation function.
   */
  typedef struct
  {
    uint16_t numRows;   /**< number of rows in the data table. */
    uint16_t numCols;   /**< number of columns in the data table. */
    q7_t *pData;        /**< points to the data table. */
  } csky_bilinear_interp_instance_q7;

  void csky_mult_q7(
  q7_t * pSrcA,
  q7_t * pSrcB,
  q7_t * pDst,
  uint32_t blockSize);

  void csky_mult_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_mult_rnd_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_mult_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_mult_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t blockSize);


  /**
   * @brief Instance structure for the Q15 CFFT/CIFFT function.
   */
  typedef struct
  {
    uint16_t fftLen;                 /**< length of the FFT. */
    uint8_t ifftFlag;                /**< flag that selects forward (ifftFlag=0) or inverse (ifftFlag=1) transform. */
    uint8_t bitReverseFlag;          /**< flag that enables (bitReverseFlag=1) or disables (bitReverseFlag=0) bit reversal of output. */
    q15_t *pTwiddle;                 /**< points to the Sin twiddle factor table. */
    uint16_t *pBitRevTable;          /**< points to the bit reversal table. */
    uint16_t twidCoefModifier;       /**< twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. */
    uint16_t bitRevFactor;           /**< bit reversal modifier that supports different size FFTs with the same bit reversal table. */
  } csky_cfft_radix2_instance_q15;

  /**
   * @brief Instance structure for the Q15 CFFT/CIFFT function.
   */
  typedef struct
  {
    uint16_t fftLen;                 /**< length of the FFT. */
    uint8_t ifftFlag;                /**< flag that selects forward (ifftFlag=0) or inverse (ifftFlag=1) transform. */
    uint8_t bitReverseFlag;          /**< flag that enables (bitReverseFlag=1) or disables (bitReverseFlag=0) bit reversal of output. */
    q15_t *pTwiddle;                 /**< points to the twiddle factor table. */
    uint16_t *pBitRevTable;          /**< points to the bit reversal table. */
    uint16_t twidCoefModifier;       /**< twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. */
    uint16_t bitRevFactor;           /**< bit reversal modifier that supports different size FFTs with the same bit reversal table. */
  } csky_cfft_radix4_instance_q15;

  /**
   * @brief Instance structure for the Radix-2 Q31 CFFT/CIFFT function.
   */
  typedef struct
  {
    uint16_t fftLen;                 /**< length of the FFT. */
    uint8_t ifftFlag;                /**< flag that selects forward (ifftFlag=0) or inverse (ifftFlag=1) transform. */
    uint8_t bitReverseFlag;          /**< flag that enables (bitReverseFlag=1) or disables (bitReverseFlag=0) bit reversal of output. */
    q31_t *pTwiddle;                 /**< points to the Twiddle factor table. */
    uint16_t *pBitRevTable;          /**< points to the bit reversal table. */
    uint16_t twidCoefModifier;       /**< twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. */
    uint16_t bitRevFactor;           /**< bit reversal modifier that supports different size FFTs with the same bit reversal table. */
  } csky_cfft_radix2_instance_q31;

  /**
   * @brief Instance structure for the Q31 CFFT/CIFFT function.
   */
  typedef struct
  {
    uint16_t fftLen;                 /**< length of the FFT. */
    uint8_t ifftFlag;                /**< flag that selects forward (ifftFlag=0) or inverse (ifftFlag=1) transform. */
    uint8_t bitReverseFlag;          /**< flag that enables (bitReverseFlag=1) or disables (bitReverseFlag=0) bit reversal of output. */
    q31_t *pTwiddle;                 /**< points to the twiddle factor table. */
    uint16_t *pBitRevTable;          /**< points to the bit reversal table. */
    uint16_t twidCoefModifier;       /**< twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. */
    uint16_t bitRevFactor;           /**< bit reversal modifier that supports different size FFTs with the same bit reversal table. */
  } csky_cfft_radix4_instance_q31;

  /**
   * @brief Instance structure for the floating-point CFFT/CIFFT function.
   */
  typedef struct
  {
    uint16_t fftLen;                   /**< length of the FFT. */
    uint8_t ifftFlag;                  /**< flag that selects forward (ifftFlag=0) or inverse (ifftFlag=1) transform. */
    uint8_t bitReverseFlag;            /**< flag that enables (bitReverseFlag=1) or disables (bitReverseFlag=0) bit reversal of output. */
    float32_t *pTwiddle;               /**< points to the Twiddle factor table. */
    uint16_t *pBitRevTable;            /**< points to the bit reversal table. */
    uint16_t twidCoefModifier;         /**< twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. */
    uint16_t bitRevFactor;             /**< bit reversal modifier that supports different size FFTs with the same bit reversal table. */
    float32_t onebyfftLen;             /**< value of 1/fftLen. */
  } csky_cfft_radix2_instance_f32;

  /**
   * @brief Instance structure for the floating-point CFFT/CIFFT function.
   */
  typedef struct
  {
    uint16_t fftLen;                   /**< length of the FFT. */
    uint8_t ifftFlag;                  /**< flag that selects forward (ifftFlag=0) or inverse (ifftFlag=1) transform. */
    uint8_t bitReverseFlag;            /**< flag that enables (bitReverseFlag=1) or disables (bitReverseFlag=0) bit reversal of output. */
    float32_t *pTwiddle;               /**< points to the Twiddle factor table. */
    uint16_t *pBitRevTable;            /**< points to the bit reversal table. */
    uint16_t twidCoefModifier;         /**< twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. */
    uint16_t bitRevFactor;             /**< bit reversal modifier that supports different size FFTs with the same bit reversal table. */
    float32_t onebyfftLen;             /**< value of 1/fftLen. */
  } csky_cfft_radix4_instance_f32;

  /**
   * @brief Instance structure for the fixed-point CFFT/CIFFT function.
   */
  typedef struct
  {
    uint16_t fftLen;                   /**< length of the FFT. */
    const q15_t *pTwiddle;             /**< points to the Twiddle factor table. */
    const uint16_t *pBitRevTable;      /**< points to the bit reversal table. */
    uint16_t bitRevLength;             /**< bit reversal table length. */
  } csky_cfft_instance_q15;

void csky_cfft_q15(
    const csky_cfft_instance_q15 * S,
    q15_t * p1,
    uint8_t ifftFlag,
    uint8_t bitReverseFlag);

  /**
   * @brief Instance structure for the fixed-point CFFT/CIFFT function.
   */
  typedef struct
  {
    uint16_t fftLen;                   /**< length of the FFT. */
    const q31_t *pTwiddle;             /**< points to the Twiddle factor table. */
    const uint16_t *pBitRevTable;      /**< points to the bit reversal table. */
    uint16_t bitRevLength;             /**< bit reversal table length. */
  } csky_cfft_instance_q31;

void csky_cfft_q31(
    const csky_cfft_instance_q31 * S,
    q31_t * p1,
    uint8_t ifftFlag,
    uint8_t bitReverseFlag);

  /**
   * @brief Instance structure for the floating-point CFFT/CIFFT function.
   */
  typedef struct
  {
    uint16_t fftLen;                   /**< length of the FFT. */
    const float32_t *pTwiddle;         /**< points to the Twiddle factor table. */
    const uint16_t *pBitRevTable;      /**< points to the bit reversal table. */
    uint16_t bitRevLength;             /**< bit reversal table length. */
  } csky_cfft_instance_f32;

  void csky_cfft_f32(
  const csky_cfft_instance_f32 * S,
  float32_t * p1,
  uint8_t ifftFlag,
  uint8_t bitReverseFlag);

  /**
   * @brief Instance structure for the Q15 RFFT/RIFFT function.
   */
  typedef struct
  {
    uint32_t fftLenReal;                      /**< length of the real FFT. */
    uint8_t ifftFlagR;                        /**< flag that selects forward (ifftFlagR=0) or inverse (ifftFlagR=1) transform. */
    uint8_t bitReverseFlagR;                  /**< flag that enables (bitReverseFlagR=1) or disables (bitReverseFlagR=0) bit reversal of output. */
    uint32_t twidCoefRModifier;               /**< twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. */
    q15_t *pTwiddleAReal;                     /**< points to the real twiddle factor table. */
    const csky_cfft_instance_q15 *pCfft;      /**< points to the complex FFT instance. */
  } csky_rfft_instance_q15;

  csky_status csky_rfft_init_q15(
  csky_rfft_instance_q15 * S,
  uint32_t fftLenReal,
  uint32_t ifftFlagR,
  uint32_t bitReverseFlag);

  void csky_rfft_q15(
  const csky_rfft_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst);

  /**
   * @brief Instance structure for the Q31 RFFT/RIFFT function.
   */
  typedef struct
  {
    uint32_t fftLenReal;                        /**< length of the real FFT. */
    uint8_t ifftFlagR;                          /**< flag that selects forward (ifftFlagR=0) or inverse (ifftFlagR=1) transform. */
    uint8_t bitReverseFlagR;                    /**< flag that enables (bitReverseFlagR=1) or disables (bitReverseFlagR=0) bit reversal of output. */
    uint32_t twidCoefRModifier;                 /**< twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. */
    q31_t *pTwiddleAReal;                       /**< points to the real twiddle factor table. */
    const csky_cfft_instance_q31 *pCfft;        /**< points to the complex FFT instance. */
  } csky_rfft_instance_q31;

  csky_status csky_rfft_init_q31(
  csky_rfft_instance_q31 * S,
  uint32_t fftLenReal,
  uint32_t ifftFlagR,
  uint32_t bitReverseFlag);

  void csky_rfft_q31(
  const csky_rfft_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst);

  /**
   * @brief Instance structure for the floating-point RFFT/RIFFT function.
   */
  typedef struct
  {
    uint32_t fftLenReal;                        /**< length of the real FFT. */
    uint16_t fftLenBy2;                         /**< length of the complex FFT. */
    uint8_t ifftFlagR;                          /**< flag that selects forward (ifftFlagR=0) or inverse (ifftFlagR=1) transform. */
    uint8_t bitReverseFlagR;                    /**< flag that enables (bitReverseFlagR=1) or disables (bitReverseFlagR=0) bit reversal of output. */
    uint32_t twidCoefRModifier;                 /**< twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. */
    float32_t *pTwiddleAReal;                   /**< points to the real twiddle factor table. */
    float32_t *pTwiddleBReal;                   /**< points to the imag twiddle factor table. */
    csky_cfft_radix4_instance_f32 *pCfft;       /**< points to the complex FFT instance. */
  } csky_rfft_instance_f32;

  csky_status csky_rfft_init_f32(
  csky_rfft_instance_f32 * S,
  csky_cfft_radix4_instance_f32 * S_CFFT,
  uint32_t fftLenReal,
  uint32_t ifftFlagR,
  uint32_t bitReverseFlag);

  void csky_rfft_f32(
  const csky_rfft_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst);

  /**
   * @brief Instance structure for the floating-point RFFT/RIFFT function.
   */
typedef struct
  {
    csky_cfft_instance_f32 Sint;     /**< Internal CFFT structure. */
    uint16_t fftLenRFFT;             /**< length of the real sequence */
    float32_t * pTwiddleRFFT;        /**< Twiddle factors real stage  */
  } csky_rfft_fast_instance_f32 ;

csky_status csky_rfft_fast_init_f32 (
   csky_rfft_fast_instance_f32 * S,
   uint16_t fftLen);

void csky_rfft_fast_f32(
  csky_rfft_fast_instance_f32 * S,
  float32_t * p, float32_t * pOut,
  uint8_t ifftFlag);

  /**
   * @brief Instance structure for the floating-point DCT4/IDCT4 function.
   */
  typedef struct
  {
    uint16_t N;                           /**< length of the DCT4. */
    uint16_t Nby2;                        /**< half of the length of the DCT4. */
    float32_t normalize;                  /**< normalizing factor. */
    float32_t *pTwiddle;                  /**< points to the twiddle factor table. */
    float32_t *pCosFactor;                /**< points to the cosFactor table. */
    csky_rfft_fast_instance_f32 *pRfft;   /**< points to the real FFT fast instance. */
    csky_cfft_radix4_instance_f32 *pCfft; /**< points to the complex FFT instance. */
  } csky_dct4_instance_f32;

  csky_status csky_dct4_init_f32(
  csky_dct4_instance_f32 * S,
  csky_rfft_fast_instance_f32 * S_RFFT,
  csky_cfft_radix4_instance_f32 * S_CFFT,
  uint16_t N,
  uint16_t Nby2,
  float32_t normalize);

  void csky_dct4_f32(
  const csky_dct4_instance_f32 * S,
  float32_t * pState,
  float32_t * pInlineBuffer);


  /**
   * @brief Instance structure for the Q31 DCT4/IDCT4 function.
   */
  typedef struct
  {
    uint16_t N;                           /**< length of the DCT4. */
    uint16_t Nby2;                        /**< half of the length of the DCT4. */
    q31_t normalize;                      /**< normalizing factor. */
    q31_t *pTwiddle;                      /**< points to the twiddle factor table. */
    q31_t *pCosFactor;                    /**< points to the cosFactor table. */
    csky_rfft_instance_q31 *pRfft;        /**< points to the real FFT instance. */
    csky_cfft_radix4_instance_q31 *pCfft; /**< points to the complex FFT instance. */
  } csky_dct4_instance_q31;

  csky_status csky_dct4_init_q31(
  csky_dct4_instance_q31 * S,
  csky_rfft_instance_q31 * S_RFFT,
  csky_cfft_radix4_instance_q31 * S_CFFT,
  uint16_t N,
  uint16_t Nby2,
  q31_t normalize);

  void csky_dct4_q31(
  const csky_dct4_instance_q31 * S,
  q31_t * pState,
  q31_t * pInlineBuffer);

  /**
   * @brief Instance structure for the Q15 DCT4/IDCT4 function.
   */
  typedef struct
  {
    uint16_t N;                           /**< length of the DCT4. */
    uint16_t Nby2;                        /**< half of the length of the DCT4. */
    q15_t normalize;                      /**< normalizing factor. */
    q15_t *pTwiddle;                      /**< points to the twiddle factor table. */
    q15_t *pCosFactor;                    /**< points to the cosFactor table. */
    csky_rfft_instance_q15 *pRfft;        /**< points to the real FFT instance. */
    csky_cfft_radix4_instance_q15 *pCfft; /**< points to the complex FFT instance. */
  } csky_dct4_instance_q15;

  csky_status csky_dct4_init_q15(
  csky_dct4_instance_q15 * S,
  csky_rfft_instance_q15 * S_RFFT,
  csky_cfft_radix4_instance_q15 * S_CFFT,
  uint16_t N,
  uint16_t Nby2,
  q15_t normalize);

  void csky_dct4_q15(
  const csky_dct4_instance_q15 * S,
  q15_t * pState,
  q15_t * pInlineBuffer);

  void csky_add_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_add_q7(
  q7_t * pSrcA,
  q7_t * pSrcB,
  q7_t * pDst,
  uint32_t blockSize);

  void csky_add_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_add_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_sub_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_sub_q7(
  q7_t * pSrcA,
  q7_t * pSrcB,
  q7_t * pDst,
  uint32_t blockSize);

  void csky_sub_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_sub_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_scale_f32(
  float32_t * pSrc,
  float32_t scale,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_scale_q7(
  q7_t * pSrc,
  q7_t scaleFract,
  int8_t shift,
  q7_t * pDst,
  uint32_t blockSize);

  void csky_scale_q15(
  q15_t * pSrc,
  q15_t scaleFract,
  int8_t shift,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_scale_q31(
  q31_t * pSrc,
  q31_t scaleFract,
  int8_t shift,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_abs_q7(
  q7_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);

  void csky_abs_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_abs_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_abs_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_abs_max_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_abs_max_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  void csky_dot_prod_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  uint32_t blockSize,
  float32_t * result);

  void csky_dot_prod_q7(
  q7_t * pSrcA,
  q7_t * pSrcB,
  uint32_t blockSize,
  q31_t * result);

  void csky_dot_prod_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  uint32_t blockSize,
  q63_t * result);

  void csky_dot_prod_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  uint32_t blockSize,
  q63_t * result);

  void csky_shift_q7(
  q7_t * pSrc,
  int8_t shiftBits,
  q7_t * pDst,
  uint32_t blockSize);

  void csky_shift_q15(
  q15_t * pSrc,
  int8_t shiftBits,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_shift_q31(
  q31_t * pSrc,
  int8_t shiftBits,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_offset_f32(
  float32_t * pSrc,
  float32_t offset,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_offset_q7(
  q7_t * pSrc,
  q7_t offset,
  q7_t * pDst,
  uint32_t blockSize);

  void csky_offset_q15(
  q15_t * pSrc,
  q15_t offset,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_offset_q31(
  q31_t * pSrc,
  q31_t offset,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_negate_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_negate_q7(
  q7_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);

  void csky_negate_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_negate_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_copy_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_copy_q7(
  q7_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);

  void csky_copy_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_copy_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_fill_f32(
  float32_t value,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_fill_q7(
  q7_t value,
  q7_t * pDst,
  uint32_t blockSize);

  void csky_fill_q15(
  q15_t value,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_fill_q31(
  q31_t value,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_conv_f32(
  float32_t * pSrcA,
  uint32_t srcALen,
  float32_t * pSrcB,
  uint32_t srcBLen,
  float32_t * pDst);

  void csky_conv_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  q15_t * pScratch1,
  q15_t * pScratch2);

  void csky_conv_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst);

  void csky_conv_fast_q15(
          q15_t * pSrcA,
          uint32_t srcALen,
          q15_t * pSrcB,
          uint32_t srcBLen,
          q15_t * pDst);

  void csky_conv_fast_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  q15_t * pScratch1,
  q15_t * pScratch2);

  void csky_conv_q31(
  q31_t * pSrcA,
  uint32_t srcALen,
  q31_t * pSrcB,
  uint32_t srcBLen,
  q31_t * pDst);

  void csky_conv_fast_q31(
  q31_t * pSrcA,
  uint32_t srcALen,
  q31_t * pSrcB,
  uint32_t srcBLen,
  q31_t * pDst);

  void csky_conv_opt_q7(
  q7_t * pSrcA,
  uint32_t srcALen,
  q7_t * pSrcB,
  uint32_t srcBLen,
  q7_t * pDst,
  q15_t * pScratch1,
  q15_t * pScratch2);

  void csky_conv_q7(
  q7_t * pSrcA,
  uint32_t srcALen,
  q7_t * pSrcB,
  uint32_t srcBLen,
  q7_t * pDst);

  csky_status csky_conv_partial_f32(
  float32_t * pSrcA,
  uint32_t srcALen,
  float32_t * pSrcB,
  uint32_t srcBLen,
  float32_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints);

  csky_status csky_conv_partial_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints,
  q15_t * pScratch1,
  q15_t * pScratch2);

  csky_status csky_conv_partial_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints);

  csky_status csky_conv_partial_fast_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints);

  csky_status csky_conv_partial_fast_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints,
  q15_t * pScratch1,
  q15_t * pScratch2);

  csky_status csky_conv_partial_q31(
  q31_t * pSrcA,
  uint32_t srcALen,
  q31_t * pSrcB,
  uint32_t srcBLen,
  q31_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints);

  csky_status csky_conv_partial_fast_q31(
  q31_t * pSrcA,
  uint32_t srcALen,
  q31_t * pSrcB,
  uint32_t srcBLen,
  q31_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints);

  csky_status csky_conv_partial_opt_q7(
  q7_t * pSrcA,
  uint32_t srcALen,
  q7_t * pSrcB,
  uint32_t srcBLen,
  q7_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints,
  q15_t * pScratch1,
  q15_t * pScratch2);

  csky_status csky_conv_partial_q7(
  q7_t * pSrcA,
  uint32_t srcALen,
  q7_t * pSrcB,
  uint32_t srcBLen,
  q7_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints);

  /**
   * functions for the yunVoice functions.
   */
   q15_t csky_dsp_lib_vec_max_abs16(
   q15_t  * A,
   uint32_t N);

   q31_t csky_dsp_lib_vec_max_abs32(
   q31_t  * A,
   uint32_t N);

   void csky_dsp_lib_vec_abs16(
   q15_t  * A,
   uint32_t N,
   q15_t  * C);

   void csky_dsp_lib_vec_abs32(
   q31_t * A,
   uint32_t N,
   q31_t * C);

   void csky_dsp_lib_vec_add16(
   q15_t * A,
   q15_t * B,
   uint32_t N,
   q15_t * C);

   void csky_dsp_lib_vec_add32(
   q31_t * A,
   q31_t * B,
   uint32_t N,
   q31_t * C);

   void csky_dsp_lib_vec_cx_conj_q15(
   q15_t * A,
   uint32_t N,
   q15_t * B);

   void csky_dsp_lib_vec_cx_conj_q31(
   q31_t * A,
   uint32_t N,
   q31_t * C);

   q31_t csky_dsp_lib_vec_dot_q15(
   q15_t * A,
   q15_t * B,
   uint32_t N);

   q31_t csky_dsp_lib_vec_dot_q31(
   q31_t * A,
   q31_t * B,
   uint32_t N);

   void csky_dsp_lib_mat_cx_add16(
   cq15_t * A,
   cq15_t * B,
   uint32_t N,
   uint32_t M,
   cq15_t * C);

   void csky_dsp_lib_mat_cx_add32(
   cq31_t * A,
   cq31_t * B,
   uint32_t N,
   uint32_t M,
   cq31_t * C);

   void csky_dsp_lib_mat_cx_mul_q15(
   cq15_t * A,
   cq15_t * B,
   uint32_t N,
   uint32_t M,
   uint32_t L,
   cq15_t * C);

   void csky_dsp_lib_mat_cx_mul_q31(
   cq31_t * A,
   cq31_t * B,
   uint32_t N,
   uint32_t M,
   uint32_t L,
   cq31_t * C);

   void csky_dsp_lib_mat_cx_sub16(
   cq15_t * A,
   cq15_t * B,
   uint32_t N,
   uint32_t M,
   cq15_t * C);

   void csky_dsp_lib_mat_cx_sub32(
   cq31_t * A,
   cq31_t * B,
   uint32_t N,
   uint32_t M,
   cq31_t * C);

   void csky_dsp_lib_vec_mul_q15(
   q15_t * A,
   q15_t * B,
   uint32_t N,
   q15_t * C);

   void csky_dsp_lib_vec_mul_q31(
   q31_t * A,
   q31_t * B,
   uint32_t N,
   q31_t * C);

   q31_t csky_dsp_lib_pow_int32(
   q31_t arg_in_x,
   q15_t arg_exp_in_x,
   q31_t arg_in_y,
   q15_t arg_exp_in_y,
   q31_t *arg_exp_out);

   void csky_dsp_lib_vec_scale_q15(
   q15_t * A,
   q15_t scaleFract,
   int8_t shift,
   q15_t * B,
   uint32_t N);

   void csky_dsp_lib_vec_scale_q31(
   q31_t * A,
   q31_t scaleFract,
   int8_t shift,
   q31_t * B,
   uint32_t N);

   void csky_dsp_lib_vec_shf16(
   q15_t * A,
   int8_t shift_val,
   uint32_t N,
   q15_t * C);

   void csky_dsp_lib_vec_shf32(
   q31_t * A,
   q31_t shift_val,
   uint32_t N,
   q31_t * C);

   q15_t csky_dsp_lib_sqrt_int32(
   q31_t x,
   uint32_t rnd_flag);

   void csky_dsp_lib_vec_sub16(
   q15_t * A,
   q15_t * B,
   uint32_t N,
   q15_t * C);

   void csky_dsp_lib_vec_sub32(
   q31_t * A,
   q31_t * B,
   uint32_t N,
   q31_t * C);

   q63_t csky_dsp_lib_vec_sum16(
   q15_t * A,
   uint32_t N);

   q63_t csky_dsp_lib_vec_sum32(
   q31_t * A,
   uint32_t N);

    void csky_fft_lib_cx16_fft(
    q31_t log2_buf_len,
    q15_t * in_buf,
    q15_t * out_buf,
    const q15_t * twi_table,
    const uint16_t * bitrev_tbl,
    q15_t * temp_buf,
    q7_t  * ScaleShift,
    q31_t br);

    void csky_fft_lib_cx32_fft(
    q31_t log2_buf_len,
    q31_t * in_buf,
    q31_t * out_buf,
    const q31_t * twi_table,
    const uint16_t * bitrev_tbl,
    q31_t * temp_buf,
    q31_t br);

    void csky_fft_lib_cx16_ifft(
    q31_t log2_buf_len,
    q15_t * in_buf,
    q15_t * out_buf,
    const q15_t * twi_table,
    const uint16_t * bitrev_tbl,
    q15_t * temp_buf,
    q7_t  * ScaleShift,
    q31_t br);

    void csky_fft_lib_cx32_ifft(
    q31_t log2_buf_len,
    q31_t * in_buf,
    q31_t * out_buf,
    const q31_t * twi_table,
    const uint16_t * bitrev_tbl,
    q31_t * temp_buf,
    q31_t br);

    void csky_fft_lib_int16_fft(
    q31_t log2_buf_len,
    q15_t * in_buf,
    q15_t * out_buf,
    const q15_t * twi_table,
    const q15_t * last_stage_twi_table,
    const uint16_t * bitrev_tbl,
    q15_t * temp_buf,
    q7_t  * ScaleShift,
    q31_t br);

    void csky_fft_lib_int32_fft(
    q31_t log2_buf_len,
    q31_t * in_buf,
    q31_t * out_buf,
    const q31_t * twi_table,
    const q31_t * last_stage_twi_table,
    const uint16_t * bitrev_tbl,
    q31_t * temp_buf,
    q31_t br);

    void csky_fft_lib_int16_ifft(
    q31_t log2_buf_len,
    q15_t * in_buf,
    q15_t * out_buf,
    const q15_t * twi_table,
    const q15_t * last_stage_twi_table,
    const uint16_t * bitrev_tbl,
    q15_t * temp_buf,
    q7_t  * ScaleShift,
    q31_t br);

    void csky_fft_lib_int32_ifft(
    q31_t log2_buf_len,
    q31_t * in_buf,
    q31_t * out_buf,
    const q31_t * twi_table,
    const q31_t * last_stage_twi_table,
    const uint16_t * bitrev_tbl,
    q31_t * temp_buf,
    q31_t br);

  /**
   * @brief Instance structure for the Q15 FIR decimator.
   */
  typedef struct
  {
    uint8_t M;                  /**< decimation factor. */
    uint16_t numTaps;           /**< number of coefficients in the filter. */
    q15_t *pCoeffs;             /**< points to the coefficient array. The array is of length numTaps.*/
    q15_t *pState;              /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
  } csky_fir_decimate_instance_q15;

  /**
   * @brief Instance structure for the Q31 FIR decimator.
   */
  typedef struct
  {
    uint8_t M;                  /**< decimation factor. */
    uint16_t numTaps;           /**< number of coefficients in the filter. */
    q31_t *pCoeffs;             /**< points to the coefficient array. The array is of length numTaps.*/
    q31_t *pState;              /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
  } csky_fir_decimate_instance_q31;

  /**
   * @brief Instance structure for the floating-point FIR decimator.
   */
  typedef struct
  {
    uint8_t M;                  /**< decimation factor. */
    uint16_t numTaps;           /**< number of coefficients in the filter. */
    float32_t *pCoeffs;         /**< points to the coefficient array. The array is of length numTaps.*/
    float32_t *pState;          /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
  } csky_fir_decimate_instance_f32;

  void csky_fir_decimate_f32(
  const csky_fir_decimate_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

  csky_status csky_fir_decimate_init_f32(
  csky_fir_decimate_instance_f32 * S,
  uint16_t numTaps,
  uint8_t M,
  float32_t * pCoeffs,
  float32_t * pState,
  uint32_t blockSize);

  void csky_fir_decimate_q15(
  const csky_fir_decimate_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_fir_decimate_fast_q15(
  const csky_fir_decimate_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  csky_status csky_fir_decimate_init_q15(
  csky_fir_decimate_instance_q15 * S,
  uint16_t numTaps,
  uint8_t M,
  q15_t * pCoeffs,
  q15_t * pState,
  uint32_t blockSize);

  void csky_fir_decimate_q31(
  const csky_fir_decimate_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_fir_decimate_fast_q31(
  csky_fir_decimate_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  csky_status csky_fir_decimate_init_q31(
  csky_fir_decimate_instance_q31 * S,
  uint16_t numTaps,
  uint8_t M,
  q31_t * pCoeffs,
  q31_t * pState,
  uint32_t blockSize);


  /**
   * @brief Instance structure for the Q15 FIR interpolator.
   */
  typedef struct
  {
    uint8_t L;                      /**< upsample factor. */
    uint16_t phaseLength;           /**< length of each polyphase filter component. */
    q15_t *pCoeffs;                 /**< points to the coefficient array. The array is of length L*phaseLength. */
    q15_t *pState;                  /**< points to the state variable array. The array is of length blockSize+phaseLength-1. */
  } csky_fir_interpolate_instance_q15;

  /**
   * @brief Instance structure for the Q31 FIR interpolator.
   */
  typedef struct
  {
    uint8_t L;                      /**< upsample factor. */
    uint16_t phaseLength;           /**< length of each polyphase filter component. */
    q31_t *pCoeffs;                 /**< points to the coefficient array. The array is of length L*phaseLength. */
    q31_t *pState;                  /**< points to the state variable array. The array is of length blockSize+phaseLength-1. */
  } csky_fir_interpolate_instance_q31;

  /**
   * @brief Instance structure for the floating-point FIR interpolator.
   */
  typedef struct
  {
    uint8_t L;                     /**< upsample factor. */
    uint16_t phaseLength;          /**< length of each polyphase filter component. */
    float32_t *pCoeffs;            /**< points to the coefficient array. The array is of length L*phaseLength. */
    float32_t *pState;             /**< points to the state variable array. The array is of length phaseLength+numTaps-1. */
  } csky_fir_interpolate_instance_f32;

  void csky_fir_interpolate_q15(
  const csky_fir_interpolate_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  csky_status csky_fir_interpolate_init_q15(
  csky_fir_interpolate_instance_q15 * S,
  uint8_t L,
  uint16_t numTaps,
  q15_t * pCoeffs,
  q15_t * pState,
  uint32_t blockSize);

  void csky_fir_interpolate_q31(
  const csky_fir_interpolate_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  csky_status csky_fir_interpolate_init_q31(
  csky_fir_interpolate_instance_q31 * S,
  uint8_t L,
  uint16_t numTaps,
  q31_t * pCoeffs,
  q31_t * pState,
  uint32_t blockSize);

  void csky_fir_interpolate_f32(
  const csky_fir_interpolate_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

  csky_status csky_fir_interpolate_init_f32(
  csky_fir_interpolate_instance_f32 * S,
  uint8_t L,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  uint32_t blockSize);


  /**
   * @brief Instance structure for the high precision Q31 Biquad cascade filter.
   */
  typedef struct
  {
    uint8_t numStages;       /**< number of 2nd order stages in the filter.  Overall order is 2*numStages. */
    q63_t *pState;           /**< points to the array of state coefficients.  The array is of length 4*numStages. */
    q31_t *pCoeffs;          /**< points to the array of coefficients.  The array is of length 5*numStages. */
    uint8_t postShift;       /**< additional shift, in bits, applied to each output sample. */
  } csky_biquad_cas_df1_32x64_ins_q31;

  void csky_biquad_cas_df1_32x64_q31(
  const csky_biquad_cas_df1_32x64_ins_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_biquad_cas_df1_32x64_init_q31(
  csky_biquad_cas_df1_32x64_ins_q31 * S,
  uint8_t numStages,
  q31_t * pCoeffs,
  q63_t * pState,
  uint8_t postShift);


  /**
   * @brief Instance structure for the floating-point transposed direct form II Biquad cascade filter.
   */
  typedef struct
  {
    uint8_t numStages;         /**< number of 2nd order stages in the filter.  Overall order is 2*numStages. */
    float32_t *pState;         /**< points to the array of state coefficients.  The array is of length 2*numStages. */
    float32_t *pCoeffs;        /**< points to the array of coefficients.  The array is of length 5*numStages. */
  } csky_biquad_cascade_df2T_instance_f32;

  /**
   * @brief Instance structure for the floating-point transposed direct form II Biquad cascade filter.
   */
  typedef struct
  {
    uint8_t numStages;         /**< number of 2nd order stages in the filter.  Overall order is 2*numStages. */
    float32_t *pState;         /**< points to the array of state coefficients.  The array is of length 4*numStages. */
    float32_t *pCoeffs;        /**< points to the array of coefficients.  The array is of length 5*numStages. */
  } csky_biquad_cascade_stereo_df2T_instance_f32;

  /**
   * @brief Instance structure for the floating-point transposed direct form II Biquad cascade filter.
   */
  typedef struct
  {
    uint8_t numStages;         /**< number of 2nd order stages in the filter.  Overall order is 2*numStages. */
    float64_t *pState;         /**< points to the array of state coefficients.  The array is of length 2*numStages. */
    float64_t *pCoeffs;        /**< points to the array of coefficients.  The array is of length 5*numStages. */
  } csky_biquad_cascade_df2T_instance_f64;

  void csky_biquad_cascade_df2T_f32(
  const csky_biquad_cascade_df2T_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_biquad_cascade_stereo_df2T_f32(
  const csky_biquad_cascade_stereo_df2T_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_biquad_cascade_df2T_f64(
  const csky_biquad_cascade_df2T_instance_f64 * S,
  float64_t * pSrc,
  float64_t * pDst,
  uint32_t blockSize);

  void csky_biquad_cascade_df2T_init_f32(
  csky_biquad_cascade_df2T_instance_f32 * S,
  uint8_t numStages,
  float32_t * pCoeffs,
  float32_t * pState);

  void csky_biquad_cascade_stereo_df2T_init_f32(
  csky_biquad_cascade_stereo_df2T_instance_f32 * S,
  uint8_t numStages,
  float32_t * pCoeffs,
  float32_t * pState);


  void csky_biquad_cascade_df2T_init_f64(
  csky_biquad_cascade_df2T_instance_f64 * S,
  uint8_t numStages,
  float64_t * pCoeffs,
  float64_t * pState);


  /**
   * @brief Instance structure for the Q15 FIR lattice filter.
   */
  typedef struct
  {
    uint16_t numStages;                  /**< number of filter stages. */
    q15_t *pState;                       /**< points to the state variable array. The array is of length numStages. */
    q15_t *pCoeffs;                      /**< points to the coefficient array. The array is of length numStages. */
  } csky_fir_lattice_instance_q15;

  /**
   * @brief Instance structure for the Q31 FIR lattice filter.
   */
  typedef struct
  {
    uint16_t numStages;                  /**< number of filter stages. */
    q31_t *pState;                       /**< points to the state variable array. The array is of length numStages. */
    q31_t *pCoeffs;                      /**< points to the coefficient array. The array is of length numStages. */
  } csky_fir_lattice_instance_q31;

  /**
   * @brief Instance structure for the floating-point FIR lattice filter.
   */
  typedef struct
  {
    uint16_t numStages;                  /**< number of filter stages. */
    float32_t *pState;                   /**< points to the state variable array. The array is of length numStages. */
    float32_t *pCoeffs;                  /**< points to the coefficient array. The array is of length numStages. */
  } csky_fir_lattice_instance_f32;

  void csky_fir_lattice_init_q15(
  csky_fir_lattice_instance_q15 * S,
  uint16_t numStages,
  q15_t * pCoeffs,
  q15_t * pState);

  void csky_fir_lattice_q15(
  const csky_fir_lattice_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_fir_lattice_init_q31(
  csky_fir_lattice_instance_q31 * S,
  uint16_t numStages,
  q31_t * pCoeffs,
  q31_t * pState);

  void csky_fir_lattice_q31(
  const csky_fir_lattice_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_fir_lattice_init_f32(
  csky_fir_lattice_instance_f32 * S,
  uint16_t numStages,
  float32_t * pCoeffs,
  float32_t * pState);

  void csky_fir_lattice_f32(
  const csky_fir_lattice_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  /**
   * @brief Instance structure for the Q15 IIR lattice filter.
   */
  typedef struct
  {
    uint16_t numStages;                  /**< number of stages in the filter. */
    q15_t *pState;                       /**< points to the state variable array. The array is of length numStages+blockSize. */
    q15_t *pkCoeffs;                     /**< points to the reflection coefficient array. The array is of length numStages. */
    q15_t *pvCoeffs;                     /**< points to the ladder coefficient array. The array is of length numStages+1. */
  } csky_iir_lattice_instance_q15;

  /**
   * @brief Instance structure for the Q31 IIR lattice filter.
   */
  typedef struct
  {
    uint16_t numStages;                  /**< number of stages in the filter. */
    q31_t *pState;                       /**< points to the state variable array. The array is of length numStages+blockSize. */
    q31_t *pkCoeffs;                     /**< points to the reflection coefficient array. The array is of length numStages. */
    q31_t *pvCoeffs;                     /**< points to the ladder coefficient array. The array is of length numStages+1. */
  } csky_iir_lattice_instance_q31;

  /**
   * @brief Instance structure for the floating-point IIR lattice filter.
   */
  typedef struct
  {
    uint16_t numStages;                  /**< number of stages in the filter. */
    float32_t *pState;                   /**< points to the state variable array. The array is of length numStages+blockSize. */
    float32_t *pkCoeffs;                 /**< points to the reflection coefficient array. The array is of length numStages. */
    float32_t *pvCoeffs;                 /**< points to the ladder coefficient array. The array is of length numStages+1. */
  } csky_iir_lattice_instance_f32;

  void csky_iir_lattice_f32(
  const csky_iir_lattice_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_iir_lattice_init_f32(
  csky_iir_lattice_instance_f32 * S,
  uint16_t numStages,
  float32_t * pkCoeffs,
  float32_t * pvCoeffs,
  float32_t * pState,
  uint32_t blockSize);

  void csky_iir_lattice_q31(
  const csky_iir_lattice_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_iir_lattice_init_q31(
  csky_iir_lattice_instance_q31 * S,
  uint16_t numStages,
  q31_t * pkCoeffs,
  q31_t * pvCoeffs,
  q31_t * pState,
  uint32_t blockSize);

  void csky_iir_lattice_q15(
  const csky_iir_lattice_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_iir_lattice_init_q15(
  csky_iir_lattice_instance_q15 * S,
  uint16_t numStages,
  q15_t * pkCoeffs,
  q15_t * pvCoeffs,
  q15_t * pState,
  uint32_t blockSize);


  /**
   * @brief Instance structure for the floating-point LMS filter.
   */
  typedef struct
  {
    uint16_t numTaps;    /**< number of coefficients in the filter. */
    float32_t *pState;   /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    float32_t *pCoeffs;  /**< points to the coefficient array. The array is of length numTaps. */
    float32_t mu;        /**< step size that controls filter coefficient updates. */
  } csky_lms_instance_f32;

  void csky_lms_f32(
  const csky_lms_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pRef,
  float32_t * pOut,
  float32_t * pErr,
  uint32_t blockSize);

  void csky_lms_init_f32(
  csky_lms_instance_f32 * S,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  float32_t mu,
  uint32_t blockSize);


  /**
   * @brief Instance structure for the Q15 LMS filter.
   */
  typedef struct
  {
    uint16_t numTaps;    /**< number of coefficients in the filter. */
    q15_t *pState;       /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    q15_t *pCoeffs;      /**< points to the coefficient array. The array is of length numTaps. */
    q15_t mu;            /**< step size that controls filter coefficient updates. */
    uint32_t postShift;  /**< bit shift applied to coefficients. */
  } csky_lms_instance_q15;

  void csky_lms_init_q15(
  csky_lms_instance_q15 * S,
  uint16_t numTaps,
  q15_t * pCoeffs,
  q15_t * pState,
  q15_t mu,
  uint32_t blockSize,
  uint32_t postShift);

  void csky_lms_q15(
  const csky_lms_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pRef,
  q15_t * pOut,
  q15_t * pErr,
  uint32_t blockSize);


  /**
   * @brief Instance structure for the Q31 LMS filter.
   */
  typedef struct
  {
    uint16_t numTaps;    /**< number of coefficients in the filter. */
    q31_t *pState;       /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    q31_t *pCoeffs;      /**< points to the coefficient array. The array is of length numTaps. */
    q31_t mu;            /**< step size that controls filter coefficient updates. */
    uint32_t postShift;  /**< bit shift applied to coefficients. */
  } csky_lms_instance_q31;

  void csky_lms_q31(
  const csky_lms_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pRef,
  q31_t * pOut,
  q31_t * pErr,
  uint32_t blockSize);

  void csky_lms_init_q31(
  csky_lms_instance_q31 * S,
  uint16_t numTaps,
  q31_t * pCoeffs,
  q31_t * pState,
  q31_t mu,
  uint32_t blockSize,
  uint32_t postShift);


  /**
   * @brief Instance structure for the floating-point normalized LMS filter.
   */
  typedef struct
  {
    uint16_t numTaps;     /**< number of coefficients in the filter. */
    float32_t *pState;    /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    float32_t *pCoeffs;   /**< points to the coefficient array. The array is of length numTaps. */
    float32_t mu;         /**< step size that control filter coefficient updates. */
    float32_t energy;     /**< saves previous frame energy. */
    float32_t x0;         /**< saves previous input sample. */
  } csky_lms_norm_instance_f32;

  void csky_lms_norm_f32(
  csky_lms_norm_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pRef,
  float32_t * pOut,
  float32_t * pErr,
  uint32_t blockSize);

  void csky_lms_norm_init_f32(
  csky_lms_norm_instance_f32 * S,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  float32_t mu,
  uint32_t blockSize);


  /**
   * @brief Instance structure for the Q31 normalized LMS filter.
   */
  typedef struct
  {
    uint16_t numTaps;     /**< number of coefficients in the filter. */
    q31_t *pState;        /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    q31_t *pCoeffs;       /**< points to the coefficient array. The array is of length numTaps. */
    q31_t mu;             /**< step size that controls filter coefficient updates. */
    uint8_t postShift;    /**< bit shift applied to coefficients. */
    q31_t *recipTable;    /**< points to the reciprocal initial value table. */
    q31_t energy;         /**< saves previous frame energy. */
    q31_t x0;             /**< saves previous input sample. */
  } csky_lms_norm_instance_q31;

  void csky_lms_norm_q31(
  csky_lms_norm_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pRef,
  q31_t * pOut,
  q31_t * pErr,
  uint32_t blockSize);

  void csky_lms_norm_init_q31(
  csky_lms_norm_instance_q31 * S,
  uint16_t numTaps,
  q31_t * pCoeffs,
  q31_t * pState,
  q31_t mu,
  uint32_t blockSize,
  uint8_t postShift);


  /**
   * @brief Instance structure for the Q15 normalized LMS filter.
   */
  typedef struct
  {
    uint16_t numTaps;     /**< Number of coefficients in the filter. */
    q15_t *pState;        /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    q15_t *pCoeffs;       /**< points to the coefficient array. The array is of length numTaps. */
    q15_t mu;             /**< step size that controls filter coefficient updates. */
    uint8_t postShift;    /**< bit shift applied to coefficients. */
    q15_t *recipTable;    /**< Points to the reciprocal initial value table. */
    q15_t energy;         /**< saves previous frame energy. */
    q15_t x0;             /**< saves previous input sample. */
  } csky_lms_norm_instance_q15;

  void csky_lms_norm_q15(
  csky_lms_norm_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pRef,
  q15_t * pOut,
  q15_t * pErr,
  uint32_t blockSize);

  void csky_lms_norm_init_q15(
  csky_lms_norm_instance_q15 * S,
  uint16_t numTaps,
  q15_t * pCoeffs,
  q15_t * pState,
  q15_t mu,
  uint32_t blockSize,
  uint8_t postShift);

  void csky_correlate_f32(
  float32_t * pSrcA,
  uint32_t srcALen,
  float32_t * pSrcB,
  uint32_t srcBLen,
  float32_t * pDst);

  void csky_correlate_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  q15_t * pScratch);

  void csky_correlate_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst);

  void csky_correlate_fast_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst);

  void csky_correlate_fast_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  q15_t * pScratch);

  void csky_correlate_q31(
  q31_t * pSrcA,
  uint32_t srcALen,
  q31_t * pSrcB,
  uint32_t srcBLen,
  q31_t * pDst);

  void csky_correlate_fast_q31(
  q31_t * pSrcA,
  uint32_t srcALen,
  q31_t * pSrcB,
  uint32_t srcBLen,
  q31_t * pDst);

  void csky_correlate_opt_q7(
  q7_t * pSrcA,
  uint32_t srcALen,
  q7_t * pSrcB,
  uint32_t srcBLen,
  q7_t * pDst,
  q15_t * pScratch1,
  q15_t * pScratch2);

  void csky_correlate_q7(
  q7_t * pSrcA,
  uint32_t srcALen,
  q7_t * pSrcB,
  uint32_t srcBLen,
  q7_t * pDst);


  /**
   * @brief Instance structure for the floating-point sparse FIR filter.
   */
  typedef struct
  {
    uint16_t numTaps;             /**< number of coefficients in the filter. */
    uint16_t stateIndex;          /**< state buffer index.  Points to the oldest sample in the state buffer. */
    float32_t *pState;            /**< points to the state buffer array. The array is of length maxDelay+blockSize-1. */
    float32_t *pCoeffs;           /**< points to the coefficient array. The array is of length numTaps.*/
    uint16_t maxDelay;            /**< maximum offset specified by the pTapDelay array. */
    int32_t *pTapDelay;           /**< points to the array of delay values.  The array is of length numTaps. */
  } csky_fir_sparse_instance_f32;

  /**
   * @brief Instance structure for the Q31 sparse FIR filter.
   */
  typedef struct
  {
    uint16_t numTaps;             /**< number of coefficients in the filter. */
    uint16_t stateIndex;          /**< state buffer index.  Points to the oldest sample in the state buffer. */
    q31_t *pState;                /**< points to the state buffer array. The array is of length maxDelay+blockSize-1. */
    q31_t *pCoeffs;               /**< points to the coefficient array. The array is of length numTaps.*/
    uint16_t maxDelay;            /**< maximum offset specified by the pTapDelay array. */
    int32_t *pTapDelay;           /**< points to the array of delay values.  The array is of length numTaps. */
  } csky_fir_sparse_instance_q31;

  /**
   * @brief Instance structure for the Q15 sparse FIR filter.
   */
  typedef struct
  {
    uint16_t numTaps;             /**< number of coefficients in the filter. */
    uint16_t stateIndex;          /**< state buffer index.  Points to the oldest sample in the state buffer. */
    q15_t *pState;                /**< points to the state buffer array. The array is of length maxDelay+blockSize-1. */
    q15_t *pCoeffs;               /**< points to the coefficient array. The array is of length numTaps.*/
    uint16_t maxDelay;            /**< maximum offset specified by the pTapDelay array. */
    int32_t *pTapDelay;           /**< points to the array of delay values.  The array is of length numTaps. */
  } csky_fir_sparse_instance_q15;

  /**
   * @brief Instance structure for the Q7 sparse FIR filter.
   */
  typedef struct
  {
    uint16_t numTaps;             /**< number of coefficients in the filter. */
    uint16_t stateIndex;          /**< state buffer index.  Points to the oldest sample in the state buffer. */
    q7_t *pState;                 /**< points to the state buffer array. The array is of length maxDelay+blockSize-1. */
    q7_t *pCoeffs;                /**< points to the coefficient array. The array is of length numTaps.*/
    uint16_t maxDelay;            /**< maximum offset specified by the pTapDelay array. */
    int32_t *pTapDelay;           /**< points to the array of delay values.  The array is of length numTaps. */
  } csky_fir_sparse_instance_q7;

  void csky_fir_sparse_f32(
  csky_fir_sparse_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  float32_t * pScratchIn,
  uint32_t blockSize);

  void csky_fir_sparse_init_f32(
  csky_fir_sparse_instance_f32 * S,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  int32_t * pTapDelay,
  uint16_t maxDelay,
  uint32_t blockSize);

  void csky_fir_sparse_q31(
  csky_fir_sparse_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  q31_t * pScratchIn,
  uint32_t blockSize);

  void csky_fir_sparse_init_q31(
  csky_fir_sparse_instance_q31 * S,
  uint16_t numTaps,
  q31_t * pCoeffs,
  q31_t * pState,
  int32_t * pTapDelay,
  uint16_t maxDelay,
  uint32_t blockSize);

  void csky_fir_sparse_q15(
  csky_fir_sparse_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  q15_t * pScratchIn,
  q31_t * pScratchOut,
  uint32_t blockSize);

  void csky_fir_sparse_init_q15(
  csky_fir_sparse_instance_q15 * S,
  uint16_t numTaps,
  q15_t * pCoeffs,
  q15_t * pState,
  int32_t * pTapDelay,
  uint16_t maxDelay,
  uint32_t blockSize);

  void csky_fir_sparse_q7(
  csky_fir_sparse_instance_q7 * S,
  q7_t * pSrc,
  q7_t * pDst,
  q7_t * pScratchIn,
  q31_t * pScratchOut,
  uint32_t blockSize);

  void csky_fir_sparse_init_q7(
  csky_fir_sparse_instance_q7 * S,
  uint16_t numTaps,
  q7_t * pCoeffs,
  q7_t * pState,
  int32_t * pTapDelay,
  uint16_t maxDelay,
  uint32_t blockSize);

  void csky_sin_cos_f32(
  float32_t theta,
  float32_t * pSinVal,
  float32_t * pCosVal);

  void csky_sin_cos_q31(
  q31_t theta,
  q31_t * pSinVal,
  q31_t * pCosVal);

  void csky_cmplx_conj_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t numSamples);

  void csky_cmplx_conj_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t numSamples);

  void csky_cmplx_conj_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t numSamples);

  void csky_cmplx_mag_squared_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t numSamples);

  void csky_cmplx_mag_squared_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t numSamples);

  void csky_cmplx_mag_squared_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t numSamples);

  void csky_vsqrt_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t numSamples);

  void csky_vsqrt_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t numSamples);

  void csky_vsqrt_q7(
  q7_t * pSrc,
  q7_t * pDst,
  uint32_t numSamples);

/**
  * @ingroup groupController
  */

/**
 * @defgroup PID PID Motor Control
 *
 * A Proportional Integral Derivative (PID) controller is a generic feedback control
 * loop mechanism widely used in industrial control systems.
 * A PID controller is the most commonly used type of feedback controller.
 *
 * This set of functions implements (PID) controllers
 * for Q15, Q31, and floating-point data types.  The functions operate on a single sample
 * of data and each call to the function returns a single processed value.
 * <code>S</code> points to an instance of the PID control data structure.  <code>in</code>
 * is the input sample value. The functions return the output value.
 *
 * \par Algorithm:
 * <pre>
 *    y[n] = y[n-1] + A0 * x[n] + A1 * x[n-1] + A2 * x[n-2]
 *    A0 = Kp + Ki + Kd
 *    A1 = (-Kp ) - (2 * Kd )
 *    A2 = Kd  </pre>
 *
 * \par
 * where \c Kp is proportional constant, \c Ki is Integral constant and \c Kd is Derivative constant
 *
 * \par
 * \image html PID.gif "Proportional Integral Derivative Controller"
 *
 * \par
 * The PID controller calculates an "error" value as the difference between
 * the measured output and the reference input.
 * The controller attempts to minimize the error by adjusting the process control inputs.
 * The proportional value determines the reaction to the current error,
 * the integral value determines the reaction based on the sum of recent errors,
 * and the derivative value determines the reaction based on the rate at which the error has been changing.
 *
 * \par Instance Structure
 * The Gains A0, A1, A2 and state variables for a PID controller are stored together in an instance data structure.
 * A separate instance structure must be defined for each PID Controller.
 * There are separate instance structure declarations for each of the 3 supported data types.
 *
 * \par Reset Functions
 * There is also an associated reset function for each data type which clears the state array.
 *
 * \par Initialization Functions
 * There is also an associated initialization function for each data type.
 * The initialization function performs the following operations:
 * - Initializes the Gains A0, A1, A2 from Kp,Ki, Kd gains.
 * - Zeros out the values in the state buffer.
 *
 * \par
 * Instance structure cannot be placed into a const data section and it is recommended to use the initialization function.
 *
 * \par Fixed-Point Behavior
 * Care must be taken when using the fixed-point versions of the PID Controller functions.
 * In particular, the overflow and saturation behavior of the accumulator used in each function must be considered.
 * Refer to the function specific documentation below for usage guidelines.
 */

/**
 * @addtogroup PID
 * @{
 */

/**
 * @brief  Process function for the floating-point PID Control.
 * @param[in,out] S   is an instance of the floating-point PID Control structure
 * @param[in]     in  input sample to process
 * @return out processed output sample.
 */
  __ALWAYS_STATIC_INLINE float32_t csky_pid_f32(
  csky_pid_instance_f32 * S,
  float32_t in)
  {
    float32_t out;

    /* y[n] = y[n-1] + A0 * x[n] + A1 * x[n-1] + A2 * x[n-2]  */
    out = (S->A0 * in) +
      (S->A1 * S->state[0]) + (S->A2 * S->state[1]) + (S->state[2]);

    /* Update state */
    S->state[1] = S->state[0];
    S->state[0] = in;
    S->state[2] = out;

    /* return to application */
    return (out);
  }

/**
 * @}
*/ // end of PID group


/**
 * @addtogroup PID
 * @{
 */

/**
 * @brief  Process function for the Q31 PID Control.
 * @param[in,out] S  points to an instance of the Q31 PID Control structure
 * @param[in]     in  input sample to process
 * @return out processed output sample.
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function is implemented using an internal 64-bit accumulator.
 * The accumulator has a 2.62 format and maintains full precision of the intermediate multiplication results but provides only a single guard bit.
 * Thus, if the accumulator result overflows it wraps around rather than clip.
 * In order to avoid overflows completely the input signal must be scaled down by 2 bits as there are four additions.
 * After all multiply-accumulates are performed, the 2.62 accumulator is truncated to 1.32 format and then saturated to 1.31 format.
 */
  __ALWAYS_STATIC_INLINE q31_t csky_pid_q31(
  csky_pid_instance_q31 * S,
  q31_t in)
  {
    q63_t acc;
    q31_t out;

  #ifdef CSKY_SIMD
    /* acc = A0 * x[n]  */
    acc = mult_32x32_keep64(S->A0, in);

    /* acc += A1 * x[n-1] */
    acc = multAcc_32x32_keep64(acc, S->A1, S->state[0]);

    /* acc += A2 * x[n-2]  */
    acc = multAcc_32x32_keep64(acc, S->A2, S->state[1]);

    /* convert output to 1.31 format to add y[n-1] */
    out = dext_31(acc);
  #else
    /* acc = A0 * x[n]  */
    acc = (q63_t) S->A0 * in;

    /* acc += A1 * x[n-1] */
    acc += (q63_t) S->A1 * S->state[0];

    /* acc += A2 * x[n-2]  */
    acc += (q63_t) S->A2 * S->state[1];

    /* convert output to 1.31 format to add y[n-1] */
    out = (q31_t) (acc >> 31u);
  #endif

    /* out += y[n-1] */
    out += S->state[2];

    /* Update state */
    S->state[1] = S->state[0];
    S->state[0] = in;
    S->state[2] = out;

    /* return to application */
    return (out);
  }

/**
 * @}
 */  // end of PID group

/**
 * @addtogroup PID
 * @{
 */
/**
 * @brief  Process function for the Q15 PID Control.
 * @param[in,out] S   points to an instance of the Q15 PID Control structure
 * @param[in]     in  input sample to process
 * @return out processed output sample.
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function is implemented using a 64-bit internal accumulator.
 * Both Gains and state variables are represented in 1.15 format and multiplications yield a 2.30 result.
 * The 2.30 intermediate results are accumulated in a 64-bit accumulator in 34.30 format.
 * There is no risk of internal overflow with this approach and the full precision of intermediate multiplications is preserved.
 * After all additions have been performed, the accumulator is truncated to 34.15 format by discarding low 15 bits.
 * Lastly, the accumulator is saturated to yield a result in 1.15 format.
 */
  __ALWAYS_STATIC_INLINE q15_t csky_pid_q15(
  csky_pid_instance_q15 * S,
  q15_t in)
  {
    q63_t acc;
    q15_t out;

    /* acc = A0 * x[n]  */
    acc = ((q31_t) S->A0) * in;

    /* acc += A1 * x[n-1] + A2 * x[n-2]  */
    acc += (q31_t) S->A1 * S->state[0];
    acc += (q31_t) S->A2 * S->state[1];

    /* acc += y[n-1] */
    acc += (q31_t) S->state[2] << 15;

    /* saturate the output */
    out = (q15_t) (__SSAT_16((acc >> 15)));

    /* Update state */
    S->state[1] = S->state[0];
    S->state[0] = in;
    S->state[2] = out;

    /* return to application */
    return (out);
  }
/**
 * @}
 */ // end of PID group

  csky_status csky_mat_inverse_f32(
  const csky_matrix_instance_f32 * src,
  csky_matrix_instance_f32 * dst);

  csky_status csky_mat_inverse_f64(
  const csky_matrix_instance_f64 * src,
  csky_matrix_instance_f64 * dst);

/**
 * @ingroup groupController
 */

/**
 * @defgroup clarke Vector Clarke Transform
 * Forward Clarke transform converts the instantaneous stator phases into a two-coordinate time invariant vector.
 * Generally the Clarke transform uses three-phase currents <code>Ia, Ib and Ic</code> to calculate currents
 * in the two-phase orthogonal stator axis <code>Ialpha</code> and <code>Ibeta</code>.
 * When <code>Ialpha</code> is superposed with <code>Ia</code> as shown in the figure below
 * \image html clarke.gif Stator current space vector and its components in (a,b).
 * and <code>Ia + Ib + Ic = 0</code>, in this condition <code>Ialpha</code> and <code>Ibeta</code>
 * can be calculated using only <code>Ia</code> and <code>Ib</code>.
 *
 * The function operates on a single sample of data and each call to the function returns the processed output.
 * The library provides separate functions for Q31 and floating-point data types.
 * \par Algorithm
 * \image html clarkeFormula.gif
 * where <code>Ia</code> and <code>Ib</code> are the instantaneous stator phases and
 * <code>pIalpha</code> and <code>pIbeta</code> are the two coordinates of time invariant vector.
 * \par Fixed-Point Behavior
 * Care must be taken when using the Q31 version of the Clarke transform.
 * In particular, the overflow and saturation behavior of the accumulator used must be considered.
 * Refer to the function specific documentation below for usage guidelines.
 */

/**
 * @addtogroup clarke
 * @{
 */

/**
 *
 * @brief  Floating-point Clarke transform
 * @param[in]  Ia       input three-phase coordinate a
 * @param[in]  Ib       input three-phase coordinate b
 * @param[out] pIalpha  points to output two-phase orthogonal vector axis alpha
 * @param[out] pIbeta   points to output two-phase orthogonal vector axis beta
 */
  __ALWAYS_STATIC_INLINE void csky_clarke_f32(
  float32_t Ia,
  float32_t Ib,
  float32_t * pIalpha,
  float32_t * pIbeta)
  {
    /* Calculate pIalpha using the equation, pIalpha = Ia */
    *pIalpha = Ia;

    /* Calculate pIbeta using the equation, pIbeta = (1/sqrt(3)) * Ia + (2/sqrt(3)) * Ib */
    *pIbeta = ((float32_t) 0.57735026919 * Ia + (float32_t) 1.15470053838 * Ib);
  }

/**
 * @}
 */ // end of clarke group


/**
 * @addtogroup clarke
 * @{
 */

/**
 * @brief  Clarke transform for Q31 version
 * @param[in]  Ia       input three-phase coordinate a
 * @param[in]  Ib       input three-phase coordinate b
 * @param[out] pIalpha  points to output two-phase orthogonal vector axis alpha
 * @param[out] pIbeta   points to output two-phase orthogonal vector axis beta
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function is implemented using an internal 32-bit accumulator.
 * The accumulator maintains 1.31 format by truncating lower 31 bits of the intermediate multiplication in 2.62 format.
 * There is saturation on the addition, hence there is no risk of overflow.
 */
  __ALWAYS_STATIC_INLINE void csky_clarke_q31(
  q31_t Ia,
  q31_t Ib,
  q31_t * pIalpha,
  q31_t * pIbeta)
  {
    q31_t product1, product2;                    /* Temporary variables used to store intermediate results */

    /* Calculating pIalpha from Ia by equation pIalpha = Ia */
    *pIalpha = Ia;

  #ifdef CSKY_SIMD
    /* Intermediate product is calculated by (1/(sqrt(3)) * Ia) */
    product1 = mult_32x32_dext_30(Ia, 0x24F34E8B);

    /* Intermediate product is calculated by (2/sqrt(3) * Ib) */
    product2 = mult_32x32_dext_30(Ib, 0x49E69D16);
  #else
    /* Intermediate product is calculated by (1/(sqrt(3)) * Ia) */
    product1 = (q31_t) (((q63_t) Ia * 0x24F34E8B) >> 30);

    /* Intermediate product is calculated by (2/sqrt(3) * Ib) */
    product2 = (q31_t) (((q63_t) Ib * 0x49E69D16) >> 30);
  #endif

    /* pIbeta is calculated by adding the intermediate products */
    *pIbeta = __QADD(product1, product2);
  }


/**
 * @}
 */ // end of clarke group

  void csky_q7_to_q31(
  q7_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

/**
 * @ingroup groupController
 */
/**
 * @defgroup inv_clarke Vector Inverse Clarke Transform
 * Inverse Clarke transform converts the two-coordinate time invariant vector into instantaneous stator phases.
 *
 * The function operates on a single sample of data and each call to the function returns the processed output.
 * The library provides separate functions for Q31 and floating-point data types.
 * \par Algorithm
 * \image html clarkeInvFormula.gif
 * where <code>pIa</code> and <code>pIb</code> are the instantaneous stator phases and
 * <code>Ialpha</code> and <code>Ibeta</code> are the two coordinates of time invariant vector.
 * \par Fixed-Point Behavior
 * Care must be taken when using the Q31 version of the Clarke transform.
 * In particular, the overflow and saturation behavior of the accumulator used must be considered.
 * Refer to the function specific documentation below for usage guidelines.
 */

/**
 * @addtogroup inv_clarke
 * @{
 */

 /**
 * @brief  Floating-point Inverse Clarke transform
 * @param[in]  Ialpha  input two-phase orthogonal vector axis alpha
 * @param[in]  Ibeta   input two-phase orthogonal vector axis beta
 * @param[out] pIa     points to output three-phase coordinate <code>a</code>
 * @param[out] pIb     points to output three-phase coordinate <code>b</code>
 */
  __ALWAYS_STATIC_INLINE void csky_inv_clarke_f32(
  float32_t Ialpha,
  float32_t Ibeta,
  float32_t * pIa,
  float32_t * pIb)
  {
    /* Calculating pIa from Ialpha by equation pIa = Ialpha */
    *pIa = Ialpha;

    /* Calculating pIb from Ialpha and Ibeta by equation pIb = -(1/2) * Ialpha + (sqrt(3)/2) * Ibeta */
    *pIb = -0.5f * Ialpha + 0.8660254039f * Ibeta;
  }


/**
 * @}
 */ // end of inv_clarke group

/**
 * @addtogroup inv_clarke
 * @{
 */

/**
 * @brief  Inverse Clarke transform for Q31 version
 * @param[in]  Ialpha  input two-phase orthogonal vector axis alpha
 * @param[in]  Ibeta   input two-phase orthogonal vector axis beta
 * @param[out] pIa     points to output three-phase coordinate <code>a</code>
 * @param[out] pIb     points to output three-phase coordinate <code>b</code>
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function is implemented using an internal 32-bit accumulator.
 * The accumulator maintains 1.31 format by truncating lower 31 bits of the intermediate multiplication in 2.62 format.
 * There is saturation on the subtraction, hence there is no risk of overflow.
 */
  __ALWAYS_STATIC_INLINE void csky_inv_clarke_q31(
  q31_t Ialpha,
  q31_t Ibeta,
  q31_t * pIa,
  q31_t * pIb)
  {
    q31_t product1, product2;                    /* Temporary variables used to store intermediate results */

    /* Calculating pIa from Ialpha by equation pIa = Ialpha */
    *pIa = Ialpha;

  #ifdef CSKY_SIMD
    /* Intermediate product is calculated by (1/(2*sqrt(3)) * Ia) */
    product1 = mult_32x32_dext_31(Ialpha, 0x40000000);

    /* Intermediate product is calculated by (1/sqrt(3) * pIb) */
    product2 = mult_32x32_dext_31(Ibeta, 0x6ED9EBA1);
  #else
    /* Intermediate product is calculated by (1/(2*sqrt(3)) * Ia) */
    product1 = (q31_t) (((q63_t) (Ialpha) * (0x40000000)) >> 31);

    /* Intermediate product is calculated by (1/sqrt(3) * pIb) */
    product2 = (q31_t) (((q63_t) (Ibeta) * (0x6ED9EBA1)) >> 31);
  #endif

    /* pIb is calculated by subtracting the products */
    *pIb = __QSUB(product2, product1);
  }

/**
 * @}
 */ // end of inv_clarke group

  void csky_q7_to_q15(
  q7_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

/**
 * @ingroup groupController
 */
/**
 * @defgroup park Vector Park Transform
 *
 * Forward Park transform converts the input two-coordinate vector to flux and torque components.
 * The Park transform can be used to realize the transformation of the <code>Ialpha</code> and the <code>Ibeta</code> currents
 * from the stationary to the moving reference frame and control the spatial relationship between
 * the stator vector current and rotor flux vector.
 * If we consider the d axis aligned with the rotor flux, the diagram below shows the
 * current vector and the relationship from the two reference frames:
 * \image html park.gif "Stator current space vector and its component in (a,b) and in the d,q rotating reference frame"
 *
 * The function operates on a single sample of data and each call to the function returns the processed output.
 * The library provides separate functions for Q31 and floating-point data types.
 * \par Algorithm
 * \image html parkFormula.gif
 * where <code>Ialpha</code> and <code>Ibeta</code> are the stator vector components,
 * <code>pId</code> and <code>pIq</code> are rotor vector components and <code>cosVal</code> and <code>sinVal</code> are the
 * cosine and sine values of theta (rotor flux position).
 * \par Fixed-Point Behavior
 * Care must be taken when using the Q31 version of the Park transform.
 * In particular, the overflow and saturation behavior of the accumulator used must be considered.
 * Refer to the function specific documentation below for usage guidelines.
 */
/**
 * @addtogroup park
 * @{
 */
/**
 * @brief Floating-point Park transform
 * @param[in]  Ialpha  input two-phase vector coordinate alpha
 * @param[in]  Ibeta   input two-phase vector coordinate beta
 * @param[out] pId     points to output   rotor reference frame d
 * @param[out] pIq     points to output   rotor reference frame q
 * @param[in]  sinVal  sine value of rotation angle theta
 * @param[in]  cosVal  cosine value of rotation angle theta
 *
 * The function implements the forward Park transform.
 *
 */
  __ALWAYS_STATIC_INLINE void csky_park_f32(
  float32_t Ialpha,
  float32_t Ibeta,
  float32_t * pId,
  float32_t * pIq,
  float32_t sinVal,
  float32_t cosVal)
{
  /* Calculate pId using the equation, pId = Ialpha * cosVal + Ibeta * sinVal */
  *pId = Ialpha * cosVal + Ibeta * sinVal;
  /* Calculate pIq using the equation, pIq = - Ialpha * sinVal + Ibeta * cosVal */
  *pIq = -Ialpha * sinVal + Ibeta * cosVal;
}
/**
 * @}
 */ // end of park group

/**
 * @addtogroup park
 * @{
 */
/**
 * @brief  Park transform for Q31 version
 * @param[in]  Ialpha  input two-phase vector coordinate alpha
 * @param[in]  Ibeta   input two-phase vector coordinate beta
 * @param[out] pId     points to output rotor reference frame d
 * @param[out] pIq     points to output rotor reference frame q
 * @param[in]  sinVal  sine value of rotation angle theta
 * @param[in]  cosVal  cosine value of rotation angle theta
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function is implemented using an internal 32-bit accumulator.
 * The accumulator maintains 1.31 format by truncating lower 31 bits of the intermediate multiplication in 2.62 format.
 * There is saturation on the addition and subtraction, hence there is no risk of overflow.
 */
  __ALWAYS_STATIC_INLINE void csky_park_q31(
  q31_t Ialpha,
  q31_t Ibeta,
  q31_t * pId,
  q31_t * pIq,
  q31_t sinVal,
  q31_t cosVal)
{
#ifdef CSKY_SIMD
  __ASM volatile(
                "rmul.s32.h t0, %0, %3\n\t"
                "rmul.s32.h t1, %1, %2\n\t"
                "add.s32.s  t0, t0, t1\n\t"
                "st.w       t0, (%4, 0x0)\n\t"
                "rmul.s32.h t0, %0, %2\n\t"
                "rmul.s32.h t1, %1, %3\n\t"
                "sub.s32.s  t1, t1, t0\n\t"
                "st.w       t1, (%5, 0x0)\n\t"
                ::"r"(Ialpha),"r"(Ibeta),"r"(sinVal),"r"(cosVal),"r"(pId),"r"(pIq)
                :"t0","t1", "memory");
#else
  q31_t product1, product2;                    /* Temporary variables used to store intermediate results */
  q31_t product3, product4;                    /* Temporary variables used to store intermediate results */
  /* Intermediate product is calculated by (Ialpha * cosVal) */
  product1 = clip_q63_to_q31 (((q63_t) (Ialpha) * (cosVal)) >> 31);
  /* Intermediate product is calculated by (Ibeta * sinVal) */
  product2 = clip_q63_to_q31 (((q63_t) (Ibeta) * (sinVal)) >> 31);
  /* Intermediate product is calculated by (Ialpha * sinVal) */
  product3 = clip_q63_to_q31 (((q63_t) (Ialpha) * (sinVal)) >> 31);
  /* Intermediate product is calculated by (Ibeta * cosVal) */
  product4 = clip_q63_to_q31 (((q63_t) (Ibeta) * (cosVal)) >> 31);
  /* Calculate pId by adding the two intermediate products 1 and 2 */
  *pId = __QADD(product1, product2);
  /* Calculate pIq by subtracting the two intermediate products 3 from 4 */
  *pIq = __QSUB(product4, product3);
#endif
}
/**
 * @}
 */ // end of park group

  void csky_q7_to_float(
  q7_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

/**
 * @ingroup groupController
 */
/**
 * @defgroup inv_park Vector Inverse Park transform
 * Inverse Park transform converts the input flux and torque components to two-coordinate vector.
 *
 * The function operates on a single sample of data and each call to the function returns the processed output.
 * The library provides separate functions for Q31 and floating-point data types.
 * \par Algorithm
 * \image html parkInvFormula.gif
 * where <code>pIalpha</code> and <code>pIbeta</code> are the stator vector components,
 * <code>Id</code> and <code>Iq</code> are rotor vector components and <code>cosVal</code> and <code>sinVal</code> are the
 * cosine and sine values of theta (rotor flux position).
 * \par Fixed-Point Behavior
 * Care must be taken when using the Q31 version of the Park transform.
 * In particular, the overflow and saturation behavior of the accumulator used must be considered.
 * Refer to the function specific documentation below for usage guidelines.
 */
/**
 * @addtogroup inv_park
 * @{
 */
 /**
 * @brief  Floating-point Inverse Park transform
 * @param[in]  Id       input coordinate of rotor reference frame d
 * @param[in]  Iq       input coordinate of rotor reference frame q
 * @param[out] pIalpha  points to output two-phase orthogonal vector axis alpha
 * @param[out] pIbeta   points to output two-phase orthogonal vector axis beta
 * @param[in]  sinVal   sine value of rotation angle theta
 * @param[in]  cosVal   cosine value of rotation angle theta
 */
  __ALWAYS_STATIC_INLINE void csky_inv_park_f32(
  float32_t Id,
  float32_t Iq,
  float32_t * pIalpha,
  float32_t * pIbeta,
  float32_t sinVal,
  float32_t cosVal)
{
  /* Calculate pIalpha using the equation, pIalpha = Id * cosVal - Iq * sinVal */
  *pIalpha = Id * cosVal - Iq * sinVal;
  /* Calculate pIbeta using the equation, pIbeta = Id * sinVal + Iq * cosVal */
  *pIbeta = Id * sinVal + Iq * cosVal;
}
/**
 * @}
 */ // end of inv_park group

/**
 * @addtogroup inv_park
 * @{
 */
/**
 * @brief  Inverse Park transform for   Q31 version
 * @param[in]  Id       input coordinate of rotor reference frame d
 * @param[in]  Iq       input coordinate of rotor reference frame q
 * @param[out] pIalpha  points to output two-phase orthogonal vector axis alpha
 * @param[out] pIbeta   points to output two-phase orthogonal vector axis beta
 * @param[in]  sinVal   sine value of rotation angle theta
 * @param[in]  cosVal   cosine value of rotation angle theta
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function is implemented using an internal 32-bit accumulator.
 * The accumulator maintains 1.31 format by truncating lower 31 bits of the intermediate multiplication in 2.62 format.
 * There is saturation on the addition, hence there is no risk of overflow.
 */
  __ALWAYS_STATIC_INLINE void csky_inv_park_q31(
  q31_t Id,
  q31_t Iq,
  q31_t * pIalpha,
  q31_t * pIbeta,
  q31_t sinVal,
  q31_t cosVal)
{
#ifdef CSKY_SIMD
  __ASM volatile(
                "rmul.s32.h t0, %0, %3\n\t"
                "rmul.s32.h t1, %1, %2\n\t"
                "sub.s32.s  t0, t0, t1\n\t"
                "st.w       t0, (%4, 0x0)\n\t"
                "rmul.s32.h t0, %0, %2\n\t"
                "rmul.s32.h t1, %1, %3\n\t"
                "add.s32.s  t0, t0, t1\n\t"
                "st.w       t0, (%5, 0x0)\n\t"
                ::"r"(Id),"r"(Iq),"r"(sinVal),"r"(cosVal),"r"(pIalpha),"r"(pIbeta)
                :"t0","t1", "memory");

#else
  q31_t product1, product2;                    /* Temporary variables used to store intermediate results */
  q31_t product3, product4;                    /* Temporary variables used to store intermediate results */
  /* Intermediate product is calculated by (Id * cosVal) */
  product1 = clip_q63_to_q31 (((q63_t) (Id) * (cosVal)) >> 31);
  /* Intermediate product is calculated by (Iq * sinVal) */
  product2 = clip_q63_to_q31 (((q63_t) (Iq) * (sinVal)) >> 31);
  /* Intermediate product is calculated by (Id * sinVal) */
  product3 = clip_q63_to_q31 (((q63_t) (Id) * (sinVal)) >> 31);
  /* Intermediate product is calculated by (Iq * cosVal) */
  product4 = clip_q63_to_q31 (((q63_t) (Iq) * (cosVal)) >> 31);
  /* Calculate pIalpha by using the two intermediate products 1 and 2 */
  *pIalpha = __QSUB(product1, product2);
  /* Calculate pIbeta by using the two intermediate products 3 and 4 */
  *pIbeta = __QADD(product4, product3);
#endif
}

/**
 * @}
 */ // end of inv_park group

  void csky_q31_to_float(
  q31_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

/**
 * @ingroup groupInterpolation
 */
/**
 * @defgroup LinearInterpolate Linear Interpolation
 *
 * Linear interpolation is a method of curve fitting using linear polynomials.
 * Linear interpolation works by effectively drawing a straight line between two neighboring samples and returning the appropriate point along that line
 *
 * \par
 * \image html LinearInterp.gif "Linear interpolation"
 *
 * \par
 * A  Linear Interpolate function calculates an output value(y), for the input(x)
 * using linear interpolation of the input values x0, x1( nearest input values) and the output values y0 and y1(nearest output values)
 *
 * \par Algorithm:
 * <pre>
 *       y = y0 + (x - x0) * ((y1 - y0)/(x1-x0))
 *       where x0, x1 are nearest values of input x
 *             y0, y1 are nearest values to output y
 * </pre>
 *
 * \par
 * This set of functions implements Linear interpolation process
 * for Q7, Q15, Q31, and floating-point data types.  The functions operate on a single
 * sample of data and each call to the function returns a single processed value.
 * <code>S</code> points to an instance of the Linear Interpolate function data structure.
 * <code>x</code> is the input sample value. The functions returns the output value.
 *
 * \par
 * if x is outside of the table boundary, Linear interpolation returns first value of the table
 * if x is below input range and returns last value of table if x is above range.
 */
/**
 * @addtogroup LinearInterpolate
 * @{
 */
/**
 * @brief  Process function for the floating-point Linear Interpolation Function.
 * @param[in,out] S  is an instance of the floating-point Linear Interpolation structure
 * @param[in]     x  input sample to process
 * @return y processed output sample.
 *
 */
__ALWAYS_STATIC_INLINE float32_t csky_linear_interp_f32(
csky_linear_interp_instance_f32 * S,
float32_t x)
{
  float32_t y;
  float32_t x0, x1;                            /* Nearest input values */
  float32_t y0, y1;                            /* Nearest output values */
  float32_t xSpacing = S->xSpacing;            /* spacing between input values */
  int32_t i;                                   /* Index variable */
  float32_t *pYData = S->pYData;               /* pointer to output table */
  /* Calculation of index */
  i = (int32_t) ((x - S->x1) / xSpacing);
  if(i < 0)
  {
    /* Iniatilize output for below specified range as least output value of table */
    y = pYData[0];
  }
  else if((uint32_t)i >= S->nValues)
  {
    /* Iniatilize output for above specified range as last output value of table */
    y = pYData[S->nValues - 1];
  }
  else
  {
    /* Calculation of nearest input values */
    x0 = S->x1 +  i      * xSpacing;
    x1 = S->x1 + (i + 1) * xSpacing;
    /* Read of nearest output values */
    y0 = pYData[i];
    y1 = pYData[i + 1];
    /* Calculation of output */
    y = y0 + (x - x0) * ((y1 - y0) / (x1 - x0));
  }
  /* returns output value */
  return (y);
}
/**
 * @}
 */ // end of LinearInterpolate group

/**
 * @addtogroup LinearInterpolate
 * @{
 */

/**
 * @brief  Process function for the Q31 Linear Interpolation Function.
 * @param[in] pYData   pointer to Q31 Linear Interpolation table
 * @param[in] x        input sample to process
 * @param[in] nValues  number of table values
 * @return y processed output sample.
 *
 * \par
 * Input sample <code>x</code> is in 12.20 format which contains 12 bits for table index and 20 bits for fractional part.
 * This function can support maximum of table size 2^12.
 *
 */
__ALWAYS_STATIC_INLINE q31_t csky_linear_interp_q31(
q31_t * pYData,
q31_t x,
uint32_t nValues)
{
  q31_t y;                                     /* output */
  q31_t y0, y1;                                /* Nearest output values */
  q31_t fract;                                 /* fractional part */
  int32_t index;                               /* Index to read nearest output values */
  /* Input is in 12.20 format */
  /* 12 bits for the table index */
  /* Index value calculation */
  index = ((x & (q31_t)0xFFF00000) >> 20);
  if(index >= (int32_t)(nValues - 1))
  {
    return (pYData[nValues - 1]);
  }
  else if(index < 0)
  {
    return (pYData[0]);
  }
  else
  {
    /* 20 bits for the fractional part */
    /* shift left by 11 to keep fract in 1.31 format */
    fract = (x & 0x000FFFFF) << 11;
    /* Read two nearest output values from the index in 1.31(q31) format */
    y0 = pYData[index];
    y1 = pYData[index + 1];
#ifdef CSKY_SIMD
    /* Calculation of y0 * (1-fract) and y is in 2.30 format */
    y = mult_32x32_keep32(y0, (0x7FFFFFFF - fract));
    /* Calculation of y0 * (1-fract) + y1 *fract and y is in 2.30 format */
    y = multAcc_32x32_keep32(y, y1, fract);
#else
    /* Calculation of y0 * (1-fract) and y is in 2.30 format */
    y = ((q31_t) ((q63_t) y0 * (0x7FFFFFFF - fract) >> 32));
    /* Calculation of y0 * (1-fract) + y1 *fract and y is in 2.30 format */
    y += ((q31_t) (((q63_t) y1 * fract) >> 32));
#endif
    /* Convert y to 1.31 format */
    return (y << 1u);
  }
}
/**
 * @}
 */ // end of LinearInterpolate group

/**
 * @addtogroup LinearInterpolate
 * @{
 */
/**
 *
 * @brief  Process function for the Q15 Linear Interpolation Function.
 * @param[in] pYData   pointer to Q15 Linear Interpolation table
 * @param[in] x        input sample to process
 * @param[in] nValues  number of table values
 * @return y processed output sample.
 *
 * \par
 * Input sample <code>x</code> is in 12.20 format which contains 12 bits for table index and 20 bits for fractional part.
 * This function can support maximum of table size 2^12.
 *
 */
__ALWAYS_STATIC_INLINE q15_t csky_linear_interp_q15(
q15_t * pYData,
q31_t x,
uint32_t nValues)
{
  q63_t y;                                     /* output */
  q15_t y0, y1;                                /* Nearest output values */
  q31_t fract;                                 /* fractional part */
  int32_t index;                               /* Index to read nearest output values */
  /* Input is in 12.20 format */
  /* 12 bits for the table index */
  /* Index value calculation */
  index = ((x & (int32_t)0xFFF00000) >> 20);
  if(index >= (int32_t)(nValues - 1))
  {
    return (pYData[nValues - 1]);
  }
  else if(index < 0)
  {
    return (pYData[0]);
  }
  else
  {
    /* 20 bits for the fractional part */
    /* fract is in 12.20 format */
    fract = (x & 0x000FFFFF);
    /* Read two nearest output values from the index */
    y0 = pYData[index];
    y1 = pYData[index + 1];
#ifdef CSKY_SIMD
    /* Calculation of y0 * (1-fract) and y is in 13.35 format */
    y = mult_32x32_keep64(y0, (0xFFFFF - fract));
    /* Calculation of (y0 * (1-fract) + y1 * fract) and y is in 13.35 format */
    y = multAcc_32x32_keep64(y, y1, (fract));
#else
    /* Calculation of y0 * (1-fract) and y is in 13.35 format */
    y = ((q63_t) y0 * (0xFFFFF - fract));
    /* Calculation of (y0 * (1-fract) + y1 * fract) and y is in 13.35 format */
    y += ((q63_t) y1 * (fract));
#endif
    /* convert y to 1.15 format */
    return (q15_t) (y >> 20);
  }
}
/**
 * @}
 */ // end of LinearInterpolate group

/**
 * @addtogroup LinearInterpolate
 * @{
 */
/**
 *
 * @brief  Process function for the Q7 Linear Interpolation Function.
 * @param[in] pYData   pointer to Q7 Linear Interpolation table
 * @param[in] x        input sample to process
 * @param[in] nValues  number of table values
 * @return y processed output sample.
 *
 * \par
 * Input sample <code>x</code> is in 12.20 format which contains 12 bits for table index and 20 bits for fractional part.
 * This function can support maximum of table size 2^12.
 */
__ALWAYS_STATIC_INLINE q7_t csky_linear_interp_q7(
q7_t * pYData,
q31_t x,
uint32_t nValues)
{
  q31_t y;                                     /* output */
  q7_t y0, y1;                                 /* Nearest output values */
  q31_t fract;                                 /* fractional part */
  uint32_t index;                              /* Index to read nearest output values */
  /* Input is in 12.20 format */
  /* 12 bits for the table index */
  /* Index value calculation */
  if (x < 0)
  {
    return (pYData[0]);
  }
  index = (x >> 20) & 0xfff;
  if(index >= (nValues - 1))
  {
    return (pYData[nValues - 1]);
  }
  else
  {
    /* 20 bits for the fractional part */
    /* fract is in 12.20 format */
    fract = (x & 0x000FFFFF);
    /* Read two nearest output values from the index and are in 1.7(q7) format */
    y0 = pYData[index];
    y1 = pYData[index + 1];
    /* Calculation of y0 * (1-fract ) and y is in 13.27(q27) format */
    y = ((y0 * (0xFFFFF - fract)));
    /* Calculation of y1 * fract + y0 * (1-fract) and y is in 13.27(q27) format */
    y += (y1 * fract);
    /* convert y to 1.7(q7) format */
    return (q7_t) (y >> 20);
   }
}
/**
 * @}
 */ // end of LinearInterpolate group

  float32_t csky_sin_f32(
  float32_t x);

  q31_t csky_sin_q31(
  q31_t x);

  q15_t csky_sin_q15(
  q15_t x);

  float32_t csky_cos_f32(
  float32_t x);

  q31_t csky_cos_q31(
  q31_t x);

  q15_t csky_cos_q15(
  q15_t x);

  csky_status csky_sqrt_f32(
  float32_t in,
  float32_t * pOut);

  csky_status csky_sqrt_q31(
  q31_t in,
  q31_t * pOut);

  csky_status csky_sqrt_q15(
  q15_t in,
  q15_t * pOut);

  void csky_power_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q63_t * pResult);

  void csky_power_int32(
  int32_t * pSrc,
  uint32_t blockSize,
  q63_t * pResult);

  void csky_power_int32(
  int32_t * pSrc,
  uint32_t blockSize,
  q63_t * pResult);

  void csky_power_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult);

  void csky_power_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q63_t * pResult);

  void csky_power_q7(
  q7_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult);

  void csky_mean_q7(
  q7_t * pSrc,
  uint32_t blockSize,
  q7_t * pResult);

  void csky_mean_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q15_t * pResult);

  void csky_mean_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult);

  void csky_mean_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult);

  void csky_var_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult);

  void csky_var_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult);

  void csky_var_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q15_t * pResult);

  void csky_rms_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult);

  void csky_rms_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult);

  void csky_rms_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q15_t * pResult);

  void csky_std_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult);

  void csky_std_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult);

  void csky_std_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q15_t * pResult);

  void csky_cmplx_mag_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t numSamples);

  void csky_cmplx_mag_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t numSamples);

  void csky_cmplx_mag_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t numSamples);

  void csky_cmplx_dot_prod_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  uint32_t numSamples,
  q31_t * realResult,
  q31_t * imagResult);

  void csky_cmplx_dot_prod_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  uint32_t numSamples,
  q63_t * realResult,
  q63_t * imagResult);

  void csky_cmplx_dot_prod_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  uint32_t numSamples,
  float32_t * realResult,
  float32_t * imagResult);

  void csky_cmplx_mult_real_q15(
  q15_t * pSrcCmplx,
  q15_t * pSrcReal,
  q15_t * pCmplxDst,
  uint32_t numSamples);

  void csky_cmplx_mult_real_q31(
  q31_t * pSrcCmplx,
  q31_t * pSrcReal,
  q31_t * pCmplxDst,
  uint32_t numSamples);

  void csky_cmplx_mult_real_f32(
  float32_t * pSrcCmplx,
  float32_t * pSrcReal,
  float32_t * pCmplxDst,
  uint32_t numSamples);

  void csky_min_q7(
  q7_t * pSrc,
  uint32_t blockSize,
  q7_t * result,
  uint32_t * index);

  void csky_min_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q15_t * pResult,
  uint32_t * pIndex);

  void csky_min_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult,
  uint32_t * pIndex);

  void csky_min_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult,
  uint32_t * pIndex);

  void csky_max_q7(
  q7_t * pSrc,
  uint32_t blockSize,
  q7_t * pResult,
  uint32_t * pIndex);

  void csky_max_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q15_t * pResult,
  uint32_t * pIndex);

  void csky_max_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult,
  uint32_t * pIndex);

  void csky_max_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult,
  uint32_t * pIndex);

  void csky_cmplx_mult_cmplx_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  q15_t * pDst,
  uint32_t numSamples);

  void csky_cmplx_mult_cmplx_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  q31_t * pDst,
  uint32_t numSamples);

  void csky_cmplx_mult_cmplx_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t numSamples);

  void csky_cmplx_mult_cmplx_re_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  q15_t * pDst,
  uint32_t numSamples);

  void csky_cmplx_mult_cmplx_re_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  q31_t * pDst,
  uint32_t numSamples);

  void csky_cmplx_mult_cmplx_re_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t numSamples);


  void csky_float_to_q31(
  float32_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_float_to_q15(
  float32_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_float_to_q7(
  float32_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);

  void csky_q31_to_q15(
  q31_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);

  void csky_q31_to_q7(
  q31_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);

  void csky_q15_to_float(
  q15_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

  void csky_q15_to_q31(
  q15_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  void csky_q15_to_q7(
  q15_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);

/**
 * @ingroup groupInterpolation
 */
/**
 * @defgroup BilinearInterpolate Bilinear Interpolation
 *
 * Bilinear interpolation is an extension of linear interpolation applied to a two dimensional grid.
 * The underlying function <code>f(x, y)</code> is sampled on a regular grid and the interpolation process
 * determines values between the grid points.
 * Bilinear interpolation is equivalent to two step linear interpolation, first in the x-dimension and then in the y-dimension.
 * Bilinear interpolation is often used in image processing to rescale images.
 * The CSI DSP library provides bilinear interpolation functions for Q7, Q15, Q31, and floating-point data types.
 *
 * <b>Algorithm</b>
 * \par
 * The instance structure used by the bilinear interpolation functions describes a two dimensional data table.
 * For floating-point, the instance structure is defined as:
 * <pre>
 *   typedef struct
 *   {
 *     uint16_t numRows;
 *     uint16_t numCols;
 *     float32_t *pData;
 * } csky_bilinear_interp_instance_f32;
 * </pre>
 *
 * \par
 * where <code>numRows</code> specifies the number of rows in the table;
 * <code>numCols</code> specifies the number of columns in the table;
 * and <code>pData</code> points to an array of size <code>numRows*numCols</code> values.
 * The data table <code>pTable</code> is organized in row order and the supplied data values fall on integer indexes.
 * That is, table element (x,y) is located at <code>pTable[x + y*numCols]</code> where x and y are integers.
 *
 * \par
 * Let <code>(x, y)</code> specify the desired interpolation point.  Then define:
 * <pre>
 *     XF = floor(x)
 *     YF = floor(y)
 * </pre>
 * \par
 * The interpolated output point is computed as:
 * <pre>
 *  f(x, y) = f(XF, YF) * (1-(x-XF)) * (1-(y-YF))
 *           + f(XF+1, YF) * (x-XF)*(1-(y-YF))
 *           + f(XF, YF+1) * (1-(x-XF))*(y-YF)
 *           + f(XF+1, YF+1) * (x-XF)*(y-YF)
 * </pre>
 * Note that the coordinates (x, y) contain integer and fractional components.
 * The integer components specify which portion of the table to use while the
 * fractional components control the interpolation processor.
 *
 * \par
 * if (x,y) are outside of the table boundary, Bilinear interpolation returns zero output.
 */
/**
 * @addtogroup BilinearInterpolate
 * @{
 */
/**
*
* @brief  Floating-point bilinear interpolation.
* @param[in,out] S  points to an instance of the interpolation structure.
* @param[in]     X  interpolation coordinate.
* @param[in]     Y  interpolation coordinate.
* @return out interpolated value.
*/
__ALWAYS_STATIC_INLINE float32_t csky_bilinear_interp_f32(
const csky_bilinear_interp_instance_f32 * S,
float32_t X,
float32_t Y)
{
  float32_t out;
  float32_t f00, f01, f10, f11;
  float32_t *pData = S->pData;
  int32_t xIndex, yIndex, index;
  float32_t xdiff, ydiff;
  float32_t b1, b2, b3, b4;
  xIndex = (int32_t) X;
  yIndex = (int32_t) Y;
  /* Care taken for table outside boundary */
  /* Returns zero output when values are outside table boundary */
  if(xIndex < 0 || xIndex > (S->numRows - 1) || yIndex < 0 || yIndex > (S->numCols - 1))
  {
    return (0);
  }
  /* Calculation of index for two nearest points in X-direction */
  index = (xIndex - 1) + (yIndex - 1) * S->numCols;
  /* Read two nearest points in X-direction */
  f00 = pData[index];
  f01 = pData[index + 1];
  /* Calculation of index for two nearest points in Y-direction */
  index = (xIndex - 1) + (yIndex) * S->numCols;
  /* Read two nearest points in Y-direction */
  f10 = pData[index];
  f11 = pData[index + 1];
  /* Calculation of intermediate values */
  b1 = f00;
  b2 = f01 - f00;
  b3 = f10 - f00;
  b4 = f00 - f01 - f10 + f11;
  /* Calculation of fractional part in X */
  xdiff = X - xIndex;
  /* Calculation of fractional part in Y */
  ydiff = Y - yIndex;
  /* Calculation of bi-linear interpolated output */
  out = b1 + b2 * xdiff + b3 * ydiff + b4 * xdiff * ydiff;
  /* return to application */
  return (out);
}
/**
 * @}
 */ // end of BilinearInterpolate group

/**
 * @addtogroup BilinearInterpolate
 * @{
 */
/**
*
* @brief  Q31 bilinear interpolation.
* @param[in,out] S  points to an instance of the interpolation structure.
* @param[in]     X  interpolation coordinate in 12.20 format.
* @param[in]     Y  interpolation coordinate in 12.20 format.
* @return out interpolated value.
*/
__ALWAYS_STATIC_INLINE q31_t csky_bilinear_interp_q31(
csky_bilinear_interp_instance_q31 * S,
q31_t X,
q31_t Y)
{
  q31_t out;                                   /* Temporary output */
  q31_t acc = 0;                               /* output */
  q31_t xfract, yfract;                        /* X, Y fractional parts */
  q31_t x1, x2, y1, y2;                        /* Nearest output values */
  int32_t rI, cI;                              /* Row and column indices */
  q31_t *pYData = S->pData;                    /* pointer to output table values */
  uint32_t nCols = S->numCols;                 /* num of rows */
  /* Input is in 12.20 format */
  /* 12 bits for the table index */
  /* Index value calculation */
  rI = ((X & (q31_t)0xFFF00000) >> 20);
  /* Input is in 12.20 format */
  /* 12 bits for the table index */
  /* Index value calculation */
  cI = ((Y & (q31_t)0xFFF00000) >> 20);
  /* Care taken for table outside boundary */
  /* Returns zero output when values are outside table boundary */
  if(rI < 0 || rI > (S->numRows - 1) || cI < 0 || cI > (S->numCols - 1))
  {
    return (0);
  }
  /* 20 bits for the fractional part */
  /* shift left xfract by 11 to keep 1.31 format */
  xfract = (X & 0x000FFFFF) << 11u;
  /* Read two nearest output values from the index */
  x1 = pYData[(rI) + (int32_t)nCols * (cI)    ];
  x2 = pYData[(rI) + (int32_t)nCols * (cI) + 1];
  /* 20 bits for the fractional part */
  /* shift left yfract by 11 to keep 1.31 format */
  yfract = (Y & 0x000FFFFF) << 11u;
  /* Read two nearest output values from the index */
  y1 = pYData[(rI) + (int32_t)nCols * (cI + 1)    ];
  y2 = pYData[(rI) + (int32_t)nCols * (cI + 1) + 1];
#ifdef CSKY_SIMD
  /* Calculation of x1 * (1-xfract ) * (1-yfract) and acc is in 3.29(q29) format */
  out = mult_32x32_keep32(x1, (0x7FFFFFFF - xfract));
  acc = mult_32x32_keep32(out, (0x7FFFFFFF - yfract));
  /* x2 * (xfract) * (1-yfract)  in 3.29(q29) and adding to acc */
  out = mult_32x32_keep32(x2, (0x7FFFFFFF - yfract));
  acc = multAcc_32x32_keep32(acc, out, xfract);
  /* y1 * (1 - xfract) * (yfract)  in 3.29(q29) and adding to acc */
  out = mult_32x32_keep32(y1,  (0x7FFFFFFF - xfract));
  acc = multAcc_32x32_keep32(acc, out, yfract);
  /* y2 * (xfract) * (yfract)  in 3.29(q29) and adding to acc */
  out = mult_32x32_keep32(y2, xfract);
  acc = multAcc_32x32_keep32(acc, out, yfract);
#else
  /* Calculation of x1 * (1-xfract ) * (1-yfract) and acc is in 3.29(q29) format */
  out = ((q31_t) (((q63_t) x1  * (0x7FFFFFFF - xfract)) >> 32));
  acc = ((q31_t) (((q63_t) out * (0x7FFFFFFF - yfract)) >> 32));
  /* x2 * (xfract) * (1-yfract)  in 3.29(q29) and adding to acc */
  out = ((q31_t) ((q63_t) x2 * (0x7FFFFFFF - yfract) >> 32));
  acc += ((q31_t) ((q63_t) out * (xfract) >> 32));
  /* y1 * (1 - xfract) * (yfract)  in 3.29(q29) and adding to acc */
  out = ((q31_t) ((q63_t) y1 * (0x7FFFFFFF - xfract) >> 32));
  acc += ((q31_t) ((q63_t) out * (yfract) >> 32));
  /* y2 * (xfract) * (yfract)  in 3.29(q29) and adding to acc */
  out = ((q31_t) ((q63_t) y2 * (xfract) >> 32));
  acc += ((q31_t) ((q63_t) out * (yfract) >> 32));
#endif
  /* Convert acc to 1.31(q31) format */
  return ((q31_t)(acc << 2));
}
/**
 * @}
 */ // end of BilinearInterpolate group

/**
 * @addtogroup BilinearInterpolate
 * @{
 */
/**
* @brief  Q15 bilinear interpolation.
* @param[in,out] S  points to an instance of the interpolation structure.
* @param[in]     X  interpolation coordinate in 12.20 format.
* @param[in]     Y  interpolation coordinate in 12.20 format.
* @return out interpolated value.
*/
__ALWAYS_STATIC_INLINE q15_t csky_bilinear_interp_q15(
csky_bilinear_interp_instance_q15 * S,
q31_t X,
q31_t Y)
{
  q63_t acc = 0;                               /* output */
  q31_t out;                                   /* Temporary output */
  q15_t x1, x2, y1, y2;                        /* Nearest output values */
  q31_t xfract, yfract;                        /* X, Y fractional parts */
  int32_t rI, cI;                              /* Row and column indices */
  q15_t *pYData = S->pData;                    /* pointer to output table values */
  uint32_t nCols = S->numCols;                 /* num of rows */
  /* Input is in 12.20 format */
  /* 12 bits for the table index */
  /* Index value calculation */
  rI = ((X & (q31_t)0xFFF00000) >> 20);
  /* Input is in 12.20 format */
  /* 12 bits for the table index */
  /* Index value calculation */
  cI = ((Y & (q31_t)0xFFF00000) >> 20);
  /* Care taken for table outside boundary */
  /* Returns zero output when values are outside table boundary */
  if(rI < 0 || rI > (S->numRows - 1) || cI < 0 || cI > (S->numCols - 1))
  {
    return (0);
  }
  /* 20 bits for the fractional part */
  /* xfract should be in 12.20 format */
  xfract = (X & 0x000FFFFF);
  /* Read two nearest output values from the index */
  x1 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI)    ];
  x2 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI) + 1];
  /* 20 bits for the fractional part */
  /* yfract should be in 12.20 format */
  yfract = (Y & 0x000FFFFF);
  /* Read two nearest output values from the index */
  y1 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI + 1)    ];
  y2 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI + 1) + 1];
  /* Calculation of x1 * (1-xfract ) * (1-yfract) and acc is in 13.51 format */
  /* x1 is in 1.15(q15), xfract in 12.20 format and out is in 13.35 format */
  /* convert 13.35 to 13.31 by right shifting  and out is in 1.31 */
#ifdef CSKY_SIMD
  out = mult_32x32_dext_4(x1, (0xFFFFF - xfract));
  acc = mult_32x32_keep64(out, (0xFFFFF - yfract));
  /* x2 * (xfract) * (1-yfract)  in 1.51 and adding to acc */
  out = mult_32x32_dext_4(x2, (0xFFFFF - yfract));
  acc = multAcc_32x32_keep64(acc, out, (xfract));
  /* y1 * (1 - xfract) * (yfract)  in 1.51 and adding to acc */
  out = mult_32x32_dext_4(y1, (0xFFFFF - xfract));
  acc = multAcc_32x32_keep64(acc, out, (yfract));
  /* y2 * (xfract) * (yfract)  in 1.51 and adding to acc */
  out = mult_32x32_dext_4(y2, (xfract));
  acc = multAcc_32x32_keep64(acc, out, (yfract));
#else
  out = (q31_t) (((q63_t) x1 * (0xFFFFF - xfract)) >> 4u);
  acc = ((q63_t) out * (0xFFFFF - yfract));
  /* x2 * (xfract) * (1-yfract)  in 1.51 and adding to acc */
  out = (q31_t) (((q63_t) x2 * (0xFFFFF - yfract)) >> 4u);
  acc += ((q63_t) out * (xfract));
  /* y1 * (1 - xfract) * (yfract)  in 1.51 and adding to acc */
  out = (q31_t) (((q63_t) y1 * (0xFFFFF - xfract)) >> 4u);
  acc += ((q63_t) out * (yfract));
  /* y2 * (xfract) * (yfract)  in 1.51 and adding to acc */
  out = (q31_t) (((q63_t) y2 * (xfract)) >> 4u);
  acc += ((q63_t) out * (yfract));
#endif
  /* acc is in 13.51 format and down shift acc by 36 times */
  /* Convert out to 1.15 format */
  return ((q15_t)(acc >> 36));
}
/**
 * @}
 */ // end of BilinearInterpolate group

/**
 * @addtogroup BilinearInterpolate
 * @{
 */
/**
* @brief  Q7 bilinear interpolation.
* @param[in,out] S  points to an instance of the interpolation structure.
* @param[in]     X  interpolation coordinate in 12.20 format.
* @param[in]     Y  interpolation coordinate in 12.20 format.
* @return out interpolated value.
*/
__ALWAYS_STATIC_INLINE q7_t csky_bilinear_interp_q7(
csky_bilinear_interp_instance_q7 * S,
q31_t X,
q31_t Y)
{
  q63_t acc = 0;                               /* output */
  q31_t out;                                   /* Temporary output */
  q31_t xfract, yfract;                        /* X, Y fractional parts */
  q7_t x1, x2, y1, y2;                         /* Nearest output values */
  int32_t rI, cI;                              /* Row and column indices */
  q7_t *pYData = S->pData;                     /* pointer to output table values */
  uint32_t nCols = S->numCols;                 /* num of rows */
  /* Input is in 12.20 format */
  /* 12 bits for the table index */
  /* Index value calculation */
  rI = ((X & (q31_t)0xFFF00000) >> 20);
  /* Input is in 12.20 format */
  /* 12 bits for the table index */
  /* Index value calculation */
  cI = ((Y & (q31_t)0xFFF00000) >> 20);
  /* Care taken for table outside boundary */
  /* Returns zero output when values are outside table boundary */
  if(rI < 0 || rI > (S->numRows - 1) || cI < 0 || cI > (S->numCols - 1))
  {
    return (0);
  }
  /* 20 bits for the fractional part */
  /* xfract should be in 12.20 format */
  xfract = (X & (q31_t)0x000FFFFF);
  /* Read two nearest output values from the index */
  x1 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI)    ];
  x2 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI) + 1];
  /* 20 bits for the fractional part */
  /* yfract should be in 12.20 format */
  yfract = (Y & (q31_t)0x000FFFFF);
  /* Read two nearest output values from the index */
  y1 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI + 1)    ];
  y2 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI + 1) + 1];
  /* Calculation of x1 * (1-xfract ) * (1-yfract) and acc is in 16.47 format */
  out = ((x1 * (0xFFFFF - xfract)));
#ifdef CSKY_SIMD
  acc = multAcc_32x32_keep64(acc, out, (0xFFFFF - yfract));
  /* x2 * (xfract) * (1-yfract)  in 2.22 and adding to acc */
  out = ((x2 * (0xFFFFF - yfract)));
  acc = multAcc_32x32_keep64(acc, out, xfract);
  /* y1 * (1 - xfract) * (yfract)  in 2.22 and adding to acc */
  out = ((y1 * (0xFFFFF - xfract)));
  acc = multAcc_32x32_keep64(acc, out, yfract);
  /* y2 * (xfract) * (yfract)  in 2.22 and adding to acc */
  out = ((y2 * (yfract)));
  acc = multAcc_32x32_keep64(acc, out, xfract);
#else
  acc = (((q63_t) out * (0xFFFFF - yfract)));
  /* x2 * (xfract) * (1-yfract)  in 2.22 and adding to acc */
  out = ((x2 * (0xFFFFF - yfract)));
  acc += (((q63_t) out * (xfract)));
  /* y1 * (1 - xfract) * (yfract)  in 2.22 and adding to acc */
  out = ((y1 * (0xFFFFF - xfract)));
  acc += (((q63_t) out * (yfract)));
  /* y2 * (xfract) * (yfract)  in 2.22 and adding to acc */
  out = ((y2 * (yfract)));
  acc += (((q63_t) out * (xfract)));
#endif
  /* acc in 16.47 format and down shift by 40 to convert to 1.7 format */
  return ((q7_t)(acc >> 40));
}
/**
 * @}
 */ // end of BilinearInterpolate group

/**
 * @ingroup groupMath
 */

/**
 * @defgroup ShiftRight Right Shift
 *
 * Shift the input value to right with appointed bits, its basic format is:
 * <pre>
 *     a = (a) >> (shift),   1 =< shift <= bitof(a) - 1.
 * </pre>
 * The basic format is only designed for q31.
 *
 * and the extended format should be rounding to +inf:
 * <pre>
 *     a = (a + (1<<(shift - 1)) >> (shift),   1 =< shift <= bitof(a) - 1.
 * </pre>
 *
 * which are designed for q31, q31 positive and q63.
 */

/**
 * @addtogroup ShiftRight
 * @{
 */
/**
 * @brief  right shift Q31 version
 * @param[in]  a        input value to be shift.
 * @param[in]  shift    input positive value, the number of bits to be shift.
 * @param[out] result   the shifted a.
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function is only used for right shift. So, the value of shift is
 * between[1,31].
 */
  __ALWAYS_STATIC_INLINE q31_t csky_shr_q31(
  q31_t a,
  q31_t shift)
{
  q31_t res;
#ifdef CSKY_SIMD
  __ASM volatile(
                "asr        %0, %1, %2\n\t"
                :"=r"(res), "=r"(a),"=r"(shift):"0"(res), "1"(a), "2"(shift));
#else
  res =  ((a) >> (shift));
#endif
  return res;
}

#define SHR(a, shift)                 csky_shr_q31(a, shift)

/**
 * @}
 */ // end of ShiftRight group


/**
 * @addtogroup ShiftRight
 * @{
 */
/**
 * @brief  right shift Q31 version
 * @param[in]  a        input value to be shift.
 * @param[in]  shift    input positive value, the number of bits to be shift.
 * @param[out] result   the shifted a.
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function is only used for right shift. So, the value of shift is
 * between[1,31]. And the output value is rounding to +inf.
 */
  __ALWAYS_STATIC_INLINE q31_t csky_pshr_q31(
  q31_t a,
  q31_t shift)
{
  q31_t res;
#ifdef CSKY_SIMD
  __ASM volatile(
                "asr.s32.r  %0, %1, %2\n\t"
                :"=r"(res), "=r"(a),"=r"(shift):"0"(res), "1"(a), "2"(shift));
#else
  res =  (a >= 0?(SHR((a) + (1<<(shift - 1)), shift))\
               :(SHR((a) + ((1<<shift)>>1) -1, shift)));
#endif
  return res;
}

/**
 * @}
 */ // end of ShiftRight group


/**
 * @addtogroup ShiftRight
 * @{
 */
/**
 * @brief  right shift Q31 version
 * @param[in]  a        input positive value to be shift.
 * @param[in]  shift    input positive value, the number of bits to be shift.
 * @param[out] result   the shifted a.
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function is only used for right shift. So, the value of shift is
 * between[1,31]. And the output value is rounding to +inf.
 */
  __ALWAYS_STATIC_INLINE q31_t csky_pshr_pos_q31(
  q31_t a,
  q31_t shift)
{
  q31_t res;
#ifdef CSKY_SIMD
  __ASM volatile(
                "asr.s32.r  %0, %1, %2\n\t"
                :"=r"(res), "=r"(a),"=r"(shift):"0"(res), "1"(a), "2"(shift));
#else
  res = SHR((a) + (1<<(shift - 1)), shift);
#endif
  return res;
}

/**
 * @}
 */ // end of ShiftRight group


/**
 * @addtogroup ShiftRight
 * @{
 */
/**
 * @brief  right shift Q63 version
 * @param[in]  a        input value to be shift.
 * @param[in]  shift    input positive value, the number of bits to be shift.
 * @param[out] result   the shifted a.
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function is only used for right shift. So, the value of shift is
 * between[1,63]. And the output value is rounding to +inf.
 */
  __ALWAYS_STATIC_INLINE q63_t csky_pshr_q63(
  q63_t a,
  q31_t shift)
{
  q63_t res;
#ifdef CSKY_SIMD
  __ASM volatile(
                "subi       t0, %2, 1\n\t"
                "cmphsi     t0, 32\n\t"
                "bt         1f\n\t"
                "movi       t1, 1\n\t"
                "lsl        t0, t1, t0\n\t"
                "movi       t1, 0\n\t"
                "add.s64.s  %1, %1, t0\n\t"
                "dext       %0, %1, %R1, %2\n\t"
                "asr        %R0, %R1, %2\n\t"
                "br         2f\n\t"
                "1:\n\t"
                "subi       %2, %2, 32\n\t"
                "subi       t0, t0, 32\n\t"
                "movi       t1, 1\n\t"
                "lsl        t1, t1, t0\n\t"
                "add.s32.s  %R1, %R1, t1\n\t"
                "asr        %0, %R1, %2\n\t"
                "asri       %R0, %R1, 31\n\t"
                "2:\n\t"
                :"=r"(res), "=r"(a),"=r"(shift):"0"(res), "1"(a), "2"(shift):"t0", "t1");
#else
  res =  (a >= 0?(SHR((a) + ((q63_t)1<<(shift - 1)), shift))\
               :(SHR((a) + (((q63_t)1<<shift)>>1) -1, shift)));
#endif
  return res;
}

/**
 * @}
 */ // end of ShiftRight group

//#define SHR(a, shift)                 csky_shr_q31(a, shift)
#define PSHR(a, shift)                csky_pshr_q31(a, shift)
#define PSHR_POSITIVE(a, shift)       csky_pshr_pos_q31(a, shift)
#define PSHR64(a, shift)              csky_pshr_q63(a, shift)


#ifdef CSKY_SIMD
#else
/* SMMLAR */
#define multAcc_32x32_keep32_R(a, x, y) \
    a = (q31_t) (((((q63_t) a) << 32) + ((q63_t) x * y) + 0x80000000LL ) >> 32)

/* SMMLSR */
#define multSub_32x32_keep32_R(a, x, y) \
    a = (q31_t) (((((q63_t) a) << 32) - ((q63_t) x * y) + 0x80000000LL ) >> 32)

/* SMMULR */
#define mult_32x32_keep32_R(a, x, y) \
    a = (q31_t) (((q63_t) x * y + 0x80000000LL ) >> 32)

/* SMMLA */
#define multAcc_32x32_keep32(a, x, y) \
    a += (q31_t) (((q63_t) x * y) >> 32)

/* SMMLS */
#define multSub_32x32_keep32(a, x, y) \
    a -= (q31_t) (((q63_t) x * y) >> 32)

/* SMMUL */
#define mult_32x32_keep32(a, x, y) \
    a = (q31_t) (((q63_t) x * y ) >> 32)
#endif

#ifdef   __cplusplus
}
#endif

#endif /* _CSKY_MATH_H */

/**
 *
 * End of file.
 */

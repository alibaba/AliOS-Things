
#ifndef HAAS_COLOR_COMMON_H
#define HAAS_COLOR_COMMON_H

#include <math.h>

#define RESIZE_LEFT_SHIFT_SCALE                 2048        /* **(1<<11)*/
#define RESIZE_QUANTBILINEAR_SHIFT_SCALE        32        /* **(1<<5)*/
#define RESIZE_RIGHT_SHIFT_SCALE1               4
#define RESIZE_RIGHT_SHIFT_SCALE2               16

// Y contribution to R,G,B.  Scale and bias.
#define YG 18997  /* round(1.164 * 64 * 256 * 256 / 257) */
#define YGB -1160 /* 1.164 * 64 * -16 + 64 / 2 */

// U and V contributions to R,G,B.
#define UB -128 /* max(-128, round(-2.018 * 64)) */
#define UG 25   /* round(0.391 * 64) */
#define VG 52   /* round(0.813 * 64) */
#define VR -102 /* round(-1.596 * 64) */

// Bias values to subtract 16 from Y and 128 from U and V.
#define BB (UB * 128 + YGB)
#define BG (UG * 128 + VG * 128 + YGB)
#define BR (VR * 128 + YGB)

#define SIMD_ALIGNED(var) var __attribute__((aligned(16)))
typedef int16_t __attribute__((vector_size(16))) vec16;
typedef int32_t __attribute__((vector_size(16))) vec32;
typedef float __attribute__((vector_size(16))) vecf32;
typedef int8_t __attribute__((vector_size(16))) vec8;
typedef uint16_t __attribute__((vector_size(16))) uvec16;
typedef uint32_t __attribute__((vector_size(16))) uvec32;
typedef uint8_t __attribute__((vector_size(16))) uvec8;
typedef int16_t __attribute__((vector_size(32))) lvec16;
typedef int32_t __attribute__((vector_size(32))) lvec32;
typedef int8_t __attribute__((vector_size(32))) lvec8;
typedef uint16_t __attribute__((vector_size(32))) ulvec16;
typedef uint32_t __attribute__((vector_size(32))) ulvec32;
typedef uint8_t __attribute__((vector_size(32))) ulvec8;

struct YuvConstants {
uvec8 kUVToRB;
uvec8 kUVToG;
vec16 kUVBiasBGR;
vec32 kYToRgb;
};

const struct YuvConstants SIMD_ALIGNED(kYuvI601Constants) = {
	{-UB, -UB, -UB, -UB, -VR, -VR, -VR, -VR, 0, 0, 0, 0, 0, 0, 0, 0},
	{UG, UG, UG, UG, VG, VG, VG, VG, 0, 0, 0, 0, 0, 0, 0, 0},
	{BB, BG, BR, YGB, 0, 0, 0, 0},
	{0x0101 * YG, YG, 0, 0}};
const struct YuvConstants SIMD_ALIGNED(kYvuI601Constants) = {
	{-VR, -VR, -VR, -VR, -UB, -UB, -UB, -UB, 0, 0, 0, 0, 0, 0, 0, 0},
	{VG, VG, VG, VG, UG, UG, UG, UG, 0, 0, 0, 0, 0, 0, 0, 0},
	{BR, BG, BB, YGB, 0, 0, 0, 0},
	{0x0101 * YG, YG, 0, 0}};



#endif

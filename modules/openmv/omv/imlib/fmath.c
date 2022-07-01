/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Fast approximate math functions.
 */
#include "fmath.h"
#include "common.h"

#define M_PI    3.14159265f
#define M_PI_2  1.57079632f
#define M_PI_4  0.78539816f

const float __atanf_lut[4] = {
    -0.0443265554792128f,    //p7
    -0.3258083974640975f,    //p3
    +0.1555786518463281f,    //p5
    +0.9997878412794807f     //p1
};

float OMV_ATTR_ALWAYS_INLINE fast_sqrtf(float x)
{
    asm volatile (
            "vsqrt.f32  %[r], %[x]\n"
            : [r] "=t" (x)
            : [x] "t"  (x));
    return x;
}

int OMV_ATTR_ALWAYS_INLINE fast_floorf(float x)
{
    int i;
    asm volatile (
            "vcvt.S32.f32  %[r], %[x]\n"
            : [r] "=t" (i)
            : [x] "t"  (x));
    return i;
}

int OMV_ATTR_ALWAYS_INLINE fast_ceilf(float x)
{
    int i;
    x += 0.9999f;
    asm volatile (
            "vcvt.S32.f32  %[r], %[x]\n"
            : [r] "=t" (i)
            : [x] "t"  (x));
    return i;
}

int OMV_ATTR_ALWAYS_INLINE fast_roundf(float x)
{
    int i;
    asm volatile (
            "vcvtr.s32.f32  %[r], %[x]\n"
            : [r] "=t" (i)
            : [x] "t"  (x));
    return i;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
typedef union{
    uint32_t l;
    struct {
        uint32_t m : 20;
        uint32_t e : 11;
        uint32_t s : 1;
    };
}exp_t;

float fast_expf(float x)
{
    exp_t e;
    e.l = (uint32_t)(1512775 * x + 1072632447);
    // IEEE binary32 format
    e.e = (e.e -1023 + 127) &0xFF; // rebase

    uint32_t packed = (e.s << 31) | (e.e << 23) | e.m <<3;
    return *((float*)&packed);
}
#pragma GCC diagnostic pop

/*
 * From Hackers Delight:
 * This is a very approximate but very fast version of acbrt. It is just eight
 * integer instructions (shift rights and adds), plus instructions to load the constant.
 * 1/3 is approximated as 1/4 + 1/16 + 1/64 + 1/256 + ... + 1/65536.
 * The constant 0x2a511cd0 balances the relative error at +-0.0321.
 */
float fast_cbrtf(float x)
{
   union {int ix; float x;} v;
   v.x = x;                // x can be viewed as int.
   v.ix = v.ix/4 + v.ix/16; // Approximate divide by 3.
   v.ix = v.ix + v.ix/16;
   v.ix = v.ix + v.ix/256;
   v.ix = 0x2a511cd0 + v.ix;  // Initial guess.
   return v.x;
}

float OMV_ATTR_ALWAYS_INLINE fast_fabsf(float x)
{
    asm volatile (
            "vabs.f32  %[r], %[x]\n"
            : [r] "=t" (x)
            : [x] "t"  (x));
    return x;
}

inline float fast_atanf(float xx)
{
    float x, y, z;
    int sign;

    x = xx;

    /* make argument positive and save the sign */
    if( xx < 0.0f )
    {
        sign = -1;
        x = -xx;
    }
    else
    {
        sign = 1;
        x = xx;
    }
    /* range reduction */
    if( x > 2.414213562373095f )  /* tan 3pi/8 */
    {
        y = M_PI_2;
        x = -( 1.0f/x );
    }

    else if( x > 0.4142135623730950f ) /* tan pi/8 */
    {
        y = M_PI_4;
        x = (x-1.0f)/(x+1.0f);
    }
    else
        y = 0.0f;

    z = x * x;
    y +=
        ((( 8.05374449538e-2f  * z
          - 1.38776856032E-1f) * z
          + 1.99777106478E-1f) * z
          - 3.33329491539E-1f) * z * x + x;

    if( sign < 0 )
        y = -y;

    return( y );
}

float fast_atan2f(float y, float x)
{
  if(x > 0 && y >= 0)
    return fast_atanf(y/x);

  if(x < 0 && y >= 0)
    return M_PI - fast_atanf(-y/x);

  if(x < 0 && y < 0)
    return M_PI + fast_atanf(y/x);

  if(x > 0 && y < 0)
    return 2*M_PI - fast_atanf(-y/x);

  return (y == 0) ? 0 : ((y > 0) ? M_PI : -M_PI);
}

float fast_log2(float x)
{
  union { float f; uint32_t i; } vx = { x };
  union { uint32_t i; float f; } mx = { (vx.i & 0x007FFFFF) | 0x3f000000 };
  float y = vx.i;
  y *= 1.1920928955078125e-7f;

  return y - 124.22551499f - 1.498030302f * mx.f
           - 1.72587999f / (0.3520887068f + mx.f);
}

float fast_log(float x)
{
  return 0.69314718f * fast_log2 (x);
}

float fast_powf(float a, float b)
{
    union { float d; int x; } u = { a };
    u.x = (int)((b * (u.x - 1064866805)) + 1064866805);
    return u.d;
}

void fast_get_min_max(float *data, size_t data_len, float *p_min, float *p_max)
{
    float min = FLT_MAX, max = -FLT_MAX;

    for (size_t i = 0; i < data_len; i++) {
        float temp = data[i];

        if (temp < min) {
            min = temp;
        }

        if (temp > max) {
            max = temp;
        }
    }

    *p_min = min;
    *p_max = max;
}

/* Copyright 2020 Google LLC. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <algorithm>
#include <cstdint>
#include <cstring>

#include "ruy/check_macros.h"
#include "ruy/kernel_common.h"
#include "ruy/kernel_x86.h"
#include "ruy/opt_set.h"
#include "ruy/platform.h"
#include "ruy/profiler/instrumentation.h"

#if RUY_PLATFORM_AVX && RUY_OPT(ASM)
#include <immintrin.h>  // IWYU pragma: keep
#endif

namespace ruy {

#if !(RUY_PLATFORM_AVX && RUY_OPT(ASM))

void Kernel8bitAvx(const KernelParams8bit<8, 8>&) {
  // CPU-ID-based checks should disable the path that would reach this point.
  RUY_DCHECK(false);
}

void Kernel8bitAvxSingleCol(const KernelParams8bit<8, 8>&) {
  // CPU-ID-based checks should disable the path that would reach this point.
  RUY_DCHECK(false);
}

void KernelFloatAvx(const KernelParamsFloat<8, 8>&) {
  // CPU-ID-based checks should disable the path that would reach this point.
  RUY_DCHECK(false);
}

void KernelFloatAvxSingleCol(const KernelParamsFloat<8, 8>&) {
  // CPU-ID-based checks should disable the path that would reach this point.
  RUY_DCHECK(false);
}

#else  // RUY_PLATFORM_AVX && RUY_OPT(ASM)

static constexpr int kAvx8bitBlockSize = 8;
static constexpr int kAvx8bitInnerSize = 4;

namespace {
namespace intrin_utils {

template <>
inline __m256i mm256_shuffle_epi8<Path::kAvx>(const __m256i& a,
                                              const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i dst_lo = _mm_shuffle_epi8(a_lo, b_lo);
  __m128i dst_hi = _mm_shuffle_epi8(a_hi, b_hi);
  return _mm256_set_m128i(dst_hi, dst_lo);
}

template <>
inline __m128i mm256_extracti128_si256<Path::kAvx>(const __m256i& a,
                                                   const int imm) {
  switch (imm) {
    case 0:
      return _mm256_extractf128_si256(a, 0);
    case 1:
      return _mm256_extractf128_si256(a, 1);
    default:
      RUY_DCHECK_LT(imm, 2);
      return _mm_setzero_si128();
  }
}

template <Path path>
inline __m256i mm256_cvtepi8_epi16(const __m128i& a) {
  // Take the upper 64 bits of a and put in the first 64 bits of 'hi'
  __m128i hi = _mm_unpackhi_epi64(a, _mm_setzero_si128());
  return _mm256_set_m128i(_mm_cvtepi8_epi16(hi), _mm_cvtepi8_epi16(a));
}

template <Path path>
inline __m256i mm256_cvtepi32_epi64(const __m128i& a) {
  // sign extend the 32-bit values in the lower 64 bits of a.
  __m128i lo = _mm_cvtepi32_epi64(a);
  __m128i hi = _mm_cvtepi32_epi64(_mm_unpackhi_epi64(a, _mm_setzero_si128()));
  return _mm256_set_m128i(hi, lo);
}

inline __m128i mm_permute_helper(const __m256i& a, const __m256i& b,
                                 const int imm) {
  __m128i tmp = _mm_setzero_si128();
  if (!(imm & 8)) {
    switch (imm & 3) {
      case 0:
        return _mm256_extractf128_si256(a, 0);
      case 1:
        return _mm256_extractf128_si256(a, 1);
      case 2:
        return _mm256_extractf128_si256(b, 0);
      case 3:
        return _mm256_extractf128_si256(b, 1);
    }
  }
  return tmp;
}

template <Path path>
inline __m256i mm256_permute2x128_si256(const __m256i& a, const __m256i& b,
                                        const int imm) {
  const int lo_imm = imm & 15;
  __m128i lo = mm_permute_helper(a, b, lo_imm);
  const int hi_imm = (imm >> 4) & 15;
  __m128i hi = mm_permute_helper(a, b, hi_imm);
  return _mm256_set_m128i(hi, lo);
}

template <Path path>
inline __m256i mm256_max_epi32(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_max_epi32(a_lo, b_lo);
  __m128i hi = _mm_max_epi32(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

template <Path path>
inline __m256i mm256_min_epi32(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_min_epi32(a_lo, b_lo);
  __m128i hi = _mm_min_epi32(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

template <Path path>
inline __m256i mm256_add_epi32(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_add_epi32(a_lo, b_lo);
  __m128i hi = _mm_add_epi32(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

template <Path path>
inline __m256i mm256_add_epi64(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_add_epi64(a_lo, b_lo);
  __m128i hi = _mm_add_epi64(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

template <Path path>
inline __m256i mm256_slli_epi64(const __m256i& a, int imm) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i lo = _mm_slli_epi64(a_lo, imm);
  __m128i hi = _mm_slli_epi64(a_hi, imm);
  return _mm256_set_m128i(hi, lo);
}

template <Path path>
inline __m256i mm256_mullo_epi32(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_mullo_epi32(a_lo, b_lo);
  __m128i hi = _mm_mullo_epi32(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

// Defined as a macro since `imm` must be an immediate.
#define BlendM128_epi32(a, b, imm) \
  _mm_castps_si128(_mm_blend_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b), imm))

// Defined as a macro since `imm` must be an immediate.
#define BlendM128_epi64(a, b, imm) \
  _mm_castpd_si128(_mm_blend_pd(_mm_castsi128_pd(a), _mm_castsi128_pd(b), imm))

// Defined as a macro since `imm` must be an immediate.
#define mm256_blend_epi32(ans, a, b, imm)              \
  __m128i a_lo = _mm256_extractf128_si256(a, 0);       \
  __m128i a_hi = _mm256_extractf128_si256(a, 1);       \
  __m128i b_lo = _mm256_extractf128_si256(b, 0);       \
  __m128i b_hi = _mm256_extractf128_si256(b, 1);       \
  __m128i lo = BlendM128_epi32(a_lo, b_lo, imm & 0xe); \
  __m128i hi = BlendM128_epi32(a_hi, b_hi, imm >> 4);  \
  ans = _mm256_set_m128i(hi, lo);

#define mm256_shuffle_epi32(ans, a, a_lo, a_hi, imm)   \
  a_lo = _mm256_extractf128_si256(a, 0);               \
  a_hi = _mm256_extractf128_si256(a, 1);               \
  ans = _mm256_set_m128i(_mm_shuffle_epi32(a_hi, imm), \
                         _mm_shuffle_epi32(a_lo, imm));

template <Path path>
inline __m256i mm256_madd_epi16(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_madd_epi16(a_lo, b_lo);
  __m128i hi = _mm_madd_epi16(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

inline __m128i mm_srlv_epi64(const __m128i& a, const __m128i& b) {
  // shift both elements of a by lower 64bits of b.
  __m128i res_lo = _mm_srl_epi64(a, b);
  // shift both elements of a by upper 64bits of b.
  __m128i hi_count = _mm_unpackhi_epi64(b, _mm_setzero_si128());
  __m128i res_hi = _mm_srl_epi64(a, hi_count);
  // Take the lower 64 bits of res_lo and upper 64 bits of res hi
  // 1. Swap the upper and lower 64 bits of res_hi
  __m128i tmp_hi =
      _mm_castpd_si128(_mm_permute_pd(_mm_castsi128_pd(res_hi), 1));
  // The lower 64 bits of res_lo and the lower 64 bits of tmp_hi.
  return _mm_unpacklo_epi64(res_lo, tmp_hi);
}

template <Path path>
inline __m256i mm256_srlv_epi64(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = mm_srlv_epi64(a_lo, b_lo);
  __m128i hi = mm_srlv_epi64(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

template <Path path>
inline __m128i mm_sllv_epi64(const __m128i& a, const __m128i& b) {
  // shift both elements of a by lower 64bits of b.
  __m128i res_lo = _mm_sll_epi64(a, b);
  // shift both elements of a by upper 64bits of b.
  __m128i hi_count = _mm_unpackhi_epi64(b, _mm_setzero_si128());
  __m128i res_hi = _mm_sll_epi64(a, hi_count);
  // Take the lower 64 bits of res_lo and upper 64 bits of res hi
  // 1. Swap the upper and lower 64 bits of res_hi
  __m128i tmp_hi =
      _mm_castpd_si128(_mm_permute_pd(_mm_castsi128_pd(res_hi), 1));
  // The lower 64 bits of res_lo and the lower 64 bits of tmp_hi.
  return _mm_unpacklo_epi64(res_lo, tmp_hi);
}

template <Path path>
inline __m256i mm256_sllv_epi64(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = mm_sllv_epi64<path>(a_lo, b_lo);
  __m128i hi = mm_sllv_epi64<path>(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

#define PermuteM128_epi32(a, imm) \
  _mm_castps_si128(_mm_permute_ps(_mm_castsi128_ps(a), imm));

inline __m128i mm_sllv_epi32(const __m128i& a, const __m128i& b) {
  // shift all elements of a by first 32bits of b.
  __m128i res0 = _mm_sll_epi32(a, BlendM128_epi32(_mm_setzero_si128(), b, 1));

  // put bits 32-63 of b in the first slot.
  __m128i tmp1 = PermuteM128_epi32(b, 1);
  // put bits 32-63 of a in the first slot.
  __m128i a1 = PermuteM128_epi32(a, 1);
  // shift all elements of a by second 32bits of b.
  __m128i res1 =
      _mm_sll_epi32(a1, BlendM128_epi32(_mm_setzero_si128(), tmp1, 1));

  // put bits 64-95 of b in the first slot.
  __m128i tmp2 = PermuteM128_epi32(b, 2);
  // shift all elements of a by third 32bits of b.
  __m128i res2 =
      _mm_sll_epi32(a, BlendM128_epi32(_mm_setzero_si128(), tmp2, 1));

  // put bits 96-127 of b in the first slot.
  __m128i tmp3 = PermuteM128_epi32(b, 3);
  // put bits 96-127 of a in the third slot.
  __m128i a3 = PermuteM128_epi32(a, 48);
  // shift all elements of a3 by fourth 32bits of b.
  __m128i res3 =
      _mm_sll_epi32(a3, BlendM128_epi32(_mm_setzero_si128(), tmp3, 1));

  // Take bits 0-31 of res0, bits 0-31 of res1,
  // bits 64-95 of res2, and bits 64-95 of res3.
  // res0 _ _ _ 0
  // res1 _ _ _ 1
  // res2 _ 2 _ _
  // res3 _ 3 _ _
  // f_01 _ _ 1 0
  // f_23 _ _ 3 2
  __m128i f_01 = _mm_unpacklo_epi32(res0, res1);
  __m128i f_23 = _mm_unpackhi_epi32(res2, res3);
  // The lower 64 bits of res_lo and the lower 64 bits of tmp_hi.
  return _mm_unpacklo_epi64(f_01, f_23);
}

template <Path path>
inline __m256i mm256_sllv_epi32(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = mm_sllv_epi32(a_lo, b_lo);
  __m128i hi = mm_sllv_epi32(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

template <Path path>
inline __m256i mm256_sub_epi32(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_sub_epi32(a_lo, b_lo);
  __m128i hi = _mm_sub_epi32(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

template <Path path>
inline __m256i mm256_mul_epi32(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_mul_epi32(a_lo, b_lo);
  __m128i hi = _mm_mul_epi32(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

// Perform the equivalent of mm256_permutevar8x32 with
// a second argument of {7, 5, 3, 1, 6, 4, 2, 0}
template <Path path>
inline __m256i PermuteEpi32EvenOdds(const __m256i& a) {
  // a_lo = 3 2 1 0
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  // a_hi = 7 6 5 4
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  // shuffle a_lo to get 3 1 2 0
  __m128i tmp_lo = _mm_shuffle_epi32(a_lo, 0xd8);
  // shuffle a_hi to get 7 5 6 4
  __m128i tmp_hi = _mm_shuffle_epi32(a_hi, 0xd8);
  // unpack lo 64 of res_lo and res hi to get 6 4 2 0
  __m128i res_lo = _mm_unpacklo_epi64(tmp_lo, tmp_hi);
  // unpack hi 64 of res_lo and res hi to get 7 5 1 3
  __m128i res_hi = _mm_unpackhi_epi64(tmp_lo, tmp_hi);
  return _mm256_set_m128i(res_hi, res_lo);
}

template <Path path>
inline __m256i AddBiasEpi32(const __m256i& a, const int32_t* bias, int offset) {
  const __m256i bias0 = _mm256_set1_epi32(*(bias + offset));
  return mm256_add_epi32<path>(a, bias0);
}

__m256i mm256_blendv_epi32(const __m256i& a, const __m256i& b,
                           const __m256i& mask) {
  __m256 result =
      _mm256_blendv_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b),
                       _mm256_castsi256_ps(mask));
  return _mm256_castps_si256(result);
}

template <Path path>
inline __m256i mm256_cmpgt_epi32(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_cmpgt_epi32(a_lo, b_lo);
  __m128i hi = _mm_cmpgt_epi32(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

template <Path path>
inline __m256i mm256_srav_epi32(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);

  __m128i r0 = _mm_srai_epi32(a_lo, _mm256_extract_epi32(b, 0));
  __m128i r1 = _mm_srai_epi32(a_lo, _mm256_extract_epi32(b, 1));
  __m128i r2 = _mm_srai_epi32(a_lo, _mm256_extract_epi32(b, 2));
  __m128i r3 = _mm_srai_epi32(a_lo, _mm256_extract_epi32(b, 3));
  __m128i r4 = _mm_srai_epi32(a_hi, _mm256_extract_epi32(b, 4));
  __m128i r5 = _mm_srai_epi32(a_hi, _mm256_extract_epi32(b, 5));
  __m128i r6 = _mm_srai_epi32(a_hi, _mm256_extract_epi32(b, 6));
  __m128i r7 = _mm_srai_epi32(a_hi, _mm256_extract_epi32(b, 7));

  // get element 0 from r0, element 1 from r1
  __m128i r01 = BlendM128_epi32(r0, r1, 2);
  // get element 2 from r2, element 3 from r3
  __m128i r23 = BlendM128_epi32(r2, r3, 8);
  // get element 0 from r4, element 1 from r5
  __m128i r45 = BlendM128_epi32(r4, r5, 2);
  // get element 2 from r6, element 3 from r7
  __m128i r67 = BlendM128_epi32(r6, r7, 8);
  // get lower 64 bits of r01, upper 64 bits of r23
  __m128i r0123 = BlendM128_epi64(r01, r23, 2);
  // get lower 64 bits of r45, upper 64 bits of r67
  __m128i r4567 = BlendM128_epi64(r45, r67, 2);
  return _mm256_set_m128i(r4567, r0123);
}

// AVX doesn't have fused multiply-add so we define an inline function to be
// used in the common code following.
template <>
inline __m256 MulAdd<Path::kAvx>(const __m256& a, const __m256& b,
                                 const __m256& c) {
  const __m256 prod = _mm256_mul_ps(a, b);
  return _mm256_add_ps(prod, c);
}

}  // namespace intrin_utils
}  // namespace

template <Path path>
void Kernel8bitAvxImpl(const KernelParams8bit<8, 8>& params) {
  profiler::ScopeLabel label("Kernel kAvx 8-bit");
  const std::int8_t splitter_idx_data[32] = {
      0, 1, 4, 5, 8,  9,  12, 13,  //
      2, 3, 6, 7, 10, 11, 14, 15,  //
      0, 1, 4, 5, 8,  9,  12, 13,  //
      2, 3, 6, 7, 10, 11, 14, 15   //
  };

  std::int32_t dst_stride = 0;
  if ((params.dst_type_id == DstTypeId<std::int8_t>::kValue) ||
      (params.dst_type_id == DstTypeId<std::uint8_t>::kValue)) {
    dst_stride = params.dst_stride;
  } else if (params.dst_type_id == DstTypeId<std::int16_t>::kValue) {
    dst_stride = params.dst_stride / sizeof(std::int16_t);
  } else if (params.dst_type_id == DstTypeId<std::int32_t>::kValue) {
    dst_stride = params.dst_stride / sizeof(std::int32_t);
  } else {
    RUY_DCHECK(false);
  }

  const std::int8_t* rhs_col_ptr = params.rhs_base_ptr;
  void* dst_col_ptr = params.dst_base_ptr;

  for (int col = params.start_col; col <= params.last_col;
       col += kAvx8bitBlockSize) {
    const std::int8_t* lhs_col_ptr = params.lhs_base_ptr;
    void* dst_ptr = dst_col_ptr;

    const std::int32_t lhs_zero_point = params.lhs_zero_point;
    const bool has_rhs_sums_offsets =
        (params.flags & RUY_ASM_FLAG_HAS_RHS_SUMS) && lhs_zero_point;
    std::int32_t rhs_sums_offsets[8];
    if (has_rhs_sums_offsets) {
      const __m256i rhs_sums_offset_v = intrin_utils::mm256_mullo_epi32<path>(
          _mm256_set1_epi32(lhs_zero_point),
          _mm256_loadu_si256(
              reinterpret_cast<__m256i const*>(&params.rhs_sums[col])));
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(rhs_sums_offsets),
                          rhs_sums_offset_v);
    }

    for (int row = params.start_row; row <= params.last_row;
         row += kAvx8bitBlockSize) {
      int channel =
          (params.flags & RUY_ASM_FLAG_CHANNEL_DIMENSION_IS_COL) ? col : row;
      int multiplier_channel =
          (params.flags & RUY_ASM_FLAG_HAS_PERCHANNEL) ? channel : 0;
      const int residual_rows =
          std::min(params.dst_rows - row, kAvx8bitBlockSize);
      const int residual_cols =
          std::min(params.dst_cols - col, kAvx8bitBlockSize);

      const __m256i splitter_idx = _mm256_loadu_si256(
          reinterpret_cast<__m256i const*>(splitter_idx_data));

      __m256i accum_data_v0;
      __m256i accum_data_v1;
      __m256i accum_data_v2;
      __m256i accum_data_v3;
      __m256i accum_data_v4;
      __m256i accum_data_v5;
      __m256i accum_data_v6;
      __m256i accum_data_v7;

      // initial_accum_data will be the initialize of each of the
      // accum_data_* accumulator registers. We compute into it terms that are
      // identical across columns.
      __m128i initial_accum_data_lo = _mm_set1_epi32(params.prod_zp_depth);
      __m128i initial_accum_data_hi = _mm_set1_epi32(params.prod_zp_depth);

      // In the channels-are-rows case, we can load bias here.
      if ((params.flags & RUY_ASM_FLAG_HAS_BIAS) &&
          !(params.flags & RUY_ASM_FLAG_CHANNEL_DIMENSION_IS_COL)) {
        initial_accum_data_lo = _mm_add_epi32(
            initial_accum_data_lo,
            _mm_loadu_si128(
                reinterpret_cast<const __m128i*>(params.bias + row)));
        initial_accum_data_hi = _mm_add_epi32(
            initial_accum_data_hi,
            _mm_loadu_si128(
                reinterpret_cast<const __m128i*>(params.bias + row + 4)));
      }

      // Adjustments common across columns.
      const std::int32_t rhs_zero_point = params.rhs_zero_point;
      if ((params.flags & RUY_ASM_FLAG_HAS_LHS_SUMS) && rhs_zero_point) {
        const __m128i rhs_zp = _mm_set1_epi32(rhs_zero_point);
        const __m128i lhs_sums_offset_lo = _mm_mullo_epi32(
            rhs_zp, _mm_loadu_si128(reinterpret_cast<__m128i const*>(
                        &params.lhs_sums[row])));
        const __m128i lhs_sums_offset_hi = _mm_mullo_epi32(
            rhs_zp, _mm_loadu_si128(reinterpret_cast<__m128i const*>(
                        &params.lhs_sums[row + 4])));

        initial_accum_data_lo =
            _mm_sub_epi32(initial_accum_data_lo, lhs_sums_offset_lo);
        initial_accum_data_hi =
            _mm_sub_epi32(initial_accum_data_hi, lhs_sums_offset_hi);
      }

      // Adjustments differing across columns.
      if (has_rhs_sums_offsets) {
        __m256i initial_accum_data =
            _mm256_set_m128i(initial_accum_data_hi, initial_accum_data_lo);

        accum_data_v0 = intrin_utils::mm256_sub_epi32<path>(
            initial_accum_data, _mm256_set1_epi32(rhs_sums_offsets[0]));
        accum_data_v1 = intrin_utils::mm256_sub_epi32<path>(
            initial_accum_data, _mm256_set1_epi32(rhs_sums_offsets[1]));
        accum_data_v2 = intrin_utils::mm256_sub_epi32<path>(
            initial_accum_data, _mm256_set1_epi32(rhs_sums_offsets[2]));
        accum_data_v3 = intrin_utils::mm256_sub_epi32<path>(
            initial_accum_data, _mm256_set1_epi32(rhs_sums_offsets[3]));
        accum_data_v4 = intrin_utils::mm256_sub_epi32<path>(
            initial_accum_data, _mm256_set1_epi32(rhs_sums_offsets[4]));
        accum_data_v5 = intrin_utils::mm256_sub_epi32<path>(
            initial_accum_data, _mm256_set1_epi32(rhs_sums_offsets[5]));
        accum_data_v6 = intrin_utils::mm256_sub_epi32<path>(
            initial_accum_data, _mm256_set1_epi32(rhs_sums_offsets[6]));
        accum_data_v7 = intrin_utils::mm256_sub_epi32<path>(
            initial_accum_data, _mm256_set1_epi32(rhs_sums_offsets[7]));
      } else {
        __m256i initial_accum_data =
            _mm256_set_m128i(initial_accum_data_hi, initial_accum_data_lo);
        accum_data_v0 = initial_accum_data;
        accum_data_v1 = initial_accum_data;
        accum_data_v2 = initial_accum_data;
        accum_data_v3 = initial_accum_data;
        accum_data_v4 = initial_accum_data;
        accum_data_v5 = initial_accum_data;
        accum_data_v6 = initial_accum_data;
        accum_data_v7 = initial_accum_data;
      }

      // Finally, in the channels-are-columns case, load bias data here.
      if ((params.flags & RUY_ASM_FLAG_HAS_BIAS) &&
          (params.flags & RUY_ASM_FLAG_CHANNEL_DIMENSION_IS_COL)) {
        accum_data_v0 = intrin_utils::AddBiasEpi32<path>(accum_data_v0,
                                                         params.bias + col, 0);
        accum_data_v1 = intrin_utils::AddBiasEpi32<path>(accum_data_v1,
                                                         params.bias + col, 1);
        accum_data_v2 = intrin_utils::AddBiasEpi32<path>(accum_data_v2,
                                                         params.bias + col, 2);
        accum_data_v3 = intrin_utils::AddBiasEpi32<path>(accum_data_v3,
                                                         params.bias + col, 3);
        accum_data_v4 = intrin_utils::AddBiasEpi32<path>(accum_data_v4,
                                                         params.bias + col, 4);
        accum_data_v5 = intrin_utils::AddBiasEpi32<path>(accum_data_v5,
                                                         params.bias + col, 5);
        accum_data_v6 = intrin_utils::AddBiasEpi32<path>(accum_data_v6,
                                                         params.bias + col, 6);
        accum_data_v7 = intrin_utils::AddBiasEpi32<path>(accum_data_v7,
                                                         params.bias + col, 7);
      }

      const std::int8_t* lhs_ptr = lhs_col_ptr;
      const std::int8_t* rhs_ptr = rhs_col_ptr;
      for (int d = 0; d < params.depth; d += kAvx8bitInnerSize) {
        const __m256i lhs_data =
            _mm256_load_si256(reinterpret_cast<const __m256i*>(lhs_ptr));
        const __m256i rhs_data_8bit =
            _mm256_load_si256(reinterpret_cast<const __m256i*>(rhs_ptr));

        // Each "int32" is two 16-bit RHS values, sign extended from 8-bit.
        std::int32_t rhs_data[16];
        const __m128i rhs_data_bottom_lane =
            _mm256_castsi256_si128(rhs_data_8bit);
        const __m128i rhs_data_top_lane =
            _mm256_extractf128_si256(rhs_data_8bit, 1);
        const __m256i rhs_16_bit_dup_low =
            intrin_utils::mm256_cvtepi8_epi16<path>(rhs_data_bottom_lane);
        const __m256i rhs_16_bit_dup_high =
            intrin_utils::mm256_cvtepi8_epi16<path>(rhs_data_top_lane);
        // Now that we have cast the RHS data, we store it so that each value
        // can be separately loaded in the accumulation loop.
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(rhs_data),
                            rhs_16_bit_dup_low);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(rhs_data + 8),
                            rhs_16_bit_dup_high);

        // NOTE: There may be opportunities for permuting the data in the
        // packing code instead of here.
        const __m256i lhs_data_split =
            intrin_utils::mm256_shuffle_epi8<path>(lhs_data, splitter_idx);
        const __m256i lhs_data_split_expand_bottom =
            intrin_utils::mm256_cvtepi8_epi16<path>(
                _mm256_extractf128_si256(lhs_data_split, 0));
        const __m256i lhs_data_split_expand_top =
            intrin_utils::mm256_cvtepi8_epi16<path>(
                _mm256_extractf128_si256(lhs_data_split, 1));

        // Take bytes 0, 1, 4, 5, 8, 9, ... expanded to 16-bit.
        const __m256i lhs_16_bit_low =
            intrin_utils::mm256_permute2x128_si256<path>(
                lhs_data_split_expand_bottom, lhs_data_split_expand_top, 0x20);
        // Take bytes 2, 3, 6, 7, 10, 11, ... expanded to 16-bit.
        const __m256i lhs_16_bit_high =
            intrin_utils::mm256_permute2x128_si256<path>(
                lhs_data_split_expand_bottom, lhs_data_split_expand_top, 0x31);

        __m256i rhs0 = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(
            rhs_data));  // Load [0 1 2 3 4 5 6 7]
        __m256i rhs1 = _mm256_lddqu_si256(
            reinterpret_cast<const __m256i*>(rhs_data + 8));  // Load [8 - 15]
        __m256i rhs0_3 =
            _mm256_permute2f128_si256(rhs0, rhs0, 0);  // [0 1 2 3 0 1 2 3]
        __m256i rhs4_7 =
            _mm256_permute2f128_si256(rhs0, rhs0, 0x11);  // [4 5 6 7 4 5 6 7]
        __m256i rhs8_11 =
            _mm256_permute2f128_si256(rhs1, rhs1, 0);  // [8 9 10 11 8 9 10 11]
        __m256i rhs12_15 =
            _mm256_permute2f128_si256(rhs1, rhs1, 17);  // [12 - 15, 12 - 15]

        auto process_column = [=](__m256i& rhs_dup_lo, __m256i& rhs_dup_hi,
                                  __m256i& accum) {
          // Perform mul-adds on low and high components of accum separately.
          __m128i accum_lo = _mm256_extractf128_si256(accum, 0);
          __m128i accum_hi = _mm256_extractf128_si256(accum, 1);

          __m128i lhs_lo_0 = _mm256_extractf128_si256(lhs_16_bit_low, 0);
          __m128i lhs_lo_1 = _mm256_extractf128_si256(lhs_16_bit_low, 1);
          __m128i rhs_dup_lo_0 = _mm256_extractf128_si256(rhs_dup_lo, 0);
          __m128i rhs_dup_lo_1 = _mm256_extractf128_si256(rhs_dup_lo, 1);
          __m128i lo_0 = _mm_madd_epi16(lhs_lo_0, rhs_dup_lo_0);
          __m128i lo_1 = _mm_madd_epi16(lhs_lo_1, rhs_dup_lo_1);

          accum_lo = _mm_add_epi32(accum_lo, lo_0);
          accum_hi = _mm_add_epi32(accum_hi, lo_1);

          __m128i lhs_hi_0 = _mm256_extractf128_si256(lhs_16_bit_high, 0);
          __m128i lhs_hi_1 = _mm256_extractf128_si256(lhs_16_bit_high, 1);
          __m128i rhs_dup_hi_0 = _mm256_extractf128_si256(rhs_dup_hi, 0);
          __m128i rhs_dup_hi_1 = _mm256_extractf128_si256(rhs_dup_hi, 1);
          __m128i hi_0 = _mm_madd_epi16(lhs_hi_0, rhs_dup_hi_0);
          __m128i hi_1 = _mm_madd_epi16(lhs_hi_1, rhs_dup_hi_1);

          accum_lo = _mm_add_epi32(accum_lo, hi_0);
          accum_hi = _mm_add_epi32(accum_hi, hi_1);
          accum = _mm256_set_m128i(accum_hi, accum_lo);
        };
        __m256i tmp0, tmp1, tmp2, tmp3;
        __m128i lo0, lo1, hi0, hi1;
        mm256_shuffle_epi32(tmp0, rhs0_3, lo0, hi0, 0);
        mm256_shuffle_epi32(tmp1, rhs0_3, lo1, hi1, 0x55);
        process_column(tmp0, tmp1, accum_data_v0);
        mm256_shuffle_epi32(tmp2, rhs0_3, lo0, hi0, 0xaa);
        mm256_shuffle_epi32(tmp3, rhs0_3, lo1, hi1, 0xff);
        process_column(tmp2, tmp3, accum_data_v1);

        mm256_shuffle_epi32(tmp0, rhs4_7, lo0, hi0, 0);
        mm256_shuffle_epi32(tmp1, rhs4_7, lo1, hi1, 0x55);
        process_column(tmp0, tmp1, accum_data_v2);
        mm256_shuffle_epi32(tmp2, rhs4_7, lo0, hi0, 0xaa);
        mm256_shuffle_epi32(tmp3, rhs4_7, lo1, hi1, 0xff);
        process_column(tmp2, tmp3, accum_data_v3);

        mm256_shuffle_epi32(tmp0, rhs8_11, lo0, hi0, 0);
        mm256_shuffle_epi32(tmp1, rhs8_11, lo1, hi1, 0x55);
        process_column(tmp0, tmp1, accum_data_v4);
        mm256_shuffle_epi32(tmp2, rhs8_11, lo0, hi0, 0xaa);
        mm256_shuffle_epi32(tmp3, rhs8_11, lo1, hi1, 0xff);
        process_column(tmp2, tmp3, accum_data_v5);

        mm256_shuffle_epi32(tmp0, rhs12_15, lo0, hi0, 0);
        mm256_shuffle_epi32(tmp1, rhs12_15, lo1, hi1, 0x55);
        process_column(tmp0, tmp1, accum_data_v6);
        mm256_shuffle_epi32(tmp2, rhs12_15, lo0, hi0, 0xaa);
        mm256_shuffle_epi32(tmp3, rhs12_15, lo1, hi1, 0xff);
        process_column(tmp2, tmp3, accum_data_v7);

        lhs_ptr += kAvx8bitBlockSize * kAvx8bitInnerSize;
        rhs_ptr += kAvx8bitBlockSize * kAvx8bitInnerSize;
      }

      if (params.dst_type_id != DstTypeId<std::int32_t>::kValue) {
        __m256i m_vector;
        __m256i e_vector;
        // Does not make use of RUY_ASM_FLAG_NEEDS_LEFT_SHIFT.
        m_vector = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(
            params.multiplier_fixedpoint + multiplier_channel));
        e_vector = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(
            params.multiplier_exponent + multiplier_channel));

        const __m256i m_64bit_low = intrin_utils::mm256_cvtepi32_epi64<path>(
            _mm256_extractf128_si256(m_vector, 0));
        const __m256i m_64bit_high = intrin_utils::mm256_cvtepi32_epi64<path>(
            _mm256_extractf128_si256(m_vector, 1));

        const __m256i zero_vector = _mm256_setzero_si256();
        const __m256i left_shift =
            intrin_utils::mm256_max_epi32<path>(e_vector, zero_vector);
        const __m256i neg_e_vector =
            intrin_utils::mm256_sub_epi32<path>(zero_vector, e_vector);
        const __m256i right_shift =
            intrin_utils::mm256_max_epi32<path>(neg_e_vector, zero_vector);
        const __m256i final_right_shift = _mm256_set1_epi32(31);
        const __m256i final_right_shift_low =
            intrin_utils::mm256_cvtepi32_epi64<path>(
                _mm256_extractf128_si256(final_right_shift, 0));
        const __m256i final_right_shift_high =
            intrin_utils::mm256_cvtepi32_epi64<path>(
                _mm256_extractf128_si256(final_right_shift, 1));
        const __m256i convert_to_unsigned_64 =
            _mm256_set1_epi64x(0x8000000000000000);

        __m256i post_scaling_offset = _mm256_setzero_si256();

        // A "half" added for rounding prior to truncation of 64-bit value.
        const __m256i offset_vector = intrin_utils::mm256_add_epi64<path>(
            intrin_utils::mm256_slli_epi64<path>(_mm256_set1_epi64x(1), 30),
            convert_to_unsigned_64);

        if (params.dst_zero_point) {
          post_scaling_offset = _mm256_set1_epi32(params.dst_zero_point);
        }

        // We cannot do
        //
        // scaled_v_low =
        //     _mm256_srav_epi64(scaled_v_low, final_right_shift_low);
        // scaled_v_high =
        //     _mm256_srav_epi64(scaled_v_high, final_right_shift_high);
        //
        // since this instruction is not in AVX2. Instead we use
        // _mm256_srlv_epi64, but this is an unsigned shift, so we applied
        // offsets before (convert_to_unsigned_64) and after
        // (convert_to_signed_halved).
        //
        // The overall process is, for 64-bit scaled accumulator:
        // unsigned_accum = signed_accum + 1 << 63;
        // unsigned_accum = (unsigned_accum >> right_shift) >> 31;
        // signed_accum = unsigned_accum - ((1 << 32) >> right_shift) / 2 * 2;

        // There are various ways to repack the results, in the absence of
        // _mm256_cvtepi64_epi32() or anything like it.
        // A.
        // accum_data_v[j] =
        //     _mm256_set_epi32(_mm256_extract_epi32(scaled_v_high, 6),
        //                      _mm256_extract_epi32(scaled_v_high, 4),
        //                      _mm256_extract_epi32(scaled_v_high, 2),
        //                      _mm256_extract_epi32(scaled_v_high, 0),
        //                      _mm256_extract_epi32(scaled_v_low, 6),
        //                      _mm256_extract_epi32(scaled_v_low, 4),
        //                      _mm256_extract_epi32(scaled_v_low, 2),
        //                      _mm256_extract_epi32(scaled_v_low, 0));
        // B.
        // scaled_v_low = _mm256_shuffle_epi32(scaled_v_low, 0xd8);
        // scaled_v_high = _mm256_shuffle_epi32(scaled_v_high, 0xd8);
        // accum_data_v[j] =
        //     _mm256_set_epi64x(_mm256_extract_epi64(scaled_v_high, 2),
        //                       _mm256_extract_epi64(scaled_v_high, 0),
        //                       _mm256_extract_epi64(scaled_v_low, 2),
        //                       _mm256_extract_epi64(scaled_v_low, 0));
        // C.
        // scaled_v_low =
        //     _mm256_permutevar8x32_epi32(scaled_v_low, repack_perm);
        // scaled_v_high =
        //     _mm256_permutevar8x32_epi32(scaled_v_high, repack_perm);
        // accum_data_v[j] =
        //     _mm256_permute2x128_si256(scaled_v_low, scaled_v_high, 0x20);
        //
        // However, we choose the following because it uses two lighter
        // instructions. The permutation does have a longer latency, but this
        // loop can be unrolled.
        // D.
        // scaled_v_high = _mm256_slli_epi64(scaled_v_high, 32);
        // __m256i results =
        //     _mm256_blend_epi32(scaled_v_low, scaled_v_high, 0xaa);
        // results = _mm256_permutevar8x32_epi32(results, repack_perm);
        // accum_data_v[j] = intrin_utils::mm256_add_epi32<path>(results,
        // post_scaling_offset);

        // This multiplier code is complex and expensive enough on x86, that
        // we prefer to implement the channels-are-columns case by transposing
        // around it, rather than duplicate it (which would also require
        // duplicating the above code computing the multiplier constants).
        // This is one instance where channels-are-columns has lower performance
        // than channels-are-rows.
        const bool transpose_around_multiplier =
            (params.flags & RUY_ASM_FLAG_HAS_PERCHANNEL) &&
            (params.flags & RUY_ASM_FLAG_CHANNEL_DIMENSION_IS_COL);
        if (transpose_around_multiplier) {
          // Transpose the 8x8 accumulators block. Will be un-transposed below
          // after the multplier implementation.
          intrin_utils::mm256_transpose8x8_epi32<path>(
              &accum_data_v0, &accum_data_v1, &accum_data_v2, &accum_data_v3,
              &accum_data_v4, &accum_data_v5, &accum_data_v6, &accum_data_v7);
        }

        auto rounding_right_shift = [=](__m256i& results,
                                        const __m256i& exponent) {
          // Construct the "nudge" value for each lane if the exponent is
          // greater than 0. Otherwise, the nudge is 0.
          const __m256i zeros = _mm256_setzero_si256();
          const __m256i mask_rightshift_gtz =
              intrin_utils::mm256_cmpgt_epi32<path>(exponent, zeros);
          const __m256i one_shift_exp_minus1 =
              intrin_utils::mm256_sllv_epi32<path>(
                  _mm256_set1_epi32(1), intrin_utils::mm256_sub_epi32<path>(
                                            exponent, _mm256_set1_epi32(1)));
          __m256i nudge = intrin_utils::mm256_blendv_epi32(
              zeros, one_shift_exp_minus1, mask_rightshift_gtz);
          // Calculate the shifted sum (results + nudge) >> exp.
          const __m256i r_plus_nudge =
              intrin_utils::mm256_add_epi32<path>(results, nudge);
          const __m256i shifted_sum =
              intrin_utils::mm256_srav_epi32<path>(r_plus_nudge, exponent);

          // Identify overflow in each lane and create mask.
          const __m256i one_shift_31minus_exp =
              intrin_utils::mm256_sllv_epi32<path>(
                  _mm256_set1_epi32(1), intrin_utils::mm256_sub_epi32<path>(
                                            _mm256_set1_epi32(31), exponent));
          const __m256i mask_num_plus_nudge_overflow =
              intrin_utils::mm256_cmpgt_epi32<path>(
                  results, intrin_utils::mm256_sub_epi32<path>(
                               _mm256_set1_epi32(0x7fffffff), nudge));
          // Fill results with either (results + nudge) >> exponent or
          // 1 << (31 - exp) in the case of overflow.
          results = intrin_utils::mm256_blendv_epi32(
              shifted_sum, one_shift_31minus_exp, mask_num_plus_nudge_overflow);
        };

        auto apply_multiplier = [=](__m256i& accum) {
          __m256i shifted_accum =
              intrin_utils::mm256_sllv_epi32<path>(accum, left_shift);
          // Apply the fixed-point part of the multiplier.
          __m256i scaled_v_low = intrin_utils::mm256_mul_epi32<path>(
              intrin_utils::mm256_cvtepi32_epi64<path>(
                  _mm256_extractf128_si256(shifted_accum, 0)),
              m_64bit_low);
          __m256i scaled_v_high = intrin_utils::mm256_mul_epi32<path>(
              intrin_utils::mm256_cvtepi32_epi64<path>(
                  _mm256_extractf128_si256(shifted_accum, 1)),
              m_64bit_high);
          scaled_v_low = intrin_utils::mm256_add_epi64<path>(scaled_v_low,
                                                             offset_vector);
          scaled_v_high = intrin_utils::mm256_add_epi64<path>(
              scaled_v_high, offset_vector);

          scaled_v_low = intrin_utils::mm256_srlv_epi64<path>(
              scaled_v_low, final_right_shift_low);
          scaled_v_high = intrin_utils::mm256_srlv_epi64<path>(
              scaled_v_high, final_right_shift_high);
          scaled_v_high =
              intrin_utils::mm256_slli_epi64<path>(scaled_v_high, 32);
          __m256i results;
          mm256_blend_epi32(results, scaled_v_low, scaled_v_high, 0xaa);
          // Permute results to this ordering of int32 elements
          // lo->hi (0, 2, 4, 6, 1, 3, 5, 7);
          results = intrin_utils::PermuteEpi32EvenOdds<path>(results);

          rounding_right_shift(results, right_shift);
          accum =
              intrin_utils::mm256_add_epi32<path>(results, post_scaling_offset);
        };
        apply_multiplier(accum_data_v0);
        apply_multiplier(accum_data_v1);
        apply_multiplier(accum_data_v2);
        apply_multiplier(accum_data_v3);
        apply_multiplier(accum_data_v4);
        apply_multiplier(accum_data_v5);
        apply_multiplier(accum_data_v6);
        apply_multiplier(accum_data_v7);
        // See above comment: here we transpose again to undo the transposition
        // of the 8x8 block of accumulators used to implement the
        // channels-are-columns case.
        if (transpose_around_multiplier) {
          intrin_utils::mm256_transpose8x8_epi32<path>(
              &accum_data_v0, &accum_data_v1, &accum_data_v2, &accum_data_v3,
              &accum_data_v4, &accum_data_v5, &accum_data_v6, &accum_data_v7);
        }
      }
      const __m256i clamp_max_v = _mm256_set1_epi32(params.clamp_max);
      const __m256i clamp_min_v = _mm256_set1_epi32(params.clamp_min);
      const bool store_full_block = (residual_rows == kAvx8bitBlockSize) &&
                                    (residual_cols == kAvx8bitBlockSize);

      __m256i accum_data_v[kAvx8bitBlockSize];
      if (!store_full_block) {
        accum_data_v[0] = accum_data_v0;
        accum_data_v[1] = accum_data_v1;
        accum_data_v[2] = accum_data_v2;
        accum_data_v[3] = accum_data_v3;
        accum_data_v[4] = accum_data_v4;
        accum_data_v[5] = accum_data_v5;
        accum_data_v[6] = accum_data_v6;
        accum_data_v[7] = accum_data_v7;
      }

      if (params.dst_type_id == DstTypeId<std::int8_t>::kValue) {
        std::int8_t* tmp_ptr = static_cast<std::int8_t*>(dst_ptr);
        if (store_full_block) {
          accum_data_v0 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v0, clamp_max_v);
          accum_data_v0 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v0, clamp_min_v);
          accum_data_v1 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v1, clamp_max_v);
          accum_data_v1 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v1, clamp_min_v);
          accum_data_v2 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v2, clamp_max_v);
          accum_data_v2 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v2, clamp_min_v);
          accum_data_v3 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v3, clamp_max_v);
          accum_data_v3 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v3, clamp_min_v);
          accum_data_v4 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v4, clamp_max_v);
          accum_data_v4 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v4, clamp_min_v);
          accum_data_v5 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v5, clamp_max_v);
          accum_data_v5 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v5, clamp_min_v);
          accum_data_v6 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v6, clamp_max_v);
          accum_data_v6 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v6, clamp_min_v);
          accum_data_v7 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v7, clamp_max_v);
          accum_data_v7 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v7, clamp_min_v);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[0 * dst_stride], accum_data_v0);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[1 * dst_stride], accum_data_v1);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[2 * dst_stride], accum_data_v2);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[3 * dst_stride], accum_data_v3);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[4 * dst_stride], accum_data_v4);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[5 * dst_stride], accum_data_v5);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[6 * dst_stride], accum_data_v6);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[7 * dst_stride], accum_data_v7);
        } else {
          for (int j = 0; j < residual_cols; ++j) {
            __m256i result = accum_data_v[j];
            result = intrin_utils::mm256_min_epi32<path>(result, clamp_max_v);
            result = intrin_utils::mm256_max_epi32<path>(result, clamp_min_v);
            intrin_utils::mm256_n_storeu_cvtepi32_epi8<path>(
                tmp_ptr, residual_rows, result);
            tmp_ptr += dst_stride;
          }
        }
        dst_ptr = static_cast<void*>(static_cast<std::int8_t*>(dst_ptr) +
                                     kAvx8bitBlockSize);
      } else if (params.dst_type_id == DstTypeId<std::uint8_t>::kValue) {
        std::uint8_t* tmp_ptr = static_cast<std::uint8_t*>(dst_ptr);
        if (store_full_block) {
          accum_data_v0 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v0, clamp_max_v);
          accum_data_v0 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v0, clamp_min_v);
          accum_data_v1 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v1, clamp_max_v);
          accum_data_v1 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v1, clamp_min_v);
          accum_data_v2 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v2, clamp_max_v);
          accum_data_v2 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v2, clamp_min_v);
          accum_data_v3 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v3, clamp_max_v);
          accum_data_v3 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v3, clamp_min_v);
          accum_data_v4 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v4, clamp_max_v);
          accum_data_v4 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v4, clamp_min_v);
          accum_data_v5 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v5, clamp_max_v);
          accum_data_v5 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v5, clamp_min_v);
          accum_data_v6 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v6, clamp_max_v);
          accum_data_v6 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v6, clamp_min_v);
          accum_data_v7 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v7, clamp_max_v);
          accum_data_v7 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v7, clamp_min_v);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(&tmp_ptr[0],
                                                         accum_data_v0);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(&tmp_ptr[dst_stride],
                                                         accum_data_v1);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[2 * dst_stride], accum_data_v2);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[3 * dst_stride], accum_data_v3);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[4 * dst_stride], accum_data_v4);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[5 * dst_stride], accum_data_v5);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[6 * dst_stride], accum_data_v6);
          intrin_utils::mm256_storeu_cvtepi32_epi8<path>(
              &tmp_ptr[7 * dst_stride], accum_data_v7);
        } else {
          for (int j = 0; j < residual_cols; ++j) {
            __m256i result = accum_data_v[j];
            result = intrin_utils::mm256_min_epi32<path>(result, clamp_max_v);
            result = intrin_utils::mm256_max_epi32<path>(result, clamp_min_v);
            intrin_utils::mm256_n_storeu_cvtepi32_epi8<path>(
                tmp_ptr, residual_rows, result);
            tmp_ptr += dst_stride;
          }
        }
        dst_ptr = static_cast<void*>(static_cast<std::uint8_t*>(dst_ptr) +
                                     kAvx8bitBlockSize);
      } else if (params.dst_type_id == DstTypeId<std::int16_t>::kValue) {
        std::int16_t* tmp_ptr = static_cast<std::int16_t*>(dst_ptr);
        if (store_full_block) {
          accum_data_v0 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v0, clamp_max_v);
          accum_data_v0 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v0, clamp_min_v);
          accum_data_v1 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v1, clamp_max_v);
          accum_data_v1 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v1, clamp_min_v);
          accum_data_v2 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v2, clamp_max_v);
          accum_data_v2 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v2, clamp_min_v);
          accum_data_v3 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v3, clamp_max_v);
          accum_data_v3 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v3, clamp_min_v);
          accum_data_v4 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v4, clamp_max_v);
          accum_data_v4 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v4, clamp_min_v);
          accum_data_v5 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v5, clamp_max_v);
          accum_data_v5 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v5, clamp_min_v);
          accum_data_v6 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v6, clamp_max_v);
          accum_data_v6 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v6, clamp_min_v);
          accum_data_v7 =
              intrin_utils::mm256_min_epi32<path>(accum_data_v7, clamp_max_v);
          accum_data_v7 =
              intrin_utils::mm256_max_epi32<path>(accum_data_v7, clamp_min_v);
          intrin_utils::mm256_storeu_cvtepi32_epi16<path>(&tmp_ptr[0],
                                                          accum_data_v0);
          intrin_utils::mm256_storeu_cvtepi32_epi16<path>(&tmp_ptr[dst_stride],
                                                          accum_data_v1);
          intrin_utils::mm256_storeu_cvtepi32_epi16<path>(
              &tmp_ptr[2 * dst_stride], accum_data_v2);
          intrin_utils::mm256_storeu_cvtepi32_epi16<path>(
              &tmp_ptr[3 * dst_stride], accum_data_v3);
          intrin_utils::mm256_storeu_cvtepi32_epi16<path>(
              &tmp_ptr[4 * dst_stride], accum_data_v4);
          intrin_utils::mm256_storeu_cvtepi32_epi16<path>(
              &tmp_ptr[5 * dst_stride], accum_data_v5);
          intrin_utils::mm256_storeu_cvtepi32_epi16<path>(
              &tmp_ptr[6 * dst_stride], accum_data_v6);
          intrin_utils::mm256_storeu_cvtepi32_epi16<path>(
              &tmp_ptr[7 * dst_stride], accum_data_v7);
        } else {
          for (int j = 0; j < residual_cols; ++j) {
            __m256i result = accum_data_v[j];
            result = intrin_utils::mm256_min_epi32<path>(result, clamp_max_v);
            result = intrin_utils::mm256_max_epi32<path>(result, clamp_min_v);
            intrin_utils::mm256_n_storeu_cvtepi32_epi16<path>(
                tmp_ptr, residual_rows, result);
            tmp_ptr += dst_stride;
          }
        }
        dst_ptr = static_cast<void*>(static_cast<std::int16_t*>(dst_ptr) +
                                     kAvx8bitBlockSize);
      } else if (params.dst_type_id == DstTypeId<std::int32_t>::kValue) {
        if (store_full_block) {
          std::int32_t* tmp_ptr = static_cast<std::int32_t*>(dst_ptr);
          intrin_utils::mm256_storeu_epi32<path>(&tmp_ptr[0], accum_data_v0);
          intrin_utils::mm256_storeu_epi32<path>(&tmp_ptr[dst_stride],
                                                 accum_data_v1);
          intrin_utils::mm256_storeu_epi32<path>(&tmp_ptr[2 * dst_stride],
                                                 accum_data_v2);
          intrin_utils::mm256_storeu_epi32<path>(&tmp_ptr[3 * dst_stride],
                                                 accum_data_v3);
          intrin_utils::mm256_storeu_epi32<path>(&tmp_ptr[4 * dst_stride],
                                                 accum_data_v4);
          intrin_utils::mm256_storeu_epi32<path>(&tmp_ptr[5 * dst_stride],
                                                 accum_data_v5);
          intrin_utils::mm256_storeu_epi32<path>(&tmp_ptr[6 * dst_stride],
                                                 accum_data_v6);
          intrin_utils::mm256_storeu_epi32<path>(&tmp_ptr[7 * dst_stride],
                                                 accum_data_v7);
        } else {
          std::int32_t* dst_block_ptr = static_cast<std::int32_t*>(dst_ptr);
          for (int j = 0; j < residual_cols; ++j) {
            intrin_utils::mm256_n_storeu_epi32<path>(
                dst_block_ptr, residual_rows, accum_data_v[j]);
            dst_block_ptr += dst_stride;
          }
        }
        dst_ptr = static_cast<void*>(static_cast<std::int32_t*>(dst_ptr) +
                                     kAvx8bitBlockSize);
      } else {
        RUY_DCHECK(false);
      }

      lhs_col_ptr += kAvx8bitBlockSize * params.lhs_stride;
    }  // End row-block loop.

    dst_col_ptr = static_cast<void*>(static_cast<char*>(dst_col_ptr) +
                                     kAvx8bitBlockSize * params.dst_stride);
    rhs_col_ptr += kAvx8bitBlockSize * params.rhs_stride;
  }  // End col-block loop.
}  // NOLINT(readability/fn_size)

void Kernel8bitAvx(const KernelParams8bit<8, 8>& params) {
  Kernel8bitAvxImpl<Path::kAvx>(params);
}

template <Path path>
void Kernel8bitAvxSingleColImpl(const KernelParams8bit<8, 8>& params) {
  profiler::ScopeLabel label("Kernel kAvx2 8-bit GEMV");

  RUY_DCHECK_EQ(params.dst_cols, 1);
  RUY_DCHECK_EQ(params.last_col, 0);
  RUY_DCHECK_EQ(params.start_col, 0);

  const std::int8_t splitter_idx_data[32] = {
      0, 1, 4, 5, 8,  9,  12, 13,  //
      2, 3, 6, 7, 10, 11, 14, 15,  //
      0, 1, 4, 5, 8,  9,  12, 13,  //
      2, 3, 6, 7, 10, 11, 14, 15   //
  };

  int bias_ptr_block_increment =
      params.flags & RUY_ASM_FLAG_HAS_BIAS ? kAvx8bitBlockSize : 0;

  const std::int8_t* rhs_col_ptr = params.rhs_base_ptr;
  void* dst_col_ptr = params.dst_base_ptr;
  const std::int32_t* bias_col_ptr = params.bias;
  if (params.flags & RUY_ASM_FLAG_HAS_BIAS) {
    bias_col_ptr += params.start_row;
  }

  const std::int8_t* lhs_col_ptr = params.lhs_base_ptr;
  void* dst_ptr = dst_col_ptr;
  const std::int32_t* bias_ptr = bias_col_ptr;

  const std::int32_t lhs_zero_point = params.lhs_zero_point;
  const bool has_rhs_sums_offsets =
      (params.flags & RUY_ASM_FLAG_HAS_RHS_SUMS) && lhs_zero_point;
  std::int32_t rhs_sums_offsets[8];
  if (has_rhs_sums_offsets) {
    const __m256i rhs_sums_offset_v = intrin_utils::mm256_mullo_epi32<path>(
        _mm256_set1_epi32(lhs_zero_point),
        _mm256_loadu_si256(
            reinterpret_cast<__m256i const*>(&params.rhs_sums[0])));
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(rhs_sums_offsets),
                        rhs_sums_offset_v);
  }

  for (int row = params.start_row; row <= params.last_row;
       row += kAvx8bitBlockSize) {
    const int residual_rows =
        std::min(params.dst_rows - row, kAvx8bitBlockSize);

    const __m256i splitter_idx =
        _mm256_loadu_si256(reinterpret_cast<__m256i const*>(splitter_idx_data));

    __m256i accum_data_v0;

    // Initialize with bias.
    __m256i initial_accum_data =
        _mm256_loadu_si256(reinterpret_cast<const __m256i*>(bias_ptr));
    bias_ptr += bias_ptr_block_increment;

    // Adjustments common across columns.
    const std::int32_t rhs_zero_point = params.rhs_zero_point;
    if ((params.flags & RUY_ASM_FLAG_HAS_LHS_SUMS) && rhs_zero_point) {
      const __m256i lhs_sums_offset = intrin_utils::mm256_mullo_epi32<path>(
          _mm256_set1_epi32(rhs_zero_point),
          _mm256_loadu_si256(
              reinterpret_cast<__m256i const*>(&params.lhs_sums[row])));
      initial_accum_data = intrin_utils::mm256_sub_epi32<path>(
          initial_accum_data, lhs_sums_offset);
    }
    const std::int32_t prod_zp_depth = params.prod_zp_depth;
    if (prod_zp_depth) {
      initial_accum_data = intrin_utils::mm256_add_epi32<path>(
          initial_accum_data, _mm256_set1_epi32(prod_zp_depth));
    }

    // Adjustments differing across columns.
    if (has_rhs_sums_offsets) {
      accum_data_v0 = intrin_utils::mm256_sub_epi32<path>(
          initial_accum_data, _mm256_set1_epi32(rhs_sums_offsets[0]));
    } else {
      accum_data_v0 = initial_accum_data;
    }

    const std::int8_t* lhs_ptr = lhs_col_ptr;
    const std::int8_t* rhs_ptr = rhs_col_ptr;
    for (int d = 0; d < params.depth; d += kAvx8bitInnerSize) {
      const __m256i lhs_data =
          _mm256_load_si256(reinterpret_cast<const __m256i*>(lhs_ptr));
      const __m128i rhs_data_8bit = intrin_utils::mm_loadu_si32<path>(rhs_ptr);

      // Each "int32" is two 16-bit RHS values, sign extended from 8-bit.
      // For simplicity we load 4x the data that we need and process twice the
      // data  that we need  and store only the data we need.
      std::int32_t rhs_data[2];
      const __m128i rhs_16_bit_dup = _mm_cvtepi8_epi16(rhs_data_8bit);
      // Now that we have cast the RHS data, we store it so that each value
      // can be separately loaded in the accumulation loop.
      _mm_storeu_si64(reinterpret_cast<__m128i*>(rhs_data), rhs_16_bit_dup);

      // NOTE: There may be opportunities for permuting the data in the packing
      // code instead of here.
      const __m256i lhs_data_split =
          intrin_utils::mm256_shuffle_epi8<path>(lhs_data, splitter_idx);
      const __m256i lhs_data_split_expand_bottom =
          intrin_utils::mm256_cvtepi8_epi16<path>(
              _mm256_extractf128_si256(lhs_data_split, 0));
      const __m256i lhs_data_split_expand_top =
          intrin_utils::mm256_cvtepi8_epi16<path>(
              _mm256_extractf128_si256(lhs_data_split, 1));

      // Take bytes 0, 1, 4, 5, 8, 9, ... expanded to 16-bit.
      const __m256i lhs_16_bit_low =
          intrin_utils::mm256_permute2x128_si256<path>(
              lhs_data_split_expand_bottom, lhs_data_split_expand_top, 0x20);
      // Take bytes 2, 3, 6, 7, 10, 11, ... expanded to 16-bit.
      const __m256i lhs_16_bit_high =
          intrin_utils::mm256_permute2x128_si256<path>(
              lhs_data_split_expand_bottom, lhs_data_split_expand_top, 0x31);
      // Accumulate for column 0.
      const std::int32_t low_rhs_value = rhs_data[0];
      const std::int32_t high_rhs_value = rhs_data[1];

      const __m256i rhs_16_bit_dup_low = _mm256_set1_epi32(low_rhs_value);
      const __m256i rhs_16_bit_dup_high = _mm256_set1_epi32(high_rhs_value);

      accum_data_v0 = intrin_utils::mm256_add_epi32<path>(
          accum_data_v0, intrin_utils::mm256_madd_epi16<path>(
                             lhs_16_bit_low, rhs_16_bit_dup_low));
      accum_data_v0 = intrin_utils::mm256_add_epi32<path>(
          accum_data_v0, intrin_utils::mm256_madd_epi16<path>(
                             lhs_16_bit_high, rhs_16_bit_dup_high));

      lhs_ptr += kAvx8bitBlockSize * kAvx8bitInnerSize;
      rhs_ptr += kAvx8bitBlockSize * kAvx8bitInnerSize;
    }

    if (params.dst_type_id != DstTypeId<std::int32_t>::kValue) {
      __m256i m_vector;
      __m256i e_vector;
      // Does not make use of RUY_ASM_FLAG_NEEDS_LEFT_SHIFT.
      int channel = (params.flags & RUY_ASM_FLAG_HAS_PERCHANNEL) ? row : 0;
      m_vector = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(
          params.multiplier_fixedpoint + channel));
      e_vector = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(
          params.multiplier_exponent + channel));

      const __m256i m_64bit_low = intrin_utils::mm256_cvtepi32_epi64<path>(
          _mm256_extractf128_si256(m_vector, 0));
      const __m256i m_64bit_high = intrin_utils::mm256_cvtepi32_epi64<path>(
          _mm256_extractf128_si256(m_vector, 1));

      const __m256i zero_vector = _mm256_setzero_si256();
      const __m256i left_shift =
          intrin_utils::mm256_max_epi32<path>(e_vector, zero_vector);
      const __m256i neg_e_vector =
          intrin_utils::mm256_sub_epi32<path>(zero_vector, e_vector);
      const __m256i right_shift =
          intrin_utils::mm256_max_epi32<path>(neg_e_vector, zero_vector);
      const __m256i final_right_shift = _mm256_set1_epi32(31);
      const __m256i final_right_shift_low =
          intrin_utils::mm256_cvtepi32_epi64<path>(
              _mm256_extractf128_si256(final_right_shift, 0));
      const __m256i final_right_shift_high =
          intrin_utils::mm256_cvtepi32_epi64<path>(
              _mm256_extractf128_si256(final_right_shift, 1));
      const __m256i convert_to_unsigned_64 =
          _mm256_set1_epi64x(0x8000000000000000);

      __m256i post_scaling_offset = _mm256_setzero_si256();

      // A "half" added for rounding prior to truncation of 64-bit value.
      const __m256i offset_vector = intrin_utils::mm256_add_epi64<path>(
          intrin_utils::mm256_slli_epi64<path>(_mm256_set1_epi64x(1), 30),
          convert_to_unsigned_64);

      if (params.dst_zero_point) {
        post_scaling_offset = _mm256_set1_epi32(params.dst_zero_point);
      }

      // See GEMM version for details of this process.
      {
        __m256i shifted_accum =
            intrin_utils::mm256_sllv_epi32<path>(accum_data_v0, left_shift);
        // Apply the fixed-point part of the multiplier.
        __m256i scaled_v_low = intrin_utils::mm256_mul_epi32<path>(
            intrin_utils::mm256_cvtepi32_epi64<path>(
                _mm256_extractf128_si256(shifted_accum, 0)),
            m_64bit_low);
        __m256i scaled_v_high = intrin_utils::mm256_mul_epi32<path>(
            intrin_utils::mm256_cvtepi32_epi64<path>(
                _mm256_extractf128_si256(shifted_accum, 1)),
            m_64bit_high);

        scaled_v_low = intrin_utils::mm256_add_epi64<path>(scaled_v_low,
                                                           offset_vector);
        scaled_v_high = intrin_utils::mm256_add_epi64<path>(scaled_v_high,
                                                            offset_vector);

        scaled_v_low = intrin_utils::mm256_srlv_epi64<path>(
            scaled_v_low, final_right_shift_low);
        scaled_v_high = intrin_utils::mm256_srlv_epi64<path>(
            scaled_v_high, final_right_shift_high);

        scaled_v_high = intrin_utils::mm256_slli_epi64<path>(scaled_v_high, 32);
        __m256i results;
        mm256_blend_epi32(results, scaled_v_low, scaled_v_high, 0xaa);
        // Permute results to this ordering of int32 elements
        // lo->hi (0, 2, 4, 6, 1, 3, 5, 7);
        results = intrin_utils::PermuteEpi32EvenOdds<path>(results);

        // Now perform the Rounding Right Shift.
        // First, construct the "nudge" value for each lane if the exponent is
        // greater than 0. Otherwise, the nudge is 0.
        const __m256i zeros = _mm256_setzero_si256();
        const __m256i mask_rightshift_gtz =
            intrin_utils::mm256_cmpgt_epi32<path>(right_shift, zeros);
        const __m256i one_shift_exp_minus1 =
            intrin_utils::mm256_sllv_epi32<path>(
                _mm256_set1_epi32(1), intrin_utils::mm256_sub_epi32<path>(
                                          right_shift, _mm256_set1_epi32(1)));
        __m256i nudge = intrin_utils::mm256_blendv_epi32(
            zeros, one_shift_exp_minus1, mask_rightshift_gtz);
        // Calculate the shifted sum (results + nudge) >> exp.
        const __m256i r_plus_nudge =
            intrin_utils::mm256_add_epi32<path>(results, nudge);
        const __m256i shifted_sum =
            intrin_utils::mm256_srav_epi32<path>(r_plus_nudge, right_shift);

        // Identify overflow in each lane and create mask.
        const __m256i one_shift_31minus_exp =
            intrin_utils::mm256_sllv_epi32<path>(
                _mm256_set1_epi32(1), intrin_utils::mm256_sub_epi32<path>(
                                          _mm256_set1_epi32(31), right_shift));
        const __m256i mask_num_plus_nudge_overflow =
            intrin_utils::mm256_cmpgt_epi32<path>(
                results, intrin_utils::mm256_sub_epi32<path>(
                             _mm256_set1_epi32(0x7fffffff), nudge));
        // Fill results with either (results + nudge) >> exponent or
        // 1 << (31 - exp) in the case of overflow.
        results = intrin_utils::mm256_blendv_epi32(
            shifted_sum, one_shift_31minus_exp, mask_num_plus_nudge_overflow);
        accum_data_v0 =
            intrin_utils::mm256_add_epi32<path>(results, post_scaling_offset);
      }
    }
    const __m256i clamp_max_v = _mm256_set1_epi32(params.clamp_max);
    const __m256i clamp_min_v = _mm256_set1_epi32(params.clamp_min);

    if (params.dst_type_id == DstTypeId<std::int8_t>::kValue) {
      std::int8_t* tmp_ptr = static_cast<std::int8_t*>(dst_ptr);
      __m256i result = accum_data_v0;
      result = intrin_utils::mm256_min_epi32<path>(result, clamp_max_v);
      result = intrin_utils::mm256_max_epi32<path>(result, clamp_min_v);
      intrin_utils::mm256_n_storeu_cvtepi32_epi8<path>(tmp_ptr, residual_rows,
                                                       result);
      dst_ptr = static_cast<void*>(static_cast<std::int8_t*>(dst_ptr) +
                                   kAvx8bitBlockSize);
    } else if (params.dst_type_id == DstTypeId<std::uint8_t>::kValue) {
      std::uint8_t* tmp_ptr = static_cast<std::uint8_t*>(dst_ptr);
      __m256i result = accum_data_v0;
      result = intrin_utils::mm256_min_epi32<path>(result, clamp_max_v);
      result = intrin_utils::mm256_max_epi32<path>(result, clamp_min_v);
      intrin_utils::mm256_n_storeu_cvtepi32_epi8<path>(tmp_ptr, residual_rows,
                                                       result);
      dst_ptr = static_cast<void*>(static_cast<std::uint8_t*>(dst_ptr) +
                                   kAvx8bitBlockSize);
    } else if (params.dst_type_id == DstTypeId<std::int16_t>::kValue) {
      std::int16_t* tmp_ptr = static_cast<std::int16_t*>(dst_ptr);
      __m256i result = accum_data_v0;
      result = intrin_utils::mm256_min_epi32<path>(result, clamp_max_v);
      result = intrin_utils::mm256_max_epi32<path>(result, clamp_min_v);
      intrin_utils::mm256_n_storeu_cvtepi32_epi16<path>(tmp_ptr, residual_rows,
                                                        result);
      dst_ptr = static_cast<void*>(static_cast<std::int16_t*>(dst_ptr) +
                                   kAvx8bitBlockSize);
    } else if (params.dst_type_id == DstTypeId<std::int32_t>::kValue) {
      std::int32_t* dst_block_ptr = static_cast<std::int32_t*>(dst_ptr);
      intrin_utils::mm256_n_storeu_epi32<path>(dst_block_ptr, residual_rows,
                                               accum_data_v0);
      dst_ptr = static_cast<void*>(static_cast<std::int32_t*>(dst_ptr) +
                                   kAvx8bitBlockSize);
    } else {
      RUY_DCHECK(false);
    }

    lhs_col_ptr += kAvx8bitBlockSize * params.lhs_stride;
  }  // End row-block loop.

  dst_col_ptr = static_cast<void*>(static_cast<char*>(dst_col_ptr) +
                                   kAvx8bitBlockSize * params.dst_stride);
  rhs_col_ptr += kAvx8bitBlockSize * params.rhs_stride;
}  // NOLINT(readability/fn_size)

void Kernel8bitAvxSingleCol(const KernelParams8bit<8, 8>& params) {
  Kernel8bitAvxSingleColImpl<Path::kAvx>(params);
}

void KernelFloatAvx(const KernelParamsFloat<8, 8>& params) {
  profiler::ScopeLabel label("Kernel kAvx float");
  KernelFloatAvxCommon<Path::kAvx>(params);
}

void KernelFloatAvxSingleCol(const KernelParamsFloat<8, 8>& params) {
  profiler::ScopeLabel label("Kernel kAvx float GEMV");
  KernelFloatAvxCommonSingleCol<Path::kAvx>(params);
}

#endif  //  RUY_PLATFORM_AVX && RUY_OPT(ASM)

}  // namespace ruy

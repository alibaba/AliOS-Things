/* Copyright 2019 Google LLC. All Rights Reserved.

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

#ifndef RUY_RUY_KERNEL_X86_H_
#define RUY_RUY_KERNEL_X86_H_

#include <cstdint>
#include <cstring>

#include "ruy/kernel_common.h"
#include "ruy/mat.h"
#include "ruy/mul_params.h"
#include "ruy/opt_set.h"
#include "ruy/path.h"
#include "ruy/platform.h"
#include "ruy/tune.h"

namespace ruy {

#if RUY_PLATFORM_X86

RUY_INHERIT_KERNEL(Path::kStandardCpp, Path::kAvx2Fma)
RUY_INHERIT_KERNEL(Path::kStandardCpp, Path::kAvx)
RUY_INHERIT_KERNEL(Path::kAvx2Fma, Path::kAvx512)

void Kernel8bitAvx512(const KernelParams8bit<16, 16>& params);
void Kernel8bitAvx512SingleCol(const KernelParams8bit<16, 16>& params);

template <typename DstScalar>
struct Kernel<Path::kAvx512, std::int8_t, std::int8_t, std::int32_t, DstScalar> {
  static constexpr Path kPath = Path::kAvx512;
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kColMajor, 4, 16>;
  using RhsLayout = FixedKernelLayout<Order::kColMajor, 4, 16>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PMat<std::int8_t>& lhs, const PMat<std::int8_t>& rhs,
           const MulParams<std::int32_t, DstScalar>& mul_params, int start_row,
           int start_col, int end_row, int end_col, Mat<DstScalar>* dst) const {
    KernelParams8bit<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParams8bit(lhs, rhs, mul_params, start_row, start_col, end_row,
                         end_col, dst, &params);
    if (dst->layout.cols == 1 &&
        mul_params.channel_dimension() == ChannelDimension::kRow) {
      Kernel8bitAvx512SingleCol(params);
    } else {
      Kernel8bitAvx512(params);
    }
  }
};

void KernelFloatAvx512(const KernelParamsFloat<16, 16>& params);
void KernelFloatAvx512SingleCol(const KernelParamsFloat<16, 16>& param);

template <>
struct Kernel<Path::kAvx512, float, float, float, float> {
  static constexpr Path kPath = Path::kAvx512;
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 16>;
  using RhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 16>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PMat<float>& lhs, const PMat<float>& rhs,
           const MulParams<float, float>& mul_params, int start_row,
           int start_col, int end_row, int end_col, Mat<float>* dst) const {
    KernelParamsFloat<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParamsFloat(lhs, rhs, mul_params, start_row, start_col, end_row,
                          end_col, dst, &params);
    if (dst->layout.cols == 1 &&
        mul_params.channel_dimension() == ChannelDimension::kRow) {
      KernelFloatAvx512SingleCol(params);
    } else {
      KernelFloatAvx512(params);
    }
  }
};

void Kernel8bitAvx2(const KernelParams8bit<8, 8>& params);
void Kernel8bitAvx2SingleCol(const KernelParams8bit<8, 8>& params);

template <typename DstScalar>
struct Kernel<Path::kAvx2Fma, std::int8_t, std::int8_t, std::int32_t,
              DstScalar> {
  static constexpr Path kPath = Path::kAvx2Fma;
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kColMajor, 4, 8>;
  using RhsLayout = FixedKernelLayout<Order::kColMajor, 4, 8>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PMat<std::int8_t>& lhs, const PMat<std::int8_t>& rhs,
           const MulParams<std::int32_t, DstScalar>& mul_params, int start_row,
           int start_col, int end_row, int end_col, Mat<DstScalar>* dst) const {
    KernelParams8bit<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParams8bit(lhs, rhs, mul_params, start_row, start_col, end_row,
                         end_col, dst, &params);
    if (dst->layout.cols == 1 &&
        mul_params.channel_dimension() == ChannelDimension::kRow) {
      Kernel8bitAvx2SingleCol(params);
    } else {
      Kernel8bitAvx2(params);
    }
  }
};

void KernelFloatAvx2(const KernelParamsFloat<8, 8>& params);
void KernelFloatAvx2SingleCol(const KernelParamsFloat<8, 8>& params);

template <>
struct Kernel<Path::kAvx2Fma, float, float, float, float> {
  static constexpr Path kPath = Path::kAvx2Fma;
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 8>;
  using RhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 8>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PMat<float>& lhs, const PMat<float>& rhs,
           const MulParams<float, float>& mul_params, int start_row,
           int start_col, int end_row, int end_col, Mat<float>* dst) const {
    KernelParamsFloat<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParamsFloat(lhs, rhs, mul_params, start_row, start_col, end_row,
                          end_col, dst, &params);
    if (dst->layout.cols == 1 &&
        mul_params.channel_dimension() == ChannelDimension::kRow) {
      KernelFloatAvx2SingleCol(params);
    } else {
      KernelFloatAvx2(params);
    }
  }
};

void KernelFloatAvx(const KernelParamsFloat<8, 8>& params);
void KernelFloatAvxSingleCol(const KernelParamsFloat<8, 8>& params);

template <>
struct Kernel<Path::kAvx, float, float, float, float> {
  static constexpr Path kPath = Path::kAvx;
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 8>;
  using RhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 8>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PMat<float>& lhs, const PMat<float>& rhs,
           const MulParams<float, float>& mul_params, int start_row,
           int start_col, int end_row, int end_col, Mat<float>* dst) const {
    KernelParamsFloat<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParamsFloat(lhs, rhs, mul_params, start_row, start_col, end_row,
                          end_col, dst, &params);
    if (dst->layout.cols == 1 &&
        mul_params.channel_dimension() == ChannelDimension::kRow) {
      KernelFloatAvxSingleCol(params);
    } else {
      KernelFloatAvx(params);
    }
  }
};

void Kernel8bitAvx(const KernelParams8bit<8, 8>& params);
void Kernel8bitAvxSingleCol(const KernelParams8bit<8, 8>& params);

template <typename DstScalar>
struct Kernel<Path::kAvx, std::int8_t, std::int8_t, std::int32_t, DstScalar> {
  static constexpr Path kPath = Path::kAvx2Fma;
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kColMajor, 4, 8>;
  using RhsLayout = FixedKernelLayout<Order::kColMajor, 4, 8>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PMat<std::int8_t>& lhs, const PMat<std::int8_t>& rhs,
           const MulParams<std::int32_t, DstScalar>& mul_params, int start_row,
           int start_col, int end_row, int end_col, Mat<DstScalar>* dst) const {
    KernelParams8bit<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParams8bit(lhs, rhs, mul_params, start_row, start_col, end_row,
                         end_col, dst, &params);
    if (dst->layout.cols == 1 &&
        mul_params.channel_dimension() == ChannelDimension::kRow) {
      Kernel8bitAvxSingleCol(params);
    } else {
      Kernel8bitAvx(params);
    }
  }
};

#endif  // RUY_PLATFORM_X86
}  // namespace ruy

#if ((RUY_PLATFORM_AVX || RUY_PLATFORM_AVX2_FMA) && RUY_OPT(ASM))

#include <immintrin.h>  // IWYU pragma: keep

namespace ruy {
namespace {
namespace intrin_utils {

// Defined as a template so clang won't detect it as an uneeded
// definition.
template <Path path>
inline float mm256_get1_ps(const __m256 a, int i) {
  __m256i ai = _mm256_castps_si256(a);
  int float_val_as_int;
  switch (i) {
    case 0:
      float_val_as_int = _mm256_extract_epi32(ai, 0);
      break;
    case 1:
      float_val_as_int = _mm256_extract_epi32(ai, 1);
      break;
    case 2:
      float_val_as_int = _mm256_extract_epi32(ai, 2);
      break;
    case 3:
      float_val_as_int = _mm256_extract_epi32(ai, 3);
      break;
    case 4:
      float_val_as_int = _mm256_extract_epi32(ai, 4);
      break;
    case 5:
      float_val_as_int = _mm256_extract_epi32(ai, 5);
      break;
    case 6:
      float_val_as_int = _mm256_extract_epi32(ai, 6);
      break;
    case 7:
      float_val_as_int = _mm256_extract_epi32(ai, 7);
      break;
    default:
      RUY_DCHECK_LT(i, 8);
      return .0f;
  }
  float float_val;
  std::memcpy(&float_val, &float_val_as_int, sizeof(float_val));
  return float_val;
}

// Defined as a template so clang won't detect it as an uneeded
// definition.
template <Path path>
inline void mm256_n_storeu_ps(float* dst, int residual_rows, const __m256 v) {
  for (int i = 0; i < residual_rows; ++i) {
    dst[i] = intrin_utils::mm256_get1_ps<path>(v, i);
  }
}

template <Path path>
inline __m256 MulAdd(const __m256&, const __m256&, const __m256&) {
  // Specializations added for AVX and AVX2FMA paths in their respective kernel
  // files.
  RUY_DCHECK(false);
  return _mm256_set1_ps(0);
}

template <Path path>
inline __m256i mm256_shuffle_epi8(const __m256i&, const __m256i&) {
  // Specializations added for AVX and AVX2FMA paths in their respective kernel
  // files.
  RUY_DCHECK(false);
  return _mm256_set1_epi32(0);
}

// Polyfill for _mm_storeu_si16(dst, v).
template <Path path>
inline void mm_storeu_si16(void* dst, __m128i v) {
#if (defined __clang__) || (defined _MSC_VER)
  _mm_storeu_si16(dst, v);
#else
  // GCC 9 lacks support for __mm_storeu_si16.
  *static_cast<std::int16_t*>(dst) = _mm_extract_epi16(v, 0);
#endif
}

// Polyfill for _mm_storeu_si32(dst, v).
template <Path path>
inline void mm_storeu_si32(void* dst, __m128i v) {
#if (defined __clang__) || (defined _MSC_VER)
  _mm_storeu_si32(dst, v);
#else
  // GCC 9 lacks support for __mm_storeu_si32.
  *static_cast<std::int32_t*>(dst) = _mm_extract_epi32(v, 0);
#endif
}

// Polyfill for _mm_loadu_si32(src).
template <Path path>
inline __m128i mm_loadu_si32(const void* src) {
#if (defined __clang__) || (defined _MSC_VER)
  return _mm_loadu_si32(src);
#else
  // GCC 9 lacks support for _mm_loadu_si32.
  __m128i res;
  asm("movss %[src], %[res]"
      : [res] "=x"(res)
      : [src] "m"(*static_cast<const int*>(src)));
  return res;
#endif
}

template <Path path>
inline __m128i mm256_extracti128_si256(const __m256i&, const int) {
  RUY_DCHECK(false);
  return _mm_setzero_si128();
}

template <Path path>
inline void mm256_n_storeu_cvtepi32_epi8(std::uint8_t* dst, int residual_rows,
                                         const __m256i v) {
  // Select bytes 0, 4, 8, 12 within each lane, effectively truncating.
  const __m256i repack_perm = _mm256_set1_epi32(0x0c080400);
  __m256i shuffled_v;
  if (residual_rows > 1) {
    // This selects 0, 4, 8, 12, 0, 4, 8, 12, ..., but we only use the first 4
    // in each 128-bit lane.
    shuffled_v = intrin_utils::mm256_shuffle_epi8<path>(v, repack_perm);
  }
  switch (residual_rows) {
    case 0:
      break;
    case 1:
      dst[0] = _mm256_extract_epi8(v, 0);
      break;
    case 2:
      mm_storeu_si16<path>(dst, mm256_extracti128_si256<path>(shuffled_v, 0));
      break;
    case 3: {
      __m128i trailing_packed = mm256_extracti128_si256<path>(shuffled_v, 0);
      mm_storeu_si16<path>(dst, trailing_packed);
      dst[2] = _mm_extract_epi8(trailing_packed, 2);
      break;
    }
    case 4:
      mm_storeu_si32<path>(dst, mm256_extracti128_si256<path>(shuffled_v, 0));
      break;
    case 5:
      mm_storeu_si32<path>(dst, mm256_extracti128_si256<path>(shuffled_v, 0));
      dst[4] = _mm256_extract_epi8(shuffled_v, 16);
      break;
    case 6:
      mm_storeu_si32<path>(dst, mm256_extracti128_si256<path>(shuffled_v, 0));
      mm_storeu_si16<path>(dst + 4,
                           mm256_extracti128_si256<path>(shuffled_v, 1));
      break;
    case 7: {
      mm_storeu_si32<path>(dst, mm256_extracti128_si256<path>(shuffled_v, 0));
      __m128i trailing_packed = mm256_extracti128_si256<path>(shuffled_v, 1);
      mm_storeu_si16<path>(dst + 4, trailing_packed);
      dst[6] = _mm_extract_epi8(trailing_packed, 2);
      break;
    }
    case 8:
      mm_storeu_si32<path>(dst, mm256_extracti128_si256<path>(shuffled_v, 0));
      mm_storeu_si32<path>(dst + 4,
                           mm256_extracti128_si256<path>(shuffled_v, 1));
      break;
    default:
      RUY_DCHECK_LE(residual_rows, 8);
      break;
  }
}

template <Path path>
inline void mm256_storeu_cvtepi32_epi8(std::uint8_t* dst, const __m256i v) {
  // Select bytes 0, 4, 8, 12 within each lane, effectively truncating.
  const __m256i repack_perm = _mm256_set1_epi32(0x0c080400);
  const __m256i shuffled_v = mm256_shuffle_epi8<path>(v, repack_perm);
  mm_storeu_si32<path>(dst, mm256_extracti128_si256<path>(shuffled_v, 0));
  mm_storeu_si32<path>(dst + 4, mm256_extracti128_si256<path>(shuffled_v, 1));
}

template <Path path>
inline void mm256_n_storeu_cvtepi32_epi8(std::int8_t* dst, int residual_rows,
                                         const __m256i v) {
  intrin_utils::mm256_n_storeu_cvtepi32_epi8<path>(
      reinterpret_cast<std::uint8_t*>(dst), residual_rows, v);
}

template <Path path>
inline void mm256_storeu_cvtepi32_epi8(std::int8_t* dst, const __m256i v) {
  // Select bytes 0, 4, 8, 12 within each lane, effectively truncating.
  const __m256i repack_perm = _mm256_set1_epi32(0x0c080400);
  const __m256i shuffled_v = mm256_shuffle_epi8<path>(v, repack_perm);
  mm_storeu_si32<path>(dst, mm256_extracti128_si256<path>(shuffled_v, 0));
  mm_storeu_si32<path>(dst + 4, mm256_extracti128_si256<path>(shuffled_v, 1));
}

template <Path path>
inline void mm256_n_storeu_cvtepi32_epi16(std::int16_t* dst, int residual_rows,
                                          const __m256i v) {
  // Select bytes 0, 1, 4, 5, 8, 9, 12, 13 within each lane, effectively
  // truncating each 16-bit integer.
  const __m256i repack_perm = _mm256_set1_epi64x(0x0d0c090805040100);
  __m256i shuffled_v;
  __m128i shuffled_v_low;
  if (residual_rows > 1) {
    shuffled_v = mm256_shuffle_epi8<path>(v, repack_perm);
    shuffled_v_low = mm256_extracti128_si256<path>(shuffled_v, 0);
  } else {
    shuffled_v_low = mm256_extracti128_si256<path>(v, 0);
  }
  switch (residual_rows) {
    case 0:
      break;
    case 1:
      mm_storeu_si16<path>(dst, shuffled_v_low);
      break;
    case 2:
      mm_storeu_si32<path>(dst, shuffled_v_low);
      break;
    case 3: {
      mm_storeu_si32<path>(dst, shuffled_v_low);
      dst[2] = _mm_extract_epi16(shuffled_v_low, 2);
      break;
    }
    case 4:
      _mm_storeu_si64(dst, shuffled_v_low);
      break;
    case 5:
      _mm_storeu_si64(dst, shuffled_v_low);
      dst[4] = _mm256_extract_epi16(shuffled_v, 8);
      break;
    case 6:
      _mm_storeu_si64(dst, shuffled_v_low);
      mm_storeu_si32<path>(dst + 4,
                           mm256_extracti128_si256<path>(shuffled_v, 1));
      break;
    case 7: {
      _mm_storeu_si64(dst, shuffled_v_low);
      __m128i trailing_packed = mm256_extracti128_si256<path>(shuffled_v, 1);
      mm_storeu_si32<path>(dst + 4, trailing_packed);
      dst[6] = _mm_extract_epi16(trailing_packed, 2);
      break;
    }
    case 8:
      _mm_storeu_si64(dst, mm256_extracti128_si256<path>(shuffled_v, 0));
      _mm_storeu_si64(dst + 4, mm256_extracti128_si256<path>(shuffled_v, 1));
      break;
    default:
      RUY_DCHECK_LE(residual_rows, 8);
      break;
  }
}

template <Path path>
inline void mm256_storeu_cvtepi32_epi16(std::int16_t* dst, const __m256i v) {
  // Select bytes 0, 1, 4, 5, 8, 9, 12, 13 within each lane, effectively
  // truncating each 16-bit integer.
  const __m256i repack_perm = _mm256_set1_epi64x(0x0d0c090805040100);
  const __m256i shuffled_v = mm256_shuffle_epi8<path>(v, repack_perm);
  _mm_storeu_si64(dst, mm256_extracti128_si256<path>(shuffled_v, 0));
  _mm_storeu_si64(dst + 4, mm256_extracti128_si256<path>(shuffled_v, 1));
}

template <Path path>
inline void mm256_n_storeu_epi32(std::int32_t* dst, int residual_rows,
                                 const __m256i v) {
  const __m128i v_low = mm256_extracti128_si256<path>(v, 0);
  switch (residual_rows) {
    case 0:
      break;
    case 1:
      mm_storeu_si32<path>(dst, v_low);
      break;
    case 2:
      _mm_storeu_si64(dst, v_low);
      break;
    case 3: {
      __m128i trailing_packed = v_low;
      _mm_storeu_si64(dst, trailing_packed);
      dst[2] = _mm_extract_epi32(trailing_packed, 2);
      break;
    }
    case 4:
      _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), v_low);
      break;
    case 5:
      _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), v_low);
      dst[4] = _mm256_extract_epi32(v, 4);
      break;
    case 6:
      _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), v_low);
      _mm_storeu_si64(dst + 4, mm256_extracti128_si256<path>(v, 1));
      break;
    case 7: {
      _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), v_low);
      __m128i trailing_packed = mm256_extracti128_si256<path>(v, 1);
      _mm_storeu_si64(dst + 4, trailing_packed);
      dst[6] = _mm_extract_epi32(trailing_packed, 2);
      break;
    }
    case 8:
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst), v);
      break;
    default:
      RUY_DCHECK_LE(residual_rows, 8);
      break;
  }
}

template <Path path>
inline void mm256_storeu_epi32(std::int32_t* dst, const __m256i v) {
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst), v);
}

// Transpose a 8x8 matrix of floats.
template <Path path>
void mm256_transpose8x8_ps(__m256* v0, __m256* v1, __m256* v2, __m256* v3,
                           __m256* v4, __m256* v5, __m256* v6, __m256* v7) {
  __m256 t2x2_0 = _mm256_unpacklo_ps(*v0, *v1);
  __m256 t2x2_1 = _mm256_unpackhi_ps(*v0, *v1);
  __m256 t2x2_2 = _mm256_unpacklo_ps(*v2, *v3);
  __m256 t2x2_3 = _mm256_unpackhi_ps(*v2, *v3);
  __m256 t2x2_4 = _mm256_unpacklo_ps(*v4, *v5);
  __m256 t2x2_5 = _mm256_unpackhi_ps(*v4, *v5);
  __m256 t2x2_6 = _mm256_unpacklo_ps(*v6, *v7);
  __m256 t2x2_7 = _mm256_unpackhi_ps(*v6, *v7);
  __m256 t4x4_0 = _mm256_shuffle_ps(t2x2_0, t2x2_2, _MM_SHUFFLE(1, 0, 1, 0));
  __m256 t4x4_1 = _mm256_shuffle_ps(t2x2_0, t2x2_2, _MM_SHUFFLE(3, 2, 3, 2));
  __m256 t4x4_2 = _mm256_shuffle_ps(t2x2_1, t2x2_3, _MM_SHUFFLE(1, 0, 1, 0));
  __m256 t4x4_3 = _mm256_shuffle_ps(t2x2_1, t2x2_3, _MM_SHUFFLE(3, 2, 3, 2));
  __m256 t4x4_4 = _mm256_shuffle_ps(t2x2_4, t2x2_6, _MM_SHUFFLE(1, 0, 1, 0));
  __m256 t4x4_5 = _mm256_shuffle_ps(t2x2_4, t2x2_6, _MM_SHUFFLE(3, 2, 3, 2));
  __m256 t4x4_6 = _mm256_shuffle_ps(t2x2_5, t2x2_7, _MM_SHUFFLE(1, 0, 1, 0));
  __m256 t4x4_7 = _mm256_shuffle_ps(t2x2_5, t2x2_7, _MM_SHUFFLE(3, 2, 3, 2));
  *v0 = _mm256_permute2f128_ps(t4x4_0, t4x4_4, 0x20);
  *v1 = _mm256_permute2f128_ps(t4x4_1, t4x4_5, 0x20);
  *v2 = _mm256_permute2f128_ps(t4x4_2, t4x4_6, 0x20);
  *v3 = _mm256_permute2f128_ps(t4x4_3, t4x4_7, 0x20);
  *v4 = _mm256_permute2f128_ps(t4x4_0, t4x4_4, 0x31);
  *v5 = _mm256_permute2f128_ps(t4x4_1, t4x4_5, 0x31);
  *v6 = _mm256_permute2f128_ps(t4x4_2, t4x4_6, 0x31);
  *v7 = _mm256_permute2f128_ps(t4x4_3, t4x4_7, 0x31);
}

// Transpose a 8x8 matrix of int32's.
template <Path path>
void mm256_transpose8x8_epi32(__m256i* v0, __m256i* v1, __m256i* v2,
                              __m256i* v3, __m256i* v4, __m256i* v5,
                              __m256i* v6, __m256i* v7) {
  mm256_transpose8x8_ps<path>(
      reinterpret_cast<__m256*>(v0), reinterpret_cast<__m256*>(v1),
      reinterpret_cast<__m256*>(v2), reinterpret_cast<__m256*>(v3),
      reinterpret_cast<__m256*>(v4), reinterpret_cast<__m256*>(v5),
      reinterpret_cast<__m256*>(v6), reinterpret_cast<__m256*>(v7));
}

}  // namespace intrin_utils
}  // namespace

template <Path path>
inline void KernelFloatAvxCommon(const KernelParamsFloat<8, 8>& params) {
  // As parameters are defined, we need to scale by sizeof(float).
  const std::int64_t lhs_stride = params.lhs_stride >> 2;
  const std::int64_t dst_stride = params.dst_stride >> 2;
  const std::int64_t rhs_stride = params.rhs_stride >> 2;
  //
  int bias_ptr_block_increment = params.flags & RUY_ASM_FLAG_HAS_BIAS ? 1 : 0;
  // AVX2 float block size = 8.
  const int end_row = std::min(params.dst_rows, params.last_row + 8);
  const int end_col = std::min(params.dst_cols, params.last_col + 8);
  //
  const float* adj_rhs_col_ptr =
      params.rhs_base_ptr - params.start_col * rhs_stride;
  float* adj_dst_col_ptr =
      params.dst_base_ptr - params.start_col * dst_stride - params.start_row;
  const float* adj_lhs_col_ptr =
      params.lhs_base_ptr - params.start_row * lhs_stride;
  const float* bias_ptr = params.bias;

  const __m256 clamp_max_v = _mm256_set1_ps(params.clamp_max);
  const __m256 clamp_min_v = _mm256_set1_ps(params.clamp_min);
  const bool channel_dimension_is_col =
      params.flags & RUY_ASM_FLAG_CHANNEL_DIMENSION_IS_COL;

  int col = params.start_col;
  // Loop through cols by float block size, leaving incomplete remainder
  for (; col <= end_col - 8; col += 8) {
    __m256 accum_data_v[8];

    const float* rhs_col_ptr = adj_rhs_col_ptr + col * rhs_stride;
    float* dst_col_ptr = adj_dst_col_ptr + col * dst_stride;

    for (int row = params.start_row; row < end_row; row += 8) {
      const int residual_rows = std::min(end_row - row, 8);

      const float* lhs_col_ptr = adj_lhs_col_ptr + row * lhs_stride;
      float* dst_ptr = dst_col_ptr + row;

      // Initialize with bias.
      if (channel_dimension_is_col) {
        const float* bias_elem_ptr = bias_ptr + col * bias_ptr_block_increment;
        for (int j = 0; j < 8; ++j) {
          accum_data_v[j] = _mm256_broadcast_ss(bias_elem_ptr + j);
        }
      } else {
        const float* bias_elem_ptr = bias_ptr + row * bias_ptr_block_increment;
        const __m256 initial_accum_data = _mm256_loadu_ps(bias_elem_ptr);

        for (int j = 0; j < 8; ++j) {
          accum_data_v[j] = initial_accum_data;
        }
      }

      const float* lhs_ptr = lhs_col_ptr;
      const float* rhs_ptr = rhs_col_ptr;
      for (int d = 0; d < params.depth; ++d) {
        const __m256 lhs_data = _mm256_loadu_ps(lhs_ptr);
        // Load 8 RHS values, then use permute instructions to broadcast each
        // value to a register. _mm256_permute2f128_ps is slow on AMD.
        __m256 rhs0_3 =
            _mm256_broadcast_ps(reinterpret_cast<const __m128*>(rhs_ptr));
        __m256 rhs4_7 =
            _mm256_broadcast_ps(reinterpret_cast<const __m128*>(rhs_ptr + 4));

        const __m256 dup_rhs_element_0 = _mm256_permute_ps(rhs0_3, 0);
        accum_data_v[0] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_0, accum_data_v[0]);

        const __m256 dup_rhs_element_1 = _mm256_permute_ps(rhs0_3, 85);
        accum_data_v[1] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_1, accum_data_v[1]);

        const __m256 dup_rhs_element_2 = _mm256_permute_ps(rhs0_3, 170);
        accum_data_v[2] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_2, accum_data_v[2]);

        const __m256 dup_rhs_element_3 = _mm256_permute_ps(rhs0_3, 255);
        accum_data_v[3] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_3, accum_data_v[3]);

        const __m256 dup_rhs_element_4 = _mm256_permute_ps(rhs4_7, 0);
        accum_data_v[4] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_4, accum_data_v[4]);

        const __m256 dup_rhs_element_5 = _mm256_permute_ps(rhs4_7, 85);
        accum_data_v[5] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_5, accum_data_v[5]);

        const __m256 dup_rhs_element_6 = _mm256_permute_ps(rhs4_7, 170);
        accum_data_v[6] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_6, accum_data_v[6]);

        const __m256 dup_rhs_element_7 = _mm256_permute_ps(rhs4_7, 255);
        accum_data_v[7] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_7, accum_data_v[7]);

        lhs_ptr += 8;
        rhs_ptr += 8;
      }

      if (residual_rows == 8) {
        for (int j = 0; j < 8; ++j) {
          float* block_ptr = dst_ptr + j * dst_stride;
          accum_data_v[j] = _mm256_min_ps(accum_data_v[j], clamp_max_v);
          accum_data_v[j] = _mm256_max_ps(accum_data_v[j], clamp_min_v);
          _mm256_storeu_ps(block_ptr, accum_data_v[j]);
        }
      } else {
        for (int j = 0; j < 8; ++j) {
          float* block_ptr = dst_ptr + j * dst_stride;
          accum_data_v[j] = _mm256_min_ps(accum_data_v[j], clamp_max_v);
          accum_data_v[j] = _mm256_max_ps(accum_data_v[j], clamp_min_v);
          intrin_utils::mm256_n_storeu_ps<path>(block_ptr, residual_rows,
                                                accum_data_v[j]);
        }
      }
    }  // End row-block loop.
  }    // End col-block loop.

  if (col < end_col) {
    // Remaining cols in [0, float block size).
    RUY_DCHECK_GE(end_col - col, 0);
    RUY_DCHECK_LT(end_col - col, 8);

    __m256 accum_data_v[8];

    const float* rhs_col_ptr = adj_rhs_col_ptr + col * rhs_stride;
    float* dst_col_ptr = adj_dst_col_ptr + col * dst_stride;
    const int residual_cols = std::min(end_col - col, 8);

    for (int row = params.start_row; row < end_row; row += 8) {
      const int residual_rows = std::min(end_row - row, 8);

      const float* lhs_col_ptr = adj_lhs_col_ptr + row * lhs_stride;
      float* dst_ptr = dst_col_ptr + row;

      // Initialize with bias.
      if (channel_dimension_is_col) {
        const float* bias_elem_ptr = bias_ptr + col * bias_ptr_block_increment;
        for (int j = 0; j < 8; ++j) {
          accum_data_v[j] = _mm256_broadcast_ss(bias_elem_ptr + j);
        }
      } else {
        const float* bias_elem_ptr = bias_ptr + row * bias_ptr_block_increment;
        const __m256 initial_accum_data = _mm256_loadu_ps(bias_elem_ptr);

        for (int j = 0; j < 8; ++j) {
          accum_data_v[j] = initial_accum_data;
        }
      }

      const float* lhs_ptr = lhs_col_ptr;
      const float* rhs_ptr = rhs_col_ptr;
      for (int d = 0; d < params.depth; ++d) {
        const __m256 lhs_data = _mm256_loadu_ps(lhs_ptr);

        __m256 rhs0_3 =
            _mm256_broadcast_ps(reinterpret_cast<const __m128*>(rhs_ptr));
        __m256 rhs4_7 =
            _mm256_broadcast_ps(reinterpret_cast<const __m128*>(rhs_ptr + 4));

        const __m256 dup_rhs_element_0 = _mm256_permute_ps(rhs0_3, 0);
        accum_data_v[0] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_0, accum_data_v[0]);

        const __m256 dup_rhs_element_1 = _mm256_permute_ps(rhs0_3, 85);
        accum_data_v[1] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_1, accum_data_v[1]);

        const __m256 dup_rhs_element_2 = _mm256_permute_ps(rhs0_3, 170);
        accum_data_v[2] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_2, accum_data_v[2]);

        const __m256 dup_rhs_element_3 = _mm256_permute_ps(rhs0_3, 255);
        accum_data_v[3] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_3, accum_data_v[3]);

        const __m256 dup_rhs_element_4 = _mm256_permute_ps(rhs4_7, 0);
        accum_data_v[4] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_4, accum_data_v[4]);

        const __m256 dup_rhs_element_5 = _mm256_permute_ps(rhs4_7, 85);
        accum_data_v[5] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_5, accum_data_v[5]);

        const __m256 dup_rhs_element_6 = _mm256_permute_ps(rhs4_7, 170);
        accum_data_v[6] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_6, accum_data_v[6]);

        const __m256 dup_rhs_element_7 = _mm256_permute_ps(rhs4_7, 255);
        accum_data_v[7] = intrin_utils::MulAdd<path>(
            lhs_data, dup_rhs_element_7, accum_data_v[7]);

        lhs_ptr += 8;
        rhs_ptr += 8;
      }

      for (int j = 0; j < residual_cols; ++j) {
        float* block_ptr = dst_ptr + j * dst_stride;
        accum_data_v[j] = _mm256_min_ps(accum_data_v[j], clamp_max_v);
        accum_data_v[j] = _mm256_max_ps(accum_data_v[j], clamp_min_v);
        intrin_utils::mm256_n_storeu_ps<path>(block_ptr, residual_rows,
                                              accum_data_v[j]);
      }
    }  // End row-block loop.
  }    // End col-block terminal conditional.
}

template <Path path>
inline void KernelFloatAvxCommonSingleCol(
    const KernelParamsFloat<8, 8>& params) {
  RUY_DCHECK_EQ(params.dst_cols, 1);
  RUY_DCHECK_EQ(params.last_col, 0);
  RUY_DCHECK_EQ(params.start_col, 0);

  // As parameters are defined, we need to scale by sizeof(float).
  const std::int64_t lhs_stride = params.lhs_stride >> 2;
  //
  int bias_ptr_block_increment = params.flags & RUY_ASM_FLAG_HAS_BIAS ? 1 : 0;
  // AVX2 float block size = 8.
  const int end_row = std::min(params.dst_rows, params.last_row + 8);

  float* adj_dst_col_ptr = params.dst_base_ptr - params.start_row;
  const float* adj_lhs_col_ptr =
      params.lhs_base_ptr - params.start_row * lhs_stride;
  const float* bias_col_ptr = params.bias;

  const __m256 clamp_max_v = _mm256_set1_ps(params.clamp_max);
  const __m256 clamp_min_v = _mm256_set1_ps(params.clamp_min);

  __m256 accum_data_v;

  const float* rhs_col_ptr = params.rhs_base_ptr;
  float* dst_col_ptr = adj_dst_col_ptr;

  int row = params.start_row;
  for (; row <= end_row - 8; row += 8) {
    const float* lhs_col_ptr = adj_lhs_col_ptr + row * lhs_stride;
    float* dst_ptr = dst_col_ptr + row;
    const float* bias_ptr = bias_col_ptr + row * bias_ptr_block_increment;

    // Initialize with bias.
    accum_data_v = _mm256_loadu_ps(bias_ptr);

    const float* lhs_ptr = lhs_col_ptr;
    const float* rhs_ptr = rhs_col_ptr;
    int d = 0;
    for (; d <= params.depth - 4; d += 4) {
      const __m256 lhs_data_0 = _mm256_loadu_ps(lhs_ptr);
      const __m256 dup_rhs_element_0 = _mm256_set1_ps(rhs_ptr[0]);
      accum_data_v = intrin_utils::MulAdd<path>(lhs_data_0, dup_rhs_element_0,
                                                accum_data_v);
      const __m256 dup_rhs_element_1 = _mm256_set1_ps(rhs_ptr[8]);
      const __m256 lhs_data_1 = _mm256_loadu_ps(lhs_ptr + 8);
      accum_data_v = intrin_utils::MulAdd<path>(lhs_data_1, dup_rhs_element_1,
                                                accum_data_v);

      const __m256 lhs_data_2 = _mm256_loadu_ps(lhs_ptr + 16);
      const __m256 dup_rhs_element_2 = _mm256_set1_ps(rhs_ptr[16]);
      accum_data_v = intrin_utils::MulAdd<path>(lhs_data_2, dup_rhs_element_2,
                                                accum_data_v);
      const __m256 dup_rhs_element_3 = _mm256_set1_ps(rhs_ptr[24]);
      const __m256 lhs_data_3 = _mm256_loadu_ps(lhs_ptr + 24);
      accum_data_v = intrin_utils::MulAdd<path>(lhs_data_3, dup_rhs_element_3,
                                                accum_data_v);
      lhs_ptr += 32;  // Loaded 8 * 4 floats.
      rhs_ptr += 32;
    }
    for (; d < params.depth; ++d) {
      const __m256 lhs_data = _mm256_loadu_ps(lhs_ptr);
      const float* rhs_data = rhs_ptr;

      const __m256 dup_rhs_element_j = _mm256_set1_ps(rhs_data[0]);
      accum_data_v =
          intrin_utils::MulAdd<path>(lhs_data, dup_rhs_element_j, accum_data_v);
      lhs_ptr += 8;
      rhs_ptr += 8;
    }

    accum_data_v = _mm256_min_ps(accum_data_v, clamp_max_v);
    accum_data_v = _mm256_max_ps(accum_data_v, clamp_min_v);
    _mm256_storeu_ps(dst_ptr, accum_data_v);
  }  // End row-block loop.

  if (row < end_row) {
    const int residual_rows = end_row - row;
    RUY_CHECK_GE(residual_rows, 1);
    RUY_CHECK_LT(residual_rows, 8);

    const float* lhs_col_ptr = adj_lhs_col_ptr + row * lhs_stride;
    float* dst_ptr = dst_col_ptr + row;
    const float* bias_ptr = bias_col_ptr + row * bias_ptr_block_increment;

    // Initialize with bias.
    accum_data_v = _mm256_loadu_ps(bias_ptr);

    const float* lhs_ptr = lhs_col_ptr;
    const float* rhs_ptr = rhs_col_ptr;
    for (int d = 0; d < params.depth; ++d) {
      const __m256 lhs_data = _mm256_loadu_ps(lhs_ptr);
      const float* rhs_data = rhs_ptr;

      const __m256 dup_rhs_element_j = _mm256_set1_ps(rhs_data[0]);
      accum_data_v =
          intrin_utils::MulAdd<path>(lhs_data, dup_rhs_element_j, accum_data_v);
      lhs_ptr += 8;
      rhs_ptr += 8;
    }

    accum_data_v = _mm256_min_ps(accum_data_v, clamp_max_v);
    accum_data_v = _mm256_max_ps(accum_data_v, clamp_min_v);
    intrin_utils::mm256_n_storeu_ps<path>(dst_ptr, residual_rows, accum_data_v);
  }  // End handling of residual rows.
}
}  // namespace ruy
#endif  //  (RUY_PLATFORM_AVX || RUY_PLATFORM_AVX2_FMA) && RUY_OPT(ASM)

#endif  // RUY_RUY_KERNEL_X86_H_

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

#ifndef RUY_RUY_PACK_X86_H_
#define RUY_RUY_PACK_X86_H_

#include <cstdint>
#include <cstring>
#include <type_traits>

#include "ruy/check_macros.h"
#include "ruy/mat.h"
#include "ruy/opt_set.h"
#include "ruy/pack_common.h"
#include "ruy/path.h"
#include "ruy/platform.h"
#include "ruy/profiler/instrumentation.h"
#include "ruy/tune.h"

namespace ruy {

#if RUY_PLATFORM_X86

RUY_INHERIT_PACK(Path::kStandardCpp, Path::kAvx)
RUY_INHERIT_PACK(Path::kAvx, Path::kAvx2Fma)
RUY_INHERIT_PACK(Path::kAvx2Fma, Path::kAvx512)

RUY_USE_MEMCPY_ROWMAJOR_FLOAT_PACK(Path::kAvx2Fma, 8)
RUY_USE_MEMCPY_ROWMAJOR_FLOAT_PACK(Path::kAvx512, 16)

template <>
struct PackedTypeImpl<Path::kAvx, std::uint8_t> {
  using Type = std::int8_t;
};

template <>
struct PackedTypeImpl<Path::kAvx2Fma, std::uint8_t> {
  using Type = std::int8_t;
};
template <>
struct PackedTypeImpl<Path::kAvx512, std::uint8_t> {
  using Type = std::int8_t;
};

// Note that source and zero buffers can be uint8 type, but in the packing
// function are reinterpreted as int8, and are XOR-ed with input_xor.
void Pack8bitColMajorForAvx2(const std::int8_t* src_ptr, std::int8_t input_xor,
                             const std::int8_t* zerobuf, int src_stride,
                             int remaining_src_cols, int src_rows,
                             std::int8_t* packed_ptr, std::int32_t* sums_ptr);

template <typename Scalar>
struct PackImpl<Path::kAvx2Fma, FixedKernelLayout<Order::kColMajor, 4, 8>,
                Scalar, std::int8_t, std::int32_t, Order::kColMajor> {
  static_assert(std::is_same<Scalar, std::int8_t>::value ||
                    std::is_same<Scalar, std::uint8_t>::value,
                "");
  using Layout = FixedKernelLayout<Order::kColMajor, 4, 8>;
  static constexpr std::int8_t kInputXor =
      std::is_same<Scalar, std::int8_t>::value ? 0 : 0x80;

  static void Run(Tuning, const Mat<Scalar>& src_matrix,
                  PMat<std::int8_t>* packed_matrix, int start_col,
                  int end_col) {
    profiler::ScopeLabel label("Pack (AVX2 8-bit)");

    RUY_DCHECK(IsColMajor(src_matrix.layout));
    RUY_DCHECK(IsColMajor(packed_matrix->layout));
    RUY_DCHECK_EQ((end_col - start_col) % Layout::kCols, 0);
    RUY_DCHECK_EQ(start_col % Layout::kCols, 0);
    std::int32_t* sums = packed_matrix->sums;
    Scalar zerobuf[Layout::kCols * Layout::kRows];
    memset(zerobuf, packed_matrix->zero_point ^ kInputXor,
           Layout::kCols * Layout::kRows * sizeof(Scalar));
    for (int block_col = start_col; block_col < end_col;
         block_col += Layout::kCols) {
      std::int32_t* sums_ptr = sums ? sums + block_col : nullptr;
      int src_stride = src_matrix.layout.stride;
      const Scalar* src_ptr = src_matrix.data.get() + src_stride * block_col;
      int remaining_src_cols = src_matrix.layout.cols - block_col;

      static constexpr int block_col_mask = ~(Layout::kCols - 1);  // High bits.
      std::int8_t* packed_ptr =
          packed_matrix->data +
          packed_matrix->layout.stride * (block_col & block_col_mask);
      Pack8bitColMajorForAvx2(
          reinterpret_cast<const std::int8_t*>(src_ptr), kInputXor,
          reinterpret_cast<const std::int8_t*>(zerobuf), src_stride,
          remaining_src_cols, src_matrix.layout.rows, packed_ptr, sums_ptr);
    }
  }
};

void Pack8bitColMajorForAvx(const std::int8_t* src_ptr, std::int8_t input_xor,
                            const std::int8_t* zerobuf, int src_stride,
                            int remaining_src_cols, int src_rows,
                            std::int8_t* packed_ptr, std::int32_t* sums_ptr);

template <typename Scalar>
struct PackImpl<Path::kAvx, FixedKernelLayout<Order::kColMajor, 4, 8>, Scalar,
                std::int8_t, std::int32_t, Order::kColMajor> {
  static_assert(std::is_same<Scalar, std::int8_t>::value ||
                    std::is_same<Scalar, std::uint8_t>::value,
                "");
  using Layout = FixedKernelLayout<Order::kColMajor, 4, 8>;
  static constexpr std::int8_t kInputXor =
      std::is_same<Scalar, std::int8_t>::value ? 0 : 0x80;

  static void Run(Tuning, const Mat<Scalar>& src_matrix,
                  PMat<std::int8_t>* packed_matrix, int start_col,
                  int end_col) {
    profiler::ScopeLabel label("Pack (AVX 8-bit)");

    RUY_DCHECK(IsColMajor(src_matrix.layout));
    RUY_DCHECK(IsColMajor(packed_matrix->layout));
    RUY_DCHECK_EQ((end_col - start_col) % Layout::kCols, 0);
    RUY_DCHECK_EQ(start_col % Layout::kCols, 0);
    std::int32_t* sums = packed_matrix->sums;
    Scalar zerobuf[Layout::kCols * Layout::kRows];
    memset(zerobuf, packed_matrix->zero_point ^ kInputXor,
           Layout::kCols * Layout::kRows * sizeof(Scalar));
    for (int block_col = start_col; block_col < end_col;
         block_col += Layout::kCols) {
      std::int32_t* sums_ptr = sums ? sums + block_col : nullptr;
      int src_stride = src_matrix.layout.stride;
      const Scalar* src_ptr = src_matrix.data.get() + src_stride * block_col;
      int remaining_src_cols = src_matrix.layout.cols - block_col;

      static constexpr int block_col_mask = ~(Layout::kCols - 1);  // High bits.
      std::int8_t* packed_ptr =
          packed_matrix->data +
          packed_matrix->layout.stride * (block_col & block_col_mask);
      Pack8bitColMajorForAvx(
          reinterpret_cast<const std::int8_t*>(src_ptr), kInputXor,
          reinterpret_cast<const std::int8_t*>(zerobuf), src_stride,
          remaining_src_cols, src_matrix.layout.rows, packed_ptr, sums_ptr);
    }
  }
};

void PackFloatColMajorForAvx(const float* src_ptr, const float* zerobuf,
                             int src_stride, int remaining_src_cols,
                             int src_rows, float* packed_ptr);

template <>
struct PackImpl<Path::kAvx, FixedKernelLayout<Order::kRowMajor, 1, 8>, float,
                float, float, Order::kColMajor> {
  using Layout = FixedKernelLayout<Order::kRowMajor, 1, 8>;
  static void Run(Tuning, const Mat<float>& src_matrix,
                  PMat<float>* packed_matrix, int start_col, int end_col) {
    profiler::ScopeLabel label("Pack (AVX float)");

    RUY_DCHECK(IsColMajor(src_matrix.layout));
    RUY_DCHECK(IsColMajor(packed_matrix->layout));
    RUY_DCHECK_EQ((end_col - start_col) % Layout::kCols, 0);
    RUY_DCHECK_EQ(start_col % Layout::kCols, 0);
    const float zerobuf[Layout::kCols] = {
        0.0f};  // Remainder default inits to 0.0f.
    for (int block_col = start_col; block_col < end_col;
         block_col += Layout::kCols) {
      int src_stride = src_matrix.layout.stride;
      const float* src_ptr = src_matrix.data.get() + src_stride * block_col;
      int remaining_src_cols = src_matrix.layout.cols - block_col;

      static constexpr int block_col_mask = ~(Layout::kCols - 1);  // High bits.
      float* packed_ptr =
          packed_matrix->data +
          packed_matrix->layout.stride * (block_col & block_col_mask);
      PackFloatColMajorForAvx(src_ptr, zerobuf, src_stride, remaining_src_cols,
                              src_matrix.layout.rows, packed_ptr);
    }
  }
};

void PackFloatColMajorForAvx2(const float* src_ptr, const float* zerobuf,
                              int src_stride, int remaining_src_cols,
                              int src_rows, float* packed_ptr);

template <>
struct PackImpl<Path::kAvx2Fma, FixedKernelLayout<Order::kRowMajor, 1, 8>,
                float, float, float, Order::kColMajor> {
  using Layout = FixedKernelLayout<Order::kRowMajor, 1, 8>;
  static void Run(Tuning, const Mat<float>& src_matrix,
                  PMat<float>* packed_matrix, int start_col, int end_col) {
    profiler::ScopeLabel label("Pack (AVX2 float)");

    RUY_DCHECK(IsColMajor(src_matrix.layout));
    RUY_DCHECK(IsColMajor(packed_matrix->layout));
    RUY_DCHECK_EQ((end_col - start_col) % Layout::kCols, 0);
    RUY_DCHECK_EQ(start_col % Layout::kCols, 0);
    const float zerobuf[Layout::kCols] = {
        0.0f};  // Remainder default inits to 0.0f.
    for (int block_col = start_col; block_col < end_col;
         block_col += Layout::kCols) {
      int src_stride = src_matrix.layout.stride;
      const float* src_ptr = src_matrix.data.get() + src_stride * block_col;
      int remaining_src_cols = src_matrix.layout.cols - block_col;

      static constexpr int block_col_mask = ~(Layout::kCols - 1);  // High bits.
      float* packed_ptr =
          packed_matrix->data +
          packed_matrix->layout.stride * (block_col & block_col_mask);
      PackFloatColMajorForAvx2(src_ptr, zerobuf, src_stride, remaining_src_cols,
                               src_matrix.layout.rows, packed_ptr);
    }
  }
};

// Note that source and zero buffers can be uint8 type, but in the packing
// function are reinterpreted as int8, and are XOR-ed with input_xor.
void Pack8bitColMajorForAvx512(const std::int8_t* src_ptr,
                               std::int8_t input_xor,
                               const std::int8_t* zerobuf, int src_stride,
                               int remaining_src_cols, int src_rows,
                               std::int8_t* packed_ptr, std::int32_t* sums_ptr);

template <typename Scalar>
struct PackImpl<Path::kAvx512, FixedKernelLayout<Order::kColMajor, 4, 16>,
                Scalar, std::int8_t, std::int32_t, Order::kColMajor> {
  static_assert(std::is_same<Scalar, std::int8_t>::value ||
                    std::is_same<Scalar, std::uint8_t>::value,
                "");
  using Layout = FixedKernelLayout<Order::kColMajor, 4, 16>;
  static constexpr int kHalfLayoutCols =
      8;  // Half the number of cols in a block.
  static constexpr std::int8_t kInputXor =
      std::is_same<Scalar, std::int8_t>::value ? 0 : 0x80;

  static void Run(Tuning, const Mat<Scalar>& src_matrix,
                  PMat<std::int8_t>* packed_matrix, int start_col,
                  int end_col) {
    profiler::ScopeLabel label("Pack (AVX-512 8-bit)");

    RUY_DCHECK(IsColMajor(src_matrix.layout));
    RUY_DCHECK(IsColMajor(packed_matrix->layout));
    RUY_DCHECK_EQ((end_col - start_col) % Layout::kCols, 0);
    RUY_DCHECK_EQ(start_col % Layout::kCols, 0);
    RUY_DCHECK_EQ(kHalfLayoutCols * 2, Layout::kCols);
    std::int32_t* sums = packed_matrix->sums;
    Scalar zerobuf[kHalfLayoutCols * Layout::kRows];
    memset(zerobuf, packed_matrix->zero_point ^ kInputXor,
           kHalfLayoutCols * Layout::kRows * sizeof(Scalar));
    for (int block_col = start_col; block_col < end_col;
         block_col += Layout::kCols) {
      std::int32_t* sums_ptr = sums ? sums + block_col : nullptr;
      int src_stride = src_matrix.layout.stride;
      const Scalar* src_ptr = src_matrix.data.get() + src_stride * block_col;
      int remaining_src_cols = src_matrix.layout.cols - block_col;

      static constexpr int block_col_mask = ~(Layout::kCols - 1);  // High bits.
      std::int8_t* packed_ptr =
          packed_matrix->data +
          packed_matrix->layout.stride * (block_col & block_col_mask);
      Pack8bitColMajorForAvx512(
          reinterpret_cast<const std::int8_t*>(src_ptr), kInputXor,
          reinterpret_cast<const std::int8_t*>(zerobuf), src_stride,
          remaining_src_cols, src_matrix.layout.rows, packed_ptr, sums_ptr);
    }
  }
};

void PackFloatColMajorForAvx512(const float* src_ptr, const float* zerobuf,
                                int src_stride, int remaining_src_cols,
                                int src_rows, float* packed_ptr);

template <>
struct PackImpl<Path::kAvx512, FixedKernelLayout<Order::kRowMajor, 1, 16>,
                float, float, float, Order::kColMajor> {
  static void Run(Tuning, const Mat<float>& src_matrix,
                  PMat<float>* packed_matrix, int start_col, int end_col) {
    profiler::ScopeLabel label("Pack (AVX-512 float)");
    using Layout = FixedKernelLayout<Order::kRowMajor, 1, 16>;
    RUY_DCHECK(IsColMajor(src_matrix.layout));
    RUY_DCHECK(IsColMajor(packed_matrix->layout));
    RUY_DCHECK_EQ((end_col - start_col) % Layout::kCols, 0);
    RUY_DCHECK_EQ(start_col % Layout::kCols, 0);
    const float zerobuf[Layout::kCols] = {
        0.0f};  // Remainder default inits to 0.0f.
    for (int block_col = start_col; block_col < end_col;
         block_col += Layout::kCols) {
      int src_stride = src_matrix.layout.stride;
      const float* src_ptr = src_matrix.data.get() + src_stride * block_col;
      int remaining_src_cols = src_matrix.layout.cols - block_col;

      static constexpr int block_col_mask = ~(Layout::kCols - 1);  // High bits.
      float* packed_ptr =
          packed_matrix->data +
          packed_matrix->layout.stride * (block_col & block_col_mask);
      PackFloatColMajorForAvx512(src_ptr, zerobuf, src_stride,
                                 remaining_src_cols, src_matrix.layout.rows,
                                 packed_ptr);
    }
  }
};

void Pack8bitRowMajorForAvx2(const std::uint8_t* src_ptr, int src_stride,
                             int src_zero_point, std::int8_t* packed_ptr,
                             int packed_stride, int start_col, int end_col,
                             int src_cols, int block_row, int src_rows,
                             int input_xor, std::int32_t* sums);

template <typename Scalar>
struct PackImpl<Path::kAvx2Fma, FixedKernelLayout<Order::kColMajor, 4, 8>,
                Scalar, std::int8_t, std::int32_t, Order::kRowMajor> {
  static void Run(Tuning, const Mat<Scalar>& src_matrix,
                  PMat<std::int8_t>* packed_matrix, int start_col,
                  int end_col) {
    profiler::ScopeLabel label("Pack (kAvx2Fma 8bit row-major)");
    RUY_DCHECK_EQ(src_matrix.layout.order, Order::kRowMajor);
    static constexpr int kInputXor =
        std::is_same<Scalar, std::int8_t>::value ? 0 : 0x80;
    std::int32_t* sums = packed_matrix->sums;
    std::memset(sums + start_col, 0, sizeof(sums[0]) * (end_col - start_col));
    int block_row = 0;
    for (; block_row < packed_matrix->layout.rows; block_row += 4) {
      int src_stride = src_matrix.layout.stride;
      int packed_stride = packed_matrix->layout.stride;
      const Scalar* src_ptr =
          src_matrix.data.get() + block_row * src_stride + start_col;
      std::int8_t* packed_ptr =
          packed_matrix->data + start_col * packed_stride + block_row * 8;
      Pack8bitRowMajorForAvx2(reinterpret_cast<const std::uint8_t*>(src_ptr),
                              src_stride, src_matrix.zero_point, packed_ptr,
                              packed_stride, start_col, end_col,
                              src_matrix.layout.cols, block_row,
                              src_matrix.layout.rows, kInputXor, sums);
    }
  }
};

void Pack8bitRowMajorForAvx(const std::uint8_t* src_ptr, int src_stride,
                            int src_zero_point, std::int8_t* packed_ptr,
                            int packed_stride, int start_col, int end_col,
                            int src_cols, int block_row, int src_rows,
                            int input_xor, std::int32_t* sums);

template <typename Scalar>
struct PackImpl<Path::kAvx, FixedKernelLayout<Order::kColMajor, 4, 8>, Scalar,
                std::int8_t, std::int32_t, Order::kRowMajor> {
  static void Run(Tuning, const Mat<Scalar>& src_matrix,
                  PMat<std::int8_t>* packed_matrix, int start_col,
                  int end_col) {
    profiler::ScopeLabel label("Pack (AVX 8bit row-major)");
    RUY_DCHECK_EQ(src_matrix.layout.order, Order::kRowMajor);
    static constexpr int kInputXor =
        std::is_same<Scalar, std::int8_t>::value ? 0 : 0x80;
    std::int32_t* sums = packed_matrix->sums;
    std::memset(sums + start_col, 0, sizeof(sums[0]) * (end_col - start_col));
    int block_row = 0;
    for (; block_row < packed_matrix->layout.rows; block_row += 4) {
      int src_stride = src_matrix.layout.stride;
      int packed_stride = packed_matrix->layout.stride;
      const Scalar* src_ptr =
          src_matrix.data.get() + block_row * src_stride + start_col;
      std::int8_t* packed_ptr =
          packed_matrix->data + start_col * packed_stride + block_row * 8;
      Pack8bitRowMajorForAvx(reinterpret_cast<const std::uint8_t*>(src_ptr),
                             src_stride, src_matrix.zero_point, packed_ptr,
                             packed_stride, start_col, end_col,
                             src_matrix.layout.cols, block_row,
                             src_matrix.layout.rows, kInputXor, sums);
    }
  }
};

void Pack8bitRowMajorForAvx512(const std::uint8_t* src_ptr, int src_stride,
                               int src_zero_point, std::int8_t* packed_ptr,
                               int packed_stride, int start_col, int end_col,
                               int src_cols, int block_row, int src_rows,
                               int input_xor, std::int32_t* sums);

template <typename Scalar>
struct PackImpl<Path::kAvx512, FixedKernelLayout<Order::kColMajor, 4, 16>,
                Scalar, std::int8_t, std::int32_t, Order::kRowMajor> {
  static void Run(Tuning, const Mat<Scalar>& src_matrix,
                  PMat<std::int8_t>* packed_matrix, int start_col,
                  int end_col) {
    profiler::ScopeLabel label("Pack (kAvx512 8bit row-major)");
    RUY_DCHECK_EQ(src_matrix.layout.order, Order::kRowMajor);
    static constexpr int kInputXor =
        std::is_same<Scalar, std::int8_t>::value ? 0 : 0x80;
    std::int32_t* sums = packed_matrix->sums;
    std::memset(sums + start_col, 0, sizeof(sums[0]) * (end_col - start_col));
    int block_row = 0;
    for (; block_row < packed_matrix->layout.rows; block_row += 4) {
      int src_stride = src_matrix.layout.stride;
      int packed_stride = packed_matrix->layout.stride;
      const Scalar* src_ptr =
          src_matrix.data.get() + block_row * src_stride + start_col;
      std::int8_t* packed_ptr =
          packed_matrix->data + start_col * packed_stride + block_row * 16;
      Pack8bitRowMajorForAvx512(reinterpret_cast<const std::uint8_t*>(src_ptr),
                                src_stride, src_matrix.zero_point, packed_ptr,
                                packed_stride, start_col, end_col,
                                src_matrix.layout.cols, block_row,
                                src_matrix.layout.rows, kInputXor, sums);
    }
  }
};
#endif  // RUY_PLATFORM_X86

}  // namespace ruy

#if ((RUY_PLATFORM_AVX || RUY_PLATFORM_AVX2_FMA) && RUY_OPT(ASM))

#include <immintrin.h>  // IWYU pragma: keep

namespace ruy {
namespace {

template <Path path>
inline __m256 Mm256UnpackloPsx2(const __m256 a, const __m256 b) {
  return _mm256_castpd_ps(
      _mm256_unpacklo_pd(_mm256_castps_pd(a), _mm256_castps_pd(b)));
}

template <Path path>
inline __m256 Mm256UnpackhiPsx2(const __m256 a, const __m256 b) {
  return _mm256_castpd_ps(
      _mm256_unpackhi_pd(_mm256_castps_pd(a), _mm256_castps_pd(b)));
}

template <Path path>
inline __m256i CompareGreaterThan(const __m256i&, const __m256i&) {
  RUY_DCHECK(false);
  return _mm256_set1_epi32(0);
}

// Shared between AVX and AVX2+FMA.
template <Path path>
inline __m256i MaskLoadu(int available_src_rows, std::int8_t zero_point,
                         const std::int8_t* addr) {
  RUY_DCHECK_LT(available_src_rows, 32);
  __m256i padded_data;

  if (available_src_rows >= 16) {
    __m128i load_hi = _mm_set1_epi8(zero_point);
    __m128i load_lo = _mm_loadu_si128(reinterpret_cast<const __m128i*>(addr));
    memcpy(&load_hi, addr + 16, available_src_rows - 16);
    padded_data = _mm256_set_m128i(load_hi, load_lo);
  } else {
    __m128i load_hi = _mm_set1_epi8(zero_point);
    __m128i load_lo = load_hi;
    memcpy(&load_lo, addr, available_src_rows);
    padded_data = _mm256_set_m128i(load_hi, load_lo);
  }
  return padded_data;
}

}  // namespace.

template <typename PackImpl, Path path>
inline void PackFloatColMajorForAvxCommonPacker(const float* src_ptr,
                                                const float* zerobuf,
                                                int src_stride,
                                                int remaining_src_cols,
                                                int src_rows, float* packed_ptr,
                                                float* trailing_buf) {
  RUY_DCHECK_EQ(PackImpl::Layout::kCols, 8);
  RUY_DCHECK_EQ(PackImpl::Layout::kRows, 1);

  // This packing amounts to transposition of 8x8 blocks.
  static constexpr int kPackCols = 8;  // Source cols packed together.
  static constexpr int kPackRows = 8;  // Short input is padded.

  const float* src_ptr0 = src_ptr;
  const float* src_ptr1 = src_ptr0 + src_stride;
  const float* src_ptr2 = src_ptr1 + src_stride;
  const float* src_ptr3 = src_ptr2 + src_stride;
  const float* src_ptr4 = src_ptr3 + src_stride;
  const float* src_ptr5 = src_ptr4 + src_stride;
  const float* src_ptr6 = src_ptr5 + src_stride;
  const float* src_ptr7 = src_ptr6 + src_stride;
  std::int64_t src_inc0 = 8;
  std::int64_t src_inc1 = 8;
  std::int64_t src_inc2 = 8;
  std::int64_t src_inc3 = 8;
  std::int64_t src_inc4 = 8;
  std::int64_t src_inc5 = 8;
  std::int64_t src_inc6 = 8;
  std::int64_t src_inc7 = 8;
  // Handle cases where source does not have kPackDim (8) columns.
  if (remaining_src_cols < kPackCols) {
    if (remaining_src_cols <= 0) {
      src_ptr0 = zerobuf;
      src_inc0 = 0;
    }
    if (remaining_src_cols <= 1) {
      src_ptr1 = zerobuf;
      src_inc1 = 0;
    }
    if (remaining_src_cols <= 2) {
      src_ptr2 = zerobuf;
      src_inc2 = 0;
    }
    if (remaining_src_cols <= 3) {
      src_ptr3 = zerobuf;
      src_inc3 = 0;
    }
    if (remaining_src_cols <= 4) {
      src_ptr4 = zerobuf;
      src_inc4 = 0;
    }
    if (remaining_src_cols <= 5) {
      src_ptr5 = zerobuf;
      src_inc5 = 0;
    }
    if (remaining_src_cols <= 6) {
      src_ptr6 = zerobuf;
      src_inc6 = 0;
    }
    src_ptr7 = zerobuf;
    src_inc7 = 0;
  }

  for (int k = 0; k < src_rows; k += kPackRows) {
    const int available_src_rows = src_rows - k;
    // Effectively,
    // available_src_rows = std::max(0, std::min(kPackDim, src_rows - k));
    // but treat each case separately.
    if (available_src_rows >= kPackRows) {
      __m256 t0, t1, t2, t3, t4, t5, t6, t7;
      __m256 r0, r1, r2, r3, r4, r5, r6, r7;

      t0 = _mm256_loadu_ps(src_ptr0);
      t4 = _mm256_loadu_ps(src_ptr4);
      t1 = _mm256_loadu_ps(src_ptr1);
      t5 = _mm256_loadu_ps(src_ptr5);
      t2 = _mm256_loadu_ps(src_ptr2);
      t6 = _mm256_loadu_ps(src_ptr6);
      t3 = _mm256_loadu_ps(src_ptr3);
      t7 = _mm256_loadu_ps(src_ptr7);

      r0 = _mm256_unpacklo_ps(t0, t1);
      r4 = _mm256_unpacklo_ps(t4, t5);
      r2 = _mm256_unpackhi_ps(t0, t1);
      r6 = _mm256_unpackhi_ps(t4, t5);
      r1 = _mm256_unpacklo_ps(t2, t3);
      r5 = _mm256_unpacklo_ps(t6, t7);
      r3 = _mm256_unpackhi_ps(t2, t3);
      r7 = _mm256_unpackhi_ps(t6, t7);

      t0 = Mm256UnpackloPsx2<path>(r0, r1);
      t4 = Mm256UnpackloPsx2<path>(r4, r5);
      t2 = Mm256UnpackhiPsx2<path>(r0, r1);
      t6 = Mm256UnpackhiPsx2<path>(r4, r5);
      t1 = Mm256UnpackloPsx2<path>(r2, r3);
      t5 = Mm256UnpackloPsx2<path>(r6, r7);
      t3 = Mm256UnpackhiPsx2<path>(r2, r3);
      t7 = Mm256UnpackhiPsx2<path>(r6, r7);

      // The preceding sets of rearrangement operations interleaved by 4 bytes
      // and then by 8 bytes *within* lanes. The following set interleave by 16
      // bytes (128-bit), operating *between* AVX lanes. For instance (t0, t4)
      // are interleaved to create (r0, r1). This complexity follows from the
      // way that AVX is centered around MM 128-bit lanes.
      r0 = _mm256_permute2f128_ps(t0, t4, 0x20);
      r4 = _mm256_permute2f128_ps(t1, t5, 0x20);
      r1 = _mm256_permute2f128_ps(t0, t4, 0x31);
      r5 = _mm256_permute2f128_ps(t1, t5, 0x31);
      r2 = _mm256_permute2f128_ps(t2, t6, 0x20);
      r6 = _mm256_permute2f128_ps(t3, t7, 0x20);
      r3 = _mm256_permute2f128_ps(t2, t6, 0x31);
      r7 = _mm256_permute2f128_ps(t3, t7, 0x31);

      _mm256_storeu_ps(packed_ptr + 0 * 8, r0);
      _mm256_storeu_ps(packed_ptr + 2 * 8, r4);
      _mm256_storeu_ps(packed_ptr + 4 * 8, r1);
      _mm256_storeu_ps(packed_ptr + 6 * 8, r5);
      _mm256_storeu_ps(packed_ptr + 1 * 8, r2);
      _mm256_storeu_ps(packed_ptr + 3 * 8, r6);
      _mm256_storeu_ps(packed_ptr + 5 * 8, r3);
      _mm256_storeu_ps(packed_ptr + 7 * 8, r7);
    } else if (available_src_rows > 0) {
      const __m256i series = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
      const __m256i row_mask_v = CompareGreaterThan<path>(
          _mm256_set1_epi32(available_src_rows), series);

      __m256 t0, t1, t2, t3, t4, t5, t6, t7;
      __m256 r0, r1, r2, r3, r4, r5, r6, r7;

      t0 = _mm256_maskload_ps(src_ptr0, row_mask_v);
      t4 = _mm256_maskload_ps(src_ptr4, row_mask_v);
      t1 = _mm256_maskload_ps(src_ptr1, row_mask_v);
      t5 = _mm256_maskload_ps(src_ptr5, row_mask_v);
      t2 = _mm256_maskload_ps(src_ptr2, row_mask_v);
      t6 = _mm256_maskload_ps(src_ptr6, row_mask_v);
      t3 = _mm256_maskload_ps(src_ptr3, row_mask_v);
      t7 = _mm256_maskload_ps(src_ptr7, row_mask_v);

      r0 = _mm256_unpacklo_ps(t0, t1);
      r4 = _mm256_unpacklo_ps(t4, t5);
      r2 = _mm256_unpackhi_ps(t0, t1);
      r6 = _mm256_unpackhi_ps(t4, t5);
      r1 = _mm256_unpacklo_ps(t2, t3);
      r5 = _mm256_unpacklo_ps(t6, t7);
      r3 = _mm256_unpackhi_ps(t2, t3);
      r7 = _mm256_unpackhi_ps(t6, t7);

      t0 = Mm256UnpackloPsx2<path>(r0, r1);
      t4 = Mm256UnpackloPsx2<path>(r4, r5);
      t2 = Mm256UnpackhiPsx2<path>(r0, r1);
      t6 = Mm256UnpackhiPsx2<path>(r4, r5);
      t1 = Mm256UnpackloPsx2<path>(r2, r3);
      t5 = Mm256UnpackloPsx2<path>(r6, r7);
      t3 = Mm256UnpackhiPsx2<path>(r2, r3);
      t7 = Mm256UnpackhiPsx2<path>(r6, r7);

      // The preceding sets of rearrangement operations interleaved by 4 bytes
      // and then by 8 bytes *within* lanes. The following set interleave by 16
      // bytes (128-bit), operating *between* AVX lanes. For instance (t0, t4)
      // are interleaved to create (r0, r1). This complexity follows from the
      // way that AVX is centered around MM 128-bit lanes.
      r0 = _mm256_permute2f128_ps(t0, t4, 0x20);
      r4 = _mm256_permute2f128_ps(t1, t5, 0x20);
      r1 = _mm256_permute2f128_ps(t0, t4, 0x31);
      r5 = _mm256_permute2f128_ps(t1, t5, 0x31);
      r2 = _mm256_permute2f128_ps(t2, t6, 0x20);
      r6 = _mm256_permute2f128_ps(t3, t7, 0x20);
      r3 = _mm256_permute2f128_ps(t2, t6, 0x31);
      // r7 no longer needed.

      _mm256_storeu_ps(trailing_buf + 0 * 8, r0);
      _mm256_storeu_ps(trailing_buf + 2 * 8, r4);
      _mm256_storeu_ps(trailing_buf + 4 * 8, r1);
      _mm256_storeu_ps(trailing_buf + 6 * 8, r5);
      _mm256_storeu_ps(trailing_buf + 1 * 8, r2);
      _mm256_storeu_ps(trailing_buf + 3 * 8, r6);
      _mm256_storeu_ps(trailing_buf + 5 * 8, r3);
      // No store to (trailing_buf + 7 * 8), space not allocated.
    }

    packed_ptr += kPackRows * kPackCols;
    src_ptr0 += src_inc0;
    src_ptr1 += src_inc1;
    src_ptr2 += src_inc2;
    src_ptr3 += src_inc3;
    src_ptr4 += src_inc4;
    src_ptr5 += src_inc5;
    src_ptr6 += src_inc6;
    src_ptr7 += src_inc7;
  }
}
}  // namespace ruy
#endif  //  (RUY_PLATFORM_AVX || RUY_PLATFORM_AVX2_FMA) && RUY_OPT(ASM)

#endif  // RUY_RUY_PACK_X86_H_

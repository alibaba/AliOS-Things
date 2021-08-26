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

#ifndef RUY_RUY_PACK_ARM_H_
#define RUY_RUY_PACK_ARM_H_

#include <algorithm>
#include <cstdint>
#include <type_traits>

#include "ruy/asm_helpers.h"
#include "ruy/check_macros.h"
#include "ruy/mat.h"
#include "ruy/opt_set.h"
#include "ruy/pack_common.h"
#include "ruy/path.h"
#include "ruy/platform.h"
#include "ruy/profiler/instrumentation.h"
#include "ruy/tune.h"

namespace ruy {

#if RUY_PLATFORM_NEON
RUY_INHERIT_PACK(Path::kStandardCpp, Path::kNeon)
RUY_INHERIT_PACK(Path::kNeon, Path::kNeonDotprod)

RUY_USE_MEMCPY_ROWMAJOR_FLOAT_PACK(Path::kNeon, 8)
#if RUY_PLATFORM_NEON_32
RUY_USE_MEMCPY_ROWMAJOR_FLOAT_PACK(Path::kNeon, 4)
#endif

template <>
struct PackedTypeImpl<Path::kNeon, std::uint8_t> {
  using Type = std::int8_t;
};
template <>
struct PackedTypeImpl<Path::kNeonDotprod, std::uint8_t> {
  using Type = std::int8_t;
};
#endif

#if RUY_PLATFORM_NEON
void Pack8bitRowMajorForNeon(const std::uint8_t* src_ptr, int src_stride,
                             int src_rows, int src_cols, int block_row,
                             int start_col, int end_col,
                             std::int8_t* packed_ptr, int packed_stride,
                             int packed_zero_point, std::int32_t* sums_ptr,
                             int input_xor, int kernel_cols);
#endif

#if RUY_PLATFORM_NEON_64 && RUY_OPT(ASM)

void Pack8bitColMajorForNeon(const void* src_ptr0, const void* src_ptr1,
                             const void* src_ptr2, const void* src_ptr3,
                             int src_inc0, int src_inc1, int src_inc2,
                             int src_inc3, int src_rows, int src_zero_point,
                             std::int8_t* packed_ptr, std::int32_t* sums_ptr,
                             int input_xor);
void Pack8bitColMajorForNeonA55ish(const void* src_ptr0, const void* src_ptr1,
                                   const void* src_ptr2, const void* src_ptr3,
                                   int src_inc0, int src_inc1, int src_inc2,
                                   int src_inc3, int src_rows,
                                   int src_zero_point, std::int8_t* packed_ptr,
                                   std::int32_t* sums_ptr, int input_xor);
void Pack8bitColMajorForNeonDotprod(const void* src_ptr0, const void* src_ptr1,
                                    const void* src_ptr2, const void* src_ptr3,
                                    int src_inc0, int src_inc1, int src_inc2,
                                    int src_inc3, int src_rows,
                                    int src_zero_point, std::int8_t* packed_ptr,
                                    std::int32_t* sums_ptr, int input_xor);
void Pack8bitColMajorForNeonDotprodA55ish(
    const void* src_ptr0, const void* src_ptr1, const void* src_ptr2,
    const void* src_ptr3, int src_inc0, int src_inc1, int src_inc2,
    int src_inc3, int src_rows, int src_zero_point, std::int8_t* packed_ptr,
    std::int32_t* sums_ptr, int input_xor);
void Pack8bitRowMajorForNeonDotprod(const void* src_ptr0, const void* src_ptr1,
                                    const void* src_ptr2, const void* src_ptr3,
                                    int src_inc0, int src_inc1, int src_inc2,
                                    int src_inc3, int src_cols,
                                    int src_zero_point, std::int8_t* packed_ptr,
                                    int packed_stride, std::int32_t* sums_ptr,
                                    int input_xor);
#elif RUY_PLATFORM_NEON_32 && RUY_OPT(ASM)

struct PackParams8bit {
  const void* src_ptr0;
  const void* src_ptr1;
  const void* src_ptr2;
  const void* src_ptr3;
  const std::int32_t* sums_ptr;
  const std::int8_t* packed_ptr;
  int src_inc0;
  int src_inc1;
  int src_inc2;
  int src_inc3;
  int src_rows;
  int src_zero_point;
  int input_xor;
};

inline void MakePackParams8bit(const void* src_ptr0, const void* src_ptr1,
                               const void* src_ptr2, const void* src_ptr3,
                               const std::int32_t* sums_ptr,
                               const std::int8_t* packed_ptr, int src_inc0,
                               int src_inc1, int src_inc2, int src_inc3,
                               int src_rows, int src_zero_point, int input_xor,
                               PackParams8bit* params) {
  params->src_ptr0 = src_ptr0;
  params->src_ptr1 = src_ptr1;
  params->src_ptr2 = src_ptr2;
  params->src_ptr3 = src_ptr3;
  params->sums_ptr = sums_ptr;
  params->packed_ptr = packed_ptr;
  params->src_inc0 = src_inc0;
  params->src_inc1 = src_inc1;
  params->src_inc2 = src_inc2;
  params->src_inc3 = src_inc3;
  params->src_rows = src_rows;
  params->src_zero_point = src_zero_point;
  params->input_xor = input_xor;
}

void Pack8bitColMajorForNeon4Cols(const PackParams8bit& params);
void Pack8bitColMajorForNeon2Cols(const PackParams8bit& params);

#endif  // (RUY_PLATFORM_NEON_32 && RUY_OPT(ASM)

#if (RUY_PLATFORM_NEON_32 || RUY_PLATFORM_NEON_64) && RUY_OPT(ASM)

template <typename Scalar>
struct PackImpl<Path::kNeon, FixedKernelLayout<Order::kColMajor, 16, 4>, Scalar,
                std::int8_t, std::int32_t, Order::kColMajor> {
  static_assert(std::is_same<Scalar, std::int8_t>::value ||
                    std::is_same<Scalar, std::uint8_t>::value,
                "");
  static constexpr int kInputXor =
      std::is_same<Scalar, std::int8_t>::value ? 0 : 0x80;

  static void Run(Tuning tuning, const Mat<Scalar>& src_matrix,
                  PMat<std::int8_t>* packed_matrix, int start_col,
                  int end_col) {
    RUY_DCHECK(IsColMajor(src_matrix.layout));
    RUY_DCHECK(IsColMajor(packed_matrix->layout));
    RUY_DCHECK_EQ(start_col % 4, 0);
    std::int32_t* sums = packed_matrix->sums;
    Scalar zerobuf[16];
    memset(zerobuf, src_matrix.zero_point, sizeof(zerobuf));
    for (int block_col = start_col; block_col < end_col; block_col += 4) {
      int src_stride = src_matrix.layout.stride;
      const Scalar* src_ptr0 = src_matrix.data.get() + src_stride * block_col;
      const Scalar* src_ptr1 = src_ptr0 + src_stride;
      const Scalar* src_ptr2 = src_ptr1 + src_stride;
      const Scalar* src_ptr3 = src_ptr2 + src_stride;
      int src_inc0 = 16;
      int src_inc1 = 16;
      int src_inc2 = 16;
      int src_inc3 = 16;
      if (block_col >= src_matrix.layout.cols - 3) {
        if (block_col >= src_matrix.layout.cols - 0) {
          src_ptr0 = zerobuf;
          src_inc0 = 0;
        }
        if (block_col >= src_matrix.layout.cols - 1) {
          src_ptr1 = zerobuf;
          src_inc1 = 0;
        }
        if (block_col >= src_matrix.layout.cols - 2) {
          src_ptr2 = zerobuf;
          src_inc2 = 0;
        }
        if (block_col >= src_matrix.layout.cols - 3) {
          src_ptr3 = zerobuf;
          src_inc3 = 0;
        }
      }
      std::int8_t* packed_ptr =
          packed_matrix->data + packed_matrix->layout.stride * block_col;
      std::int32_t* sums_ptr = sums ? sums + block_col : nullptr;
#if RUY_PLATFORM_NEON_64
      if (__builtin_expect(tuning == Tuning::kA55ish, true)) {
        Pack8bitColMajorForNeonA55ish(
            src_ptr0, src_ptr1, src_ptr2, src_ptr3, src_inc0, src_inc1,
            src_inc2, src_inc3, src_matrix.layout.rows, src_matrix.zero_point,
            packed_ptr, sums_ptr, kInputXor);
      } else {
        Pack8bitColMajorForNeon(src_ptr0, src_ptr1, src_ptr2, src_ptr3,
                                src_inc0, src_inc1, src_inc2, src_inc3,
                                src_matrix.layout.rows, src_matrix.zero_point,
                                packed_ptr, sums_ptr, kInputXor);
      }
#else
      (void)tuning;
      // We have a more limited set of general purpose registers in ARMv7, so
      // we use the "params" struct technique from the kernel code to save
      // registers.
      PackParams8bit params;
      MakePackParams8bit(src_ptr0, src_ptr1, src_ptr2, src_ptr3, sums_ptr,
                         packed_ptr, src_inc0, src_inc1, src_inc2, src_inc3,
                         src_matrix.layout.rows, src_matrix.zero_point,
                         kInputXor, &params);
      Pack8bitColMajorForNeon4Cols(params);
#endif  // RUY_PLATFORM_NEON_64
    }
  }
};

#endif  // (RUY_PLATFORM_NEON_32 || RUY_PLATFORM_NEON_64) &&
        // RUY_OPT(ASM)

#if RUY_PLATFORM_NEON_32 && RUY_OPT(ASM)
// The 32-bit float kernel is 4 rows X 2 columns, so we need an additional
// partial specialization for the RHS, which has a FixedKernelLayout with 2
// columns.
template <typename Scalar>
struct PackImpl<Path::kNeon, FixedKernelLayout<Order::kColMajor, 16, 2>, Scalar,
                std::int8_t, std::int32_t, Order::kColMajor> {
  static_assert(std::is_same<Scalar, std::int8_t>::value ||
                    std::is_same<Scalar, std::uint8_t>::value,
                "");
  static constexpr int kInputXor =
      std::is_same<Scalar, std::int8_t>::value ? 0 : 0x80;
  static void Run(Tuning, const Mat<Scalar>& src_matrix,
                  PMat<std::int8_t>* packed_matrix, int start_col,
                  int end_col) {
    RUY_DCHECK(IsColMajor(src_matrix.layout));
    RUY_DCHECK(IsColMajor(packed_matrix->layout));
    RUY_DCHECK_EQ(start_col % 2, 0);
    std::int32_t* sums = packed_matrix->sums;
    Scalar zerobuf[16];
    memset(zerobuf, src_matrix.zero_point, sizeof(zerobuf));
    for (int block_col = start_col; block_col < end_col; block_col += 2) {
      int src_stride = src_matrix.layout.stride;
      const Scalar* src_ptr0 = src_matrix.data.get() + src_stride * block_col;
      const Scalar* src_ptr1 = src_ptr0 + src_stride;
      int src_inc0 = 16;
      int src_inc1 = 16;
      if (block_col >= src_matrix.layout.cols - 2) {
        if (block_col >= src_matrix.layout.cols - 0) {
          src_ptr0 = zerobuf;
          src_inc0 = 0;
        }
        if (block_col >= src_matrix.layout.cols - 1) {
          src_ptr1 = zerobuf;
          src_inc1 = 0;
        }
      }
      std::int8_t* packed_ptr =
          packed_matrix->data + packed_matrix->layout.stride * block_col;
      std::int32_t* sums_ptr = sums ? sums + block_col : nullptr;
      PackParams8bit params;
      MakePackParams8bit(src_ptr0, src_ptr1, nullptr, nullptr, sums_ptr,
                         packed_ptr, src_inc0, src_inc1, -1, -1,
                         src_matrix.layout.rows, src_matrix.zero_point,
                         kInputXor, &params);
      Pack8bitColMajorForNeon2Cols(params);
    }
  }
};
#endif  // (RUY_PLATFORM_NEON_32) && RUY_OPT(ASM)

#if RUY_PLATFORM_NEON_64 && RUY_OPT(ASM)
template <typename Scalar>
struct PackImpl<Path::kNeonDotprod, FixedKernelLayout<Order::kColMajor, 4, 8>,
                Scalar, std::int8_t, std::int32_t, Order::kColMajor> {
  static_assert(std::is_same<Scalar, std::int8_t>::value ||
                    std::is_same<Scalar, std::uint8_t>::value,
                "");
  static constexpr int kInputXor =
      std::is_same<Scalar, std::int8_t>::value ? 0 : 0x80;

  static void Run(Tuning tuning, const Mat<Scalar>& src_matrix,
                  PMat<std::int8_t>* packed_matrix, int start_col,
                  int end_col) {
    RUY_DCHECK(IsColMajor(src_matrix.layout));
    RUY_DCHECK(IsColMajor(packed_matrix->layout));
    RUY_DCHECK_EQ(start_col % 8, 0);
    std::int32_t* sums = packed_matrix->sums;
    Scalar zerobuf[16];
    memset(zerobuf, src_matrix.zero_point, sizeof(zerobuf));
    for (int block_col = start_col; block_col < end_col; block_col += 4) {
      int src_stride = src_matrix.layout.stride;
      const Scalar* src_ptr0 = src_matrix.data.get() + src_stride * block_col;
      const Scalar* src_ptr1 = src_ptr0 + src_stride;
      const Scalar* src_ptr2 = src_ptr1 + src_stride;
      const Scalar* src_ptr3 = src_ptr2 + src_stride;
      std::int64_t src_inc0 = 16;
      std::int64_t src_inc1 = 16;
      std::int64_t src_inc2 = 16;
      std::int64_t src_inc3 = 16;
      if (block_col >= src_matrix.layout.cols - 3) {
        if (block_col >= src_matrix.layout.cols - 0) {
          src_ptr0 = zerobuf;
          src_inc0 = 0;
        }
        if (block_col >= src_matrix.layout.cols - 1) {
          src_ptr1 = zerobuf;
          src_inc1 = 0;
        }
        if (block_col >= src_matrix.layout.cols - 2) {
          src_ptr2 = zerobuf;
          src_inc2 = 0;
        }
        if (block_col >= src_matrix.layout.cols - 3) {
          src_ptr3 = zerobuf;
          src_inc3 = 0;
        }
      }
      std::int8_t* packed_ptr =
          packed_matrix->data +
          packed_matrix->layout.stride * (block_col & ~7) +
          ((block_col & 4) * 4);
      std::int32_t* sums_ptr = sums ? sums + block_col : nullptr;
      if (__builtin_expect(tuning == Tuning::kA55ish, true)) {
        Pack8bitColMajorForNeonDotprodA55ish(
            src_ptr0, src_ptr1, src_ptr2, src_ptr3, src_inc0, src_inc1,
            src_inc2, src_inc3, src_matrix.layout.rows, src_matrix.zero_point,
            packed_ptr, sums_ptr, kInputXor);
      } else {
        Pack8bitColMajorForNeonDotprod(
            src_ptr0, src_ptr1, src_ptr2, src_ptr3, src_inc0, src_inc1,
            src_inc2, src_inc3, src_matrix.layout.rows, src_matrix.zero_point,
            packed_ptr, sums_ptr, kInputXor);
      }
    }
  }
};
#endif  // (RUY_PLATFORM_NEON_64&& RUY_OPT(ASM)

#if RUY_PLATFORM_NEON_64 && RUY_OPT(ASM)
void PackFloatColMajorForNeon(const float* src_ptr0, const float* src_ptr1,
                              const float* src_ptr2, const float* src_ptr3,
                              int src_inc0, int src_inc1, int src_inc2,
                              int src_inc3, int src_rows, float* packed_ptr);
void PackFloatColMajorForNeonA55ish(const float* src_ptr0,
                                    const float* src_ptr1,
                                    const float* src_ptr2,
                                    const float* src_ptr3, int src_inc0,
                                    int src_inc1, int src_inc2, int src_inc3,
                                    int src_rows, float* packed_ptr);

#elif RUY_PLATFORM_NEON_32 && RUY_OPT(ASM)
void PackFloatColMajorForNeon(const float* src_ptr0, const float* src_ptr1,
                              const float* src_ptr2, const float* src_ptr3,
                              int src_inc, int src_rows, float* packed_ptr,
                              int stride);
#endif  // (RUY_PLATFORM_NEON_64&& RUY_OPT(ASM)

#if (RUY_PLATFORM_NEON_32 || RUY_PLATFORM_NEON_64) && RUY_OPT(ASM)

template <>
struct PackImpl<Path::kNeon, FixedKernelLayout<Order::kRowMajor, 1, 8>, float,
                float, float, Order::kColMajor> {
  static void Run(Tuning tuning, const Mat<float>& src_matrix,
                  PMat<float>* packed_matrix, int start_col, int end_col) {
    RUY_DCHECK(IsColMajor(src_matrix.layout));
    RUY_DCHECK(IsColMajor(packed_matrix->layout));
    RUY_DCHECK_EQ(start_col % 8, 0);
    const float zerobuf[4] = {0};
    for (int block_col = start_col; block_col < end_col; block_col += 4) {
      int src_stride = src_matrix.layout.stride;
      const float* src_ptr0 = src_matrix.data.get() + src_stride * block_col;
      const float* src_ptr1 = src_ptr0 + src_stride;
      const float* src_ptr2 = src_ptr1 + src_stride;
      const float* src_ptr3 = src_ptr2 + src_stride;
      std::int64_t src_inc0 = 16;
      std::int64_t src_inc1 = 16;
      std::int64_t src_inc2 = 16;
      std::int64_t src_inc3 = 16;
      if (block_col >= src_matrix.layout.cols - 3) {
        if (block_col >= src_matrix.layout.cols - 0) {
          src_ptr0 = zerobuf;
          src_inc0 = 0;
        }
        if (block_col >= src_matrix.layout.cols - 1) {
          src_ptr1 = zerobuf;
          src_inc1 = 0;
        }
        if (block_col >= src_matrix.layout.cols - 2) {
          src_ptr2 = zerobuf;
          src_inc2 = 0;
        }
        if (block_col >= src_matrix.layout.cols - 3) {
          src_ptr3 = zerobuf;
          src_inc3 = 0;
        }
      }
      float* packed_ptr = packed_matrix->data +
                          packed_matrix->layout.stride * (block_col & ~7) +
                          ((block_col & 4));
#if RUY_PLATFORM_NEON_64
      if (__builtin_expect(tuning == Tuning::kA55ish, true)) {
        PackFloatColMajorForNeonA55ish(src_ptr0, src_ptr1, src_ptr2, src_ptr3,
                                       src_inc0, src_inc1, src_inc2, src_inc3,
                                       src_matrix.layout.rows, packed_ptr);
      } else {
        PackFloatColMajorForNeon(src_ptr0, src_ptr1, src_ptr2, src_ptr3,
                                 src_inc0, src_inc1, src_inc2, src_inc3,
                                 src_matrix.layout.rows, packed_ptr);
      }
#else
      (void)tuning;
      // Encode each of src_inc0, ..., src_inc3 in lowest 4 bits of src_inc
      // to save on registers (we have fewer general purpose registers in
      // 32-bit ARM than in 64-bit ARM). For the 64-bit case, we pass four
      // values that are each either 16 or 0 and use them directly. For the
      // 32-bit case, bits 0, 1, 2, and 3 are used to determine if we should
      // use the value 16 (bit is set) or 0 (bit is not set) for the
      // respective increment value.
      std::int64_t src_inc = 0;
      src_inc += src_inc0 == 16 ? 1 : 0;
      src_inc += src_inc1 == 16 ? 2 : 0;
      src_inc += src_inc2 == 16 ? 4 : 0;
      src_inc += src_inc3 == 16 ? 8 : 0;
      const int kOutputStride = 32;
      PackFloatColMajorForNeon(src_ptr0, src_ptr1, src_ptr2, src_ptr3, src_inc,
                               src_matrix.layout.rows, packed_ptr,
                               kOutputStride);
#endif  // RUY_PLATFORM_NEON_64
    }
  }
};

#if RUY_PLATFORM_NEON_32
// The 32-bit float kernel is 8 rows X 4 columns, so we need an additional
// specialization for a FixedKernelLayout with 4 columns.
template <>
struct PackImpl<Path::kNeon, FixedKernelLayout<Order::kRowMajor, 1, 4>, float,
                float, float, Order::kColMajor> {
  static void Run(Tuning, const Mat<float>& src_matrix,
                  PMat<float>* packed_matrix, int start_col, int end_col) {
    RUY_DCHECK(IsColMajor(src_matrix.layout));
    RUY_DCHECK(IsColMajor(packed_matrix->layout));
    RUY_DCHECK_EQ(start_col % 4, 0);
    const float zerobuf[4] = {0};
    for (int block_col = start_col; block_col < end_col; block_col += 4) {
      int src_stride = src_matrix.layout.stride;
      const float* src_ptr0 = src_matrix.data.get() + src_stride * block_col;
      const float* src_ptr1 = src_ptr0 + src_stride;
      const float* src_ptr2 = src_ptr1 + src_stride;
      const float* src_ptr3 = src_ptr2 + src_stride;
      std::int64_t src_inc0 = 16;
      std::int64_t src_inc1 = 16;
      std::int64_t src_inc2 = 16;
      std::int64_t src_inc3 = 16;
      if (block_col >= src_matrix.layout.cols - 3) {
        if (block_col >= src_matrix.layout.cols - 0) {
          src_ptr0 = zerobuf;
          src_inc0 = 0;
        }
        if (block_col >= src_matrix.layout.cols - 1) {
          src_ptr1 = zerobuf;
          src_inc1 = 0;
        }
        if (block_col >= src_matrix.layout.cols - 2) {
          src_ptr2 = zerobuf;
          src_inc2 = 0;
        }
        if (block_col >= src_matrix.layout.cols - 3) {
          src_ptr3 = zerobuf;
          src_inc3 = 0;
        }
      }
      float* packed_ptr =
          packed_matrix->data + packed_matrix->layout.stride * (block_col);
      // Encode each of src_inc0, ..., src_inc1 in lowest 4 bits of scrc_inc
      // to save registers.
      std::int64_t src_inc = 0;
      src_inc += src_inc0 == 16 ? 1 : 0;
      src_inc += src_inc1 == 16 ? 2 : 0;
      src_inc += src_inc2 == 16 ? 4 : 0;
      src_inc += src_inc3 == 16 ? 8 : 0;
      const int kOutputStride = 16;
      PackFloatColMajorForNeon(src_ptr0, src_ptr1, src_ptr2, src_ptr3, src_inc,
                               src_matrix.layout.rows, packed_ptr,
                               kOutputStride);
    }
  }
};
#endif  // (RUY_PLATFORM_NEON_32)
#endif  // (RUY_PLATFORM_NEON_64 || RUY_PLATFORM_NEON_32) && \
        // RUY_OPT(ASM)

#if RUY_PLATFORM_NEON_64 && RUY_OPT(ASM)

template <typename Scalar>
struct PackImpl<Path::kNeonDotprod, FixedKernelLayout<Order::kColMajor, 4, 8>,
                Scalar, std::int8_t, std::int32_t, Order::kRowMajor> {
  static_assert(std::is_same<Scalar, std::int8_t>::value ||
                    std::is_same<Scalar, std::uint8_t>::value,
                "");
  static constexpr int kInputXor =
      std::is_same<Scalar, std::int8_t>::value ? 0 : 0x80;

  static void Run(Tuning, const Mat<Scalar>& src_matrix,
                  PMat<std::int8_t>* packed_matrix, int start_col,
                  int end_col) {
    RUY_DCHECK(IsRowMajor(src_matrix.layout));
    RUY_DCHECK(IsColMajor(packed_matrix->layout));
    RUY_DCHECK_EQ(start_col % 8, 0);
    std::int32_t* sums = packed_matrix->sums;
    std::memset(sums + start_col, 0, sizeof(sums[0]) * (end_col - start_col));
    Scalar zerobuf[8];
    memset(zerobuf, src_matrix.zero_point, sizeof(zerobuf));
    int src_stride = src_matrix.layout.stride;
    // As the source matrix is row-major and the destination packed matrix is
    // column-major, there is no traversal order that will be optimal for both
    // so we choose to favor the source matrix with a row-major traversal order.
    // Loop over groups of 4 rows.
    for (int block_row = 0; block_row < packed_matrix->layout.rows;
         block_row += 4) {
      // src_ptr[0-3] shall point to the positions in the 4 rows of the source
      // matrix that we are loading from, and will be incremented by
      // src_inc[0-3] after each 4x8 block is loaded.
      // First we compute these src_ptr and src_inc values for the case where
      // there are 4 rows left to be loaded from in the source matrix ...
      const Scalar* src_ptr0 =
          src_matrix.data.get() + src_stride * block_row + start_col;
      const Scalar* src_ptr1 = src_ptr0 + src_stride;
      const Scalar* src_ptr2 = src_ptr1 + src_stride;
      const Scalar* src_ptr3 = src_ptr2 + src_stride;
      std::int64_t src_inc0 = 8;
      std::int64_t src_inc1 = 8;
      std::int64_t src_inc2 = 8;
      std::int64_t src_inc3 = 8;
      // ... and now we adjust these values in case there are fewer than 4 rows
      // left to load from in the source matrix. In that case, we set the
      // corresponding src_ptr pointer to load from `zerobuf` and set src_inc
      // to 0 to avoid overrunning that small buffer.
      if (block_row >= src_matrix.layout.rows - 3) {
        if (block_row >= src_matrix.layout.rows - 0) {
          src_ptr0 = zerobuf;
          src_inc0 = 0;
        }
        if (block_row >= src_matrix.layout.rows - 1) {
          src_ptr1 = zerobuf;
          src_inc1 = 0;
        }
        if (block_row >= src_matrix.layout.rows - 2) {
          src_ptr2 = zerobuf;
          src_inc2 = 0;
        }
        if (block_row >= src_matrix.layout.rows - 3) {
          src_ptr3 = zerobuf;
          src_inc3 = 0;
        }
      }
      // Let src_cols be the number of source matrix columns to handle.
      int src_cols = std::min(end_col, src_matrix.layout.cols) - start_col;
      std::int8_t* packed_ptr = packed_matrix->data +
                                packed_matrix->layout.stride * start_col +
                                8 * block_row;
      std::int32_t* sums_ptr = sums + start_col;
      Pack8bitRowMajorForNeonDotprod(
          src_ptr0, src_ptr1, src_ptr2, src_ptr3, src_inc0, src_inc1, src_inc2,
          src_inc3, src_cols, src_matrix.zero_point, packed_ptr,
          packed_matrix->layout.stride, sums_ptr, kInputXor);
    }
  }
};

#endif  // RUY_PLATFORM_NEON_64 && RUY_OPT(ASM)

#if RUY_PLATFORM_NEON

template <typename Scalar, int KernelCols>
struct PackImpl<Path::kNeon,
                FixedKernelLayout<Order::kColMajor, 16, KernelCols>, Scalar,
                std::int8_t, std::int32_t, Order::kRowMajor> {
  static void Run(Tuning, const Mat<Scalar>& src_matrix,
                  PMat<std::int8_t>* packed_matrix, int start_col,
                  int end_col) {
    profiler::ScopeLabel label("Pack (KNeon, from row-major source)");
    static constexpr int kInputXor =
        std::is_same<Scalar, std::int8_t>::value ? 0 : 0x80;
    RUY_DCHECK_EQ(src_matrix.layout.order, Order::kRowMajor);
    RUY_DCHECK_EQ((end_col - start_col) % KernelCols, 0);
    std::int32_t* sums = packed_matrix->sums;
    std::memset(sums + start_col, 0, sizeof(sums[0]) * (end_col - start_col));
    int block_row = 0;
    for (; block_row < packed_matrix->layout.rows; block_row += 16) {
      int src_stride = src_matrix.layout.stride;
      int packed_stride = packed_matrix->layout.stride;
      const Scalar* src_ptr =
          src_matrix.data.get() + block_row * src_stride + start_col;
      std::int8_t* packed_ptr = packed_matrix->data +
                                start_col * packed_stride +
                                block_row * KernelCols;

      Pack8bitRowMajorForNeon(
          reinterpret_cast<const std::uint8_t*>(src_ptr), src_stride,
          src_matrix.layout.rows, src_matrix.layout.cols, block_row, start_col,
          end_col, packed_ptr, packed_stride, packed_matrix->zero_point, sums,
          kInputXor, KernelCols);
    }
  }
};
#endif

}  // namespace ruy

#endif  // RUY_RUY_PACK_ARM_H_

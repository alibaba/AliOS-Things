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

#ifndef RUY_RUY_PACK_COMMON_H_
#define RUY_RUY_PACK_COMMON_H_

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

#include "ruy/check_macros.h"
#include "ruy/mat.h"
#include "ruy/matrix.h"
#include "ruy/opt_set.h"
#include "ruy/path.h"
#include "ruy/platform.h"
#include "ruy/profiler/instrumentation.h"
#include "ruy/tune.h"

namespace ruy {

template <typename Scalar>
Scalar SymmetricZeroPoint() {
  if (std::is_floating_point<Scalar>::value) {
    return 0;
  }
  if (std::is_signed<Scalar>::value) {
    return 0;
  }
  return std::numeric_limits<Scalar>::max() / 2 + 1;
}

template <Path ThePath, typename Scalar>
struct PackedTypeImpl {
  using Type = Scalar;
};

template <Path ThePath, typename Scalar>
using PackedType = typename PackedTypeImpl<ThePath, Scalar>::Type;

template <typename PackedScalar, typename Scalar>
PackedScalar Pack(Scalar x) {
  return x - SymmetricZeroPoint<Scalar>() + SymmetricZeroPoint<PackedScalar>();
}

template <Path ThePath, typename FixedKernelLayout, typename Scalar,
          typename PackedScalar, typename SumsType, Order SrcOrder>
struct PackImpl;

#define RUY_INHERIT_PACK(PARENT, CHILD)                                     \
  template <typename FixedKernelLayout, typename Scalar,                    \
            typename PackedScalar, typename SumsType, Order SrcOrder>       \
  struct PackImpl<CHILD, FixedKernelLayout, Scalar, PackedScalar, SumsType, \
                  SrcOrder> : PackImpl<PARENT, FixedKernelLayout, Scalar,   \
                                       PackedScalar, SumsType, SrcOrder> {};

// A generic yet fairly fast implementation of
//
//    PackImpl<ThePath, FixedKernelLayout<Order::kRowMajor, 1, KernelCols>,
//             float, float, float, Order::kRowMajor>
//
// that is, a packing code path for the case of floating-point, row-major
// source matrix, targeting typical float kernel layouts consisting of a
// single row.
//
// The only reason why this isn't a partial specialization of PackImpl is that
// this leads to ambiguous partial specializations as this conflicts with
// the ones defined by RUY_INHERIT_PACK.
//
// What's special about floating-point kernels is that they tend to use
// FixedKernelLayout<Order::kRowMajor, 1, KernelCols> for some value of
// KernelCols, making it easy to implement the packing code as essentially
// a bunch of memcpy's with compile-time-fixed size
// (KernelCols * sizeof(float)), typically 16, 32 or 64 bytes. Unlike the
// quantized case, there are no sums to compute, and the float kernels tend
// to use this kind of simple layout on multiple architectures, unlike the
// heavily architecture-specific layouts of quantized kernels.
//
// Here are the current instantiations of this template (as of 2020):
// Path          | KernelCols
// --------------+---------------------------------
// kNeon (ARM32) | 8 and 4 (for LHS and RHS sides)
// kNeon (ARM64) | 8
// kAvxFma       | 8
// kAvx512       | 16
template <Path ThePath, int KernelCols>
struct MemcpyRowMajorFloatPackImpl {
  static void Run(Tuning, const Mat<float>& src_matrix,
                  PMat<float>* packed_matrix, int start_col, int end_col) {
    RUY_DCHECK(IsRowMajor(src_matrix.layout));
    RUY_DCHECK(IsColMajor(packed_matrix->layout));
    RUY_DCHECK_EQ(start_col % KernelCols, 0);
    int src_stride = src_matrix.layout.stride;
    // As the source matrix is row-major and the destination packed matrix is
    // column-major, there is no traversal order that will be optimal for both
    // so we choose to favor the source matrix with a row-major traversal order.
    for (int block_row = 0; block_row < src_matrix.layout.rows;
         block_row += 1) {
      const float* src_ptr =
          src_matrix.data.get() + src_stride * block_row + start_col;
      float* packed_ptr = packed_matrix->data +
                          packed_matrix->layout.stride * start_col +
                          KernelCols * block_row;
      int src_cols = std::min(end_col, src_matrix.layout.cols) - start_col;
      int col = 0;
      for (; col <= src_cols - KernelCols; col += KernelCols) {
        memcpy(packed_ptr, src_ptr, KernelCols * sizeof(float));
        packed_ptr += KernelCols * packed_matrix->layout.stride;
        src_ptr += KernelCols;
      }
      int remaining_cols = src_cols - col;
      if (remaining_cols > 0) {
        memcpy(packed_ptr, src_ptr, remaining_cols * sizeof(float));
        memset(packed_ptr + remaining_cols, 0,
               (KernelCols - remaining_cols) * sizeof(float));
      }
    }
  }
};

#define RUY_USE_MEMCPY_ROWMAJOR_FLOAT_PACK(ThePath, KernelCols)                \
  template <>                                                                  \
  struct PackImpl<ThePath, FixedKernelLayout<Order::kRowMajor, 1, KernelCols>, \
                  float, float, float, Order::kRowMajor>                       \
      : MemcpyRowMajorFloatPackImpl<ThePath, KernelCols> {};

}  // namespace ruy

#endif  // RUY_RUY_PACK_COMMON_H_

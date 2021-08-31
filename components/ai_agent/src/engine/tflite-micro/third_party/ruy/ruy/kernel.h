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

#ifndef RUY_RUY_KERNEL_H_
#define RUY_RUY_KERNEL_H_

#include "ruy/kernel_common.h"
#include "ruy/mul_params.h"
#include "ruy/platform.h"
#include "ruy/trace.h"

// IWYU pragma: begin_exports
#if RUY_PLATFORM_NEON
#include "ruy/kernel_arm.h"
#elif RUY_PLATFORM_X86
#include "ruy/kernel_x86.h"
#endif
// IWYU pragma: end_exports

namespace ruy {

// KernelArgs is a helper to access the template parameter values from a Kernel
// template instantiation.
template <typename KernelType>
struct KernelArgs {};

template <Path tPath, typename tLhsScalar, typename tRhsScalar,
          typename tAccumScalar, typename tDstScalar>
struct KernelArgs<
    Kernel<tPath, tLhsScalar, tRhsScalar, tAccumScalar, tDstScalar>> {
  static constexpr Path kPath = tPath;
  using LhsScalar = tLhsScalar;
  using RhsScalar = tRhsScalar;
  using AccumScalar = tAccumScalar;
  using DstScalar = tDstScalar;
};

// RunKernel::Run() is the only place that directly invokes Kernel::Run().
// It performs the types un-erasure, and factoring all Kernel::Run() calls
// through this function also gives a single place where to conditionally
// implement RUY_OPT(FAT_KERNEL). This should be a function but is a class to
// hide and share some boilerplate (see the member types, and the RunTyped
// method also using them).
template <typename KernelType>
class RunKernel final {
 public:
  static void Run(Tuning tuning, const SidePair<PEMat>& src,
                  const void* mul_params, const SidePair<int>& start,
                  const SidePair<int>& end, EMat* dst) {
    RUY_TRACE_SCOPE_NAME("RunKernel");
    const auto& unerased_lhs = UneraseType<LhsScalar>(src[Side::kLhs]);
    const auto& unerased_rhs = UneraseType<RhsScalar>(src[Side::kRhs]);
    auto unerased_dst = UneraseType<DstScalar>(*dst);
    RUY_TRACE_INFO(RUN_KERNEL);
    RunTyped(tuning, unerased_lhs, unerased_rhs,
             *static_cast<const MulParamsType*>(mul_params), start, end,
             &unerased_dst);
  }

 private:
  using Args = KernelArgs<KernelType>;
  using LhsScalar = typename Args::LhsScalar;
  using RhsScalar = typename Args::RhsScalar;
  using AccumScalar = typename Args::AccumScalar;
  using DstScalar = typename Args::DstScalar;
  using MulParamsType = MulParams<AccumScalar, DstScalar>;
  static void RunTyped(Tuning tuning, const PMat<LhsScalar>& lhs,
                       const PMat<RhsScalar>& rhs,
                       const MulParamsType& mul_params,
                       const SidePair<int>& start, const SidePair<int>& end,
                       Mat<DstScalar>* dst) {
    const int start_row = start[Side::kLhs];
    const int start_col = start[Side::kRhs];
    const int end_row = end[Side::kLhs];
    const int end_col = end[Side::kRhs];
    KernelType kernel(tuning);
    using LhsLayout = typename KernelType::LhsLayout;
    using RhsLayout = typename KernelType::RhsLayout;
    // This is a good place to validate kernel layouts. The Kernel class
    // template itself isn't a good place to do that because it has
    // specializations.
    // The kRows of both sides have to match: in TrMul, kRows is the depth
    // dimension, on which LHS and RHS have to agree for the matrix
    // multiplication to be defined at all, so requiring the corresponding
    // dimension of the kernel layouts to also match is reasonable. If it didn't
    // match, then the packed matrices could have mismatching depth dimensions
    // even with the source matrices agreeing.
    static_assert(LhsLayout::kRows == RhsLayout::kRows, "");
    // The kernel layouts have to be power-of-two. This simplifies BlockMap
    // logic considerably. This also avoids leaking fine performance
    // optimization details up the stack. For instance, if one of the dimensions
    // were 6, then users might notice that optimal performance is achieved with
    // matrix dimensions that are multiples of 6, and might start contorting
    // their own application code to match that requirement, in a way that would
    // not be future-proof.
    static_assert(is_pot(LhsLayout::kRows), "");
    static_assert(is_pot(LhsLayout::kCols), "");
    static_assert(is_pot(RhsLayout::kRows), "");
    static_assert(is_pot(RhsLayout::kCols), "");
    // end_row and end_col may be larger than dst dimensions.
    // that is because kernels write directly to the destination matrix, whose
    // dimensions may not be a multiple of the kernel dimensions, and we try to
    // keep this annoyance localized as an implementation detail in kernels,
    // by allowing to pass rounded-up values down as far as possible.
    // These assertions encode the contract.
    RUY_DCHECK_LE(0, start_row);
    RUY_DCHECK_LE(start_row, end_row);
    RUY_DCHECK_LT(end_row, dst->layout.rows + LhsLayout::kCols);
    RUY_DCHECK_EQ((end_row - start_row) % LhsLayout::kCols, 0);
    RUY_DCHECK_LE(0, start_col);
    RUY_DCHECK_LE(start_col, end_col);
    RUY_DCHECK_LT(end_col, dst->layout.cols + RhsLayout::kCols);
    RUY_DCHECK_EQ((end_col - start_col) % RhsLayout::kCols, 0);
#if RUY_OPT(FAT_KERNEL)
  kernel.Run(lhs, rhs, mul_params, start_row, start_col, end_row, end_col, dst);
#else
  for (int col = start_col; col < end_col; col += RhsLayout::kCols) {
    int block_end_col = std::min(col + RhsLayout::kCols, end_col);
    for (int row = start_row; row < end_row; row += LhsLayout::kCols) {
      int block_end_row = std::min(row + LhsLayout::kCols, end_row);
      kernel.Run(lhs, rhs, mul_params, row, col, block_end_row, block_end_col,
                 dst);
    }
  }
#endif
  }
};

template <Path ThePath>
struct StandardCppKernelLayout {};

template <>
struct StandardCppKernelLayout<Path::kStandardCpp> {
  using Lhs = FixedKernelLayout<Order::kColMajor, 1, 1>;
  using Rhs = FixedKernelLayout<Order::kColMajor, 1, 1>;
};

// A variant exercising RowMajor square blocks
template <>
struct StandardCppKernelLayout<Path::kInternalStandardCppVariant1> {
  using Lhs = FixedKernelLayout<Order::kRowMajor, 4, 4>;
  using Rhs = FixedKernelLayout<Order::kRowMajor, 4, 4>;
};

// A variant with a rectangular layout: 4x8
template <>
struct StandardCppKernelLayout<Path::kInternalStandardCppVariant2> {
  using Lhs = FixedKernelLayout<Order::kColMajor, 1, 4>;
  using Rhs = FixedKernelLayout<Order::kColMajor, 1, 8>;
};

// A variant with different block orders in LHS vs RHS.
template <>
struct StandardCppKernelLayout<Path::kInternalStandardCppVariant3> {
  using Lhs = FixedKernelLayout<Order::kColMajor, 2, 16>;
  using Rhs = FixedKernelLayout<Order::kRowMajor, 2, 8>;
};

// General implementation of the Kernel template, overridden by template
// specializations for specific SIMD code paths. This general implementation
// covers Path::kStandardCpp and its internal test-only variants.
template <Path ThePath, typename LhsScalar, typename RhsScalar,
          typename AccumScalar, typename DstScalar>
struct Kernel {
  // Each Kernel specialization defines kPath as the ground-truth path that it
  // implements. This is used in assertions. As we support fallbacks between
  // paths (see RUY_INHERIT_KERNEL), Unless a specialization for a specific set
  // of template parameters was defined, it is normal for template
  // instantiations of the form Kernel<SomePath, ...> to have kPath!=SomePath.
  // Assertions that kPath==SomePath are used in places where we know that we
  // should be using a template specialization for a specific path rather than a
  // fallback.
  static constexpr Path kPath = ThePath;
  using MulParamsType = MulParams<AccumScalar, DstScalar>;
  using LhsLayout = typename StandardCppKernelLayout<ThePath>::Lhs;
  using RhsLayout = typename StandardCppKernelLayout<ThePath>::Rhs;
  explicit Kernel(Tuning) {}
  void Run(const PMat<LhsScalar>& lhs, const PMat<RhsScalar>& rhs,
           const MulParamsType& mul_params, int start_row, int start_col,
           int end_row, int end_col, Mat<DstScalar>* dst) const {
    // See the comment in RunKernelTyped. end_row may be larger than
    // dst->layout.rows. It's the responsibility of the kernel to avoid
    // overrunning dst boundaries, which we do here by computing
    // clamped_end_row.
    int clamped_end_row = std::min(end_row, dst->layout.rows);
    int clamped_end_col = std::min(end_col, dst->layout.cols);
    RUY_DCHECK_LE(0, start_row);
    RUY_DCHECK_LE(start_row, clamped_end_row);
    RUY_DCHECK_LE(clamped_end_row, dst->layout.rows);
    RUY_DCHECK_LE(clamped_end_row, end_row);
    RUY_DCHECK_LE(end_row - clamped_end_row, LhsLayout::kCols);
    RUY_DCHECK_LE(0, start_col);
    RUY_DCHECK_LE(start_col, clamped_end_col);
    RUY_DCHECK_LE(clamped_end_col, dst->layout.cols);
    RUY_DCHECK_LE(clamped_end_col, end_col);
    RUY_DCHECK_LE(end_col - clamped_end_col, RhsLayout::kCols);
    profiler::ScopeLabel label("Kernel (Standard Cpp)");
    const int depth = lhs.layout.rows;
    for (int i = start_row; i < clamped_end_row; i++) {
      for (int j = start_col; j < clamped_end_col; j++) {
        AccumScalar accum = 0;
        for (int k = 0; k < depth; k++) {
          AccumScalar lhs_val = Element(lhs, k, i);
          AccumScalar rhs_val = Element(rhs, k, j);
          accum += lhs_val * rhs_val;
        }
        int channel =
            mul_params.channel_dimension() == ChannelDimension::kRow ? i : j;
        if (mul_params.bias()) {
          accum += mul_params.bias()[channel];
        }
        if (lhs.zero_point) {
          accum -= lhs.zero_point * rhs.sums[j];
        }
        if (rhs.zero_point) {
          accum -= rhs.zero_point * lhs.sums[i];
        }
        if (lhs.zero_point && rhs.zero_point) {
          accum += lhs.zero_point * rhs.zero_point * depth;
        }
        ApplyMultiplier(mul_params, channel, &accum);
        accum += dst->zero_point;
        accum = std::min<AccumScalar>(accum, mul_params.clamp_max());
        accum = std::max<AccumScalar>(accum, mul_params.clamp_min());
        *ElementPtr(dst, i, j) = static_cast<DstScalar>(accum);
      }
    }
  }
};

}  // namespace ruy

#endif  // RUY_RUY_KERNEL_H_

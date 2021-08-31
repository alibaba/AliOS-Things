/* Copyright 2020 Google LLC. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required_capacity by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

// Implementation of CreateTrMulParams, see function comment.

#ifndef RUY_RUY_CREATE_TRMUL_PARAMS_H_
#define RUY_RUY_CREATE_TRMUL_PARAMS_H_

#include <cstdint>
#include <cstring>
#include <type_traits>

#include "ruy/allocator.h"
#include "ruy/ctx.h"
#include "ruy/kernel.h"
#include "ruy/mat.h"
#include "ruy/mul_params.h"
#include "ruy/pack.h"
#include "ruy/path.h"
#include "ruy/performance_advisory.h"
#include "ruy/trace.h"
#include "ruy/trmul_params.h"

namespace ruy {
// While the only entry point to this file is CreateTrMulParams, its templatized
// nature requires putting more code in this header than we would like. This
// internal implementation code is enclosed in namespace 'detail'.
namespace detail {

inline void CreatePackedLayout(const MatLayout& src,
                               const KernelLayout& kernel_layout,
                               PMatLayout* packed_layout) {
  // Packed matrices are always column-major, because in TrMul that is always
  // the dimension of traversal of the kernel's inner loop.
  packed_layout->order = Order::kColMajor;
  packed_layout->rows = round_up_pot(src.rows, kernel_layout.rows);
  packed_layout->cols = round_up_pot(src.cols, kernel_layout.cols);
  packed_layout->stride = packed_layout->rows;
  packed_layout->kernel = kernel_layout;
}

template <typename Scalar, typename PackedScalar>
void CreatePackedMatrix(Side side, const KernelLayout& kernel_layout,
                        TrMulParams* params) {
  // Ruy always uses 32-bit signed accumulators for quantized
  // matrix multiplication, so we would like to always use std::int32_t
  // unconditionally for SumsType.
  // However, for floating point types, we still need a reasonable type here to
  // avoid tripping assertions elsewhere in the code.
  using SumsType =
      typename std::conditional<std::is_floating_point<Scalar>::value, Scalar,
                                std::int32_t>::type;

  const EMat& src = params->src[side];
  PEMat* packed_matrix = &params->packed_matrix[side];
  packed_matrix->data_type = Type::Create<PackedScalar>();
  packed_matrix->sums_type = Type::Create<SumsType>();
  CreatePackedLayout(src.layout, kernel_layout, &packed_matrix->layout);
  packed_matrix->zero_point = Pack<PackedScalar, Scalar>(src.zero_point);
}

template <typename KernelType>
struct CheckKernelPathImpl {
  static void Run(Path) {
    // Do nothing.
    // Path fallbacks are normal in general (see RUY_INHERIT_KERNEL).
    // That is to say that one may instantiate ruy::Mul with a weird combination
    // of types, such as LhsScalar==float and RhsScalar==double, and have it
    // work by silently falling back to Path::kStandardCpp. Only in specific
    // cases do we have dedicated kernels overriding that fallback, and that is
    // what partial specializations of this template will check.
  }
};

#if RUY_DCHECK_IS_ENABLED
template <Path ThePath, typename SrcScalar, typename AccumScalar,
          typename DstScalar>
struct CheckKernelPathImpl<Kernel<ThePath, SrcScalar, SrcScalar, DstScalar,
                                  MulParams<AccumScalar, DstScalar>>>
    final {
  using KernelType = Kernel<ThePath, SrcScalar, SrcScalar, DstScalar,
                            MulParams<AccumScalar, DstScalar>>;
  static void Run(Path expected_path) {
    // We want to assert that we are using a dedicated Kernel specialization and
    // not a fallback when we know we are in a case where such a kernel
    // specialization exists. At the moment in the current state of ruy's
    // architecture support for ARM and x86, that is when LhsScalar==RhsScalar
    // (already implied in this partial specialization) and when that type is
    // either float, int8, or uint8. Indeed, we have kernels supporting float
    // and int8, and we have the packing code converting uint8 to int8 (see
    // PackedTypeImpl).
    static constexpr bool kSrcScalarTypeSupportsFastKernels =
        std::is_same<SrcScalar, float>::value ||
        std::is_same<SrcScalar, std::int8_t>::value ||
        std::is_same<SrcScalar, std::uint8_t>::value;
    if (kSrcScalarTypeSupportsFastKernels) {
      RUY_DCHECK_EQ(expected_path, KernelType::kPath);
    }
  }
};
#endif

template <typename KernelType>
void CheckKernelPath(Path expected_path) {
  CheckKernelPathImpl<KernelType>::Run(expected_path);
}

template <Path ThePath, typename LhsScalar, typename RhsScalar,
          typename AccumScalar, typename DstScalar>
void PopulateTrMulParams(TrMulParams* params) {
  RUY_TRACE_SCOPE;
  using PackedLhsScalar = PackedType<ThePath, LhsScalar>;
  using PackedRhsScalar = PackedType<ThePath, RhsScalar>;
  using Kernel =
      Kernel<ThePath, PackedLhsScalar, PackedRhsScalar, AccumScalar, DstScalar>;
  using LhsKernelLayout = typename Kernel::LhsLayout;
  using RhsKernelLayout = typename Kernel::RhsLayout;

  params->path = ThePath;

  CreatePackedMatrix<LhsScalar, PackedLhsScalar>(
      Side::kLhs, ToKernelLayout<LhsKernelLayout>(), params);
  CreatePackedMatrix<RhsScalar, PackedRhsScalar>(
      Side::kRhs, ToKernelLayout<RhsKernelLayout>(), params);
  params->run_pack[Side::kLhs] =
      &RunPack<ThePath, LhsKernelLayout, LhsScalar, PackedLhsScalar>;
  params->run_pack[Side::kRhs] =
      &RunPack<ThePath, RhsKernelLayout, RhsScalar, PackedRhsScalar>;
  params->run_kernel = &RunKernel<Kernel>::Run;
  CheckKernelPath<Kernel>(ThePath);
  RUY_TRACE_INFO(POPULATE_TRMUL_PARAMS);
}

// PopulateTrMulParamsAllCompiledPaths calls into one of multiple
// instantiations of PopulateTrMulParams. For each bit that is set in
// CompiledPaths, it statically instantiates PopulateTrMulParams with a Path
// corresponding to that single bit. The call to PopulateTrMulParams is
// guarded by a runtime check that it is in fact the dynamically selected path.
//
// PopulateTrMulParamsAllCompiledPaths is implemented with template
// metaprogramming by mutual recursion between PathSearchCountdown and
// PathSearchCompiledPaths.
//
// PopulateTrMulParamsAllCompiledPaths is logically implementing the following
// computation:
//
// template <Path CompiledPaths>
// void PopulateTrMulParamsAllCompiledPaths(Path the_path,
//                                            TrMulParams* params) {
//   for (int bit = 8 * sizeof(Path) - 1; bit != -1; bit--) { // [1]
//     Path current_path = static_cast<Path>(1 << bit);
//     if ((CompiledPaths & current_path) != Path::kNone) { // [2]
//       if (current_path == the_path) { // [3]
//         PopulateTrMulParams<current_path, ...>(the_path, params);
//         return;
//       }
//     }
//   }
// }
//
//
//
// [1] - Done by the main definition of PathSearchCountdown. The `bit--` is
// done in the recursion of PathSearchOnlyCompiledPaths.
// [2] - Done by PathSearchOnlyCompiledPaths's partial template
// specialization on InCompiledPaths. This is the check which necessitates
// doing the whole computation at C++ compile time.
// [3] - Done by the `if` in the main definition of
// PathSearchOnlyCompiledPaths.
//
// The template metaprogramming is necessary because:
// - In `PopulateTrMulParams<current_path, ...>`, current_path must be a C++
// compile-time constant.
// - PopulateTrMulParamsAllCompiledPaths must not instantiate
// inner loops for paths that are not in CompiledPaths, since that can result in
// bogus instantiations which cause a compile time failure.
template <Path CompiledPaths, int BitNumber, typename LhsScalar,
          typename RhsScalar, typename AccumScalar, typename DstScalar>
struct PathSearchCountdown;

template <Path CompiledPaths, bool InCompiledPaths, int BitNumber,
          typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
struct PathSearchOnlyCompiledPaths {
  static constexpr Path kCurrentPath = static_cast<Path>(1 << BitNumber);
  static void Search(Path the_path, TrMulParams* params) {
    if (kCurrentPath == the_path) {
      PopulateTrMulParams<kCurrentPath, LhsScalar, RhsScalar, AccumScalar,
                          DstScalar>(params);
      return;
    }
    PathSearchCountdown<CompiledPaths, BitNumber - 1, LhsScalar, RhsScalar,
                        AccumScalar, DstScalar>::Search(the_path, params);
  }
};

// Skip this iteration if CompiledPaths doesn't contain the specified path.
template <Path CompiledPaths, int BitNumber, typename LhsScalar,
          typename RhsScalar, typename AccumScalar, typename DstScalar>
struct PathSearchOnlyCompiledPaths<CompiledPaths, false, BitNumber, LhsScalar,
                                   RhsScalar, AccumScalar, DstScalar> {
  static void Search(Path the_path, TrMulParams* params) {
    PathSearchCountdown<CompiledPaths, BitNumber - 1, LhsScalar, RhsScalar,
                        AccumScalar, DstScalar>::Search(the_path, params);
  }
};

template <Path CompiledPaths, int BitNumber, typename LhsScalar,
          typename RhsScalar, typename AccumScalar, typename DstScalar>
struct PathSearchCountdown {
  static constexpr Path kCurrentPath = static_cast<Path>(1 << BitNumber);
  static void Search(Path the_path, TrMulParams* params) {
    PathSearchOnlyCompiledPaths<
        CompiledPaths, (CompiledPaths & kCurrentPath) != Path::kNone, BitNumber,
        LhsScalar, RhsScalar, AccumScalar, DstScalar>::Search(the_path, params);
  }
};

// Termination of the countdown. If the counter reaches -1, then we haven't
// found the specified path.
template <Path CompiledPaths, typename LhsScalar, typename RhsScalar,
          typename AccumScalar, typename DstScalar>
struct PathSearchCountdown<CompiledPaths, -1, LhsScalar, RhsScalar, AccumScalar,
                           DstScalar> {
  static void Search(Path, TrMulParams*) { RUY_DCHECK(false); }
};

template <Path CompiledPaths, typename LhsScalar, typename RhsScalar,
          typename AccumScalar, typename DstScalar>
void PopulateTrMulParamsAllCompiledPaths(Path the_path, TrMulParams* params) {
  RUY_TRACE_SCOPE;
  return PathSearchCountdown<CompiledPaths, 8 * sizeof(Path) - 1, LhsScalar,
                             RhsScalar, AccumScalar,
                             DstScalar>::Search(the_path, params);
}

template <typename AccumScalar, typename DstScalar>
void AssertThatExtraCapacityInPerChannelBuffersIsZeroInitialized(
    const MulParams<AccumScalar, DstScalar>& mul_params, int user_size,
    int user_capacity) {
#if RUY_DCHECK_IS_ENABLED
  if (mul_params.bias()) {
    for (int i = user_size; i < user_capacity; i++) {
      RUY_DCHECK_EQ(mul_params.bias()[i], 0);
    }
  }
  if (mul_params.multiplier_fixedpoint_perchannel()) {
    for (int i = user_size; i < user_capacity; i++) {
      RUY_DCHECK_EQ(mul_params.multiplier_fixedpoint_perchannel()[i], 0);
    }
  }
  if (mul_params.multiplier_exponent_perchannel()) {
    for (int i = user_size; i < user_capacity; i++) {
      RUY_DCHECK_EQ(mul_params.multiplier_exponent_perchannel()[i], 0);
    }
  }
#else
  (void)mul_params;
  (void)user_size;
  (void)user_capacity;
#endif
}

template <typename AccumScalar, typename DstScalar,
          bool HaveQuantizedMultipliers =
              std::is_same<AccumScalar, std::int32_t>::value &&
              !std::is_same<DstScalar, std::int32_t>::value>
struct EnsurePerChannelBuffersLargeEnoughImpl {
  static void Run(const TrMulParams& params, Allocator* allocator,
                  MulParams<AccumScalar, DstScalar>* mul_params) {
    const Side channel_side =
        mul_params->channel_dimension() == ChannelDimension::kRow ? Side::kLhs
                                                                  : Side::kRhs;
    const int required_capacity =
        params.packed_matrix[channel_side].layout.cols;
    const int user_size = params.src[channel_side].layout.cols;
    const int user_capacity = round_up_pot(
        user_size, mul_params->perchannel_buffers_capacity_rounding());
    // We should have already checked earlier for the case where
    // user_capacity >= required_capacity.
    RUY_DCHECK_GT(required_capacity, user_capacity);
    if (mul_params->bias()) {
      AccumScalar* new_data =
          allocator->Allocate<AccumScalar>(required_capacity);
      std::memcpy(new_data, mul_params->bias(),
                  user_size * sizeof(AccumScalar));
      std::memset(new_data + user_size, 0,
                  (required_capacity - user_size) * sizeof(AccumScalar));
      mul_params->set_bias(new_data);
    }
    if (mul_params->multiplier_fixedpoint_perchannel()) {
      AccumScalar* new_data =
          allocator->Allocate<AccumScalar>(required_capacity);
      std::memcpy(new_data, mul_params->multiplier_fixedpoint_perchannel(),
                  user_size * sizeof(AccumScalar));
      std::memset(new_data + user_size, 0,
                  (required_capacity - user_size) * sizeof(AccumScalar));
      mul_params->set_multiplier_fixedpoint_perchannel(new_data);
    }
    if (mul_params->multiplier_exponent_perchannel()) {
      int* new_data = allocator->Allocate<int>(required_capacity);
      std::memcpy(new_data, mul_params->multiplier_exponent_perchannel(),
                  user_size * sizeof(int));
      std::memset(new_data + user_size, 0,
                  (required_capacity - user_size) * sizeof(int));
      mul_params->set_multiplier_exponent_perchannel(new_data);
    }
  }
};

template <typename AccumScalar, typename DstScalar>
struct EnsurePerChannelBuffersLargeEnoughImpl<AccumScalar, DstScalar, false> {
  static void Run(const TrMulParams& params, Allocator* allocator,
                  MulParams<AccumScalar, DstScalar>* mul_params) {
    const Side channel_side =
        mul_params->channel_dimension() == ChannelDimension::kRow ? Side::kLhs
                                                                  : Side::kRhs;
    const int required_capacity =
        params.packed_matrix[channel_side].layout.cols;
    const int user_size = params.src[channel_side].layout.cols;
    const int user_capacity = round_up_pot(
        user_size, mul_params->perchannel_buffers_capacity_rounding());
    // We should have already checked earlier for the case where
    // user_capacity >= required_capacity.
    RUY_DCHECK_GT(required_capacity, user_capacity);
    if (mul_params->bias()) {
      AccumScalar* new_data =
          allocator->Allocate<AccumScalar>(required_capacity);
      std::memcpy(new_data, mul_params->bias(),
                  user_size * sizeof(AccumScalar));
      std::memset(new_data + user_size, 0,
                  (required_capacity - user_size) * sizeof(AccumScalar));
      mul_params->set_bias(new_data);
    }
  }
};

template <typename AccumScalar, typename DstScalar>
void EnsurePerChannelBuffersLargeEnough(
    const TrMulParams& params, Ctx* ctx,
    MulParams<AccumScalar, DstScalar>* mul_params) {
  // Early exit in the common case where the packed matrix size matches the
  // number of channels (as opposed to having been rounded up to a slightly
  // larger value).
  const Side channel_side =
      mul_params->channel_dimension() == ChannelDimension::kRow ? Side::kLhs
                                                                : Side::kRhs;
  const int required_capacity = params.packed_matrix[channel_side].layout.cols;
  const int user_size = params.src[channel_side].layout.cols;
  const int user_capacity = round_up_pot(
      user_size, mul_params->perchannel_buffers_capacity_rounding());
  AssertThatExtraCapacityInPerChannelBuffersIsZeroInitialized(
      *mul_params, user_size, user_capacity);
  if (required_capacity <= user_capacity) {
    return;
  }
  ctx->set_performance_advisory(
      PerformanceAdvisory::kReallocatedPerChannelBuffer);
  EnsurePerChannelBuffersLargeEnoughImpl<AccumScalar, DstScalar>::Run(
      params, ctx->GetMainAllocator(), mul_params);
}

// Ensures that `params->mul_params_bytes` contains MulParams data that's ready
// to be consumed by the kernel. As a first-order approximation, that is simply
// copying the user-provided `mul_params`, however there are a few changes.
//
//   1. The specified `channel_dimension` value overrides the channel_dimension
//      member in `mul_params`. The reason why `channel_dimension` is being
//      special-cased among MulParams members is that we will need to transpose
//      MulParams, and that consists just in toggling channel_dimension.
//   2. Per-channel buffers may be reallocated, see
//      EnsurePerChannelBuffersLargeEnough.
template <typename AccumScalar, typename DstScalar>
void FinalizeMulParams(const MulParams<AccumScalar, DstScalar>& mul_params,
                       ChannelDimension channel_dimension, Ctx* ctx,
                       TrMulParams* params) {
  using MulParamsType = MulParams<AccumScalar, DstScalar>;
  static_assert(alignof(MulParamsType) <= kMaxMulParamsAlignment, "");
  static_assert(sizeof(MulParamsType) <= kMaxMulParamsSize, "");
  static_assert(std::is_trivially_copyable<MulParamsType>::value, "");
  auto* dst_mul_params =
      reinterpret_cast<MulParamsType*>(params->mul_params_bytes);
  std::memcpy(dst_mul_params, &mul_params, sizeof(MulParamsType));
  dst_mul_params->set_channel_dimension(channel_dimension);
  EnsurePerChannelBuffersLargeEnough(*params, ctx, dst_mul_params);
}

// In this function, the `channel_dimension` parameter overrides the value
// of the channel_dimension member in the `mul_params` parameter. See the
// FinalizeMulParams comment.
template <Path CompiledPaths, typename LhsScalar, typename RhsScalar,
          typename AccumScalar, typename DstScalar>
void CreateTrMulParamsAssumingColMajorDst(
    const Mat<LhsScalar>& lhs, const Mat<RhsScalar>& rhs,
    const Mat<DstScalar>& dst,
    const MulParams<AccumScalar, DstScalar>& mul_params,
    ChannelDimension channel_dimension, Ctx* ctx, TrMulParams* params) {
  RUY_TRACE_SCOPE;
  RUY_DCHECK(IsColMajor(dst.layout));

  // Fill in the fields we already know.
  params->src[Side::kLhs] = EraseType(lhs);
  params->src[Side::kRhs] = EraseType(rhs);
  params->dst = EraseType(dst);

  // Determine which exact Path we're going to take in this Mul call.
  // This is cheap because it's cached in `ctx`. In user scenarios this always
  // evaluates to the same value on a given machine with given `CompiledPaths`,
  // but could be invalidated by a call to Ctx::SetRuntimeEnabledPaths(), which
  // might be exposed publicly in Context in the future.
  const Path the_path = ctx->SelectPath(CompiledPaths);

  RUY_TRACE_INFO(CREATE_TRMUL_PARAMS_ASSUMING_COLMAJOR_DST);

  // If we ever need again to fall back to Path::kStandardCpp, this is a good
  // place to do it -- just pass Path::kStandardCpp as both the template and
  // runtime parameters in this function call.
  // In the past we did that here (as version control history remembers).
  // A typical reason why we might need to resurrect that is if we implement
  // a new Path (i.e. port to a new ISA) and need to subdivide that work into
  // a series of incremental changes.
  PopulateTrMulParamsAllCompiledPaths<CompiledPaths, LhsScalar, RhsScalar,
                                      AccumScalar, DstScalar>(the_path, params);

  // This must be done last, as it depends on the specific choice of kernel.
  // Specifically, the EnsurePerChannelBuffersLargeEnough part of this will read
  // the packed matrix layouts that are written to `params` by the above
  // PopulateTrMulParams* call.
  FinalizeMulParams(mul_params, channel_dimension, ctx, params);
}

}  // namespace detail

inline ChannelDimension Transpose(ChannelDimension channel_dimension) {
  return channel_dimension == ChannelDimension::kCol ? ChannelDimension::kRow
                                                     : ChannelDimension::kCol;
}

// CreateTrMulParams's output is a TrMulParams object that encodes
// all of the input information required_capacity by the middle-end, that is,
// the TrMul function.
//
// CreateTrMulParams performs the following tasks:
//   1. Reduce to the case of column-major destination, by transposing the
//      whole problem as needed.
//   2. Select the single code path to be taken, out of the set of paths
//      described by the `CompiledPaths` template parameter, based on the
//      runtime input parameter `the_path`.
//   3. Perform type-erasure, converting templatized typed input parameters
//      to the un-typed data stored in TrMulParams.
template <Path CompiledPaths, typename LhsScalar, typename RhsScalar,
          typename AccumScalar, typename DstScalar>
void CreateTrMulParams(const Mat<LhsScalar>& lhs, const Mat<RhsScalar>& rhs,
                       const Mat<DstScalar>& dst,
                       const MulParams<AccumScalar, DstScalar>& mul_params,
                       Ctx* ctx, TrMulParams* params) {
  RUY_TRACE_SCOPE;
  ChannelDimension channel_dimension = mul_params.channel_dimension();
  if (IsColMajor(dst.layout)) {
    detail::CreateTrMulParamsAssumingColMajorDst<CompiledPaths>(
        lhs, rhs, dst, mul_params, channel_dimension, ctx, params);
  } else {
    RUY_TRACE_INFO(CREATE_TRMUL_PARAMS_TRANSPOSING);
    detail::CreateTrMulParamsAssumingColMajorDst<CompiledPaths>(
        rhs, lhs, Transpose(dst), mul_params, Transpose(channel_dimension), ctx,
        params);
  }
}

}  // namespace ruy

#endif  // RUY_RUY_CREATE_TRMUL_PARAMS_H_

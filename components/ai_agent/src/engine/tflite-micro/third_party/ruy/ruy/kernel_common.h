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

#ifndef RUY_RUY_KERNEL_COMMON_H_
#define RUY_RUY_KERNEL_COMMON_H_

#include <algorithm>
#include <cstdint>
#include <type_traits>

#include "ruy/apply_multiplier.h"
#include "ruy/check_macros.h"
#include "ruy/mat.h"
#include "ruy/matrix.h"
#include "ruy/mul_params.h"
#include "ruy/opt_set.h"
#include "ruy/path.h"
#include "ruy/platform.h"
#include "ruy/profiler/instrumentation.h"
#include "ruy/side_pair.h"
#include "ruy/size_util.h"
#include "ruy/tune.h"

namespace ruy {

template <Path ThePath, typename LhsScalar, typename RhsScalar,
          typename AccumScalar, typename DstScalar>
struct Kernel;

#define RUY_INHERIT_KERNEL(PARENT, CHILD)                               \
  template <typename LhsScalar, typename RhsScalar, typename DstScalar, \
            typename AccumScalar>                                       \
  struct Kernel<CHILD, LhsScalar, RhsScalar, AccumScalar, DstScalar>    \
      : Kernel<PARENT, LhsScalar, RhsScalar, AccumScalar, DstScalar> {  \
    explicit Kernel(Tuning tuning)                                      \
        : Kernel<PARENT, LhsScalar, RhsScalar, AccumScalar, DstScalar>( \
              tuning) {}                                                \
  };

// KernelParams are shared across 32-bit and 64-bit NEON code, and x86 code.
//
// In other cases, we still define (empty) versions, so that dummy kernels
// can use the classes in function signatures.
#if ((RUY_PLATFORM_NEON_64 || RUY_PLATFORM_NEON_32) && RUY_OPT(ASM)) || \
    RUY_PLATFORM_X86

#define RUY_ASM_FLAG_HAS_BIAS 0x1
#define RUY_ASM_FLAG_HAS_LHS_SUMS 0x2
#define RUY_ASM_FLAG_HAS_RHS_SUMS 0x4
#define RUY_ASM_FLAG_HAS_PERCHANNEL 0x8
#define RUY_ASM_FLAG_NEEDS_LEFT_SHIFT 0x10
#define RUY_ASM_FLAG_CHANNEL_DIMENSION_IS_COL 0x20

#define RUY_ASM_TYPE_ID_UINT8 1
#define RUY_ASM_TYPE_ID_INT8 2
#define RUY_ASM_TYPE_ID_INT16 3
#define RUY_ASM_TYPE_ID_INT32 4

template <typename DstScalar>
struct DstTypeId {};

template <>
struct DstTypeId<std::uint8_t> {
  static constexpr int kValue = RUY_ASM_TYPE_ID_UINT8;
};

template <>
struct DstTypeId<std::int8_t> {
  static constexpr int kValue = RUY_ASM_TYPE_ID_INT8;
};

template <>
struct DstTypeId<std::int16_t> {
  static constexpr int kValue = RUY_ASM_TYPE_ID_INT16;
};

template <>
struct DstTypeId<std::int32_t> {
  static constexpr int kValue = RUY_ASM_TYPE_ID_INT32;
};

template <int LhsCols, int RhsCols>
struct KernelParams8bit {
  static constexpr int kMaxDstTypeSize = 4;

  const std::int32_t* bias;
  const std::int32_t* lhs_sums;
  const std::int32_t* rhs_sums;
  const std::int8_t* lhs_base_ptr;
  const std::int32_t* multiplier_fixedpoint;
  const std::int32_t* multiplier_exponent;
  const std::int8_t* rhs_base_ptr;
  void* dst_base_ptr;
  std::int32_t lhs_zero_point;
  std::int32_t rhs_zero_point;
  std::int32_t dst_zero_point;
  std::int32_t prod_zp_depth;
  std::int32_t start_row;
  std::int32_t start_col;
  std::int32_t last_row;
  std::int32_t last_col;
  std::int32_t dst_rows;
  std::int32_t dst_cols;
  std::int32_t lhs_stride;
  std::int32_t rhs_stride;
  std::int32_t dst_stride;
  std::int32_t depth;
  std::int32_t clamp_min;
  std::int32_t clamp_max;
  std::uint8_t flags;
  std::uint8_t dst_type_id;
  const std::int32_t zero_data[LhsCols] = {0};
  std::uint8_t dst_tmp_buf[LhsCols * RhsCols * kMaxDstTypeSize];
  std::int32_t multiplier_fixedpoint_buf[LhsCols];
  std::int32_t multiplier_exponent_buf[LhsCols];
};

template <typename DstScalar, int LhsCols, int RhsCols>
void MakeKernelParams8bit(const PMat<std::int8_t>& lhs,
                          const PMat<std::int8_t>& rhs,
                          const MulParams<std::int32_t, DstScalar>& mul_params,
                          int start_row, int start_col, int end_row,
                          int end_col, Mat<DstScalar>* dst,
                          KernelParams8bit<LhsCols, RhsCols>* params) {
  using Params = KernelParams8bit<LhsCols, RhsCols>;

  static_assert(sizeof(DstScalar) <= Params::kMaxDstTypeSize, "");

  const int depth = lhs.layout.rows;
  RUY_DCHECK_EQ(start_row % LhsCols, 0);
  RUY_DCHECK_EQ(start_col % RhsCols, 0);
  RUY_DCHECK_EQ(end_row % LhsCols, 0);
  RUY_DCHECK_EQ(end_col % RhsCols, 0);

  params->lhs_base_ptr = lhs.data + start_row * lhs.layout.stride;
  params->rhs_base_ptr = rhs.data + start_col * rhs.layout.stride;
  params->flags = 0;
  params->bias = params->zero_data;
  if (mul_params.bias()) {
    params->bias = mul_params.bias();
    params->flags |= RUY_ASM_FLAG_HAS_BIAS;
  }
  if (lhs.sums) {
    params->lhs_sums = lhs.sums;
    params->flags |= RUY_ASM_FLAG_HAS_LHS_SUMS;
  }
  if (rhs.sums) {
    params->rhs_sums = rhs.sums;
    params->flags |= RUY_ASM_FLAG_HAS_RHS_SUMS;
  }
  if (mul_params.channel_dimension() == ChannelDimension::kCol) {
    params->flags |= RUY_ASM_FLAG_CHANNEL_DIMENSION_IS_COL;
  }
  params->start_row = start_row;
  params->start_col = start_col;
  params->last_row = end_row - LhsCols;
  params->last_col = end_col - RhsCols;
  params->lhs_stride = lhs.layout.stride;
  params->rhs_stride = rhs.layout.stride;
  params->dst_stride = sizeof(DstScalar) * dst->layout.stride;
  params->lhs_zero_point = lhs.zero_point;
  params->rhs_zero_point = rhs.zero_point;
  params->dst_zero_point = dst->zero_point;
  params->depth = depth;
  params->prod_zp_depth = lhs.zero_point * rhs.zero_point * depth;
  params->flags |= RUY_ASM_FLAG_NEEDS_LEFT_SHIFT;
  if (mul_params.multiplier_fixedpoint_perchannel()) {
    // Temporary release-assert to debug some crashes in an application.
    RUY_CHECK(mul_params.multiplier_exponent_perchannel());
    params->flags |= RUY_ASM_FLAG_HAS_PERCHANNEL;
    params->multiplier_fixedpoint =
        mul_params.multiplier_fixedpoint_perchannel();
    params->multiplier_exponent = mul_params.multiplier_exponent_perchannel();
  } else {
    params->multiplier_fixedpoint = params->multiplier_fixedpoint_buf;
    params->multiplier_exponent = params->multiplier_exponent_buf;
    for (int i = 0; i < LhsCols; i++) {
      params->multiplier_fixedpoint_buf[i] = mul_params.multiplier_fixedpoint();
      params->multiplier_exponent_buf[i] = mul_params.multiplier_exponent();
    }
  }
  params->clamp_min = mul_params.clamp_min();
  params->clamp_max = mul_params.clamp_max();
  params->dst_rows = dst->layout.rows;
  params->dst_cols = dst->layout.cols;

  RUY_DCHECK_LT(params->last_row, params->dst_rows);
  RUY_DCHECK_LT(params->last_col, params->dst_cols);

  params->dst_type_id = DstTypeId<DstScalar>::kValue;
  params->dst_base_ptr =
      dst->data.get() + start_col * dst->layout.stride + start_row;

  // Temporary release-asserts to debug some crashes in an application.
  RUY_CHECK(params->multiplier_fixedpoint);
  RUY_CHECK(params->multiplier_exponent);
  RUY_CHECK(params->bias);
}

template <int LhsCols, int RhsCols>
struct KernelParamsFloat {
  const float* lhs_base_ptr;
  const float* rhs_base_ptr;
  float* dst_base_ptr;
  const float* bias;
  std::int32_t start_row;
  std::int32_t start_col;
  std::int32_t last_row;
  std::int32_t last_col;
  std::int32_t dst_rows;
  std::int32_t dst_cols;
  std::int32_t lhs_stride;
  std::int32_t rhs_stride;
  std::int32_t dst_stride;
  std::int32_t depth;
  float clamp_min;
  float clamp_max;
  std::uint8_t flags;
  const float zero_data[LhsCols] = {0};
  float dst_tmp_buf[LhsCols * RhsCols];
};

template <int LhsCols, int RhsCols>
inline void MakeKernelParamsFloat(const PMat<float>& lhs,
                                  const PMat<float>& rhs,
                                  const MulParams<float, float>& mul_params,
                                  int start_row, int start_col, int end_row,
                                  int end_col, Mat<float>* dst,
                                  KernelParamsFloat<LhsCols, RhsCols>* params) {
  const int depth = lhs.layout.rows;
  RUY_DCHECK_EQ(start_row % LhsCols, 0);
  RUY_DCHECK_EQ(start_col % RhsCols, 0);
  RUY_DCHECK_EQ(end_row % LhsCols, 0);
  RUY_DCHECK_EQ(end_col % RhsCols, 0);

  params->lhs_base_ptr = lhs.data + start_row * lhs.layout.stride;
  params->rhs_base_ptr = rhs.data + start_col * rhs.layout.stride;
  params->dst_base_ptr =
      dst->data.get() + start_col * dst->layout.stride + start_row;

  std::uint8_t flags = 0;
  params->bias = params->zero_data;
  if (mul_params.bias()) {
    params->bias = mul_params.bias();
    flags |= RUY_ASM_FLAG_HAS_BIAS;
  }
  if (mul_params.channel_dimension() == ChannelDimension::kCol) {
    flags |= RUY_ASM_FLAG_CHANNEL_DIMENSION_IS_COL;
  }
  params->flags = flags;
  params->start_row = start_row;
  params->start_col = start_col;
  params->last_row = end_row - LhsCols;
  params->last_col = end_col - RhsCols;
  params->lhs_stride = sizeof(float) * lhs.layout.stride;
  params->rhs_stride = sizeof(float) * rhs.layout.stride;
  params->dst_stride = sizeof(float) * dst->layout.stride;
  params->depth = depth;
  params->clamp_min = mul_params.clamp_min();
  params->clamp_max = mul_params.clamp_max();
  params->dst_rows = dst->layout.rows;
  params->dst_cols = dst->layout.cols;

  RUY_DCHECK_LT(params->last_row, params->dst_rows);
  RUY_DCHECK_LT(params->last_col, params->dst_cols);
}

#else  // ((RUY_PLATFORM_NEON_64 || RUY_PLATFORM_NEON_32) &&
       // RUY_OPT(ASM)) || RUY_PLATFORM_X86

template <int LhsCols, int RhsCols>
struct KernelParams8bit {};

template <int LhsCols, int RhsCols>
struct KernelParamsFloat {};

#endif  // ((RUY_PLATFORM_NEON_64 || RUY_PLATFORM_NEON_32) &&
        //  RUY_OPT(ASM)) || RUY_PLATFORM_X86

}  // namespace ruy

#endif  // RUY_RUY_KERNEL_COMMON_H_

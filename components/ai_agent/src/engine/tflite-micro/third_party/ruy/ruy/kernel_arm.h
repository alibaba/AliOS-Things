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

#ifndef RUY_RUY_KERNEL_ARM_H_
#define RUY_RUY_KERNEL_ARM_H_

#include <cstddef>
#include <cstdint>

#include "ruy/asm_helpers.h"
#include "ruy/kernel_common.h"
#include "ruy/mat.h"
#include "ruy/mul_params.h"
#include "ruy/opt_set.h"
#include "ruy/path.h"
#include "ruy/platform.h"
#include "ruy/profiler/instrumentation.h"
#include "ruy/side_pair.h"
#include "ruy/size_util.h"
#include "ruy/tune.h"

namespace ruy {

#if RUY_PLATFORM_NEON && RUY_OPT(ASM)

RUY_INHERIT_KERNEL(Path::kStandardCpp, Path::kNeon)
RUY_INHERIT_KERNEL(Path::kNeon, Path::kNeonDotprod)

#if RUY_PLATFORM_NEON_64
void Kernel8bitNeon(const KernelParams8bit<4, 4>& params);
void Kernel8bitNeon1Col(const KernelParams8bit<4, 4>& params);
#elif RUY_PLATFORM_NEON_32
void Kernel8bitNeon(const KernelParams8bit<4, 2>& params);
void Kernel8bitNeon1Col(const KernelParams8bit<4, 2>& params);
#endif
void Kernel8bitNeonA55ish(const KernelParams8bit<4, 4>& params);
void Kernel8bitNeonDotprod(const KernelParams8bit<8, 8>& params);
void Kernel8bitNeonDotprod1Col(const KernelParams8bit<8, 8>& params);
void Kernel8bitNeonDotprodA55ish(const KernelParams8bit<8, 8>& params);
void Kernel8bitNeonDotprodX1(const KernelParams8bit<8, 8>& params);

#if RUY_PLATFORM_NEON_64
template <typename DstScalar>
struct Kernel<Path::kNeon, std::int8_t, std::int8_t, std::int32_t, DstScalar> {
  static constexpr Path kPath = Path::kNeon;
  using LhsLayout = FixedKernelLayout<Order::kColMajor, 16, 4>;
  using RhsLayout = FixedKernelLayout<Order::kColMajor, 16, 4>;
  Tuning tuning = Tuning::kAuto;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PMat<std::int8_t>& lhs, const PMat<std::int8_t>& rhs,
           const MulParams<std::int32_t, DstScalar>& mul_params, int start_row,
           int start_col, int end_row, int end_col, Mat<DstScalar>* dst) const {
    KernelParams8bit<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParams8bit(lhs, rhs, mul_params, start_row, start_col, end_row,
                         end_col, dst, &params);
    if (dst->layout.cols == 1 &&
        mul_params.channel_dimension() == ChannelDimension::kRow) {
      Kernel8bitNeon1Col(params);
      return;
    }
    if (__builtin_expect(tuning == Tuning::kA55ish, true)) {
      Kernel8bitNeonA55ish(params);
    } else {
      Kernel8bitNeon(params);
    }
  }
};
#endif

#if RUY_PLATFORM_NEON_32
template <typename DstScalar>
struct Kernel<Path::kNeon, std::int8_t, std::int8_t, std::int32_t, DstScalar> {
  static constexpr Path kPath = Path::kNeon;
  using LhsLayout = FixedKernelLayout<Order::kColMajor, 16, 4>;
  using RhsLayout = FixedKernelLayout<Order::kColMajor, 16, 2>;
  Tuning tuning = Tuning::kAuto;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PMat<std::int8_t>& lhs, const PMat<std::int8_t>& rhs,
           const MulParams<std::int32_t, DstScalar>& mul_params, int start_row,
           int start_col, int end_row, int end_col, Mat<DstScalar>* dst) const {
    KernelParams8bit<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParams8bit(lhs, rhs, mul_params, start_row, start_col, end_row,
                         end_col, dst, &params);
    if (dst->layout.cols == 1 &&
        mul_params.channel_dimension() == ChannelDimension::kRow) {
      Kernel8bitNeon1Col(params);
      return;
    }
    Kernel8bitNeon(params);
  }
};
#endif

#if RUY_PLATFORM_NEON_64
template <typename DstScalar>
struct Kernel<Path::kNeonDotprod, std::int8_t, std::int8_t, std::int32_t,
              DstScalar> {
  static constexpr Path kPath = Path::kNeonDotprod;
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
      Kernel8bitNeonDotprod1Col(params);
    } else if (__builtin_expect(tuning == Tuning::kA55ish, true)) {
      Kernel8bitNeonDotprodA55ish(params);
    } else if (tuning == Tuning::kX1) {
      Kernel8bitNeonDotprodX1(params);
    } else {
      Kernel8bitNeonDotprod(params);
    }
  }
};
#endif

void KernelFloatNeon(const KernelParamsFloat<8, 8>& params);
void KernelFloatNeonX1(const KernelParamsFloat<8, 8>& params);
void KernelFloatNeonA55ish(const KernelParamsFloat<8, 8>& params);
void KernelFloat32Neon(const KernelParamsFloat<8, 4>& params);
void KernelFloatNeonDotprodA55ish(const KernelParamsFloat<8, 8>& params);

#if RUY_PLATFORM_NEON_64
// A Float kernel for ARM64 Neon.
template <>
struct Kernel<Path::kNeon, float, float, float, float> {
  static constexpr Path kPath = Path::kNeon;
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
    if (__builtin_expect(tuning == Tuning::kA55ish, true)) {
      KernelFloatNeonA55ish(params);
    } else if (tuning == Tuning::kX1) {
      KernelFloatNeonX1(params);
    } else {
      KernelFloatNeon(params);
    }
  }
};
#endif

#if RUY_PLATFORM_NEON_32
// A Float kernel for ARM32 Neon.
template <>
struct Kernel<Path::kNeon, float, float, float, float> {
  static constexpr Path kPath = Path::kNeon;
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 8>;
  using RhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 4>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PMat<float>& lhs, const PMat<float>& rhs,
           const MulParams<float, float>& mul_params, int start_row,
           int start_col, int end_row, int end_col, Mat<float>* dst) const {
    KernelParamsFloat<8, 4> params;

    MakeKernelParamsFloat(lhs, rhs, mul_params, start_row, start_col, end_row,
                          end_col, dst, &params);

    KernelFloat32Neon(params);
  }
};
#endif

// While the dotprod NEON extension does not concern floating-point arithmetic,
// its presence allows us to distinguish, in the in-order tuning case, between
// A53 and A55r1. TODO: should this be folded into tuning?
template <>
struct Kernel<Path::kNeonDotprod, float, float, float, float> {
  static constexpr Path kPath = Path::kNeonDotprod;
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 8>;
  using RhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 8>;
  using Base = Kernel<Path::kNeon, float, float, float, float>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PMat<float>& lhs, const PMat<float>& rhs,
           const MulParams<float, float>& mul_params, int start_row,
           int start_col, int end_row, int end_col, Mat<float>* dst) const {
    KernelParamsFloat<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParamsFloat(lhs, rhs, mul_params, start_row, start_col, end_row,
                          end_col, dst, &params);
    if (__builtin_expect(tuning == Tuning::kA55ish, true)) {
      KernelFloatNeonDotprodA55ish(params);
    } else if (tuning == Tuning::kX1) {
      KernelFloatNeonX1(params);
    } else {
      KernelFloatNeon(params);
    }
  }
};

#endif  // RUY_PLATFORM_NEON && RUY_OPT(ASM)

}  // namespace ruy

#endif  // RUY_RUY_KERNEL_ARM_H_

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

// Provides a reference (portable, non-optimized) ApplyMultiplier function.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Warning: this code is not meant to be bit-exact-normative.
// Please refer to the class comment of ruy::MulParams, in mul_params.h.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef RUY_RUY_APPLY_MULTIPLIER_H_
#define RUY_RUY_APPLY_MULTIPLIER_H_

#include <cstdint>
#include <type_traits>

#include "ruy/check_macros.h"
#include "ruy/mul_params.h"

namespace ruy {

// Applies the quantized multiplier to the `*accum` accumulator value, if
// applicable, that is, if AccumScalar==int32 and DstScalar!=int32. Otherwise,
// does nothing.
//
// This is slow, portable, 'reference' code. It should only be used in
// ReferenceMul and in Path::kStandardCpp. There isn't a point in optimizing it,
// either. Fast paths have that multiplier work done as part of the kernel,
// typically written in assembly anyway.
template <typename AccumScalar, typename DstScalar>
void ApplyMultiplier(const MulParams<AccumScalar, DstScalar>& mul_params,
                     int channel, AccumScalar* accum);

namespace detail {

// Copied from TF Lite code.
std::int32_t MultiplyByQuantizedMultiplier(std::int32_t x,
                                           std::int32_t quantized_multiplier,
                                           int shift);

// Helper to apply a fixed-point multiplier.  Only 'applicable' if AccumScalar
// is int32 (i.e. in all cases except floating-point) and if the destination is
// not int32 (i.e. unless the user wants to get raw accumulators).
template <typename AccumScalar, typename DstScalar,
          bool IsApplicable = std::is_same<AccumScalar, std::int32_t>::value &&
                              !std::is_same<DstScalar, std::int32_t>::value>
struct ApplyMultiplierImpl {};

// Specialization in non-applicable case: do nothing.
template <typename AccumScalar, typename DstScalar>
struct ApplyMultiplierImpl<AccumScalar, DstScalar, false> {
  static void Run(const MulParams<AccumScalar, DstScalar>&, int, AccumScalar*) {
  }
};

template <typename AccumScalar, typename DstScalar>
struct ApplyMultiplierImpl<AccumScalar, DstScalar, true> {
  static void Run(const MulParams<AccumScalar, DstScalar>& mul_params,
                  int channel, AccumScalar* accum) {
    AccumScalar m = mul_params.multiplier_fixedpoint_perchannel()
                        ? mul_params.multiplier_fixedpoint_perchannel()[channel]
                        : mul_params.multiplier_fixedpoint();
    int e = mul_params.multiplier_exponent_perchannel()
                ? mul_params.multiplier_exponent_perchannel()[channel]
                : mul_params.multiplier_exponent();
    *accum = MultiplyByQuantizedMultiplier(*accum, m, e);
  }
};

}  // namespace detail

template <typename AccumScalar, typename DstScalar>
void ApplyMultiplier(const MulParams<AccumScalar, DstScalar>& mul_params,
                     int channel, AccumScalar* accum) {
  detail::ApplyMultiplierImpl<AccumScalar, DstScalar>::Run(mul_params, channel,
                                                           accum);
}

}  // namespace ruy

#endif  // RUY_RUY_APPLY_MULTIPLIER_H_

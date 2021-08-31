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

#include "ruy/apply_multiplier.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>

namespace ruy {
namespace detail {

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Warning: this code is not meant to be bit-exact-normative.
// Please refer to the class comment of ruy::MulParams, in mul_params.h.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Simplified multiplier application function
//
// Double rounding and symmetric rounding are removed compared to reference.
// Double rounding seems unnecessary and can complicate implementations.
// Symmetric rounding also adds implementation complexity.
//
// Composed of a single rounding shift right and can lead to more HW
// friendly implementations.
//
// On NEON this can be translated to a SQDMULH + rounding shift right sequence.
// The use of SQDMULH rather than SQRDMULH gives a result that is
// equivalent to a single rounded shift since the truncating shift of SQDMULH
// can be combined with the rounding right shift via the formula (for k>=1):
//  ((x>>31)+(1<<(k-1)))>>k = (x + (1<<(30+k))>>(31+k)
//
// Preconditions:
// - quantized_multiplier >= 0
// - shift is -31 to +7 (negative for right shift)
std::int32_t MultiplyByQuantizedMultiplier(std::int32_t x,
                                           std::int32_t quantized_multiplier,
                                           int shift) {
  RUY_CHECK_GE(shift, -31);

  int total_shift = 31 - shift;

  std::int64_t x_64(x);
  std::int64_t quantized_multiplier_64(quantized_multiplier);
  std::int64_t round = (int64_t)1 << (total_shift - 1);
  int64_t result = x_64 * quantized_multiplier_64 + round;
  result = result >> total_shift;

  RUY_DCHECK_GE(result, std::numeric_limits<std::int32_t>::lowest());
  RUY_DCHECK_LE(result, std::numeric_limits<std::int32_t>::max());

  return static_cast<std::int32_t>(result);
}

}  // namespace detail

}  // namespace ruy

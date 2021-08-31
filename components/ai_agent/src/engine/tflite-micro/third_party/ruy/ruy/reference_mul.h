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

#ifndef RUY_RUY_REFERENCE_MUL_H_
#define RUY_RUY_REFERENCE_MUL_H_

#include <algorithm>

#include "ruy/apply_multiplier.h"
#include "ruy/matrix.h"
#include "ruy/mul_params.h"

namespace ruy {

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void ReferenceMul(const Matrix<LhsScalar>& lhs, const Matrix<RhsScalar>& rhs,
                  const MulParams<AccumScalar, DstScalar>& mul_params,
                  Matrix<DstScalar>* dst) {
  for (int i = 0; i < lhs.layout().rows(); i++) {
    for (int j = 0; j < rhs.layout().cols(); j++) {
      AccumScalar accum = 0;
      for (int k = 0; k < lhs.layout().cols(); k++) {
        AccumScalar lhs_val = Element(lhs, i, k);
        AccumScalar rhs_val = Element(rhs, k, j);
        accum += (lhs_val - lhs.zero_point()) * (rhs_val - rhs.zero_point());
      }
      int channel =
          mul_params.channel_dimension() == ChannelDimension::kRow ? i : j;
      if (mul_params.bias()) {
        accum += mul_params.bias()[channel];
      }
      ApplyMultiplier(mul_params, channel, &accum);
      accum += dst->zero_point();
      accum = std::min<AccumScalar>(accum, mul_params.clamp_max());
      accum = std::max<AccumScalar>(accum, mul_params.clamp_min());
      *ElementPtr(dst, i, j) = static_cast<DstScalar>(accum);
    }
  }
}

}  // namespace ruy

#endif  // RUY_RUY_REFERENCE_MUL_H_

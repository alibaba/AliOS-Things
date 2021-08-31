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

// Front-end validation code, see the Validate function.

#ifndef RUY_RUY_VALIDATE_H_
#define RUY_RUY_VALIDATE_H_

#include <cstdint>
#include <limits>
#include <type_traits>

#include "ruy/check_macros.h"
#include "ruy/mat.h"
#include "ruy/mul_params.h"
#include "ruy/side_pair.h"

namespace ruy {
namespace detail {

template <typename Scalar>
void CheckZeroPoint(Scalar zero_point) {
  if (std::is_floating_point<Scalar>::value) {
    RUY_DCHECK(!zero_point);
  }
}

template <typename LhsScalar, typename RhsScalar, typename DstScalar>
void ValidateZeroPoints(LhsScalar lhs_zero_point, RhsScalar rhs_zero_point,
                        DstScalar dst_zero_point) {
  CheckZeroPoint(lhs_zero_point);
  CheckZeroPoint(rhs_zero_point);
  CheckZeroPoint(dst_zero_point);

  // Guard against the case when both LHS and RHS zero_point's are equal to
  // the minimum representable value. In that case, padding with zero_point
  // values will generate the bad case for fast int8 kernels on NEON
  // (pre-dotprod) which attempt to multiply-accumulate two pairs of int8
  // into a int16:  this is safe except in the bad case -128*-128 + -128*-128.
  // See b/131609283. This only affects the kNeon path but we ban this for all
  // paths in order for ruy to have the same supported parameter space
  // on all paths.
  // We disable this check for now for the case of LhsScalar==RhsScalar==uint8
  // for backwards compatability with gemmlowp. The issue is still relevant
  // because we convert from uint8 to int8 for the backend kernels.
  if (!std::is_same<LhsScalar, uint8_t>::value ||
      !std::is_same<RhsScalar, uint8_t>::value) {
    RUY_DCHECK(lhs_zero_point != std::numeric_limits<LhsScalar>::lowest() ||
               rhs_zero_point != std::numeric_limits<RhsScalar>::lowest());
  }
}

}  // namespace detail

template <typename LhsScalar, typename RhsScalar, typename DstScalar>
void Validate(const Mat<LhsScalar>& lhs, const Mat<RhsScalar>& rhs,
              const Mat<DstScalar>& dst) {
  detail::ValidateZeroPoints(lhs.zero_point, rhs.zero_point, dst.zero_point);
}

}  // namespace ruy

#endif  // RUY_RUY_VALIDATE_H_

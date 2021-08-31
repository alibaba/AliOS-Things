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

#ifndef RUY_RUY_TRMUL_PARAMS_H_
#define RUY_RUY_TRMUL_PARAMS_H_

#include <cstdint>

#include "ruy/mat.h"
#include "ruy/mul_params.h"
#include "ruy/path.h"
#include "ruy/side_pair.h"
#include "ruy/tune.h"

namespace ruy {

using RunKernelFn = void(Tuning, const SidePair<PEMat>&, const void*,
                         const SidePair<int>&, const SidePair<int>&, EMat*);

using RunPackFn = void(Tuning, const EMat&, PEMat*, int, int);

// This should not be needed since we require c++14, where std::max is already
// constexpr, but TensorFlow continuous integration uses Ubuntu 16 with a
// libstdc++ that does not support that.
constexpr int constexpr_max(int a, int b) { return a > b ? a : b; }

// Under-estimating these values would be caught by a static_assert in
// StoreMulParams. Over-estimating these values cannot easily be caught, and
// would cause unnecessary inflation of the TrMulParams data structure.
constexpr int kMaxMulParamsAlignment =
    constexpr_max(alignof(void*), alignof(double));
constexpr int kMaxMulParamsSizeFloatingPointCase =
    sizeof(MulParams<double, double>);
constexpr int kMaxMulParamsSizeRawIntegerCase =
    sizeof(MulParams<std::int32_t, std::int32_t>);
constexpr int kMaxMulParamsSizeQuantizedIntegerCase =
    sizeof(MulParams<std::int32_t, std::int16_t>);
constexpr int kMaxMulParamsSize =
    constexpr_max(kMaxMulParamsSizeFloatingPointCase,
                  constexpr_max(kMaxMulParamsSizeRawIntegerCase,
                                kMaxMulParamsSizeQuantizedIntegerCase));

// OK to adjust as needed, but we want to avoid unnecessarily inflating that.
// Temporarily bumped from 32 to 48 as part of temporarily not using unions
// in MulParams.
static_assert(kMaxMulParamsSize <= 48, "");

// Type-erased data needed for implementing TrMul.
struct TrMulParams {
  TrMulParams() : run_pack{nullptr, nullptr}, is_prepacked{false, false} {}
  // Helper functions for invoking the function pointers.
  void RunPack(Side side, Tuning tuning, int start, int end) {
    run_pack[side](tuning, src[side], &packed_matrix[side], start, end);
  }
  void RunKernel(Tuning tuning, const SidePair<int>& start,
                 const SidePair<int>& end) {
    run_kernel(tuning, packed_matrix, mul_params_bytes, start, end, &dst);
  }

  // path id, can be useful info for some fine-tuning, e.g. to guess reasonable
  // cache sizes when not runtime-detectable.
  Path path;

  // Function pointers to type-erased entry points for kernels and packers.
  SidePair<RunPackFn*> run_pack;
  RunKernelFn* run_kernel = nullptr;

  // Matrices and packed matrices.
  SidePair<EMat> src;
  EMat dst;
  SidePair<PEMat> packed_matrix;
  SidePair<bool> is_prepacked;

  // Bytes underlying the MulParams, used as type-erased storage for MulParams
  // data as it isn't used until we reach the kernel code, where it is casted
  // back to the original MulParams type.
  alignas(kMaxMulParamsAlignment) char mul_params_bytes[kMaxMulParamsSize];
};

}  // namespace ruy

#endif  // RUY_RUY_TRMUL_PARAMS_H_

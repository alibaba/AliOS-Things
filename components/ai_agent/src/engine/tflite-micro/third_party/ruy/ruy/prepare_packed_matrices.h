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

#ifndef RUY_RUY_PREPARE_PACKED_MATRICES_H_
#define RUY_RUY_PREPARE_PACKED_MATRICES_H_

#include "ruy/ctx.h"
#include "ruy/trmul_params.h"

namespace ruy {

// Ensures that the packed matrices are ready for TrMul's work. In the generic
// case, this is merely allocating their buffers.
//
// In the non-default case where
// a matrix has a cache_policy allowing caching, this is where we implement
// this caching feature: determining whether to cache each matrix, performing
// the cache lookup, and possibly performing the packing and cache update if
// not already cached.
//
// Assumes that the packed matrices have previously been created, with their
// fields already set except for the buffer allocations, as part of
// CreateTrMulParams. The reason for separating this preparation from the
// creation is that the creation needs to be templatized and this preparation
// does not.
void PreparePackedMatrices(Ctx* ctx, TrMulParams* params);

}  // namespace ruy

#endif  // RUY_RUY_PREPARE_PACKED_MATRICES_H_

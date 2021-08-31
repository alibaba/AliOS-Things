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

#include "ruy/frontend.h"

#include "ruy/allocator.h"
#include "ruy/prepare_packed_matrices.h"
#include "ruy/trmul.h"
#include "ruy/trmul_params.h"

namespace ruy {

void MulFrontEndFromTrMulParams(Ctx* ctx, TrMulParams* params) {
  RUY_TRACE_SCOPE;
  // Handle Matrix::cache_policy, possibly retrieving existing packed matrices
  // or packing and caching now.
  PreparePackedMatrices(ctx, params);

  // We're done with the front-end work, now enter the middle-end.
  TrMul(ctx, params);

  ctx->GetMainAllocator()->FreeAll();
}

}  // namespace ruy

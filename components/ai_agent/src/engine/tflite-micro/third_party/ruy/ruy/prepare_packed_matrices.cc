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

#include "ruy/prepare_packed_matrices.h"

#include "ruy/allocator.h"
#include "ruy/ctx.h"
#include "ruy/matrix.h"
#include "ruy/prepacked_cache.h"
#include "ruy/side_pair.h"
#include "ruy/trace.h"
#include "ruy/trmul_params.h"

namespace ruy {
namespace {

// Returns true if the operand on the given side should use caching of the
// packed form. This may either be explicitly dictated by its cache_policy
// (if it is kNeverCache, the default, or kAlwaysCache), or it may depend
// on a heuristic decision based on the other operand's width. For example,
// in a matrix*vector product, for the LHS matrix operand, the other side is
// the RHS vector, with a width of 1, causing the packing of the LHS to be
// a large fraction of the overall work, so a heuristic would typically
// decide in favor of caching, if permitted at all by the cache_policy.
bool ShouldCache(const TrMulParams& params, Side side) {
  const CachePolicy cache_policy = params.src[side].cache_policy;
  // The width that matters is that of the other side, it is what determines
  // the amortization of the packing work done on the present side.
  const Side other_side = OtherSide(side);
  const int other_width = params.src[other_side].layout.cols;
  const int other_kernel_width =
      params.packed_matrix[other_side].layout.kernel.cols;
  switch (cache_policy) {
    case CachePolicy::kNeverCache:
      return false;
    case CachePolicy::kAlwaysCache:
      return true;
    case CachePolicy::kCacheIfLargeSpeedup:
      // The condition (other_width <= other_kernel_width) means that the kernel
      // will traverse each value of the present side only once, meaning that
      // the overhead of the packing work will be maximal, hence maximally
      // worth caching.
      return (other_width <= other_kernel_width);
    case CachePolicy::kCacheIfSignificantSpeedup:
      // Variant of the heuristic used in the kCacheIfLargeSpeedup case. The
      // kernel will run on each value of the present side only a few times,
      // so packing overhead will be significant.
      return (other_width <= 4 * other_kernel_width);
    default:
      RUY_DCHECK(false);
      return false;
  }
}

}  // namespace

void PreparePackedMatrices(Ctx* ctx, TrMulParams* params) {
  RUY_TRACE_SCOPE;
  for (Side side : {Side::kLhs, Side::kRhs}) {
    PEMat& packed_matrix = params->packed_matrix[side];
    if (ShouldCache(*params, side)) {
      // Use a cached packed matrix (possibly packing and caching now).
      auto* cache = ctx->GetPrepackedCache();
      auto action = cache->Get(params->src[side].data, &packed_matrix);
      RUY_TRACE_INFO(PREPARE_PACKED_MATRICES_SHOULD_CACHE);
      if (action == PrepackedCache::Action::kInsertedNewEntry) {
        params->RunPack(side, ctx->GetMainThreadTuning(), 0,
                        packed_matrix.layout.cols);
      }
      params->is_prepacked[side] = true;
    } else {
      RUY_TRACE_INFO(PREPARE_PACKED_MATRICES_NO_CACHE);
      // Do not use a cached packed matrix. Only need to allocate buffers now.
      Allocator* allocator = ctx->GetMainAllocator();
      packed_matrix.data = allocator->AllocateBytesAvoidingAliasingWith(
          DataBytes(packed_matrix), params->src[side].data);
      packed_matrix.sums = allocator->AllocateBytes(SumsBytes(packed_matrix));
    }
  }
}

}  // namespace ruy

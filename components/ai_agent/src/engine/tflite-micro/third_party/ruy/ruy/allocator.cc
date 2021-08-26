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

#include "ruy/allocator.h"

#include "ruy/opt_set.h"
#include "ruy/size_util.h"
#include "ruy/system_aligned_alloc.h"

namespace ruy {

Allocator::~Allocator() {
  FreeAll();
  detail::SystemAlignedFree(ptr_);
}

void* Allocator::AllocateFast(std::ptrdiff_t num_bytes) {
  if (current_ + num_bytes > size_) {
    return nullptr;
  }
  void* ret = static_cast<char*>(ptr_) + current_;
  current_ += num_bytes;
  return ret;
}

void* Allocator::AllocateSlow(std::ptrdiff_t num_bytes) {
  void* p = detail::SystemAlignedAlloc(num_bytes);
  fallback_blocks_total_size_ += num_bytes;
  fallback_blocks_.push_back(p);
  return p;
}

void* Allocator::AllocateBytes(std::ptrdiff_t num_bytes) {
  if (num_bytes == 0) {
    return nullptr;
  }
  const std::ptrdiff_t rounded_num_bytes =
      round_up_pot(num_bytes, detail::kMinimumBlockAlignment);
  if (void* p = AllocateFast(rounded_num_bytes)) {
    return p;
  }
  return AllocateSlow(rounded_num_bytes);
}

void* Allocator::AllocateBytesAvoidingAliasingWith(std::ptrdiff_t num_bytes,
                                                   const void* to_avoid) {
#if RUY_OPT(AVOID_ALIASING)
  if (num_bytes == 0) {
    return nullptr;
  }
  // The minimum L1D cache aliasing periodicity in bytes that we expect to
  // encounter on any device. This does not seem to be documented, but
  // empirically we observe the following:
  //   Cortex-A53:   1024
  //   Cortex-A55r1: 2048
  //   Cortex-A76:   not as easily observable.
  // Over-estimating this makes the AVOID_ALIASING optimization useless on
  // devices with lower periodicity.
  // Under-estimating this by 2x should be harmless.
  // Under-estimating this by a larger factor should gradually degrade
  // performance due to cache aliasing causing mutual eviction between
  // the packed matrix data, and the source matrix data being prefetched by the
  // CPU ahead of the packing code execution.
  static constexpr std::uint32_t kMinPeriod = 1024;
  static_assert(is_pot(kMinPeriod), "");
  void* p = AllocateBytes(num_bytes + kMinPeriod);
  auto unsigned_low_bits = [](const void* p) {
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p));
  };
  // This relies on unsigned integer overflow wrapping around.
  std::uint32_t diff_modulus =
      (unsigned_low_bits(p) - unsigned_low_bits(to_avoid)) % kMinPeriod;
  // diff_modulus is in [0, kMinPeriod).
  // We want it as close as possible to the middle of that interval,
  // kMinPeriod/2. The bad 'aliasing' case, that we are working to avoid,
  // is when diff_modulus is close to the ends of that interval, 0 or
  // kMinPeriod. So we want to add an offset of kMinPeriod/2 if it is in the
  // first or the last quarter of that interval.
  bool need_offset =
      diff_modulus < kMinPeriod / 4 || diff_modulus > 3 * kMinPeriod / 4;
  return static_cast<char*>(p) + (need_offset ? (kMinPeriod / 2) : 0);
#else
  (void)to_avoid;
  return AllocateBytes(num_bytes);
#endif
}

void Allocator::FreeAll() {
  current_ = 0;
  if (fallback_blocks_.empty()) {
    return;
  }

  // No rounding-up of the size means linear instead of logarithmic
  // bound on the number of allocation in some worst-case calling patterns.
  // This is considered worth it because minimizing memory usage is important
  // and actual calling patterns in applications that we care about still
  // reach the no-further-allocations steady state in a small finite number
  // of iterations.
  std::ptrdiff_t new_size = size_ + fallback_blocks_total_size_;
  detail::SystemAlignedFree(ptr_);
  ptr_ = detail::SystemAlignedAlloc(new_size);
  size_ = new_size;

  for (void* p : fallback_blocks_) {
    detail::SystemAlignedFree(p);
  }
  fallback_blocks_.clear();
  fallback_blocks_total_size_ = 0;
}

}  // namespace ruy

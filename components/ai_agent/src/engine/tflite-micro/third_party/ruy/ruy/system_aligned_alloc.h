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

#ifndef RUY_RUY_SYSTEM_ALIGNED_ALLOC_H_
#define RUY_RUY_SYSTEM_ALIGNED_ALLOC_H_

#include <cstddef>

namespace ruy {

namespace detail {

// Minimum alignment for blocks.
//
// Considerations:
//  - This needs to be at least the alignment of any usual data type.
//  - It's useful that this is at least the size of a cache line to limit
//    possible cache side effects (if only on performance behavior).
//  - It's useful that this is at least the size of SIMD registers, as
//    some SIMD instruction sets have at least performance behavior
//    differences (e.g. NEON) or even different requirements (e.g. SSE)
//    based on that.
//  - It's useful that this is at least the size of an "exclusive reservation
//    granule" on ARM, meaning that if we use this Allocator to allocate
//    an atomic variable, there will be no side effects from other things
//    contending for exclusive/atomic memory accesses to it. While the
//    ARM reference manual mentions that this granule size may be as large
//    as 2048 bytes, in practice we observe it to be 64 bytes. It can
//    be queried cheaply, at runtime, from userspace, if needed.
constexpr std::ptrdiff_t kMinimumBlockAlignment = 64;

// Primitive allocation functions obtaining aligned memory from the
// operating system.
void* SystemAlignedAlloc(std::ptrdiff_t num_bytes);
void SystemAlignedFree(void* ptr);

}  // namespace detail

}  // namespace ruy

#endif  // RUY_RUY_SYSTEM_ALIGNED_ALLOC_H_

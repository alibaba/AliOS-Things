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

#ifndef RUY_RUY_ALLOCATOR_H_
#define RUY_RUY_ALLOCATOR_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace ruy {

// Specialized allocator designed to converge to a steady-state where all
// allocations are bump-ptr allocations from an already-allocated buffer.
//
// To support these constraints, this allocator only supports two
// operations.
// - AllocateBytes/Allocate<Pointer>: allocates a pointer to storage of a
// specified size, which will be aligned to kMinimumBlockAlignment.
// - FreeAll: frees all previous allocations (but retains the internal
// buffer to minimize future calls into the system allocator).
//
// This class is specialized for supporting just those two operations
// under this specific steady-state usage pattern. Extending this class
// with new allocation interfaces that don't fit that pattern is probably not
// the right choice. Instead, build a new class on top of
// SystemAlignedAlloc/SystemAlignedFree.
//
// All operations happen on aligned blocks for simplicity.
//
// Theory of operation:
//
// - ptr_, current_, and size_ implement a basic bump-ptr allocator.
//
// - in AllocateBytes, the fast path is just a bump-ptr
// allocation. If our bump-ptr allocator doesn't have enough space for an
// allocation, then we allocate a block from the system allocator to
// service the allocation request. We save that block in fallback_blocks_
// and track the total size of the fallback blocks in
// fallback_blocks_total_size_.
//
// - in FreeAll, the fast path just resets the bump-ptr allocator. If
// there are any fallback blocks, we free them and reallocate the
// bump-ptr allocator's buffer so that the next sequence of allocations
// will hopefully not need any fallback blocks.
class Allocator final {
 public:
  ~Allocator();

  // Allocate a buffer.
  void* AllocateBytes(std::ptrdiff_t num_bytes);
  // Allocate a buffer, trying to avoid having its address close to aliasing
  // the specified `to_avoid` in the L1D cache.
  void* AllocateBytesAvoidingAliasingWith(std::ptrdiff_t num_bytes,
                                          const void* to_avoid);
  // Allocate an array of `count` elements of type T.
  template <typename T>
  T* Allocate(std::ptrdiff_t count) {
    return static_cast<T*>(AllocateBytes(count * sizeof(T)));
  }
  // Allocate an array of `count` elements of the given `Pointer` type's
  // element_type.
  template <typename Pointer>
  void Allocate(std::ptrdiff_t count, Pointer* out) {
    using T = typename std::pointer_traits<Pointer>::element_type;
    *out = Allocate<T>(count);
  }

  // Free all allocated blocks. Internally consolidate allocated buffers as
  // explained in the class comment.
  void FreeAll();

 private:
  void operator=(const Allocator&) = delete;
  void* AllocateFast(std::ptrdiff_t num_bytes);
  void* AllocateSlow(std::ptrdiff_t num_bytes);

  void* ptr_ = nullptr;
  std::ptrdiff_t current_ = 0;
  std::ptrdiff_t size_ = 0;
  std::vector<void*> fallback_blocks_;
  std::ptrdiff_t fallback_blocks_total_size_ = 0;
};

}  // namespace ruy

#endif  // RUY_RUY_ALLOCATOR_H_

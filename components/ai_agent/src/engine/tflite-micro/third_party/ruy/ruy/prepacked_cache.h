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

#ifndef RUY_RUY_PREPACKED_CACHE_H_
#define RUY_RUY_PREPACKED_CACHE_H_

#include <cstddef>
#include <cstdint>
#include <unordered_map>

#include "ruy/mat.h"

namespace ruy {

// "Low effort" Least Recently Used Cache for Prepacked Matrices
// A cache mechanism for prepacked matrices that ejects oldest entries.
// The implementation is "low effort" in the following ways:
//  - we just linearly search for the oldest entry when doing an ejection
//  - the ejection policy is very simple: if the new size would be above the
// .  threshold, we will eject entries until the size is below the threshold.
// Current use cases (RNNs with GEMV operations) indicate that ejection is rare
// and memory constraints are tight, so we devote no additional storage to the
// LRU mechanism and accept O(n) search to eject oldest entry. In practice,
// the number of total entries has not been shown to be large.
//
// An instance of PrepackedCache is always owned by a Context. Just like
// Context, no "thread safety" consideration is applicable to this class:
// no two threads may simulaneously be accessing the same instance.
class PrepackedCache final {
 public:
  enum class Action { kGotExistingEntry, kInsertedNewEntry };

  static constexpr int kDefaultMaxBuffersBytes = 1 << 28;

  // A key in this key-value cache. Equality of keys implies interchangeable
  // packed matrices, so we must be careful to make this Key type specific
  // enough, and its equality comparison operator strict enough.
  //
  // These keys need to be used before the packed matrix buffers are allocated
  // (since they are used to determine whether to allocate a new buffer).
  // So they instead use the source matrix's data pointer. On the other hand,
  // the packed matrix's layout structure is already available by the time we
  // need to handle Keys, and that's fortunate because it is more specific
  // than the source matrix's layout: it also encodes details about the kernel's
  // small-scale block layout. In the past, we made the kernel function pointer
  // part of the cache key, but all that is relevant here is the packed layout.
  //
  // The only other field that needs to be involved is the zero_point, for
  // quantized matrices, although it seems far-fetched that the same matrix
  // data would be reused with different zero_point values.
  //
  // The data types (PEMat::data_type and PEMat::sums_type) are omitted based on
  // the "strict aliasing" model: each memory location should contain data of
  // only one type. This could be relaxed in the future, by adding data types
  // to this Key type, if a use case arises.
  struct Key {
    // The source matrix's data pointer.
    const void* src_data;
    // The packed matrix's layout, see PEMat::layout.
    PMatLayout packed_layout;
    // The packed matrix's zero point (for integer-quantized matrices only).
    std::int32_t zero_point;
  };

  friend bool operator==(const Key& a, const Key& b) {
    return a.src_data == b.src_data && a.packed_layout == b.packed_layout &&
           a.zero_point == b.zero_point;
  }

  struct KeyHash {
    std::size_t operator()(const Key&) const;
  };

  // A dummy timestamp to associate to each entry (see struct Entry) to
  // determine which entry is "least recently used" when ejecting entries.
  // This is just an integer counter, not related to physical time.
  // It does not need to be atomic because only one thread uses an instance
  // of PrepackedCache at a time (see class comment).
  using Timestamp = std::uint64_t;

  struct Entry {
    PEMat packed_matrix;
    Timestamp timestamp;
  };

  explicit PrepackedCache(int max_buffers_bytes = kDefaultMaxBuffersBytes)
      : max_buffers_bytes_(max_buffers_bytes) {}

  ~PrepackedCache();

  // Returns the total size in bytes of buffers held in this cache.
  std::ptrdiff_t BuffersBytes() const { return buffers_bytes_; }

  // Returns the number of packed matrices held in this cache.
  int MatrixCount() const { return cache_.size(); }

  // This is the method by which new matrices are cached, and existing cache
  // entries are queried.
  // `src_data` is the source matrix data pointer.
  // `packed_matrix` is a packed matrix structure where all fields have already
  // been populated, except for the `data` and `sums` pointers which have not
  // yet been allocated.
  //
  // This method:
  // 1. Queries the cache for an entry matching the given `src_data` pointer and
  //    the relevant fields of `packed_matrix`, particularly its `layout`.
  // 2. If a matching cache entry does not exist, it is created and inserted
  //    into the cache, and its `data` and `sums` buffers are allocated.
  // 3. The `packed_matrix` has its `data` and `sums` pointers set to point
  //    to the allocated buffers.
  // 4. The cache entry's timestamp is updated so it's the most recently used
  //    entry.
  // 5. The return value is Action::kInsertedNewEntry if at step 2 a new
  //    entry was created. Otherwise it is Action::kGotExistingEntry.
  Action Get(const void* src_data, PEMat* packed_matrix);

 private:
  void EjectOne();
  void EjectUntilRoomFor(std::ptrdiff_t new_bytes);

  std::unordered_map<Key, Entry, KeyHash> cache_;
  const std::ptrdiff_t max_buffers_bytes_;
  std::ptrdiff_t buffers_bytes_ = 0;
  Timestamp timestamp_ = 0;
};

}  // namespace ruy

#endif  // RUY_RUY_PREPACKED_CACHE_H_

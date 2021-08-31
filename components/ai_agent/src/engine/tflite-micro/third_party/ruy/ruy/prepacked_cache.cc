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

#include "ruy/prepacked_cache.h"

#include "ruy/mat.h"
#include "ruy/profiler/instrumentation.h"
#include "ruy/system_aligned_alloc.h"

namespace ruy {

namespace {

// Allocates the `data` and `sums` buffers, and sets the corresponding
// pointer fields, in a PEMat whose other fields, particularly `layout`
// and the runtime data types, are already populated.
std::ptrdiff_t AllocateBuffers(PEMat* packed_matrix) {
  const std::ptrdiff_t data_bytes = DataBytes(*packed_matrix);
  packed_matrix->data = detail::SystemAlignedAlloc(data_bytes);
  std::ptrdiff_t sums_bytes = 0;
  if (!packed_matrix->sums_type.is_floating_point) {
    // Integer quantized matrices also need the `sums` buffer.
    sums_bytes = SumsBytes(*packed_matrix);
    packed_matrix->sums = detail::SystemAlignedAlloc(sums_bytes);
  }
  return data_bytes + sums_bytes;
}

// Frees the `data` and `sums` buffers held by a PEMat.
void FreeBuffers(const PEMat& packed_matrix) {
  detail::SystemAlignedFree(packed_matrix.data);
  detail::SystemAlignedFree(packed_matrix.sums);
}

}  // end anonymous namespace

std::size_t PrepackedCache::KeyHash::operator()(
    const PrepackedCache::Key& key) const {
  std::size_t src_data_hash = reinterpret_cast<std::size_t>(key.src_data);
  // Naive hash of the layout. Based on some heuristic reasoning, not any
  // benchmarking.
  // A choice of hash function here is just an optimization matter
  // anyway, since a hash collision only results in some Key::operator== calls
  // to disambiguate, and even just returning src_data_hash, ignoring the layout
  // altogether, would probably be good enough, as the case of multiple entries
  // with the same data pointer will be uncommon.
  // Here we multiply-add the layout fields using some small constant prime
  // numbers as multipliers. The conventional approach of xor-ing bit-rotations
  // would result in some hash collisions because these values are typically
  // small positive integers, so bit-rotations are essentially bit-shifts,
  // and powers of two are common.
  std::size_t packed_layout_hash =
      static_cast<int>(key.packed_layout.order) +
      static_cast<int>(key.packed_layout.kernel.order) * 2 +
      key.packed_layout.stride * 3 + key.packed_layout.kernel.rows * 5 +
      key.packed_layout.kernel.cols * 7 + key.packed_layout.rows * 11 +
      key.packed_layout.cols * 13;
  return src_data_hash ^ packed_layout_hash;
}

PrepackedCache::~PrepackedCache() {
  for (const auto& pair : cache_) {
    FreeBuffers(pair.second.packed_matrix);
  }
}

PrepackedCache::Action PrepackedCache::Get(const void* src_data,
                                           PEMat* packed_matrix) {
  // Construct a Key and look up the cache.
  Key key;
  key.src_data = src_data;
  key.packed_layout = packed_matrix->layout;
  key.zero_point = packed_matrix->zero_point;
  const auto& itr = cache_.find(key);

  if (itr != cache_.end()) {
    // Found existing entry. Update its timestamp and return it.
    itr->second.timestamp = timestamp_++;
    *packed_matrix = itr->second.packed_matrix;
    return Action::kGotExistingEntry;
  }

  // No existing entry found. Allocate new buffers now and insert in the cache.
  const std::ptrdiff_t new_bytes = AllocateBuffers(packed_matrix);
  EjectUntilRoomFor(new_bytes);
  Entry entry{*packed_matrix, timestamp_++};
  cache_.emplace(key, entry);
  buffers_bytes_ += new_bytes;
  return Action::kInsertedNewEntry;
}

void PrepackedCache::EjectUntilRoomFor(std::ptrdiff_t new_bytes) {
  profiler::ScopeLabel label("PrepackedCacheEjection");
  // While we are above the threshold of ejection, eject the LRU entry.
  while (!cache_.empty() && buffers_bytes_ + new_bytes > max_buffers_bytes_) {
    EjectOne();
  }
}

void PrepackedCache::EjectOne() {
  auto oldest = cache_.begin();
  Timestamp oldest_timestamp = oldest->second.timestamp;
  {
    for (auto itr = cache_.begin(); itr != cache_.end(); ++itr) {
      if (itr->second.timestamp < oldest_timestamp) {
        oldest = itr;
        oldest_timestamp = itr->second.timestamp;
      }
    }
  }
  const PEMat& packed_matrix = oldest->second.packed_matrix;
  buffers_bytes_ -= DataBytes(packed_matrix) + SumsBytes(packed_matrix);
  FreeBuffers(packed_matrix);
  cache_.erase(oldest);
}

}  // namespace ruy

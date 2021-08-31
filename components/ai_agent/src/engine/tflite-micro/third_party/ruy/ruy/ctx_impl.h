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

// Internal implementation details for Ctx. Drags in the entire world. Avoid
// #including this, use "ctx.h" instead.

#ifndef RUY_RUY_CTX_IMPL_H_
#define RUY_RUY_CTX_IMPL_H_

#include <cstddef>
#include <memory>
#include <vector>

#include "ruy/allocator.h"
#include "ruy/cpuinfo.h"
#include "ruy/ctx.h"
#include "ruy/path.h"
#include "ruy/performance_advisory.h"
#include "ruy/prepacked_cache.h"
#include "ruy/thread_pool.h"
#include "ruy/tune.h"

namespace ruy {

// The resources private to each Ruy thread.
struct ThreadSpecificResource final {
  // Each thread may be running on a different microarchitecture. For example,
  // some threads may be on big cores, while others are on little cores. Thus,
  // it's best for the tuning to be per-thread.
  TuningResolver tuning_resolver;
  // Each thread has its own local allocator.
  Allocator allocator;
};

// CtxImpl is what actually holds all the data members in a context.
// It is a subclass of Ctx, which provides the interface that is what most
// of ruy's code needs.
//
// A key requirement is that since many ruy files, including public headers,
// need a definition of Ctx, the "ctx.h" header defining it must minimize how
// many other ruy internal headers it includes. That is achieved by putting data
// members in the CtxImpl subclass, and ensuring that only a few .cc files, not
// header files, need a definition of CtxImpl.
class CtxImpl final : public Ctx {
 private:
  friend class Ctx;

  // Single Path bit indicating which Path was used last.
  Path last_used_path_ = Path::kNone;
  PerformanceAdvisory performance_advisory_ = PerformanceAdvisory::kNone;
  Tuning explicit_tuning_ = Tuning::kAuto;
  ThreadPool thread_pool_;
  int max_num_threads_ = 1;
  // Allocator for main thread work before invoking the threadpool.
  // Our simple Allocator does not allow reserving/allocating more blocks
  // while it's already in committed state, so the main thread needs both
  // this allocator, and its per-thread allocator.
  std::unique_ptr<Allocator> main_allocator_;
  std::unique_ptr<PrepackedCache> prepacked_cache_;
  // Set of Paths enabled at runtime. By default, that is based on runtime
  // detection, but may be overridden. The initial value kNone
  // means that detection has not yet been performed.
  Path runtime_enabled_paths_ = Path::kNone;
  CpuInfo cpuinfo_;
  // State for each thread in the thread pool. Entry 0 is the main thread.
  std::vector<std::unique_ptr<ThreadSpecificResource>>
      thread_specific_resources_;
};

}  // namespace ruy

#endif  // RUY_RUY_CTX_IMPL_H_

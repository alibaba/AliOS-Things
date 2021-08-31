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

// Context is the user-facing context class.

#ifndef RUY_RUY_CONTEXT_H_
#define RUY_RUY_CONTEXT_H_

#include <cstdint>

namespace ruy {

class Ctx;
class CtxImpl;
class ThreadPool;
enum class Path : std::uint8_t;
enum class Tuning;
enum class PerformanceAdvisory;

// A Context holds runtime information used by Ruy. It holds runtime resources
// such as the workers thread pool and the allocator (which holds buffers for
// temporary data), as well as runtime options controlling which Paths are
// enabled (typically based on which instruction sets are detected) and how
// many threads to use.
class Context final {
 public:
  Context();
  ~Context();

  // Returns the Path enum value that corresponds to the code path used by
  // the last ruy::Mul with this Context.
  Path last_used_path() const;

  // Control of whether to use kernels tuned for in-order or out-of-order CPU
  // cores. The default is auto-detection, so these methods should only be used
  // to override that auto-detection if it's not working as intended or for
  // testing.
  Tuning explicit_tuning() const;
  void set_explicit_tuning(Tuning value);

  // The thread pool held by this context to dispatch a ruy::Mul to worker
  // threads.
  //
  // By default, threads may spin-wait for a few milliseconds before reverting
  // to passive wait. This can be controlled by
  // `mutable_thread_pool()->set_spin_milliseconds(value)`.
  const ThreadPool& thread_pool() const;
  ThreadPool* mutable_thread_pool();

  // Controls the maximum number of threads to be used by ruy::Mul with this
  // Context. The number of threads in the pool will be that value minus one,
  // as the remaining portion of the work is done directly on the calling
  // thread.
  //
  // This defaults to 1. Multi-threading in ruy is always opt-in. There is
  // no auto-detection of hardware concurrency. That is on purpose, ruy focuses
  // on mobile applications where such concepts are difficult to define
  // (e.g. ARM big.LITTLE).
  int max_num_threads() const;
  void set_max_num_threads(int value);

  // Returns true of the last ruy::Mul using this Context flagged the specified
  // `advisory`. This is reset by each ruy::Mul call.
  bool performance_advisory(PerformanceAdvisory advisory) const;

  // When using Matrix::set_cache_policy(), this Context will keep a cache of
  // pre-packed matrix data. This function clears that cache.
  void ClearPrepackedCache();

  // Override auto-detection of supported code paths.
  //
  // Passing `paths == Path::kNone` means reverting to the default behavior.
  // This will trigger auto-detection on the next use.
  //
  // Other values will override auto-detection with the explicitly provided set
  // of paths.
  //
  // Paths in kNonArchPaths are always implicitly supported.
  void set_runtime_enabled_paths(Path paths);

 private:
  CtxImpl* const impl_;

  const Ctx& ctx() const;
  Ctx* mutable_ctx();

  friend const Ctx* get_ctx(const Context*);
  friend Ctx* get_ctx(Context*);

  // Disallow copy
  Context(const Context&) = delete;
};

}  // end namespace ruy

#endif  // RUY_RUY_CONTEXT_H_

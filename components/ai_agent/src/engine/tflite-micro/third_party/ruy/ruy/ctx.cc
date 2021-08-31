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

#include "ruy/ctx.h"

#include <cstdlib>
#include <functional>
#include <string>

#include "ruy/check_macros.h"
#include "ruy/cpuinfo.h"
#include "ruy/ctx_impl.h"
#include "ruy/have_built_path_for.h"
#include "ruy/path.h"
#include "ruy/performance_advisory.h"
#include "ruy/platform.h"
#include "ruy/prepacked_cache.h"
#include "ruy/trace.h"

namespace ruy {

const CtxImpl& Ctx::impl() const { return static_cast<const CtxImpl&>(*this); }
CtxImpl* Ctx::mutable_impl() { return static_cast<CtxImpl*>(this); }

Path Ctx::last_used_path() const { return impl().last_used_path_; }
Tuning Ctx::explicit_tuning() const { return impl().explicit_tuning_; }
void Ctx::set_explicit_tuning(Tuning value) {
  mutable_impl()->explicit_tuning_ = value;
}
const ThreadPool& Ctx::thread_pool() const { return impl().thread_pool_; }
ThreadPool* Ctx::mutable_thread_pool() { return &mutable_impl()->thread_pool_; }
int Ctx::max_num_threads() const { return impl().max_num_threads_; }
void Ctx::set_max_num_threads(int value) {
  mutable_impl()->max_num_threads_ = value;
}
void Ctx::clear_performance_advisories() {
  mutable_impl()->performance_advisory_ = PerformanceAdvisory::kNone;
}
void Ctx::set_performance_advisory(PerformanceAdvisory advisory) {
  mutable_impl()->performance_advisory_ =
      mutable_impl()->performance_advisory_ | advisory;
}
bool Ctx::performance_advisory(PerformanceAdvisory advisory) const {
  return (impl().performance_advisory_ & advisory) !=
         PerformanceAdvisory::kNone;
}

void Ctx::SetRuntimeEnabledPaths(Path paths) {
  if (paths == Path::kNone) {
    // Revert to default behavior using runtime detection.
    mutable_impl()->runtime_enabled_paths_ = Path::kNone;
  } else {
    // Explicitly set enabled paths. Ensure that non-arch are always enabled
    // (needed for fallbacks).
    mutable_impl()->runtime_enabled_paths_ = paths | kNonArchPaths;
  }
}

CpuInfo* Ctx::mutable_cpuinfo() { return &mutable_impl()->cpuinfo_; }

namespace {

int GetHexIntEnvVarOrZero(const char* name) {
  const char* val = getenv(name);
  if (!val) {
    return 0;
  }
  return std::stoi(val, nullptr, 16);
}

// For each Path bit set in `paths_to_test`, performs runtime detection and
// sets the corresponding bit in the return value if and only if it is
// supported. Path bits that are not set in the input
// `paths_to_detect` value are also left not set in the return value.
Path DetectRuntimeSupportedPaths(Path paths_to_detect, CpuInfo* cpuinfo) {
  // Paths in kNonArchPathsIncludingInternalVariants are always implicitly
  // supported. Further logic below may add more bits to `results`.
  Path result = kNonArchPathsIncludingInternalVariants;

  // Conditionally sets the `path` bit in `result`, if reported as supported
  // by the `is_supported` predicate.
  auto maybe_add = [&](Path path, std::function<bool(void)> is_supported) {
    if ((paths_to_detect & path) != Path::kNone) {
      if (is_supported()) {
        result = result | path;
      }
    }
  };

#if RUY_PLATFORM_ARM
  // NEON is unconditionally available on ARM64.
  // On ARM32 it's technically possible for it to be unavailable, but we've
  // always chosen to just crash on such devices. We could reevaluate that,
  // however for non-NEON devices to be actually supported, we would need to
  // address also compiler-generated NEON code. That would mean to remove
  // -mfpu=neon from ruy_copts and only use this flag in select NEON translation
  // units, and implement have_built_path_for_neon, similar to the x86 SIMD
  // paths.
  maybe_add(Path::kNeon, []() { return true; });

  // NEON dotprod requires runtime detection, however unlike the x86 SIMD paths
  // it still does not require have_built_path_for because we unconditionally
  // build it at the moment. That is largely because we have had to machine
  // encode dotprod instructions, so we don't actually rely on toolchain support
  // for them.
  maybe_add(Path::kNeonDotprod, [=]() { return cpuinfo->NeonDotprod(); });
#elif RUY_PLATFORM_X86
  // x86 SIMD paths currently require both runtime detection, and detection of
  // whether we're building the path at all.
  maybe_add(Path::kAvx,
            [=]() { return HaveBuiltPathForAvx() && cpuinfo->Avx(); });
  maybe_add(Path::kAvx2Fma,
            [=]() { return HaveBuiltPathForAvx2Fma() && cpuinfo->Avx2Fma(); });
  maybe_add(Path::kAvx512,
            [=]() { return HaveBuiltPathForAvx512() && cpuinfo->Avx512(); });
#else
  (void)maybe_add;
  (void)cpuinfo;
#endif

  // Sanity checks
  RUY_DCHECK_EQ(kNonArchPaths & ~result, Path::kNone);
  RUY_DCHECK_EQ(
      result & ~(kNonArchPathsIncludingInternalVariants | paths_to_detect),
      Path::kNone);
  return result;
}

}  // namespace

Path Ctx::GetRuntimeEnabledPaths() {
  RUY_TRACE_SCOPE;
  // Just a shorthand alias. Using a pointer to make it clear we're mutating
  // this value in-place.
  Path* paths = &mutable_impl()->runtime_enabled_paths_;

  // The value Path::kNone indicates the initial state before detection has been
  // performed.
  if (*paths != Path::kNone) {
    RUY_TRACE_INFO(GET_RUNTIME_ENABLED_PATHS_USING_SET_VALUE);
    return *paths;
  }
  // User may have set path explicitly in env var.
  Path paths_bitfield = static_cast<Path>(GetHexIntEnvVarOrZero("RUY_PATHS"));
  if (paths_bitfield != Path::kNone) {
    *paths = paths_bitfield;
    RUY_TRACE_INFO(GET_RUNTIME_ENABLED_PATHS_USING_ENV_VAR);
    return *paths;
  }
  // Finally, use runtime detection.
  *paths = DetectRuntimeSupportedPaths(kAllPaths, mutable_cpuinfo());
  RUY_TRACE_INFO(GET_RUNTIME_ENABLED_PATHS_USING_DETECTION);
  return *paths;
}

Path Ctx::SelectPath(Path compiled_paths) {
  return mutable_impl()->last_used_path_ =
             GetMostSignificantPath(compiled_paths & GetRuntimeEnabledPaths());
}

void Ctx::EnsureThreadSpecificResources(int thread_count) {
  auto& resources = mutable_impl()->thread_specific_resources_;
  while (thread_count > static_cast<int>(resources.size())) {
    resources.emplace_back(new ThreadSpecificResource);
  }
  RUY_DCHECK_LE(thread_count, static_cast<int>(resources.size()));
}

TuningResolver* Ctx::GetThreadSpecificTuningResolver(int thread_index) const {
  const auto& resources = impl().thread_specific_resources_;
  RUY_DCHECK_LT(thread_index, static_cast<int>(resources.size()));
  return &resources[thread_index]->tuning_resolver;
}

Allocator* Ctx::GetThreadSpecificAllocator(int thread_index) const {
  const auto& resources = impl().thread_specific_resources_;
  RUY_DCHECK_LT(thread_index, static_cast<int>(resources.size()));
  return &resources[thread_index]->allocator;
}

Allocator* Ctx::GetMainAllocator() {
  if (!impl().main_allocator_) {
    mutable_impl()->main_allocator_.reset(new Allocator);
  }
  return impl().main_allocator_.get();
}

PrepackedCache* Ctx::GetPrepackedCache() {
  if (!impl().prepacked_cache_) {
    mutable_impl()->prepacked_cache_.reset(new PrepackedCache);
  }
  return impl().prepacked_cache_.get();
}

Tuning Ctx::GetMainThreadTuning() {
  EnsureThreadSpecificResources(1);
  TuningResolver* tuning_resolver = GetThreadSpecificTuningResolver(0);
  tuning_resolver->SetTuning(explicit_tuning());
  return tuning_resolver->Resolve(mutable_cpuinfo());
}

void Ctx::ClearPrepackedCache() { mutable_impl()->prepacked_cache_ = nullptr; }

}  // namespace ruy

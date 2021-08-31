#include "ruy/cpuinfo.h"

#include <algorithm>
#include <cstdint>
#include <limits>

#include "ruy/check_macros.h"
#include "ruy/cpu_cache_params.h"
#include "ruy/platform.h"

#ifdef RUY_HAVE_CPUINFO
#include <cpuinfo.h>
#endif

namespace ruy {

namespace {
void MakeDummyCacheParams(CpuCacheParams* result) {
  // Reasonable dummy values
  result->local_cache_size = 32 * 1024;
  result->last_level_cache_size = 512 * 1024;
}
}  // end namespace

#ifdef RUY_HAVE_CPUINFO

CpuInfo::~CpuInfo() {
  if (init_status_ == InitStatus::kInitialized) {
    cpuinfo_deinitialize();
  }
}

bool CpuInfo::EnsureInitialized() {
  if (init_status_ == InitStatus::kNotYetAttempted) {
    init_status_ = Initialize();
    RUY_DCHECK_NE(init_status_, InitStatus::kNotYetAttempted);
  }
  return init_status_ == InitStatus::kInitialized;
}

namespace {
void QueryCacheParams(CpuCacheParams* cache_params) {
  const int processors_count = cpuinfo_get_processors_count();
  RUY_DCHECK_GT(processors_count, 0);
  int overall_local_cache_size = std::numeric_limits<int>::max();
  int overall_last_level_cache_size = std::numeric_limits<int>::max();
  for (int i = 0; i < processors_count; i++) {
    int local_cache_size = 0;
    int last_level_cache_size = 0;
    const cpuinfo_processor* processor = cpuinfo_get_processor(i);
    // Loop over cache levels. Ignoring L4 for now: it seems that in CPUs that
    // have L4, we would still prefer to stay in lower-latency L3.
    for (const cpuinfo_cache* cache :
         {processor->cache.l1d, processor->cache.l2, processor->cache.l3}) {
      if (!cache) {
        continue;  // continue, not break, it is possible to have L1+L3 but no
                   // L2.
      }
      const bool is_local =
          cpuinfo_get_processor(cache->processor_start)->core ==
          cpuinfo_get_processor(cache->processor_start +
                                cache->processor_count - 1)
              ->core;
      if (is_local) {
        local_cache_size = cache->size;
      }
      last_level_cache_size = cache->size;
    }
    // If no local cache was found, use the last-level cache.
    if (!local_cache_size) {
      local_cache_size = last_level_cache_size;
    }
    RUY_DCHECK_GT(local_cache_size, 0);
    RUY_DCHECK_GT(last_level_cache_size, 0);
    RUY_DCHECK_GE(last_level_cache_size, local_cache_size);
    overall_local_cache_size =
        std::min(overall_local_cache_size, local_cache_size);
    overall_last_level_cache_size =
        std::min(overall_last_level_cache_size, last_level_cache_size);
  }
  cache_params->local_cache_size = overall_local_cache_size;
  cache_params->last_level_cache_size = overall_last_level_cache_size;
}
}  // end namespace

CpuInfo::InitStatus CpuInfo::Initialize() {
  RUY_DCHECK_EQ(init_status_, InitStatus::kNotYetAttempted);
  if (!cpuinfo_initialize()) {
    MakeDummyCacheParams(&cache_params_);
    return InitStatus::kFailed;
  }
  QueryCacheParams(&cache_params_);
  return InitStatus::kInitialized;
}

bool CpuInfo::NeonDotprod() {
  return EnsureInitialized() && cpuinfo_has_arm_neon_dot();
}

bool CpuInfo::Sse42() {
  return EnsureInitialized() && cpuinfo_has_x86_sse4_2();
}

bool CpuInfo::Avx2Fma() {
  return EnsureInitialized() && cpuinfo_has_x86_avx2() &&
         cpuinfo_has_x86_fma3();
}

bool CpuInfo::Avx() { return EnsureInitialized() && cpuinfo_has_x86_avx(); }

bool CpuInfo::Avx512() {
  return EnsureInitialized() && cpuinfo_has_x86_avx512f() &&
         cpuinfo_has_x86_avx512dq() && cpuinfo_has_x86_avx512cd() &&
         cpuinfo_has_x86_avx512bw() && cpuinfo_has_x86_avx512vl();
}

bool CpuInfo::AvxVnni() {
  return EnsureInitialized() && cpuinfo_has_x86_avx512vnni();
}

bool CpuInfo::CurrentCpuIsA55ish() {
  if (!EnsureInitialized()) {
    return false;
  }

  switch (cpuinfo_get_uarch(cpuinfo_get_current_uarch_index())->uarch) {
    case cpuinfo_uarch_cortex_a53:
    case cpuinfo_uarch_cortex_a55r0:
    case cpuinfo_uarch_cortex_a55:
      return true;
    default:
      return false;
  }
}

bool CpuInfo::CurrentCpuIsX1() {
  if (!EnsureInitialized()) {
    return false;
  }
  if (cpuinfo_get_uarch(cpuinfo_get_current_uarch_index())->uarch ==
      cpuinfo_uarch_cortex_x1) {
    return true;
  }
  return false;
}

#else  // not defined RUY_HAVE_CPUINFO

CpuInfo::~CpuInfo() {}
bool CpuInfo::EnsureInitialized() {
  if (init_status_ == InitStatus::kNotYetAttempted) {
    MakeDummyCacheParams(&cache_params_);
    init_status_ = InitStatus::kInitialized;
  }
  RUY_DCHECK_EQ(init_status_, InitStatus::kInitialized);
  return true;
}
bool CpuInfo::NeonDotprod() { return false; }
bool CpuInfo::Sse42() { return false; }
bool CpuInfo::Avx() { return false; }
bool CpuInfo::Avx2Fma() { return false; }
bool CpuInfo::Avx512() { return false; }
bool CpuInfo::AvxVnni() { return false; }
bool CpuInfo::CurrentCpuIsA55ish() { return false; }
bool CpuInfo::CurrentCpuIsX1() { return false; }

#endif

const CpuCacheParams& CpuInfo::CacheParams() {
  EnsureInitialized();
  // On failure, EnsureInitialized leaves dummy values in cache_params_.
  return cache_params_;
}

}  // namespace ruy

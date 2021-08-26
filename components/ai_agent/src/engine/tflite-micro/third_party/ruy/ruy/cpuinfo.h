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

#ifndef RUY_RUY_CPUINFO_H_
#define RUY_RUY_CPUINFO_H_

#include "ruy/cpu_cache_params.h"

namespace ruy {

// Wraps the functionality that ruy needs from the cpuinfo library.
class CpuInfo final {
 public:
  CpuInfo() {}
  ~CpuInfo();

  // ARM features
  bool NeonDotprod();

  // X86 features
  bool Sse42();
  bool Avx();
  bool Avx2Fma();
  bool Avx512();
  bool AvxVnni();

  // Common features
  const CpuCacheParams& CacheParams();
  bool CurrentCpuIsA55ish();
  bool CurrentCpuIsX1();

 private:
  enum class InitStatus {
    kNotYetAttempted,
    kInitialized,
    kFailed,
  };

  InitStatus init_status_ = InitStatus::kNotYetAttempted;
  CpuCacheParams cache_params_;

  bool EnsureInitialized();
  InitStatus Initialize();

  CpuInfo(const CpuInfo&) = delete;
};

}  // namespace ruy

#endif  // RUY_RUY_CPUINFO_H_

/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

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

#ifndef RUY_RUY_CPU_CACHE_PARAMS_H_
#define RUY_RUY_CPU_CACHE_PARAMS_H_

namespace ruy {

// Holds some information about a CPU's data caches.
//
// Meaning of 'local': a 'local' cache means a cache that is used by only one
// CPU core, not shared with other cores. It might still be used by multiple
// 'processors' in case of SMT as in Intel HyperThreading. CPUs often have
// multiple levels of local cache, e.g. L1 and L2. We typically return the
// larger one, the assumption being that even the larger one has substantially
// lower latency than any higher (non-local) cache, however as noted below (*)
// the implementation may choose to ignore a cache level.
//
// Meaning of 'last level': this refers to some higher cache level, typically
// shared among multiple CPU cores, so we considered using the terminology
// 'shared' instead of 'last_level'. However that created some confusion of its
// own, as the meaning of 'shared' varies between CPUs, with some CPUs not
// having any level of cache shared among all cores. That is why we stick with
// the 'last_level' terminology, however with the following caveats:
//   1. As noted below (*) the implementation may choose to ignore a cache
// level, which could cause the 'last level' cache according to ruy not to be
// the actual last level.
//   2. On some systems-on-chip there is a 'last level' cache outside of the
// last level cache in the CPU complex. Ruy is not currently doing anything
// specific regarding such caches.
//   3. We haven't figured out how to amend our terminology to be meaningful
// on NUMA architectures. NUMA hasn't been part of ruy's scope so far.
//
// (*) Note on ignoring certain cache levels:
// The implementation may choose to ignore a cache if it's suspected not to
// have compelling performance. This is true about all cache levels, but more
// likely regarding the 'last level' cache. For example, a L4 cache may be
// ignored if we believe that it's not the right latency/size compromise for us,
// so on such a CPU, the L3 cache may be used as the 'last level' cache instead.
//
// (**) Note on CPUs with heterogeneous cores:
// Some CPUs have multiple cores with different local caches. For example, some
// ARM big.LITTLE CPUs have some CPU cores with L1=32k and L2=128k, and some
// other CPU cores with L1=64k and L2=256k or even 512k. On such CPUs, the
// fields in this struct refer to the minimum value over all cores. In other
// words, we use conservative values that do not risk over-estimating local
// cache sizes in case of a migration of our threads to smaller cores.
//
// Example:
// On a Qualcomm S855 SoC, there are 8 CPU cores. Each core has L1 and L2 data
// caches local to it:
// - 4 cores have L1=32k, L2=128k.
// - 3 cores have L1=64k, L2=256k.
// - 1 core has   L1=64k, L2=512k.
// All 8 cores share a L3 cache of size 2M, and there is beyond that a SoC-level
// cache of size 3M.
// On such a system, we should have:
// - local_level_cache_size=128k, the smallest L2 size.
// - last_level_cache_size=2M, the L3 cache size, ignoring the SoC-level cache.
struct CpuCacheParams final {
  // Minimum value (see (**)), over all cores, of the size in bytes of its local
  // cache (see "Meaning of 'local'").
  int local_cache_size = 0;
  // Minimum value (see (**)), over all cores, of the size in bytes of its last
  // level cache (see "Meaning of 'last level'").
  int last_level_cache_size = 0;
};

}  // namespace ruy

#endif  // RUY_RUY_CPU_CACHE_PARAMS_H_

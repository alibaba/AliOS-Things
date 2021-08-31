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

// Library doing minimal CPU detection to decide what to tune asm code for.
//
// # Tuning vs Path
//
// Tunings are merely local variations of optimized code paths, that are
// drop-in replacements for each other --- the input and output data layouts
// are identical.  By contrast, what ruy calls a Path dictates its own
// data layouts. For example, Path::kNeonDotprod will use different
// layouts compared to Path::kNeon; but within each, different tunings
// will share that same layout.
//
// # Tuning is for now only based on 1 bit: Generic / A55ish
//
// In practice, each of our asm code paths only needs one bit information to
// decide on tuning: whether the CPU is out-of-order or in-order.
// That is because out-of-order CPUs are by definition relatively insensitive
// to small-scale asm details (which is what "tuning" is about); and for each
// asm code path, there tends to be one main in-order CPU architecture that
// we focus our tuning effort on. Examples:
//  * For Path::kNeon, the main in-order CPU is Cortex-A53/A55 (pre-dotprod)
//  * For Path::kNeonDotprod, the main in-order CPU is Cortex-A55r1 (dotprod)
//
// Because having tuned code paths is a compromise of efficiency gains
// versus implementation effort and code size, we are happy to stop at just this
// single bit of information, Generic / A55ish, at least in the current CPU
// landscape. This could change in the future.
#ifndef RUY_RUY_TUNE_H_
#define RUY_RUY_TUNE_H_

#include "ruy/cpuinfo.h"
#include "ruy/opt_set.h"
#include "ruy/platform.h"
#include "ruy/time.h"

namespace ruy {

enum class Tuning {
  // kAuto means please use auto-detection. It's the default in the
  // user-visible parts (see Context). It's meant to be resolved to an
  // actual tuning at some point by means of TuningResolver.
  kAuto,
  // Use code not tuned for any particular CPU, typically performing well
  // on out-of-order cores that don't require as much tuning.
  kGeneric,
  // Use code tuned for "Cortex-A55-ish" CPUs, by which we mean mostly:
  // A53, A55r0 (pre-dotprod), A55r1 (with dotprod). These CPUs have in common
  // that they are in-order CPU cores with largely similar requirements of code
  // tuning. The most important such requirement is to use only 64-bit loads
  // to maximize dual-issuing.
  //
  // A55r1 differs from A55r0 and A53 in that it dual-issues 64-bit NEON loads
  // whereas A55r0 and A53 require using non-NEON ARM 64-bit loads together with
  // INS instructions to insert 64bit lanes into NEON registers. However, since
  // A55r1 supports dotprod unlike A55r0 and A53, they are not using the same
  // kernels in practice anyway, so there was no need to distinguish them with
  // separate Tuning values.
  kA55ish,
  // Use code tuned for Cortex-X1 CPUs. Currently, the driver to distinguish
  // this CPU is the get maximum performance on the dotprod kernels, where we
  // attain high performance simply by avoiding any manual loop unrolling. As a
  // purely performance oriented microarchitecture, there will likely be
  // additional reasons to distinguish the X1 from other CPUs.
  kX1
};

// Why a TuningResolver class?
//
// Ideally, this Library would offer a single function,
//   Tuning GetCurrentCPUTuning();
//
// However, determining information about the current CPU is not necessarily
// cheap, so we currently cache that and only invalidate/reevaluate after
// a fixed amount of time. This need to store state is why this library
// has to expose a class, TuningResolver, not just a function.
class TuningResolver {
 public:
  TuningResolver();

  // Allows the user to specify an explicit Tuning value, bypassing auto
  // detection; or to specify Tuning::kAuto, reverting to auto detection.
  void SetTuning(Tuning tuning) { unresolved_tuning_ = tuning; }

  // Get an actual tuning --- that is the function that this class wanted to be.
  Tuning Resolve(CpuInfo* cpuinfo);

 private:
  TuningResolver(const TuningResolver&) = delete;

  // Perform the tuning resolution now. That may typically use EvalRatio and
  // ThresholdRatio, but an implementation may use a different approach instead.
  Tuning ResolveNow(CpuInfo* cpuinfo);

  // The tuning as specified by the user, before actual resolution happens
  // i.e. before querying any specifics of the current CPU.
  // The default value kAuto means try to auto-detect. Other values mean
  // bypass auto-detect, use explicit value instead. See SetTuning().
  Tuning unresolved_tuning_ = Tuning::kAuto;
  // Cached last resolved tuning.
  Tuning last_resolved_tuning_ = Tuning::kAuto;
  // Timepoint of cached last resolved tuning, for invalidation purposes.
  TimePoint last_resolved_timepoint_;
  // Cached last resolved tunings that are older than this age are invalid.
  const Duration expiry_duration_;
};

}  // namespace ruy

#endif  // RUY_RUY_TUNE_H_

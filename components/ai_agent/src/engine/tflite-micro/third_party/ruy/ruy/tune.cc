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

#include "ruy/tune.h"

#include <algorithm>
#include <cstdint>

#include "ruy/cpuinfo.h"

namespace ruy {

Tuning TuningResolver::ResolveNow(CpuInfo* cpuinfo) {
  if (cpuinfo->CurrentCpuIsA55ish()) {
    return Tuning::kA55ish;
  }
  if (cpuinfo->CurrentCpuIsX1()) {
    return Tuning::kX1;
  }
  return Tuning::kGeneric;
}

TuningResolver::TuningResolver()
    : expiry_duration_(DurationFromMilliseconds(250)) {}

Tuning TuningResolver::Resolve(CpuInfo* cpuinfo) {
  if (unresolved_tuning_ != Tuning::kAuto) {
    return unresolved_tuning_;
  }
  TimePoint new_timepoint = CoarseNow();
  if (last_resolved_tuning_ != Tuning::kAuto &&
      (new_timepoint - last_resolved_timepoint_) < expiry_duration_) {
    return last_resolved_tuning_;
  }
  last_resolved_timepoint_ = new_timepoint;
  last_resolved_tuning_ = ResolveNow(cpuinfo);
  return last_resolved_tuning_;
}

}  // namespace ruy

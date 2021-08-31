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

#ifndef RUY_RUY_PERFORMANCE_ADVISORY_H_
#define RUY_RUY_PERFORMANCE_ADVISORY_H_

namespace ruy {

enum class PerformanceAdvisory {
  kNone = 0,
  kReallocatedPerChannelBuffer = 0x1
};

inline constexpr PerformanceAdvisory operator|(PerformanceAdvisory p,
                                               PerformanceAdvisory q) {
  return static_cast<PerformanceAdvisory>(static_cast<int>(p) |
                                          static_cast<int>(q));
}

inline constexpr PerformanceAdvisory operator&(PerformanceAdvisory p,
                                               PerformanceAdvisory q) {
  return static_cast<PerformanceAdvisory>(static_cast<int>(p) &
                                          static_cast<int>(q));
}

}  // namespace ruy

#endif  // RUY_RUY_PERFORMANCE_ADVISORY_H_

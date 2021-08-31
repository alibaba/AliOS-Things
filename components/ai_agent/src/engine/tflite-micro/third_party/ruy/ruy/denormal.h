/* Copyright 2021 Google LLC. All Rights Reserved.

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
#ifndef RUY_RUY_DENORMAL_H_
#define RUY_RUY_DENORMAL_H_

#include <cstdint>

namespace ruy {
// NOTE: the following 'fpu_state' struct is copied from
// pthreadpool/src/threadpool-utils.h that's not exposed by the pthreadpool
// library (https://github.com/Maratyszcza/pthreadpool).
struct fpu_state {
#if defined(__SSE__) || defined(__x86_64__) || defined(_M_X64) || \
    (defined(_M_IX86_FP) && _M_IX86_FP >= 1)
  std::uint32_t mxcsr;
#elif defined(__GNUC__) && defined(__arm__) && defined(__ARM_FP) && \
        (__ARM_FP != 0) ||                                          \
    defined(_MSC_VER) && defined(_M_ARM)
  std::uint32_t fpscr;
#elif defined(__GNUC__) && defined(__aarch64__) || \
    defined(_MSC_VER) && defined(_M_ARM64)
  std::uint64_t fpcr;
#endif
};

// While this class is active, denormal floating point numbers are suppressed.
// The destructor restores the original flags.
class ScopedSuppressDenormals {
 public:
  ScopedSuppressDenormals();
  ~ScopedSuppressDenormals();

 private:
  fpu_state restore_;

  ScopedSuppressDenormals(const ScopedSuppressDenormals&) = delete;
  void operator=(const ScopedSuppressDenormals&) = delete;
};
}  // namespace ruy

#endif  // RUY_RUY_DENORMAL_H_

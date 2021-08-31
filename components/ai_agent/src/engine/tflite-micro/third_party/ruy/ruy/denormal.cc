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

#include "ruy/denormal.h"

// NOTE: this is simply a copy of pthreadpool/src/threadpool-utils.h that's not
// exposed by the pthreadpool library
// (https://github.com/Maratyszcza/pthreadpool), but with an additional C++
// helper class to suppress floating-point denormal values.

/* SSE-specific headers */
#if defined(__SSE__) || defined(__x86_64__) || defined(_M_X64) || \
    (defined(_M_IX86_FP) && _M_IX86_FP >= 1)
#include <xmmintrin.h>
#endif

/* MSVC-specific headers */
#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace ruy {
namespace {
inline struct fpu_state get_fpu_state() {
  struct fpu_state state = {};
#if defined(__SSE__) || defined(__x86_64__) || defined(_M_X64) || \
    (defined(_M_IX86_FP) && _M_IX86_FP >= 1)
  state.mxcsr = static_cast<std::uint32_t>(_mm_getcsr());
#elif defined(_MSC_VER) && defined(_M_ARM)
  state.fpscr =
      static_cast<std::uint32_t>(_MoveFromCoprocessor(10, 7, 1, 0, 0));
#elif defined(_MSC_VER) && defined(_M_ARM64)
  state.fpcr = static_cast<std::uint64_t>(_ReadStatusReg(0x5A20));
#elif defined(__GNUC__) && defined(__arm__) && defined(__ARM_FP) && \
    (__ARM_FP != 0)
  __asm__ __volatile__("VMRS %[fpscr], fpscr" : [fpscr] "=r"(state.fpscr));
#elif defined(__GNUC__) && defined(__aarch64__)
  __asm__ __volatile__("MRS %[fpcr], fpcr" : [fpcr] "=r"(state.fpcr));
#endif
  return state;
}

inline void set_fpu_state(const struct fpu_state state) {
#if defined(__SSE__) || defined(__x86_64__) || defined(_M_X64) || \
    (defined(_M_IX86_FP) && _M_IX86_FP >= 1)
  _mm_setcsr(static_cast<unsigned int>(state.mxcsr));
#elif defined(_MSC_VER) && defined(_M_ARM)
  _MoveToCoprocessor(static_cast<int>(state.fpscr, 10, 7, 1, 0, 0));
#elif defined(_MSC_VER) && defined(_M_ARM64)
  _WriteStatusReg(0x5A20, static_cast<__int64>(state.fpcr));
#elif defined(__GNUC__) && defined(__arm__) && defined(__ARM_FP) && \
    (__ARM_FP != 0)
  __asm__ __volatile__("VMSR fpscr, %[fpscr]" : : [fpscr] "r"(state.fpscr));
#elif defined(__GNUC__) && defined(__aarch64__)
  __asm__ __volatile__("MSR fpcr, %[fpcr]" : : [fpcr] "r"(state.fpcr));
#else
  (void)state;
#endif
}

inline void disable_fpu_denormals() {
#if defined(__SSE__) || defined(__x86_64__) || defined(_M_X64) || \
    (defined(_M_IX86_FP) && _M_IX86_FP >= 1)
  _mm_setcsr(_mm_getcsr() | 0x8040);
#elif defined(_MSC_VER) && defined(_M_ARM)
  int fpscr = _MoveFromCoprocessor(10, 7, 1, 0, 0);
  fpscr |= 0x1000000;
  _MoveToCoprocessor(fpscr, 10, 7, 1, 0, 0);
#elif defined(_MSC_VER) && defined(_M_ARM64)
  __int64 fpcr = _ReadStatusReg(0x5A20);
  fpcr |= 0x1080000;
  _WriteStatusReg(0x5A20, fpcr);
#elif defined(__GNUC__) && defined(__arm__) && defined(__ARM_FP) && \
    (__ARM_FP != 0)
  std::uint32_t fpscr;
#if defined(__thumb__) && !defined(__thumb2__)
  __asm__ __volatile__(
      "VMRS %[fpscr], fpscr\n"
      "ORRS %[fpscr], %[bitmask]\n"
      "VMSR fpscr, %[fpscr]\n"
      : [fpscr] "=l"(fpscr)
      : [bitmask] "l"(0x1000000)
      : "cc");
#else
  __asm__ __volatile__(
      "VMRS %[fpscr], fpscr\n"
      "ORR %[fpscr], #0x1000000\n"
      "VMSR fpscr, %[fpscr]\n"
      : [fpscr] "=r"(fpscr));
#endif
#elif defined(__GNUC__) && defined(__aarch64__)
  std::uint64_t fpcr;
  __asm__ __volatile__(
      "MRS %[fpcr], fpcr\n"
      "ORR %w[fpcr], %w[fpcr], 0x1000000\n"
      "ORR %w[fpcr], %w[fpcr], 0x80000\n"
      "MSR fpcr, %[fpcr]\n"
      : [fpcr] "=r"(fpcr));
#endif
}
}  // namespace

ScopedSuppressDenormals::ScopedSuppressDenormals() {
  restore_ = get_fpu_state();
  disable_fpu_denormals();
}

ScopedSuppressDenormals::~ScopedSuppressDenormals() { set_fpu_state(restore_); }
}  // namespace ruy

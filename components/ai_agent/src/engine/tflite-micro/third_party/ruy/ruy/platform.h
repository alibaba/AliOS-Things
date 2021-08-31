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

// Preprocessor platform check macros.
// Note that ruy_copts contains '-Wundef', which ensures that we get a compile
// error if these macros are mistyped or if they are used without having
// #included this header.

#ifndef RUY_RUY_PLATFORM_H_
#define RUY_RUY_PLATFORM_H_

#ifdef __ANDROID_NDK__
#include <android/ndk-version.h>
#endif

// Detect APPLE.
#ifdef __APPLE__
#define RUY_PLATFORM_APPLE 1
#else
#define RUY_PLATFORM_APPLE 0
#endif

// Detect APPLE.
#if defined(__ppc__) || defined(__powerpc__)
#define RUY_PLATFORM_PPC 1
#else
#define RUY_PLATFORM_PPC 0
#endif

// Detect Fuchsia
#ifdef __Fuchsia__
#define RUY_PLATFORM_FUCHSIA 1
#else
#define RUY_PLATFORM_FUCHSIA 0
#endif

// Architecture-level platform detection.
//
// Ruy requires these to be mutually exclusive.

// Detect x86.
#if defined(__x86_64__) || defined(__i386__) || defined(__i386) || \
    defined(__x86__) || defined(__X86__) || defined(_X86_) ||      \
    defined(_M_IX86) || defined(_M_X64)
#define RUY_PLATFORM_X86 1
#else
#define RUY_PLATFORM_X86 0
#endif

// Detect ARM 32-bit.
#ifdef __arm__
#define RUY_PLATFORM_ARM_32 1
#else
#define RUY_PLATFORM_ARM_32 0
#endif

// Detect ARM 64-bit.
#ifdef __aarch64__
#define RUY_PLATFORM_ARM_64 1
#else
#define RUY_PLATFORM_ARM_64 0
#endif

// Combined ARM.
#define RUY_PLATFORM_ARM (RUY_PLATFORM_ARM_64 || RUY_PLATFORM_ARM_32)

// Feature and capability platform detection.
//
// These are mostly sub-selections of architectures.

// Detect NEON. Explicitly avoid emulation, or anything like it, on x86.
#if (defined(__ARM_NEON) || defined(__ARM_NEON__)) && !RUY_PLATFORM_X86
#define RUY_PLATFORM_NEON 1
#else
#define RUY_PLATFORM_NEON 0
#endif

// Define ARM 32-bit NEON.
#define RUY_PLATFORM_NEON_32 (RUY_PLATFORM_NEON && RUY_PLATFORM_ARM_32)

// Define ARM 64-bit NEON.
// Note: NEON is implied by ARM64, so this define is redundant.
// It still allows some conveyance of intent.
#define RUY_PLATFORM_NEON_64 (RUY_PLATFORM_NEON && RUY_PLATFORM_ARM_64)

// Determine whether to enable X86 non-portable performance improvements,
// typically x86 SIMD paths (AVX, etc).
#if defined(RUY_FORCE_ENABLE_X86_ENHANCEMENTS)
#define RUY_PLATFORM_X86_ENHANCEMENTS 1
#elif defined(__EMSCRIPTEN__)
// We use some x86 asm e.g. in runtime CPU detection and to implement missing
// intrinsics. This can't build to Emscripten.
#define RUY_PLATFORM_X86_ENHANCEMENTS 0
#elif defined(__ANDROID_NDK__) && defined(__NDK_MAJOR__) && \
    (__NDK_MAJOR__ >= 20)
// Enable on sufficiently recent Android NDK. Earlier versions had broken
// intrinsics headers.
#define RUY_PLATFORM_X86_ENHANCEMENTS 1
#elif defined(__linux__) && defined(__clang__) && (__clang_major__ >= 8)
// Enable on recent versions of Clang on Linux. Might be possible
// to relax this version requirement.
// Not enabling on Apple at the moment because b/138922878, see comment #8, we
// may only need to disable this on XCode <= 10.2.
#define RUY_PLATFORM_X86_ENHANCEMENTS 1
#elif defined(__GNUC__) && (__GNUC__ >= 9)
// Enable on recent versions of GCC. Might be possible
// to relax this version requirement.
#define RUY_PLATFORM_X86_ENHANCEMENTS 1
// Things are working on MSVC 2019. This should also enable on sufficiently
// recent Clang-CL.
#elif defined(_MSC_VER) && (_MSC_VER >= 1920)
#define RUY_PLATFORM_X86_ENHANCEMENTS 1
#else
#define RUY_PLATFORM_X86_ENHANCEMENTS 0
#endif

// These CPU capabilities will all be true when Skylake, etc, are enabled during
// compilation.
#if RUY_PLATFORM_X86_ENHANCEMENTS && RUY_PLATFORM_X86 &&                      \
    defined(__AVX512F__) && defined(__AVX512DQ__) && defined(__AVX512CD__) && \
    defined(__AVX512BW__) && defined(__AVX512VL__)
#define RUY_PLATFORM_AVX512 1
#else
#define RUY_PLATFORM_AVX512 0
#endif

#if RUY_PLATFORM_X86_ENHANCEMENTS && RUY_PLATFORM_X86 && defined(__AVX2__) && \
    (defined(__FMA__) || defined(_MSC_VER))
#define RUY_PLATFORM_AVX2_FMA 1
#else
#define RUY_PLATFORM_AVX2_FMA 0
#endif

#if RUY_PLATFORM_X86_ENHANCEMENTS && RUY_PLATFORM_X86 && defined(__AVX__)
#define RUY_PLATFORM_AVX 1
#else
#define RUY_PLATFORM_AVX 0
#endif

// Detect Emscripten, typically Wasm.
#ifdef __EMSCRIPTEN__
#define RUY_PLATFORM_EMSCRIPTEN 1
#else
#define RUY_PLATFORM_EMSCRIPTEN 0
#endif

#endif  // RUY_RUY_PLATFORM_H_

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

#ifndef RUY_RUY_PATH_H_
#define RUY_RUY_PATH_H_

#include <cstdint>

#include "ruy/platform.h"
#include "ruy/size_util.h"

namespace ruy {

// A Path is an implementation path, typically corresponding to a SIMD
// instruction set being targetted. For example, on the ARM architecture,
// Path::kNeon means using NEON instructions, and Path::kNeonDotprod means
// also using the newer NEON dot-product instructions.
//
// Different Path enum values are defined on different CPU architectures,
// corresponding to different SIMD ISA extensions available there.
//
// Path::kStandardCpp is the one Path that is always available.
//
// Path enum values are bits and may be OR-ed to form "sets of Paths".
// Ruy entry points such as ruy::Mul either implicitly use such a set of Paths,
// or allow passing an explicit one as a template parameter. The meaning of such
// an OR-ed Path combination is "compile all of
// these paths; which path is used will be determined at runtime". This is why
// for most users, it is enough to call ruy::Mul(...), which will compile a
// reasonable selection of paths for the target CPU architecture's various
// SIMD ISA extensions, and let ruy determine at runtime which one to use.
// Internally, after the actual path has been resolved, ruy's internal functions
// templatized on a Path tend to require that to be a single bit.
//
// An element of ruy's internal design was to allow for code compiled for
// multiple such paths to coexist without violating the C++ One Definition Rule
// (ODR). This is achieved by having all ruy internal functions, whose
// definition depends on a choice of Path, be templatized on a Path, so that
// each path-specific specialization is a separate symbol. There is never
// a need to compile ruy code with different compilation flags to enable
// different SIMD extensions and dispatch at runtime between them, as this is
// taken care of internally by ruy in an ODR-correct way.
enum class Path : std::uint8_t {
  // This is a special null value, representing the absence of any path.
  kNone = 0,
  // Standard C++ implementation of Ruy's architecture-specific parts.
  //
  // This is intended for testing/development, and as a fallback for when
  // the SIMD ISA extensions required by other paths are unavailable at runtime.
  kStandardCpp = 0x1,
  // Internal, test-only variants of StandardCpp used to exercise more corners
  // of internal ruy logic.
  // They are intentionally omitted from ruy::kAllPaths and ruy::kNonArchPaths,
  // and are only ever used in dedicated ruy tests explicitly referencing them.
  kInternalStandardCppVariant1 = 0x2,
  kInternalStandardCppVariant2 = 0x4,
  kInternalStandardCppVariant3 = 0x8,

#if RUY_PLATFORM_ARM
  // Optimized path using a widely available subset of ARM NEON instructions.
  kNeon = 0x10,
  // Optimized path making use of ARM NEON dot product instructions that are
  // available on newer ARM cores.
  kNeonDotprod = 0x20,
#endif  // RUY_PLATFORM_ARM

#if RUY_PLATFORM_X86
  // Optimized for AVX
  // Compiled with -mavx
  kAvx = 0x10,
  // Optimized for AVX2+FMA.
  // Compiled with -mavx2 -mfma.
  kAvx2Fma = 0x20,
  // Optimized for AVX-512.
  // Compiled with -mavx512f -mavx512vl -mavx512cd -mavx512bw -mavx512dq.
  kAvx512 = 0x40,
#endif  // RUY_PLATFORM_X86
};

inline constexpr Path operator|(Path p, Path q) {
  return static_cast<Path>(static_cast<std::uint32_t>(p) |
                           static_cast<std::uint32_t>(q));
}

inline constexpr Path operator&(Path p, Path q) {
  return static_cast<Path>(static_cast<std::uint32_t>(p) &
                           static_cast<std::uint32_t>(q));
}

inline constexpr Path operator^(Path p, Path q) {
  return static_cast<Path>(static_cast<std::uint32_t>(p) ^
                           static_cast<std::uint32_t>(q));
}

inline constexpr Path operator~(Path p) {
  return static_cast<Path>(~static_cast<std::uint32_t>(p));
}

inline constexpr bool Disjoint(Path p, Path q) {
  return (p & q) == Path::kNone;
}

inline Path GetMostSignificantPath(Path path_mask) {
  return static_cast<Path>(round_down_pot(static_cast<int>(path_mask)));
}

// We define three disjoint sets of paths.
//
// kNonArchPaths is the set of paths that are defined regardless of
// the CPU architecture (excluding some internal test-only paths).
// These paths are slow, but portable. At the moment,
// that is only kStandardCpp. In the past, that used to also include a
// kReference path providing an even more basic implementation, but that has
// been split out into a separate library, see the ReferenceMul function.
constexpr Path kNonArchPaths = Path::kStandardCpp;

// The other two are specific to each CPU architecture. Note that these sets
// do NOT include a fallback for when none of these architecture paths are
// supported at runtime by the CPU. For that, see the other constants defined
// further below.
//
// kDefaultArchPaths is the set of architecture-specific paths that
// we recommend for most users. It is part of kDefaultPaths defined
// below.
//
// kExtraArchPaths is the set of all other architecture-specific paths
// that for whatever reason we're not recommending to most users at the moment.
// Typically that would include work-in-progress paths, or paths targeting
// minority hardware that isn't the best compromise of code size to performance
// for most users.

#if RUY_PLATFORM_NEON_64
constexpr Path kDefaultArchPaths = Path::kNeon | Path::kNeonDotprod;
constexpr Path kExtraArchPaths = Path::kNone;
#elif RUY_PLATFORM_NEON_32
constexpr Path kDefaultArchPaths = Path::kNeon;
constexpr Path kExtraArchPaths = Path::kNone;
#elif RUY_PLATFORM_X86
constexpr Path kDefaultArchPaths = Path::kAvx | Path::kAvx2Fma | Path::kAvx512;
constexpr Path kExtraArchPaths = Path::kNone;
#else
constexpr Path kDefaultArchPaths = Path::kNone;
constexpr Path kExtraArchPaths = Path::kNone;
#endif

// kNonArchPathsIncludingInternalVariants is the set of all
// non-architecture-specific paths without exception. This includes some paths
// that are internal-only and test-only and not useful to any user.
static constexpr Path kNonArchPathsIncludingInternalVariants =
    kNonArchPaths | Path::kInternalStandardCppVariant1 |
    Path::kInternalStandardCppVariant2 | Path::kInternalStandardCppVariant3;

// Enforce that kDefaultArchPaths, kExtraArchPaths and
// kNonArchPathsIncludingInternalVariants are mutually disjoint,
// and that kNonArchPaths is a subset of kNonArchPathsIncludingInternalVariants.
static_assert(Disjoint(kDefaultArchPaths, kExtraArchPaths), "");
static_assert(Disjoint(kDefaultArchPaths,
                       kNonArchPathsIncludingInternalVariants),
              "");
static_assert(Disjoint(kExtraArchPaths, kNonArchPathsIncludingInternalVariants),
              "");
static_assert(Disjoint(kNonArchPaths, ~kNonArchPathsIncludingInternalVariants),
              "");

// We now define two aggregate sets of paths for convenience, including
// both architecture-specific paths and some portable fallbacks.
//
// kDefaultPaths is the set of paths that we recommend most users to use.
// It is what ruy::Mul(...), the entry point not taking an explicit Path value,
// uses.
constexpr Path kDefaultPaths = Path::kStandardCpp | kDefaultArchPaths;

// kAllPaths is the set of all paths that are available to compile, except
// some some internal test-only paths that no user would ever want to use.
// In addition to the Default paths, it also includes the extra
// architecture paths, as well as any other non-arch path besides kStandardCpp
// (there is none at the moment).
constexpr Path kAllPaths = kNonArchPaths | kDefaultArchPaths | kExtraArchPaths;

// kAllPathsIncludingInternalVariants is the set of all paths without exception.
// This includes some paths that are internal-only and test-only and not useful
// to any user.
static constexpr Path kAllPathsIncludingInternalVariants =
    kAllPaths | kNonArchPathsIncludingInternalVariants;

static_assert(Disjoint(kDefaultPaths, ~kAllPaths), "");
static_assert(Disjoint(kAllPaths, ~kAllPathsIncludingInternalVariants), "");

}  // namespace ruy

#endif  // RUY_RUY_PATH_H_

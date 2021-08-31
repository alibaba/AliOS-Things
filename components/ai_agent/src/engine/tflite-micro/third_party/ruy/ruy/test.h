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

#ifndef RUY_RUY_TEST_H_
#define RUY_RUY_TEST_H_

#include <math.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "ruy/allocator.h"
#include "ruy/context.h"
#include "ruy/context_get_ctx.h"
#include "ruy/ctx.h"
#include "ruy/gtest_wrapper.h"  // IWYU pragma: export
#include "ruy/matrix.h"         // IWYU pragma: export
#include "ruy/mul_params.h"     // IWYU pragma: export
#include "ruy/pack_common.h"
#include "ruy/platform.h"
#include "ruy/pmu.h"
#include "ruy/reference_mul.h"
#include "ruy/ruy.h"
#include "ruy/size_util.h"
#include "ruy/time.h"

#ifdef RUY_TEST_EXTERNAL_PATHS
#define EIGEN_USE_THREADS
#define EIGEN_USE_CUSTOM_THREAD_POOL
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "third_party/eigen3/Eigen/Core"
#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"
#pragma GCC diagnostic pop
#include "third_party/gemmlowp/public/gemmlowp.h"
#include "third_party/lapack/blas.h"
#endif

#ifdef RUY_PROFILER
#include "ruy/profiler/profiler.h"
#endif

#ifdef __linux__
#include <sys/mman.h>
#include <unistd.h>
#endif

namespace ruy {

const float kClampRatio = 0.1f;

enum class ExternalPath {
  kNone,
  kReference,
  kGemmlowp,
  kEigen,
  kEigenTensor,
  kOpenBlas
};

inline std::vector<std::string>* CoveredPaths() {
  static std::vector<std::string> covered_paths;
  return &covered_paths;
}

inline const char* PathName(Path path) {
#define RUY_PATHNAME_CASE(NAME) \
  case Path::NAME:              \
    return #NAME;
  switch (path) {
    RUY_PATHNAME_CASE(kStandardCpp)
    RUY_PATHNAME_CASE(kInternalStandardCppVariant1)
    RUY_PATHNAME_CASE(kInternalStandardCppVariant2)
    RUY_PATHNAME_CASE(kInternalStandardCppVariant3)

#if RUY_PLATFORM_NEON
    RUY_PATHNAME_CASE(kNeon)
    RUY_PATHNAME_CASE(kNeonDotprod)
#elif RUY_PLATFORM_X86
    RUY_PATHNAME_CASE(kAvx2Fma)
    RUY_PATHNAME_CASE(kAvx512)
    RUY_PATHNAME_CASE(kAvx)
#endif
    default:
      RUY_CHECK(false);
      return nullptr;
  }
#undef RUY_PATHNAME_CASE
}

inline const char* TuningName(Tuning tuning) {
#define RUY_SUBPATHNAME_CASE(NAME) \
  case Tuning::NAME:               \
    return #NAME;
  switch (tuning) {
    RUY_SUBPATHNAME_CASE(kA55ish)
    RUY_SUBPATHNAME_CASE(kGeneric)
    default:
      RUY_CHECK(false);
      return nullptr;
  }
#undef RUY_SUBPATHNAME_CASE
}

inline const char* PathName(ExternalPath path) {
#define RUY_PATHNAME_CASE(NAME) \
  case ExternalPath::NAME:      \
    return #NAME;
  switch (path) {
    RUY_PATHNAME_CASE(kReference)
    RUY_PATHNAME_CASE(kGemmlowp)
    RUY_PATHNAME_CASE(kEigen)
    RUY_PATHNAME_CASE(kEigenTensor)
    RUY_PATHNAME_CASE(kOpenBlas)
    default:
      RUY_CHECK(false);
      return nullptr;
  }
#undef RUY_PATHNAME_CASE
}

inline std::ostream& operator<<(std::ostream& stream, Path path) {
  return stream << PathName(path);
}

inline std::ostream& operator<<(std::ostream& stream,
                                ExternalPath external_path) {
  return stream << PathName(external_path);
}

template <typename ContainerType>
std::string Join(const ContainerType& container) {
  if (container.empty()) {
    return "<empty>";
  }
  std::ostringstream stream;
  auto it = container.begin();
  stream << *it++;
  for (; it != container.end(); ++it) {
    stream << ", ";
    stream << *it;
  }
  return stream.str();
}

struct LogCoveredPathsOnDestruction final {
  ~LogCoveredPathsOnDestruction() {
    std::cerr << "Covered paths: " << Join(*CoveredPaths()) << std::endl;

    // When we know that it would be abnormal for some path not to be covered,
    // we check it here. Accidentally disabling SIMD paths has occurred in the
    // past and is one of the biggest performance regressions imaginable.
    //
    // TODO: we should be able to require some x86 paths as well, at least
    // SSE4.2.

#if RUY_PLATFORM_ARM
    // When testing on ARM32 or ARM64, make sure that we covered the NEON path.
    // NEON is always available on ARM64, and we treat it as always available
    // also on ARM32.
    bool found_neon = false;
    for (const std::string& covered_path : *CoveredPaths()) {
      if (covered_path == "kNeon") {
        found_neon = true;
      }
    }
    if (!found_neon) {
      std::cerr
          << "Error: we haven't tested the kNeon path as we should have.\n"
          << std::endl;
      abort();
    }
#endif

    // When testing on ARM64 ChromiumOS emulator, make sure that we covered
    // the dotprod path. We're getting such coverage at the moment thanks to
    // using a sufficiently recent emulator, and we don't want to regress that.
#if RUY_PLATFORM_ARM_64 && defined RUY_TESTING_ON_CHROMIUMOS
    bool found_dotprod = false;
    for (const std::string& covered_path : *CoveredPaths()) {
      if (covered_path == "kNeonDotprod") {
        found_dotprod = true;
      }
    }
    if (!found_dotprod) {
      std::cerr
          << "Error: we haven't tested the kNeonDotprod path as we should "
             "have. At the moment, this is required on ChromiumOS as this is "
             "what we run emulator tests in, that currently supports "
             "dot-product "
             "instructions, and we care very much about not regressing that. "
             "If this test was run in an emulator, please upgrade to a newer "
             "emulator version. If this test was run on an actual device, and "
             "you need to be able to run ruy tests on devices not supporting "
             "dot-product instructions, get in touch with us.\n"
          << std::endl;
      abort();
    }
#endif
  }
  static void Singleton() { static LogCoveredPathsOnDestruction singleton; }
};

enum class RandomRange {
  kGeneral,
  kAvoidMinValue,
  kOffCenterAvoidMinValue,
  kReasonableSrcZeroPoint,
  kReasonableDstZeroPoint,
  kBias
};

template <typename Scalar,
          bool IsFloatingPoint = std::is_floating_point<Scalar>::value>
struct RandomRangeBounds {};

template <typename Scalar>
struct RandomRangeBounds<Scalar, true> {
  static Scalar GetMinBound(RandomRange range) {
    switch (range) {
      case RandomRange::kGeneral:
        return -1;
      case RandomRange::kAvoidMinValue:
        return -1;
      case RandomRange::kOffCenterAvoidMinValue:
        return -1;
      case RandomRange::kReasonableSrcZeroPoint:
        return 0;
      case RandomRange::kReasonableDstZeroPoint:
        return 0;
      case RandomRange::kBias:
        return -1;
      default:
        RUY_CHECK(false);
        return 0;
    }
  }
  static Scalar GetMaxBound(RandomRange range) {
    switch (range) {
      case RandomRange::kGeneral:
        return 1;
      case RandomRange::kAvoidMinValue:
        return 1;
      case RandomRange::kOffCenterAvoidMinValue:
        return 1;
      case RandomRange::kReasonableSrcZeroPoint:
        return 0;
      case RandomRange::kReasonableDstZeroPoint:
        return 0;
      case RandomRange::kBias:
        return 1;
      default:
        RUY_CHECK(false);
        return 0;
    }
  }
};

template <typename Scalar>
Scalar WeightedSum(Scalar s1, float weight1, Scalar s2, float weight2) {
  float sum = s1 * weight1 + s2 * weight2;
  float clamped = std::min<float>(
      std::numeric_limits<Scalar>::max(),
      std::max<float>(std::numeric_limits<Scalar>::lowest(), sum));
  return static_cast<Scalar>(clamped);
}

template <typename Scalar>
Scalar Parametrized(float param) {
  return WeightedSum(std::numeric_limits<Scalar>::max(), param,
                     std::numeric_limits<Scalar>::lowest(), 1 - param);
}

template <typename Scalar>
struct RandomRangeBounds<Scalar, false> {
  static Scalar GetMinBound(RandomRange range) {
    static constexpr double offcenteredness =
        0.02;  // Shift lower limit by about 5 for range of 255.
    switch (range) {
      case RandomRange::kGeneral:
        return std::numeric_limits<Scalar>::lowest();
      case RandomRange::kAvoidMinValue:
        return 1 + std::numeric_limits<Scalar>::lowest();
      case RandomRange::kOffCenterAvoidMinValue:
        return 1 + std::numeric_limits<Scalar>::lowest() +
               static_cast<Scalar>(
                   offcenteredness * std::numeric_limits<Scalar>::max() -
                   offcenteredness *
                       (std::numeric_limits<Scalar>::lowest() + 1));
      case RandomRange::kReasonableSrcZeroPoint:
        return std::numeric_limits<Scalar>::lowest();
      case RandomRange::kReasonableDstZeroPoint:
        return Parametrized<Scalar>(0.4);
      case RandomRange::kBias:
        return std::is_same<Scalar, std::int32_t>::value
                   ? static_cast<Scalar>(-10000)
                   : 0;
      default:
        RUY_CHECK(false);
        return 0;
    }
  }
  static Scalar GetMaxBound(RandomRange range) {
    switch (range) {
      case RandomRange::kGeneral:
        return std::numeric_limits<Scalar>::max();
      case RandomRange::kAvoidMinValue:
        return std::numeric_limits<Scalar>::max();
      case RandomRange::kOffCenterAvoidMinValue:
        return std::numeric_limits<Scalar>::max();
      case RandomRange::kReasonableSrcZeroPoint:
        return std::numeric_limits<Scalar>::max();
      case RandomRange::kReasonableDstZeroPoint:
        return Parametrized<Scalar>(0.6);
      case RandomRange::kBias:
        return std::is_same<Scalar, std::int32_t>::value
                   ? static_cast<Scalar>(10000)
                   : 0;
      default:
        RUY_CHECK(false);
        return 0;
    }
  }
};

inline std::default_random_engine& global_random_engine() {
  static std::default_random_engine engine;
  return engine;
}

template <typename Scalar>
struct UniformRandomDistribution {
  UniformRandomDistribution(RandomRange range)
      : dist(RandomRangeBounds<Scalar>::GetMinBound(range),
             RandomRangeBounds<Scalar>::GetMaxBound(range)) {}
  Scalar Get() { return dist(global_random_engine()); }
  // std::uniform_int_distribution is specified not to support char types,
  // only short and wider types. MSVC actually generates an error on
  // std::uniform_int_distribution<std::int8_t>.
  using StdDistType = typename std::conditional<
      std::is_floating_point<Scalar>::value,
      std::uniform_real_distribution<Scalar>,
      std::uniform_int_distribution<std::int32_t>>::type;
  StdDistType dist;
};

template <typename Scalar>
void MakeRandomScalar(UniformRandomDistribution<Scalar>* uniform_dist,
                      Scalar* dst) {
  *dst = uniform_dist->Get();
}

#if defined(__has_feature)
#if __has_feature(address_sanitizer)
#define RUY_TEST_BUILT_WITH_ASAN
#endif
#endif

// Don't use separate mappings when building with Address Sanitizer, as the
// manual mappings could hide actual address errors from ASan (ASan can't see
// the actual buffer valid address range inside the manual mapping).
#if defined __linux__ && !defined RUY_TEST_BUILT_WITH_ASAN
#define RUY_TEST_USE_SEPARATE_MAPPINGS
#endif

template <typename T>
struct SeparateMappingAllocator {
  using value_type = T;

  T* allocate(std::size_t n) {
#ifdef RUY_TEST_USE_SEPARATE_MAPPINGS
    const std::size_t page_size = getpagesize();
    std::size_t buffer_size = n * sizeof(T);
    std::size_t rounded_buffer_size = round_up_pot(buffer_size, page_size);
    // We are going to map an additional page at the end of our buffer, then
    // unmap it, to ensure that our buffer's end is the last mapped byte, so as
    // to catch any overrun.
    void* mapping =
        mmap(nullptr, rounded_buffer_size + page_size, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    RUY_CHECK_NE(mapping, MAP_FAILED);
    int unmap_result =
        munmap(static_cast<char*>(mapping) + rounded_buffer_size, page_size);
    RUY_CHECK_EQ(unmap_result, 0);
    // Clearing bytes should be redundant since mmap has do do it already, but
    // it does not hurt and acts as an assertion checking that we got the above
    // mapping and unmapping right.
    std::memset(mapping, 0, rounded_buffer_size);
    // Compute the offset to make our buffer end at the last mapped byte.
    std::size_t buffer_offset = rounded_buffer_size - buffer_size;
    void* buffer =
        static_cast<void*>(static_cast<char*>(mapping) + buffer_offset);
    return static_cast<T*>(buffer);
#else
    T* ret = new T[n];
    std::memset(ret, 0, n * sizeof(T));
    return ret;
#endif
  }
  void deallocate(T* p, std::size_t n) {
#ifdef RUY_TEST_USE_SEPARATE_MAPPINGS
    // The mapped bytes are the buffer address range, rounded on both ends
    // to page boundary.
    const std::size_t page_size = getpagesize();
    std::size_t buffer_size = n * sizeof(T);
    std::size_t rounded_buffer_size = round_up_pot(buffer_size, page_size);
    std::uintptr_t p_addr = reinterpret_cast<std::uintptr_t>(p);
    void* mapping = reinterpret_cast<void*>(p_addr & ~(page_size - 1));
    int ret = munmap(mapping, rounded_buffer_size);
    RUY_CHECK_EQ(ret, 0);
#else
    (void)n;
    delete[] p;
#endif
  }
};

template <typename T>
using SeparateMappingVector = std::vector<T, SeparateMappingAllocator<T>>;

template <typename Scalar, typename Allocator>
void MakeRandomVector(UniformRandomDistribution<Scalar>* uniform_dist, int size,
                      std::vector<Scalar, Allocator>* dst) {
  dst->resize(size);
  for (auto& x : *dst) {
    MakeRandomScalar(uniform_dist, &x);
  }
}

template <typename Scalar>
void MakeRandomScalar(RandomRange range, Scalar* dst) {
  UniformRandomDistribution<Scalar> dist(range);
  *dst = dist.Get();
  if (range == RandomRange::kReasonableDstZeroPoint ||
      range == RandomRange::kReasonableSrcZeroPoint) {
    if (global_random_engine()() & 1) {
      *dst = SymmetricZeroPoint<Scalar>();
    }
  }
}

template <typename Scalar, typename Allocator>
void MakeRandomVector(RandomRange range, int size,
                      std::vector<Scalar, Allocator>* dst) {
  UniformRandomDistribution<Scalar> dist(range);
  dst->resize(size);
  for (auto& x : *dst) {
    MakeRandomScalar(&dist, &x);
  }
}

enum class LayoutStyle { kUnstridedLinear, kLinear };

inline void MakeLayout(int rows, int cols, Order order,
                       LayoutStyle layout_style, Layout* layout) {
  layout->set_rows(rows);
  layout->set_cols(cols);
  layout->set_order(order);

  const int min_stride = order == Order::kColMajor ? rows : cols;

  RUY_CHECK(layout_style == LayoutStyle::kUnstridedLinear ||
            layout_style == LayoutStyle::kLinear);
  if (layout_style == LayoutStyle::kUnstridedLinear) {
    layout->set_stride(min_stride);
  } else {
    layout->set_stride(min_stride + 1);
  }
}

template <typename Scalar>
struct StorageMatrix {
  StorageMatrix() = default;
  StorageMatrix(const StorageMatrix&) = delete;
  SeparateMappingVector<Scalar> data;
  Matrix<Scalar> matrix;
};

inline bool IsUnstrided(const Layout& layout) {
  if (layout.order() == Order::kColMajor) {
    return layout.stride() == layout.rows();
  } else {
    return layout.stride() == layout.cols();
  }
}

inline bool IsRowMajor(const Layout& layout) {
  return layout.order() == Order::kRowMajor;
}

inline bool IsColMajor(const Layout& layout) {
  return layout.order() == Order::kColMajor;
}

inline int FlatSize(const Layout& layout) {
  const int outerdim =
      layout.order() == Order::kColMajor ? layout.cols() : layout.rows();
  return layout.stride() * outerdim;
}

template <typename Scalar>
void VerifyConsistentFields(const StorageMatrix<Scalar>& storage_matrix) {
  if (storage_matrix.data.empty()) {
    RUY_CHECK_EQ(storage_matrix.matrix.data(), nullptr);
    RUY_CHECK_EQ(storage_matrix.matrix.layout().rows(), 0);
    RUY_CHECK_EQ(storage_matrix.matrix.layout().cols(), 0);
  } else {
    RUY_CHECK_EQ(storage_matrix.matrix.data(), storage_matrix.data.data());
    RUY_CHECK_EQ(FlatSize(storage_matrix.matrix.layout()),
                 static_cast<int>(storage_matrix.data.size()));
  }
}

template <typename Scalar>
void MakeRandom(int rows, int cols, Order order, Scalar zero_point,
                LayoutStyle layout_style, RandomRange range,
                StorageMatrix<Scalar>* storage_matrix) {
  MakeLayout(rows, cols, order, layout_style,
             storage_matrix->matrix.mutable_layout());
  storage_matrix->matrix.set_zero_point(zero_point);
  UniformRandomDistribution<Scalar> data_dist(range);
  MakeRandomVector(&data_dist, FlatSize(storage_matrix->matrix.layout()),
                   &storage_matrix->data);
  storage_matrix->matrix.set_data(storage_matrix->data.data());
  VerifyConsistentFields(*storage_matrix);
}

template <typename Scalar>
struct TestResult {
  void operator=(const TestResult&) = delete;
  void operator=(const TestResult&&) = delete;
  StorageMatrix<Scalar> storage_matrix;
  Path path = Path::kNone;
  Tuning tuning = Tuning::kAuto;
  ExternalPath external_path = ExternalPath::kNone;
  float latency;
  float l1_refill_rate;
  float l2_refill_rate;
  float l3_refill_rate;
  float l1tlb_refill_rate;
  float l2tlb_refill_rate;
  float mispred_rate;
  float frontend_stall_rate;
  float backend_stall_rate;
};

template <typename Scalar>
std::string PathName(const TestResult<Scalar>& result) {
  std::string pathname;
  if (result.path != Path::kNone) {
    pathname.assign(PathName(result.path));
  } else if (result.external_path != ExternalPath::kNone) {
    pathname.assign(PathName(result.external_path));
  } else {
    RUY_CHECK(false);
  }
  if (result.tuning != Tuning::kAuto) {
    pathname.append("/");
    pathname.append(TuningName(result.tuning));
  }
  return pathname;
}

template <typename tLhsScalar, typename tRhsScalar, typename tAccumScalar,
          typename tDstScalar>
struct TestSet final {
  using LhsScalar = tLhsScalar;
  using RhsScalar = tRhsScalar;
  using AccumScalar = tAccumScalar;
  using DstScalar = tDstScalar;
  using MulParamsType = MulParams<AccumScalar, DstScalar>;
  using TestResultType = TestResult<DstScalar>;

  void Run() {
    MakeZeroPoints();
    MakeLhsRhs();
    MakeMulParams();
    MakeOtherParams();
    MakeResultPaths();
    Eval();
    Verify();
  }

 private:
  void MakeZeroPoints();
  void MakeLhsRhs();
  void MakeMulParams();
  void MakeResultPaths();
  void MakeOtherParams();
  void EvalAndVerify();
  void Eval();
  void Verify();

  void EvalResult(TestResultType* result);
  void EvalRuy(TestResultType* result);
  void DoMul(TestResultType* result);
  void Benchmark(TestResultType* result);
  void VerifyTestResults() const;

 public:
  enum class LifeStage {
    kInitial,
    kHasZeroPoints,
    kHasLhsRhs,
    kHasMulParams,
    kHasOtherParams,
    kHasResultPaths,
    kEvaluated,
    kFinal
  };

  ~TestSet();

  LifeStage life_stage = LifeStage::kInitial;

  int rows = 0;
  int cols = 0;
  int depth = 0;
  Order lhs_order = Order::kRowMajor;
  Order rhs_order = Order::kColMajor;
  Order dst_order = Order::kColMajor;
  LayoutStyle layout_style = LayoutStyle::kUnstridedLinear;

  bool use_specified_zero_points = false;
  LhsScalar lhs_zero_point = 0;
  RhsScalar rhs_zero_point = 0;
  DstScalar dst_zero_point = 0;

  SeparateMappingVector<AccumScalar> per_channel_multiplier_fixedpoint;
  SeparateMappingVector<int> per_channel_multiplier_exponent;

  StorageMatrix<LhsScalar> lhs;
  StorageMatrix<RhsScalar> rhs;
  MulParamsType mul_params;
  SeparateMappingVector<AccumScalar> bias_data;
  std::vector<std::unique_ptr<TestResultType>> results;

  std::vector<Path> paths;
  std::vector<ExternalPath> external_paths;

  bool benchmark = false;
  bool perchannel = false;
  int max_num_threads = 0;

  bool cache_lhs = false;
  bool cache_rhs = false;
};

inline PmuEvents& GlobalPmuEvents() {
  static PmuEvents pmu;
  return pmu;
}

inline Context& GlobalContext() {
  // Ensure that GlobalPmuEvents is constructed before we create any context.
  // This ensures that pmu counters are opened before we create any worker
  // thread, which is necessary to count events from worker threads.
  GlobalPmuEvents();

  static Context context;
  return context;
}

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
TestSet<LhsScalar, RhsScalar, AccumScalar, DstScalar>::~TestSet() {
  RUY_CHECK_EQ(life_stage, LifeStage::kFinal);
  LogCoveredPathsOnDestruction::Singleton();
  GlobalContext().ClearPrepackedCache();
}

#if defined(__has_feature)
#if __has_feature(thread_sanitizer)
#define RUY_TSAN
#endif
#if __has_feature(address_sanitizer)
#define RUY_ASAN
#endif
#endif  // defined(__has_feature)

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void TestSet<LhsScalar, RhsScalar, AccumScalar, DstScalar>::DoMul(
    TestResultType* result) {
  Mul<kAllPathsIncludingInternalVariants>(lhs.matrix, rhs.matrix, mul_params,
                                          &GlobalContext(),
                                          &result->storage_matrix.matrix);
}

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void TestSet<LhsScalar, RhsScalar, AccumScalar, DstScalar>::EvalRuy(
    TestResultType* result) {
  GlobalContext().set_explicit_tuning(result->tuning);
  if (max_num_threads) {
    GlobalContext().set_max_num_threads(max_num_threads);
  } else if (benchmark) {
    GlobalContext().set_max_num_threads(1);
  } else {
    GlobalContext().set_max_num_threads(1 + global_random_engine()() % 8);
  }
  get_ctx(&GlobalContext())->SetRuntimeEnabledPaths(result->path);
  DoMul(result);
  // If enabling caching, Mul is stateful, so we run it a second time to get
  // coverage of these aspects.
  if (!benchmark && (cache_lhs || cache_rhs)) {
    DoMul(result);
  }
  RUY_CHECK_EQ(GlobalContext().last_used_path(), result->path);
  GlobalContext().set_explicit_tuning(Tuning::kAuto);
  GlobalContext().set_max_num_threads(1);
}

#ifdef RUY_TEST_EXTERNAL_PATHS

template <typename Scalar, gemmlowp::MapOrder tOrder>
void WrapGemmlowp(const Matrix<Scalar>& src,
                  gemmlowp::MatrixMap<const Scalar, tOrder>* dst) {
  RUY_CHECK(src.layout().order() == (tOrder == gemmlowp::MapOrder::ColMajor
                                         ? Order::kColMajor
                                         : Order::kRowMajor));
  *dst = gemmlowp::MatrixMap<const Scalar, tOrder>(
      src.data(), src.layout().rows(), src.layout().cols(),
      src.layout().stride());
}

template <typename Scalar, gemmlowp::MapOrder tOrder>
void WrapGemmlowpMutable(Matrix<Scalar>* src,
                         gemmlowp::MatrixMap<Scalar, tOrder>* dst) {
  RUY_CHECK(src->layout().order() == (tOrder == gemmlowp::MapOrder::ColMajor
                                          ? Order::kColMajor
                                          : Order::kRowMajor));
  *dst = gemmlowp::MatrixMap<Scalar, tOrder>(src->data(), src->layout().rows(),
                                             src->layout().cols(),
                                             src->layout().stride());
}

template <Order tOrder>
struct GemmlowpOrder {};

template <>
struct GemmlowpOrder<Order::kColMajor> {
  static constexpr gemmlowp::MapOrder kValue = gemmlowp::MapOrder::ColMajor;
};

template <>
struct GemmlowpOrder<Order::kRowMajor> {
  static constexpr gemmlowp::MapOrder kValue = gemmlowp::MapOrder::RowMajor;
};

inline gemmlowp::GemmContext& GlobalGemmlowpContext() {
  static gemmlowp::GemmContext context;
  return context;
}

template <Order LhsOrder, Order RhsOrder, Order DstOrder, typename LhsScalar,
          typename RhsScalar, typename DstScalar, typename MulParamsType>
void EvalGemmlowp(const Matrix<LhsScalar>& lhs, const Matrix<RhsScalar>& rhs,
                  const MulParamsType& mul_params, int max_num_threads,
                  Matrix<DstScalar>* dst) {
  static constexpr gemmlowp::MapOrder kGemmlowpLhsOrder =
      GemmlowpOrder<LhsOrder>::kValue;
  static constexpr gemmlowp::MapOrder kGemmlowpRhsOrder =
      GemmlowpOrder<RhsOrder>::kValue;
  static constexpr gemmlowp::MapOrder kGemmlowpDstOrder =
      GemmlowpOrder<DstOrder>::kValue;
  gemmlowp::MatrixMap<const LhsScalar, kGemmlowpLhsOrder> gemmlowp_lhs;
  gemmlowp::MatrixMap<const RhsScalar, kGemmlowpRhsOrder> gemmlowp_rhs;
  gemmlowp::MatrixMap<DstScalar, kGemmlowpDstOrder> gemmlowp_dst;
  WrapGemmlowp(lhs, &gemmlowp_lhs);
  WrapGemmlowp(rhs, &gemmlowp_rhs);
  WrapGemmlowpMutable(dst, &gemmlowp_dst);

  gemmlowp::OutputStageScaleInt32ByFixedPointAndExponent quantize_down_stage;
  quantize_down_stage.result_offset_after_shift = dst->zero_point();
  quantize_down_stage.result_fixedpoint_multiplier =
      mul_params.multiplier_fixedpoint();
  quantize_down_stage.result_exponent = mul_params.multiplier_exponent();
  gemmlowp::OutputStageScaleInt32ByFixedPointAndExponentPC<
      gemmlowp::VectorShape::Col>
      quantize_down_stage_pc;
  quantize_down_stage_pc.result_offset_after_shift = dst->zero_point();
  using ColVectorMap =
      gemmlowp::VectorMap<const std::int32_t, gemmlowp::VectorShape::Col>;
  quantize_down_stage_pc.result_fixedpoint_multiplier = ColVectorMap(
      mul_params.multiplier_fixedpoint_perchannel(), lhs.layout().rows());
  quantize_down_stage_pc.result_exponent = ColVectorMap(
      mul_params.multiplier_exponent_perchannel(), lhs.layout().rows());

  gemmlowp::OutputStageClamp clamp_stage;
  clamp_stage.min = mul_params.clamp_min();
  clamp_stage.max = mul_params.clamp_max();
  using OutputStageSaturatingCast = typename std::conditional<
      std::is_same<DstScalar, std::uint8_t>::value,
      gemmlowp::OutputStageSaturatingCastToUint8,
      gemmlowp::OutputStageSaturatingCastToInt16>::type;
  OutputStageSaturatingCast saturating_cast_stage;

  GlobalGemmlowpContext().set_max_num_threads(max_num_threads ? max_num_threads
                                                              : 1);
  if (mul_params.bias()) {
    using ColVectorMap =
        gemmlowp::VectorMap<const std::int32_t, gemmlowp::VectorShape::Col>;
    gemmlowp::OutputStageBiasAddition<ColVectorMap> bias_add_stage;
    bias_add_stage.bias_vector =
        ColVectorMap(mul_params.bias(), dst->layout().rows());
#ifndef GEMMLOWP_SSE4  // gemmlowp perchannel stuff does not build on SSE
    if (mul_params.multiplier_exponent_perchannel()) {
      const auto& output_pipeline =
          std::make_tuple(bias_add_stage, quantize_down_stage_pc, clamp_stage,
                          saturating_cast_stage);
      gemmlowp::GemmWithOutputPipeline<
          LhsScalar, DstScalar, gemmlowp::L8R8WithLhsNonzeroBitDepthParams>(
          &GlobalGemmlowpContext(), gemmlowp_lhs, gemmlowp_rhs, &gemmlowp_dst,
          -lhs.zero_point(), -rhs.zero_point(), output_pipeline);
    } else  // NOLINT[readability/braces]
#endif
    {
      const auto& output_pipeline =
          std::make_tuple(bias_add_stage, quantize_down_stage, clamp_stage,
                          saturating_cast_stage);
      gemmlowp::GemmWithOutputPipeline<
          LhsScalar, DstScalar, gemmlowp::L8R8WithLhsNonzeroBitDepthParams>(
          &GlobalGemmlowpContext(), gemmlowp_lhs, gemmlowp_rhs, &gemmlowp_dst,
          -lhs.zero_point(), -rhs.zero_point(), output_pipeline);
    }
  } else {
#ifndef GEMMLOWP_SSE4  // gemmlowp perchannel stuff does not build on SSE
    if (mul_params.multiplier_exponent_perchannel()) {
      const auto& output_pipeline = std::make_tuple(
          quantize_down_stage_pc, clamp_stage, saturating_cast_stage);
      gemmlowp::GemmWithOutputPipeline<
          LhsScalar, DstScalar, gemmlowp::L8R8WithLhsNonzeroBitDepthParams>(
          &GlobalGemmlowpContext(), gemmlowp_lhs, gemmlowp_rhs, &gemmlowp_dst,
          -lhs.zero_point(), -rhs.zero_point(), output_pipeline);
    } else  // NOLINT[readability/braces]
#endif
    {
      const auto& output_pipeline = std::make_tuple(
          quantize_down_stage, clamp_stage, saturating_cast_stage);
      gemmlowp::GemmWithOutputPipeline<
          LhsScalar, DstScalar, gemmlowp::L8R8WithLhsNonzeroBitDepthParams>(
          &GlobalGemmlowpContext(), gemmlowp_lhs, gemmlowp_rhs, &gemmlowp_dst,
          -lhs.zero_point(), -rhs.zero_point(), output_pipeline);
    }
  }
}

inline constexpr int Mash(Order LhsOrder, Order RhsOrder, Order DstOrder) {
  return (LhsOrder == Order::kRowMajor ? 4 : 0) +
         (RhsOrder == Order::kRowMajor ? 2 : 0) +
         (DstOrder == Order::kRowMajor ? 1 : 0);
}

template <typename LhsScalar, typename RhsScalar, typename DstScalar,
          typename MulParamsType>
void EvalGemmlowp(const Matrix<LhsScalar>& lhs, const Matrix<RhsScalar>& rhs,
                  const MulParamsType& mul_params, int max_num_threads,
                  Matrix<DstScalar>* dst) {
  int index =
      Mash(lhs.layout().order(), rhs.layout().order(), dst->layout().order());
  switch (index) {
#define EVALGEMMLOWP_CASE3(LHS, RHS, DST)                                     \
  case Mash(LHS, RHS, DST):                                                   \
    return EvalGemmlowp<LHS, RHS, DST>(lhs, rhs, mul_params, max_num_threads, \
                                       dst);
#define EVALGEMMLOWP_CASE2(LHS, RHS)             \
  EVALGEMMLOWP_CASE3(LHS, RHS, Order::kColMajor) \
  EVALGEMMLOWP_CASE3(LHS, RHS, Order::kRowMajor)
#define EVALGEMMLOWP_CASE1(LHS)             \
  EVALGEMMLOWP_CASE2(LHS, Order::kColMajor) \
  EVALGEMMLOWP_CASE2(LHS, Order::kRowMajor)

    EVALGEMMLOWP_CASE1(Order::kColMajor)
    EVALGEMMLOWP_CASE1(Order::kRowMajor)

#undef EVALGEMMLOWP_CASE1
#undef EVALGEMMLOWP_CASE2
#undef EVALGEMMLOWP_CASE3

    default:
      RUY_CHECK(false);
  }
}

template <Order tOrder>
struct EigenOrder {};

template <>
struct EigenOrder<Order::kColMajor> {
  static constexpr int kValue = Eigen::ColMajor;
};

template <>
struct EigenOrder<Order::kRowMajor> {
  static constexpr int kValue = Eigen::RowMajor;
};

template <Order LhsOrder, Order RhsOrder, Order DstOrder, typename LhsScalar,
          typename RhsScalar, typename DstScalar, typename MulParamsType>
void EvalEigen(const Matrix<LhsScalar>& lhs, const Matrix<RhsScalar>& rhs,
               const MulParamsType& mul_params, int max_num_threads,
               Matrix<DstScalar>* dst) {
  RUY_CHECK_EQ(lhs.zero_point(), 0);
  RUY_CHECK_EQ(rhs.zero_point(), 0);
  RUY_CHECK_EQ(dst->zero_point(), 0);
  RUY_CHECK_EQ(mul_params.multiplier_fixedpoint(), 0);
  RUY_CHECK_EQ(mul_params.multiplier_exponent(), 0);

  static constexpr int kEigenLhsOrder = EigenOrder<LhsOrder>::kValue;
  static constexpr int kEigenRhsOrder = EigenOrder<RhsOrder>::kValue;
  static constexpr int kEigenDstOrder = EigenOrder<DstOrder>::kValue;

  using EigenLhsType = typename Eigen::Matrix<LhsScalar, Eigen::Dynamic,
                                              Eigen::Dynamic, kEigenLhsOrder>::
      template StridedConstMapType<Eigen::OuterStride<Eigen::Dynamic>>::type;
  using EigenRhsType = typename Eigen::Matrix<RhsScalar, Eigen::Dynamic,
                                              Eigen::Dynamic, kEigenRhsOrder>::
      template StridedConstMapType<Eigen::OuterStride<Eigen::Dynamic>>::type;
  using EigenDstType = typename Eigen::Matrix<DstScalar, Eigen::Dynamic,
                                              Eigen::Dynamic, kEigenDstOrder>::
      template StridedMapType<Eigen::OuterStride<Eigen::Dynamic>>::type;
  using EigenBiasType =
      typename Eigen::Matrix<DstScalar, Eigen::Dynamic, 1>::ConstMapType;

  EigenLhsType eigen_lhs(
      lhs.data(), lhs.layout().rows(), lhs.layout().cols(),
      Eigen::OuterStride<Eigen::Dynamic>(lhs.layout().stride()));
  EigenRhsType eigen_rhs(
      rhs.data(), rhs.layout().rows(), rhs.layout().cols(),
      Eigen::OuterStride<Eigen::Dynamic>(rhs.layout().stride()));
  EigenDstType eigen_dst(
      dst->data(), dst->layout().rows(), dst->layout().cols(),
      Eigen::OuterStride<Eigen::Dynamic>(dst->layout().stride()));
  Eigen::setNbThreads(max_num_threads ? max_num_threads : 1);

  if (mul_params.bias()) {
    EigenBiasType eigen_bias(mul_params.bias(), dst->layout().rows());
    if (mul_params.clamp_max() == std::numeric_limits<DstScalar>::infinity() &&
        mul_params.clamp_min() == -std::numeric_limits<DstScalar>::infinity()) {
      eigen_dst.noalias() = (eigen_lhs * eigen_rhs).colwise() + eigen_bias;
    } else {
      eigen_dst.noalias() = ((eigen_lhs * eigen_rhs).colwise() + eigen_bias)
                                .cwiseMin(mul_params.clamp_max())
                                .cwiseMax(mul_params.clamp_min());
    }
  } else {
    if (mul_params.clamp_max() == std::numeric_limits<DstScalar>::infinity() &&
        mul_params.clamp_min() == -std::numeric_limits<DstScalar>::infinity()) {
      eigen_dst.noalias() = eigen_lhs * eigen_rhs;
    } else {
      eigen_dst.noalias() = (eigen_lhs * eigen_rhs)
                                .cwiseMin(mul_params.clamp_max())
                                .cwiseMax(mul_params.clamp_min());
    }
  }
}

template <typename LhsScalar, typename RhsScalar, typename DstScalar,
          typename MulParamsType>
void EvalEigen(const Matrix<LhsScalar>& lhs, const Matrix<RhsScalar>& rhs,
               const MulParamsType& mul_params, int max_num_threads,
               Matrix<DstScalar>* dst) {
  int index =
      Mash(lhs.layout().order(), rhs.layout().order(), dst->layout().order());
  switch (index) {
#define EVALEIGEN_CASE3(LHS, RHS, DST) \
  case Mash(LHS, RHS, DST):            \
    return EvalEigen<LHS, RHS, DST>(lhs, rhs, mul_params, max_num_threads, dst);
#define EVALEIGEN_CASE2(LHS, RHS)             \
  EVALEIGEN_CASE3(LHS, RHS, Order::kColMajor) \
  EVALEIGEN_CASE3(LHS, RHS, Order::kRowMajor)
#define EVALEIGEN_CASE1(LHS)             \
  EVALEIGEN_CASE2(LHS, Order::kColMajor) \
  EVALEIGEN_CASE2(LHS, Order::kRowMajor)

    EVALEIGEN_CASE1(Order::kColMajor)
    EVALEIGEN_CASE1(Order::kRowMajor)

#undef EVALEIGEN_CASE1
#undef EVALEIGEN_CASE2
#undef EVALEIGEN_CASE3

    default:
      RUY_CHECK(false);
  }
}

template <Order LhsOrder, Order RhsOrder, Order DstOrder, typename Scalar,
          typename MulParamsType>
void EvalEigenTensor(const Matrix<Scalar>& lhs, const Matrix<Scalar>& rhs,
                     const MulParamsType& mul_params, int max_num_threads,
                     Matrix<Scalar>* dst) {
  RUY_CHECK_EQ(lhs.zero_point(), 0);
  RUY_CHECK_EQ(rhs.zero_point(), 0);
  RUY_CHECK_EQ(dst->zero_point(), 0);
  RUY_CHECK_EQ(mul_params.multiplier_fixedpoint(), 0);
  RUY_CHECK_EQ(mul_params.multiplier_exponent(), 0);

  // Eigen::TensorMap only supports unstrided layouts
  RUY_CHECK(IsUnstrided(lhs.layout()));
  RUY_CHECK(IsUnstrided(rhs.layout()));
  RUY_CHECK(IsUnstrided(dst->layout()));

  using TensorLhsType =
      Eigen::TensorMap<Eigen::Tensor<const Scalar, 2, Eigen::ColMajor>>;
  using TensorRhsType =
      Eigen::TensorMap<Eigen::Tensor<const Scalar, 2, Eigen::ColMajor>>;
  using TensorDstType =
      Eigen::TensorMap<Eigen::Tensor<Scalar, 2, Eigen::ColMajor>>;
  using TensorBiasType =
      Eigen::TensorMap<Eigen::Tensor<const Scalar, 1, Eigen::ColMajor>>;

  const bool tr = DstOrder == Order::kRowMajor;
  const auto& contract_lhs = tr ? rhs : lhs;
  const auto& contract_rhs = tr ? lhs : rhs;

  TensorLhsType tensor_lhs(
      contract_lhs.data(),
      LhsOrder == Order::kColMajor ? contract_lhs.layout().rows()
                                   : contract_lhs.layout().cols(),
      LhsOrder == Order::kColMajor ? contract_lhs.layout().cols()
                                   : contract_lhs.layout().rows());
  TensorRhsType tensor_rhs(
      contract_rhs.data(),
      RhsOrder == Order::kColMajor ? contract_rhs.layout().rows()
                                   : contract_rhs.layout().cols(),
      RhsOrder == Order::kColMajor ? contract_rhs.layout().cols()
                                   : contract_rhs.layout().rows());
  TensorDstType tensor_dst(dst->data(),
                           DstOrder == Order::kColMajor ? dst->layout().rows()
                                                        : dst->layout().cols(),
                           DstOrder == Order::kColMajor ? dst->layout().cols()
                                                        : dst->layout().rows());
  using DimPair =
      typename Eigen::Tensor<Scalar, 1, 0, Eigen::Index>::DimensionPair;
  Eigen::array<DimPair, 1> contract_dims(
      {DimPair((LhsOrder == Order::kColMajor) ? 1 : 0,
               (RhsOrder == Order::kColMajor) ? 0 : 1)});
  Eigen::array<int, 2> shuffle(DstOrder == Order::kColMajor ? 0 : 1,
                               DstOrder == Order::kColMajor ? 1 : 0);
  static Eigen::ThreadPool pool(max_num_threads ? max_num_threads : 1);
  static Eigen::ThreadPoolDevice device(&pool, pool.NumThreads());
  if (mul_params.bias()) {
    TensorBiasType tensor_bias(mul_params.bias(), dst->layout().rows());
    Eigen::array<int, 2> bias_2d_shape(tr ? 1 : dst->layout().rows(),
                                       tr ? dst->layout().rows() : 1);
    Eigen::array<int, 2> bcast(tr ? dst->layout().cols() : 1,
                               tr ? 1 : dst->layout().cols());
    if (mul_params.clamp_max() == std::numeric_limits<Scalar>::infinity() &&
        mul_params.clamp_min() == -std::numeric_limits<Scalar>::infinity()) {
      tensor_dst.device(device) =
          tensor_lhs.contract(tensor_rhs, contract_dims);
    } else {
      tensor_dst.device(device) =
          (tensor_lhs.contract(tensor_rhs, contract_dims) +
           tensor_bias.reshape(bias_2d_shape).broadcast(bcast))
              .cwiseMin(mul_params.clamp_max())
              .cwiseMax(mul_params.clamp_min());
    }
  } else {
    if (mul_params.clamp_max() == std::numeric_limits<Scalar>::infinity() &&
        mul_params.clamp_min() == -std::numeric_limits<Scalar>::infinity()) {
      tensor_dst.device(device) =
          tensor_lhs.contract(tensor_rhs, contract_dims);
    } else {
      tensor_dst.device(device) = tensor_lhs.contract(tensor_rhs, contract_dims)
                                      .cwiseMin(mul_params.clamp_max())
                                      .cwiseMax(mul_params.clamp_min());
    }
  }
}

template <typename Scalar, typename MulParamsType>
void EvalEigenTensor(const Matrix<Scalar>& lhs, const Matrix<Scalar>& rhs,
                     const MulParamsType& mul_params, int max_num_threads,
                     Matrix<Scalar>* dst) {
  int index =
      Mash(lhs.layout().order(), rhs.layout().order(), dst->layout().order());
  switch (index) {
#define EVALEIGENTENSOR_CASE3(LHS, RHS, DST)                    \
  case Mash(LHS, RHS, DST):                                     \
    return EvalEigenTensor<LHS, RHS, DST>(lhs, rhs, mul_params, \
                                          max_num_threads, dst);
#define EVALEIGENTENSOR_CASE2(LHS, RHS)             \
  EVALEIGENTENSOR_CASE3(LHS, RHS, Order::kColMajor) \
  EVALEIGENTENSOR_CASE3(LHS, RHS, Order::kRowMajor)
#define EVALEIGENTENSOR_CASE1(LHS)             \
  EVALEIGENTENSOR_CASE2(LHS, Order::kColMajor) \
  EVALEIGENTENSOR_CASE2(LHS, Order::kRowMajor)

    EVALEIGENTENSOR_CASE1(Order::kColMajor)
    EVALEIGENTENSOR_CASE1(Order::kRowMajor)

#undef EVALEIGENTENSOR_CASE1
#undef EVALEIGENTENSOR_CASE2
#undef EVALEIGENTENSOR_CASE3

    default:
      RUY_CHECK(false);
  }
}

template <typename Scalar>
struct GenericBlasGemm {};

template <>
struct GenericBlasGemm<lapack::doublereal> {
  static void Run(char* transa, char* transb, lapack::integer* m,
                  lapack::integer* n, lapack::integer* k,
                  lapack::doublereal* alpha, lapack::doublereal* a,
                  lapack::integer* lda, lapack::doublereal* b,
                  lapack::integer* ldb, lapack::doublereal* beta,
                  lapack::doublereal* c, lapack::integer* ldc) {
    dgemm_(transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc);
  }
};

template <>
struct GenericBlasGemm<lapack::real> {
  static void Run(char* transa, char* transb, lapack::integer* m,
                  lapack::integer* n, lapack::integer* k, lapack::real* alpha,
                  lapack::real* a, lapack::integer* lda, lapack::real* b,
                  lapack::integer* ldb, lapack::real* beta, lapack::real* c,
                  lapack::integer* ldc) {
    sgemm_(transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc);
  }
};

inline void TransposeLayout(Layout* layout) {
  layout->set_order((layout->order() == Order::kRowMajor) ? Order::kColMajor
                                                          : Order::kRowMajor);
  int tmp_rows = layout->rows();
  layout->set_rows(layout->cols());
  layout->set_cols(tmp_rows);
}

template <typename Scalar>
void Transpose(Matrix<Scalar>* matrix) {
  TransposeLayout(matrix->mutable_layout());
}

template <typename Scalar, typename MulParamsType>
void EvalOpenBlas(const Matrix<Scalar>& lhs, const Matrix<Scalar>& rhs,
                  const MulParamsType& mul_params, int max_num_threads,
                  Matrix<Scalar>* dst) {
  RUY_CHECK_EQ(lhs.zero_point(), 0);
  RUY_CHECK_EQ(rhs.zero_point(), 0);
  RUY_CHECK_EQ(dst->zero_point(), 0);
  RUY_CHECK_EQ(mul_params.multiplier_fixedpoint(), 0);
  RUY_CHECK_EQ(mul_params.multiplier_exponent(), 0);

  Matrix<Scalar> gemm_lhs;
  Matrix<Scalar> gemm_rhs;
  Matrix<Scalar> gemm_dst;
  gemm_dst = *dst;

  // Use Transpose to reduce to the all-column-major case.
  // Notice that ruy::Matrix merely holds a pointer, does not own data,
  // so Transpose is cheap -- no actual matrix data is being transposed here.
  if (dst->layout().order() == Order::kColMajor) {
    gemm_lhs = lhs;
    gemm_rhs = rhs;
  } else {
    gemm_lhs = rhs;
    gemm_rhs = lhs;
    Transpose(&gemm_lhs);
    Transpose(&gemm_rhs);
    Transpose(&gemm_dst);
  }
  bool transposed_lhs = false;
  bool transposed_rhs = false;

  if (gemm_lhs.layout().order() == Order::kRowMajor) {
    Transpose(&gemm_lhs);
    transposed_lhs = true;
  }
  if (gemm_rhs.layout().order() == Order::kRowMajor) {
    Transpose(&gemm_rhs);
    transposed_rhs = true;
  }

  RUY_CHECK_EQ(gemm_lhs.layout().order(), Order::kColMajor);
  RUY_CHECK_EQ(gemm_rhs.layout().order(), Order::kColMajor);
  RUY_CHECK_EQ(gemm_dst.layout().order(), Order::kColMajor);

  char transa = transposed_lhs ? 'T' : 'N';
  char transb = transposed_rhs ? 'T' : 'N';
  int m = gemm_lhs.layout().rows();
  int n = gemm_rhs.layout().cols();
  int k = gemm_lhs.layout().cols();
  float alpha = 1;
  Scalar* a = gemm_lhs.data();
  int lda = gemm_lhs.layout().stride();
  Scalar* b = gemm_rhs.data();
  int ldb = gemm_rhs.layout().stride();
  float beta = 0;
  Scalar* c = gemm_dst.data();
  int ldc = gemm_dst.layout().stride();
  GenericBlasGemm<Scalar>::Run(&transa, &transb, &m, &n, &k, &alpha, a, &lda, b,
                               &ldb, &beta, c, &ldc);

  // BLAS does not allow us to express the bias-addition and clamping, so
  // we use Eigen for that.

  using EigenDstType =
      typename Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>::
          template StridedMapType<Eigen::OuterStride<Eigen::Dynamic>>::type;
  using EigenBiasType =
      typename Eigen::Matrix<Scalar, Eigen::Dynamic, 1>::ConstMapType;

  EigenDstType eigen_dst(
      gemm_dst.data(), gemm_dst.layout().rows(), gemm_dst.layout().cols(),
      Eigen::OuterStride<Eigen::Dynamic>(gemm_dst.layout().stride()));
  Eigen::setNbThreads(max_num_threads ? max_num_threads : 1);

  if (mul_params.bias()) {
    EigenBiasType eigen_bias(mul_params.bias(), dst->layout().rows());
    if (mul_params.clamp_max() == std::numeric_limits<Scalar>::infinity() &&
        mul_params.clamp_min() == -std::numeric_limits<Scalar>::infinity()) {
      eigen_dst.noalias() = eigen_dst.colwise() + eigen_bias;
    } else {
      eigen_dst.noalias() = (eigen_dst.colwise() + eigen_bias)
                                .cwiseMin(mul_params.clamp_max())
                                .cwiseMax(mul_params.clamp_min());
    }
  } else {
    if (mul_params.clamp_max() == std::numeric_limits<Scalar>::infinity() &&
        mul_params.clamp_min() == -std::numeric_limits<Scalar>::infinity()) {
    } else {
      eigen_dst.noalias() = eigen_dst.cwiseMin(mul_params.clamp_max())
                                .cwiseMax(mul_params.clamp_min());
    }
  }
}

template <typename TestSetType>
struct SupportsGemmlowp {
  static constexpr bool kValue =
      std::is_same<typename TestSetType::LhsScalar, std::uint8_t>::value &&
      std::is_same<typename TestSetType::RhsScalar, std::uint8_t>::value;
};

template <typename TestSetType>
struct UsesSingleScalarType {
  static constexpr bool kValue =
      std::is_same<typename TestSetType::DstScalar,
                   typename TestSetType::LhsScalar>::value &&
      std::is_same<typename TestSetType::DstScalar,
                   typename TestSetType::RhsScalar>::value &&
      std::is_same<typename TestSetType::DstScalar,
                   typename TestSetType::AccumScalar>::value;
};

template <typename TestSetType,
          bool IsFloatingPoint =
              std::is_floating_point<typename TestSetType::AccumScalar>::value,
          bool EnableGemmlowp = SupportsGemmlowp<TestSetType>::kValue,
          bool SingleScalarType = UsesSingleScalarType<TestSetType>::kValue>
struct EvalExternalPathImpl {
  using DstScalar = typename TestSetType::DstScalar;
  static void Run(TestSetType*, TestResult<DstScalar>*) { RUY_CHECK(false); }
};

template <typename TestSetType>
struct EvalExternalPathImpl<TestSetType, true, false, true> {
  using DstScalar = typename TestSetType::DstScalar;
  static void Run(TestSetType* test_set, TestResult<DstScalar>* test_result) {
    if (test_result->external_path == ExternalPath::kEigen) {
      EvalEigen(test_set->lhs.matrix, test_set->rhs.matrix,
                test_set->mul_params, test_set->max_num_threads,
                &test_result->storage_matrix.matrix);
    } else if (test_result->external_path == ExternalPath::kEigenTensor) {
      EvalEigenTensor(test_set->lhs.matrix, test_set->rhs.matrix,
                      test_set->mul_params, test_set->max_num_threads,
                      &test_result->storage_matrix.matrix);
    } else if (test_result->external_path == ExternalPath::kOpenBlas) {
      EvalOpenBlas(test_set->lhs.matrix, test_set->rhs.matrix,
                   test_set->mul_params, test_set->max_num_threads,
                   &test_result->storage_matrix.matrix);
    } else {
      RUY_CHECK(false);
    }
  }
};

template <typename TestSetType, bool SingleScalarType>
struct EvalExternalPathImpl<TestSetType, false, true, SingleScalarType> {
  using DstScalar = typename TestSetType::DstScalar;
  static void Run(TestSetType* test_set, TestResult<DstScalar>* test_result) {
    if (test_result->external_path == ExternalPath::kGemmlowp) {
      EvalGemmlowp(test_set->lhs.matrix, test_set->rhs.matrix,
                   test_set->mul_params, test_set->max_num_threads,
                   &test_result->storage_matrix.matrix);
    } else {
      RUY_CHECK(false);
    }
  }
};

#endif  // RUY_TEST_EXTERNAL_PATHS

template <typename TestSetType>
void EvalExternalPath(
    TestSetType* test_set,
    TestResult<typename TestSetType::DstScalar>* test_result) {
  if (test_result->external_path == ExternalPath::kReference) {
    // kReference is special because it's always available (the implementation
    // is provided by ruy) and supports all cases (quantized and float).
    ruy::ReferenceMul(test_set->lhs.matrix, test_set->rhs.matrix,
                      test_set->mul_params,
                      &test_result->storage_matrix.matrix);
  } else {
#ifdef RUY_TEST_EXTERNAL_PATHS
    EvalExternalPathImpl<TestSetType>::Run(test_set, test_result);
#endif  // RUY_TEST_EXTERNAL_PATHS
  }
}

template <typename Scalar>
bool Agree(ExternalPath external_path1, const Matrix<Scalar>& matrix1,
           ExternalPath external_path2, const Matrix<Scalar>& matrix2,
           int depth) {
  RUY_CHECK_EQ(matrix1.layout().rows(), matrix2.layout().rows());
  RUY_CHECK_EQ(matrix1.layout().cols(), matrix2.layout().cols());
  RUY_CHECK_EQ(matrix1.zero_point(), matrix2.zero_point());
  const int size = matrix1.layout().rows() * matrix1.layout().cols();
  double tolerated_max_diff = 0;
  double tolerated_mean_diff = 0;
  const float kSmallestAllowedDifference =
      4. * std::numeric_limits<Scalar>::epsilon();
  if (std::is_floating_point<Scalar>::value) {
    double max_abs_val = 0;
    for (int row = 0; row < matrix1.layout().rows(); row++) {
      for (int col = 0; col < matrix1.layout().cols(); col++) {
        max_abs_val =
            std::max(max_abs_val,
                     std::abs(static_cast<double>(Element(matrix1, row, col))));
        max_abs_val =
            std::max(max_abs_val,
                     std::abs(static_cast<double>(Element(matrix2, row, col))));
      }
    }
    tolerated_max_diff = max_abs_val * std::numeric_limits<Scalar>::epsilon() *
                         64 * std::sqrt(static_cast<float>(depth));
    if (tolerated_max_diff < kSmallestAllowedDifference) {
      // Clamp the tolerated max diff to be a bit above machine epsilon if the
      // calculated value is too small.
      tolerated_max_diff = kSmallestAllowedDifference;
      if (external_path1 == ExternalPath::kEigen ||
          external_path2 == ExternalPath::kEigen ||
          external_path1 == ExternalPath::kEigenTensor ||
          external_path2 == ExternalPath::kEigenTensor) {
        // Make additional allowance for Eigen differences.
        tolerated_max_diff *= 10.0f;
      }
    }
    tolerated_mean_diff = tolerated_max_diff / std::sqrt(size);
  } else if (std::is_same<Scalar, std::int32_t>::value) {
    // raw integer case, no rounding, so we can require exactness
    tolerated_max_diff = 0;
    tolerated_mean_diff = 0;
  } else {
    // quantized case, with rounding errors in downscaling int32 accumulators
    // to final 8bit or 16bit values.

    if (external_path1 != ExternalPath::kNone ||
        external_path2 != ExternalPath::kNone) {
      // In this case, we are comparing against some other library than ruy.
      //
      // We may have to tolerate an error of +/- 1 from using different
      // rounding in fixed-point multiplication, and then again an error of +/-
      // 1 from using different rounding in right shifts, so the tolerance on
      // the difference may have to be as large as 2.
      tolerated_max_diff = 2;
    } else if (RUY_PLATFORM_ARM) {
      // All our code paths on ARM (32 and 64-bit) are bit-exact
      // with the reference code (by design of the reference code).
      tolerated_max_diff = 0;
    } else if (RUY_PLATFORM_X86) {
      // Our reference and ARM paths have diverged from x86 paths in PR #227.
      // TODO: update the x86 path to adapt to that and reset that tolerance
      // to 0.
      tolerated_max_diff = 1;
    } else {
      // Other architectures, which we don't have dedicated code paths for
      // at the moment.
      // TODO: try resetting that tolerance to 0, since by
      // definition we're only using non-optimized code paths here.
      tolerated_max_diff = 1;
    }

    // totally empirical
    tolerated_mean_diff = std::min(1.0, 2.0 * std::pow(size, -0.18));
  }
  double sum_diff = 0;
  for (int row = 0; row < matrix1.layout().rows(); row++) {
    for (int col = 0; col < matrix1.layout().cols(); col++) {
      double elem1 = Element(matrix1, row, col);
      double elem2 = Element(matrix2, row, col);
      double diff = elem1 - elem2;

      sum_diff += diff;
      // Test (std::abs(diff) > tolerated_max_diff), but also true if diff is
      // NaN.
      if (!(std::abs(diff) <= tolerated_max_diff)) {
        return false;
      }
    }
  }
  double mean_diff = sum_diff / size;
  if (std::abs(mean_diff) > tolerated_mean_diff) {
    return false;
  }
  return true;
}

template <typename Scalar>
bool Agree(ExternalPath external_path1,
           const StorageMatrix<Scalar>& storage_matrix1,
           ExternalPath external_path2,
           const StorageMatrix<Scalar>& storage_matrix2, int depth) {
  VerifyConsistentFields(storage_matrix1);
  VerifyConsistentFields(storage_matrix2);
  return Agree(external_path1, storage_matrix1.matrix, external_path2,
               storage_matrix2.matrix, depth);
}

template <typename Scalar>
bool Agree(const TestResult<Scalar>& result1, const TestResult<Scalar>& result2,
           int depth) {
  return Agree(result1.external_path, result1.storage_matrix,
               result2.external_path, result2.storage_matrix, depth);
}

template <typename Scalar>
void AddTestResultToCluster(
    TestResult<Scalar>** result,
    std::vector<std::vector<TestResult<Scalar>*>>& clusters, int depth) {
  bool inserted = false;
  for (auto& cluster : clusters) {
    bool agreement = true;
    // Test for agreement with every result in the cluster.
    for (auto& other_result : cluster) {
      agreement &= Agree(**result, *other_result, depth);
    }
    if (agreement) {
      cluster.push_back(*result);
      inserted = true;
    }
  }
  if (!inserted) {
    std::vector<TestResult<Scalar>*> new_results;
    new_results.push_back(*result);
    clusters.push_back(new_results);
  }
}

template <typename Scalar>
void PrintPathsInAgreement(
    const std::vector<std::unique_ptr<TestResult<Scalar>>>& results,
    int depth) {
  // A container holding vectors of TestResults, where membership indicates
  // that all TestResults agree with each other.
  std::vector<std::vector<TestResult<Scalar>*>> clusters;
  for (const auto& result : results) {
    TestResult<Scalar>* test_result = result.get();
    AddTestResultToCluster(&test_result, clusters, depth);
  }

  std::cerr << "Error: Not all paths agree. \n";
  for (auto& cluster : clusters) {
    std::cerr << "These paths all agree with each other: ";
    for (auto& result : cluster) {
      std::cerr << PathName(*result) << ", ";
    }
    std::cerr << "but disagree with the rest.\n";
  }
}

struct Stats {
  double median;
  double mean;
  double min;
  double max;
};

inline std::string StatsAsString(const Stats& stats) {
  char buf[256];
  snprintf(buf, sizeof(buf), "(median = %g, mean = %g, min = %g, max = %g)",
           stats.median, stats.mean, stats.min, stats.max);
  return std::string(buf);
}

template <typename Scalar>
void GetMatrixStats(const Matrix<Scalar>& matrix, Stats* stats) {
  double min = std::numeric_limits<double>::infinity();
  double max = -std::numeric_limits<double>::infinity();
  double sum = 0;
  std::vector<double> allvals;
  for (int row = 0; row < matrix.layout().rows(); row++) {
    for (int col = 0; col < matrix.layout().cols(); col++) {
      double val = Element(matrix, row, col);
      min = std::min(min, val);
      max = std::max(max, val);
      sum += val;
      allvals.push_back(val);
    }
  }
  std::sort(allvals.begin(), allvals.end());
  stats->min = min;
  stats->max = max;
  stats->mean = sum / allvals.size();
  stats->median = allvals[allvals.size() / 2];
}

struct ErrorAnalysis {
  Stats stats_good;
  Stats stats_bad;
  // The below is to help document departure from bit exactness. It's probably
  // not going to be relevant to floating-point.
  std::set<int> error_rows;
  std::set<int> error_cols;
  int row_of_first_error = 0;
  int col_of_first_error = 0;
  double first_error_good_value = 0;
  double first_error_bad_value = 0;
};

template <typename TestSetType>
void AnalyzeTestError(const TestSetType& test_set, int first_bad_result_index,
                      ErrorAnalysis* error_analysis) {
  const auto& good_matrix = test_set.results[0]->storage_matrix.matrix;
  const auto& bad_matrix =
      test_set.results[first_bad_result_index]->storage_matrix.matrix;
  GetMatrixStats(good_matrix, &error_analysis->stats_good);
  GetMatrixStats(bad_matrix, &error_analysis->stats_bad);
  bool found_first_error = false;
  for (int row = 0; row < good_matrix.layout().rows(); row++) {
    for (int col = 0; col < good_matrix.layout().cols(); col++) {
      if (Element(good_matrix, row, col) != Element(bad_matrix, row, col)) {
        if (!found_first_error) {
          found_first_error = true;
          error_analysis->row_of_first_error = row;
          error_analysis->col_of_first_error = col;
          error_analysis->first_error_good_value =
              Element(good_matrix, row, col);
          error_analysis->first_error_bad_value = Element(bad_matrix, row, col);
        }
        error_analysis->error_rows.insert(row);
        error_analysis->error_cols.insert(col);
      }
    }
  }
}

template <typename TestSetType>
void ComputeReasonableMultiplier(
    const Matrix<typename TestSetType::LhsScalar>& lhs,
    const Matrix<typename TestSetType::RhsScalar>&, double* multiplier) {
  using LhsScalar = typename TestSetType::LhsScalar;
  using RhsScalar = typename TestSetType::RhsScalar;
  using DstScalar = typename TestSetType::DstScalar;
  if (std::is_floating_point<DstScalar>::value ||
      std::is_same<DstScalar, std::int32_t>::value) {
    *multiplier = 0;
    return;
  }
  *multiplier = static_cast<double>(std::numeric_limits<DstScalar>::max()) /
                (static_cast<double>(lhs.layout().cols()) *
                 std::numeric_limits<LhsScalar>::max() *
                 std::numeric_limits<RhsScalar>::max());
}

inline void QuantizeMultiplier(double multiplier_double,
                               std::int32_t* multiplier_fixedpoint,
                               int* multiplier_exponent) {
  RUY_CHECK_GT(multiplier_double, 0);
  if (multiplier_double == 0.) {
    *multiplier_fixedpoint = 0;
    *multiplier_exponent = 0;
    return;
  }
  const double q = std::frexp(multiplier_double, multiplier_exponent);
  auto q_fixed = static_cast<std::int64_t>(std::round(q * (1ll << 31)));
  RUY_CHECK_LE(q_fixed, (1ll << 31));
  if (q_fixed == (1ll << 31)) {
    q_fixed /= 2;
    ++*multiplier_exponent;
  }
  RUY_CHECK_LE(q_fixed, std::numeric_limits<std::int32_t>::max());
  *multiplier_fixedpoint = static_cast<std::int32_t>(q_fixed);
}

template <typename TestSetType>
void SwitchMultiplierToPerChannel(TestSetType* test_set) {
  ChannelDimension channel_dimension =
      (test_set->benchmark || global_random_engine()() % 2)
          ? ChannelDimension::kRow
          : ChannelDimension::kCol;
  test_set->mul_params.set_channel_dimension(channel_dimension);
  const int num_channels = channel_dimension == ChannelDimension::kRow
                               ? test_set->rows
                               : test_set->cols;
  test_set->per_channel_multiplier_fixedpoint.resize(num_channels);
  test_set->per_channel_multiplier_exponent.resize(num_channels);
  for (int i = 0; i < num_channels; i++) {
    // multipliers typically range in [2^30 ; 2^31 - 1].
    // Values in [0, 2^30 - 1] are normally unused, but harmless.
    // Thus a good way to randomize multipliers is to subtract from them
    // a random value smaller than 2^30 but still significant compared to it.
    std::int32_t nudged_multiplier =
        test_set->mul_params.multiplier_fixedpoint() -
        (global_random_engine()() % (1 << 26));
    int nudged_exponent = test_set->mul_params.multiplier_exponent() - 1 +
                          (global_random_engine()() % 4);
    test_set->per_channel_multiplier_fixedpoint[i] = nudged_multiplier;
    test_set->per_channel_multiplier_exponent[i] = nudged_exponent;
  }
  test_set->mul_params.set_multiplier_fixedpoint(0);
  test_set->mul_params.set_multiplier_exponent(0);
  test_set->mul_params.set_multiplier_fixedpoint_perchannel(
      test_set->per_channel_multiplier_fixedpoint.data());
  test_set->mul_params.set_multiplier_exponent_perchannel(
      test_set->per_channel_multiplier_exponent.data());
}

template <
    typename TestSetType,
    bool IsApplicable =
        std::is_same<typename TestSetType::AccumScalar, std::int32_t>::value &&
        !std::is_same<typename TestSetType::DstScalar, std::int32_t>::value>
struct MakeSpecMultiplierFieldsImpl {};

template <typename TestSetType>
struct MakeSpecMultiplierFieldsImpl<TestSetType, true> {
  static void Run(TestSetType* test_set) {
    double multiplier;
    ComputeReasonableMultiplier<TestSetType>(test_set->lhs.matrix,
                                             test_set->rhs.matrix, &multiplier);
    typename TestSetType::AccumScalar multiplier_fixedpoint;
    int multiplier_exponent;
    QuantizeMultiplier(multiplier, &multiplier_fixedpoint,
                       &multiplier_exponent);
    test_set->mul_params.set_multiplier_fixedpoint(multiplier_fixedpoint);
    test_set->mul_params.set_multiplier_exponent(multiplier_exponent);

    if (!test_set->benchmark) {
      test_set->perchannel = global_random_engine()() & 1;
    }
    if (test_set->perchannel) {
      SwitchMultiplierToPerChannel(test_set);
    }
  }
};

template <typename TestSetType>
struct MakeSpecMultiplierFieldsImpl<TestSetType, false> {
  static void Run(TestSetType*) {}
};

template <typename MulParamsType>
void MakeSpecClampFields(MulParamsType* mul_params) {
  using DstScalar = typename MulParamsType::DstScalar;

  if (getenv("BENCHMARK_ONLY_MATMUL")) {
    if (std::is_floating_point<DstScalar>::value) {
      mul_params->set_clamp_min(-std::numeric_limits<DstScalar>::infinity());
      mul_params->set_clamp_max(std::numeric_limits<DstScalar>::infinity());
    } else {
      mul_params->set_clamp_min(std::numeric_limits<DstScalar>::lowest());
      mul_params->set_clamp_max(std::numeric_limits<DstScalar>::max());
    }
    return;
  }

  mul_params->set_clamp_min(std::numeric_limits<DstScalar>::lowest() + 1);
  mul_params->set_clamp_max(std::numeric_limits<DstScalar>::max() - 1);
}

void MakeSpecClampFields(MulParams<std::int32_t, std::int32_t>*) {
  // Returning raw accumulators, clamping is not supported.
}

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void TestSet<LhsScalar, RhsScalar, AccumScalar, DstScalar>::MakeZeroPoints() {
  RUY_CHECK_EQ(life_stage, LifeStage::kInitial);
  if (!benchmark && !use_specified_zero_points) {
    MakeRandomScalar(RandomRange::kReasonableSrcZeroPoint, &lhs_zero_point);
    MakeRandomScalar(RandomRange::kReasonableSrcZeroPoint, &rhs_zero_point);
    // If destination is std::int32_t, no dst_zero_point is necessary.
    if (std::is_same<DstScalar, std::int32_t>::value) {
      dst_zero_point = 0;
    } else {
      MakeRandomScalar(RandomRange::kReasonableDstZeroPoint, &dst_zero_point);
    }
  }
  life_stage = LifeStage::kHasZeroPoints;
}

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void TestSet<LhsScalar, RhsScalar, AccumScalar, DstScalar>::MakeLhsRhs() {
  RUY_CHECK_EQ(life_stage, LifeStage::kHasZeroPoints);
  MakeRandom(rows, depth, lhs_order, lhs_zero_point, layout_style,
             RandomRange::kOffCenterAvoidMinValue, &lhs);
  MakeRandom(depth, cols, rhs_order, rhs_zero_point, layout_style,
             RandomRange::kGeneral, &rhs);
  if (!benchmark) {
    cache_lhs = (global_random_engine()() & 0xf) == 0;
    cache_rhs = (global_random_engine()() & 0xf) == 0;
  }
  if (cache_lhs) {
    lhs.matrix.set_cache_policy(CachePolicy::kAlwaysCache);
  }
  if (cache_rhs) {
    rhs.matrix.set_cache_policy(CachePolicy::kAlwaysCache);
  }
  life_stage = LifeStage::kHasLhsRhs;
}

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void TestSet<LhsScalar, RhsScalar, AccumScalar, DstScalar>::MakeMulParams() {
  RUY_CHECK_EQ(life_stage, LifeStage::kHasLhsRhs);

  if (!getenv("BENCHMARK_ONLY_MATMUL") &&
      (benchmark || (global_random_engine()() & 1))) {
    MakeRandomVector(RandomRange::kBias, std::max(rows, cols), &bias_data);
    mul_params.set_bias(bias_data.data());
  }
  if (lhs.matrix.zero_point() == std::numeric_limits<LhsScalar>::lowest() &&
      rhs.matrix.zero_point() == std::numeric_limits<RhsScalar>::lowest()) {
    lhs.matrix.set_zero_point(lhs.matrix.zero_point() + 1);
  }
  MakeSpecMultiplierFieldsImpl<TestSet>::Run(this);
  MakeSpecClampFields(&mul_params);
  life_stage = LifeStage::kHasMulParams;
}

inline int GetIntEnvVarOrZero(const char* name) {
  const char* val = getenv(name);
  if (!val) {
    return 0;
  }
  return std::stoi(val);
}

inline float GetFloatEnvVarOrZero(const char* name) {
  const char* val = getenv(name);
  if (!val) {
    return 0;
  }
  return std::stof(val);
}

inline int GetHexIntEnvVarOrZero(const char* name) {
  const char* val = getenv(name);
  if (!val) {
    return 0;
  }
  return std::stoi(val, nullptr, 16);
}

inline bool GetBoolEnvVarOrFalse(const char* name) {
  return static_cast<bool>(GetIntEnvVarOrZero(name));
}

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void TestSet<LhsScalar, RhsScalar, AccumScalar, DstScalar>::MakeOtherParams() {
  RUY_CHECK_EQ(life_stage, LifeStage::kHasMulParams);
  if (max_num_threads == 0) {
    max_num_threads = GetIntEnvVarOrZero("THREADS");
  }
  life_stage = LifeStage::kHasOtherParams;
}

inline std::vector<Path> PathsBitfieldAsVector(Path paths_bitfield) {
  std::vector<Path> result;
  std::uint32_t remaining_paths = static_cast<std::uint32_t>(paths_bitfield);
  std::uint32_t test_bit = 1;
  while (remaining_paths) {
    if (remaining_paths & test_bit) {
      result.push_back(static_cast<Path>(test_bit));
    }
    remaining_paths &= ~test_bit;
    test_bit <<= 1;
  }
  return result;
}

inline std::vector<Tuning> EnumerateTuningsForPath(Path path, bool benchmark) {
  if (benchmark) {
    return {Tuning::kAuto};
  }
#if RUY_PLATFORM_ARM
  if (path == Path::kNeon || path == Path::kNeonDotprod) {
    return {Tuning::kA55ish, Tuning::kGeneric, Tuning::kAuto};
  }
#endif
  (void)path;
  return {Tuning::kAuto};
}

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void TestSet<LhsScalar, RhsScalar, AccumScalar, DstScalar>::MakeResultPaths() {
  RUY_CHECK_EQ(life_stage, LifeStage::kHasOtherParams);

  Path paths_bitfield = static_cast<Path>(GetHexIntEnvVarOrZero("PATHS"));

  if (paths_bitfield == Path::kNone) {
    // Use a dummy Context just to perform the resolution of specific runtime
    // enabled paths.
    Context context;
    paths_bitfield = get_ctx(&context)->GetRuntimeEnabledPaths();
  }

  // Disable the internal test-only variants of the StandardCpp path on large
  // tests.
  // This constant be large enough to exercise some interesting BlockMap logic,
  // small enough to avoid large test latency increases from running these
  // slow code paths on large matrix multiplications.
  const int kMaxSizeToTestInternalStandardCppVariants = 300;
  if (rows > kMaxSizeToTestInternalStandardCppVariants ||
      cols > kMaxSizeToTestInternalStandardCppVariants ||
      depth > kMaxSizeToTestInternalStandardCppVariants) {
    paths_bitfield = paths_bitfield & kAllPaths;
  }

  // Trim bits that don't correspond to a compiled path,
  // to allow specifying e.g. ffff to mean 'all paths' regardless of whether all
  // those bits exist as actual paths.
  paths_bitfield = paths_bitfield & kAllPathsIncludingInternalVariants;
  RUY_CHECK_NE(paths_bitfield, Path::kNone);
  paths = PathsBitfieldAsVector(paths_bitfield);

  // kReference is a special 'external path' that's always available.
  // It can still be disabled by NOEXT.
  if (!GetBoolEnvVarOrFalse("NOEXT")) {
    external_paths.push_back(ExternalPath::kReference);
  }

#ifdef RUY_TEST_EXTERNAL_PATHS

  if (!GetBoolEnvVarOrFalse("NOEXT")) {
    if (SupportsGemmlowp<TestSet>::kValue) {
#ifdef GEMMLOWP_SSE4
      const bool gemmlowp_supported =
          !mul_params.multiplier_fixedpoint_perchannel() &&
          mul_params.channel_dimension() == ChannelDimension::kRow;
#else
      const bool gemmlowp_supported =
          mul_params.channel_dimension() == ChannelDimension::kRow;
#endif
      if (gemmlowp_supported) {
        external_paths.push_back(ExternalPath::kGemmlowp);
      }
    }
    if (UsesSingleScalarType<TestSet>::kValue &&
        std::is_floating_point<AccumScalar>::value) {
      external_paths.push_back(ExternalPath::kEigen);
      if (layout_style == LayoutStyle::kUnstridedLinear) {
        external_paths.push_back(ExternalPath::kEigenTensor);
      }
// We link against a generic BLAS target that only maps to OpenBLAS on specific
// architectures.
#if RUY_PLATFORM_ARM_32 || RUY_PLATFORM_ARM_64
      // OpenBLAS multi-threading is disabled, so avoid mixing single-threaded
      // and multi-threaded benchmark results.
      if (max_num_threads == 1 && !getenv("NO_OPENBLAS")) {
        external_paths.push_back(ExternalPath::kOpenBlas);
      }
#endif
    }
  }

#endif  // RUY_TEST_EXTERNAL_PATHS

  for (Path path : paths) {
    for (Tuning tuning : EnumerateTuningsForPath(path, benchmark)) {
      results.emplace_back(new TestResultType);
      TestResultType& result = *results.back();
      result.path = path;
      result.tuning = tuning;
      MakeRandom(rows, cols, dst_order, dst_zero_point, layout_style,
                 RandomRange::kGeneral, &result.storage_matrix);
    }
  }

  for (ExternalPath external_path : external_paths) {
    results.emplace_back(new TestResultType);
    TestResultType& result = *results.back();
    result.external_path = external_path;
    MakeRandom(rows, cols, dst_order, dst_zero_point, layout_style,
               RandomRange::kGeneral, &result.storage_matrix);
  }

  life_stage = LifeStage::kHasResultPaths;
}

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void TestSet<LhsScalar, RhsScalar, AccumScalar, DstScalar>::EvalResult(
    TestResult<DstScalar>* result) {
  RUY_CHECK(result->path != Path::kNone ||
            result->external_path != ExternalPath::kNone);
  if (result->path != Path::kNone) {
    EvalRuy(result);
  } else {
    EvalExternalPath(this, result);
  }
  const std::string& pathname = PathName(*result);
  if (std::find(CoveredPaths()->begin(), CoveredPaths()->end(), pathname) ==
      CoveredPaths()->end()) {
    CoveredPaths()->push_back(pathname);
  }
}

using f32 = float;
using f64 = double;
using u8 = std::uint8_t;
using i8 = std::int8_t;
using u16 = std::uint16_t;
using i16 = std::int16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
using i64 = std::int64_t;

template <typename Scalar>
const char* TypeName() {
  return nullptr;
}

#define RUY_TYPENAME(TYPE)              \
  template <>                           \
  inline const char* TypeName<TYPE>() { \
    return #TYPE;                       \
  }

RUY_TYPENAME(f32)
RUY_TYPENAME(f64)
RUY_TYPENAME(u8)
RUY_TYPENAME(i8)
RUY_TYPENAME(u16)
RUY_TYPENAME(i16)
RUY_TYPENAME(u32)
RUY_TYPENAME(i32)
RUY_TYPENAME(u64)
RUY_TYPENAME(i64)

#undef RUY_TYPENAME

template <typename Scalar>
const char* SymmetryName(const Matrix<Scalar>& matrix) {
  if (matrix.zero_point() == SymmetricZeroPoint<Scalar>()) {
    return "symm";
  } else {
    return "asymm";
  }
}

template <typename Scalar>
int StorageSize(const Matrix<Scalar>& matrix) {
  return sizeof(Scalar) * FlatSize(matrix.layout());
}

// Helper that replicates a buffer and gives out pointers to the replicas.
// This is useful when one wants to traverse data so that it is cold in cache.
// By having a sufficiently large value of num_repeats, one can ensure that the
// working set covered by the replicas is greater than the cache size.
template <typename T>
class RepeatedBuffer {
 public:
  RepeatedBuffer() = default;
  void Init(const T* elems, int num_elems, int num_repeats) {
    buffers_.clear();
    allocator_.FreeAll();
    for (int i = 0; i < num_repeats; i++) {
      T* p;
      allocator_.Allocate(num_elems, &p);
      memcpy(p, elems, num_elems * sizeof(T));
      buffers_.push_back(p);
    }
  }
  T* Next() {
    T* ret = buffers_[current_];
    current_ = (current_ + 1) % buffers_.size();
    return ret;
  }

 private:
  Allocator allocator_;
  std::vector<T*> buffers_;
  int current_ = 0;
};

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void TestSet<LhsScalar, RhsScalar, AccumScalar, DstScalar>::Benchmark(
    TestResult<DstScalar>* result) {
  const bool cold = getenv("RUY_BENCHMARK_COLD");
  LhsScalar* orig_lhs_data = lhs.matrix.data();
  RhsScalar* orig_rhs_data = rhs.matrix.data();
  DstScalar* orig_dst_data = result->storage_matrix.matrix.data();

  int num_matmul_sets = 0;

  RepeatedBuffer<LhsScalar> cold_lhs;
  RepeatedBuffer<RhsScalar> cold_rhs;
  RepeatedBuffer<DstScalar> cold_dst;

  if (cold) {
    const int kWorkingSetSize = 100 << 20;
    const int each_matmul_set_size = StorageSize(lhs.matrix) +
                                     StorageSize(rhs.matrix) +
                                     StorageSize(result->storage_matrix.matrix);
    num_matmul_sets =
        (kWorkingSetSize + each_matmul_set_size - 1) / each_matmul_set_size;

    cold_lhs.Init(lhs.matrix.data(), FlatSize(lhs.matrix.layout()),
                  num_matmul_sets);
    cold_rhs.Init(rhs.matrix.data(), FlatSize(rhs.matrix.layout()),
                  num_matmul_sets);
    cold_dst.Init(result->storage_matrix.matrix.data(),
                  FlatSize(result->storage_matrix.matrix.layout()),
                  num_matmul_sets);
  }
  const bool record_pmu = GetBoolEnvVarOrFalse("RUY_BENCHMARK_PMU");
  int repeats = GetIntEnvVarOrZero("RUY_BENCHMARK_REPEATS");
  if (!repeats) {
    repeats = 4;
  }
  float benchmark_min_secs = GetFloatEnvVarOrZero("RUY_BENCHMARK_MIN_SECS");
  if (!benchmark_min_secs) {
    benchmark_min_secs = 0.5;
  }
#ifdef RUY_PROFILER
  {
    const char* lhstype = TypeName<LhsScalar>();
    const char* lhssymm = SymmetryName(lhs.matrix);
    const char* rhstype = TypeName<RhsScalar>();
    const char* rhssymm = SymmetryName(rhs.matrix);

    printf("Profiling path=%s shape=(%dx%dx%d) lhs=(%s,%s) rhs=(%s,%s)\n",
           PathName(*result).c_str(), rows, depth, cols, lhstype, lhssymm,
           rhstype, rhssymm);
    ruy::profiler::ScopeProfile profile;
#endif

  float latency = std::numeric_limits<float>::infinity();
  float l1_refill_rate = std::numeric_limits<float>::infinity();
  float l2_refill_rate = std::numeric_limits<float>::infinity();
  float l3_refill_rate = std::numeric_limits<float>::infinity();
  float l1tlb_refill_rate = std::numeric_limits<float>::infinity();
  float l2tlb_refill_rate = std::numeric_limits<float>::infinity();
  float mispred_rate = std::numeric_limits<float>::infinity();
  float frontend_stall_rate = std::numeric_limits<float>::infinity();
  float backend_stall_rate = std::numeric_limits<float>::infinity();

  for (int repeat = 0; repeat < repeats; repeat++) {
    auto& pmu_events = GlobalPmuEvents();
    if (record_pmu) {
      pmu_events.StartRecording();
    }
    TimePoint time_start = Now();
    TimePoint t = time_start;
    int iters = 0;
    int iters_at_a_time = 1;
    while (ToFloatSeconds(t - time_start) < benchmark_min_secs) {
      for (int i = 0; i < iters_at_a_time; i++) {
        if (cold) {
          lhs.matrix.set_data(cold_lhs.Next());
          rhs.matrix.set_data(cold_rhs.Next());
          result->storage_matrix.matrix.set_data(cold_dst.Next());
        }
        EvalResult(result);
        iters++;
      }
      iters_at_a_time *= 2;
      t = Now();
    }
    latency = std::min(
        latency, static_cast<float>(ToFloatSeconds(t - time_start) / iters));
    if (record_pmu) {
      pmu_events.StopRecording();
      const float normalization_factor =
          1.0f / (static_cast<float>(iters) * rows * cols * depth);
      l1_refill_rate = std::min(
          l1_refill_rate, pmu_events.L1RefillCount() * normalization_factor);
      l2_refill_rate = std::min(
          l2_refill_rate, pmu_events.L2RefillCount() * normalization_factor);
      l3_refill_rate = std::min(
          l3_refill_rate, pmu_events.L3RefillCount() * normalization_factor);
      l1tlb_refill_rate =
          std::min(l1tlb_refill_rate,
                   pmu_events.L1TLBRefillCount() * normalization_factor);
      l2tlb_refill_rate =
          std::min(l2tlb_refill_rate,
                   pmu_events.L2TLBRefillCount() * normalization_factor);
      mispred_rate =
          std::min(mispred_rate, pmu_events.BranchMispredictionCount() *
                                     normalization_factor);
      frontend_stall_rate =
          std::min(frontend_stall_rate,
                   pmu_events.FrontendStallCount() * normalization_factor);
      backend_stall_rate =
          std::min(backend_stall_rate,
                   pmu_events.BackendStallCount() * normalization_factor);
    }
  }
  result->latency = latency;
  if (record_pmu) {
    result->l1_refill_rate = l1_refill_rate;
    result->l2_refill_rate = l2_refill_rate;
    result->l3_refill_rate = l3_refill_rate;
    result->l1tlb_refill_rate = l1tlb_refill_rate;
    result->l2tlb_refill_rate = l2tlb_refill_rate;
    result->mispred_rate = mispred_rate;
    result->frontend_stall_rate = frontend_stall_rate;
    result->backend_stall_rate = backend_stall_rate;
  }

#ifdef RUY_PROFILER
  }
  fflush(stdout);
#endif

  if (cold) {
    lhs.matrix.set_data(orig_lhs_data);
    rhs.matrix.set_data(orig_rhs_data);
    memcpy(orig_dst_data, result->storage_matrix.matrix.data(),
           StorageSize(result->storage_matrix.matrix));
    result->storage_matrix.matrix.set_data(orig_dst_data);
  }
}

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void TestSet<LhsScalar, RhsScalar, AccumScalar, DstScalar>::Eval() {
  RUY_CHECK_EQ(life_stage, LifeStage::kHasResultPaths);
  for (auto& result : results) {
    if (benchmark) {
      Benchmark(result.get());
    } else {
      EvalResult(result.get());
    }
  }
  life_stage = LifeStage::kEvaluated;
}

template <typename Scalar>
std::string DumpRegion(const Matrix<Scalar>& matrix, int center_row,
                       int center_col) {
  static constexpr int kRadius = 20;
  int first_row = std::max(0, center_row - kRadius);
  int last_row = std::min(matrix.layout().rows() - 1, center_row + kRadius);
  int first_col = std::max(0, center_col - kRadius);
  int last_col = std::min(matrix.layout().cols() - 1, center_col + kRadius);
  std::ostringstream stream;
  for (int row = first_row; row <= last_row; row++) {
    for (int col = first_col; col <= last_col; col++) {
      stream << static_cast<double>(Element(matrix, row, col)) << " ";
    }
    stream << "\n";
  }
  return stream.str();
}

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void TestSet<LhsScalar, RhsScalar, AccumScalar, DstScalar>::VerifyTestResults()
    const {
  const int depth = lhs.matrix.layout().cols();
  for (int i = 0; i < static_cast<int>(results.size()) - 1; i++) {
    if (!Agree(*results[i], *results[i + 1], depth)) {
      PrintPathsInAgreement(results, depth);
      ErrorAnalysis error_analysis;
      AnalyzeTestError(*this, i + 1, &error_analysis);
      std::cerr << "Shape: rows = " << rows << ", cols = " << cols
                << ", depth = " << depth << std::endl;
      std::cerr << "Stats of the good result matrix: "
                << StatsAsString(error_analysis.stats_good) << std::endl;
      std::cerr << "Stats of the bad result matrix:  "
                << StatsAsString(error_analysis.stats_bad) << std::endl;
      if (static_cast<int>(error_analysis.error_rows.size()) < rows) {
        std::cerr << "Rows containing errors: "
                  << Join(error_analysis.error_rows) << std::endl;
      } else {
        std::cerr << "Errors found in ALL rows." << std::endl;
      }
      if (static_cast<int>(error_analysis.error_cols.size()) < cols) {
        std::cerr << "Cols containing errors: "
                  << Join(error_analysis.error_cols) << std::endl;
      } else {
        std::cerr << "Errors found in ALL cols." << std::endl;
      }
      std::cerr << "The first error occurs at row "
                << error_analysis.row_of_first_error << ", col "
                << error_analysis.col_of_first_error << std::endl;
      std::cerr << "Good value: " << error_analysis.first_error_good_value
                << std::endl;
      std::cerr << "Bad value : " << error_analysis.first_error_bad_value
                << std::endl;
      std::cerr << "Region of Good result matrix around first error:\n\n"
                << DumpRegion(results[0]->storage_matrix.matrix,
                              error_analysis.row_of_first_error,
                              error_analysis.col_of_first_error)
                << std::endl;
      std::cerr << "Region of Bad result matrix around first error:\n\n"
                << DumpRegion(results[i + 1]->storage_matrix.matrix,
                              error_analysis.row_of_first_error,
                              error_analysis.col_of_first_error)
                << std::endl;
      RUY_CHECK(false);
    }
  }
}

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void TestSet<LhsScalar, RhsScalar, AccumScalar, DstScalar>::Verify() {
  RUY_CHECK_EQ(life_stage, LifeStage::kEvaluated);
  VerifyTestResults();
  life_stage = LifeStage::kFinal;
}

template <typename TestSetType>
void TestRCC(int rows, int depth, int cols) {
  TestSetType test_set;
  test_set.rows = rows;
  test_set.depth = depth;
  test_set.cols = cols;
  test_set.lhs_order = Order::kRowMajor;
  test_set.rhs_order = Order::kColMajor;
  test_set.dst_order = Order::kColMajor;
  test_set.layout_style = LayoutStyle::kUnstridedLinear;
  test_set.Run();
}

template <typename TestSetType>
void TestNonRCC(int rows, int depth, int cols) {
  TestSetType test_set;
  test_set.rows = rows;
  test_set.depth = depth;
  test_set.cols = cols;
  test_set.lhs_order = Order::kColMajor;
  test_set.rhs_order = Order::kColMajor;
  test_set.dst_order = Order::kColMajor;
  test_set.layout_style = LayoutStyle::kUnstridedLinear;
  test_set.Run();
}

template <typename TestSetType>
void TestLinearAllOrders(int rows, int depth, int cols) {
  const std::vector<Order> orders{Order::kColMajor, Order::kRowMajor};

  for (Order lhs_order : orders) {
    for (Order rhs_order : orders) {
      for (Order dst_order : orders) {
        TestSetType test_set;
        test_set.rows = rows;
        test_set.depth = depth;
        test_set.cols = cols;
        test_set.lhs_order = lhs_order;
        test_set.rhs_order = rhs_order;
        test_set.dst_order = dst_order;
        test_set.layout_style = LayoutStyle::kLinear;
        test_set.Run();
      }
    }
  }
}

}  // namespace ruy

#endif  // RUY_RUY_TEST_H_

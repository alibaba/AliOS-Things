#include "ruy/context.h"
#include "ruy/gtest_wrapper.h"
#include "ruy/kernel.h"
#include "ruy/matrix.h"
#include "ruy/path.h"
#include "ruy/performance_advisory.h"
#include "ruy/ruy.h"

namespace ruy {
namespace {

constexpr Path kPath = Path::kInternalStandardCppVariant3;
constexpr int kBufferSize = 64;

template <typename AccumScalar, typename DstScalar,
          bool HaveQuantizedMultipliers =
              std::is_same<AccumScalar, std::int32_t>::value &&
              !std::is_same<DstScalar, std::int32_t>::value>
struct PopulatePerChannelBuffersImpl {
  static void Run(MulParams<AccumScalar, DstScalar>* mul_params) {
    static const AccumScalar bias_buf[kBufferSize] = {0};
    static const AccumScalar multiplier_fixedpoint_buf[kBufferSize] = {0};
    static const int multiplier_exponent_buf[kBufferSize] = {0};
    mul_params->set_bias(bias_buf);
    mul_params->set_multiplier_fixedpoint_perchannel(multiplier_fixedpoint_buf);
    mul_params->set_multiplier_exponent_perchannel(multiplier_exponent_buf);
  }
};

template <typename AccumScalar, typename DstScalar>
struct PopulatePerChannelBuffersImpl<AccumScalar, DstScalar, false> {
  static void Run(MulParams<AccumScalar, DstScalar>* mul_params) {
    static const AccumScalar bias_buf[kBufferSize] = {0};
    mul_params->set_bias(bias_buf);
  }
};

template <typename AccumScalar, typename DstScalar>
void PopulatePerChannelBuffers(MulParams<AccumScalar, DstScalar>* mul_params) {
  PopulatePerChannelBuffersImpl<AccumScalar, DstScalar>::Run(mul_params);
}

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar>
void TestPerChannelBuffersReallocation() {
  using KernelType = Kernel<kPath, float, float, float, float>;

  MulParams<AccumScalar, DstScalar> mul_params;
  PopulatePerChannelBuffers(&mul_params);

  const int kMatrixSize = 3;
  ruy::Matrix<LhsScalar> lhs;
  ruy::MakeSimpleLayout(kMatrixSize, kMatrixSize, ruy::Order::kRowMajor,
                        lhs.mutable_layout());
  const LhsScalar lhs_data[kMatrixSize * kMatrixSize] = {0};
  lhs.set_data(lhs_data);
  ruy::Matrix<RhsScalar> rhs;
  ruy::MakeSimpleLayout(kMatrixSize, kMatrixSize, ruy::Order::kColMajor,
                        rhs.mutable_layout());
  const RhsScalar rhs_data[kMatrixSize * kMatrixSize] = {0};
  rhs.set_data(rhs_data);
  DstScalar dst_data[kMatrixSize * kMatrixSize] = {0};
  ruy::Matrix<DstScalar> dst;
  ruy::MakeSimpleLayout(kMatrixSize, kMatrixSize, ruy::Order::kColMajor,
                        dst.mutable_layout());
  dst.set_data(dst_data);

  ruy::Context context;

  auto test_advisory = [&](bool expect_advisory,
                           ChannelDimension channel_dimension,
                           int capacity_rounding) {
    mul_params.set_channel_dimension(channel_dimension);
    mul_params.set_perchannel_buffers_capacity_rounding(capacity_rounding);
    ruy::Mul<kPath>(lhs, rhs, mul_params, &context, &dst);
    EXPECT_EQ(context.performance_advisory(
                  PerformanceAdvisory::kReallocatedPerChannelBuffer),
              expect_advisory);
  };

  static_assert(KernelType::LhsLayout::kCols == 16, "");
  test_advisory(true, ChannelDimension::kRow, 1);
  test_advisory(true, ChannelDimension::kRow, 2);
  test_advisory(true, ChannelDimension::kRow, 4);
  test_advisory(true, ChannelDimension::kRow, 8);
  test_advisory(false, ChannelDimension::kRow, 16);
  test_advisory(false, ChannelDimension::kRow, 32);
  test_advisory(false, ChannelDimension::kRow, 64);

  static_assert(KernelType::RhsLayout::kCols == 8, "");
  test_advisory(true, ChannelDimension::kCol, 1);
  test_advisory(true, ChannelDimension::kCol, 2);
  test_advisory(true, ChannelDimension::kCol, 4);
  test_advisory(false, ChannelDimension::kCol, 8);
  test_advisory(false, ChannelDimension::kCol, 16);
  test_advisory(false, ChannelDimension::kCol, 32);
  test_advisory(false, ChannelDimension::kCol, 64);
}

TEST(PerChannelBuffersReallocationTest, Float) {
  TestPerChannelBuffersReallocation<float, float, float, float>();
}

TEST(PerChannelBuffersReallocationTest, Quantized) {
  TestPerChannelBuffersReallocation<std::int8_t, std::int8_t, std::int32_t,
                                    std::int8_t>();
}

TEST(PerChannelBuffersReallocationTest, RawInt32) {
  TestPerChannelBuffersReallocation<std::int8_t, std::int8_t, std::int32_t,
                                    std::int32_t>();
}

}  // namespace
}  // namespace ruy

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

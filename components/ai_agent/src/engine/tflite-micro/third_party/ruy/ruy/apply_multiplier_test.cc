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

#include "ruy/apply_multiplier.h"

#include <cstdint>
#include <limits>

#include "ruy/gtest_wrapper.h"
#include "ruy/mul_params.h"

namespace ruy {
namespace {

void TestMultiplyByQuantizedMultiplier(std::int32_t input,
                                       std::int32_t multiplier_fixedpoint,
                                       int multiplier_exponent,
                                       std::int32_t expected_output) {
  EXPECT_EQ(expected_output,
            detail::MultiplyByQuantizedMultiplier(input, multiplier_fixedpoint,
                                                  multiplier_exponent));
}

// These testcases exercise various multiplier_fixedpoint values, leaving
// multiplier_exponent = 0. They exercise the logic in
// SaturatingRoundingDoublingHighMul.
TEST(ApplyMultiplierTest, SaturatingRoundingDoublingHighMul) {
  const std::int32_t max_int32 = std::numeric_limits<std::int32_t>::max();
  TestMultiplyByQuantizedMultiplier(1000, max_int32, 0, 1000);
  TestMultiplyByQuantizedMultiplier(1000, 1 << 30, 0, 500);
  TestMultiplyByQuantizedMultiplier(1000, 1 << 29, 0, 250);
  TestMultiplyByQuantizedMultiplier(1000, (1 << 30) + (1 << 29), 0, 750);
  TestMultiplyByQuantizedMultiplier(1000, (1 << 30) + (1 << 28), 0, 625);
  // This 563 expected value comes from the breaking of the tie on 562.5.
  // As a positive value, it does not distinguish between 'upward' and
  // 'away from zero' tie-breaking behavior.
  TestMultiplyByQuantizedMultiplier(1000, (1 << 30) + (1 << 27), 0, 563);
  TestMultiplyByQuantizedMultiplier(1000, (1 << 30) + (1 << 26), 0, 531);
  TestMultiplyByQuantizedMultiplier(-1000, max_int32, 0, -1000);
  TestMultiplyByQuantizedMultiplier(-1000, 1 << 30, 0, -500);
  TestMultiplyByQuantizedMultiplier(-1000, 1 << 29, 0, -250);
  TestMultiplyByQuantizedMultiplier(-1000, (1 << 30) + (1 << 29), 0, -750);
  TestMultiplyByQuantizedMultiplier(-1000, (1 << 30) + (1 << 28), 0, -625);
  // This -562 expected value is because the SaturatingRoundingDoublingHighMul
  // breaks ties upwards, not away-from-zero. The value before rounding is
  // -562.5. No other test case here tests a tie on a negative value.
  TestMultiplyByQuantizedMultiplier(-1000, (1 << 30) + (1 << 27), 0, -562);
  TestMultiplyByQuantizedMultiplier(-1000, (1 << 30) + (1 << 26), 0, -531);
}

// These testcases exercise various negative multiplier_exponent values while
// keeping multiplier_fixedpoint trivial.
TEST(ApplyMultiplierTest, RoundingRightShift) {
  const std::int32_t max_int32 = std::numeric_limits<std::int32_t>::max();
  TestMultiplyByQuantizedMultiplier(1000, max_int32, -1, 500);
  TestMultiplyByQuantizedMultiplier(1000, max_int32, -2, 250);
  TestMultiplyByQuantizedMultiplier(1000, max_int32, -3, 125);
  TestMultiplyByQuantizedMultiplier(1000, max_int32, -4, 62);
  TestMultiplyByQuantizedMultiplier(1000, max_int32, -5, 31);
  TestMultiplyByQuantizedMultiplier(1000, max_int32, -6, 16);
  TestMultiplyByQuantizedMultiplier(-1000, max_int32, -1, -500);
  TestMultiplyByQuantizedMultiplier(-1000, max_int32, -2, -250);
  TestMultiplyByQuantizedMultiplier(-1000, max_int32, -3, -125);
  // This -62 value comes from rounding -62.5, which is a tie.
  // Is the only test case here that exercises a tie-break on a negative value,
  // distinguishing between 'upward' and 'away from zero'.
  TestMultiplyByQuantizedMultiplier(-1000, max_int32, -4, -62);
  TestMultiplyByQuantizedMultiplier(-1000, max_int32, -5, -31);
  TestMultiplyByQuantizedMultiplier(-1000, max_int32, -6, -16);
}

// These testcases exercise various positive multiplier_exponent values while
// keeping multiplier_fixedpoint trivial.
TEST(ApplyMultiplierTest, LeftShift) {
  const std::int32_t max_int32 = std::numeric_limits<std::int32_t>::max();
  TestMultiplyByQuantizedMultiplier(1000, max_int32, 1, 2000);
  TestMultiplyByQuantizedMultiplier(1000, max_int32, 2, 4000);
  TestMultiplyByQuantizedMultiplier(1000, max_int32, 3, 8000);
  TestMultiplyByQuantizedMultiplier(-1000, max_int32, 1, -2000);
  TestMultiplyByQuantizedMultiplier(-1000, max_int32, 2, -4000);
  TestMultiplyByQuantizedMultiplier(-1000, max_int32, 3, -8000);
}

template <typename AccumScalar, typename DstScalar>
void TestApplyMultiplier(const MulParams<AccumScalar, DstScalar>& mul_params,
                         int channel, AccumScalar input,
                         AccumScalar expected_output) {
  AccumScalar actual_output = input;
  ApplyMultiplier(mul_params, channel, &actual_output);
  EXPECT_EQ(expected_output, actual_output);
}

TEST(ApplyMultiplierTest, ApplyMultiplierUniform) {
  MulParams<std::int32_t, std::int8_t> mul_params;
  mul_params.set_multiplier_fixedpoint(1 << 30);
  mul_params.set_multiplier_exponent(-1);
  TestApplyMultiplier(mul_params, 0, 1000, 250);
}

TEST(ApplyMultiplierTest, ApplyMultiplierPerChannel) {
  const std::int32_t max_int32 = std::numeric_limits<std::int32_t>::max();
  const std::int32_t multiplier_fixedpoint[4] = {max_int32, 1 << 30, max_int32,
                                                 1 << 30};
  const int multiplier_exponent[4] = {0, 0, -1, -1};
  MulParams<std::int32_t, std::int8_t> mul_params;
  mul_params.set_multiplier_fixedpoint_perchannel(multiplier_fixedpoint);
  mul_params.set_multiplier_exponent_perchannel(multiplier_exponent);
  TestApplyMultiplier(mul_params, 0, 1000, 1000);
  TestApplyMultiplier(mul_params, 1, 1000, 500);
  TestApplyMultiplier(mul_params, 2, 1000, 500);
  TestApplyMultiplier(mul_params, 3, 1000, 250);
}

}  // namespace
}  // namespace ruy

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

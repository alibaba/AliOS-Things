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

#include "ruy/matrix.h"

#include "ruy/gtest_wrapper.h"

namespace ruy {
namespace {

TEST(MatrixTest, LayoutClassSanity) {
  Layout layout;
  EXPECT_EQ(layout.rows(), 0);
  EXPECT_EQ(layout.cols(), 0);
  EXPECT_EQ(layout.stride(), 0);
  EXPECT_EQ(layout.order(), Order::kColMajor);
  layout.set_rows(123);
  layout.set_cols(456);
  layout.set_stride(789);
  layout.set_order(Order::kRowMajor);
  EXPECT_EQ(layout.rows(), 123);
  EXPECT_EQ(layout.cols(), 456);
  EXPECT_EQ(layout.stride(), 789);
  EXPECT_EQ(layout.order(), Order::kRowMajor);
}

TEST(MatrixTest, MakeSimpleLayout) {
  Layout layout;
  MakeSimpleLayout(123, 456, Order::kColMajor, &layout);
  EXPECT_EQ(layout.rows(), 123);
  EXPECT_EQ(layout.cols(), 456);
  EXPECT_EQ(layout.stride(), 123);
  EXPECT_EQ(layout.order(), Order::kColMajor);
  MakeSimpleLayout(321, 654, Order::kRowMajor, &layout);
  EXPECT_EQ(layout.rows(), 321);
  EXPECT_EQ(layout.cols(), 654);
  EXPECT_EQ(layout.stride(), 654);
  EXPECT_EQ(layout.order(), Order::kRowMajor);
}

TEST(MatrixTest, ConstCheckingPtrSanity) {
  using PtrType = detail::ConstCheckingPtr<int>;
  PtrType ptr;
  int some_nonconst;
  const int some_const = 0;
  EXPECT_EQ(ptr.get(), nullptr);
  ptr.set(&some_nonconst);
  EXPECT_EQ(static_cast<const PtrType&>(ptr).get(), &some_nonconst);
  EXPECT_EQ(ptr.get(), &some_nonconst);
  ptr.set(&some_const);
  EXPECT_EQ(static_cast<const PtrType&>(ptr).get(), &some_const);
#ifndef NDEBUG
  RUY_ASSERT_DEATH(ptr.get(), "");
#endif
}

TEST(MatrixTest, MatrixClassSanity) {
  Matrix<int> matrix;
  EXPECT_EQ(matrix.data(), nullptr);
  EXPECT_EQ(matrix.zero_point(), 0);
  EXPECT_EQ(matrix.cache_policy(), CachePolicy::kNeverCache);
  EXPECT_EQ(matrix.layout().rows(), 0);
  EXPECT_EQ(matrix.layout().cols(), 0);
  EXPECT_EQ(matrix.layout().stride(), 0);
  EXPECT_EQ(matrix.layout().order(), Order::kColMajor);
  const int some_const = 0;
  matrix.set_data(&some_const);
  matrix.set_zero_point(123);
  matrix.set_cache_policy(CachePolicy::kAlwaysCache);
  MakeSimpleLayout(12, 34, Order::kRowMajor, matrix.mutable_layout());
  EXPECT_EQ(static_cast<const Matrix<int>&>(matrix).data(), &some_const);
#ifndef NDEBUG
  RUY_ASSERT_DEATH(matrix.data(), "");
#endif
  EXPECT_EQ(matrix.zero_point(), 123);
  EXPECT_EQ(matrix.cache_policy(), CachePolicy::kAlwaysCache);
  EXPECT_EQ(matrix.layout().rows(), 12);
  EXPECT_EQ(matrix.layout().cols(), 34);
  EXPECT_EQ(matrix.layout().stride(), 34);
  EXPECT_EQ(matrix.layout().order(), Order::kRowMajor);
}

}  // namespace
}  // namespace ruy

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

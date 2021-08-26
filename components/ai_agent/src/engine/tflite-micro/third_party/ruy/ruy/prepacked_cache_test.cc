/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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

#include "ruy/prepacked_cache.h"

#include <thread>  // NOLINT(build/c++11)

#include "ruy/context.h"
#include "ruy/context_get_ctx.h"
#include "ruy/gtest_wrapper.h"
#include "ruy/mat.h"
#include "ruy/matrix.h"
#include "ruy/ruy.h"
#include "ruy/time.h"

namespace ruy {
namespace {

PEMat MakeDummyPEMat(Type data_type, int rows, int cols) {
  PEMat ret;
  ret.data_type = data_type;
  if (!data_type.is_floating_point) {
    ret.sums_type = Type::Create<std::int32_t>();
  }
  ret.layout.rows = rows;
  ret.layout.cols = cols;
  ret.layout.stride = rows;
  ret.layout.order = Order::kColMajor;
  // The kernel block layout is not relevant to this test, so we leave it
  // trivial 1x1.
  ret.layout.kernel.rows = 1;
  ret.layout.kernel.cols = 1;
  return ret;
}

template <typename T>
void DummyPack(const std::vector<T>& data, PEMat* packed_matrix) {
  EXPECT_EQ(data.size(), FlatSize(packed_matrix->layout));
  memcpy(packed_matrix->data, data.data(), data.size() * sizeof(T));
}

TEST(PrepackedCacheTest, TestCacheBasic) {
  PrepackedCache prepacked_cache(307);
  // Allocate the prepacked matrix.
  // DataBytes=200, SumsBytes=20*4=80, Total: 280 bytes
  std::vector<std::uint8_t> data1(10 * 20);
  PEMat mat1 = MakeDummyPEMat(Type::Create<std::uint8_t>(), 10, 20);
  EXPECT_TRUE(prepacked_cache.Get(data1.data(), &mat1) ==
              PrepackedCache::Action::kInsertedNewEntry);
  DummyPack(data1, &mat1);

  // DataBytes=15, SumsBytes=3*4=12, Total: 27 bytes
  std::vector<std::uint8_t> data2(5 * 3);
  PEMat mat2 = MakeDummyPEMat(Type::Create<std::uint8_t>(), 5, 3);
  EXPECT_TRUE(prepacked_cache.Get(data2.data(), &mat2) ==
              PrepackedCache::Action::kInsertedNewEntry);
  DummyPack(data2, &mat2);

  // Both should now be in cache.
  EXPECT_EQ(prepacked_cache.MatrixCount(), 2);
  EXPECT_EQ(prepacked_cache.BuffersBytes(), 307);
  EXPECT_TRUE(prepacked_cache.Get(data1.data(), &mat1) ==
              PrepackedCache::Action::kGotExistingEntry);
  EXPECT_TRUE(prepacked_cache.Get(data2.data(), &mat2) ==
              PrepackedCache::Action::kGotExistingEntry);
}

TEST(PrepackedCacheTest, TestCacheBasicFloat) {
  PrepackedCache prepacked_cache(860);
  // Allocate the prepacked matrix.
  // DataBytes=200*4, SumsBytes=0 because float, Total: 800 bytes
  std::vector<float> data1(10 * 20);
  PEMat mat1 = MakeDummyPEMat(Type::Create<float>(), 10, 20);
  EXPECT_TRUE(prepacked_cache.Get(data1.data(), &mat1) ==
              PrepackedCache::Action::kInsertedNewEntry);
  DummyPack(data1, &mat1);

  // DataBytes=15*4, SumsBytes=0 because float, Total: 60 bytes
  std::vector<float> data2(5 * 3);
  PEMat mat2 = MakeDummyPEMat(Type::Create<float>(), 5, 3);
  EXPECT_TRUE(prepacked_cache.Get(data2.data(), &mat2) ==
              PrepackedCache::Action::kInsertedNewEntry);
  DummyPack(data2, &mat2);

  // Both should now be in cache.
  EXPECT_EQ(prepacked_cache.MatrixCount(), 2);
  EXPECT_EQ(prepacked_cache.BuffersBytes(), 860);
  EXPECT_TRUE(prepacked_cache.Get(data1.data(), &mat1) ==
              PrepackedCache::Action::kGotExistingEntry);
  EXPECT_TRUE(prepacked_cache.Get(data2.data(), &mat2) ==
              PrepackedCache::Action::kGotExistingEntry);
}

TEST(PrepackedCacheTest, TestCacheEjection) {
  PrepackedCache prepacked_cache(306);
  // Allocate the prepacked matrix.
  // DataBytes=200, SumsBytes=20*4=80, Total: 280 bytes
  std::vector<std::uint8_t> data1(10 * 20);
  PEMat mat1 = MakeDummyPEMat(Type::Create<std::uint8_t>(), 10, 20);
  prepacked_cache.Get(data1.data(), &mat1);
  DummyPack(data1, &mat1);

  // DataBytes=15, SumsBytes=3*4=12, Total: 27 bytes
  std::vector<std::uint8_t> data2(5 * 3);
  PEMat mat2 = MakeDummyPEMat(Type::Create<std::uint8_t>(), 5, 3);
  prepacked_cache.Get(data2.data(), &mat2);
  DummyPack(data2, &mat2);

  // The first matrix should have been ejected from the cache.
  // Only the second matrix should now be in cache.
  EXPECT_EQ(prepacked_cache.MatrixCount(), 1);
  EXPECT_EQ(prepacked_cache.BuffersBytes(), 27);
  EXPECT_TRUE(prepacked_cache.Get(data2.data(), &mat2) ==
              PrepackedCache::Action::kGotExistingEntry);
  EXPECT_TRUE(prepacked_cache.Get(data1.data(), &mat1) ==
              PrepackedCache::Action::kInsertedNewEntry);

  // The second matrix should have been ejected from the cache.
  // Only the first matrix should now be in cache.
  EXPECT_EQ(prepacked_cache.MatrixCount(), 1);
  EXPECT_EQ(prepacked_cache.BuffersBytes(), 280);
  EXPECT_TRUE(prepacked_cache.Get(data1.data(), &mat1) ==
              PrepackedCache::Action::kGotExistingEntry);
  EXPECT_TRUE(prepacked_cache.Get(data2.data(), &mat2) ==
              PrepackedCache::Action::kInsertedNewEntry);
}

TEST(PrepackedCacheTest, TestCacheEjection2) {
  PrepackedCache prepacked_cache(1000);
  // Allocate the prepacked matrix 1.
  // DataBytes=200, SumsBytes=20*4=80, Total: 280 bytes
  std::vector<std::uint8_t> data1(10 * 20);
  PEMat mat1 = MakeDummyPEMat(Type::Create<std::uint8_t>(), 10, 20);
  prepacked_cache.Get(data1.data(), &mat1);
  DummyPack(data1, &mat1);

  // Allocate the prepacked matrix 2.
  // DataBytes=200, SumsBytes=20*4=80, Total: 280 bytes
  std::vector<std::uint8_t> data2(10 * 20);
  PEMat mat2 = MakeDummyPEMat(Type::Create<std::uint8_t>(), 10, 20);
  prepacked_cache.Get(data2.data(), &mat2);
  DummyPack(data2, &mat2);

  // Allocate the prepacked matrix 3.
  // DataBytes=200, SumsBytes=20*4=80, Total: 280 bytes
  std::vector<std::uint8_t> data3(10 * 20);
  PEMat mat3 = MakeDummyPEMat(Type::Create<std::uint8_t>(), 10, 20);
  prepacked_cache.Get(data3.data(), &mat3);
  DummyPack(data3, &mat3);

  // The next insertion will cause the cache size to go over the ejection
  // threshold. Touch matrix 1 and matrix 3 to make matrix 2 the oldest
  EXPECT_TRUE(prepacked_cache.Get(data1.data(), &mat1) ==
              PrepackedCache::Action::kGotExistingEntry);
  EXPECT_TRUE(prepacked_cache.Get(data3.data(), &mat3) ==
              PrepackedCache::Action::kGotExistingEntry);

  // Allocate the prepacked matrix 4.
  // DataBytes=200, SumsBytes=20*4=80, Total: 280 bytes
  std::vector<std::uint8_t> data4(10 * 20);
  PEMat mat4 = MakeDummyPEMat(Type::Create<std::uint8_t>(), 10, 20);
  prepacked_cache.Get(data4.data(), &mat4);
  DummyPack(data4, &mat4);

  // Ensure that mat2 was ejected, but mat1, mat3, and mat4 were not.
  EXPECT_EQ(prepacked_cache.MatrixCount(), 3);
  EXPECT_TRUE(prepacked_cache.Get(data1.data(), &mat1) ==
              PrepackedCache::Action::kGotExistingEntry);
  EXPECT_TRUE(prepacked_cache.Get(data3.data(), &mat3) ==
              PrepackedCache::Action::kGotExistingEntry);
  EXPECT_TRUE(prepacked_cache.Get(data4.data(), &mat4) ==
              PrepackedCache::Action::kGotExistingEntry);
  EXPECT_TRUE(prepacked_cache.Get(data2.data(), &mat2) ==
              PrepackedCache::Action::kInsertedNewEntry);
}

TEST(PrepackedCacheTest, TestDistinguishSubtlyDifferentMatrices) {
  PrepackedCache prepacked_cache;

  std::vector<std::uint8_t> data(10 * 20);
  PEMat mat = MakeDummyPEMat(Type::Create<std::uint8_t>(), 10, 20);
  EXPECT_EQ(prepacked_cache.Get(data.data(), &mat),
            PrepackedCache::Action::kInsertedNewEntry);

  // Same layout, different source data pointer
  EXPECT_EQ(prepacked_cache.Get(data.data() + 1, &mat),
            PrepackedCache::Action::kInsertedNewEntry);

  // Layout tweaks
  mat.layout.rows = 9;
  EXPECT_EQ(prepacked_cache.Get(data.data(), &mat),
            PrepackedCache::Action::kInsertedNewEntry);

  mat.layout.cols = 19;
  EXPECT_EQ(prepacked_cache.Get(data.data(), &mat),
            PrepackedCache::Action::kInsertedNewEntry);

  mat.layout.order = Order::kRowMajor;
  EXPECT_EQ(prepacked_cache.Get(data.data(), &mat),
            PrepackedCache::Action::kInsertedNewEntry);

  mat.layout.kernel.rows = 2;
  EXPECT_EQ(prepacked_cache.Get(data.data(), &mat),
            PrepackedCache::Action::kInsertedNewEntry);

  mat.layout.kernel.cols = 2;
  EXPECT_EQ(prepacked_cache.Get(data.data(), &mat),
            PrepackedCache::Action::kInsertedNewEntry);

  mat.layout.kernel.order = Order::kRowMajor;
  EXPECT_EQ(prepacked_cache.Get(data.data(), &mat),
            PrepackedCache::Action::kInsertedNewEntry);

  EXPECT_EQ(prepacked_cache.MatrixCount(), 8);
}

void TestCachePolicies(CachePolicy cache_policy, bool expected_cached) {
  ruy::Context context;
  ruy::Ctx* ctx = get_ctx(&context);
  PrepackedCache* cache = ctx->GetPrepackedCache();
  EXPECT_EQ(cache->MatrixCount(), 0);

  const float lhs_data[] = {1, 2, 3, 4};
  const float rhs_data[] = {1, 2};
  float dst_data[4];

  ruy::Matrix<float> lhs;
  ruy::MakeSimpleLayout(2, 2, ruy::Order::kRowMajor, lhs.mutable_layout());
  lhs.set_data(lhs_data);
  ruy::Matrix<float> rhs;
  ruy::MakeSimpleLayout(2, 1, ruy::Order::kColMajor, rhs.mutable_layout());
  rhs.set_data(rhs_data);
  ruy::Matrix<float> dst;
  ruy::MakeSimpleLayout(2, 1, ruy::Order::kColMajor, dst.mutable_layout());
  dst.set_data(dst_data);

  ruy::MulParams<float, float> mul_params;
  // Perform the multiplication and confirm no caching occurred.
  ruy::Mul<ruy::kAllPaths>(lhs, rhs, mul_params, &context, &dst);
  EXPECT_EQ(cache->MatrixCount(), 0);

  // Set cache policy for the LHS, repeat the multiplication, and see
  // that caching did occur.
  lhs.set_cache_policy(cache_policy);
  ruy::Mul<ruy::kAllPaths>(lhs, rhs, mul_params, &context, &dst);
  const bool actual_cached = cache->MatrixCount() == 1;
  EXPECT_EQ(actual_cached, expected_cached);
}

TEST(PrepackedCacheTest, TestCachePolicies) {
  for (CachePolicy cache_policy :
       {CachePolicy::kNeverCache, CachePolicy::kCacheIfLargeSpeedup,
        CachePolicy::kCacheIfSignificantSpeedup, CachePolicy::kAlwaysCache}) {
    TestCachePolicies(cache_policy,
                         cache_policy != CachePolicy::kNeverCache);
  }
}

TEST(PrepackedCacheTest, TestClearCache) {
  ruy::Context context;
  PrepackedCache* cache = get_ctx(&context)->GetPrepackedCache();
  EXPECT_EQ(cache->MatrixCount(), 0);

  const float lhs_data[] = {1, 2, 3, 4};
  const float rhs_data[] = {1, 2};
  float dst_data[4];

  ruy::Matrix<float> lhs;
  ruy::MakeSimpleLayout(2, 2, ruy::Order::kRowMajor, lhs.mutable_layout());
  lhs.set_data(lhs_data);
  ruy::Matrix<float> rhs;
  ruy::MakeSimpleLayout(2, 1, ruy::Order::kColMajor, rhs.mutable_layout());
  rhs.set_data(rhs_data);
  ruy::Matrix<float> dst;
  ruy::MakeSimpleLayout(2, 1, ruy::Order::kColMajor, dst.mutable_layout());
  dst.set_data(dst_data);

  ruy::MulParams<float, float> mul_params;
  // Set cache policy for the LHS and see that caching occurs.
  lhs.set_cache_policy(CachePolicy::kAlwaysCache);
  ruy::Mul<ruy::kAllPaths>(lhs, rhs, mul_params, &context, &dst);
  EXPECT_NE(cache->MatrixCount(), 0);

  // Clear the cache via the Context.
  context.ClearPrepackedCache();
  // Verify that the cache is now empty.
  cache = get_ctx(&context)->GetPrepackedCache();
  EXPECT_EQ(cache->MatrixCount(), 0);
}

}  // namespace
}  // namespace ruy

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

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

#include "ruy/ctx_impl.h"
#include "ruy/gtest_wrapper.h"
#include "ruy/path.h"
#include "ruy/platform.h"

namespace ruy {
namespace {

TEST(ContextInternalTest, EnabledPathsGeneral) {
  CtxImpl ctx;
  const auto ruy_paths = ctx.GetRuntimeEnabledPaths();
  const auto ruy_paths_repeat = ctx.GetRuntimeEnabledPaths();
  ASSERT_EQ(ruy_paths, ruy_paths_repeat);
  EXPECT_NE(ruy_paths, Path::kNone);
  EXPECT_EQ(ruy_paths & Path::kStandardCpp, Path::kStandardCpp);
}

#if RUY_PLATFORM_X86
TEST(ContextInternalTest, EnabledPathsX86Explicit) {
  CtxImpl ctx;
  ctx.SetRuntimeEnabledPaths(Path::kAvx2Fma);
  const auto ruy_paths = ctx.GetRuntimeEnabledPaths();
  EXPECT_EQ(ruy_paths, Path::kStandardCpp | Path::kAvx2Fma);
}
#endif  // RUY_PLATFORM_X86

#if RUY_PLATFORM_ARM
TEST(ContextInternalTest, EnabledPathsX86Explicit) {
  CtxImpl ctx;
  ctx.SetRuntimeEnabledPaths(Path::kNeonDotprod);
  const auto ruy_paths = ctx.GetRuntimeEnabledPaths();
  EXPECT_EQ(ruy_paths, Path::kStandardCpp | Path::kNeonDotprod);
}

TEST(ContextInternalTest, EnabledPathsArmDefault) {
  CtxImpl ctx;
  const auto ruy_paths = ctx.GetRuntimeEnabledPaths();
  EXPECT_EQ(ruy_paths & Path::kStandardCpp, Path::kStandardCpp);
  // NEON is always assumed to be supported at the moment.
  EXPECT_EQ(ruy_paths & Path::kNeon, Path::kNeon);
}
#endif  // RUY_PLATFORM_ARM

TEST(ContextInternalTest, ThreadSpecificResources) {
  CtxImpl ctx;
  for (int i = 1; i <= 4; i++) {
    ctx.EnsureThreadSpecificResources(i);
    for (int j = 0; j < i; j++) {
      EXPECT_NE(ctx.GetThreadSpecificAllocator(j), nullptr);
      EXPECT_NE(ctx.GetThreadSpecificTuningResolver(j), nullptr);
    }
  }
}

}  // namespace
}  // namespace ruy

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

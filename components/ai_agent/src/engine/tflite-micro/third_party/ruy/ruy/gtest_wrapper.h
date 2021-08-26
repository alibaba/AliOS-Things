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

// Wrapper around GTest that works around warnings and inconsistencies.

#ifndef THIRD_PARTY_RUY_RUY_GTEST_WRAPPER_H_
#define THIRD_PARTY_RUY_RUY_GTEST_WRAPPER_H_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "gtest/gtest.h"  // IWYU pragma: export
#pragma GCC diagnostic pop

// When building for WAsm, ASSERT_DEATH is not defined.
#ifdef ASSERT_DEATH
#define RUY_ASSERT_DEATH(CONDITION, MESSAGE) ASSERT_DEATH(CONDITION, MESSAGE)
#else
#define RUY_ASSERT_DEATH(CONDITION, MESSAGE)
#endif

#endif  // THIRD_PARTY_RUY_RUY_GTEST_WRAPPER_H_

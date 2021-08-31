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

// Some helpers to write inline asm.

#ifndef RUY_RUY_ASM_HELPERS_H_
#define RUY_RUY_ASM_HELPERS_H_

#include "ruy/opt_set.h"

// Enclose load-prefetch instructions in RUY_PREFETCH_LOAD() so we can
// conditionally enable them based on the RUY_OPT_SET.
#if RUY_OPT(PREFETCH_LOAD)
#define RUY_PREFETCH_LOAD(X) X
#else
#define RUY_PREFETCH_LOAD(X)
#endif

// Enclose store-prefetch instructions in RUY_PREFETCH_STORE() so we can
// conditionally enable them based on the RUY_OPT_SET.
#if RUY_OPT(PREFETCH_STORE)
#define RUY_PREFETCH_STORE(X) X
#else
#define RUY_PREFETCH_STORE(X)
#endif

// The usual stringification macro.
#define RUY_STR(s) RUY_STR_UNEXPANDED(s)
#define RUY_STR_UNEXPANDED(s) #s

#endif  // RUY_RUY_ASM_HELPERS_H_

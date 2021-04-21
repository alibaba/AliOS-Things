/*
 * Copyright 1999-2019 Alibaba Cloud All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_GLOBAL_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_GLOBAL_H_

#include "Config.h"

#if defined(_WIN32)
#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif // _MSC_VER
#define ALIBABACLOUD_DECL_EXPORT __declspec(dllexport)
#define ALIBABACLOUD_DECL_IMPORT __declspec(dllimport)
#elif defined(__linux__)
#define ALIBABACLOUD_DECL_EXPORT __attribute__((visibility("default")))
#define ALIBABACLOUD_DECL_IMPORT __attribute__((visibility("default")))
#endif

#if !defined(ALIBABACLOUD_DECL_EXPORT)
#define ALIBABACLOUD_DECL_EXPORT
#endif

#if !defined(ALIBABACLOUD_DECL_IMPORT)
#define ALIBABACLOUD_DECL_IMPORT
#endif

#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_GLOBAL_H_

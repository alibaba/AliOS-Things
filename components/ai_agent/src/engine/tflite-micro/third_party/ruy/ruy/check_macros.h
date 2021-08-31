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

// self-contained, minimal, CHECK/DCHECK macros similar to glog.

#ifndef RUY_RUY_CHECK_MACROS_H_
#define RUY_RUY_CHECK_MACROS_H_

#include <cstdio>
#include <cstdlib>
#include <functional>
#include <type_traits>

namespace ruy {
namespace check_macros {

constexpr int kValueBufSize = 32;

template <typename T, typename Enable = void>
struct ToString {
  static void Run(const T&, char* buf) { snprintf(buf, kValueBufSize, "(?)"); }
};

template <>
struct ToString<float, void> {
  static void Run(float value, char* buf) {
    snprintf(buf, kValueBufSize, "%.9g", static_cast<double>(value));
  }
};

template <>
struct ToString<double, void> {
  static void Run(double value, char* buf) {
    snprintf(buf, kValueBufSize, "%.16g", value);
  }
};

template <typename T>
struct ToString<T, typename std::enable_if<std::is_integral<T>::value>::type> {
  static void Run(const T& value, char* buf) {
    snprintf(buf, kValueBufSize, "%lld", static_cast<long long>(value));
  }
};

template <typename T>
struct ToString<T*, void> {
  static void Run(T* value, char* buf) {
    snprintf(buf, kValueBufSize, "%p", value);
  }
};

template <typename T>
struct ToString<T, typename std::enable_if<std::is_enum<T>::value>::type> {
  static void Run(const T& value, char* buf) {
    snprintf(buf, kValueBufSize, "(enum value %d)", static_cast<int>(value));
  }
};

inline void CheckImpl(bool condition, const char* file, int line,
                      const char* macro, const char* condition_str) {
  if (!condition) {
    fprintf(stderr, "%s:%d: %s condition not satisfied: %s\n", file, line,
            macro, condition_str);
    abort();
  }
}

template <template <typename T> class Comparison, typename LhsType,
          typename RhsType>
inline void CheckImpl(const char* file, int line, const char* macro,
                      const char* lhs, const LhsType& lhs_value,
                      const char* op_symbol, const char* rhs,
                      const RhsType& rhs_value) {
  using CommonType = typename std::common_type<LhsType, RhsType>::type;
  if (!Comparison<CommonType>()(lhs_value, rhs_value)) {
    char lhs_value_buf[kValueBufSize];
    ToString<LhsType>::Run(lhs_value, lhs_value_buf);
    char rhs_value_buf[kValueBufSize];
    ToString<RhsType>::Run(rhs_value, rhs_value_buf);
    fprintf(
        stderr,
        "%s:%d: %s condition not satisfied:   [ %s %s %s ]   with values   [ "
        "%s %s %s ].\n",
        file, line, macro, lhs, op_symbol, rhs, lhs_value_buf, op_symbol,
        rhs_value_buf);
    abort();
  }
}

#define RUY_CHECK_IMPL(macro, condition)                              \
  ruy::check_macros::CheckImpl(condition, __FILE__, __LINE__, #macro, \
                               #condition)

#define RUY_CHECK_OP_IMPL(macro, lhs, op_symbol, op_comparison, rhs) \
  ruy::check_macros::CheckImpl<op_comparison>(                       \
      __FILE__, __LINE__, #macro, #lhs, lhs, #op_symbol, #rhs, rhs)

#define RUY_CHECK(condition) RUY_CHECK_IMPL(RUY_CHECK, condition)
#define RUY_CHECK_EQ(x, y) \
  RUY_CHECK_OP_IMPL(RUY_CHECK_EQ, x, ==, std::equal_to, y)
#define RUY_CHECK_NE(x, y) \
  RUY_CHECK_OP_IMPL(RUY_CHECK_NE, x, !=, std::not_equal_to, y)
#define RUY_CHECK_GE(x, y) \
  RUY_CHECK_OP_IMPL(RUY_CHECK_GE, x, >=, std::greater_equal, y)
#define RUY_CHECK_GT(x, y) \
  RUY_CHECK_OP_IMPL(RUY_CHECK_GT, x, >, std::greater, y)
#define RUY_CHECK_LE(x, y) \
  RUY_CHECK_OP_IMPL(RUY_CHECK_LE, x, <=, std::less_equal, y)
#define RUY_CHECK_LT(x, y) RUY_CHECK_OP_IMPL(RUY_CHECK_LT, x, <, std::less, y)

#ifdef NDEBUG
#define RUY_DCHECK_IS_ENABLED false
#else
#define RUY_DCHECK_IS_ENABLED true
#endif

#define RUY_DCHECK(condition) \
  if (RUY_DCHECK_IS_ENABLED) RUY_CHECK(condition)
#define RUY_DCHECK_EQ(x, y) \
  if (RUY_DCHECK_IS_ENABLED) RUY_CHECK_EQ(x, y)
#define RUY_DCHECK_NE(x, y) \
  if (RUY_DCHECK_IS_ENABLED) RUY_CHECK_NE(x, y)
#define RUY_DCHECK_GE(x, y) \
  if (RUY_DCHECK_IS_ENABLED) RUY_CHECK_GE(x, y)
#define RUY_DCHECK_GT(x, y) \
  if (RUY_DCHECK_IS_ENABLED) RUY_CHECK_GT(x, y)
#define RUY_DCHECK_LE(x, y) \
  if (RUY_DCHECK_IS_ENABLED) RUY_CHECK_LE(x, y)
#define RUY_DCHECK_LT(x, y) \
  if (RUY_DCHECK_IS_ENABLED) RUY_CHECK_LT(x, y)

}  // end namespace check_macros
}  // end namespace ruy

#endif  // RUY_RUY_CHECK_MACROS_H_

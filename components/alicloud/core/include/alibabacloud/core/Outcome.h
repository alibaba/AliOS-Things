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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_OUTCOME_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_OUTCOME_H_

#include <utility>

namespace AlibabaCloud {
template <typename E, typename R> class Outcome {
public:
  Outcome() : success_(true), e_(), r_() {}
  explicit Outcome(const E &e) : e_(e), success_(false), r_() {}
  explicit Outcome(const R &r) : r_(r), success_(true), e_() {}
  Outcome(const Outcome &other)
      : success_(other.success_), e_(other.e_), r_(other.r_) {}
  Outcome(Outcome &&other) { *this = std::move(other); }
  Outcome &operator=(const Outcome &other) {
    if (this != &other) {
      success_ = other.success_;
      e_ = other.e_;
      r_ = other.r_;
    }
    return *this;
  }
  Outcome &operator=(Outcome &&other) {
    if (this != &other) {
      success_ = other.success_;
      r_ = other.r_;
      e_ = other.e_;
    }
    return *this;
  }

  bool isSuccess() const { return success_; }
  E error() const { return e_; }
  R result() const { return r_; }

private:
  bool success_;
  E e_;
  R r_;
};
} // namespace AlibabaCloud
#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_OUTCOME_H_

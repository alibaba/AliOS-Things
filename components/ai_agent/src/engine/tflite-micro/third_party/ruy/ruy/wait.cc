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

#include "ruy/wait.h"

#include <chrono>  // NOLINT(build/c++11)

namespace ruy {

void Wait(const std::function<bool()>& condition, const Duration& spin_duration,
          std::condition_variable* condvar, std::mutex* mutex) {
  // First, trivial case where the `condition` is already true;
  if (condition()) {
    return;
  }

  // Then, if spin_duration is nonzero, try busy-waiting.
  if (spin_duration.count() > 0) {
    const TimePoint wait_start = Now();
    while (Now() - wait_start < spin_duration) {
      if (condition()) {
        return;
      }
    }
  }

  // Finally, do real passive waiting.
  std::unique_lock<std::mutex> lock(*mutex);
  condvar->wait(lock, condition);
}

}  // namespace ruy

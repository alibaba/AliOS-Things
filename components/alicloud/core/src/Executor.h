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

#ifndef CORE_SRC_EXECUTOR_H_
#define CORE_SRC_EXECUTOR_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace AlibabaCloud {
class Runnable;
class Executor {
public:
  Executor();
  ~Executor();

  static Executor *instance();
  void execute(Runnable *task);
  bool isShutdown() const;
  bool start();
  void shutdown();
  void wakeUp();

private:
  static Executor *self_;
  std::atomic<bool> shutdown_;
  std::queue<Runnable *> tasksQueue_;
  std::mutex tasksQueueMutex_;
  std::thread thread_;
  std::condition_variable cv_;
  std::mutex cvMutex_;
};
} // namespace AlibabaCloud

#endif // CORE_SRC_EXECUTOR_H_

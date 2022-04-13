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

#include "Executor.h"
#include <alibabacloud/core/Runnable.h>

namespace AlibabaCloud {

Executor *Executor::self_ = nullptr;

Executor::Executor()
    : cvMutex_(), shutdown_(true), tasksQueue_(), tasksQueueMutex_(),
      thread_() {
  self_ = this;
}

Executor::~Executor() {
  self_ = nullptr;
  shutdown();
}

Executor *Executor::instance() { return self_; }

bool Executor::start() {
  if (!isShutdown())
    return true;

  auto threadMain = [this]() {
    while (!shutdown_) {
      while (!tasksQueue_.empty()) {
        Runnable *task = nullptr;
        {
          std::lock_guard<std::mutex> lock(tasksQueueMutex_);
          if (!tasksQueue_.empty()) {
            task = tasksQueue_.front();
            tasksQueue_.pop();
          }
        }

        if (task) {
          task->run();
          delete task;
        }
      }

      if (!shutdown_) {
        std::unique_lock<std::mutex> lk(cvMutex_);
        cv_.wait(lk);
      }
    }
  };

  shutdown_ = false;
  thread_ = std::thread(threadMain);
  return true;
}

bool Executor::isShutdown() const { return shutdown_; }

void Executor::execute(Runnable *task) {
  if (isShutdown())
    return;

  std::lock_guard<std::mutex> locker(tasksQueueMutex_);
  tasksQueue_.push(task);
  wakeUp();
}

void Executor::wakeUp() {
  std::unique_lock<std::mutex> lk(cvMutex_);
  cv_.notify_one();
}

void Executor::shutdown() {
  if (isShutdown())
    return;

  {
    std::lock_guard<std::mutex> locker(tasksQueueMutex_);
    while (tasksQueue_.size() > 0) {
      auto task = tasksQueue_.front();
      delete task;
      tasksQueue_.pop();
    }
  }

  shutdown_ = true;
  wakeUp();

  if (thread_.joinable())
    thread_.join();
}

} // namespace AlibabaCloud

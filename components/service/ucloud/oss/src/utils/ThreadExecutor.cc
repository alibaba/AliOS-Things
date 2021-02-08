/*
* Copyright 2009-2017 Alibaba Cloud All rights reserved.
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

#include "ThreadExecutor.h"
#include <assert.h>
#include "../utils/LogUtils.h"

using namespace AlibabaCloud::OSS;

static const char *TAG = "ThreadExecutor";

ThreadExecutor::ThreadExecutor():
    state_(State::Free)
{
}

ThreadExecutor::~ThreadExecutor()
{
    auto expected = State::Free;
    while (!state_.compare_exchange_strong(expected, State::Shutdown))
    {
        assert(expected == State::Locked);
        expected = State::Free;
    }

    auto it = threads_.begin();
    while (!threads_.empty())
    {
        it->second.join();
        it = threads_.erase(it);
    }
}

void ThreadExecutor::execute(Runnable* task)
{
    auto main = [task, this] {
        OSS_LOG(LogLevel::LogDebug, TAG, "task(%p) enter execute main thread", task);
        task->run();
        delete task;
        detach(std::this_thread::get_id());
        OSS_LOG(LogLevel::LogDebug, TAG, "task(%p) leave execute main thread", task);
    };

    State expected;
    do
    {
        expected = State::Free;
        if (state_.compare_exchange_strong(expected, State::Locked))
        {
            std::thread t(main);
            const auto id = t.get_id(); 
            threads_.emplace(id, std::move(t));
            state_ = State::Free;
            return;
        }
    } while (expected != State::Shutdown);
    return;
}

void ThreadExecutor::detach(std::thread::id id)
{
    State expected;
    do
    {
        expected = State::Free;
        if (state_.compare_exchange_strong(expected, State::Locked))
        {
            auto it = threads_.find(id);
            assert(it != threads_.end());
            it->second.detach();
            threads_.erase(it);
            state_ = State::Free;
            return;
        }
    } while (expected != State::Shutdown);
}
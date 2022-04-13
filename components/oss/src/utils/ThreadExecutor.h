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

#pragma once

#include <alibabacloud/oss/utils/Executor.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ThreadExecutor: public Executor
    {
    public:
        ThreadExecutor();
        virtual ~ThreadExecutor();
        void execute(Runnable* task);
    private:
        enum class State
        {
            Free, Locked, Shutdown
        };
        void detach(std::thread::id id);
        std::atomic<State> state_;
        std::unordered_map<std::thread::id, std::thread> threads_;
    };
}
}

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

#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <alibabacloud/oss/http/HttpRequest.h>
#include <alibabacloud/oss/http/HttpResponse.h>

namespace AlibabaCloud
{
namespace OSS
{

    class ALIBABACLOUD_OSS_EXPORT HttpClient
    {
    public:
        HttpClient();
        virtual ~HttpClient();

        virtual std::shared_ptr<HttpResponse> makeRequest(const std::shared_ptr<HttpRequest> &request) = 0;

        bool isEnable();
        void disable();
        void enable();
        void waitForRetry(long milliseconds);
        
    protected:
        std::atomic<bool> disable_;
        std::mutex requestLock_;
        std::condition_variable requestSignal_;
    };
}
}

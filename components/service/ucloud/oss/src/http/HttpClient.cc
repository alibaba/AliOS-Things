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

#include <alibabacloud/oss/http/HttpClient.h>


using namespace AlibabaCloud::OSS;

HttpClient::HttpClient():
    disable_(false)
{
}

HttpClient::~HttpClient()
{
}

bool HttpClient::isEnable()
{
    return disable_.load() == false;
}

void HttpClient::disable()
{
    disable_ = true;
    requestSignal_.notify_all();
}

void HttpClient::enable()
{
    disable_ = false;
}

void HttpClient::waitForRetry(long milliseconds)
{
    if (milliseconds == 0)
        return;
    std::unique_lock<std::mutex> lck(requestLock_);
    requestSignal_.wait_for(lck, std::chrono::milliseconds(milliseconds), [this] ()-> bool { return disable_.load() == true; });
}


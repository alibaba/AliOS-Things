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

#include <alibabacloud/oss/client/ClientConfiguration.h>
#include <alibabacloud/oss/http/HttpClient.h>

namespace AlibabaCloud
{
namespace OSS
{

    class CurlContainer;
    class RateLimiter;

    class CurlHttpClient : public HttpClient
    {
    public:
        CurlHttpClient(const ClientConfiguration &configuration);
        ~CurlHttpClient();

        static void initGlobalState();
        static void cleanupGlobalState();

        virtual std::shared_ptr<HttpResponse> makeRequest(const std::shared_ptr<HttpRequest> &request) override;
    private:
        CurlContainer *curlContainer_;
        std::string userAgent_;
        Http::Scheme proxyScheme_;
        std::string proxyHost_;
        unsigned int proxyPort_;
        std::string proxyUserName_;
        std::string proxyPassword_;
        bool verifySSL_;
        std::string caPath_;
        std::string caFile_;
        std::string networkInterface_;
    public:
        std::shared_ptr<RateLimiter> sendRateLimiter_;
        std::shared_ptr<RateLimiter> recvRateLimiter_;
    };
}
}

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
#include <string>
#include <alibabacloud/oss/auth/CredentialsProvider.h>
#include <alibabacloud/oss/http/HttpClient.h>
#include <alibabacloud/oss/utils/Executor.h>

namespace AlibabaCloud
{
namespace OSS
{
    class RetryStrategy;
    class RateLimiter;
    class ALIBABACLOUD_OSS_EXPORT ClientConfiguration
    {
    public:
        ClientConfiguration();
        ~ClientConfiguration() = default;
    public:
        /**
        * User Agent string user for http calls.
        */
        std::string userAgent;
        /**
        * Http scheme to use. E.g. Http or Https.
        */
        Http::Scheme scheme;
        /**
        * Max concurrent tcp connections for a single http client to use.
        */
        unsigned maxConnections;
        /**
        * Socket read timeouts. Default 3000 ms. 
        */
        long requestTimeoutMs;
        /**
        * Socket connect timeout. 
        */
        long connectTimeoutMs;
        /**
        * Strategy to use in case of failed requests.
        */
        std::shared_ptr<RetryStrategy> retryStrategy;
        /**
        * The proxy scheme. Default HTTP
        */
        Http::Scheme proxyScheme;
        /**
        * The proxy host.
        */
        std::string proxyHost;
        /**
        * The proxy port.
        */
        unsigned int proxyPort;
        /**
        * The proxy username.
        */
        std::string proxyUserName;
        /**
        *  The proxy password
        */
        std::string proxyPassword;
        /**
        * set false to bypass SSL check.
        */
        bool verifySSL;
        /**
        * your Certificate Authority path. 
        */
        std::string caPath;
        /**
        * your certificate file.
        */
        std::string caFile;
        /**
        * your certificate file.
        */
        bool isCname;
        /**
        * enable or disable crc64 check.
        */
        bool enableCrc64;
        /**
        * enable or disable auto correct http request date.
        */
        bool enableDateSkewAdjustment;
        /**
        * Rate limit data upload speed.
        */
        std::shared_ptr<RateLimiter> sendRateLimiter;
        /**
        * Rate limit data download speed.
        */
        std::shared_ptr<RateLimiter> recvRateLimiter;
        /**
        * The interface for outgoing traffic. E.g. eth0 in linux
        */
        std::string networkInterface;
        /**
        * Your executor's implement
        */
        std::shared_ptr<Executor> executor;
        /**
        * Your http client' implement
        */
        std::shared_ptr<HttpClient> httpClient;
    };
}
}

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

#include <functional>
#include <memory>
#include <alibabacloud/oss/ServiceRequest.h>
#include <alibabacloud/oss/ServiceResult.h>
#include <alibabacloud/oss/client/ClientConfiguration.h>
#include <alibabacloud/oss/client/Error.h>
#include <alibabacloud/oss/utils/Outcome.h>
#include <alibabacloud/oss/http/HttpClient.h>

namespace AlibabaCloud
{
namespace OSS
{

    class  Client
    {
    public:
        using ClientOutcome =  Outcome<Error, std::shared_ptr<HttpResponse>> ;

        Client(const std::string & servicename, const ClientConfiguration &configuration);
        virtual ~Client();

        std::string serviceName()const;
        const ClientConfiguration &configuration()const;

        bool isEnableRequest() const;

    protected:
        ClientOutcome AttemptRequest(const std::string & endpoint, const ServiceRequest &request, Http::Method method) const;
        ClientOutcome AttemptOnceRequest(const std::string & endpoint, const ServiceRequest &request, Http::Method method) const;
        virtual std::shared_ptr<HttpRequest> buildHttpRequest(const std::string & endpoint, const ServiceRequest &msg, Http::Method method) const = 0;
        virtual bool hasResponseError(const std::shared_ptr<HttpResponse>&response) const;
        
        void setRequestDateOffset(uint64_t offset) const;
        uint64_t getRequestDateOffset() const;

        void disableRequest();
        void enableRequest();
    private:
        Error buildError(const std::shared_ptr<HttpResponse> &response) const ;
        std::string analyzeServerTime(const std::string &message) const;

        mutable uint64_t requestDateOffset_;
        std::string serviceName_;
        ClientConfiguration configuration_;
        std::shared_ptr<HttpClient> httpClient_;
    };
}
}

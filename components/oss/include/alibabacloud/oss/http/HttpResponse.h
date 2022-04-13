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

#include <string>
#include <memory>
#include <alibabacloud/oss/http/HttpMessage.h>
#include <alibabacloud/oss/http/HttpRequest.h>

namespace AlibabaCloud
{
namespace OSS
{

    class ALIBABACLOUD_OSS_EXPORT HttpResponse : public HttpMessage
    {
        public:
            HttpResponse(const std::shared_ptr<HttpRequest> & request);
            ~HttpResponse();

            const HttpRequest &request()const;
            void setStatusCode(int code);
            int statusCode()const;
            void setStatusMsg(std::string &msg);
            void setStatusMsg(const char *msg);
            std::string statusMsg()const;
        private:
            HttpResponse() = delete;
            std::shared_ptr<HttpRequest> request_;
            mutable int statusCode_;
            mutable std::string statusMsg_;
    };
}
}

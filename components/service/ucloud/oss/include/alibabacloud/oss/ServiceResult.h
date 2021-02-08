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
#include <iostream>

namespace AlibabaCloud
{
namespace OSS
{
    class  ServiceResult
    {
    public:
        ServiceResult() {}
        virtual ~ServiceResult() {};

        inline const std::string& RequestId() const {return requestId_;}
        inline const std::shared_ptr<std::iostream>& payload() const {return payload_;}
        inline const HeaderCollection& headerCollection() const {return headerCollection_;}
        inline int responseCode() const {return responseCode_;}

        void setRequestId(const std::string& requestId) {requestId_ = requestId;}
        void setPlayload(const std::shared_ptr<std::iostream>& payload) {payload_ = payload;}
        void setHeaderCollection(const HeaderCollection& values) { headerCollection_ = values;}
        void setResponseCode(const int code) { responseCode_ = code;} 
    private:
        std::string requestId_;
        std::shared_ptr<std::iostream> payload_;
        HeaderCollection headerCollection_;
        int responseCode_;
    };
}
}

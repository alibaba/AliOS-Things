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
#include <alibabacloud/oss/Export.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT OssError
    {
    public:
        OssError() = default;
        OssError(const std::string& code, const std::string& message) :
            code_(code),
            message_(message)
        {
        }
        OssError(const OssError& rhs) :
            code_(rhs.code_),
            message_(rhs.message_),
            requestId_(rhs.requestId_),
            host_(rhs.host_)
        {
        }
        OssError(OssError&& lhs) :
            code_(std::move(lhs.code_)),
            message_(std::move(lhs.message_)),
            requestId_(std::move(lhs.requestId_)),
            host_(std::move(lhs.host_))
        {
        }
        OssError& operator=(OssError&& lhs)
        {
            code_ = std::move(lhs.code_);
            message_ = std::move(lhs.message_);
            requestId_ = std::move(lhs.requestId_);
            host_ = std::move(lhs.host_);
            return *this;
        }
        OssError& operator=(const OssError& rhs)
        {
            code_ = rhs.code_;
            message_ = rhs.message_;
            requestId_ = rhs.requestId_;
            host_ = rhs.host_;
            return *this;
        }

        ~OssError() = default;
        const std::string& Code()const { return code_; }
        const std::string& Message() const { return message_; }
        const std::string& RequestId() const { return requestId_; }
        const std::string& Host() const { return host_; }
        void setCode(const std::string& value) { code_ = value; }
        void setCode(const char *value) { code_ = value; }
        void setMessage(const std::string& value) { message_ = value; }
        void setMessage(const char *value) { message_ = value; }
        void setRequestId(const std::string& value) { requestId_ = value; }
        void setRequestId(const char *value) { requestId_ = value; }
        void setHost(const std::string& value) { host_ = value; }
        void setHost(const char *value) { host_ = value; }
    private:
        std::string code_;
        std::string message_;
        std::string requestId_;
        std::string host_;
    };
}
}

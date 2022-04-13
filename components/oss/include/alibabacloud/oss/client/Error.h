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
#include <alibabacloud/oss/Types.h>

namespace AlibabaCloud
{
namespace OSS
{
    /*
    The status comes from the following modules: client, server, httpclient(ex. curl).
    server: [100-600)  
    client: [100000-199999]
    curl  : [200000-299999], 200000 + CURLcode

    it's sucessful only if the status/100 equals to 2.
    */
    const int ERROR_CLIENT_BASE      = 100000;
    const int ERROR_CRC_INCONSISTENT = ERROR_CLIENT_BASE + 1;
    const int ERROR_REQUEST_DISABLE  = ERROR_CLIENT_BASE + 2;

    const int ERROR_CURL_BASE = 200000;

    class ALIBABACLOUD_OSS_EXPORT Error
    {
    public:
        Error() = default;
        Error(const std::string& code, const std::string& message):
            status_(0),
            code_(code),
            message_(message)
        {
        }
        ~Error() = default;

        long Status() const {return status_;}
        const std::string& Code()const {return code_;}
        const std::string& Message() const {return message_;}
        const HeaderCollection& Headers() const { return headers_; }
        void setStatus(long status) { status_ = status;}
        void setCode(const std::string& code) { code_ = code;}
        void setMessage(const std::string& message) { message_ = message;}
        void setHeaders(const HeaderCollection& headers) { headers_ = headers; }
    private:
        long status_;
        std::string code_;
        std::string message_;
        HeaderCollection headers_;
    };
}
}

/*
 * Copyright 2009-2018 Alibaba Cloud All rights reserved.
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
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/OssRequest.h>
#include <sstream>
#include <iostream>

namespace AlibabaCloud
{
namespace OSS
{

    class ALIBABACLOUD_OSS_EXPORT UploadPartRequest: public OssObjectRequest
    {
    public:
        UploadPartRequest(const std::string& bucket, const std::string& key,
            const std::shared_ptr<std::iostream>& content);
        UploadPartRequest(const std::string &bucket, const std::string& key,
            int partNumber, const std::string& uploadId,
            const std::shared_ptr<std::iostream>& content);
        virtual std::shared_ptr<std::iostream> Body() const;
        void setPartNumber(int partNumber);
        void setUploadId(const std::string& uploadId);
        void setConetent(const std::shared_ptr<std::iostream>& content);
        void setContentLength(uint64_t length);
        void setTrafficLimit(uint64_t value);
        void setUserAgent(const std::string& ua);
        int PartNumber() const;
    protected:
        virtual HeaderCollection specialHeaders() const;
        virtual ParameterCollection specialParameters() const;
        virtual int validate() const;
    private:
        int partNumber_;
        std::string uploadId_;
        std::shared_ptr<std::iostream> content_;
        uint64_t contentLength_;
        bool contentLengthIsSet_;
        uint64_t trafficLimit_;
        std::string userAgent_;
    };
} 
}

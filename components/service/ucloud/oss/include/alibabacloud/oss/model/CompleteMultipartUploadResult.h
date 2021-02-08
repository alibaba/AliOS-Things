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
#include <iostream>
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/OssResult.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT CompleteMultipartUploadResult: public OssObjectResult
    {
    public:
        CompleteMultipartUploadResult();
        CompleteMultipartUploadResult(const std::string& data);
        CompleteMultipartUploadResult(const std::shared_ptr<std::iostream>& data, 
            const HeaderCollection& headers);
        CompleteMultipartUploadResult& operator=(const std::string& data);
        const std::string& Location() const;
        const std::string& Bucket() const;
        const std::string& Key() const;
        const std::string& ETag() const;
        const std::string& EncodingType() const;
        uint64_t CRC64() const;
        const std::shared_ptr<std::iostream>& Content() const;
    private:
        std::string location_;
        std::string bucket_;
        std::string key_;
        std::string eTag_;
        std::string encodingType_;
        uint64_t crc64_;
        std::shared_ptr<std::iostream> content_;
    };
} 
}

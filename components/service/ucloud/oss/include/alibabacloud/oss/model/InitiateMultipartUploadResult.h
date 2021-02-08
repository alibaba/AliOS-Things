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
#include <string>
#include <memory>
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/OssResult.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT InitiateMultipartUploadResult :public OssResult
    {
    public:
        InitiateMultipartUploadResult();
        InitiateMultipartUploadResult(const std::string& data);
        InitiateMultipartUploadResult(const std::shared_ptr<std::iostream>& data);
        InitiateMultipartUploadResult& operator=(const std::string& data);

        const std::string& Bucket() const { return bucket_; }
        const std::string& Key() const { return key_; }
        const std::string& UploadId() const { return uploadId_; }
        const std::string& EncodingType() const { return encodingType_; }
    private:
        std::string bucket_;
        std::string key_;
        std::string uploadId_;
        std::string encodingType_;
    };
}
}

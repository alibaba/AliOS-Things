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
#include <alibabacloud/oss/Types.h>
#include <alibabacloud/oss/encryption/ContentCryptoMaterial.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT MultipartUploadCryptoContext
    {
    public:
        MultipartUploadCryptoContext():dataSize_(0), partSize_(0) {}
        ~MultipartUploadCryptoContext() {}
        const ContentCryptoMaterial& ContentMaterial() const { return content_; }
        const std::string& UploadId() const { return uploadId_; }
        int64_t PartSize() const { return partSize_; }
        int64_t DataSize() const { return dataSize_; }

        void setContentMaterial(const ContentCryptoMaterial& content) { content_ = content; }
        void setUploadId(const std::string& uploadId) { uploadId_ = uploadId; }
        void setPartSize(int64_t size) { partSize_ = size; }
        void setDataSize(int64_t size) { dataSize_ = size; }
    private:
        ContentCryptoMaterial content_;
        int64_t dataSize_;
        int64_t partSize_;
        std::string uploadId_;
    };
} 
}

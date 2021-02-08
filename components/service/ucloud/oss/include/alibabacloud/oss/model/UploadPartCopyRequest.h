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
    class ALIBABACLOUD_OSS_EXPORT UploadPartCopyRequest: public OssObjectRequest
    {
    public:
        UploadPartCopyRequest(const std::string& bucket, const std::string& key);
        UploadPartCopyRequest(const std::string& bucket, const std::string& key,
                              const std::string& srcBucket, const std::string& srcKey);
        UploadPartCopyRequest(const std::string& bucket, const std::string& key,
                              const std::string& srcBucket, const std::string& srcKey,
                              const std::string& uploadId, int partNumber);
        UploadPartCopyRequest(const std::string& bucket, const std::string& key,
                              const std::string& srcBucket, const std::string& srcKey,
                              const std::string& uploadId, int partNumber,
                              const std::string& sourceIfMatchETag,
                              const std::string& sourceIfNotMatchETag,
                              const std::string& sourceIfModifiedSince,
                              const std::string& sourceIfUnModifiedSince);

        void setPartNumber(uint32_t partNumber); 
        void setUploadId(const std::string& uploadId);
        void SetCopySource(const std::string& srcBucket, const std::string& srcKey);
        void setCopySourceRange(uint64_t begin, uint64_t end); 
        void SetSourceIfMatchETag(const std::string& value);
        void SetSourceIfNotMatchETag(const std::string& value);
        void SetSourceIfModifiedSince(const std::string& value);
        void SetSourceIfUnModifiedSince(const std::string& value);
        void setTrafficLimit(uint64_t value);
    protected:
        virtual ParameterCollection specialParameters() const;
        virtual HeaderCollection specialHeaders() const;
        virtual int validate() const;
    private:
        std::string uploadId_;
        std::string sourceBucket_;
        std::string sourceKey_;
        uint32_t partNumber_;
        uint64_t sourceRange_[2];
        bool sourceRangeIsSet_;
        std::string sourceIfMatchETag_;
        bool sourceIfMatchETagIsSet_;
        std::string sourceIfNotMatchETag_;
        bool sourceIfNotMatchETagIsSet_;
        std::string sourceIfModifiedSince_;
        bool sourceIfModifiedSinceIsSet_;
        std::string sourceIfUnModifiedSince_;
        bool sourceIfUnModifiedSinceIsSet_;
        uint64_t trafficLimit_;
    };
} 
}

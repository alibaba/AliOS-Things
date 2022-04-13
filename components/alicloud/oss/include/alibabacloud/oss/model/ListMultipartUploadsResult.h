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
#include <vector>

namespace AlibabaCloud
{
namespace OSS
{
    class MultipartUpload
    {
    public:
        MultipartUpload() = default;
    public:
        std::string Key;
        std::string UploadId;
        std::string Initiated;
    };

    using MultipartUploadList = std::vector<MultipartUpload>;
    class ALIBABACLOUD_OSS_EXPORT ListMultipartUploadsResult : public OssResult
    {
    public:
        ListMultipartUploadsResult();
        ListMultipartUploadsResult(const std::string& data);
        ListMultipartUploadsResult(const std::shared_ptr<std::iostream>& data);
        ListMultipartUploadsResult& operator=(const std::string& data);

        const std::string& Bucket() const { return bucket_; }
        const std::string& KeyMarker() const { return keyMarker_; }
        const std::string& UploadIdMarker() const { return uploadIdMarker_; }
        const std::string& EncodingType() const { return encodingType_; }
        const std::string& NextKeyMarker() const { return nextKeyMarker_; }
        const std::string& NextUploadIdMarker() const { return nextUploadIdMarker_; }
        uint32_t MaxUploads() const { return maxUploads_; }
        bool IsTruncated() const { return isTruncated_; }
        const CommonPrefixeList& CommonPrefixes() const { return commonPrefixes_; }
        const AlibabaCloud::OSS::MultipartUploadList& MultipartUploadList() const { return multipartUploadList_; }
    private:
        std::string bucket_;
        std::string keyMarker_;
        std::string uploadIdMarker_;
        std::string encodingType_;
        std::string nextKeyMarker_;
        std::string nextUploadIdMarker_;
        uint32_t maxUploads_;
        bool isTruncated_;
        CommonPrefixeList commonPrefixes_;
        AlibabaCloud::OSS::MultipartUploadList multipartUploadList_;
    };
} 
}

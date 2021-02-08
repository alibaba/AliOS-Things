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
#include <alibabacloud/oss/model/Bucket.h>
#include <vector>
#include <memory>
#include <iostream>
#include <alibabacloud/oss/OssResult.h>
#include <alibabacloud/oss/model/Owner.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ListObjectsResult;
    class ALIBABACLOUD_OSS_EXPORT ObjectSummary
    {
    public:
        ObjectSummary() = default;
        const std::string& Key() const { return key_; }
        const std::string& ETag()const { return eTag_; }
        int64_t Size() const { return size_; }
        const std::string& LastModified() const { return lastModified_; }
        const std::string& StorageClass() const { return storageClass_; }
        const std::string& Type() const { return type_; }
        const AlibabaCloud::OSS::Owner& Owner() const { return owner_; }
    private:
        friend class ListObjectsResult;
        std::string key_;
        std::string eTag_;
        int64_t size_;
        std::string lastModified_;
        std::string storageClass_;
        std::string type_;
        AlibabaCloud::OSS::Owner owner_;
    };

    using ObjectSummaryList = std::vector<ObjectSummary>;

    class ALIBABACLOUD_OSS_EXPORT ListObjectsResult : public OssResult
    {
    public:
        ListObjectsResult();
        ListObjectsResult(const std::string& data);
        ListObjectsResult(const std::shared_ptr<std::iostream>& data);
        ListObjectsResult& operator=(const std::string& data);
        const std::string& Name() const { return name_; }
        const std::string& Prefix() const { return prefix_; }
        const std::string& Marker() const { return marker_; }
        const std::string& NextMarker() const { return nextMarker_; }
        const std::string& Delimiter() const { return delimiter_; }
        const std::string& EncodingType() const { return encodingType_; }
        int MaxKeys() const { return maxKeys_; }
        bool IsTruncated() const { return isTruncated_; }
        const CommonPrefixeList& CommonPrefixes() const { return commonPrefixes_; }
        const ObjectSummaryList& ObjectSummarys() const { return objectSummarys_; }
    private:
        std::string name_;
        std::string prefix_;
        std::string marker_;
        std::string delimiter_;
        std::string nextMarker_;
        std::string encodingType_;
        bool        isTruncated_;
        int         maxKeys_;
        CommonPrefixeList commonPrefixes_;
        ObjectSummaryList objectSummarys_;
    };
} 
}

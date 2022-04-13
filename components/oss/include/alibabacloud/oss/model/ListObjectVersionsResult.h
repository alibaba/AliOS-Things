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
    class ListObjectVersionsResult;
    class ALIBABACLOUD_OSS_EXPORT ObjectVersionSummary
    {
    public:
        ObjectVersionSummary() = default;
        const std::string& Key() const { return key_; }
        const std::string& VersionId() const { return versionid_; }
        const std::string& ETag()const { return eTag_; }
        const std::string& LastModified() const { return lastModified_; }
        const std::string& StorageClass() const { return storageClass_; }
        const std::string& Type() const { return type_; }
        int64_t Size() const { return size_; }
        bool IsLatest() const { return isLatest_; }
        const AlibabaCloud::OSS::Owner& Owner() const { return owner_; }
    private:
        friend class ListObjectVersionsResult;
        std::string key_;
        std::string versionid_;
        std::string eTag_;
        std::string lastModified_;
        std::string storageClass_;
        std::string type_;
        int64_t size_;
        bool isLatest_;
        AlibabaCloud::OSS::Owner owner_;
    };
    using ObjectVersionSummaryList = std::vector<ObjectVersionSummary>;

    class ALIBABACLOUD_OSS_EXPORT DeleteMarkerSummary
    {
    public:
        DeleteMarkerSummary() = default;
        const std::string& Key() const { return key_; }
        const std::string& VersionId() const { return versionid_; }
        const std::string& LastModified() const { return lastModified_; }
        bool IsLatest() const { return isLatest_; }
        const AlibabaCloud::OSS::Owner& Owner() const { return owner_; }
    private:
        friend class ListObjectVersionsResult;
        std::string key_;
        std::string versionid_;
        std::string lastModified_;
        bool isLatest_;
        AlibabaCloud::OSS::Owner owner_;
    };
    using DeleteMarkerSummaryList = std::vector<DeleteMarkerSummary>;

    class ALIBABACLOUD_OSS_EXPORT ListObjectVersionsResult : public OssResult
    {
    public:
        ListObjectVersionsResult();
        ListObjectVersionsResult(const std::string& data);
        ListObjectVersionsResult(const std::shared_ptr<std::iostream>& data);
        ListObjectVersionsResult& operator=(const std::string& data);
        const std::string& Name() const { return name_; }
        const std::string& Prefix() const { return prefix_; }
        const std::string& KeyMarker() const { return keyMarker_; }
        const std::string& NextKeyMarker() const { return nextKeyMarker_; }
        const std::string& VersionIdMarker() const { return versionIdMarker_; }
        const std::string& NextVersionIdMarker() const { return nextVersionIdMarker_; }
        const std::string& Delimiter() const { return delimiter_; }
        const std::string& EncodingType() const { return encodingType_; }
        int MaxKeys() const { return maxKeys_; }
        bool IsTruncated() const { return isTruncated_; }
        const CommonPrefixeList& CommonPrefixes() const { return commonPrefixes_; }
        const ObjectVersionSummaryList& ObjectVersionSummarys() const { return objectVersionSummarys_; }
        const DeleteMarkerSummaryList& DeleteMarkerSummarys() const { return deleteMarkerSummarys_; }
    private:
        std::string name_;
        std::string prefix_;
        std::string keyMarker_;
        std::string nextKeyMarker_;
        std::string versionIdMarker_;
        std::string nextVersionIdMarker_;
        std::string delimiter_;
        std::string encodingType_;
        bool        isTruncated_;
        int         maxKeys_;
        CommonPrefixeList commonPrefixes_;
        ObjectVersionSummaryList objectVersionSummarys_;
        DeleteMarkerSummaryList deleteMarkerSummarys_;
    };
} 
}

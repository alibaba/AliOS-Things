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
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/Types.h>
#include <alibabacloud/oss/OssRequest.h>
#include <alibabacloud/oss/model/ObjectMetaData.h>
#include <iostream>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT MultiCopyObjectRequest : public OssResumableBaseRequest
    {
    public:
        MultiCopyObjectRequest(const std::string& bucket, const std::string& key, 
            const std::string& srcBucket, const std::string& srcKey);
        MultiCopyObjectRequest(const std::string& bucket, const std::string& key, 
            const std::string& srcBucket, const std::string& srcKey, 
            const std::string& checkpointDir);
        MultiCopyObjectRequest(const std::string& bucket, const std::string& key, 
            const std::string& srcBucket, const std::string& srcKey, 
            const std::string& checkpointDir, const ObjectMetaData& meta);
        MultiCopyObjectRequest(const std::string& bucket, const std::string& key, 
            const std::string& srcBucket, const std::string& srcKey, 
            const std::string& checkpointDir, uint64_t partSize, uint32_t threadNum);
        MultiCopyObjectRequest(const std::string& bucket, const std::string& key, 
            const std::string& srcBucket, const std::string& srcKey,
            const std::string& checkpointDir, uint64_t partSize, uint32_t threadNum, 
            const ObjectMetaData& metaData);

        MultiCopyObjectRequest(const std::string& bucket, const std::string& key,
            const std::string& srcBucket, const std::string& srcKey,
            const std::wstring& checkpointDir);
        MultiCopyObjectRequest(const std::string& bucket, const std::string& key,
            const std::string& srcBucket, const std::string& srcKey,
            const std::wstring& checkpointDir, const ObjectMetaData& meta);
        MultiCopyObjectRequest(const std::string& bucket, const std::string& key,
            const std::string& srcBucket, const std::string& srcKey,
            const std::wstring& checkpointDir, uint64_t partSize, uint32_t threadNum);
        MultiCopyObjectRequest(const std::string& bucket, const std::string& key,
            const std::string& srcBucket, const std::string& srcKey,
            const std::wstring& checkpointDir, uint64_t partSize, uint32_t threadNum,
            const ObjectMetaData& metaData);

        const std::string& SrcBucket() const { return srcBucket_; }
        const std::string& SrcKey() const { return srcKey_; }

        const std::string& EncodingType() const { return encodingType_; }
        const ObjectMetaData& MetaData() const { return metaData_; }

        void setCopySource(const std::string& srcBucket, const std::string& srcObject);
        void setSourceIfMatchEtag(const std::string& value);
        void setSourceIfNotMatchEtag(const std::string& value);
        void setSourceIfUnModifiedSince(const std::string& value);
        void setSourceIfModifiedSince(const std::string& value);
        void setMetadataDirective(const CopyActionList& action);
        void setAcl(const CannedAccessControlList& acl);

        void setEncodingType(const std::string& type) { encodingType_ = type; }

        const std::string& SourceIfMatchEtag() const;
        const std::string& SourceIfNotMatchEtag() const;
        const std::string& SourceIfUnModifiedSince() const;
        const std::string& SourceIfModifiedSince() const;

    protected:
        virtual int validate() const;

    private:
        std::string srcBucket_;
        std::string srcKey_;
        std::string encodingType_;

        ObjectMetaData metaData_;
    };
}
}
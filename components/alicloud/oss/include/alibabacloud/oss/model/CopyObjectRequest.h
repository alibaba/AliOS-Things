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
#include <alibabacloud/oss/OssRequest.h>
#include <alibabacloud/oss/Types.h>
#include <alibabacloud/oss/model/ObjectMetaData.h>


namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT CopyObjectRequest: public OssObjectRequest
    {
    public:
        CopyObjectRequest(const std::string& bucket, const std::string& key);
        CopyObjectRequest(const std::string& bucket, const std::string& key,
            const ObjectMetaData& meta);
        void setCopySource(const std::string& srcBucket,const std::string& srcObject);
        void setSourceIfMatchETag(const std::string& value);
        void setSourceIfNotMatchETag(const std::string& value);
        void setSourceIfUnModifiedSince(const std::string& value);
        void setSourceIfModifiedSince(const std::string& value);
        void setMetadataDirective(const CopyActionList& action);
        void setAcl(const CannedAccessControlList& acl);
        void setTagging(const std::string& value);
        void setTaggingDirective(const CopyActionList& action);
        void setTrafficLimit(uint64_t value);

    protected:
        virtual HeaderCollection specialHeaders() const ;
        virtual ParameterCollection specialParameters() const;
    private:
        std::string sourceBucket_;
        std::string sourceKey_;
        ObjectMetaData metaData_;
    };
}
}


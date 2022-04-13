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
#include <alibabacloud/oss/model/ObjectMetaData.h>

namespace AlibabaCloud
{
namespace OSS
{

    class ALIBABACLOUD_OSS_EXPORT InitiateMultipartUploadRequest: public OssObjectRequest
    {
    public:
        InitiateMultipartUploadRequest(const std::string& bucket, const std::string& key);
        InitiateMultipartUploadRequest(const std::string& bucket, const std::string& key,
            const ObjectMetaData& metaData);
        void setCacheControl(const std::string& value);
        void setContentDisposition(const std::string& value);
        void setContentEncoding(const std::string& value);
        void setExpires(const std::string& value);
        ObjectMetaData& MetaData();
        void setEncodingType(const std::string& encodingType);
        void setTagging(const std::string& value);
        void setSequential(bool value);
    protected:
        virtual HeaderCollection specialHeaders() const;
        virtual ParameterCollection specialParameters() const;
    private:
        ObjectMetaData metaData_;
        std::string encodingType_;
        bool encodingTypeIsSet_;
        bool sequential_;
    };
} 
}

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
#include <alibabacloud/oss/http/HttpType.h>

namespace AlibabaCloud
{
namespace OSS
{
    class OssClientImpl;
    class ALIBABACLOUD_OSS_EXPORT GeneratePresignedUrlRequest
    {
    public:
        GeneratePresignedUrlRequest(const std::string& bucket, const std::string& key);
        GeneratePresignedUrlRequest(const std::string& bucket, const std::string& key,
            Http::Method method);
        void setBucket(const std::string& bucket);
        void setKey(const std::string& key);
        void setContentType(const std::string& value);
        void setContentMd5(const std::string& value);
        void setExpires(int64_t unixTime);
        void setProcess(const std::string& process);
        void setTrafficLimit(uint64_t value);
        void setVersionId(const std::string& versionId);
        void setRequestPayer(RequestPayer value);
        void addResponseHeaders(RequestResponseHeader header, const std::string& value);
        void addParameter(const std::string& key, const std::string& value);
        MetaData& UserMetaData();
    private:
        friend class OssClientImpl;
        std::string bucket_;
        std::string key_;
        Http::Method method_;
        ObjectMetaData metaData_;
        ParameterCollection parameters_;
    };
} 
}

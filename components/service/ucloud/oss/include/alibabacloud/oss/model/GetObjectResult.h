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
#include <string>
#include <alibabacloud/oss/OssResult.h>
#include <alibabacloud/oss/model/ObjectMetaData.h>
#include <alibabacloud/oss/ServiceRequest.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT GetObjectResult :public OssObjectResult
    {
    public:
        GetObjectResult();
        GetObjectResult(const std::string& bucket, const std::string& key, 
            const std::shared_ptr<std::iostream>& content,
            const HeaderCollection& headers);
        GetObjectResult(const std::string& bucket, const std::string& key,
            const ObjectMetaData& metaData);
        const std::string& Bucket() const { return bucket_; }
        const std::string& Key()  const { return key_; }
        const ObjectMetaData& Metadata()  const { return metaData_; }
        const std::shared_ptr<std::iostream>& Content() const { return content_; }
        void setContent(const std::shared_ptr<std::iostream>& content) { content_ = content; }
        void setMetaData(const ObjectMetaData& meta) { metaData_ = meta; }
    private:
        std::string bucket_;
        std::string key_;
        ObjectMetaData metaData_;
        std::shared_ptr<std::iostream> content_;
    };
}
}

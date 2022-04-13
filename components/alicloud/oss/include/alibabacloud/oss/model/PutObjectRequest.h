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
    class ALIBABACLOUD_OSS_EXPORT PutObjectRequest: public OssObjectRequest
    {
    public:
        PutObjectRequest(const std::string& bucket, const std::string& key,
            const std::shared_ptr<std::iostream>& content);
        PutObjectRequest(const std::string& bucket, const std::string& key,
            const std::shared_ptr<std::iostream>& content, 
            const ObjectMetaData& meta);
        void setCacheControl(const std::string& value);
        void setContentDisposition(const std::string& value);
        void setContentEncoding(const std::string& value);
        void setContentMd5(const std::string& value);
        void setExpires(const std::string& value);
        void setCallback(const std::string& callback, const std::string& callbackVar = "");
        void setTrafficLimit(uint64_t value);
        void setTagging(const std::string& value);
        ObjectMetaData& MetaData();
        virtual std::shared_ptr<std::iostream> Body() const;
    protected:
        virtual HeaderCollection specialHeaders() const;
        virtual int validate() const;
    private:
        std::shared_ptr<std::iostream> content_;
        ObjectMetaData metaData_;
    };
} 
}

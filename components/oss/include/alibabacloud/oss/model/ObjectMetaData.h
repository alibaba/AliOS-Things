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
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/Types.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT ObjectMetaData
    {
    public:
        ObjectMetaData() = default;
        ObjectMetaData(const HeaderCollection& data);
        ObjectMetaData& operator=(const HeaderCollection& data);
        const std::string& LastModified() const;
        const std::string& ExpirationTime() const;
        int64_t ContentLength() const ;
        const std::string& ContentType() const;
        const std::string& ContentEncoding() const;
        const std::string& CacheControl() const;
        const std::string& ContentDisposition() const;
        const std::string& ETag() const;
        const std::string& ContentMd5() const;
        const std::string& ObjectType() const;
        const std::string& VersionId() const;
        uint64_t CRC64() const;
        void setExpirationTime(const std::string& value);
        void setContentLength(int64_t value);
        void setContentType(const std::string& value);
        void setContentEncoding(const std::string& value);
        void setCacheControl(const std::string& value);
        void setContentDisposition(const std::string& value);
        void setETag(const std::string& value);
        void setContentMd5(const std::string& value);
        void setCrc64(uint64_t value);
        void addHeader(const std::string& key, const std::string& value);
        bool hasHeader(const std::string& key) const;
        void removeHeader(const std::string& key);
        MetaData& HttpMetaData();
        const MetaData& HttpMetaData() const;
        void addUserHeader(const std::string& key, const std::string& value);
        bool hasUserHeader(const std::string& key) const;
        void removeUserHeader(const std::string& key);
        MetaData& UserMetaData();
        const MetaData& UserMetaData() const;
        HeaderCollection toHeaderCollection() const;
    private:
        MetaData userMetaData_;
        MetaData metaData_;
    };
}
}

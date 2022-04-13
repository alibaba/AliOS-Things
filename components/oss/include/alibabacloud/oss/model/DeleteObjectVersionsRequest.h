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
#include <list>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT ObjectIdentifier
    {
    public:
        ObjectIdentifier() {};
        ObjectIdentifier(const std::string& key) : key_(key) {};
        ObjectIdentifier(const std::string& key, const std::string& versionId) : key_(key), versionId_(versionId) {};
        void setKey(const std::string& key) { key_ = key; };
        void setVersionId(const std::string& versionId) { versionId_ = versionId; };
        const std::string& Key() const { return key_; }
        const std::string& VersionId() const { return versionId_; }
    private:
        std::string key_;
        std::string versionId_;
    };

    using ObjectIdentifierList = std::vector<ObjectIdentifier>;

    class ALIBABACLOUD_OSS_EXPORT DeleteObjectVersionsRequest : public OssBucketRequest
    {
    public:
        DeleteObjectVersionsRequest(const std::string& bucket);
        bool Quiet() const;
        const std::string& EncodingType() const;
        void setQuiet(bool quiet);
        void setEncodingType(const std::string& value);

        void addObject(const ObjectIdentifier& object);
        void setObjects(const ObjectIdentifierList& objects);
        const ObjectIdentifierList& Objects() const;
        void clearObjects();

        void setRequestPayer(RequestPayer value);
    protected:
        virtual std::string payload() const;
        virtual ParameterCollection specialParameters() const;
        virtual HeaderCollection specialHeaders() const;
    private:
        bool quiet_;
        std::string encodingType_;
        ObjectIdentifierList objects_;
        RequestPayer requestPayer_;
    };
} 
}

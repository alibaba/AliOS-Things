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
    using DeletedKeyList = std::list<std::string>;
    class ALIBABACLOUD_OSS_EXPORT DeleteObjectsRequest : public OssBucketRequest
    {
    public:
        DeleteObjectsRequest(const std::string& bucket);
        bool Quiet() const;
        const std::string& EncodingType() const;
        const DeletedKeyList& KeyList() const;
        void setQuiet(bool quiet);
        void setEncodingType(const std::string& value);
        void addKey(const std::string& key);
        void setKeyList(const DeletedKeyList& keyList);
        void clearKeyList();
        void setRequestPayer(RequestPayer value);
    protected:
        virtual std::string payload() const;
        virtual ParameterCollection specialParameters() const;
        virtual HeaderCollection specialHeaders() const;
    private:
        bool quiet_;
        std::string encodingType_;
        DeletedKeyList keyList_;
        RequestPayer requestPayer_;
    };
} 
}

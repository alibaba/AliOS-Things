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
#include <string>
#include <vector>

namespace AlibabaCloud
{
namespace OSS
{
    using CommonPrefixeList = std::vector<std::string>;

    class OssClientImpl;
    class ALIBABACLOUD_OSS_EXPORT OssResult
    {
    public:
        OssResult();
        OssResult(const HeaderCollection& value);
        virtual ~OssResult() {};
        const std::string& RequestId() const {return requestId_;}
    protected:
        friend class OssClientImpl;
        bool ParseDone() { return parseDone_; };
        bool parseDone_;
        std::string requestId_;
    };

    class ALIBABACLOUD_OSS_EXPORT OssObjectResult : public OssResult
    {
    public:
        OssObjectResult();
        OssObjectResult(const HeaderCollection& value);
        virtual ~OssObjectResult() {};
        const std::string& VersionId() const { return versionId_; }
    protected:
        std::string versionId_;
    };
}
}

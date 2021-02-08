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
#include <alibabacloud/oss/OssResult.h>
#include <alibabacloud/oss/ServiceRequest.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT GetSymlinkResult : public OssObjectResult
    {
    public:
       public:
        GetSymlinkResult();
        GetSymlinkResult(const std::string& symlink,const std::string& etag);
        GetSymlinkResult(const HeaderCollection& headers);
        const std::string& SymlinkTarget() const { return symlink_; }
        const std::string& ETag() const { return etag_; }
     private:
        std::string symlink_;
        std::string etag_;
    };
} 
}


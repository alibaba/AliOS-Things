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
    class ALIBABACLOUD_OSS_EXPORT CopyObjectResult : public OssObjectResult
    {
    public:
        CopyObjectResult();
        CopyObjectResult(const std::string& data);
        CopyObjectResult(const std::shared_ptr<std::iostream>& data);
        CopyObjectResult(const HeaderCollection& headers, const std::shared_ptr<std::iostream>& data);
        CopyObjectResult& operator=(const std::string& data);
        const std::string& ETag() const { return etag_; }
        const std::string& LastModified() const { return lastModified_; }
        const std::string& SourceVersionId() { return sourceVersionId_; }

        void setEtag(const std::string& etag) { etag_ = etag; }
        void setLastModified(const std::string& lastModified) { lastModified_ = lastModified; }
        void setVersionId(const std::string& versionId) { versionId_ = versionId; }
        void setRequestId(const std::string& requestId) { requestId_ = requestId; }
     private:
        std::string etag_;
        std::string lastModified_;
        std::string sourceVersionId_;
    };
} 
}


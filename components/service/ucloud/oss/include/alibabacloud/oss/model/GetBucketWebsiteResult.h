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
#include <memory>
#include <iostream>
#include <alibabacloud/oss/OssResult.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT GetBucketWebsiteResult: public OssResult
    {
    public:
        GetBucketWebsiteResult();
        GetBucketWebsiteResult(const std::string& data);
        GetBucketWebsiteResult(const std::shared_ptr<std::iostream>& data);
        GetBucketWebsiteResult& operator=(const std::string& data);
        const std::string& IndexDocument() const { return indexDocument_; }
        const std::string& ErrorDocument() const { return errorDocument_; }
    private:
        std::string indexDocument_;
        std::string errorDocument_;
    };
} 
}

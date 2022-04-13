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
#include "Bucket.h"
#include <vector>
#include <memory>
#include <iostream>
#include <alibabacloud/oss/OssResult.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT ListBucketsResult: public OssResult
    {
    public:
        ListBucketsResult();
        ListBucketsResult(const std::string& data);
        ListBucketsResult(const std::shared_ptr<std::iostream>& data);
        ListBucketsResult& operator=(const std::string& data);
        const std::string& Prefix() const { return prefix_; }
        const std::string& Marker() const { return marker_; }
        const std::string& NextMarker() const { return nextMarker_; }
        int MaxKeys() const { return maxKeys_; }
        bool IsTruncated() const { return isTruncated_; }
        const std::vector<Bucket>& Buckets() const { return buckets_; }
    private:
        std::string prefix_;
        std::string marker_;
        std::string nextMarker_;
        bool        isTruncated_;
        int         maxKeys_;
        std::vector<Bucket> buckets_;
    };
} 
}

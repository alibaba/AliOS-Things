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
#include <alibabacloud/oss/model/Bucket.h>
#include <vector>
#include <memory>
#include <iostream>
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/model/Owner.h>
#include <alibabacloud/oss/OssResult.h>
#include <alibabacloud/oss/model/Part.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT ListPartsResult :public OssResult
    {
    public:
        ListPartsResult();
        ListPartsResult(const std::string& data);
        ListPartsResult(const std::shared_ptr<std::iostream>& data);
        ListPartsResult& operator=(const std::string& data);
        const std::string& Bucket() const;
        const std::string& Key() const;
        const std::string& UploadId() const;
        const std::string& EncodingType() const;
        uint32_t MaxParts() const;
        uint32_t PartNumberMarker() const;
        uint32_t NextPartNumberMarker() const;
        const AlibabaCloud::OSS::PartList& PartList()const;
        bool IsTruncated() const;
    private:
        std::string uploadId_;
        uint32_t maxParts_;
        uint32_t partNumberMarker_;
        uint32_t nextPartNumberMarker_;
        std::string encodingType_;
        std::string key_;
        std::string bucket_;
        bool isTruncated_;
        AlibabaCloud::OSS::PartList partList_;
    };
} 
}

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
namespace AlibabaCloud
{
namespace OSS
{
    class ListPartsResult;
    class ResumableUploader;
    class ResumableCopier;
    class ALIBABACLOUD_OSS_EXPORT Part
    {
    public:
        Part() :partNumber_(0), size_(0), cRC64_(0) {}
        Part(int32_t partNumber, const std::string& eTag):partNumber_(partNumber), eTag_(eTag){}
        int32_t PartNumber() const { return partNumber_; }
        int64_t Size() const { return size_; }
        uint64_t CRC64() const { return cRC64_; }
        const std::string& LastModified() const { return lastModified_; }
        const std::string& ETag() const { return eTag_; }
    private:
        friend class ListPartsResult;
        friend class ResumableUploader;
        friend class ResumableCopier;
        int32_t partNumber_;
        int64_t size_;
        uint64_t cRC64_;
        std::string lastModified_;
        std::string eTag_;
    };
    using PartList = std::vector<Part>;
}
}

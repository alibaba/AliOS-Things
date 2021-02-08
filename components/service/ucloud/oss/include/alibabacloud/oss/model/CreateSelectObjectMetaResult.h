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
#include <alibabacloud/oss/OssResult.h>
#include <memory>
#include <iostream>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT CreateSelectObjectMetaResult : public OssResult
    {
    public:
        CreateSelectObjectMetaResult();
        CreateSelectObjectMetaResult(
            const std::string& bucket,
            const std::string& key,
            const std::string& requestId,
            const std::shared_ptr<std::iostream>& data);
        CreateSelectObjectMetaResult& operator=(const std::shared_ptr<std::iostream>& data);

        const std::string& Bucket() const { return bucket_; }
        const std::string& Key()  const { return key_; }
        uint64_t Offset() const { return offset_; }
        uint64_t TotalScanned() const { return totalScanned_; }
        uint32_t Status() const { return status_; }
        uint32_t SplitsCount() const { return splitsCount_; }
        uint64_t RowsCount() const { return rowsCount_; }
        uint32_t ColsCount() const { return colsCount_; }
        const std::string& ErrorMessage() const { return errorMessage_; }

    private:
        std::string bucket_;
        std::string key_;
        uint64_t offset_;
        uint64_t totalScanned_;
        uint32_t status_;
        uint32_t splitsCount_;
        uint64_t rowsCount_;
        uint32_t colsCount_;
        std::string errorMessage_;
    };

}
}

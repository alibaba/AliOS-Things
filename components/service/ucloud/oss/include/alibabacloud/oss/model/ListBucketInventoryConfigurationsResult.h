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
#include <alibabacloud/oss/model/InventoryConfiguration.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT ListBucketInventoryConfigurationsResult : public OssResult
    {
    public:
        ListBucketInventoryConfigurationsResult();
        ListBucketInventoryConfigurationsResult(const std::string& data);
        ListBucketInventoryConfigurationsResult(const std::shared_ptr<std::iostream>& data);
        ListBucketInventoryConfigurationsResult& operator=(const std::string& data);
        const AlibabaCloud::OSS::InventoryConfigurationList& InventoryConfigurationList()const { return inventoryConfigurationList_; }
        bool IsTruncated() const { return isTruncated_; }
        const std::string& NextContinuationToken() const { return nextContinuationToken_; }

    private:
        AlibabaCloud::OSS::InventoryConfigurationList inventoryConfigurationList_;
        bool isTruncated_;
        std::string nextContinuationToken_;
    };
}
}

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
#include <alibabacloud/oss/model/InventoryConfiguration.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT SetBucketInventoryConfigurationRequest : public OssBucketRequest
    {
    public:
        SetBucketInventoryConfigurationRequest(const std::string& bucket);
        SetBucketInventoryConfigurationRequest(const std::string& bucket, const InventoryConfiguration& conf);
        void setInventoryConfiguration(InventoryConfiguration conf);
    protected:
        virtual ParameterCollection specialParameters() const;
        virtual std::string payload() const;
    private:
        InventoryConfiguration inventoryConfiguration_;
    };
}
}

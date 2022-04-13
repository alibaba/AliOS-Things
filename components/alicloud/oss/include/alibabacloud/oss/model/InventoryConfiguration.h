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
    class ALIBABACLOUD_OSS_EXPORT InventoryFilter
    {
    public:
        InventoryFilter();
        InventoryFilter(const std::string& prefix);
        const std::string& Prefix() const { return prefix_; }
        void setPrefix(const std::string& prefix) { prefix_ = prefix; }
    private:
        std::string prefix_;
    };

    class ALIBABACLOUD_OSS_EXPORT InventorySSEOSS
    {
    public:
        InventorySSEOSS();
    };

    class ALIBABACLOUD_OSS_EXPORT InventorySSEKMS
    {
    public:
        InventorySSEKMS();
        InventorySSEKMS(const std::string& key);
        const std::string& KeyId() const { return keyId_; }
        void setKeyId(const std::string& key) { keyId_ = key; }
    private:
        std::string keyId_;
    };

    class ALIBABACLOUD_OSS_EXPORT InventoryEncryption
    {
    public:
        InventoryEncryption();
        InventoryEncryption(const InventorySSEOSS& value);
        InventoryEncryption(const InventorySSEKMS& value);

        const InventorySSEOSS& SSEOSS() const { return inventorySSEOSS_; }
        void setSSEOSS(const InventorySSEOSS& value) { inventorySSEOSS_ = value;  inventorySSEOSIsSet_ = true; }
        bool hasSSEOSS() const { return inventorySSEOSIsSet_; }

        const InventorySSEKMS& SSEKMS() const { return inventorySSEKMS_; }
        void setSSEKMS(const InventorySSEKMS& value) { inventorySSEKMS_ = value;  inventorySSEKMSIsSet_ = true; }
        bool hasSSEKMS() const { return inventorySSEKMSIsSet_; }

    private:
        InventorySSEOSS inventorySSEOSS_;
        bool inventorySSEOSIsSet_;

        InventorySSEKMS inventorySSEKMS_;
        bool inventorySSEKMSIsSet_;
    };

    class ALIBABACLOUD_OSS_EXPORT InventoryOSSBucketDestination
    {
    public:
        InventoryOSSBucketDestination();
        InventoryFormat Format() const { return format_; }
        const std::string& AccountId() const { return accountId_; }
        const std::string& RoleArn() const { return roleArn_; }
        const std::string& Bucket() const { return bucket_; }
        const std::string& Prefix() const { return prefix_; }
        const InventoryEncryption& Encryption() const { return encryption_; }

        void setFormat(InventoryFormat format) { format_ = format; }
        void setAccountId(const std::string& accountId) { accountId_ = accountId; }
        void setRoleArn(const std::string& roleArn) { roleArn_ = roleArn; }
        void setBucket(const std::string& bucket) { bucket_ = bucket; }
        void setPrefix(const std::string& prefix) { prefix_ = prefix; }
        void setEncryption(const InventoryEncryption& encryption) { encryption_ = encryption; }

    private:
        InventoryFormat format_;
        std::string accountId_;
        std::string roleArn_;
        std::string bucket_;
        std::string prefix_;
        InventoryEncryption encryption_;
    };

    class InventoryDestination
    {
    public:
        InventoryDestination() {}
        InventoryDestination(const InventoryOSSBucketDestination& destination):InventoryOSSBucketDestination_(destination){}
        const InventoryOSSBucketDestination& OSSBucketDestination() const { return InventoryOSSBucketDestination_; }
        void setOSSBucketDestination(const InventoryOSSBucketDestination& destination) { InventoryOSSBucketDestination_ = destination; }
    private:
        InventoryOSSBucketDestination InventoryOSSBucketDestination_;
    };

    using InventoryOptionalFields = std::vector<InventoryOptionalField>;

    class ALIBABACLOUD_OSS_EXPORT InventoryConfiguration
    {
    public:
        InventoryConfiguration();
        const std::string& Id() const { return id_; }
        bool IsEnabled() const { return isEnabled_; }
        const InventoryFilter& Filter() const { return filter_; }
        const InventoryDestination& Destination() const { return destination_; }
        const InventoryFrequency& Schedule() const { return schedule_; }
        const InventoryIncludedObjectVersions& IncludedObjectVersions() const { return includedObjectVersions_; }
        const InventoryOptionalFields& OptionalFields() const { return optionalFields_; }

        void setId(const std::string& id) { id_ = id; }
        void setIsEnabled(bool isEnabled) { isEnabled_ = isEnabled; }
        void setFilter(const InventoryFilter& prefix) { filter_ = prefix; }
        void setDestination(const InventoryDestination& destination) { destination_ = destination; }
        void setSchedule(const InventoryFrequency& schedule) { schedule_ = schedule; }
        void setIncludedObjectVersions(const InventoryIncludedObjectVersions& includedObjectVersions) { includedObjectVersions_ = includedObjectVersions; }
        void setOptionalFields(const InventoryOptionalFields& opt) { optionalFields_ = opt; }

    private:
        std::string id_;
        bool isEnabled_;
        InventoryFilter filter_;
        InventoryDestination destination_;
        InventoryFrequency schedule_;
        InventoryIncludedObjectVersions includedObjectVersions_;
        InventoryOptionalFields optionalFields_;
    };

    using InventoryConfigurationList = std::vector<InventoryConfiguration>;
}
}

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
#include <alibabacloud/oss/Types.h>
#include <alibabacloud/oss/OssResult.h>
#include <alibabacloud/oss/model/Owner.h>
namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT GetBucketInfoResult : public OssResult
    {
    public:
        GetBucketInfoResult();
        GetBucketInfoResult(const std::string& data);
        GetBucketInfoResult(const std::shared_ptr<std::iostream>& data);
        GetBucketInfoResult& operator=(const std::string& data);
        const std::string& Location() const { return location_; }
        const std::string& Name() const { return name_; }
        const std::string& CreationDate() const { return creationDate_; }
        const std::string& IntranetEndpoint() const { return intranetEndpoint_; }
        const std::string& ExtranetEndpoint() const { return extranetEndpoint_; }
        AlibabaCloud::OSS::StorageClass StorageClass() const { return storageClass_; }
        CannedAccessControlList Acl() const { return acl_; }
        const AlibabaCloud::OSS::Owner& Owner() { return owner_; }
        AlibabaCloud::OSS::DataRedundancyType DataRedundancyType() const { return dataRedundancyType_; }
        const std::string& Comment() const { return comment_; }
        AlibabaCloud::OSS::SSEAlgorithm SSEAlgorithm() { return sseAlgorithm_; }
        const std::string& KMSMasterKeyID() { return kmsMasterKeyID_; }
        AlibabaCloud::OSS::VersioningStatus VersioningStatus() { return versioningStatus_; }
    private:
        std::string location_;
        std::string name_;
        std::string creationDate_;
        std::string intranetEndpoint_;
        std::string extranetEndpoint_;
        AlibabaCloud::OSS::StorageClass storageClass_;
        CannedAccessControlList acl_;
        AlibabaCloud::OSS::Owner owner_;
        AlibabaCloud::OSS::DataRedundancyType dataRedundancyType_;
        std::string comment_;
        AlibabaCloud::OSS::SSEAlgorithm sseAlgorithm_;
        std::string kmsMasterKeyID_;
        AlibabaCloud::OSS::VersioningStatus versioningStatus_;
    };
} 
}

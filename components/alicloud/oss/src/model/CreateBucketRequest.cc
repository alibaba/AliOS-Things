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

#include <alibabacloud/oss/model/CreateBucketRequest.h>
#include <utils/Utils.h>
using namespace AlibabaCloud::OSS;

CreateBucketRequest::CreateBucketRequest(const std::string &bucket, StorageClass storageClass):
    CreateBucketRequest(bucket, storageClass, CannedAccessControlList::Default)
{
}

CreateBucketRequest::CreateBucketRequest(const std::string &bucket, StorageClass storageClass, CannedAccessControlList acl):
    OssBucketRequest(bucket),
    storageClass_(storageClass),
    acl_(acl),
    dataRedundancyType_(DataRedundancyType::NotSet)
{
}

std::string CreateBucketRequest::payload() const
{
    std::string str;
    str.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    str.append("<CreateBucketConfiguration>\n");
    str.append("<StorageClass>");
    str.append(ToStorageClassName(storageClass_));
    str.append("</StorageClass>\n");
    if (dataRedundancyType_ != DataRedundancyType::NotSet) {
        str.append("<DataRedundancyType>");
        str.append(ToDataRedundancyTypeName(dataRedundancyType_));
        str.append("</DataRedundancyType>\n");
    }
    str.append("</CreateBucketConfiguration>");
    return str;
}

HeaderCollection CreateBucketRequest::specialHeaders() const
{
    if (acl_ < CannedAccessControlList::Default) {
        HeaderCollection headers;
        headers["x-oss-acl"] = ToAclName(acl_);
        return headers;
    }
    return OssRequest::specialHeaders();
}

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

#include <alibabacloud/oss/model/SetBucketAclRequest.h>
#include "../utils/Utils.h"

using namespace AlibabaCloud::OSS;


SetBucketAclRequest::SetBucketAclRequest(const std::string &bucket, CannedAccessControlList acl) :
    OssBucketRequest(bucket),
    acl_(acl)
{
}

void SetBucketAclRequest::setAcl(CannedAccessControlList acl)
{
    acl_ = acl;
}

HeaderCollection SetBucketAclRequest::specialHeaders() const
{
    HeaderCollection headers;
    if (acl_ < CannedAccessControlList::Default) {
        headers["x-oss-acl"] = ToAclName(acl_);
    }
    return headers;
}

ParameterCollection SetBucketAclRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["acl"] = "";
    return parameters;
}


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

#include <alibabacloud/oss/model/SetBucketVersioningRequest.h>
#include <sstream>
#include <../utils/Utils.h>
#include "ModelError.h"

using namespace AlibabaCloud::OSS;

SetBucketVersioningRequest::SetBucketVersioningRequest(const std::string& bucket, VersioningStatus status) :
    OssBucketRequest(bucket),
    status_(status)
{
}

void SetBucketVersioningRequest::setStatus(VersioningStatus status)
{
    status_ = status;
}

std::string SetBucketVersioningRequest::payload() const
{
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<VersioningConfiguration>"  << std::endl;
    ss << "<Status>" << ToVersioningStatusName(status_) << "</Status>" << std::endl;
    ss << "</VersioningConfiguration>" << std::endl;
    return ss.str();
}

ParameterCollection SetBucketVersioningRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["versioning"] = "";
    return parameters;
}

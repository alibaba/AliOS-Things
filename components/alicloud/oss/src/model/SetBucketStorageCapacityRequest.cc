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

#include <alibabacloud/oss/model/SetBucketStorageCapacityRequest.h>
#include "utils/Utils.h"
#include "ModelError.h"
#include <sstream>

using namespace AlibabaCloud::OSS;

SetBucketStorageCapacityRequest::SetBucketStorageCapacityRequest(const std::string &bucket, int64_t storageCapacity) :
    OssBucketRequest(bucket),
    storageCapacity_(storageCapacity)
{
    setFlags(Flags() | REQUEST_FLAG_CONTENTMD5);
}

ParameterCollection SetBucketStorageCapacityRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["qos"] = "";
    return parameters;
}

std::string SetBucketStorageCapacityRequest::payload() const
{
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<BucketUserQos>" << std::endl;
    ss << "  <StorageCapacity>" << std::to_string(storageCapacity_) << "</StorageCapacity>" << std::endl;
    ss << "</BucketUserQos>" << std::endl;
    return ss.str();
}

int SetBucketStorageCapacityRequest::validate() const
{
    int ret;
    if ((ret = OssBucketRequest::validate()) != 0) {
        return ret;
    }

    if (storageCapacity_ < 0)
        return ARG_ERROR_STORAGECAPACITY_INVALID;

    return 0;
}



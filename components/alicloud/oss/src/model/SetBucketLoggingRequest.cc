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

#include <alibabacloud/oss/model/SetBucketLoggingRequest.h>
#include <sstream>
#include <utils/Utils.h>
#include "ModelError.h"

using namespace AlibabaCloud::OSS;

SetBucketLoggingRequest::SetBucketLoggingRequest(const std::string& bucket) :
    SetBucketLoggingRequest(bucket, "", "")
{
}

SetBucketLoggingRequest::SetBucketLoggingRequest(const std::string& bucket,
    const std::string& targetBucket, const std::string& targetPrefix) :
    OssBucketRequest(bucket),
    targetBucket_(targetBucket),
    targetPrefix_(targetPrefix)
{
    setFlags(Flags() | REQUEST_FLAG_CONTENTMD5);
}

std::string SetBucketLoggingRequest::payload() const
{
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<BucketLoggingStatus>" << std::endl;
    ss << "<LoggingEnabled>" << std::endl;
    ss << "<TargetBucket>" << targetBucket_ << "</TargetBucket>" << std::endl;
    ss << "<TargetPrefix>" << targetPrefix_ << "</TargetPrefix>" << std::endl;
    ss << "</LoggingEnabled>" << std::endl;
    ss << "</BucketLoggingStatus>" << std::endl;
    return ss.str();
}

ParameterCollection SetBucketLoggingRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["logging"] = "";
    return parameters;
}

int SetBucketLoggingRequest::validate() const
{
    int ret;
    if ((ret = OssBucketRequest::validate()) != 0 ) {
        return ret;
    }

    if (!IsValidBucketName(targetBucket_)) {
        return ARG_ERROR_BUCKET_NAME;
    }

    if (!IsValidLoggingPrefix(targetPrefix_)) {
        return ARG_ERROR_LOGGING_TARGETPREFIX_INVALID;
    }

    return 0;
}

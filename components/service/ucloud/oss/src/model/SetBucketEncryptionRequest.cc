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

#include <alibabacloud/oss/model/SetBucketEncryptionRequest.h>
#include "../utils/Utils.h"
#include <sstream>

using namespace AlibabaCloud::OSS;


SetBucketEncryptionRequest::SetBucketEncryptionRequest(const std::string& bucket, SSEAlgorithm sse, const std::string& key):
    OssBucketRequest(bucket),
    SSEAlgorithm_(sse),
    KMSMasterKeyID_(key)
{
    setFlags(Flags() | REQUEST_FLAG_CONTENTMD5);
}

void SetBucketEncryptionRequest::setSSEAlgorithm(SSEAlgorithm sse)
{
    SSEAlgorithm_ = sse;
}

void SetBucketEncryptionRequest::setKMSMasterKeyID(const std::string& key)
{
    KMSMasterKeyID_ = key;
}

ParameterCollection SetBucketEncryptionRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["encryption"] = "";
    return parameters;
}

std::string SetBucketEncryptionRequest::payload() const
{
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<ServerSideEncryptionRule>" << std::endl;
    ss << "  <ApplyServerSideEncryptionByDefault>" << std::endl; 
    ss << "<SSEAlgorithm>" << ToSSEAlgorithmName(SSEAlgorithm_) << "</SSEAlgorithm>" << std::endl;
    ss << "<KMSMasterKeyID>" << KMSMasterKeyID_ << "</KMSMasterKeyID>" << std::endl;
    ss << "</ApplyServerSideEncryptionByDefault>" << std::endl;
    ss << "</ServerSideEncryptionRule>" << std::endl;
    return ss.str();
}


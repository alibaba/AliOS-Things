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


#include <alibabacloud/oss/model/ProcessObjectRequest.h>
#include <sstream>
using namespace AlibabaCloud::OSS;

ProcessObjectRequest::ProcessObjectRequest(const std::string &bucket, const std::string &key):
    ProcessObjectRequest(bucket, key, "")
{
}

ProcessObjectRequest::ProcessObjectRequest(const std::string &bucket, const std::string &key, const std::string &process) :
    OssObjectRequest(bucket, key),
    process_(process)
{
    setFlags(Flags() | REQUEST_FLAG_CONTENTMD5);
}


void ProcessObjectRequest::setProcess(const std::string &process)
{
    process_ = process;
}

ParameterCollection ProcessObjectRequest::specialParameters() const
{
    auto parameters = OssObjectRequest::specialParameters();
    parameters["x-oss-process"];
    return parameters;
}

std::string  ProcessObjectRequest::payload() const
{
    std::stringstream ss;
    ss << "x-oss-process=" << process_;
    return ss.str();
}



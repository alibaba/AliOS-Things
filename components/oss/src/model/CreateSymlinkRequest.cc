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

#include <alibabacloud/oss/model/CreateSymlinkRequest.h>
#include <alibabacloud/oss/http/HttpType.h>
#include "utils/Utils.h"

using namespace AlibabaCloud::OSS;

CreateSymlinkRequest::CreateSymlinkRequest(const std::string &bucket, const std::string &key):
    OssObjectRequest(bucket, key)
{
}

CreateSymlinkRequest::CreateSymlinkRequest(const std::string &bucket, const std::string &key,
    const ObjectMetaData &metaData):
    OssObjectRequest(bucket, key),
    metaData_(metaData)     
{
}

void CreateSymlinkRequest::SetSymlinkTarget(const std::string& value)
{
    metaData_.addHeader("x-oss-symlink-target", value);
}

void CreateSymlinkRequest::setTagging(const std::string& value)
{
    metaData_.addHeader("x-oss-tagging", value);
}

HeaderCollection CreateSymlinkRequest::specialHeaders() const
{
	auto headers = metaData_.toHeaderCollection();
    auto baseHeaders = OssObjectRequest::specialHeaders();
    headers.insert(baseHeaders.begin(), baseHeaders.end());
    return headers;
}


ParameterCollection CreateSymlinkRequest::specialParameters() const
{
    ParameterCollection paramters;
    paramters["symlink"] = "";
    return paramters;
}



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


#include <alibabacloud/oss/model/GetObjectResult.h>
#include <alibabacloud/oss/http/HttpType.h>
#include "utils/Utils.h"

using namespace AlibabaCloud::OSS;

GetObjectResult::GetObjectResult() :
    OssObjectResult()
{
}

GetObjectResult::GetObjectResult(
    const std::string &bucket,
    const std::string &key,
    const std::shared_ptr<std::iostream> &content,
    const HeaderCollection &headers):
    OssObjectResult(headers),
    bucket_(bucket),
    key_(key),
    content_(content)
{
    metaData_  = headers;
    std::string etag = metaData_.HttpMetaData()[Http::ETAG];
    metaData_.HttpMetaData()[Http::ETAG] = TrimQuotes(etag.c_str());
}

GetObjectResult::GetObjectResult(
    const std::string& bucket, const std::string& key,
    const ObjectMetaData& metaData) :
    bucket_(bucket),
    key_(key)
{
    metaData_ = metaData;
    requestId_ = metaData_.HttpMetaData()["x-oss-request-id"];
    versionId_ = metaData_.HttpMetaData()["x-oss-version-id"]; 
}
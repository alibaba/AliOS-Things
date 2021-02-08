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

#include <alibabacloud/oss/model/GetSymlinkResult.h>
#include <alibabacloud/oss/http/HttpType.h>
#include "../utils/Utils.h"

using namespace AlibabaCloud::OSS;

GetSymlinkResult::GetSymlinkResult():
    OssObjectResult()
{
}

GetSymlinkResult::GetSymlinkResult(const std::string& symlink,
    const std::string& etag) :
    OssObjectResult(),
    symlink_(symlink),
    etag_(etag)
{
}

GetSymlinkResult::GetSymlinkResult(const HeaderCollection& headers):
    OssObjectResult(headers)
{
    if (headers.find("x-oss-symlink-target") != headers.end()) {
        symlink_ = headers.at("x-oss-symlink-target");
    }

    if (headers.find(Http::ETAG) != headers.end()) {
        etag_ = TrimQuotes(headers.at(Http::ETAG).c_str());
    }
}



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

#include <alibabacloud/oss/OssResult.h>
#include <sstream>

using namespace AlibabaCloud::OSS;

OssResult::OssResult() :
    parseDone_(false) 
{
}

OssResult::OssResult(const HeaderCollection& header):
    OssResult()
{
    if (header.find("x-oss-request-id") != header.end()) {
        requestId_ = header.at("x-oss-request-id");
    }
}

OssObjectResult::OssObjectResult() :
    OssResult()
{
}

OssObjectResult::OssObjectResult(const HeaderCollection& header) :
    OssResult(header)
{
    if (header.find("x-oss-version-id") != header.end()) {
        versionId_ = header.at("x-oss-version-id");
    }
}

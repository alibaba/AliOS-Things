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


#include <alibabacloud/oss/model/AppendObjectResult.h>
#include "utils/Utils.h"
using namespace AlibabaCloud::OSS;

AppendObjectResult::AppendObjectResult():
    OssObjectResult(),
    length_(0),
    crc64_(0)
{
}

AppendObjectResult::AppendObjectResult(const HeaderCollection& headers):
    OssObjectResult(headers),
    length_(0),
    crc64_(0)
{
   if (headers.find("x-oss-next-append-position") == headers.end()) {
       parseDone_ = false;
   } else {
       length_ = std::strtoull(headers.at("x-oss-next-append-position").c_str(), nullptr, 10);
   }

   if (headers.find("x-oss-hash-crc64ecma") == headers.end()) {
       parseDone_ = false;
   } else {
       crc64_  = std::strtoull(headers.at("x-oss-hash-crc64ecma").c_str(), nullptr, 10);
   }
   parseDone_ = true;
}


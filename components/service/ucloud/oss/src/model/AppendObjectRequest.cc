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


#include <alibabacloud/oss/model/AppendObjectRequest.h>
#include <alibabacloud/oss/http/HttpType.h>
#include "../utils/Utils.h"
using namespace AlibabaCloud::OSS;

AppendObjectRequest::AppendObjectRequest(const std::string &bucket, const std::string &key,
        const std::shared_ptr<std::iostream>& content):
    OssObjectRequest(bucket, key),
    position_(0),
    content_(content)
{
}

AppendObjectRequest::AppendObjectRequest(const std::string &bucket, const std::string &key,
        const std::shared_ptr<std::iostream>& content, const ObjectMetaData &metaData):
    OssObjectRequest(bucket, key),
    position_(0),
    content_(content),
    metaData_(metaData)
{
}

void AppendObjectRequest::setPosition(uint64_t position)
{
    position_ = position;
}

void AppendObjectRequest::setCacheControl(const std::string& value)
{
    metaData_.addHeader(Http::CACHE_CONTROL, value);
}

void AppendObjectRequest::setContentDisposition(const std::string& value)
{
    metaData_.addHeader(Http::CONTENT_DISPOSITION, value);
}

void AppendObjectRequest::setContentEncoding(const std::string& value)
{
    metaData_.addHeader(Http::CONTENT_ENCODING, value);
}

void AppendObjectRequest::setContentMd5(const std::string& value)
{
    metaData_.addHeader(Http::CONTENT_MD5, value);
}

void AppendObjectRequest::setExpires(uint64_t expires)
{
	metaData_.addHeader(Http::EXPIRES, std::to_string(expires));
}

void AppendObjectRequest::setExpires(const std::string& value)
{
    metaData_.addHeader(Http::EXPIRES, value);
}

void AppendObjectRequest::setAcl(const CannedAccessControlList& acl)
{
	metaData_.addHeader("x-oss-object-acl", ToAclName(acl));
}

void AppendObjectRequest::setTagging(const std::string& value)
{
    metaData_.addHeader("x-oss-tagging", value);
}

void AppendObjectRequest::setTrafficLimit(uint64_t value)
{
    metaData_.addHeader("x-oss-traffic-limit", std::to_string(value));
}

std::shared_ptr<std::iostream> AppendObjectRequest::Body() const
{
    return content_;
}


HeaderCollection AppendObjectRequest::specialHeaders() const
{
    auto headers = metaData_.toHeaderCollection();

    if (headers.find(Http::CONTENT_TYPE) == headers.end()) {
        headers[Http::CONTENT_TYPE] = LookupMimeType(Key());
    }

    auto baseHeaders = OssObjectRequest::specialHeaders();
    headers.insert(baseHeaders.begin(), baseHeaders.end());

    return headers;
}

ParameterCollection AppendObjectRequest::specialParameters() const
{
    ParameterCollection paramters;
    paramters["append"] = "";
    paramters["position"] = std::to_string(position_);

    return paramters;
}



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


#include <alibabacloud/oss/model/PutObjectRequest.h>
#include <alibabacloud/oss/http/HttpType.h>
#include "utils/Utils.h"
#include "ModelError.h"
#include <sstream>
using namespace AlibabaCloud::OSS;

PutObjectRequest::PutObjectRequest(const std::string &bucket, const std::string &key,
    const std::shared_ptr<std::iostream> &content) :
    OssObjectRequest(bucket, key),
    content_(content)
{
    setFlags(Flags() | REQUEST_FLAG_CHECK_CRC64);
}

PutObjectRequest::PutObjectRequest(const std::string &bucket, const std::string &key,
    const std::shared_ptr<std::iostream> &content, const ObjectMetaData &metaData) :
    OssObjectRequest(bucket, key),
    content_(content),
    metaData_(metaData)
{
    setFlags(Flags() | REQUEST_FLAG_CHECK_CRC64);
}

void PutObjectRequest::setCacheControl(const std::string &value)
{
    metaData_.addHeader(Http::CACHE_CONTROL, value);
}

void PutObjectRequest::setContentDisposition(const std::string &value)
{
    metaData_.addHeader(Http::CONTENT_DISPOSITION, value);
}

void PutObjectRequest::setContentEncoding(const std::string &value)
{
    metaData_.addHeader(Http::CONTENT_ENCODING, value);
}

void PutObjectRequest::setContentMd5(const std::string &value)
{
    metaData_.addHeader(Http::CONTENT_MD5, value);
}

void PutObjectRequest::setExpires(const std::string &value)
{
    metaData_.addHeader(Http::EXPIRES, value);
}

void PutObjectRequest::setCallback(const std::string& callback, const std::string& callbackVar)
{
    metaData_.removeHeader("x-oss-callback");
    metaData_.removeHeader("x-oss-callback-var");

    if (!callback.empty()) {
        metaData_.addHeader("x-oss-callback", callback);
    }

    if (!callbackVar.empty()) {
        metaData_.addHeader("x-oss-callback-var", callbackVar);
    }
}

void PutObjectRequest::setTagging(const std::string& value)
{
    metaData_.addHeader("x-oss-tagging", value);
}

void PutObjectRequest::setTrafficLimit(uint64_t value)
{
    metaData_.addHeader("x-oss-traffic-limit", std::to_string(value));
}

ObjectMetaData &PutObjectRequest::MetaData()
{
    return metaData_;
}

std::shared_ptr<std::iostream> PutObjectRequest::Body() const
{
    return content_;
}

HeaderCollection PutObjectRequest::specialHeaders() const
{
    auto headers = metaData_.toHeaderCollection();

    if (headers.find(Http::CONTENT_TYPE) == headers.end()) {
        headers[Http::CONTENT_TYPE] = LookupMimeType(Key());
    }

    auto baseHeaders = OssObjectRequest::specialHeaders();
    headers.insert(baseHeaders.begin(), baseHeaders.end());

    return headers;
}

int PutObjectRequest::validate() const
{
    int ret = OssObjectRequest::validate();
    if (ret != 0) {
        return ret;
    }

    if (content_ == nullptr) {
        return ARG_ERROR_REQUEST_BODY_NULLPTR;
    }

    if (content_->bad()) {
        return ARG_ERROR_REQUEST_BODY_BAD_STATE;
    }

    if (content_->fail()) {
        return ARG_ERROR_REQUEST_BODY_FAIL_STATE;
    }

    return 0;
}


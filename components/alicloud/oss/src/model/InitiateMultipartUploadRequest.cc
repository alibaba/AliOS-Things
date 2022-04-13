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


#include <alibabacloud/oss/model/InitiateMultipartUploadRequest.h>
#include <alibabacloud/oss/http/HttpType.h>
#include "utils/Utils.h"
#include <sstream>
using namespace AlibabaCloud::OSS;

InitiateMultipartUploadRequest::InitiateMultipartUploadRequest(const std::string &bucket, const std::string &key) :
    InitiateMultipartUploadRequest(bucket, key, ObjectMetaData())
{
}

InitiateMultipartUploadRequest::InitiateMultipartUploadRequest(const std::string &bucket, const std::string &key, 
                                                               const ObjectMetaData &metaData) :
    OssObjectRequest(bucket, key),
    metaData_(metaData),
    encodingTypeIsSet_(false),
    sequential_(false)
{
}

void InitiateMultipartUploadRequest::setEncodingType(const std::string &encodingType)
{
    encodingType_ = encodingType;
    encodingTypeIsSet_ = true;
}

void InitiateMultipartUploadRequest::setCacheControl(const std::string &value)
{
    metaData_.addHeader(Http::CACHE_CONTROL, value);
}

void InitiateMultipartUploadRequest::setContentDisposition(const std::string &value)
{
    metaData_.addHeader(Http::CONTENT_DISPOSITION, value);
}

void InitiateMultipartUploadRequest::setContentEncoding(const std::string &value)
{
    metaData_.addHeader(Http::CONTENT_ENCODING, value);
}

void InitiateMultipartUploadRequest::setExpires(const std::string &value)
{
    metaData_.addHeader(Http::EXPIRES, value);
}

void InitiateMultipartUploadRequest::setTagging(const std::string& value)
{
    metaData_.addHeader("x-oss-tagging", value);
}

void InitiateMultipartUploadRequest::setSequential(bool value)
{
    sequential_ = value;
}

ObjectMetaData &InitiateMultipartUploadRequest::MetaData()
{
    return metaData_;
}

HeaderCollection InitiateMultipartUploadRequest::specialHeaders() const
{
    auto headers = metaData_.toHeaderCollection();
    if (headers.find(Http::CONTENT_TYPE) == headers.end()) {
        headers[Http::CONTENT_TYPE] = LookupMimeType(Key());
    }

    auto baseHeaders = OssObjectRequest::specialHeaders();
    headers.insert(baseHeaders.begin(), baseHeaders.end());

    return headers;
}

ParameterCollection InitiateMultipartUploadRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["uploads"] = "";
    if (encodingTypeIsSet_) {
        parameters["encoding-type"] = encodingType_;
    }

    if (sequential_) {
        parameters["sequential"] = "";
    }
    return parameters;
}

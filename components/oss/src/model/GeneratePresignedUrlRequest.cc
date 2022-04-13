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


#include <alibabacloud/oss/model/GeneratePresignedUrlRequest.h>
#include <alibabacloud/oss/http/HttpType.h>
#include "utils/Utils.h"
#include <sstream>
#include <chrono>
using namespace AlibabaCloud::OSS;

GeneratePresignedUrlRequest::GeneratePresignedUrlRequest(const std::string &bucket, const std::string &key) :
    GeneratePresignedUrlRequest(bucket, key, Http::Method::Get)
{
}

GeneratePresignedUrlRequest::GeneratePresignedUrlRequest(const std::string &bucket, const std::string &key, Http::Method method):
    bucket_(bucket),
    key_(key),
    method_(method)
{
    //defalt 15 min
    std::time_t t = std::time(nullptr) + 15*60;
    metaData_.setExpirationTime(std::to_string(t));
}

void GeneratePresignedUrlRequest::setBucket(const std::string &bucket)
{
    bucket_ = bucket;
}

void GeneratePresignedUrlRequest::setKey(const std::string &key)
{
    key_ = key;
}

void GeneratePresignedUrlRequest::setContentType(const std::string &value)
{
    metaData_.HttpMetaData()[Http::CONTENT_TYPE] = value;
}

void GeneratePresignedUrlRequest::setContentMd5(const std::string &value)
{
    metaData_.HttpMetaData()[Http::CONTENT_MD5] = value;
}

void GeneratePresignedUrlRequest::setExpires(int64_t unixTime)
{
    metaData_.setExpirationTime(std::to_string(unixTime));
}

void GeneratePresignedUrlRequest::setProcess(const std::string &value)
{
    parameters_["x-oss-process"] = value;
}

void GeneratePresignedUrlRequest::setTrafficLimit(uint64_t value)
{
    parameters_["x-oss-traffic-limit"] = std::to_string(value);
}

void GeneratePresignedUrlRequest::setVersionId(const std::string& versionId)
{
    parameters_["versionId"] = versionId;
}

void GeneratePresignedUrlRequest::setRequestPayer(RequestPayer value)
{
    if (value == RequestPayer::Requester) {
        parameters_["x-oss-request-payer"] = ToLower(ToRequestPayerName(value));
    }
}

void GeneratePresignedUrlRequest::addResponseHeaders(RequestResponseHeader header, const std::string &value)
{
    static const char *ResponseHeader[] = {
        "response-content-type", "response-content-language",
        "response-expires", "response-cache-control",
        "response-content-disposition", "response-content-encoding" };
    parameters_[ResponseHeader[header - RequestResponseHeader::ContentType]] = value;
}

void GeneratePresignedUrlRequest::addParameter(const std::string&key, const std::string &value)
{
    parameters_[key] = value;
}

MetaData &GeneratePresignedUrlRequest::UserMetaData()
{
    return metaData_.UserMetaData();
}

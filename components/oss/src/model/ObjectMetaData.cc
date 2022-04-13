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

#include <alibabacloud/oss/model/ObjectMetaData.h>
#include <alibabacloud/oss/http/HttpType.h>
#include "utils/Utils.h"

using namespace AlibabaCloud::OSS;

static const std::string gEmpty = "";

ObjectMetaData::ObjectMetaData(const HeaderCollection& data)
{
    *this = data;
}

ObjectMetaData& ObjectMetaData::operator=(const HeaderCollection& data)
{
    for (auto const &header : data) {
        if (!header.first.compare(0, 11, "x-oss-meta-", 11))
            userMetaData_[header.first.substr(11)] = header.second;
        else
            metaData_[header.first] = header.second;
    }

    if (metaData_.find(Http::ETAG) != metaData_.end()) {
        metaData_[Http::ETAG] = TrimQuotes(metaData_.at(Http::ETAG).c_str());
    }

    return *this;
}

const std::string &ObjectMetaData::LastModified() const
{
    if (metaData_.find(Http::LAST_MODIFIED) != metaData_.end()) {
        return metaData_.at(Http::LAST_MODIFIED);
    }
    return gEmpty;
}

const std::string &ObjectMetaData::ExpirationTime() const
{
    if (metaData_.find(Http::EXPIRES) != metaData_.end()) {
        return metaData_.at(Http::EXPIRES);
    }
    return gEmpty;
}

int64_t ObjectMetaData::ContentLength() const
{
    if (metaData_.find(Http::CONTENT_LENGTH) != metaData_.end()) {
        return atoll(metaData_.at(Http::CONTENT_LENGTH).c_str());
    }
    return -1;
}

const std::string &ObjectMetaData::ContentType() const
{
    if (metaData_.find(Http::CONTENT_TYPE) != metaData_.end()) {
        return metaData_.at(Http::CONTENT_TYPE);
    }
    return gEmpty;
}

const std::string &ObjectMetaData::ContentEncoding() const
{
    if (metaData_.find(Http::CONTENT_ENCODING) != metaData_.end()) {
        return metaData_.at(Http::CONTENT_ENCODING);
    }
    return gEmpty;
}

const std::string &ObjectMetaData::CacheControl() const
{
    if (metaData_.find(Http::CACHE_CONTROL) != metaData_.end()) {
        return metaData_.at(Http::CACHE_CONTROL);
    }
    return gEmpty;
}

const std::string &ObjectMetaData::ContentDisposition() const
{
    if (metaData_.find(Http::CONTENT_DISPOSITION) != metaData_.end()) {
        return metaData_.at(Http::CONTENT_DISPOSITION);
    }
    return gEmpty;
}

const std::string &ObjectMetaData::ETag() const
{
    if (metaData_.find(Http::ETAG) != metaData_.end()) {
        return metaData_.at(Http::ETAG);
    }
    return gEmpty;
}

const std::string &ObjectMetaData::ContentMd5() const
{
    if (metaData_.find(Http::CONTENT_MD5) != metaData_.end()) {
        return metaData_.at(Http::CONTENT_MD5);
    }
    return gEmpty;
}

uint64_t ObjectMetaData::CRC64() const
{
    if (metaData_.find("x-oss-hash-crc64ecma") != metaData_.end()) {
        return std::strtoull(metaData_.at("x-oss-hash-crc64ecma").c_str(), nullptr, 10);
    }
    return 0ULL;
}

const std::string &ObjectMetaData::ObjectType() const
{
    if (metaData_.find("x-oss-object-type") != metaData_.end()) {
        return metaData_.at("x-oss-object-type");
    }
    return gEmpty;
}

const std::string& ObjectMetaData::VersionId() const
{
    if (metaData_.find("x-oss-version-id") != metaData_.end()) {
        return metaData_.at("x-oss-version-id");
    }
    return gEmpty;
}

void ObjectMetaData::setExpirationTime(const std::string &value)
{
    metaData_[Http::EXPIRES] = value;
}

void ObjectMetaData::setContentLength(int64_t value)
{
    metaData_[Http::CONTENT_LENGTH] = std::to_string(value);
}

void ObjectMetaData::setContentType(const std::string &value)
{
    metaData_[Http::CONTENT_TYPE] = value;
}

void ObjectMetaData::setContentEncoding(const std::string &value)
{
    metaData_[Http::CONTENT_ENCODING] = value;
}

void ObjectMetaData::setCacheControl(const std::string &value)
{
    metaData_[Http::CACHE_CONTROL] = value;
}

void ObjectMetaData::setContentDisposition(const std::string &value)
{
    metaData_[Http::CONTENT_DISPOSITION] = value;
}

void ObjectMetaData::setETag(const std::string &value)
{
    metaData_[Http::ETAG] = value;
}

void ObjectMetaData::setContentMd5(const std::string &value)
{
    metaData_[Http::CONTENT_MD5] = value;
}

void ObjectMetaData::setCrc64(uint64_t value)
{
    metaData_["x-oss-hash-crc64ecma"] = std::to_string(value);
}

void ObjectMetaData::addHeader(const std::string &key, const std::string &value)
{
    metaData_[key] = value;
}

bool ObjectMetaData::hasHeader(const std::string& key) const
{
    return (metaData_.find(key) != metaData_.end());
}

void ObjectMetaData::removeHeader(const std::string& key)
{
    if (metaData_.find(key) != metaData_.end()) {
        metaData_.erase(key);
    }
}

MetaData &ObjectMetaData::HttpMetaData()
{
    return metaData_;
}

const MetaData &ObjectMetaData::HttpMetaData() const
{
    return metaData_;
}

void ObjectMetaData::addUserHeader(const std::string &key, const std::string &value)
{
    userMetaData_[key] = value;
}

bool ObjectMetaData::hasUserHeader(const std::string& key) const
{
    return (userMetaData_.find(key) != userMetaData_.end());
}

void ObjectMetaData::removeUserHeader(const std::string& key)
{
    if (userMetaData_.find(key) != userMetaData_.end()) {
        userMetaData_.erase(key);
    }
}

MetaData &ObjectMetaData::UserMetaData()
{
    return userMetaData_;
}

const MetaData &ObjectMetaData::UserMetaData() const
{
    return userMetaData_;
}

HeaderCollection ObjectMetaData::toHeaderCollection() const
{
    HeaderCollection headers;
    for (auto const&header : metaData_) {
        headers[header.first] = header.second;
    }

    for (auto const&header : userMetaData_) {
        std::string key("x-oss-meta-");
        key.append(header.first);
        headers[key] = header.second;
    }
    return headers;
}

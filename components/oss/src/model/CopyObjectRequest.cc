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


#include <alibabacloud/oss/model/CopyObjectRequest.h>
#include <alibabacloud/oss/http/HttpType.h>
#include "utils/Utils.h"
#include <sstream>
using namespace AlibabaCloud::OSS;

CopyObjectRequest::CopyObjectRequest(const std::string &bucket, const std::string &key):
    OssObjectRequest(bucket, key),
    sourceBucket_(),
    sourceKey_()
{
}

CopyObjectRequest::CopyObjectRequest(const std::string &bucket, const std::string &key,
    const ObjectMetaData &metaData):
    OssObjectRequest(bucket, key),
    sourceBucket_(),
    sourceKey_(),
    metaData_(metaData)
{
}

void CopyObjectRequest::setCopySource(const std::string& srcBucket,const std::string& srcKey)
{
    sourceBucket_ = srcBucket;
    sourceKey_ = srcKey;
}

void CopyObjectRequest::setSourceIfMatchETag(const std::string& value)
{
    metaData_.addHeader("x-oss-copy-source-if-match", value);
}

void CopyObjectRequest::setSourceIfNotMatchETag(const std::string& value)
{
    metaData_.addHeader("x-oss-copy-source-if-none-match", value);
}

void CopyObjectRequest::setSourceIfUnModifiedSince(const std::string& value)
{
    metaData_.addHeader("x-oss-copy-source-if-unmodified-since", value);
}

void CopyObjectRequest::setSourceIfModifiedSince(const std::string& value)
{
    metaData_.addHeader("x-oss-copy-source-if-modified-since", value);
}

void CopyObjectRequest::setMetadataDirective(const CopyActionList& action)
{
    metaData_.addHeader("x-oss-metadata-directive", ToCopyActionName(action));
}

void CopyObjectRequest::setAcl(const CannedAccessControlList& acl)
{
    metaData_.addHeader("x-oss-object-acl", ToAclName(acl));
}

void CopyObjectRequest::setTagging(const std::string& value)
{
    metaData_.addHeader("x-oss-tagging", value);
}

void CopyObjectRequest::setTaggingDirective(const CopyActionList& action)
{
    metaData_.addHeader("x-oss-tagging-directive", ToCopyActionName(action));
}

void CopyObjectRequest::setTrafficLimit(uint64_t value)
{
    metaData_.addHeader("x-oss-traffic-limit", std::to_string(value));
}

HeaderCollection CopyObjectRequest::specialHeaders() const
{
    auto headers = metaData_.toHeaderCollection();

    if (headers.find(Http::CONTENT_TYPE) == headers.end()) {
        headers[Http::CONTENT_TYPE] = LookupMimeType(Key());
    }

    std::string source;
    source.append("/").append(sourceBucket_).append("/").append(UrlEncode(sourceKey_));
    if (!versionId_.empty()) {
        source.append("?versionId=").append(versionId_);
    }
    headers["x-oss-copy-source"] = source;

    auto baseHeaders = OssObjectRequest::specialHeaders();
    headers.insert(baseHeaders.begin(), baseHeaders.end());

    return headers;
}

ParameterCollection CopyObjectRequest::specialParameters() const
{
    return ParameterCollection();
}

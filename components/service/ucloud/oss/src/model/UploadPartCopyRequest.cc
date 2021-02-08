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

#include <alibabacloud/oss/model/UploadPartCopyRequest.h>
#include <sstream>
#include "../utils/Utils.h"
#include "ModelError.h"
#include <alibabacloud/oss/Const.h>

using namespace AlibabaCloud::OSS;
using std::stringstream;


UploadPartCopyRequest::UploadPartCopyRequest(const std::string &bucket, const std::string &key) :
    UploadPartCopyRequest(bucket, key, std::string(), std::string(), std::string(), 0)
{
}

UploadPartCopyRequest::UploadPartCopyRequest(const std::string &bucket, const std::string &key,
    const std::string &srcBucket, const std::string &srcKey) :
    UploadPartCopyRequest(bucket, key, srcBucket, srcKey, std::string(), 0)
{
}

UploadPartCopyRequest::UploadPartCopyRequest(const std::string &bucket, const std::string &key,
    const std::string &srcBucket, const std::string &srcKey,
    const std::string &uploadId, int partNumber):
    OssObjectRequest(bucket, key),
    uploadId_(uploadId),
    sourceBucket_(srcBucket),
    sourceKey_(srcKey),
    partNumber_(partNumber),
    sourceRangeIsSet_(false),
    sourceIfMatchETagIsSet_(false),
    sourceIfNotMatchETagIsSet_(false),
    sourceIfModifiedSinceIsSet_(false),
    sourceIfUnModifiedSinceIsSet_(false),
    trafficLimit_(0)
{
}

UploadPartCopyRequest::UploadPartCopyRequest(const std::string& bucket, const std::string& key,
    const std::string& srcBucket, const std::string& srcKey,
    const std::string& uploadId, int partNumber,
    const std::string& sourceIfMatchETag,
    const std::string& sourceIfNotMatchETag,
    const std::string& sourceIfModifiedSince,
    const std::string& sourceIfUnModifiedSince) :
    OssObjectRequest(bucket, key),
    uploadId_(uploadId),
    sourceBucket_(srcBucket),
    sourceKey_(srcKey),
    partNumber_(partNumber),
    sourceRangeIsSet_(false),
    trafficLimit_(0)
{
    if (!sourceIfMatchETag.empty()) {
        sourceIfMatchETag_ = sourceIfMatchETag;
        sourceIfMatchETagIsSet_ = true;
    }
    else {
        sourceIfMatchETagIsSet_ = false;
    }

    if (!sourceIfNotMatchETag.empty()) {
        sourceIfNotMatchETag_ = sourceIfNotMatchETag;
        sourceIfNotMatchETagIsSet_ = true;
    }
    else {
        sourceIfNotMatchETagIsSet_ = false;
    }

    if (!sourceIfModifiedSince.empty()) {
        sourceIfModifiedSince_ = sourceIfModifiedSince;
        sourceIfModifiedSinceIsSet_ = true;
    }
    else {
        sourceIfModifiedSinceIsSet_ = false;
    }

    if (!sourceIfUnModifiedSince.empty()) {
        sourceIfUnModifiedSince_ = sourceIfUnModifiedSince;
        sourceIfUnModifiedSinceIsSet_ = true;
    }
    else {
        sourceIfUnModifiedSinceIsSet_ = false;
    }
}

void UploadPartCopyRequest::setPartNumber(uint32_t partNumber)
{
    partNumber_ = partNumber; 
}

void UploadPartCopyRequest::setUploadId(const std::string &uploadId)
{
    uploadId_ = uploadId; 
}

void UploadPartCopyRequest::SetCopySource(const std::string& srcBucket, const std::string& srcKey)
{
    sourceBucket_ = srcBucket;
    sourceKey_ = srcKey;
}

void UploadPartCopyRequest::setCopySourceRange(uint64_t begin, uint64_t end)
{
    sourceRange_[0] = begin;
    sourceRange_[1] = end;
    sourceRangeIsSet_ = true;
}

void UploadPartCopyRequest::SetSourceIfMatchETag(const std::string& value)
{
    sourceIfMatchETag_ = value;
    sourceIfMatchETagIsSet_ = true;
}

void UploadPartCopyRequest::SetSourceIfNotMatchETag(const std::string& value)
{
    sourceIfNotMatchETag_ = value;
    sourceIfNotMatchETagIsSet_ = true;
}

void UploadPartCopyRequest::SetSourceIfModifiedSince(const std::string& value)
{
    sourceIfModifiedSince_ = value;
    sourceIfModifiedSinceIsSet_ = true;
}

void UploadPartCopyRequest::SetSourceIfUnModifiedSince(const std::string& value)
{
    sourceIfUnModifiedSince_ = value;
    sourceIfUnModifiedSinceIsSet_ = true;
}

void UploadPartCopyRequest::setTrafficLimit(uint64_t value)
{
    trafficLimit_ = value;
}

ParameterCollection UploadPartCopyRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["partNumber"] = std::to_string(partNumber_);
    parameters["uploadId"] = uploadId_;
    return parameters;
}

HeaderCollection UploadPartCopyRequest::specialHeaders() const
{
    auto headers = OssObjectRequest::specialHeaders();
    std::string source;
    source.append("/").append(sourceBucket_).append("/").append(UrlEncode(sourceKey_));
    if (!versionId_.empty()) {
        source.append("?versionId=").append(versionId_);
    }
    headers["x-oss-copy-source"] = source;

    if (sourceRangeIsSet_) {
        std::string range("bytes=");
        range.append(std::to_string(sourceRange_[0])).append("-");
        if (sourceRange_[1] > 0){
            range.append(std::to_string(sourceRange_[1]));
        }
        headers["x-oss-copy-source-range"] = range;
    }

    if(sourceIfMatchETagIsSet_) {
        headers["x-oss-copy-source-if-match"] = sourceIfMatchETag_;
    }

    if(sourceIfNotMatchETagIsSet_) {
        headers["x-oss-copy-source-if-none-match"] = sourceIfNotMatchETag_;
    }

    if(sourceIfModifiedSinceIsSet_) {
        headers["x-oss-copy-source-if-modified-since"] = sourceIfModifiedSince_;
    }

    if (sourceIfUnModifiedSinceIsSet_) {
        headers["x-oss-copy-source-if-unmodified-since"] = sourceIfUnModifiedSince_;
    }

    if (trafficLimit_ != 0) {
        headers["x-oss-traffic-limit"] = std::to_string(trafficLimit_);
    }

    return headers;
}

int UploadPartCopyRequest::validate() const
{
    int ret = OssObjectRequest::validate();
    if (ret != 0){
        return ret;
    }

    if (!IsValidBucketName(sourceBucket_)) {
        return ARG_ERROR_BUCKET_NAME;
    }

    if (!IsValidObjectKey(sourceKey_)) {
        return ARG_ERROR_OBJECT_NAME;
    }

    if (sourceRangeIsSet_ && 
          ((sourceRange_[1] < sourceRange_[0]) || 
           ((sourceRange_[1] - sourceRange_[0] + 1) > MaxFileSize))) {
        return ARG_ERROR_MULTIPARTUPLOAD_PARTSIZE_RANGE;
    }

    if(!(partNumber_ > 0 && partNumber_ < PartNumberUpperLimit)){
        return ARG_ERROR_MULTIPARTUPLOAD_PARTNUMBER_RANGE;
    }

    return 0;
}

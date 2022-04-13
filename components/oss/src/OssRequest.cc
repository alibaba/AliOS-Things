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

#include <alibabacloud/oss/OssRequest.h>
#include <alibabacloud/oss/Const.h>
#include <sstream>
#include "http/HttpType.h"
#include "utils/Utils.h"
#include "model/ModelError.h"
#include "utils/FileSystemUtils.h"

using namespace AlibabaCloud::OSS;

 OssRequest:: OssRequest():
    ServiceRequest(),
    bucket_(),
    key_()
{
}

OssRequest:: OssRequest(const std::string &bucket, const std::string &key):
    ServiceRequest(),
    bucket_(bucket),
    key_(key)
{
}


HeaderCollection OssRequest::Headers() const
{
    auto headers = specialHeaders();

    if(headers.size() == 0 || (headers.size() > 0 && headers.count(Http::CONTENT_TYPE) == 0))
    {
        headers[Http::CONTENT_TYPE] = "application/xml";
    }

    return headers;
}

ParameterCollection OssRequest::Parameters() const
{
    return  specialParameters();
}

std::shared_ptr<std::iostream> OssRequest::Body() const
{
    std::string&& p = payload();
    std::shared_ptr<std::iostream> payloadBody;
    if (!p.empty())
    {
      payloadBody = std::make_shared<std::stringstream>();
      *payloadBody << p;
    }
    return payloadBody;
}

int OssRequest::validate() const 
{ 
    return 0; 
}

const char * OssRequest::validateMessage(int code) const
{
    return GetModelErrorMsg(code);
}

std::string OssRequest::payload() const
{
    return "";
}

HeaderCollection OssRequest::specialHeaders() const
{
    return HeaderCollection();
}

ParameterCollection OssRequest::specialParameters() const
{
    return ParameterCollection();
}

const std::string& OssRequest::bucket() const 
{ 
    return bucket_; 
}

const std::string& OssRequest::key()  const 
{ 
    return key_; 
}

int OssBucketRequest::validate() const
{
    if (!IsValidBucketName(Bucket())) {
        return ARG_ERROR_BUCKET_NAME;
    }
    return 0; 
}

void OssBucketRequest::setBucket(const std::string &bucket) 
{ 
    bucket_ = bucket;
}

const std::string& OssBucketRequest::Bucket() const
{ 
    return OssRequest::bucket(); 
}

int OssObjectRequest::validate() const
{
    if (!IsValidBucketName(Bucket())) {
        return ARG_ERROR_BUCKET_NAME;
    }

    if (!IsValidObjectKey(Key())) {
        return ARG_ERROR_OBJECT_NAME;
    }

    return 0;
}

void OssObjectRequest::setBucket(const std::string &bucket)
{
    bucket_ = bucket;
}

const std::string& OssObjectRequest::Bucket() const
{
    return OssRequest::bucket();
}

void OssObjectRequest::setKey(const std::string &key)
{
    key_ = key;
}

const std::string& OssObjectRequest::Key() const
{
    return OssRequest::key();
}


void OssObjectRequest::setRequestPayer(AlibabaCloud::OSS::RequestPayer key)
{
    requestPayer_ = key;
}

AlibabaCloud::OSS::RequestPayer OssObjectRequest::RequestPayer() const
{
    return requestPayer_;
}

void OssObjectRequest::setVersionId(const std::string& versionId)
{
    versionId_ = versionId;
}

const std::string& OssObjectRequest::VersionId() const
{
    return versionId_;
}

HeaderCollection OssObjectRequest::specialHeaders() const
{
    auto headers = OssRequest::specialHeaders();
    if (requestPayer_ == AlibabaCloud::OSS::RequestPayer::Requester) {
        headers["x-oss-request-payer"] = ToLower(ToRequestPayerName(AlibabaCloud::OSS::RequestPayer::Requester));
    }
    return headers;
}

ParameterCollection OssObjectRequest::specialParameters() const
{
    auto parameters = OssRequest::specialParameters();
    if (!versionId_.empty()) {
        parameters["versionId"] = versionId_;
    }
    return parameters;
}

int OssResumableBaseRequest::validate() const
{
    if (!IsValidBucketName(Bucket())) {
        return ARG_ERROR_BUCKET_NAME;
    }

    if (!IsValidObjectKey(Key())) {
        return ARG_ERROR_OBJECT_NAME;
    }

    if (partSize_ < PartSizeLowerLimit) {
        return ARG_ERROR_CHECK_PART_SIZE_LOWER;
    }

    if (threadNum_ <= 0) {
        return ARG_ERROR_CHECK_THREAD_NUM_LOWER;
    }

#if !defined(_WIN32)
    if (!checkpointDirW_.empty()) {
        return ARG_ERROR_PATH_NOT_SUPPORT_WSTRING_TYPE;
    }
#endif

    // if directory do not exist, return error
    if (hasCheckpointDir()) {
        if ((!checkpointDir_.empty() && !IsDirectoryExist(checkpointDir_))
#ifdef _WIN32
            || (!checkpointDirW_.empty() && !IsDirectoryExist(checkpointDirW_))
#endif
            ) {
            return ARG_ERROR_CHECK_POINT_DIR_NONEXIST;
        }
    }

    return 0;
}

const char *OssResumableBaseRequest::validateMessage(int code) const
{
    return GetModelErrorMsg(code);
}

void OssResumableBaseRequest::setBucket(const std::string &bucket)
{
    bucket_ = bucket;
}

const std::string& OssResumableBaseRequest::Bucket() const
{
    return OssRequest::bucket();
}

void OssResumableBaseRequest::setKey(const std::string &key)
{
    key_ = key;
}

const std::string& OssResumableBaseRequest::Key() const
{
    return OssRequest::key();
}

void OssResumableBaseRequest::setPartSize(uint64_t partSize)
{
    partSize_ = partSize;
}

uint64_t OssResumableBaseRequest::PartSize() const
{
    return partSize_;
}

void OssResumableBaseRequest::setObjectSize(uint64_t objectSize)
{
    objectSize_ = objectSize;
}

uint64_t OssResumableBaseRequest::ObjectSize() const
{
    return objectSize_;
}

void OssResumableBaseRequest::setThreadNum(uint32_t threadNum)
{
    threadNum_ = threadNum;
}

uint32_t OssResumableBaseRequest::ThreadNum() const
{
    return threadNum_;
}

void OssResumableBaseRequest::setCheckpointDir(const std::string &checkpointDir)
{
    checkpointDir_ = checkpointDir;
    checkpointDirW_.clear();
}

const std::string& OssResumableBaseRequest::CheckpointDir() const
{
    return checkpointDir_;
}

void OssResumableBaseRequest::setCheckpointDir(const std::wstring& checkpointDir)
{
    checkpointDirW_ = checkpointDir;
    checkpointDir_.clear();
}

const std::wstring& OssResumableBaseRequest::CheckpointDirW() const
{
    return checkpointDirW_;
}

void OssResumableBaseRequest::setObjectMtime(const std::string &mtime)
{
    mtime_ = mtime;
}

const std::string& OssResumableBaseRequest::ObjectMtime() const
{
    return mtime_;
}

void OssResumableBaseRequest::setRequestPayer(AlibabaCloud::OSS::RequestPayer value)
{
    requestPayer_ = value;
}

AlibabaCloud::OSS::RequestPayer OssResumableBaseRequest::RequestPayer() const
{
    return requestPayer_;
}

void OssResumableBaseRequest::setTrafficLimit(uint64_t value)
{
    trafficLimit_ = value;
}

uint64_t OssResumableBaseRequest::TrafficLimit() const
{
    return trafficLimit_;
}

void OssResumableBaseRequest::setVersionId(const std::string& versionId)
{
    versionId_ = versionId;
}

const std::string& OssResumableBaseRequest::VersionId() const
{
    return versionId_;
}

bool OssResumableBaseRequest::hasCheckpointDir() const
{
    return (!checkpointDir_.empty() || !checkpointDirW_.empty());
}

void LiveChannelRequest::setBucket(const std::string &bucket)
{
    bucket_ = bucket;
}

const std::string& LiveChannelRequest::Bucket() const
{
    return bucket_;
}

void LiveChannelRequest::setChannelName(const std::string &channelName)
{
    channelName_ = channelName;
    key_ = channelName;
}

const std::string& LiveChannelRequest::ChannelName() const
{
    return channelName_;
}

int LiveChannelRequest::validate() const
{
    if (!IsValidBucketName(Bucket())) {
        return ARG_ERROR_BUCKET_NAME;
    }

    if(!IsValidChannelName(channelName_))
    {
        return ARG_ERROR_LIVECHANNEL_BAD_CHANNELNAME_PARAM;
    }

    return 0;
}

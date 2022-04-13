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

#include <alibabacloud/oss/model/UploadObjectRequest.h>
#include <alibabacloud/oss/http/HttpType.h>
#include <alibabacloud/oss/Const.h>
#include <fstream>
#include "utils/Utils.h"
#include "../utils/FileSystemUtils.h"
#include "../model/ModelError.h"

using namespace AlibabaCloud::OSS;

UploadObjectRequest::UploadObjectRequest(const std::string &bucket, const std::string &key, 
    const std::string &filePath, const std::string &checkpointDir, 
    const uint64_t partSize, const uint32_t threadNum): 
    OssResumableBaseRequest(bucket, key, checkpointDir, partSize, threadNum), 
    filePath_(filePath)
{
    time_t lastMtime;
    std::streamsize fileSize;
    isFileExist_ = true;
    if (!GetPathInfo(filePath_, lastMtime, fileSize)) {
        //if fail, ignore the lastmodified time.
        lastMtime = 0;
        fileSize = 0;
        isFileExist_ = false;
    }
    mtime_ = ToGmtTime(lastMtime);
    objectSize_ = static_cast<uint64_t>(fileSize);
}

UploadObjectRequest::UploadObjectRequest(const std::string &bucket, const std::string &key,
    const std::string &filePath, const std::string &checkpointDir,
    const uint64_t partSize, const uint32_t threadNum, const ObjectMetaData& meta): 
    UploadObjectRequest(bucket, key, filePath, checkpointDir, partSize, threadNum)
{
    metaData_ = meta;
}

UploadObjectRequest::UploadObjectRequest(const std::string &bucket, const std::string &key, 
    const std::string &filePath, const std::string &checkpointDir, const ObjectMetaData& meta):
    UploadObjectRequest(bucket, key, filePath, checkpointDir, DefaultPartSize, DefaultResumableThreadNum, meta)
{}

UploadObjectRequest::UploadObjectRequest(const std::string &bucket, const std::string &key, 
    const std::string &filePath, const std::string &checkpointDir) : 
    UploadObjectRequest(bucket, key, filePath, checkpointDir, DefaultPartSize, DefaultResumableThreadNum)
{}

UploadObjectRequest::UploadObjectRequest(const std::string &bucket, const std::string &key, 
    const std::string &filePath): 
    UploadObjectRequest(bucket, key, filePath, "", DefaultPartSize, DefaultResumableThreadNum)
{}

//wstring
UploadObjectRequest::UploadObjectRequest(const std::string &bucket, const std::string &key,
    const std::wstring &filePath, const std::wstring &checkpointDir,
    const uint64_t partSize, const uint32_t threadNum) :
    OssResumableBaseRequest(bucket, key, checkpointDir, partSize, threadNum),
    filePathW_(filePath)
{
#ifdef _WIN32
    time_t lastMtime;
    std::streamsize fileSize;
    isFileExist_ = true;
    if (!GetPathInfo(filePathW_, lastMtime, fileSize)) {
        //if fail, ignore the lastmodified time.
        lastMtime = 0;
        fileSize = 0;
        isFileExist_ = false;
    }
    mtime_ = ToGmtTime(lastMtime);
    objectSize_ = static_cast<uint64_t>(fileSize);
#else
    objectSize_ = 0;
    time_t lastMtime = 0;
    mtime_ = ToGmtTime(lastMtime);
    isFileExist_ = false;
#endif
}

UploadObjectRequest::UploadObjectRequest(const std::string &bucket, const std::string &key,
    const std::wstring &filePath, const std::wstring &checkpointDir,
    const uint64_t partSize, const uint32_t threadNum, const ObjectMetaData& meta) :
    UploadObjectRequest(bucket, key, filePath, checkpointDir, partSize, threadNum)
{
    metaData_ = meta;
}

UploadObjectRequest::UploadObjectRequest(const std::string &bucket, const std::string &key,
    const std::wstring &filePath, const std::wstring &checkpointDir, const ObjectMetaData& meta) :
    UploadObjectRequest(bucket, key, filePath, checkpointDir, DefaultPartSize, DefaultResumableThreadNum, meta)
{}

UploadObjectRequest::UploadObjectRequest(const std::string &bucket, const std::string &key,
    const std::wstring &filePath, const std::wstring &checkpointDir) :
    UploadObjectRequest(bucket, key, filePath, checkpointDir, DefaultPartSize, DefaultResumableThreadNum)
{}

UploadObjectRequest::UploadObjectRequest(const std::string &bucket, const std::string &key,
    const std::wstring &filePath) :
    UploadObjectRequest(bucket, key, filePath, L"", DefaultPartSize, DefaultResumableThreadNum)
{}


void UploadObjectRequest::setAcl(CannedAccessControlList& acl)
{
    metaData_.addHeader("x-oss-object-acl", ToAclName(acl));
}

void UploadObjectRequest::setCallback(const std::string& callback, const std::string& callbackVar)
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

void UploadObjectRequest::setTagging(const std::string& value)
{
    metaData_.addHeader("x-oss-tagging", value);
}

int UploadObjectRequest::validate() const 
{
    auto ret = OssResumableBaseRequest::validate();
    if (ret != 0) {
        return ret;
    }

#if !defined(_WIN32)
    if (!filePathW_.empty()) {
        return ARG_ERROR_PATH_NOT_SUPPORT_WSTRING_TYPE;
    }
#endif

    //path and checkpoint must be same type.
    if ((!filePath_.empty() && !checkpointDirW_.empty()) ||
        (!filePathW_.empty() && !checkpointDir_.empty())) {
        return ARG_ERROR_PATH_NOT_SAME_TYPE;
    }

    if (!isFileExist_) {
        return ARG_ERROR_OPEN_UPLOAD_FILE;
    }

    return 0;
}

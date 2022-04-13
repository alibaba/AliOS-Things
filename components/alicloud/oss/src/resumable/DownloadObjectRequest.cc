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


#include <alibabacloud/oss/model/DownloadObjectRequest.h>
#include <alibabacloud/oss/Const.h>
#include <sstream>
#include <fstream>
#include "../model/ModelError.h"
#include "../utils/FileSystemUtils.h"

using namespace AlibabaCloud::OSS;

DownloadObjectRequest::DownloadObjectRequest(const std::string &bucket, const std::string &key, 
    const std::string &filePath, const std::string &checkpointDir,
    const uint64_t partSize, const uint32_t threadNum):
    OssResumableBaseRequest(bucket, key, checkpointDir, partSize, threadNum), 
    rangeIsSet_(false),
    filePath_(filePath)
{
    tempFilePath_ = filePath + ".temp";
}

DownloadObjectRequest::DownloadObjectRequest(const std::string &bucket, const std::string &key, 
    const std::string &filePath, const std::string &checkpointDir) : 
    DownloadObjectRequest(bucket, key, filePath, checkpointDir, DefaultPartSize, DefaultResumableThreadNum)
{}

DownloadObjectRequest::DownloadObjectRequest(const std::string &bucket, const std::string &key, 
    const std::string &filePath) :
    DownloadObjectRequest(bucket, key, filePath, "", DefaultPartSize, DefaultResumableThreadNum)
{}

//wstring
DownloadObjectRequest::DownloadObjectRequest(const std::string &bucket, const std::string &key,
    const std::wstring &filePath, const std::wstring &checkpointDir,
    const uint64_t partSize, const uint32_t threadNum) :
    OssResumableBaseRequest(bucket, key, checkpointDir, partSize, threadNum),
    rangeIsSet_(false),
    filePathW_(filePath)
{
    tempFilePathW_ = filePath + L".temp";
}

DownloadObjectRequest::DownloadObjectRequest(const std::string &bucket, const std::string &key,
    const std::wstring &filePath, const std::wstring &checkpointDir) :
    DownloadObjectRequest(bucket, key, filePath, checkpointDir, DefaultPartSize, DefaultResumableThreadNum)
{}

DownloadObjectRequest::DownloadObjectRequest(const std::string &bucket, const std::string &key,
    const std::wstring &filePath) :
    DownloadObjectRequest(bucket, key, filePath, L"", DefaultPartSize, DefaultResumableThreadNum)
{}


void DownloadObjectRequest::setRange(int64_t start, int64_t end)
{
    range_[0] = start;
    range_[1] = end;
    rangeIsSet_ = true;
}

void DownloadObjectRequest::setModifiedSinceConstraint(const std::string &value)
{
    modifiedSince_ = value;
}

void DownloadObjectRequest::setUnmodifiedSinceConstraint(const std::string &value)
{
    unmodifiedSince_ = value;
}

void DownloadObjectRequest::setMatchingETagConstraints(const std::vector<std::string> &values)
{
    matchingETags_ = values;
}

void DownloadObjectRequest::setNonmatchingETagConstraints(const std::vector<std::string> &values)
{
    nonmatchingETags_ = values;
}

void DownloadObjectRequest::addResponseHeaders(RequestResponseHeader header, const std::string &value)
{
    static const char *ResponseHeader[] = {
        "response-content-type", "response-content-language",
        "response-expires", "response-cache-control",
        "response-content-disposition", "response-content-encoding" };
    responseHeaderParameters_[ResponseHeader[header - RequestResponseHeader::ContentType]] = value;
}

int DownloadObjectRequest::validate() const
{
    auto ret = OssResumableBaseRequest::validate();
    if (ret != 0) {
        return ret;
    }

    if (rangeIsSet_ && (range_[0] < 0 || range_[1] < -1 || (range_[1] > -1 && range_[1] < range_[0]))) {
        return ARG_ERROR_INVALID_RANGE;
    }

#if !defined(_WIN32)
    if (!filePathW_.empty()) {
        return ARG_ERROR_PATH_NOT_SUPPORT_WSTRING_TYPE;
    }
#endif

    if (filePath_.empty() && filePathW_.empty()) {
        return ARG_ERROR_DOWNLOAD_FILE_PATH_EMPTY;
    }

    //path and checkpoint must be same type.
    if ((!filePath_.empty() && !checkpointDirW_.empty()) ||
        (!filePathW_.empty() && !checkpointDir_.empty())) {
        return ARG_ERROR_PATH_NOT_SAME_TYPE;
    }

    //check tmpfilePath is available
    auto stream = GetFstreamByPath(tempFilePath_, tempFilePathW_, std::ios::out | std::ios::app);
    if (!stream->is_open()) {
        return ARG_ERROR_OPEN_DOWNLOAD_TEMP_FILE;
    }
    stream->close();

    return 0;
}

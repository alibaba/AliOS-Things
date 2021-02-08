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

#include <string>
#include <algorithm>
#ifdef _WIN32
#include <codecvt>
#endif
#include <alibabacloud/oss/Const.h>
#include "ResumableBaseWorker.h"
#include "../utils/FileSystemUtils.h"
#include "../utils/Utils.h"

using namespace AlibabaCloud::OSS;


ResumableBaseWorker::ResumableBaseWorker(uint64_t objectSize, uint64_t partSize) :
    hasRecord_(false),
    objectSize_(objectSize),
    consumedSize_(0),
    partSize_(partSize)
{
}

int ResumableBaseWorker::validate(OssError& err)
{
    genRecordPath();
    
    if (hasRecordPath()) {
        if (0 != loadRecord()) {
            removeRecordFile();
        }
    }

    if (hasRecord_) {
        if (0 != validateRecord()) {
            removeRecordFile();
            if (0 != prepare(err)) {
                return -1;
            }
        }
    }
    else {
        if (0 != prepare(err)) {
            return -1;
        }
    }
    return 0;
}

void ResumableBaseWorker::determinePartSize()
{
    uint64_t partSize = partSize_;
    uint64_t objectSize = objectSize_;
    uint64_t partCount = (objectSize - 1) / partSize + 1;
    while (partCount > PartNumberUpperLimit) {
        partSize = partSize * 2;
        partCount = (objectSize - 1) / partSize + 1;
    }

    partSize_ = partSize;
}

bool ResumableBaseWorker::hasRecordPath()
{
    return !(recordPath_.empty() && recordPathW_.empty());
}

void ResumableBaseWorker::removeRecordFile()
{
    if (!recordPath_.empty()) {
        RemoveFile(recordPath_);
    }
#ifdef _WIN32
    if (!recordPathW_.empty()) {
        RemoveFile(recordPathW_);
    }
#endif
}

#ifdef _WIN32

std::string ResumableBaseWorker::toString(const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(str);
}

std::wstring ResumableBaseWorker::toWString(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(str);
}

#else

std::string ResumableBaseWorker::toString(const std::wstring& str)
{
    UNUSED_PARAM(str);
    return "";
}

std::wstring ResumableBaseWorker::toWString(const std::string& str)
{
    UNUSED_PARAM(str);
    return L"";
}

#endif


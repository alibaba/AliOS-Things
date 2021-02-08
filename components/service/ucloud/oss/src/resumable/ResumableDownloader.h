#pragma once
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

#pragma once
#include "ResumableBaseWorker.h"

namespace AlibabaCloud
{
namespace OSS
{
    struct PartRecord {
        int32_t partNumber;
        int64_t offset;
        int64_t size;
        uint64_t crc64;
    };
    typedef std::vector<PartRecord> PartRecordList;
    struct DownloadRecord {
        std::string opType;
        std::string bucket;
        std::string key;
        std::string filePath;
        std::string mtime;
        uint64_t size;
        uint64_t partSize;
        PartRecordList parts;
        std::string md5Sum;
        int64_t rangeStart;
        int64_t rangeEnd;
        //crc64
    };


    class ResumableDownloader : public ResumableBaseWorker
    {
    public:
        ResumableDownloader(const DownloadObjectRequest& request, const OssClientImpl *client, uint64_t objectSize)
            : ResumableBaseWorker(objectSize, request.PartSize()), request_(request),client_(client), contentLength_(objectSize)
        {}

        GetObjectOutcome Download();

    protected:
        void genRecordPath();
        int loadRecord();
        int validateRecord();
        int prepare(OssError& err);
        void initRecord();
        int getPartsToDownload(OssError &err, PartRecordList &partsToDownload);
        bool renameTempFile();
        static void DownloadPartProcessCallback(size_t increment, int64_t transfered, int64_t total, void *userData);

        virtual GetObjectOutcome GetObjectWrap(const GetObjectRequest &request) const;

        const DownloadObjectRequest request_;
        DownloadRecord record_;
        const OssClientImpl *client_;
        uint64_t contentLength_;
    };
}
}

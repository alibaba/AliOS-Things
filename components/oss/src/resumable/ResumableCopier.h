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
    struct MultiCopyRecord {
        std::string opType;
        std::string uploadID;
        std::string srcBucket;
        std::string srcKey;
        std::string bucket;
        std::string key;
        std::string mtime;
        uint64_t size;
        uint64_t partSize;
        std::string md5Sum;
    };

    class ResumableCopier : public ResumableBaseWorker
    {
    public:
        ResumableCopier(const MultiCopyObjectRequest &request, const OssClientImpl *client, uint64_t objectSize) 
            :ResumableBaseWorker(objectSize, request.PartSize()), request_(request), client_(client)
        {
        }
	    CopyObjectOutcome Copy();

    protected:
        void genRecordPath();
        int loadRecord();
        int validateRecord();
        int prepare(OssError& err);
        void initRecord(const std::string &uploadID);
        int getPartsToUploadCopy(OssError &err, PartList &partsCopied, PartList &partsToUpload);

        const MultiCopyObjectRequest request_;
        MultiCopyRecord record_;
        const OssClientImpl *client_;
        std::string uploadID_;
    };
}
}

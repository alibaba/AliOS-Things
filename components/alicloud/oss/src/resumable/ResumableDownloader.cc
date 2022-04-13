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

#include <fstream>
#include <algorithm>
#include <set>
#include <alibabacloud/oss/Const.h>
#include "utils/Utils.h"
#include "utils/Crc64.h"
#include "../utils/LogUtils.h"
#include "../utils/FileSystemUtils.h"
#include "../external/json/json.h"
//#include "OssClientImpl.h"
#include "ResumableDownloader.h"
#include "../model/ModelError.h"

using namespace AlibabaCloud::OSS;

GetObjectOutcome ResumableDownloader::Download() 
{
    OssError err;

    if (0 != validate(err)) {
        return GetObjectOutcome(err);
    }

    PartRecordList partsToDownload;
    if (getPartsToDownload(err, partsToDownload) != 0) {
        return GetObjectOutcome(err);
    }

    //task queue
    PartRecordList downloadedParts;
    if (hasRecord_) {
        downloadedParts = record_.parts;
    }
    std::vector<GetObjectOutcome> outcomes;
    std::vector<std::thread> threadPool;

    for (uint32_t i = 0; i < request_.ThreadNum(); i++) {
        threadPool.emplace_back(std::thread([&]() {
            PartRecord part;
            while (true) {
                {
                std::lock_guard<std::mutex> lck(lock_);
                if (partsToDownload.empty())
                    break;
                part = partsToDownload.front();
                partsToDownload.erase(partsToDownload.begin());
                }

                if (!client_->isEnableRequest())
                    break;

                uint64_t pos = partSize_ * (part.partNumber - 1);
                uint64_t start = part.offset;
                uint64_t end = start + part.size - 1;
                auto getObjectReq = GetObjectRequest(request_.Bucket(), request_.Key(), request_.ModifiedSinceConstraint(), request_.UnmodifiedSinceConstraint(),
                    request_.MatchingETagsConstraint(), request_.NonmatchingETagsConstraint(), request_.ResponseHeaderParameters());
                getObjectReq.setResponseStreamFactory([=]() {
                    auto tmpFstream = GetFstreamByPath(request_.TempFilePath(), request_.TempFilePathW(),
                        std::ios_base::in | std::ios_base::out | std::ios_base::binary);
                    tmpFstream->seekp(pos, tmpFstream->beg);
                    return tmpFstream;
                });
                getObjectReq.setRange(start, end);
                getObjectReq.setFlags(getObjectReq.Flags() | REQUEST_FLAG_CHECK_CRC64 | REQUEST_FLAG_SAVE_CLIENT_CRC64);

                auto process = request_.TransferProgress();
                if (process.Handler) {
                    TransferProgress uploadPartProcess = { DownloadPartProcessCallback, (void *)this };
                    getObjectReq.setTransferProgress(uploadPartProcess);
                }
                if (request_.RequestPayer() == RequestPayer::Requester) {
                    getObjectReq.setRequestPayer(request_.RequestPayer());
                }
                if (request_.TrafficLimit() != 0) {
                    getObjectReq.setTrafficLimit(request_.TrafficLimit());
                }
                if (!request_.VersionId().empty()) {
                    getObjectReq.setVersionId(request_.VersionId());
                }
                auto outcome = GetObjectWrap(getObjectReq);
#ifdef ENABLE_OSS_TEST
                if (!!(request_.Flags() & 0x40000000) && part.partNumber == 2) {
                    const char* TAG = "ResumableDownloadObjectClient";
                    OSS_LOG(LogLevel::LogDebug, TAG, "NO.2 part data download failed.");
                    outcome = GetObjectOutcome();
                }
#endif // ENABLE_OSS_TEST

                // lock
                {
                    std::lock_guard<std::mutex> lck(lock_);
                    if (outcome.isSuccess()) {
                        part.crc64 = std::strtoull(outcome.result().Metadata().HttpMetaData().at("x-oss-hash-crc64ecma-by-client").c_str(), nullptr, 10);
                        downloadedParts.push_back(part);
                    }
                    outcomes.push_back(outcome);

                    //update record
                    if (hasRecordPath() && outcome.isSuccess()) {
                        auto &record = record_;
                        record.parts = downloadedParts;

                        Json::Value root;
                        root["opType"] = record.opType;
                        root["bucket"] = record.bucket;
                        root["key"] = record.key;
                        root["filePath"] = record.filePath;
                        root["mtime"] = record.mtime;
                        root["size"] = record.size;
                        root["partSize"] = record.partSize;

                        int index = 0;
                        for (PartRecord& partR : record.parts) {
                            root["parts"][index]["partNumber"] = partR.partNumber;
                            root["parts"][index]["size"] = partR.size;
                            root["parts"][index]["crc64"] = partR.crc64;
                            index++;
                        }

                        std::stringstream ss;
                        ss << root;
                        std::string md5Sum = ComputeContentETag(ss);
                        root["md5Sum"] = md5Sum;

                        if (request_.RangeIsSet()) {
                            root["rangeStart"] = record.rangeStart;
                            root["rangeEnd"] = record.rangeEnd;
                        }

                        auto recordStream = GetFstreamByPath(recordPath_, recordPathW_, std::ios::out);
                        if (recordStream->is_open()) {
                            *recordStream << root;
                            recordStream->close();
                        }
                    }
                }
            }
        }));
    }

    for (auto& worker : threadPool) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    std::shared_ptr<std::iostream> content = nullptr;
    for (auto& outcome : outcomes) {
        if (!outcome.isSuccess()) {
            return GetObjectOutcome(outcome.error());
        }
        outcome.result().setContent(content);
    }

    if (!client_->isEnableRequest()) {
        return GetObjectOutcome(OssError("ClientError:100002", "Disable all requests by upper."));
    }

    // sort
    std::sort(downloadedParts.begin(), downloadedParts.end(), [&](const PartRecord& a, const PartRecord& b)
    {
        return a.partNumber < b.partNumber;
    });

    ObjectMetaData meta;
    if (outcomes.empty()) {
        HeadObjectRequest hRequest(request_.Bucket(), request_.Key());
        if (request_.RequestPayer() == RequestPayer::Requester) {
            hRequest.setRequestPayer(request_.RequestPayer());
        }
        if (!request_.VersionId().empty()) {
            hRequest.setVersionId(request_.VersionId());
        }
        auto hOutcome = client_->HeadObject(hRequest);
        if (!hOutcome.isSuccess()) {
            return GetObjectOutcome(hOutcome.error());
        }
        meta = hOutcome.result();
    }
    else {
        meta = outcomes[0].result().Metadata();
    }
    meta.setContentLength(contentLength_);

    //check crc and update metadata
    if (!request_.RangeIsSet()) {
        if (client_->configuration().enableCrc64) {
            uint64_t localCRC64 = downloadedParts[0].crc64;
            for (size_t i = 1; i < downloadedParts.size(); i++) {
                localCRC64 = CRC64::CombineCRC(localCRC64, downloadedParts[i].crc64, downloadedParts[i].size);
            }
            if (localCRC64 != outcomes[0].result().Metadata().CRC64()) {
                return GetObjectOutcome(OssError("CrcCheckError", "ResumableDownload object CRC checksum fail."));
            }
        }
        meta.HttpMetaData().erase(Http::CONTENT_RANGE);
    }
    else {
        std::stringstream ss;
        ss << "bytes " << request_.RangeStart() << "-";
        if (request_.RangeEnd() != -1) { 
            ss << request_.RangeEnd() << "/" << objectSize_; 
        } 
        else {
            ss << (objectSize_ - 1) << "/" << objectSize_;
        }
        meta.HttpMetaData()["Content-Range"] = ss.str();
    }

    if (meta.HttpMetaData().find("x-oss-hash-crc64ecma-by-client") != meta.HttpMetaData().end()) {
        meta.HttpMetaData().erase("x-oss-hash-crc64ecma-by-client");
    }

    if (!renameTempFile()) {
        std::stringstream ss;
        ss << "rename temp file failed";
        return GetObjectOutcome(OssError("RenameError", ss.str()));
    }

    removeRecordFile();

    GetObjectResult result(request_.Bucket(), request_.Key(), meta);
    return GetObjectOutcome(result);
}

int ResumableDownloader::prepare(OssError& err) 
{
    UNUSED_PARAM(err);

    determinePartSize();
    if (hasRecordPath()) {
        initRecord();

        Json::Value root;
        root["opType"] = record_.opType;
        root["bucket"] = record_.bucket;
        root["key"] = record_.key;
        root["filePath"] = record_.filePath;
        root["mtime"] = record_.mtime;
        root["size"] = record_.size;
        root["partSize"] = record_.partSize;
        root["parts"].resize(0);

        std::stringstream ss;
        ss << root;
        std::string md5Sum = ComputeContentETag(ss);
        root["md5Sum"] = md5Sum;

        if (request_.RangeIsSet()) {
            root["rangeStart"] = record_.rangeStart;
            root["rangeEnd"] = record_.rangeEnd;
        }

        auto recordStream = GetFstreamByPath(recordPath_, recordPathW_, std::ios::out);
        if (recordStream->is_open()) {
            *recordStream << root;
            recordStream->close();
        }
    }
    return 0;
}

int ResumableDownloader::validateRecord() 
{
    auto record = record_;

    if (record.size != objectSize_ || record.mtime != request_.ObjectMtime()) {
        return ARG_ERROR_DOWNLOAD_OBJECT_MODIFIED;
    }

    if (request_.RangeIsSet()) {
        if (record.rangeStart != request_.RangeStart() || record.rangeEnd != request_.RangeEnd()) {
            return ARG_ERROR_RANGE_HAS_BEEN_RESET;
        }
    }

    Json::Value root;
    root["opType"] = record.opType;
    root["bucket"] = record.bucket;
    root["key"] = record.key;
    root["filePath"] = record.filePath;
    root["mtime"] = record.mtime;
    root["size"] = record.size;
    root["partSize"] = record.partSize;
    root["parts"].resize(0);
    int index = 0;
    for (PartRecord& part : record.parts) {
        root["parts"][index]["partNumber"] = part.partNumber;
        root["parts"][index]["size"] = part.size;
        root["parts"][index]["crc64"] = part.crc64;
        index++;
    }

    if (!(record.rangeStart == 0 && record.rangeEnd == -1)) {
        root["rangeStart"] = record.rangeStart;
        root["rangeEnd"] = record.rangeEnd;
    }
	
    std::stringstream recordStream;
    recordStream << root;

    std::string md5Sum = ComputeContentETag(recordStream);
    if (md5Sum != record.md5Sum) {
        return -1;
    }
    return 0;
}

int ResumableDownloader::loadRecord() 
{
    auto recordStream = GetFstreamByPath(recordPath_, recordPathW_, std::ios::in);
    if (recordStream->is_open()) {
        Json::Value root;
        Json::CharReaderBuilder rbuilder;
        std::string errMsg;
        if (!Json::parseFromStream(rbuilder, *recordStream, &root, &errMsg))
        {
            return ARG_ERROR_PARSE_DOWNLOAD_RECORD_FILE;
        }

        record_.opType = root["opType"].asString();
        record_.bucket = root["bucket"].asString();
        record_.key = root["key"].asString();
        record_.filePath = root["filePath"].asString();
        record_.mtime = root["mtime"].asString();
        record_.size = root["size"].asUInt64();
        record_.partSize = root["partSize"].asUInt64();
		
        PartRecord part;
        for (uint32_t i = 0; i < root["parts"].size(); i++) {
            Json::Value partValue = root["parts"][i];
            part.partNumber = partValue["partNumber"].asInt();
            part.size = partValue["size"].asInt64();
            part.crc64 = partValue["crc64"].asUInt64();
            record_.parts.push_back(part);
        }
        record_.md5Sum = root["md5Sum"].asString();
		
        if (root["rangeStart"] != Json::nullValue && root["rangeEnd"] != Json::nullValue) {
            record_.rangeStart = root["rangeStart"].asInt64();
            record_.rangeEnd = root["rangeEnd"].asInt64();
        }
        else if(root["rangeStart"] == Json::nullValue && root["rangeEnd"] == Json::nullValue){
            record_.rangeStart = 0;
            record_.rangeEnd = -1;
        }else {
            return ARG_ERROR_INVALID_RANGE_IN_DWONLOAD_RECORD;
        }

        partSize_ = record_.partSize;
        hasRecord_ = true;
        recordStream->close();
    }

    return 0;
}

void ResumableDownloader::genRecordPath() 
{
    recordPath_ = "";
    recordPathW_ = L"";

    if (!request_.hasCheckpointDir())
        return;

    std::stringstream ss;
    ss << "oss://" << request_.Bucket() << "/" << request_.Key();
    if (!request_.VersionId().empty()) {
        ss << "?versionId=" << request_.VersionId();
    }
    auto srcPath = ss.str();
    auto destPath = !request_.FilePathW().empty() ? toString(request_.FilePathW()) : request_.FilePath();
    auto safeFileName = ComputeContentETag(srcPath) + "--" + ComputeContentETag(destPath);

    if (!request_.CheckpointDirW().empty()) {
        recordPathW_ = request_.CheckpointDirW() + WPATH_DELIMITER + toWString(safeFileName);;
    }
    else {
        recordPath_ = request_.CheckpointDir() + PATH_DELIMITER + safeFileName;
    }
}

int ResumableDownloader::getPartsToDownload(OssError &err, PartRecordList &partsToDownload) 
{
    UNUSED_PARAM(err);

    std::set<uint64_t> partNumbersDownloaded;
    if (hasRecord_) {
        for (PartRecord &part : record_.parts) {
            partNumbersDownloaded.insert(part.partNumber);
            consumedSize_ += part.size;
        }
    }

    int64_t start = 0;
    int64_t end = objectSize_ - 1;

    if (request_.RangeIsSet()) {
        start = request_.RangeStart();
        end = request_.RangeEnd();
        if (end == -1) {
            end = objectSize_ - 1;
        }
        contentLength_ = end - start + 1;
    }

    int32_t index = 1;
    for (int64_t offset = start; offset < end + 1; offset += partSize_, index++) {
        PartRecord part;
        part.partNumber = index;
        part.offset = offset;
        if (offset + (int64_t)partSize_ > end) {
            part.size = end - offset + 1;
        }
        else {
            part.size = partSize_;
        }

        auto iterNum = partNumbersDownloaded.find(part.partNumber);
        if (iterNum == partNumbersDownloaded.end()) {
            partsToDownload.push_back(part);
        }
    }
    return 0;
}

void ResumableDownloader::initRecord()
{
    auto filePath = request_.FilePath();
    if (!request_.FilePathW().empty()) {
        filePath = toString(request_.FilePathW());
    }

    record_.opType = "ResumableDownload";
    record_.bucket = request_.Bucket();
    record_.key = request_.Key();
    record_.filePath = filePath;
    record_.mtime = request_.ObjectMtime();
    record_.size = objectSize_;
    record_.partSize = partSize_;

    //in this place we shuold consider the condition that range is not set
    if (request_.RangeIsSet()) {
        record_.rangeStart = request_.RangeStart();
        record_.rangeEnd = request_.RangeEnd();
    }
    else {
        record_.rangeStart = 0;
        record_.rangeEnd = -1;
    }
}

void ResumableDownloader::DownloadPartProcessCallback(size_t increment, int64_t transfered, int64_t total, void *userData) 
{
    UNUSED_PARAM(transfered);
    UNUSED_PARAM(total);

    auto downloader = (ResumableDownloader*)userData;
    std::lock_guard<std::mutex> lck(downloader->lock_);
    downloader->consumedSize_ += increment;

    auto process = downloader->request_.TransferProgress();
    if (process.Handler) {
        process.Handler(increment, downloader->consumedSize_, downloader->contentLength_, process.UserData);
    }
}

bool ResumableDownloader::renameTempFile()
{
#ifdef _WIN32
    if (!request_.TempFilePathW().empty()) {
        return RenameFile(request_.TempFilePathW(), request_.FilePathW());
    }
    else
#endif
    {
        return RenameFile(request_.TempFilePath(), request_.FilePath());
    }
}

GetObjectOutcome ResumableDownloader::GetObjectWrap(const GetObjectRequest &request) const
{
    return client_->GetObject(request);
}


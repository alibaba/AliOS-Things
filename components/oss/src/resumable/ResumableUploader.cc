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
#include <alibabacloud/oss/model/InitiateMultipartUploadRequest.h>
#include <alibabacloud/oss/model/CompleteMultipartUploadRequest.h>
#include <alibabacloud/oss/OssFwd.h>
#include <alibabacloud/oss/Const.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <set>
#include "../external/json/json.h"
#include "../utils/FileSystemUtils.h"
#include "utils/Utils.h"
#include "../utils/LogUtils.h"
#include "utils/Crc64.h"
#include "../OssClientImpl.h"
#include "../model/ModelError.h"
#include "ResumableUploader.h"

using namespace AlibabaCloud::OSS;


ResumableUploader::ResumableUploader(const UploadObjectRequest& request, const OssClientImpl *client) :
    ResumableBaseWorker(request.ObjectSize(), request.PartSize()),
    request_(request),
    client_(client)
{
    if (!request.FilePath().empty()) {
        time_t lastMtime;
        std::streamsize fileSize;
        if (GetPathInfo(request.FilePath(), lastMtime, fileSize)) {
            objectSize_ = static_cast<uint64_t>(fileSize);
        }
    }
#ifdef _WIN32
    else if (!request.FilePathW().empty()) {
        time_t lastMtime;
        std::streamsize fileSize;
        if (GetPathInfo(request.FilePathW(), lastMtime, fileSize)) {
            objectSize_ = static_cast<uint64_t>(fileSize);
        }
    }
#endif
}

PutObjectOutcome ResumableUploader::Upload()
{
    OssError err;

    if (0 != validate(err)) {
        return PutObjectOutcome(err);
    }

    PartList partsToUpload;
    PartList uploadedParts;
    if (getPartsToUpload(err, uploadedParts, partsToUpload) != 0){
        return PutObjectOutcome(err);
    }

    std::vector<PutObjectOutcome> outcomes;
    std::vector<std::thread> threadPool;

    for (uint32_t i = 0; i < request_.ThreadNum(); i++) {
        threadPool.emplace_back(std::thread([&]() {
            Part part;
            while (true) {
                {
                std::lock_guard<std::mutex> lck(lock_);
                if (partsToUpload.empty())
                    break;
                part = partsToUpload.front();
                partsToUpload.erase(partsToUpload.begin());
                }

                if (!client_->isEnableRequest())
                    break;

                uint64_t offset = partSize_ * (part.PartNumber() - 1);
                uint64_t length = part.Size();

                auto content = GetFstreamByPath(request_.FilePath(), request_.FilePathW(),
                    std::ios::in | std::ios::binary);
                content->seekg(offset, content->beg);

                UploadPartRequest uploadPartRequest(request_.Bucket(), request_.Key(), part.PartNumber(), uploadID_, content);
                uploadPartRequest.setContentLength(length);

                auto process = request_.TransferProgress();
                if (process.Handler) {
                    TransferProgress uploadPartProcess = { UploadPartProcessCallback, (void *)this };
                    uploadPartRequest.setTransferProgress(uploadPartProcess);
                }
                if (request_.RequestPayer() == RequestPayer::Requester) {
                    uploadPartRequest.setRequestPayer(request_.RequestPayer());
                }
                if (request_.TrafficLimit() != 0) {
                    uploadPartRequest.setTrafficLimit(request_.TrafficLimit());
                }
                auto outcome = UploadPartWrap(uploadPartRequest);
#ifdef ENABLE_OSS_TEST
                if (!!(request_.Flags() & 0x40000000) && part.PartNumber() == 2) {
                    const char* TAG = "ResumableUploadObjectClient";
                    OSS_LOG(LogLevel::LogDebug, TAG, "NO.2 part data upload failed.");
                    outcome = PutObjectOutcome();
                }
#endif // ENABLE_OSS_TEST

                if (outcome.isSuccess()) {
                    part.eTag_  = outcome.result().ETag();
                    part.cRC64_ = outcome.result().CRC64();
                }

                //lock
                {
                std::lock_guard<std::mutex> lck(lock_);
                uploadedParts.push_back(part);
                outcomes.push_back(outcome);
                }
            }
        }));
    }

    for (auto& worker:threadPool) {
        if(worker.joinable()){
            worker.join();
        }
    }

    if (!client_->isEnableRequest()) {
        return PutObjectOutcome(OssError("ClientError:100002", "Disable all requests by upper."));
    }

    for (const auto& outcome : outcomes) {
        if (!outcome.isSuccess()) {
            return PutObjectOutcome(outcome.error());
        }
    }

    // sort uploadedParts
    std::sort(uploadedParts.begin(), uploadedParts.end(), [&](const Part& a, const Part& b)
    {
        return a.PartNumber() < b.PartNumber();
    });

    CompleteMultipartUploadRequest completeMultipartUploadReq(request_.Bucket(), request_.Key(), uploadedParts, uploadID_);
    if (request_.MetaData().hasHeader("x-oss-object-acl")) {
        completeMultipartUploadReq.MetaData().HttpMetaData()["x-oss-object-acl"] =
            request_.MetaData().HttpMetaData().at("x-oss-object-acl");
    }
    if (!request_.EncodingType().empty()) {
        completeMultipartUploadReq.setEncodingType(request_.EncodingType());
    }
    if (request_.MetaData().hasHeader("x-oss-callback")) {
        completeMultipartUploadReq.MetaData().HttpMetaData()["x-oss-callback"] =
            request_.MetaData().HttpMetaData().at("x-oss-callback");
        if (request_.MetaData().hasHeader("x-oss-callback-var")) {
            completeMultipartUploadReq.MetaData().HttpMetaData()["x-oss-callback-var"] =
                request_.MetaData().HttpMetaData().at("x-oss-callback-var");
        }
        if (request_.MetaData().hasHeader("x-oss-pub-key-url")) {
            completeMultipartUploadReq.MetaData().HttpMetaData()["x-oss-pub-key-url"] =
                request_.MetaData().HttpMetaData().at("x-oss-pub-key-url");
        }
    }
    if (request_.RequestPayer() == RequestPayer::Requester) {
        completeMultipartUploadReq.setRequestPayer(request_.RequestPayer());
    }
    auto outcome = CompleteMultipartUploadWrap(completeMultipartUploadReq);
    if (!outcome.isSuccess()) {
        return PutObjectOutcome(outcome.error());
    }

    // crc
    uint64_t localCRC64 = uploadedParts[0].CRC64();
    for (size_t i = 1; i < uploadedParts.size(); i++) {
        localCRC64 = CRC64::CombineCRC(localCRC64, uploadedParts[i].CRC64(), uploadedParts[i].Size());
    }

    uint64_t ossCRC64 = outcome.result().CRC64();
    if (ossCRC64 != 0 && localCRC64 != ossCRC64) {
        return PutObjectOutcome(OssError("CrcCheckError", "ResumableUpload Object CRC Checksum fail."));
    }

    removeRecordFile();

    HeaderCollection headers;
    headers[Http::ETAG] = outcome.result().ETag();
    headers["x-oss-hash-crc64ecma"] = std::to_string(outcome.result().CRC64());
    headers["x-oss-request-id"] = outcome.result().RequestId();
    if (!outcome.result().VersionId().empty()) {
        headers["x-oss-version-id"] = outcome.result().VersionId();
    }
    return PutObjectOutcome(PutObjectResult(headers, outcome.result().Content()));
}

int ResumableUploader::prepare(OssError& err)
{
    determinePartSize();
    auto initMultipartUploadReq = InitiateMultipartUploadRequest(request_.Bucket(), request_.Key(), request_.MetaData());
    if (!request_.EncodingType().empty()) {
        initMultipartUploadReq.setEncodingType(request_.EncodingType());
    }
    if (request_.RequestPayer() == RequestPayer::Requester) {
        initMultipartUploadReq.setRequestPayer(request_.RequestPayer());
    }
    auto outcome = InitiateMultipartUploadWrap(initMultipartUploadReq);
    if(!outcome.isSuccess()){
        err = outcome.error();
        return -1;
    }

    //init record_
    uploadID_ = outcome.result().UploadId();

    if (hasRecordPath()) {
        Json::Value root;

        initRecordInfo();
        dumpRecordInfo(root);

        std::stringstream ss;
        ss << root;
        std::string md5Sum = ComputeContentETag(ss);
        root["md5Sum"] = md5Sum;

        auto recordStream = GetFstreamByPath(recordPath_, recordPathW_, std::ios::out);
        if (recordStream->is_open()) {
            *recordStream << root;
            recordStream->close();
        }
    }
    return 0;
}

int ResumableUploader::validateRecord()
{
    if (record_.size != objectSize_ || record_.mtime != request_.ObjectMtime()){
        return ARG_ERROR_UPLOAD_FILE_MODIFIED;
    }

    Json::Value root;

    dumpRecordInfo(root);

    std::stringstream recordStream;
    recordStream << root;

    std::string md5Sum = ComputeContentETag(recordStream);
    if (md5Sum != record_.md5Sum){
        return ARG_ERROR_UPLOAD_RECORD_INVALID;
    }
    return 0;
}

int ResumableUploader::loadRecord()
{
    auto recordStream = GetFstreamByPath(recordPath_, recordPathW_, std::ios::in);
    if (recordStream->is_open()){
        Json::Value root;
        Json::CharReaderBuilder rbuilder;
        std::string errMsg;
        if (!Json::parseFromStream(rbuilder, *recordStream, &root, &errMsg))
        {
            return ARG_ERROR_PARSE_UPLOAD_RECORD_FILE;
        }

        buildRecordInfo(root);

        partSize_ = record_.partSize;
        uploadID_ = record_.uploadID;
        hasRecord_ = true;

        recordStream->close();
    }

    return 0;
}

void ResumableUploader::genRecordPath()
{
    recordPath_  = "";
    recordPathW_ = L"";

    if (!request_.hasCheckpointDir())
        return;

    auto srcPath = !request_.FilePathW().empty()? toString(request_.FilePathW()): request_.FilePath();
    std::stringstream ss;
    ss << "oss://" << request_.Bucket() << "/" << request_.Key();
    auto destPath = ss.str();

    auto safeFileName = ComputeContentETag(srcPath) + "--" + ComputeContentETag(destPath);
  
    if (!request_.CheckpointDirW().empty()) {
        recordPathW_ = request_.CheckpointDirW() + WPATH_DELIMITER + toWString(safeFileName);;
    }
    else {
        recordPath_ = request_.CheckpointDir() + PATH_DELIMITER + safeFileName;
    }
}

int ResumableUploader::getPartsToUpload(OssError &err, PartList &partsUploaded, PartList &partsToUpload)
{
    std::set<uint64_t> partNumbersUploaded;
    
    if(hasRecord_){ 
        uint32_t marker = 0;
        auto listPartsRequest = ListPartsRequest(request_.Bucket(), request_.Key(), uploadID_);
        if (!request_.EncodingType().empty()) {
            listPartsRequest.setEncodingType(request_.EncodingType());
        }
        if (request_.RequestPayer() == RequestPayer::Requester) {
            listPartsRequest.setRequestPayer(request_.RequestPayer());
        }
        while(true){
            listPartsRequest.setPartNumberMarker(marker);
            auto outcome = ListPartsWrap(listPartsRequest);
            if(!outcome.isSuccess()){
                err = outcome.error();
                return -1;
            }

            auto parts = outcome.result().PartList();
            for(auto iter = parts.begin(); iter != parts.end(); iter++){
                partNumbersUploaded.insert(iter->PartNumber());
                partsUploaded.emplace_back(*iter);
                consumedSize_ += iter->Size();
            }

            if(outcome.result().IsTruncated()){
                marker = outcome.result().NextPartNumberMarker();
            }else{
                break;
            }
        }
    }

    int32_t partCount = (int32_t)((objectSize_ - 1)/ partSize_ + 1);
    for(int32_t i = 0; i < partCount; i++){
        Part part;
        part.partNumber_ = i+1;
        if (i == partCount -1 ){
            part.size_ = objectSize_ - partSize_ * (partCount - 1);
        }else{
            part.size_ = partSize_;
        }

        auto iterNum = partNumbersUploaded.find(part.PartNumber());
        if (iterNum == partNumbersUploaded.end()){
            partsToUpload.push_back(part);
        }
    }

    return 0;
}

void ResumableUploader::initRecordInfo()
{
    record_.opType = "ResumableUpload";
    record_.uploadID = uploadID_;
    record_.filePath = request_.FilePath();
    record_.bucket = request_.Bucket();
    record_.key = request_.Key();
    record_.mtime = request_.ObjectMtime();
    record_.size = objectSize_;
    record_.partSize = partSize_;
}

void ResumableUploader::buildRecordInfo(const AlibabaCloud::OSS::Json::Value& root)
{
    record_.opType = root["opType"].asString();
    record_.uploadID = root["uploadID"].asString();
    record_.filePath = root["filePath"].asString();
    record_.bucket = root["bucket"].asString();
    record_.key = root["key"].asString();
    record_.size = root["size"].asUInt64();
    record_.mtime = root["mtime"].asString();
    record_.partSize = root["partSize"].asUInt64();
    record_.md5Sum = root["md5Sum"].asString();
}

void ResumableUploader::dumpRecordInfo(AlibabaCloud::OSS::Json::Value& root)
{
    root["opType"] = record_.opType;
    root["uploadID"] = record_.uploadID;
    root["filePath"] = record_.filePath;
    root["bucket"] = record_.bucket;
    root["key"] = record_.key;
    root["mtime"] = record_.mtime;
    root["size"] = record_.size;
    root["partSize"] = record_.partSize;
}

void ResumableUploader::UploadPartProcessCallback(size_t increment, int64_t transfered, int64_t total, void *userData) 
{
    UNUSED_PARAM(transfered);
    UNUSED_PARAM(total);

    auto uploader = (ResumableUploader*)userData;
    std::lock_guard<std::mutex> lck(uploader->lock_);
    uploader->consumedSize_ += increment;

    auto process = uploader->request_.TransferProgress();
    if (process.Handler) {
        process.Handler(increment, uploader->consumedSize_, uploader->objectSize_, process.UserData);
    }
}

InitiateMultipartUploadOutcome ResumableUploader::InitiateMultipartUploadWrap(const InitiateMultipartUploadRequest &request) const
{
    return client_->InitiateMultipartUpload(request);
}

PutObjectOutcome ResumableUploader::UploadPartWrap(const UploadPartRequest &request) const
{
    return client_->UploadPart(request);
}

ListPartsOutcome ResumableUploader::ListPartsWrap(const ListPartsRequest &request) const
{
    return client_->ListParts(request);
}

CompleteMultipartUploadOutcome ResumableUploader::CompleteMultipartUploadWrap(const CompleteMultipartUploadRequest &request) const
{
    return client_->CompleteMultipartUpload(request);
}



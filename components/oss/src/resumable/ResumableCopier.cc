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
#include <alibabacloud/oss/model/UploadPartCopyRequest.h>
#include <alibabacloud/oss/model/InitiateMultipartUploadRequest.h>
#include <alibabacloud/oss/model/CompleteMultipartUploadRequest.h>
#include <alibabacloud/oss/Const.h>
#include "utils/Utils.h"
#include "../utils/LogUtils.h"
#include "../utils/FileSystemUtils.h"
#include "../external/json/json.h"
//#include "OssClientImpl.h"
#include "../model/ModelError.h"
#include "ResumableCopier.h"

using namespace AlibabaCloud::OSS;

CopyObjectOutcome ResumableCopier::Copy() 
{
    OssError err;

    if (0 != validate(err)) {
        return CopyObjectOutcome(err);
    }

    PartList partsToUploadCopy;
    PartList partsCopied;
    if (getPartsToUploadCopy(err, partsCopied, partsToUploadCopy) != 0) {
        return CopyObjectOutcome(err);
    }

    std::vector<UploadPartCopyOutcome> outcomes;
    std::vector<std::thread> threadPool;

    for (uint32_t i = 0; i < request_.ThreadNum(); i++) {
        threadPool.emplace_back(std::thread([&]() {
            Part part;
            while (true) {
                {
                std::lock_guard<std::mutex> lck(lock_);
                if (partsToUploadCopy.empty())
                    break;
                part = partsToUploadCopy.front();
                partsToUploadCopy.erase(partsToUploadCopy.begin());
                }

                if (!client_->isEnableRequest())
                    break;

                uint64_t offset = partSize_ * (part.PartNumber() - 1);
                uint64_t length = part.Size();

                auto uploadPartCopyReq = UploadPartCopyRequest(request_.Bucket(), request_.Key(), request_.SrcBucket(), request_.SrcKey(),
                    uploadID_, part.PartNumber(), 
                    request_.SourceIfMatchEtag(), request_.SourceIfNotMatchEtag(),
                    request_.SourceIfModifiedSince(), request_.SourceIfUnModifiedSince());
                uploadPartCopyReq.setCopySourceRange(offset, offset + length - 1);
                if (request_.RequestPayer() == RequestPayer::Requester) {
                    uploadPartCopyReq.setRequestPayer(request_.RequestPayer());
                }
                if (request_.TrafficLimit() != 0) {
                    uploadPartCopyReq.setTrafficLimit(request_.TrafficLimit());
                }
                if (!request_.VersionId().empty()) {
                    uploadPartCopyReq.setVersionId(request_.VersionId());
                }
                auto outcome = client_->UploadPartCopy(uploadPartCopyReq);
#ifdef ENABLE_OSS_TEST
                if (!!(request_.Flags() & 0x40000000) && (part.PartNumber() == 2 || part.PartNumber() == 4)) {
                    const char* TAG = "ResumableCopyObjectClient";
                    OSS_LOG(LogLevel::LogDebug, TAG, "NO.%d part data copy failed!", part.PartNumber());
                    outcome = UploadPartCopyOutcome();
                }
#endif // ENABLE_OSS_TEST

                //lock
                {
                    std::lock_guard<std::mutex> lck(lock_);
                    if (outcome.isSuccess()) {
                        part.eTag_ = outcome.result().ETag();
                        partsCopied.push_back(part);
                    }
                    outcomes.push_back(outcome);
                    if (outcome.isSuccess()) {
                        auto process = request_.TransferProgress();

                        if (process.Handler) {
                            consumedSize_ += length;
                            process.Handler((size_t)length, consumedSize_, objectSize_, process.UserData);
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

    for (const auto& outcome : outcomes) {
        if (!outcome.isSuccess()) {
            return CopyObjectOutcome(outcome.error());
        }
    }

    if (!client_->isEnableRequest()) {
        return CopyObjectOutcome(OssError("ClientError:100002", "Disable all requests by upper."));
    }

    // sort partsCopied
    std::sort(partsCopied.begin(), partsCopied.end(), [](const Part& a, const Part& b) 
    {
        return a.PartNumber() < b.PartNumber();
    });

    CompleteMultipartUploadRequest completeMultipartUploadReq(request_.Bucket(), request_.Key(), partsCopied, uploadID_);
    if (request_.MetaData().HttpMetaData().find("x-oss-object-acl")
        != request_.MetaData().HttpMetaData().end()) {
        std::string aclName = request_.MetaData().HttpMetaData().at("x-oss-object-acl");
        completeMultipartUploadReq.setAcl(ToAclType(aclName.c_str()));
    }
    if (!request_.EncodingType().empty()) {
        completeMultipartUploadReq.setEncodingType(request_.EncodingType());
    }
    if (request_.RequestPayer() == RequestPayer::Requester) {
        completeMultipartUploadReq.setRequestPayer(request_.RequestPayer());
    }

    auto compOutcome = client_->CompleteMultipartUpload(completeMultipartUploadReq);
    if (!compOutcome.isSuccess()) {
        return CopyObjectOutcome(compOutcome.error());
    }

    removeRecordFile();

    CopyObjectResult result;
    HeadObjectRequest hRequest(request_.Bucket(), request_.Key());
    if (request_.RequestPayer() == RequestPayer::Requester) {
        hRequest.setRequestPayer(request_.RequestPayer());
    }
    if (!compOutcome.result().VersionId().empty()) {
        hRequest.setVersionId(compOutcome.result().VersionId());
    }
    auto hOutcome = client_->HeadObject(HeadObjectRequest(hRequest));
    if (hOutcome.isSuccess()) {
        result.setLastModified(hOutcome.result().LastModified());
    }
    result.setEtag(compOutcome.result().ETag());
    result.setRequestId(compOutcome.result().RequestId());
    result.setVersionId(compOutcome.result().VersionId());
    return CopyObjectOutcome(result);
}

int ResumableCopier::prepare(OssError& err) 
{
    determinePartSize();
    ObjectMetaData metaData(request_.MetaData());
    if (metaData.HttpMetaData().find("x-oss-metadata-directive") == metaData.HttpMetaData().end() ||
        (metaData.HttpMetaData().find("x-oss-metadata-directive") != metaData.HttpMetaData().end() && 
            metaData.HttpMetaData().at("x-oss-metadata-directive") == "COPY")) {
        HeadObjectRequest hRequest(request_.SrcBucket(), request_.SrcKey());
        if (request_.RequestPayer() == RequestPayer::Requester) {
            hRequest.setRequestPayer(request_.RequestPayer());
        }
        if (!request_.VersionId().empty()) {
            hRequest.setVersionId(request_.VersionId());
        }
        auto headObjectOutcome = client_->HeadObject(hRequest);
        if (!headObjectOutcome.isSuccess()) {
            err = headObjectOutcome.error();
            return -1;
        }
        auto& meta = metaData.UserMetaData();
        meta = headObjectOutcome.result().UserMetaData();
    }

    auto initMultipartUploadReq = InitiateMultipartUploadRequest(request_.Bucket(), request_.Key(), metaData);
    if (!request_.EncodingType().empty()) {
        initMultipartUploadReq.setEncodingType(request_.EncodingType());
    }
    if (request_.RequestPayer() == RequestPayer::Requester) {
        initMultipartUploadReq.setRequestPayer(request_.RequestPayer());
    }
    auto outcome = client_->InitiateMultipartUpload(initMultipartUploadReq);
    if (!outcome.isSuccess()) {
        err = outcome.error();
        return -1;
    }

    //init record_
    uploadID_ = outcome.result().UploadId();

    if (hasRecordPath()) {
        initRecord(uploadID_);

        Json::Value root;
        root["opType"] = record_.opType;
        root["uploadID"] = record_.uploadID;
        root["srcBucket"] = record_.srcBucket;
        root["srcKey"] = record_.srcKey;
        root["bucket"] = record_.bucket;
        root["key"] = record_.key;
        root["mtime"] = record_.mtime;
        root["size"] = record_.size;
        root["partSize"] = record_.partSize;

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

int ResumableCopier::validateRecord() 
{
    auto record = record_;

    if (record.size != objectSize_ || record.mtime != request_.ObjectMtime()) {
        return ARG_ERROR_COPY_SRC_OBJECT_MODIFIED;
    }

    Json::Value root;
    root["opType"] = record.opType;
    root["uploadID"] = record.uploadID;
    root["srcBucket"] = record.srcBucket;
    root["srcKey"] = record.srcKey;
    root["bucket"] = record.bucket;
    root["key"] = record.key;
    root["mtime"] = record.mtime;
    root["size"] = record.size;
    root["partSize"] = record.partSize;

    std::stringstream recordStream;
    recordStream << root;
    
    std::string md5Sum = ComputeContentETag(recordStream);
    if (md5Sum != record.md5Sum) {
        return ARG_ERROR_COPY_RECORD_INVALID;
    }
    return 0;
}

int ResumableCopier::loadRecord() 
{
    if (hasRecordPath()) {
        auto recordStream = GetFstreamByPath(recordPath_, recordPathW_, std::ios::in);
        if (recordStream->is_open()) {
            Json::Value root;
            Json::CharReaderBuilder rbuilder;
            std::string errMsg;
            if (!Json::parseFromStream(rbuilder, *recordStream, &root, &errMsg))
            {
                return ARG_ERROR_PARSE_COPY_RECORD_FILE;
            }

            record_.opType = root["opType"].asString();
            record_.uploadID = root["uploadID"].asString();
            record_.srcBucket = root["srcBucket"].asString();
            record_.srcKey = root["srcKey"].asString();
            record_.bucket = root["bucket"].asString();
            record_.key = root["key"].asString();
            record_.size = root["size"].asUInt64();
            record_.mtime = root["mtime"].asString();
            record_.partSize = root["partSize"].asUInt64();
            record_.md5Sum = root["md5Sum"].asString();

            partSize_ = record_.partSize;
            uploadID_ = record_.uploadID;
            hasRecord_ = true;

            recordStream->close();
        }
    }

    return 0;
}

void ResumableCopier::genRecordPath() 
{
    recordPath_ = "";
    recordPathW_ = L"";

    if (!request_.hasCheckpointDir()) {
        return;
    }

    std::stringstream ss;
    ss << "oss://" << request_.SrcBucket() << "/" << request_.SrcKey();
    if (!request_.VersionId().empty()) {
        ss << "?versionId=" << request_.VersionId();
    }
    auto srcPath = ss.str();
    ss.str("");
    ss << "oss://" << request_.Bucket() << "/" << request_.Key();
    auto destPath = ss.str();

    auto safeFileName = ComputeContentETag(srcPath) + "--" + ComputeContentETag(destPath);

    if (!request_.CheckpointDirW().empty()) {
        recordPathW_ = request_.CheckpointDirW() + WPATH_DELIMITER + toWString(safeFileName);
    }
    else {
        recordPath_ = request_.CheckpointDir() + PATH_DELIMITER + safeFileName;
    }
}

int ResumableCopier::getPartsToUploadCopy(OssError &err, PartList &partsCopied, PartList &partsToUploadCopy) 
{
    std::set<uint64_t> partNumbersUploaded;

    if (hasRecord_) {
        uint32_t marker = 0;
        auto listPartsRequest = ListPartsRequest(request_.Bucket(), request_.Key(), uploadID_);
        if (!request_.EncodingType().empty()) {
            listPartsRequest.setEncodingType(request_.EncodingType());
        }
        if (request_.RequestPayer() == RequestPayer::Requester) {
            listPartsRequest.setRequestPayer(request_.RequestPayer());
        }
        while (true) {
            listPartsRequest.setPartNumberMarker(marker);
            auto outcome = client_->ListParts(listPartsRequest);
            if (!outcome.isSuccess()) {
                err = outcome.error();
                return -1;
            }

            auto parts = outcome.result().PartList();
            for (auto iter = parts.begin(); iter != parts.end(); iter++) {
                partNumbersUploaded.insert(iter->PartNumber());
                partsCopied.emplace_back(*iter);
                consumedSize_ += iter->Size();
            }

            if (outcome.result().IsTruncated()) {
                marker = outcome.result().NextPartNumberMarker();
            }
            else {
                break;
            }
        }
    }

    int32_t partCount = (int32_t)((objectSize_ - 1) / partSize_ + 1);
    for (int32_t i = 0; i < partCount; i++) {
        Part part;
        part.partNumber_ = i + 1;
        if (i == partCount - 1) {
            part.size_ = objectSize_ - partSize_ * (partCount - 1);
        }
        else {
            part.size_ = partSize_;
        }

        auto iterNum = partNumbersUploaded.find(part.PartNumber());
        if (iterNum == partNumbersUploaded.end()) {
            partsToUploadCopy.push_back(part);
        }
    }

    return 0;
}

void ResumableCopier::initRecord(const std::string &uploadID) 
{
    record_.opType = "ResumableCopy";
    record_.uploadID = uploadID;
    record_.srcBucket = request_.SrcBucket();
    record_.srcKey = request_.SrcKey();
    record_.bucket = request_.Bucket();
    record_.key = request_.Key();
    record_.mtime = request_.ObjectMtime();
    record_.size = objectSize_;
    record_.partSize = partSize_;
}

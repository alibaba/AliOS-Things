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
#include <alibabacloud/oss/OssEncryptionClient.h>
#include "../auth/SimpleCredentialsProvider.h"
#include "../encryption/CryptoModule.h"
#include "utils/Utils.h"
#include "../utils/FileSystemUtils.h"
#include "../resumable/ResumableDownloader.h"
#include "../resumable/ResumableUploader.h"

using namespace AlibabaCloud::OSS;

///////////////////////////////////////////////////////////////////////////////////////////////////////
namespace AlibabaCloud
{
namespace OSS
{
#if !defined(DISABLE_RESUAMABLE)
static GetObjectRequest ConvertToGetObjectRequest(const DownloadObjectRequest& request)
{
    auto gRequest = GetObjectRequest(request.Bucket(), request.Key(), request.ModifiedSinceConstraint(), request.UnmodifiedSinceConstraint(), request.MatchingETagsConstraint(), request.NonmatchingETagsConstraint(), request.ResponseHeaderParameters());
    if (request.RangeIsSet()) {
        gRequest.setRange(request.RangeStart(), request.RangeEnd());
    }
    if (request.TransferProgress().Handler) {
        gRequest.setTransferProgress(request.TransferProgress());
    }
    if (request.RequestPayer() == RequestPayer::Requester) {
        gRequest.setRequestPayer(request.RequestPayer());
    }
    if (request.TrafficLimit() != 0) {
        gRequest.setTrafficLimit(request.TrafficLimit());
    }
    if (!request.VersionId().empty()) {
        gRequest.setVersionId(request.VersionId());
    }
    gRequest.setResponseStreamFactory([&]() {return std::make_shared<std::fstream>(request.FilePath(), std::ios_base::out | std::ios_base::in | std::ios_base::trunc | std::ios_base::binary); });
    return gRequest;
}

static PutObjectRequest ConvertToPutObjectRequest(const UploadObjectRequest& request)
{
    std::shared_ptr<std::iostream> content = std::make_shared<std::fstream>(request.FilePath(), std::ios::in | std::ios::binary);
    PutObjectRequest pRequest(request.Bucket(), request.Key(), content, request.MetaData());
    if (request.TransferProgress().Handler) {
        pRequest.setTransferProgress(request.TransferProgress());
    }
    if (request.RequestPayer() == RequestPayer::Requester) {
        pRequest.setRequestPayer(request.RequestPayer());
    }
    if (request.TrafficLimit() != 0) {
        pRequest.setTrafficLimit(request.TrafficLimit());
    }
    return pRequest;
}

class EncryptionResumableDownloader : public ResumableDownloader
{
public:
    EncryptionResumableDownloader(const DownloadObjectRequest& request, const  ObjectMetaData& meta,
        const OssEncryptionClient* enClient, const OssClientImpl *client, uint64_t objectSize)
        : ResumableDownloader(request, client, objectSize)
        , encryptionClient_(enClient)
        , metaData_(meta)
    {
        //16 bytes alignment
        partSize_ = (partSize_ >> 4) << 4;
    }
protected:
    virtual GetObjectOutcome GetObjectWrap(const GetObjectRequest &request) const
    {
        return encryptionClient_->GetObjectInternal(request, metaData_);
    }
private:
    const OssEncryptionClient* encryptionClient_;
    const ObjectMetaData& metaData_;
};

class EncryptionResumableUploader : public ResumableUploader
{
public:
    EncryptionResumableUploader(const UploadObjectRequest& request,
        const OssEncryptionClient* enClient, const OssClientImpl *client)
        : ResumableUploader(request, client)
        , encryptionClient_(enClient)
    {
        //16 bytes alignment
        partSize_ = (partSize_ >> 4) << 4;
    }
protected:
    virtual void initRecordInfo()
    {
        ResumableUploader::initRecordInfo();
        encryptionCheckData_ = cryptoContext_.ContentMaterial().ContentIV();
    }

    virtual void buildRecordInfo(const Json::Value& value)
    {
        ResumableUploader::buildRecordInfo(value);
        ContentCryptoMaterial material;
        material.setEncryptedContentKey(Base64Decode(value["encryption-key"].asString()));
        material.setEncryptedContentIV(Base64Decode(value["encryption-iv"].asString()));
        material.setKeyWrapAlgorithm(value["encryption-wrap-alg"].asString());
        material.setDescription(JsonStringToMap(value["encryption-matdesc"].asString()));
        encryptionClient_->encryptionMaterials_->DecryptCEK(material);

        material.setCipherName(value["encryption-cek-alg"].asString());

        cryptoContext_.setUploadId(value["uploadID"].asString());
        cryptoContext_.setDataSize(value["size"].asInt64());
        cryptoContext_.setPartSize(value["partSize"].asInt64());
        cryptoContext_.setContentMaterial(material);

        encryptionCheckData_ = Base64Decode(value["encryption-check-data"].asString());
    }

    virtual void dumpRecordInfo(Json::Value& value)
    {
        ResumableUploader::dumpRecordInfo(value);
        const ByteBuffer& buff1 = cryptoContext_.ContentMaterial().EncryptedContentKey();
        value["encryption-key"] = Base64Encode((const char*)buff1.data(), buff1.size());
        const ByteBuffer& buff2 = cryptoContext_.ContentMaterial().EncryptedContentIV();
        value["encryption-iv"] = Base64Encode((const char*)buff2.data(), buff2.size());
        value["encryption-cek-alg"] = cryptoContext_.ContentMaterial().CipherName();
        value["encryption-wrap-alg"] = cryptoContext_.ContentMaterial().KeyWrapAlgorithm();
        value["encryption-matdesc"] = MapToJsonString(cryptoContext_.ContentMaterial().Description());

        const ByteBuffer& buff3 = cryptoContext_.ContentMaterial().ContentIV();
        value["encryption-check-data"] = Base64Encode((const char*)buff3.data(), buff3.size());
    }

    virtual int validateRecord()
    {
        int ret = ResumableUploader::validateRecord();
        if (ret != 0) {
            return ret;
        }

        if (cryptoContext_.ContentMaterial().ContentKey().empty()||
            cryptoContext_.ContentMaterial().ContentIV().empty() ||
            cryptoContext_.ContentMaterial().CipherName().empty()) {
            return -2;
        }

        if (encryptionCheckData_ != cryptoContext_.ContentMaterial().ContentIV()) {
            return -2;
        }

        return 0;
    }

    virtual InitiateMultipartUploadOutcome InitiateMultipartUploadWrap(const InitiateMultipartUploadRequest &request) const
    {
        cryptoContext_.setPartSize(static_cast<int64_t>(partSize_));
        cryptoContext_.setDataSize(static_cast<int64_t>(objectSize_));
        return encryptionClient_->InitiateMultipartUpload(request, cryptoContext_);
    }

    virtual PutObjectOutcome UploadPartWrap(const UploadPartRequest &request) const
    {
        return encryptionClient_->UploadPart(request, cryptoContext_);
    }

    virtual ListPartsOutcome ListPartsWrap(const ListPartsRequest &request) const
    {
        return encryptionClient_->ListParts(request);
    }

    virtual CompleteMultipartUploadOutcome CompleteMultipartUploadWrap(const CompleteMultipartUploadRequest &request) const
    {
        return encryptionClient_->CompleteMultipartUpload(request, cryptoContext_);
    }
private:
    const OssEncryptionClient* encryptionClient_;
    mutable MultipartUploadCryptoContext cryptoContext_;
    ByteBuffer encryptionCheckData_;
};
#endif
}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

OssEncryptionClient::OssEncryptionClient(const std::string& endpoint, const std::string& accessKeyId, const std::string& accessKeySecret,
    const ClientConfiguration& configuration,
    const std::shared_ptr<EncryptionMaterials>& encryptionMaterials, const CryptoConfiguration& cryptoConfig):
    OssEncryptionClient(endpoint, accessKeyId, accessKeySecret, "", configuration, encryptionMaterials, cryptoConfig)
{
}

OssEncryptionClient::OssEncryptionClient(const std::string& endpoint, const std::string& accessKeyId, const std::string& accessKeySecret, const std::string& securityToken,
    const ClientConfiguration& configuration, const std::shared_ptr<EncryptionMaterials>& encryptionMaterials, const CryptoConfiguration& cryptoConfig):
    OssClient(endpoint, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret, securityToken), configuration),
    encryptionMaterials_(encryptionMaterials),
    cryptoConfig_(cryptoConfig)
{
}

OssEncryptionClient::OssEncryptionClient(const std::string &endpoint, 
    const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration& configuration,
    const std::shared_ptr<EncryptionMaterials>& encryptionMaterials, const CryptoConfiguration& cryptoConfig) :
    OssClient(endpoint, credentialsProvider, configuration),
    encryptionMaterials_(encryptionMaterials),
    cryptoConfig_(cryptoConfig)
{
}

OssEncryptionClient::~OssEncryptionClient()
{
}

GetObjectOutcome OssEncryptionClient::GetObjectInternal(const GetObjectRequest& request, const ObjectMetaData& meta) const
{
    if (meta.hasUserHeader("client-side-encryption-key")) {
        auto module = CryptoModule::CreateCryptoModule(encryptionMaterials_, cryptoConfig_);
        return module->GetObjectSecurely(client_, request, meta);
    }
    else {
        return client_->GetObject(request);
    }
}

GetObjectOutcome OssEncryptionClient::GetObject(const GetObjectRequest& request) const
{
    const auto& reqeustBase = static_cast<const OssRequest &>(request);
    int ret = reqeustBase.validate();
    if (ret != 0) {
        return GetObjectOutcome(OssError("ValidateError", request.validateMessage(ret)));
    }

    HeadObjectRequest hRequest(request.Bucket(), request.Key());
    if (request.RequestPayer() == RequestPayer::Requester) {
        hRequest.setRequestPayer(request.RequestPayer());
    }
    if (!request.VersionId().empty()) {
        hRequest.setVersionId(request.VersionId());
    }
    auto outcome = client_->HeadObject(hRequest);
    if (!outcome.isSuccess()) {
        return GetObjectOutcome(outcome.error());
    }

    return GetObjectInternal(request, outcome.result());
}

GetObjectOutcome OssEncryptionClient::GetObject(const std::string &bucket, const std::string &key, const std::shared_ptr<std::iostream> &content) const
{
    GetObjectRequest request(bucket, key);
    request.setResponseStreamFactory([=]() { return content; });
    return GetObject(request);
}

GetObjectOutcome OssEncryptionClient::GetObject(const std::string &bucket, const std::string &key, const std::string &fileToSave) const
{
    GetObjectRequest request(bucket, key);
    request.setResponseStreamFactory([=]() {return std::make_shared<std::fstream>(fileToSave, std::ios_base::out | std::ios_base::trunc | std::ios::binary); });
    return GetObject(request);
}

PutObjectOutcome OssEncryptionClient::PutObject(const PutObjectRequest& request) const
{
    const auto& reqeustBase = static_cast<const OssRequest &>(request);
    int ret = reqeustBase.validate();
    if (ret != 0) {
        return PutObjectOutcome(OssError("ValidateError", request.validateMessage(ret)));
    }
    auto module = CryptoModule::CreateCryptoModule(encryptionMaterials_, cryptoConfig_);
    return module->PutObjectSecurely(client_, request);
}

PutObjectOutcome OssEncryptionClient::PutObject(const std::string &bucket, const std::string &key, const std::shared_ptr<std::iostream> &content) const
{
    return PutObject(PutObjectRequest(bucket, key, content));
}

PutObjectOutcome OssEncryptionClient::PutObject(const std::string &bucket, const std::string &key, const std::string &fileToUpload) const
{
    std::shared_ptr<std::iostream> content = std::make_shared<std::fstream>(fileToUpload, std::ios::in | std::ios::binary);
    return PutObject(PutObjectRequest(bucket, key, content));
}

AppendObjectOutcome OssEncryptionClient::AppendObject(const AppendObjectRequest& request) const
{
    UNUSED_PARAM(request);
    return AppendObjectOutcome(OssError("EncryptionClientError", "Not Support this operation."));
}

/*MultipartUpload*/
InitiateMultipartUploadOutcome OssEncryptionClient::InitiateMultipartUpload(const InitiateMultipartUploadRequest& request, MultipartUploadCryptoContext& ctx) const
{
    auto module = CryptoModule::CreateCryptoModule(encryptionMaterials_, cryptoConfig_);
    return module->InitiateMultipartUploadSecurely(client_, request, ctx);
}

PutObjectOutcome OssEncryptionClient::UploadPart(const UploadPartRequest& request, const MultipartUploadCryptoContext& ctx) const
{
    const auto& reqeustBase = static_cast<const OssRequest &>(request);
    int ret = reqeustBase.validate();
    if (ret != 0) {
        return PutObjectOutcome(OssError("ValidateError", request.validateMessage(ret)));
    }
    auto module = CryptoModule::CreateCryptoModule(encryptionMaterials_, cryptoConfig_);
    return module->UploadPartSecurely(client_, request, ctx);
}

UploadPartCopyOutcome OssEncryptionClient::UploadPartCopy(const UploadPartCopyRequest& request, const MultipartUploadCryptoContext& ctx) const
{
    UNUSED_PARAM(request);
    UNUSED_PARAM(ctx);
    return UploadPartCopyOutcome(OssError("EncryptionClientError", "Not Support this operation."));
}

CompleteMultipartUploadOutcome OssEncryptionClient::CompleteMultipartUpload(const CompleteMultipartUploadRequest& request, const MultipartUploadCryptoContext& ctx) const
{   
    UNUSED_PARAM(ctx);
    return OssClient::CompleteMultipartUpload(request);
}

#if !defined(DISABLE_RESUAMABLE)
/*Resumable Operation*/
PutObjectOutcome OssEncryptionClient::ResumableUploadObject(const UploadObjectRequest& request) const
{
    const auto& reqeustBase = static_cast<const OssResumableBaseRequest &>(request);
    int code = reqeustBase.validate();
    if (code != 0) {
        return PutObjectOutcome(OssError("ValidateError", reqeustBase.validateMessage(code)));
    }

    if (request.ObjectSize() <= request.PartSize()) {
        auto pRequest = ConvertToPutObjectRequest(request);
        return PutObject(pRequest);
    }
    else {
        EncryptionResumableUploader uploader(request, this, client_.get());
        return uploader.Upload();
    }
}

CopyObjectOutcome OssEncryptionClient::ResumableCopyObject(const MultiCopyObjectRequest& request) const
{
    UNUSED_PARAM(request);
    return CopyObjectOutcome(OssError("EncryptionClientError", "Not Support this operation."));
}

GetObjectOutcome OssEncryptionClient::ResumableDownloadObject(const DownloadObjectRequest& request) const
{
    HeadObjectRequest hRequest(request.Bucket(), request.Key());
    if (request.RequestPayer() == RequestPayer::Requester) {
        hRequest.setRequestPayer(request.RequestPayer());
    }
    if (!request.VersionId().empty()) {
        hRequest.setVersionId(request.VersionId());
    }
    auto hOutcome = client_->HeadObject(hRequest);
    if (!hOutcome.isSuccess()) {
        return GetObjectOutcome(hOutcome.error());
    }
    auto objectSize = static_cast<uint64_t>(hOutcome.result().ContentLength());
    if (objectSize <= request.PartSize()) {
        auto gRequest = ConvertToGetObjectRequest(request);
        auto gOutcome = GetObjectInternal(gRequest, hOutcome.result());
        if (gOutcome.isSuccess()) {
            gOutcome.result().setContent(nullptr);
        }
        return gOutcome;
    }
    else {
        const auto& reqeustBase = static_cast<const OssResumableBaseRequest &>(request);
        int code = reqeustBase.validate();
        if (code != 0) {
            return GetObjectOutcome(OssError("ValidateError", reqeustBase.validateMessage(code)));
        }
        if (hOutcome.result().UserMetaData().find("client-side-encryption-key") != hOutcome.result().UserMetaData().end()) {
            EncryptionResumableDownloader downloader(request, hOutcome.result(), this, client_.get(), objectSize);
            return downloader.Download();
        }
        else {
            ResumableDownloader downloader(request, client_.get(), objectSize);
            return downloader.Download();
        }
    }
}
#endif

/*Aysnc APIs*/
void OssEncryptionClient::GetObjectAsync(const GetObjectRequest& request, const GetObjectAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
    auto fn = [this, request, handler, context]()
    {
        handler(this, request, GetObject(request), context);
    };

    client_->asyncExecute(new Runnable(fn));
}

void OssEncryptionClient::PutObjectAsync(const PutObjectRequest& request, const PutObjectAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
    auto fn = [this, request, handler, context]()
    {
        handler(this, request, PutObject(request), context);
    };

    client_->asyncExecute(new Runnable(fn));
}

void OssEncryptionClient::UploadPartAsync(const UploadPartRequest& request, const UploadPartAsyncHandler& handler, const MultipartUploadCryptoContext& cryptoCtx, const std::shared_ptr<const AsyncCallerContext>& context) const
{
    auto fn = [this, request, handler, cryptoCtx, context]()
    {
        handler(this, request, UploadPart(request, cryptoCtx), context);
    };

    client_->asyncExecute(new Runnable(fn));
}

void OssEncryptionClient::UploadPartCopyAsync(const UploadPartCopyRequest& request, const UploadPartCopyAsyncHandler& handler, const MultipartUploadCryptoContext& cryptoCtx, const std::shared_ptr<const AsyncCallerContext>& context) const
{
    auto fn = [this, request, handler, cryptoCtx, context]()
    {
        handler(this, request, UploadPartCopy(request, cryptoCtx), context);
    };

    client_->asyncExecute(new Runnable(fn));
}

/*Callable APIs*/
GetObjectOutcomeCallable OssEncryptionClient::GetObjectCallable(const GetObjectRequest& request) const
{
    auto task = std::make_shared<std::packaged_task<GetObjectOutcome()>>(
        [this, request]()
    {
        return GetObject(request);
    });
    client_->asyncExecute(new Runnable([task]() { (*task)(); }));
    return task->get_future();
}

PutObjectOutcomeCallable OssEncryptionClient::PutObjectCallable(const PutObjectRequest& request) const
{
    auto task = std::make_shared<std::packaged_task<PutObjectOutcome()>>(
        [this, request]()
    {
        return PutObject(request);
    });
    client_->asyncExecute(new Runnable([task]() { (*task)(); }));
    return task->get_future();
}

PutObjectOutcomeCallable OssEncryptionClient::UploadPartCallable(const UploadPartRequest& request, const MultipartUploadCryptoContext& cryptoCtx) const
{
    auto task = std::make_shared<std::packaged_task<PutObjectOutcome()>>(
        [this, request, cryptoCtx]()
    {
        return UploadPart(request, cryptoCtx);
    });
    client_->asyncExecute(new Runnable([task]() { (*task)(); }));
    return task->get_future();
}

UploadPartCopyOutcomeCallable OssEncryptionClient::UploadPartCopyCallable(const UploadPartCopyRequest& request, const MultipartUploadCryptoContext& cryptoCtx) const
{
    auto task = std::make_shared<std::packaged_task<UploadPartCopyOutcome()>>(
        [this, request, cryptoCtx]()
    {
        return UploadPartCopy(request, cryptoCtx);
    });
    client_->asyncExecute(new Runnable([task]() { (*task)(); }));
    return task->get_future();
}

/*Generate URL*/
GetObjectOutcome OssEncryptionClient::GetObjectByUrl(const GetObjectByUrlRequest& request) const
{
    UNUSED_PARAM(request);
    return GetObjectOutcome(OssError("EncryptionClientError", "Not Support this operation."));
}

PutObjectOutcome OssEncryptionClient::PutObjectByUrl(const PutObjectByUrlRequest& request) const
{
    UNUSED_PARAM(request);
    return PutObjectOutcome(OssError("EncryptionClientError", "Not Support this operation."));
}
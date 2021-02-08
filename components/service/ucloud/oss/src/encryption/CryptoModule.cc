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

#include <alibabacloud/oss/Const.h>
#include "CryptoModule.h"
#include "CryptoStreamBuf.h"
#include "../utils/Utils.h"

using namespace AlibabaCloud::OSS;

#define CHECK_FUNC_RET(outcome, func, ret) \
    do { \
        if (ret != 0) { \
            std::string errMsg(#func" fail, return value:"); errMsg.append(std::to_string(ret));\
            return outcome(OssError("EncryptionClientError", errMsg)); \
        } \
    } while (0)



static std::string getUserAgent(const std::string& prefix)
{
    std::stringstream ss;
    ss << prefix << "/OssEncryptionClient";
    return ss.str();
}

std::shared_ptr<CryptoModule> CryptoModule::CreateCryptoModule(const std::shared_ptr<EncryptionMaterials>& encryptionMaterials,
    const CryptoConfiguration& cryptoConfig)
{
    switch(cryptoConfig.cryptoMode) {
    case CryptoMode::ENCRYPTION_AESCTR:
    default:
        return std::make_shared<CryptoModuleAESCTR>(encryptionMaterials, cryptoConfig);
    };
}

CryptoModule::CryptoModule(const std::shared_ptr<EncryptionMaterials>& encryptionMaterials, const CryptoConfiguration& cryptoConfig):
    encryptionMaterials_(encryptionMaterials),
    cryptoConfig_(cryptoConfig)
{
}

CryptoModule::~CryptoModule()
{
}

PutObjectOutcome CryptoModule::PutObjectSecurely(const std::shared_ptr<OssClientImpl>& client, const PutObjectRequest& request)
{
    PutObjectRequest putRequest(request);
    ContentCryptoMaterial material;

    initEncryptionCipher(material);
    generateKeyIV(material);
    auto ret = encryptionMaterials_->EncryptCEK(material);
    CHECK_FUNC_RET(PutObjectOutcome, EncryptCEK, ret);
    addMetaData(material, putRequest.MetaData());
    addUserAgent(putRequest.MetaData(), client->configuration().userAgent);
    CryptoStreamBuf cryptoStream(*putRequest.Body(), cipher_, material.ContentKey(), material.ContentIV());

    return client->PutObject(putRequest);
}

GetObjectOutcome CryptoModule::GetObjectSecurely(const std::shared_ptr<OssClientImpl>& client, const GetObjectRequest& request, const ObjectMetaData& meta)
{
    GetObjectRequest getRequest(request);
    ContentCryptoMaterial material;

    readMetaData(material, meta);
    initDecryptionCipher(material);

    if (material.CipherName() != cipher_->Name()) {
        std::stringstream ss;
        ss << "Cipher name is not support, " << 
              "expect " << cipher_->Name() << ", "
              "got " << material.CipherName() << ".";
        return GetObjectOutcome(OssError("EncryptionClientError", ss.str()));
    }

    auto ret = encryptionMaterials_->DecryptCEK(material);
    CHECK_FUNC_RET(GetObjectOutcome, DecryptCEK, ret);

    //range
    auto iv = material.ContentIV();
    auto range = request.Range();
    int64_t skipCnt = 0;
    int64_t blkSize = static_cast<int64_t>(cipher_->BlockSize());
    if (range.first > 0) {
        auto blockOfNum = range.first / blkSize;
        auto newBegin = blockOfNum * blkSize;
        skipCnt = range.first % blkSize;
        getRequest.setRange(newBegin, range.second);
        iv = cipher_->IncCTRCounter(iv, static_cast<uint64_t>(blockOfNum));
    }

    //ua
    getRequest.setUserAgent(getUserAgent(client->configuration().userAgent));

    std::shared_ptr<CryptoStreamBuf> cryptoStream = nullptr;
    std::shared_ptr<std::iostream> userContent = nullptr;
    getRequest.setResponseStreamFactory([&]() { 
            auto content = request.ResponseStreamFactory()();
            cryptoStream = std::make_shared<CryptoStreamBuf>(*content, cipher_, material.ContentKey(), iv, static_cast<int>(skipCnt));
            userContent = content;
            return content;
        }
    );

    auto outcome = client->GetObject(getRequest);
    if (skipCnt > 0 && outcome.isSuccess()) {
        ObjectMetaData ometa = outcome.result().Metadata();
        auto len = ometa.ContentLength();
        ometa.setContentLength(len - skipCnt);
        outcome.result().setMetaData(ometa);
    }

    cryptoStream = nullptr;
    userContent = nullptr;
    return outcome;
}

InitiateMultipartUploadOutcome CryptoModule::InitiateMultipartUploadSecurely(const std::shared_ptr<OssClientImpl>& client,
    const InitiateMultipartUploadRequest& request, MultipartUploadCryptoContext& ctx)
{
    std::string errMsg1;
    if (!checkUserParameter(ctx, errMsg1)) {
        return InitiateMultipartUploadOutcome(OssError("EncryptionClientError", errMsg1));
    }

    InitiateMultipartUploadRequest initRequest(request);
    ContentCryptoMaterial material;

    initEncryptionCipher(material);
    generateKeyIV(material);
    auto ret = encryptionMaterials_->EncryptCEK(material);
    CHECK_FUNC_RET(InitiateMultipartUploadOutcome, EncryptCEK, ret);
    addMetaData(material, initRequest.MetaData());
    addMetaDataMultipart(ctx, initRequest.MetaData());
    addUserAgent(initRequest.MetaData(), client->configuration().userAgent);

    auto outcome = client->InitiateMultipartUpload(initRequest);
    if (outcome.isSuccess()) {
        ctx.setContentMaterial(material);
        ctx.setUploadId(outcome.result().UploadId());
    }
    return outcome;
}

PutObjectOutcome CryptoModule::UploadPartSecurely(const std::shared_ptr<OssClientImpl>& client, const UploadPartRequest& request,
    const MultipartUploadCryptoContext& ctx)
{
    std::string errMsg;
    if (!checkUserParameter(ctx, errMsg)) {
        return PutObjectOutcome(OssError("EncryptionClientError", errMsg));
    }

    //check material
    if (ctx.ContentMaterial().ContentKey().empty() ||
        ctx.ContentMaterial().ContentIV().empty()) {
        return PutObjectOutcome(OssError("EncryptionClientError", "ContentKey/IV in CryptoContext is empty."));
    }

    UploadPartRequest uRequest(request);
    ContentCryptoMaterial material;
    initEncryptionCipher(material);
#if 0
    ObjectMetaData meta;
    addMetaData(ctx.ContentMaterial(), meta);
    addMetaDataMultipart(ctx, meta);
    uRequest.setMetaData(meta);
#endif
    //ua
    uRequest.setUserAgent(getUserAgent(client->configuration().userAgent));

    auto fileOffset = ctx.PartSize() * static_cast<int64_t>(uRequest.PartNumber() - 1);
    auto blockNum = fileOffset / static_cast<int64_t>(cipher_->BlockSize());
    auto iv = cipher_->IncCTRCounter(ctx.ContentMaterial().ContentIV(), static_cast<uint64_t>(blockNum));
    CryptoStreamBuf cryptoStream(*uRequest.Body(), cipher_, ctx.ContentMaterial().ContentKey(), iv);
    return client->UploadPart(uRequest);
}

void CryptoModule::addMetaData(const ContentCryptoMaterial& content, ObjectMetaData& meta)
{
    //x-oss-meta-client-side-encryption-key
    meta.addUserHeader("client-side-encryption-key",
        Base64Encode((const char*)content.EncryptedContentKey().data(), content.EncryptedContentKey().size()));

    //x-oss-meta-client-side-encryption-start //iv
    meta.addUserHeader("client-side-encryption-start",
        Base64Encode((const char*)content.EncryptedContentIV().data(), content.EncryptedContentIV().size()));

    //x-oss-meta-client-side-encryption-cek-alg 
    meta.addUserHeader("client-side-encryption-cek-alg", content.CipherName());

    //x-oss-meta-client-side-encryption-wrap-alg 
    meta.addUserHeader("client-side-encryption-wrap-alg", content.KeyWrapAlgorithm());

    //x-oss-meta-client-side-encryption-matdesc,json format 
    if (!content.Description().empty()) {
        std::string jsonStr = MapToJsonString(content.Description());
        if (!jsonStr.empty()) {
            meta.addUserHeader("client-side-encryption-matdesc", jsonStr);
        }
    }

    //x-oss-meta-client-side-encryption-magic-number-hmac
    if (!content.MagicNumber().empty()) {
        meta.addUserHeader("client-side-encryption-magic-number-hmac", content.MagicNumber());
    }

    //x-oss-meta-client-side-encryption-unencrypted-content-md5
    if (meta.hasHeader(Http::CONTENT_MD5)) {
        meta.addUserHeader("client-side-encryption-unencrypted-content-md5", meta.ContentMd5());
        meta.removeHeader(Http::CONTENT_MD5);
    }

    //x-oss-meta-client-side-encryption-unencrypted-content-length
    //ToDo
}

void CryptoModule::addMetaDataMultipart(const MultipartUploadCryptoContext& ctx, ObjectMetaData& meta)
{
    if (ctx.DataSize() > 0) {
        meta.addUserHeader("client-side-encryption-data-size", std::to_string(ctx.DataSize()));
    }
    meta.addUserHeader("client-side-encryption-part-size", std::to_string(ctx.PartSize()));
}

void CryptoModule::readMetaData(ContentCryptoMaterial& content, const ObjectMetaData& meta)
{
    //x-oss-meta-client-side-encryption-key
    if (meta.hasUserHeader("client-side-encryption-key")) {
        content.setEncryptedContentKey(Base64Decode(meta.UserMetaData().at("client-side-encryption-key")));
    }

    //x-oss-meta-client-side-encryption-start //iv
    if (meta.hasUserHeader("client-side-encryption-start")) {
        content.setEncryptedContentIV(Base64Decode(meta.UserMetaData().at("client-side-encryption-start")));
    }

    //x-oss-meta-client-side-encryption-cek-alg 
    if (meta.hasUserHeader("client-side-encryption-cek-alg")) {
        content.setCipherName(meta.UserMetaData().at("client-side-encryption-cek-alg"));
    }

    //x-oss-meta-client-side-encryption-wrap-alg 
    if (meta.hasUserHeader("client-side-encryption-wrap-alg")) {
        content.setKeyWrapAlgorithm(meta.UserMetaData().at("client-side-encryption-wrap-alg"));
    }

    //x-oss-meta-client-side-encryption-matdesc 
    if (meta.hasUserHeader("client-side-encryption-matdesc")) {
        content.setDescription(JsonStringToMap(meta.UserMetaData().at("client-side-encryption-matdesc")));
    }
}

void CryptoModule::addUserAgent(ObjectMetaData& meta, const std::string& prefix)
{
    if (!meta.hasHeader(Http::USER_AGENT)) {
        meta.addHeader(Http::USER_AGENT, getUserAgent(prefix));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//CryptoModuleAES265_CTR

CryptoModuleAESCTR::CryptoModuleAESCTR(const std::shared_ptr<EncryptionMaterials>& encryptionMaterials, 
    const CryptoConfiguration& cryptoConfig):
    CryptoModule(encryptionMaterials, cryptoConfig)
{
}

CryptoModuleAESCTR::~CryptoModuleAESCTR()
{
}

void CryptoModuleAESCTR::initEncryptionCipher(ContentCryptoMaterial& content)
{
    cipher_ = SymmetricCipher::CreateAES256_CTRImpl();
    content.setCipherName(cipher_->Name());
}

void CryptoModuleAESCTR::generateKeyIV(ContentCryptoMaterial& content)
{
    content.setContentKey(SymmetricCipher::GenerateKey(32));
    auto iv = SymmetricCipher::GenerateIV(16);
    iv[8] = iv[9] = iv[10] = iv[11] = 0;
    content.setContentIV(iv);
}

void CryptoModuleAESCTR::initDecryptionCipher(ContentCryptoMaterial& content)
{
    UNUSED_PARAM(content);
    cipher_ = SymmetricCipher::CreateAES256_CTRImpl();
}

bool CryptoModuleAESCTR::checkUserParameter(const MultipartUploadCryptoContext& ctx, std::string& errMsg)
{
    if (ctx.PartSize() < PartSizeLowerLimit) {
        errMsg = "PartSize should not be less than 102400.";
        return false;
    }

    if (ctx.PartSize() % 16) {
        errMsg = "PartSize is not 16 bytes alignment.";
        return false;
    }
    return true;
}

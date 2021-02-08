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
#include <alibabacloud/oss/encryption/CryptoConfiguration.h>
#include <alibabacloud/oss/encryption/EncryptionMaterials.h>
#include <alibabacloud/oss/encryption/Cipher.h>
#include <alibabacloud/oss/OssFwd.h>
#include <alibabacloud/oss/model/MultipartUploadCryptoContext.h>
#include "../OssClientImpl.h"

namespace AlibabaCloud
{
namespace OSS
{
    class CryptoModule
    {
    public:
        virtual ~CryptoModule();

        PutObjectOutcome PutObjectSecurely(const std::shared_ptr<OssClientImpl>& client, const PutObjectRequest& request);
        GetObjectOutcome GetObjectSecurely(const std::shared_ptr<OssClientImpl>& client, const GetObjectRequest& request, const ObjectMetaData& meta);

        InitiateMultipartUploadOutcome InitiateMultipartUploadSecurely(const std::shared_ptr<OssClientImpl>& client, 
            const InitiateMultipartUploadRequest& request, MultipartUploadCryptoContext& ctx);
        PutObjectOutcome UploadPartSecurely(const std::shared_ptr<OssClientImpl>& client, const UploadPartRequest& request,
            const MultipartUploadCryptoContext& ctx);
    public:
        static std::shared_ptr<CryptoModule> CreateCryptoModule(const std::shared_ptr<EncryptionMaterials>& encryptionMaterials,
            const CryptoConfiguration& cryptoConfig);

    protected:
        CryptoModule(const std::shared_ptr<EncryptionMaterials>& encryptionMaterials, const CryptoConfiguration& cryptoConfig);
        void addMetaData(const ContentCryptoMaterial& content, ObjectMetaData& meta);
        void addMetaDataMultipart(const MultipartUploadCryptoContext& ctx, ObjectMetaData& meta);
        void readMetaData(ContentCryptoMaterial& content, const ObjectMetaData& meta);
        void addUserAgent(ObjectMetaData& meta, const std::string& prefix);

        virtual void initEncryptionCipher(ContentCryptoMaterial& content) = 0;
        virtual void generateKeyIV(ContentCryptoMaterial& content) = 0;
        virtual void initDecryptionCipher(ContentCryptoMaterial& content) = 0;
        virtual bool checkUserParameter(const MultipartUploadCryptoContext& ctx, std::string& errMsg) = 0;

    protected:
        std::shared_ptr<EncryptionMaterials> encryptionMaterials_;
        CryptoConfiguration cryptoConfig_;
        std::shared_ptr<SymmetricCipher> cipher_;
    };

    class CryptoModuleAESCTR :public CryptoModule
    {
    public:
        CryptoModuleAESCTR(const std::shared_ptr<EncryptionMaterials>& encryptionMaterials, const CryptoConfiguration& cryptoConfig);
        ~CryptoModuleAESCTR();
    protected:
        virtual void initEncryptionCipher(ContentCryptoMaterial& content);
        virtual void generateKeyIV(ContentCryptoMaterial& content);
        virtual void initDecryptionCipher(ContentCryptoMaterial& content);
        virtual bool checkUserParameter(const MultipartUploadCryptoContext& ctx, std::string& errMsg);
    private:

    };

}
}

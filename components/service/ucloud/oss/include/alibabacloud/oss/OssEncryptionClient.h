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

#include <alibabacloud/oss/OssClient.h>
#include <alibabacloud/oss/encryption/EncryptionMaterials.h>
#include <alibabacloud/oss/encryption/CryptoConfiguration.h>
#include <alibabacloud/oss/model/MultipartUploadCryptoContext.h>

namespace AlibabaCloud
{
namespace OSS
{
    class EncryptionResumableDownloader;
    class EncryptionResumableUploader;
    class ALIBABACLOUD_OSS_EXPORT OssEncryptionClient : public OssClient
    {
    public:
        OssEncryptionClient(const std::string& endpoint, const std::string& accessKeyId, const std::string& accessKeySecret,
            const ClientConfiguration& configuration,
            const std::shared_ptr<EncryptionMaterials>& encryptionMaterials, const CryptoConfiguration& cryptoConfig);

        OssEncryptionClient(const std::string& endpoint, const std::string& accessKeyId, const std::string& accessKeySecret, const std::string& securityToken,
            const ClientConfiguration& configuration,
            const std::shared_ptr<EncryptionMaterials>& encryptionMaterials, const CryptoConfiguration& cryptoConfig);

        OssEncryptionClient(const std::string& endpoint, 
            const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration& configuration,
            const std::shared_ptr<EncryptionMaterials>& encryptionMaterials, const CryptoConfiguration& cryptoConfig);
        virtual ~OssEncryptionClient();

        /*Object*/
        GetObjectOutcome GetObject(const GetObjectRequest& request) const;
        GetObjectOutcome GetObject(const std::string &bucket, const std::string &key, const std::shared_ptr<std::iostream> &content) const;
        GetObjectOutcome GetObject(const std::string &bucket, const std::string &key, const std::string &fileToSave) const;
        PutObjectOutcome PutObject(const PutObjectRequest& request) const;
        PutObjectOutcome PutObject(const std::string &bucket, const std::string &key, const std::shared_ptr<std::iostream> &content) const;
        PutObjectOutcome PutObject(const std::string &bucket, const std::string &key, const std::string &fileToUpload) const;

        /*MultipartUpload*/
        InitiateMultipartUploadOutcome InitiateMultipartUpload(const InitiateMultipartUploadRequest& request, MultipartUploadCryptoContext& ctx) const;
        PutObjectOutcome UploadPart(const UploadPartRequest& request, const MultipartUploadCryptoContext& ctx) const;
        CompleteMultipartUploadOutcome CompleteMultipartUpload(const CompleteMultipartUploadRequest& request, const MultipartUploadCryptoContext& ctx) const;

#if !defined(OSS_DISABLE_RESUAMABLE)
        /*Resumable Operation*/
        PutObjectOutcome ResumableUploadObject(const UploadObjectRequest& request) const;
        GetObjectOutcome ResumableDownloadObject(const DownloadObjectRequest& request) const;
#endif

        /*Aysnc APIs*/
        void GetObjectAsync(const GetObjectRequest& request, const GetObjectAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
        void PutObjectAsync(const PutObjectRequest& request, const PutObjectAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
        void UploadPartAsync(const UploadPartRequest& request, const UploadPartAsyncHandler& handler, const MultipartUploadCryptoContext& cryptoCtx, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;

        /*Callable APIs*/
        GetObjectOutcomeCallable GetObjectCallable(const GetObjectRequest& request) const;
        PutObjectOutcomeCallable PutObjectCallable(const PutObjectRequest& request) const;
        PutObjectOutcomeCallable UploadPartCallable(const UploadPartRequest& request, const MultipartUploadCryptoContext& cryptoCtx) const;

    protected:
        AppendObjectOutcome AppendObject(const AppendObjectRequest& request) const;
        UploadPartCopyOutcome UploadPartCopy(const UploadPartCopyRequest& request, const MultipartUploadCryptoContext& ctx) const;
        void UploadPartCopyAsync(const UploadPartCopyRequest& request, const UploadPartCopyAsyncHandler& handler, const MultipartUploadCryptoContext& cryptoCtx, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
        UploadPartCopyOutcomeCallable UploadPartCopyCallable(const UploadPartCopyRequest& request, const MultipartUploadCryptoContext& cryptoCtx) const;
        CopyObjectOutcome ResumableCopyObject(const MultiCopyObjectRequest& request) const;
        GetObjectOutcome GetObjectByUrl(const GetObjectByUrlRequest& request) const;
        PutObjectOutcome PutObjectByUrl(const PutObjectByUrlRequest& request) const;

    private:
        friend class EncryptionResumableDownloader;
        friend class EncryptionResumableUploader;
        GetObjectOutcome GetObjectInternal(const GetObjectRequest& request, const ObjectMetaData& meta) const;
    private:
        std::shared_ptr<EncryptionMaterials> encryptionMaterials_;
        CryptoConfiguration cryptoConfig_;
    };
}
}

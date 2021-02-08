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

#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/client/ClientConfiguration.h>
#include <alibabacloud/oss/auth/CredentialsProvider.h>
#include <alibabacloud/oss/OssFwd.h>
#include <alibabacloud/oss/client/AsyncCallerContext.h>
#include <future>
#include <ctime>

namespace AlibabaCloud
{
namespace OSS
{
    /*Global Init/Deinit*/
    void ALIBABACLOUD_OSS_EXPORT InitializeSdk();
    bool ALIBABACLOUD_OSS_EXPORT IsSdkInitialized();
    void ALIBABACLOUD_OSS_EXPORT ShutdownSdk();

    /*Log*/
    void ALIBABACLOUD_OSS_EXPORT SetLogLevel(LogLevel level);
    void ALIBABACLOUD_OSS_EXPORT SetLogCallback(LogCallback callback);

    /*Utils*/
    std::string ALIBABACLOUD_OSS_EXPORT ComputeContentMD5(const char *data, size_t size);
    std::string ALIBABACLOUD_OSS_EXPORT ComputeContentMD5(std::istream& stream);
    std::string ALIBABACLOUD_OSS_EXPORT ComputeContentETag(const char* data, size_t size);
    std::string ALIBABACLOUD_OSS_EXPORT ComputeContentETag(std::istream& stream);
    std::string ALIBABACLOUD_OSS_EXPORT UrlEncode(const std::string& src);
    std::string ALIBABACLOUD_OSS_EXPORT UrlDecode(const std::string& src);
    std::string ALIBABACLOUD_OSS_EXPORT Base64Encode(const std::string& src);
    std::string ALIBABACLOUD_OSS_EXPORT Base64Encode(const char* src, int len);
    std::string ALIBABACLOUD_OSS_EXPORT Base64EncodeUrlSafe(const std::string& src);
    std::string ALIBABACLOUD_OSS_EXPORT Base64EncodeUrlSafe(const char* src, int len);
    std::string ALIBABACLOUD_OSS_EXPORT ToGmtTime(std::time_t& t);
    std::string ALIBABACLOUD_OSS_EXPORT ToUtcTime(std::time_t& t);
    std::time_t ALIBABACLOUD_OSS_EXPORT UtcToUnixTime(const std::string& t);
    uint64_t    ALIBABACLOUD_OSS_EXPORT ComputeCRC64(uint64_t crc, void* buf, size_t len);
    uint64_t    ALIBABACLOUD_OSS_EXPORT CombineCRC64(uint64_t crc1, uint64_t crc2, uintmax_t len2);

    /*Aysnc APIs*/
    class OssClient;
    using ListObjectAsyncHandler = std::function<void(const AlibabaCloud::OSS::OssClient*, const ListObjectsRequest&, const ListObjectOutcome&, const std::shared_ptr<const AsyncCallerContext>&)>;
    using GetObjectAsyncHandler  = std::function<void(const AlibabaCloud::OSS::OssClient*, const GetObjectRequest&, const GetObjectOutcome&, const std::shared_ptr<const AsyncCallerContext>&)>;
    using PutObjectAsyncHandler = std::function<void(const AlibabaCloud::OSS::OssClient*, const PutObjectRequest&, const PutObjectOutcome&, const std::shared_ptr<const AsyncCallerContext>&)>;
    using UploadPartAsyncHandler = std::function<void(const AlibabaCloud::OSS::OssClient*, const UploadPartRequest&, const PutObjectOutcome&, const std::shared_ptr<const AsyncCallerContext>&)>;
    using UploadPartCopyAsyncHandler = std::function<void(const AlibabaCloud::OSS::OssClient*, const UploadPartCopyRequest&, const UploadPartCopyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)>;

    /*Callable*/
    using ListObjectOutcomeCallable = std::future<ListObjectOutcome>;
    using GetObjectOutcomeCallable  = std::future<GetObjectOutcome>;
    using PutObjectOutcomeCallable  = std::future<PutObjectOutcome>;
    using UploadPartCopyOutcomeCallable = std::future<UploadPartCopyOutcome>;

    class OssClientImpl;
    class ALIBABACLOUD_OSS_EXPORT OssClient
    {
    public:

        OssClient(const std::string& endpoint, const std::string& accessKeyId, const std::string& accessKeySecret, 
                                const ClientConfiguration& configuration);
        OssClient(const std::string& endpoint, const std::string& accessKeyId, const std::string& accessKeySecret, const std::string& securityToken,
                                const ClientConfiguration& configuration); 
        OssClient(const std::string& endpoint, const Credentials& credentials, const ClientConfiguration& configuration);
        OssClient(const std::string& endpoint, const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration& configuration);
        virtual ~OssClient();

#if !defined(OSS_DISABLE_BUCKET)
        /*Service*/
        ListBucketsOutcome ListBuckets() const;
        ListBucketsOutcome ListBuckets(const ListBucketsRequest& request) const;

        /*Bucket*/
        CreateBucketOutcome CreateBucket(const std::string& bucket, StorageClass storageClass = StorageClass::Standard) const;
        CreateBucketOutcome CreateBucket(const std::string& bucket, StorageClass storageClass, CannedAccessControlList acl) const;
        CreateBucketOutcome CreateBucket(const CreateBucketRequest& request) const;

        ListBucketInventoryConfigurationsOutcome ListBucketInventoryConfigurations(const ListBucketInventoryConfigurationsRequest& request) const;

        VoidOutcome SetBucketAcl(const std::string& bucket, CannedAccessControlList acl) const;
        VoidOutcome SetBucketAcl(const SetBucketAclRequest& request) const;
        VoidOutcome SetBucketLogging(const std::string& bucket, const std::string& targetBucket, const std::string& targetPrefix) const;
        VoidOutcome SetBucketLogging(const SetBucketLoggingRequest& request) const;
        VoidOutcome SetBucketWebsite(const std::string& bucket, const std::string& indexDocument) const;
        VoidOutcome SetBucketWebsite(const std::string& bucket, const std::string& indexDocument, const std::string& errorDocument) const;
        VoidOutcome SetBucketWebsite(const SetBucketWebsiteRequest& request) const;
        VoidOutcome SetBucketReferer(const std::string& bucket, const RefererList& refererList, bool allowEmptyReferer) const;
        VoidOutcome SetBucketReferer(const SetBucketRefererRequest& request) const;
        VoidOutcome SetBucketLifecycle(const SetBucketLifecycleRequest& request) const;
        VoidOutcome SetBucketCors(const std::string& bucket, const CORSRuleList& rules) const;
        VoidOutcome SetBucketCors(const SetBucketCorsRequest& request) const;
        VoidOutcome SetBucketStorageCapacity(const std::string& bucket, int64_t storageCapacity) const;
        VoidOutcome SetBucketStorageCapacity(const SetBucketStorageCapacityRequest& request) const;
        VoidOutcome SetBucketPolicy(const SetBucketPolicyRequest& request) const;
        VoidOutcome SetBucketRequestPayment(const SetBucketRequestPaymentRequest& request) const;
        VoidOutcome SetBucketEncryption(const SetBucketEncryptionRequest& request) const;
        VoidOutcome SetBucketTagging(const SetBucketTaggingRequest& request) const;
        VoidOutcome SetBucketQosInfo(const SetBucketQosInfoRequest& request) const;
        VoidOutcome SetBucketVersioning(const SetBucketVersioningRequest& request) const;
        VoidOutcome SetBucketInventoryConfiguration(const SetBucketInventoryConfigurationRequest& request) const;

        VoidOutcome DeleteBucket(const std::string& bucket) const;
        VoidOutcome DeleteBucket(const DeleteBucketRequest& request) const;
        VoidOutcome DeleteBucketLogging(const std::string& bucket) const;
        VoidOutcome DeleteBucketLogging(const DeleteBucketLoggingRequest& request) const;
        VoidOutcome DeleteBucketPolicy(const DeleteBucketPolicyRequest& request) const;
        VoidOutcome DeleteBucketWebsite(const std::string& bucket) const;
        VoidOutcome DeleteBucketWebsite(const DeleteBucketWebsiteRequest& request) const;
        VoidOutcome DeleteBucketLifecycle(const std::string& bucket) const;
        VoidOutcome DeleteBucketLifecycle(const DeleteBucketLifecycleRequest& request) const;
        VoidOutcome DeleteBucketCors(const std::string& bucket) const;
        VoidOutcome DeleteBucketCors(const DeleteBucketCorsRequest& request) const;
        VoidOutcome DeleteBucketEncryption(const DeleteBucketEncryptionRequest& request) const;
        VoidOutcome DeleteBucketTagging(const DeleteBucketTaggingRequest& request) const;
        VoidOutcome DeleteBucketQosInfo(const DeleteBucketQosInfoRequest& request) const;
        VoidOutcome DeleteBucketInventoryConfiguration(const DeleteBucketInventoryConfigurationRequest& request) const;

        GetBucketAclOutcome GetBucketAcl(const std::string& bucket) const;
        GetBucketAclOutcome GetBucketAcl(const GetBucketAclRequest& request) const;
        GetBucketLocationOutcome GetBucketLocation(const std::string& bucket) const;
        GetBucketLocationOutcome GetBucketLocation(const GetBucketLocationRequest& request) const;
        GetBucketInfoOutcome  GetBucketInfo(const std::string& bucket) const;
        GetBucketInfoOutcome  GetBucketInfo(const GetBucketInfoRequest& request) const;
        GetBucketLoggingOutcome GetBucketLogging(const std::string& bucket) const;
        GetBucketLoggingOutcome GetBucketLogging(const GetBucketLoggingRequest& request) const;
        GetBucketWebsiteOutcome GetBucketWebsite(const std::string& bucket) const;
        GetBucketWebsiteOutcome GetBucketWebsite(const GetBucketWebsiteRequest& request) const;
        GetBucketRefererOutcome GetBucketReferer(const std::string& bucket) const;
        GetBucketRefererOutcome GetBucketReferer(const GetBucketRefererRequest& request) const;
        GetBucketLifecycleOutcome GetBucketLifecycle(const std::string& bucket) const;
        GetBucketLifecycleOutcome GetBucketLifecycle(const GetBucketLifecycleRequest& request) const;
        GetBucketStatOutcome GetBucketStat(const std::string& bucket) const;
        GetBucketStatOutcome GetBucketStat(const GetBucketStatRequest& request) const;
        GetBucketCorsOutcome GetBucketCors(const std::string& bucket) const;
        GetBucketCorsOutcome GetBucketCors(const GetBucketCorsRequest& request) const;
        GetBucketStorageCapacityOutcome GetBucketStorageCapacity(const std::string& bucket) const;
        GetBucketStorageCapacityOutcome GetBucketStorageCapacity(const GetBucketStorageCapacityRequest& request) const;
        GetBucketPolicyOutcome GetBucketPolicy(const GetBucketPolicyRequest& request) const;
        GetBucketPaymentOutcome GetBucketRequestPayment(const GetBucketRequestPaymentRequest& request) const;
        GetBucketEncryptionOutcome GetBucketEncryption(const GetBucketEncryptionRequest& request) const;
        GetBucketTaggingOutcome GetBucketTagging(const GetBucketTaggingRequest& request) const;
        GetBucketQosInfoOutcome GetBucketQosInfo(const GetBucketQosInfoRequest& request) const;
        GetUserQosInfoOutcome GetUserQosInfo(const GetUserQosInfoRequest& request) const;
        GetBucketVersioningOutcome GetBucketVersioning(const GetBucketVersioningRequest& request) const;
        GetBucketInventoryConfigurationOutcome GetBucketInventoryConfiguration(const GetBucketInventoryConfigurationRequest& request) const;
#endif

        /*Object*/
        ListObjectOutcome ListObjects(const std::string& bucket) const;
        ListObjectOutcome ListObjects(const std::string& bucket, const std::string& prefix) const;
        ListObjectOutcome ListObjects(const ListObjectsRequest& request) const;
        ListObjectVersionsOutcome ListObjectVersions(const std::string& bucket) const;
        ListObjectVersionsOutcome ListObjectVersions(const std::string& bucket, const std::string& prefix) const;
        ListObjectVersionsOutcome ListObjectVersions(const ListObjectVersionsRequest& request) const;

        GetObjectOutcome GetObject(const std::string& bucket, const std::string& key) const;
        GetObjectOutcome GetObject(const std::string& bucket, const std::string& key, const std::shared_ptr<std::iostream>& content) const;
        GetObjectOutcome GetObject(const std::string& bucket, const std::string& key, const std::string& fileToSave) const;
        GetObjectOutcome GetObject(const GetObjectRequest& request) const;
        PutObjectOutcome PutObject(const std::string& bucket, const std::string& key, const std::shared_ptr<std::iostream>& content) const;
        PutObjectOutcome PutObject(const std::string& bucket, const std::string& key, const std::string& fileToUpload) const;
        PutObjectOutcome PutObject(const std::string& bucket, const std::string& key, const std::shared_ptr<std::iostream>& content, const ObjectMetaData& meta) const;
        PutObjectOutcome PutObject(const std::string& bucket, const std::string& key, const std::string& fileToUpload, const ObjectMetaData& meta) const;
        PutObjectOutcome PutObject(const PutObjectRequest& request) const;
        DeleteObjectOutcome DeleteObject(const std::string& bucket, const std::string& key) const;
        DeleteObjectOutcome DeleteObject(const DeleteObjectRequest& request) const;
        DeleteObjecstOutcome DeleteObjects(const std::string bucket, const DeletedKeyList &keyList) const;
        DeleteObjecstOutcome DeleteObjects(const DeleteObjectsRequest& request) const;
        DeleteObjecVersionstOutcome DeleteObjectVersions(const std::string bucket, const ObjectIdentifierList &objectList) const;
        DeleteObjecVersionstOutcome DeleteObjectVersions(const DeleteObjectVersionsRequest& request) const;
        ObjectMetaDataOutcome HeadObject(const std::string& bucket, const std::string& key) const;
        ObjectMetaDataOutcome HeadObject(const HeadObjectRequest& request) const;
        ObjectMetaDataOutcome GetObjectMeta(const std::string& bucket, const std::string& key) const;
        ObjectMetaDataOutcome GetObjectMeta(const GetObjectMetaRequest& request) const;
        AppendObjectOutcome AppendObject(const AppendObjectRequest& request) const;
        CopyObjectOutcome CopyObject(const CopyObjectRequest& request) const;
        RestoreObjectOutcome RestoreObject(const std::string& bucket, const std::string& key) const;
        RestoreObjectOutcome RestoreObject(const RestoreObjectRequest& request) const;
        SetObjectAclOutcome SetObjectAcl(const SetObjectAclRequest& request) const;
        GetObjectAclOutcome GetObjectAcl(const GetObjectAclRequest& request) const;
        CreateSymlinkOutcome CreateSymlink(const CreateSymlinkRequest& request) const;
        GetSymlinkOutcome GetSymlink(const GetSymlinkRequest& request) const;
        GetObjectOutcome ProcessObject(const ProcessObjectRequest& request) const;

        GetObjectOutcome SelectObject(const SelectObjectRequest& request) const;
        CreateSelectObjectMetaOutcome CreateSelectObjectMeta(const CreateSelectObjectMetaRequest& request) const;

        SetObjectTaggingOutcome SetObjectTagging(const SetObjectTaggingRequest& request) const;
        DeleteObjectTaggingOutcome DeleteObjectTagging(const DeleteObjectTaggingRequest& request) const;
        GetObjectTaggingOutcome GetObjectTagging(const GetObjectTaggingRequest& request) const;

        /*MultipartUpload*/
        InitiateMultipartUploadOutcome InitiateMultipartUpload(const InitiateMultipartUploadRequest& request) const;
        PutObjectOutcome UploadPart(const UploadPartRequest& request) const;
        UploadPartCopyOutcome UploadPartCopy(const UploadPartCopyRequest& request) const;
        CompleteMultipartUploadOutcome CompleteMultipartUpload(const CompleteMultipartUploadRequest& request) const;
        VoidOutcome AbortMultipartUpload(const AbortMultipartUploadRequest& request) const;
        ListMultipartUploadsOutcome ListMultipartUploads(const ListMultipartUploadsRequest& request) const;
        ListPartsOutcome ListParts(const ListPartsRequest& request) const;

        /*Generate URL*/
        StringOutcome GeneratePresignedUrl(const GeneratePresignedUrlRequest& request) const;
        StringOutcome GeneratePresignedUrl(const std::string& bucket, const std::string& key) const;
        StringOutcome GeneratePresignedUrl(const std::string& bucket, const std::string& key, int64_t expires) const;
        StringOutcome GeneratePresignedUrl(const std::string& bucket, const std::string& key, int64_t expires, Http::Method method) const;
        GetObjectOutcome GetObjectByUrl(const GetObjectByUrlRequest& request) const;
        GetObjectOutcome GetObjectByUrl(const std::string& url) const;
        GetObjectOutcome GetObjectByUrl(const std::string& url, const std::string& file) const;
        PutObjectOutcome PutObjectByUrl(const PutObjectByUrlRequest& request) const;
        PutObjectOutcome PutObjectByUrl(const std::string& url, const std::string& file) const;
        PutObjectOutcome PutObjectByUrl(const std::string& url, const std::string& file, const ObjectMetaData& metaData) const;
        PutObjectOutcome PutObjectByUrl(const std::string& url, const std::shared_ptr<std::iostream>& content) const;
        PutObjectOutcome PutObjectByUrl(const std::string& url, const std::shared_ptr<std::iostream>& content, const ObjectMetaData& metaData) const;

        /*Generate Post Policy*/

        /*Resumable Operation*/
#if !defined(OSS_DISABLE_RESUAMABLE)
        PutObjectOutcome ResumableUploadObject(const UploadObjectRequest& request) const;
        CopyObjectOutcome ResumableCopyObject(const MultiCopyObjectRequest& request) const;
        GetObjectOutcome ResumableDownloadObject(const DownloadObjectRequest& request) const;
#endif

#if !defined(OSS_DISABLE_LIVECHANNEL)
        /*Live Channel*/
        VoidOutcome PutLiveChannelStatus(const PutLiveChannelStatusRequest& request) const;
        PutLiveChannelOutcome PutLiveChannel(const PutLiveChannelRequest& request) const;
        VoidOutcome PostVodPlaylist(const PostVodPlaylistRequest& request) const;
        GetVodPlaylistOutcome GetVodPlaylist(const GetVodPlaylistRequest& request) const;
        GetLiveChannelStatOutcome GetLiveChannelStat(const GetLiveChannelStatRequest& request) const;
        GetLiveChannelInfoOutcome GetLiveChannelInfo(const GetLiveChannelInfoRequest& request) const;
        GetLiveChannelHistoryOutcome GetLiveChannelHistory(const GetLiveChannelHistoryRequest& request) const;
        ListLiveChannelOutcome ListLiveChannel(const ListLiveChannelRequest& request) const;
        VoidOutcome DeleteLiveChannel(const DeleteLiveChannelRequest& request) const;
        StringOutcome GenerateRTMPSignedUrl(const GenerateRTMPSignedUrlRequest& request) const;
#endif

        /*Aysnc APIs*/
        void ListObjectsAsync(const ListObjectsRequest& request, const ListObjectAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
        void GetObjectAsync(const GetObjectRequest& request, const GetObjectAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
        void PutObjectAsync(const PutObjectRequest& request, const PutObjectAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
        void UploadPartAsync(const UploadPartRequest& request, const UploadPartAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
        void UploadPartCopyAsync(const UploadPartCopyRequest& request, const UploadPartCopyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;

        /*Callable APIs*/
        ListObjectOutcomeCallable ListObjectsCallable(const ListObjectsRequest& request) const;
        GetObjectOutcomeCallable GetObjectCallable(const GetObjectRequest& request) const;
        PutObjectOutcomeCallable PutObjectCallable(const PutObjectRequest& request) const;
        PutObjectOutcomeCallable UploadPartCallable(const UploadPartRequest& request) const;
        UploadPartCopyOutcomeCallable UploadPartCopyCallable(const UploadPartCopyRequest& request) const;

        /*Extended APIs*/
#if !defined(OSS_DISABLE_BUCKET)
        bool DoesBucketExist(const std::string& bucket) const;
#endif
        bool DoesObjectExist(const std::string& bucket, const std::string& key) const;
        CopyObjectOutcome ModifyObjectMeta(const std::string& bucket, const std::string& key, const ObjectMetaData& meta);

        /*Requests control*/
        void DisableRequest();
        void EnableRequest();
    protected:
        std::shared_ptr<OssClientImpl> client_;
    };
}
}

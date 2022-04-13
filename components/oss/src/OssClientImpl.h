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

#ifndef ALIBABACLOUD_OSS_OSSCLIENTIMPL_H_
#define ALIBABACLOUD_OSS_OSSCLIENTIMPL_H_

#include <alibabacloud/oss/client/ClientConfiguration.h>
#include <alibabacloud/oss/auth/CredentialsProvider.h>
#include <alibabacloud/oss/OssRequest.h>
#include <alibabacloud/oss/OssResponse.h>
#include <alibabacloud/oss/utils/Executor.h>
#include <alibabacloud/oss/OssFwd.h>
#include "auth/Signer.h"
#include "client/Client.h"
#ifdef GetObject
#undef GetObject
#endif

namespace AlibabaCloud
{
namespace OSS
{
    class OssClientImpl : public Client
    {
    public:
        typedef Client BASE;

        OssClientImpl(const std::string &endpoint, const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration);
        virtual ~OssClientImpl();
        int asyncExecute(Runnable * r) const;

#if !defined(OSS_DISABLE_BUCKET)
        ListBucketsOutcome ListBuckets(const ListBucketsRequest &request) const;
        CreateBucketOutcome CreateBucket(const CreateBucketRequest &request) const;
        VoidOutcome SetBucketAcl(const SetBucketAclRequest& request) const;
        VoidOutcome SetBucketLogging(const SetBucketLoggingRequest& request) const;
        VoidOutcome SetBucketWebsite(const SetBucketWebsiteRequest& request) const;
        VoidOutcome SetBucketReferer(const SetBucketRefererRequest& request) const;
        VoidOutcome SetBucketLifecycle(const SetBucketLifecycleRequest& request) const;
        VoidOutcome SetBucketCors(const SetBucketCorsRequest& request) const;
        VoidOutcome SetBucketStorageCapacity(const SetBucketStorageCapacityRequest& request) const;
        VoidOutcome SetBucketPolicy(const SetBucketPolicyRequest& request) const;
        VoidOutcome SetBucketRequestPayment(const SetBucketRequestPaymentRequest& request) const;
        VoidOutcome SetBucketEncryption(const SetBucketEncryptionRequest& request) const;
        VoidOutcome SetBucketTagging(const SetBucketTaggingRequest& request) const;
        VoidOutcome SetBucketQosInfo(const SetBucketQosInfoRequest& request) const;
        VoidOutcome SetBucketVersioning(const SetBucketVersioningRequest& request) const;
        VoidOutcome SetBucketInventoryConfiguration(const SetBucketInventoryConfigurationRequest& request) const;

        VoidOutcome DeleteBucket(const DeleteBucketRequest &request) const;
        VoidOutcome DeleteBucketLogging(const DeleteBucketLoggingRequest& request) const;
        VoidOutcome DeleteBucketWebsite(const DeleteBucketWebsiteRequest& request) const;
        VoidOutcome DeleteBucketLifecycle(const DeleteBucketLifecycleRequest& request) const;
        VoidOutcome DeleteBucketCors(const DeleteBucketCorsRequest& request) const;
        VoidOutcome DeleteBucketPolicy(const DeleteBucketPolicyRequest& request) const;
        VoidOutcome DeleteBucketEncryption(const DeleteBucketEncryptionRequest& request) const;
        VoidOutcome DeleteBucketTagging(const DeleteBucketTaggingRequest& request) const;
        VoidOutcome DeleteBucketQosInfo(const DeleteBucketQosInfoRequest& request) const;
        VoidOutcome DeleteBucketInventoryConfiguration(const DeleteBucketInventoryConfigurationRequest& request) const;

        ListBucketInventoryConfigurationsOutcome ListBucketInventoryConfigurations(const ListBucketInventoryConfigurationsRequest& request) const;

        GetBucketAclOutcome GetBucketAcl(const GetBucketAclRequest &request) const;
        GetBucketLocationOutcome GetBucketLocation(const GetBucketLocationRequest &request) const;
        GetBucketInfoOutcome  GetBucketInfo(const  GetBucketInfoRequest &request) const;
        GetBucketLoggingOutcome GetBucketLogging(const GetBucketLoggingRequest &request) const;
        GetBucketWebsiteOutcome GetBucketWebsite(const GetBucketWebsiteRequest &request) const;
        GetBucketRefererOutcome GetBucketReferer(const GetBucketRefererRequest &request) const;
        GetBucketLifecycleOutcome GetBucketLifecycle(const GetBucketLifecycleRequest &request) const;
        GetBucketStatOutcome GetBucketStat(const GetBucketStatRequest &request) const;
        GetBucketCorsOutcome GetBucketCors(const GetBucketCorsRequest &request) const;
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
        ListObjectOutcome ListObjects(const ListObjectsRequest &request) const;
        ListObjectVersionsOutcome ListObjectVersions(const ListObjectVersionsRequest &request) const;

        GetObjectOutcome GetObject(const GetObjectRequest &request) const;
        PutObjectOutcome PutObject(const PutObjectRequest &request) const;
        DeleteObjectOutcome DeleteObject(const DeleteObjectRequest &request) const;
        DeleteObjecstOutcome DeleteObjects(const DeleteObjectsRequest &request) const;
        DeleteObjecVersionstOutcome DeleteObjectVersions(const DeleteObjectVersionsRequest& request) const;
        ObjectMetaDataOutcome HeadObject(const HeadObjectRequest &request) const;
        ObjectMetaDataOutcome GetObjectMeta(const GetObjectMetaRequest &request) const;

        GetObjectAclOutcome GetObjectAcl(const GetObjectAclRequest &request) const;
        AppendObjectOutcome AppendObject(const AppendObjectRequest &request) const;
        CopyObjectOutcome CopyObject(const CopyObjectRequest &request) const;
        GetSymlinkOutcome GetSymlink(const GetSymlinkRequest &request) const;
        RestoreObjectOutcome RestoreObject(const RestoreObjectRequest &request) const;
        CreateSymlinkOutcome CreateSymlink(const CreateSymlinkRequest &request) const;
        SetObjectAclOutcome SetObjectAcl(const SetObjectAclRequest &request) const;
        GetObjectOutcome ProcessObject(const ProcessObjectRequest &request) const;

        GetObjectOutcome SelectObject(const SelectObjectRequest &request) const;
        CreateSelectObjectMetaOutcome CreateSelectObjectMeta(const CreateSelectObjectMetaRequest &request) const;

        SetObjectTaggingOutcome SetObjectTagging(const SetObjectTaggingRequest& request) const;
        DeleteObjectTaggingOutcome DeleteObjectTagging(const DeleteObjectTaggingRequest& request) const;
        GetObjectTaggingOutcome GetObjectTagging(const GetObjectTaggingRequest& request) const;

        /*MultipartUpload*/
        InitiateMultipartUploadOutcome InitiateMultipartUpload(const InitiateMultipartUploadRequest &request) const;
        PutObjectOutcome UploadPart(const UploadPartRequest& request) const;
        UploadPartCopyOutcome UploadPartCopy(const UploadPartCopyRequest &request) const;
        CompleteMultipartUploadOutcome CompleteMultipartUpload(const CompleteMultipartUploadRequest &request) const;
        VoidOutcome AbortMultipartUpload(const AbortMultipartUploadRequest &request) const;
        ListMultipartUploadsOutcome ListMultipartUploads(const ListMultipartUploadsRequest &request) const;
        ListPartsOutcome ListParts(const ListPartsRequest &request) const;
        
        /*Generate URL*/
        StringOutcome GeneratePresignedUrl(const GeneratePresignedUrlRequest &request) const;
        GetObjectOutcome GetObjectByUrl(const GetObjectByUrlRequest &request) const;
        PutObjectOutcome PutObjectByUrl(const PutObjectByUrlRequest &request) const;


        /*Generate Post Policy*/

#if !defined(OSS_DISABLE_RESUAMABLE)
        /*Resumable Operation*/
        PutObjectOutcome ResumableUploadObject(const UploadObjectRequest& request) const;
        CopyObjectOutcome ResumableCopyObject(const MultiCopyObjectRequest& request) const;
        GetObjectOutcome ResumableDownloadObject(const DownloadObjectRequest& request) const;
#endif

#if !defined(OSS_DISABLE_LIVECHANNEL)
        /*Live Channel*/
        VoidOutcome PutLiveChannelStatus(const PutLiveChannelStatusRequest &request) const;
        PutLiveChannelOutcome PutLiveChannel(const PutLiveChannelRequest &request) const;
        VoidOutcome PostVodPlaylist(const PostVodPlaylistRequest &request) const;
        GetVodPlaylistOutcome GetVodPlaylist(const GetVodPlaylistRequest& request) const;
        GetLiveChannelStatOutcome GetLiveChannelStat(const GetLiveChannelStatRequest &request) const;
        GetLiveChannelInfoOutcome GetLiveChannelInfo(const GetLiveChannelInfoRequest &request) const;
        GetLiveChannelHistoryOutcome GetLiveChannelHistory(const GetLiveChannelHistoryRequest &request) const;
        ListLiveChannelOutcome ListLiveChannel(const ListLiveChannelRequest &request) const;
        VoidOutcome DeleteLiveChannel(const DeleteLiveChannelRequest &request) const;
        StringOutcome GenerateRTMPSignedUrl(const GenerateRTMPSignedUrlRequest &request) const;
#endif

        /*Requests control*/
        void DisableRequest();
        void EnableRequest();

    protected:
        virtual std::shared_ptr<HttpRequest> buildHttpRequest(const std::string & endpoint, const ServiceRequest &msg, Http::Method method) const;
        virtual bool hasResponseError(const std::shared_ptr<HttpResponse>&response)  const;
        OssOutcome MakeRequest(const OssRequest &request, Http::Method method) const;

    private:
        void addHeaders(const std::shared_ptr<HttpRequest> &httpRequest, const HeaderCollection &headers) const;
        void addBody(const std::shared_ptr<HttpRequest> &httpRequest, const std::shared_ptr<std::iostream>& body, bool contentMd5 = false) const;
        void addSignInfo(const std::shared_ptr<HttpRequest> &httpRequest, const ServiceRequest &request) const;
        void addUrl(const std::shared_ptr<HttpRequest> &httpRequest, const std::string &endpoint, const ServiceRequest &request) const;
        void addOther(const std::shared_ptr<HttpRequest> &httpRequest, const ServiceRequest &request) const;

        OssError buildError(const Error &error) const;
        ServiceResult buildResult(const OssRequest &request, const std::shared_ptr<HttpResponse> &httpResponse) const;

    private:
        std::string endpoint_;
        std::shared_ptr<CredentialsProvider> credentialsProvider_;
        std::shared_ptr<Signer> signer_;
        std::shared_ptr<Executor> executor_;
    };
}
}
#endif // !ALIBABACLOUD_OSS_OSSCLIENTIMPL_H_

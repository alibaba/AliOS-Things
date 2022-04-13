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

#include <memory>
#include <iostream>
#include <alibabacloud/oss/Global.h>
#include <alibabacloud/oss/Types.h>
#include <alibabacloud/oss/OssError.h>
#include <alibabacloud/oss/ServiceResult.h>
#include <alibabacloud/oss/utils/Outcome.h>
#include <alibabacloud/oss/model/VoidResult.h>

#if !defined(OSS_DISABLE_BUCKET)
#include <alibabacloud/oss/model/ListBucketsRequest.h>
#include <alibabacloud/oss/model/ListBucketsResult.h>
#include <alibabacloud/oss/model/CreateBucketRequest.h>
#include <alibabacloud/oss/model/SetBucketAclRequest.h>
#include <alibabacloud/oss/model/SetBucketLoggingRequest.h>
#include <alibabacloud/oss/model/SetBucketWebsiteRequest.h>
#include <alibabacloud/oss/model/SetBucketRefererRequest.h>
#include <alibabacloud/oss/model/SetBucketLifecycleRequest.h>
#include <alibabacloud/oss/model/SetBucketCorsRequest.h>
#include <alibabacloud/oss/model/SetBucketStorageCapacityRequest.h>
#include <alibabacloud/oss/model/DeleteBucketRequest.h>
#include <alibabacloud/oss/model/DeleteBucketLoggingRequest.h>
#include <alibabacloud/oss/model/DeleteBucketWebsiteRequest.h>
#include <alibabacloud/oss/model/DeleteBucketLifecycleRequest.h>
#include <alibabacloud/oss/model/DeleteBucketCorsRequest.h>
#include <alibabacloud/oss/model/GetBucketAclRequest.h>
#include <alibabacloud/oss/model/GetBucketAclResult.h>
#include <alibabacloud/oss/model/GetBucketLocationRequest.h>
#include <alibabacloud/oss/model/GetBucketLocationResult.h>
#include <alibabacloud/oss/model/GetBucketInfoRequest.h>
#include <alibabacloud/oss/model/GetBucketInfoResult.h>
#include <alibabacloud/oss/model/GetBucketLoggingRequest.h>
#include <alibabacloud/oss/model/GetBucketLoggingResult.h>
#include <alibabacloud/oss/model/GetBucketWebsiteRequest.h>
#include <alibabacloud/oss/model/GetBucketWebsiteResult.h>
#include <alibabacloud/oss/model/GetBucketRefererRequest.h>
#include <alibabacloud/oss/model/GetBucketRefererResult.h>
#include <alibabacloud/oss/model/GetBucketLifecycleRequest.h>
#include <alibabacloud/oss/model/GetBucketLifecycleResult.h>
#include <alibabacloud/oss/model/GetBucketStatRequest.h>
#include <alibabacloud/oss/model/GetBucketStatResult.h>
#include <alibabacloud/oss/model/GetBucketCorsRequest.h>
#include <alibabacloud/oss/model/GetBucketCorsResult.h>
#include <alibabacloud/oss/model/GetBucketStorageCapacityRequest.h>
#include <alibabacloud/oss/model/GetBucketStorageCapacityResult.h>
#include <alibabacloud/oss/model/SetBucketPolicyRequest.h>
#include <alibabacloud/oss/model/GetBucketPolicyRequest.h>
#include <alibabacloud/oss/model/GetBucketPolicyResult.h>
#include <alibabacloud/oss/model/DeleteBucketPolicyRequest.h>
#include <alibabacloud/oss/model/SetBucketPaymentRequest.h>
#include <alibabacloud/oss/model/GetBucketPaymentRequest.h>
#include <alibabacloud/oss/model/GetBucketPaymentResult.h>
#include <alibabacloud/oss/model/SetBucketEncryptionRequest.h>
#include <alibabacloud/oss/model/DeleteBucketEncryptionRequest.h>
#include <alibabacloud/oss/model/GetBucketEncryptionRequest.h>
#include <alibabacloud/oss/model/GetBucketEncryptionResult.h>
#include <alibabacloud/oss/model/SetBucketTaggingRequest.h>
#include <alibabacloud/oss/model/GetBucketTaggingRequest.h>
#include <alibabacloud/oss/model/GetBucketTaggingResult.h>
#include <alibabacloud/oss/model/DeleteBucketTaggingRequest.h>
#include <alibabacloud/oss/model/SetBucketQosInfoRequest.h>
#include <alibabacloud/oss/model/DeleteBucketQosInfoRequest.h>
#include <alibabacloud/oss/model/GetBucketQosInfoRequest.h>
#include <alibabacloud/oss/model/GetBucketQosInfoResult.h>
#include <alibabacloud/oss/model/GetUserQosInfoRequest.h>
#include <alibabacloud/oss/model/GetUserQosInfoResult.h>
#include <alibabacloud/oss/model/SetBucketVersioningRequest.h>
#include <alibabacloud/oss/model/GetBucketVersioningRequest.h>
#include <alibabacloud/oss/model/GetBucketVersioningResult.h>
#include <alibabacloud/oss/model/SetBucketInventoryConfigurationRequest.h>
#include <alibabacloud/oss/model/DeleteBucketInventoryConfigurationRequest.h>
#include <alibabacloud/oss/model/GetBucketInventoryConfigurationResult.h>
#include <alibabacloud/oss/model/GetBucketInventoryConfigurationRequest.h>
#include <alibabacloud/oss/model/ListBucketInventoryConfigurationsRequest.h>
#include <alibabacloud/oss/model/ListBucketInventoryConfigurationsResult.h>
#endif

#include <alibabacloud/oss/model/ListObjectsRequest.h>
#include <alibabacloud/oss/model/ListObjectsResult.h>
#include <alibabacloud/oss/model/ListObjectVersionsRequest.h>
#include <alibabacloud/oss/model/ListObjectVersionsResult.h>
#include <alibabacloud/oss/model/GetObjectRequest.h>
#include <alibabacloud/oss/model/GetObjectResult.h>
#include <alibabacloud/oss/model/PutObjectRequest.h>
#include <alibabacloud/oss/model/PutObjectResult.h>
#include <alibabacloud/oss/model/DeleteObjectRequest.h>
#include <alibabacloud/oss/model/DeleteObjectResult.h>
#include <alibabacloud/oss/model/DeleteObjectsRequest.h>
#include <alibabacloud/oss/model/DeleteObjectsResult.h>
#include <alibabacloud/oss/model/DeleteObjectVersionsRequest.h>
#include <alibabacloud/oss/model/DeleteObjectVersionsResult.h>
#include <alibabacloud/oss/model/HeadObjectRequest.h>
#include <alibabacloud/oss/model/GetObjectMetaRequest.h>
#include <alibabacloud/oss/model/GeneratePresignedUrlRequest.h>
#include <alibabacloud/oss/model/GetObjectByUrlRequest.h>
#include <alibabacloud/oss/model/PutObjectByUrlRequest.h>
#include <alibabacloud/oss/model/GetObjectAclRequest.h>
#include <alibabacloud/oss/model/GetObjectAclResult.h>
#include <alibabacloud/oss/model/AppendObjectRequest.h>
#include <alibabacloud/oss/model/AppendObjectResult.h>
#include <alibabacloud/oss/model/CopyObjectRequest.h>
#include <alibabacloud/oss/model/CopyObjectResult.h>
#include <alibabacloud/oss/model/GetSymlinkRequest.h>
#include <alibabacloud/oss/model/GetSymlinkResult.h>
#include <alibabacloud/oss/model/RestoreObjectRequest.h>
#include <alibabacloud/oss/model/RestoreObjectResult.h>
#include <alibabacloud/oss/model/CreateSymlinkRequest.h>
#include <alibabacloud/oss/model/CreateSymlinkResult.h>
#include <alibabacloud/oss/model/SetObjectAclRequest.h>
#include <alibabacloud/oss/model/SetObjectAclResult.h>
#include <alibabacloud/oss/model/ProcessObjectRequest.h>
#include <alibabacloud/oss/model/ObjectCallbackBuilder.h>
#include <alibabacloud/oss/model/SelectObjectRequest.h>
#include <alibabacloud/oss/model/CreateSelectObjectMetaRequest.h>
#include <alibabacloud/oss/model/CreateSelectObjectMetaResult.h>
#include <alibabacloud/oss/model/SetObjectTaggingRequest.h>
#include <alibabacloud/oss/model/SetObjectTaggingResult.h>
#include <alibabacloud/oss/model/GetObjectTaggingRequest.h>
#include <alibabacloud/oss/model/GetObjectTaggingResult.h>
#include <alibabacloud/oss/model/DeleteObjectTaggingRequest.h>
#include <alibabacloud/oss/model/DeleteObjectTaggingResult.h>

#include <alibabacloud/oss/model/InitiateMultipartUploadRequest.h>
#include <alibabacloud/oss/model/InitiateMultipartUploadResult.h>
#include <alibabacloud/oss/model/UploadPartRequest.h>
#include <alibabacloud/oss/model/UploadPartCopyRequest.h>
#include <alibabacloud/oss/model/UploadPartCopyResult.h>
#include <alibabacloud/oss/model/CompleteMultipartUploadRequest.h>
#include <alibabacloud/oss/model/CompleteMultipartUploadResult.h>
#include <alibabacloud/oss/model/AbortMultipartUploadRequest.h>
#include <alibabacloud/oss/model/ListMultipartUploadsRequest.h>
#include <alibabacloud/oss/model/ListMultipartUploadsResult.h>
#include <alibabacloud/oss/model/ListPartsRequest.h>
#include <alibabacloud/oss/model/ListPartsResult.h>

#if !defined(OSS_DISABLE_RESUAMABLE)
#include <alibabacloud/oss/model/UploadObjectRequest.h>
#include <alibabacloud/oss/model/MultiCopyObjectRequest.h>
#include <alibabacloud/oss/model/DownloadObjectRequest.h>
#endif

#if !defined(OSS_DISABLE_LIVECHANNEL)
#include <alibabacloud/oss/model/PutLiveChannelStatusRequest.h>
#include <alibabacloud/oss/model/PutLiveChannelRequest.h>
#include <alibabacloud/oss/model/PutLiveChannelResult.h>
#include <alibabacloud/oss/model/PostVodPlaylistRequest.h>
#include <alibabacloud/oss/model/GetVodPlaylistRequest.h>
#include <alibabacloud/oss/model/GetVodPlaylistResult.h>
#include <alibabacloud/oss/model/GetLiveChannelStatRequest.h>
#include <alibabacloud/oss/model/GetLiveChannelStatResult.h>
#include <alibabacloud/oss/model/GetLiveChannelInfoRequest.h>
#include <alibabacloud/oss/model/GetLiveChannelInfoResult.h>
#include <alibabacloud/oss/model/GetLiveChannelHistoryRequest.h>
#include <alibabacloud/oss/model/GetLiveChannelHistoryResult.h>
#include <alibabacloud/oss/model/ListLiveChannelRequest.h>
#include <alibabacloud/oss/model/ListLiveChannelResult.h>
#include <alibabacloud/oss/model/DeleteLiveChannelRequest.h>
#include <alibabacloud/oss/model/GenerateRTMPSignedUrlRequest.h>
#endif

namespace AlibabaCloud
{
namespace OSS
{
    using OssOutcome = Outcome<OssError, ServiceResult>;
    using VoidOutcome = Outcome<OssError, VoidResult>;
    using StringOutcome = Outcome<OssError, std::string>;

#if !defined(OSS_DISABLE_BUCKET)
    using ListBucketsOutcome = Outcome<OssError, ListBucketsResult>;
    using CreateBucketOutcome = Outcome<OssError, Bucket>;
    using ListBucketInventoryConfigurationsOutcome = Outcome<OssError, ListBucketInventoryConfigurationsResult>;

    using GetBucketAclOutcome = Outcome<OssError, GetBucketAclResult>;
    using GetBucketLocationOutcome = Outcome<OssError, GetBucketLocationResult>;
    using GetBucketInfoOutcome = Outcome<OssError, GetBucketInfoResult>;
    using GetBucketLoggingOutcome = Outcome<OssError, GetBucketLoggingResult>;
    using GetBucketWebsiteOutcome = Outcome<OssError, GetBucketWebsiteResult>;
    using GetBucketRefererOutcome = Outcome<OssError, GetBucketRefererResult>;
    using GetBucketLifecycleOutcome = Outcome<OssError, GetBucketLifecycleResult>;
    using GetBucketStatOutcome = Outcome<OssError, GetBucketStatResult>;
    using GetBucketCorsOutcome = Outcome<OssError, GetBucketCorsResult>;
    using GetBucketStorageCapacityOutcome = Outcome<OssError, GetBucketStorageCapacityResult>;
    using GetBucketPolicyOutcome = Outcome<OssError, GetBucketPolicyResult>;
    using GetBucketPaymentOutcome = Outcome<OssError, GetBucketPaymentResult>;
    using GetBucketEncryptionOutcome = Outcome<OssError, GetBucketEncryptionResult>;
    using GetBucketTaggingOutcome = Outcome<OssError, GetBucketTaggingResult>;
    using GetBucketQosInfoOutcome = Outcome<OssError, GetBucketQosInfoResult>;
    using GetUserQosInfoOutcome = Outcome<OssError, GetUserQosInfoResult>;
    using GetBucketVersioningOutcome = Outcome<OssError, GetBucketVersioningResult>;
    using GetBucketInventoryConfigurationOutcome = Outcome<OssError, GetBucketInventoryConfigurationResult>;
#endif

    using ListObjectOutcome = Outcome<OssError, ListObjectsResult>;
    using ListObjectVersionsOutcome = Outcome<OssError, ListObjectVersionsResult>;

    using GetObjectOutcome = Outcome<OssError, GetObjectResult>;
    using PutObjectOutcome = Outcome<OssError, PutObjectResult>;
    using DeleteObjectOutcome = Outcome<OssError, DeleteObjectResult>;
    using DeleteObjecstOutcome = Outcome<OssError, DeleteObjectsResult>;
    using DeleteObjecVersionstOutcome = Outcome<OssError, DeleteObjectVersionsResult>;
    using ObjectMetaDataOutcome = Outcome<OssError, ObjectMetaData>;

    using GetObjectAclOutcome = Outcome<OssError, GetObjectAclResult>;
    using SetObjectAclOutcome = Outcome<OssError, SetObjectAclResult>;
    using AppendObjectOutcome = Outcome<OssError, AppendObjectResult>;
    using CopyObjectOutcome = Outcome<OssError, CopyObjectResult>;
    using RestoreObjectOutcome = Outcome<OssError, RestoreObjectResult>;
    using GetSymlinkOutcome = Outcome<OssError, GetSymlinkResult>;
    using CreateSymlinkOutcome = Outcome<OssError, CreateSymlinkResult>;
    using CreateSelectObjectMetaOutcome = Outcome<OssError, CreateSelectObjectMetaResult>;
    using SetObjectTaggingOutcome = Outcome<OssError, SetObjectTaggingResult>;
    using GetObjectTaggingOutcome = Outcome<OssError, GetObjectTaggingResult>;
    using DeleteObjectTaggingOutcome = Outcome<OssError, DeleteObjectTaggingResult>;

    /*multipart*/
    using InitiateMultipartUploadOutcome = Outcome<OssError, InitiateMultipartUploadResult>;
    using UploadPartCopyOutcome = Outcome<OssError, UploadPartCopyResult>;
    using CompleteMultipartUploadOutcome = Outcome<OssError, CompleteMultipartUploadResult>;
    using ListMultipartUploadsOutcome = Outcome<OssError, ListMultipartUploadsResult>;
    using ListPartsOutcome = Outcome<OssError, ListPartsResult>;

#if !defined(OSS_DISABLE_LIVECHANNEL)
    /*livechannel*/
    using PutLiveChannelOutcome = Outcome<OssError, PutLiveChannelResult>;
    using GetLiveChannelStatOutcome = Outcome<OssError, GetLiveChannelStatResult>;
    using GetLiveChannelInfoOutcome = Outcome<OssError, GetLiveChannelInfoResult>;
    using GetLiveChannelHistoryOutcome = Outcome<OssError, GetLiveChannelHistoryResult>;
    using ListLiveChannelOutcome = Outcome<OssError, ListLiveChannelResult>;
    using GetVodPlaylistOutcome = Outcome<OssError, GetVodPlaylistResult>;
#endif
}
}

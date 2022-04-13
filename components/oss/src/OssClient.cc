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

#include <alibabacloud/oss/OssClient.h>
#include "auth/SimpleCredentialsProvider.h"
#include "http/CurlHttpClient.h"
#include "OssClientImpl.h"
#include <fstream>
#include "utils/LogUtils.h"
#include "utils/Crc64.h"

using namespace AlibabaCloud::OSS;

static bool SdkInitDone = false;

bool AlibabaCloud::OSS::IsSdkInitialized()
{
    return SdkInitDone;
}

void AlibabaCloud::OSS::InitializeSdk()
{
    if (IsSdkInitialized())
        return;
    InitLogInner();
    CurlHttpClient::initGlobalState();
    SdkInitDone = true;
}

void AlibabaCloud::OSS::ShutdownSdk()
{
    if (!IsSdkInitialized())
        return;
    
    CurlHttpClient::cleanupGlobalState();
    DeinitLogInner();
    SdkInitDone = false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

void AlibabaCloud::OSS::SetLogLevel(LogLevel level)
{
    SetLogLevelInner(level);
}

void AlibabaCloud::OSS::SetLogCallback(LogCallback callback)
{
    SetLogCallbackInner(callback);
}
////////////////////////////////////////////////////////////////////////////////////////////////////

uint64_t AlibabaCloud::OSS::ComputeCRC64(uint64_t crc, void *buf, size_t len)
{
    return CRC64::CalcCRC(crc, buf, len);
}

uint64_t AlibabaCloud::OSS::CombineCRC64(uint64_t crc1, uint64_t crc2, uintmax_t len2)
{
    return CRC64::CombineCRC(crc1, crc2, len2);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

OssClient::OssClient(const std::string &endpoint, const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
    OssClient(endpoint, accessKeyId, accessKeySecret, "", configuration)
{
}

OssClient::OssClient(const std::string &endpoint, const std::string & accessKeyId, const std::string & accessKeySecret, const std::string & securityToken,
    const ClientConfiguration & configuration) :
    OssClient(endpoint, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret, securityToken), configuration)
{
}

OssClient::OssClient(const std::string &endpoint, const Credentials &credentials, const ClientConfiguration &configuration) :
    OssClient(endpoint, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
}

OssClient::OssClient(const std::string &endpoint, const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
    client_(std::make_shared<OssClientImpl>(endpoint, credentialsProvider, configuration))
{
}

OssClient::~OssClient()
{
}

#if !defined(OSS_DISABLE_BUCKET)

ListBucketsOutcome OssClient::ListBuckets() const
{
    return client_->ListBuckets(ListBucketsRequest());
}

ListBucketsOutcome OssClient::ListBuckets(const ListBucketsRequest &request) const
{
    return client_->ListBuckets(request);
}

ListBucketInventoryConfigurationsOutcome OssClient::ListBucketInventoryConfigurations(const ListBucketInventoryConfigurationsRequest& request) const
{
    return client_->ListBucketInventoryConfigurations(request);
}

CreateBucketOutcome OssClient::CreateBucket(const std::string &bucket, StorageClass storageClass) const
{
    return client_->CreateBucket(CreateBucketRequest(bucket, storageClass));
}

CreateBucketOutcome OssClient::CreateBucket(const std::string &bucket, StorageClass storageClass, CannedAccessControlList acl) const
{
    return client_->CreateBucket(CreateBucketRequest(bucket, storageClass, acl));
}

CreateBucketOutcome OssClient::CreateBucket(const CreateBucketRequest &request) const
{
    return client_->CreateBucket(request);
}

VoidOutcome OssClient::SetBucketAcl(const std::string &bucket, CannedAccessControlList acl) const
{
    return client_->SetBucketAcl(SetBucketAclRequest(bucket, acl));
}

VoidOutcome OssClient::SetBucketAcl(const SetBucketAclRequest& request) const
{
    return client_->SetBucketAcl(request);
}

VoidOutcome OssClient::SetBucketLogging(const std::string &bucket, const std::string &targetBucket, const std::string &targetPrefix) const
{
    return client_->SetBucketLogging(SetBucketLoggingRequest(bucket, targetBucket, targetPrefix));
}

VoidOutcome OssClient::SetBucketLogging(const SetBucketLoggingRequest& request) const
{
    return client_->SetBucketLogging(request);
}

VoidOutcome OssClient::SetBucketWebsite(const std::string &bucket, const std::string &indexDocument) const
{
    SetBucketWebsiteRequest request(bucket);
    request.setIndexDocument(indexDocument);
    return client_->SetBucketWebsite(request);
}

VoidOutcome OssClient::SetBucketWebsite(const std::string &bucket, const std::string &indexDocument, const std::string &errorDocument) const
{
    SetBucketWebsiteRequest request(bucket);
    request.setIndexDocument(indexDocument);
    request.setErrorDocument(errorDocument);
    return client_->SetBucketWebsite(request);
}

VoidOutcome OssClient::SetBucketWebsite(const SetBucketWebsiteRequest& request) const
{
    return client_->SetBucketWebsite(request);
}

VoidOutcome OssClient::SetBucketReferer(const std::string &bucket, const RefererList &refererList, bool allowEmptyReferer) const
{
    return client_->SetBucketReferer(SetBucketRefererRequest(bucket, refererList, allowEmptyReferer));
}

VoidOutcome OssClient::SetBucketReferer(const SetBucketRefererRequest& request) const
{
    return client_->SetBucketReferer(request);
}

VoidOutcome OssClient::SetBucketLifecycle(const SetBucketLifecycleRequest& request) const
{
    return client_->SetBucketLifecycle(request);
}

VoidOutcome OssClient::SetBucketCors(const std::string &bucket, const CORSRuleList &rules) const
{
    SetBucketCorsRequest request(bucket);
    request.setCORSRules(rules);
    return client_->SetBucketCors(request);
}

VoidOutcome OssClient::SetBucketCors(const SetBucketCorsRequest& request) const
{
    return client_->SetBucketCors(request);
}

VoidOutcome OssClient::SetBucketStorageCapacity(const std::string &bucket, int64_t storageCapacity) const
{
    return client_->SetBucketStorageCapacity(SetBucketStorageCapacityRequest(bucket, storageCapacity));
}

VoidOutcome OssClient::SetBucketStorageCapacity(const SetBucketStorageCapacityRequest& request) const
{
    return client_->SetBucketStorageCapacity(request);
}

VoidOutcome OssClient::SetBucketPolicy(const SetBucketPolicyRequest& request) const
{
    return client_->SetBucketPolicy(request);
}
VoidOutcome OssClient::SetBucketRequestPayment(const SetBucketRequestPaymentRequest& request) const
{
    return client_->SetBucketRequestPayment(request);
}

VoidOutcome OssClient::SetBucketEncryption(const SetBucketEncryptionRequest& request) const
{
    return client_->SetBucketEncryption(request);
}

VoidOutcome OssClient::SetBucketTagging(const SetBucketTaggingRequest& request) const
{
    return client_->SetBucketTagging(request);
}

VoidOutcome OssClient::SetBucketQosInfo(const SetBucketQosInfoRequest& request) const
{
    return client_->SetBucketQosInfo(request);
}

VoidOutcome OssClient::DeleteBucketPolicy(const DeleteBucketPolicyRequest& request) const
{
    return client_->DeleteBucketPolicy(request);
}

VoidOutcome OssClient::SetBucketVersioning(const SetBucketVersioningRequest& request) const
{
    return client_->SetBucketVersioning(request);
}

VoidOutcome OssClient::SetBucketInventoryConfiguration(const SetBucketInventoryConfigurationRequest& request) const
{
    return client_->SetBucketInventoryConfiguration(request);
}

VoidOutcome OssClient::DeleteBucket(const std::string &bucket) const
{
    return client_->DeleteBucket(DeleteBucketRequest(bucket));
}

VoidOutcome OssClient::DeleteBucket(const DeleteBucketRequest &request) const
{
    return client_->DeleteBucket(request);
}

VoidOutcome OssClient::DeleteBucketLogging(const std::string &bucket) const
{
    return client_->DeleteBucketLogging(DeleteBucketLoggingRequest(bucket));
}

VoidOutcome OssClient::DeleteBucketLogging(const DeleteBucketLoggingRequest& request) const
{
    return client_->DeleteBucketLogging(request);
}

VoidOutcome OssClient::DeleteBucketWebsite(const std::string &bucket) const
{
    return client_->DeleteBucketWebsite(DeleteBucketWebsiteRequest(bucket));
}

VoidOutcome OssClient::DeleteBucketWebsite(const DeleteBucketWebsiteRequest& request) const
{
    return client_->DeleteBucketWebsite(request);
}

VoidOutcome OssClient::DeleteBucketLifecycle(const std::string &bucket) const
{
    return client_->DeleteBucketLifecycle(DeleteBucketLifecycleRequest(bucket));
}

VoidOutcome OssClient::DeleteBucketLifecycle(const DeleteBucketLifecycleRequest& request) const
{
    return client_->DeleteBucketLifecycle(request);
}

VoidOutcome OssClient::DeleteBucketCors(const std::string &bucket) const
{
    return client_->DeleteBucketCors(DeleteBucketCorsRequest(bucket));
}

VoidOutcome OssClient::DeleteBucketCors(const DeleteBucketCorsRequest& request) const
{
    return client_->DeleteBucketCors(request);
}

VoidOutcome OssClient::DeleteBucketEncryption(const DeleteBucketEncryptionRequest& request) const
{
    return client_->DeleteBucketEncryption(request);
}

VoidOutcome OssClient::DeleteBucketTagging(const DeleteBucketTaggingRequest& request) const
{
    return client_->DeleteBucketTagging(request);
}

VoidOutcome OssClient::DeleteBucketQosInfo(const DeleteBucketQosInfoRequest& request) const
{
    return client_->DeleteBucketQosInfo(request);
}

VoidOutcome OssClient::DeleteBucketInventoryConfiguration(const DeleteBucketInventoryConfigurationRequest& request) const
{
    return client_->DeleteBucketInventoryConfiguration(request);
}

GetBucketAclOutcome OssClient::GetBucketAcl(const std::string &bucket) const
{
    return client_->GetBucketAcl(GetBucketAclRequest(bucket));
}

GetBucketAclOutcome OssClient::GetBucketAcl(const GetBucketAclRequest &request) const
{
    return client_->GetBucketAcl(request);
}

GetBucketLocationOutcome OssClient::GetBucketLocation(const std::string &bucket) const
{
    return client_->GetBucketLocation(GetBucketLocationRequest(bucket));
}

GetBucketLocationOutcome OssClient::GetBucketLocation(const GetBucketLocationRequest &request) const
{
    return client_->GetBucketLocation(request);
}

GetBucketInfoOutcome OssClient::GetBucketInfo(const std::string &bucket) const
{
    return client_->GetBucketInfo(GetBucketInfoRequest(bucket));
}

GetBucketInfoOutcome  OssClient::GetBucketInfo(const  GetBucketInfoRequest &request) const
{
    return client_->GetBucketInfo(request);
}

GetBucketLoggingOutcome OssClient::GetBucketLogging(const std::string &bucket) const
{
    return client_->GetBucketLogging(GetBucketLoggingRequest(bucket));
}

GetBucketLoggingOutcome OssClient::GetBucketLogging(const GetBucketLoggingRequest &request) const
{
    return client_->GetBucketLogging(request);
}

GetBucketWebsiteOutcome OssClient::GetBucketWebsite(const std::string &bucket) const
{
    return client_->GetBucketWebsite(GetBucketWebsiteRequest(bucket));
}

GetBucketWebsiteOutcome OssClient::GetBucketWebsite(const GetBucketWebsiteRequest &request) const
{
    return client_->GetBucketWebsite(request);
}

GetBucketRefererOutcome OssClient::GetBucketReferer(const std::string &bucket) const
{
    return client_->GetBucketReferer(GetBucketRefererRequest(bucket));
}

GetBucketRefererOutcome OssClient::GetBucketReferer(const GetBucketRefererRequest &request) const
{
    return client_->GetBucketReferer(request);
}

GetBucketLifecycleOutcome OssClient::GetBucketLifecycle(const std::string &bucket) const
{
    return client_->GetBucketLifecycle(GetBucketLifecycleRequest(bucket));
}

GetBucketLifecycleOutcome OssClient::GetBucketLifecycle(const GetBucketLifecycleRequest &request) const
{
    return client_->GetBucketLifecycle(request);
}

GetBucketStatOutcome OssClient::GetBucketStat(const std::string &bucket) const
{
    return client_->GetBucketStat(GetBucketStatRequest(bucket));
}

GetBucketStatOutcome OssClient::GetBucketStat(const GetBucketStatRequest &request) const
{
    return client_->GetBucketStat(request);
}

GetBucketCorsOutcome OssClient::GetBucketCors(const std::string &bucket) const
{
    return client_->GetBucketCors(GetBucketCorsRequest(bucket));
}

GetBucketCorsOutcome OssClient::GetBucketCors(const GetBucketCorsRequest &request) const
{
    return client_->GetBucketCors(request);
}

GetBucketStorageCapacityOutcome OssClient::GetBucketStorageCapacity(const std::string &bucket) const
{
    return client_->GetBucketStorageCapacity(GetBucketStorageCapacityRequest(bucket));
}

GetBucketStorageCapacityOutcome OssClient::GetBucketStorageCapacity(const GetBucketStorageCapacityRequest& request) const
{
    return client_->GetBucketStorageCapacity(request);
}

GetBucketPolicyOutcome OssClient::GetBucketPolicy(const GetBucketPolicyRequest& request) const
{
    return client_->GetBucketPolicy(request);
}
GetBucketPaymentOutcome OssClient::GetBucketRequestPayment(const GetBucketRequestPaymentRequest& request) const
{
    return client_->GetBucketRequestPayment(request);
}

GetBucketEncryptionOutcome OssClient::GetBucketEncryption(const GetBucketEncryptionRequest& request) const
{
    return client_->GetBucketEncryption(request);
}

GetBucketTaggingOutcome OssClient::GetBucketTagging(const GetBucketTaggingRequest& request) const
{
    return client_->GetBucketTagging(request);
}

GetBucketQosInfoOutcome OssClient::GetBucketQosInfo(const GetBucketQosInfoRequest& request) const
{
    return client_->GetBucketQosInfo(request);
}

GetUserQosInfoOutcome OssClient::GetUserQosInfo(const GetUserQosInfoRequest& request) const
{
    return client_->GetUserQosInfo(request);
}

GetBucketVersioningOutcome OssClient::GetBucketVersioning(const GetBucketVersioningRequest& request) const
{
    return client_->GetBucketVersioning(request);
}

GetBucketInventoryConfigurationOutcome OssClient::GetBucketInventoryConfiguration(const GetBucketInventoryConfigurationRequest& request) const
{
    return client_->GetBucketInventoryConfiguration(request);
}

#endif

ListObjectOutcome OssClient::ListObjects(const std::string &bucket) const
{
    return client_->ListObjects(ListObjectsRequest(bucket));
}

ListObjectOutcome OssClient::ListObjects(const std::string &bucket, const std::string &prefix) const
{
    ListObjectsRequest request(bucket);
    request.setPrefix(prefix);
    return client_->ListObjects(request);
}

ListObjectOutcome OssClient::ListObjects(const ListObjectsRequest &request) const
{
    return client_->ListObjects(request);
}

ListObjectVersionsOutcome OssClient::ListObjectVersions(const std::string &bucket) const
{
    return client_->ListObjectVersions(ListObjectVersionsRequest(bucket));
}

ListObjectVersionsOutcome OssClient::ListObjectVersions(const std::string &bucket, const std::string &prefix) const
{
    ListObjectVersionsRequest request(bucket);
    request.setPrefix(prefix);
    return client_->ListObjectVersions(request);
}

ListObjectVersionsOutcome OssClient::ListObjectVersions(const ListObjectVersionsRequest& request) const
{
    return client_->ListObjectVersions(request);
}

GetObjectOutcome OssClient::GetObject(const std::string &bucket, const std::string &key) const
{
    return client_->GetObject(GetObjectRequest(bucket, key));
}

GetObjectOutcome OssClient::GetObject(const std::string &bucket, const std::string &key, const std::shared_ptr<std::iostream> &content) const
{
    GetObjectRequest request(bucket, key);
    request.setResponseStreamFactory([=]() { return content; });
    return client_->GetObject(request);
}

GetObjectOutcome OssClient::GetObject(const std::string &bucket, const std::string &key, const std::string &fileToSave) const
{
    GetObjectRequest request(bucket, key);
    request.setResponseStreamFactory([=]() {return std::make_shared<std::fstream>(fileToSave, std::ios_base::out | std::ios_base::trunc | std::ios::binary); });
    return client_->GetObject(request);
}

GetObjectOutcome OssClient::GetObject(const GetObjectRequest &request) const
{
    return client_->GetObject(request);
}

PutObjectOutcome OssClient::PutObject(const std::string &bucket, const std::string &key, const std::shared_ptr<std::iostream> &content) const
{
    return client_->PutObject(PutObjectRequest(bucket, key, content));
}

PutObjectOutcome OssClient::PutObject(const std::string &bucket, const std::string &key, const std::string &fileToUpload) const
{
    std::shared_ptr<std::iostream> content = std::make_shared<std::fstream>(fileToUpload, std::ios::in|std::ios::binary);
    return client_->PutObject(PutObjectRequest(bucket, key, content));
}

PutObjectOutcome OssClient::PutObject(const std::string &bucket, const std::string &key, const std::shared_ptr<std::iostream> &content, const ObjectMetaData &meta) const
{
    return client_->PutObject(PutObjectRequest(bucket, key, content, meta));
}

PutObjectOutcome OssClient::PutObject(const std::string &bucket, const std::string &key, const std::string &fileToUpload, const ObjectMetaData &meta) const
{
    std::shared_ptr<std::iostream> content = std::make_shared<std::fstream>(fileToUpload, std::ios::in | std::ios::binary);
    return client_->PutObject(PutObjectRequest(bucket, key, content, meta));
}

PutObjectOutcome OssClient::PutObject(const PutObjectRequest &request) const
{
    return client_->PutObject(request);
}

DeleteObjectOutcome OssClient::DeleteObject(const std::string &bucket, const std::string &key) const
{
    return client_->DeleteObject(DeleteObjectRequest(bucket, key));
}

DeleteObjectOutcome OssClient::DeleteObject(const DeleteObjectRequest &request) const
{
    return client_->DeleteObject(request);
}

DeleteObjecstOutcome OssClient::DeleteObjects(const std::string bucket, const DeletedKeyList &keyList) const
{
    DeleteObjectsRequest request(bucket);
    request.setKeyList(keyList);
    return client_->DeleteObjects(request);
}

DeleteObjecstOutcome OssClient::DeleteObjects(const DeleteObjectsRequest &request) const
{
    return client_->DeleteObjects(request);
}

DeleteObjecVersionstOutcome OssClient::DeleteObjectVersions(const std::string bucket, const ObjectIdentifierList &objectList) const
{
    DeleteObjectVersionsRequest request(bucket);
    request.setObjects(objectList);
    return client_->DeleteObjectVersions(request);
}

DeleteObjecVersionstOutcome OssClient::DeleteObjectVersions(const DeleteObjectVersionsRequest &request) const
{
    return client_->DeleteObjectVersions(request);
}

ObjectMetaDataOutcome OssClient::HeadObject(const std::string &bucket, const std::string &key) const
{
    return client_->HeadObject(HeadObjectRequest(bucket, key));
}

ObjectMetaDataOutcome OssClient::HeadObject(const HeadObjectRequest &request) const
{
    return client_->HeadObject(request);
}

ObjectMetaDataOutcome OssClient::GetObjectMeta(const std::string &bucket, const std::string &key) const
{
    return client_->GetObjectMeta(GetObjectMetaRequest(bucket, key));
}

ObjectMetaDataOutcome OssClient::GetObjectMeta(const GetObjectMetaRequest &request) const
{
    return client_->GetObjectMeta(request);
}

GetObjectAclOutcome OssClient::GetObjectAcl(const GetObjectAclRequest &request) const
{
    return client_->GetObjectAcl(request);
}

AppendObjectOutcome OssClient::AppendObject(const AppendObjectRequest &request) const
{
    return client_->AppendObject(request);
}

CopyObjectOutcome OssClient::CopyObject(const CopyObjectRequest &request) const
{
    return client_->CopyObject(request);
}

GetSymlinkOutcome OssClient::GetSymlink(const GetSymlinkRequest &request) const
{
    return client_->GetSymlink(request);
}

GetObjectOutcome OssClient::ProcessObject(const ProcessObjectRequest &request) const
{
    return client_->ProcessObject(request);
}

RestoreObjectOutcome OssClient::RestoreObject(const std::string &bucket, const std::string &key) const
{
    return client_->RestoreObject(RestoreObjectRequest(bucket, key));
}

RestoreObjectOutcome OssClient::RestoreObject(const RestoreObjectRequest &request) const
{
    return client_->RestoreObject(request);
}

CreateSymlinkOutcome OssClient::CreateSymlink(const CreateSymlinkRequest &request) const
{
    return client_->CreateSymlink(request);
}

SetObjectAclOutcome OssClient::SetObjectAcl(const SetObjectAclRequest &request) const
{
    return client_->SetObjectAcl(request);
}

GetObjectOutcome OssClient::SelectObject(const SelectObjectRequest &request) const
{
    return client_->SelectObject(request);
}

CreateSelectObjectMetaOutcome OssClient::CreateSelectObjectMeta(const CreateSelectObjectMetaRequest &request) const
{
    return client_->CreateSelectObjectMeta(request);
}

SetObjectTaggingOutcome OssClient::SetObjectTagging(const SetObjectTaggingRequest& request) const
{
    return client_->SetObjectTagging(request);
}

DeleteObjectTaggingOutcome OssClient::DeleteObjectTagging(const DeleteObjectTaggingRequest& request) const
{
    return client_->DeleteObjectTagging(request);
}

GetObjectTaggingOutcome OssClient::GetObjectTagging(const GetObjectTaggingRequest& request) const
{
    return client_->GetObjectTagging(request);
}

StringOutcome OssClient::GeneratePresignedUrl(const GeneratePresignedUrlRequest &request) const
{
    return client_->GeneratePresignedUrl(request);
}

StringOutcome OssClient::GeneratePresignedUrl(const std::string &bucket, const std::string &key) const
{
    return GeneratePresignedUrl(GeneratePresignedUrlRequest(bucket, key));
}

StringOutcome OssClient::GeneratePresignedUrl(const std::string &bucket, const std::string &key, int64_t expires) const
{
    GeneratePresignedUrlRequest request(bucket, key);
    request.setExpires(expires);
    return GeneratePresignedUrl(request);
}

StringOutcome OssClient::GeneratePresignedUrl(const std::string &bucket, const std::string &key, int64_t expires, Http::Method method) const
{
    GeneratePresignedUrlRequest request(bucket, key, method);
    request.setExpires(expires);
    return GeneratePresignedUrl(request);
}

GetObjectOutcome OssClient::GetObjectByUrl(const GetObjectByUrlRequest &request) const
{
    return client_->GetObjectByUrl(request);
}

GetObjectOutcome OssClient::GetObjectByUrl(const std::string &url) const
{
    return client_->GetObjectByUrl(GetObjectByUrlRequest(url));
}

GetObjectOutcome OssClient::GetObjectByUrl(const std::string &url, const std::string &file) const
{
    GetObjectByUrlRequest request(url);
    request.setResponseStreamFactory([=]() {return std::make_shared<std::fstream>(file, std::ios_base::out | std::ios_base::in | std::ios_base::trunc); });
    return client_->GetObjectByUrl(request);
}

PutObjectOutcome OssClient::PutObjectByUrl(const PutObjectByUrlRequest &request) const
{
    return client_->PutObjectByUrl(request);
}

PutObjectOutcome OssClient::PutObjectByUrl(const std::string &url, const std::string &file) const
{
    std::shared_ptr<std::iostream> content = std::make_shared<std::fstream>(file, std::ios::in|std::ios::binary);
    return client_->PutObjectByUrl(PutObjectByUrlRequest(url, content));
}

PutObjectOutcome OssClient::PutObjectByUrl(const std::string &url, const std::string &file, const ObjectMetaData &metaData) const
{
    std::shared_ptr<std::iostream> content = std::make_shared<std::fstream>(file, std::ios::in | std::ios::binary);
    return client_->PutObjectByUrl(PutObjectByUrlRequest(url, content, metaData));
}

PutObjectOutcome OssClient::PutObjectByUrl(const std::string &url, const std::shared_ptr<std::iostream> &content) const
{
    return client_->PutObjectByUrl(PutObjectByUrlRequest(url, content));
}

PutObjectOutcome OssClient::PutObjectByUrl(const std::string &url, const std::shared_ptr<std::iostream> &content, const ObjectMetaData &metaData) const
{
    return client_->PutObjectByUrl(PutObjectByUrlRequest(url, content, metaData));
}

InitiateMultipartUploadOutcome OssClient::InitiateMultipartUpload(const InitiateMultipartUploadRequest &request)const
{
    return client_->InitiateMultipartUpload(request);
}

PutObjectOutcome OssClient::UploadPart(const UploadPartRequest &request) const
{
    return client_->UploadPart(request);
}

UploadPartCopyOutcome OssClient::UploadPartCopy(const UploadPartCopyRequest &request) const
{
    return client_->UploadPartCopy(request);
}

CompleteMultipartUploadOutcome OssClient::CompleteMultipartUpload(const CompleteMultipartUploadRequest &request) const
{
    return client_->CompleteMultipartUpload(request);
}

VoidOutcome OssClient::AbortMultipartUpload(const AbortMultipartUploadRequest &request) const
{
    return client_->AbortMultipartUpload(request);
}

ListMultipartUploadsOutcome OssClient::ListMultipartUploads(const ListMultipartUploadsRequest &request) const
{
    return client_->ListMultipartUploads(request);
}

ListPartsOutcome OssClient::ListParts(const ListPartsRequest &request) const
{
    return client_->ListParts(request);
}

/*Aysnc APIs*/
void OssClient::ListObjectsAsync(const ListObjectsRequest &request, const ListObjectAsyncHandler &handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
    auto fn = [this, request, handler, context]()
    {
        handler(this, request, client_->ListObjects(request), context);
    };

    client_->asyncExecute(new Runnable(fn));
}

void OssClient::GetObjectAsync(const GetObjectRequest &request, const GetObjectAsyncHandler &handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
    auto fn = [this, request, handler, context]()
    {
        handler(this, request, client_->GetObject(request), context);
    };

    client_->asyncExecute(new Runnable(fn));
}

void OssClient::PutObjectAsync(const PutObjectRequest &request, const PutObjectAsyncHandler &handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
    auto fn = [this, request, handler, context]()
    {
        handler(this, request, client_->PutObject(request), context);
    };

    client_->asyncExecute(new Runnable(fn));
}

void OssClient::UploadPartAsync(const UploadPartRequest &request, const UploadPartAsyncHandler &handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
    auto fn = [this, request, handler, context]()
    {
        handler(this, request, client_->UploadPart(request), context);
    };

    client_->asyncExecute(new Runnable(fn));
}

void OssClient::UploadPartCopyAsync(const UploadPartCopyRequest &request, const UploadPartCopyAsyncHandler &handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
    auto fn = [this, request, handler, context]()
    {
        handler(this, request, client_->UploadPartCopy(request), context);
    };

    client_->asyncExecute(new Runnable(fn));
}


/*Callable APIs*/
ListObjectOutcomeCallable OssClient::ListObjectsCallable(const ListObjectsRequest &request) const
{
    auto task = std::make_shared<std::packaged_task<ListObjectOutcome()>>(
        [this, request]()
    {
        return this->ListObjects(request);
    });
    client_->asyncExecute(new Runnable([task]() { (*task)(); }));
    return task->get_future();
}

GetObjectOutcomeCallable OssClient::GetObjectCallable(const GetObjectRequest &request) const
{
    auto task = std::make_shared<std::packaged_task<GetObjectOutcome()>>(
        [this, request]()
    {
        return this->GetObject(request);
    });
    client_->asyncExecute(new Runnable([task]() { (*task)(); }));
    return task->get_future();

}

PutObjectOutcomeCallable OssClient::PutObjectCallable(const PutObjectRequest &request) const
{
    auto task = std::make_shared<std::packaged_task<PutObjectOutcome()>>(
        [this, request]()
    {
        return this->PutObject(request);
    });
    client_->asyncExecute(new Runnable([task]() { (*task)(); }));
    return task->get_future();
}

PutObjectOutcomeCallable OssClient::UploadPartCallable(const UploadPartRequest &request) const
{
    auto task = std::make_shared<std::packaged_task<PutObjectOutcome()>>(
        [this, request]()
    {
        return this->UploadPart(request);
    });
    client_->asyncExecute(new Runnable([task]() { (*task)(); }));
    return task->get_future();
}

UploadPartCopyOutcomeCallable OssClient::UploadPartCopyCallable(const UploadPartCopyRequest &request) const
{
    auto task = std::make_shared<std::packaged_task<UploadPartCopyOutcome()>>(
        [this, request]()
    {
        return this->UploadPartCopy(request);
    });
    client_->asyncExecute(new Runnable([task]() { (*task)(); }));
    return task->get_future();
}

/*Extended APIs*/
#if !defined(OSS_DISABLE_BUCKET)
bool OssClient::DoesBucketExist(const std::string &bucket) const
{
    return client_->GetBucketAcl(GetBucketAclRequest(bucket)).isSuccess();
}
#endif

bool OssClient::DoesObjectExist(const std::string &bucket, const std::string &key) const
{
    return client_->GetObjectMeta(GetObjectMetaRequest(bucket, key)).isSuccess();
}

CopyObjectOutcome OssClient::ModifyObjectMeta(const std::string& bucket, const std::string& key, const ObjectMetaData& meta)
{
    CopyObjectRequest copyRequest(bucket, key, meta);
    copyRequest.setCopySource(bucket, key);
    copyRequest.setMetadataDirective(CopyActionList::Replace);
    return client_->CopyObject(copyRequest);
}

#if !defined(OSS_DISABLE_LIVECHANNEL)
VoidOutcome OssClient::PutLiveChannelStatus(const PutLiveChannelStatusRequest &request) const
{
    return client_->PutLiveChannelStatus(request);
}

PutLiveChannelOutcome OssClient::PutLiveChannel(const PutLiveChannelRequest &request) const
{
    return client_->PutLiveChannel(request);
}

VoidOutcome OssClient::PostVodPlaylist(const PostVodPlaylistRequest &request) const
{
    return client_->PostVodPlaylist(request);
}

GetVodPlaylistOutcome OssClient::GetVodPlaylist(const GetVodPlaylistRequest &request) const
{
    return client_->GetVodPlaylist(request);
}

GetLiveChannelStatOutcome OssClient::GetLiveChannelStat(const GetLiveChannelStatRequest &request) const
{
    return client_->GetLiveChannelStat(request);
}

GetLiveChannelInfoOutcome OssClient::GetLiveChannelInfo(const GetLiveChannelInfoRequest &request) const
{
    return client_->GetLiveChannelInfo(request);
}

GetLiveChannelHistoryOutcome OssClient::GetLiveChannelHistory(const GetLiveChannelHistoryRequest &request) const
{
    return client_->GetLiveChannelHistory(request);
}

ListLiveChannelOutcome OssClient::ListLiveChannel(const ListLiveChannelRequest &request) const
{
    return client_->ListLiveChannel(request);
}

VoidOutcome OssClient::DeleteLiveChannel(const DeleteLiveChannelRequest &request) const
{
    return client_->DeleteLiveChannel(request);
}

StringOutcome OssClient::GenerateRTMPSignedUrl(const GenerateRTMPSignedUrlRequest &request) const
{
    return client_->GenerateRTMPSignedUrl(request);
}
#endif

void OssClient::DisableRequest()
{
    client_->DisableRequest();
}

void OssClient::EnableRequest()
{
    client_->EnableRequest();
}

#if !defined(OSS_DISABLE_RESUAMABLE)
PutObjectOutcome OssClient::ResumableUploadObject(const UploadObjectRequest &request) const
{
    return client_->ResumableUploadObject(request);
}

CopyObjectOutcome OssClient::ResumableCopyObject(const MultiCopyObjectRequest &request) const 
{
    return client_->ResumableCopyObject(request);
}

GetObjectOutcome OssClient::ResumableDownloadObject(const DownloadObjectRequest &request) const 
{
    return client_->ResumableDownloadObject(request);
}
#endif
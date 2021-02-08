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

#ifndef ALIBABACLOUD_CR_CRCLIENT_H_
#define ALIBABACLOUD_CR_CRCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RoaServiceClient.h>
#include "CrExport.h"
#include "model/CancelRepoBuildRequest.h"
#include "model/CancelRepoBuildResult.h"
#include "model/CreateCollectionRequest.h"
#include "model/CreateCollectionResult.h"
#include "model/CreateNamespaceRequest.h"
#include "model/CreateNamespaceResult.h"
#include "model/CreateNamespaceAuthorizationRequest.h"
#include "model/CreateNamespaceAuthorizationResult.h"
#include "model/CreateRepoRequest.h"
#include "model/CreateRepoResult.h"
#include "model/CreateRepoAuthorizationRequest.h"
#include "model/CreateRepoAuthorizationResult.h"
#include "model/CreateRepoBuildRuleRequest.h"
#include "model/CreateRepoBuildRuleResult.h"
#include "model/CreateRepoSyncTaskRequest.h"
#include "model/CreateRepoSyncTaskResult.h"
#include "model/CreateRepoWebhookRequest.h"
#include "model/CreateRepoWebhookResult.h"
#include "model/CreateUserInfoRequest.h"
#include "model/CreateUserInfoResult.h"
#include "model/CreateUserSourceAccountRequest.h"
#include "model/CreateUserSourceAccountResult.h"
#include "model/DeleteCollectionRequest.h"
#include "model/DeleteCollectionResult.h"
#include "model/DeleteImageRequest.h"
#include "model/DeleteImageResult.h"
#include "model/DeleteNamespaceRequest.h"
#include "model/DeleteNamespaceResult.h"
#include "model/DeleteNamespaceAuthorizationRequest.h"
#include "model/DeleteNamespaceAuthorizationResult.h"
#include "model/DeleteRepoRequest.h"
#include "model/DeleteRepoResult.h"
#include "model/DeleteRepoAuthorizationRequest.h"
#include "model/DeleteRepoAuthorizationResult.h"
#include "model/DeleteRepoBuildRuleRequest.h"
#include "model/DeleteRepoBuildRuleResult.h"
#include "model/DeleteRepoWebhookRequest.h"
#include "model/DeleteRepoWebhookResult.h"
#include "model/DeleteUserSourceAccountRequest.h"
#include "model/DeleteUserSourceAccountResult.h"
#include "model/GetAuthorizationTokenRequest.h"
#include "model/GetAuthorizationTokenResult.h"
#include "model/GetCollectionRequest.h"
#include "model/GetCollectionResult.h"
#include "model/GetImageLayerRequest.h"
#include "model/GetImageLayerResult.h"
#include "model/GetImageManifestRequest.h"
#include "model/GetImageManifestResult.h"
#include "model/GetImageScanRequest.h"
#include "model/GetImageScanResult.h"
#include "model/GetMirrorListRequest.h"
#include "model/GetMirrorListResult.h"
#include "model/GetNamespaceRequest.h"
#include "model/GetNamespaceResult.h"
#include "model/GetNamespaceAuthorizationListRequest.h"
#include "model/GetNamespaceAuthorizationListResult.h"
#include "model/GetNamespaceListRequest.h"
#include "model/GetNamespaceListResult.h"
#include "model/GetRegionRequest.h"
#include "model/GetRegionResult.h"
#include "model/GetRegionListRequest.h"
#include "model/GetRegionListResult.h"
#include "model/GetRepoRequest.h"
#include "model/GetRepoResult.h"
#include "model/GetRepoAuthorizationListRequest.h"
#include "model/GetRepoAuthorizationListResult.h"
#include "model/GetRepoBatchRequest.h"
#include "model/GetRepoBatchResult.h"
#include "model/GetRepoBuildListRequest.h"
#include "model/GetRepoBuildListResult.h"
#include "model/GetRepoBuildLogsRequest.h"
#include "model/GetRepoBuildLogsResult.h"
#include "model/GetRepoBuildRuleListRequest.h"
#include "model/GetRepoBuildRuleListResult.h"
#include "model/GetRepoBuildStatusRequest.h"
#include "model/GetRepoBuildStatusResult.h"
#include "model/GetRepoListRequest.h"
#include "model/GetRepoListResult.h"
#include "model/GetRepoListByNamespaceRequest.h"
#include "model/GetRepoListByNamespaceResult.h"
#include "model/GetRepoSourceRepoRequest.h"
#include "model/GetRepoSourceRepoResult.h"
#include "model/GetRepoSyncTaskRequest.h"
#include "model/GetRepoSyncTaskResult.h"
#include "model/GetRepoSyncTaskListRequest.h"
#include "model/GetRepoSyncTaskListResult.h"
#include "model/GetRepoTagsRequest.h"
#include "model/GetRepoTagsResult.h"
#include "model/GetRepoWebhookRequest.h"
#include "model/GetRepoWebhookResult.h"
#include "model/GetRepoWebhookLogListRequest.h"
#include "model/GetRepoWebhookLogListResult.h"
#include "model/GetSearchRequest.h"
#include "model/GetSearchResult.h"
#include "model/GetSubUserListRequest.h"
#include "model/GetSubUserListResult.h"
#include "model/GetUserInfoRequest.h"
#include "model/GetUserInfoResult.h"
#include "model/GetUserSourceAccountRequest.h"
#include "model/GetUserSourceAccountResult.h"
#include "model/GetUserSourceRepoListRequest.h"
#include "model/GetUserSourceRepoListResult.h"
#include "model/GetUserSourceRepoRefListRequest.h"
#include "model/GetUserSourceRepoRefListResult.h"
#include "model/SearchRepoRequest.h"
#include "model/SearchRepoResult.h"
#include "model/StartImageScanRequest.h"
#include "model/StartImageScanResult.h"
#include "model/StartRepoBuildRequest.h"
#include "model/StartRepoBuildResult.h"
#include "model/StartRepoBuildByRuleRequest.h"
#include "model/StartRepoBuildByRuleResult.h"
#include "model/UpdateNamespaceRequest.h"
#include "model/UpdateNamespaceResult.h"
#include "model/UpdateNamespaceAuthorizationRequest.h"
#include "model/UpdateNamespaceAuthorizationResult.h"
#include "model/UpdateRepoRequest.h"
#include "model/UpdateRepoResult.h"
#include "model/UpdateRepoAuthorizationRequest.h"
#include "model/UpdateRepoAuthorizationResult.h"
#include "model/UpdateRepoBuildRuleRequest.h"
#include "model/UpdateRepoBuildRuleResult.h"
#include "model/UpdateRepoSourceRepoRequest.h"
#include "model/UpdateRepoSourceRepoResult.h"
#include "model/UpdateRepoWebhookRequest.h"
#include "model/UpdateRepoWebhookResult.h"
#include "model/UpdateUserInfoRequest.h"
#include "model/UpdateUserInfoResult.h"


namespace AlibabaCloud
{
	namespace Cr
	{
		class ALIBABACLOUD_CR_EXPORT CrClient : public RoaServiceClient
		{
		public:
			typedef Outcome<Error, Model::CancelRepoBuildResult> CancelRepoBuildOutcome;
			typedef std::future<CancelRepoBuildOutcome> CancelRepoBuildOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::CancelRepoBuildRequest&, const CancelRepoBuildOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CancelRepoBuildAsyncHandler;
			typedef Outcome<Error, Model::CreateCollectionResult> CreateCollectionOutcome;
			typedef std::future<CreateCollectionOutcome> CreateCollectionOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::CreateCollectionRequest&, const CreateCollectionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateCollectionAsyncHandler;
			typedef Outcome<Error, Model::CreateNamespaceResult> CreateNamespaceOutcome;
			typedef std::future<CreateNamespaceOutcome> CreateNamespaceOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::CreateNamespaceRequest&, const CreateNamespaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateNamespaceAsyncHandler;
			typedef Outcome<Error, Model::CreateNamespaceAuthorizationResult> CreateNamespaceAuthorizationOutcome;
			typedef std::future<CreateNamespaceAuthorizationOutcome> CreateNamespaceAuthorizationOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::CreateNamespaceAuthorizationRequest&, const CreateNamespaceAuthorizationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateNamespaceAuthorizationAsyncHandler;
			typedef Outcome<Error, Model::CreateRepoResult> CreateRepoOutcome;
			typedef std::future<CreateRepoOutcome> CreateRepoOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::CreateRepoRequest&, const CreateRepoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateRepoAsyncHandler;
			typedef Outcome<Error, Model::CreateRepoAuthorizationResult> CreateRepoAuthorizationOutcome;
			typedef std::future<CreateRepoAuthorizationOutcome> CreateRepoAuthorizationOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::CreateRepoAuthorizationRequest&, const CreateRepoAuthorizationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateRepoAuthorizationAsyncHandler;
			typedef Outcome<Error, Model::CreateRepoBuildRuleResult> CreateRepoBuildRuleOutcome;
			typedef std::future<CreateRepoBuildRuleOutcome> CreateRepoBuildRuleOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::CreateRepoBuildRuleRequest&, const CreateRepoBuildRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateRepoBuildRuleAsyncHandler;
			typedef Outcome<Error, Model::CreateRepoSyncTaskResult> CreateRepoSyncTaskOutcome;
			typedef std::future<CreateRepoSyncTaskOutcome> CreateRepoSyncTaskOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::CreateRepoSyncTaskRequest&, const CreateRepoSyncTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateRepoSyncTaskAsyncHandler;
			typedef Outcome<Error, Model::CreateRepoWebhookResult> CreateRepoWebhookOutcome;
			typedef std::future<CreateRepoWebhookOutcome> CreateRepoWebhookOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::CreateRepoWebhookRequest&, const CreateRepoWebhookOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateRepoWebhookAsyncHandler;
			typedef Outcome<Error, Model::CreateUserInfoResult> CreateUserInfoOutcome;
			typedef std::future<CreateUserInfoOutcome> CreateUserInfoOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::CreateUserInfoRequest&, const CreateUserInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateUserInfoAsyncHandler;
			typedef Outcome<Error, Model::CreateUserSourceAccountResult> CreateUserSourceAccountOutcome;
			typedef std::future<CreateUserSourceAccountOutcome> CreateUserSourceAccountOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::CreateUserSourceAccountRequest&, const CreateUserSourceAccountOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateUserSourceAccountAsyncHandler;
			typedef Outcome<Error, Model::DeleteCollectionResult> DeleteCollectionOutcome;
			typedef std::future<DeleteCollectionOutcome> DeleteCollectionOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::DeleteCollectionRequest&, const DeleteCollectionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteCollectionAsyncHandler;
			typedef Outcome<Error, Model::DeleteImageResult> DeleteImageOutcome;
			typedef std::future<DeleteImageOutcome> DeleteImageOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::DeleteImageRequest&, const DeleteImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteImageAsyncHandler;
			typedef Outcome<Error, Model::DeleteNamespaceResult> DeleteNamespaceOutcome;
			typedef std::future<DeleteNamespaceOutcome> DeleteNamespaceOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::DeleteNamespaceRequest&, const DeleteNamespaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteNamespaceAsyncHandler;
			typedef Outcome<Error, Model::DeleteNamespaceAuthorizationResult> DeleteNamespaceAuthorizationOutcome;
			typedef std::future<DeleteNamespaceAuthorizationOutcome> DeleteNamespaceAuthorizationOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::DeleteNamespaceAuthorizationRequest&, const DeleteNamespaceAuthorizationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteNamespaceAuthorizationAsyncHandler;
			typedef Outcome<Error, Model::DeleteRepoResult> DeleteRepoOutcome;
			typedef std::future<DeleteRepoOutcome> DeleteRepoOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::DeleteRepoRequest&, const DeleteRepoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteRepoAsyncHandler;
			typedef Outcome<Error, Model::DeleteRepoAuthorizationResult> DeleteRepoAuthorizationOutcome;
			typedef std::future<DeleteRepoAuthorizationOutcome> DeleteRepoAuthorizationOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::DeleteRepoAuthorizationRequest&, const DeleteRepoAuthorizationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteRepoAuthorizationAsyncHandler;
			typedef Outcome<Error, Model::DeleteRepoBuildRuleResult> DeleteRepoBuildRuleOutcome;
			typedef std::future<DeleteRepoBuildRuleOutcome> DeleteRepoBuildRuleOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::DeleteRepoBuildRuleRequest&, const DeleteRepoBuildRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteRepoBuildRuleAsyncHandler;
			typedef Outcome<Error, Model::DeleteRepoWebhookResult> DeleteRepoWebhookOutcome;
			typedef std::future<DeleteRepoWebhookOutcome> DeleteRepoWebhookOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::DeleteRepoWebhookRequest&, const DeleteRepoWebhookOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteRepoWebhookAsyncHandler;
			typedef Outcome<Error, Model::DeleteUserSourceAccountResult> DeleteUserSourceAccountOutcome;
			typedef std::future<DeleteUserSourceAccountOutcome> DeleteUserSourceAccountOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::DeleteUserSourceAccountRequest&, const DeleteUserSourceAccountOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteUserSourceAccountAsyncHandler;
			typedef Outcome<Error, Model::GetAuthorizationTokenResult> GetAuthorizationTokenOutcome;
			typedef std::future<GetAuthorizationTokenOutcome> GetAuthorizationTokenOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetAuthorizationTokenRequest&, const GetAuthorizationTokenOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetAuthorizationTokenAsyncHandler;
			typedef Outcome<Error, Model::GetCollectionResult> GetCollectionOutcome;
			typedef std::future<GetCollectionOutcome> GetCollectionOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetCollectionRequest&, const GetCollectionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetCollectionAsyncHandler;
			typedef Outcome<Error, Model::GetImageLayerResult> GetImageLayerOutcome;
			typedef std::future<GetImageLayerOutcome> GetImageLayerOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetImageLayerRequest&, const GetImageLayerOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetImageLayerAsyncHandler;
			typedef Outcome<Error, Model::GetImageManifestResult> GetImageManifestOutcome;
			typedef std::future<GetImageManifestOutcome> GetImageManifestOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetImageManifestRequest&, const GetImageManifestOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetImageManifestAsyncHandler;
			typedef Outcome<Error, Model::GetImageScanResult> GetImageScanOutcome;
			typedef std::future<GetImageScanOutcome> GetImageScanOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetImageScanRequest&, const GetImageScanOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetImageScanAsyncHandler;
			typedef Outcome<Error, Model::GetMirrorListResult> GetMirrorListOutcome;
			typedef std::future<GetMirrorListOutcome> GetMirrorListOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetMirrorListRequest&, const GetMirrorListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetMirrorListAsyncHandler;
			typedef Outcome<Error, Model::GetNamespaceResult> GetNamespaceOutcome;
			typedef std::future<GetNamespaceOutcome> GetNamespaceOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetNamespaceRequest&, const GetNamespaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetNamespaceAsyncHandler;
			typedef Outcome<Error, Model::GetNamespaceAuthorizationListResult> GetNamespaceAuthorizationListOutcome;
			typedef std::future<GetNamespaceAuthorizationListOutcome> GetNamespaceAuthorizationListOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetNamespaceAuthorizationListRequest&, const GetNamespaceAuthorizationListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetNamespaceAuthorizationListAsyncHandler;
			typedef Outcome<Error, Model::GetNamespaceListResult> GetNamespaceListOutcome;
			typedef std::future<GetNamespaceListOutcome> GetNamespaceListOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetNamespaceListRequest&, const GetNamespaceListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetNamespaceListAsyncHandler;
			typedef Outcome<Error, Model::GetRegionResult> GetRegionOutcome;
			typedef std::future<GetRegionOutcome> GetRegionOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRegionRequest&, const GetRegionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRegionAsyncHandler;
			typedef Outcome<Error, Model::GetRegionListResult> GetRegionListOutcome;
			typedef std::future<GetRegionListOutcome> GetRegionListOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRegionListRequest&, const GetRegionListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRegionListAsyncHandler;
			typedef Outcome<Error, Model::GetRepoResult> GetRepoOutcome;
			typedef std::future<GetRepoOutcome> GetRepoOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoRequest&, const GetRepoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoAsyncHandler;
			typedef Outcome<Error, Model::GetRepoAuthorizationListResult> GetRepoAuthorizationListOutcome;
			typedef std::future<GetRepoAuthorizationListOutcome> GetRepoAuthorizationListOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoAuthorizationListRequest&, const GetRepoAuthorizationListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoAuthorizationListAsyncHandler;
			typedef Outcome<Error, Model::GetRepoBatchResult> GetRepoBatchOutcome;
			typedef std::future<GetRepoBatchOutcome> GetRepoBatchOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoBatchRequest&, const GetRepoBatchOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoBatchAsyncHandler;
			typedef Outcome<Error, Model::GetRepoBuildListResult> GetRepoBuildListOutcome;
			typedef std::future<GetRepoBuildListOutcome> GetRepoBuildListOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoBuildListRequest&, const GetRepoBuildListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoBuildListAsyncHandler;
			typedef Outcome<Error, Model::GetRepoBuildLogsResult> GetRepoBuildLogsOutcome;
			typedef std::future<GetRepoBuildLogsOutcome> GetRepoBuildLogsOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoBuildLogsRequest&, const GetRepoBuildLogsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoBuildLogsAsyncHandler;
			typedef Outcome<Error, Model::GetRepoBuildRuleListResult> GetRepoBuildRuleListOutcome;
			typedef std::future<GetRepoBuildRuleListOutcome> GetRepoBuildRuleListOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoBuildRuleListRequest&, const GetRepoBuildRuleListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoBuildRuleListAsyncHandler;
			typedef Outcome<Error, Model::GetRepoBuildStatusResult> GetRepoBuildStatusOutcome;
			typedef std::future<GetRepoBuildStatusOutcome> GetRepoBuildStatusOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoBuildStatusRequest&, const GetRepoBuildStatusOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoBuildStatusAsyncHandler;
			typedef Outcome<Error, Model::GetRepoListResult> GetRepoListOutcome;
			typedef std::future<GetRepoListOutcome> GetRepoListOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoListRequest&, const GetRepoListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoListAsyncHandler;
			typedef Outcome<Error, Model::GetRepoListByNamespaceResult> GetRepoListByNamespaceOutcome;
			typedef std::future<GetRepoListByNamespaceOutcome> GetRepoListByNamespaceOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoListByNamespaceRequest&, const GetRepoListByNamespaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoListByNamespaceAsyncHandler;
			typedef Outcome<Error, Model::GetRepoSourceRepoResult> GetRepoSourceRepoOutcome;
			typedef std::future<GetRepoSourceRepoOutcome> GetRepoSourceRepoOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoSourceRepoRequest&, const GetRepoSourceRepoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoSourceRepoAsyncHandler;
			typedef Outcome<Error, Model::GetRepoSyncTaskResult> GetRepoSyncTaskOutcome;
			typedef std::future<GetRepoSyncTaskOutcome> GetRepoSyncTaskOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoSyncTaskRequest&, const GetRepoSyncTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoSyncTaskAsyncHandler;
			typedef Outcome<Error, Model::GetRepoSyncTaskListResult> GetRepoSyncTaskListOutcome;
			typedef std::future<GetRepoSyncTaskListOutcome> GetRepoSyncTaskListOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoSyncTaskListRequest&, const GetRepoSyncTaskListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoSyncTaskListAsyncHandler;
			typedef Outcome<Error, Model::GetRepoTagsResult> GetRepoTagsOutcome;
			typedef std::future<GetRepoTagsOutcome> GetRepoTagsOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoTagsRequest&, const GetRepoTagsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoTagsAsyncHandler;
			typedef Outcome<Error, Model::GetRepoWebhookResult> GetRepoWebhookOutcome;
			typedef std::future<GetRepoWebhookOutcome> GetRepoWebhookOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoWebhookRequest&, const GetRepoWebhookOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoWebhookAsyncHandler;
			typedef Outcome<Error, Model::GetRepoWebhookLogListResult> GetRepoWebhookLogListOutcome;
			typedef std::future<GetRepoWebhookLogListOutcome> GetRepoWebhookLogListOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetRepoWebhookLogListRequest&, const GetRepoWebhookLogListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRepoWebhookLogListAsyncHandler;
			typedef Outcome<Error, Model::GetSearchResult> GetSearchOutcome;
			typedef std::future<GetSearchOutcome> GetSearchOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetSearchRequest&, const GetSearchOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetSearchAsyncHandler;
			typedef Outcome<Error, Model::GetSubUserListResult> GetSubUserListOutcome;
			typedef std::future<GetSubUserListOutcome> GetSubUserListOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetSubUserListRequest&, const GetSubUserListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetSubUserListAsyncHandler;
			typedef Outcome<Error, Model::GetUserInfoResult> GetUserInfoOutcome;
			typedef std::future<GetUserInfoOutcome> GetUserInfoOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetUserInfoRequest&, const GetUserInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetUserInfoAsyncHandler;
			typedef Outcome<Error, Model::GetUserSourceAccountResult> GetUserSourceAccountOutcome;
			typedef std::future<GetUserSourceAccountOutcome> GetUserSourceAccountOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetUserSourceAccountRequest&, const GetUserSourceAccountOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetUserSourceAccountAsyncHandler;
			typedef Outcome<Error, Model::GetUserSourceRepoListResult> GetUserSourceRepoListOutcome;
			typedef std::future<GetUserSourceRepoListOutcome> GetUserSourceRepoListOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetUserSourceRepoListRequest&, const GetUserSourceRepoListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetUserSourceRepoListAsyncHandler;
			typedef Outcome<Error, Model::GetUserSourceRepoRefListResult> GetUserSourceRepoRefListOutcome;
			typedef std::future<GetUserSourceRepoRefListOutcome> GetUserSourceRepoRefListOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::GetUserSourceRepoRefListRequest&, const GetUserSourceRepoRefListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetUserSourceRepoRefListAsyncHandler;
			typedef Outcome<Error, Model::SearchRepoResult> SearchRepoOutcome;
			typedef std::future<SearchRepoOutcome> SearchRepoOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::SearchRepoRequest&, const SearchRepoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SearchRepoAsyncHandler;
			typedef Outcome<Error, Model::StartImageScanResult> StartImageScanOutcome;
			typedef std::future<StartImageScanOutcome> StartImageScanOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::StartImageScanRequest&, const StartImageScanOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartImageScanAsyncHandler;
			typedef Outcome<Error, Model::StartRepoBuildResult> StartRepoBuildOutcome;
			typedef std::future<StartRepoBuildOutcome> StartRepoBuildOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::StartRepoBuildRequest&, const StartRepoBuildOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartRepoBuildAsyncHandler;
			typedef Outcome<Error, Model::StartRepoBuildByRuleResult> StartRepoBuildByRuleOutcome;
			typedef std::future<StartRepoBuildByRuleOutcome> StartRepoBuildByRuleOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::StartRepoBuildByRuleRequest&, const StartRepoBuildByRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartRepoBuildByRuleAsyncHandler;
			typedef Outcome<Error, Model::UpdateNamespaceResult> UpdateNamespaceOutcome;
			typedef std::future<UpdateNamespaceOutcome> UpdateNamespaceOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::UpdateNamespaceRequest&, const UpdateNamespaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateNamespaceAsyncHandler;
			typedef Outcome<Error, Model::UpdateNamespaceAuthorizationResult> UpdateNamespaceAuthorizationOutcome;
			typedef std::future<UpdateNamespaceAuthorizationOutcome> UpdateNamespaceAuthorizationOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::UpdateNamespaceAuthorizationRequest&, const UpdateNamespaceAuthorizationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateNamespaceAuthorizationAsyncHandler;
			typedef Outcome<Error, Model::UpdateRepoResult> UpdateRepoOutcome;
			typedef std::future<UpdateRepoOutcome> UpdateRepoOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::UpdateRepoRequest&, const UpdateRepoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateRepoAsyncHandler;
			typedef Outcome<Error, Model::UpdateRepoAuthorizationResult> UpdateRepoAuthorizationOutcome;
			typedef std::future<UpdateRepoAuthorizationOutcome> UpdateRepoAuthorizationOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::UpdateRepoAuthorizationRequest&, const UpdateRepoAuthorizationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateRepoAuthorizationAsyncHandler;
			typedef Outcome<Error, Model::UpdateRepoBuildRuleResult> UpdateRepoBuildRuleOutcome;
			typedef std::future<UpdateRepoBuildRuleOutcome> UpdateRepoBuildRuleOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::UpdateRepoBuildRuleRequest&, const UpdateRepoBuildRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateRepoBuildRuleAsyncHandler;
			typedef Outcome<Error, Model::UpdateRepoSourceRepoResult> UpdateRepoSourceRepoOutcome;
			typedef std::future<UpdateRepoSourceRepoOutcome> UpdateRepoSourceRepoOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::UpdateRepoSourceRepoRequest&, const UpdateRepoSourceRepoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateRepoSourceRepoAsyncHandler;
			typedef Outcome<Error, Model::UpdateRepoWebhookResult> UpdateRepoWebhookOutcome;
			typedef std::future<UpdateRepoWebhookOutcome> UpdateRepoWebhookOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::UpdateRepoWebhookRequest&, const UpdateRepoWebhookOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateRepoWebhookAsyncHandler;
			typedef Outcome<Error, Model::UpdateUserInfoResult> UpdateUserInfoOutcome;
			typedef std::future<UpdateUserInfoOutcome> UpdateUserInfoOutcomeCallable;
			typedef std::function<void(const CrClient*, const Model::UpdateUserInfoRequest&, const UpdateUserInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateUserInfoAsyncHandler;

			CrClient(const Credentials &credentials, const ClientConfiguration &configuration);
			CrClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			CrClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~CrClient();
			CancelRepoBuildOutcome cancelRepoBuild(const Model::CancelRepoBuildRequest &request)const;
			void cancelRepoBuildAsync(const Model::CancelRepoBuildRequest& request, const CancelRepoBuildAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CancelRepoBuildOutcomeCallable cancelRepoBuildCallable(const Model::CancelRepoBuildRequest& request) const;
			CreateCollectionOutcome createCollection(const Model::CreateCollectionRequest &request)const;
			void createCollectionAsync(const Model::CreateCollectionRequest& request, const CreateCollectionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateCollectionOutcomeCallable createCollectionCallable(const Model::CreateCollectionRequest& request) const;
			CreateNamespaceOutcome createNamespace(const Model::CreateNamespaceRequest &request)const;
			void createNamespaceAsync(const Model::CreateNamespaceRequest& request, const CreateNamespaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateNamespaceOutcomeCallable createNamespaceCallable(const Model::CreateNamespaceRequest& request) const;
			CreateNamespaceAuthorizationOutcome createNamespaceAuthorization(const Model::CreateNamespaceAuthorizationRequest &request)const;
			void createNamespaceAuthorizationAsync(const Model::CreateNamespaceAuthorizationRequest& request, const CreateNamespaceAuthorizationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateNamespaceAuthorizationOutcomeCallable createNamespaceAuthorizationCallable(const Model::CreateNamespaceAuthorizationRequest& request) const;
			CreateRepoOutcome createRepo(const Model::CreateRepoRequest &request)const;
			void createRepoAsync(const Model::CreateRepoRequest& request, const CreateRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateRepoOutcomeCallable createRepoCallable(const Model::CreateRepoRequest& request) const;
			CreateRepoAuthorizationOutcome createRepoAuthorization(const Model::CreateRepoAuthorizationRequest &request)const;
			void createRepoAuthorizationAsync(const Model::CreateRepoAuthorizationRequest& request, const CreateRepoAuthorizationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateRepoAuthorizationOutcomeCallable createRepoAuthorizationCallable(const Model::CreateRepoAuthorizationRequest& request) const;
			CreateRepoBuildRuleOutcome createRepoBuildRule(const Model::CreateRepoBuildRuleRequest &request)const;
			void createRepoBuildRuleAsync(const Model::CreateRepoBuildRuleRequest& request, const CreateRepoBuildRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateRepoBuildRuleOutcomeCallable createRepoBuildRuleCallable(const Model::CreateRepoBuildRuleRequest& request) const;
			CreateRepoSyncTaskOutcome createRepoSyncTask(const Model::CreateRepoSyncTaskRequest &request)const;
			void createRepoSyncTaskAsync(const Model::CreateRepoSyncTaskRequest& request, const CreateRepoSyncTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateRepoSyncTaskOutcomeCallable createRepoSyncTaskCallable(const Model::CreateRepoSyncTaskRequest& request) const;
			CreateRepoWebhookOutcome createRepoWebhook(const Model::CreateRepoWebhookRequest &request)const;
			void createRepoWebhookAsync(const Model::CreateRepoWebhookRequest& request, const CreateRepoWebhookAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateRepoWebhookOutcomeCallable createRepoWebhookCallable(const Model::CreateRepoWebhookRequest& request) const;
			CreateUserInfoOutcome createUserInfo(const Model::CreateUserInfoRequest &request)const;
			void createUserInfoAsync(const Model::CreateUserInfoRequest& request, const CreateUserInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateUserInfoOutcomeCallable createUserInfoCallable(const Model::CreateUserInfoRequest& request) const;
			CreateUserSourceAccountOutcome createUserSourceAccount(const Model::CreateUserSourceAccountRequest &request)const;
			void createUserSourceAccountAsync(const Model::CreateUserSourceAccountRequest& request, const CreateUserSourceAccountAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateUserSourceAccountOutcomeCallable createUserSourceAccountCallable(const Model::CreateUserSourceAccountRequest& request) const;
			DeleteCollectionOutcome deleteCollection(const Model::DeleteCollectionRequest &request)const;
			void deleteCollectionAsync(const Model::DeleteCollectionRequest& request, const DeleteCollectionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteCollectionOutcomeCallable deleteCollectionCallable(const Model::DeleteCollectionRequest& request) const;
			DeleteImageOutcome deleteImage(const Model::DeleteImageRequest &request)const;
			void deleteImageAsync(const Model::DeleteImageRequest& request, const DeleteImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteImageOutcomeCallable deleteImageCallable(const Model::DeleteImageRequest& request) const;
			DeleteNamespaceOutcome deleteNamespace(const Model::DeleteNamespaceRequest &request)const;
			void deleteNamespaceAsync(const Model::DeleteNamespaceRequest& request, const DeleteNamespaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteNamespaceOutcomeCallable deleteNamespaceCallable(const Model::DeleteNamespaceRequest& request) const;
			DeleteNamespaceAuthorizationOutcome deleteNamespaceAuthorization(const Model::DeleteNamespaceAuthorizationRequest &request)const;
			void deleteNamespaceAuthorizationAsync(const Model::DeleteNamespaceAuthorizationRequest& request, const DeleteNamespaceAuthorizationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteNamespaceAuthorizationOutcomeCallable deleteNamespaceAuthorizationCallable(const Model::DeleteNamespaceAuthorizationRequest& request) const;
			DeleteRepoOutcome deleteRepo(const Model::DeleteRepoRequest &request)const;
			void deleteRepoAsync(const Model::DeleteRepoRequest& request, const DeleteRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteRepoOutcomeCallable deleteRepoCallable(const Model::DeleteRepoRequest& request) const;
			DeleteRepoAuthorizationOutcome deleteRepoAuthorization(const Model::DeleteRepoAuthorizationRequest &request)const;
			void deleteRepoAuthorizationAsync(const Model::DeleteRepoAuthorizationRequest& request, const DeleteRepoAuthorizationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteRepoAuthorizationOutcomeCallable deleteRepoAuthorizationCallable(const Model::DeleteRepoAuthorizationRequest& request) const;
			DeleteRepoBuildRuleOutcome deleteRepoBuildRule(const Model::DeleteRepoBuildRuleRequest &request)const;
			void deleteRepoBuildRuleAsync(const Model::DeleteRepoBuildRuleRequest& request, const DeleteRepoBuildRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteRepoBuildRuleOutcomeCallable deleteRepoBuildRuleCallable(const Model::DeleteRepoBuildRuleRequest& request) const;
			DeleteRepoWebhookOutcome deleteRepoWebhook(const Model::DeleteRepoWebhookRequest &request)const;
			void deleteRepoWebhookAsync(const Model::DeleteRepoWebhookRequest& request, const DeleteRepoWebhookAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteRepoWebhookOutcomeCallable deleteRepoWebhookCallable(const Model::DeleteRepoWebhookRequest& request) const;
			DeleteUserSourceAccountOutcome deleteUserSourceAccount(const Model::DeleteUserSourceAccountRequest &request)const;
			void deleteUserSourceAccountAsync(const Model::DeleteUserSourceAccountRequest& request, const DeleteUserSourceAccountAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteUserSourceAccountOutcomeCallable deleteUserSourceAccountCallable(const Model::DeleteUserSourceAccountRequest& request) const;
			GetAuthorizationTokenOutcome getAuthorizationToken(const Model::GetAuthorizationTokenRequest &request)const;
			void getAuthorizationTokenAsync(const Model::GetAuthorizationTokenRequest& request, const GetAuthorizationTokenAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetAuthorizationTokenOutcomeCallable getAuthorizationTokenCallable(const Model::GetAuthorizationTokenRequest& request) const;
			GetCollectionOutcome getCollection(const Model::GetCollectionRequest &request)const;
			void getCollectionAsync(const Model::GetCollectionRequest& request, const GetCollectionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetCollectionOutcomeCallable getCollectionCallable(const Model::GetCollectionRequest& request) const;
			GetImageLayerOutcome getImageLayer(const Model::GetImageLayerRequest &request)const;
			void getImageLayerAsync(const Model::GetImageLayerRequest& request, const GetImageLayerAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetImageLayerOutcomeCallable getImageLayerCallable(const Model::GetImageLayerRequest& request) const;
			GetImageManifestOutcome getImageManifest(const Model::GetImageManifestRequest &request)const;
			void getImageManifestAsync(const Model::GetImageManifestRequest& request, const GetImageManifestAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetImageManifestOutcomeCallable getImageManifestCallable(const Model::GetImageManifestRequest& request) const;
			GetImageScanOutcome getImageScan(const Model::GetImageScanRequest &request)const;
			void getImageScanAsync(const Model::GetImageScanRequest& request, const GetImageScanAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetImageScanOutcomeCallable getImageScanCallable(const Model::GetImageScanRequest& request) const;
			GetMirrorListOutcome getMirrorList(const Model::GetMirrorListRequest &request)const;
			void getMirrorListAsync(const Model::GetMirrorListRequest& request, const GetMirrorListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetMirrorListOutcomeCallable getMirrorListCallable(const Model::GetMirrorListRequest& request) const;
			GetNamespaceOutcome getNamespace(const Model::GetNamespaceRequest &request)const;
			void getNamespaceAsync(const Model::GetNamespaceRequest& request, const GetNamespaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetNamespaceOutcomeCallable getNamespaceCallable(const Model::GetNamespaceRequest& request) const;
			GetNamespaceAuthorizationListOutcome getNamespaceAuthorizationList(const Model::GetNamespaceAuthorizationListRequest &request)const;
			void getNamespaceAuthorizationListAsync(const Model::GetNamespaceAuthorizationListRequest& request, const GetNamespaceAuthorizationListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetNamespaceAuthorizationListOutcomeCallable getNamespaceAuthorizationListCallable(const Model::GetNamespaceAuthorizationListRequest& request) const;
			GetNamespaceListOutcome getNamespaceList(const Model::GetNamespaceListRequest &request)const;
			void getNamespaceListAsync(const Model::GetNamespaceListRequest& request, const GetNamespaceListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetNamespaceListOutcomeCallable getNamespaceListCallable(const Model::GetNamespaceListRequest& request) const;
			GetRegionOutcome getRegion(const Model::GetRegionRequest &request)const;
			void getRegionAsync(const Model::GetRegionRequest& request, const GetRegionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRegionOutcomeCallable getRegionCallable(const Model::GetRegionRequest& request) const;
			GetRegionListOutcome getRegionList(const Model::GetRegionListRequest &request)const;
			void getRegionListAsync(const Model::GetRegionListRequest& request, const GetRegionListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRegionListOutcomeCallable getRegionListCallable(const Model::GetRegionListRequest& request) const;
			GetRepoOutcome getRepo(const Model::GetRepoRequest &request)const;
			void getRepoAsync(const Model::GetRepoRequest& request, const GetRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoOutcomeCallable getRepoCallable(const Model::GetRepoRequest& request) const;
			GetRepoAuthorizationListOutcome getRepoAuthorizationList(const Model::GetRepoAuthorizationListRequest &request)const;
			void getRepoAuthorizationListAsync(const Model::GetRepoAuthorizationListRequest& request, const GetRepoAuthorizationListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoAuthorizationListOutcomeCallable getRepoAuthorizationListCallable(const Model::GetRepoAuthorizationListRequest& request) const;
			GetRepoBatchOutcome getRepoBatch(const Model::GetRepoBatchRequest &request)const;
			void getRepoBatchAsync(const Model::GetRepoBatchRequest& request, const GetRepoBatchAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoBatchOutcomeCallable getRepoBatchCallable(const Model::GetRepoBatchRequest& request) const;
			GetRepoBuildListOutcome getRepoBuildList(const Model::GetRepoBuildListRequest &request)const;
			void getRepoBuildListAsync(const Model::GetRepoBuildListRequest& request, const GetRepoBuildListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoBuildListOutcomeCallable getRepoBuildListCallable(const Model::GetRepoBuildListRequest& request) const;
			GetRepoBuildLogsOutcome getRepoBuildLogs(const Model::GetRepoBuildLogsRequest &request)const;
			void getRepoBuildLogsAsync(const Model::GetRepoBuildLogsRequest& request, const GetRepoBuildLogsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoBuildLogsOutcomeCallable getRepoBuildLogsCallable(const Model::GetRepoBuildLogsRequest& request) const;
			GetRepoBuildRuleListOutcome getRepoBuildRuleList(const Model::GetRepoBuildRuleListRequest &request)const;
			void getRepoBuildRuleListAsync(const Model::GetRepoBuildRuleListRequest& request, const GetRepoBuildRuleListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoBuildRuleListOutcomeCallable getRepoBuildRuleListCallable(const Model::GetRepoBuildRuleListRequest& request) const;
			GetRepoBuildStatusOutcome getRepoBuildStatus(const Model::GetRepoBuildStatusRequest &request)const;
			void getRepoBuildStatusAsync(const Model::GetRepoBuildStatusRequest& request, const GetRepoBuildStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoBuildStatusOutcomeCallable getRepoBuildStatusCallable(const Model::GetRepoBuildStatusRequest& request) const;
			GetRepoListOutcome getRepoList(const Model::GetRepoListRequest &request)const;
			void getRepoListAsync(const Model::GetRepoListRequest& request, const GetRepoListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoListOutcomeCallable getRepoListCallable(const Model::GetRepoListRequest& request) const;
			GetRepoListByNamespaceOutcome getRepoListByNamespace(const Model::GetRepoListByNamespaceRequest &request)const;
			void getRepoListByNamespaceAsync(const Model::GetRepoListByNamespaceRequest& request, const GetRepoListByNamespaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoListByNamespaceOutcomeCallable getRepoListByNamespaceCallable(const Model::GetRepoListByNamespaceRequest& request) const;
			GetRepoSourceRepoOutcome getRepoSourceRepo(const Model::GetRepoSourceRepoRequest &request)const;
			void getRepoSourceRepoAsync(const Model::GetRepoSourceRepoRequest& request, const GetRepoSourceRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoSourceRepoOutcomeCallable getRepoSourceRepoCallable(const Model::GetRepoSourceRepoRequest& request) const;
			GetRepoSyncTaskOutcome getRepoSyncTask(const Model::GetRepoSyncTaskRequest &request)const;
			void getRepoSyncTaskAsync(const Model::GetRepoSyncTaskRequest& request, const GetRepoSyncTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoSyncTaskOutcomeCallable getRepoSyncTaskCallable(const Model::GetRepoSyncTaskRequest& request) const;
			GetRepoSyncTaskListOutcome getRepoSyncTaskList(const Model::GetRepoSyncTaskListRequest &request)const;
			void getRepoSyncTaskListAsync(const Model::GetRepoSyncTaskListRequest& request, const GetRepoSyncTaskListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoSyncTaskListOutcomeCallable getRepoSyncTaskListCallable(const Model::GetRepoSyncTaskListRequest& request) const;
			GetRepoTagsOutcome getRepoTags(const Model::GetRepoTagsRequest &request)const;
			void getRepoTagsAsync(const Model::GetRepoTagsRequest& request, const GetRepoTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoTagsOutcomeCallable getRepoTagsCallable(const Model::GetRepoTagsRequest& request) const;
			GetRepoWebhookOutcome getRepoWebhook(const Model::GetRepoWebhookRequest &request)const;
			void getRepoWebhookAsync(const Model::GetRepoWebhookRequest& request, const GetRepoWebhookAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoWebhookOutcomeCallable getRepoWebhookCallable(const Model::GetRepoWebhookRequest& request) const;
			GetRepoWebhookLogListOutcome getRepoWebhookLogList(const Model::GetRepoWebhookLogListRequest &request)const;
			void getRepoWebhookLogListAsync(const Model::GetRepoWebhookLogListRequest& request, const GetRepoWebhookLogListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRepoWebhookLogListOutcomeCallable getRepoWebhookLogListCallable(const Model::GetRepoWebhookLogListRequest& request) const;
			GetSearchOutcome getSearch(const Model::GetSearchRequest &request)const;
			void getSearchAsync(const Model::GetSearchRequest& request, const GetSearchAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetSearchOutcomeCallable getSearchCallable(const Model::GetSearchRequest& request) const;
			GetSubUserListOutcome getSubUserList(const Model::GetSubUserListRequest &request)const;
			void getSubUserListAsync(const Model::GetSubUserListRequest& request, const GetSubUserListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetSubUserListOutcomeCallable getSubUserListCallable(const Model::GetSubUserListRequest& request) const;
			GetUserInfoOutcome getUserInfo(const Model::GetUserInfoRequest &request)const;
			void getUserInfoAsync(const Model::GetUserInfoRequest& request, const GetUserInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetUserInfoOutcomeCallable getUserInfoCallable(const Model::GetUserInfoRequest& request) const;
			GetUserSourceAccountOutcome getUserSourceAccount(const Model::GetUserSourceAccountRequest &request)const;
			void getUserSourceAccountAsync(const Model::GetUserSourceAccountRequest& request, const GetUserSourceAccountAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetUserSourceAccountOutcomeCallable getUserSourceAccountCallable(const Model::GetUserSourceAccountRequest& request) const;
			GetUserSourceRepoListOutcome getUserSourceRepoList(const Model::GetUserSourceRepoListRequest &request)const;
			void getUserSourceRepoListAsync(const Model::GetUserSourceRepoListRequest& request, const GetUserSourceRepoListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetUserSourceRepoListOutcomeCallable getUserSourceRepoListCallable(const Model::GetUserSourceRepoListRequest& request) const;
			GetUserSourceRepoRefListOutcome getUserSourceRepoRefList(const Model::GetUserSourceRepoRefListRequest &request)const;
			void getUserSourceRepoRefListAsync(const Model::GetUserSourceRepoRefListRequest& request, const GetUserSourceRepoRefListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetUserSourceRepoRefListOutcomeCallable getUserSourceRepoRefListCallable(const Model::GetUserSourceRepoRefListRequest& request) const;
			SearchRepoOutcome searchRepo(const Model::SearchRepoRequest &request)const;
			void searchRepoAsync(const Model::SearchRepoRequest& request, const SearchRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SearchRepoOutcomeCallable searchRepoCallable(const Model::SearchRepoRequest& request) const;
			StartImageScanOutcome startImageScan(const Model::StartImageScanRequest &request)const;
			void startImageScanAsync(const Model::StartImageScanRequest& request, const StartImageScanAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartImageScanOutcomeCallable startImageScanCallable(const Model::StartImageScanRequest& request) const;
			StartRepoBuildOutcome startRepoBuild(const Model::StartRepoBuildRequest &request)const;
			void startRepoBuildAsync(const Model::StartRepoBuildRequest& request, const StartRepoBuildAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartRepoBuildOutcomeCallable startRepoBuildCallable(const Model::StartRepoBuildRequest& request) const;
			StartRepoBuildByRuleOutcome startRepoBuildByRule(const Model::StartRepoBuildByRuleRequest &request)const;
			void startRepoBuildByRuleAsync(const Model::StartRepoBuildByRuleRequest& request, const StartRepoBuildByRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartRepoBuildByRuleOutcomeCallable startRepoBuildByRuleCallable(const Model::StartRepoBuildByRuleRequest& request) const;
			UpdateNamespaceOutcome updateNamespace(const Model::UpdateNamespaceRequest &request)const;
			void updateNamespaceAsync(const Model::UpdateNamespaceRequest& request, const UpdateNamespaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateNamespaceOutcomeCallable updateNamespaceCallable(const Model::UpdateNamespaceRequest& request) const;
			UpdateNamespaceAuthorizationOutcome updateNamespaceAuthorization(const Model::UpdateNamespaceAuthorizationRequest &request)const;
			void updateNamespaceAuthorizationAsync(const Model::UpdateNamespaceAuthorizationRequest& request, const UpdateNamespaceAuthorizationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateNamespaceAuthorizationOutcomeCallable updateNamespaceAuthorizationCallable(const Model::UpdateNamespaceAuthorizationRequest& request) const;
			UpdateRepoOutcome updateRepo(const Model::UpdateRepoRequest &request)const;
			void updateRepoAsync(const Model::UpdateRepoRequest& request, const UpdateRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateRepoOutcomeCallable updateRepoCallable(const Model::UpdateRepoRequest& request) const;
			UpdateRepoAuthorizationOutcome updateRepoAuthorization(const Model::UpdateRepoAuthorizationRequest &request)const;
			void updateRepoAuthorizationAsync(const Model::UpdateRepoAuthorizationRequest& request, const UpdateRepoAuthorizationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateRepoAuthorizationOutcomeCallable updateRepoAuthorizationCallable(const Model::UpdateRepoAuthorizationRequest& request) const;
			UpdateRepoBuildRuleOutcome updateRepoBuildRule(const Model::UpdateRepoBuildRuleRequest &request)const;
			void updateRepoBuildRuleAsync(const Model::UpdateRepoBuildRuleRequest& request, const UpdateRepoBuildRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateRepoBuildRuleOutcomeCallable updateRepoBuildRuleCallable(const Model::UpdateRepoBuildRuleRequest& request) const;
			UpdateRepoSourceRepoOutcome updateRepoSourceRepo(const Model::UpdateRepoSourceRepoRequest &request)const;
			void updateRepoSourceRepoAsync(const Model::UpdateRepoSourceRepoRequest& request, const UpdateRepoSourceRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateRepoSourceRepoOutcomeCallable updateRepoSourceRepoCallable(const Model::UpdateRepoSourceRepoRequest& request) const;
			UpdateRepoWebhookOutcome updateRepoWebhook(const Model::UpdateRepoWebhookRequest &request)const;
			void updateRepoWebhookAsync(const Model::UpdateRepoWebhookRequest& request, const UpdateRepoWebhookAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateRepoWebhookOutcomeCallable updateRepoWebhookCallable(const Model::UpdateRepoWebhookRequest& request) const;
			UpdateUserInfoOutcome updateUserInfo(const Model::UpdateUserInfoRequest &request)const;
			void updateUserInfoAsync(const Model::UpdateUserInfoRequest& request, const UpdateUserInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateUserInfoOutcomeCallable updateUserInfoCallable(const Model::UpdateUserInfoRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_CR_CRCLIENT_H_

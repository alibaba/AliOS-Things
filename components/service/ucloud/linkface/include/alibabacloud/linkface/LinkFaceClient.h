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

#ifndef ALIBABACLOUD_LINKFACE_LINKFACECLIENT_H_
#define ALIBABACLOUD_LINKFACE_LINKFACECLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "LinkFaceExport.h"
#include "model/CreateGroupRequest.h"
#include "model/CreateGroupResult.h"
#include "model/DeleteDeviceAllGroupRequest.h"
#include "model/DeleteDeviceAllGroupResult.h"
#include "model/DeleteDeviceGroupRequest.h"
#include "model/DeleteDeviceGroupResult.h"
#include "model/DeleteFaceRequest.h"
#include "model/DeleteFaceResult.h"
#include "model/DeleteGroupRequest.h"
#include "model/DeleteGroupResult.h"
#include "model/LinkFaceRequest.h"
#include "model/LinkFaceResult.h"
#include "model/QueryAddUserInfoRequest.h"
#include "model/QueryAddUserInfoResult.h"
#include "model/QueryAllGroupsRequest.h"
#include "model/QueryAllGroupsResult.h"
#include "model/QueryAuthenticationRequest.h"
#include "model/QueryAuthenticationResult.h"
#include "model/QueryFaceRequest.h"
#include "model/QueryFaceResult.h"
#include "model/QueryGroupUsersRequest.h"
#include "model/QueryGroupUsersResult.h"
#include "model/QueryLicensesRequest.h"
#include "model/QueryLicensesResult.h"
#include "model/QuerySyncPicScheduleRequest.h"
#include "model/QuerySyncPicScheduleResult.h"
#include "model/RegisterFaceRequest.h"
#include "model/RegisterFaceResult.h"
#include "model/SearchFaceRequest.h"
#include "model/SearchFaceResult.h"
#include "model/SyncFacePicturesRequest.h"
#include "model/SyncFacePicturesResult.h"
#include "model/UnlinkFaceRequest.h"
#include "model/UnlinkFaceResult.h"
#include "model/UpdateFaceRequest.h"
#include "model/UpdateFaceResult.h"


namespace AlibabaCloud
{
	namespace LinkFace
	{
		class ALIBABACLOUD_LINKFACE_EXPORT LinkFaceClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::CreateGroupResult> CreateGroupOutcome;
			typedef std::future<CreateGroupOutcome> CreateGroupOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::CreateGroupRequest&, const CreateGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateGroupAsyncHandler;
			typedef Outcome<Error, Model::DeleteDeviceAllGroupResult> DeleteDeviceAllGroupOutcome;
			typedef std::future<DeleteDeviceAllGroupOutcome> DeleteDeviceAllGroupOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::DeleteDeviceAllGroupRequest&, const DeleteDeviceAllGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteDeviceAllGroupAsyncHandler;
			typedef Outcome<Error, Model::DeleteDeviceGroupResult> DeleteDeviceGroupOutcome;
			typedef std::future<DeleteDeviceGroupOutcome> DeleteDeviceGroupOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::DeleteDeviceGroupRequest&, const DeleteDeviceGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteDeviceGroupAsyncHandler;
			typedef Outcome<Error, Model::DeleteFaceResult> DeleteFaceOutcome;
			typedef std::future<DeleteFaceOutcome> DeleteFaceOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::DeleteFaceRequest&, const DeleteFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteFaceAsyncHandler;
			typedef Outcome<Error, Model::DeleteGroupResult> DeleteGroupOutcome;
			typedef std::future<DeleteGroupOutcome> DeleteGroupOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::DeleteGroupRequest&, const DeleteGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteGroupAsyncHandler;
			typedef Outcome<Error, Model::LinkFaceResult> LinkFaceOutcome;
			typedef std::future<LinkFaceOutcome> LinkFaceOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::LinkFaceRequest&, const LinkFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> LinkFaceAsyncHandler;
			typedef Outcome<Error, Model::QueryAddUserInfoResult> QueryAddUserInfoOutcome;
			typedef std::future<QueryAddUserInfoOutcome> QueryAddUserInfoOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::QueryAddUserInfoRequest&, const QueryAddUserInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryAddUserInfoAsyncHandler;
			typedef Outcome<Error, Model::QueryAllGroupsResult> QueryAllGroupsOutcome;
			typedef std::future<QueryAllGroupsOutcome> QueryAllGroupsOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::QueryAllGroupsRequest&, const QueryAllGroupsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryAllGroupsAsyncHandler;
			typedef Outcome<Error, Model::QueryAuthenticationResult> QueryAuthenticationOutcome;
			typedef std::future<QueryAuthenticationOutcome> QueryAuthenticationOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::QueryAuthenticationRequest&, const QueryAuthenticationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryAuthenticationAsyncHandler;
			typedef Outcome<Error, Model::QueryFaceResult> QueryFaceOutcome;
			typedef std::future<QueryFaceOutcome> QueryFaceOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::QueryFaceRequest&, const QueryFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryFaceAsyncHandler;
			typedef Outcome<Error, Model::QueryGroupUsersResult> QueryGroupUsersOutcome;
			typedef std::future<QueryGroupUsersOutcome> QueryGroupUsersOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::QueryGroupUsersRequest&, const QueryGroupUsersOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryGroupUsersAsyncHandler;
			typedef Outcome<Error, Model::QueryLicensesResult> QueryLicensesOutcome;
			typedef std::future<QueryLicensesOutcome> QueryLicensesOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::QueryLicensesRequest&, const QueryLicensesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryLicensesAsyncHandler;
			typedef Outcome<Error, Model::QuerySyncPicScheduleResult> QuerySyncPicScheduleOutcome;
			typedef std::future<QuerySyncPicScheduleOutcome> QuerySyncPicScheduleOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::QuerySyncPicScheduleRequest&, const QuerySyncPicScheduleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QuerySyncPicScheduleAsyncHandler;
			typedef Outcome<Error, Model::RegisterFaceResult> RegisterFaceOutcome;
			typedef std::future<RegisterFaceOutcome> RegisterFaceOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::RegisterFaceRequest&, const RegisterFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RegisterFaceAsyncHandler;
			typedef Outcome<Error, Model::SearchFaceResult> SearchFaceOutcome;
			typedef std::future<SearchFaceOutcome> SearchFaceOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::SearchFaceRequest&, const SearchFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SearchFaceAsyncHandler;
			typedef Outcome<Error, Model::SyncFacePicturesResult> SyncFacePicturesOutcome;
			typedef std::future<SyncFacePicturesOutcome> SyncFacePicturesOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::SyncFacePicturesRequest&, const SyncFacePicturesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SyncFacePicturesAsyncHandler;
			typedef Outcome<Error, Model::UnlinkFaceResult> UnlinkFaceOutcome;
			typedef std::future<UnlinkFaceOutcome> UnlinkFaceOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::UnlinkFaceRequest&, const UnlinkFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UnlinkFaceAsyncHandler;
			typedef Outcome<Error, Model::UpdateFaceResult> UpdateFaceOutcome;
			typedef std::future<UpdateFaceOutcome> UpdateFaceOutcomeCallable;
			typedef std::function<void(const LinkFaceClient*, const Model::UpdateFaceRequest&, const UpdateFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateFaceAsyncHandler;

			LinkFaceClient(const Credentials &credentials, const ClientConfiguration &configuration);
			LinkFaceClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			LinkFaceClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~LinkFaceClient();
			CreateGroupOutcome createGroup(const Model::CreateGroupRequest &request)const;
			void createGroupAsync(const Model::CreateGroupRequest& request, const CreateGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateGroupOutcomeCallable createGroupCallable(const Model::CreateGroupRequest& request) const;
			DeleteDeviceAllGroupOutcome deleteDeviceAllGroup(const Model::DeleteDeviceAllGroupRequest &request)const;
			void deleteDeviceAllGroupAsync(const Model::DeleteDeviceAllGroupRequest& request, const DeleteDeviceAllGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteDeviceAllGroupOutcomeCallable deleteDeviceAllGroupCallable(const Model::DeleteDeviceAllGroupRequest& request) const;
			DeleteDeviceGroupOutcome deleteDeviceGroup(const Model::DeleteDeviceGroupRequest &request)const;
			void deleteDeviceGroupAsync(const Model::DeleteDeviceGroupRequest& request, const DeleteDeviceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteDeviceGroupOutcomeCallable deleteDeviceGroupCallable(const Model::DeleteDeviceGroupRequest& request) const;
			DeleteFaceOutcome deleteFace(const Model::DeleteFaceRequest &request)const;
			void deleteFaceAsync(const Model::DeleteFaceRequest& request, const DeleteFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteFaceOutcomeCallable deleteFaceCallable(const Model::DeleteFaceRequest& request) const;
			DeleteGroupOutcome deleteGroup(const Model::DeleteGroupRequest &request)const;
			void deleteGroupAsync(const Model::DeleteGroupRequest& request, const DeleteGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteGroupOutcomeCallable deleteGroupCallable(const Model::DeleteGroupRequest& request) const;
			LinkFaceOutcome linkFace(const Model::LinkFaceRequest &request)const;
			void linkFaceAsync(const Model::LinkFaceRequest& request, const LinkFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			LinkFaceOutcomeCallable linkFaceCallable(const Model::LinkFaceRequest& request) const;
			QueryAddUserInfoOutcome queryAddUserInfo(const Model::QueryAddUserInfoRequest &request)const;
			void queryAddUserInfoAsync(const Model::QueryAddUserInfoRequest& request, const QueryAddUserInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryAddUserInfoOutcomeCallable queryAddUserInfoCallable(const Model::QueryAddUserInfoRequest& request) const;
			QueryAllGroupsOutcome queryAllGroups(const Model::QueryAllGroupsRequest &request)const;
			void queryAllGroupsAsync(const Model::QueryAllGroupsRequest& request, const QueryAllGroupsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryAllGroupsOutcomeCallable queryAllGroupsCallable(const Model::QueryAllGroupsRequest& request) const;
			QueryAuthenticationOutcome queryAuthentication(const Model::QueryAuthenticationRequest &request)const;
			void queryAuthenticationAsync(const Model::QueryAuthenticationRequest& request, const QueryAuthenticationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryAuthenticationOutcomeCallable queryAuthenticationCallable(const Model::QueryAuthenticationRequest& request) const;
			QueryFaceOutcome queryFace(const Model::QueryFaceRequest &request)const;
			void queryFaceAsync(const Model::QueryFaceRequest& request, const QueryFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryFaceOutcomeCallable queryFaceCallable(const Model::QueryFaceRequest& request) const;
			QueryGroupUsersOutcome queryGroupUsers(const Model::QueryGroupUsersRequest &request)const;
			void queryGroupUsersAsync(const Model::QueryGroupUsersRequest& request, const QueryGroupUsersAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryGroupUsersOutcomeCallable queryGroupUsersCallable(const Model::QueryGroupUsersRequest& request) const;
			QueryLicensesOutcome queryLicenses(const Model::QueryLicensesRequest &request)const;
			void queryLicensesAsync(const Model::QueryLicensesRequest& request, const QueryLicensesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryLicensesOutcomeCallable queryLicensesCallable(const Model::QueryLicensesRequest& request) const;
			QuerySyncPicScheduleOutcome querySyncPicSchedule(const Model::QuerySyncPicScheduleRequest &request)const;
			void querySyncPicScheduleAsync(const Model::QuerySyncPicScheduleRequest& request, const QuerySyncPicScheduleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QuerySyncPicScheduleOutcomeCallable querySyncPicScheduleCallable(const Model::QuerySyncPicScheduleRequest& request) const;
			RegisterFaceOutcome registerFace(const Model::RegisterFaceRequest &request)const;
			void registerFaceAsync(const Model::RegisterFaceRequest& request, const RegisterFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RegisterFaceOutcomeCallable registerFaceCallable(const Model::RegisterFaceRequest& request) const;
			SearchFaceOutcome searchFace(const Model::SearchFaceRequest &request)const;
			void searchFaceAsync(const Model::SearchFaceRequest& request, const SearchFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SearchFaceOutcomeCallable searchFaceCallable(const Model::SearchFaceRequest& request) const;
			SyncFacePicturesOutcome syncFacePictures(const Model::SyncFacePicturesRequest &request)const;
			void syncFacePicturesAsync(const Model::SyncFacePicturesRequest& request, const SyncFacePicturesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SyncFacePicturesOutcomeCallable syncFacePicturesCallable(const Model::SyncFacePicturesRequest& request) const;
			UnlinkFaceOutcome unlinkFace(const Model::UnlinkFaceRequest &request)const;
			void unlinkFaceAsync(const Model::UnlinkFaceRequest& request, const UnlinkFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UnlinkFaceOutcomeCallable unlinkFaceCallable(const Model::UnlinkFaceRequest& request) const;
			UpdateFaceOutcome updateFace(const Model::UpdateFaceRequest &request)const;
			void updateFaceAsync(const Model::UpdateFaceRequest& request, const UpdateFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateFaceOutcomeCallable updateFaceCallable(const Model::UpdateFaceRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_LINKFACE_LINKFACECLIENT_H_

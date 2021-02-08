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

#include <alibabacloud/linkface/LinkFaceClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::LinkFace;
using namespace AlibabaCloud::LinkFace::Model;

namespace
{
	const std::string SERVICE_NAME = "LinkFace";
}

LinkFaceClient::LinkFaceClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "linkface");
}

LinkFaceClient::LinkFaceClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "linkface");
}

LinkFaceClient::LinkFaceClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "linkface");
}

LinkFaceClient::~LinkFaceClient()
{}

LinkFaceClient::CreateGroupOutcome LinkFaceClient::createGroup(const CreateGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateGroupOutcome(CreateGroupResult(outcome.result()));
	else
		return CreateGroupOutcome(outcome.error());
}

void LinkFaceClient::createGroupAsync(const CreateGroupRequest& request, const CreateGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::CreateGroupOutcomeCallable LinkFaceClient::createGroupCallable(const CreateGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateGroupOutcome()>>(
			[this, request]()
			{
			return this->createGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::DeleteDeviceAllGroupOutcome LinkFaceClient::deleteDeviceAllGroup(const DeleteDeviceAllGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteDeviceAllGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteDeviceAllGroupOutcome(DeleteDeviceAllGroupResult(outcome.result()));
	else
		return DeleteDeviceAllGroupOutcome(outcome.error());
}

void LinkFaceClient::deleteDeviceAllGroupAsync(const DeleteDeviceAllGroupRequest& request, const DeleteDeviceAllGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteDeviceAllGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::DeleteDeviceAllGroupOutcomeCallable LinkFaceClient::deleteDeviceAllGroupCallable(const DeleteDeviceAllGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteDeviceAllGroupOutcome()>>(
			[this, request]()
			{
			return this->deleteDeviceAllGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::DeleteDeviceGroupOutcome LinkFaceClient::deleteDeviceGroup(const DeleteDeviceGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteDeviceGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteDeviceGroupOutcome(DeleteDeviceGroupResult(outcome.result()));
	else
		return DeleteDeviceGroupOutcome(outcome.error());
}

void LinkFaceClient::deleteDeviceGroupAsync(const DeleteDeviceGroupRequest& request, const DeleteDeviceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteDeviceGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::DeleteDeviceGroupOutcomeCallable LinkFaceClient::deleteDeviceGroupCallable(const DeleteDeviceGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteDeviceGroupOutcome()>>(
			[this, request]()
			{
			return this->deleteDeviceGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::DeleteFaceOutcome LinkFaceClient::deleteFace(const DeleteFaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteFaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteFaceOutcome(DeleteFaceResult(outcome.result()));
	else
		return DeleteFaceOutcome(outcome.error());
}

void LinkFaceClient::deleteFaceAsync(const DeleteFaceRequest& request, const DeleteFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteFace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::DeleteFaceOutcomeCallable LinkFaceClient::deleteFaceCallable(const DeleteFaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteFaceOutcome()>>(
			[this, request]()
			{
			return this->deleteFace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::DeleteGroupOutcome LinkFaceClient::deleteGroup(const DeleteGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteGroupOutcome(DeleteGroupResult(outcome.result()));
	else
		return DeleteGroupOutcome(outcome.error());
}

void LinkFaceClient::deleteGroupAsync(const DeleteGroupRequest& request, const DeleteGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::DeleteGroupOutcomeCallable LinkFaceClient::deleteGroupCallable(const DeleteGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteGroupOutcome()>>(
			[this, request]()
			{
			return this->deleteGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::LinkFaceOutcome LinkFaceClient::linkFace(const LinkFaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return LinkFaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return LinkFaceOutcome(LinkFaceResult(outcome.result()));
	else
		return LinkFaceOutcome(outcome.error());
}

void LinkFaceClient::linkFaceAsync(const LinkFaceRequest& request, const LinkFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, linkFace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::LinkFaceOutcomeCallable LinkFaceClient::linkFaceCallable(const LinkFaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<LinkFaceOutcome()>>(
			[this, request]()
			{
			return this->linkFace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::QueryAddUserInfoOutcome LinkFaceClient::queryAddUserInfo(const QueryAddUserInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryAddUserInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryAddUserInfoOutcome(QueryAddUserInfoResult(outcome.result()));
	else
		return QueryAddUserInfoOutcome(outcome.error());
}

void LinkFaceClient::queryAddUserInfoAsync(const QueryAddUserInfoRequest& request, const QueryAddUserInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryAddUserInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::QueryAddUserInfoOutcomeCallable LinkFaceClient::queryAddUserInfoCallable(const QueryAddUserInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryAddUserInfoOutcome()>>(
			[this, request]()
			{
			return this->queryAddUserInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::QueryAllGroupsOutcome LinkFaceClient::queryAllGroups(const QueryAllGroupsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryAllGroupsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryAllGroupsOutcome(QueryAllGroupsResult(outcome.result()));
	else
		return QueryAllGroupsOutcome(outcome.error());
}

void LinkFaceClient::queryAllGroupsAsync(const QueryAllGroupsRequest& request, const QueryAllGroupsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryAllGroups(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::QueryAllGroupsOutcomeCallable LinkFaceClient::queryAllGroupsCallable(const QueryAllGroupsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryAllGroupsOutcome()>>(
			[this, request]()
			{
			return this->queryAllGroups(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::QueryAuthenticationOutcome LinkFaceClient::queryAuthentication(const QueryAuthenticationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryAuthenticationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryAuthenticationOutcome(QueryAuthenticationResult(outcome.result()));
	else
		return QueryAuthenticationOutcome(outcome.error());
}

void LinkFaceClient::queryAuthenticationAsync(const QueryAuthenticationRequest& request, const QueryAuthenticationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryAuthentication(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::QueryAuthenticationOutcomeCallable LinkFaceClient::queryAuthenticationCallable(const QueryAuthenticationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryAuthenticationOutcome()>>(
			[this, request]()
			{
			return this->queryAuthentication(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::QueryFaceOutcome LinkFaceClient::queryFace(const QueryFaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryFaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryFaceOutcome(QueryFaceResult(outcome.result()));
	else
		return QueryFaceOutcome(outcome.error());
}

void LinkFaceClient::queryFaceAsync(const QueryFaceRequest& request, const QueryFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryFace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::QueryFaceOutcomeCallable LinkFaceClient::queryFaceCallable(const QueryFaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryFaceOutcome()>>(
			[this, request]()
			{
			return this->queryFace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::QueryGroupUsersOutcome LinkFaceClient::queryGroupUsers(const QueryGroupUsersRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryGroupUsersOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryGroupUsersOutcome(QueryGroupUsersResult(outcome.result()));
	else
		return QueryGroupUsersOutcome(outcome.error());
}

void LinkFaceClient::queryGroupUsersAsync(const QueryGroupUsersRequest& request, const QueryGroupUsersAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryGroupUsers(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::QueryGroupUsersOutcomeCallable LinkFaceClient::queryGroupUsersCallable(const QueryGroupUsersRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryGroupUsersOutcome()>>(
			[this, request]()
			{
			return this->queryGroupUsers(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::QueryLicensesOutcome LinkFaceClient::queryLicenses(const QueryLicensesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryLicensesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryLicensesOutcome(QueryLicensesResult(outcome.result()));
	else
		return QueryLicensesOutcome(outcome.error());
}

void LinkFaceClient::queryLicensesAsync(const QueryLicensesRequest& request, const QueryLicensesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryLicenses(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::QueryLicensesOutcomeCallable LinkFaceClient::queryLicensesCallable(const QueryLicensesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryLicensesOutcome()>>(
			[this, request]()
			{
			return this->queryLicenses(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::QuerySyncPicScheduleOutcome LinkFaceClient::querySyncPicSchedule(const QuerySyncPicScheduleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QuerySyncPicScheduleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QuerySyncPicScheduleOutcome(QuerySyncPicScheduleResult(outcome.result()));
	else
		return QuerySyncPicScheduleOutcome(outcome.error());
}

void LinkFaceClient::querySyncPicScheduleAsync(const QuerySyncPicScheduleRequest& request, const QuerySyncPicScheduleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, querySyncPicSchedule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::QuerySyncPicScheduleOutcomeCallable LinkFaceClient::querySyncPicScheduleCallable(const QuerySyncPicScheduleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QuerySyncPicScheduleOutcome()>>(
			[this, request]()
			{
			return this->querySyncPicSchedule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::RegisterFaceOutcome LinkFaceClient::registerFace(const RegisterFaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RegisterFaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RegisterFaceOutcome(RegisterFaceResult(outcome.result()));
	else
		return RegisterFaceOutcome(outcome.error());
}

void LinkFaceClient::registerFaceAsync(const RegisterFaceRequest& request, const RegisterFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, registerFace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::RegisterFaceOutcomeCallable LinkFaceClient::registerFaceCallable(const RegisterFaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RegisterFaceOutcome()>>(
			[this, request]()
			{
			return this->registerFace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::SearchFaceOutcome LinkFaceClient::searchFace(const SearchFaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SearchFaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SearchFaceOutcome(SearchFaceResult(outcome.result()));
	else
		return SearchFaceOutcome(outcome.error());
}

void LinkFaceClient::searchFaceAsync(const SearchFaceRequest& request, const SearchFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, searchFace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::SearchFaceOutcomeCallable LinkFaceClient::searchFaceCallable(const SearchFaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SearchFaceOutcome()>>(
			[this, request]()
			{
			return this->searchFace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::SyncFacePicturesOutcome LinkFaceClient::syncFacePictures(const SyncFacePicturesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SyncFacePicturesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SyncFacePicturesOutcome(SyncFacePicturesResult(outcome.result()));
	else
		return SyncFacePicturesOutcome(outcome.error());
}

void LinkFaceClient::syncFacePicturesAsync(const SyncFacePicturesRequest& request, const SyncFacePicturesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, syncFacePictures(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::SyncFacePicturesOutcomeCallable LinkFaceClient::syncFacePicturesCallable(const SyncFacePicturesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SyncFacePicturesOutcome()>>(
			[this, request]()
			{
			return this->syncFacePictures(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::UnlinkFaceOutcome LinkFaceClient::unlinkFace(const UnlinkFaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UnlinkFaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UnlinkFaceOutcome(UnlinkFaceResult(outcome.result()));
	else
		return UnlinkFaceOutcome(outcome.error());
}

void LinkFaceClient::unlinkFaceAsync(const UnlinkFaceRequest& request, const UnlinkFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, unlinkFace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::UnlinkFaceOutcomeCallable LinkFaceClient::unlinkFaceCallable(const UnlinkFaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UnlinkFaceOutcome()>>(
			[this, request]()
			{
			return this->unlinkFace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LinkFaceClient::UpdateFaceOutcome LinkFaceClient::updateFace(const UpdateFaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateFaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateFaceOutcome(UpdateFaceResult(outcome.result()));
	else
		return UpdateFaceOutcome(outcome.error());
}

void LinkFaceClient::updateFaceAsync(const UpdateFaceRequest& request, const UpdateFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateFace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LinkFaceClient::UpdateFaceOutcomeCallable LinkFaceClient::updateFaceCallable(const UpdateFaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateFaceOutcome()>>(
			[this, request]()
			{
			return this->updateFace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


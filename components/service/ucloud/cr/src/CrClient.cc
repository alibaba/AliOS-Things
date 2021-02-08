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

#include <alibabacloud/cr/CrClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Cr;
using namespace AlibabaCloud::Cr::Model;

namespace
{
	const std::string SERVICE_NAME = "cr";
}

CrClient::CrClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RoaServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "cr");
}

CrClient::CrClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RoaServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "cr");
}

CrClient::CrClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RoaServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "cr");
}

CrClient::~CrClient()
{}

CrClient::CancelRepoBuildOutcome CrClient::cancelRepoBuild(const CancelRepoBuildRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CancelRepoBuildOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CancelRepoBuildOutcome(CancelRepoBuildResult(outcome.result()));
	else
		return CancelRepoBuildOutcome(outcome.error());
}

void CrClient::cancelRepoBuildAsync(const CancelRepoBuildRequest& request, const CancelRepoBuildAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, cancelRepoBuild(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::CancelRepoBuildOutcomeCallable CrClient::cancelRepoBuildCallable(const CancelRepoBuildRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CancelRepoBuildOutcome()>>(
			[this, request]()
			{
			return this->cancelRepoBuild(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::CreateCollectionOutcome CrClient::createCollection(const CreateCollectionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateCollectionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateCollectionOutcome(CreateCollectionResult(outcome.result()));
	else
		return CreateCollectionOutcome(outcome.error());
}

void CrClient::createCollectionAsync(const CreateCollectionRequest& request, const CreateCollectionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createCollection(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::CreateCollectionOutcomeCallable CrClient::createCollectionCallable(const CreateCollectionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateCollectionOutcome()>>(
			[this, request]()
			{
			return this->createCollection(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::CreateNamespaceOutcome CrClient::createNamespace(const CreateNamespaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateNamespaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateNamespaceOutcome(CreateNamespaceResult(outcome.result()));
	else
		return CreateNamespaceOutcome(outcome.error());
}

void CrClient::createNamespaceAsync(const CreateNamespaceRequest& request, const CreateNamespaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createNamespace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::CreateNamespaceOutcomeCallable CrClient::createNamespaceCallable(const CreateNamespaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateNamespaceOutcome()>>(
			[this, request]()
			{
			return this->createNamespace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::CreateNamespaceAuthorizationOutcome CrClient::createNamespaceAuthorization(const CreateNamespaceAuthorizationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateNamespaceAuthorizationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateNamespaceAuthorizationOutcome(CreateNamespaceAuthorizationResult(outcome.result()));
	else
		return CreateNamespaceAuthorizationOutcome(outcome.error());
}

void CrClient::createNamespaceAuthorizationAsync(const CreateNamespaceAuthorizationRequest& request, const CreateNamespaceAuthorizationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createNamespaceAuthorization(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::CreateNamespaceAuthorizationOutcomeCallable CrClient::createNamespaceAuthorizationCallable(const CreateNamespaceAuthorizationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateNamespaceAuthorizationOutcome()>>(
			[this, request]()
			{
			return this->createNamespaceAuthorization(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::CreateRepoOutcome CrClient::createRepo(const CreateRepoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateRepoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateRepoOutcome(CreateRepoResult(outcome.result()));
	else
		return CreateRepoOutcome(outcome.error());
}

void CrClient::createRepoAsync(const CreateRepoRequest& request, const CreateRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createRepo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::CreateRepoOutcomeCallable CrClient::createRepoCallable(const CreateRepoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateRepoOutcome()>>(
			[this, request]()
			{
			return this->createRepo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::CreateRepoAuthorizationOutcome CrClient::createRepoAuthorization(const CreateRepoAuthorizationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateRepoAuthorizationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateRepoAuthorizationOutcome(CreateRepoAuthorizationResult(outcome.result()));
	else
		return CreateRepoAuthorizationOutcome(outcome.error());
}

void CrClient::createRepoAuthorizationAsync(const CreateRepoAuthorizationRequest& request, const CreateRepoAuthorizationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createRepoAuthorization(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::CreateRepoAuthorizationOutcomeCallable CrClient::createRepoAuthorizationCallable(const CreateRepoAuthorizationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateRepoAuthorizationOutcome()>>(
			[this, request]()
			{
			return this->createRepoAuthorization(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::CreateRepoBuildRuleOutcome CrClient::createRepoBuildRule(const CreateRepoBuildRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateRepoBuildRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateRepoBuildRuleOutcome(CreateRepoBuildRuleResult(outcome.result()));
	else
		return CreateRepoBuildRuleOutcome(outcome.error());
}

void CrClient::createRepoBuildRuleAsync(const CreateRepoBuildRuleRequest& request, const CreateRepoBuildRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createRepoBuildRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::CreateRepoBuildRuleOutcomeCallable CrClient::createRepoBuildRuleCallable(const CreateRepoBuildRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateRepoBuildRuleOutcome()>>(
			[this, request]()
			{
			return this->createRepoBuildRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::CreateRepoSyncTaskOutcome CrClient::createRepoSyncTask(const CreateRepoSyncTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateRepoSyncTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateRepoSyncTaskOutcome(CreateRepoSyncTaskResult(outcome.result()));
	else
		return CreateRepoSyncTaskOutcome(outcome.error());
}

void CrClient::createRepoSyncTaskAsync(const CreateRepoSyncTaskRequest& request, const CreateRepoSyncTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createRepoSyncTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::CreateRepoSyncTaskOutcomeCallable CrClient::createRepoSyncTaskCallable(const CreateRepoSyncTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateRepoSyncTaskOutcome()>>(
			[this, request]()
			{
			return this->createRepoSyncTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::CreateRepoWebhookOutcome CrClient::createRepoWebhook(const CreateRepoWebhookRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateRepoWebhookOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateRepoWebhookOutcome(CreateRepoWebhookResult(outcome.result()));
	else
		return CreateRepoWebhookOutcome(outcome.error());
}

void CrClient::createRepoWebhookAsync(const CreateRepoWebhookRequest& request, const CreateRepoWebhookAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createRepoWebhook(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::CreateRepoWebhookOutcomeCallable CrClient::createRepoWebhookCallable(const CreateRepoWebhookRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateRepoWebhookOutcome()>>(
			[this, request]()
			{
			return this->createRepoWebhook(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::CreateUserInfoOutcome CrClient::createUserInfo(const CreateUserInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateUserInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateUserInfoOutcome(CreateUserInfoResult(outcome.result()));
	else
		return CreateUserInfoOutcome(outcome.error());
}

void CrClient::createUserInfoAsync(const CreateUserInfoRequest& request, const CreateUserInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createUserInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::CreateUserInfoOutcomeCallable CrClient::createUserInfoCallable(const CreateUserInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateUserInfoOutcome()>>(
			[this, request]()
			{
			return this->createUserInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::CreateUserSourceAccountOutcome CrClient::createUserSourceAccount(const CreateUserSourceAccountRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateUserSourceAccountOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateUserSourceAccountOutcome(CreateUserSourceAccountResult(outcome.result()));
	else
		return CreateUserSourceAccountOutcome(outcome.error());
}

void CrClient::createUserSourceAccountAsync(const CreateUserSourceAccountRequest& request, const CreateUserSourceAccountAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createUserSourceAccount(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::CreateUserSourceAccountOutcomeCallable CrClient::createUserSourceAccountCallable(const CreateUserSourceAccountRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateUserSourceAccountOutcome()>>(
			[this, request]()
			{
			return this->createUserSourceAccount(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::DeleteCollectionOutcome CrClient::deleteCollection(const DeleteCollectionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteCollectionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteCollectionOutcome(DeleteCollectionResult(outcome.result()));
	else
		return DeleteCollectionOutcome(outcome.error());
}

void CrClient::deleteCollectionAsync(const DeleteCollectionRequest& request, const DeleteCollectionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteCollection(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::DeleteCollectionOutcomeCallable CrClient::deleteCollectionCallable(const DeleteCollectionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteCollectionOutcome()>>(
			[this, request]()
			{
			return this->deleteCollection(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::DeleteImageOutcome CrClient::deleteImage(const DeleteImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteImageOutcome(DeleteImageResult(outcome.result()));
	else
		return DeleteImageOutcome(outcome.error());
}

void CrClient::deleteImageAsync(const DeleteImageRequest& request, const DeleteImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::DeleteImageOutcomeCallable CrClient::deleteImageCallable(const DeleteImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteImageOutcome()>>(
			[this, request]()
			{
			return this->deleteImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::DeleteNamespaceOutcome CrClient::deleteNamespace(const DeleteNamespaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteNamespaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteNamespaceOutcome(DeleteNamespaceResult(outcome.result()));
	else
		return DeleteNamespaceOutcome(outcome.error());
}

void CrClient::deleteNamespaceAsync(const DeleteNamespaceRequest& request, const DeleteNamespaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteNamespace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::DeleteNamespaceOutcomeCallable CrClient::deleteNamespaceCallable(const DeleteNamespaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteNamespaceOutcome()>>(
			[this, request]()
			{
			return this->deleteNamespace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::DeleteNamespaceAuthorizationOutcome CrClient::deleteNamespaceAuthorization(const DeleteNamespaceAuthorizationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteNamespaceAuthorizationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteNamespaceAuthorizationOutcome(DeleteNamespaceAuthorizationResult(outcome.result()));
	else
		return DeleteNamespaceAuthorizationOutcome(outcome.error());
}

void CrClient::deleteNamespaceAuthorizationAsync(const DeleteNamespaceAuthorizationRequest& request, const DeleteNamespaceAuthorizationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteNamespaceAuthorization(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::DeleteNamespaceAuthorizationOutcomeCallable CrClient::deleteNamespaceAuthorizationCallable(const DeleteNamespaceAuthorizationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteNamespaceAuthorizationOutcome()>>(
			[this, request]()
			{
			return this->deleteNamespaceAuthorization(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::DeleteRepoOutcome CrClient::deleteRepo(const DeleteRepoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteRepoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteRepoOutcome(DeleteRepoResult(outcome.result()));
	else
		return DeleteRepoOutcome(outcome.error());
}

void CrClient::deleteRepoAsync(const DeleteRepoRequest& request, const DeleteRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteRepo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::DeleteRepoOutcomeCallable CrClient::deleteRepoCallable(const DeleteRepoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteRepoOutcome()>>(
			[this, request]()
			{
			return this->deleteRepo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::DeleteRepoAuthorizationOutcome CrClient::deleteRepoAuthorization(const DeleteRepoAuthorizationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteRepoAuthorizationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteRepoAuthorizationOutcome(DeleteRepoAuthorizationResult(outcome.result()));
	else
		return DeleteRepoAuthorizationOutcome(outcome.error());
}

void CrClient::deleteRepoAuthorizationAsync(const DeleteRepoAuthorizationRequest& request, const DeleteRepoAuthorizationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteRepoAuthorization(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::DeleteRepoAuthorizationOutcomeCallable CrClient::deleteRepoAuthorizationCallable(const DeleteRepoAuthorizationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteRepoAuthorizationOutcome()>>(
			[this, request]()
			{
			return this->deleteRepoAuthorization(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::DeleteRepoBuildRuleOutcome CrClient::deleteRepoBuildRule(const DeleteRepoBuildRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteRepoBuildRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteRepoBuildRuleOutcome(DeleteRepoBuildRuleResult(outcome.result()));
	else
		return DeleteRepoBuildRuleOutcome(outcome.error());
}

void CrClient::deleteRepoBuildRuleAsync(const DeleteRepoBuildRuleRequest& request, const DeleteRepoBuildRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteRepoBuildRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::DeleteRepoBuildRuleOutcomeCallable CrClient::deleteRepoBuildRuleCallable(const DeleteRepoBuildRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteRepoBuildRuleOutcome()>>(
			[this, request]()
			{
			return this->deleteRepoBuildRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::DeleteRepoWebhookOutcome CrClient::deleteRepoWebhook(const DeleteRepoWebhookRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteRepoWebhookOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteRepoWebhookOutcome(DeleteRepoWebhookResult(outcome.result()));
	else
		return DeleteRepoWebhookOutcome(outcome.error());
}

void CrClient::deleteRepoWebhookAsync(const DeleteRepoWebhookRequest& request, const DeleteRepoWebhookAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteRepoWebhook(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::DeleteRepoWebhookOutcomeCallable CrClient::deleteRepoWebhookCallable(const DeleteRepoWebhookRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteRepoWebhookOutcome()>>(
			[this, request]()
			{
			return this->deleteRepoWebhook(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::DeleteUserSourceAccountOutcome CrClient::deleteUserSourceAccount(const DeleteUserSourceAccountRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteUserSourceAccountOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteUserSourceAccountOutcome(DeleteUserSourceAccountResult(outcome.result()));
	else
		return DeleteUserSourceAccountOutcome(outcome.error());
}

void CrClient::deleteUserSourceAccountAsync(const DeleteUserSourceAccountRequest& request, const DeleteUserSourceAccountAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteUserSourceAccount(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::DeleteUserSourceAccountOutcomeCallable CrClient::deleteUserSourceAccountCallable(const DeleteUserSourceAccountRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteUserSourceAccountOutcome()>>(
			[this, request]()
			{
			return this->deleteUserSourceAccount(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetAuthorizationTokenOutcome CrClient::getAuthorizationToken(const GetAuthorizationTokenRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetAuthorizationTokenOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetAuthorizationTokenOutcome(GetAuthorizationTokenResult(outcome.result()));
	else
		return GetAuthorizationTokenOutcome(outcome.error());
}

void CrClient::getAuthorizationTokenAsync(const GetAuthorizationTokenRequest& request, const GetAuthorizationTokenAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getAuthorizationToken(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetAuthorizationTokenOutcomeCallable CrClient::getAuthorizationTokenCallable(const GetAuthorizationTokenRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetAuthorizationTokenOutcome()>>(
			[this, request]()
			{
			return this->getAuthorizationToken(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetCollectionOutcome CrClient::getCollection(const GetCollectionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetCollectionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetCollectionOutcome(GetCollectionResult(outcome.result()));
	else
		return GetCollectionOutcome(outcome.error());
}

void CrClient::getCollectionAsync(const GetCollectionRequest& request, const GetCollectionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getCollection(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetCollectionOutcomeCallable CrClient::getCollectionCallable(const GetCollectionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetCollectionOutcome()>>(
			[this, request]()
			{
			return this->getCollection(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetImageLayerOutcome CrClient::getImageLayer(const GetImageLayerRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetImageLayerOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetImageLayerOutcome(GetImageLayerResult(outcome.result()));
	else
		return GetImageLayerOutcome(outcome.error());
}

void CrClient::getImageLayerAsync(const GetImageLayerRequest& request, const GetImageLayerAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getImageLayer(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetImageLayerOutcomeCallable CrClient::getImageLayerCallable(const GetImageLayerRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetImageLayerOutcome()>>(
			[this, request]()
			{
			return this->getImageLayer(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetImageManifestOutcome CrClient::getImageManifest(const GetImageManifestRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetImageManifestOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetImageManifestOutcome(GetImageManifestResult(outcome.result()));
	else
		return GetImageManifestOutcome(outcome.error());
}

void CrClient::getImageManifestAsync(const GetImageManifestRequest& request, const GetImageManifestAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getImageManifest(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetImageManifestOutcomeCallable CrClient::getImageManifestCallable(const GetImageManifestRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetImageManifestOutcome()>>(
			[this, request]()
			{
			return this->getImageManifest(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetImageScanOutcome CrClient::getImageScan(const GetImageScanRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetImageScanOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetImageScanOutcome(GetImageScanResult(outcome.result()));
	else
		return GetImageScanOutcome(outcome.error());
}

void CrClient::getImageScanAsync(const GetImageScanRequest& request, const GetImageScanAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getImageScan(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetImageScanOutcomeCallable CrClient::getImageScanCallable(const GetImageScanRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetImageScanOutcome()>>(
			[this, request]()
			{
			return this->getImageScan(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetMirrorListOutcome CrClient::getMirrorList(const GetMirrorListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetMirrorListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetMirrorListOutcome(GetMirrorListResult(outcome.result()));
	else
		return GetMirrorListOutcome(outcome.error());
}

void CrClient::getMirrorListAsync(const GetMirrorListRequest& request, const GetMirrorListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getMirrorList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetMirrorListOutcomeCallable CrClient::getMirrorListCallable(const GetMirrorListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetMirrorListOutcome()>>(
			[this, request]()
			{
			return this->getMirrorList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetNamespaceOutcome CrClient::getNamespace(const GetNamespaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetNamespaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetNamespaceOutcome(GetNamespaceResult(outcome.result()));
	else
		return GetNamespaceOutcome(outcome.error());
}

void CrClient::getNamespaceAsync(const GetNamespaceRequest& request, const GetNamespaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getNamespace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetNamespaceOutcomeCallable CrClient::getNamespaceCallable(const GetNamespaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetNamespaceOutcome()>>(
			[this, request]()
			{
			return this->getNamespace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetNamespaceAuthorizationListOutcome CrClient::getNamespaceAuthorizationList(const GetNamespaceAuthorizationListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetNamespaceAuthorizationListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetNamespaceAuthorizationListOutcome(GetNamespaceAuthorizationListResult(outcome.result()));
	else
		return GetNamespaceAuthorizationListOutcome(outcome.error());
}

void CrClient::getNamespaceAuthorizationListAsync(const GetNamespaceAuthorizationListRequest& request, const GetNamespaceAuthorizationListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getNamespaceAuthorizationList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetNamespaceAuthorizationListOutcomeCallable CrClient::getNamespaceAuthorizationListCallable(const GetNamespaceAuthorizationListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetNamespaceAuthorizationListOutcome()>>(
			[this, request]()
			{
			return this->getNamespaceAuthorizationList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetNamespaceListOutcome CrClient::getNamespaceList(const GetNamespaceListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetNamespaceListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetNamespaceListOutcome(GetNamespaceListResult(outcome.result()));
	else
		return GetNamespaceListOutcome(outcome.error());
}

void CrClient::getNamespaceListAsync(const GetNamespaceListRequest& request, const GetNamespaceListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getNamespaceList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetNamespaceListOutcomeCallable CrClient::getNamespaceListCallable(const GetNamespaceListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetNamespaceListOutcome()>>(
			[this, request]()
			{
			return this->getNamespaceList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRegionOutcome CrClient::getRegion(const GetRegionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRegionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRegionOutcome(GetRegionResult(outcome.result()));
	else
		return GetRegionOutcome(outcome.error());
}

void CrClient::getRegionAsync(const GetRegionRequest& request, const GetRegionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRegion(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRegionOutcomeCallable CrClient::getRegionCallable(const GetRegionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRegionOutcome()>>(
			[this, request]()
			{
			return this->getRegion(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRegionListOutcome CrClient::getRegionList(const GetRegionListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRegionListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRegionListOutcome(GetRegionListResult(outcome.result()));
	else
		return GetRegionListOutcome(outcome.error());
}

void CrClient::getRegionListAsync(const GetRegionListRequest& request, const GetRegionListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRegionList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRegionListOutcomeCallable CrClient::getRegionListCallable(const GetRegionListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRegionListOutcome()>>(
			[this, request]()
			{
			return this->getRegionList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoOutcome CrClient::getRepo(const GetRepoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoOutcome(GetRepoResult(outcome.result()));
	else
		return GetRepoOutcome(outcome.error());
}

void CrClient::getRepoAsync(const GetRepoRequest& request, const GetRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoOutcomeCallable CrClient::getRepoCallable(const GetRepoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoOutcome()>>(
			[this, request]()
			{
			return this->getRepo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoAuthorizationListOutcome CrClient::getRepoAuthorizationList(const GetRepoAuthorizationListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoAuthorizationListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoAuthorizationListOutcome(GetRepoAuthorizationListResult(outcome.result()));
	else
		return GetRepoAuthorizationListOutcome(outcome.error());
}

void CrClient::getRepoAuthorizationListAsync(const GetRepoAuthorizationListRequest& request, const GetRepoAuthorizationListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoAuthorizationList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoAuthorizationListOutcomeCallable CrClient::getRepoAuthorizationListCallable(const GetRepoAuthorizationListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoAuthorizationListOutcome()>>(
			[this, request]()
			{
			return this->getRepoAuthorizationList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoBatchOutcome CrClient::getRepoBatch(const GetRepoBatchRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoBatchOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoBatchOutcome(GetRepoBatchResult(outcome.result()));
	else
		return GetRepoBatchOutcome(outcome.error());
}

void CrClient::getRepoBatchAsync(const GetRepoBatchRequest& request, const GetRepoBatchAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoBatch(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoBatchOutcomeCallable CrClient::getRepoBatchCallable(const GetRepoBatchRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoBatchOutcome()>>(
			[this, request]()
			{
			return this->getRepoBatch(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoBuildListOutcome CrClient::getRepoBuildList(const GetRepoBuildListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoBuildListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoBuildListOutcome(GetRepoBuildListResult(outcome.result()));
	else
		return GetRepoBuildListOutcome(outcome.error());
}

void CrClient::getRepoBuildListAsync(const GetRepoBuildListRequest& request, const GetRepoBuildListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoBuildList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoBuildListOutcomeCallable CrClient::getRepoBuildListCallable(const GetRepoBuildListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoBuildListOutcome()>>(
			[this, request]()
			{
			return this->getRepoBuildList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoBuildLogsOutcome CrClient::getRepoBuildLogs(const GetRepoBuildLogsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoBuildLogsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoBuildLogsOutcome(GetRepoBuildLogsResult(outcome.result()));
	else
		return GetRepoBuildLogsOutcome(outcome.error());
}

void CrClient::getRepoBuildLogsAsync(const GetRepoBuildLogsRequest& request, const GetRepoBuildLogsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoBuildLogs(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoBuildLogsOutcomeCallable CrClient::getRepoBuildLogsCallable(const GetRepoBuildLogsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoBuildLogsOutcome()>>(
			[this, request]()
			{
			return this->getRepoBuildLogs(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoBuildRuleListOutcome CrClient::getRepoBuildRuleList(const GetRepoBuildRuleListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoBuildRuleListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoBuildRuleListOutcome(GetRepoBuildRuleListResult(outcome.result()));
	else
		return GetRepoBuildRuleListOutcome(outcome.error());
}

void CrClient::getRepoBuildRuleListAsync(const GetRepoBuildRuleListRequest& request, const GetRepoBuildRuleListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoBuildRuleList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoBuildRuleListOutcomeCallable CrClient::getRepoBuildRuleListCallable(const GetRepoBuildRuleListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoBuildRuleListOutcome()>>(
			[this, request]()
			{
			return this->getRepoBuildRuleList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoBuildStatusOutcome CrClient::getRepoBuildStatus(const GetRepoBuildStatusRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoBuildStatusOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoBuildStatusOutcome(GetRepoBuildStatusResult(outcome.result()));
	else
		return GetRepoBuildStatusOutcome(outcome.error());
}

void CrClient::getRepoBuildStatusAsync(const GetRepoBuildStatusRequest& request, const GetRepoBuildStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoBuildStatus(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoBuildStatusOutcomeCallable CrClient::getRepoBuildStatusCallable(const GetRepoBuildStatusRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoBuildStatusOutcome()>>(
			[this, request]()
			{
			return this->getRepoBuildStatus(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoListOutcome CrClient::getRepoList(const GetRepoListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoListOutcome(GetRepoListResult(outcome.result()));
	else
		return GetRepoListOutcome(outcome.error());
}

void CrClient::getRepoListAsync(const GetRepoListRequest& request, const GetRepoListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoListOutcomeCallable CrClient::getRepoListCallable(const GetRepoListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoListOutcome()>>(
			[this, request]()
			{
			return this->getRepoList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoListByNamespaceOutcome CrClient::getRepoListByNamespace(const GetRepoListByNamespaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoListByNamespaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoListByNamespaceOutcome(GetRepoListByNamespaceResult(outcome.result()));
	else
		return GetRepoListByNamespaceOutcome(outcome.error());
}

void CrClient::getRepoListByNamespaceAsync(const GetRepoListByNamespaceRequest& request, const GetRepoListByNamespaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoListByNamespace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoListByNamespaceOutcomeCallable CrClient::getRepoListByNamespaceCallable(const GetRepoListByNamespaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoListByNamespaceOutcome()>>(
			[this, request]()
			{
			return this->getRepoListByNamespace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoSourceRepoOutcome CrClient::getRepoSourceRepo(const GetRepoSourceRepoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoSourceRepoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoSourceRepoOutcome(GetRepoSourceRepoResult(outcome.result()));
	else
		return GetRepoSourceRepoOutcome(outcome.error());
}

void CrClient::getRepoSourceRepoAsync(const GetRepoSourceRepoRequest& request, const GetRepoSourceRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoSourceRepo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoSourceRepoOutcomeCallable CrClient::getRepoSourceRepoCallable(const GetRepoSourceRepoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoSourceRepoOutcome()>>(
			[this, request]()
			{
			return this->getRepoSourceRepo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoSyncTaskOutcome CrClient::getRepoSyncTask(const GetRepoSyncTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoSyncTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoSyncTaskOutcome(GetRepoSyncTaskResult(outcome.result()));
	else
		return GetRepoSyncTaskOutcome(outcome.error());
}

void CrClient::getRepoSyncTaskAsync(const GetRepoSyncTaskRequest& request, const GetRepoSyncTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoSyncTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoSyncTaskOutcomeCallable CrClient::getRepoSyncTaskCallable(const GetRepoSyncTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoSyncTaskOutcome()>>(
			[this, request]()
			{
			return this->getRepoSyncTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoSyncTaskListOutcome CrClient::getRepoSyncTaskList(const GetRepoSyncTaskListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoSyncTaskListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoSyncTaskListOutcome(GetRepoSyncTaskListResult(outcome.result()));
	else
		return GetRepoSyncTaskListOutcome(outcome.error());
}

void CrClient::getRepoSyncTaskListAsync(const GetRepoSyncTaskListRequest& request, const GetRepoSyncTaskListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoSyncTaskList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoSyncTaskListOutcomeCallable CrClient::getRepoSyncTaskListCallable(const GetRepoSyncTaskListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoSyncTaskListOutcome()>>(
			[this, request]()
			{
			return this->getRepoSyncTaskList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoTagsOutcome CrClient::getRepoTags(const GetRepoTagsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoTagsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoTagsOutcome(GetRepoTagsResult(outcome.result()));
	else
		return GetRepoTagsOutcome(outcome.error());
}

void CrClient::getRepoTagsAsync(const GetRepoTagsRequest& request, const GetRepoTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoTags(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoTagsOutcomeCallable CrClient::getRepoTagsCallable(const GetRepoTagsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoTagsOutcome()>>(
			[this, request]()
			{
			return this->getRepoTags(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoWebhookOutcome CrClient::getRepoWebhook(const GetRepoWebhookRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoWebhookOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoWebhookOutcome(GetRepoWebhookResult(outcome.result()));
	else
		return GetRepoWebhookOutcome(outcome.error());
}

void CrClient::getRepoWebhookAsync(const GetRepoWebhookRequest& request, const GetRepoWebhookAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoWebhook(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoWebhookOutcomeCallable CrClient::getRepoWebhookCallable(const GetRepoWebhookRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoWebhookOutcome()>>(
			[this, request]()
			{
			return this->getRepoWebhook(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetRepoWebhookLogListOutcome CrClient::getRepoWebhookLogList(const GetRepoWebhookLogListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRepoWebhookLogListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRepoWebhookLogListOutcome(GetRepoWebhookLogListResult(outcome.result()));
	else
		return GetRepoWebhookLogListOutcome(outcome.error());
}

void CrClient::getRepoWebhookLogListAsync(const GetRepoWebhookLogListRequest& request, const GetRepoWebhookLogListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRepoWebhookLogList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetRepoWebhookLogListOutcomeCallable CrClient::getRepoWebhookLogListCallable(const GetRepoWebhookLogListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRepoWebhookLogListOutcome()>>(
			[this, request]()
			{
			return this->getRepoWebhookLogList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetSearchOutcome CrClient::getSearch(const GetSearchRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetSearchOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetSearchOutcome(GetSearchResult(outcome.result()));
	else
		return GetSearchOutcome(outcome.error());
}

void CrClient::getSearchAsync(const GetSearchRequest& request, const GetSearchAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getSearch(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetSearchOutcomeCallable CrClient::getSearchCallable(const GetSearchRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetSearchOutcome()>>(
			[this, request]()
			{
			return this->getSearch(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetSubUserListOutcome CrClient::getSubUserList(const GetSubUserListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetSubUserListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetSubUserListOutcome(GetSubUserListResult(outcome.result()));
	else
		return GetSubUserListOutcome(outcome.error());
}

void CrClient::getSubUserListAsync(const GetSubUserListRequest& request, const GetSubUserListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getSubUserList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetSubUserListOutcomeCallable CrClient::getSubUserListCallable(const GetSubUserListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetSubUserListOutcome()>>(
			[this, request]()
			{
			return this->getSubUserList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetUserInfoOutcome CrClient::getUserInfo(const GetUserInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetUserInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetUserInfoOutcome(GetUserInfoResult(outcome.result()));
	else
		return GetUserInfoOutcome(outcome.error());
}

void CrClient::getUserInfoAsync(const GetUserInfoRequest& request, const GetUserInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getUserInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetUserInfoOutcomeCallable CrClient::getUserInfoCallable(const GetUserInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetUserInfoOutcome()>>(
			[this, request]()
			{
			return this->getUserInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetUserSourceAccountOutcome CrClient::getUserSourceAccount(const GetUserSourceAccountRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetUserSourceAccountOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetUserSourceAccountOutcome(GetUserSourceAccountResult(outcome.result()));
	else
		return GetUserSourceAccountOutcome(outcome.error());
}

void CrClient::getUserSourceAccountAsync(const GetUserSourceAccountRequest& request, const GetUserSourceAccountAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getUserSourceAccount(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetUserSourceAccountOutcomeCallable CrClient::getUserSourceAccountCallable(const GetUserSourceAccountRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetUserSourceAccountOutcome()>>(
			[this, request]()
			{
			return this->getUserSourceAccount(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetUserSourceRepoListOutcome CrClient::getUserSourceRepoList(const GetUserSourceRepoListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetUserSourceRepoListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetUserSourceRepoListOutcome(GetUserSourceRepoListResult(outcome.result()));
	else
		return GetUserSourceRepoListOutcome(outcome.error());
}

void CrClient::getUserSourceRepoListAsync(const GetUserSourceRepoListRequest& request, const GetUserSourceRepoListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getUserSourceRepoList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetUserSourceRepoListOutcomeCallable CrClient::getUserSourceRepoListCallable(const GetUserSourceRepoListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetUserSourceRepoListOutcome()>>(
			[this, request]()
			{
			return this->getUserSourceRepoList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::GetUserSourceRepoRefListOutcome CrClient::getUserSourceRepoRefList(const GetUserSourceRepoRefListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetUserSourceRepoRefListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetUserSourceRepoRefListOutcome(GetUserSourceRepoRefListResult(outcome.result()));
	else
		return GetUserSourceRepoRefListOutcome(outcome.error());
}

void CrClient::getUserSourceRepoRefListAsync(const GetUserSourceRepoRefListRequest& request, const GetUserSourceRepoRefListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getUserSourceRepoRefList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::GetUserSourceRepoRefListOutcomeCallable CrClient::getUserSourceRepoRefListCallable(const GetUserSourceRepoRefListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetUserSourceRepoRefListOutcome()>>(
			[this, request]()
			{
			return this->getUserSourceRepoRefList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::SearchRepoOutcome CrClient::searchRepo(const SearchRepoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SearchRepoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SearchRepoOutcome(SearchRepoResult(outcome.result()));
	else
		return SearchRepoOutcome(outcome.error());
}

void CrClient::searchRepoAsync(const SearchRepoRequest& request, const SearchRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, searchRepo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::SearchRepoOutcomeCallable CrClient::searchRepoCallable(const SearchRepoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SearchRepoOutcome()>>(
			[this, request]()
			{
			return this->searchRepo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::StartImageScanOutcome CrClient::startImageScan(const StartImageScanRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartImageScanOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartImageScanOutcome(StartImageScanResult(outcome.result()));
	else
		return StartImageScanOutcome(outcome.error());
}

void CrClient::startImageScanAsync(const StartImageScanRequest& request, const StartImageScanAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startImageScan(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::StartImageScanOutcomeCallable CrClient::startImageScanCallable(const StartImageScanRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartImageScanOutcome()>>(
			[this, request]()
			{
			return this->startImageScan(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::StartRepoBuildOutcome CrClient::startRepoBuild(const StartRepoBuildRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartRepoBuildOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartRepoBuildOutcome(StartRepoBuildResult(outcome.result()));
	else
		return StartRepoBuildOutcome(outcome.error());
}

void CrClient::startRepoBuildAsync(const StartRepoBuildRequest& request, const StartRepoBuildAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startRepoBuild(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::StartRepoBuildOutcomeCallable CrClient::startRepoBuildCallable(const StartRepoBuildRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartRepoBuildOutcome()>>(
			[this, request]()
			{
			return this->startRepoBuild(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::StartRepoBuildByRuleOutcome CrClient::startRepoBuildByRule(const StartRepoBuildByRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartRepoBuildByRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartRepoBuildByRuleOutcome(StartRepoBuildByRuleResult(outcome.result()));
	else
		return StartRepoBuildByRuleOutcome(outcome.error());
}

void CrClient::startRepoBuildByRuleAsync(const StartRepoBuildByRuleRequest& request, const StartRepoBuildByRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startRepoBuildByRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::StartRepoBuildByRuleOutcomeCallable CrClient::startRepoBuildByRuleCallable(const StartRepoBuildByRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartRepoBuildByRuleOutcome()>>(
			[this, request]()
			{
			return this->startRepoBuildByRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::UpdateNamespaceOutcome CrClient::updateNamespace(const UpdateNamespaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateNamespaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateNamespaceOutcome(UpdateNamespaceResult(outcome.result()));
	else
		return UpdateNamespaceOutcome(outcome.error());
}

void CrClient::updateNamespaceAsync(const UpdateNamespaceRequest& request, const UpdateNamespaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateNamespace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::UpdateNamespaceOutcomeCallable CrClient::updateNamespaceCallable(const UpdateNamespaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateNamespaceOutcome()>>(
			[this, request]()
			{
			return this->updateNamespace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::UpdateNamespaceAuthorizationOutcome CrClient::updateNamespaceAuthorization(const UpdateNamespaceAuthorizationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateNamespaceAuthorizationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateNamespaceAuthorizationOutcome(UpdateNamespaceAuthorizationResult(outcome.result()));
	else
		return UpdateNamespaceAuthorizationOutcome(outcome.error());
}

void CrClient::updateNamespaceAuthorizationAsync(const UpdateNamespaceAuthorizationRequest& request, const UpdateNamespaceAuthorizationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateNamespaceAuthorization(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::UpdateNamespaceAuthorizationOutcomeCallable CrClient::updateNamespaceAuthorizationCallable(const UpdateNamespaceAuthorizationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateNamespaceAuthorizationOutcome()>>(
			[this, request]()
			{
			return this->updateNamespaceAuthorization(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::UpdateRepoOutcome CrClient::updateRepo(const UpdateRepoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateRepoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateRepoOutcome(UpdateRepoResult(outcome.result()));
	else
		return UpdateRepoOutcome(outcome.error());
}

void CrClient::updateRepoAsync(const UpdateRepoRequest& request, const UpdateRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateRepo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::UpdateRepoOutcomeCallable CrClient::updateRepoCallable(const UpdateRepoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateRepoOutcome()>>(
			[this, request]()
			{
			return this->updateRepo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::UpdateRepoAuthorizationOutcome CrClient::updateRepoAuthorization(const UpdateRepoAuthorizationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateRepoAuthorizationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateRepoAuthorizationOutcome(UpdateRepoAuthorizationResult(outcome.result()));
	else
		return UpdateRepoAuthorizationOutcome(outcome.error());
}

void CrClient::updateRepoAuthorizationAsync(const UpdateRepoAuthorizationRequest& request, const UpdateRepoAuthorizationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateRepoAuthorization(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::UpdateRepoAuthorizationOutcomeCallable CrClient::updateRepoAuthorizationCallable(const UpdateRepoAuthorizationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateRepoAuthorizationOutcome()>>(
			[this, request]()
			{
			return this->updateRepoAuthorization(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::UpdateRepoBuildRuleOutcome CrClient::updateRepoBuildRule(const UpdateRepoBuildRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateRepoBuildRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateRepoBuildRuleOutcome(UpdateRepoBuildRuleResult(outcome.result()));
	else
		return UpdateRepoBuildRuleOutcome(outcome.error());
}

void CrClient::updateRepoBuildRuleAsync(const UpdateRepoBuildRuleRequest& request, const UpdateRepoBuildRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateRepoBuildRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::UpdateRepoBuildRuleOutcomeCallable CrClient::updateRepoBuildRuleCallable(const UpdateRepoBuildRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateRepoBuildRuleOutcome()>>(
			[this, request]()
			{
			return this->updateRepoBuildRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::UpdateRepoSourceRepoOutcome CrClient::updateRepoSourceRepo(const UpdateRepoSourceRepoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateRepoSourceRepoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateRepoSourceRepoOutcome(UpdateRepoSourceRepoResult(outcome.result()));
	else
		return UpdateRepoSourceRepoOutcome(outcome.error());
}

void CrClient::updateRepoSourceRepoAsync(const UpdateRepoSourceRepoRequest& request, const UpdateRepoSourceRepoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateRepoSourceRepo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::UpdateRepoSourceRepoOutcomeCallable CrClient::updateRepoSourceRepoCallable(const UpdateRepoSourceRepoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateRepoSourceRepoOutcome()>>(
			[this, request]()
			{
			return this->updateRepoSourceRepo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::UpdateRepoWebhookOutcome CrClient::updateRepoWebhook(const UpdateRepoWebhookRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateRepoWebhookOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateRepoWebhookOutcome(UpdateRepoWebhookResult(outcome.result()));
	else
		return UpdateRepoWebhookOutcome(outcome.error());
}

void CrClient::updateRepoWebhookAsync(const UpdateRepoWebhookRequest& request, const UpdateRepoWebhookAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateRepoWebhook(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::UpdateRepoWebhookOutcomeCallable CrClient::updateRepoWebhookCallable(const UpdateRepoWebhookRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateRepoWebhookOutcome()>>(
			[this, request]()
			{
			return this->updateRepoWebhook(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CrClient::UpdateUserInfoOutcome CrClient::updateUserInfo(const UpdateUserInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateUserInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateUserInfoOutcome(UpdateUserInfoResult(outcome.result()));
	else
		return UpdateUserInfoOutcome(outcome.error());
}

void CrClient::updateUserInfoAsync(const UpdateUserInfoRequest& request, const UpdateUserInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateUserInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CrClient::UpdateUserInfoOutcomeCallable CrClient::updateUserInfoCallable(const UpdateUserInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateUserInfoOutcome()>>(
			[this, request]()
			{
			return this->updateUserInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


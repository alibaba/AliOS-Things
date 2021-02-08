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

#include <alibabacloud/videosearch/VideosearchClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Videosearch;
using namespace AlibabaCloud::Videosearch::Model;

namespace
{
	const std::string SERVICE_NAME = "videosearch";
}

VideosearchClient::VideosearchClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "");
}

VideosearchClient::VideosearchClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "");
}

VideosearchClient::VideosearchClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "");
}

VideosearchClient::~VideosearchClient()
{}

VideosearchClient::AddDeletionVideoTaskOutcome VideosearchClient::addDeletionVideoTask(const AddDeletionVideoTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddDeletionVideoTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddDeletionVideoTaskOutcome(AddDeletionVideoTaskResult(outcome.result()));
	else
		return AddDeletionVideoTaskOutcome(outcome.error());
}

void VideosearchClient::addDeletionVideoTaskAsync(const AddDeletionVideoTaskRequest& request, const AddDeletionVideoTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addDeletionVideoTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

VideosearchClient::AddDeletionVideoTaskOutcomeCallable VideosearchClient::addDeletionVideoTaskCallable(const AddDeletionVideoTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddDeletionVideoTaskOutcome()>>(
			[this, request]()
			{
			return this->addDeletionVideoTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

VideosearchClient::AddSearchVideoTaskOutcome VideosearchClient::addSearchVideoTask(const AddSearchVideoTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddSearchVideoTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddSearchVideoTaskOutcome(AddSearchVideoTaskResult(outcome.result()));
	else
		return AddSearchVideoTaskOutcome(outcome.error());
}

void VideosearchClient::addSearchVideoTaskAsync(const AddSearchVideoTaskRequest& request, const AddSearchVideoTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addSearchVideoTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

VideosearchClient::AddSearchVideoTaskOutcomeCallable VideosearchClient::addSearchVideoTaskCallable(const AddSearchVideoTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddSearchVideoTaskOutcome()>>(
			[this, request]()
			{
			return this->addSearchVideoTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

VideosearchClient::AddStorageVideoTaskOutcome VideosearchClient::addStorageVideoTask(const AddStorageVideoTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddStorageVideoTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddStorageVideoTaskOutcome(AddStorageVideoTaskResult(outcome.result()));
	else
		return AddStorageVideoTaskOutcome(outcome.error());
}

void VideosearchClient::addStorageVideoTaskAsync(const AddStorageVideoTaskRequest& request, const AddStorageVideoTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addStorageVideoTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

VideosearchClient::AddStorageVideoTaskOutcomeCallable VideosearchClient::addStorageVideoTaskCallable(const AddStorageVideoTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddStorageVideoTaskOutcome()>>(
			[this, request]()
			{
			return this->addStorageVideoTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

VideosearchClient::GetTaskStatusOutcome VideosearchClient::getTaskStatus(const GetTaskStatusRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetTaskStatusOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetTaskStatusOutcome(GetTaskStatusResult(outcome.result()));
	else
		return GetTaskStatusOutcome(outcome.error());
}

void VideosearchClient::getTaskStatusAsync(const GetTaskStatusRequest& request, const GetTaskStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getTaskStatus(request), context);
	};

	asyncExecute(new Runnable(fn));
}

VideosearchClient::GetTaskStatusOutcomeCallable VideosearchClient::getTaskStatusCallable(const GetTaskStatusRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetTaskStatusOutcome()>>(
			[this, request]()
			{
			return this->getTaskStatus(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


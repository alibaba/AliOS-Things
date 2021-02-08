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

#include <alibabacloud/imgsearch/ImgsearchClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Imgsearch;
using namespace AlibabaCloud::Imgsearch::Model;

namespace
{
	const std::string SERVICE_NAME = "imgsearch";
}

ImgsearchClient::ImgsearchClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imgsearch");
}

ImgsearchClient::ImgsearchClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imgsearch");
}

ImgsearchClient::ImgsearchClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imgsearch");
}

ImgsearchClient::~ImgsearchClient()
{}

ImgsearchClient::AddImageOutcome ImgsearchClient::addImage(const AddImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddImageOutcome(AddImageResult(outcome.result()));
	else
		return AddImageOutcome(outcome.error());
}

void ImgsearchClient::addImageAsync(const AddImageRequest& request, const AddImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImgsearchClient::AddImageOutcomeCallable ImgsearchClient::addImageCallable(const AddImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddImageOutcome()>>(
			[this, request]()
			{
			return this->addImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImgsearchClient::CreateImageDbOutcome ImgsearchClient::createImageDb(const CreateImageDbRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateImageDbOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateImageDbOutcome(CreateImageDbResult(outcome.result()));
	else
		return CreateImageDbOutcome(outcome.error());
}

void ImgsearchClient::createImageDbAsync(const CreateImageDbRequest& request, const CreateImageDbAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createImageDb(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImgsearchClient::CreateImageDbOutcomeCallable ImgsearchClient::createImageDbCallable(const CreateImageDbRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateImageDbOutcome()>>(
			[this, request]()
			{
			return this->createImageDb(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImgsearchClient::DeleteImageOutcome ImgsearchClient::deleteImage(const DeleteImageRequest &request) const
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

void ImgsearchClient::deleteImageAsync(const DeleteImageRequest& request, const DeleteImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImgsearchClient::DeleteImageOutcomeCallable ImgsearchClient::deleteImageCallable(const DeleteImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteImageOutcome()>>(
			[this, request]()
			{
			return this->deleteImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImgsearchClient::DeleteImageDbOutcome ImgsearchClient::deleteImageDb(const DeleteImageDbRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteImageDbOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteImageDbOutcome(DeleteImageDbResult(outcome.result()));
	else
		return DeleteImageDbOutcome(outcome.error());
}

void ImgsearchClient::deleteImageDbAsync(const DeleteImageDbRequest& request, const DeleteImageDbAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteImageDb(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImgsearchClient::DeleteImageDbOutcomeCallable ImgsearchClient::deleteImageDbCallable(const DeleteImageDbRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteImageDbOutcome()>>(
			[this, request]()
			{
			return this->deleteImageDb(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImgsearchClient::ListImageDbsOutcome ImgsearchClient::listImageDbs(const ListImageDbsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListImageDbsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListImageDbsOutcome(ListImageDbsResult(outcome.result()));
	else
		return ListImageDbsOutcome(outcome.error());
}

void ImgsearchClient::listImageDbsAsync(const ListImageDbsRequest& request, const ListImageDbsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listImageDbs(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImgsearchClient::ListImageDbsOutcomeCallable ImgsearchClient::listImageDbsCallable(const ListImageDbsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListImageDbsOutcome()>>(
			[this, request]()
			{
			return this->listImageDbs(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImgsearchClient::ListImagesOutcome ImgsearchClient::listImages(const ListImagesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListImagesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListImagesOutcome(ListImagesResult(outcome.result()));
	else
		return ListImagesOutcome(outcome.error());
}

void ImgsearchClient::listImagesAsync(const ListImagesRequest& request, const ListImagesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listImages(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImgsearchClient::ListImagesOutcomeCallable ImgsearchClient::listImagesCallable(const ListImagesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListImagesOutcome()>>(
			[this, request]()
			{
			return this->listImages(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImgsearchClient::SearchImageOutcome ImgsearchClient::searchImage(const SearchImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SearchImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SearchImageOutcome(SearchImageResult(outcome.result()));
	else
		return SearchImageOutcome(outcome.error());
}

void ImgsearchClient::searchImageAsync(const SearchImageRequest& request, const SearchImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, searchImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImgsearchClient::SearchImageOutcomeCallable ImgsearchClient::searchImageCallable(const SearchImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SearchImageOutcome()>>(
			[this, request]()
			{
			return this->searchImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


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

#include <alibabacloud/imagesearch/ImageSearchClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::ImageSearch;
using namespace AlibabaCloud::ImageSearch::Model;

namespace
{
	const std::string SERVICE_NAME = "ImageSearch";
}

ImageSearchClient::ImageSearchClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RoaServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imagesearch");
}

ImageSearchClient::ImageSearchClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RoaServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imagesearch");
}

ImageSearchClient::ImageSearchClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RoaServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imagesearch");
}

ImageSearchClient::~ImageSearchClient()
{}

ImageSearchClient::AddItemOutcome ImageSearchClient::addItem(const AddItemRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddItemOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddItemOutcome(AddItemResult(outcome.result()));
	else
		return AddItemOutcome(outcome.error());
}

void ImageSearchClient::addItemAsync(const AddItemRequest& request, const AddItemAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addItem(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageSearchClient::AddItemOutcomeCallable ImageSearchClient::addItemCallable(const AddItemRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddItemOutcome()>>(
			[this, request]()
			{
			return this->addItem(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageSearchClient::DeleteItemOutcome ImageSearchClient::deleteItem(const DeleteItemRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteItemOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteItemOutcome(DeleteItemResult(outcome.result()));
	else
		return DeleteItemOutcome(outcome.error());
}

void ImageSearchClient::deleteItemAsync(const DeleteItemRequest& request, const DeleteItemAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteItem(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageSearchClient::DeleteItemOutcomeCallable ImageSearchClient::deleteItemCallable(const DeleteItemRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteItemOutcome()>>(
			[this, request]()
			{
			return this->deleteItem(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageSearchClient::SearchItemOutcome ImageSearchClient::searchItem(const SearchItemRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SearchItemOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SearchItemOutcome(SearchItemResult(outcome.result()));
	else
		return SearchItemOutcome(outcome.error());
}

void ImageSearchClient::searchItemAsync(const SearchItemRequest& request, const SearchItemAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, searchItem(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageSearchClient::SearchItemOutcomeCallable ImageSearchClient::searchItemCallable(const SearchItemRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SearchItemOutcome()>>(
			[this, request]()
			{
			return this->searchItem(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


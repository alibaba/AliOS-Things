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

#include <alibabacloud/metering/MeteringClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Metering;
using namespace AlibabaCloud::Metering::Model;

namespace
{
	const std::string SERVICE_NAME = "Metering";
}

MeteringClient::MeteringClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RoaServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "pai");
}

MeteringClient::MeteringClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RoaServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "pai");
}

MeteringClient::MeteringClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RoaServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "pai");
}

MeteringClient::~MeteringClient()
{}

MeteringClient::PostDataOutcome MeteringClient::postData(const PostDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return PostDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return PostDataOutcome(PostDataResult(outcome.result()));
	else
		return PostDataOutcome(outcome.error());
}

void MeteringClient::postDataAsync(const PostDataRequest& request, const PostDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, postData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MeteringClient::PostDataOutcomeCallable MeteringClient::postDataCallable(const PostDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<PostDataOutcome()>>(
			[this, request]()
			{
			return this->postData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MeteringClient::SyncDataOutcome MeteringClient::syncData(const SyncDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SyncDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SyncDataOutcome(SyncDataResult(outcome.result()));
	else
		return SyncDataOutcome(outcome.error());
}

void MeteringClient::syncDataAsync(const SyncDataRequest& request, const SyncDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, syncData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MeteringClient::SyncDataOutcomeCallable MeteringClient::syncDataCallable(const SyncDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SyncDataOutcome()>>(
			[this, request]()
			{
			return this->syncData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


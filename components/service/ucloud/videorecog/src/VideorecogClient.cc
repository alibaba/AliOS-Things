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

#include <alibabacloud/videorecog/VideorecogClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Videorecog;
using namespace AlibabaCloud::Videorecog::Model;

namespace
{
	const std::string SERVICE_NAME = "videorecog";
}

VideorecogClient::VideorecogClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "videorecog");
}

VideorecogClient::VideorecogClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "videorecog");
}

VideorecogClient::VideorecogClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "videorecog");
}

VideorecogClient::~VideorecogClient()
{}

VideorecogClient::DetectVideoShotOutcome VideorecogClient::detectVideoShot(const DetectVideoShotRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectVideoShotOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectVideoShotOutcome(DetectVideoShotResult(outcome.result()));
	else
		return DetectVideoShotOutcome(outcome.error());
}

void VideorecogClient::detectVideoShotAsync(const DetectVideoShotRequest& request, const DetectVideoShotAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectVideoShot(request), context);
	};

	asyncExecute(new Runnable(fn));
}

VideorecogClient::DetectVideoShotOutcomeCallable VideorecogClient::detectVideoShotCallable(const DetectVideoShotRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectVideoShotOutcome()>>(
			[this, request]()
			{
			return this->detectVideoShot(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

VideorecogClient::GenerateVideoCoverOutcome VideorecogClient::generateVideoCover(const GenerateVideoCoverRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GenerateVideoCoverOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GenerateVideoCoverOutcome(GenerateVideoCoverResult(outcome.result()));
	else
		return GenerateVideoCoverOutcome(outcome.error());
}

void VideorecogClient::generateVideoCoverAsync(const GenerateVideoCoverRequest& request, const GenerateVideoCoverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, generateVideoCover(request), context);
	};

	asyncExecute(new Runnable(fn));
}

VideorecogClient::GenerateVideoCoverOutcomeCallable VideorecogClient::generateVideoCoverCallable(const GenerateVideoCoverRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GenerateVideoCoverOutcome()>>(
			[this, request]()
			{
			return this->generateVideoCover(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

VideorecogClient::GetAsyncJobResultOutcome VideorecogClient::getAsyncJobResult(const GetAsyncJobResultRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetAsyncJobResultOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetAsyncJobResultOutcome(GetAsyncJobResultResult(outcome.result()));
	else
		return GetAsyncJobResultOutcome(outcome.error());
}

void VideorecogClient::getAsyncJobResultAsync(const GetAsyncJobResultRequest& request, const GetAsyncJobResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getAsyncJobResult(request), context);
	};

	asyncExecute(new Runnable(fn));
}

VideorecogClient::GetAsyncJobResultOutcomeCallable VideorecogClient::getAsyncJobResultCallable(const GetAsyncJobResultRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetAsyncJobResultOutcome()>>(
			[this, request]()
			{
			return this->getAsyncJobResult(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


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

#include <alibabacloud/videoseg/VideosegClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Videoseg;
using namespace AlibabaCloud::Videoseg::Model;

namespace
{
	const std::string SERVICE_NAME = "videoseg";
}

VideosegClient::VideosegClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "videoseg");
}

VideosegClient::VideosegClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "videoseg");
}

VideosegClient::VideosegClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "videoseg");
}

VideosegClient::~VideosegClient()
{}

VideosegClient::GetAsyncJobResultOutcome VideosegClient::getAsyncJobResult(const GetAsyncJobResultRequest &request) const
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

void VideosegClient::getAsyncJobResultAsync(const GetAsyncJobResultRequest& request, const GetAsyncJobResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getAsyncJobResult(request), context);
	};

	asyncExecute(new Runnable(fn));
}

VideosegClient::GetAsyncJobResultOutcomeCallable VideosegClient::getAsyncJobResultCallable(const GetAsyncJobResultRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetAsyncJobResultOutcome()>>(
			[this, request]()
			{
			return this->getAsyncJobResult(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

VideosegClient::SegmentHalfBodyOutcome VideosegClient::segmentHalfBody(const SegmentHalfBodyRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentHalfBodyOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentHalfBodyOutcome(SegmentHalfBodyResult(outcome.result()));
	else
		return SegmentHalfBodyOutcome(outcome.error());
}

void VideosegClient::segmentHalfBodyAsync(const SegmentHalfBodyRequest& request, const SegmentHalfBodyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentHalfBody(request), context);
	};

	asyncExecute(new Runnable(fn));
}

VideosegClient::SegmentHalfBodyOutcomeCallable VideosegClient::segmentHalfBodyCallable(const SegmentHalfBodyRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentHalfBodyOutcome()>>(
			[this, request]()
			{
			return this->segmentHalfBody(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

VideosegClient::SegmentVideoBodyOutcome VideosegClient::segmentVideoBody(const SegmentVideoBodyRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentVideoBodyOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentVideoBodyOutcome(SegmentVideoBodyResult(outcome.result()));
	else
		return SegmentVideoBodyOutcome(outcome.error());
}

void VideosegClient::segmentVideoBodyAsync(const SegmentVideoBodyRequest& request, const SegmentVideoBodyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentVideoBody(request), context);
	};

	asyncExecute(new Runnable(fn));
}

VideosegClient::SegmentVideoBodyOutcomeCallable VideosegClient::segmentVideoBodyCallable(const SegmentVideoBodyRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentVideoBodyOutcome()>>(
			[this, request]()
			{
			return this->segmentVideoBody(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


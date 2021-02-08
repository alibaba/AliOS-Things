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

#include <alibabacloud/imageaudit/ImageauditClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Imageaudit;
using namespace AlibabaCloud::Imageaudit::Model;

namespace
{
	const std::string SERVICE_NAME = "imageaudit";
}

ImageauditClient::ImageauditClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imageaudit");
}

ImageauditClient::ImageauditClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imageaudit");
}

ImageauditClient::ImageauditClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imageaudit");
}

ImageauditClient::~ImageauditClient()
{}

ImageauditClient::ScanImageOutcome ImageauditClient::scanImage(const ScanImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ScanImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ScanImageOutcome(ScanImageResult(outcome.result()));
	else
		return ScanImageOutcome(outcome.error());
}

void ImageauditClient::scanImageAsync(const ScanImageRequest& request, const ScanImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, scanImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageauditClient::ScanImageOutcomeCallable ImageauditClient::scanImageCallable(const ScanImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ScanImageOutcome()>>(
			[this, request]()
			{
			return this->scanImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageauditClient::ScanTextOutcome ImageauditClient::scanText(const ScanTextRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ScanTextOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ScanTextOutcome(ScanTextResult(outcome.result()));
	else
		return ScanTextOutcome(outcome.error());
}

void ImageauditClient::scanTextAsync(const ScanTextRequest& request, const ScanTextAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, scanText(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageauditClient::ScanTextOutcomeCallable ImageauditClient::scanTextCallable(const ScanTextRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ScanTextOutcome()>>(
			[this, request]()
			{
			return this->scanText(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


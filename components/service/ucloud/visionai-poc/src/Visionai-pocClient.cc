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

#include <alibabacloud/visionai-poc/Visionai_pocClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Visionai_poc;
using namespace AlibabaCloud::Visionai_poc::Model;

namespace
{
	const std::string SERVICE_NAME = "visionai-poc";
}

Visionai_pocClient::Visionai_pocClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "");
}

Visionai_pocClient::Visionai_pocClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "");
}

Visionai_pocClient::Visionai_pocClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "");
}

Visionai_pocClient::~Visionai_pocClient()
{}

Visionai_pocClient::CheckMultiagentOutcome Visionai_pocClient::checkMultiagent(const CheckMultiagentRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CheckMultiagentOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CheckMultiagentOutcome(CheckMultiagentResult(outcome.result()));
	else
		return CheckMultiagentOutcome(outcome.error());
}

void Visionai_pocClient::checkMultiagentAsync(const CheckMultiagentRequest& request, const CheckMultiagentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, checkMultiagent(request), context);
	};

	asyncExecute(new Runnable(fn));
}

Visionai_pocClient::CheckMultiagentOutcomeCallable Visionai_pocClient::checkMultiagentCallable(const CheckMultiagentRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CheckMultiagentOutcome()>>(
			[this, request]()
			{
			return this->checkMultiagent(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

Visionai_pocClient::PredictCategoryOutcome Visionai_pocClient::predictCategory(const PredictCategoryRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return PredictCategoryOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return PredictCategoryOutcome(PredictCategoryResult(outcome.result()));
	else
		return PredictCategoryOutcome(outcome.error());
}

void Visionai_pocClient::predictCategoryAsync(const PredictCategoryRequest& request, const PredictCategoryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, predictCategory(request), context);
	};

	asyncExecute(new Runnable(fn));
}

Visionai_pocClient::PredictCategoryOutcomeCallable Visionai_pocClient::predictCategoryCallable(const PredictCategoryRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<PredictCategoryOutcome()>>(
			[this, request]()
			{
			return this->predictCategory(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

Visionai_pocClient::RecognizeCarOutcome Visionai_pocClient::recognizeCar(const RecognizeCarRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeCarOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeCarOutcome(RecognizeCarResult(outcome.result()));
	else
		return RecognizeCarOutcome(outcome.error());
}

void Visionai_pocClient::recognizeCarAsync(const RecognizeCarRequest& request, const RecognizeCarAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeCar(request), context);
	};

	asyncExecute(new Runnable(fn));
}

Visionai_pocClient::RecognizeCarOutcomeCallable Visionai_pocClient::recognizeCarCallable(const RecognizeCarRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeCarOutcome()>>(
			[this, request]()
			{
			return this->recognizeCar(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

Visionai_pocClient::RecognizeEntityOutcome Visionai_pocClient::recognizeEntity(const RecognizeEntityRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeEntityOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeEntityOutcome(RecognizeEntityResult(outcome.result()));
	else
		return RecognizeEntityOutcome(outcome.error());
}

void Visionai_pocClient::recognizeEntityAsync(const RecognizeEntityRequest& request, const RecognizeEntityAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeEntity(request), context);
	};

	asyncExecute(new Runnable(fn));
}

Visionai_pocClient::RecognizeEntityOutcomeCallable Visionai_pocClient::recognizeEntityCallable(const RecognizeEntityRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeEntityOutcome()>>(
			[this, request]()
			{
			return this->recognizeEntity(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

Visionai_pocClient::RecognizeFlowerOutcome Visionai_pocClient::recognizeFlower(const RecognizeFlowerRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeFlowerOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeFlowerOutcome(RecognizeFlowerResult(outcome.result()));
	else
		return RecognizeFlowerOutcome(outcome.error());
}

void Visionai_pocClient::recognizeFlowerAsync(const RecognizeFlowerRequest& request, const RecognizeFlowerAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeFlower(request), context);
	};

	asyncExecute(new Runnable(fn));
}

Visionai_pocClient::RecognizeFlowerOutcomeCallable Visionai_pocClient::recognizeFlowerCallable(const RecognizeFlowerRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeFlowerOutcome()>>(
			[this, request]()
			{
			return this->recognizeFlower(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

Visionai_pocClient::RecognizeLabelOutcome Visionai_pocClient::recognizeLabel(const RecognizeLabelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeLabelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeLabelOutcome(RecognizeLabelResult(outcome.result()));
	else
		return RecognizeLabelOutcome(outcome.error());
}

void Visionai_pocClient::recognizeLabelAsync(const RecognizeLabelRequest& request, const RecognizeLabelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeLabel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

Visionai_pocClient::RecognizeLabelOutcomeCallable Visionai_pocClient::recognizeLabelCallable(const RecognizeLabelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeLabelOutcome()>>(
			[this, request]()
			{
			return this->recognizeLabel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

Visionai_pocClient::RecognizePetOutcome Visionai_pocClient::recognizePet(const RecognizePetRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizePetOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizePetOutcome(RecognizePetResult(outcome.result()));
	else
		return RecognizePetOutcome(outcome.error());
}

void Visionai_pocClient::recognizePetAsync(const RecognizePetRequest& request, const RecognizePetAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizePet(request), context);
	};

	asyncExecute(new Runnable(fn));
}

Visionai_pocClient::RecognizePetOutcomeCallable Visionai_pocClient::recognizePetCallable(const RecognizePetRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizePetOutcome()>>(
			[this, request]()
			{
			return this->recognizePet(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


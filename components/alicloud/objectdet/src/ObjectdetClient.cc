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

#include <alibabacloud/objectdet/ObjectdetClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Objectdet;
using namespace AlibabaCloud::Objectdet::Model;

namespace
{
	const std::string SERVICE_NAME = "objectdet";
}

ObjectdetClient::ObjectdetClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "objectdet");
}

ObjectdetClient::ObjectdetClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "objectdet");
}

ObjectdetClient::ObjectdetClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "objectdet");
}

ObjectdetClient::~ObjectdetClient()
{}

ObjectdetClient::ClassifyVehicleInsuranceOutcome ObjectdetClient::classifyVehicleInsurance(const ClassifyVehicleInsuranceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ClassifyVehicleInsuranceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ClassifyVehicleInsuranceOutcome(ClassifyVehicleInsuranceResult(outcome.result()));
	else
		return ClassifyVehicleInsuranceOutcome(outcome.error());
}

void ObjectdetClient::classifyVehicleInsuranceAsync(const ClassifyVehicleInsuranceRequest& request, const ClassifyVehicleInsuranceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, classifyVehicleInsurance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ObjectdetClient::ClassifyVehicleInsuranceOutcomeCallable ObjectdetClient::classifyVehicleInsuranceCallable(const ClassifyVehicleInsuranceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ClassifyVehicleInsuranceOutcome()>>(
			[this, request]()
			{
			return this->classifyVehicleInsurance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ObjectdetClient::DetectMainBodyOutcome ObjectdetClient::detectMainBody(const DetectMainBodyRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectMainBodyOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectMainBodyOutcome(DetectMainBodyResult(outcome.result()));
	else
		return DetectMainBodyOutcome(outcome.error());
}

void ObjectdetClient::detectMainBodyAsync(const DetectMainBodyRequest& request, const DetectMainBodyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectMainBody(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ObjectdetClient::DetectMainBodyOutcomeCallable ObjectdetClient::detectMainBodyCallable(const DetectMainBodyRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectMainBodyOutcome()>>(
			[this, request]()
			{
			return this->detectMainBody(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ObjectdetClient::DetectObjectOutcome ObjectdetClient::detectObject(const DetectObjectRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectObjectOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectObjectOutcome(DetectObjectResult(outcome.result()));
	else
		return DetectObjectOutcome(outcome.error());
}

void ObjectdetClient::detectObjectAsync(const DetectObjectRequest& request, const DetectObjectAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectObject(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ObjectdetClient::DetectObjectOutcomeCallable ObjectdetClient::detectObjectCallable(const DetectObjectRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectObjectOutcome()>>(
			[this, request]()
			{
			return this->detectObject(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ObjectdetClient::DetectTransparentImageOutcome ObjectdetClient::detectTransparentImage(const DetectTransparentImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectTransparentImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectTransparentImageOutcome(DetectTransparentImageResult(outcome.result()));
	else
		return DetectTransparentImageOutcome(outcome.error());
}

void ObjectdetClient::detectTransparentImageAsync(const DetectTransparentImageRequest& request, const DetectTransparentImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectTransparentImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ObjectdetClient::DetectTransparentImageOutcomeCallable ObjectdetClient::detectTransparentImageCallable(const DetectTransparentImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectTransparentImageOutcome()>>(
			[this, request]()
			{
			return this->detectTransparentImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ObjectdetClient::DetectVehicleOutcome ObjectdetClient::detectVehicle(const DetectVehicleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectVehicleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectVehicleOutcome(DetectVehicleResult(outcome.result()));
	else
		return DetectVehicleOutcome(outcome.error());
}

void ObjectdetClient::detectVehicleAsync(const DetectVehicleRequest& request, const DetectVehicleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectVehicle(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ObjectdetClient::DetectVehicleOutcomeCallable ObjectdetClient::detectVehicleCallable(const DetectVehicleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectVehicleOutcome()>>(
			[this, request]()
			{
			return this->detectVehicle(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ObjectdetClient::DetectWhiteBaseImageOutcome ObjectdetClient::detectWhiteBaseImage(const DetectWhiteBaseImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectWhiteBaseImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectWhiteBaseImageOutcome(DetectWhiteBaseImageResult(outcome.result()));
	else
		return DetectWhiteBaseImageOutcome(outcome.error());
}

void ObjectdetClient::detectWhiteBaseImageAsync(const DetectWhiteBaseImageRequest& request, const DetectWhiteBaseImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectWhiteBaseImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ObjectdetClient::DetectWhiteBaseImageOutcomeCallable ObjectdetClient::detectWhiteBaseImageCallable(const DetectWhiteBaseImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectWhiteBaseImageOutcome()>>(
			[this, request]()
			{
			return this->detectWhiteBaseImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ObjectdetClient::GenerateVehicleRepairPlanOutcome ObjectdetClient::generateVehicleRepairPlan(const GenerateVehicleRepairPlanRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GenerateVehicleRepairPlanOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GenerateVehicleRepairPlanOutcome(GenerateVehicleRepairPlanResult(outcome.result()));
	else
		return GenerateVehicleRepairPlanOutcome(outcome.error());
}

void ObjectdetClient::generateVehicleRepairPlanAsync(const GenerateVehicleRepairPlanRequest& request, const GenerateVehicleRepairPlanAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, generateVehicleRepairPlan(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ObjectdetClient::GenerateVehicleRepairPlanOutcomeCallable ObjectdetClient::generateVehicleRepairPlanCallable(const GenerateVehicleRepairPlanRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GenerateVehicleRepairPlanOutcome()>>(
			[this, request]()
			{
			return this->generateVehicleRepairPlan(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ObjectdetClient::GetVehicleRepairPlanOutcome ObjectdetClient::getVehicleRepairPlan(const GetVehicleRepairPlanRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetVehicleRepairPlanOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetVehicleRepairPlanOutcome(GetVehicleRepairPlanResult(outcome.result()));
	else
		return GetVehicleRepairPlanOutcome(outcome.error());
}

void ObjectdetClient::getVehicleRepairPlanAsync(const GetVehicleRepairPlanRequest& request, const GetVehicleRepairPlanAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getVehicleRepairPlan(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ObjectdetClient::GetVehicleRepairPlanOutcomeCallable ObjectdetClient::getVehicleRepairPlanCallable(const GetVehicleRepairPlanRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetVehicleRepairPlanOutcome()>>(
			[this, request]()
			{
			return this->getVehicleRepairPlan(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ObjectdetClient::RecognizeVehicleDamageOutcome ObjectdetClient::recognizeVehicleDamage(const RecognizeVehicleDamageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeVehicleDamageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeVehicleDamageOutcome(RecognizeVehicleDamageResult(outcome.result()));
	else
		return RecognizeVehicleDamageOutcome(outcome.error());
}

void ObjectdetClient::recognizeVehicleDamageAsync(const RecognizeVehicleDamageRequest& request, const RecognizeVehicleDamageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeVehicleDamage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ObjectdetClient::RecognizeVehicleDamageOutcomeCallable ObjectdetClient::recognizeVehicleDamageCallable(const RecognizeVehicleDamageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeVehicleDamageOutcome()>>(
			[this, request]()
			{
			return this->recognizeVehicleDamage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ObjectdetClient::RecognizeVehicleDashboardOutcome ObjectdetClient::recognizeVehicleDashboard(const RecognizeVehicleDashboardRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeVehicleDashboardOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeVehicleDashboardOutcome(RecognizeVehicleDashboardResult(outcome.result()));
	else
		return RecognizeVehicleDashboardOutcome(outcome.error());
}

void ObjectdetClient::recognizeVehicleDashboardAsync(const RecognizeVehicleDashboardRequest& request, const RecognizeVehicleDashboardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeVehicleDashboard(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ObjectdetClient::RecognizeVehicleDashboardOutcomeCallable ObjectdetClient::recognizeVehicleDashboardCallable(const RecognizeVehicleDashboardRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeVehicleDashboardOutcome()>>(
			[this, request]()
			{
			return this->recognizeVehicleDashboard(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ObjectdetClient::RecognizeVehiclePartsOutcome ObjectdetClient::recognizeVehicleParts(const RecognizeVehiclePartsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeVehiclePartsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeVehiclePartsOutcome(RecognizeVehiclePartsResult(outcome.result()));
	else
		return RecognizeVehiclePartsOutcome(outcome.error());
}

void ObjectdetClient::recognizeVehiclePartsAsync(const RecognizeVehiclePartsRequest& request, const RecognizeVehiclePartsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeVehicleParts(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ObjectdetClient::RecognizeVehiclePartsOutcomeCallable ObjectdetClient::recognizeVehiclePartsCallable(const RecognizeVehiclePartsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeVehiclePartsOutcome()>>(
			[this, request]()
			{
			return this->recognizeVehicleParts(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


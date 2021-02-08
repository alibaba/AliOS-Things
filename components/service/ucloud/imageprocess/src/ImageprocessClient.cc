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

#include <alibabacloud/imageprocess/ImageprocessClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Imageprocess;
using namespace AlibabaCloud::Imageprocess::Model;

namespace
{
	const std::string SERVICE_NAME = "imageprocess";
}

ImageprocessClient::ImageprocessClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imageprocess");
}

ImageprocessClient::ImageprocessClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imageprocess");
}

ImageprocessClient::ImageprocessClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imageprocess");
}

ImageprocessClient::~ImageprocessClient()
{}

ImageprocessClient::CalcCACSOutcome ImageprocessClient::calcCACS(const CalcCACSRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CalcCACSOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CalcCACSOutcome(CalcCACSResult(outcome.result()));
	else
		return CalcCACSOutcome(outcome.error());
}

void ImageprocessClient::calcCACSAsync(const CalcCACSRequest& request, const CalcCACSAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, calcCACS(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageprocessClient::CalcCACSOutcomeCallable ImageprocessClient::calcCACSCallable(const CalcCACSRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CalcCACSOutcome()>>(
			[this, request]()
			{
			return this->calcCACS(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageprocessClient::ClassifyFNFOutcome ImageprocessClient::classifyFNF(const ClassifyFNFRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ClassifyFNFOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ClassifyFNFOutcome(ClassifyFNFResult(outcome.result()));
	else
		return ClassifyFNFOutcome(outcome.error());
}

void ImageprocessClient::classifyFNFAsync(const ClassifyFNFRequest& request, const ClassifyFNFAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, classifyFNF(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageprocessClient::ClassifyFNFOutcomeCallable ImageprocessClient::classifyFNFCallable(const ClassifyFNFRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ClassifyFNFOutcome()>>(
			[this, request]()
			{
			return this->classifyFNF(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageprocessClient::DetectCovid19CadOutcome ImageprocessClient::detectCovid19Cad(const DetectCovid19CadRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectCovid19CadOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectCovid19CadOutcome(DetectCovid19CadResult(outcome.result()));
	else
		return DetectCovid19CadOutcome(outcome.error());
}

void ImageprocessClient::detectCovid19CadAsync(const DetectCovid19CadRequest& request, const DetectCovid19CadAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectCovid19Cad(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageprocessClient::DetectCovid19CadOutcomeCallable ImageprocessClient::detectCovid19CadCallable(const DetectCovid19CadRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectCovid19CadOutcome()>>(
			[this, request]()
			{
			return this->detectCovid19Cad(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageprocessClient::DetectHipKeypointXRayOutcome ImageprocessClient::detectHipKeypointXRay(const DetectHipKeypointXRayRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectHipKeypointXRayOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectHipKeypointXRayOutcome(DetectHipKeypointXRayResult(outcome.result()));
	else
		return DetectHipKeypointXRayOutcome(outcome.error());
}

void ImageprocessClient::detectHipKeypointXRayAsync(const DetectHipKeypointXRayRequest& request, const DetectHipKeypointXRayAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectHipKeypointXRay(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageprocessClient::DetectHipKeypointXRayOutcomeCallable ImageprocessClient::detectHipKeypointXRayCallable(const DetectHipKeypointXRayRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectHipKeypointXRayOutcome()>>(
			[this, request]()
			{
			return this->detectHipKeypointXRay(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageprocessClient::DetectKneeKeypointXRayOutcome ImageprocessClient::detectKneeKeypointXRay(const DetectKneeKeypointXRayRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectKneeKeypointXRayOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectKneeKeypointXRayOutcome(DetectKneeKeypointXRayResult(outcome.result()));
	else
		return DetectKneeKeypointXRayOutcome(outcome.error());
}

void ImageprocessClient::detectKneeKeypointXRayAsync(const DetectKneeKeypointXRayRequest& request, const DetectKneeKeypointXRayAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectKneeKeypointXRay(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageprocessClient::DetectKneeKeypointXRayOutcomeCallable ImageprocessClient::detectKneeKeypointXRayCallable(const DetectKneeKeypointXRayRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectKneeKeypointXRayOutcome()>>(
			[this, request]()
			{
			return this->detectKneeKeypointXRay(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageprocessClient::DetectKneeXRayOutcome ImageprocessClient::detectKneeXRay(const DetectKneeXRayRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectKneeXRayOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectKneeXRayOutcome(DetectKneeXRayResult(outcome.result()));
	else
		return DetectKneeXRayOutcome(outcome.error());
}

void ImageprocessClient::detectKneeXRayAsync(const DetectKneeXRayRequest& request, const DetectKneeXRayAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectKneeXRay(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageprocessClient::DetectKneeXRayOutcomeCallable ImageprocessClient::detectKneeXRayCallable(const DetectKneeXRayRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectKneeXRayOutcome()>>(
			[this, request]()
			{
			return this->detectKneeXRay(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageprocessClient::DetectLungNoduleOutcome ImageprocessClient::detectLungNodule(const DetectLungNoduleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectLungNoduleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectLungNoduleOutcome(DetectLungNoduleResult(outcome.result()));
	else
		return DetectLungNoduleOutcome(outcome.error());
}

void ImageprocessClient::detectLungNoduleAsync(const DetectLungNoduleRequest& request, const DetectLungNoduleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectLungNodule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageprocessClient::DetectLungNoduleOutcomeCallable ImageprocessClient::detectLungNoduleCallable(const DetectLungNoduleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectLungNoduleOutcome()>>(
			[this, request]()
			{
			return this->detectLungNodule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageprocessClient::DetectSkinDiseaseOutcome ImageprocessClient::detectSkinDisease(const DetectSkinDiseaseRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectSkinDiseaseOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectSkinDiseaseOutcome(DetectSkinDiseaseResult(outcome.result()));
	else
		return DetectSkinDiseaseOutcome(outcome.error());
}

void ImageprocessClient::detectSkinDiseaseAsync(const DetectSkinDiseaseRequest& request, const DetectSkinDiseaseAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectSkinDisease(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageprocessClient::DetectSkinDiseaseOutcomeCallable ImageprocessClient::detectSkinDiseaseCallable(const DetectSkinDiseaseRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectSkinDiseaseOutcome()>>(
			[this, request]()
			{
			return this->detectSkinDisease(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageprocessClient::DetectSpineMRIOutcome ImageprocessClient::detectSpineMRI(const DetectSpineMRIRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectSpineMRIOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectSpineMRIOutcome(DetectSpineMRIResult(outcome.result()));
	else
		return DetectSpineMRIOutcome(outcome.error());
}

void ImageprocessClient::detectSpineMRIAsync(const DetectSpineMRIRequest& request, const DetectSpineMRIAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectSpineMRI(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageprocessClient::DetectSpineMRIOutcomeCallable ImageprocessClient::detectSpineMRICallable(const DetectSpineMRIRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectSpineMRIOutcome()>>(
			[this, request]()
			{
			return this->detectSpineMRI(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageprocessClient::GetAsyncJobResultOutcome ImageprocessClient::getAsyncJobResult(const GetAsyncJobResultRequest &request) const
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

void ImageprocessClient::getAsyncJobResultAsync(const GetAsyncJobResultRequest& request, const GetAsyncJobResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getAsyncJobResult(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageprocessClient::GetAsyncJobResultOutcomeCallable ImageprocessClient::getAsyncJobResultCallable(const GetAsyncJobResultRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetAsyncJobResultOutcome()>>(
			[this, request]()
			{
			return this->getAsyncJobResult(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageprocessClient::RunCTRegistrationOutcome ImageprocessClient::runCTRegistration(const RunCTRegistrationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RunCTRegistrationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RunCTRegistrationOutcome(RunCTRegistrationResult(outcome.result()));
	else
		return RunCTRegistrationOutcome(outcome.error());
}

void ImageprocessClient::runCTRegistrationAsync(const RunCTRegistrationRequest& request, const RunCTRegistrationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, runCTRegistration(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageprocessClient::RunCTRegistrationOutcomeCallable ImageprocessClient::runCTRegistrationCallable(const RunCTRegistrationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RunCTRegistrationOutcome()>>(
			[this, request]()
			{
			return this->runCTRegistration(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageprocessClient::RunMedQAOutcome ImageprocessClient::runMedQA(const RunMedQARequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RunMedQAOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RunMedQAOutcome(RunMedQAResult(outcome.result()));
	else
		return RunMedQAOutcome(outcome.error());
}

void ImageprocessClient::runMedQAAsync(const RunMedQARequest& request, const RunMedQAAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, runMedQA(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageprocessClient::RunMedQAOutcomeCallable ImageprocessClient::runMedQACallable(const RunMedQARequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RunMedQAOutcome()>>(
			[this, request]()
			{
			return this->runMedQA(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageprocessClient::TranslateMedOutcome ImageprocessClient::translateMed(const TranslateMedRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return TranslateMedOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return TranslateMedOutcome(TranslateMedResult(outcome.result()));
	else
		return TranslateMedOutcome(outcome.error());
}

void ImageprocessClient::translateMedAsync(const TranslateMedRequest& request, const TranslateMedAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, translateMed(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageprocessClient::TranslateMedOutcomeCallable ImageprocessClient::translateMedCallable(const TranslateMedRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<TranslateMedOutcome()>>(
			[this, request]()
			{
			return this->translateMed(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


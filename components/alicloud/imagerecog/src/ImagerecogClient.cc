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

#include <alibabacloud/imagerecog/ImagerecogClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Imagerecog;
using namespace AlibabaCloud::Imagerecog::Model;

namespace
{
	const std::string SERVICE_NAME = "imagerecog";
}

ImagerecogClient::ImagerecogClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imagerecog");
}

ImagerecogClient::ImagerecogClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imagerecog");
}

ImagerecogClient::ImagerecogClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imagerecog");
}

ImagerecogClient::~ImagerecogClient()
{}

ImagerecogClient::ClassifyingRubbishOutcome ImagerecogClient::classifyingRubbish(const ClassifyingRubbishRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ClassifyingRubbishOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ClassifyingRubbishOutcome(ClassifyingRubbishResult(outcome.result()));
	else
		return ClassifyingRubbishOutcome(outcome.error());
}

void ImagerecogClient::classifyingRubbishAsync(const ClassifyingRubbishRequest& request, const ClassifyingRubbishAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, classifyingRubbish(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagerecogClient::ClassifyingRubbishOutcomeCallable ImagerecogClient::classifyingRubbishCallable(const ClassifyingRubbishRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ClassifyingRubbishOutcome()>>(
			[this, request]()
			{
			return this->classifyingRubbish(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagerecogClient::DetectFruitsOutcome ImagerecogClient::detectFruits(const DetectFruitsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectFruitsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectFruitsOutcome(DetectFruitsResult(outcome.result()));
	else
		return DetectFruitsOutcome(outcome.error());
}

void ImagerecogClient::detectFruitsAsync(const DetectFruitsRequest& request, const DetectFruitsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectFruits(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagerecogClient::DetectFruitsOutcomeCallable ImagerecogClient::detectFruitsCallable(const DetectFruitsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectFruitsOutcome()>>(
			[this, request]()
			{
			return this->detectFruits(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagerecogClient::DetectImageElementsOutcome ImagerecogClient::detectImageElements(const DetectImageElementsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectImageElementsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectImageElementsOutcome(DetectImageElementsResult(outcome.result()));
	else
		return DetectImageElementsOutcome(outcome.error());
}

void ImagerecogClient::detectImageElementsAsync(const DetectImageElementsRequest& request, const DetectImageElementsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectImageElements(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagerecogClient::DetectImageElementsOutcomeCallable ImagerecogClient::detectImageElementsCallable(const DetectImageElementsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectImageElementsOutcome()>>(
			[this, request]()
			{
			return this->detectImageElements(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagerecogClient::RecognizeImageColorOutcome ImagerecogClient::recognizeImageColor(const RecognizeImageColorRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeImageColorOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeImageColorOutcome(RecognizeImageColorResult(outcome.result()));
	else
		return RecognizeImageColorOutcome(outcome.error());
}

void ImagerecogClient::recognizeImageColorAsync(const RecognizeImageColorRequest& request, const RecognizeImageColorAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeImageColor(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagerecogClient::RecognizeImageColorOutcomeCallable ImagerecogClient::recognizeImageColorCallable(const RecognizeImageColorRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeImageColorOutcome()>>(
			[this, request]()
			{
			return this->recognizeImageColor(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagerecogClient::RecognizeImageStyleOutcome ImagerecogClient::recognizeImageStyle(const RecognizeImageStyleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeImageStyleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeImageStyleOutcome(RecognizeImageStyleResult(outcome.result()));
	else
		return RecognizeImageStyleOutcome(outcome.error());
}

void ImagerecogClient::recognizeImageStyleAsync(const RecognizeImageStyleRequest& request, const RecognizeImageStyleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeImageStyle(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagerecogClient::RecognizeImageStyleOutcomeCallable ImagerecogClient::recognizeImageStyleCallable(const RecognizeImageStyleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeImageStyleOutcome()>>(
			[this, request]()
			{
			return this->recognizeImageStyle(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagerecogClient::RecognizeLogoOutcome ImagerecogClient::recognizeLogo(const RecognizeLogoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeLogoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeLogoOutcome(RecognizeLogoResult(outcome.result()));
	else
		return RecognizeLogoOutcome(outcome.error());
}

void ImagerecogClient::recognizeLogoAsync(const RecognizeLogoRequest& request, const RecognizeLogoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeLogo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagerecogClient::RecognizeLogoOutcomeCallable ImagerecogClient::recognizeLogoCallable(const RecognizeLogoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeLogoOutcome()>>(
			[this, request]()
			{
			return this->recognizeLogo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagerecogClient::RecognizeSceneOutcome ImagerecogClient::recognizeScene(const RecognizeSceneRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeSceneOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeSceneOutcome(RecognizeSceneResult(outcome.result()));
	else
		return RecognizeSceneOutcome(outcome.error());
}

void ImagerecogClient::recognizeSceneAsync(const RecognizeSceneRequest& request, const RecognizeSceneAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeScene(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagerecogClient::RecognizeSceneOutcomeCallable ImagerecogClient::recognizeSceneCallable(const RecognizeSceneRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeSceneOutcome()>>(
			[this, request]()
			{
			return this->recognizeScene(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagerecogClient::RecognizeVehicleTypeOutcome ImagerecogClient::recognizeVehicleType(const RecognizeVehicleTypeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeVehicleTypeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeVehicleTypeOutcome(RecognizeVehicleTypeResult(outcome.result()));
	else
		return RecognizeVehicleTypeOutcome(outcome.error());
}

void ImagerecogClient::recognizeVehicleTypeAsync(const RecognizeVehicleTypeRequest& request, const RecognizeVehicleTypeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeVehicleType(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagerecogClient::RecognizeVehicleTypeOutcomeCallable ImagerecogClient::recognizeVehicleTypeCallable(const RecognizeVehicleTypeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeVehicleTypeOutcome()>>(
			[this, request]()
			{
			return this->recognizeVehicleType(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagerecogClient::TaggingImageOutcome ImagerecogClient::taggingImage(const TaggingImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return TaggingImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return TaggingImageOutcome(TaggingImageResult(outcome.result()));
	else
		return TaggingImageOutcome(outcome.error());
}

void ImagerecogClient::taggingImageAsync(const TaggingImageRequest& request, const TaggingImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, taggingImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagerecogClient::TaggingImageOutcomeCallable ImagerecogClient::taggingImageCallable(const TaggingImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<TaggingImageOutcome()>>(
			[this, request]()
			{
			return this->taggingImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


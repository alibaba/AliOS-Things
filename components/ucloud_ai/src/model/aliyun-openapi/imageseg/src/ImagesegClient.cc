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

#include <alibabacloud/imageseg/ImagesegClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Imageseg;
using namespace AlibabaCloud::Imageseg::Model;

namespace
{
	const std::string SERVICE_NAME = "imageseg";
}

ImagesegClient::ImagesegClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imageseg");
}

ImagesegClient::ImagesegClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imageseg");
}

ImagesegClient::ImagesegClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imageseg");
}

ImagesegClient::~ImagesegClient()
{}

ImagesegClient::ChangeSkyOutcome ImagesegClient::changeSky(const ChangeSkyRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ChangeSkyOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ChangeSkyOutcome(ChangeSkyResult(outcome.result()));
	else
		return ChangeSkyOutcome(outcome.error());
}

void ImagesegClient::changeSkyAsync(const ChangeSkyRequest& request, const ChangeSkyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, changeSky(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::ChangeSkyOutcomeCallable ImagesegClient::changeSkyCallable(const ChangeSkyRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ChangeSkyOutcome()>>(
			[this, request]()
			{
			return this->changeSky(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::GetAsyncJobResultOutcome ImagesegClient::getAsyncJobResult(const GetAsyncJobResultRequest &request) const
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

void ImagesegClient::getAsyncJobResultAsync(const GetAsyncJobResultRequest& request, const GetAsyncJobResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getAsyncJobResult(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::GetAsyncJobResultOutcomeCallable ImagesegClient::getAsyncJobResultCallable(const GetAsyncJobResultRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetAsyncJobResultOutcome()>>(
			[this, request]()
			{
			return this->getAsyncJobResult(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::ParseFaceOutcome ImagesegClient::parseFace(const ParseFaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ParseFaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ParseFaceOutcome(ParseFaceResult(outcome.result()));
	else
		return ParseFaceOutcome(outcome.error());
}

void ImagesegClient::parseFaceAsync(const ParseFaceRequest& request, const ParseFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, parseFace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::ParseFaceOutcomeCallable ImagesegClient::parseFaceCallable(const ParseFaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ParseFaceOutcome()>>(
			[this, request]()
			{
			return this->parseFace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::RefineMaskOutcome ImagesegClient::refineMask(const RefineMaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RefineMaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RefineMaskOutcome(RefineMaskResult(outcome.result()));
	else
		return RefineMaskOutcome(outcome.error());
}

void ImagesegClient::refineMaskAsync(const RefineMaskRequest& request, const RefineMaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, refineMask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::RefineMaskOutcomeCallable ImagesegClient::refineMaskCallable(const RefineMaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RefineMaskOutcome()>>(
			[this, request]()
			{
			return this->refineMask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentAnimalOutcome ImagesegClient::segmentAnimal(const SegmentAnimalRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentAnimalOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentAnimalOutcome(SegmentAnimalResult(outcome.result()));
	else
		return SegmentAnimalOutcome(outcome.error());
}

void ImagesegClient::segmentAnimalAsync(const SegmentAnimalRequest& request, const SegmentAnimalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentAnimal(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentAnimalOutcomeCallable ImagesegClient::segmentAnimalCallable(const SegmentAnimalRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentAnimalOutcome()>>(
			[this, request]()
			{
			return this->segmentAnimal(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentBodyOutcome ImagesegClient::segmentBody(const SegmentBodyRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentBodyOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentBodyOutcome(SegmentBodyResult(outcome.result()));
	else
		return SegmentBodyOutcome(outcome.error());
}

void ImagesegClient::segmentBodyAsync(const SegmentBodyRequest& request, const SegmentBodyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentBody(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentBodyOutcomeCallable ImagesegClient::segmentBodyCallable(const SegmentBodyRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentBodyOutcome()>>(
			[this, request]()
			{
			return this->segmentBody(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentClothOutcome ImagesegClient::segmentCloth(const SegmentClothRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentClothOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentClothOutcome(SegmentClothResult(outcome.result()));
	else
		return SegmentClothOutcome(outcome.error());
}

void ImagesegClient::segmentClothAsync(const SegmentClothRequest& request, const SegmentClothAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentCloth(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentClothOutcomeCallable ImagesegClient::segmentClothCallable(const SegmentClothRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentClothOutcome()>>(
			[this, request]()
			{
			return this->segmentCloth(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentCommodityOutcome ImagesegClient::segmentCommodity(const SegmentCommodityRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentCommodityOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentCommodityOutcome(SegmentCommodityResult(outcome.result()));
	else
		return SegmentCommodityOutcome(outcome.error());
}

void ImagesegClient::segmentCommodityAsync(const SegmentCommodityRequest& request, const SegmentCommodityAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentCommodity(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentCommodityOutcomeCallable ImagesegClient::segmentCommodityCallable(const SegmentCommodityRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentCommodityOutcome()>>(
			[this, request]()
			{
			return this->segmentCommodity(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentCommonImageOutcome ImagesegClient::segmentCommonImage(const SegmentCommonImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentCommonImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentCommonImageOutcome(SegmentCommonImageResult(outcome.result()));
	else
		return SegmentCommonImageOutcome(outcome.error());
}

void ImagesegClient::segmentCommonImageAsync(const SegmentCommonImageRequest& request, const SegmentCommonImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentCommonImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentCommonImageOutcomeCallable ImagesegClient::segmentCommonImageCallable(const SegmentCommonImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentCommonImageOutcome()>>(
			[this, request]()
			{
			return this->segmentCommonImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentFaceOutcome ImagesegClient::segmentFace(const SegmentFaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentFaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentFaceOutcome(SegmentFaceResult(outcome.result()));
	else
		return SegmentFaceOutcome(outcome.error());
}

void ImagesegClient::segmentFaceAsync(const SegmentFaceRequest& request, const SegmentFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentFace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentFaceOutcomeCallable ImagesegClient::segmentFaceCallable(const SegmentFaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentFaceOutcome()>>(
			[this, request]()
			{
			return this->segmentFace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentFoodOutcome ImagesegClient::segmentFood(const SegmentFoodRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentFoodOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentFoodOutcome(SegmentFoodResult(outcome.result()));
	else
		return SegmentFoodOutcome(outcome.error());
}

void ImagesegClient::segmentFoodAsync(const SegmentFoodRequest& request, const SegmentFoodAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentFood(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentFoodOutcomeCallable ImagesegClient::segmentFoodCallable(const SegmentFoodRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentFoodOutcome()>>(
			[this, request]()
			{
			return this->segmentFood(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentFurnitureOutcome ImagesegClient::segmentFurniture(const SegmentFurnitureRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentFurnitureOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentFurnitureOutcome(SegmentFurnitureResult(outcome.result()));
	else
		return SegmentFurnitureOutcome(outcome.error());
}

void ImagesegClient::segmentFurnitureAsync(const SegmentFurnitureRequest& request, const SegmentFurnitureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentFurniture(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentFurnitureOutcomeCallable ImagesegClient::segmentFurnitureCallable(const SegmentFurnitureRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentFurnitureOutcome()>>(
			[this, request]()
			{
			return this->segmentFurniture(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentHDBodyOutcome ImagesegClient::segmentHDBody(const SegmentHDBodyRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentHDBodyOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentHDBodyOutcome(SegmentHDBodyResult(outcome.result()));
	else
		return SegmentHDBodyOutcome(outcome.error());
}

void ImagesegClient::segmentHDBodyAsync(const SegmentHDBodyRequest& request, const SegmentHDBodyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentHDBody(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentHDBodyOutcomeCallable ImagesegClient::segmentHDBodyCallable(const SegmentHDBodyRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentHDBodyOutcome()>>(
			[this, request]()
			{
			return this->segmentHDBody(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentHDCommonImageOutcome ImagesegClient::segmentHDCommonImage(const SegmentHDCommonImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentHDCommonImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentHDCommonImageOutcome(SegmentHDCommonImageResult(outcome.result()));
	else
		return SegmentHDCommonImageOutcome(outcome.error());
}

void ImagesegClient::segmentHDCommonImageAsync(const SegmentHDCommonImageRequest& request, const SegmentHDCommonImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentHDCommonImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentHDCommonImageOutcomeCallable ImagesegClient::segmentHDCommonImageCallable(const SegmentHDCommonImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentHDCommonImageOutcome()>>(
			[this, request]()
			{
			return this->segmentHDCommonImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentHDSkyOutcome ImagesegClient::segmentHDSky(const SegmentHDSkyRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentHDSkyOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentHDSkyOutcome(SegmentHDSkyResult(outcome.result()));
	else
		return SegmentHDSkyOutcome(outcome.error());
}

void ImagesegClient::segmentHDSkyAsync(const SegmentHDSkyRequest& request, const SegmentHDSkyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentHDSky(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentHDSkyOutcomeCallable ImagesegClient::segmentHDSkyCallable(const SegmentHDSkyRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentHDSkyOutcome()>>(
			[this, request]()
			{
			return this->segmentHDSky(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentHairOutcome ImagesegClient::segmentHair(const SegmentHairRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentHairOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentHairOutcome(SegmentHairResult(outcome.result()));
	else
		return SegmentHairOutcome(outcome.error());
}

void ImagesegClient::segmentHairAsync(const SegmentHairRequest& request, const SegmentHairAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentHair(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentHairOutcomeCallable ImagesegClient::segmentHairCallable(const SegmentHairRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentHairOutcome()>>(
			[this, request]()
			{
			return this->segmentHair(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentHeadOutcome ImagesegClient::segmentHead(const SegmentHeadRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentHeadOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentHeadOutcome(SegmentHeadResult(outcome.result()));
	else
		return SegmentHeadOutcome(outcome.error());
}

void ImagesegClient::segmentHeadAsync(const SegmentHeadRequest& request, const SegmentHeadAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentHead(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentHeadOutcomeCallable ImagesegClient::segmentHeadCallable(const SegmentHeadRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentHeadOutcome()>>(
			[this, request]()
			{
			return this->segmentHead(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentLogoOutcome ImagesegClient::segmentLogo(const SegmentLogoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentLogoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentLogoOutcome(SegmentLogoResult(outcome.result()));
	else
		return SegmentLogoOutcome(outcome.error());
}

void ImagesegClient::segmentLogoAsync(const SegmentLogoRequest& request, const SegmentLogoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentLogo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentLogoOutcomeCallable ImagesegClient::segmentLogoCallable(const SegmentLogoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentLogoOutcome()>>(
			[this, request]()
			{
			return this->segmentLogo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentSceneOutcome ImagesegClient::segmentScene(const SegmentSceneRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentSceneOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentSceneOutcome(SegmentSceneResult(outcome.result()));
	else
		return SegmentSceneOutcome(outcome.error());
}

void ImagesegClient::segmentSceneAsync(const SegmentSceneRequest& request, const SegmentSceneAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentScene(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentSceneOutcomeCallable ImagesegClient::segmentSceneCallable(const SegmentSceneRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentSceneOutcome()>>(
			[this, request]()
			{
			return this->segmentScene(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentSkinOutcome ImagesegClient::segmentSkin(const SegmentSkinRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentSkinOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentSkinOutcome(SegmentSkinResult(outcome.result()));
	else
		return SegmentSkinOutcome(outcome.error());
}

void ImagesegClient::segmentSkinAsync(const SegmentSkinRequest& request, const SegmentSkinAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentSkin(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentSkinOutcomeCallable ImagesegClient::segmentSkinCallable(const SegmentSkinRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentSkinOutcome()>>(
			[this, request]()
			{
			return this->segmentSkin(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentSkyOutcome ImagesegClient::segmentSky(const SegmentSkyRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentSkyOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentSkyOutcome(SegmentSkyResult(outcome.result()));
	else
		return SegmentSkyOutcome(outcome.error());
}

void ImagesegClient::segmentSkyAsync(const SegmentSkyRequest& request, const SegmentSkyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentSky(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentSkyOutcomeCallable ImagesegClient::segmentSkyCallable(const SegmentSkyRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentSkyOutcome()>>(
			[this, request]()
			{
			return this->segmentSky(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImagesegClient::SegmentVehicleOutcome ImagesegClient::segmentVehicle(const SegmentVehicleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SegmentVehicleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SegmentVehicleOutcome(SegmentVehicleResult(outcome.result()));
	else
		return SegmentVehicleOutcome(outcome.error());
}

void ImagesegClient::segmentVehicleAsync(const SegmentVehicleRequest& request, const SegmentVehicleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, segmentVehicle(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImagesegClient::SegmentVehicleOutcomeCallable ImagesegClient::segmentVehicleCallable(const SegmentVehicleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SegmentVehicleOutcome()>>(
			[this, request]()
			{
			return this->segmentVehicle(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


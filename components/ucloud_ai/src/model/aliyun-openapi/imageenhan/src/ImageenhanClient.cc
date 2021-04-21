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

#include <alibabacloud/imageenhan/ImageenhanClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Imageenhan;
using namespace AlibabaCloud::Imageenhan::Model;

namespace
{
	const std::string SERVICE_NAME = "imageenhan";
}

ImageenhanClient::ImageenhanClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imageenhan");
}

ImageenhanClient::ImageenhanClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imageenhan");
}

ImageenhanClient::ImageenhanClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "imageenhan");
}

ImageenhanClient::~ImageenhanClient()
{}

ImageenhanClient::AssessCompositionOutcome ImageenhanClient::assessComposition(const AssessCompositionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AssessCompositionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AssessCompositionOutcome(AssessCompositionResult(outcome.result()));
	else
		return AssessCompositionOutcome(outcome.error());
}

void ImageenhanClient::assessCompositionAsync(const AssessCompositionRequest& request, const AssessCompositionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, assessComposition(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::AssessCompositionOutcomeCallable ImageenhanClient::assessCompositionCallable(const AssessCompositionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AssessCompositionOutcome()>>(
			[this, request]()
			{
			return this->assessComposition(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::AssessExposureOutcome ImageenhanClient::assessExposure(const AssessExposureRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AssessExposureOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AssessExposureOutcome(AssessExposureResult(outcome.result()));
	else
		return AssessExposureOutcome(outcome.error());
}

void ImageenhanClient::assessExposureAsync(const AssessExposureRequest& request, const AssessExposureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, assessExposure(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::AssessExposureOutcomeCallable ImageenhanClient::assessExposureCallable(const AssessExposureRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AssessExposureOutcome()>>(
			[this, request]()
			{
			return this->assessExposure(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::AssessSharpnessOutcome ImageenhanClient::assessSharpness(const AssessSharpnessRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AssessSharpnessOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AssessSharpnessOutcome(AssessSharpnessResult(outcome.result()));
	else
		return AssessSharpnessOutcome(outcome.error());
}

void ImageenhanClient::assessSharpnessAsync(const AssessSharpnessRequest& request, const AssessSharpnessAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, assessSharpness(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::AssessSharpnessOutcomeCallable ImageenhanClient::assessSharpnessCallable(const AssessSharpnessRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AssessSharpnessOutcome()>>(
			[this, request]()
			{
			return this->assessSharpness(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::ChangeImageSizeOutcome ImageenhanClient::changeImageSize(const ChangeImageSizeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ChangeImageSizeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ChangeImageSizeOutcome(ChangeImageSizeResult(outcome.result()));
	else
		return ChangeImageSizeOutcome(outcome.error());
}

void ImageenhanClient::changeImageSizeAsync(const ChangeImageSizeRequest& request, const ChangeImageSizeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, changeImageSize(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::ChangeImageSizeOutcomeCallable ImageenhanClient::changeImageSizeCallable(const ChangeImageSizeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ChangeImageSizeOutcome()>>(
			[this, request]()
			{
			return this->changeImageSize(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::ColorizeImageOutcome ImageenhanClient::colorizeImage(const ColorizeImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ColorizeImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ColorizeImageOutcome(ColorizeImageResult(outcome.result()));
	else
		return ColorizeImageOutcome(outcome.error());
}

void ImageenhanClient::colorizeImageAsync(const ColorizeImageRequest& request, const ColorizeImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, colorizeImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::ColorizeImageOutcomeCallable ImageenhanClient::colorizeImageCallable(const ColorizeImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ColorizeImageOutcome()>>(
			[this, request]()
			{
			return this->colorizeImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::EnhanceImageColorOutcome ImageenhanClient::enhanceImageColor(const EnhanceImageColorRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return EnhanceImageColorOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return EnhanceImageColorOutcome(EnhanceImageColorResult(outcome.result()));
	else
		return EnhanceImageColorOutcome(outcome.error());
}

void ImageenhanClient::enhanceImageColorAsync(const EnhanceImageColorRequest& request, const EnhanceImageColorAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, enhanceImageColor(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::EnhanceImageColorOutcomeCallable ImageenhanClient::enhanceImageColorCallable(const EnhanceImageColorRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<EnhanceImageColorOutcome()>>(
			[this, request]()
			{
			return this->enhanceImageColor(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::ErasePersonOutcome ImageenhanClient::erasePerson(const ErasePersonRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ErasePersonOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ErasePersonOutcome(ErasePersonResult(outcome.result()));
	else
		return ErasePersonOutcome(outcome.error());
}

void ImageenhanClient::erasePersonAsync(const ErasePersonRequest& request, const ErasePersonAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, erasePerson(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::ErasePersonOutcomeCallable ImageenhanClient::erasePersonCallable(const ErasePersonRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ErasePersonOutcome()>>(
			[this, request]()
			{
			return this->erasePerson(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::ExtendImageStyleOutcome ImageenhanClient::extendImageStyle(const ExtendImageStyleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ExtendImageStyleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ExtendImageStyleOutcome(ExtendImageStyleResult(outcome.result()));
	else
		return ExtendImageStyleOutcome(outcome.error());
}

void ImageenhanClient::extendImageStyleAsync(const ExtendImageStyleRequest& request, const ExtendImageStyleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, extendImageStyle(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::ExtendImageStyleOutcomeCallable ImageenhanClient::extendImageStyleCallable(const ExtendImageStyleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ExtendImageStyleOutcome()>>(
			[this, request]()
			{
			return this->extendImageStyle(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::GenerateDynamicImageOutcome ImageenhanClient::generateDynamicImage(const GenerateDynamicImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GenerateDynamicImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GenerateDynamicImageOutcome(GenerateDynamicImageResult(outcome.result()));
	else
		return GenerateDynamicImageOutcome(outcome.error());
}

void ImageenhanClient::generateDynamicImageAsync(const GenerateDynamicImageRequest& request, const GenerateDynamicImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, generateDynamicImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::GenerateDynamicImageOutcomeCallable ImageenhanClient::generateDynamicImageCallable(const GenerateDynamicImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GenerateDynamicImageOutcome()>>(
			[this, request]()
			{
			return this->generateDynamicImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::GetAsyncJobResultOutcome ImageenhanClient::getAsyncJobResult(const GetAsyncJobResultRequest &request) const
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

void ImageenhanClient::getAsyncJobResultAsync(const GetAsyncJobResultRequest& request, const GetAsyncJobResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getAsyncJobResult(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::GetAsyncJobResultOutcomeCallable ImageenhanClient::getAsyncJobResultCallable(const GetAsyncJobResultRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetAsyncJobResultOutcome()>>(
			[this, request]()
			{
			return this->getAsyncJobResult(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::ImageBlindCharacterWatermarkOutcome ImageenhanClient::imageBlindCharacterWatermark(const ImageBlindCharacterWatermarkRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ImageBlindCharacterWatermarkOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ImageBlindCharacterWatermarkOutcome(ImageBlindCharacterWatermarkResult(outcome.result()));
	else
		return ImageBlindCharacterWatermarkOutcome(outcome.error());
}

void ImageenhanClient::imageBlindCharacterWatermarkAsync(const ImageBlindCharacterWatermarkRequest& request, const ImageBlindCharacterWatermarkAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, imageBlindCharacterWatermark(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::ImageBlindCharacterWatermarkOutcomeCallable ImageenhanClient::imageBlindCharacterWatermarkCallable(const ImageBlindCharacterWatermarkRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ImageBlindCharacterWatermarkOutcome()>>(
			[this, request]()
			{
			return this->imageBlindCharacterWatermark(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::ImageBlindPicWatermarkOutcome ImageenhanClient::imageBlindPicWatermark(const ImageBlindPicWatermarkRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ImageBlindPicWatermarkOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ImageBlindPicWatermarkOutcome(ImageBlindPicWatermarkResult(outcome.result()));
	else
		return ImageBlindPicWatermarkOutcome(outcome.error());
}

void ImageenhanClient::imageBlindPicWatermarkAsync(const ImageBlindPicWatermarkRequest& request, const ImageBlindPicWatermarkAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, imageBlindPicWatermark(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::ImageBlindPicWatermarkOutcomeCallable ImageenhanClient::imageBlindPicWatermarkCallable(const ImageBlindPicWatermarkRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ImageBlindPicWatermarkOutcome()>>(
			[this, request]()
			{
			return this->imageBlindPicWatermark(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::ImitatePhotoStyleOutcome ImageenhanClient::imitatePhotoStyle(const ImitatePhotoStyleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ImitatePhotoStyleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ImitatePhotoStyleOutcome(ImitatePhotoStyleResult(outcome.result()));
	else
		return ImitatePhotoStyleOutcome(outcome.error());
}

void ImageenhanClient::imitatePhotoStyleAsync(const ImitatePhotoStyleRequest& request, const ImitatePhotoStyleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, imitatePhotoStyle(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::ImitatePhotoStyleOutcomeCallable ImageenhanClient::imitatePhotoStyleCallable(const ImitatePhotoStyleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ImitatePhotoStyleOutcome()>>(
			[this, request]()
			{
			return this->imitatePhotoStyle(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::IntelligentCompositionOutcome ImageenhanClient::intelligentComposition(const IntelligentCompositionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return IntelligentCompositionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return IntelligentCompositionOutcome(IntelligentCompositionResult(outcome.result()));
	else
		return IntelligentCompositionOutcome(outcome.error());
}

void ImageenhanClient::intelligentCompositionAsync(const IntelligentCompositionRequest& request, const IntelligentCompositionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, intelligentComposition(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::IntelligentCompositionOutcomeCallable ImageenhanClient::intelligentCompositionCallable(const IntelligentCompositionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<IntelligentCompositionOutcome()>>(
			[this, request]()
			{
			return this->intelligentComposition(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::MakeSuperResolutionImageOutcome ImageenhanClient::makeSuperResolutionImage(const MakeSuperResolutionImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return MakeSuperResolutionImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return MakeSuperResolutionImageOutcome(MakeSuperResolutionImageResult(outcome.result()));
	else
		return MakeSuperResolutionImageOutcome(outcome.error());
}

void ImageenhanClient::makeSuperResolutionImageAsync(const MakeSuperResolutionImageRequest& request, const MakeSuperResolutionImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, makeSuperResolutionImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::MakeSuperResolutionImageOutcomeCallable ImageenhanClient::makeSuperResolutionImageCallable(const MakeSuperResolutionImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<MakeSuperResolutionImageOutcome()>>(
			[this, request]()
			{
			return this->makeSuperResolutionImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::RecolorHDImageOutcome ImageenhanClient::recolorHDImage(const RecolorHDImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecolorHDImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecolorHDImageOutcome(RecolorHDImageResult(outcome.result()));
	else
		return RecolorHDImageOutcome(outcome.error());
}

void ImageenhanClient::recolorHDImageAsync(const RecolorHDImageRequest& request, const RecolorHDImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recolorHDImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::RecolorHDImageOutcomeCallable ImageenhanClient::recolorHDImageCallable(const RecolorHDImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecolorHDImageOutcome()>>(
			[this, request]()
			{
			return this->recolorHDImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::RecolorImageOutcome ImageenhanClient::recolorImage(const RecolorImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecolorImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecolorImageOutcome(RecolorImageResult(outcome.result()));
	else
		return RecolorImageOutcome(outcome.error());
}

void ImageenhanClient::recolorImageAsync(const RecolorImageRequest& request, const RecolorImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recolorImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::RecolorImageOutcomeCallable ImageenhanClient::recolorImageCallable(const RecolorImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecolorImageOutcome()>>(
			[this, request]()
			{
			return this->recolorImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::RemoveImageSubtitlesOutcome ImageenhanClient::removeImageSubtitles(const RemoveImageSubtitlesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveImageSubtitlesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveImageSubtitlesOutcome(RemoveImageSubtitlesResult(outcome.result()));
	else
		return RemoveImageSubtitlesOutcome(outcome.error());
}

void ImageenhanClient::removeImageSubtitlesAsync(const RemoveImageSubtitlesRequest& request, const RemoveImageSubtitlesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeImageSubtitles(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::RemoveImageSubtitlesOutcomeCallable ImageenhanClient::removeImageSubtitlesCallable(const RemoveImageSubtitlesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveImageSubtitlesOutcome()>>(
			[this, request]()
			{
			return this->removeImageSubtitles(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ImageenhanClient::RemoveImageWatermarkOutcome ImageenhanClient::removeImageWatermark(const RemoveImageWatermarkRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveImageWatermarkOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveImageWatermarkOutcome(RemoveImageWatermarkResult(outcome.result()));
	else
		return RemoveImageWatermarkOutcome(outcome.error());
}

void ImageenhanClient::removeImageWatermarkAsync(const RemoveImageWatermarkRequest& request, const RemoveImageWatermarkAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeImageWatermark(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ImageenhanClient::RemoveImageWatermarkOutcomeCallable ImageenhanClient::removeImageWatermarkCallable(const RemoveImageWatermarkRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveImageWatermarkOutcome()>>(
			[this, request]()
			{
			return this->removeImageWatermark(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


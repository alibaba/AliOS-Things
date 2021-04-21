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

#ifndef ALIBABACLOUD_IMAGEENHAN_IMAGEENHANCLIENT_H_
#define ALIBABACLOUD_IMAGEENHAN_IMAGEENHANCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "ImageenhanExport.h"
#include "model/AssessCompositionRequest.h"
#include "model/AssessCompositionResult.h"
#include "model/AssessExposureRequest.h"
#include "model/AssessExposureResult.h"
#include "model/AssessSharpnessRequest.h"
#include "model/AssessSharpnessResult.h"
#include "model/ChangeImageSizeRequest.h"
#include "model/ChangeImageSizeResult.h"
#include "model/ColorizeImageRequest.h"
#include "model/ColorizeImageResult.h"
#include "model/EnhanceImageColorRequest.h"
#include "model/EnhanceImageColorResult.h"
#include "model/ErasePersonRequest.h"
#include "model/ErasePersonResult.h"
#include "model/ExtendImageStyleRequest.h"
#include "model/ExtendImageStyleResult.h"
#include "model/GenerateDynamicImageRequest.h"
#include "model/GenerateDynamicImageResult.h"
#include "model/GetAsyncJobResultRequest.h"
#include "model/GetAsyncJobResultResult.h"
#include "model/ImageBlindCharacterWatermarkRequest.h"
#include "model/ImageBlindCharacterWatermarkResult.h"
#include "model/ImageBlindPicWatermarkRequest.h"
#include "model/ImageBlindPicWatermarkResult.h"
#include "model/ImitatePhotoStyleRequest.h"
#include "model/ImitatePhotoStyleResult.h"
#include "model/IntelligentCompositionRequest.h"
#include "model/IntelligentCompositionResult.h"
#include "model/MakeSuperResolutionImageRequest.h"
#include "model/MakeSuperResolutionImageResult.h"
#include "model/RecolorHDImageRequest.h"
#include "model/RecolorHDImageResult.h"
#include "model/RecolorImageRequest.h"
#include "model/RecolorImageResult.h"
#include "model/RemoveImageSubtitlesRequest.h"
#include "model/RemoveImageSubtitlesResult.h"
#include "model/RemoveImageWatermarkRequest.h"
#include "model/RemoveImageWatermarkResult.h"


namespace AlibabaCloud
{
	namespace Imageenhan
	{
		class ALIBABACLOUD_IMAGEENHAN_EXPORT ImageenhanClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::AssessCompositionResult> AssessCompositionOutcome;
			typedef std::future<AssessCompositionOutcome> AssessCompositionOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::AssessCompositionRequest&, const AssessCompositionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AssessCompositionAsyncHandler;
			typedef Outcome<Error, Model::AssessExposureResult> AssessExposureOutcome;
			typedef std::future<AssessExposureOutcome> AssessExposureOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::AssessExposureRequest&, const AssessExposureOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AssessExposureAsyncHandler;
			typedef Outcome<Error, Model::AssessSharpnessResult> AssessSharpnessOutcome;
			typedef std::future<AssessSharpnessOutcome> AssessSharpnessOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::AssessSharpnessRequest&, const AssessSharpnessOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AssessSharpnessAsyncHandler;
			typedef Outcome<Error, Model::ChangeImageSizeResult> ChangeImageSizeOutcome;
			typedef std::future<ChangeImageSizeOutcome> ChangeImageSizeOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::ChangeImageSizeRequest&, const ChangeImageSizeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ChangeImageSizeAsyncHandler;
			typedef Outcome<Error, Model::ColorizeImageResult> ColorizeImageOutcome;
			typedef std::future<ColorizeImageOutcome> ColorizeImageOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::ColorizeImageRequest&, const ColorizeImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ColorizeImageAsyncHandler;
			typedef Outcome<Error, Model::EnhanceImageColorResult> EnhanceImageColorOutcome;
			typedef std::future<EnhanceImageColorOutcome> EnhanceImageColorOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::EnhanceImageColorRequest&, const EnhanceImageColorOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EnhanceImageColorAsyncHandler;
			typedef Outcome<Error, Model::ErasePersonResult> ErasePersonOutcome;
			typedef std::future<ErasePersonOutcome> ErasePersonOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::ErasePersonRequest&, const ErasePersonOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ErasePersonAsyncHandler;
			typedef Outcome<Error, Model::ExtendImageStyleResult> ExtendImageStyleOutcome;
			typedef std::future<ExtendImageStyleOutcome> ExtendImageStyleOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::ExtendImageStyleRequest&, const ExtendImageStyleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ExtendImageStyleAsyncHandler;
			typedef Outcome<Error, Model::GenerateDynamicImageResult> GenerateDynamicImageOutcome;
			typedef std::future<GenerateDynamicImageOutcome> GenerateDynamicImageOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::GenerateDynamicImageRequest&, const GenerateDynamicImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GenerateDynamicImageAsyncHandler;
			typedef Outcome<Error, Model::GetAsyncJobResultResult> GetAsyncJobResultOutcome;
			typedef std::future<GetAsyncJobResultOutcome> GetAsyncJobResultOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::GetAsyncJobResultRequest&, const GetAsyncJobResultOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetAsyncJobResultAsyncHandler;
			typedef Outcome<Error, Model::ImageBlindCharacterWatermarkResult> ImageBlindCharacterWatermarkOutcome;
			typedef std::future<ImageBlindCharacterWatermarkOutcome> ImageBlindCharacterWatermarkOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::ImageBlindCharacterWatermarkRequest&, const ImageBlindCharacterWatermarkOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ImageBlindCharacterWatermarkAsyncHandler;
			typedef Outcome<Error, Model::ImageBlindPicWatermarkResult> ImageBlindPicWatermarkOutcome;
			typedef std::future<ImageBlindPicWatermarkOutcome> ImageBlindPicWatermarkOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::ImageBlindPicWatermarkRequest&, const ImageBlindPicWatermarkOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ImageBlindPicWatermarkAsyncHandler;
			typedef Outcome<Error, Model::ImitatePhotoStyleResult> ImitatePhotoStyleOutcome;
			typedef std::future<ImitatePhotoStyleOutcome> ImitatePhotoStyleOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::ImitatePhotoStyleRequest&, const ImitatePhotoStyleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ImitatePhotoStyleAsyncHandler;
			typedef Outcome<Error, Model::IntelligentCompositionResult> IntelligentCompositionOutcome;
			typedef std::future<IntelligentCompositionOutcome> IntelligentCompositionOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::IntelligentCompositionRequest&, const IntelligentCompositionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> IntelligentCompositionAsyncHandler;
			typedef Outcome<Error, Model::MakeSuperResolutionImageResult> MakeSuperResolutionImageOutcome;
			typedef std::future<MakeSuperResolutionImageOutcome> MakeSuperResolutionImageOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::MakeSuperResolutionImageRequest&, const MakeSuperResolutionImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> MakeSuperResolutionImageAsyncHandler;
			typedef Outcome<Error, Model::RecolorHDImageResult> RecolorHDImageOutcome;
			typedef std::future<RecolorHDImageOutcome> RecolorHDImageOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::RecolorHDImageRequest&, const RecolorHDImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecolorHDImageAsyncHandler;
			typedef Outcome<Error, Model::RecolorImageResult> RecolorImageOutcome;
			typedef std::future<RecolorImageOutcome> RecolorImageOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::RecolorImageRequest&, const RecolorImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecolorImageAsyncHandler;
			typedef Outcome<Error, Model::RemoveImageSubtitlesResult> RemoveImageSubtitlesOutcome;
			typedef std::future<RemoveImageSubtitlesOutcome> RemoveImageSubtitlesOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::RemoveImageSubtitlesRequest&, const RemoveImageSubtitlesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RemoveImageSubtitlesAsyncHandler;
			typedef Outcome<Error, Model::RemoveImageWatermarkResult> RemoveImageWatermarkOutcome;
			typedef std::future<RemoveImageWatermarkOutcome> RemoveImageWatermarkOutcomeCallable;
			typedef std::function<void(const ImageenhanClient*, const Model::RemoveImageWatermarkRequest&, const RemoveImageWatermarkOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RemoveImageWatermarkAsyncHandler;

			ImageenhanClient(const Credentials &credentials, const ClientConfiguration &configuration);
			ImageenhanClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			ImageenhanClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~ImageenhanClient();
			AssessCompositionOutcome assessComposition(const Model::AssessCompositionRequest &request)const;
			void assessCompositionAsync(const Model::AssessCompositionRequest& request, const AssessCompositionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AssessCompositionOutcomeCallable assessCompositionCallable(const Model::AssessCompositionRequest& request) const;
			AssessExposureOutcome assessExposure(const Model::AssessExposureRequest &request)const;
			void assessExposureAsync(const Model::AssessExposureRequest& request, const AssessExposureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AssessExposureOutcomeCallable assessExposureCallable(const Model::AssessExposureRequest& request) const;
			AssessSharpnessOutcome assessSharpness(const Model::AssessSharpnessRequest &request)const;
			void assessSharpnessAsync(const Model::AssessSharpnessRequest& request, const AssessSharpnessAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AssessSharpnessOutcomeCallable assessSharpnessCallable(const Model::AssessSharpnessRequest& request) const;
			ChangeImageSizeOutcome changeImageSize(const Model::ChangeImageSizeRequest &request)const;
			void changeImageSizeAsync(const Model::ChangeImageSizeRequest& request, const ChangeImageSizeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ChangeImageSizeOutcomeCallable changeImageSizeCallable(const Model::ChangeImageSizeRequest& request) const;
			ColorizeImageOutcome colorizeImage(const Model::ColorizeImageRequest &request)const;
			void colorizeImageAsync(const Model::ColorizeImageRequest& request, const ColorizeImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ColorizeImageOutcomeCallable colorizeImageCallable(const Model::ColorizeImageRequest& request) const;
			EnhanceImageColorOutcome enhanceImageColor(const Model::EnhanceImageColorRequest &request)const;
			void enhanceImageColorAsync(const Model::EnhanceImageColorRequest& request, const EnhanceImageColorAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EnhanceImageColorOutcomeCallable enhanceImageColorCallable(const Model::EnhanceImageColorRequest& request) const;
			ErasePersonOutcome erasePerson(const Model::ErasePersonRequest &request)const;
			void erasePersonAsync(const Model::ErasePersonRequest& request, const ErasePersonAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ErasePersonOutcomeCallable erasePersonCallable(const Model::ErasePersonRequest& request) const;
			ExtendImageStyleOutcome extendImageStyle(const Model::ExtendImageStyleRequest &request)const;
			void extendImageStyleAsync(const Model::ExtendImageStyleRequest& request, const ExtendImageStyleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ExtendImageStyleOutcomeCallable extendImageStyleCallable(const Model::ExtendImageStyleRequest& request) const;
			GenerateDynamicImageOutcome generateDynamicImage(const Model::GenerateDynamicImageRequest &request)const;
			void generateDynamicImageAsync(const Model::GenerateDynamicImageRequest& request, const GenerateDynamicImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GenerateDynamicImageOutcomeCallable generateDynamicImageCallable(const Model::GenerateDynamicImageRequest& request) const;
			GetAsyncJobResultOutcome getAsyncJobResult(const Model::GetAsyncJobResultRequest &request)const;
			void getAsyncJobResultAsync(const Model::GetAsyncJobResultRequest& request, const GetAsyncJobResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetAsyncJobResultOutcomeCallable getAsyncJobResultCallable(const Model::GetAsyncJobResultRequest& request) const;
			ImageBlindCharacterWatermarkOutcome imageBlindCharacterWatermark(const Model::ImageBlindCharacterWatermarkRequest &request)const;
			void imageBlindCharacterWatermarkAsync(const Model::ImageBlindCharacterWatermarkRequest& request, const ImageBlindCharacterWatermarkAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ImageBlindCharacterWatermarkOutcomeCallable imageBlindCharacterWatermarkCallable(const Model::ImageBlindCharacterWatermarkRequest& request) const;
			ImageBlindPicWatermarkOutcome imageBlindPicWatermark(const Model::ImageBlindPicWatermarkRequest &request)const;
			void imageBlindPicWatermarkAsync(const Model::ImageBlindPicWatermarkRequest& request, const ImageBlindPicWatermarkAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ImageBlindPicWatermarkOutcomeCallable imageBlindPicWatermarkCallable(const Model::ImageBlindPicWatermarkRequest& request) const;
			ImitatePhotoStyleOutcome imitatePhotoStyle(const Model::ImitatePhotoStyleRequest &request)const;
			void imitatePhotoStyleAsync(const Model::ImitatePhotoStyleRequest& request, const ImitatePhotoStyleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ImitatePhotoStyleOutcomeCallable imitatePhotoStyleCallable(const Model::ImitatePhotoStyleRequest& request) const;
			IntelligentCompositionOutcome intelligentComposition(const Model::IntelligentCompositionRequest &request)const;
			void intelligentCompositionAsync(const Model::IntelligentCompositionRequest& request, const IntelligentCompositionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			IntelligentCompositionOutcomeCallable intelligentCompositionCallable(const Model::IntelligentCompositionRequest& request) const;
			MakeSuperResolutionImageOutcome makeSuperResolutionImage(const Model::MakeSuperResolutionImageRequest &request)const;
			void makeSuperResolutionImageAsync(const Model::MakeSuperResolutionImageRequest& request, const MakeSuperResolutionImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			MakeSuperResolutionImageOutcomeCallable makeSuperResolutionImageCallable(const Model::MakeSuperResolutionImageRequest& request) const;
			RecolorHDImageOutcome recolorHDImage(const Model::RecolorHDImageRequest &request)const;
			void recolorHDImageAsync(const Model::RecolorHDImageRequest& request, const RecolorHDImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecolorHDImageOutcomeCallable recolorHDImageCallable(const Model::RecolorHDImageRequest& request) const;
			RecolorImageOutcome recolorImage(const Model::RecolorImageRequest &request)const;
			void recolorImageAsync(const Model::RecolorImageRequest& request, const RecolorImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecolorImageOutcomeCallable recolorImageCallable(const Model::RecolorImageRequest& request) const;
			RemoveImageSubtitlesOutcome removeImageSubtitles(const Model::RemoveImageSubtitlesRequest &request)const;
			void removeImageSubtitlesAsync(const Model::RemoveImageSubtitlesRequest& request, const RemoveImageSubtitlesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RemoveImageSubtitlesOutcomeCallable removeImageSubtitlesCallable(const Model::RemoveImageSubtitlesRequest& request) const;
			RemoveImageWatermarkOutcome removeImageWatermark(const Model::RemoveImageWatermarkRequest &request)const;
			void removeImageWatermarkAsync(const Model::RemoveImageWatermarkRequest& request, const RemoveImageWatermarkAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RemoveImageWatermarkOutcomeCallable removeImageWatermarkCallable(const Model::RemoveImageWatermarkRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_IMAGEENHAN_IMAGEENHANCLIENT_H_

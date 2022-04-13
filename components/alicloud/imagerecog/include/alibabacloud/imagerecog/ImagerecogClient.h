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

#ifndef ALIBABACLOUD_IMAGERECOG_IMAGERECOGCLIENT_H_
#define ALIBABACLOUD_IMAGERECOG_IMAGERECOGCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "ImagerecogExport.h"
#include "model/ClassifyingRubbishRequest.h"
#include "model/ClassifyingRubbishResult.h"
#include "model/DetectFruitsRequest.h"
#include "model/DetectFruitsResult.h"
#include "model/DetectImageElementsRequest.h"
#include "model/DetectImageElementsResult.h"
#include "model/RecognizeImageColorRequest.h"
#include "model/RecognizeImageColorResult.h"
#include "model/RecognizeImageStyleRequest.h"
#include "model/RecognizeImageStyleResult.h"
#include "model/RecognizeLogoRequest.h"
#include "model/RecognizeLogoResult.h"
#include "model/RecognizeSceneRequest.h"
#include "model/RecognizeSceneResult.h"
#include "model/RecognizeVehicleTypeRequest.h"
#include "model/RecognizeVehicleTypeResult.h"
#include "model/TaggingImageRequest.h"
#include "model/TaggingImageResult.h"


namespace AlibabaCloud
{
	namespace Imagerecog
	{
		class ALIBABACLOUD_IMAGERECOG_EXPORT ImagerecogClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::ClassifyingRubbishResult> ClassifyingRubbishOutcome;
			typedef std::future<ClassifyingRubbishOutcome> ClassifyingRubbishOutcomeCallable;
			typedef std::function<void(const ImagerecogClient*, const Model::ClassifyingRubbishRequest&, const ClassifyingRubbishOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ClassifyingRubbishAsyncHandler;
			typedef Outcome<Error, Model::DetectFruitsResult> DetectFruitsOutcome;
			typedef std::future<DetectFruitsOutcome> DetectFruitsOutcomeCallable;
			typedef std::function<void(const ImagerecogClient*, const Model::DetectFruitsRequest&, const DetectFruitsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DetectFruitsAsyncHandler;
			typedef Outcome<Error, Model::DetectImageElementsResult> DetectImageElementsOutcome;
			typedef std::future<DetectImageElementsOutcome> DetectImageElementsOutcomeCallable;
			typedef std::function<void(const ImagerecogClient*, const Model::DetectImageElementsRequest&, const DetectImageElementsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DetectImageElementsAsyncHandler;
			typedef Outcome<Error, Model::RecognizeImageColorResult> RecognizeImageColorOutcome;
			typedef std::future<RecognizeImageColorOutcome> RecognizeImageColorOutcomeCallable;
			typedef std::function<void(const ImagerecogClient*, const Model::RecognizeImageColorRequest&, const RecognizeImageColorOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeImageColorAsyncHandler;
			typedef Outcome<Error, Model::RecognizeImageStyleResult> RecognizeImageStyleOutcome;
			typedef std::future<RecognizeImageStyleOutcome> RecognizeImageStyleOutcomeCallable;
			typedef std::function<void(const ImagerecogClient*, const Model::RecognizeImageStyleRequest&, const RecognizeImageStyleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeImageStyleAsyncHandler;
			typedef Outcome<Error, Model::RecognizeLogoResult> RecognizeLogoOutcome;
			typedef std::future<RecognizeLogoOutcome> RecognizeLogoOutcomeCallable;
			typedef std::function<void(const ImagerecogClient*, const Model::RecognizeLogoRequest&, const RecognizeLogoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeLogoAsyncHandler;
			typedef Outcome<Error, Model::RecognizeSceneResult> RecognizeSceneOutcome;
			typedef std::future<RecognizeSceneOutcome> RecognizeSceneOutcomeCallable;
			typedef std::function<void(const ImagerecogClient*, const Model::RecognizeSceneRequest&, const RecognizeSceneOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeSceneAsyncHandler;
			typedef Outcome<Error, Model::RecognizeVehicleTypeResult> RecognizeVehicleTypeOutcome;
			typedef std::future<RecognizeVehicleTypeOutcome> RecognizeVehicleTypeOutcomeCallable;
			typedef std::function<void(const ImagerecogClient*, const Model::RecognizeVehicleTypeRequest&, const RecognizeVehicleTypeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeVehicleTypeAsyncHandler;
			typedef Outcome<Error, Model::TaggingImageResult> TaggingImageOutcome;
			typedef std::future<TaggingImageOutcome> TaggingImageOutcomeCallable;
			typedef std::function<void(const ImagerecogClient*, const Model::TaggingImageRequest&, const TaggingImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> TaggingImageAsyncHandler;

			ImagerecogClient(const Credentials &credentials, const ClientConfiguration &configuration);
			ImagerecogClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			ImagerecogClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~ImagerecogClient();
			ClassifyingRubbishOutcome classifyingRubbish(const Model::ClassifyingRubbishRequest &request)const;
			void classifyingRubbishAsync(const Model::ClassifyingRubbishRequest& request, const ClassifyingRubbishAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ClassifyingRubbishOutcomeCallable classifyingRubbishCallable(const Model::ClassifyingRubbishRequest& request) const;
			DetectFruitsOutcome detectFruits(const Model::DetectFruitsRequest &request)const;
			void detectFruitsAsync(const Model::DetectFruitsRequest& request, const DetectFruitsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DetectFruitsOutcomeCallable detectFruitsCallable(const Model::DetectFruitsRequest& request) const;
			DetectImageElementsOutcome detectImageElements(const Model::DetectImageElementsRequest &request)const;
			void detectImageElementsAsync(const Model::DetectImageElementsRequest& request, const DetectImageElementsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DetectImageElementsOutcomeCallable detectImageElementsCallable(const Model::DetectImageElementsRequest& request) const;
			RecognizeImageColorOutcome recognizeImageColor(const Model::RecognizeImageColorRequest &request)const;
			void recognizeImageColorAsync(const Model::RecognizeImageColorRequest& request, const RecognizeImageColorAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeImageColorOutcomeCallable recognizeImageColorCallable(const Model::RecognizeImageColorRequest& request) const;
			RecognizeImageStyleOutcome recognizeImageStyle(const Model::RecognizeImageStyleRequest &request)const;
			void recognizeImageStyleAsync(const Model::RecognizeImageStyleRequest& request, const RecognizeImageStyleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeImageStyleOutcomeCallable recognizeImageStyleCallable(const Model::RecognizeImageStyleRequest& request) const;
			RecognizeLogoOutcome recognizeLogo(const Model::RecognizeLogoRequest &request)const;
			void recognizeLogoAsync(const Model::RecognizeLogoRequest& request, const RecognizeLogoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeLogoOutcomeCallable recognizeLogoCallable(const Model::RecognizeLogoRequest& request) const;
			RecognizeSceneOutcome recognizeScene(const Model::RecognizeSceneRequest &request)const;
			void recognizeSceneAsync(const Model::RecognizeSceneRequest& request, const RecognizeSceneAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeSceneOutcomeCallable recognizeSceneCallable(const Model::RecognizeSceneRequest& request) const;
			RecognizeVehicleTypeOutcome recognizeVehicleType(const Model::RecognizeVehicleTypeRequest &request)const;
			void recognizeVehicleTypeAsync(const Model::RecognizeVehicleTypeRequest& request, const RecognizeVehicleTypeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeVehicleTypeOutcomeCallable recognizeVehicleTypeCallable(const Model::RecognizeVehicleTypeRequest& request) const;
			TaggingImageOutcome taggingImage(const Model::TaggingImageRequest &request)const;
			void taggingImageAsync(const Model::TaggingImageRequest& request, const TaggingImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			TaggingImageOutcomeCallable taggingImageCallable(const Model::TaggingImageRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_IMAGERECOG_IMAGERECOGCLIENT_H_

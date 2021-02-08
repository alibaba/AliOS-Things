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

#ifndef ALIBABACLOUD_IMAGESEG_IMAGESEGCLIENT_H_
#define ALIBABACLOUD_IMAGESEG_IMAGESEGCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "ImagesegExport.h"
#include "model/ChangeSkyRequest.h"
#include "model/ChangeSkyResult.h"
#include "model/GetAsyncJobResultRequest.h"
#include "model/GetAsyncJobResultResult.h"
#include "model/ParseFaceRequest.h"
#include "model/ParseFaceResult.h"
#include "model/RefineMaskRequest.h"
#include "model/RefineMaskResult.h"
#include "model/SegmentAnimalRequest.h"
#include "model/SegmentAnimalResult.h"
#include "model/SegmentBodyRequest.h"
#include "model/SegmentBodyResult.h"
#include "model/SegmentClothRequest.h"
#include "model/SegmentClothResult.h"
#include "model/SegmentCommodityRequest.h"
#include "model/SegmentCommodityResult.h"
#include "model/SegmentCommonImageRequest.h"
#include "model/SegmentCommonImageResult.h"
#include "model/SegmentFaceRequest.h"
#include "model/SegmentFaceResult.h"
#include "model/SegmentFoodRequest.h"
#include "model/SegmentFoodResult.h"
#include "model/SegmentFurnitureRequest.h"
#include "model/SegmentFurnitureResult.h"
#include "model/SegmentHDBodyRequest.h"
#include "model/SegmentHDBodyResult.h"
#include "model/SegmentHDCommonImageRequest.h"
#include "model/SegmentHDCommonImageResult.h"
#include "model/SegmentHDSkyRequest.h"
#include "model/SegmentHDSkyResult.h"
#include "model/SegmentHairRequest.h"
#include "model/SegmentHairResult.h"
#include "model/SegmentHeadRequest.h"
#include "model/SegmentHeadResult.h"
#include "model/SegmentLogoRequest.h"
#include "model/SegmentLogoResult.h"
#include "model/SegmentSceneRequest.h"
#include "model/SegmentSceneResult.h"
#include "model/SegmentSkinRequest.h"
#include "model/SegmentSkinResult.h"
#include "model/SegmentSkyRequest.h"
#include "model/SegmentSkyResult.h"
#include "model/SegmentVehicleRequest.h"
#include "model/SegmentVehicleResult.h"


namespace AlibabaCloud
{
	namespace Imageseg
	{
		class ALIBABACLOUD_IMAGESEG_EXPORT ImagesegClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::ChangeSkyResult> ChangeSkyOutcome;
			typedef std::future<ChangeSkyOutcome> ChangeSkyOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::ChangeSkyRequest&, const ChangeSkyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ChangeSkyAsyncHandler;
			typedef Outcome<Error, Model::GetAsyncJobResultResult> GetAsyncJobResultOutcome;
			typedef std::future<GetAsyncJobResultOutcome> GetAsyncJobResultOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::GetAsyncJobResultRequest&, const GetAsyncJobResultOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetAsyncJobResultAsyncHandler;
			typedef Outcome<Error, Model::ParseFaceResult> ParseFaceOutcome;
			typedef std::future<ParseFaceOutcome> ParseFaceOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::ParseFaceRequest&, const ParseFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ParseFaceAsyncHandler;
			typedef Outcome<Error, Model::RefineMaskResult> RefineMaskOutcome;
			typedef std::future<RefineMaskOutcome> RefineMaskOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::RefineMaskRequest&, const RefineMaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RefineMaskAsyncHandler;
			typedef Outcome<Error, Model::SegmentAnimalResult> SegmentAnimalOutcome;
			typedef std::future<SegmentAnimalOutcome> SegmentAnimalOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentAnimalRequest&, const SegmentAnimalOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentAnimalAsyncHandler;
			typedef Outcome<Error, Model::SegmentBodyResult> SegmentBodyOutcome;
			typedef std::future<SegmentBodyOutcome> SegmentBodyOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentBodyRequest&, const SegmentBodyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentBodyAsyncHandler;
			typedef Outcome<Error, Model::SegmentClothResult> SegmentClothOutcome;
			typedef std::future<SegmentClothOutcome> SegmentClothOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentClothRequest&, const SegmentClothOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentClothAsyncHandler;
			typedef Outcome<Error, Model::SegmentCommodityResult> SegmentCommodityOutcome;
			typedef std::future<SegmentCommodityOutcome> SegmentCommodityOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentCommodityRequest&, const SegmentCommodityOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentCommodityAsyncHandler;
			typedef Outcome<Error, Model::SegmentCommonImageResult> SegmentCommonImageOutcome;
			typedef std::future<SegmentCommonImageOutcome> SegmentCommonImageOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentCommonImageRequest&, const SegmentCommonImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentCommonImageAsyncHandler;
			typedef Outcome<Error, Model::SegmentFaceResult> SegmentFaceOutcome;
			typedef std::future<SegmentFaceOutcome> SegmentFaceOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentFaceRequest&, const SegmentFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentFaceAsyncHandler;
			typedef Outcome<Error, Model::SegmentFoodResult> SegmentFoodOutcome;
			typedef std::future<SegmentFoodOutcome> SegmentFoodOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentFoodRequest&, const SegmentFoodOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentFoodAsyncHandler;
			typedef Outcome<Error, Model::SegmentFurnitureResult> SegmentFurnitureOutcome;
			typedef std::future<SegmentFurnitureOutcome> SegmentFurnitureOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentFurnitureRequest&, const SegmentFurnitureOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentFurnitureAsyncHandler;
			typedef Outcome<Error, Model::SegmentHDBodyResult> SegmentHDBodyOutcome;
			typedef std::future<SegmentHDBodyOutcome> SegmentHDBodyOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentHDBodyRequest&, const SegmentHDBodyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentHDBodyAsyncHandler;
			typedef Outcome<Error, Model::SegmentHDCommonImageResult> SegmentHDCommonImageOutcome;
			typedef std::future<SegmentHDCommonImageOutcome> SegmentHDCommonImageOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentHDCommonImageRequest&, const SegmentHDCommonImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentHDCommonImageAsyncHandler;
			typedef Outcome<Error, Model::SegmentHDSkyResult> SegmentHDSkyOutcome;
			typedef std::future<SegmentHDSkyOutcome> SegmentHDSkyOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentHDSkyRequest&, const SegmentHDSkyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentHDSkyAsyncHandler;
			typedef Outcome<Error, Model::SegmentHairResult> SegmentHairOutcome;
			typedef std::future<SegmentHairOutcome> SegmentHairOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentHairRequest&, const SegmentHairOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentHairAsyncHandler;
			typedef Outcome<Error, Model::SegmentHeadResult> SegmentHeadOutcome;
			typedef std::future<SegmentHeadOutcome> SegmentHeadOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentHeadRequest&, const SegmentHeadOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentHeadAsyncHandler;
			typedef Outcome<Error, Model::SegmentLogoResult> SegmentLogoOutcome;
			typedef std::future<SegmentLogoOutcome> SegmentLogoOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentLogoRequest&, const SegmentLogoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentLogoAsyncHandler;
			typedef Outcome<Error, Model::SegmentSceneResult> SegmentSceneOutcome;
			typedef std::future<SegmentSceneOutcome> SegmentSceneOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentSceneRequest&, const SegmentSceneOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentSceneAsyncHandler;
			typedef Outcome<Error, Model::SegmentSkinResult> SegmentSkinOutcome;
			typedef std::future<SegmentSkinOutcome> SegmentSkinOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentSkinRequest&, const SegmentSkinOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentSkinAsyncHandler;
			typedef Outcome<Error, Model::SegmentSkyResult> SegmentSkyOutcome;
			typedef std::future<SegmentSkyOutcome> SegmentSkyOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentSkyRequest&, const SegmentSkyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentSkyAsyncHandler;
			typedef Outcome<Error, Model::SegmentVehicleResult> SegmentVehicleOutcome;
			typedef std::future<SegmentVehicleOutcome> SegmentVehicleOutcomeCallable;
			typedef std::function<void(const ImagesegClient*, const Model::SegmentVehicleRequest&, const SegmentVehicleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentVehicleAsyncHandler;

			ImagesegClient(const Credentials &credentials, const ClientConfiguration &configuration);
			ImagesegClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			ImagesegClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~ImagesegClient();
			ChangeSkyOutcome changeSky(const Model::ChangeSkyRequest &request)const;
			void changeSkyAsync(const Model::ChangeSkyRequest& request, const ChangeSkyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ChangeSkyOutcomeCallable changeSkyCallable(const Model::ChangeSkyRequest& request) const;
			GetAsyncJobResultOutcome getAsyncJobResult(const Model::GetAsyncJobResultRequest &request)const;
			void getAsyncJobResultAsync(const Model::GetAsyncJobResultRequest& request, const GetAsyncJobResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetAsyncJobResultOutcomeCallable getAsyncJobResultCallable(const Model::GetAsyncJobResultRequest& request) const;
			ParseFaceOutcome parseFace(const Model::ParseFaceRequest &request)const;
			void parseFaceAsync(const Model::ParseFaceRequest& request, const ParseFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ParseFaceOutcomeCallable parseFaceCallable(const Model::ParseFaceRequest& request) const;
			RefineMaskOutcome refineMask(const Model::RefineMaskRequest &request)const;
			void refineMaskAsync(const Model::RefineMaskRequest& request, const RefineMaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RefineMaskOutcomeCallable refineMaskCallable(const Model::RefineMaskRequest& request) const;
			SegmentAnimalOutcome segmentAnimal(const Model::SegmentAnimalRequest &request)const;
			void segmentAnimalAsync(const Model::SegmentAnimalRequest& request, const SegmentAnimalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentAnimalOutcomeCallable segmentAnimalCallable(const Model::SegmentAnimalRequest& request) const;
			SegmentBodyOutcome segmentBody(const Model::SegmentBodyRequest &request)const;
			void segmentBodyAsync(const Model::SegmentBodyRequest& request, const SegmentBodyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentBodyOutcomeCallable segmentBodyCallable(const Model::SegmentBodyRequest& request) const;
			SegmentClothOutcome segmentCloth(const Model::SegmentClothRequest &request)const;
			void segmentClothAsync(const Model::SegmentClothRequest& request, const SegmentClothAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentClothOutcomeCallable segmentClothCallable(const Model::SegmentClothRequest& request) const;
			SegmentCommodityOutcome segmentCommodity(const Model::SegmentCommodityRequest &request)const;
			void segmentCommodityAsync(const Model::SegmentCommodityRequest& request, const SegmentCommodityAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentCommodityOutcomeCallable segmentCommodityCallable(const Model::SegmentCommodityRequest& request) const;
			SegmentCommonImageOutcome segmentCommonImage(const Model::SegmentCommonImageRequest &request)const;
			void segmentCommonImageAsync(const Model::SegmentCommonImageRequest& request, const SegmentCommonImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentCommonImageOutcomeCallable segmentCommonImageCallable(const Model::SegmentCommonImageRequest& request) const;
			SegmentFaceOutcome segmentFace(const Model::SegmentFaceRequest &request)const;
			void segmentFaceAsync(const Model::SegmentFaceRequest& request, const SegmentFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentFaceOutcomeCallable segmentFaceCallable(const Model::SegmentFaceRequest& request) const;
			SegmentFoodOutcome segmentFood(const Model::SegmentFoodRequest &request)const;
			void segmentFoodAsync(const Model::SegmentFoodRequest& request, const SegmentFoodAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentFoodOutcomeCallable segmentFoodCallable(const Model::SegmentFoodRequest& request) const;
			SegmentFurnitureOutcome segmentFurniture(const Model::SegmentFurnitureRequest &request)const;
			void segmentFurnitureAsync(const Model::SegmentFurnitureRequest& request, const SegmentFurnitureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentFurnitureOutcomeCallable segmentFurnitureCallable(const Model::SegmentFurnitureRequest& request) const;
			SegmentHDBodyOutcome segmentHDBody(const Model::SegmentHDBodyRequest &request)const;
			void segmentHDBodyAsync(const Model::SegmentHDBodyRequest& request, const SegmentHDBodyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentHDBodyOutcomeCallable segmentHDBodyCallable(const Model::SegmentHDBodyRequest& request) const;
			SegmentHDCommonImageOutcome segmentHDCommonImage(const Model::SegmentHDCommonImageRequest &request)const;
			void segmentHDCommonImageAsync(const Model::SegmentHDCommonImageRequest& request, const SegmentHDCommonImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentHDCommonImageOutcomeCallable segmentHDCommonImageCallable(const Model::SegmentHDCommonImageRequest& request) const;
			SegmentHDSkyOutcome segmentHDSky(const Model::SegmentHDSkyRequest &request)const;
			void segmentHDSkyAsync(const Model::SegmentHDSkyRequest& request, const SegmentHDSkyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentHDSkyOutcomeCallable segmentHDSkyCallable(const Model::SegmentHDSkyRequest& request) const;
			SegmentHairOutcome segmentHair(const Model::SegmentHairRequest &request)const;
			void segmentHairAsync(const Model::SegmentHairRequest& request, const SegmentHairAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentHairOutcomeCallable segmentHairCallable(const Model::SegmentHairRequest& request) const;
			SegmentHeadOutcome segmentHead(const Model::SegmentHeadRequest &request)const;
			void segmentHeadAsync(const Model::SegmentHeadRequest& request, const SegmentHeadAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentHeadOutcomeCallable segmentHeadCallable(const Model::SegmentHeadRequest& request) const;
			SegmentLogoOutcome segmentLogo(const Model::SegmentLogoRequest &request)const;
			void segmentLogoAsync(const Model::SegmentLogoRequest& request, const SegmentLogoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentLogoOutcomeCallable segmentLogoCallable(const Model::SegmentLogoRequest& request) const;
			SegmentSceneOutcome segmentScene(const Model::SegmentSceneRequest &request)const;
			void segmentSceneAsync(const Model::SegmentSceneRequest& request, const SegmentSceneAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentSceneOutcomeCallable segmentSceneCallable(const Model::SegmentSceneRequest& request) const;
			SegmentSkinOutcome segmentSkin(const Model::SegmentSkinRequest &request)const;
			void segmentSkinAsync(const Model::SegmentSkinRequest& request, const SegmentSkinAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentSkinOutcomeCallable segmentSkinCallable(const Model::SegmentSkinRequest& request) const;
			SegmentSkyOutcome segmentSky(const Model::SegmentSkyRequest &request)const;
			void segmentSkyAsync(const Model::SegmentSkyRequest& request, const SegmentSkyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentSkyOutcomeCallable segmentSkyCallable(const Model::SegmentSkyRequest& request) const;
			SegmentVehicleOutcome segmentVehicle(const Model::SegmentVehicleRequest &request)const;
			void segmentVehicleAsync(const Model::SegmentVehicleRequest& request, const SegmentVehicleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentVehicleOutcomeCallable segmentVehicleCallable(const Model::SegmentVehicleRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_IMAGESEG_IMAGESEGCLIENT_H_

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

#ifndef ALIBABACLOUD_FACEBODY_FACEBODYCLIENT_H_
#define ALIBABACLOUD_FACEBODY_FACEBODYCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "FacebodyExport.h"
#include "model/AddFaceRequest.h"
#include "model/AddFaceResult.h"
#include "model/AddFaceEntityRequest.h"
#include "model/AddFaceEntityResult.h"
#include "model/BlurFaceRequest.h"
#include "model/BlurFaceResult.h"
#include "model/BodyPostureRequest.h"
#include "model/BodyPostureResult.h"
#include "model/CompareFaceRequest.h"
#include "model/CompareFaceResult.h"
#include "model/CountCrowdRequest.h"
#include "model/CountCrowdResult.h"
#include "model/CreateFaceDbRequest.h"
#include "model/CreateFaceDbResult.h"
#include "model/DeleteFaceRequest.h"
#include "model/DeleteFaceResult.h"
#include "model/DeleteFaceDbRequest.h"
#include "model/DeleteFaceDbResult.h"
#include "model/DeleteFaceEntityRequest.h"
#include "model/DeleteFaceEntityResult.h"
#include "model/DetectBodyCountRequest.h"
#include "model/DetectBodyCountResult.h"
#include "model/DetectCelebrityRequest.h"
#include "model/DetectCelebrityResult.h"
#include "model/DetectChefCapRequest.h"
#include "model/DetectChefCapResult.h"
#include "model/DetectFaceRequest.h"
#include "model/DetectFaceResult.h"
#include "model/DetectIPCPedestrianRequest.h"
#include "model/DetectIPCPedestrianResult.h"
#include "model/DetectLivingFaceRequest.h"
#include "model/DetectLivingFaceResult.h"
#include "model/DetectMaskRequest.h"
#include "model/DetectMaskResult.h"
#include "model/DetectPedestrianRequest.h"
#include "model/DetectPedestrianResult.h"
#include "model/DetectVideoLivingFaceRequest.h"
#include "model/DetectVideoLivingFaceResult.h"
#include "model/EnhanceFaceRequest.h"
#include "model/EnhanceFaceResult.h"
#include "model/ExtractPedestrianFeatureAttrRequest.h"
#include "model/ExtractPedestrianFeatureAttrResult.h"
#include "model/ExtractPedestrianFeatureAttributeRequest.h"
#include "model/ExtractPedestrianFeatureAttributeResult.h"
#include "model/FaceBeautyRequest.h"
#include "model/FaceBeautyResult.h"
#include "model/FaceFilterRequest.h"
#include "model/FaceFilterResult.h"
#include "model/FaceMakeupRequest.h"
#include "model/FaceMakeupResult.h"
#include "model/FaceTidyupRequest.h"
#include "model/FaceTidyupResult.h"
#include "model/GenerateHumanAnimeStyleRequest.h"
#include "model/GenerateHumanAnimeStyleResult.h"
#include "model/GetFaceEntityRequest.h"
#include "model/GetFaceEntityResult.h"
#include "model/HandPostureRequest.h"
#include "model/HandPostureResult.h"
#include "model/ListFaceDbsRequest.h"
#include "model/ListFaceDbsResult.h"
#include "model/ListFaceEntitiesRequest.h"
#include "model/ListFaceEntitiesResult.h"
#include "model/PedestrianDetectAttributeRequest.h"
#include "model/PedestrianDetectAttributeResult.h"
#include "model/RecognizeActionRequest.h"
#include "model/RecognizeActionResult.h"
#include "model/RecognizeExpressionRequest.h"
#include "model/RecognizeExpressionResult.h"
#include "model/RecognizeFaceRequest.h"
#include "model/RecognizeFaceResult.h"
#include "model/RecognizePublicFaceRequest.h"
#include "model/RecognizePublicFaceResult.h"
#include "model/SearchFaceRequest.h"
#include "model/SearchFaceResult.h"
#include "model/SwapFacialFeaturesRequest.h"
#include "model/SwapFacialFeaturesResult.h"
#include "model/UpdateFaceEntityRequest.h"
#include "model/UpdateFaceEntityResult.h"
#include "model/VerifyFaceMaskRequest.h"
#include "model/VerifyFaceMaskResult.h"


namespace AlibabaCloud
{
	namespace Facebody
	{
		class ALIBABACLOUD_FACEBODY_EXPORT FacebodyClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::AddFaceResult> AddFaceOutcome;
			typedef std::future<AddFaceOutcome> AddFaceOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::AddFaceRequest&, const AddFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddFaceAsyncHandler;
			typedef Outcome<Error, Model::AddFaceEntityResult> AddFaceEntityOutcome;
			typedef std::future<AddFaceEntityOutcome> AddFaceEntityOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::AddFaceEntityRequest&, const AddFaceEntityOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddFaceEntityAsyncHandler;
			typedef Outcome<Error, Model::BlurFaceResult> BlurFaceOutcome;
			typedef std::future<BlurFaceOutcome> BlurFaceOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::BlurFaceRequest&, const BlurFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BlurFaceAsyncHandler;
			typedef Outcome<Error, Model::BodyPostureResult> BodyPostureOutcome;
			typedef std::future<BodyPostureOutcome> BodyPostureOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::BodyPostureRequest&, const BodyPostureOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BodyPostureAsyncHandler;
			typedef Outcome<Error, Model::CompareFaceResult> CompareFaceOutcome;
			typedef std::future<CompareFaceOutcome> CompareFaceOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::CompareFaceRequest&, const CompareFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CompareFaceAsyncHandler;
			typedef Outcome<Error, Model::CountCrowdResult> CountCrowdOutcome;
			typedef std::future<CountCrowdOutcome> CountCrowdOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::CountCrowdRequest&, const CountCrowdOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CountCrowdAsyncHandler;
			typedef Outcome<Error, Model::CreateFaceDbResult> CreateFaceDbOutcome;
			typedef std::future<CreateFaceDbOutcome> CreateFaceDbOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::CreateFaceDbRequest&, const CreateFaceDbOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateFaceDbAsyncHandler;
			typedef Outcome<Error, Model::DeleteFaceResult> DeleteFaceOutcome;
			typedef std::future<DeleteFaceOutcome> DeleteFaceOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::DeleteFaceRequest&, const DeleteFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteFaceAsyncHandler;
			typedef Outcome<Error, Model::DeleteFaceDbResult> DeleteFaceDbOutcome;
			typedef std::future<DeleteFaceDbOutcome> DeleteFaceDbOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::DeleteFaceDbRequest&, const DeleteFaceDbOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteFaceDbAsyncHandler;
			typedef Outcome<Error, Model::DeleteFaceEntityResult> DeleteFaceEntityOutcome;
			typedef std::future<DeleteFaceEntityOutcome> DeleteFaceEntityOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::DeleteFaceEntityRequest&, const DeleteFaceEntityOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteFaceEntityAsyncHandler;
			typedef Outcome<Error, Model::DetectBodyCountResult> DetectBodyCountOutcome;
			typedef std::future<DetectBodyCountOutcome> DetectBodyCountOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::DetectBodyCountRequest&, const DetectBodyCountOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DetectBodyCountAsyncHandler;
			typedef Outcome<Error, Model::DetectCelebrityResult> DetectCelebrityOutcome;
			typedef std::future<DetectCelebrityOutcome> DetectCelebrityOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::DetectCelebrityRequest&, const DetectCelebrityOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DetectCelebrityAsyncHandler;
			typedef Outcome<Error, Model::DetectChefCapResult> DetectChefCapOutcome;
			typedef std::future<DetectChefCapOutcome> DetectChefCapOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::DetectChefCapRequest&, const DetectChefCapOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DetectChefCapAsyncHandler;
			typedef Outcome<Error, Model::DetectFaceResult> DetectFaceOutcome;
			typedef std::future<DetectFaceOutcome> DetectFaceOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::DetectFaceRequest&, const DetectFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DetectFaceAsyncHandler;
			typedef Outcome<Error, Model::DetectIPCPedestrianResult> DetectIPCPedestrianOutcome;
			typedef std::future<DetectIPCPedestrianOutcome> DetectIPCPedestrianOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::DetectIPCPedestrianRequest&, const DetectIPCPedestrianOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DetectIPCPedestrianAsyncHandler;
			typedef Outcome<Error, Model::DetectLivingFaceResult> DetectLivingFaceOutcome;
			typedef std::future<DetectLivingFaceOutcome> DetectLivingFaceOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::DetectLivingFaceRequest&, const DetectLivingFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DetectLivingFaceAsyncHandler;
			typedef Outcome<Error, Model::DetectMaskResult> DetectMaskOutcome;
			typedef std::future<DetectMaskOutcome> DetectMaskOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::DetectMaskRequest&, const DetectMaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DetectMaskAsyncHandler;
			typedef Outcome<Error, Model::DetectPedestrianResult> DetectPedestrianOutcome;
			typedef std::future<DetectPedestrianOutcome> DetectPedestrianOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::DetectPedestrianRequest&, const DetectPedestrianOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DetectPedestrianAsyncHandler;
			typedef Outcome<Error, Model::DetectVideoLivingFaceResult> DetectVideoLivingFaceOutcome;
			typedef std::future<DetectVideoLivingFaceOutcome> DetectVideoLivingFaceOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::DetectVideoLivingFaceRequest&, const DetectVideoLivingFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DetectVideoLivingFaceAsyncHandler;
			typedef Outcome<Error, Model::EnhanceFaceResult> EnhanceFaceOutcome;
			typedef std::future<EnhanceFaceOutcome> EnhanceFaceOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::EnhanceFaceRequest&, const EnhanceFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EnhanceFaceAsyncHandler;
			typedef Outcome<Error, Model::ExtractPedestrianFeatureAttrResult> ExtractPedestrianFeatureAttrOutcome;
			typedef std::future<ExtractPedestrianFeatureAttrOutcome> ExtractPedestrianFeatureAttrOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::ExtractPedestrianFeatureAttrRequest&, const ExtractPedestrianFeatureAttrOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ExtractPedestrianFeatureAttrAsyncHandler;
			typedef Outcome<Error, Model::ExtractPedestrianFeatureAttributeResult> ExtractPedestrianFeatureAttributeOutcome;
			typedef std::future<ExtractPedestrianFeatureAttributeOutcome> ExtractPedestrianFeatureAttributeOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::ExtractPedestrianFeatureAttributeRequest&, const ExtractPedestrianFeatureAttributeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ExtractPedestrianFeatureAttributeAsyncHandler;
			typedef Outcome<Error, Model::FaceBeautyResult> FaceBeautyOutcome;
			typedef std::future<FaceBeautyOutcome> FaceBeautyOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::FaceBeautyRequest&, const FaceBeautyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> FaceBeautyAsyncHandler;
			typedef Outcome<Error, Model::FaceFilterResult> FaceFilterOutcome;
			typedef std::future<FaceFilterOutcome> FaceFilterOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::FaceFilterRequest&, const FaceFilterOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> FaceFilterAsyncHandler;
			typedef Outcome<Error, Model::FaceMakeupResult> FaceMakeupOutcome;
			typedef std::future<FaceMakeupOutcome> FaceMakeupOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::FaceMakeupRequest&, const FaceMakeupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> FaceMakeupAsyncHandler;
			typedef Outcome<Error, Model::FaceTidyupResult> FaceTidyupOutcome;
			typedef std::future<FaceTidyupOutcome> FaceTidyupOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::FaceTidyupRequest&, const FaceTidyupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> FaceTidyupAsyncHandler;
			typedef Outcome<Error, Model::GenerateHumanAnimeStyleResult> GenerateHumanAnimeStyleOutcome;
			typedef std::future<GenerateHumanAnimeStyleOutcome> GenerateHumanAnimeStyleOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::GenerateHumanAnimeStyleRequest&, const GenerateHumanAnimeStyleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GenerateHumanAnimeStyleAsyncHandler;
			typedef Outcome<Error, Model::GetFaceEntityResult> GetFaceEntityOutcome;
			typedef std::future<GetFaceEntityOutcome> GetFaceEntityOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::GetFaceEntityRequest&, const GetFaceEntityOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetFaceEntityAsyncHandler;
			typedef Outcome<Error, Model::HandPostureResult> HandPostureOutcome;
			typedef std::future<HandPostureOutcome> HandPostureOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::HandPostureRequest&, const HandPostureOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> HandPostureAsyncHandler;
			typedef Outcome<Error, Model::ListFaceDbsResult> ListFaceDbsOutcome;
			typedef std::future<ListFaceDbsOutcome> ListFaceDbsOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::ListFaceDbsRequest&, const ListFaceDbsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListFaceDbsAsyncHandler;
			typedef Outcome<Error, Model::ListFaceEntitiesResult> ListFaceEntitiesOutcome;
			typedef std::future<ListFaceEntitiesOutcome> ListFaceEntitiesOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::ListFaceEntitiesRequest&, const ListFaceEntitiesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListFaceEntitiesAsyncHandler;
			typedef Outcome<Error, Model::PedestrianDetectAttributeResult> PedestrianDetectAttributeOutcome;
			typedef std::future<PedestrianDetectAttributeOutcome> PedestrianDetectAttributeOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::PedestrianDetectAttributeRequest&, const PedestrianDetectAttributeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> PedestrianDetectAttributeAsyncHandler;
			typedef Outcome<Error, Model::RecognizeActionResult> RecognizeActionOutcome;
			typedef std::future<RecognizeActionOutcome> RecognizeActionOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::RecognizeActionRequest&, const RecognizeActionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeActionAsyncHandler;
			typedef Outcome<Error, Model::RecognizeExpressionResult> RecognizeExpressionOutcome;
			typedef std::future<RecognizeExpressionOutcome> RecognizeExpressionOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::RecognizeExpressionRequest&, const RecognizeExpressionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeExpressionAsyncHandler;
			typedef Outcome<Error, Model::RecognizeFaceResult> RecognizeFaceOutcome;
			typedef std::future<RecognizeFaceOutcome> RecognizeFaceOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::RecognizeFaceRequest&, const RecognizeFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeFaceAsyncHandler;
			typedef Outcome<Error, Model::RecognizePublicFaceResult> RecognizePublicFaceOutcome;
			typedef std::future<RecognizePublicFaceOutcome> RecognizePublicFaceOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::RecognizePublicFaceRequest&, const RecognizePublicFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizePublicFaceAsyncHandler;
			typedef Outcome<Error, Model::SearchFaceResult> SearchFaceOutcome;
			typedef std::future<SearchFaceOutcome> SearchFaceOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::SearchFaceRequest&, const SearchFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SearchFaceAsyncHandler;
			typedef Outcome<Error, Model::SwapFacialFeaturesResult> SwapFacialFeaturesOutcome;
			typedef std::future<SwapFacialFeaturesOutcome> SwapFacialFeaturesOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::SwapFacialFeaturesRequest&, const SwapFacialFeaturesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SwapFacialFeaturesAsyncHandler;
			typedef Outcome<Error, Model::UpdateFaceEntityResult> UpdateFaceEntityOutcome;
			typedef std::future<UpdateFaceEntityOutcome> UpdateFaceEntityOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::UpdateFaceEntityRequest&, const UpdateFaceEntityOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateFaceEntityAsyncHandler;
			typedef Outcome<Error, Model::VerifyFaceMaskResult> VerifyFaceMaskOutcome;
			typedef std::future<VerifyFaceMaskOutcome> VerifyFaceMaskOutcomeCallable;
			typedef std::function<void(const FacebodyClient*, const Model::VerifyFaceMaskRequest&, const VerifyFaceMaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> VerifyFaceMaskAsyncHandler;

			FacebodyClient(const Credentials &credentials, const ClientConfiguration &configuration);
			FacebodyClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			FacebodyClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~FacebodyClient();
			AddFaceOutcome addFace(const Model::AddFaceRequest &request)const;
			void addFaceAsync(const Model::AddFaceRequest& request, const AddFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddFaceOutcomeCallable addFaceCallable(const Model::AddFaceRequest& request) const;
			AddFaceEntityOutcome addFaceEntity(const Model::AddFaceEntityRequest &request)const;
			void addFaceEntityAsync(const Model::AddFaceEntityRequest& request, const AddFaceEntityAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddFaceEntityOutcomeCallable addFaceEntityCallable(const Model::AddFaceEntityRequest& request) const;
			BlurFaceOutcome blurFace(const Model::BlurFaceRequest &request)const;
			void blurFaceAsync(const Model::BlurFaceRequest& request, const BlurFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BlurFaceOutcomeCallable blurFaceCallable(const Model::BlurFaceRequest& request) const;
			BodyPostureOutcome bodyPosture(const Model::BodyPostureRequest &request)const;
			void bodyPostureAsync(const Model::BodyPostureRequest& request, const BodyPostureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BodyPostureOutcomeCallable bodyPostureCallable(const Model::BodyPostureRequest& request) const;
			CompareFaceOutcome compareFace(const Model::CompareFaceRequest &request)const;
			void compareFaceAsync(const Model::CompareFaceRequest& request, const CompareFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CompareFaceOutcomeCallable compareFaceCallable(const Model::CompareFaceRequest& request) const;
			CountCrowdOutcome countCrowd(const Model::CountCrowdRequest &request)const;
			void countCrowdAsync(const Model::CountCrowdRequest& request, const CountCrowdAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CountCrowdOutcomeCallable countCrowdCallable(const Model::CountCrowdRequest& request) const;
			CreateFaceDbOutcome createFaceDb(const Model::CreateFaceDbRequest &request)const;
			void createFaceDbAsync(const Model::CreateFaceDbRequest& request, const CreateFaceDbAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateFaceDbOutcomeCallable createFaceDbCallable(const Model::CreateFaceDbRequest& request) const;
			DeleteFaceOutcome deleteFace(const Model::DeleteFaceRequest &request)const;
			void deleteFaceAsync(const Model::DeleteFaceRequest& request, const DeleteFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteFaceOutcomeCallable deleteFaceCallable(const Model::DeleteFaceRequest& request) const;
			DeleteFaceDbOutcome deleteFaceDb(const Model::DeleteFaceDbRequest &request)const;
			void deleteFaceDbAsync(const Model::DeleteFaceDbRequest& request, const DeleteFaceDbAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteFaceDbOutcomeCallable deleteFaceDbCallable(const Model::DeleteFaceDbRequest& request) const;
			DeleteFaceEntityOutcome deleteFaceEntity(const Model::DeleteFaceEntityRequest &request)const;
			void deleteFaceEntityAsync(const Model::DeleteFaceEntityRequest& request, const DeleteFaceEntityAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteFaceEntityOutcomeCallable deleteFaceEntityCallable(const Model::DeleteFaceEntityRequest& request) const;
			DetectBodyCountOutcome detectBodyCount(const Model::DetectBodyCountRequest &request)const;
			void detectBodyCountAsync(const Model::DetectBodyCountRequest& request, const DetectBodyCountAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DetectBodyCountOutcomeCallable detectBodyCountCallable(const Model::DetectBodyCountRequest& request) const;
			DetectCelebrityOutcome detectCelebrity(const Model::DetectCelebrityRequest &request)const;
			void detectCelebrityAsync(const Model::DetectCelebrityRequest& request, const DetectCelebrityAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DetectCelebrityOutcomeCallable detectCelebrityCallable(const Model::DetectCelebrityRequest& request) const;
			DetectChefCapOutcome detectChefCap(const Model::DetectChefCapRequest &request)const;
			void detectChefCapAsync(const Model::DetectChefCapRequest& request, const DetectChefCapAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DetectChefCapOutcomeCallable detectChefCapCallable(const Model::DetectChefCapRequest& request) const;
			DetectFaceOutcome detectFace(const Model::DetectFaceRequest &request)const;
			void detectFaceAsync(const Model::DetectFaceRequest& request, const DetectFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DetectFaceOutcomeCallable detectFaceCallable(const Model::DetectFaceRequest& request) const;
			DetectIPCPedestrianOutcome detectIPCPedestrian(const Model::DetectIPCPedestrianRequest &request)const;
			void detectIPCPedestrianAsync(const Model::DetectIPCPedestrianRequest& request, const DetectIPCPedestrianAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DetectIPCPedestrianOutcomeCallable detectIPCPedestrianCallable(const Model::DetectIPCPedestrianRequest& request) const;
			DetectLivingFaceOutcome detectLivingFace(const Model::DetectLivingFaceRequest &request)const;
			void detectLivingFaceAsync(const Model::DetectLivingFaceRequest& request, const DetectLivingFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DetectLivingFaceOutcomeCallable detectLivingFaceCallable(const Model::DetectLivingFaceRequest& request) const;
			DetectMaskOutcome detectMask(const Model::DetectMaskRequest &request)const;
			void detectMaskAsync(const Model::DetectMaskRequest& request, const DetectMaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DetectMaskOutcomeCallable detectMaskCallable(const Model::DetectMaskRequest& request) const;
			DetectPedestrianOutcome detectPedestrian(const Model::DetectPedestrianRequest &request)const;
			void detectPedestrianAsync(const Model::DetectPedestrianRequest& request, const DetectPedestrianAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DetectPedestrianOutcomeCallable detectPedestrianCallable(const Model::DetectPedestrianRequest& request) const;
			DetectVideoLivingFaceOutcome detectVideoLivingFace(const Model::DetectVideoLivingFaceRequest &request)const;
			void detectVideoLivingFaceAsync(const Model::DetectVideoLivingFaceRequest& request, const DetectVideoLivingFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DetectVideoLivingFaceOutcomeCallable detectVideoLivingFaceCallable(const Model::DetectVideoLivingFaceRequest& request) const;
			EnhanceFaceOutcome enhanceFace(const Model::EnhanceFaceRequest &request)const;
			void enhanceFaceAsync(const Model::EnhanceFaceRequest& request, const EnhanceFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EnhanceFaceOutcomeCallable enhanceFaceCallable(const Model::EnhanceFaceRequest& request) const;
			ExtractPedestrianFeatureAttrOutcome extractPedestrianFeatureAttr(const Model::ExtractPedestrianFeatureAttrRequest &request)const;
			void extractPedestrianFeatureAttrAsync(const Model::ExtractPedestrianFeatureAttrRequest& request, const ExtractPedestrianFeatureAttrAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ExtractPedestrianFeatureAttrOutcomeCallable extractPedestrianFeatureAttrCallable(const Model::ExtractPedestrianFeatureAttrRequest& request) const;
			ExtractPedestrianFeatureAttributeOutcome extractPedestrianFeatureAttribute(const Model::ExtractPedestrianFeatureAttributeRequest &request)const;
			void extractPedestrianFeatureAttributeAsync(const Model::ExtractPedestrianFeatureAttributeRequest& request, const ExtractPedestrianFeatureAttributeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ExtractPedestrianFeatureAttributeOutcomeCallable extractPedestrianFeatureAttributeCallable(const Model::ExtractPedestrianFeatureAttributeRequest& request) const;
			FaceBeautyOutcome faceBeauty(const Model::FaceBeautyRequest &request)const;
			void faceBeautyAsync(const Model::FaceBeautyRequest& request, const FaceBeautyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			FaceBeautyOutcomeCallable faceBeautyCallable(const Model::FaceBeautyRequest& request) const;
			FaceFilterOutcome faceFilter(const Model::FaceFilterRequest &request)const;
			void faceFilterAsync(const Model::FaceFilterRequest& request, const FaceFilterAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			FaceFilterOutcomeCallable faceFilterCallable(const Model::FaceFilterRequest& request) const;
			FaceMakeupOutcome faceMakeup(const Model::FaceMakeupRequest &request)const;
			void faceMakeupAsync(const Model::FaceMakeupRequest& request, const FaceMakeupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			FaceMakeupOutcomeCallable faceMakeupCallable(const Model::FaceMakeupRequest& request) const;
			FaceTidyupOutcome faceTidyup(const Model::FaceTidyupRequest &request)const;
			void faceTidyupAsync(const Model::FaceTidyupRequest& request, const FaceTidyupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			FaceTidyupOutcomeCallable faceTidyupCallable(const Model::FaceTidyupRequest& request) const;
			GenerateHumanAnimeStyleOutcome generateHumanAnimeStyle(const Model::GenerateHumanAnimeStyleRequest &request)const;
			void generateHumanAnimeStyleAsync(const Model::GenerateHumanAnimeStyleRequest& request, const GenerateHumanAnimeStyleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GenerateHumanAnimeStyleOutcomeCallable generateHumanAnimeStyleCallable(const Model::GenerateHumanAnimeStyleRequest& request) const;
			GetFaceEntityOutcome getFaceEntity(const Model::GetFaceEntityRequest &request)const;
			void getFaceEntityAsync(const Model::GetFaceEntityRequest& request, const GetFaceEntityAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetFaceEntityOutcomeCallable getFaceEntityCallable(const Model::GetFaceEntityRequest& request) const;
			HandPostureOutcome handPosture(const Model::HandPostureRequest &request)const;
			void handPostureAsync(const Model::HandPostureRequest& request, const HandPostureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			HandPostureOutcomeCallable handPostureCallable(const Model::HandPostureRequest& request) const;
			ListFaceDbsOutcome listFaceDbs(const Model::ListFaceDbsRequest &request)const;
			void listFaceDbsAsync(const Model::ListFaceDbsRequest& request, const ListFaceDbsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListFaceDbsOutcomeCallable listFaceDbsCallable(const Model::ListFaceDbsRequest& request) const;
			ListFaceEntitiesOutcome listFaceEntities(const Model::ListFaceEntitiesRequest &request)const;
			void listFaceEntitiesAsync(const Model::ListFaceEntitiesRequest& request, const ListFaceEntitiesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListFaceEntitiesOutcomeCallable listFaceEntitiesCallable(const Model::ListFaceEntitiesRequest& request) const;
			PedestrianDetectAttributeOutcome pedestrianDetectAttribute(const Model::PedestrianDetectAttributeRequest &request)const;
			void pedestrianDetectAttributeAsync(const Model::PedestrianDetectAttributeRequest& request, const PedestrianDetectAttributeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			PedestrianDetectAttributeOutcomeCallable pedestrianDetectAttributeCallable(const Model::PedestrianDetectAttributeRequest& request) const;
			RecognizeActionOutcome recognizeAction(const Model::RecognizeActionRequest &request)const;
			void recognizeActionAsync(const Model::RecognizeActionRequest& request, const RecognizeActionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeActionOutcomeCallable recognizeActionCallable(const Model::RecognizeActionRequest& request) const;
			RecognizeExpressionOutcome recognizeExpression(const Model::RecognizeExpressionRequest &request)const;
			void recognizeExpressionAsync(const Model::RecognizeExpressionRequest& request, const RecognizeExpressionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeExpressionOutcomeCallable recognizeExpressionCallable(const Model::RecognizeExpressionRequest& request) const;
			RecognizeFaceOutcome recognizeFace(const Model::RecognizeFaceRequest &request)const;
			void recognizeFaceAsync(const Model::RecognizeFaceRequest& request, const RecognizeFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeFaceOutcomeCallable recognizeFaceCallable(const Model::RecognizeFaceRequest& request) const;
			RecognizePublicFaceOutcome recognizePublicFace(const Model::RecognizePublicFaceRequest &request)const;
			void recognizePublicFaceAsync(const Model::RecognizePublicFaceRequest& request, const RecognizePublicFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizePublicFaceOutcomeCallable recognizePublicFaceCallable(const Model::RecognizePublicFaceRequest& request) const;
			SearchFaceOutcome searchFace(const Model::SearchFaceRequest &request)const;
			void searchFaceAsync(const Model::SearchFaceRequest& request, const SearchFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SearchFaceOutcomeCallable searchFaceCallable(const Model::SearchFaceRequest& request) const;
			SwapFacialFeaturesOutcome swapFacialFeatures(const Model::SwapFacialFeaturesRequest &request)const;
			void swapFacialFeaturesAsync(const Model::SwapFacialFeaturesRequest& request, const SwapFacialFeaturesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SwapFacialFeaturesOutcomeCallable swapFacialFeaturesCallable(const Model::SwapFacialFeaturesRequest& request) const;
			UpdateFaceEntityOutcome updateFaceEntity(const Model::UpdateFaceEntityRequest &request)const;
			void updateFaceEntityAsync(const Model::UpdateFaceEntityRequest& request, const UpdateFaceEntityAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateFaceEntityOutcomeCallable updateFaceEntityCallable(const Model::UpdateFaceEntityRequest& request) const;
			VerifyFaceMaskOutcome verifyFaceMask(const Model::VerifyFaceMaskRequest &request)const;
			void verifyFaceMaskAsync(const Model::VerifyFaceMaskRequest& request, const VerifyFaceMaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			VerifyFaceMaskOutcomeCallable verifyFaceMaskCallable(const Model::VerifyFaceMaskRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_FACEBODY_FACEBODYCLIENT_H_

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

#ifndef ALIBABACLOUD_VIDEOENHAN_VIDEOENHANCLIENT_H_
#define ALIBABACLOUD_VIDEOENHAN_VIDEOENHANCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "VideoenhanExport.h"
#include "model/AbstractEcommerceVideoRequest.h"
#include "model/AbstractEcommerceVideoResult.h"
#include "model/AbstractFilmVideoRequest.h"
#include "model/AbstractFilmVideoResult.h"
#include "model/AdjustVideoColorRequest.h"
#include "model/AdjustVideoColorResult.h"
#include "model/ChangeVideoSizeRequest.h"
#include "model/ChangeVideoSizeResult.h"
#include "model/EnhanceVideoQualityRequest.h"
#include "model/EnhanceVideoQualityResult.h"
#include "model/EraseVideoLogoRequest.h"
#include "model/EraseVideoLogoResult.h"
#include "model/EraseVideoSubtitlesRequest.h"
#include "model/EraseVideoSubtitlesResult.h"
#include "model/GenerateVideoRequest.h"
#include "model/GenerateVideoResult.h"
#include "model/GetAsyncJobResultRequest.h"
#include "model/GetAsyncJobResultResult.h"
#include "model/MergeVideoFaceRequest.h"
#include "model/MergeVideoFaceResult.h"
#include "model/SuperResolveVideoRequest.h"
#include "model/SuperResolveVideoResult.h"


namespace AlibabaCloud
{
	namespace Videoenhan
	{
		class ALIBABACLOUD_VIDEOENHAN_EXPORT VideoenhanClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::AbstractEcommerceVideoResult> AbstractEcommerceVideoOutcome;
			typedef std::future<AbstractEcommerceVideoOutcome> AbstractEcommerceVideoOutcomeCallable;
			typedef std::function<void(const VideoenhanClient*, const Model::AbstractEcommerceVideoRequest&, const AbstractEcommerceVideoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AbstractEcommerceVideoAsyncHandler;
			typedef Outcome<Error, Model::AbstractFilmVideoResult> AbstractFilmVideoOutcome;
			typedef std::future<AbstractFilmVideoOutcome> AbstractFilmVideoOutcomeCallable;
			typedef std::function<void(const VideoenhanClient*, const Model::AbstractFilmVideoRequest&, const AbstractFilmVideoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AbstractFilmVideoAsyncHandler;
			typedef Outcome<Error, Model::AdjustVideoColorResult> AdjustVideoColorOutcome;
			typedef std::future<AdjustVideoColorOutcome> AdjustVideoColorOutcomeCallable;
			typedef std::function<void(const VideoenhanClient*, const Model::AdjustVideoColorRequest&, const AdjustVideoColorOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AdjustVideoColorAsyncHandler;
			typedef Outcome<Error, Model::ChangeVideoSizeResult> ChangeVideoSizeOutcome;
			typedef std::future<ChangeVideoSizeOutcome> ChangeVideoSizeOutcomeCallable;
			typedef std::function<void(const VideoenhanClient*, const Model::ChangeVideoSizeRequest&, const ChangeVideoSizeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ChangeVideoSizeAsyncHandler;
			typedef Outcome<Error, Model::EnhanceVideoQualityResult> EnhanceVideoQualityOutcome;
			typedef std::future<EnhanceVideoQualityOutcome> EnhanceVideoQualityOutcomeCallable;
			typedef std::function<void(const VideoenhanClient*, const Model::EnhanceVideoQualityRequest&, const EnhanceVideoQualityOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EnhanceVideoQualityAsyncHandler;
			typedef Outcome<Error, Model::EraseVideoLogoResult> EraseVideoLogoOutcome;
			typedef std::future<EraseVideoLogoOutcome> EraseVideoLogoOutcomeCallable;
			typedef std::function<void(const VideoenhanClient*, const Model::EraseVideoLogoRequest&, const EraseVideoLogoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EraseVideoLogoAsyncHandler;
			typedef Outcome<Error, Model::EraseVideoSubtitlesResult> EraseVideoSubtitlesOutcome;
			typedef std::future<EraseVideoSubtitlesOutcome> EraseVideoSubtitlesOutcomeCallable;
			typedef std::function<void(const VideoenhanClient*, const Model::EraseVideoSubtitlesRequest&, const EraseVideoSubtitlesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EraseVideoSubtitlesAsyncHandler;
			typedef Outcome<Error, Model::GenerateVideoResult> GenerateVideoOutcome;
			typedef std::future<GenerateVideoOutcome> GenerateVideoOutcomeCallable;
			typedef std::function<void(const VideoenhanClient*, const Model::GenerateVideoRequest&, const GenerateVideoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GenerateVideoAsyncHandler;
			typedef Outcome<Error, Model::GetAsyncJobResultResult> GetAsyncJobResultOutcome;
			typedef std::future<GetAsyncJobResultOutcome> GetAsyncJobResultOutcomeCallable;
			typedef std::function<void(const VideoenhanClient*, const Model::GetAsyncJobResultRequest&, const GetAsyncJobResultOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetAsyncJobResultAsyncHandler;
			typedef Outcome<Error, Model::MergeVideoFaceResult> MergeVideoFaceOutcome;
			typedef std::future<MergeVideoFaceOutcome> MergeVideoFaceOutcomeCallable;
			typedef std::function<void(const VideoenhanClient*, const Model::MergeVideoFaceRequest&, const MergeVideoFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> MergeVideoFaceAsyncHandler;
			typedef Outcome<Error, Model::SuperResolveVideoResult> SuperResolveVideoOutcome;
			typedef std::future<SuperResolveVideoOutcome> SuperResolveVideoOutcomeCallable;
			typedef std::function<void(const VideoenhanClient*, const Model::SuperResolveVideoRequest&, const SuperResolveVideoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SuperResolveVideoAsyncHandler;

			VideoenhanClient(const Credentials &credentials, const ClientConfiguration &configuration);
			VideoenhanClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			VideoenhanClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~VideoenhanClient();
			AbstractEcommerceVideoOutcome abstractEcommerceVideo(const Model::AbstractEcommerceVideoRequest &request)const;
			void abstractEcommerceVideoAsync(const Model::AbstractEcommerceVideoRequest& request, const AbstractEcommerceVideoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AbstractEcommerceVideoOutcomeCallable abstractEcommerceVideoCallable(const Model::AbstractEcommerceVideoRequest& request) const;
			AbstractFilmVideoOutcome abstractFilmVideo(const Model::AbstractFilmVideoRequest &request)const;
			void abstractFilmVideoAsync(const Model::AbstractFilmVideoRequest& request, const AbstractFilmVideoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AbstractFilmVideoOutcomeCallable abstractFilmVideoCallable(const Model::AbstractFilmVideoRequest& request) const;
			AdjustVideoColorOutcome adjustVideoColor(const Model::AdjustVideoColorRequest &request)const;
			void adjustVideoColorAsync(const Model::AdjustVideoColorRequest& request, const AdjustVideoColorAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AdjustVideoColorOutcomeCallable adjustVideoColorCallable(const Model::AdjustVideoColorRequest& request) const;
			ChangeVideoSizeOutcome changeVideoSize(const Model::ChangeVideoSizeRequest &request)const;
			void changeVideoSizeAsync(const Model::ChangeVideoSizeRequest& request, const ChangeVideoSizeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ChangeVideoSizeOutcomeCallable changeVideoSizeCallable(const Model::ChangeVideoSizeRequest& request) const;
			EnhanceVideoQualityOutcome enhanceVideoQuality(const Model::EnhanceVideoQualityRequest &request)const;
			void enhanceVideoQualityAsync(const Model::EnhanceVideoQualityRequest& request, const EnhanceVideoQualityAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EnhanceVideoQualityOutcomeCallable enhanceVideoQualityCallable(const Model::EnhanceVideoQualityRequest& request) const;
			EraseVideoLogoOutcome eraseVideoLogo(const Model::EraseVideoLogoRequest &request)const;
			void eraseVideoLogoAsync(const Model::EraseVideoLogoRequest& request, const EraseVideoLogoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EraseVideoLogoOutcomeCallable eraseVideoLogoCallable(const Model::EraseVideoLogoRequest& request) const;
			EraseVideoSubtitlesOutcome eraseVideoSubtitles(const Model::EraseVideoSubtitlesRequest &request)const;
			void eraseVideoSubtitlesAsync(const Model::EraseVideoSubtitlesRequest& request, const EraseVideoSubtitlesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EraseVideoSubtitlesOutcomeCallable eraseVideoSubtitlesCallable(const Model::EraseVideoSubtitlesRequest& request) const;
			GenerateVideoOutcome generateVideo(const Model::GenerateVideoRequest &request)const;
			void generateVideoAsync(const Model::GenerateVideoRequest& request, const GenerateVideoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GenerateVideoOutcomeCallable generateVideoCallable(const Model::GenerateVideoRequest& request) const;
			GetAsyncJobResultOutcome getAsyncJobResult(const Model::GetAsyncJobResultRequest &request)const;
			void getAsyncJobResultAsync(const Model::GetAsyncJobResultRequest& request, const GetAsyncJobResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetAsyncJobResultOutcomeCallable getAsyncJobResultCallable(const Model::GetAsyncJobResultRequest& request) const;
			MergeVideoFaceOutcome mergeVideoFace(const Model::MergeVideoFaceRequest &request)const;
			void mergeVideoFaceAsync(const Model::MergeVideoFaceRequest& request, const MergeVideoFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			MergeVideoFaceOutcomeCallable mergeVideoFaceCallable(const Model::MergeVideoFaceRequest& request) const;
			SuperResolveVideoOutcome superResolveVideo(const Model::SuperResolveVideoRequest &request)const;
			void superResolveVideoAsync(const Model::SuperResolveVideoRequest& request, const SuperResolveVideoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SuperResolveVideoOutcomeCallable superResolveVideoCallable(const Model::SuperResolveVideoRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_VIDEOENHAN_VIDEOENHANCLIENT_H_

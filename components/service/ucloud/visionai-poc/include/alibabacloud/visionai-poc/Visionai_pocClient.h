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

#ifndef ALIBABACLOUD_VISIONAI_POC_VISIONAI_POCCLIENT_H_
#define ALIBABACLOUD_VISIONAI_POC_VISIONAI_POCCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "Visionai_pocExport.h"
#include "model/CheckMultiagentRequest.h"
#include "model/CheckMultiagentResult.h"
#include "model/PredictCategoryRequest.h"
#include "model/PredictCategoryResult.h"
#include "model/RecognizeCarRequest.h"
#include "model/RecognizeCarResult.h"
#include "model/RecognizeEntityRequest.h"
#include "model/RecognizeEntityResult.h"
#include "model/RecognizeFlowerRequest.h"
#include "model/RecognizeFlowerResult.h"
#include "model/RecognizeLabelRequest.h"
#include "model/RecognizeLabelResult.h"
#include "model/RecognizePetRequest.h"
#include "model/RecognizePetResult.h"


namespace AlibabaCloud
{
	namespace Visionai_poc
	{
		class ALIBABACLOUD_VISIONAI_POC_EXPORT Visionai_pocClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::CheckMultiagentResult> CheckMultiagentOutcome;
			typedef std::future<CheckMultiagentOutcome> CheckMultiagentOutcomeCallable;
			typedef std::function<void(const Visionai_pocClient*, const Model::CheckMultiagentRequest&, const CheckMultiagentOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CheckMultiagentAsyncHandler;
			typedef Outcome<Error, Model::PredictCategoryResult> PredictCategoryOutcome;
			typedef std::future<PredictCategoryOutcome> PredictCategoryOutcomeCallable;
			typedef std::function<void(const Visionai_pocClient*, const Model::PredictCategoryRequest&, const PredictCategoryOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> PredictCategoryAsyncHandler;
			typedef Outcome<Error, Model::RecognizeCarResult> RecognizeCarOutcome;
			typedef std::future<RecognizeCarOutcome> RecognizeCarOutcomeCallable;
			typedef std::function<void(const Visionai_pocClient*, const Model::RecognizeCarRequest&, const RecognizeCarOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeCarAsyncHandler;
			typedef Outcome<Error, Model::RecognizeEntityResult> RecognizeEntityOutcome;
			typedef std::future<RecognizeEntityOutcome> RecognizeEntityOutcomeCallable;
			typedef std::function<void(const Visionai_pocClient*, const Model::RecognizeEntityRequest&, const RecognizeEntityOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeEntityAsyncHandler;
			typedef Outcome<Error, Model::RecognizeFlowerResult> RecognizeFlowerOutcome;
			typedef std::future<RecognizeFlowerOutcome> RecognizeFlowerOutcomeCallable;
			typedef std::function<void(const Visionai_pocClient*, const Model::RecognizeFlowerRequest&, const RecognizeFlowerOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeFlowerAsyncHandler;
			typedef Outcome<Error, Model::RecognizeLabelResult> RecognizeLabelOutcome;
			typedef std::future<RecognizeLabelOutcome> RecognizeLabelOutcomeCallable;
			typedef std::function<void(const Visionai_pocClient*, const Model::RecognizeLabelRequest&, const RecognizeLabelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeLabelAsyncHandler;
			typedef Outcome<Error, Model::RecognizePetResult> RecognizePetOutcome;
			typedef std::future<RecognizePetOutcome> RecognizePetOutcomeCallable;
			typedef std::function<void(const Visionai_pocClient*, const Model::RecognizePetRequest&, const RecognizePetOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizePetAsyncHandler;

			Visionai_pocClient(const Credentials &credentials, const ClientConfiguration &configuration);
			Visionai_pocClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			Visionai_pocClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~Visionai_pocClient();
			CheckMultiagentOutcome checkMultiagent(const Model::CheckMultiagentRequest &request)const;
			void checkMultiagentAsync(const Model::CheckMultiagentRequest& request, const CheckMultiagentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CheckMultiagentOutcomeCallable checkMultiagentCallable(const Model::CheckMultiagentRequest& request) const;
			PredictCategoryOutcome predictCategory(const Model::PredictCategoryRequest &request)const;
			void predictCategoryAsync(const Model::PredictCategoryRequest& request, const PredictCategoryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			PredictCategoryOutcomeCallable predictCategoryCallable(const Model::PredictCategoryRequest& request) const;
			RecognizeCarOutcome recognizeCar(const Model::RecognizeCarRequest &request)const;
			void recognizeCarAsync(const Model::RecognizeCarRequest& request, const RecognizeCarAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeCarOutcomeCallable recognizeCarCallable(const Model::RecognizeCarRequest& request) const;
			RecognizeEntityOutcome recognizeEntity(const Model::RecognizeEntityRequest &request)const;
			void recognizeEntityAsync(const Model::RecognizeEntityRequest& request, const RecognizeEntityAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeEntityOutcomeCallable recognizeEntityCallable(const Model::RecognizeEntityRequest& request) const;
			RecognizeFlowerOutcome recognizeFlower(const Model::RecognizeFlowerRequest &request)const;
			void recognizeFlowerAsync(const Model::RecognizeFlowerRequest& request, const RecognizeFlowerAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeFlowerOutcomeCallable recognizeFlowerCallable(const Model::RecognizeFlowerRequest& request) const;
			RecognizeLabelOutcome recognizeLabel(const Model::RecognizeLabelRequest &request)const;
			void recognizeLabelAsync(const Model::RecognizeLabelRequest& request, const RecognizeLabelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeLabelOutcomeCallable recognizeLabelCallable(const Model::RecognizeLabelRequest& request) const;
			RecognizePetOutcome recognizePet(const Model::RecognizePetRequest &request)const;
			void recognizePetAsync(const Model::RecognizePetRequest& request, const RecognizePetAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizePetOutcomeCallable recognizePetCallable(const Model::RecognizePetRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_VISIONAI_POC_VISIONAI_POCCLIENT_H_

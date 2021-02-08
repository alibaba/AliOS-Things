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

#ifndef ALIBABACLOUD_NLP_AUTOML_NLP_AUTOMLCLIENT_H_
#define ALIBABACLOUD_NLP_AUTOML_NLP_AUTOMLCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "Nlp_automlExport.h"
#include "model/CreateAsyncPredictRequest.h"
#include "model/CreateAsyncPredictResult.h"
#include "model/GetAsyncPredictRequest.h"
#include "model/GetAsyncPredictResult.h"
#include "model/GetPredictResultRequest.h"
#include "model/GetPredictResultResult.h"
#include "model/RunContactReviewRequest.h"
#include "model/RunContactReviewResult.h"
#include "model/RunPreTrainServiceRequest.h"
#include "model/RunPreTrainServiceResult.h"
#include "model/RunSmartCallServiceRequest.h"
#include "model/RunSmartCallServiceResult.h"


namespace AlibabaCloud
{
	namespace Nlp_automl
	{
		class ALIBABACLOUD_NLP_AUTOML_EXPORT Nlp_automlClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::CreateAsyncPredictResult> CreateAsyncPredictOutcome;
			typedef std::future<CreateAsyncPredictOutcome> CreateAsyncPredictOutcomeCallable;
			typedef std::function<void(const Nlp_automlClient*, const Model::CreateAsyncPredictRequest&, const CreateAsyncPredictOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateAsyncPredictAsyncHandler;
			typedef Outcome<Error, Model::GetAsyncPredictResult> GetAsyncPredictOutcome;
			typedef std::future<GetAsyncPredictOutcome> GetAsyncPredictOutcomeCallable;
			typedef std::function<void(const Nlp_automlClient*, const Model::GetAsyncPredictRequest&, const GetAsyncPredictOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetAsyncPredictAsyncHandler;
			typedef Outcome<Error, Model::GetPredictResultResult> GetPredictResultOutcome;
			typedef std::future<GetPredictResultOutcome> GetPredictResultOutcomeCallable;
			typedef std::function<void(const Nlp_automlClient*, const Model::GetPredictResultRequest&, const GetPredictResultOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetPredictResultAsyncHandler;
			typedef Outcome<Error, Model::RunContactReviewResult> RunContactReviewOutcome;
			typedef std::future<RunContactReviewOutcome> RunContactReviewOutcomeCallable;
			typedef std::function<void(const Nlp_automlClient*, const Model::RunContactReviewRequest&, const RunContactReviewOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RunContactReviewAsyncHandler;
			typedef Outcome<Error, Model::RunPreTrainServiceResult> RunPreTrainServiceOutcome;
			typedef std::future<RunPreTrainServiceOutcome> RunPreTrainServiceOutcomeCallable;
			typedef std::function<void(const Nlp_automlClient*, const Model::RunPreTrainServiceRequest&, const RunPreTrainServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RunPreTrainServiceAsyncHandler;
			typedef Outcome<Error, Model::RunSmartCallServiceResult> RunSmartCallServiceOutcome;
			typedef std::future<RunSmartCallServiceOutcome> RunSmartCallServiceOutcomeCallable;
			typedef std::function<void(const Nlp_automlClient*, const Model::RunSmartCallServiceRequest&, const RunSmartCallServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RunSmartCallServiceAsyncHandler;

			Nlp_automlClient(const Credentials &credentials, const ClientConfiguration &configuration);
			Nlp_automlClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			Nlp_automlClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~Nlp_automlClient();
			CreateAsyncPredictOutcome createAsyncPredict(const Model::CreateAsyncPredictRequest &request)const;
			void createAsyncPredictAsync(const Model::CreateAsyncPredictRequest& request, const CreateAsyncPredictAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateAsyncPredictOutcomeCallable createAsyncPredictCallable(const Model::CreateAsyncPredictRequest& request) const;
			GetAsyncPredictOutcome getAsyncPredict(const Model::GetAsyncPredictRequest &request)const;
			void getAsyncPredictAsync(const Model::GetAsyncPredictRequest& request, const GetAsyncPredictAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetAsyncPredictOutcomeCallable getAsyncPredictCallable(const Model::GetAsyncPredictRequest& request) const;
			GetPredictResultOutcome getPredictResult(const Model::GetPredictResultRequest &request)const;
			void getPredictResultAsync(const Model::GetPredictResultRequest& request, const GetPredictResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetPredictResultOutcomeCallable getPredictResultCallable(const Model::GetPredictResultRequest& request) const;
			RunContactReviewOutcome runContactReview(const Model::RunContactReviewRequest &request)const;
			void runContactReviewAsync(const Model::RunContactReviewRequest& request, const RunContactReviewAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RunContactReviewOutcomeCallable runContactReviewCallable(const Model::RunContactReviewRequest& request) const;
			RunPreTrainServiceOutcome runPreTrainService(const Model::RunPreTrainServiceRequest &request)const;
			void runPreTrainServiceAsync(const Model::RunPreTrainServiceRequest& request, const RunPreTrainServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RunPreTrainServiceOutcomeCallable runPreTrainServiceCallable(const Model::RunPreTrainServiceRequest& request) const;
			RunSmartCallServiceOutcome runSmartCallService(const Model::RunSmartCallServiceRequest &request)const;
			void runSmartCallServiceAsync(const Model::RunSmartCallServiceRequest& request, const RunSmartCallServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RunSmartCallServiceOutcomeCallable runSmartCallServiceCallable(const Model::RunSmartCallServiceRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_NLP_AUTOML_NLP_AUTOMLCLIENT_H_

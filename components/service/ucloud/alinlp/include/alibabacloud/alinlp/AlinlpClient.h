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

#ifndef ALIBABACLOUD_ALINLP_ALINLPCLIENT_H_
#define ALIBABACLOUD_ALINLP_ALINLPCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "AlinlpExport.h"
#include "model/GetDpChEcomRequest.h"
#include "model/GetDpChEcomResult.h"
#include "model/GetEcChGeneralRequest.h"
#include "model/GetEcChGeneralResult.h"
#include "model/GetNerChMedicalRequest.h"
#include "model/GetNerChMedicalResult.h"
#include "model/GetNerCustomizedChEcomRequest.h"
#include "model/GetNerCustomizedChEcomResult.h"
#include "model/GetPosChEcomRequest.h"
#include "model/GetPosChEcomResult.h"
#include "model/GetSummaryChEcomRequest.h"
#include "model/GetSummaryChEcomResult.h"
#include "model/GetTsChEcomRequest.h"
#include "model/GetTsChEcomResult.h"
#include "model/GetWeChEcomRequest.h"
#include "model/GetWeChEcomResult.h"
#include "model/GetWsCustomizedChGeneralRequest.h"
#include "model/GetWsCustomizedChGeneralResult.h"
#include "model/GetWsCustomizedSeaGeneralRequest.h"
#include "model/GetWsCustomizedSeaGeneralResult.h"


namespace AlibabaCloud
{
	namespace Alinlp
	{
		class ALIBABACLOUD_ALINLP_EXPORT AlinlpClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::GetDpChEcomResult> GetDpChEcomOutcome;
			typedef std::future<GetDpChEcomOutcome> GetDpChEcomOutcomeCallable;
			typedef std::function<void(const AlinlpClient*, const Model::GetDpChEcomRequest&, const GetDpChEcomOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDpChEcomAsyncHandler;
			typedef Outcome<Error, Model::GetEcChGeneralResult> GetEcChGeneralOutcome;
			typedef std::future<GetEcChGeneralOutcome> GetEcChGeneralOutcomeCallable;
			typedef std::function<void(const AlinlpClient*, const Model::GetEcChGeneralRequest&, const GetEcChGeneralOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetEcChGeneralAsyncHandler;
			typedef Outcome<Error, Model::GetNerChMedicalResult> GetNerChMedicalOutcome;
			typedef std::future<GetNerChMedicalOutcome> GetNerChMedicalOutcomeCallable;
			typedef std::function<void(const AlinlpClient*, const Model::GetNerChMedicalRequest&, const GetNerChMedicalOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetNerChMedicalAsyncHandler;
			typedef Outcome<Error, Model::GetNerCustomizedChEcomResult> GetNerCustomizedChEcomOutcome;
			typedef std::future<GetNerCustomizedChEcomOutcome> GetNerCustomizedChEcomOutcomeCallable;
			typedef std::function<void(const AlinlpClient*, const Model::GetNerCustomizedChEcomRequest&, const GetNerCustomizedChEcomOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetNerCustomizedChEcomAsyncHandler;
			typedef Outcome<Error, Model::GetPosChEcomResult> GetPosChEcomOutcome;
			typedef std::future<GetPosChEcomOutcome> GetPosChEcomOutcomeCallable;
			typedef std::function<void(const AlinlpClient*, const Model::GetPosChEcomRequest&, const GetPosChEcomOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetPosChEcomAsyncHandler;
			typedef Outcome<Error, Model::GetSummaryChEcomResult> GetSummaryChEcomOutcome;
			typedef std::future<GetSummaryChEcomOutcome> GetSummaryChEcomOutcomeCallable;
			typedef std::function<void(const AlinlpClient*, const Model::GetSummaryChEcomRequest&, const GetSummaryChEcomOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetSummaryChEcomAsyncHandler;
			typedef Outcome<Error, Model::GetTsChEcomResult> GetTsChEcomOutcome;
			typedef std::future<GetTsChEcomOutcome> GetTsChEcomOutcomeCallable;
			typedef std::function<void(const AlinlpClient*, const Model::GetTsChEcomRequest&, const GetTsChEcomOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetTsChEcomAsyncHandler;
			typedef Outcome<Error, Model::GetWeChEcomResult> GetWeChEcomOutcome;
			typedef std::future<GetWeChEcomOutcome> GetWeChEcomOutcomeCallable;
			typedef std::function<void(const AlinlpClient*, const Model::GetWeChEcomRequest&, const GetWeChEcomOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetWeChEcomAsyncHandler;
			typedef Outcome<Error, Model::GetWsCustomizedChGeneralResult> GetWsCustomizedChGeneralOutcome;
			typedef std::future<GetWsCustomizedChGeneralOutcome> GetWsCustomizedChGeneralOutcomeCallable;
			typedef std::function<void(const AlinlpClient*, const Model::GetWsCustomizedChGeneralRequest&, const GetWsCustomizedChGeneralOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetWsCustomizedChGeneralAsyncHandler;
			typedef Outcome<Error, Model::GetWsCustomizedSeaGeneralResult> GetWsCustomizedSeaGeneralOutcome;
			typedef std::future<GetWsCustomizedSeaGeneralOutcome> GetWsCustomizedSeaGeneralOutcomeCallable;
			typedef std::function<void(const AlinlpClient*, const Model::GetWsCustomizedSeaGeneralRequest&, const GetWsCustomizedSeaGeneralOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetWsCustomizedSeaGeneralAsyncHandler;

			AlinlpClient(const Credentials &credentials, const ClientConfiguration &configuration);
			AlinlpClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			AlinlpClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~AlinlpClient();
			GetDpChEcomOutcome getDpChEcom(const Model::GetDpChEcomRequest &request)const;
			void getDpChEcomAsync(const Model::GetDpChEcomRequest& request, const GetDpChEcomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDpChEcomOutcomeCallable getDpChEcomCallable(const Model::GetDpChEcomRequest& request) const;
			GetEcChGeneralOutcome getEcChGeneral(const Model::GetEcChGeneralRequest &request)const;
			void getEcChGeneralAsync(const Model::GetEcChGeneralRequest& request, const GetEcChGeneralAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetEcChGeneralOutcomeCallable getEcChGeneralCallable(const Model::GetEcChGeneralRequest& request) const;
			GetNerChMedicalOutcome getNerChMedical(const Model::GetNerChMedicalRequest &request)const;
			void getNerChMedicalAsync(const Model::GetNerChMedicalRequest& request, const GetNerChMedicalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetNerChMedicalOutcomeCallable getNerChMedicalCallable(const Model::GetNerChMedicalRequest& request) const;
			GetNerCustomizedChEcomOutcome getNerCustomizedChEcom(const Model::GetNerCustomizedChEcomRequest &request)const;
			void getNerCustomizedChEcomAsync(const Model::GetNerCustomizedChEcomRequest& request, const GetNerCustomizedChEcomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetNerCustomizedChEcomOutcomeCallable getNerCustomizedChEcomCallable(const Model::GetNerCustomizedChEcomRequest& request) const;
			GetPosChEcomOutcome getPosChEcom(const Model::GetPosChEcomRequest &request)const;
			void getPosChEcomAsync(const Model::GetPosChEcomRequest& request, const GetPosChEcomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetPosChEcomOutcomeCallable getPosChEcomCallable(const Model::GetPosChEcomRequest& request) const;
			GetSummaryChEcomOutcome getSummaryChEcom(const Model::GetSummaryChEcomRequest &request)const;
			void getSummaryChEcomAsync(const Model::GetSummaryChEcomRequest& request, const GetSummaryChEcomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetSummaryChEcomOutcomeCallable getSummaryChEcomCallable(const Model::GetSummaryChEcomRequest& request) const;
			GetTsChEcomOutcome getTsChEcom(const Model::GetTsChEcomRequest &request)const;
			void getTsChEcomAsync(const Model::GetTsChEcomRequest& request, const GetTsChEcomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetTsChEcomOutcomeCallable getTsChEcomCallable(const Model::GetTsChEcomRequest& request) const;
			GetWeChEcomOutcome getWeChEcom(const Model::GetWeChEcomRequest &request)const;
			void getWeChEcomAsync(const Model::GetWeChEcomRequest& request, const GetWeChEcomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetWeChEcomOutcomeCallable getWeChEcomCallable(const Model::GetWeChEcomRequest& request) const;
			GetWsCustomizedChGeneralOutcome getWsCustomizedChGeneral(const Model::GetWsCustomizedChGeneralRequest &request)const;
			void getWsCustomizedChGeneralAsync(const Model::GetWsCustomizedChGeneralRequest& request, const GetWsCustomizedChGeneralAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetWsCustomizedChGeneralOutcomeCallable getWsCustomizedChGeneralCallable(const Model::GetWsCustomizedChGeneralRequest& request) const;
			GetWsCustomizedSeaGeneralOutcome getWsCustomizedSeaGeneral(const Model::GetWsCustomizedSeaGeneralRequest &request)const;
			void getWsCustomizedSeaGeneralAsync(const Model::GetWsCustomizedSeaGeneralRequest& request, const GetWsCustomizedSeaGeneralAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetWsCustomizedSeaGeneralOutcomeCallable getWsCustomizedSeaGeneralCallable(const Model::GetWsCustomizedSeaGeneralRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_ALINLP_ALINLPCLIENT_H_

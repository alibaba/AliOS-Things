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

#ifndef ALIBABACLOUD_ALIMT_ALIMTCLIENT_H_
#define ALIBABACLOUD_ALIMT_ALIMTCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "AlimtExport.h"
#include "model/CreateDocTranslateTaskRequest.h"
#include "model/CreateDocTranslateTaskResult.h"
#include "model/CreateImageTranslateTaskRequest.h"
#include "model/CreateImageTranslateTaskResult.h"
#include "model/GetDetectLanguageRequest.h"
#include "model/GetDetectLanguageResult.h"
#include "model/GetDocTranslateTaskRequest.h"
#include "model/GetDocTranslateTaskResult.h"
#include "model/GetImageDiagnoseRequest.h"
#include "model/GetImageDiagnoseResult.h"
#include "model/GetImageTranslateRequest.h"
#include "model/GetImageTranslateResult.h"
#include "model/GetImageTranslateTaskRequest.h"
#include "model/GetImageTranslateTaskResult.h"
#include "model/GetTitleDiagnoseRequest.h"
#include "model/GetTitleDiagnoseResult.h"
#include "model/GetTitleGenerateRequest.h"
#include "model/GetTitleGenerateResult.h"
#include "model/GetTitleIntelligenceRequest.h"
#include "model/GetTitleIntelligenceResult.h"
#include "model/OpenAlimtServiceRequest.h"
#include "model/OpenAlimtServiceResult.h"
#include "model/TranslateRequest.h"
#include "model/TranslateResult.h"
#include "model/TranslateCertificateRequest.h"
#include "model/TranslateCertificateResult.h"
#include "model/TranslateECommerceRequest.h"
#include "model/TranslateECommerceResult.h"
#include "model/TranslateGeneralRequest.h"
#include "model/TranslateGeneralResult.h"


namespace AlibabaCloud
{
	namespace Alimt
	{
		class ALIBABACLOUD_ALIMT_EXPORT AlimtClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::CreateDocTranslateTaskResult> CreateDocTranslateTaskOutcome;
			typedef std::future<CreateDocTranslateTaskOutcome> CreateDocTranslateTaskOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::CreateDocTranslateTaskRequest&, const CreateDocTranslateTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateDocTranslateTaskAsyncHandler;
			typedef Outcome<Error, Model::CreateImageTranslateTaskResult> CreateImageTranslateTaskOutcome;
			typedef std::future<CreateImageTranslateTaskOutcome> CreateImageTranslateTaskOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::CreateImageTranslateTaskRequest&, const CreateImageTranslateTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateImageTranslateTaskAsyncHandler;
			typedef Outcome<Error, Model::GetDetectLanguageResult> GetDetectLanguageOutcome;
			typedef std::future<GetDetectLanguageOutcome> GetDetectLanguageOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::GetDetectLanguageRequest&, const GetDetectLanguageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDetectLanguageAsyncHandler;
			typedef Outcome<Error, Model::GetDocTranslateTaskResult> GetDocTranslateTaskOutcome;
			typedef std::future<GetDocTranslateTaskOutcome> GetDocTranslateTaskOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::GetDocTranslateTaskRequest&, const GetDocTranslateTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDocTranslateTaskAsyncHandler;
			typedef Outcome<Error, Model::GetImageDiagnoseResult> GetImageDiagnoseOutcome;
			typedef std::future<GetImageDiagnoseOutcome> GetImageDiagnoseOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::GetImageDiagnoseRequest&, const GetImageDiagnoseOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetImageDiagnoseAsyncHandler;
			typedef Outcome<Error, Model::GetImageTranslateResult> GetImageTranslateOutcome;
			typedef std::future<GetImageTranslateOutcome> GetImageTranslateOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::GetImageTranslateRequest&, const GetImageTranslateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetImageTranslateAsyncHandler;
			typedef Outcome<Error, Model::GetImageTranslateTaskResult> GetImageTranslateTaskOutcome;
			typedef std::future<GetImageTranslateTaskOutcome> GetImageTranslateTaskOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::GetImageTranslateTaskRequest&, const GetImageTranslateTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetImageTranslateTaskAsyncHandler;
			typedef Outcome<Error, Model::GetTitleDiagnoseResult> GetTitleDiagnoseOutcome;
			typedef std::future<GetTitleDiagnoseOutcome> GetTitleDiagnoseOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::GetTitleDiagnoseRequest&, const GetTitleDiagnoseOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetTitleDiagnoseAsyncHandler;
			typedef Outcome<Error, Model::GetTitleGenerateResult> GetTitleGenerateOutcome;
			typedef std::future<GetTitleGenerateOutcome> GetTitleGenerateOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::GetTitleGenerateRequest&, const GetTitleGenerateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetTitleGenerateAsyncHandler;
			typedef Outcome<Error, Model::GetTitleIntelligenceResult> GetTitleIntelligenceOutcome;
			typedef std::future<GetTitleIntelligenceOutcome> GetTitleIntelligenceOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::GetTitleIntelligenceRequest&, const GetTitleIntelligenceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetTitleIntelligenceAsyncHandler;
			typedef Outcome<Error, Model::OpenAlimtServiceResult> OpenAlimtServiceOutcome;
			typedef std::future<OpenAlimtServiceOutcome> OpenAlimtServiceOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::OpenAlimtServiceRequest&, const OpenAlimtServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> OpenAlimtServiceAsyncHandler;
			typedef Outcome<Error, Model::TranslateResult> TranslateOutcome;
			typedef std::future<TranslateOutcome> TranslateOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::TranslateRequest&, const TranslateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> TranslateAsyncHandler;
			typedef Outcome<Error, Model::TranslateCertificateResult> TranslateCertificateOutcome;
			typedef std::future<TranslateCertificateOutcome> TranslateCertificateOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::TranslateCertificateRequest&, const TranslateCertificateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> TranslateCertificateAsyncHandler;
			typedef Outcome<Error, Model::TranslateECommerceResult> TranslateECommerceOutcome;
			typedef std::future<TranslateECommerceOutcome> TranslateECommerceOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::TranslateECommerceRequest&, const TranslateECommerceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> TranslateECommerceAsyncHandler;
			typedef Outcome<Error, Model::TranslateGeneralResult> TranslateGeneralOutcome;
			typedef std::future<TranslateGeneralOutcome> TranslateGeneralOutcomeCallable;
			typedef std::function<void(const AlimtClient*, const Model::TranslateGeneralRequest&, const TranslateGeneralOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> TranslateGeneralAsyncHandler;

			AlimtClient(const Credentials &credentials, const ClientConfiguration &configuration);
			AlimtClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			AlimtClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~AlimtClient();
			CreateDocTranslateTaskOutcome createDocTranslateTask(const Model::CreateDocTranslateTaskRequest &request)const;
			void createDocTranslateTaskAsync(const Model::CreateDocTranslateTaskRequest& request, const CreateDocTranslateTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateDocTranslateTaskOutcomeCallable createDocTranslateTaskCallable(const Model::CreateDocTranslateTaskRequest& request) const;
			CreateImageTranslateTaskOutcome createImageTranslateTask(const Model::CreateImageTranslateTaskRequest &request)const;
			void createImageTranslateTaskAsync(const Model::CreateImageTranslateTaskRequest& request, const CreateImageTranslateTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateImageTranslateTaskOutcomeCallable createImageTranslateTaskCallable(const Model::CreateImageTranslateTaskRequest& request) const;
			GetDetectLanguageOutcome getDetectLanguage(const Model::GetDetectLanguageRequest &request)const;
			void getDetectLanguageAsync(const Model::GetDetectLanguageRequest& request, const GetDetectLanguageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDetectLanguageOutcomeCallable getDetectLanguageCallable(const Model::GetDetectLanguageRequest& request) const;
			GetDocTranslateTaskOutcome getDocTranslateTask(const Model::GetDocTranslateTaskRequest &request)const;
			void getDocTranslateTaskAsync(const Model::GetDocTranslateTaskRequest& request, const GetDocTranslateTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDocTranslateTaskOutcomeCallable getDocTranslateTaskCallable(const Model::GetDocTranslateTaskRequest& request) const;
			GetImageDiagnoseOutcome getImageDiagnose(const Model::GetImageDiagnoseRequest &request)const;
			void getImageDiagnoseAsync(const Model::GetImageDiagnoseRequest& request, const GetImageDiagnoseAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetImageDiagnoseOutcomeCallable getImageDiagnoseCallable(const Model::GetImageDiagnoseRequest& request) const;
			GetImageTranslateOutcome getImageTranslate(const Model::GetImageTranslateRequest &request)const;
			void getImageTranslateAsync(const Model::GetImageTranslateRequest& request, const GetImageTranslateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetImageTranslateOutcomeCallable getImageTranslateCallable(const Model::GetImageTranslateRequest& request) const;
			GetImageTranslateTaskOutcome getImageTranslateTask(const Model::GetImageTranslateTaskRequest &request)const;
			void getImageTranslateTaskAsync(const Model::GetImageTranslateTaskRequest& request, const GetImageTranslateTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetImageTranslateTaskOutcomeCallable getImageTranslateTaskCallable(const Model::GetImageTranslateTaskRequest& request) const;
			GetTitleDiagnoseOutcome getTitleDiagnose(const Model::GetTitleDiagnoseRequest &request)const;
			void getTitleDiagnoseAsync(const Model::GetTitleDiagnoseRequest& request, const GetTitleDiagnoseAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetTitleDiagnoseOutcomeCallable getTitleDiagnoseCallable(const Model::GetTitleDiagnoseRequest& request) const;
			GetTitleGenerateOutcome getTitleGenerate(const Model::GetTitleGenerateRequest &request)const;
			void getTitleGenerateAsync(const Model::GetTitleGenerateRequest& request, const GetTitleGenerateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetTitleGenerateOutcomeCallable getTitleGenerateCallable(const Model::GetTitleGenerateRequest& request) const;
			GetTitleIntelligenceOutcome getTitleIntelligence(const Model::GetTitleIntelligenceRequest &request)const;
			void getTitleIntelligenceAsync(const Model::GetTitleIntelligenceRequest& request, const GetTitleIntelligenceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetTitleIntelligenceOutcomeCallable getTitleIntelligenceCallable(const Model::GetTitleIntelligenceRequest& request) const;
			OpenAlimtServiceOutcome openAlimtService(const Model::OpenAlimtServiceRequest &request)const;
			void openAlimtServiceAsync(const Model::OpenAlimtServiceRequest& request, const OpenAlimtServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			OpenAlimtServiceOutcomeCallable openAlimtServiceCallable(const Model::OpenAlimtServiceRequest& request) const;
			TranslateOutcome translate(const Model::TranslateRequest &request)const;
			void translateAsync(const Model::TranslateRequest& request, const TranslateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			TranslateOutcomeCallable translateCallable(const Model::TranslateRequest& request) const;
			TranslateCertificateOutcome translateCertificate(const Model::TranslateCertificateRequest &request)const;
			void translateCertificateAsync(const Model::TranslateCertificateRequest& request, const TranslateCertificateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			TranslateCertificateOutcomeCallable translateCertificateCallable(const Model::TranslateCertificateRequest& request) const;
			TranslateECommerceOutcome translateECommerce(const Model::TranslateECommerceRequest &request)const;
			void translateECommerceAsync(const Model::TranslateECommerceRequest& request, const TranslateECommerceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			TranslateECommerceOutcomeCallable translateECommerceCallable(const Model::TranslateECommerceRequest& request) const;
			TranslateGeneralOutcome translateGeneral(const Model::TranslateGeneralRequest &request)const;
			void translateGeneralAsync(const Model::TranslateGeneralRequest& request, const TranslateGeneralAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			TranslateGeneralOutcomeCallable translateGeneralCallable(const Model::TranslateGeneralRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_ALIMT_ALIMTCLIENT_H_

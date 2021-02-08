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

#include <alibabacloud/alimt/AlimtClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Alimt;
using namespace AlibabaCloud::Alimt::Model;

namespace
{
	const std::string SERVICE_NAME = "alimt";
}

AlimtClient::AlimtClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "alimt");
}

AlimtClient::AlimtClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "alimt");
}

AlimtClient::AlimtClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "alimt");
}

AlimtClient::~AlimtClient()
{}

AlimtClient::CreateDocTranslateTaskOutcome AlimtClient::createDocTranslateTask(const CreateDocTranslateTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateDocTranslateTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateDocTranslateTaskOutcome(CreateDocTranslateTaskResult(outcome.result()));
	else
		return CreateDocTranslateTaskOutcome(outcome.error());
}

void AlimtClient::createDocTranslateTaskAsync(const CreateDocTranslateTaskRequest& request, const CreateDocTranslateTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createDocTranslateTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::CreateDocTranslateTaskOutcomeCallable AlimtClient::createDocTranslateTaskCallable(const CreateDocTranslateTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateDocTranslateTaskOutcome()>>(
			[this, request]()
			{
			return this->createDocTranslateTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::CreateImageTranslateTaskOutcome AlimtClient::createImageTranslateTask(const CreateImageTranslateTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateImageTranslateTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateImageTranslateTaskOutcome(CreateImageTranslateTaskResult(outcome.result()));
	else
		return CreateImageTranslateTaskOutcome(outcome.error());
}

void AlimtClient::createImageTranslateTaskAsync(const CreateImageTranslateTaskRequest& request, const CreateImageTranslateTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createImageTranslateTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::CreateImageTranslateTaskOutcomeCallable AlimtClient::createImageTranslateTaskCallable(const CreateImageTranslateTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateImageTranslateTaskOutcome()>>(
			[this, request]()
			{
			return this->createImageTranslateTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::GetDetectLanguageOutcome AlimtClient::getDetectLanguage(const GetDetectLanguageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDetectLanguageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDetectLanguageOutcome(GetDetectLanguageResult(outcome.result()));
	else
		return GetDetectLanguageOutcome(outcome.error());
}

void AlimtClient::getDetectLanguageAsync(const GetDetectLanguageRequest& request, const GetDetectLanguageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDetectLanguage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::GetDetectLanguageOutcomeCallable AlimtClient::getDetectLanguageCallable(const GetDetectLanguageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDetectLanguageOutcome()>>(
			[this, request]()
			{
			return this->getDetectLanguage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::GetDocTranslateTaskOutcome AlimtClient::getDocTranslateTask(const GetDocTranslateTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDocTranslateTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDocTranslateTaskOutcome(GetDocTranslateTaskResult(outcome.result()));
	else
		return GetDocTranslateTaskOutcome(outcome.error());
}

void AlimtClient::getDocTranslateTaskAsync(const GetDocTranslateTaskRequest& request, const GetDocTranslateTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDocTranslateTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::GetDocTranslateTaskOutcomeCallable AlimtClient::getDocTranslateTaskCallable(const GetDocTranslateTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDocTranslateTaskOutcome()>>(
			[this, request]()
			{
			return this->getDocTranslateTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::GetImageDiagnoseOutcome AlimtClient::getImageDiagnose(const GetImageDiagnoseRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetImageDiagnoseOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetImageDiagnoseOutcome(GetImageDiagnoseResult(outcome.result()));
	else
		return GetImageDiagnoseOutcome(outcome.error());
}

void AlimtClient::getImageDiagnoseAsync(const GetImageDiagnoseRequest& request, const GetImageDiagnoseAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getImageDiagnose(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::GetImageDiagnoseOutcomeCallable AlimtClient::getImageDiagnoseCallable(const GetImageDiagnoseRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetImageDiagnoseOutcome()>>(
			[this, request]()
			{
			return this->getImageDiagnose(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::GetImageTranslateOutcome AlimtClient::getImageTranslate(const GetImageTranslateRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetImageTranslateOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetImageTranslateOutcome(GetImageTranslateResult(outcome.result()));
	else
		return GetImageTranslateOutcome(outcome.error());
}

void AlimtClient::getImageTranslateAsync(const GetImageTranslateRequest& request, const GetImageTranslateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getImageTranslate(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::GetImageTranslateOutcomeCallable AlimtClient::getImageTranslateCallable(const GetImageTranslateRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetImageTranslateOutcome()>>(
			[this, request]()
			{
			return this->getImageTranslate(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::GetImageTranslateTaskOutcome AlimtClient::getImageTranslateTask(const GetImageTranslateTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetImageTranslateTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetImageTranslateTaskOutcome(GetImageTranslateTaskResult(outcome.result()));
	else
		return GetImageTranslateTaskOutcome(outcome.error());
}

void AlimtClient::getImageTranslateTaskAsync(const GetImageTranslateTaskRequest& request, const GetImageTranslateTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getImageTranslateTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::GetImageTranslateTaskOutcomeCallable AlimtClient::getImageTranslateTaskCallable(const GetImageTranslateTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetImageTranslateTaskOutcome()>>(
			[this, request]()
			{
			return this->getImageTranslateTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::GetTitleDiagnoseOutcome AlimtClient::getTitleDiagnose(const GetTitleDiagnoseRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetTitleDiagnoseOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetTitleDiagnoseOutcome(GetTitleDiagnoseResult(outcome.result()));
	else
		return GetTitleDiagnoseOutcome(outcome.error());
}

void AlimtClient::getTitleDiagnoseAsync(const GetTitleDiagnoseRequest& request, const GetTitleDiagnoseAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getTitleDiagnose(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::GetTitleDiagnoseOutcomeCallable AlimtClient::getTitleDiagnoseCallable(const GetTitleDiagnoseRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetTitleDiagnoseOutcome()>>(
			[this, request]()
			{
			return this->getTitleDiagnose(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::GetTitleGenerateOutcome AlimtClient::getTitleGenerate(const GetTitleGenerateRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetTitleGenerateOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetTitleGenerateOutcome(GetTitleGenerateResult(outcome.result()));
	else
		return GetTitleGenerateOutcome(outcome.error());
}

void AlimtClient::getTitleGenerateAsync(const GetTitleGenerateRequest& request, const GetTitleGenerateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getTitleGenerate(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::GetTitleGenerateOutcomeCallable AlimtClient::getTitleGenerateCallable(const GetTitleGenerateRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetTitleGenerateOutcome()>>(
			[this, request]()
			{
			return this->getTitleGenerate(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::GetTitleIntelligenceOutcome AlimtClient::getTitleIntelligence(const GetTitleIntelligenceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetTitleIntelligenceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetTitleIntelligenceOutcome(GetTitleIntelligenceResult(outcome.result()));
	else
		return GetTitleIntelligenceOutcome(outcome.error());
}

void AlimtClient::getTitleIntelligenceAsync(const GetTitleIntelligenceRequest& request, const GetTitleIntelligenceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getTitleIntelligence(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::GetTitleIntelligenceOutcomeCallable AlimtClient::getTitleIntelligenceCallable(const GetTitleIntelligenceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetTitleIntelligenceOutcome()>>(
			[this, request]()
			{
			return this->getTitleIntelligence(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::OpenAlimtServiceOutcome AlimtClient::openAlimtService(const OpenAlimtServiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return OpenAlimtServiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return OpenAlimtServiceOutcome(OpenAlimtServiceResult(outcome.result()));
	else
		return OpenAlimtServiceOutcome(outcome.error());
}

void AlimtClient::openAlimtServiceAsync(const OpenAlimtServiceRequest& request, const OpenAlimtServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, openAlimtService(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::OpenAlimtServiceOutcomeCallable AlimtClient::openAlimtServiceCallable(const OpenAlimtServiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<OpenAlimtServiceOutcome()>>(
			[this, request]()
			{
			return this->openAlimtService(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::TranslateOutcome AlimtClient::translate(const TranslateRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return TranslateOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return TranslateOutcome(TranslateResult(outcome.result()));
	else
		return TranslateOutcome(outcome.error());
}

void AlimtClient::translateAsync(const TranslateRequest& request, const TranslateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, translate(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::TranslateOutcomeCallable AlimtClient::translateCallable(const TranslateRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<TranslateOutcome()>>(
			[this, request]()
			{
			return this->translate(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::TranslateCertificateOutcome AlimtClient::translateCertificate(const TranslateCertificateRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return TranslateCertificateOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return TranslateCertificateOutcome(TranslateCertificateResult(outcome.result()));
	else
		return TranslateCertificateOutcome(outcome.error());
}

void AlimtClient::translateCertificateAsync(const TranslateCertificateRequest& request, const TranslateCertificateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, translateCertificate(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::TranslateCertificateOutcomeCallable AlimtClient::translateCertificateCallable(const TranslateCertificateRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<TranslateCertificateOutcome()>>(
			[this, request]()
			{
			return this->translateCertificate(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::TranslateECommerceOutcome AlimtClient::translateECommerce(const TranslateECommerceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return TranslateECommerceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return TranslateECommerceOutcome(TranslateECommerceResult(outcome.result()));
	else
		return TranslateECommerceOutcome(outcome.error());
}

void AlimtClient::translateECommerceAsync(const TranslateECommerceRequest& request, const TranslateECommerceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, translateECommerce(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::TranslateECommerceOutcomeCallable AlimtClient::translateECommerceCallable(const TranslateECommerceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<TranslateECommerceOutcome()>>(
			[this, request]()
			{
			return this->translateECommerce(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlimtClient::TranslateGeneralOutcome AlimtClient::translateGeneral(const TranslateGeneralRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return TranslateGeneralOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return TranslateGeneralOutcome(TranslateGeneralResult(outcome.result()));
	else
		return TranslateGeneralOutcome(outcome.error());
}

void AlimtClient::translateGeneralAsync(const TranslateGeneralRequest& request, const TranslateGeneralAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, translateGeneral(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlimtClient::TranslateGeneralOutcomeCallable AlimtClient::translateGeneralCallable(const TranslateGeneralRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<TranslateGeneralOutcome()>>(
			[this, request]()
			{
			return this->translateGeneral(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


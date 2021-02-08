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

#include <alibabacloud/alinlp/AlinlpClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Alinlp;
using namespace AlibabaCloud::Alinlp::Model;

namespace
{
	const std::string SERVICE_NAME = "alinlp";
}

AlinlpClient::AlinlpClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "alinlp");
}

AlinlpClient::AlinlpClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "alinlp");
}

AlinlpClient::AlinlpClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "alinlp");
}

AlinlpClient::~AlinlpClient()
{}

AlinlpClient::GetDpChEcomOutcome AlinlpClient::getDpChEcom(const GetDpChEcomRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDpChEcomOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDpChEcomOutcome(GetDpChEcomResult(outcome.result()));
	else
		return GetDpChEcomOutcome(outcome.error());
}

void AlinlpClient::getDpChEcomAsync(const GetDpChEcomRequest& request, const GetDpChEcomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDpChEcom(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlinlpClient::GetDpChEcomOutcomeCallable AlinlpClient::getDpChEcomCallable(const GetDpChEcomRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDpChEcomOutcome()>>(
			[this, request]()
			{
			return this->getDpChEcom(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlinlpClient::GetEcChGeneralOutcome AlinlpClient::getEcChGeneral(const GetEcChGeneralRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetEcChGeneralOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetEcChGeneralOutcome(GetEcChGeneralResult(outcome.result()));
	else
		return GetEcChGeneralOutcome(outcome.error());
}

void AlinlpClient::getEcChGeneralAsync(const GetEcChGeneralRequest& request, const GetEcChGeneralAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getEcChGeneral(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlinlpClient::GetEcChGeneralOutcomeCallable AlinlpClient::getEcChGeneralCallable(const GetEcChGeneralRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetEcChGeneralOutcome()>>(
			[this, request]()
			{
			return this->getEcChGeneral(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlinlpClient::GetNerChMedicalOutcome AlinlpClient::getNerChMedical(const GetNerChMedicalRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetNerChMedicalOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetNerChMedicalOutcome(GetNerChMedicalResult(outcome.result()));
	else
		return GetNerChMedicalOutcome(outcome.error());
}

void AlinlpClient::getNerChMedicalAsync(const GetNerChMedicalRequest& request, const GetNerChMedicalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getNerChMedical(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlinlpClient::GetNerChMedicalOutcomeCallable AlinlpClient::getNerChMedicalCallable(const GetNerChMedicalRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetNerChMedicalOutcome()>>(
			[this, request]()
			{
			return this->getNerChMedical(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlinlpClient::GetNerCustomizedChEcomOutcome AlinlpClient::getNerCustomizedChEcom(const GetNerCustomizedChEcomRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetNerCustomizedChEcomOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetNerCustomizedChEcomOutcome(GetNerCustomizedChEcomResult(outcome.result()));
	else
		return GetNerCustomizedChEcomOutcome(outcome.error());
}

void AlinlpClient::getNerCustomizedChEcomAsync(const GetNerCustomizedChEcomRequest& request, const GetNerCustomizedChEcomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getNerCustomizedChEcom(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlinlpClient::GetNerCustomizedChEcomOutcomeCallable AlinlpClient::getNerCustomizedChEcomCallable(const GetNerCustomizedChEcomRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetNerCustomizedChEcomOutcome()>>(
			[this, request]()
			{
			return this->getNerCustomizedChEcom(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlinlpClient::GetPosChEcomOutcome AlinlpClient::getPosChEcom(const GetPosChEcomRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetPosChEcomOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetPosChEcomOutcome(GetPosChEcomResult(outcome.result()));
	else
		return GetPosChEcomOutcome(outcome.error());
}

void AlinlpClient::getPosChEcomAsync(const GetPosChEcomRequest& request, const GetPosChEcomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getPosChEcom(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlinlpClient::GetPosChEcomOutcomeCallable AlinlpClient::getPosChEcomCallable(const GetPosChEcomRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetPosChEcomOutcome()>>(
			[this, request]()
			{
			return this->getPosChEcom(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlinlpClient::GetSummaryChEcomOutcome AlinlpClient::getSummaryChEcom(const GetSummaryChEcomRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetSummaryChEcomOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetSummaryChEcomOutcome(GetSummaryChEcomResult(outcome.result()));
	else
		return GetSummaryChEcomOutcome(outcome.error());
}

void AlinlpClient::getSummaryChEcomAsync(const GetSummaryChEcomRequest& request, const GetSummaryChEcomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getSummaryChEcom(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlinlpClient::GetSummaryChEcomOutcomeCallable AlinlpClient::getSummaryChEcomCallable(const GetSummaryChEcomRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetSummaryChEcomOutcome()>>(
			[this, request]()
			{
			return this->getSummaryChEcom(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlinlpClient::GetTsChEcomOutcome AlinlpClient::getTsChEcom(const GetTsChEcomRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetTsChEcomOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetTsChEcomOutcome(GetTsChEcomResult(outcome.result()));
	else
		return GetTsChEcomOutcome(outcome.error());
}

void AlinlpClient::getTsChEcomAsync(const GetTsChEcomRequest& request, const GetTsChEcomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getTsChEcom(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlinlpClient::GetTsChEcomOutcomeCallable AlinlpClient::getTsChEcomCallable(const GetTsChEcomRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetTsChEcomOutcome()>>(
			[this, request]()
			{
			return this->getTsChEcom(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlinlpClient::GetWeChEcomOutcome AlinlpClient::getWeChEcom(const GetWeChEcomRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetWeChEcomOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetWeChEcomOutcome(GetWeChEcomResult(outcome.result()));
	else
		return GetWeChEcomOutcome(outcome.error());
}

void AlinlpClient::getWeChEcomAsync(const GetWeChEcomRequest& request, const GetWeChEcomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getWeChEcom(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlinlpClient::GetWeChEcomOutcomeCallable AlinlpClient::getWeChEcomCallable(const GetWeChEcomRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetWeChEcomOutcome()>>(
			[this, request]()
			{
			return this->getWeChEcom(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlinlpClient::GetWsCustomizedChGeneralOutcome AlinlpClient::getWsCustomizedChGeneral(const GetWsCustomizedChGeneralRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetWsCustomizedChGeneralOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetWsCustomizedChGeneralOutcome(GetWsCustomizedChGeneralResult(outcome.result()));
	else
		return GetWsCustomizedChGeneralOutcome(outcome.error());
}

void AlinlpClient::getWsCustomizedChGeneralAsync(const GetWsCustomizedChGeneralRequest& request, const GetWsCustomizedChGeneralAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getWsCustomizedChGeneral(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlinlpClient::GetWsCustomizedChGeneralOutcomeCallable AlinlpClient::getWsCustomizedChGeneralCallable(const GetWsCustomizedChGeneralRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetWsCustomizedChGeneralOutcome()>>(
			[this, request]()
			{
			return this->getWsCustomizedChGeneral(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AlinlpClient::GetWsCustomizedSeaGeneralOutcome AlinlpClient::getWsCustomizedSeaGeneral(const GetWsCustomizedSeaGeneralRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetWsCustomizedSeaGeneralOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetWsCustomizedSeaGeneralOutcome(GetWsCustomizedSeaGeneralResult(outcome.result()));
	else
		return GetWsCustomizedSeaGeneralOutcome(outcome.error());
}

void AlinlpClient::getWsCustomizedSeaGeneralAsync(const GetWsCustomizedSeaGeneralRequest& request, const GetWsCustomizedSeaGeneralAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getWsCustomizedSeaGeneral(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AlinlpClient::GetWsCustomizedSeaGeneralOutcomeCallable AlinlpClient::getWsCustomizedSeaGeneralCallable(const GetWsCustomizedSeaGeneralRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetWsCustomizedSeaGeneralOutcome()>>(
			[this, request]()
			{
			return this->getWsCustomizedSeaGeneral(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


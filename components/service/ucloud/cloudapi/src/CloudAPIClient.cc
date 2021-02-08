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

#include <alibabacloud/cloudapi/CloudAPIClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

namespace
{
	const std::string SERVICE_NAME = "CloudAPI";
}

CloudAPIClient::CloudAPIClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "cloudapi");
}

CloudAPIClient::CloudAPIClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "cloudapi");
}

CloudAPIClient::CloudAPIClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "cloudapi");
}

CloudAPIClient::~CloudAPIClient()
{}

CloudAPIClient::AbolishApiOutcome CloudAPIClient::abolishApi(const AbolishApiRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AbolishApiOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AbolishApiOutcome(AbolishApiResult(outcome.result()));
	else
		return AbolishApiOutcome(outcome.error());
}

void CloudAPIClient::abolishApiAsync(const AbolishApiRequest& request, const AbolishApiAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, abolishApi(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::AbolishApiOutcomeCallable CloudAPIClient::abolishApiCallable(const AbolishApiRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AbolishApiOutcome()>>(
			[this, request]()
			{
			return this->abolishApi(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::AddIpControlPolicyItemOutcome CloudAPIClient::addIpControlPolicyItem(const AddIpControlPolicyItemRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddIpControlPolicyItemOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddIpControlPolicyItemOutcome(AddIpControlPolicyItemResult(outcome.result()));
	else
		return AddIpControlPolicyItemOutcome(outcome.error());
}

void CloudAPIClient::addIpControlPolicyItemAsync(const AddIpControlPolicyItemRequest& request, const AddIpControlPolicyItemAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addIpControlPolicyItem(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::AddIpControlPolicyItemOutcomeCallable CloudAPIClient::addIpControlPolicyItemCallable(const AddIpControlPolicyItemRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddIpControlPolicyItemOutcome()>>(
			[this, request]()
			{
			return this->addIpControlPolicyItem(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::AddTrafficSpecialControlOutcome CloudAPIClient::addTrafficSpecialControl(const AddTrafficSpecialControlRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddTrafficSpecialControlOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddTrafficSpecialControlOutcome(AddTrafficSpecialControlResult(outcome.result()));
	else
		return AddTrafficSpecialControlOutcome(outcome.error());
}

void CloudAPIClient::addTrafficSpecialControlAsync(const AddTrafficSpecialControlRequest& request, const AddTrafficSpecialControlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addTrafficSpecialControl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::AddTrafficSpecialControlOutcomeCallable CloudAPIClient::addTrafficSpecialControlCallable(const AddTrafficSpecialControlRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddTrafficSpecialControlOutcome()>>(
			[this, request]()
			{
			return this->addTrafficSpecialControl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::CreateApiOutcome CloudAPIClient::createApi(const CreateApiRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateApiOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateApiOutcome(CreateApiResult(outcome.result()));
	else
		return CreateApiOutcome(outcome.error());
}

void CloudAPIClient::createApiAsync(const CreateApiRequest& request, const CreateApiAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createApi(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::CreateApiOutcomeCallable CloudAPIClient::createApiCallable(const CreateApiRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateApiOutcome()>>(
			[this, request]()
			{
			return this->createApi(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::CreateApiGroupOutcome CloudAPIClient::createApiGroup(const CreateApiGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateApiGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateApiGroupOutcome(CreateApiGroupResult(outcome.result()));
	else
		return CreateApiGroupOutcome(outcome.error());
}

void CloudAPIClient::createApiGroupAsync(const CreateApiGroupRequest& request, const CreateApiGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createApiGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::CreateApiGroupOutcomeCallable CloudAPIClient::createApiGroupCallable(const CreateApiGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateApiGroupOutcome()>>(
			[this, request]()
			{
			return this->createApiGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::CreateApiStageVariableOutcome CloudAPIClient::createApiStageVariable(const CreateApiStageVariableRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateApiStageVariableOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateApiStageVariableOutcome(CreateApiStageVariableResult(outcome.result()));
	else
		return CreateApiStageVariableOutcome(outcome.error());
}

void CloudAPIClient::createApiStageVariableAsync(const CreateApiStageVariableRequest& request, const CreateApiStageVariableAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createApiStageVariable(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::CreateApiStageVariableOutcomeCallable CloudAPIClient::createApiStageVariableCallable(const CreateApiStageVariableRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateApiStageVariableOutcome()>>(
			[this, request]()
			{
			return this->createApiStageVariable(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::CreateAppOutcome CloudAPIClient::createApp(const CreateAppRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateAppOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateAppOutcome(CreateAppResult(outcome.result()));
	else
		return CreateAppOutcome(outcome.error());
}

void CloudAPIClient::createAppAsync(const CreateAppRequest& request, const CreateAppAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createApp(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::CreateAppOutcomeCallable CloudAPIClient::createAppCallable(const CreateAppRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateAppOutcome()>>(
			[this, request]()
			{
			return this->createApp(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::CreateInstanceOutcome CloudAPIClient::createInstance(const CreateInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateInstanceOutcome(CreateInstanceResult(outcome.result()));
	else
		return CreateInstanceOutcome(outcome.error());
}

void CloudAPIClient::createInstanceAsync(const CreateInstanceRequest& request, const CreateInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::CreateInstanceOutcomeCallable CloudAPIClient::createInstanceCallable(const CreateInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateInstanceOutcome()>>(
			[this, request]()
			{
			return this->createInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::CreateIntranetDomainOutcome CloudAPIClient::createIntranetDomain(const CreateIntranetDomainRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateIntranetDomainOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateIntranetDomainOutcome(CreateIntranetDomainResult(outcome.result()));
	else
		return CreateIntranetDomainOutcome(outcome.error());
}

void CloudAPIClient::createIntranetDomainAsync(const CreateIntranetDomainRequest& request, const CreateIntranetDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createIntranetDomain(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::CreateIntranetDomainOutcomeCallable CloudAPIClient::createIntranetDomainCallable(const CreateIntranetDomainRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateIntranetDomainOutcome()>>(
			[this, request]()
			{
			return this->createIntranetDomain(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::CreateIpControlOutcome CloudAPIClient::createIpControl(const CreateIpControlRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateIpControlOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateIpControlOutcome(CreateIpControlResult(outcome.result()));
	else
		return CreateIpControlOutcome(outcome.error());
}

void CloudAPIClient::createIpControlAsync(const CreateIpControlRequest& request, const CreateIpControlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createIpControl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::CreateIpControlOutcomeCallable CloudAPIClient::createIpControlCallable(const CreateIpControlRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateIpControlOutcome()>>(
			[this, request]()
			{
			return this->createIpControl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::CreateLogConfigOutcome CloudAPIClient::createLogConfig(const CreateLogConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateLogConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateLogConfigOutcome(CreateLogConfigResult(outcome.result()));
	else
		return CreateLogConfigOutcome(outcome.error());
}

void CloudAPIClient::createLogConfigAsync(const CreateLogConfigRequest& request, const CreateLogConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createLogConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::CreateLogConfigOutcomeCallable CloudAPIClient::createLogConfigCallable(const CreateLogConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateLogConfigOutcome()>>(
			[this, request]()
			{
			return this->createLogConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::CreateSignatureOutcome CloudAPIClient::createSignature(const CreateSignatureRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateSignatureOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateSignatureOutcome(CreateSignatureResult(outcome.result()));
	else
		return CreateSignatureOutcome(outcome.error());
}

void CloudAPIClient::createSignatureAsync(const CreateSignatureRequest& request, const CreateSignatureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createSignature(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::CreateSignatureOutcomeCallable CloudAPIClient::createSignatureCallable(const CreateSignatureRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateSignatureOutcome()>>(
			[this, request]()
			{
			return this->createSignature(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::CreateTrafficControlOutcome CloudAPIClient::createTrafficControl(const CreateTrafficControlRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateTrafficControlOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateTrafficControlOutcome(CreateTrafficControlResult(outcome.result()));
	else
		return CreateTrafficControlOutcome(outcome.error());
}

void CloudAPIClient::createTrafficControlAsync(const CreateTrafficControlRequest& request, const CreateTrafficControlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createTrafficControl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::CreateTrafficControlOutcomeCallable CloudAPIClient::createTrafficControlCallable(const CreateTrafficControlRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateTrafficControlOutcome()>>(
			[this, request]()
			{
			return this->createTrafficControl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeleteAllTrafficSpecialControlOutcome CloudAPIClient::deleteAllTrafficSpecialControl(const DeleteAllTrafficSpecialControlRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteAllTrafficSpecialControlOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteAllTrafficSpecialControlOutcome(DeleteAllTrafficSpecialControlResult(outcome.result()));
	else
		return DeleteAllTrafficSpecialControlOutcome(outcome.error());
}

void CloudAPIClient::deleteAllTrafficSpecialControlAsync(const DeleteAllTrafficSpecialControlRequest& request, const DeleteAllTrafficSpecialControlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteAllTrafficSpecialControl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeleteAllTrafficSpecialControlOutcomeCallable CloudAPIClient::deleteAllTrafficSpecialControlCallable(const DeleteAllTrafficSpecialControlRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteAllTrafficSpecialControlOutcome()>>(
			[this, request]()
			{
			return this->deleteAllTrafficSpecialControl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeleteApiOutcome CloudAPIClient::deleteApi(const DeleteApiRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteApiOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteApiOutcome(DeleteApiResult(outcome.result()));
	else
		return DeleteApiOutcome(outcome.error());
}

void CloudAPIClient::deleteApiAsync(const DeleteApiRequest& request, const DeleteApiAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteApi(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeleteApiOutcomeCallable CloudAPIClient::deleteApiCallable(const DeleteApiRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteApiOutcome()>>(
			[this, request]()
			{
			return this->deleteApi(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeleteApiGroupOutcome CloudAPIClient::deleteApiGroup(const DeleteApiGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteApiGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteApiGroupOutcome(DeleteApiGroupResult(outcome.result()));
	else
		return DeleteApiGroupOutcome(outcome.error());
}

void CloudAPIClient::deleteApiGroupAsync(const DeleteApiGroupRequest& request, const DeleteApiGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteApiGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeleteApiGroupOutcomeCallable CloudAPIClient::deleteApiGroupCallable(const DeleteApiGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteApiGroupOutcome()>>(
			[this, request]()
			{
			return this->deleteApiGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeleteApiStageVariableOutcome CloudAPIClient::deleteApiStageVariable(const DeleteApiStageVariableRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteApiStageVariableOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteApiStageVariableOutcome(DeleteApiStageVariableResult(outcome.result()));
	else
		return DeleteApiStageVariableOutcome(outcome.error());
}

void CloudAPIClient::deleteApiStageVariableAsync(const DeleteApiStageVariableRequest& request, const DeleteApiStageVariableAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteApiStageVariable(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeleteApiStageVariableOutcomeCallable CloudAPIClient::deleteApiStageVariableCallable(const DeleteApiStageVariableRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteApiStageVariableOutcome()>>(
			[this, request]()
			{
			return this->deleteApiStageVariable(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeleteAppOutcome CloudAPIClient::deleteApp(const DeleteAppRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteAppOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteAppOutcome(DeleteAppResult(outcome.result()));
	else
		return DeleteAppOutcome(outcome.error());
}

void CloudAPIClient::deleteAppAsync(const DeleteAppRequest& request, const DeleteAppAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteApp(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeleteAppOutcomeCallable CloudAPIClient::deleteAppCallable(const DeleteAppRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteAppOutcome()>>(
			[this, request]()
			{
			return this->deleteApp(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeleteDomainOutcome CloudAPIClient::deleteDomain(const DeleteDomainRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteDomainOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteDomainOutcome(DeleteDomainResult(outcome.result()));
	else
		return DeleteDomainOutcome(outcome.error());
}

void CloudAPIClient::deleteDomainAsync(const DeleteDomainRequest& request, const DeleteDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteDomain(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeleteDomainOutcomeCallable CloudAPIClient::deleteDomainCallable(const DeleteDomainRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteDomainOutcome()>>(
			[this, request]()
			{
			return this->deleteDomain(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeleteDomainCertificateOutcome CloudAPIClient::deleteDomainCertificate(const DeleteDomainCertificateRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteDomainCertificateOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteDomainCertificateOutcome(DeleteDomainCertificateResult(outcome.result()));
	else
		return DeleteDomainCertificateOutcome(outcome.error());
}

void CloudAPIClient::deleteDomainCertificateAsync(const DeleteDomainCertificateRequest& request, const DeleteDomainCertificateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteDomainCertificate(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeleteDomainCertificateOutcomeCallable CloudAPIClient::deleteDomainCertificateCallable(const DeleteDomainCertificateRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteDomainCertificateOutcome()>>(
			[this, request]()
			{
			return this->deleteDomainCertificate(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeleteInstanceOutcome CloudAPIClient::deleteInstance(const DeleteInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteInstanceOutcome(DeleteInstanceResult(outcome.result()));
	else
		return DeleteInstanceOutcome(outcome.error());
}

void CloudAPIClient::deleteInstanceAsync(const DeleteInstanceRequest& request, const DeleteInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeleteInstanceOutcomeCallable CloudAPIClient::deleteInstanceCallable(const DeleteInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteInstanceOutcome()>>(
			[this, request]()
			{
			return this->deleteInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeleteIpControlOutcome CloudAPIClient::deleteIpControl(const DeleteIpControlRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteIpControlOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteIpControlOutcome(DeleteIpControlResult(outcome.result()));
	else
		return DeleteIpControlOutcome(outcome.error());
}

void CloudAPIClient::deleteIpControlAsync(const DeleteIpControlRequest& request, const DeleteIpControlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteIpControl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeleteIpControlOutcomeCallable CloudAPIClient::deleteIpControlCallable(const DeleteIpControlRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteIpControlOutcome()>>(
			[this, request]()
			{
			return this->deleteIpControl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeleteLogConfigOutcome CloudAPIClient::deleteLogConfig(const DeleteLogConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLogConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLogConfigOutcome(DeleteLogConfigResult(outcome.result()));
	else
		return DeleteLogConfigOutcome(outcome.error());
}

void CloudAPIClient::deleteLogConfigAsync(const DeleteLogConfigRequest& request, const DeleteLogConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLogConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeleteLogConfigOutcomeCallable CloudAPIClient::deleteLogConfigCallable(const DeleteLogConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLogConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteLogConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeleteSignatureOutcome CloudAPIClient::deleteSignature(const DeleteSignatureRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteSignatureOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteSignatureOutcome(DeleteSignatureResult(outcome.result()));
	else
		return DeleteSignatureOutcome(outcome.error());
}

void CloudAPIClient::deleteSignatureAsync(const DeleteSignatureRequest& request, const DeleteSignatureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteSignature(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeleteSignatureOutcomeCallable CloudAPIClient::deleteSignatureCallable(const DeleteSignatureRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteSignatureOutcome()>>(
			[this, request]()
			{
			return this->deleteSignature(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeleteTrafficControlOutcome CloudAPIClient::deleteTrafficControl(const DeleteTrafficControlRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteTrafficControlOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteTrafficControlOutcome(DeleteTrafficControlResult(outcome.result()));
	else
		return DeleteTrafficControlOutcome(outcome.error());
}

void CloudAPIClient::deleteTrafficControlAsync(const DeleteTrafficControlRequest& request, const DeleteTrafficControlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteTrafficControl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeleteTrafficControlOutcomeCallable CloudAPIClient::deleteTrafficControlCallable(const DeleteTrafficControlRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteTrafficControlOutcome()>>(
			[this, request]()
			{
			return this->deleteTrafficControl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeleteTrafficSpecialControlOutcome CloudAPIClient::deleteTrafficSpecialControl(const DeleteTrafficSpecialControlRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteTrafficSpecialControlOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteTrafficSpecialControlOutcome(DeleteTrafficSpecialControlResult(outcome.result()));
	else
		return DeleteTrafficSpecialControlOutcome(outcome.error());
}

void CloudAPIClient::deleteTrafficSpecialControlAsync(const DeleteTrafficSpecialControlRequest& request, const DeleteTrafficSpecialControlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteTrafficSpecialControl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeleteTrafficSpecialControlOutcomeCallable CloudAPIClient::deleteTrafficSpecialControlCallable(const DeleteTrafficSpecialControlRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteTrafficSpecialControlOutcome()>>(
			[this, request]()
			{
			return this->deleteTrafficSpecialControl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DeployApiOutcome CloudAPIClient::deployApi(const DeployApiRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeployApiOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeployApiOutcome(DeployApiResult(outcome.result()));
	else
		return DeployApiOutcome(outcome.error());
}

void CloudAPIClient::deployApiAsync(const DeployApiRequest& request, const DeployApiAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deployApi(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DeployApiOutcomeCallable CloudAPIClient::deployApiCallable(const DeployApiRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeployApiOutcome()>>(
			[this, request]()
			{
			return this->deployApi(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiOutcome CloudAPIClient::describeApi(const DescribeApiRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiOutcome(DescribeApiResult(outcome.result()));
	else
		return DescribeApiOutcome(outcome.error());
}

void CloudAPIClient::describeApiAsync(const DescribeApiRequest& request, const DescribeApiAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApi(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiOutcomeCallable CloudAPIClient::describeApiCallable(const DescribeApiRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiOutcome()>>(
			[this, request]()
			{
			return this->describeApi(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiDocOutcome CloudAPIClient::describeApiDoc(const DescribeApiDocRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiDocOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiDocOutcome(DescribeApiDocResult(outcome.result()));
	else
		return DescribeApiDocOutcome(outcome.error());
}

void CloudAPIClient::describeApiDocAsync(const DescribeApiDocRequest& request, const DescribeApiDocAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiDoc(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiDocOutcomeCallable CloudAPIClient::describeApiDocCallable(const DescribeApiDocRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiDocOutcome()>>(
			[this, request]()
			{
			return this->describeApiDoc(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiErrorDataOutcome CloudAPIClient::describeApiErrorData(const DescribeApiErrorDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiErrorDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiErrorDataOutcome(DescribeApiErrorDataResult(outcome.result()));
	else
		return DescribeApiErrorDataOutcome(outcome.error());
}

void CloudAPIClient::describeApiErrorDataAsync(const DescribeApiErrorDataRequest& request, const DescribeApiErrorDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiErrorData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiErrorDataOutcomeCallable CloudAPIClient::describeApiErrorDataCallable(const DescribeApiErrorDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiErrorDataOutcome()>>(
			[this, request]()
			{
			return this->describeApiErrorData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiGroupOutcome CloudAPIClient::describeApiGroup(const DescribeApiGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiGroupOutcome(DescribeApiGroupResult(outcome.result()));
	else
		return DescribeApiGroupOutcome(outcome.error());
}

void CloudAPIClient::describeApiGroupAsync(const DescribeApiGroupRequest& request, const DescribeApiGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiGroupOutcomeCallable CloudAPIClient::describeApiGroupCallable(const DescribeApiGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiGroupOutcome()>>(
			[this, request]()
			{
			return this->describeApiGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiGroupVpcWhitelistOutcome CloudAPIClient::describeApiGroupVpcWhitelist(const DescribeApiGroupVpcWhitelistRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiGroupVpcWhitelistOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiGroupVpcWhitelistOutcome(DescribeApiGroupVpcWhitelistResult(outcome.result()));
	else
		return DescribeApiGroupVpcWhitelistOutcome(outcome.error());
}

void CloudAPIClient::describeApiGroupVpcWhitelistAsync(const DescribeApiGroupVpcWhitelistRequest& request, const DescribeApiGroupVpcWhitelistAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiGroupVpcWhitelist(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiGroupVpcWhitelistOutcomeCallable CloudAPIClient::describeApiGroupVpcWhitelistCallable(const DescribeApiGroupVpcWhitelistRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiGroupVpcWhitelistOutcome()>>(
			[this, request]()
			{
			return this->describeApiGroupVpcWhitelist(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiGroupsOutcome CloudAPIClient::describeApiGroups(const DescribeApiGroupsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiGroupsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiGroupsOutcome(DescribeApiGroupsResult(outcome.result()));
	else
		return DescribeApiGroupsOutcome(outcome.error());
}

void CloudAPIClient::describeApiGroupsAsync(const DescribeApiGroupsRequest& request, const DescribeApiGroupsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiGroups(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiGroupsOutcomeCallable CloudAPIClient::describeApiGroupsCallable(const DescribeApiGroupsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiGroupsOutcome()>>(
			[this, request]()
			{
			return this->describeApiGroups(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiHistoriesOutcome CloudAPIClient::describeApiHistories(const DescribeApiHistoriesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiHistoriesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiHistoriesOutcome(DescribeApiHistoriesResult(outcome.result()));
	else
		return DescribeApiHistoriesOutcome(outcome.error());
}

void CloudAPIClient::describeApiHistoriesAsync(const DescribeApiHistoriesRequest& request, const DescribeApiHistoriesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiHistories(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiHistoriesOutcomeCallable CloudAPIClient::describeApiHistoriesCallable(const DescribeApiHistoriesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiHistoriesOutcome()>>(
			[this, request]()
			{
			return this->describeApiHistories(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiHistoryOutcome CloudAPIClient::describeApiHistory(const DescribeApiHistoryRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiHistoryOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiHistoryOutcome(DescribeApiHistoryResult(outcome.result()));
	else
		return DescribeApiHistoryOutcome(outcome.error());
}

void CloudAPIClient::describeApiHistoryAsync(const DescribeApiHistoryRequest& request, const DescribeApiHistoryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiHistory(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiHistoryOutcomeCallable CloudAPIClient::describeApiHistoryCallable(const DescribeApiHistoryRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiHistoryOutcome()>>(
			[this, request]()
			{
			return this->describeApiHistory(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiIpControlsOutcome CloudAPIClient::describeApiIpControls(const DescribeApiIpControlsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiIpControlsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiIpControlsOutcome(DescribeApiIpControlsResult(outcome.result()));
	else
		return DescribeApiIpControlsOutcome(outcome.error());
}

void CloudAPIClient::describeApiIpControlsAsync(const DescribeApiIpControlsRequest& request, const DescribeApiIpControlsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiIpControls(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiIpControlsOutcomeCallable CloudAPIClient::describeApiIpControlsCallable(const DescribeApiIpControlsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiIpControlsOutcome()>>(
			[this, request]()
			{
			return this->describeApiIpControls(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiLatencyDataOutcome CloudAPIClient::describeApiLatencyData(const DescribeApiLatencyDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiLatencyDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiLatencyDataOutcome(DescribeApiLatencyDataResult(outcome.result()));
	else
		return DescribeApiLatencyDataOutcome(outcome.error());
}

void CloudAPIClient::describeApiLatencyDataAsync(const DescribeApiLatencyDataRequest& request, const DescribeApiLatencyDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiLatencyData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiLatencyDataOutcomeCallable CloudAPIClient::describeApiLatencyDataCallable(const DescribeApiLatencyDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiLatencyDataOutcome()>>(
			[this, request]()
			{
			return this->describeApiLatencyData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiMarketAttributesOutcome CloudAPIClient::describeApiMarketAttributes(const DescribeApiMarketAttributesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiMarketAttributesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiMarketAttributesOutcome(DescribeApiMarketAttributesResult(outcome.result()));
	else
		return DescribeApiMarketAttributesOutcome(outcome.error());
}

void CloudAPIClient::describeApiMarketAttributesAsync(const DescribeApiMarketAttributesRequest& request, const DescribeApiMarketAttributesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiMarketAttributes(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiMarketAttributesOutcomeCallable CloudAPIClient::describeApiMarketAttributesCallable(const DescribeApiMarketAttributesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiMarketAttributesOutcome()>>(
			[this, request]()
			{
			return this->describeApiMarketAttributes(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiQpsDataOutcome CloudAPIClient::describeApiQpsData(const DescribeApiQpsDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiQpsDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiQpsDataOutcome(DescribeApiQpsDataResult(outcome.result()));
	else
		return DescribeApiQpsDataOutcome(outcome.error());
}

void CloudAPIClient::describeApiQpsDataAsync(const DescribeApiQpsDataRequest& request, const DescribeApiQpsDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiQpsData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiQpsDataOutcomeCallable CloudAPIClient::describeApiQpsDataCallable(const DescribeApiQpsDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiQpsDataOutcome()>>(
			[this, request]()
			{
			return this->describeApiQpsData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiSignaturesOutcome CloudAPIClient::describeApiSignatures(const DescribeApiSignaturesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiSignaturesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiSignaturesOutcome(DescribeApiSignaturesResult(outcome.result()));
	else
		return DescribeApiSignaturesOutcome(outcome.error());
}

void CloudAPIClient::describeApiSignaturesAsync(const DescribeApiSignaturesRequest& request, const DescribeApiSignaturesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiSignatures(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiSignaturesOutcomeCallable CloudAPIClient::describeApiSignaturesCallable(const DescribeApiSignaturesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiSignaturesOutcome()>>(
			[this, request]()
			{
			return this->describeApiSignatures(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiStageOutcome CloudAPIClient::describeApiStage(const DescribeApiStageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiStageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiStageOutcome(DescribeApiStageResult(outcome.result()));
	else
		return DescribeApiStageOutcome(outcome.error());
}

void CloudAPIClient::describeApiStageAsync(const DescribeApiStageRequest& request, const DescribeApiStageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiStage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiStageOutcomeCallable CloudAPIClient::describeApiStageCallable(const DescribeApiStageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiStageOutcome()>>(
			[this, request]()
			{
			return this->describeApiStage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiTrafficControlsOutcome CloudAPIClient::describeApiTrafficControls(const DescribeApiTrafficControlsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiTrafficControlsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiTrafficControlsOutcome(DescribeApiTrafficControlsResult(outcome.result()));
	else
		return DescribeApiTrafficControlsOutcome(outcome.error());
}

void CloudAPIClient::describeApiTrafficControlsAsync(const DescribeApiTrafficControlsRequest& request, const DescribeApiTrafficControlsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiTrafficControls(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiTrafficControlsOutcomeCallable CloudAPIClient::describeApiTrafficControlsCallable(const DescribeApiTrafficControlsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiTrafficControlsOutcome()>>(
			[this, request]()
			{
			return this->describeApiTrafficControls(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApiTrafficDataOutcome CloudAPIClient::describeApiTrafficData(const DescribeApiTrafficDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApiTrafficDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApiTrafficDataOutcome(DescribeApiTrafficDataResult(outcome.result()));
	else
		return DescribeApiTrafficDataOutcome(outcome.error());
}

void CloudAPIClient::describeApiTrafficDataAsync(const DescribeApiTrafficDataRequest& request, const DescribeApiTrafficDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApiTrafficData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApiTrafficDataOutcomeCallable CloudAPIClient::describeApiTrafficDataCallable(const DescribeApiTrafficDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApiTrafficDataOutcome()>>(
			[this, request]()
			{
			return this->describeApiTrafficData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApisOutcome CloudAPIClient::describeApis(const DescribeApisRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApisOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApisOutcome(DescribeApisResult(outcome.result()));
	else
		return DescribeApisOutcome(outcome.error());
}

void CloudAPIClient::describeApisAsync(const DescribeApisRequest& request, const DescribeApisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApis(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApisOutcomeCallable CloudAPIClient::describeApisCallable(const DescribeApisRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApisOutcome()>>(
			[this, request]()
			{
			return this->describeApis(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApisByAppOutcome CloudAPIClient::describeApisByApp(const DescribeApisByAppRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApisByAppOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApisByAppOutcome(DescribeApisByAppResult(outcome.result()));
	else
		return DescribeApisByAppOutcome(outcome.error());
}

void CloudAPIClient::describeApisByAppAsync(const DescribeApisByAppRequest& request, const DescribeApisByAppAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApisByApp(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApisByAppOutcomeCallable CloudAPIClient::describeApisByAppCallable(const DescribeApisByAppRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApisByAppOutcome()>>(
			[this, request]()
			{
			return this->describeApisByApp(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApisByIpControlOutcome CloudAPIClient::describeApisByIpControl(const DescribeApisByIpControlRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApisByIpControlOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApisByIpControlOutcome(DescribeApisByIpControlResult(outcome.result()));
	else
		return DescribeApisByIpControlOutcome(outcome.error());
}

void CloudAPIClient::describeApisByIpControlAsync(const DescribeApisByIpControlRequest& request, const DescribeApisByIpControlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApisByIpControl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApisByIpControlOutcomeCallable CloudAPIClient::describeApisByIpControlCallable(const DescribeApisByIpControlRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApisByIpControlOutcome()>>(
			[this, request]()
			{
			return this->describeApisByIpControl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApisBySignatureOutcome CloudAPIClient::describeApisBySignature(const DescribeApisBySignatureRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApisBySignatureOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApisBySignatureOutcome(DescribeApisBySignatureResult(outcome.result()));
	else
		return DescribeApisBySignatureOutcome(outcome.error());
}

void CloudAPIClient::describeApisBySignatureAsync(const DescribeApisBySignatureRequest& request, const DescribeApisBySignatureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApisBySignature(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApisBySignatureOutcomeCallable CloudAPIClient::describeApisBySignatureCallable(const DescribeApisBySignatureRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApisBySignatureOutcome()>>(
			[this, request]()
			{
			return this->describeApisBySignature(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeApisByTrafficControlOutcome CloudAPIClient::describeApisByTrafficControl(const DescribeApisByTrafficControlRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeApisByTrafficControlOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeApisByTrafficControlOutcome(DescribeApisByTrafficControlResult(outcome.result()));
	else
		return DescribeApisByTrafficControlOutcome(outcome.error());
}

void CloudAPIClient::describeApisByTrafficControlAsync(const DescribeApisByTrafficControlRequest& request, const DescribeApisByTrafficControlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApisByTrafficControl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeApisByTrafficControlOutcomeCallable CloudAPIClient::describeApisByTrafficControlCallable(const DescribeApisByTrafficControlRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeApisByTrafficControlOutcome()>>(
			[this, request]()
			{
			return this->describeApisByTrafficControl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeAppOutcome CloudAPIClient::describeApp(const DescribeAppRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeAppOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeAppOutcome(DescribeAppResult(outcome.result()));
	else
		return DescribeAppOutcome(outcome.error());
}

void CloudAPIClient::describeAppAsync(const DescribeAppRequest& request, const DescribeAppAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApp(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeAppOutcomeCallable CloudAPIClient::describeAppCallable(const DescribeAppRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeAppOutcome()>>(
			[this, request]()
			{
			return this->describeApp(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeAppAttributesOutcome CloudAPIClient::describeAppAttributes(const DescribeAppAttributesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeAppAttributesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeAppAttributesOutcome(DescribeAppAttributesResult(outcome.result()));
	else
		return DescribeAppAttributesOutcome(outcome.error());
}

void CloudAPIClient::describeAppAttributesAsync(const DescribeAppAttributesRequest& request, const DescribeAppAttributesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeAppAttributes(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeAppAttributesOutcomeCallable CloudAPIClient::describeAppAttributesCallable(const DescribeAppAttributesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeAppAttributesOutcome()>>(
			[this, request]()
			{
			return this->describeAppAttributes(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeAppSecurityOutcome CloudAPIClient::describeAppSecurity(const DescribeAppSecurityRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeAppSecurityOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeAppSecurityOutcome(DescribeAppSecurityResult(outcome.result()));
	else
		return DescribeAppSecurityOutcome(outcome.error());
}

void CloudAPIClient::describeAppSecurityAsync(const DescribeAppSecurityRequest& request, const DescribeAppSecurityAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeAppSecurity(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeAppSecurityOutcomeCallable CloudAPIClient::describeAppSecurityCallable(const DescribeAppSecurityRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeAppSecurityOutcome()>>(
			[this, request]()
			{
			return this->describeAppSecurity(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeAppsOutcome CloudAPIClient::describeApps(const DescribeAppsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeAppsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeAppsOutcome(DescribeAppsResult(outcome.result()));
	else
		return DescribeAppsOutcome(outcome.error());
}

void CloudAPIClient::describeAppsAsync(const DescribeAppsRequest& request, const DescribeAppsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeApps(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeAppsOutcomeCallable CloudAPIClient::describeAppsCallable(const DescribeAppsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeAppsOutcome()>>(
			[this, request]()
			{
			return this->describeApps(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeAuthorizedApisOutcome CloudAPIClient::describeAuthorizedApis(const DescribeAuthorizedApisRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeAuthorizedApisOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeAuthorizedApisOutcome(DescribeAuthorizedApisResult(outcome.result()));
	else
		return DescribeAuthorizedApisOutcome(outcome.error());
}

void CloudAPIClient::describeAuthorizedApisAsync(const DescribeAuthorizedApisRequest& request, const DescribeAuthorizedApisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeAuthorizedApis(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeAuthorizedApisOutcomeCallable CloudAPIClient::describeAuthorizedApisCallable(const DescribeAuthorizedApisRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeAuthorizedApisOutcome()>>(
			[this, request]()
			{
			return this->describeAuthorizedApis(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeAuthorizedAppsOutcome CloudAPIClient::describeAuthorizedApps(const DescribeAuthorizedAppsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeAuthorizedAppsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeAuthorizedAppsOutcome(DescribeAuthorizedAppsResult(outcome.result()));
	else
		return DescribeAuthorizedAppsOutcome(outcome.error());
}

void CloudAPIClient::describeAuthorizedAppsAsync(const DescribeAuthorizedAppsRequest& request, const DescribeAuthorizedAppsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeAuthorizedApps(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeAuthorizedAppsOutcomeCallable CloudAPIClient::describeAuthorizedAppsCallable(const DescribeAuthorizedAppsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeAuthorizedAppsOutcome()>>(
			[this, request]()
			{
			return this->describeAuthorizedApps(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeDeployedApiOutcome CloudAPIClient::describeDeployedApi(const DescribeDeployedApiRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeDeployedApiOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeDeployedApiOutcome(DescribeDeployedApiResult(outcome.result()));
	else
		return DescribeDeployedApiOutcome(outcome.error());
}

void CloudAPIClient::describeDeployedApiAsync(const DescribeDeployedApiRequest& request, const DescribeDeployedApiAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeDeployedApi(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeDeployedApiOutcomeCallable CloudAPIClient::describeDeployedApiCallable(const DescribeDeployedApiRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeDeployedApiOutcome()>>(
			[this, request]()
			{
			return this->describeDeployedApi(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeDeployedApisOutcome CloudAPIClient::describeDeployedApis(const DescribeDeployedApisRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeDeployedApisOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeDeployedApisOutcome(DescribeDeployedApisResult(outcome.result()));
	else
		return DescribeDeployedApisOutcome(outcome.error());
}

void CloudAPIClient::describeDeployedApisAsync(const DescribeDeployedApisRequest& request, const DescribeDeployedApisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeDeployedApis(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeDeployedApisOutcomeCallable CloudAPIClient::describeDeployedApisCallable(const DescribeDeployedApisRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeDeployedApisOutcome()>>(
			[this, request]()
			{
			return this->describeDeployedApis(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeDomainOutcome CloudAPIClient::describeDomain(const DescribeDomainRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeDomainOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeDomainOutcome(DescribeDomainResult(outcome.result()));
	else
		return DescribeDomainOutcome(outcome.error());
}

void CloudAPIClient::describeDomainAsync(const DescribeDomainRequest& request, const DescribeDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeDomain(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeDomainOutcomeCallable CloudAPIClient::describeDomainCallable(const DescribeDomainRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeDomainOutcome()>>(
			[this, request]()
			{
			return this->describeDomain(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeDomainsResolutionOutcome CloudAPIClient::describeDomainsResolution(const DescribeDomainsResolutionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeDomainsResolutionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeDomainsResolutionOutcome(DescribeDomainsResolutionResult(outcome.result()));
	else
		return DescribeDomainsResolutionOutcome(outcome.error());
}

void CloudAPIClient::describeDomainsResolutionAsync(const DescribeDomainsResolutionRequest& request, const DescribeDomainsResolutionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeDomainsResolution(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeDomainsResolutionOutcomeCallable CloudAPIClient::describeDomainsResolutionCallable(const DescribeDomainsResolutionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeDomainsResolutionOutcome()>>(
			[this, request]()
			{
			return this->describeDomainsResolution(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeHistoryApisOutcome CloudAPIClient::describeHistoryApis(const DescribeHistoryApisRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeHistoryApisOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeHistoryApisOutcome(DescribeHistoryApisResult(outcome.result()));
	else
		return DescribeHistoryApisOutcome(outcome.error());
}

void CloudAPIClient::describeHistoryApisAsync(const DescribeHistoryApisRequest& request, const DescribeHistoryApisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeHistoryApis(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeHistoryApisOutcomeCallable CloudAPIClient::describeHistoryApisCallable(const DescribeHistoryApisRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeHistoryApisOutcome()>>(
			[this, request]()
			{
			return this->describeHistoryApis(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeIpControlPolicyItemsOutcome CloudAPIClient::describeIpControlPolicyItems(const DescribeIpControlPolicyItemsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeIpControlPolicyItemsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeIpControlPolicyItemsOutcome(DescribeIpControlPolicyItemsResult(outcome.result()));
	else
		return DescribeIpControlPolicyItemsOutcome(outcome.error());
}

void CloudAPIClient::describeIpControlPolicyItemsAsync(const DescribeIpControlPolicyItemsRequest& request, const DescribeIpControlPolicyItemsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeIpControlPolicyItems(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeIpControlPolicyItemsOutcomeCallable CloudAPIClient::describeIpControlPolicyItemsCallable(const DescribeIpControlPolicyItemsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeIpControlPolicyItemsOutcome()>>(
			[this, request]()
			{
			return this->describeIpControlPolicyItems(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeIpControlsOutcome CloudAPIClient::describeIpControls(const DescribeIpControlsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeIpControlsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeIpControlsOutcome(DescribeIpControlsResult(outcome.result()));
	else
		return DescribeIpControlsOutcome(outcome.error());
}

void CloudAPIClient::describeIpControlsAsync(const DescribeIpControlsRequest& request, const DescribeIpControlsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeIpControls(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeIpControlsOutcomeCallable CloudAPIClient::describeIpControlsCallable(const DescribeIpControlsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeIpControlsOutcome()>>(
			[this, request]()
			{
			return this->describeIpControls(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeLogConfigOutcome CloudAPIClient::describeLogConfig(const DescribeLogConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLogConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLogConfigOutcome(DescribeLogConfigResult(outcome.result()));
	else
		return DescribeLogConfigOutcome(outcome.error());
}

void CloudAPIClient::describeLogConfigAsync(const DescribeLogConfigRequest& request, const DescribeLogConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLogConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeLogConfigOutcomeCallable CloudAPIClient::describeLogConfigCallable(const DescribeLogConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLogConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLogConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribePurchasedApiGroupOutcome CloudAPIClient::describePurchasedApiGroup(const DescribePurchasedApiGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribePurchasedApiGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribePurchasedApiGroupOutcome(DescribePurchasedApiGroupResult(outcome.result()));
	else
		return DescribePurchasedApiGroupOutcome(outcome.error());
}

void CloudAPIClient::describePurchasedApiGroupAsync(const DescribePurchasedApiGroupRequest& request, const DescribePurchasedApiGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describePurchasedApiGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribePurchasedApiGroupOutcomeCallable CloudAPIClient::describePurchasedApiGroupCallable(const DescribePurchasedApiGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribePurchasedApiGroupOutcome()>>(
			[this, request]()
			{
			return this->describePurchasedApiGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribePurchasedApiGroupsOutcome CloudAPIClient::describePurchasedApiGroups(const DescribePurchasedApiGroupsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribePurchasedApiGroupsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribePurchasedApiGroupsOutcome(DescribePurchasedApiGroupsResult(outcome.result()));
	else
		return DescribePurchasedApiGroupsOutcome(outcome.error());
}

void CloudAPIClient::describePurchasedApiGroupsAsync(const DescribePurchasedApiGroupsRequest& request, const DescribePurchasedApiGroupsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describePurchasedApiGroups(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribePurchasedApiGroupsOutcomeCallable CloudAPIClient::describePurchasedApiGroupsCallable(const DescribePurchasedApiGroupsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribePurchasedApiGroupsOutcome()>>(
			[this, request]()
			{
			return this->describePurchasedApiGroups(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribePurchasedApisOutcome CloudAPIClient::describePurchasedApis(const DescribePurchasedApisRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribePurchasedApisOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribePurchasedApisOutcome(DescribePurchasedApisResult(outcome.result()));
	else
		return DescribePurchasedApisOutcome(outcome.error());
}

void CloudAPIClient::describePurchasedApisAsync(const DescribePurchasedApisRequest& request, const DescribePurchasedApisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describePurchasedApis(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribePurchasedApisOutcomeCallable CloudAPIClient::describePurchasedApisCallable(const DescribePurchasedApisRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribePurchasedApisOutcome()>>(
			[this, request]()
			{
			return this->describePurchasedApis(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeRegionsOutcome CloudAPIClient::describeRegions(const DescribeRegionsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeRegionsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeRegionsOutcome(DescribeRegionsResult(outcome.result()));
	else
		return DescribeRegionsOutcome(outcome.error());
}

void CloudAPIClient::describeRegionsAsync(const DescribeRegionsRequest& request, const DescribeRegionsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeRegions(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeRegionsOutcomeCallable CloudAPIClient::describeRegionsCallable(const DescribeRegionsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeRegionsOutcome()>>(
			[this, request]()
			{
			return this->describeRegions(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeSignaturesOutcome CloudAPIClient::describeSignatures(const DescribeSignaturesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeSignaturesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeSignaturesOutcome(DescribeSignaturesResult(outcome.result()));
	else
		return DescribeSignaturesOutcome(outcome.error());
}

void CloudAPIClient::describeSignaturesAsync(const DescribeSignaturesRequest& request, const DescribeSignaturesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeSignatures(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeSignaturesOutcomeCallable CloudAPIClient::describeSignaturesCallable(const DescribeSignaturesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeSignaturesOutcome()>>(
			[this, request]()
			{
			return this->describeSignatures(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeSignaturesByApiOutcome CloudAPIClient::describeSignaturesByApi(const DescribeSignaturesByApiRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeSignaturesByApiOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeSignaturesByApiOutcome(DescribeSignaturesByApiResult(outcome.result()));
	else
		return DescribeSignaturesByApiOutcome(outcome.error());
}

void CloudAPIClient::describeSignaturesByApiAsync(const DescribeSignaturesByApiRequest& request, const DescribeSignaturesByApiAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeSignaturesByApi(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeSignaturesByApiOutcomeCallable CloudAPIClient::describeSignaturesByApiCallable(const DescribeSignaturesByApiRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeSignaturesByApiOutcome()>>(
			[this, request]()
			{
			return this->describeSignaturesByApi(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeSystemParametersOutcome CloudAPIClient::describeSystemParameters(const DescribeSystemParametersRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeSystemParametersOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeSystemParametersOutcome(DescribeSystemParametersResult(outcome.result()));
	else
		return DescribeSystemParametersOutcome(outcome.error());
}

void CloudAPIClient::describeSystemParametersAsync(const DescribeSystemParametersRequest& request, const DescribeSystemParametersAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeSystemParameters(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeSystemParametersOutcomeCallable CloudAPIClient::describeSystemParametersCallable(const DescribeSystemParametersRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeSystemParametersOutcome()>>(
			[this, request]()
			{
			return this->describeSystemParameters(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeTrafficControlsOutcome CloudAPIClient::describeTrafficControls(const DescribeTrafficControlsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeTrafficControlsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeTrafficControlsOutcome(DescribeTrafficControlsResult(outcome.result()));
	else
		return DescribeTrafficControlsOutcome(outcome.error());
}

void CloudAPIClient::describeTrafficControlsAsync(const DescribeTrafficControlsRequest& request, const DescribeTrafficControlsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeTrafficControls(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeTrafficControlsOutcomeCallable CloudAPIClient::describeTrafficControlsCallable(const DescribeTrafficControlsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeTrafficControlsOutcome()>>(
			[this, request]()
			{
			return this->describeTrafficControls(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeTrafficControlsByApiOutcome CloudAPIClient::describeTrafficControlsByApi(const DescribeTrafficControlsByApiRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeTrafficControlsByApiOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeTrafficControlsByApiOutcome(DescribeTrafficControlsByApiResult(outcome.result()));
	else
		return DescribeTrafficControlsByApiOutcome(outcome.error());
}

void CloudAPIClient::describeTrafficControlsByApiAsync(const DescribeTrafficControlsByApiRequest& request, const DescribeTrafficControlsByApiAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeTrafficControlsByApi(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeTrafficControlsByApiOutcomeCallable CloudAPIClient::describeTrafficControlsByApiCallable(const DescribeTrafficControlsByApiRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeTrafficControlsByApiOutcome()>>(
			[this, request]()
			{
			return this->describeTrafficControlsByApi(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::DescribeVpcAccessesOutcome CloudAPIClient::describeVpcAccesses(const DescribeVpcAccessesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeVpcAccessesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeVpcAccessesOutcome(DescribeVpcAccessesResult(outcome.result()));
	else
		return DescribeVpcAccessesOutcome(outcome.error());
}

void CloudAPIClient::describeVpcAccessesAsync(const DescribeVpcAccessesRequest& request, const DescribeVpcAccessesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeVpcAccesses(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::DescribeVpcAccessesOutcomeCallable CloudAPIClient::describeVpcAccessesCallable(const DescribeVpcAccessesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeVpcAccessesOutcome()>>(
			[this, request]()
			{
			return this->describeVpcAccesses(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ImportSwaggerOutcome CloudAPIClient::importSwagger(const ImportSwaggerRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ImportSwaggerOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ImportSwaggerOutcome(ImportSwaggerResult(outcome.result()));
	else
		return ImportSwaggerOutcome(outcome.error());
}

void CloudAPIClient::importSwaggerAsync(const ImportSwaggerRequest& request, const ImportSwaggerAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, importSwagger(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ImportSwaggerOutcomeCallable CloudAPIClient::importSwaggerCallable(const ImportSwaggerRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ImportSwaggerOutcome()>>(
			[this, request]()
			{
			return this->importSwagger(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ListTagResourcesOutcome CloudAPIClient::listTagResources(const ListTagResourcesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListTagResourcesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListTagResourcesOutcome(ListTagResourcesResult(outcome.result()));
	else
		return ListTagResourcesOutcome(outcome.error());
}

void CloudAPIClient::listTagResourcesAsync(const ListTagResourcesRequest& request, const ListTagResourcesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listTagResources(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ListTagResourcesOutcomeCallable CloudAPIClient::listTagResourcesCallable(const ListTagResourcesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListTagResourcesOutcome()>>(
			[this, request]()
			{
			return this->listTagResources(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ModifyApiOutcome CloudAPIClient::modifyApi(const ModifyApiRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyApiOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyApiOutcome(ModifyApiResult(outcome.result()));
	else
		return ModifyApiOutcome(outcome.error());
}

void CloudAPIClient::modifyApiAsync(const ModifyApiRequest& request, const ModifyApiAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyApi(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ModifyApiOutcomeCallable CloudAPIClient::modifyApiCallable(const ModifyApiRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyApiOutcome()>>(
			[this, request]()
			{
			return this->modifyApi(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ModifyApiGroupOutcome CloudAPIClient::modifyApiGroup(const ModifyApiGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyApiGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyApiGroupOutcome(ModifyApiGroupResult(outcome.result()));
	else
		return ModifyApiGroupOutcome(outcome.error());
}

void CloudAPIClient::modifyApiGroupAsync(const ModifyApiGroupRequest& request, const ModifyApiGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyApiGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ModifyApiGroupOutcomeCallable CloudAPIClient::modifyApiGroupCallable(const ModifyApiGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyApiGroupOutcome()>>(
			[this, request]()
			{
			return this->modifyApiGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ModifyApiGroupVpcWhitelistOutcome CloudAPIClient::modifyApiGroupVpcWhitelist(const ModifyApiGroupVpcWhitelistRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyApiGroupVpcWhitelistOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyApiGroupVpcWhitelistOutcome(ModifyApiGroupVpcWhitelistResult(outcome.result()));
	else
		return ModifyApiGroupVpcWhitelistOutcome(outcome.error());
}

void CloudAPIClient::modifyApiGroupVpcWhitelistAsync(const ModifyApiGroupVpcWhitelistRequest& request, const ModifyApiGroupVpcWhitelistAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyApiGroupVpcWhitelist(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ModifyApiGroupVpcWhitelistOutcomeCallable CloudAPIClient::modifyApiGroupVpcWhitelistCallable(const ModifyApiGroupVpcWhitelistRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyApiGroupVpcWhitelistOutcome()>>(
			[this, request]()
			{
			return this->modifyApiGroupVpcWhitelist(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ModifyApiMarketAttributesOutcome CloudAPIClient::modifyApiMarketAttributes(const ModifyApiMarketAttributesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyApiMarketAttributesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyApiMarketAttributesOutcome(ModifyApiMarketAttributesResult(outcome.result()));
	else
		return ModifyApiMarketAttributesOutcome(outcome.error());
}

void CloudAPIClient::modifyApiMarketAttributesAsync(const ModifyApiMarketAttributesRequest& request, const ModifyApiMarketAttributesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyApiMarketAttributes(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ModifyApiMarketAttributesOutcomeCallable CloudAPIClient::modifyApiMarketAttributesCallable(const ModifyApiMarketAttributesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyApiMarketAttributesOutcome()>>(
			[this, request]()
			{
			return this->modifyApiMarketAttributes(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ModifyAppOutcome CloudAPIClient::modifyApp(const ModifyAppRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyAppOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyAppOutcome(ModifyAppResult(outcome.result()));
	else
		return ModifyAppOutcome(outcome.error());
}

void CloudAPIClient::modifyAppAsync(const ModifyAppRequest& request, const ModifyAppAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyApp(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ModifyAppOutcomeCallable CloudAPIClient::modifyAppCallable(const ModifyAppRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyAppOutcome()>>(
			[this, request]()
			{
			return this->modifyApp(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ModifyIpControlOutcome CloudAPIClient::modifyIpControl(const ModifyIpControlRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyIpControlOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyIpControlOutcome(ModifyIpControlResult(outcome.result()));
	else
		return ModifyIpControlOutcome(outcome.error());
}

void CloudAPIClient::modifyIpControlAsync(const ModifyIpControlRequest& request, const ModifyIpControlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyIpControl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ModifyIpControlOutcomeCallable CloudAPIClient::modifyIpControlCallable(const ModifyIpControlRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyIpControlOutcome()>>(
			[this, request]()
			{
			return this->modifyIpControl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ModifyIpControlPolicyItemOutcome CloudAPIClient::modifyIpControlPolicyItem(const ModifyIpControlPolicyItemRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyIpControlPolicyItemOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyIpControlPolicyItemOutcome(ModifyIpControlPolicyItemResult(outcome.result()));
	else
		return ModifyIpControlPolicyItemOutcome(outcome.error());
}

void CloudAPIClient::modifyIpControlPolicyItemAsync(const ModifyIpControlPolicyItemRequest& request, const ModifyIpControlPolicyItemAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyIpControlPolicyItem(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ModifyIpControlPolicyItemOutcomeCallable CloudAPIClient::modifyIpControlPolicyItemCallable(const ModifyIpControlPolicyItemRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyIpControlPolicyItemOutcome()>>(
			[this, request]()
			{
			return this->modifyIpControlPolicyItem(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ModifyLogConfigOutcome CloudAPIClient::modifyLogConfig(const ModifyLogConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyLogConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyLogConfigOutcome(ModifyLogConfigResult(outcome.result()));
	else
		return ModifyLogConfigOutcome(outcome.error());
}

void CloudAPIClient::modifyLogConfigAsync(const ModifyLogConfigRequest& request, const ModifyLogConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyLogConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ModifyLogConfigOutcomeCallable CloudAPIClient::modifyLogConfigCallable(const ModifyLogConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyLogConfigOutcome()>>(
			[this, request]()
			{
			return this->modifyLogConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ModifySignatureOutcome CloudAPIClient::modifySignature(const ModifySignatureRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifySignatureOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifySignatureOutcome(ModifySignatureResult(outcome.result()));
	else
		return ModifySignatureOutcome(outcome.error());
}

void CloudAPIClient::modifySignatureAsync(const ModifySignatureRequest& request, const ModifySignatureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifySignature(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ModifySignatureOutcomeCallable CloudAPIClient::modifySignatureCallable(const ModifySignatureRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifySignatureOutcome()>>(
			[this, request]()
			{
			return this->modifySignature(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ModifyTrafficControlOutcome CloudAPIClient::modifyTrafficControl(const ModifyTrafficControlRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyTrafficControlOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyTrafficControlOutcome(ModifyTrafficControlResult(outcome.result()));
	else
		return ModifyTrafficControlOutcome(outcome.error());
}

void CloudAPIClient::modifyTrafficControlAsync(const ModifyTrafficControlRequest& request, const ModifyTrafficControlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyTrafficControl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ModifyTrafficControlOutcomeCallable CloudAPIClient::modifyTrafficControlCallable(const ModifyTrafficControlRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyTrafficControlOutcome()>>(
			[this, request]()
			{
			return this->modifyTrafficControl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ReactivateDomainOutcome CloudAPIClient::reactivateDomain(const ReactivateDomainRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ReactivateDomainOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ReactivateDomainOutcome(ReactivateDomainResult(outcome.result()));
	else
		return ReactivateDomainOutcome(outcome.error());
}

void CloudAPIClient::reactivateDomainAsync(const ReactivateDomainRequest& request, const ReactivateDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, reactivateDomain(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ReactivateDomainOutcomeCallable CloudAPIClient::reactivateDomainCallable(const ReactivateDomainRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ReactivateDomainOutcome()>>(
			[this, request]()
			{
			return this->reactivateDomain(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::RemoveApisAuthoritiesOutcome CloudAPIClient::removeApisAuthorities(const RemoveApisAuthoritiesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveApisAuthoritiesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveApisAuthoritiesOutcome(RemoveApisAuthoritiesResult(outcome.result()));
	else
		return RemoveApisAuthoritiesOutcome(outcome.error());
}

void CloudAPIClient::removeApisAuthoritiesAsync(const RemoveApisAuthoritiesRequest& request, const RemoveApisAuthoritiesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeApisAuthorities(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::RemoveApisAuthoritiesOutcomeCallable CloudAPIClient::removeApisAuthoritiesCallable(const RemoveApisAuthoritiesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveApisAuthoritiesOutcome()>>(
			[this, request]()
			{
			return this->removeApisAuthorities(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::RemoveAppsAuthoritiesOutcome CloudAPIClient::removeAppsAuthorities(const RemoveAppsAuthoritiesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveAppsAuthoritiesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveAppsAuthoritiesOutcome(RemoveAppsAuthoritiesResult(outcome.result()));
	else
		return RemoveAppsAuthoritiesOutcome(outcome.error());
}

void CloudAPIClient::removeAppsAuthoritiesAsync(const RemoveAppsAuthoritiesRequest& request, const RemoveAppsAuthoritiesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeAppsAuthorities(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::RemoveAppsAuthoritiesOutcomeCallable CloudAPIClient::removeAppsAuthoritiesCallable(const RemoveAppsAuthoritiesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveAppsAuthoritiesOutcome()>>(
			[this, request]()
			{
			return this->removeAppsAuthorities(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::RemoveIpControlApisOutcome CloudAPIClient::removeIpControlApis(const RemoveIpControlApisRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveIpControlApisOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveIpControlApisOutcome(RemoveIpControlApisResult(outcome.result()));
	else
		return RemoveIpControlApisOutcome(outcome.error());
}

void CloudAPIClient::removeIpControlApisAsync(const RemoveIpControlApisRequest& request, const RemoveIpControlApisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeIpControlApis(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::RemoveIpControlApisOutcomeCallable CloudAPIClient::removeIpControlApisCallable(const RemoveIpControlApisRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveIpControlApisOutcome()>>(
			[this, request]()
			{
			return this->removeIpControlApis(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::RemoveIpControlPolicyItemOutcome CloudAPIClient::removeIpControlPolicyItem(const RemoveIpControlPolicyItemRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveIpControlPolicyItemOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveIpControlPolicyItemOutcome(RemoveIpControlPolicyItemResult(outcome.result()));
	else
		return RemoveIpControlPolicyItemOutcome(outcome.error());
}

void CloudAPIClient::removeIpControlPolicyItemAsync(const RemoveIpControlPolicyItemRequest& request, const RemoveIpControlPolicyItemAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeIpControlPolicyItem(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::RemoveIpControlPolicyItemOutcomeCallable CloudAPIClient::removeIpControlPolicyItemCallable(const RemoveIpControlPolicyItemRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveIpControlPolicyItemOutcome()>>(
			[this, request]()
			{
			return this->removeIpControlPolicyItem(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::RemoveSignatureApisOutcome CloudAPIClient::removeSignatureApis(const RemoveSignatureApisRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveSignatureApisOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveSignatureApisOutcome(RemoveSignatureApisResult(outcome.result()));
	else
		return RemoveSignatureApisOutcome(outcome.error());
}

void CloudAPIClient::removeSignatureApisAsync(const RemoveSignatureApisRequest& request, const RemoveSignatureApisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeSignatureApis(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::RemoveSignatureApisOutcomeCallable CloudAPIClient::removeSignatureApisCallable(const RemoveSignatureApisRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveSignatureApisOutcome()>>(
			[this, request]()
			{
			return this->removeSignatureApis(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::RemoveTrafficControlApisOutcome CloudAPIClient::removeTrafficControlApis(const RemoveTrafficControlApisRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveTrafficControlApisOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveTrafficControlApisOutcome(RemoveTrafficControlApisResult(outcome.result()));
	else
		return RemoveTrafficControlApisOutcome(outcome.error());
}

void CloudAPIClient::removeTrafficControlApisAsync(const RemoveTrafficControlApisRequest& request, const RemoveTrafficControlApisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeTrafficControlApis(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::RemoveTrafficControlApisOutcomeCallable CloudAPIClient::removeTrafficControlApisCallable(const RemoveTrafficControlApisRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveTrafficControlApisOutcome()>>(
			[this, request]()
			{
			return this->removeTrafficControlApis(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::RemoveVpcAccessOutcome CloudAPIClient::removeVpcAccess(const RemoveVpcAccessRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveVpcAccessOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveVpcAccessOutcome(RemoveVpcAccessResult(outcome.result()));
	else
		return RemoveVpcAccessOutcome(outcome.error());
}

void CloudAPIClient::removeVpcAccessAsync(const RemoveVpcAccessRequest& request, const RemoveVpcAccessAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeVpcAccess(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::RemoveVpcAccessOutcomeCallable CloudAPIClient::removeVpcAccessCallable(const RemoveVpcAccessRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveVpcAccessOutcome()>>(
			[this, request]()
			{
			return this->removeVpcAccess(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ResetAppCodeOutcome CloudAPIClient::resetAppCode(const ResetAppCodeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ResetAppCodeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ResetAppCodeOutcome(ResetAppCodeResult(outcome.result()));
	else
		return ResetAppCodeOutcome(outcome.error());
}

void CloudAPIClient::resetAppCodeAsync(const ResetAppCodeRequest& request, const ResetAppCodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, resetAppCode(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ResetAppCodeOutcomeCallable CloudAPIClient::resetAppCodeCallable(const ResetAppCodeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ResetAppCodeOutcome()>>(
			[this, request]()
			{
			return this->resetAppCode(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::ResetAppSecretOutcome CloudAPIClient::resetAppSecret(const ResetAppSecretRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ResetAppSecretOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ResetAppSecretOutcome(ResetAppSecretResult(outcome.result()));
	else
		return ResetAppSecretOutcome(outcome.error());
}

void CloudAPIClient::resetAppSecretAsync(const ResetAppSecretRequest& request, const ResetAppSecretAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, resetAppSecret(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::ResetAppSecretOutcomeCallable CloudAPIClient::resetAppSecretCallable(const ResetAppSecretRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ResetAppSecretOutcome()>>(
			[this, request]()
			{
			return this->resetAppSecret(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::SdkGenerateByAppOutcome CloudAPIClient::sdkGenerateByApp(const SdkGenerateByAppRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SdkGenerateByAppOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SdkGenerateByAppOutcome(SdkGenerateByAppResult(outcome.result()));
	else
		return SdkGenerateByAppOutcome(outcome.error());
}

void CloudAPIClient::sdkGenerateByAppAsync(const SdkGenerateByAppRequest& request, const SdkGenerateByAppAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, sdkGenerateByApp(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::SdkGenerateByAppOutcomeCallable CloudAPIClient::sdkGenerateByAppCallable(const SdkGenerateByAppRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SdkGenerateByAppOutcome()>>(
			[this, request]()
			{
			return this->sdkGenerateByApp(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::SdkGenerateByGroupOutcome CloudAPIClient::sdkGenerateByGroup(const SdkGenerateByGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SdkGenerateByGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SdkGenerateByGroupOutcome(SdkGenerateByGroupResult(outcome.result()));
	else
		return SdkGenerateByGroupOutcome(outcome.error());
}

void CloudAPIClient::sdkGenerateByGroupAsync(const SdkGenerateByGroupRequest& request, const SdkGenerateByGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, sdkGenerateByGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::SdkGenerateByGroupOutcomeCallable CloudAPIClient::sdkGenerateByGroupCallable(const SdkGenerateByGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SdkGenerateByGroupOutcome()>>(
			[this, request]()
			{
			return this->sdkGenerateByGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::SetApisAuthoritiesOutcome CloudAPIClient::setApisAuthorities(const SetApisAuthoritiesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetApisAuthoritiesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetApisAuthoritiesOutcome(SetApisAuthoritiesResult(outcome.result()));
	else
		return SetApisAuthoritiesOutcome(outcome.error());
}

void CloudAPIClient::setApisAuthoritiesAsync(const SetApisAuthoritiesRequest& request, const SetApisAuthoritiesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setApisAuthorities(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::SetApisAuthoritiesOutcomeCallable CloudAPIClient::setApisAuthoritiesCallable(const SetApisAuthoritiesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetApisAuthoritiesOutcome()>>(
			[this, request]()
			{
			return this->setApisAuthorities(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::SetAppsAuthoritiesOutcome CloudAPIClient::setAppsAuthorities(const SetAppsAuthoritiesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetAppsAuthoritiesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetAppsAuthoritiesOutcome(SetAppsAuthoritiesResult(outcome.result()));
	else
		return SetAppsAuthoritiesOutcome(outcome.error());
}

void CloudAPIClient::setAppsAuthoritiesAsync(const SetAppsAuthoritiesRequest& request, const SetAppsAuthoritiesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setAppsAuthorities(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::SetAppsAuthoritiesOutcomeCallable CloudAPIClient::setAppsAuthoritiesCallable(const SetAppsAuthoritiesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetAppsAuthoritiesOutcome()>>(
			[this, request]()
			{
			return this->setAppsAuthorities(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::SetDomainOutcome CloudAPIClient::setDomain(const SetDomainRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetDomainOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetDomainOutcome(SetDomainResult(outcome.result()));
	else
		return SetDomainOutcome(outcome.error());
}

void CloudAPIClient::setDomainAsync(const SetDomainRequest& request, const SetDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setDomain(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::SetDomainOutcomeCallable CloudAPIClient::setDomainCallable(const SetDomainRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetDomainOutcome()>>(
			[this, request]()
			{
			return this->setDomain(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::SetDomainCertificateOutcome CloudAPIClient::setDomainCertificate(const SetDomainCertificateRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetDomainCertificateOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetDomainCertificateOutcome(SetDomainCertificateResult(outcome.result()));
	else
		return SetDomainCertificateOutcome(outcome.error());
}

void CloudAPIClient::setDomainCertificateAsync(const SetDomainCertificateRequest& request, const SetDomainCertificateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setDomainCertificate(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::SetDomainCertificateOutcomeCallable CloudAPIClient::setDomainCertificateCallable(const SetDomainCertificateRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetDomainCertificateOutcome()>>(
			[this, request]()
			{
			return this->setDomainCertificate(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::SetDomainWebSocketStatusOutcome CloudAPIClient::setDomainWebSocketStatus(const SetDomainWebSocketStatusRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetDomainWebSocketStatusOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetDomainWebSocketStatusOutcome(SetDomainWebSocketStatusResult(outcome.result()));
	else
		return SetDomainWebSocketStatusOutcome(outcome.error());
}

void CloudAPIClient::setDomainWebSocketStatusAsync(const SetDomainWebSocketStatusRequest& request, const SetDomainWebSocketStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setDomainWebSocketStatus(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::SetDomainWebSocketStatusOutcomeCallable CloudAPIClient::setDomainWebSocketStatusCallable(const SetDomainWebSocketStatusRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetDomainWebSocketStatusOutcome()>>(
			[this, request]()
			{
			return this->setDomainWebSocketStatus(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::SetIpControlApisOutcome CloudAPIClient::setIpControlApis(const SetIpControlApisRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetIpControlApisOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetIpControlApisOutcome(SetIpControlApisResult(outcome.result()));
	else
		return SetIpControlApisOutcome(outcome.error());
}

void CloudAPIClient::setIpControlApisAsync(const SetIpControlApisRequest& request, const SetIpControlApisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setIpControlApis(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::SetIpControlApisOutcomeCallable CloudAPIClient::setIpControlApisCallable(const SetIpControlApisRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetIpControlApisOutcome()>>(
			[this, request]()
			{
			return this->setIpControlApis(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::SetSignatureApisOutcome CloudAPIClient::setSignatureApis(const SetSignatureApisRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetSignatureApisOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetSignatureApisOutcome(SetSignatureApisResult(outcome.result()));
	else
		return SetSignatureApisOutcome(outcome.error());
}

void CloudAPIClient::setSignatureApisAsync(const SetSignatureApisRequest& request, const SetSignatureApisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setSignatureApis(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::SetSignatureApisOutcomeCallable CloudAPIClient::setSignatureApisCallable(const SetSignatureApisRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetSignatureApisOutcome()>>(
			[this, request]()
			{
			return this->setSignatureApis(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::SetTrafficControlApisOutcome CloudAPIClient::setTrafficControlApis(const SetTrafficControlApisRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetTrafficControlApisOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetTrafficControlApisOutcome(SetTrafficControlApisResult(outcome.result()));
	else
		return SetTrafficControlApisOutcome(outcome.error());
}

void CloudAPIClient::setTrafficControlApisAsync(const SetTrafficControlApisRequest& request, const SetTrafficControlApisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setTrafficControlApis(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::SetTrafficControlApisOutcomeCallable CloudAPIClient::setTrafficControlApisCallable(const SetTrafficControlApisRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetTrafficControlApisOutcome()>>(
			[this, request]()
			{
			return this->setTrafficControlApis(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::SetVpcAccessOutcome CloudAPIClient::setVpcAccess(const SetVpcAccessRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetVpcAccessOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetVpcAccessOutcome(SetVpcAccessResult(outcome.result()));
	else
		return SetVpcAccessOutcome(outcome.error());
}

void CloudAPIClient::setVpcAccessAsync(const SetVpcAccessRequest& request, const SetVpcAccessAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setVpcAccess(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::SetVpcAccessOutcomeCallable CloudAPIClient::setVpcAccessCallable(const SetVpcAccessRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetVpcAccessOutcome()>>(
			[this, request]()
			{
			return this->setVpcAccess(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::SetWildcardDomainPatternsOutcome CloudAPIClient::setWildcardDomainPatterns(const SetWildcardDomainPatternsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetWildcardDomainPatternsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetWildcardDomainPatternsOutcome(SetWildcardDomainPatternsResult(outcome.result()));
	else
		return SetWildcardDomainPatternsOutcome(outcome.error());
}

void CloudAPIClient::setWildcardDomainPatternsAsync(const SetWildcardDomainPatternsRequest& request, const SetWildcardDomainPatternsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setWildcardDomainPatterns(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::SetWildcardDomainPatternsOutcomeCallable CloudAPIClient::setWildcardDomainPatternsCallable(const SetWildcardDomainPatternsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetWildcardDomainPatternsOutcome()>>(
			[this, request]()
			{
			return this->setWildcardDomainPatterns(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::SwitchApiOutcome CloudAPIClient::switchApi(const SwitchApiRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SwitchApiOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SwitchApiOutcome(SwitchApiResult(outcome.result()));
	else
		return SwitchApiOutcome(outcome.error());
}

void CloudAPIClient::switchApiAsync(const SwitchApiRequest& request, const SwitchApiAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, switchApi(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::SwitchApiOutcomeCallable CloudAPIClient::switchApiCallable(const SwitchApiRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SwitchApiOutcome()>>(
			[this, request]()
			{
			return this->switchApi(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::TagResourcesOutcome CloudAPIClient::tagResources(const TagResourcesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return TagResourcesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return TagResourcesOutcome(TagResourcesResult(outcome.result()));
	else
		return TagResourcesOutcome(outcome.error());
}

void CloudAPIClient::tagResourcesAsync(const TagResourcesRequest& request, const TagResourcesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, tagResources(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::TagResourcesOutcomeCallable CloudAPIClient::tagResourcesCallable(const TagResourcesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<TagResourcesOutcome()>>(
			[this, request]()
			{
			return this->tagResources(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudAPIClient::UntagResourcesOutcome CloudAPIClient::untagResources(const UntagResourcesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UntagResourcesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UntagResourcesOutcome(UntagResourcesResult(outcome.result()));
	else
		return UntagResourcesOutcome(outcome.error());
}

void CloudAPIClient::untagResourcesAsync(const UntagResourcesRequest& request, const UntagResourcesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, untagResources(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudAPIClient::UntagResourcesOutcomeCallable CloudAPIClient::untagResourcesCallable(const UntagResourcesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UntagResourcesOutcome()>>(
			[this, request]()
			{
			return this->untagResources(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


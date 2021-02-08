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

#include <alibabacloud/eas/EasClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Eas;
using namespace AlibabaCloud::Eas::Model;

namespace
{
	const std::string SERVICE_NAME = "eas";
}

EasClient::EasClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RoaServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "");
}

EasClient::EasClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RoaServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "");
}

EasClient::EasClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RoaServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "");
}

EasClient::~EasClient()
{}

EasClient::CheckServiceExistsOutcome EasClient::checkServiceExists(const CheckServiceExistsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CheckServiceExistsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CheckServiceExistsOutcome(CheckServiceExistsResult(outcome.result()));
	else
		return CheckServiceExistsOutcome(outcome.error());
}

void EasClient::checkServiceExistsAsync(const CheckServiceExistsRequest& request, const CheckServiceExistsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, checkServiceExists(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::CheckServiceExistsOutcomeCallable EasClient::checkServiceExistsCallable(const CheckServiceExistsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CheckServiceExistsOutcome()>>(
			[this, request]()
			{
			return this->checkServiceExists(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::CreateNetworkInterfaceOutcome EasClient::createNetworkInterface(const CreateNetworkInterfaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateNetworkInterfaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateNetworkInterfaceOutcome(CreateNetworkInterfaceResult(outcome.result()));
	else
		return CreateNetworkInterfaceOutcome(outcome.error());
}

void EasClient::createNetworkInterfaceAsync(const CreateNetworkInterfaceRequest& request, const CreateNetworkInterfaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createNetworkInterface(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::CreateNetworkInterfaceOutcomeCallable EasClient::createNetworkInterfaceCallable(const CreateNetworkInterfaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateNetworkInterfaceOutcome()>>(
			[this, request]()
			{
			return this->createNetworkInterface(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::CreateSLSOutcome EasClient::createSLS(const CreateSLSRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateSLSOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateSLSOutcome(CreateSLSResult(outcome.result()));
	else
		return CreateSLSOutcome(outcome.error());
}

void EasClient::createSLSAsync(const CreateSLSRequest& request, const CreateSLSAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createSLS(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::CreateSLSOutcomeCallable EasClient::createSLSCallable(const CreateSLSRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateSLSOutcome()>>(
			[this, request]()
			{
			return this->createSLS(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::CreateServiceOutcome EasClient::createService(const CreateServiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateServiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateServiceOutcome(CreateServiceResult(outcome.result()));
	else
		return CreateServiceOutcome(outcome.error());
}

void EasClient::createServiceAsync(const CreateServiceRequest& request, const CreateServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createService(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::CreateServiceOutcomeCallable EasClient::createServiceCallable(const CreateServiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateServiceOutcome()>>(
			[this, request]()
			{
			return this->createService(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::CreateTaskOutcome EasClient::createTask(const CreateTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateTaskOutcome(CreateTaskResult(outcome.result()));
	else
		return CreateTaskOutcome(outcome.error());
}

void EasClient::createTaskAsync(const CreateTaskRequest& request, const CreateTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::CreateTaskOutcomeCallable EasClient::createTaskCallable(const CreateTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateTaskOutcome()>>(
			[this, request]()
			{
			return this->createTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::DeleteResourceOutcome EasClient::deleteResource(const DeleteResourceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteResourceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteResourceOutcome(DeleteResourceResult(outcome.result()));
	else
		return DeleteResourceOutcome(outcome.error());
}

void EasClient::deleteResourceAsync(const DeleteResourceRequest& request, const DeleteResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteResource(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::DeleteResourceOutcomeCallable EasClient::deleteResourceCallable(const DeleteResourceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteResourceOutcome()>>(
			[this, request]()
			{
			return this->deleteResource(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::DeleteResourceInstanceOutcome EasClient::deleteResourceInstance(const DeleteResourceInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteResourceInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteResourceInstanceOutcome(DeleteResourceInstanceResult(outcome.result()));
	else
		return DeleteResourceInstanceOutcome(outcome.error());
}

void EasClient::deleteResourceInstanceAsync(const DeleteResourceInstanceRequest& request, const DeleteResourceInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteResourceInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::DeleteResourceInstanceOutcomeCallable EasClient::deleteResourceInstanceCallable(const DeleteResourceInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteResourceInstanceOutcome()>>(
			[this, request]()
			{
			return this->deleteResourceInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::DeleteServiceOutcome EasClient::deleteService(const DeleteServiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteServiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteServiceOutcome(DeleteServiceResult(outcome.result()));
	else
		return DeleteServiceOutcome(outcome.error());
}

void EasClient::deleteServiceAsync(const DeleteServiceRequest& request, const DeleteServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteService(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::DeleteServiceOutcomeCallable EasClient::deleteServiceCallable(const DeleteServiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteServiceOutcome()>>(
			[this, request]()
			{
			return this->deleteService(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::DeleteTaskOutcome EasClient::deleteTask(const DeleteTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteTaskOutcome(DeleteTaskResult(outcome.result()));
	else
		return DeleteTaskOutcome(outcome.error());
}

void EasClient::deleteTaskAsync(const DeleteTaskRequest& request, const DeleteTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::DeleteTaskOutcomeCallable EasClient::deleteTaskCallable(const DeleteTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteTaskOutcome()>>(
			[this, request]()
			{
			return this->deleteTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetAvailableRegionsOutcome EasClient::getAvailableRegions(const GetAvailableRegionsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetAvailableRegionsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetAvailableRegionsOutcome(GetAvailableRegionsResult(outcome.result()));
	else
		return GetAvailableRegionsOutcome(outcome.error());
}

void EasClient::getAvailableRegionsAsync(const GetAvailableRegionsRequest& request, const GetAvailableRegionsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getAvailableRegions(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetAvailableRegionsOutcomeCallable EasClient::getAvailableRegionsCallable(const GetAvailableRegionsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetAvailableRegionsOutcome()>>(
			[this, request]()
			{
			return this->getAvailableRegions(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetNetworkInterfaceOutcome EasClient::getNetworkInterface(const GetNetworkInterfaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetNetworkInterfaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetNetworkInterfaceOutcome(GetNetworkInterfaceResult(outcome.result()));
	else
		return GetNetworkInterfaceOutcome(outcome.error());
}

void EasClient::getNetworkInterfaceAsync(const GetNetworkInterfaceRequest& request, const GetNetworkInterfaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getNetworkInterface(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetNetworkInterfaceOutcomeCallable EasClient::getNetworkInterfaceCallable(const GetNetworkInterfaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetNetworkInterfaceOutcome()>>(
			[this, request]()
			{
			return this->getNetworkInterface(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetOrCreateResourceOutcome EasClient::getOrCreateResource(const GetOrCreateResourceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetOrCreateResourceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetOrCreateResourceOutcome(GetOrCreateResourceResult(outcome.result()));
	else
		return GetOrCreateResourceOutcome(outcome.error());
}

void EasClient::getOrCreateResourceAsync(const GetOrCreateResourceRequest& request, const GetOrCreateResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getOrCreateResource(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetOrCreateResourceOutcomeCallable EasClient::getOrCreateResourceCallable(const GetOrCreateResourceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetOrCreateResourceOutcome()>>(
			[this, request]()
			{
			return this->getOrCreateResource(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetOssAuthorizeOutcome EasClient::getOssAuthorize(const GetOssAuthorizeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetOssAuthorizeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetOssAuthorizeOutcome(GetOssAuthorizeResult(outcome.result()));
	else
		return GetOssAuthorizeOutcome(outcome.error());
}

void EasClient::getOssAuthorizeAsync(const GetOssAuthorizeRequest& request, const GetOssAuthorizeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getOssAuthorize(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetOssAuthorizeOutcomeCallable EasClient::getOssAuthorizeCallable(const GetOssAuthorizeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetOssAuthorizeOutcome()>>(
			[this, request]()
			{
			return this->getOssAuthorize(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetResourceInstancesOutcome EasClient::getResourceInstances(const GetResourceInstancesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetResourceInstancesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetResourceInstancesOutcome(GetResourceInstancesResult(outcome.result()));
	else
		return GetResourceInstancesOutcome(outcome.error());
}

void EasClient::getResourceInstancesAsync(const GetResourceInstancesRequest& request, const GetResourceInstancesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getResourceInstances(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetResourceInstancesOutcomeCallable EasClient::getResourceInstancesCallable(const GetResourceInstancesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetResourceInstancesOutcome()>>(
			[this, request]()
			{
			return this->getResourceInstances(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetResourcePodsOutcome EasClient::getResourcePods(const GetResourcePodsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetResourcePodsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetResourcePodsOutcome(GetResourcePodsResult(outcome.result()));
	else
		return GetResourcePodsOutcome(outcome.error());
}

void EasClient::getResourcePodsAsync(const GetResourcePodsRequest& request, const GetResourcePodsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getResourcePods(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetResourcePodsOutcomeCallable EasClient::getResourcePodsCallable(const GetResourcePodsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetResourcePodsOutcome()>>(
			[this, request]()
			{
			return this->getResourcePods(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetResourceServicesOutcome EasClient::getResourceServices(const GetResourceServicesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetResourceServicesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetResourceServicesOutcome(GetResourceServicesResult(outcome.result()));
	else
		return GetResourceServicesOutcome(outcome.error());
}

void EasClient::getResourceServicesAsync(const GetResourceServicesRequest& request, const GetResourceServicesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getResourceServices(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetResourceServicesOutcomeCallable EasClient::getResourceServicesCallable(const GetResourceServicesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetResourceServicesOutcome()>>(
			[this, request]()
			{
			return this->getResourceServices(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetResourceStatusOutcome EasClient::getResourceStatus(const GetResourceStatusRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetResourceStatusOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetResourceStatusOutcome(GetResourceStatusResult(outcome.result()));
	else
		return GetResourceStatusOutcome(outcome.error());
}

void EasClient::getResourceStatusAsync(const GetResourceStatusRequest& request, const GetResourceStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getResourceStatus(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetResourceStatusOutcomeCallable EasClient::getResourceStatusCallable(const GetResourceStatusRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetResourceStatusOutcome()>>(
			[this, request]()
			{
			return this->getResourceStatus(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetServiceOutcome EasClient::getService(const GetServiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetServiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetServiceOutcome(GetServiceResult(outcome.result()));
	else
		return GetServiceOutcome(outcome.error());
}

void EasClient::getServiceAsync(const GetServiceRequest& request, const GetServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getService(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetServiceOutcomeCallable EasClient::getServiceCallable(const GetServiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetServiceOutcome()>>(
			[this, request]()
			{
			return this->getService(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetServiceRegionOutcome EasClient::getServiceRegion(const GetServiceRegionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetServiceRegionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetServiceRegionOutcome(GetServiceRegionResult(outcome.result()));
	else
		return GetServiceRegionOutcome(outcome.error());
}

void EasClient::getServiceRegionAsync(const GetServiceRegionRequest& request, const GetServiceRegionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getServiceRegion(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetServiceRegionOutcomeCallable EasClient::getServiceRegionCallable(const GetServiceRegionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetServiceRegionOutcome()>>(
			[this, request]()
			{
			return this->getServiceRegion(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetServiceStatisticsOutcome EasClient::getServiceStatistics(const GetServiceStatisticsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetServiceStatisticsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetServiceStatisticsOutcome(GetServiceStatisticsResult(outcome.result()));
	else
		return GetServiceStatisticsOutcome(outcome.error());
}

void EasClient::getServiceStatisticsAsync(const GetServiceStatisticsRequest& request, const GetServiceStatisticsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getServiceStatistics(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetServiceStatisticsOutcomeCallable EasClient::getServiceStatisticsCallable(const GetServiceStatisticsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetServiceStatisticsOutcome()>>(
			[this, request]()
			{
			return this->getServiceStatistics(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetServiceTokenOutcome EasClient::getServiceToken(const GetServiceTokenRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetServiceTokenOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetServiceTokenOutcome(GetServiceTokenResult(outcome.result()));
	else
		return GetServiceTokenOutcome(outcome.error());
}

void EasClient::getServiceTokenAsync(const GetServiceTokenRequest& request, const GetServiceTokenAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getServiceToken(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetServiceTokenOutcomeCallable EasClient::getServiceTokenCallable(const GetServiceTokenRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetServiceTokenOutcome()>>(
			[this, request]()
			{
			return this->getServiceToken(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetServiceVersionOutcome EasClient::getServiceVersion(const GetServiceVersionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetServiceVersionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetServiceVersionOutcome(GetServiceVersionResult(outcome.result()));
	else
		return GetServiceVersionOutcome(outcome.error());
}

void EasClient::getServiceVersionAsync(const GetServiceVersionRequest& request, const GetServiceVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getServiceVersion(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetServiceVersionOutcomeCallable EasClient::getServiceVersionCallable(const GetServiceVersionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetServiceVersionOutcome()>>(
			[this, request]()
			{
			return this->getServiceVersion(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetServiceWorkerOutcome EasClient::getServiceWorker(const GetServiceWorkerRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetServiceWorkerOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetServiceWorkerOutcome(GetServiceWorkerResult(outcome.result()));
	else
		return GetServiceWorkerOutcome(outcome.error());
}

void EasClient::getServiceWorkerAsync(const GetServiceWorkerRequest& request, const GetServiceWorkerAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getServiceWorker(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetServiceWorkerOutcomeCallable EasClient::getServiceWorkerCallable(const GetServiceWorkerRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetServiceWorkerOutcome()>>(
			[this, request]()
			{
			return this->getServiceWorker(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::GetTaskOutcome EasClient::getTask(const GetTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetTaskOutcome(GetTaskResult(outcome.result()));
	else
		return GetTaskOutcome(outcome.error());
}

void EasClient::getTaskAsync(const GetTaskRequest& request, const GetTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::GetTaskOutcomeCallable EasClient::getTaskCallable(const GetTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetTaskOutcome()>>(
			[this, request]()
			{
			return this->getTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::ListServicesOutcome EasClient::listServices(const ListServicesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListServicesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListServicesOutcome(ListServicesResult(outcome.result()));
	else
		return ListServicesOutcome(outcome.error());
}

void EasClient::listServicesAsync(const ListServicesRequest& request, const ListServicesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listServices(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::ListServicesOutcomeCallable EasClient::listServicesCallable(const ListServicesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListServicesOutcome()>>(
			[this, request]()
			{
			return this->listServices(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::ListTasksOutcome EasClient::listTasks(const ListTasksRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListTasksOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListTasksOutcome(ListTasksResult(outcome.result()));
	else
		return ListTasksOutcome(outcome.error());
}

void EasClient::listTasksAsync(const ListTasksRequest& request, const ListTasksAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listTasks(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::ListTasksOutcomeCallable EasClient::listTasksCallable(const ListTasksRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListTasksOutcome()>>(
			[this, request]()
			{
			return this->listTasks(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::ListVIndexOutcome EasClient::listVIndex(const ListVIndexRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListVIndexOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListVIndexOutcome(ListVIndexResult(outcome.result()));
	else
		return ListVIndexOutcome(outcome.error());
}

void EasClient::listVIndexAsync(const ListVIndexRequest& request, const ListVIndexAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listVIndex(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::ListVIndexOutcomeCallable EasClient::listVIndexCallable(const ListVIndexRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListVIndexOutcome()>>(
			[this, request]()
			{
			return this->listVIndex(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::ModifyServiceOutcome EasClient::modifyService(const ModifyServiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyServiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyServiceOutcome(ModifyServiceResult(outcome.result()));
	else
		return ModifyServiceOutcome(outcome.error());
}

void EasClient::modifyServiceAsync(const ModifyServiceRequest& request, const ModifyServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyService(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::ModifyServiceOutcomeCallable EasClient::modifyServiceCallable(const ModifyServiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyServiceOutcome()>>(
			[this, request]()
			{
			return this->modifyService(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::ModifyServiceVersionOutcome EasClient::modifyServiceVersion(const ModifyServiceVersionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyServiceVersionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyServiceVersionOutcome(ModifyServiceVersionResult(outcome.result()));
	else
		return ModifyServiceVersionOutcome(outcome.error());
}

void EasClient::modifyServiceVersionAsync(const ModifyServiceVersionRequest& request, const ModifyServiceVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyServiceVersion(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::ModifyServiceVersionOutcomeCallable EasClient::modifyServiceVersionCallable(const ModifyServiceVersionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyServiceVersionOutcome()>>(
			[this, request]()
			{
			return this->modifyServiceVersion(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::ReleaseServiceOutcome EasClient::releaseService(const ReleaseServiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ReleaseServiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ReleaseServiceOutcome(ReleaseServiceResult(outcome.result()));
	else
		return ReleaseServiceOutcome(outcome.error());
}

void EasClient::releaseServiceAsync(const ReleaseServiceRequest& request, const ReleaseServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, releaseService(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::ReleaseServiceOutcomeCallable EasClient::releaseServiceCallable(const ReleaseServiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ReleaseServiceOutcome()>>(
			[this, request]()
			{
			return this->releaseService(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::StartServiceOutcome EasClient::startService(const StartServiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartServiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartServiceOutcome(StartServiceResult(outcome.result()));
	else
		return StartServiceOutcome(outcome.error());
}

void EasClient::startServiceAsync(const StartServiceRequest& request, const StartServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startService(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::StartServiceOutcomeCallable EasClient::startServiceCallable(const StartServiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartServiceOutcome()>>(
			[this, request]()
			{
			return this->startService(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::StopServiceOutcome EasClient::stopService(const StopServiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StopServiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StopServiceOutcome(StopServiceResult(outcome.result()));
	else
		return StopServiceOutcome(outcome.error());
}

void EasClient::stopServiceAsync(const StopServiceRequest& request, const StopServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, stopService(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::StopServiceOutcomeCallable EasClient::stopServiceCallable(const StopServiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StopServiceOutcome()>>(
			[this, request]()
			{
			return this->stopService(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

EasClient::SyncServiceOutcome EasClient::syncService(const SyncServiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SyncServiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SyncServiceOutcome(SyncServiceResult(outcome.result()));
	else
		return SyncServiceOutcome(outcome.error());
}

void EasClient::syncServiceAsync(const SyncServiceRequest& request, const SyncServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, syncService(request), context);
	};

	asyncExecute(new Runnable(fn));
}

EasClient::SyncServiceOutcomeCallable EasClient::syncServiceCallable(const SyncServiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SyncServiceOutcome()>>(
			[this, request]()
			{
			return this->syncService(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


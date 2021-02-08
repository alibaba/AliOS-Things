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

#include <alibabacloud/servicemesh/ServicemeshClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Servicemesh;
using namespace AlibabaCloud::Servicemesh::Model;

namespace
{
	const std::string SERVICE_NAME = "servicemesh";
}

ServicemeshClient::ServicemeshClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "servicemesh");
}

ServicemeshClient::ServicemeshClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "servicemesh");
}

ServicemeshClient::ServicemeshClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "servicemesh");
}

ServicemeshClient::~ServicemeshClient()
{}

ServicemeshClient::AddClusterIntoServiceMeshOutcome ServicemeshClient::addClusterIntoServiceMesh(const AddClusterIntoServiceMeshRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddClusterIntoServiceMeshOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddClusterIntoServiceMeshOutcome(AddClusterIntoServiceMeshResult(outcome.result()));
	else
		return AddClusterIntoServiceMeshOutcome(outcome.error());
}

void ServicemeshClient::addClusterIntoServiceMeshAsync(const AddClusterIntoServiceMeshRequest& request, const AddClusterIntoServiceMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addClusterIntoServiceMesh(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::AddClusterIntoServiceMeshOutcomeCallable ServicemeshClient::addClusterIntoServiceMeshCallable(const AddClusterIntoServiceMeshRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddClusterIntoServiceMeshOutcome()>>(
			[this, request]()
			{
			return this->addClusterIntoServiceMesh(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::AddVmAppToMeshOutcome ServicemeshClient::addVmAppToMesh(const AddVmAppToMeshRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddVmAppToMeshOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddVmAppToMeshOutcome(AddVmAppToMeshResult(outcome.result()));
	else
		return AddVmAppToMeshOutcome(outcome.error());
}

void ServicemeshClient::addVmAppToMeshAsync(const AddVmAppToMeshRequest& request, const AddVmAppToMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addVmAppToMesh(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::AddVmAppToMeshOutcomeCallable ServicemeshClient::addVmAppToMeshCallable(const AddVmAppToMeshRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddVmAppToMeshOutcome()>>(
			[this, request]()
			{
			return this->addVmAppToMesh(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::CreateServiceMeshOutcome ServicemeshClient::createServiceMesh(const CreateServiceMeshRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateServiceMeshOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateServiceMeshOutcome(CreateServiceMeshResult(outcome.result()));
	else
		return CreateServiceMeshOutcome(outcome.error());
}

void ServicemeshClient::createServiceMeshAsync(const CreateServiceMeshRequest& request, const CreateServiceMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createServiceMesh(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::CreateServiceMeshOutcomeCallable ServicemeshClient::createServiceMeshCallable(const CreateServiceMeshRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateServiceMeshOutcome()>>(
			[this, request]()
			{
			return this->createServiceMesh(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::DeleteServiceMeshOutcome ServicemeshClient::deleteServiceMesh(const DeleteServiceMeshRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteServiceMeshOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteServiceMeshOutcome(DeleteServiceMeshResult(outcome.result()));
	else
		return DeleteServiceMeshOutcome(outcome.error());
}

void ServicemeshClient::deleteServiceMeshAsync(const DeleteServiceMeshRequest& request, const DeleteServiceMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteServiceMesh(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::DeleteServiceMeshOutcomeCallable ServicemeshClient::deleteServiceMeshCallable(const DeleteServiceMeshRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteServiceMeshOutcome()>>(
			[this, request]()
			{
			return this->deleteServiceMesh(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::DescribeCensOutcome ServicemeshClient::describeCens(const DescribeCensRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeCensOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeCensOutcome(DescribeCensResult(outcome.result()));
	else
		return DescribeCensOutcome(outcome.error());
}

void ServicemeshClient::describeCensAsync(const DescribeCensRequest& request, const DescribeCensAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCens(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::DescribeCensOutcomeCallable ServicemeshClient::describeCensCallable(const DescribeCensRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeCensOutcome()>>(
			[this, request]()
			{
			return this->describeCens(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::DescribeClusterGrafanaOutcome ServicemeshClient::describeClusterGrafana(const DescribeClusterGrafanaRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeClusterGrafanaOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeClusterGrafanaOutcome(DescribeClusterGrafanaResult(outcome.result()));
	else
		return DescribeClusterGrafanaOutcome(outcome.error());
}

void ServicemeshClient::describeClusterGrafanaAsync(const DescribeClusterGrafanaRequest& request, const DescribeClusterGrafanaAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeClusterGrafana(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::DescribeClusterGrafanaOutcomeCallable ServicemeshClient::describeClusterGrafanaCallable(const DescribeClusterGrafanaRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeClusterGrafanaOutcome()>>(
			[this, request]()
			{
			return this->describeClusterGrafana(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::DescribeClusterPrometheusOutcome ServicemeshClient::describeClusterPrometheus(const DescribeClusterPrometheusRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeClusterPrometheusOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeClusterPrometheusOutcome(DescribeClusterPrometheusResult(outcome.result()));
	else
		return DescribeClusterPrometheusOutcome(outcome.error());
}

void ServicemeshClient::describeClusterPrometheusAsync(const DescribeClusterPrometheusRequest& request, const DescribeClusterPrometheusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeClusterPrometheus(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::DescribeClusterPrometheusOutcomeCallable ServicemeshClient::describeClusterPrometheusCallable(const DescribeClusterPrometheusRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeClusterPrometheusOutcome()>>(
			[this, request]()
			{
			return this->describeClusterPrometheus(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::DescribeClustersInServiceMeshOutcome ServicemeshClient::describeClustersInServiceMesh(const DescribeClustersInServiceMeshRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeClustersInServiceMeshOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeClustersInServiceMeshOutcome(DescribeClustersInServiceMeshResult(outcome.result()));
	else
		return DescribeClustersInServiceMeshOutcome(outcome.error());
}

void ServicemeshClient::describeClustersInServiceMeshAsync(const DescribeClustersInServiceMeshRequest& request, const DescribeClustersInServiceMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeClustersInServiceMesh(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::DescribeClustersInServiceMeshOutcomeCallable ServicemeshClient::describeClustersInServiceMeshCallable(const DescribeClustersInServiceMeshRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeClustersInServiceMeshOutcome()>>(
			[this, request]()
			{
			return this->describeClustersInServiceMesh(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::DescribeGuestClusterAccessLogDashboardsOutcome ServicemeshClient::describeGuestClusterAccessLogDashboards(const DescribeGuestClusterAccessLogDashboardsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeGuestClusterAccessLogDashboardsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeGuestClusterAccessLogDashboardsOutcome(DescribeGuestClusterAccessLogDashboardsResult(outcome.result()));
	else
		return DescribeGuestClusterAccessLogDashboardsOutcome(outcome.error());
}

void ServicemeshClient::describeGuestClusterAccessLogDashboardsAsync(const DescribeGuestClusterAccessLogDashboardsRequest& request, const DescribeGuestClusterAccessLogDashboardsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeGuestClusterAccessLogDashboards(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::DescribeGuestClusterAccessLogDashboardsOutcomeCallable ServicemeshClient::describeGuestClusterAccessLogDashboardsCallable(const DescribeGuestClusterAccessLogDashboardsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeGuestClusterAccessLogDashboardsOutcome()>>(
			[this, request]()
			{
			return this->describeGuestClusterAccessLogDashboards(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::DescribeIngressGatewaysOutcome ServicemeshClient::describeIngressGateways(const DescribeIngressGatewaysRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeIngressGatewaysOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeIngressGatewaysOutcome(DescribeIngressGatewaysResult(outcome.result()));
	else
		return DescribeIngressGatewaysOutcome(outcome.error());
}

void ServicemeshClient::describeIngressGatewaysAsync(const DescribeIngressGatewaysRequest& request, const DescribeIngressGatewaysAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeIngressGateways(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::DescribeIngressGatewaysOutcomeCallable ServicemeshClient::describeIngressGatewaysCallable(const DescribeIngressGatewaysRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeIngressGatewaysOutcome()>>(
			[this, request]()
			{
			return this->describeIngressGateways(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::DescribeServiceMeshDetailOutcome ServicemeshClient::describeServiceMeshDetail(const DescribeServiceMeshDetailRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeServiceMeshDetailOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeServiceMeshDetailOutcome(DescribeServiceMeshDetailResult(outcome.result()));
	else
		return DescribeServiceMeshDetailOutcome(outcome.error());
}

void ServicemeshClient::describeServiceMeshDetailAsync(const DescribeServiceMeshDetailRequest& request, const DescribeServiceMeshDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeServiceMeshDetail(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::DescribeServiceMeshDetailOutcomeCallable ServicemeshClient::describeServiceMeshDetailCallable(const DescribeServiceMeshDetailRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeServiceMeshDetailOutcome()>>(
			[this, request]()
			{
			return this->describeServiceMeshDetail(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::DescribeServiceMeshKubeconfigOutcome ServicemeshClient::describeServiceMeshKubeconfig(const DescribeServiceMeshKubeconfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeServiceMeshKubeconfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeServiceMeshKubeconfigOutcome(DescribeServiceMeshKubeconfigResult(outcome.result()));
	else
		return DescribeServiceMeshKubeconfigOutcome(outcome.error());
}

void ServicemeshClient::describeServiceMeshKubeconfigAsync(const DescribeServiceMeshKubeconfigRequest& request, const DescribeServiceMeshKubeconfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeServiceMeshKubeconfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::DescribeServiceMeshKubeconfigOutcomeCallable ServicemeshClient::describeServiceMeshKubeconfigCallable(const DescribeServiceMeshKubeconfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeServiceMeshKubeconfigOutcome()>>(
			[this, request]()
			{
			return this->describeServiceMeshKubeconfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::DescribeServiceMeshesOutcome ServicemeshClient::describeServiceMeshes(const DescribeServiceMeshesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeServiceMeshesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeServiceMeshesOutcome(DescribeServiceMeshesResult(outcome.result()));
	else
		return DescribeServiceMeshesOutcome(outcome.error());
}

void ServicemeshClient::describeServiceMeshesAsync(const DescribeServiceMeshesRequest& request, const DescribeServiceMeshesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeServiceMeshes(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::DescribeServiceMeshesOutcomeCallable ServicemeshClient::describeServiceMeshesCallable(const DescribeServiceMeshesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeServiceMeshesOutcome()>>(
			[this, request]()
			{
			return this->describeServiceMeshes(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::DescribeUpgradeVersionOutcome ServicemeshClient::describeUpgradeVersion(const DescribeUpgradeVersionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeUpgradeVersionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeUpgradeVersionOutcome(DescribeUpgradeVersionResult(outcome.result()));
	else
		return DescribeUpgradeVersionOutcome(outcome.error());
}

void ServicemeshClient::describeUpgradeVersionAsync(const DescribeUpgradeVersionRequest& request, const DescribeUpgradeVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeUpgradeVersion(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::DescribeUpgradeVersionOutcomeCallable ServicemeshClient::describeUpgradeVersionCallable(const DescribeUpgradeVersionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeUpgradeVersionOutcome()>>(
			[this, request]()
			{
			return this->describeUpgradeVersion(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::GetDiagnosisOutcome ServicemeshClient::getDiagnosis(const GetDiagnosisRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDiagnosisOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDiagnosisOutcome(GetDiagnosisResult(outcome.result()));
	else
		return GetDiagnosisOutcome(outcome.error());
}

void ServicemeshClient::getDiagnosisAsync(const GetDiagnosisRequest& request, const GetDiagnosisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDiagnosis(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::GetDiagnosisOutcomeCallable ServicemeshClient::getDiagnosisCallable(const GetDiagnosisRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDiagnosisOutcome()>>(
			[this, request]()
			{
			return this->getDiagnosis(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::GetRegisteredServiceEndpointsOutcome ServicemeshClient::getRegisteredServiceEndpoints(const GetRegisteredServiceEndpointsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRegisteredServiceEndpointsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRegisteredServiceEndpointsOutcome(GetRegisteredServiceEndpointsResult(outcome.result()));
	else
		return GetRegisteredServiceEndpointsOutcome(outcome.error());
}

void ServicemeshClient::getRegisteredServiceEndpointsAsync(const GetRegisteredServiceEndpointsRequest& request, const GetRegisteredServiceEndpointsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRegisteredServiceEndpoints(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::GetRegisteredServiceEndpointsOutcomeCallable ServicemeshClient::getRegisteredServiceEndpointsCallable(const GetRegisteredServiceEndpointsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRegisteredServiceEndpointsOutcome()>>(
			[this, request]()
			{
			return this->getRegisteredServiceEndpoints(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::GetRegisteredServiceNamespacesOutcome ServicemeshClient::getRegisteredServiceNamespaces(const GetRegisteredServiceNamespacesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRegisteredServiceNamespacesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRegisteredServiceNamespacesOutcome(GetRegisteredServiceNamespacesResult(outcome.result()));
	else
		return GetRegisteredServiceNamespacesOutcome(outcome.error());
}

void ServicemeshClient::getRegisteredServiceNamespacesAsync(const GetRegisteredServiceNamespacesRequest& request, const GetRegisteredServiceNamespacesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRegisteredServiceNamespaces(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::GetRegisteredServiceNamespacesOutcomeCallable ServicemeshClient::getRegisteredServiceNamespacesCallable(const GetRegisteredServiceNamespacesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRegisteredServiceNamespacesOutcome()>>(
			[this, request]()
			{
			return this->getRegisteredServiceNamespaces(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::GetRegisteredServicesOutcome ServicemeshClient::getRegisteredServices(const GetRegisteredServicesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRegisteredServicesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRegisteredServicesOutcome(GetRegisteredServicesResult(outcome.result()));
	else
		return GetRegisteredServicesOutcome(outcome.error());
}

void ServicemeshClient::getRegisteredServicesAsync(const GetRegisteredServicesRequest& request, const GetRegisteredServicesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRegisteredServices(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::GetRegisteredServicesOutcomeCallable ServicemeshClient::getRegisteredServicesCallable(const GetRegisteredServicesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRegisteredServicesOutcome()>>(
			[this, request]()
			{
			return this->getRegisteredServices(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::GetServiceMeshSlbOutcome ServicemeshClient::getServiceMeshSlb(const GetServiceMeshSlbRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetServiceMeshSlbOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetServiceMeshSlbOutcome(GetServiceMeshSlbResult(outcome.result()));
	else
		return GetServiceMeshSlbOutcome(outcome.error());
}

void ServicemeshClient::getServiceMeshSlbAsync(const GetServiceMeshSlbRequest& request, const GetServiceMeshSlbAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getServiceMeshSlb(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::GetServiceMeshSlbOutcomeCallable ServicemeshClient::getServiceMeshSlbCallable(const GetServiceMeshSlbRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetServiceMeshSlbOutcome()>>(
			[this, request]()
			{
			return this->getServiceMeshSlb(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::GetVmAppMeshInfoOutcome ServicemeshClient::getVmAppMeshInfo(const GetVmAppMeshInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetVmAppMeshInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetVmAppMeshInfoOutcome(GetVmAppMeshInfoResult(outcome.result()));
	else
		return GetVmAppMeshInfoOutcome(outcome.error());
}

void ServicemeshClient::getVmAppMeshInfoAsync(const GetVmAppMeshInfoRequest& request, const GetVmAppMeshInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getVmAppMeshInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::GetVmAppMeshInfoOutcomeCallable ServicemeshClient::getVmAppMeshInfoCallable(const GetVmAppMeshInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetVmAppMeshInfoOutcome()>>(
			[this, request]()
			{
			return this->getVmAppMeshInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::GetVmMetaOutcome ServicemeshClient::getVmMeta(const GetVmMetaRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetVmMetaOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetVmMetaOutcome(GetVmMetaResult(outcome.result()));
	else
		return GetVmMetaOutcome(outcome.error());
}

void ServicemeshClient::getVmMetaAsync(const GetVmMetaRequest& request, const GetVmMetaAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getVmMeta(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::GetVmMetaOutcomeCallable ServicemeshClient::getVmMetaCallable(const GetVmMetaRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetVmMetaOutcome()>>(
			[this, request]()
			{
			return this->getVmMeta(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::RemoveClusterFromServiceMeshOutcome ServicemeshClient::removeClusterFromServiceMesh(const RemoveClusterFromServiceMeshRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveClusterFromServiceMeshOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveClusterFromServiceMeshOutcome(RemoveClusterFromServiceMeshResult(outcome.result()));
	else
		return RemoveClusterFromServiceMeshOutcome(outcome.error());
}

void ServicemeshClient::removeClusterFromServiceMeshAsync(const RemoveClusterFromServiceMeshRequest& request, const RemoveClusterFromServiceMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeClusterFromServiceMesh(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::RemoveClusterFromServiceMeshOutcomeCallable ServicemeshClient::removeClusterFromServiceMeshCallable(const RemoveClusterFromServiceMeshRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveClusterFromServiceMeshOutcome()>>(
			[this, request]()
			{
			return this->removeClusterFromServiceMesh(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::RemoveVmAppFromMeshOutcome ServicemeshClient::removeVmAppFromMesh(const RemoveVmAppFromMeshRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveVmAppFromMeshOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveVmAppFromMeshOutcome(RemoveVmAppFromMeshResult(outcome.result()));
	else
		return RemoveVmAppFromMeshOutcome(outcome.error());
}

void ServicemeshClient::removeVmAppFromMeshAsync(const RemoveVmAppFromMeshRequest& request, const RemoveVmAppFromMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeVmAppFromMesh(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::RemoveVmAppFromMeshOutcomeCallable ServicemeshClient::removeVmAppFromMeshCallable(const RemoveVmAppFromMeshRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveVmAppFromMeshOutcome()>>(
			[this, request]()
			{
			return this->removeVmAppFromMesh(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::RunDiagnosisOutcome ServicemeshClient::runDiagnosis(const RunDiagnosisRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RunDiagnosisOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RunDiagnosisOutcome(RunDiagnosisResult(outcome.result()));
	else
		return RunDiagnosisOutcome(outcome.error());
}

void ServicemeshClient::runDiagnosisAsync(const RunDiagnosisRequest& request, const RunDiagnosisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, runDiagnosis(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::RunDiagnosisOutcomeCallable ServicemeshClient::runDiagnosisCallable(const RunDiagnosisRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RunDiagnosisOutcome()>>(
			[this, request]()
			{
			return this->runDiagnosis(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::UpdateIstioInjectionConfigOutcome ServicemeshClient::updateIstioInjectionConfig(const UpdateIstioInjectionConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateIstioInjectionConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateIstioInjectionConfigOutcome(UpdateIstioInjectionConfigResult(outcome.result()));
	else
		return UpdateIstioInjectionConfigOutcome(outcome.error());
}

void ServicemeshClient::updateIstioInjectionConfigAsync(const UpdateIstioInjectionConfigRequest& request, const UpdateIstioInjectionConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateIstioInjectionConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::UpdateIstioInjectionConfigOutcomeCallable ServicemeshClient::updateIstioInjectionConfigCallable(const UpdateIstioInjectionConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateIstioInjectionConfigOutcome()>>(
			[this, request]()
			{
			return this->updateIstioInjectionConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::UpdateMeshFeatureOutcome ServicemeshClient::updateMeshFeature(const UpdateMeshFeatureRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateMeshFeatureOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateMeshFeatureOutcome(UpdateMeshFeatureResult(outcome.result()));
	else
		return UpdateMeshFeatureOutcome(outcome.error());
}

void ServicemeshClient::updateMeshFeatureAsync(const UpdateMeshFeatureRequest& request, const UpdateMeshFeatureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateMeshFeature(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::UpdateMeshFeatureOutcomeCallable ServicemeshClient::updateMeshFeatureCallable(const UpdateMeshFeatureRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateMeshFeatureOutcome()>>(
			[this, request]()
			{
			return this->updateMeshFeature(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ServicemeshClient::UpgradeMeshVersionOutcome ServicemeshClient::upgradeMeshVersion(const UpgradeMeshVersionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpgradeMeshVersionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpgradeMeshVersionOutcome(UpgradeMeshVersionResult(outcome.result()));
	else
		return UpgradeMeshVersionOutcome(outcome.error());
}

void ServicemeshClient::upgradeMeshVersionAsync(const UpgradeMeshVersionRequest& request, const UpgradeMeshVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, upgradeMeshVersion(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ServicemeshClient::UpgradeMeshVersionOutcomeCallable ServicemeshClient::upgradeMeshVersionCallable(const UpgradeMeshVersionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpgradeMeshVersionOutcome()>>(
			[this, request]()
			{
			return this->upgradeMeshVersion(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


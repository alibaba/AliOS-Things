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

#ifndef ALIBABACLOUD_SERVICEMESH_SERVICEMESHCLIENT_H_
#define ALIBABACLOUD_SERVICEMESH_SERVICEMESHCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "ServicemeshExport.h"
#include "model/AddClusterIntoServiceMeshRequest.h"
#include "model/AddClusterIntoServiceMeshResult.h"
#include "model/AddVmAppToMeshRequest.h"
#include "model/AddVmAppToMeshResult.h"
#include "model/CreateServiceMeshRequest.h"
#include "model/CreateServiceMeshResult.h"
#include "model/DeleteServiceMeshRequest.h"
#include "model/DeleteServiceMeshResult.h"
#include "model/DescribeCensRequest.h"
#include "model/DescribeCensResult.h"
#include "model/DescribeClusterGrafanaRequest.h"
#include "model/DescribeClusterGrafanaResult.h"
#include "model/DescribeClusterPrometheusRequest.h"
#include "model/DescribeClusterPrometheusResult.h"
#include "model/DescribeClustersInServiceMeshRequest.h"
#include "model/DescribeClustersInServiceMeshResult.h"
#include "model/DescribeGuestClusterAccessLogDashboardsRequest.h"
#include "model/DescribeGuestClusterAccessLogDashboardsResult.h"
#include "model/DescribeIngressGatewaysRequest.h"
#include "model/DescribeIngressGatewaysResult.h"
#include "model/DescribeServiceMeshDetailRequest.h"
#include "model/DescribeServiceMeshDetailResult.h"
#include "model/DescribeServiceMeshKubeconfigRequest.h"
#include "model/DescribeServiceMeshKubeconfigResult.h"
#include "model/DescribeServiceMeshesRequest.h"
#include "model/DescribeServiceMeshesResult.h"
#include "model/DescribeUpgradeVersionRequest.h"
#include "model/DescribeUpgradeVersionResult.h"
#include "model/GetDiagnosisRequest.h"
#include "model/GetDiagnosisResult.h"
#include "model/GetRegisteredServiceEndpointsRequest.h"
#include "model/GetRegisteredServiceEndpointsResult.h"
#include "model/GetRegisteredServiceNamespacesRequest.h"
#include "model/GetRegisteredServiceNamespacesResult.h"
#include "model/GetRegisteredServicesRequest.h"
#include "model/GetRegisteredServicesResult.h"
#include "model/GetServiceMeshSlbRequest.h"
#include "model/GetServiceMeshSlbResult.h"
#include "model/GetVmAppMeshInfoRequest.h"
#include "model/GetVmAppMeshInfoResult.h"
#include "model/GetVmMetaRequest.h"
#include "model/GetVmMetaResult.h"
#include "model/RemoveClusterFromServiceMeshRequest.h"
#include "model/RemoveClusterFromServiceMeshResult.h"
#include "model/RemoveVmAppFromMeshRequest.h"
#include "model/RemoveVmAppFromMeshResult.h"
#include "model/RunDiagnosisRequest.h"
#include "model/RunDiagnosisResult.h"
#include "model/UpdateIstioInjectionConfigRequest.h"
#include "model/UpdateIstioInjectionConfigResult.h"
#include "model/UpdateMeshFeatureRequest.h"
#include "model/UpdateMeshFeatureResult.h"
#include "model/UpgradeMeshVersionRequest.h"
#include "model/UpgradeMeshVersionResult.h"


namespace AlibabaCloud
{
	namespace Servicemesh
	{
		class ALIBABACLOUD_SERVICEMESH_EXPORT ServicemeshClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::AddClusterIntoServiceMeshResult> AddClusterIntoServiceMeshOutcome;
			typedef std::future<AddClusterIntoServiceMeshOutcome> AddClusterIntoServiceMeshOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::AddClusterIntoServiceMeshRequest&, const AddClusterIntoServiceMeshOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddClusterIntoServiceMeshAsyncHandler;
			typedef Outcome<Error, Model::AddVmAppToMeshResult> AddVmAppToMeshOutcome;
			typedef std::future<AddVmAppToMeshOutcome> AddVmAppToMeshOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::AddVmAppToMeshRequest&, const AddVmAppToMeshOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddVmAppToMeshAsyncHandler;
			typedef Outcome<Error, Model::CreateServiceMeshResult> CreateServiceMeshOutcome;
			typedef std::future<CreateServiceMeshOutcome> CreateServiceMeshOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::CreateServiceMeshRequest&, const CreateServiceMeshOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateServiceMeshAsyncHandler;
			typedef Outcome<Error, Model::DeleteServiceMeshResult> DeleteServiceMeshOutcome;
			typedef std::future<DeleteServiceMeshOutcome> DeleteServiceMeshOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::DeleteServiceMeshRequest&, const DeleteServiceMeshOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteServiceMeshAsyncHandler;
			typedef Outcome<Error, Model::DescribeCensResult> DescribeCensOutcome;
			typedef std::future<DescribeCensOutcome> DescribeCensOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::DescribeCensRequest&, const DescribeCensOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeCensAsyncHandler;
			typedef Outcome<Error, Model::DescribeClusterGrafanaResult> DescribeClusterGrafanaOutcome;
			typedef std::future<DescribeClusterGrafanaOutcome> DescribeClusterGrafanaOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::DescribeClusterGrafanaRequest&, const DescribeClusterGrafanaOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeClusterGrafanaAsyncHandler;
			typedef Outcome<Error, Model::DescribeClusterPrometheusResult> DescribeClusterPrometheusOutcome;
			typedef std::future<DescribeClusterPrometheusOutcome> DescribeClusterPrometheusOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::DescribeClusterPrometheusRequest&, const DescribeClusterPrometheusOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeClusterPrometheusAsyncHandler;
			typedef Outcome<Error, Model::DescribeClustersInServiceMeshResult> DescribeClustersInServiceMeshOutcome;
			typedef std::future<DescribeClustersInServiceMeshOutcome> DescribeClustersInServiceMeshOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::DescribeClustersInServiceMeshRequest&, const DescribeClustersInServiceMeshOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeClustersInServiceMeshAsyncHandler;
			typedef Outcome<Error, Model::DescribeGuestClusterAccessLogDashboardsResult> DescribeGuestClusterAccessLogDashboardsOutcome;
			typedef std::future<DescribeGuestClusterAccessLogDashboardsOutcome> DescribeGuestClusterAccessLogDashboardsOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::DescribeGuestClusterAccessLogDashboardsRequest&, const DescribeGuestClusterAccessLogDashboardsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeGuestClusterAccessLogDashboardsAsyncHandler;
			typedef Outcome<Error, Model::DescribeIngressGatewaysResult> DescribeIngressGatewaysOutcome;
			typedef std::future<DescribeIngressGatewaysOutcome> DescribeIngressGatewaysOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::DescribeIngressGatewaysRequest&, const DescribeIngressGatewaysOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeIngressGatewaysAsyncHandler;
			typedef Outcome<Error, Model::DescribeServiceMeshDetailResult> DescribeServiceMeshDetailOutcome;
			typedef std::future<DescribeServiceMeshDetailOutcome> DescribeServiceMeshDetailOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::DescribeServiceMeshDetailRequest&, const DescribeServiceMeshDetailOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeServiceMeshDetailAsyncHandler;
			typedef Outcome<Error, Model::DescribeServiceMeshKubeconfigResult> DescribeServiceMeshKubeconfigOutcome;
			typedef std::future<DescribeServiceMeshKubeconfigOutcome> DescribeServiceMeshKubeconfigOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::DescribeServiceMeshKubeconfigRequest&, const DescribeServiceMeshKubeconfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeServiceMeshKubeconfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeServiceMeshesResult> DescribeServiceMeshesOutcome;
			typedef std::future<DescribeServiceMeshesOutcome> DescribeServiceMeshesOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::DescribeServiceMeshesRequest&, const DescribeServiceMeshesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeServiceMeshesAsyncHandler;
			typedef Outcome<Error, Model::DescribeUpgradeVersionResult> DescribeUpgradeVersionOutcome;
			typedef std::future<DescribeUpgradeVersionOutcome> DescribeUpgradeVersionOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::DescribeUpgradeVersionRequest&, const DescribeUpgradeVersionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeUpgradeVersionAsyncHandler;
			typedef Outcome<Error, Model::GetDiagnosisResult> GetDiagnosisOutcome;
			typedef std::future<GetDiagnosisOutcome> GetDiagnosisOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::GetDiagnosisRequest&, const GetDiagnosisOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDiagnosisAsyncHandler;
			typedef Outcome<Error, Model::GetRegisteredServiceEndpointsResult> GetRegisteredServiceEndpointsOutcome;
			typedef std::future<GetRegisteredServiceEndpointsOutcome> GetRegisteredServiceEndpointsOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::GetRegisteredServiceEndpointsRequest&, const GetRegisteredServiceEndpointsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRegisteredServiceEndpointsAsyncHandler;
			typedef Outcome<Error, Model::GetRegisteredServiceNamespacesResult> GetRegisteredServiceNamespacesOutcome;
			typedef std::future<GetRegisteredServiceNamespacesOutcome> GetRegisteredServiceNamespacesOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::GetRegisteredServiceNamespacesRequest&, const GetRegisteredServiceNamespacesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRegisteredServiceNamespacesAsyncHandler;
			typedef Outcome<Error, Model::GetRegisteredServicesResult> GetRegisteredServicesOutcome;
			typedef std::future<GetRegisteredServicesOutcome> GetRegisteredServicesOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::GetRegisteredServicesRequest&, const GetRegisteredServicesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRegisteredServicesAsyncHandler;
			typedef Outcome<Error, Model::GetServiceMeshSlbResult> GetServiceMeshSlbOutcome;
			typedef std::future<GetServiceMeshSlbOutcome> GetServiceMeshSlbOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::GetServiceMeshSlbRequest&, const GetServiceMeshSlbOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetServiceMeshSlbAsyncHandler;
			typedef Outcome<Error, Model::GetVmAppMeshInfoResult> GetVmAppMeshInfoOutcome;
			typedef std::future<GetVmAppMeshInfoOutcome> GetVmAppMeshInfoOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::GetVmAppMeshInfoRequest&, const GetVmAppMeshInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetVmAppMeshInfoAsyncHandler;
			typedef Outcome<Error, Model::GetVmMetaResult> GetVmMetaOutcome;
			typedef std::future<GetVmMetaOutcome> GetVmMetaOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::GetVmMetaRequest&, const GetVmMetaOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetVmMetaAsyncHandler;
			typedef Outcome<Error, Model::RemoveClusterFromServiceMeshResult> RemoveClusterFromServiceMeshOutcome;
			typedef std::future<RemoveClusterFromServiceMeshOutcome> RemoveClusterFromServiceMeshOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::RemoveClusterFromServiceMeshRequest&, const RemoveClusterFromServiceMeshOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RemoveClusterFromServiceMeshAsyncHandler;
			typedef Outcome<Error, Model::RemoveVmAppFromMeshResult> RemoveVmAppFromMeshOutcome;
			typedef std::future<RemoveVmAppFromMeshOutcome> RemoveVmAppFromMeshOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::RemoveVmAppFromMeshRequest&, const RemoveVmAppFromMeshOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RemoveVmAppFromMeshAsyncHandler;
			typedef Outcome<Error, Model::RunDiagnosisResult> RunDiagnosisOutcome;
			typedef std::future<RunDiagnosisOutcome> RunDiagnosisOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::RunDiagnosisRequest&, const RunDiagnosisOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RunDiagnosisAsyncHandler;
			typedef Outcome<Error, Model::UpdateIstioInjectionConfigResult> UpdateIstioInjectionConfigOutcome;
			typedef std::future<UpdateIstioInjectionConfigOutcome> UpdateIstioInjectionConfigOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::UpdateIstioInjectionConfigRequest&, const UpdateIstioInjectionConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateIstioInjectionConfigAsyncHandler;
			typedef Outcome<Error, Model::UpdateMeshFeatureResult> UpdateMeshFeatureOutcome;
			typedef std::future<UpdateMeshFeatureOutcome> UpdateMeshFeatureOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::UpdateMeshFeatureRequest&, const UpdateMeshFeatureOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateMeshFeatureAsyncHandler;
			typedef Outcome<Error, Model::UpgradeMeshVersionResult> UpgradeMeshVersionOutcome;
			typedef std::future<UpgradeMeshVersionOutcome> UpgradeMeshVersionOutcomeCallable;
			typedef std::function<void(const ServicemeshClient*, const Model::UpgradeMeshVersionRequest&, const UpgradeMeshVersionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpgradeMeshVersionAsyncHandler;

			ServicemeshClient(const Credentials &credentials, const ClientConfiguration &configuration);
			ServicemeshClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			ServicemeshClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~ServicemeshClient();
			AddClusterIntoServiceMeshOutcome addClusterIntoServiceMesh(const Model::AddClusterIntoServiceMeshRequest &request)const;
			void addClusterIntoServiceMeshAsync(const Model::AddClusterIntoServiceMeshRequest& request, const AddClusterIntoServiceMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddClusterIntoServiceMeshOutcomeCallable addClusterIntoServiceMeshCallable(const Model::AddClusterIntoServiceMeshRequest& request) const;
			AddVmAppToMeshOutcome addVmAppToMesh(const Model::AddVmAppToMeshRequest &request)const;
			void addVmAppToMeshAsync(const Model::AddVmAppToMeshRequest& request, const AddVmAppToMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddVmAppToMeshOutcomeCallable addVmAppToMeshCallable(const Model::AddVmAppToMeshRequest& request) const;
			CreateServiceMeshOutcome createServiceMesh(const Model::CreateServiceMeshRequest &request)const;
			void createServiceMeshAsync(const Model::CreateServiceMeshRequest& request, const CreateServiceMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateServiceMeshOutcomeCallable createServiceMeshCallable(const Model::CreateServiceMeshRequest& request) const;
			DeleteServiceMeshOutcome deleteServiceMesh(const Model::DeleteServiceMeshRequest &request)const;
			void deleteServiceMeshAsync(const Model::DeleteServiceMeshRequest& request, const DeleteServiceMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteServiceMeshOutcomeCallable deleteServiceMeshCallable(const Model::DeleteServiceMeshRequest& request) const;
			DescribeCensOutcome describeCens(const Model::DescribeCensRequest &request)const;
			void describeCensAsync(const Model::DescribeCensRequest& request, const DescribeCensAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeCensOutcomeCallable describeCensCallable(const Model::DescribeCensRequest& request) const;
			DescribeClusterGrafanaOutcome describeClusterGrafana(const Model::DescribeClusterGrafanaRequest &request)const;
			void describeClusterGrafanaAsync(const Model::DescribeClusterGrafanaRequest& request, const DescribeClusterGrafanaAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeClusterGrafanaOutcomeCallable describeClusterGrafanaCallable(const Model::DescribeClusterGrafanaRequest& request) const;
			DescribeClusterPrometheusOutcome describeClusterPrometheus(const Model::DescribeClusterPrometheusRequest &request)const;
			void describeClusterPrometheusAsync(const Model::DescribeClusterPrometheusRequest& request, const DescribeClusterPrometheusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeClusterPrometheusOutcomeCallable describeClusterPrometheusCallable(const Model::DescribeClusterPrometheusRequest& request) const;
			DescribeClustersInServiceMeshOutcome describeClustersInServiceMesh(const Model::DescribeClustersInServiceMeshRequest &request)const;
			void describeClustersInServiceMeshAsync(const Model::DescribeClustersInServiceMeshRequest& request, const DescribeClustersInServiceMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeClustersInServiceMeshOutcomeCallable describeClustersInServiceMeshCallable(const Model::DescribeClustersInServiceMeshRequest& request) const;
			DescribeGuestClusterAccessLogDashboardsOutcome describeGuestClusterAccessLogDashboards(const Model::DescribeGuestClusterAccessLogDashboardsRequest &request)const;
			void describeGuestClusterAccessLogDashboardsAsync(const Model::DescribeGuestClusterAccessLogDashboardsRequest& request, const DescribeGuestClusterAccessLogDashboardsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeGuestClusterAccessLogDashboardsOutcomeCallable describeGuestClusterAccessLogDashboardsCallable(const Model::DescribeGuestClusterAccessLogDashboardsRequest& request) const;
			DescribeIngressGatewaysOutcome describeIngressGateways(const Model::DescribeIngressGatewaysRequest &request)const;
			void describeIngressGatewaysAsync(const Model::DescribeIngressGatewaysRequest& request, const DescribeIngressGatewaysAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeIngressGatewaysOutcomeCallable describeIngressGatewaysCallable(const Model::DescribeIngressGatewaysRequest& request) const;
			DescribeServiceMeshDetailOutcome describeServiceMeshDetail(const Model::DescribeServiceMeshDetailRequest &request)const;
			void describeServiceMeshDetailAsync(const Model::DescribeServiceMeshDetailRequest& request, const DescribeServiceMeshDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeServiceMeshDetailOutcomeCallable describeServiceMeshDetailCallable(const Model::DescribeServiceMeshDetailRequest& request) const;
			DescribeServiceMeshKubeconfigOutcome describeServiceMeshKubeconfig(const Model::DescribeServiceMeshKubeconfigRequest &request)const;
			void describeServiceMeshKubeconfigAsync(const Model::DescribeServiceMeshKubeconfigRequest& request, const DescribeServiceMeshKubeconfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeServiceMeshKubeconfigOutcomeCallable describeServiceMeshKubeconfigCallable(const Model::DescribeServiceMeshKubeconfigRequest& request) const;
			DescribeServiceMeshesOutcome describeServiceMeshes(const Model::DescribeServiceMeshesRequest &request)const;
			void describeServiceMeshesAsync(const Model::DescribeServiceMeshesRequest& request, const DescribeServiceMeshesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeServiceMeshesOutcomeCallable describeServiceMeshesCallable(const Model::DescribeServiceMeshesRequest& request) const;
			DescribeUpgradeVersionOutcome describeUpgradeVersion(const Model::DescribeUpgradeVersionRequest &request)const;
			void describeUpgradeVersionAsync(const Model::DescribeUpgradeVersionRequest& request, const DescribeUpgradeVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeUpgradeVersionOutcomeCallable describeUpgradeVersionCallable(const Model::DescribeUpgradeVersionRequest& request) const;
			GetDiagnosisOutcome getDiagnosis(const Model::GetDiagnosisRequest &request)const;
			void getDiagnosisAsync(const Model::GetDiagnosisRequest& request, const GetDiagnosisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDiagnosisOutcomeCallable getDiagnosisCallable(const Model::GetDiagnosisRequest& request) const;
			GetRegisteredServiceEndpointsOutcome getRegisteredServiceEndpoints(const Model::GetRegisteredServiceEndpointsRequest &request)const;
			void getRegisteredServiceEndpointsAsync(const Model::GetRegisteredServiceEndpointsRequest& request, const GetRegisteredServiceEndpointsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRegisteredServiceEndpointsOutcomeCallable getRegisteredServiceEndpointsCallable(const Model::GetRegisteredServiceEndpointsRequest& request) const;
			GetRegisteredServiceNamespacesOutcome getRegisteredServiceNamespaces(const Model::GetRegisteredServiceNamespacesRequest &request)const;
			void getRegisteredServiceNamespacesAsync(const Model::GetRegisteredServiceNamespacesRequest& request, const GetRegisteredServiceNamespacesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRegisteredServiceNamespacesOutcomeCallable getRegisteredServiceNamespacesCallable(const Model::GetRegisteredServiceNamespacesRequest& request) const;
			GetRegisteredServicesOutcome getRegisteredServices(const Model::GetRegisteredServicesRequest &request)const;
			void getRegisteredServicesAsync(const Model::GetRegisteredServicesRequest& request, const GetRegisteredServicesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRegisteredServicesOutcomeCallable getRegisteredServicesCallable(const Model::GetRegisteredServicesRequest& request) const;
			GetServiceMeshSlbOutcome getServiceMeshSlb(const Model::GetServiceMeshSlbRequest &request)const;
			void getServiceMeshSlbAsync(const Model::GetServiceMeshSlbRequest& request, const GetServiceMeshSlbAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetServiceMeshSlbOutcomeCallable getServiceMeshSlbCallable(const Model::GetServiceMeshSlbRequest& request) const;
			GetVmAppMeshInfoOutcome getVmAppMeshInfo(const Model::GetVmAppMeshInfoRequest &request)const;
			void getVmAppMeshInfoAsync(const Model::GetVmAppMeshInfoRequest& request, const GetVmAppMeshInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetVmAppMeshInfoOutcomeCallable getVmAppMeshInfoCallable(const Model::GetVmAppMeshInfoRequest& request) const;
			GetVmMetaOutcome getVmMeta(const Model::GetVmMetaRequest &request)const;
			void getVmMetaAsync(const Model::GetVmMetaRequest& request, const GetVmMetaAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetVmMetaOutcomeCallable getVmMetaCallable(const Model::GetVmMetaRequest& request) const;
			RemoveClusterFromServiceMeshOutcome removeClusterFromServiceMesh(const Model::RemoveClusterFromServiceMeshRequest &request)const;
			void removeClusterFromServiceMeshAsync(const Model::RemoveClusterFromServiceMeshRequest& request, const RemoveClusterFromServiceMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RemoveClusterFromServiceMeshOutcomeCallable removeClusterFromServiceMeshCallable(const Model::RemoveClusterFromServiceMeshRequest& request) const;
			RemoveVmAppFromMeshOutcome removeVmAppFromMesh(const Model::RemoveVmAppFromMeshRequest &request)const;
			void removeVmAppFromMeshAsync(const Model::RemoveVmAppFromMeshRequest& request, const RemoveVmAppFromMeshAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RemoveVmAppFromMeshOutcomeCallable removeVmAppFromMeshCallable(const Model::RemoveVmAppFromMeshRequest& request) const;
			RunDiagnosisOutcome runDiagnosis(const Model::RunDiagnosisRequest &request)const;
			void runDiagnosisAsync(const Model::RunDiagnosisRequest& request, const RunDiagnosisAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RunDiagnosisOutcomeCallable runDiagnosisCallable(const Model::RunDiagnosisRequest& request) const;
			UpdateIstioInjectionConfigOutcome updateIstioInjectionConfig(const Model::UpdateIstioInjectionConfigRequest &request)const;
			void updateIstioInjectionConfigAsync(const Model::UpdateIstioInjectionConfigRequest& request, const UpdateIstioInjectionConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateIstioInjectionConfigOutcomeCallable updateIstioInjectionConfigCallable(const Model::UpdateIstioInjectionConfigRequest& request) const;
			UpdateMeshFeatureOutcome updateMeshFeature(const Model::UpdateMeshFeatureRequest &request)const;
			void updateMeshFeatureAsync(const Model::UpdateMeshFeatureRequest& request, const UpdateMeshFeatureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateMeshFeatureOutcomeCallable updateMeshFeatureCallable(const Model::UpdateMeshFeatureRequest& request) const;
			UpgradeMeshVersionOutcome upgradeMeshVersion(const Model::UpgradeMeshVersionRequest &request)const;
			void upgradeMeshVersionAsync(const Model::UpgradeMeshVersionRequest& request, const UpgradeMeshVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpgradeMeshVersionOutcomeCallable upgradeMeshVersionCallable(const Model::UpgradeMeshVersionRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_SERVICEMESH_SERVICEMESHCLIENT_H_

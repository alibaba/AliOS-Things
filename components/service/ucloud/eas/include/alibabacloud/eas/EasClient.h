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

#ifndef ALIBABACLOUD_EAS_EASCLIENT_H_
#define ALIBABACLOUD_EAS_EASCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RoaServiceClient.h>
#include "EasExport.h"
#include "model/CheckServiceExistsRequest.h"
#include "model/CheckServiceExistsResult.h"
#include "model/CreateNetworkInterfaceRequest.h"
#include "model/CreateNetworkInterfaceResult.h"
#include "model/CreateSLSRequest.h"
#include "model/CreateSLSResult.h"
#include "model/CreateServiceRequest.h"
#include "model/CreateServiceResult.h"
#include "model/CreateTaskRequest.h"
#include "model/CreateTaskResult.h"
#include "model/DeleteResourceRequest.h"
#include "model/DeleteResourceResult.h"
#include "model/DeleteResourceInstanceRequest.h"
#include "model/DeleteResourceInstanceResult.h"
#include "model/DeleteServiceRequest.h"
#include "model/DeleteServiceResult.h"
#include "model/DeleteTaskRequest.h"
#include "model/DeleteTaskResult.h"
#include "model/GetAvailableRegionsRequest.h"
#include "model/GetAvailableRegionsResult.h"
#include "model/GetNetworkInterfaceRequest.h"
#include "model/GetNetworkInterfaceResult.h"
#include "model/GetOrCreateResourceRequest.h"
#include "model/GetOrCreateResourceResult.h"
#include "model/GetOssAuthorizeRequest.h"
#include "model/GetOssAuthorizeResult.h"
#include "model/GetResourceInstancesRequest.h"
#include "model/GetResourceInstancesResult.h"
#include "model/GetResourcePodsRequest.h"
#include "model/GetResourcePodsResult.h"
#include "model/GetResourceServicesRequest.h"
#include "model/GetResourceServicesResult.h"
#include "model/GetResourceStatusRequest.h"
#include "model/GetResourceStatusResult.h"
#include "model/GetServiceRequest.h"
#include "model/GetServiceResult.h"
#include "model/GetServiceRegionRequest.h"
#include "model/GetServiceRegionResult.h"
#include "model/GetServiceStatisticsRequest.h"
#include "model/GetServiceStatisticsResult.h"
#include "model/GetServiceTokenRequest.h"
#include "model/GetServiceTokenResult.h"
#include "model/GetServiceVersionRequest.h"
#include "model/GetServiceVersionResult.h"
#include "model/GetServiceWorkerRequest.h"
#include "model/GetServiceWorkerResult.h"
#include "model/GetTaskRequest.h"
#include "model/GetTaskResult.h"
#include "model/ListServicesRequest.h"
#include "model/ListServicesResult.h"
#include "model/ListTasksRequest.h"
#include "model/ListTasksResult.h"
#include "model/ListVIndexRequest.h"
#include "model/ListVIndexResult.h"
#include "model/ModifyServiceRequest.h"
#include "model/ModifyServiceResult.h"
#include "model/ModifyServiceVersionRequest.h"
#include "model/ModifyServiceVersionResult.h"
#include "model/ReleaseServiceRequest.h"
#include "model/ReleaseServiceResult.h"
#include "model/StartServiceRequest.h"
#include "model/StartServiceResult.h"
#include "model/StopServiceRequest.h"
#include "model/StopServiceResult.h"
#include "model/SyncServiceRequest.h"
#include "model/SyncServiceResult.h"


namespace AlibabaCloud
{
	namespace Eas
	{
		class ALIBABACLOUD_EAS_EXPORT EasClient : public RoaServiceClient
		{
		public:
			typedef Outcome<Error, Model::CheckServiceExistsResult> CheckServiceExistsOutcome;
			typedef std::future<CheckServiceExistsOutcome> CheckServiceExistsOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::CheckServiceExistsRequest&, const CheckServiceExistsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CheckServiceExistsAsyncHandler;
			typedef Outcome<Error, Model::CreateNetworkInterfaceResult> CreateNetworkInterfaceOutcome;
			typedef std::future<CreateNetworkInterfaceOutcome> CreateNetworkInterfaceOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::CreateNetworkInterfaceRequest&, const CreateNetworkInterfaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateNetworkInterfaceAsyncHandler;
			typedef Outcome<Error, Model::CreateSLSResult> CreateSLSOutcome;
			typedef std::future<CreateSLSOutcome> CreateSLSOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::CreateSLSRequest&, const CreateSLSOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateSLSAsyncHandler;
			typedef Outcome<Error, Model::CreateServiceResult> CreateServiceOutcome;
			typedef std::future<CreateServiceOutcome> CreateServiceOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::CreateServiceRequest&, const CreateServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateServiceAsyncHandler;
			typedef Outcome<Error, Model::CreateTaskResult> CreateTaskOutcome;
			typedef std::future<CreateTaskOutcome> CreateTaskOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::CreateTaskRequest&, const CreateTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateTaskAsyncHandler;
			typedef Outcome<Error, Model::DeleteResourceResult> DeleteResourceOutcome;
			typedef std::future<DeleteResourceOutcome> DeleteResourceOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::DeleteResourceRequest&, const DeleteResourceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteResourceAsyncHandler;
			typedef Outcome<Error, Model::DeleteResourceInstanceResult> DeleteResourceInstanceOutcome;
			typedef std::future<DeleteResourceInstanceOutcome> DeleteResourceInstanceOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::DeleteResourceInstanceRequest&, const DeleteResourceInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteResourceInstanceAsyncHandler;
			typedef Outcome<Error, Model::DeleteServiceResult> DeleteServiceOutcome;
			typedef std::future<DeleteServiceOutcome> DeleteServiceOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::DeleteServiceRequest&, const DeleteServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteServiceAsyncHandler;
			typedef Outcome<Error, Model::DeleteTaskResult> DeleteTaskOutcome;
			typedef std::future<DeleteTaskOutcome> DeleteTaskOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::DeleteTaskRequest&, const DeleteTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteTaskAsyncHandler;
			typedef Outcome<Error, Model::GetAvailableRegionsResult> GetAvailableRegionsOutcome;
			typedef std::future<GetAvailableRegionsOutcome> GetAvailableRegionsOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetAvailableRegionsRequest&, const GetAvailableRegionsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetAvailableRegionsAsyncHandler;
			typedef Outcome<Error, Model::GetNetworkInterfaceResult> GetNetworkInterfaceOutcome;
			typedef std::future<GetNetworkInterfaceOutcome> GetNetworkInterfaceOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetNetworkInterfaceRequest&, const GetNetworkInterfaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetNetworkInterfaceAsyncHandler;
			typedef Outcome<Error, Model::GetOrCreateResourceResult> GetOrCreateResourceOutcome;
			typedef std::future<GetOrCreateResourceOutcome> GetOrCreateResourceOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetOrCreateResourceRequest&, const GetOrCreateResourceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetOrCreateResourceAsyncHandler;
			typedef Outcome<Error, Model::GetOssAuthorizeResult> GetOssAuthorizeOutcome;
			typedef std::future<GetOssAuthorizeOutcome> GetOssAuthorizeOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetOssAuthorizeRequest&, const GetOssAuthorizeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetOssAuthorizeAsyncHandler;
			typedef Outcome<Error, Model::GetResourceInstancesResult> GetResourceInstancesOutcome;
			typedef std::future<GetResourceInstancesOutcome> GetResourceInstancesOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetResourceInstancesRequest&, const GetResourceInstancesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetResourceInstancesAsyncHandler;
			typedef Outcome<Error, Model::GetResourcePodsResult> GetResourcePodsOutcome;
			typedef std::future<GetResourcePodsOutcome> GetResourcePodsOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetResourcePodsRequest&, const GetResourcePodsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetResourcePodsAsyncHandler;
			typedef Outcome<Error, Model::GetResourceServicesResult> GetResourceServicesOutcome;
			typedef std::future<GetResourceServicesOutcome> GetResourceServicesOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetResourceServicesRequest&, const GetResourceServicesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetResourceServicesAsyncHandler;
			typedef Outcome<Error, Model::GetResourceStatusResult> GetResourceStatusOutcome;
			typedef std::future<GetResourceStatusOutcome> GetResourceStatusOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetResourceStatusRequest&, const GetResourceStatusOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetResourceStatusAsyncHandler;
			typedef Outcome<Error, Model::GetServiceResult> GetServiceOutcome;
			typedef std::future<GetServiceOutcome> GetServiceOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetServiceRequest&, const GetServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetServiceAsyncHandler;
			typedef Outcome<Error, Model::GetServiceRegionResult> GetServiceRegionOutcome;
			typedef std::future<GetServiceRegionOutcome> GetServiceRegionOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetServiceRegionRequest&, const GetServiceRegionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetServiceRegionAsyncHandler;
			typedef Outcome<Error, Model::GetServiceStatisticsResult> GetServiceStatisticsOutcome;
			typedef std::future<GetServiceStatisticsOutcome> GetServiceStatisticsOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetServiceStatisticsRequest&, const GetServiceStatisticsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetServiceStatisticsAsyncHandler;
			typedef Outcome<Error, Model::GetServiceTokenResult> GetServiceTokenOutcome;
			typedef std::future<GetServiceTokenOutcome> GetServiceTokenOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetServiceTokenRequest&, const GetServiceTokenOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetServiceTokenAsyncHandler;
			typedef Outcome<Error, Model::GetServiceVersionResult> GetServiceVersionOutcome;
			typedef std::future<GetServiceVersionOutcome> GetServiceVersionOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetServiceVersionRequest&, const GetServiceVersionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetServiceVersionAsyncHandler;
			typedef Outcome<Error, Model::GetServiceWorkerResult> GetServiceWorkerOutcome;
			typedef std::future<GetServiceWorkerOutcome> GetServiceWorkerOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetServiceWorkerRequest&, const GetServiceWorkerOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetServiceWorkerAsyncHandler;
			typedef Outcome<Error, Model::GetTaskResult> GetTaskOutcome;
			typedef std::future<GetTaskOutcome> GetTaskOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::GetTaskRequest&, const GetTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetTaskAsyncHandler;
			typedef Outcome<Error, Model::ListServicesResult> ListServicesOutcome;
			typedef std::future<ListServicesOutcome> ListServicesOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::ListServicesRequest&, const ListServicesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListServicesAsyncHandler;
			typedef Outcome<Error, Model::ListTasksResult> ListTasksOutcome;
			typedef std::future<ListTasksOutcome> ListTasksOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::ListTasksRequest&, const ListTasksOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListTasksAsyncHandler;
			typedef Outcome<Error, Model::ListVIndexResult> ListVIndexOutcome;
			typedef std::future<ListVIndexOutcome> ListVIndexOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::ListVIndexRequest&, const ListVIndexOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListVIndexAsyncHandler;
			typedef Outcome<Error, Model::ModifyServiceResult> ModifyServiceOutcome;
			typedef std::future<ModifyServiceOutcome> ModifyServiceOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::ModifyServiceRequest&, const ModifyServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyServiceAsyncHandler;
			typedef Outcome<Error, Model::ModifyServiceVersionResult> ModifyServiceVersionOutcome;
			typedef std::future<ModifyServiceVersionOutcome> ModifyServiceVersionOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::ModifyServiceVersionRequest&, const ModifyServiceVersionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyServiceVersionAsyncHandler;
			typedef Outcome<Error, Model::ReleaseServiceResult> ReleaseServiceOutcome;
			typedef std::future<ReleaseServiceOutcome> ReleaseServiceOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::ReleaseServiceRequest&, const ReleaseServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ReleaseServiceAsyncHandler;
			typedef Outcome<Error, Model::StartServiceResult> StartServiceOutcome;
			typedef std::future<StartServiceOutcome> StartServiceOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::StartServiceRequest&, const StartServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartServiceAsyncHandler;
			typedef Outcome<Error, Model::StopServiceResult> StopServiceOutcome;
			typedef std::future<StopServiceOutcome> StopServiceOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::StopServiceRequest&, const StopServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StopServiceAsyncHandler;
			typedef Outcome<Error, Model::SyncServiceResult> SyncServiceOutcome;
			typedef std::future<SyncServiceOutcome> SyncServiceOutcomeCallable;
			typedef std::function<void(const EasClient*, const Model::SyncServiceRequest&, const SyncServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SyncServiceAsyncHandler;

			EasClient(const Credentials &credentials, const ClientConfiguration &configuration);
			EasClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			EasClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~EasClient();
			CheckServiceExistsOutcome checkServiceExists(const Model::CheckServiceExistsRequest &request)const;
			void checkServiceExistsAsync(const Model::CheckServiceExistsRequest& request, const CheckServiceExistsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CheckServiceExistsOutcomeCallable checkServiceExistsCallable(const Model::CheckServiceExistsRequest& request) const;
			CreateNetworkInterfaceOutcome createNetworkInterface(const Model::CreateNetworkInterfaceRequest &request)const;
			void createNetworkInterfaceAsync(const Model::CreateNetworkInterfaceRequest& request, const CreateNetworkInterfaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateNetworkInterfaceOutcomeCallable createNetworkInterfaceCallable(const Model::CreateNetworkInterfaceRequest& request) const;
			CreateSLSOutcome createSLS(const Model::CreateSLSRequest &request)const;
			void createSLSAsync(const Model::CreateSLSRequest& request, const CreateSLSAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateSLSOutcomeCallable createSLSCallable(const Model::CreateSLSRequest& request) const;
			CreateServiceOutcome createService(const Model::CreateServiceRequest &request)const;
			void createServiceAsync(const Model::CreateServiceRequest& request, const CreateServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateServiceOutcomeCallable createServiceCallable(const Model::CreateServiceRequest& request) const;
			CreateTaskOutcome createTask(const Model::CreateTaskRequest &request)const;
			void createTaskAsync(const Model::CreateTaskRequest& request, const CreateTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateTaskOutcomeCallable createTaskCallable(const Model::CreateTaskRequest& request) const;
			DeleteResourceOutcome deleteResource(const Model::DeleteResourceRequest &request)const;
			void deleteResourceAsync(const Model::DeleteResourceRequest& request, const DeleteResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteResourceOutcomeCallable deleteResourceCallable(const Model::DeleteResourceRequest& request) const;
			DeleteResourceInstanceOutcome deleteResourceInstance(const Model::DeleteResourceInstanceRequest &request)const;
			void deleteResourceInstanceAsync(const Model::DeleteResourceInstanceRequest& request, const DeleteResourceInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteResourceInstanceOutcomeCallable deleteResourceInstanceCallable(const Model::DeleteResourceInstanceRequest& request) const;
			DeleteServiceOutcome deleteService(const Model::DeleteServiceRequest &request)const;
			void deleteServiceAsync(const Model::DeleteServiceRequest& request, const DeleteServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteServiceOutcomeCallable deleteServiceCallable(const Model::DeleteServiceRequest& request) const;
			DeleteTaskOutcome deleteTask(const Model::DeleteTaskRequest &request)const;
			void deleteTaskAsync(const Model::DeleteTaskRequest& request, const DeleteTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteTaskOutcomeCallable deleteTaskCallable(const Model::DeleteTaskRequest& request) const;
			GetAvailableRegionsOutcome getAvailableRegions(const Model::GetAvailableRegionsRequest &request)const;
			void getAvailableRegionsAsync(const Model::GetAvailableRegionsRequest& request, const GetAvailableRegionsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetAvailableRegionsOutcomeCallable getAvailableRegionsCallable(const Model::GetAvailableRegionsRequest& request) const;
			GetNetworkInterfaceOutcome getNetworkInterface(const Model::GetNetworkInterfaceRequest &request)const;
			void getNetworkInterfaceAsync(const Model::GetNetworkInterfaceRequest& request, const GetNetworkInterfaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetNetworkInterfaceOutcomeCallable getNetworkInterfaceCallable(const Model::GetNetworkInterfaceRequest& request) const;
			GetOrCreateResourceOutcome getOrCreateResource(const Model::GetOrCreateResourceRequest &request)const;
			void getOrCreateResourceAsync(const Model::GetOrCreateResourceRequest& request, const GetOrCreateResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetOrCreateResourceOutcomeCallable getOrCreateResourceCallable(const Model::GetOrCreateResourceRequest& request) const;
			GetOssAuthorizeOutcome getOssAuthorize(const Model::GetOssAuthorizeRequest &request)const;
			void getOssAuthorizeAsync(const Model::GetOssAuthorizeRequest& request, const GetOssAuthorizeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetOssAuthorizeOutcomeCallable getOssAuthorizeCallable(const Model::GetOssAuthorizeRequest& request) const;
			GetResourceInstancesOutcome getResourceInstances(const Model::GetResourceInstancesRequest &request)const;
			void getResourceInstancesAsync(const Model::GetResourceInstancesRequest& request, const GetResourceInstancesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetResourceInstancesOutcomeCallable getResourceInstancesCallable(const Model::GetResourceInstancesRequest& request) const;
			GetResourcePodsOutcome getResourcePods(const Model::GetResourcePodsRequest &request)const;
			void getResourcePodsAsync(const Model::GetResourcePodsRequest& request, const GetResourcePodsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetResourcePodsOutcomeCallable getResourcePodsCallable(const Model::GetResourcePodsRequest& request) const;
			GetResourceServicesOutcome getResourceServices(const Model::GetResourceServicesRequest &request)const;
			void getResourceServicesAsync(const Model::GetResourceServicesRequest& request, const GetResourceServicesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetResourceServicesOutcomeCallable getResourceServicesCallable(const Model::GetResourceServicesRequest& request) const;
			GetResourceStatusOutcome getResourceStatus(const Model::GetResourceStatusRequest &request)const;
			void getResourceStatusAsync(const Model::GetResourceStatusRequest& request, const GetResourceStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetResourceStatusOutcomeCallable getResourceStatusCallable(const Model::GetResourceStatusRequest& request) const;
			GetServiceOutcome getService(const Model::GetServiceRequest &request)const;
			void getServiceAsync(const Model::GetServiceRequest& request, const GetServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetServiceOutcomeCallable getServiceCallable(const Model::GetServiceRequest& request) const;
			GetServiceRegionOutcome getServiceRegion(const Model::GetServiceRegionRequest &request)const;
			void getServiceRegionAsync(const Model::GetServiceRegionRequest& request, const GetServiceRegionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetServiceRegionOutcomeCallable getServiceRegionCallable(const Model::GetServiceRegionRequest& request) const;
			GetServiceStatisticsOutcome getServiceStatistics(const Model::GetServiceStatisticsRequest &request)const;
			void getServiceStatisticsAsync(const Model::GetServiceStatisticsRequest& request, const GetServiceStatisticsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetServiceStatisticsOutcomeCallable getServiceStatisticsCallable(const Model::GetServiceStatisticsRequest& request) const;
			GetServiceTokenOutcome getServiceToken(const Model::GetServiceTokenRequest &request)const;
			void getServiceTokenAsync(const Model::GetServiceTokenRequest& request, const GetServiceTokenAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetServiceTokenOutcomeCallable getServiceTokenCallable(const Model::GetServiceTokenRequest& request) const;
			GetServiceVersionOutcome getServiceVersion(const Model::GetServiceVersionRequest &request)const;
			void getServiceVersionAsync(const Model::GetServiceVersionRequest& request, const GetServiceVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetServiceVersionOutcomeCallable getServiceVersionCallable(const Model::GetServiceVersionRequest& request) const;
			GetServiceWorkerOutcome getServiceWorker(const Model::GetServiceWorkerRequest &request)const;
			void getServiceWorkerAsync(const Model::GetServiceWorkerRequest& request, const GetServiceWorkerAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetServiceWorkerOutcomeCallable getServiceWorkerCallable(const Model::GetServiceWorkerRequest& request) const;
			GetTaskOutcome getTask(const Model::GetTaskRequest &request)const;
			void getTaskAsync(const Model::GetTaskRequest& request, const GetTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetTaskOutcomeCallable getTaskCallable(const Model::GetTaskRequest& request) const;
			ListServicesOutcome listServices(const Model::ListServicesRequest &request)const;
			void listServicesAsync(const Model::ListServicesRequest& request, const ListServicesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListServicesOutcomeCallable listServicesCallable(const Model::ListServicesRequest& request) const;
			ListTasksOutcome listTasks(const Model::ListTasksRequest &request)const;
			void listTasksAsync(const Model::ListTasksRequest& request, const ListTasksAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListTasksOutcomeCallable listTasksCallable(const Model::ListTasksRequest& request) const;
			ListVIndexOutcome listVIndex(const Model::ListVIndexRequest &request)const;
			void listVIndexAsync(const Model::ListVIndexRequest& request, const ListVIndexAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListVIndexOutcomeCallable listVIndexCallable(const Model::ListVIndexRequest& request) const;
			ModifyServiceOutcome modifyService(const Model::ModifyServiceRequest &request)const;
			void modifyServiceAsync(const Model::ModifyServiceRequest& request, const ModifyServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyServiceOutcomeCallable modifyServiceCallable(const Model::ModifyServiceRequest& request) const;
			ModifyServiceVersionOutcome modifyServiceVersion(const Model::ModifyServiceVersionRequest &request)const;
			void modifyServiceVersionAsync(const Model::ModifyServiceVersionRequest& request, const ModifyServiceVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyServiceVersionOutcomeCallable modifyServiceVersionCallable(const Model::ModifyServiceVersionRequest& request) const;
			ReleaseServiceOutcome releaseService(const Model::ReleaseServiceRequest &request)const;
			void releaseServiceAsync(const Model::ReleaseServiceRequest& request, const ReleaseServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ReleaseServiceOutcomeCallable releaseServiceCallable(const Model::ReleaseServiceRequest& request) const;
			StartServiceOutcome startService(const Model::StartServiceRequest &request)const;
			void startServiceAsync(const Model::StartServiceRequest& request, const StartServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartServiceOutcomeCallable startServiceCallable(const Model::StartServiceRequest& request) const;
			StopServiceOutcome stopService(const Model::StopServiceRequest &request)const;
			void stopServiceAsync(const Model::StopServiceRequest& request, const StopServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StopServiceOutcomeCallable stopServiceCallable(const Model::StopServiceRequest& request) const;
			SyncServiceOutcome syncService(const Model::SyncServiceRequest &request)const;
			void syncServiceAsync(const Model::SyncServiceRequest& request, const SyncServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SyncServiceOutcomeCallable syncServiceCallable(const Model::SyncServiceRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_EAS_EASCLIENT_H_

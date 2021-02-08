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

#ifndef ALIBABACLOUD_CONFIG_CONFIGCLIENT_H_
#define ALIBABACLOUD_CONFIG_CONFIGCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "ConfigExport.h"
#include "model/ActiveConfigRulesRequest.h"
#include "model/ActiveConfigRulesResult.h"
#include "model/DeleteConfigRulesRequest.h"
#include "model/DeleteConfigRulesResult.h"
#include "model/DescribeComplianceRequest.h"
#include "model/DescribeComplianceResult.h"
#include "model/DescribeComplianceSummaryRequest.h"
#include "model/DescribeComplianceSummaryResult.h"
#include "model/DescribeConfigRuleRequest.h"
#include "model/DescribeConfigRuleResult.h"
#include "model/DescribeConfigurationRecorderRequest.h"
#include "model/DescribeConfigurationRecorderResult.h"
#include "model/DescribeDeliveryChannelsRequest.h"
#include "model/DescribeDeliveryChannelsResult.h"
#include "model/DescribeDiscoveredResourceRequest.h"
#include "model/DescribeDiscoveredResourceResult.h"
#include "model/DescribeEvaluationResultsRequest.h"
#include "model/DescribeEvaluationResultsResult.h"
#include "model/GetDiscoveredResourceCountsRequest.h"
#include "model/GetDiscoveredResourceCountsResult.h"
#include "model/GetDiscoveredResourceSummaryRequest.h"
#include "model/GetDiscoveredResourceSummaryResult.h"
#include "model/GetResourceComplianceTimelineRequest.h"
#include "model/GetResourceComplianceTimelineResult.h"
#include "model/GetResourceConfigurationTimelineRequest.h"
#include "model/GetResourceConfigurationTimelineResult.h"
#include "model/GetSupportedResourceTypesRequest.h"
#include "model/GetSupportedResourceTypesResult.h"
#include "model/ListConfigRulesRequest.h"
#include "model/ListConfigRulesResult.h"
#include "model/ListDiscoveredResourcesRequest.h"
#include "model/ListDiscoveredResourcesResult.h"
#include "model/PutConfigRuleRequest.h"
#include "model/PutConfigRuleResult.h"
#include "model/PutConfigurationRecorderRequest.h"
#include "model/PutConfigurationRecorderResult.h"
#include "model/PutDeliveryChannelRequest.h"
#include "model/PutDeliveryChannelResult.h"
#include "model/PutEvaluationsRequest.h"
#include "model/PutEvaluationsResult.h"
#include "model/StartConfigRuleEvaluationRequest.h"
#include "model/StartConfigRuleEvaluationResult.h"
#include "model/StartConfigurationRecorderRequest.h"
#include "model/StartConfigurationRecorderResult.h"
#include "model/StopConfigRulesRequest.h"
#include "model/StopConfigRulesResult.h"


namespace AlibabaCloud
{
	namespace Config
	{
		class ALIBABACLOUD_CONFIG_EXPORT ConfigClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::ActiveConfigRulesResult> ActiveConfigRulesOutcome;
			typedef std::future<ActiveConfigRulesOutcome> ActiveConfigRulesOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::ActiveConfigRulesRequest&, const ActiveConfigRulesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ActiveConfigRulesAsyncHandler;
			typedef Outcome<Error, Model::DeleteConfigRulesResult> DeleteConfigRulesOutcome;
			typedef std::future<DeleteConfigRulesOutcome> DeleteConfigRulesOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::DeleteConfigRulesRequest&, const DeleteConfigRulesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteConfigRulesAsyncHandler;
			typedef Outcome<Error, Model::DescribeComplianceResult> DescribeComplianceOutcome;
			typedef std::future<DescribeComplianceOutcome> DescribeComplianceOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::DescribeComplianceRequest&, const DescribeComplianceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeComplianceAsyncHandler;
			typedef Outcome<Error, Model::DescribeComplianceSummaryResult> DescribeComplianceSummaryOutcome;
			typedef std::future<DescribeComplianceSummaryOutcome> DescribeComplianceSummaryOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::DescribeComplianceSummaryRequest&, const DescribeComplianceSummaryOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeComplianceSummaryAsyncHandler;
			typedef Outcome<Error, Model::DescribeConfigRuleResult> DescribeConfigRuleOutcome;
			typedef std::future<DescribeConfigRuleOutcome> DescribeConfigRuleOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::DescribeConfigRuleRequest&, const DescribeConfigRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeConfigRuleAsyncHandler;
			typedef Outcome<Error, Model::DescribeConfigurationRecorderResult> DescribeConfigurationRecorderOutcome;
			typedef std::future<DescribeConfigurationRecorderOutcome> DescribeConfigurationRecorderOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::DescribeConfigurationRecorderRequest&, const DescribeConfigurationRecorderOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeConfigurationRecorderAsyncHandler;
			typedef Outcome<Error, Model::DescribeDeliveryChannelsResult> DescribeDeliveryChannelsOutcome;
			typedef std::future<DescribeDeliveryChannelsOutcome> DescribeDeliveryChannelsOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::DescribeDeliveryChannelsRequest&, const DescribeDeliveryChannelsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeDeliveryChannelsAsyncHandler;
			typedef Outcome<Error, Model::DescribeDiscoveredResourceResult> DescribeDiscoveredResourceOutcome;
			typedef std::future<DescribeDiscoveredResourceOutcome> DescribeDiscoveredResourceOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::DescribeDiscoveredResourceRequest&, const DescribeDiscoveredResourceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeDiscoveredResourceAsyncHandler;
			typedef Outcome<Error, Model::DescribeEvaluationResultsResult> DescribeEvaluationResultsOutcome;
			typedef std::future<DescribeEvaluationResultsOutcome> DescribeEvaluationResultsOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::DescribeEvaluationResultsRequest&, const DescribeEvaluationResultsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeEvaluationResultsAsyncHandler;
			typedef Outcome<Error, Model::GetDiscoveredResourceCountsResult> GetDiscoveredResourceCountsOutcome;
			typedef std::future<GetDiscoveredResourceCountsOutcome> GetDiscoveredResourceCountsOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::GetDiscoveredResourceCountsRequest&, const GetDiscoveredResourceCountsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDiscoveredResourceCountsAsyncHandler;
			typedef Outcome<Error, Model::GetDiscoveredResourceSummaryResult> GetDiscoveredResourceSummaryOutcome;
			typedef std::future<GetDiscoveredResourceSummaryOutcome> GetDiscoveredResourceSummaryOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::GetDiscoveredResourceSummaryRequest&, const GetDiscoveredResourceSummaryOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDiscoveredResourceSummaryAsyncHandler;
			typedef Outcome<Error, Model::GetResourceComplianceTimelineResult> GetResourceComplianceTimelineOutcome;
			typedef std::future<GetResourceComplianceTimelineOutcome> GetResourceComplianceTimelineOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::GetResourceComplianceTimelineRequest&, const GetResourceComplianceTimelineOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetResourceComplianceTimelineAsyncHandler;
			typedef Outcome<Error, Model::GetResourceConfigurationTimelineResult> GetResourceConfigurationTimelineOutcome;
			typedef std::future<GetResourceConfigurationTimelineOutcome> GetResourceConfigurationTimelineOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::GetResourceConfigurationTimelineRequest&, const GetResourceConfigurationTimelineOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetResourceConfigurationTimelineAsyncHandler;
			typedef Outcome<Error, Model::GetSupportedResourceTypesResult> GetSupportedResourceTypesOutcome;
			typedef std::future<GetSupportedResourceTypesOutcome> GetSupportedResourceTypesOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::GetSupportedResourceTypesRequest&, const GetSupportedResourceTypesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetSupportedResourceTypesAsyncHandler;
			typedef Outcome<Error, Model::ListConfigRulesResult> ListConfigRulesOutcome;
			typedef std::future<ListConfigRulesOutcome> ListConfigRulesOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::ListConfigRulesRequest&, const ListConfigRulesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListConfigRulesAsyncHandler;
			typedef Outcome<Error, Model::ListDiscoveredResourcesResult> ListDiscoveredResourcesOutcome;
			typedef std::future<ListDiscoveredResourcesOutcome> ListDiscoveredResourcesOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::ListDiscoveredResourcesRequest&, const ListDiscoveredResourcesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListDiscoveredResourcesAsyncHandler;
			typedef Outcome<Error, Model::PutConfigRuleResult> PutConfigRuleOutcome;
			typedef std::future<PutConfigRuleOutcome> PutConfigRuleOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::PutConfigRuleRequest&, const PutConfigRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> PutConfigRuleAsyncHandler;
			typedef Outcome<Error, Model::PutConfigurationRecorderResult> PutConfigurationRecorderOutcome;
			typedef std::future<PutConfigurationRecorderOutcome> PutConfigurationRecorderOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::PutConfigurationRecorderRequest&, const PutConfigurationRecorderOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> PutConfigurationRecorderAsyncHandler;
			typedef Outcome<Error, Model::PutDeliveryChannelResult> PutDeliveryChannelOutcome;
			typedef std::future<PutDeliveryChannelOutcome> PutDeliveryChannelOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::PutDeliveryChannelRequest&, const PutDeliveryChannelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> PutDeliveryChannelAsyncHandler;
			typedef Outcome<Error, Model::PutEvaluationsResult> PutEvaluationsOutcome;
			typedef std::future<PutEvaluationsOutcome> PutEvaluationsOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::PutEvaluationsRequest&, const PutEvaluationsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> PutEvaluationsAsyncHandler;
			typedef Outcome<Error, Model::StartConfigRuleEvaluationResult> StartConfigRuleEvaluationOutcome;
			typedef std::future<StartConfigRuleEvaluationOutcome> StartConfigRuleEvaluationOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::StartConfigRuleEvaluationRequest&, const StartConfigRuleEvaluationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartConfigRuleEvaluationAsyncHandler;
			typedef Outcome<Error, Model::StartConfigurationRecorderResult> StartConfigurationRecorderOutcome;
			typedef std::future<StartConfigurationRecorderOutcome> StartConfigurationRecorderOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::StartConfigurationRecorderRequest&, const StartConfigurationRecorderOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartConfigurationRecorderAsyncHandler;
			typedef Outcome<Error, Model::StopConfigRulesResult> StopConfigRulesOutcome;
			typedef std::future<StopConfigRulesOutcome> StopConfigRulesOutcomeCallable;
			typedef std::function<void(const ConfigClient*, const Model::StopConfigRulesRequest&, const StopConfigRulesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StopConfigRulesAsyncHandler;

			ConfigClient(const Credentials &credentials, const ClientConfiguration &configuration);
			ConfigClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			ConfigClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~ConfigClient();
			ActiveConfigRulesOutcome activeConfigRules(const Model::ActiveConfigRulesRequest &request)const;
			void activeConfigRulesAsync(const Model::ActiveConfigRulesRequest& request, const ActiveConfigRulesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ActiveConfigRulesOutcomeCallable activeConfigRulesCallable(const Model::ActiveConfigRulesRequest& request) const;
			DeleteConfigRulesOutcome deleteConfigRules(const Model::DeleteConfigRulesRequest &request)const;
			void deleteConfigRulesAsync(const Model::DeleteConfigRulesRequest& request, const DeleteConfigRulesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteConfigRulesOutcomeCallable deleteConfigRulesCallable(const Model::DeleteConfigRulesRequest& request) const;
			DescribeComplianceOutcome describeCompliance(const Model::DescribeComplianceRequest &request)const;
			void describeComplianceAsync(const Model::DescribeComplianceRequest& request, const DescribeComplianceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeComplianceOutcomeCallable describeComplianceCallable(const Model::DescribeComplianceRequest& request) const;
			DescribeComplianceSummaryOutcome describeComplianceSummary(const Model::DescribeComplianceSummaryRequest &request)const;
			void describeComplianceSummaryAsync(const Model::DescribeComplianceSummaryRequest& request, const DescribeComplianceSummaryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeComplianceSummaryOutcomeCallable describeComplianceSummaryCallable(const Model::DescribeComplianceSummaryRequest& request) const;
			DescribeConfigRuleOutcome describeConfigRule(const Model::DescribeConfigRuleRequest &request)const;
			void describeConfigRuleAsync(const Model::DescribeConfigRuleRequest& request, const DescribeConfigRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeConfigRuleOutcomeCallable describeConfigRuleCallable(const Model::DescribeConfigRuleRequest& request) const;
			DescribeConfigurationRecorderOutcome describeConfigurationRecorder(const Model::DescribeConfigurationRecorderRequest &request)const;
			void describeConfigurationRecorderAsync(const Model::DescribeConfigurationRecorderRequest& request, const DescribeConfigurationRecorderAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeConfigurationRecorderOutcomeCallable describeConfigurationRecorderCallable(const Model::DescribeConfigurationRecorderRequest& request) const;
			DescribeDeliveryChannelsOutcome describeDeliveryChannels(const Model::DescribeDeliveryChannelsRequest &request)const;
			void describeDeliveryChannelsAsync(const Model::DescribeDeliveryChannelsRequest& request, const DescribeDeliveryChannelsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeDeliveryChannelsOutcomeCallable describeDeliveryChannelsCallable(const Model::DescribeDeliveryChannelsRequest& request) const;
			DescribeDiscoveredResourceOutcome describeDiscoveredResource(const Model::DescribeDiscoveredResourceRequest &request)const;
			void describeDiscoveredResourceAsync(const Model::DescribeDiscoveredResourceRequest& request, const DescribeDiscoveredResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeDiscoveredResourceOutcomeCallable describeDiscoveredResourceCallable(const Model::DescribeDiscoveredResourceRequest& request) const;
			DescribeEvaluationResultsOutcome describeEvaluationResults(const Model::DescribeEvaluationResultsRequest &request)const;
			void describeEvaluationResultsAsync(const Model::DescribeEvaluationResultsRequest& request, const DescribeEvaluationResultsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeEvaluationResultsOutcomeCallable describeEvaluationResultsCallable(const Model::DescribeEvaluationResultsRequest& request) const;
			GetDiscoveredResourceCountsOutcome getDiscoveredResourceCounts(const Model::GetDiscoveredResourceCountsRequest &request)const;
			void getDiscoveredResourceCountsAsync(const Model::GetDiscoveredResourceCountsRequest& request, const GetDiscoveredResourceCountsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDiscoveredResourceCountsOutcomeCallable getDiscoveredResourceCountsCallable(const Model::GetDiscoveredResourceCountsRequest& request) const;
			GetDiscoveredResourceSummaryOutcome getDiscoveredResourceSummary(const Model::GetDiscoveredResourceSummaryRequest &request)const;
			void getDiscoveredResourceSummaryAsync(const Model::GetDiscoveredResourceSummaryRequest& request, const GetDiscoveredResourceSummaryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDiscoveredResourceSummaryOutcomeCallable getDiscoveredResourceSummaryCallable(const Model::GetDiscoveredResourceSummaryRequest& request) const;
			GetResourceComplianceTimelineOutcome getResourceComplianceTimeline(const Model::GetResourceComplianceTimelineRequest &request)const;
			void getResourceComplianceTimelineAsync(const Model::GetResourceComplianceTimelineRequest& request, const GetResourceComplianceTimelineAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetResourceComplianceTimelineOutcomeCallable getResourceComplianceTimelineCallable(const Model::GetResourceComplianceTimelineRequest& request) const;
			GetResourceConfigurationTimelineOutcome getResourceConfigurationTimeline(const Model::GetResourceConfigurationTimelineRequest &request)const;
			void getResourceConfigurationTimelineAsync(const Model::GetResourceConfigurationTimelineRequest& request, const GetResourceConfigurationTimelineAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetResourceConfigurationTimelineOutcomeCallable getResourceConfigurationTimelineCallable(const Model::GetResourceConfigurationTimelineRequest& request) const;
			GetSupportedResourceTypesOutcome getSupportedResourceTypes(const Model::GetSupportedResourceTypesRequest &request)const;
			void getSupportedResourceTypesAsync(const Model::GetSupportedResourceTypesRequest& request, const GetSupportedResourceTypesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetSupportedResourceTypesOutcomeCallable getSupportedResourceTypesCallable(const Model::GetSupportedResourceTypesRequest& request) const;
			ListConfigRulesOutcome listConfigRules(const Model::ListConfigRulesRequest &request)const;
			void listConfigRulesAsync(const Model::ListConfigRulesRequest& request, const ListConfigRulesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListConfigRulesOutcomeCallable listConfigRulesCallable(const Model::ListConfigRulesRequest& request) const;
			ListDiscoveredResourcesOutcome listDiscoveredResources(const Model::ListDiscoveredResourcesRequest &request)const;
			void listDiscoveredResourcesAsync(const Model::ListDiscoveredResourcesRequest& request, const ListDiscoveredResourcesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListDiscoveredResourcesOutcomeCallable listDiscoveredResourcesCallable(const Model::ListDiscoveredResourcesRequest& request) const;
			PutConfigRuleOutcome putConfigRule(const Model::PutConfigRuleRequest &request)const;
			void putConfigRuleAsync(const Model::PutConfigRuleRequest& request, const PutConfigRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			PutConfigRuleOutcomeCallable putConfigRuleCallable(const Model::PutConfigRuleRequest& request) const;
			PutConfigurationRecorderOutcome putConfigurationRecorder(const Model::PutConfigurationRecorderRequest &request)const;
			void putConfigurationRecorderAsync(const Model::PutConfigurationRecorderRequest& request, const PutConfigurationRecorderAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			PutConfigurationRecorderOutcomeCallable putConfigurationRecorderCallable(const Model::PutConfigurationRecorderRequest& request) const;
			PutDeliveryChannelOutcome putDeliveryChannel(const Model::PutDeliveryChannelRequest &request)const;
			void putDeliveryChannelAsync(const Model::PutDeliveryChannelRequest& request, const PutDeliveryChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			PutDeliveryChannelOutcomeCallable putDeliveryChannelCallable(const Model::PutDeliveryChannelRequest& request) const;
			PutEvaluationsOutcome putEvaluations(const Model::PutEvaluationsRequest &request)const;
			void putEvaluationsAsync(const Model::PutEvaluationsRequest& request, const PutEvaluationsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			PutEvaluationsOutcomeCallable putEvaluationsCallable(const Model::PutEvaluationsRequest& request) const;
			StartConfigRuleEvaluationOutcome startConfigRuleEvaluation(const Model::StartConfigRuleEvaluationRequest &request)const;
			void startConfigRuleEvaluationAsync(const Model::StartConfigRuleEvaluationRequest& request, const StartConfigRuleEvaluationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartConfigRuleEvaluationOutcomeCallable startConfigRuleEvaluationCallable(const Model::StartConfigRuleEvaluationRequest& request) const;
			StartConfigurationRecorderOutcome startConfigurationRecorder(const Model::StartConfigurationRecorderRequest &request)const;
			void startConfigurationRecorderAsync(const Model::StartConfigurationRecorderRequest& request, const StartConfigurationRecorderAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartConfigurationRecorderOutcomeCallable startConfigurationRecorderCallable(const Model::StartConfigurationRecorderRequest& request) const;
			StopConfigRulesOutcome stopConfigRules(const Model::StopConfigRulesRequest &request)const;
			void stopConfigRulesAsync(const Model::StopConfigRulesRequest& request, const StopConfigRulesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StopConfigRulesOutcomeCallable stopConfigRulesCallable(const Model::StopConfigRulesRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_CONFIG_CONFIGCLIENT_H_

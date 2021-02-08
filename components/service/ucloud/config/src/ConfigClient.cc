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

#include <alibabacloud/config/ConfigClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Config;
using namespace AlibabaCloud::Config::Model;

namespace
{
	const std::string SERVICE_NAME = "Config";
}

ConfigClient::ConfigClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "Config");
}

ConfigClient::ConfigClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "Config");
}

ConfigClient::ConfigClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "Config");
}

ConfigClient::~ConfigClient()
{}

ConfigClient::ActiveConfigRulesOutcome ConfigClient::activeConfigRules(const ActiveConfigRulesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ActiveConfigRulesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ActiveConfigRulesOutcome(ActiveConfigRulesResult(outcome.result()));
	else
		return ActiveConfigRulesOutcome(outcome.error());
}

void ConfigClient::activeConfigRulesAsync(const ActiveConfigRulesRequest& request, const ActiveConfigRulesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, activeConfigRules(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::ActiveConfigRulesOutcomeCallable ConfigClient::activeConfigRulesCallable(const ActiveConfigRulesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ActiveConfigRulesOutcome()>>(
			[this, request]()
			{
			return this->activeConfigRules(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::DeleteConfigRulesOutcome ConfigClient::deleteConfigRules(const DeleteConfigRulesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteConfigRulesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteConfigRulesOutcome(DeleteConfigRulesResult(outcome.result()));
	else
		return DeleteConfigRulesOutcome(outcome.error());
}

void ConfigClient::deleteConfigRulesAsync(const DeleteConfigRulesRequest& request, const DeleteConfigRulesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteConfigRules(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::DeleteConfigRulesOutcomeCallable ConfigClient::deleteConfigRulesCallable(const DeleteConfigRulesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteConfigRulesOutcome()>>(
			[this, request]()
			{
			return this->deleteConfigRules(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::DescribeComplianceOutcome ConfigClient::describeCompliance(const DescribeComplianceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeComplianceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeComplianceOutcome(DescribeComplianceResult(outcome.result()));
	else
		return DescribeComplianceOutcome(outcome.error());
}

void ConfigClient::describeComplianceAsync(const DescribeComplianceRequest& request, const DescribeComplianceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCompliance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::DescribeComplianceOutcomeCallable ConfigClient::describeComplianceCallable(const DescribeComplianceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeComplianceOutcome()>>(
			[this, request]()
			{
			return this->describeCompliance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::DescribeComplianceSummaryOutcome ConfigClient::describeComplianceSummary(const DescribeComplianceSummaryRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeComplianceSummaryOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeComplianceSummaryOutcome(DescribeComplianceSummaryResult(outcome.result()));
	else
		return DescribeComplianceSummaryOutcome(outcome.error());
}

void ConfigClient::describeComplianceSummaryAsync(const DescribeComplianceSummaryRequest& request, const DescribeComplianceSummaryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeComplianceSummary(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::DescribeComplianceSummaryOutcomeCallable ConfigClient::describeComplianceSummaryCallable(const DescribeComplianceSummaryRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeComplianceSummaryOutcome()>>(
			[this, request]()
			{
			return this->describeComplianceSummary(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::DescribeConfigRuleOutcome ConfigClient::describeConfigRule(const DescribeConfigRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeConfigRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeConfigRuleOutcome(DescribeConfigRuleResult(outcome.result()));
	else
		return DescribeConfigRuleOutcome(outcome.error());
}

void ConfigClient::describeConfigRuleAsync(const DescribeConfigRuleRequest& request, const DescribeConfigRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeConfigRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::DescribeConfigRuleOutcomeCallable ConfigClient::describeConfigRuleCallable(const DescribeConfigRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeConfigRuleOutcome()>>(
			[this, request]()
			{
			return this->describeConfigRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::DescribeConfigurationRecorderOutcome ConfigClient::describeConfigurationRecorder(const DescribeConfigurationRecorderRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeConfigurationRecorderOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeConfigurationRecorderOutcome(DescribeConfigurationRecorderResult(outcome.result()));
	else
		return DescribeConfigurationRecorderOutcome(outcome.error());
}

void ConfigClient::describeConfigurationRecorderAsync(const DescribeConfigurationRecorderRequest& request, const DescribeConfigurationRecorderAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeConfigurationRecorder(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::DescribeConfigurationRecorderOutcomeCallable ConfigClient::describeConfigurationRecorderCallable(const DescribeConfigurationRecorderRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeConfigurationRecorderOutcome()>>(
			[this, request]()
			{
			return this->describeConfigurationRecorder(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::DescribeDeliveryChannelsOutcome ConfigClient::describeDeliveryChannels(const DescribeDeliveryChannelsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeDeliveryChannelsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeDeliveryChannelsOutcome(DescribeDeliveryChannelsResult(outcome.result()));
	else
		return DescribeDeliveryChannelsOutcome(outcome.error());
}

void ConfigClient::describeDeliveryChannelsAsync(const DescribeDeliveryChannelsRequest& request, const DescribeDeliveryChannelsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeDeliveryChannels(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::DescribeDeliveryChannelsOutcomeCallable ConfigClient::describeDeliveryChannelsCallable(const DescribeDeliveryChannelsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeDeliveryChannelsOutcome()>>(
			[this, request]()
			{
			return this->describeDeliveryChannels(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::DescribeDiscoveredResourceOutcome ConfigClient::describeDiscoveredResource(const DescribeDiscoveredResourceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeDiscoveredResourceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeDiscoveredResourceOutcome(DescribeDiscoveredResourceResult(outcome.result()));
	else
		return DescribeDiscoveredResourceOutcome(outcome.error());
}

void ConfigClient::describeDiscoveredResourceAsync(const DescribeDiscoveredResourceRequest& request, const DescribeDiscoveredResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeDiscoveredResource(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::DescribeDiscoveredResourceOutcomeCallable ConfigClient::describeDiscoveredResourceCallable(const DescribeDiscoveredResourceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeDiscoveredResourceOutcome()>>(
			[this, request]()
			{
			return this->describeDiscoveredResource(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::DescribeEvaluationResultsOutcome ConfigClient::describeEvaluationResults(const DescribeEvaluationResultsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeEvaluationResultsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeEvaluationResultsOutcome(DescribeEvaluationResultsResult(outcome.result()));
	else
		return DescribeEvaluationResultsOutcome(outcome.error());
}

void ConfigClient::describeEvaluationResultsAsync(const DescribeEvaluationResultsRequest& request, const DescribeEvaluationResultsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeEvaluationResults(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::DescribeEvaluationResultsOutcomeCallable ConfigClient::describeEvaluationResultsCallable(const DescribeEvaluationResultsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeEvaluationResultsOutcome()>>(
			[this, request]()
			{
			return this->describeEvaluationResults(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::GetDiscoveredResourceCountsOutcome ConfigClient::getDiscoveredResourceCounts(const GetDiscoveredResourceCountsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDiscoveredResourceCountsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDiscoveredResourceCountsOutcome(GetDiscoveredResourceCountsResult(outcome.result()));
	else
		return GetDiscoveredResourceCountsOutcome(outcome.error());
}

void ConfigClient::getDiscoveredResourceCountsAsync(const GetDiscoveredResourceCountsRequest& request, const GetDiscoveredResourceCountsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDiscoveredResourceCounts(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::GetDiscoveredResourceCountsOutcomeCallable ConfigClient::getDiscoveredResourceCountsCallable(const GetDiscoveredResourceCountsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDiscoveredResourceCountsOutcome()>>(
			[this, request]()
			{
			return this->getDiscoveredResourceCounts(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::GetDiscoveredResourceSummaryOutcome ConfigClient::getDiscoveredResourceSummary(const GetDiscoveredResourceSummaryRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDiscoveredResourceSummaryOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDiscoveredResourceSummaryOutcome(GetDiscoveredResourceSummaryResult(outcome.result()));
	else
		return GetDiscoveredResourceSummaryOutcome(outcome.error());
}

void ConfigClient::getDiscoveredResourceSummaryAsync(const GetDiscoveredResourceSummaryRequest& request, const GetDiscoveredResourceSummaryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDiscoveredResourceSummary(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::GetDiscoveredResourceSummaryOutcomeCallable ConfigClient::getDiscoveredResourceSummaryCallable(const GetDiscoveredResourceSummaryRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDiscoveredResourceSummaryOutcome()>>(
			[this, request]()
			{
			return this->getDiscoveredResourceSummary(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::GetResourceComplianceTimelineOutcome ConfigClient::getResourceComplianceTimeline(const GetResourceComplianceTimelineRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetResourceComplianceTimelineOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetResourceComplianceTimelineOutcome(GetResourceComplianceTimelineResult(outcome.result()));
	else
		return GetResourceComplianceTimelineOutcome(outcome.error());
}

void ConfigClient::getResourceComplianceTimelineAsync(const GetResourceComplianceTimelineRequest& request, const GetResourceComplianceTimelineAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getResourceComplianceTimeline(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::GetResourceComplianceTimelineOutcomeCallable ConfigClient::getResourceComplianceTimelineCallable(const GetResourceComplianceTimelineRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetResourceComplianceTimelineOutcome()>>(
			[this, request]()
			{
			return this->getResourceComplianceTimeline(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::GetResourceConfigurationTimelineOutcome ConfigClient::getResourceConfigurationTimeline(const GetResourceConfigurationTimelineRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetResourceConfigurationTimelineOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetResourceConfigurationTimelineOutcome(GetResourceConfigurationTimelineResult(outcome.result()));
	else
		return GetResourceConfigurationTimelineOutcome(outcome.error());
}

void ConfigClient::getResourceConfigurationTimelineAsync(const GetResourceConfigurationTimelineRequest& request, const GetResourceConfigurationTimelineAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getResourceConfigurationTimeline(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::GetResourceConfigurationTimelineOutcomeCallable ConfigClient::getResourceConfigurationTimelineCallable(const GetResourceConfigurationTimelineRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetResourceConfigurationTimelineOutcome()>>(
			[this, request]()
			{
			return this->getResourceConfigurationTimeline(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::GetSupportedResourceTypesOutcome ConfigClient::getSupportedResourceTypes(const GetSupportedResourceTypesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetSupportedResourceTypesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetSupportedResourceTypesOutcome(GetSupportedResourceTypesResult(outcome.result()));
	else
		return GetSupportedResourceTypesOutcome(outcome.error());
}

void ConfigClient::getSupportedResourceTypesAsync(const GetSupportedResourceTypesRequest& request, const GetSupportedResourceTypesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getSupportedResourceTypes(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::GetSupportedResourceTypesOutcomeCallable ConfigClient::getSupportedResourceTypesCallable(const GetSupportedResourceTypesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetSupportedResourceTypesOutcome()>>(
			[this, request]()
			{
			return this->getSupportedResourceTypes(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::ListConfigRulesOutcome ConfigClient::listConfigRules(const ListConfigRulesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListConfigRulesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListConfigRulesOutcome(ListConfigRulesResult(outcome.result()));
	else
		return ListConfigRulesOutcome(outcome.error());
}

void ConfigClient::listConfigRulesAsync(const ListConfigRulesRequest& request, const ListConfigRulesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listConfigRules(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::ListConfigRulesOutcomeCallable ConfigClient::listConfigRulesCallable(const ListConfigRulesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListConfigRulesOutcome()>>(
			[this, request]()
			{
			return this->listConfigRules(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::ListDiscoveredResourcesOutcome ConfigClient::listDiscoveredResources(const ListDiscoveredResourcesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListDiscoveredResourcesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListDiscoveredResourcesOutcome(ListDiscoveredResourcesResult(outcome.result()));
	else
		return ListDiscoveredResourcesOutcome(outcome.error());
}

void ConfigClient::listDiscoveredResourcesAsync(const ListDiscoveredResourcesRequest& request, const ListDiscoveredResourcesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listDiscoveredResources(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::ListDiscoveredResourcesOutcomeCallable ConfigClient::listDiscoveredResourcesCallable(const ListDiscoveredResourcesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListDiscoveredResourcesOutcome()>>(
			[this, request]()
			{
			return this->listDiscoveredResources(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::PutConfigRuleOutcome ConfigClient::putConfigRule(const PutConfigRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return PutConfigRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return PutConfigRuleOutcome(PutConfigRuleResult(outcome.result()));
	else
		return PutConfigRuleOutcome(outcome.error());
}

void ConfigClient::putConfigRuleAsync(const PutConfigRuleRequest& request, const PutConfigRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, putConfigRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::PutConfigRuleOutcomeCallable ConfigClient::putConfigRuleCallable(const PutConfigRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<PutConfigRuleOutcome()>>(
			[this, request]()
			{
			return this->putConfigRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::PutConfigurationRecorderOutcome ConfigClient::putConfigurationRecorder(const PutConfigurationRecorderRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return PutConfigurationRecorderOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return PutConfigurationRecorderOutcome(PutConfigurationRecorderResult(outcome.result()));
	else
		return PutConfigurationRecorderOutcome(outcome.error());
}

void ConfigClient::putConfigurationRecorderAsync(const PutConfigurationRecorderRequest& request, const PutConfigurationRecorderAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, putConfigurationRecorder(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::PutConfigurationRecorderOutcomeCallable ConfigClient::putConfigurationRecorderCallable(const PutConfigurationRecorderRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<PutConfigurationRecorderOutcome()>>(
			[this, request]()
			{
			return this->putConfigurationRecorder(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::PutDeliveryChannelOutcome ConfigClient::putDeliveryChannel(const PutDeliveryChannelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return PutDeliveryChannelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return PutDeliveryChannelOutcome(PutDeliveryChannelResult(outcome.result()));
	else
		return PutDeliveryChannelOutcome(outcome.error());
}

void ConfigClient::putDeliveryChannelAsync(const PutDeliveryChannelRequest& request, const PutDeliveryChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, putDeliveryChannel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::PutDeliveryChannelOutcomeCallable ConfigClient::putDeliveryChannelCallable(const PutDeliveryChannelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<PutDeliveryChannelOutcome()>>(
			[this, request]()
			{
			return this->putDeliveryChannel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::PutEvaluationsOutcome ConfigClient::putEvaluations(const PutEvaluationsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return PutEvaluationsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return PutEvaluationsOutcome(PutEvaluationsResult(outcome.result()));
	else
		return PutEvaluationsOutcome(outcome.error());
}

void ConfigClient::putEvaluationsAsync(const PutEvaluationsRequest& request, const PutEvaluationsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, putEvaluations(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::PutEvaluationsOutcomeCallable ConfigClient::putEvaluationsCallable(const PutEvaluationsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<PutEvaluationsOutcome()>>(
			[this, request]()
			{
			return this->putEvaluations(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::StartConfigRuleEvaluationOutcome ConfigClient::startConfigRuleEvaluation(const StartConfigRuleEvaluationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartConfigRuleEvaluationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartConfigRuleEvaluationOutcome(StartConfigRuleEvaluationResult(outcome.result()));
	else
		return StartConfigRuleEvaluationOutcome(outcome.error());
}

void ConfigClient::startConfigRuleEvaluationAsync(const StartConfigRuleEvaluationRequest& request, const StartConfigRuleEvaluationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startConfigRuleEvaluation(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::StartConfigRuleEvaluationOutcomeCallable ConfigClient::startConfigRuleEvaluationCallable(const StartConfigRuleEvaluationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartConfigRuleEvaluationOutcome()>>(
			[this, request]()
			{
			return this->startConfigRuleEvaluation(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::StartConfigurationRecorderOutcome ConfigClient::startConfigurationRecorder(const StartConfigurationRecorderRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartConfigurationRecorderOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartConfigurationRecorderOutcome(StartConfigurationRecorderResult(outcome.result()));
	else
		return StartConfigurationRecorderOutcome(outcome.error());
}

void ConfigClient::startConfigurationRecorderAsync(const StartConfigurationRecorderRequest& request, const StartConfigurationRecorderAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startConfigurationRecorder(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::StartConfigurationRecorderOutcomeCallable ConfigClient::startConfigurationRecorderCallable(const StartConfigurationRecorderRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartConfigurationRecorderOutcome()>>(
			[this, request]()
			{
			return this->startConfigurationRecorder(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

ConfigClient::StopConfigRulesOutcome ConfigClient::stopConfigRules(const StopConfigRulesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StopConfigRulesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StopConfigRulesOutcome(StopConfigRulesResult(outcome.result()));
	else
		return StopConfigRulesOutcome(outcome.error());
}

void ConfigClient::stopConfigRulesAsync(const StopConfigRulesRequest& request, const StopConfigRulesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, stopConfigRules(request), context);
	};

	asyncExecute(new Runnable(fn));
}

ConfigClient::StopConfigRulesOutcomeCallable ConfigClient::stopConfigRulesCallable(const StopConfigRulesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StopConfigRulesOutcome()>>(
			[this, request]()
			{
			return this->stopConfigRules(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


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

#include <alibabacloud/live/LiveClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

namespace
{
	const std::string SERVICE_NAME = "live";
}

LiveClient::LiveClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "live");
}

LiveClient::LiveClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "live");
}

LiveClient::LiveClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "live");
}

LiveClient::~LiveClient()
{}

LiveClient::AddCasterComponentOutcome LiveClient::addCasterComponent(const AddCasterComponentRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddCasterComponentOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddCasterComponentOutcome(AddCasterComponentResult(outcome.result()));
	else
		return AddCasterComponentOutcome(outcome.error());
}

void LiveClient::addCasterComponentAsync(const AddCasterComponentRequest& request, const AddCasterComponentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addCasterComponent(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddCasterComponentOutcomeCallable LiveClient::addCasterComponentCallable(const AddCasterComponentRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddCasterComponentOutcome()>>(
			[this, request]()
			{
			return this->addCasterComponent(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddCasterEpisodeOutcome LiveClient::addCasterEpisode(const AddCasterEpisodeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddCasterEpisodeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddCasterEpisodeOutcome(AddCasterEpisodeResult(outcome.result()));
	else
		return AddCasterEpisodeOutcome(outcome.error());
}

void LiveClient::addCasterEpisodeAsync(const AddCasterEpisodeRequest& request, const AddCasterEpisodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addCasterEpisode(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddCasterEpisodeOutcomeCallable LiveClient::addCasterEpisodeCallable(const AddCasterEpisodeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddCasterEpisodeOutcome()>>(
			[this, request]()
			{
			return this->addCasterEpisode(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddCasterEpisodeGroupOutcome LiveClient::addCasterEpisodeGroup(const AddCasterEpisodeGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddCasterEpisodeGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddCasterEpisodeGroupOutcome(AddCasterEpisodeGroupResult(outcome.result()));
	else
		return AddCasterEpisodeGroupOutcome(outcome.error());
}

void LiveClient::addCasterEpisodeGroupAsync(const AddCasterEpisodeGroupRequest& request, const AddCasterEpisodeGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addCasterEpisodeGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddCasterEpisodeGroupOutcomeCallable LiveClient::addCasterEpisodeGroupCallable(const AddCasterEpisodeGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddCasterEpisodeGroupOutcome()>>(
			[this, request]()
			{
			return this->addCasterEpisodeGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddCasterEpisodeGroupContentOutcome LiveClient::addCasterEpisodeGroupContent(const AddCasterEpisodeGroupContentRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddCasterEpisodeGroupContentOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddCasterEpisodeGroupContentOutcome(AddCasterEpisodeGroupContentResult(outcome.result()));
	else
		return AddCasterEpisodeGroupContentOutcome(outcome.error());
}

void LiveClient::addCasterEpisodeGroupContentAsync(const AddCasterEpisodeGroupContentRequest& request, const AddCasterEpisodeGroupContentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addCasterEpisodeGroupContent(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddCasterEpisodeGroupContentOutcomeCallable LiveClient::addCasterEpisodeGroupContentCallable(const AddCasterEpisodeGroupContentRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddCasterEpisodeGroupContentOutcome()>>(
			[this, request]()
			{
			return this->addCasterEpisodeGroupContent(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddCasterLayoutOutcome LiveClient::addCasterLayout(const AddCasterLayoutRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddCasterLayoutOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddCasterLayoutOutcome(AddCasterLayoutResult(outcome.result()));
	else
		return AddCasterLayoutOutcome(outcome.error());
}

void LiveClient::addCasterLayoutAsync(const AddCasterLayoutRequest& request, const AddCasterLayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addCasterLayout(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddCasterLayoutOutcomeCallable LiveClient::addCasterLayoutCallable(const AddCasterLayoutRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddCasterLayoutOutcome()>>(
			[this, request]()
			{
			return this->addCasterLayout(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddCasterProgramOutcome LiveClient::addCasterProgram(const AddCasterProgramRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddCasterProgramOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddCasterProgramOutcome(AddCasterProgramResult(outcome.result()));
	else
		return AddCasterProgramOutcome(outcome.error());
}

void LiveClient::addCasterProgramAsync(const AddCasterProgramRequest& request, const AddCasterProgramAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addCasterProgram(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddCasterProgramOutcomeCallable LiveClient::addCasterProgramCallable(const AddCasterProgramRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddCasterProgramOutcome()>>(
			[this, request]()
			{
			return this->addCasterProgram(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddCasterVideoResourceOutcome LiveClient::addCasterVideoResource(const AddCasterVideoResourceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddCasterVideoResourceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddCasterVideoResourceOutcome(AddCasterVideoResourceResult(outcome.result()));
	else
		return AddCasterVideoResourceOutcome(outcome.error());
}

void LiveClient::addCasterVideoResourceAsync(const AddCasterVideoResourceRequest& request, const AddCasterVideoResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addCasterVideoResource(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddCasterVideoResourceOutcomeCallable LiveClient::addCasterVideoResourceCallable(const AddCasterVideoResourceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddCasterVideoResourceOutcome()>>(
			[this, request]()
			{
			return this->addCasterVideoResource(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddCustomLiveStreamTranscodeOutcome LiveClient::addCustomLiveStreamTranscode(const AddCustomLiveStreamTranscodeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddCustomLiveStreamTranscodeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddCustomLiveStreamTranscodeOutcome(AddCustomLiveStreamTranscodeResult(outcome.result()));
	else
		return AddCustomLiveStreamTranscodeOutcome(outcome.error());
}

void LiveClient::addCustomLiveStreamTranscodeAsync(const AddCustomLiveStreamTranscodeRequest& request, const AddCustomLiveStreamTranscodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addCustomLiveStreamTranscode(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddCustomLiveStreamTranscodeOutcomeCallable LiveClient::addCustomLiveStreamTranscodeCallable(const AddCustomLiveStreamTranscodeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddCustomLiveStreamTranscodeOutcome()>>(
			[this, request]()
			{
			return this->addCustomLiveStreamTranscode(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLiveASRConfigOutcome LiveClient::addLiveASRConfig(const AddLiveASRConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLiveASRConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLiveASRConfigOutcome(AddLiveASRConfigResult(outcome.result()));
	else
		return AddLiveASRConfigOutcome(outcome.error());
}

void LiveClient::addLiveASRConfigAsync(const AddLiveASRConfigRequest& request, const AddLiveASRConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLiveASRConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLiveASRConfigOutcomeCallable LiveClient::addLiveASRConfigCallable(const AddLiveASRConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLiveASRConfigOutcome()>>(
			[this, request]()
			{
			return this->addLiveASRConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLiveAppRecordConfigOutcome LiveClient::addLiveAppRecordConfig(const AddLiveAppRecordConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLiveAppRecordConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLiveAppRecordConfigOutcome(AddLiveAppRecordConfigResult(outcome.result()));
	else
		return AddLiveAppRecordConfigOutcome(outcome.error());
}

void LiveClient::addLiveAppRecordConfigAsync(const AddLiveAppRecordConfigRequest& request, const AddLiveAppRecordConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLiveAppRecordConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLiveAppRecordConfigOutcomeCallable LiveClient::addLiveAppRecordConfigCallable(const AddLiveAppRecordConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLiveAppRecordConfigOutcome()>>(
			[this, request]()
			{
			return this->addLiveAppRecordConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLiveAppSnapshotConfigOutcome LiveClient::addLiveAppSnapshotConfig(const AddLiveAppSnapshotConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLiveAppSnapshotConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLiveAppSnapshotConfigOutcome(AddLiveAppSnapshotConfigResult(outcome.result()));
	else
		return AddLiveAppSnapshotConfigOutcome(outcome.error());
}

void LiveClient::addLiveAppSnapshotConfigAsync(const AddLiveAppSnapshotConfigRequest& request, const AddLiveAppSnapshotConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLiveAppSnapshotConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLiveAppSnapshotConfigOutcomeCallable LiveClient::addLiveAppSnapshotConfigCallable(const AddLiveAppSnapshotConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLiveAppSnapshotConfigOutcome()>>(
			[this, request]()
			{
			return this->addLiveAppSnapshotConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLiveAudioAuditConfigOutcome LiveClient::addLiveAudioAuditConfig(const AddLiveAudioAuditConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLiveAudioAuditConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLiveAudioAuditConfigOutcome(AddLiveAudioAuditConfigResult(outcome.result()));
	else
		return AddLiveAudioAuditConfigOutcome(outcome.error());
}

void LiveClient::addLiveAudioAuditConfigAsync(const AddLiveAudioAuditConfigRequest& request, const AddLiveAudioAuditConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLiveAudioAuditConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLiveAudioAuditConfigOutcomeCallable LiveClient::addLiveAudioAuditConfigCallable(const AddLiveAudioAuditConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLiveAudioAuditConfigOutcome()>>(
			[this, request]()
			{
			return this->addLiveAudioAuditConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLiveAudioAuditNotifyConfigOutcome LiveClient::addLiveAudioAuditNotifyConfig(const AddLiveAudioAuditNotifyConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLiveAudioAuditNotifyConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLiveAudioAuditNotifyConfigOutcome(AddLiveAudioAuditNotifyConfigResult(outcome.result()));
	else
		return AddLiveAudioAuditNotifyConfigOutcome(outcome.error());
}

void LiveClient::addLiveAudioAuditNotifyConfigAsync(const AddLiveAudioAuditNotifyConfigRequest& request, const AddLiveAudioAuditNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLiveAudioAuditNotifyConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLiveAudioAuditNotifyConfigOutcomeCallable LiveClient::addLiveAudioAuditNotifyConfigCallable(const AddLiveAudioAuditNotifyConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLiveAudioAuditNotifyConfigOutcome()>>(
			[this, request]()
			{
			return this->addLiveAudioAuditNotifyConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLiveDetectNotifyConfigOutcome LiveClient::addLiveDetectNotifyConfig(const AddLiveDetectNotifyConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLiveDetectNotifyConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLiveDetectNotifyConfigOutcome(AddLiveDetectNotifyConfigResult(outcome.result()));
	else
		return AddLiveDetectNotifyConfigOutcome(outcome.error());
}

void LiveClient::addLiveDetectNotifyConfigAsync(const AddLiveDetectNotifyConfigRequest& request, const AddLiveDetectNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLiveDetectNotifyConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLiveDetectNotifyConfigOutcomeCallable LiveClient::addLiveDetectNotifyConfigCallable(const AddLiveDetectNotifyConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLiveDetectNotifyConfigOutcome()>>(
			[this, request]()
			{
			return this->addLiveDetectNotifyConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLiveDomainOutcome LiveClient::addLiveDomain(const AddLiveDomainRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLiveDomainOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLiveDomainOutcome(AddLiveDomainResult(outcome.result()));
	else
		return AddLiveDomainOutcome(outcome.error());
}

void LiveClient::addLiveDomainAsync(const AddLiveDomainRequest& request, const AddLiveDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLiveDomain(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLiveDomainOutcomeCallable LiveClient::addLiveDomainCallable(const AddLiveDomainRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLiveDomainOutcome()>>(
			[this, request]()
			{
			return this->addLiveDomain(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLiveDomainMappingOutcome LiveClient::addLiveDomainMapping(const AddLiveDomainMappingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLiveDomainMappingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLiveDomainMappingOutcome(AddLiveDomainMappingResult(outcome.result()));
	else
		return AddLiveDomainMappingOutcome(outcome.error());
}

void LiveClient::addLiveDomainMappingAsync(const AddLiveDomainMappingRequest& request, const AddLiveDomainMappingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLiveDomainMapping(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLiveDomainMappingOutcomeCallable LiveClient::addLiveDomainMappingCallable(const AddLiveDomainMappingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLiveDomainMappingOutcome()>>(
			[this, request]()
			{
			return this->addLiveDomainMapping(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLiveDomainPlayMappingOutcome LiveClient::addLiveDomainPlayMapping(const AddLiveDomainPlayMappingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLiveDomainPlayMappingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLiveDomainPlayMappingOutcome(AddLiveDomainPlayMappingResult(outcome.result()));
	else
		return AddLiveDomainPlayMappingOutcome(outcome.error());
}

void LiveClient::addLiveDomainPlayMappingAsync(const AddLiveDomainPlayMappingRequest& request, const AddLiveDomainPlayMappingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLiveDomainPlayMapping(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLiveDomainPlayMappingOutcomeCallable LiveClient::addLiveDomainPlayMappingCallable(const AddLiveDomainPlayMappingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLiveDomainPlayMappingOutcome()>>(
			[this, request]()
			{
			return this->addLiveDomainPlayMapping(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLivePullStreamInfoConfigOutcome LiveClient::addLivePullStreamInfoConfig(const AddLivePullStreamInfoConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLivePullStreamInfoConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLivePullStreamInfoConfigOutcome(AddLivePullStreamInfoConfigResult(outcome.result()));
	else
		return AddLivePullStreamInfoConfigOutcome(outcome.error());
}

void LiveClient::addLivePullStreamInfoConfigAsync(const AddLivePullStreamInfoConfigRequest& request, const AddLivePullStreamInfoConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLivePullStreamInfoConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLivePullStreamInfoConfigOutcomeCallable LiveClient::addLivePullStreamInfoConfigCallable(const AddLivePullStreamInfoConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLivePullStreamInfoConfigOutcome()>>(
			[this, request]()
			{
			return this->addLivePullStreamInfoConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLiveRecordNotifyConfigOutcome LiveClient::addLiveRecordNotifyConfig(const AddLiveRecordNotifyConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLiveRecordNotifyConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLiveRecordNotifyConfigOutcome(AddLiveRecordNotifyConfigResult(outcome.result()));
	else
		return AddLiveRecordNotifyConfigOutcome(outcome.error());
}

void LiveClient::addLiveRecordNotifyConfigAsync(const AddLiveRecordNotifyConfigRequest& request, const AddLiveRecordNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLiveRecordNotifyConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLiveRecordNotifyConfigOutcomeCallable LiveClient::addLiveRecordNotifyConfigCallable(const AddLiveRecordNotifyConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLiveRecordNotifyConfigOutcome()>>(
			[this, request]()
			{
			return this->addLiveRecordNotifyConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLiveRecordVodConfigOutcome LiveClient::addLiveRecordVodConfig(const AddLiveRecordVodConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLiveRecordVodConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLiveRecordVodConfigOutcome(AddLiveRecordVodConfigResult(outcome.result()));
	else
		return AddLiveRecordVodConfigOutcome(outcome.error());
}

void LiveClient::addLiveRecordVodConfigAsync(const AddLiveRecordVodConfigRequest& request, const AddLiveRecordVodConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLiveRecordVodConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLiveRecordVodConfigOutcomeCallable LiveClient::addLiveRecordVodConfigCallable(const AddLiveRecordVodConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLiveRecordVodConfigOutcome()>>(
			[this, request]()
			{
			return this->addLiveRecordVodConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLiveSnapshotDetectPornConfigOutcome LiveClient::addLiveSnapshotDetectPornConfig(const AddLiveSnapshotDetectPornConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLiveSnapshotDetectPornConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLiveSnapshotDetectPornConfigOutcome(AddLiveSnapshotDetectPornConfigResult(outcome.result()));
	else
		return AddLiveSnapshotDetectPornConfigOutcome(outcome.error());
}

void LiveClient::addLiveSnapshotDetectPornConfigAsync(const AddLiveSnapshotDetectPornConfigRequest& request, const AddLiveSnapshotDetectPornConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLiveSnapshotDetectPornConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLiveSnapshotDetectPornConfigOutcomeCallable LiveClient::addLiveSnapshotDetectPornConfigCallable(const AddLiveSnapshotDetectPornConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLiveSnapshotDetectPornConfigOutcome()>>(
			[this, request]()
			{
			return this->addLiveSnapshotDetectPornConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddLiveStreamTranscodeOutcome LiveClient::addLiveStreamTranscode(const AddLiveStreamTranscodeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddLiveStreamTranscodeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddLiveStreamTranscodeOutcome(AddLiveStreamTranscodeResult(outcome.result()));
	else
		return AddLiveStreamTranscodeOutcome(outcome.error());
}

void LiveClient::addLiveStreamTranscodeAsync(const AddLiveStreamTranscodeRequest& request, const AddLiveStreamTranscodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addLiveStreamTranscode(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddLiveStreamTranscodeOutcomeCallable LiveClient::addLiveStreamTranscodeCallable(const AddLiveStreamTranscodeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddLiveStreamTranscodeOutcome()>>(
			[this, request]()
			{
			return this->addLiveStreamTranscode(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddRtsLiveStreamTranscodeOutcome LiveClient::addRtsLiveStreamTranscode(const AddRtsLiveStreamTranscodeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddRtsLiveStreamTranscodeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddRtsLiveStreamTranscodeOutcome(AddRtsLiveStreamTranscodeResult(outcome.result()));
	else
		return AddRtsLiveStreamTranscodeOutcome(outcome.error());
}

void LiveClient::addRtsLiveStreamTranscodeAsync(const AddRtsLiveStreamTranscodeRequest& request, const AddRtsLiveStreamTranscodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addRtsLiveStreamTranscode(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddRtsLiveStreamTranscodeOutcomeCallable LiveClient::addRtsLiveStreamTranscodeCallable(const AddRtsLiveStreamTranscodeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddRtsLiveStreamTranscodeOutcome()>>(
			[this, request]()
			{
			return this->addRtsLiveStreamTranscode(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AddTrancodeSEIOutcome LiveClient::addTrancodeSEI(const AddTrancodeSEIRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddTrancodeSEIOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddTrancodeSEIOutcome(AddTrancodeSEIResult(outcome.result()));
	else
		return AddTrancodeSEIOutcome(outcome.error());
}

void LiveClient::addTrancodeSEIAsync(const AddTrancodeSEIRequest& request, const AddTrancodeSEIAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addTrancodeSEI(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AddTrancodeSEIOutcomeCallable LiveClient::addTrancodeSEICallable(const AddTrancodeSEIRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddTrancodeSEIOutcome()>>(
			[this, request]()
			{
			return this->addTrancodeSEI(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::AllowPushStreamOutcome LiveClient::allowPushStream(const AllowPushStreamRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AllowPushStreamOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AllowPushStreamOutcome(AllowPushStreamResult(outcome.result()));
	else
		return AllowPushStreamOutcome(outcome.error());
}

void LiveClient::allowPushStreamAsync(const AllowPushStreamRequest& request, const AllowPushStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, allowPushStream(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::AllowPushStreamOutcomeCallable LiveClient::allowPushStreamCallable(const AllowPushStreamRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AllowPushStreamOutcome()>>(
			[this, request]()
			{
			return this->allowPushStream(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ApplyBoardTokenOutcome LiveClient::applyBoardToken(const ApplyBoardTokenRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ApplyBoardTokenOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ApplyBoardTokenOutcome(ApplyBoardTokenResult(outcome.result()));
	else
		return ApplyBoardTokenOutcome(outcome.error());
}

void LiveClient::applyBoardTokenAsync(const ApplyBoardTokenRequest& request, const ApplyBoardTokenAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, applyBoardToken(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ApplyBoardTokenOutcomeCallable LiveClient::applyBoardTokenCallable(const ApplyBoardTokenRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ApplyBoardTokenOutcome()>>(
			[this, request]()
			{
			return this->applyBoardToken(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ApplyRecordTokenOutcome LiveClient::applyRecordToken(const ApplyRecordTokenRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ApplyRecordTokenOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ApplyRecordTokenOutcome(ApplyRecordTokenResult(outcome.result()));
	else
		return ApplyRecordTokenOutcome(outcome.error());
}

void LiveClient::applyRecordTokenAsync(const ApplyRecordTokenRequest& request, const ApplyRecordTokenAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, applyRecordToken(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ApplyRecordTokenOutcomeCallable LiveClient::applyRecordTokenCallable(const ApplyRecordTokenRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ApplyRecordTokenOutcome()>>(
			[this, request]()
			{
			return this->applyRecordToken(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::BatchDeleteLiveDomainConfigsOutcome LiveClient::batchDeleteLiveDomainConfigs(const BatchDeleteLiveDomainConfigsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchDeleteLiveDomainConfigsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchDeleteLiveDomainConfigsOutcome(BatchDeleteLiveDomainConfigsResult(outcome.result()));
	else
		return BatchDeleteLiveDomainConfigsOutcome(outcome.error());
}

void LiveClient::batchDeleteLiveDomainConfigsAsync(const BatchDeleteLiveDomainConfigsRequest& request, const BatchDeleteLiveDomainConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchDeleteLiveDomainConfigs(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::BatchDeleteLiveDomainConfigsOutcomeCallable LiveClient::batchDeleteLiveDomainConfigsCallable(const BatchDeleteLiveDomainConfigsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchDeleteLiveDomainConfigsOutcome()>>(
			[this, request]()
			{
			return this->batchDeleteLiveDomainConfigs(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::BatchSetLiveDomainConfigsOutcome LiveClient::batchSetLiveDomainConfigs(const BatchSetLiveDomainConfigsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchSetLiveDomainConfigsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchSetLiveDomainConfigsOutcome(BatchSetLiveDomainConfigsResult(outcome.result()));
	else
		return BatchSetLiveDomainConfigsOutcome(outcome.error());
}

void LiveClient::batchSetLiveDomainConfigsAsync(const BatchSetLiveDomainConfigsRequest& request, const BatchSetLiveDomainConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchSetLiveDomainConfigs(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::BatchSetLiveDomainConfigsOutcomeCallable LiveClient::batchSetLiveDomainConfigsCallable(const BatchSetLiveDomainConfigsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchSetLiveDomainConfigsOutcome()>>(
			[this, request]()
			{
			return this->batchSetLiveDomainConfigs(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::CompleteBoardOutcome LiveClient::completeBoard(const CompleteBoardRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CompleteBoardOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CompleteBoardOutcome(CompleteBoardResult(outcome.result()));
	else
		return CompleteBoardOutcome(outcome.error());
}

void LiveClient::completeBoardAsync(const CompleteBoardRequest& request, const CompleteBoardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, completeBoard(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::CompleteBoardOutcomeCallable LiveClient::completeBoardCallable(const CompleteBoardRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CompleteBoardOutcome()>>(
			[this, request]()
			{
			return this->completeBoard(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::CompleteBoardRecordOutcome LiveClient::completeBoardRecord(const CompleteBoardRecordRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CompleteBoardRecordOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CompleteBoardRecordOutcome(CompleteBoardRecordResult(outcome.result()));
	else
		return CompleteBoardRecordOutcome(outcome.error());
}

void LiveClient::completeBoardRecordAsync(const CompleteBoardRecordRequest& request, const CompleteBoardRecordAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, completeBoardRecord(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::CompleteBoardRecordOutcomeCallable LiveClient::completeBoardRecordCallable(const CompleteBoardRecordRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CompleteBoardRecordOutcome()>>(
			[this, request]()
			{
			return this->completeBoardRecord(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ControlHtmlResourceOutcome LiveClient::controlHtmlResource(const ControlHtmlResourceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ControlHtmlResourceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ControlHtmlResourceOutcome(ControlHtmlResourceResult(outcome.result()));
	else
		return ControlHtmlResourceOutcome(outcome.error());
}

void LiveClient::controlHtmlResourceAsync(const ControlHtmlResourceRequest& request, const ControlHtmlResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, controlHtmlResource(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ControlHtmlResourceOutcomeCallable LiveClient::controlHtmlResourceCallable(const ControlHtmlResourceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ControlHtmlResourceOutcome()>>(
			[this, request]()
			{
			return this->controlHtmlResource(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::CopyCasterOutcome LiveClient::copyCaster(const CopyCasterRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CopyCasterOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CopyCasterOutcome(CopyCasterResult(outcome.result()));
	else
		return CopyCasterOutcome(outcome.error());
}

void LiveClient::copyCasterAsync(const CopyCasterRequest& request, const CopyCasterAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, copyCaster(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::CopyCasterOutcomeCallable LiveClient::copyCasterCallable(const CopyCasterRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CopyCasterOutcome()>>(
			[this, request]()
			{
			return this->copyCaster(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::CopyCasterSceneConfigOutcome LiveClient::copyCasterSceneConfig(const CopyCasterSceneConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CopyCasterSceneConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CopyCasterSceneConfigOutcome(CopyCasterSceneConfigResult(outcome.result()));
	else
		return CopyCasterSceneConfigOutcome(outcome.error());
}

void LiveClient::copyCasterSceneConfigAsync(const CopyCasterSceneConfigRequest& request, const CopyCasterSceneConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, copyCasterSceneConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::CopyCasterSceneConfigOutcomeCallable LiveClient::copyCasterSceneConfigCallable(const CopyCasterSceneConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CopyCasterSceneConfigOutcome()>>(
			[this, request]()
			{
			return this->copyCasterSceneConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::CreateBoardOutcome LiveClient::createBoard(const CreateBoardRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateBoardOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateBoardOutcome(CreateBoardResult(outcome.result()));
	else
		return CreateBoardOutcome(outcome.error());
}

void LiveClient::createBoardAsync(const CreateBoardRequest& request, const CreateBoardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createBoard(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::CreateBoardOutcomeCallable LiveClient::createBoardCallable(const CreateBoardRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateBoardOutcome()>>(
			[this, request]()
			{
			return this->createBoard(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::CreateCasterOutcome LiveClient::createCaster(const CreateCasterRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateCasterOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateCasterOutcome(CreateCasterResult(outcome.result()));
	else
		return CreateCasterOutcome(outcome.error());
}

void LiveClient::createCasterAsync(const CreateCasterRequest& request, const CreateCasterAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createCaster(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::CreateCasterOutcomeCallable LiveClient::createCasterCallable(const CreateCasterRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateCasterOutcome()>>(
			[this, request]()
			{
			return this->createCaster(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::CreateLiveRealTimeLogDeliveryOutcome LiveClient::createLiveRealTimeLogDelivery(const CreateLiveRealTimeLogDeliveryRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateLiveRealTimeLogDeliveryOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateLiveRealTimeLogDeliveryOutcome(CreateLiveRealTimeLogDeliveryResult(outcome.result()));
	else
		return CreateLiveRealTimeLogDeliveryOutcome(outcome.error());
}

void LiveClient::createLiveRealTimeLogDeliveryAsync(const CreateLiveRealTimeLogDeliveryRequest& request, const CreateLiveRealTimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createLiveRealTimeLogDelivery(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::CreateLiveRealTimeLogDeliveryOutcomeCallable LiveClient::createLiveRealTimeLogDeliveryCallable(const CreateLiveRealTimeLogDeliveryRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateLiveRealTimeLogDeliveryOutcome()>>(
			[this, request]()
			{
			return this->createLiveRealTimeLogDelivery(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::CreateLiveStreamRecordIndexFilesOutcome LiveClient::createLiveStreamRecordIndexFiles(const CreateLiveStreamRecordIndexFilesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateLiveStreamRecordIndexFilesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateLiveStreamRecordIndexFilesOutcome(CreateLiveStreamRecordIndexFilesResult(outcome.result()));
	else
		return CreateLiveStreamRecordIndexFilesOutcome(outcome.error());
}

void LiveClient::createLiveStreamRecordIndexFilesAsync(const CreateLiveStreamRecordIndexFilesRequest& request, const CreateLiveStreamRecordIndexFilesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createLiveStreamRecordIndexFiles(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::CreateLiveStreamRecordIndexFilesOutcomeCallable LiveClient::createLiveStreamRecordIndexFilesCallable(const CreateLiveStreamRecordIndexFilesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateLiveStreamRecordIndexFilesOutcome()>>(
			[this, request]()
			{
			return this->createLiveStreamRecordIndexFiles(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::CreateMixStreamOutcome LiveClient::createMixStream(const CreateMixStreamRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateMixStreamOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateMixStreamOutcome(CreateMixStreamResult(outcome.result()));
	else
		return CreateMixStreamOutcome(outcome.error());
}

void LiveClient::createMixStreamAsync(const CreateMixStreamRequest& request, const CreateMixStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createMixStream(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::CreateMixStreamOutcomeCallable LiveClient::createMixStreamCallable(const CreateMixStreamRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateMixStreamOutcome()>>(
			[this, request]()
			{
			return this->createMixStream(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::CreateRoomOutcome LiveClient::createRoom(const CreateRoomRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateRoomOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateRoomOutcome(CreateRoomResult(outcome.result()));
	else
		return CreateRoomOutcome(outcome.error());
}

void LiveClient::createRoomAsync(const CreateRoomRequest& request, const CreateRoomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createRoom(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::CreateRoomOutcomeCallable LiveClient::createRoomCallable(const CreateRoomRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateRoomOutcome()>>(
			[this, request]()
			{
			return this->createRoom(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteBoardOutcome LiveClient::deleteBoard(const DeleteBoardRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteBoardOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteBoardOutcome(DeleteBoardResult(outcome.result()));
	else
		return DeleteBoardOutcome(outcome.error());
}

void LiveClient::deleteBoardAsync(const DeleteBoardRequest& request, const DeleteBoardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteBoard(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteBoardOutcomeCallable LiveClient::deleteBoardCallable(const DeleteBoardRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteBoardOutcome()>>(
			[this, request]()
			{
			return this->deleteBoard(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteCasterOutcome LiveClient::deleteCaster(const DeleteCasterRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteCasterOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteCasterOutcome(DeleteCasterResult(outcome.result()));
	else
		return DeleteCasterOutcome(outcome.error());
}

void LiveClient::deleteCasterAsync(const DeleteCasterRequest& request, const DeleteCasterAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteCaster(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteCasterOutcomeCallable LiveClient::deleteCasterCallable(const DeleteCasterRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteCasterOutcome()>>(
			[this, request]()
			{
			return this->deleteCaster(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteCasterComponentOutcome LiveClient::deleteCasterComponent(const DeleteCasterComponentRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteCasterComponentOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteCasterComponentOutcome(DeleteCasterComponentResult(outcome.result()));
	else
		return DeleteCasterComponentOutcome(outcome.error());
}

void LiveClient::deleteCasterComponentAsync(const DeleteCasterComponentRequest& request, const DeleteCasterComponentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteCasterComponent(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteCasterComponentOutcomeCallable LiveClient::deleteCasterComponentCallable(const DeleteCasterComponentRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteCasterComponentOutcome()>>(
			[this, request]()
			{
			return this->deleteCasterComponent(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteCasterEpisodeOutcome LiveClient::deleteCasterEpisode(const DeleteCasterEpisodeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteCasterEpisodeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteCasterEpisodeOutcome(DeleteCasterEpisodeResult(outcome.result()));
	else
		return DeleteCasterEpisodeOutcome(outcome.error());
}

void LiveClient::deleteCasterEpisodeAsync(const DeleteCasterEpisodeRequest& request, const DeleteCasterEpisodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteCasterEpisode(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteCasterEpisodeOutcomeCallable LiveClient::deleteCasterEpisodeCallable(const DeleteCasterEpisodeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteCasterEpisodeOutcome()>>(
			[this, request]()
			{
			return this->deleteCasterEpisode(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteCasterEpisodeGroupOutcome LiveClient::deleteCasterEpisodeGroup(const DeleteCasterEpisodeGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteCasterEpisodeGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteCasterEpisodeGroupOutcome(DeleteCasterEpisodeGroupResult(outcome.result()));
	else
		return DeleteCasterEpisodeGroupOutcome(outcome.error());
}

void LiveClient::deleteCasterEpisodeGroupAsync(const DeleteCasterEpisodeGroupRequest& request, const DeleteCasterEpisodeGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteCasterEpisodeGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteCasterEpisodeGroupOutcomeCallable LiveClient::deleteCasterEpisodeGroupCallable(const DeleteCasterEpisodeGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteCasterEpisodeGroupOutcome()>>(
			[this, request]()
			{
			return this->deleteCasterEpisodeGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteCasterLayoutOutcome LiveClient::deleteCasterLayout(const DeleteCasterLayoutRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteCasterLayoutOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteCasterLayoutOutcome(DeleteCasterLayoutResult(outcome.result()));
	else
		return DeleteCasterLayoutOutcome(outcome.error());
}

void LiveClient::deleteCasterLayoutAsync(const DeleteCasterLayoutRequest& request, const DeleteCasterLayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteCasterLayout(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteCasterLayoutOutcomeCallable LiveClient::deleteCasterLayoutCallable(const DeleteCasterLayoutRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteCasterLayoutOutcome()>>(
			[this, request]()
			{
			return this->deleteCasterLayout(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteCasterProgramOutcome LiveClient::deleteCasterProgram(const DeleteCasterProgramRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteCasterProgramOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteCasterProgramOutcome(DeleteCasterProgramResult(outcome.result()));
	else
		return DeleteCasterProgramOutcome(outcome.error());
}

void LiveClient::deleteCasterProgramAsync(const DeleteCasterProgramRequest& request, const DeleteCasterProgramAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteCasterProgram(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteCasterProgramOutcomeCallable LiveClient::deleteCasterProgramCallable(const DeleteCasterProgramRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteCasterProgramOutcome()>>(
			[this, request]()
			{
			return this->deleteCasterProgram(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteCasterSceneConfigOutcome LiveClient::deleteCasterSceneConfig(const DeleteCasterSceneConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteCasterSceneConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteCasterSceneConfigOutcome(DeleteCasterSceneConfigResult(outcome.result()));
	else
		return DeleteCasterSceneConfigOutcome(outcome.error());
}

void LiveClient::deleteCasterSceneConfigAsync(const DeleteCasterSceneConfigRequest& request, const DeleteCasterSceneConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteCasterSceneConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteCasterSceneConfigOutcomeCallable LiveClient::deleteCasterSceneConfigCallable(const DeleteCasterSceneConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteCasterSceneConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteCasterSceneConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteCasterVideoResourceOutcome LiveClient::deleteCasterVideoResource(const DeleteCasterVideoResourceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteCasterVideoResourceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteCasterVideoResourceOutcome(DeleteCasterVideoResourceResult(outcome.result()));
	else
		return DeleteCasterVideoResourceOutcome(outcome.error());
}

void LiveClient::deleteCasterVideoResourceAsync(const DeleteCasterVideoResourceRequest& request, const DeleteCasterVideoResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteCasterVideoResource(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteCasterVideoResourceOutcomeCallable LiveClient::deleteCasterVideoResourceCallable(const DeleteCasterVideoResourceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteCasterVideoResourceOutcome()>>(
			[this, request]()
			{
			return this->deleteCasterVideoResource(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteHtmlResourceOutcome LiveClient::deleteHtmlResource(const DeleteHtmlResourceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteHtmlResourceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteHtmlResourceOutcome(DeleteHtmlResourceResult(outcome.result()));
	else
		return DeleteHtmlResourceOutcome(outcome.error());
}

void LiveClient::deleteHtmlResourceAsync(const DeleteHtmlResourceRequest& request, const DeleteHtmlResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteHtmlResource(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteHtmlResourceOutcomeCallable LiveClient::deleteHtmlResourceCallable(const DeleteHtmlResourceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteHtmlResourceOutcome()>>(
			[this, request]()
			{
			return this->deleteHtmlResource(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveASRConfigOutcome LiveClient::deleteLiveASRConfig(const DeleteLiveASRConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveASRConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveASRConfigOutcome(DeleteLiveASRConfigResult(outcome.result()));
	else
		return DeleteLiveASRConfigOutcome(outcome.error());
}

void LiveClient::deleteLiveASRConfigAsync(const DeleteLiveASRConfigRequest& request, const DeleteLiveASRConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveASRConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveASRConfigOutcomeCallable LiveClient::deleteLiveASRConfigCallable(const DeleteLiveASRConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveASRConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveASRConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveAppRecordConfigOutcome LiveClient::deleteLiveAppRecordConfig(const DeleteLiveAppRecordConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveAppRecordConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveAppRecordConfigOutcome(DeleteLiveAppRecordConfigResult(outcome.result()));
	else
		return DeleteLiveAppRecordConfigOutcome(outcome.error());
}

void LiveClient::deleteLiveAppRecordConfigAsync(const DeleteLiveAppRecordConfigRequest& request, const DeleteLiveAppRecordConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveAppRecordConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveAppRecordConfigOutcomeCallable LiveClient::deleteLiveAppRecordConfigCallable(const DeleteLiveAppRecordConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveAppRecordConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveAppRecordConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveAppSnapshotConfigOutcome LiveClient::deleteLiveAppSnapshotConfig(const DeleteLiveAppSnapshotConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveAppSnapshotConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveAppSnapshotConfigOutcome(DeleteLiveAppSnapshotConfigResult(outcome.result()));
	else
		return DeleteLiveAppSnapshotConfigOutcome(outcome.error());
}

void LiveClient::deleteLiveAppSnapshotConfigAsync(const DeleteLiveAppSnapshotConfigRequest& request, const DeleteLiveAppSnapshotConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveAppSnapshotConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveAppSnapshotConfigOutcomeCallable LiveClient::deleteLiveAppSnapshotConfigCallable(const DeleteLiveAppSnapshotConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveAppSnapshotConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveAppSnapshotConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveAudioAuditConfigOutcome LiveClient::deleteLiveAudioAuditConfig(const DeleteLiveAudioAuditConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveAudioAuditConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveAudioAuditConfigOutcome(DeleteLiveAudioAuditConfigResult(outcome.result()));
	else
		return DeleteLiveAudioAuditConfigOutcome(outcome.error());
}

void LiveClient::deleteLiveAudioAuditConfigAsync(const DeleteLiveAudioAuditConfigRequest& request, const DeleteLiveAudioAuditConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveAudioAuditConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveAudioAuditConfigOutcomeCallable LiveClient::deleteLiveAudioAuditConfigCallable(const DeleteLiveAudioAuditConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveAudioAuditConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveAudioAuditConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveAudioAuditNotifyConfigOutcome LiveClient::deleteLiveAudioAuditNotifyConfig(const DeleteLiveAudioAuditNotifyConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveAudioAuditNotifyConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveAudioAuditNotifyConfigOutcome(DeleteLiveAudioAuditNotifyConfigResult(outcome.result()));
	else
		return DeleteLiveAudioAuditNotifyConfigOutcome(outcome.error());
}

void LiveClient::deleteLiveAudioAuditNotifyConfigAsync(const DeleteLiveAudioAuditNotifyConfigRequest& request, const DeleteLiveAudioAuditNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveAudioAuditNotifyConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveAudioAuditNotifyConfigOutcomeCallable LiveClient::deleteLiveAudioAuditNotifyConfigCallable(const DeleteLiveAudioAuditNotifyConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveAudioAuditNotifyConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveAudioAuditNotifyConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveDetectNotifyConfigOutcome LiveClient::deleteLiveDetectNotifyConfig(const DeleteLiveDetectNotifyConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveDetectNotifyConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveDetectNotifyConfigOutcome(DeleteLiveDetectNotifyConfigResult(outcome.result()));
	else
		return DeleteLiveDetectNotifyConfigOutcome(outcome.error());
}

void LiveClient::deleteLiveDetectNotifyConfigAsync(const DeleteLiveDetectNotifyConfigRequest& request, const DeleteLiveDetectNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveDetectNotifyConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveDetectNotifyConfigOutcomeCallable LiveClient::deleteLiveDetectNotifyConfigCallable(const DeleteLiveDetectNotifyConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveDetectNotifyConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveDetectNotifyConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveDomainOutcome LiveClient::deleteLiveDomain(const DeleteLiveDomainRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveDomainOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveDomainOutcome(DeleteLiveDomainResult(outcome.result()));
	else
		return DeleteLiveDomainOutcome(outcome.error());
}

void LiveClient::deleteLiveDomainAsync(const DeleteLiveDomainRequest& request, const DeleteLiveDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveDomain(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveDomainOutcomeCallable LiveClient::deleteLiveDomainCallable(const DeleteLiveDomainRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveDomainOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveDomain(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveDomainMappingOutcome LiveClient::deleteLiveDomainMapping(const DeleteLiveDomainMappingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveDomainMappingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveDomainMappingOutcome(DeleteLiveDomainMappingResult(outcome.result()));
	else
		return DeleteLiveDomainMappingOutcome(outcome.error());
}

void LiveClient::deleteLiveDomainMappingAsync(const DeleteLiveDomainMappingRequest& request, const DeleteLiveDomainMappingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveDomainMapping(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveDomainMappingOutcomeCallable LiveClient::deleteLiveDomainMappingCallable(const DeleteLiveDomainMappingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveDomainMappingOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveDomainMapping(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveLazyPullStreamInfoConfigOutcome LiveClient::deleteLiveLazyPullStreamInfoConfig(const DeleteLiveLazyPullStreamInfoConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveLazyPullStreamInfoConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveLazyPullStreamInfoConfigOutcome(DeleteLiveLazyPullStreamInfoConfigResult(outcome.result()));
	else
		return DeleteLiveLazyPullStreamInfoConfigOutcome(outcome.error());
}

void LiveClient::deleteLiveLazyPullStreamInfoConfigAsync(const DeleteLiveLazyPullStreamInfoConfigRequest& request, const DeleteLiveLazyPullStreamInfoConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveLazyPullStreamInfoConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveLazyPullStreamInfoConfigOutcomeCallable LiveClient::deleteLiveLazyPullStreamInfoConfigCallable(const DeleteLiveLazyPullStreamInfoConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveLazyPullStreamInfoConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveLazyPullStreamInfoConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLivePullStreamInfoConfigOutcome LiveClient::deleteLivePullStreamInfoConfig(const DeleteLivePullStreamInfoConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLivePullStreamInfoConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLivePullStreamInfoConfigOutcome(DeleteLivePullStreamInfoConfigResult(outcome.result()));
	else
		return DeleteLivePullStreamInfoConfigOutcome(outcome.error());
}

void LiveClient::deleteLivePullStreamInfoConfigAsync(const DeleteLivePullStreamInfoConfigRequest& request, const DeleteLivePullStreamInfoConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLivePullStreamInfoConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLivePullStreamInfoConfigOutcomeCallable LiveClient::deleteLivePullStreamInfoConfigCallable(const DeleteLivePullStreamInfoConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLivePullStreamInfoConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteLivePullStreamInfoConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveRealTimeLogLogstoreOutcome LiveClient::deleteLiveRealTimeLogLogstore(const DeleteLiveRealTimeLogLogstoreRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveRealTimeLogLogstoreOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveRealTimeLogLogstoreOutcome(DeleteLiveRealTimeLogLogstoreResult(outcome.result()));
	else
		return DeleteLiveRealTimeLogLogstoreOutcome(outcome.error());
}

void LiveClient::deleteLiveRealTimeLogLogstoreAsync(const DeleteLiveRealTimeLogLogstoreRequest& request, const DeleteLiveRealTimeLogLogstoreAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveRealTimeLogLogstore(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveRealTimeLogLogstoreOutcomeCallable LiveClient::deleteLiveRealTimeLogLogstoreCallable(const DeleteLiveRealTimeLogLogstoreRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveRealTimeLogLogstoreOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveRealTimeLogLogstore(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveRealtimeLogDeliveryOutcome LiveClient::deleteLiveRealtimeLogDelivery(const DeleteLiveRealtimeLogDeliveryRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveRealtimeLogDeliveryOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveRealtimeLogDeliveryOutcome(DeleteLiveRealtimeLogDeliveryResult(outcome.result()));
	else
		return DeleteLiveRealtimeLogDeliveryOutcome(outcome.error());
}

void LiveClient::deleteLiveRealtimeLogDeliveryAsync(const DeleteLiveRealtimeLogDeliveryRequest& request, const DeleteLiveRealtimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveRealtimeLogDelivery(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveRealtimeLogDeliveryOutcomeCallable LiveClient::deleteLiveRealtimeLogDeliveryCallable(const DeleteLiveRealtimeLogDeliveryRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveRealtimeLogDeliveryOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveRealtimeLogDelivery(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveRecordNotifyConfigOutcome LiveClient::deleteLiveRecordNotifyConfig(const DeleteLiveRecordNotifyConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveRecordNotifyConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveRecordNotifyConfigOutcome(DeleteLiveRecordNotifyConfigResult(outcome.result()));
	else
		return DeleteLiveRecordNotifyConfigOutcome(outcome.error());
}

void LiveClient::deleteLiveRecordNotifyConfigAsync(const DeleteLiveRecordNotifyConfigRequest& request, const DeleteLiveRecordNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveRecordNotifyConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveRecordNotifyConfigOutcomeCallable LiveClient::deleteLiveRecordNotifyConfigCallable(const DeleteLiveRecordNotifyConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveRecordNotifyConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveRecordNotifyConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveRecordVodConfigOutcome LiveClient::deleteLiveRecordVodConfig(const DeleteLiveRecordVodConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveRecordVodConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveRecordVodConfigOutcome(DeleteLiveRecordVodConfigResult(outcome.result()));
	else
		return DeleteLiveRecordVodConfigOutcome(outcome.error());
}

void LiveClient::deleteLiveRecordVodConfigAsync(const DeleteLiveRecordVodConfigRequest& request, const DeleteLiveRecordVodConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveRecordVodConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveRecordVodConfigOutcomeCallable LiveClient::deleteLiveRecordVodConfigCallable(const DeleteLiveRecordVodConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveRecordVodConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveRecordVodConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveSnapshotDetectPornConfigOutcome LiveClient::deleteLiveSnapshotDetectPornConfig(const DeleteLiveSnapshotDetectPornConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveSnapshotDetectPornConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveSnapshotDetectPornConfigOutcome(DeleteLiveSnapshotDetectPornConfigResult(outcome.result()));
	else
		return DeleteLiveSnapshotDetectPornConfigOutcome(outcome.error());
}

void LiveClient::deleteLiveSnapshotDetectPornConfigAsync(const DeleteLiveSnapshotDetectPornConfigRequest& request, const DeleteLiveSnapshotDetectPornConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveSnapshotDetectPornConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveSnapshotDetectPornConfigOutcomeCallable LiveClient::deleteLiveSnapshotDetectPornConfigCallable(const DeleteLiveSnapshotDetectPornConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveSnapshotDetectPornConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveSnapshotDetectPornConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveStreamTranscodeOutcome LiveClient::deleteLiveStreamTranscode(const DeleteLiveStreamTranscodeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveStreamTranscodeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveStreamTranscodeOutcome(DeleteLiveStreamTranscodeResult(outcome.result()));
	else
		return DeleteLiveStreamTranscodeOutcome(outcome.error());
}

void LiveClient::deleteLiveStreamTranscodeAsync(const DeleteLiveStreamTranscodeRequest& request, const DeleteLiveStreamTranscodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveStreamTranscode(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveStreamTranscodeOutcomeCallable LiveClient::deleteLiveStreamTranscodeCallable(const DeleteLiveStreamTranscodeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveStreamTranscodeOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveStreamTranscode(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteLiveStreamsNotifyUrlConfigOutcome LiveClient::deleteLiveStreamsNotifyUrlConfig(const DeleteLiveStreamsNotifyUrlConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveStreamsNotifyUrlConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveStreamsNotifyUrlConfigOutcome(DeleteLiveStreamsNotifyUrlConfigResult(outcome.result()));
	else
		return DeleteLiveStreamsNotifyUrlConfigOutcome(outcome.error());
}

void LiveClient::deleteLiveStreamsNotifyUrlConfigAsync(const DeleteLiveStreamsNotifyUrlConfigRequest& request, const DeleteLiveStreamsNotifyUrlConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLiveStreamsNotifyUrlConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteLiveStreamsNotifyUrlConfigOutcomeCallable LiveClient::deleteLiveStreamsNotifyUrlConfigCallable(const DeleteLiveStreamsNotifyUrlConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveStreamsNotifyUrlConfigOutcome()>>(
			[this, request]()
			{
			return this->deleteLiveStreamsNotifyUrlConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteMixStreamOutcome LiveClient::deleteMixStream(const DeleteMixStreamRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteMixStreamOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteMixStreamOutcome(DeleteMixStreamResult(outcome.result()));
	else
		return DeleteMixStreamOutcome(outcome.error());
}

void LiveClient::deleteMixStreamAsync(const DeleteMixStreamRequest& request, const DeleteMixStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteMixStream(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteMixStreamOutcomeCallable LiveClient::deleteMixStreamCallable(const DeleteMixStreamRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteMixStreamOutcome()>>(
			[this, request]()
			{
			return this->deleteMixStream(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DeleteRoomOutcome LiveClient::deleteRoom(const DeleteRoomRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteRoomOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteRoomOutcome(DeleteRoomResult(outcome.result()));
	else
		return DeleteRoomOutcome(outcome.error());
}

void LiveClient::deleteRoomAsync(const DeleteRoomRequest& request, const DeleteRoomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteRoom(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DeleteRoomOutcomeCallable LiveClient::deleteRoomCallable(const DeleteRoomRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteRoomOutcome()>>(
			[this, request]()
			{
			return this->deleteRoom(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeBoardEventsOutcome LiveClient::describeBoardEvents(const DescribeBoardEventsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeBoardEventsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeBoardEventsOutcome(DescribeBoardEventsResult(outcome.result()));
	else
		return DescribeBoardEventsOutcome(outcome.error());
}

void LiveClient::describeBoardEventsAsync(const DescribeBoardEventsRequest& request, const DescribeBoardEventsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeBoardEvents(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeBoardEventsOutcomeCallable LiveClient::describeBoardEventsCallable(const DescribeBoardEventsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeBoardEventsOutcome()>>(
			[this, request]()
			{
			return this->describeBoardEvents(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeBoardSnapshotOutcome LiveClient::describeBoardSnapshot(const DescribeBoardSnapshotRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeBoardSnapshotOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeBoardSnapshotOutcome(DescribeBoardSnapshotResult(outcome.result()));
	else
		return DescribeBoardSnapshotOutcome(outcome.error());
}

void LiveClient::describeBoardSnapshotAsync(const DescribeBoardSnapshotRequest& request, const DescribeBoardSnapshotAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeBoardSnapshot(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeBoardSnapshotOutcomeCallable LiveClient::describeBoardSnapshotCallable(const DescribeBoardSnapshotRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeBoardSnapshotOutcome()>>(
			[this, request]()
			{
			return this->describeBoardSnapshot(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeBoardsOutcome LiveClient::describeBoards(const DescribeBoardsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeBoardsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeBoardsOutcome(DescribeBoardsResult(outcome.result()));
	else
		return DescribeBoardsOutcome(outcome.error());
}

void LiveClient::describeBoardsAsync(const DescribeBoardsRequest& request, const DescribeBoardsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeBoards(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeBoardsOutcomeCallable LiveClient::describeBoardsCallable(const DescribeBoardsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeBoardsOutcome()>>(
			[this, request]()
			{
			return this->describeBoards(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeCasterChannelsOutcome LiveClient::describeCasterChannels(const DescribeCasterChannelsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeCasterChannelsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeCasterChannelsOutcome(DescribeCasterChannelsResult(outcome.result()));
	else
		return DescribeCasterChannelsOutcome(outcome.error());
}

void LiveClient::describeCasterChannelsAsync(const DescribeCasterChannelsRequest& request, const DescribeCasterChannelsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCasterChannels(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeCasterChannelsOutcomeCallable LiveClient::describeCasterChannelsCallable(const DescribeCasterChannelsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeCasterChannelsOutcome()>>(
			[this, request]()
			{
			return this->describeCasterChannels(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeCasterComponentsOutcome LiveClient::describeCasterComponents(const DescribeCasterComponentsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeCasterComponentsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeCasterComponentsOutcome(DescribeCasterComponentsResult(outcome.result()));
	else
		return DescribeCasterComponentsOutcome(outcome.error());
}

void LiveClient::describeCasterComponentsAsync(const DescribeCasterComponentsRequest& request, const DescribeCasterComponentsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCasterComponents(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeCasterComponentsOutcomeCallable LiveClient::describeCasterComponentsCallable(const DescribeCasterComponentsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeCasterComponentsOutcome()>>(
			[this, request]()
			{
			return this->describeCasterComponents(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeCasterConfigOutcome LiveClient::describeCasterConfig(const DescribeCasterConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeCasterConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeCasterConfigOutcome(DescribeCasterConfigResult(outcome.result()));
	else
		return DescribeCasterConfigOutcome(outcome.error());
}

void LiveClient::describeCasterConfigAsync(const DescribeCasterConfigRequest& request, const DescribeCasterConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCasterConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeCasterConfigOutcomeCallable LiveClient::describeCasterConfigCallable(const DescribeCasterConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeCasterConfigOutcome()>>(
			[this, request]()
			{
			return this->describeCasterConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeCasterLayoutsOutcome LiveClient::describeCasterLayouts(const DescribeCasterLayoutsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeCasterLayoutsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeCasterLayoutsOutcome(DescribeCasterLayoutsResult(outcome.result()));
	else
		return DescribeCasterLayoutsOutcome(outcome.error());
}

void LiveClient::describeCasterLayoutsAsync(const DescribeCasterLayoutsRequest& request, const DescribeCasterLayoutsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCasterLayouts(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeCasterLayoutsOutcomeCallable LiveClient::describeCasterLayoutsCallable(const DescribeCasterLayoutsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeCasterLayoutsOutcome()>>(
			[this, request]()
			{
			return this->describeCasterLayouts(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeCasterProgramOutcome LiveClient::describeCasterProgram(const DescribeCasterProgramRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeCasterProgramOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeCasterProgramOutcome(DescribeCasterProgramResult(outcome.result()));
	else
		return DescribeCasterProgramOutcome(outcome.error());
}

void LiveClient::describeCasterProgramAsync(const DescribeCasterProgramRequest& request, const DescribeCasterProgramAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCasterProgram(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeCasterProgramOutcomeCallable LiveClient::describeCasterProgramCallable(const DescribeCasterProgramRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeCasterProgramOutcome()>>(
			[this, request]()
			{
			return this->describeCasterProgram(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeCasterRtcInfoOutcome LiveClient::describeCasterRtcInfo(const DescribeCasterRtcInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeCasterRtcInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeCasterRtcInfoOutcome(DescribeCasterRtcInfoResult(outcome.result()));
	else
		return DescribeCasterRtcInfoOutcome(outcome.error());
}

void LiveClient::describeCasterRtcInfoAsync(const DescribeCasterRtcInfoRequest& request, const DescribeCasterRtcInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCasterRtcInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeCasterRtcInfoOutcomeCallable LiveClient::describeCasterRtcInfoCallable(const DescribeCasterRtcInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeCasterRtcInfoOutcome()>>(
			[this, request]()
			{
			return this->describeCasterRtcInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeCasterSceneAudioOutcome LiveClient::describeCasterSceneAudio(const DescribeCasterSceneAudioRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeCasterSceneAudioOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeCasterSceneAudioOutcome(DescribeCasterSceneAudioResult(outcome.result()));
	else
		return DescribeCasterSceneAudioOutcome(outcome.error());
}

void LiveClient::describeCasterSceneAudioAsync(const DescribeCasterSceneAudioRequest& request, const DescribeCasterSceneAudioAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCasterSceneAudio(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeCasterSceneAudioOutcomeCallable LiveClient::describeCasterSceneAudioCallable(const DescribeCasterSceneAudioRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeCasterSceneAudioOutcome()>>(
			[this, request]()
			{
			return this->describeCasterSceneAudio(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeCasterScenesOutcome LiveClient::describeCasterScenes(const DescribeCasterScenesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeCasterScenesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeCasterScenesOutcome(DescribeCasterScenesResult(outcome.result()));
	else
		return DescribeCasterScenesOutcome(outcome.error());
}

void LiveClient::describeCasterScenesAsync(const DescribeCasterScenesRequest& request, const DescribeCasterScenesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCasterScenes(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeCasterScenesOutcomeCallable LiveClient::describeCasterScenesCallable(const DescribeCasterScenesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeCasterScenesOutcome()>>(
			[this, request]()
			{
			return this->describeCasterScenes(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeCasterStreamUrlOutcome LiveClient::describeCasterStreamUrl(const DescribeCasterStreamUrlRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeCasterStreamUrlOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeCasterStreamUrlOutcome(DescribeCasterStreamUrlResult(outcome.result()));
	else
		return DescribeCasterStreamUrlOutcome(outcome.error());
}

void LiveClient::describeCasterStreamUrlAsync(const DescribeCasterStreamUrlRequest& request, const DescribeCasterStreamUrlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCasterStreamUrl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeCasterStreamUrlOutcomeCallable LiveClient::describeCasterStreamUrlCallable(const DescribeCasterStreamUrlRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeCasterStreamUrlOutcome()>>(
			[this, request]()
			{
			return this->describeCasterStreamUrl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeCasterSyncGroupOutcome LiveClient::describeCasterSyncGroup(const DescribeCasterSyncGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeCasterSyncGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeCasterSyncGroupOutcome(DescribeCasterSyncGroupResult(outcome.result()));
	else
		return DescribeCasterSyncGroupOutcome(outcome.error());
}

void LiveClient::describeCasterSyncGroupAsync(const DescribeCasterSyncGroupRequest& request, const DescribeCasterSyncGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCasterSyncGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeCasterSyncGroupOutcomeCallable LiveClient::describeCasterSyncGroupCallable(const DescribeCasterSyncGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeCasterSyncGroupOutcome()>>(
			[this, request]()
			{
			return this->describeCasterSyncGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeCasterVideoResourcesOutcome LiveClient::describeCasterVideoResources(const DescribeCasterVideoResourcesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeCasterVideoResourcesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeCasterVideoResourcesOutcome(DescribeCasterVideoResourcesResult(outcome.result()));
	else
		return DescribeCasterVideoResourcesOutcome(outcome.error());
}

void LiveClient::describeCasterVideoResourcesAsync(const DescribeCasterVideoResourcesRequest& request, const DescribeCasterVideoResourcesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCasterVideoResources(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeCasterVideoResourcesOutcomeCallable LiveClient::describeCasterVideoResourcesCallable(const DescribeCasterVideoResourcesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeCasterVideoResourcesOutcome()>>(
			[this, request]()
			{
			return this->describeCasterVideoResources(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeCastersOutcome LiveClient::describeCasters(const DescribeCastersRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeCastersOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeCastersOutcome(DescribeCastersResult(outcome.result()));
	else
		return DescribeCastersOutcome(outcome.error());
}

void LiveClient::describeCastersAsync(const DescribeCastersRequest& request, const DescribeCastersAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeCasters(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeCastersOutcomeCallable LiveClient::describeCastersCallable(const DescribeCastersRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeCastersOutcome()>>(
			[this, request]()
			{
			return this->describeCasters(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeForbidPushStreamRoomListOutcome LiveClient::describeForbidPushStreamRoomList(const DescribeForbidPushStreamRoomListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeForbidPushStreamRoomListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeForbidPushStreamRoomListOutcome(DescribeForbidPushStreamRoomListResult(outcome.result()));
	else
		return DescribeForbidPushStreamRoomListOutcome(outcome.error());
}

void LiveClient::describeForbidPushStreamRoomListAsync(const DescribeForbidPushStreamRoomListRequest& request, const DescribeForbidPushStreamRoomListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeForbidPushStreamRoomList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeForbidPushStreamRoomListOutcomeCallable LiveClient::describeForbidPushStreamRoomListCallable(const DescribeForbidPushStreamRoomListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeForbidPushStreamRoomListOutcome()>>(
			[this, request]()
			{
			return this->describeForbidPushStreamRoomList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeHlsLiveStreamRealTimeBpsDataOutcome LiveClient::describeHlsLiveStreamRealTimeBpsData(const DescribeHlsLiveStreamRealTimeBpsDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeHlsLiveStreamRealTimeBpsDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeHlsLiveStreamRealTimeBpsDataOutcome(DescribeHlsLiveStreamRealTimeBpsDataResult(outcome.result()));
	else
		return DescribeHlsLiveStreamRealTimeBpsDataOutcome(outcome.error());
}

void LiveClient::describeHlsLiveStreamRealTimeBpsDataAsync(const DescribeHlsLiveStreamRealTimeBpsDataRequest& request, const DescribeHlsLiveStreamRealTimeBpsDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeHlsLiveStreamRealTimeBpsData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeHlsLiveStreamRealTimeBpsDataOutcomeCallable LiveClient::describeHlsLiveStreamRealTimeBpsDataCallable(const DescribeHlsLiveStreamRealTimeBpsDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeHlsLiveStreamRealTimeBpsDataOutcome()>>(
			[this, request]()
			{
			return this->describeHlsLiveStreamRealTimeBpsData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeHtmlResourceOutcome LiveClient::describeHtmlResource(const DescribeHtmlResourceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeHtmlResourceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeHtmlResourceOutcome(DescribeHtmlResourceResult(outcome.result()));
	else
		return DescribeHtmlResourceOutcome(outcome.error());
}

void LiveClient::describeHtmlResourceAsync(const DescribeHtmlResourceRequest& request, const DescribeHtmlResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeHtmlResource(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeHtmlResourceOutcomeCallable LiveClient::describeHtmlResourceCallable(const DescribeHtmlResourceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeHtmlResourceOutcome()>>(
			[this, request]()
			{
			return this->describeHtmlResource(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveAsrConfigOutcome LiveClient::describeLiveAsrConfig(const DescribeLiveAsrConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveAsrConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveAsrConfigOutcome(DescribeLiveAsrConfigResult(outcome.result()));
	else
		return DescribeLiveAsrConfigOutcome(outcome.error());
}

void LiveClient::describeLiveAsrConfigAsync(const DescribeLiveAsrConfigRequest& request, const DescribeLiveAsrConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveAsrConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveAsrConfigOutcomeCallable LiveClient::describeLiveAsrConfigCallable(const DescribeLiveAsrConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveAsrConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLiveAsrConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveAudioAuditConfigOutcome LiveClient::describeLiveAudioAuditConfig(const DescribeLiveAudioAuditConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveAudioAuditConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveAudioAuditConfigOutcome(DescribeLiveAudioAuditConfigResult(outcome.result()));
	else
		return DescribeLiveAudioAuditConfigOutcome(outcome.error());
}

void LiveClient::describeLiveAudioAuditConfigAsync(const DescribeLiveAudioAuditConfigRequest& request, const DescribeLiveAudioAuditConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveAudioAuditConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveAudioAuditConfigOutcomeCallable LiveClient::describeLiveAudioAuditConfigCallable(const DescribeLiveAudioAuditConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveAudioAuditConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLiveAudioAuditConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveAudioAuditNotifyConfigOutcome LiveClient::describeLiveAudioAuditNotifyConfig(const DescribeLiveAudioAuditNotifyConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveAudioAuditNotifyConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveAudioAuditNotifyConfigOutcome(DescribeLiveAudioAuditNotifyConfigResult(outcome.result()));
	else
		return DescribeLiveAudioAuditNotifyConfigOutcome(outcome.error());
}

void LiveClient::describeLiveAudioAuditNotifyConfigAsync(const DescribeLiveAudioAuditNotifyConfigRequest& request, const DescribeLiveAudioAuditNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveAudioAuditNotifyConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveAudioAuditNotifyConfigOutcomeCallable LiveClient::describeLiveAudioAuditNotifyConfigCallable(const DescribeLiveAudioAuditNotifyConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveAudioAuditNotifyConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLiveAudioAuditNotifyConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveCertificateDetailOutcome LiveClient::describeLiveCertificateDetail(const DescribeLiveCertificateDetailRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveCertificateDetailOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveCertificateDetailOutcome(DescribeLiveCertificateDetailResult(outcome.result()));
	else
		return DescribeLiveCertificateDetailOutcome(outcome.error());
}

void LiveClient::describeLiveCertificateDetailAsync(const DescribeLiveCertificateDetailRequest& request, const DescribeLiveCertificateDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveCertificateDetail(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveCertificateDetailOutcomeCallable LiveClient::describeLiveCertificateDetailCallable(const DescribeLiveCertificateDetailRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveCertificateDetailOutcome()>>(
			[this, request]()
			{
			return this->describeLiveCertificateDetail(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveCertificateListOutcome LiveClient::describeLiveCertificateList(const DescribeLiveCertificateListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveCertificateListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveCertificateListOutcome(DescribeLiveCertificateListResult(outcome.result()));
	else
		return DescribeLiveCertificateListOutcome(outcome.error());
}

void LiveClient::describeLiveCertificateListAsync(const DescribeLiveCertificateListRequest& request, const DescribeLiveCertificateListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveCertificateList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveCertificateListOutcomeCallable LiveClient::describeLiveCertificateListCallable(const DescribeLiveCertificateListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveCertificateListOutcome()>>(
			[this, request]()
			{
			return this->describeLiveCertificateList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDetectNotifyConfigOutcome LiveClient::describeLiveDetectNotifyConfig(const DescribeLiveDetectNotifyConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDetectNotifyConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDetectNotifyConfigOutcome(DescribeLiveDetectNotifyConfigResult(outcome.result()));
	else
		return DescribeLiveDetectNotifyConfigOutcome(outcome.error());
}

void LiveClient::describeLiveDetectNotifyConfigAsync(const DescribeLiveDetectNotifyConfigRequest& request, const DescribeLiveDetectNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDetectNotifyConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDetectNotifyConfigOutcomeCallable LiveClient::describeLiveDetectNotifyConfigCallable(const DescribeLiveDetectNotifyConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDetectNotifyConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDetectNotifyConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDetectPornDataOutcome LiveClient::describeLiveDetectPornData(const DescribeLiveDetectPornDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDetectPornDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDetectPornDataOutcome(DescribeLiveDetectPornDataResult(outcome.result()));
	else
		return DescribeLiveDetectPornDataOutcome(outcome.error());
}

void LiveClient::describeLiveDetectPornDataAsync(const DescribeLiveDetectPornDataRequest& request, const DescribeLiveDetectPornDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDetectPornData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDetectPornDataOutcomeCallable LiveClient::describeLiveDetectPornDataCallable(const DescribeLiveDetectPornDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDetectPornDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDetectPornData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainBpsDataOutcome LiveClient::describeLiveDomainBpsData(const DescribeLiveDomainBpsDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainBpsDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainBpsDataOutcome(DescribeLiveDomainBpsDataResult(outcome.result()));
	else
		return DescribeLiveDomainBpsDataOutcome(outcome.error());
}

void LiveClient::describeLiveDomainBpsDataAsync(const DescribeLiveDomainBpsDataRequest& request, const DescribeLiveDomainBpsDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainBpsData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainBpsDataOutcomeCallable LiveClient::describeLiveDomainBpsDataCallable(const DescribeLiveDomainBpsDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainBpsDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainBpsData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainBpsDataByTimeStampOutcome LiveClient::describeLiveDomainBpsDataByTimeStamp(const DescribeLiveDomainBpsDataByTimeStampRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainBpsDataByTimeStampOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainBpsDataByTimeStampOutcome(DescribeLiveDomainBpsDataByTimeStampResult(outcome.result()));
	else
		return DescribeLiveDomainBpsDataByTimeStampOutcome(outcome.error());
}

void LiveClient::describeLiveDomainBpsDataByTimeStampAsync(const DescribeLiveDomainBpsDataByTimeStampRequest& request, const DescribeLiveDomainBpsDataByTimeStampAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainBpsDataByTimeStamp(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainBpsDataByTimeStampOutcomeCallable LiveClient::describeLiveDomainBpsDataByTimeStampCallable(const DescribeLiveDomainBpsDataByTimeStampRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainBpsDataByTimeStampOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainBpsDataByTimeStamp(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainCertificateInfoOutcome LiveClient::describeLiveDomainCertificateInfo(const DescribeLiveDomainCertificateInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainCertificateInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainCertificateInfoOutcome(DescribeLiveDomainCertificateInfoResult(outcome.result()));
	else
		return DescribeLiveDomainCertificateInfoOutcome(outcome.error());
}

void LiveClient::describeLiveDomainCertificateInfoAsync(const DescribeLiveDomainCertificateInfoRequest& request, const DescribeLiveDomainCertificateInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainCertificateInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainCertificateInfoOutcomeCallable LiveClient::describeLiveDomainCertificateInfoCallable(const DescribeLiveDomainCertificateInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainCertificateInfoOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainCertificateInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainConfigsOutcome LiveClient::describeLiveDomainConfigs(const DescribeLiveDomainConfigsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainConfigsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainConfigsOutcome(DescribeLiveDomainConfigsResult(outcome.result()));
	else
		return DescribeLiveDomainConfigsOutcome(outcome.error());
}

void LiveClient::describeLiveDomainConfigsAsync(const DescribeLiveDomainConfigsRequest& request, const DescribeLiveDomainConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainConfigs(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainConfigsOutcomeCallable LiveClient::describeLiveDomainConfigsCallable(const DescribeLiveDomainConfigsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainConfigsOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainConfigs(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainDetailOutcome LiveClient::describeLiveDomainDetail(const DescribeLiveDomainDetailRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainDetailOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainDetailOutcome(DescribeLiveDomainDetailResult(outcome.result()));
	else
		return DescribeLiveDomainDetailOutcome(outcome.error());
}

void LiveClient::describeLiveDomainDetailAsync(const DescribeLiveDomainDetailRequest& request, const DescribeLiveDomainDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainDetail(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainDetailOutcomeCallable LiveClient::describeLiveDomainDetailCallable(const DescribeLiveDomainDetailRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainDetailOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainDetail(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainFrameRateAndBitRateDataOutcome LiveClient::describeLiveDomainFrameRateAndBitRateData(const DescribeLiveDomainFrameRateAndBitRateDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainFrameRateAndBitRateDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainFrameRateAndBitRateDataOutcome(DescribeLiveDomainFrameRateAndBitRateDataResult(outcome.result()));
	else
		return DescribeLiveDomainFrameRateAndBitRateDataOutcome(outcome.error());
}

void LiveClient::describeLiveDomainFrameRateAndBitRateDataAsync(const DescribeLiveDomainFrameRateAndBitRateDataRequest& request, const DescribeLiveDomainFrameRateAndBitRateDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainFrameRateAndBitRateData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainFrameRateAndBitRateDataOutcomeCallable LiveClient::describeLiveDomainFrameRateAndBitRateDataCallable(const DescribeLiveDomainFrameRateAndBitRateDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainFrameRateAndBitRateDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainFrameRateAndBitRateData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainLimitOutcome LiveClient::describeLiveDomainLimit(const DescribeLiveDomainLimitRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainLimitOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainLimitOutcome(DescribeLiveDomainLimitResult(outcome.result()));
	else
		return DescribeLiveDomainLimitOutcome(outcome.error());
}

void LiveClient::describeLiveDomainLimitAsync(const DescribeLiveDomainLimitRequest& request, const DescribeLiveDomainLimitAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainLimit(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainLimitOutcomeCallable LiveClient::describeLiveDomainLimitCallable(const DescribeLiveDomainLimitRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainLimitOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainLimit(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainMappingOutcome LiveClient::describeLiveDomainMapping(const DescribeLiveDomainMappingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainMappingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainMappingOutcome(DescribeLiveDomainMappingResult(outcome.result()));
	else
		return DescribeLiveDomainMappingOutcome(outcome.error());
}

void LiveClient::describeLiveDomainMappingAsync(const DescribeLiveDomainMappingRequest& request, const DescribeLiveDomainMappingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainMapping(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainMappingOutcomeCallable LiveClient::describeLiveDomainMappingCallable(const DescribeLiveDomainMappingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainMappingOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainMapping(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainOnlineUserNumOutcome LiveClient::describeLiveDomainOnlineUserNum(const DescribeLiveDomainOnlineUserNumRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainOnlineUserNumOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainOnlineUserNumOutcome(DescribeLiveDomainOnlineUserNumResult(outcome.result()));
	else
		return DescribeLiveDomainOnlineUserNumOutcome(outcome.error());
}

void LiveClient::describeLiveDomainOnlineUserNumAsync(const DescribeLiveDomainOnlineUserNumRequest& request, const DescribeLiveDomainOnlineUserNumAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainOnlineUserNum(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainOnlineUserNumOutcomeCallable LiveClient::describeLiveDomainOnlineUserNumCallable(const DescribeLiveDomainOnlineUserNumRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainOnlineUserNumOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainOnlineUserNum(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainPushBpsDataOutcome LiveClient::describeLiveDomainPushBpsData(const DescribeLiveDomainPushBpsDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainPushBpsDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainPushBpsDataOutcome(DescribeLiveDomainPushBpsDataResult(outcome.result()));
	else
		return DescribeLiveDomainPushBpsDataOutcome(outcome.error());
}

void LiveClient::describeLiveDomainPushBpsDataAsync(const DescribeLiveDomainPushBpsDataRequest& request, const DescribeLiveDomainPushBpsDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainPushBpsData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainPushBpsDataOutcomeCallable LiveClient::describeLiveDomainPushBpsDataCallable(const DescribeLiveDomainPushBpsDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainPushBpsDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainPushBpsData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainPushTrafficDataOutcome LiveClient::describeLiveDomainPushTrafficData(const DescribeLiveDomainPushTrafficDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainPushTrafficDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainPushTrafficDataOutcome(DescribeLiveDomainPushTrafficDataResult(outcome.result()));
	else
		return DescribeLiveDomainPushTrafficDataOutcome(outcome.error());
}

void LiveClient::describeLiveDomainPushTrafficDataAsync(const DescribeLiveDomainPushTrafficDataRequest& request, const DescribeLiveDomainPushTrafficDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainPushTrafficData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainPushTrafficDataOutcomeCallable LiveClient::describeLiveDomainPushTrafficDataCallable(const DescribeLiveDomainPushTrafficDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainPushTrafficDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainPushTrafficData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainRealTimeBpsDataOutcome LiveClient::describeLiveDomainRealTimeBpsData(const DescribeLiveDomainRealTimeBpsDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainRealTimeBpsDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainRealTimeBpsDataOutcome(DescribeLiveDomainRealTimeBpsDataResult(outcome.result()));
	else
		return DescribeLiveDomainRealTimeBpsDataOutcome(outcome.error());
}

void LiveClient::describeLiveDomainRealTimeBpsDataAsync(const DescribeLiveDomainRealTimeBpsDataRequest& request, const DescribeLiveDomainRealTimeBpsDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainRealTimeBpsData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainRealTimeBpsDataOutcomeCallable LiveClient::describeLiveDomainRealTimeBpsDataCallable(const DescribeLiveDomainRealTimeBpsDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainRealTimeBpsDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainRealTimeBpsData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainRealTimeHttpCodeDataOutcome LiveClient::describeLiveDomainRealTimeHttpCodeData(const DescribeLiveDomainRealTimeHttpCodeDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainRealTimeHttpCodeDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainRealTimeHttpCodeDataOutcome(DescribeLiveDomainRealTimeHttpCodeDataResult(outcome.result()));
	else
		return DescribeLiveDomainRealTimeHttpCodeDataOutcome(outcome.error());
}

void LiveClient::describeLiveDomainRealTimeHttpCodeDataAsync(const DescribeLiveDomainRealTimeHttpCodeDataRequest& request, const DescribeLiveDomainRealTimeHttpCodeDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainRealTimeHttpCodeData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainRealTimeHttpCodeDataOutcomeCallable LiveClient::describeLiveDomainRealTimeHttpCodeDataCallable(const DescribeLiveDomainRealTimeHttpCodeDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainRealTimeHttpCodeDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainRealTimeHttpCodeData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainRealTimeTrafficDataOutcome LiveClient::describeLiveDomainRealTimeTrafficData(const DescribeLiveDomainRealTimeTrafficDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainRealTimeTrafficDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainRealTimeTrafficDataOutcome(DescribeLiveDomainRealTimeTrafficDataResult(outcome.result()));
	else
		return DescribeLiveDomainRealTimeTrafficDataOutcome(outcome.error());
}

void LiveClient::describeLiveDomainRealTimeTrafficDataAsync(const DescribeLiveDomainRealTimeTrafficDataRequest& request, const DescribeLiveDomainRealTimeTrafficDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainRealTimeTrafficData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainRealTimeTrafficDataOutcomeCallable LiveClient::describeLiveDomainRealTimeTrafficDataCallable(const DescribeLiveDomainRealTimeTrafficDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainRealTimeTrafficDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainRealTimeTrafficData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainRealtimeLogDeliveryOutcome LiveClient::describeLiveDomainRealtimeLogDelivery(const DescribeLiveDomainRealtimeLogDeliveryRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainRealtimeLogDeliveryOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainRealtimeLogDeliveryOutcome(DescribeLiveDomainRealtimeLogDeliveryResult(outcome.result()));
	else
		return DescribeLiveDomainRealtimeLogDeliveryOutcome(outcome.error());
}

void LiveClient::describeLiveDomainRealtimeLogDeliveryAsync(const DescribeLiveDomainRealtimeLogDeliveryRequest& request, const DescribeLiveDomainRealtimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainRealtimeLogDelivery(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainRealtimeLogDeliveryOutcomeCallable LiveClient::describeLiveDomainRealtimeLogDeliveryCallable(const DescribeLiveDomainRealtimeLogDeliveryRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainRealtimeLogDeliveryOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainRealtimeLogDelivery(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainRecordDataOutcome LiveClient::describeLiveDomainRecordData(const DescribeLiveDomainRecordDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainRecordDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainRecordDataOutcome(DescribeLiveDomainRecordDataResult(outcome.result()));
	else
		return DescribeLiveDomainRecordDataOutcome(outcome.error());
}

void LiveClient::describeLiveDomainRecordDataAsync(const DescribeLiveDomainRecordDataRequest& request, const DescribeLiveDomainRecordDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainRecordData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainRecordDataOutcomeCallable LiveClient::describeLiveDomainRecordDataCallable(const DescribeLiveDomainRecordDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainRecordDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainRecordData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainSnapshotDataOutcome LiveClient::describeLiveDomainSnapshotData(const DescribeLiveDomainSnapshotDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainSnapshotDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainSnapshotDataOutcome(DescribeLiveDomainSnapshotDataResult(outcome.result()));
	else
		return DescribeLiveDomainSnapshotDataOutcome(outcome.error());
}

void LiveClient::describeLiveDomainSnapshotDataAsync(const DescribeLiveDomainSnapshotDataRequest& request, const DescribeLiveDomainSnapshotDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainSnapshotData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainSnapshotDataOutcomeCallable LiveClient::describeLiveDomainSnapshotDataCallable(const DescribeLiveDomainSnapshotDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainSnapshotDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainSnapshotData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainTimeShiftDataOutcome LiveClient::describeLiveDomainTimeShiftData(const DescribeLiveDomainTimeShiftDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainTimeShiftDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainTimeShiftDataOutcome(DescribeLiveDomainTimeShiftDataResult(outcome.result()));
	else
		return DescribeLiveDomainTimeShiftDataOutcome(outcome.error());
}

void LiveClient::describeLiveDomainTimeShiftDataAsync(const DescribeLiveDomainTimeShiftDataRequest& request, const DescribeLiveDomainTimeShiftDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainTimeShiftData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainTimeShiftDataOutcomeCallable LiveClient::describeLiveDomainTimeShiftDataCallable(const DescribeLiveDomainTimeShiftDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainTimeShiftDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainTimeShiftData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainTrafficDataOutcome LiveClient::describeLiveDomainTrafficData(const DescribeLiveDomainTrafficDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainTrafficDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainTrafficDataOutcome(DescribeLiveDomainTrafficDataResult(outcome.result()));
	else
		return DescribeLiveDomainTrafficDataOutcome(outcome.error());
}

void LiveClient::describeLiveDomainTrafficDataAsync(const DescribeLiveDomainTrafficDataRequest& request, const DescribeLiveDomainTrafficDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainTrafficData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainTrafficDataOutcomeCallable LiveClient::describeLiveDomainTrafficDataCallable(const DescribeLiveDomainTrafficDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainTrafficDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainTrafficData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveDomainTranscodeDataOutcome LiveClient::describeLiveDomainTranscodeData(const DescribeLiveDomainTranscodeDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveDomainTranscodeDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveDomainTranscodeDataOutcome(DescribeLiveDomainTranscodeDataResult(outcome.result()));
	else
		return DescribeLiveDomainTranscodeDataOutcome(outcome.error());
}

void LiveClient::describeLiveDomainTranscodeDataAsync(const DescribeLiveDomainTranscodeDataRequest& request, const DescribeLiveDomainTranscodeDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveDomainTranscodeData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveDomainTranscodeDataOutcomeCallable LiveClient::describeLiveDomainTranscodeDataCallable(const DescribeLiveDomainTranscodeDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveDomainTranscodeDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveDomainTranscodeData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveLazyPullStreamConfigOutcome LiveClient::describeLiveLazyPullStreamConfig(const DescribeLiveLazyPullStreamConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveLazyPullStreamConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveLazyPullStreamConfigOutcome(DescribeLiveLazyPullStreamConfigResult(outcome.result()));
	else
		return DescribeLiveLazyPullStreamConfigOutcome(outcome.error());
}

void LiveClient::describeLiveLazyPullStreamConfigAsync(const DescribeLiveLazyPullStreamConfigRequest& request, const DescribeLiveLazyPullStreamConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveLazyPullStreamConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveLazyPullStreamConfigOutcomeCallable LiveClient::describeLiveLazyPullStreamConfigCallable(const DescribeLiveLazyPullStreamConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveLazyPullStreamConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLiveLazyPullStreamConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLivePullStreamConfigOutcome LiveClient::describeLivePullStreamConfig(const DescribeLivePullStreamConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLivePullStreamConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLivePullStreamConfigOutcome(DescribeLivePullStreamConfigResult(outcome.result()));
	else
		return DescribeLivePullStreamConfigOutcome(outcome.error());
}

void LiveClient::describeLivePullStreamConfigAsync(const DescribeLivePullStreamConfigRequest& request, const DescribeLivePullStreamConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLivePullStreamConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLivePullStreamConfigOutcomeCallable LiveClient::describeLivePullStreamConfigCallable(const DescribeLivePullStreamConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLivePullStreamConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLivePullStreamConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveRealtimeDeliveryAccOutcome LiveClient::describeLiveRealtimeDeliveryAcc(const DescribeLiveRealtimeDeliveryAccRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveRealtimeDeliveryAccOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveRealtimeDeliveryAccOutcome(DescribeLiveRealtimeDeliveryAccResult(outcome.result()));
	else
		return DescribeLiveRealtimeDeliveryAccOutcome(outcome.error());
}

void LiveClient::describeLiveRealtimeDeliveryAccAsync(const DescribeLiveRealtimeDeliveryAccRequest& request, const DescribeLiveRealtimeDeliveryAccAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveRealtimeDeliveryAcc(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveRealtimeDeliveryAccOutcomeCallable LiveClient::describeLiveRealtimeDeliveryAccCallable(const DescribeLiveRealtimeDeliveryAccRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveRealtimeDeliveryAccOutcome()>>(
			[this, request]()
			{
			return this->describeLiveRealtimeDeliveryAcc(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveRealtimeLogAuthorizedOutcome LiveClient::describeLiveRealtimeLogAuthorized(const DescribeLiveRealtimeLogAuthorizedRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveRealtimeLogAuthorizedOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveRealtimeLogAuthorizedOutcome(DescribeLiveRealtimeLogAuthorizedResult(outcome.result()));
	else
		return DescribeLiveRealtimeLogAuthorizedOutcome(outcome.error());
}

void LiveClient::describeLiveRealtimeLogAuthorizedAsync(const DescribeLiveRealtimeLogAuthorizedRequest& request, const DescribeLiveRealtimeLogAuthorizedAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveRealtimeLogAuthorized(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveRealtimeLogAuthorizedOutcomeCallable LiveClient::describeLiveRealtimeLogAuthorizedCallable(const DescribeLiveRealtimeLogAuthorizedRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveRealtimeLogAuthorizedOutcome()>>(
			[this, request]()
			{
			return this->describeLiveRealtimeLogAuthorized(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveRecordConfigOutcome LiveClient::describeLiveRecordConfig(const DescribeLiveRecordConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveRecordConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveRecordConfigOutcome(DescribeLiveRecordConfigResult(outcome.result()));
	else
		return DescribeLiveRecordConfigOutcome(outcome.error());
}

void LiveClient::describeLiveRecordConfigAsync(const DescribeLiveRecordConfigRequest& request, const DescribeLiveRecordConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveRecordConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveRecordConfigOutcomeCallable LiveClient::describeLiveRecordConfigCallable(const DescribeLiveRecordConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveRecordConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLiveRecordConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveRecordNotifyConfigOutcome LiveClient::describeLiveRecordNotifyConfig(const DescribeLiveRecordNotifyConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveRecordNotifyConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveRecordNotifyConfigOutcome(DescribeLiveRecordNotifyConfigResult(outcome.result()));
	else
		return DescribeLiveRecordNotifyConfigOutcome(outcome.error());
}

void LiveClient::describeLiveRecordNotifyConfigAsync(const DescribeLiveRecordNotifyConfigRequest& request, const DescribeLiveRecordNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveRecordNotifyConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveRecordNotifyConfigOutcomeCallable LiveClient::describeLiveRecordNotifyConfigCallable(const DescribeLiveRecordNotifyConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveRecordNotifyConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLiveRecordNotifyConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveRecordVodConfigsOutcome LiveClient::describeLiveRecordVodConfigs(const DescribeLiveRecordVodConfigsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveRecordVodConfigsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveRecordVodConfigsOutcome(DescribeLiveRecordVodConfigsResult(outcome.result()));
	else
		return DescribeLiveRecordVodConfigsOutcome(outcome.error());
}

void LiveClient::describeLiveRecordVodConfigsAsync(const DescribeLiveRecordVodConfigsRequest& request, const DescribeLiveRecordVodConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveRecordVodConfigs(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveRecordVodConfigsOutcomeCallable LiveClient::describeLiveRecordVodConfigsCallable(const DescribeLiveRecordVodConfigsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveRecordVodConfigsOutcome()>>(
			[this, request]()
			{
			return this->describeLiveRecordVodConfigs(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveSnapshotConfigOutcome LiveClient::describeLiveSnapshotConfig(const DescribeLiveSnapshotConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveSnapshotConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveSnapshotConfigOutcome(DescribeLiveSnapshotConfigResult(outcome.result()));
	else
		return DescribeLiveSnapshotConfigOutcome(outcome.error());
}

void LiveClient::describeLiveSnapshotConfigAsync(const DescribeLiveSnapshotConfigRequest& request, const DescribeLiveSnapshotConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveSnapshotConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveSnapshotConfigOutcomeCallable LiveClient::describeLiveSnapshotConfigCallable(const DescribeLiveSnapshotConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveSnapshotConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLiveSnapshotConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveSnapshotDetectPornConfigOutcome LiveClient::describeLiveSnapshotDetectPornConfig(const DescribeLiveSnapshotDetectPornConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveSnapshotDetectPornConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveSnapshotDetectPornConfigOutcome(DescribeLiveSnapshotDetectPornConfigResult(outcome.result()));
	else
		return DescribeLiveSnapshotDetectPornConfigOutcome(outcome.error());
}

void LiveClient::describeLiveSnapshotDetectPornConfigAsync(const DescribeLiveSnapshotDetectPornConfigRequest& request, const DescribeLiveSnapshotDetectPornConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveSnapshotDetectPornConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveSnapshotDetectPornConfigOutcomeCallable LiveClient::describeLiveSnapshotDetectPornConfigCallable(const DescribeLiveSnapshotDetectPornConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveSnapshotDetectPornConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLiveSnapshotDetectPornConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamBitRateDataOutcome LiveClient::describeLiveStreamBitRateData(const DescribeLiveStreamBitRateDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamBitRateDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamBitRateDataOutcome(DescribeLiveStreamBitRateDataResult(outcome.result()));
	else
		return DescribeLiveStreamBitRateDataOutcome(outcome.error());
}

void LiveClient::describeLiveStreamBitRateDataAsync(const DescribeLiveStreamBitRateDataRequest& request, const DescribeLiveStreamBitRateDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamBitRateData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamBitRateDataOutcomeCallable LiveClient::describeLiveStreamBitRateDataCallable(const DescribeLiveStreamBitRateDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamBitRateDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamBitRateData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamCountOutcome LiveClient::describeLiveStreamCount(const DescribeLiveStreamCountRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamCountOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamCountOutcome(DescribeLiveStreamCountResult(outcome.result()));
	else
		return DescribeLiveStreamCountOutcome(outcome.error());
}

void LiveClient::describeLiveStreamCountAsync(const DescribeLiveStreamCountRequest& request, const DescribeLiveStreamCountAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamCount(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamCountOutcomeCallable LiveClient::describeLiveStreamCountCallable(const DescribeLiveStreamCountRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamCountOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamCount(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamDelayConfigOutcome LiveClient::describeLiveStreamDelayConfig(const DescribeLiveStreamDelayConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamDelayConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamDelayConfigOutcome(DescribeLiveStreamDelayConfigResult(outcome.result()));
	else
		return DescribeLiveStreamDelayConfigOutcome(outcome.error());
}

void LiveClient::describeLiveStreamDelayConfigAsync(const DescribeLiveStreamDelayConfigRequest& request, const DescribeLiveStreamDelayConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamDelayConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamDelayConfigOutcomeCallable LiveClient::describeLiveStreamDelayConfigCallable(const DescribeLiveStreamDelayConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamDelayConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamDelayConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamHistoryUserNumOutcome LiveClient::describeLiveStreamHistoryUserNum(const DescribeLiveStreamHistoryUserNumRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamHistoryUserNumOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamHistoryUserNumOutcome(DescribeLiveStreamHistoryUserNumResult(outcome.result()));
	else
		return DescribeLiveStreamHistoryUserNumOutcome(outcome.error());
}

void LiveClient::describeLiveStreamHistoryUserNumAsync(const DescribeLiveStreamHistoryUserNumRequest& request, const DescribeLiveStreamHistoryUserNumAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamHistoryUserNum(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamHistoryUserNumOutcomeCallable LiveClient::describeLiveStreamHistoryUserNumCallable(const DescribeLiveStreamHistoryUserNumRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamHistoryUserNumOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamHistoryUserNum(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamOnlineUserNumOutcome LiveClient::describeLiveStreamOnlineUserNum(const DescribeLiveStreamOnlineUserNumRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamOnlineUserNumOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamOnlineUserNumOutcome(DescribeLiveStreamOnlineUserNumResult(outcome.result()));
	else
		return DescribeLiveStreamOnlineUserNumOutcome(outcome.error());
}

void LiveClient::describeLiveStreamOnlineUserNumAsync(const DescribeLiveStreamOnlineUserNumRequest& request, const DescribeLiveStreamOnlineUserNumAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamOnlineUserNum(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamOnlineUserNumOutcomeCallable LiveClient::describeLiveStreamOnlineUserNumCallable(const DescribeLiveStreamOnlineUserNumRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamOnlineUserNumOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamOnlineUserNum(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamOptimizedFeatureConfigOutcome LiveClient::describeLiveStreamOptimizedFeatureConfig(const DescribeLiveStreamOptimizedFeatureConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamOptimizedFeatureConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamOptimizedFeatureConfigOutcome(DescribeLiveStreamOptimizedFeatureConfigResult(outcome.result()));
	else
		return DescribeLiveStreamOptimizedFeatureConfigOutcome(outcome.error());
}

void LiveClient::describeLiveStreamOptimizedFeatureConfigAsync(const DescribeLiveStreamOptimizedFeatureConfigRequest& request, const DescribeLiveStreamOptimizedFeatureConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamOptimizedFeatureConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamOptimizedFeatureConfigOutcomeCallable LiveClient::describeLiveStreamOptimizedFeatureConfigCallable(const DescribeLiveStreamOptimizedFeatureConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamOptimizedFeatureConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamOptimizedFeatureConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamRecordContentOutcome LiveClient::describeLiveStreamRecordContent(const DescribeLiveStreamRecordContentRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamRecordContentOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamRecordContentOutcome(DescribeLiveStreamRecordContentResult(outcome.result()));
	else
		return DescribeLiveStreamRecordContentOutcome(outcome.error());
}

void LiveClient::describeLiveStreamRecordContentAsync(const DescribeLiveStreamRecordContentRequest& request, const DescribeLiveStreamRecordContentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamRecordContent(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamRecordContentOutcomeCallable LiveClient::describeLiveStreamRecordContentCallable(const DescribeLiveStreamRecordContentRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamRecordContentOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamRecordContent(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamRecordIndexFileOutcome LiveClient::describeLiveStreamRecordIndexFile(const DescribeLiveStreamRecordIndexFileRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamRecordIndexFileOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamRecordIndexFileOutcome(DescribeLiveStreamRecordIndexFileResult(outcome.result()));
	else
		return DescribeLiveStreamRecordIndexFileOutcome(outcome.error());
}

void LiveClient::describeLiveStreamRecordIndexFileAsync(const DescribeLiveStreamRecordIndexFileRequest& request, const DescribeLiveStreamRecordIndexFileAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamRecordIndexFile(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamRecordIndexFileOutcomeCallable LiveClient::describeLiveStreamRecordIndexFileCallable(const DescribeLiveStreamRecordIndexFileRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamRecordIndexFileOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamRecordIndexFile(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamRecordIndexFilesOutcome LiveClient::describeLiveStreamRecordIndexFiles(const DescribeLiveStreamRecordIndexFilesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamRecordIndexFilesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamRecordIndexFilesOutcome(DescribeLiveStreamRecordIndexFilesResult(outcome.result()));
	else
		return DescribeLiveStreamRecordIndexFilesOutcome(outcome.error());
}

void LiveClient::describeLiveStreamRecordIndexFilesAsync(const DescribeLiveStreamRecordIndexFilesRequest& request, const DescribeLiveStreamRecordIndexFilesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamRecordIndexFiles(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamRecordIndexFilesOutcomeCallable LiveClient::describeLiveStreamRecordIndexFilesCallable(const DescribeLiveStreamRecordIndexFilesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamRecordIndexFilesOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamRecordIndexFiles(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamSnapshotInfoOutcome LiveClient::describeLiveStreamSnapshotInfo(const DescribeLiveStreamSnapshotInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamSnapshotInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamSnapshotInfoOutcome(DescribeLiveStreamSnapshotInfoResult(outcome.result()));
	else
		return DescribeLiveStreamSnapshotInfoOutcome(outcome.error());
}

void LiveClient::describeLiveStreamSnapshotInfoAsync(const DescribeLiveStreamSnapshotInfoRequest& request, const DescribeLiveStreamSnapshotInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamSnapshotInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamSnapshotInfoOutcomeCallable LiveClient::describeLiveStreamSnapshotInfoCallable(const DescribeLiveStreamSnapshotInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamSnapshotInfoOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamSnapshotInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamTranscodeInfoOutcome LiveClient::describeLiveStreamTranscodeInfo(const DescribeLiveStreamTranscodeInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamTranscodeInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamTranscodeInfoOutcome(DescribeLiveStreamTranscodeInfoResult(outcome.result()));
	else
		return DescribeLiveStreamTranscodeInfoOutcome(outcome.error());
}

void LiveClient::describeLiveStreamTranscodeInfoAsync(const DescribeLiveStreamTranscodeInfoRequest& request, const DescribeLiveStreamTranscodeInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamTranscodeInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamTranscodeInfoOutcomeCallable LiveClient::describeLiveStreamTranscodeInfoCallable(const DescribeLiveStreamTranscodeInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamTranscodeInfoOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamTranscodeInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamTranscodeStreamNumOutcome LiveClient::describeLiveStreamTranscodeStreamNum(const DescribeLiveStreamTranscodeStreamNumRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamTranscodeStreamNumOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamTranscodeStreamNumOutcome(DescribeLiveStreamTranscodeStreamNumResult(outcome.result()));
	else
		return DescribeLiveStreamTranscodeStreamNumOutcome(outcome.error());
}

void LiveClient::describeLiveStreamTranscodeStreamNumAsync(const DescribeLiveStreamTranscodeStreamNumRequest& request, const DescribeLiveStreamTranscodeStreamNumAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamTranscodeStreamNum(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamTranscodeStreamNumOutcomeCallable LiveClient::describeLiveStreamTranscodeStreamNumCallable(const DescribeLiveStreamTranscodeStreamNumRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamTranscodeStreamNumOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamTranscodeStreamNum(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamsBlockListOutcome LiveClient::describeLiveStreamsBlockList(const DescribeLiveStreamsBlockListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamsBlockListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamsBlockListOutcome(DescribeLiveStreamsBlockListResult(outcome.result()));
	else
		return DescribeLiveStreamsBlockListOutcome(outcome.error());
}

void LiveClient::describeLiveStreamsBlockListAsync(const DescribeLiveStreamsBlockListRequest& request, const DescribeLiveStreamsBlockListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamsBlockList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamsBlockListOutcomeCallable LiveClient::describeLiveStreamsBlockListCallable(const DescribeLiveStreamsBlockListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamsBlockListOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamsBlockList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamsControlHistoryOutcome LiveClient::describeLiveStreamsControlHistory(const DescribeLiveStreamsControlHistoryRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamsControlHistoryOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamsControlHistoryOutcome(DescribeLiveStreamsControlHistoryResult(outcome.result()));
	else
		return DescribeLiveStreamsControlHistoryOutcome(outcome.error());
}

void LiveClient::describeLiveStreamsControlHistoryAsync(const DescribeLiveStreamsControlHistoryRequest& request, const DescribeLiveStreamsControlHistoryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamsControlHistory(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamsControlHistoryOutcomeCallable LiveClient::describeLiveStreamsControlHistoryCallable(const DescribeLiveStreamsControlHistoryRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamsControlHistoryOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamsControlHistory(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamsFrameRateAndBitRateDataOutcome LiveClient::describeLiveStreamsFrameRateAndBitRateData(const DescribeLiveStreamsFrameRateAndBitRateDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamsFrameRateAndBitRateDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamsFrameRateAndBitRateDataOutcome(DescribeLiveStreamsFrameRateAndBitRateDataResult(outcome.result()));
	else
		return DescribeLiveStreamsFrameRateAndBitRateDataOutcome(outcome.error());
}

void LiveClient::describeLiveStreamsFrameRateAndBitRateDataAsync(const DescribeLiveStreamsFrameRateAndBitRateDataRequest& request, const DescribeLiveStreamsFrameRateAndBitRateDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamsFrameRateAndBitRateData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamsFrameRateAndBitRateDataOutcomeCallable LiveClient::describeLiveStreamsFrameRateAndBitRateDataCallable(const DescribeLiveStreamsFrameRateAndBitRateDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamsFrameRateAndBitRateDataOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamsFrameRateAndBitRateData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamsNotifyUrlConfigOutcome LiveClient::describeLiveStreamsNotifyUrlConfig(const DescribeLiveStreamsNotifyUrlConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamsNotifyUrlConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamsNotifyUrlConfigOutcome(DescribeLiveStreamsNotifyUrlConfigResult(outcome.result()));
	else
		return DescribeLiveStreamsNotifyUrlConfigOutcome(outcome.error());
}

void LiveClient::describeLiveStreamsNotifyUrlConfigAsync(const DescribeLiveStreamsNotifyUrlConfigRequest& request, const DescribeLiveStreamsNotifyUrlConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamsNotifyUrlConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamsNotifyUrlConfigOutcomeCallable LiveClient::describeLiveStreamsNotifyUrlConfigCallable(const DescribeLiveStreamsNotifyUrlConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamsNotifyUrlConfigOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamsNotifyUrlConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamsOnlineListOutcome LiveClient::describeLiveStreamsOnlineList(const DescribeLiveStreamsOnlineListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamsOnlineListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamsOnlineListOutcome(DescribeLiveStreamsOnlineListResult(outcome.result()));
	else
		return DescribeLiveStreamsOnlineListOutcome(outcome.error());
}

void LiveClient::describeLiveStreamsOnlineListAsync(const DescribeLiveStreamsOnlineListRequest& request, const DescribeLiveStreamsOnlineListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamsOnlineList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamsOnlineListOutcomeCallable LiveClient::describeLiveStreamsOnlineListCallable(const DescribeLiveStreamsOnlineListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamsOnlineListOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamsOnlineList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveStreamsPublishListOutcome LiveClient::describeLiveStreamsPublishList(const DescribeLiveStreamsPublishListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveStreamsPublishListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveStreamsPublishListOutcome(DescribeLiveStreamsPublishListResult(outcome.result()));
	else
		return DescribeLiveStreamsPublishListOutcome(outcome.error());
}

void LiveClient::describeLiveStreamsPublishListAsync(const DescribeLiveStreamsPublishListRequest& request, const DescribeLiveStreamsPublishListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveStreamsPublishList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveStreamsPublishListOutcomeCallable LiveClient::describeLiveStreamsPublishListCallable(const DescribeLiveStreamsPublishListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveStreamsPublishListOutcome()>>(
			[this, request]()
			{
			return this->describeLiveStreamsPublishList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveTagResourcesOutcome LiveClient::describeLiveTagResources(const DescribeLiveTagResourcesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveTagResourcesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveTagResourcesOutcome(DescribeLiveTagResourcesResult(outcome.result()));
	else
		return DescribeLiveTagResourcesOutcome(outcome.error());
}

void LiveClient::describeLiveTagResourcesAsync(const DescribeLiveTagResourcesRequest& request, const DescribeLiveTagResourcesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveTagResources(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveTagResourcesOutcomeCallable LiveClient::describeLiveTagResourcesCallable(const DescribeLiveTagResourcesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveTagResourcesOutcome()>>(
			[this, request]()
			{
			return this->describeLiveTagResources(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveTopDomainsByFlowOutcome LiveClient::describeLiveTopDomainsByFlow(const DescribeLiveTopDomainsByFlowRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveTopDomainsByFlowOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveTopDomainsByFlowOutcome(DescribeLiveTopDomainsByFlowResult(outcome.result()));
	else
		return DescribeLiveTopDomainsByFlowOutcome(outcome.error());
}

void LiveClient::describeLiveTopDomainsByFlowAsync(const DescribeLiveTopDomainsByFlowRequest& request, const DescribeLiveTopDomainsByFlowAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveTopDomainsByFlow(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveTopDomainsByFlowOutcomeCallable LiveClient::describeLiveTopDomainsByFlowCallable(const DescribeLiveTopDomainsByFlowRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveTopDomainsByFlowOutcome()>>(
			[this, request]()
			{
			return this->describeLiveTopDomainsByFlow(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveUserDomainsOutcome LiveClient::describeLiveUserDomains(const DescribeLiveUserDomainsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveUserDomainsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveUserDomainsOutcome(DescribeLiveUserDomainsResult(outcome.result()));
	else
		return DescribeLiveUserDomainsOutcome(outcome.error());
}

void LiveClient::describeLiveUserDomainsAsync(const DescribeLiveUserDomainsRequest& request, const DescribeLiveUserDomainsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveUserDomains(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveUserDomainsOutcomeCallable LiveClient::describeLiveUserDomainsCallable(const DescribeLiveUserDomainsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveUserDomainsOutcome()>>(
			[this, request]()
			{
			return this->describeLiveUserDomains(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeLiveUserTagsOutcome LiveClient::describeLiveUserTags(const DescribeLiveUserTagsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeLiveUserTagsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeLiveUserTagsOutcome(DescribeLiveUserTagsResult(outcome.result()));
	else
		return DescribeLiveUserTagsOutcome(outcome.error());
}

void LiveClient::describeLiveUserTagsAsync(const DescribeLiveUserTagsRequest& request, const DescribeLiveUserTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeLiveUserTags(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeLiveUserTagsOutcomeCallable LiveClient::describeLiveUserTagsCallable(const DescribeLiveUserTagsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeLiveUserTagsOutcome()>>(
			[this, request]()
			{
			return this->describeLiveUserTags(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeMixStreamListOutcome LiveClient::describeMixStreamList(const DescribeMixStreamListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeMixStreamListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeMixStreamListOutcome(DescribeMixStreamListResult(outcome.result()));
	else
		return DescribeMixStreamListOutcome(outcome.error());
}

void LiveClient::describeMixStreamListAsync(const DescribeMixStreamListRequest& request, const DescribeMixStreamListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeMixStreamList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeMixStreamListOutcomeCallable LiveClient::describeMixStreamListCallable(const DescribeMixStreamListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeMixStreamListOutcome()>>(
			[this, request]()
			{
			return this->describeMixStreamList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeRecordOutcome LiveClient::describeRecord(const DescribeRecordRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeRecordOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeRecordOutcome(DescribeRecordResult(outcome.result()));
	else
		return DescribeRecordOutcome(outcome.error());
}

void LiveClient::describeRecordAsync(const DescribeRecordRequest& request, const DescribeRecordAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeRecord(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeRecordOutcomeCallable LiveClient::describeRecordCallable(const DescribeRecordRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeRecordOutcome()>>(
			[this, request]()
			{
			return this->describeRecord(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeRecordsOutcome LiveClient::describeRecords(const DescribeRecordsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeRecordsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeRecordsOutcome(DescribeRecordsResult(outcome.result()));
	else
		return DescribeRecordsOutcome(outcome.error());
}

void LiveClient::describeRecordsAsync(const DescribeRecordsRequest& request, const DescribeRecordsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeRecords(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeRecordsOutcomeCallable LiveClient::describeRecordsCallable(const DescribeRecordsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeRecordsOutcome()>>(
			[this, request]()
			{
			return this->describeRecords(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeRoomKickoutUserListOutcome LiveClient::describeRoomKickoutUserList(const DescribeRoomKickoutUserListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeRoomKickoutUserListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeRoomKickoutUserListOutcome(DescribeRoomKickoutUserListResult(outcome.result()));
	else
		return DescribeRoomKickoutUserListOutcome(outcome.error());
}

void LiveClient::describeRoomKickoutUserListAsync(const DescribeRoomKickoutUserListRequest& request, const DescribeRoomKickoutUserListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeRoomKickoutUserList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeRoomKickoutUserListOutcomeCallable LiveClient::describeRoomKickoutUserListCallable(const DescribeRoomKickoutUserListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeRoomKickoutUserListOutcome()>>(
			[this, request]()
			{
			return this->describeRoomKickoutUserList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeRoomListOutcome LiveClient::describeRoomList(const DescribeRoomListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeRoomListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeRoomListOutcome(DescribeRoomListResult(outcome.result()));
	else
		return DescribeRoomListOutcome(outcome.error());
}

void LiveClient::describeRoomListAsync(const DescribeRoomListRequest& request, const DescribeRoomListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeRoomList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeRoomListOutcomeCallable LiveClient::describeRoomListCallable(const DescribeRoomListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeRoomListOutcome()>>(
			[this, request]()
			{
			return this->describeRoomList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeRoomStatusOutcome LiveClient::describeRoomStatus(const DescribeRoomStatusRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeRoomStatusOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeRoomStatusOutcome(DescribeRoomStatusResult(outcome.result()));
	else
		return DescribeRoomStatusOutcome(outcome.error());
}

void LiveClient::describeRoomStatusAsync(const DescribeRoomStatusRequest& request, const DescribeRoomStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeRoomStatus(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeRoomStatusOutcomeCallable LiveClient::describeRoomStatusCallable(const DescribeRoomStatusRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeRoomStatusOutcome()>>(
			[this, request]()
			{
			return this->describeRoomStatus(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeUpBpsPeakDataOutcome LiveClient::describeUpBpsPeakData(const DescribeUpBpsPeakDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeUpBpsPeakDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeUpBpsPeakDataOutcome(DescribeUpBpsPeakDataResult(outcome.result()));
	else
		return DescribeUpBpsPeakDataOutcome(outcome.error());
}

void LiveClient::describeUpBpsPeakDataAsync(const DescribeUpBpsPeakDataRequest& request, const DescribeUpBpsPeakDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeUpBpsPeakData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeUpBpsPeakDataOutcomeCallable LiveClient::describeUpBpsPeakDataCallable(const DescribeUpBpsPeakDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeUpBpsPeakDataOutcome()>>(
			[this, request]()
			{
			return this->describeUpBpsPeakData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeUpBpsPeakOfLineOutcome LiveClient::describeUpBpsPeakOfLine(const DescribeUpBpsPeakOfLineRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeUpBpsPeakOfLineOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeUpBpsPeakOfLineOutcome(DescribeUpBpsPeakOfLineResult(outcome.result()));
	else
		return DescribeUpBpsPeakOfLineOutcome(outcome.error());
}

void LiveClient::describeUpBpsPeakOfLineAsync(const DescribeUpBpsPeakOfLineRequest& request, const DescribeUpBpsPeakOfLineAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeUpBpsPeakOfLine(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeUpBpsPeakOfLineOutcomeCallable LiveClient::describeUpBpsPeakOfLineCallable(const DescribeUpBpsPeakOfLineRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeUpBpsPeakOfLineOutcome()>>(
			[this, request]()
			{
			return this->describeUpBpsPeakOfLine(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DescribeUpPeakPublishStreamDataOutcome LiveClient::describeUpPeakPublishStreamData(const DescribeUpPeakPublishStreamDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeUpPeakPublishStreamDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeUpPeakPublishStreamDataOutcome(DescribeUpPeakPublishStreamDataResult(outcome.result()));
	else
		return DescribeUpPeakPublishStreamDataOutcome(outcome.error());
}

void LiveClient::describeUpPeakPublishStreamDataAsync(const DescribeUpPeakPublishStreamDataRequest& request, const DescribeUpPeakPublishStreamDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeUpPeakPublishStreamData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DescribeUpPeakPublishStreamDataOutcomeCallable LiveClient::describeUpPeakPublishStreamDataCallable(const DescribeUpPeakPublishStreamDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeUpPeakPublishStreamDataOutcome()>>(
			[this, request]()
			{
			return this->describeUpPeakPublishStreamData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::DisableLiveRealtimeLogDeliveryOutcome LiveClient::disableLiveRealtimeLogDelivery(const DisableLiveRealtimeLogDeliveryRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DisableLiveRealtimeLogDeliveryOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DisableLiveRealtimeLogDeliveryOutcome(DisableLiveRealtimeLogDeliveryResult(outcome.result()));
	else
		return DisableLiveRealtimeLogDeliveryOutcome(outcome.error());
}

void LiveClient::disableLiveRealtimeLogDeliveryAsync(const DisableLiveRealtimeLogDeliveryRequest& request, const DisableLiveRealtimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, disableLiveRealtimeLogDelivery(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::DisableLiveRealtimeLogDeliveryOutcomeCallable LiveClient::disableLiveRealtimeLogDeliveryCallable(const DisableLiveRealtimeLogDeliveryRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DisableLiveRealtimeLogDeliveryOutcome()>>(
			[this, request]()
			{
			return this->disableLiveRealtimeLogDelivery(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::EditHtmlResourceOutcome LiveClient::editHtmlResource(const EditHtmlResourceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return EditHtmlResourceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return EditHtmlResourceOutcome(EditHtmlResourceResult(outcome.result()));
	else
		return EditHtmlResourceOutcome(outcome.error());
}

void LiveClient::editHtmlResourceAsync(const EditHtmlResourceRequest& request, const EditHtmlResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, editHtmlResource(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::EditHtmlResourceOutcomeCallable LiveClient::editHtmlResourceCallable(const EditHtmlResourceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<EditHtmlResourceOutcome()>>(
			[this, request]()
			{
			return this->editHtmlResource(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::EffectCasterUrgentOutcome LiveClient::effectCasterUrgent(const EffectCasterUrgentRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return EffectCasterUrgentOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return EffectCasterUrgentOutcome(EffectCasterUrgentResult(outcome.result()));
	else
		return EffectCasterUrgentOutcome(outcome.error());
}

void LiveClient::effectCasterUrgentAsync(const EffectCasterUrgentRequest& request, const EffectCasterUrgentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, effectCasterUrgent(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::EffectCasterUrgentOutcomeCallable LiveClient::effectCasterUrgentCallable(const EffectCasterUrgentRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<EffectCasterUrgentOutcome()>>(
			[this, request]()
			{
			return this->effectCasterUrgent(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::EffectCasterVideoResourceOutcome LiveClient::effectCasterVideoResource(const EffectCasterVideoResourceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return EffectCasterVideoResourceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return EffectCasterVideoResourceOutcome(EffectCasterVideoResourceResult(outcome.result()));
	else
		return EffectCasterVideoResourceOutcome(outcome.error());
}

void LiveClient::effectCasterVideoResourceAsync(const EffectCasterVideoResourceRequest& request, const EffectCasterVideoResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, effectCasterVideoResource(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::EffectCasterVideoResourceOutcomeCallable LiveClient::effectCasterVideoResourceCallable(const EffectCasterVideoResourceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<EffectCasterVideoResourceOutcome()>>(
			[this, request]()
			{
			return this->effectCasterVideoResource(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::EnableLiveRealtimeLogDeliveryOutcome LiveClient::enableLiveRealtimeLogDelivery(const EnableLiveRealtimeLogDeliveryRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return EnableLiveRealtimeLogDeliveryOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return EnableLiveRealtimeLogDeliveryOutcome(EnableLiveRealtimeLogDeliveryResult(outcome.result()));
	else
		return EnableLiveRealtimeLogDeliveryOutcome(outcome.error());
}

void LiveClient::enableLiveRealtimeLogDeliveryAsync(const EnableLiveRealtimeLogDeliveryRequest& request, const EnableLiveRealtimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, enableLiveRealtimeLogDelivery(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::EnableLiveRealtimeLogDeliveryOutcomeCallable LiveClient::enableLiveRealtimeLogDeliveryCallable(const EnableLiveRealtimeLogDeliveryRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<EnableLiveRealtimeLogDeliveryOutcome()>>(
			[this, request]()
			{
			return this->enableLiveRealtimeLogDelivery(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ForbidLiveStreamOutcome LiveClient::forbidLiveStream(const ForbidLiveStreamRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ForbidLiveStreamOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ForbidLiveStreamOutcome(ForbidLiveStreamResult(outcome.result()));
	else
		return ForbidLiveStreamOutcome(outcome.error());
}

void LiveClient::forbidLiveStreamAsync(const ForbidLiveStreamRequest& request, const ForbidLiveStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, forbidLiveStream(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ForbidLiveStreamOutcomeCallable LiveClient::forbidLiveStreamCallable(const ForbidLiveStreamRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ForbidLiveStreamOutcome()>>(
			[this, request]()
			{
			return this->forbidLiveStream(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ForbidPushStreamOutcome LiveClient::forbidPushStream(const ForbidPushStreamRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ForbidPushStreamOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ForbidPushStreamOutcome(ForbidPushStreamResult(outcome.result()));
	else
		return ForbidPushStreamOutcome(outcome.error());
}

void LiveClient::forbidPushStreamAsync(const ForbidPushStreamRequest& request, const ForbidPushStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, forbidPushStream(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ForbidPushStreamOutcomeCallable LiveClient::forbidPushStreamCallable(const ForbidPushStreamRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ForbidPushStreamOutcome()>>(
			[this, request]()
			{
			return this->forbidPushStream(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::JoinBoardOutcome LiveClient::joinBoard(const JoinBoardRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return JoinBoardOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return JoinBoardOutcome(JoinBoardResult(outcome.result()));
	else
		return JoinBoardOutcome(outcome.error());
}

void LiveClient::joinBoardAsync(const JoinBoardRequest& request, const JoinBoardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, joinBoard(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::JoinBoardOutcomeCallable LiveClient::joinBoardCallable(const JoinBoardRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<JoinBoardOutcome()>>(
			[this, request]()
			{
			return this->joinBoard(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ListLiveRealtimeLogDeliveryOutcome LiveClient::listLiveRealtimeLogDelivery(const ListLiveRealtimeLogDeliveryRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListLiveRealtimeLogDeliveryOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListLiveRealtimeLogDeliveryOutcome(ListLiveRealtimeLogDeliveryResult(outcome.result()));
	else
		return ListLiveRealtimeLogDeliveryOutcome(outcome.error());
}

void LiveClient::listLiveRealtimeLogDeliveryAsync(const ListLiveRealtimeLogDeliveryRequest& request, const ListLiveRealtimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listLiveRealtimeLogDelivery(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ListLiveRealtimeLogDeliveryOutcomeCallable LiveClient::listLiveRealtimeLogDeliveryCallable(const ListLiveRealtimeLogDeliveryRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListLiveRealtimeLogDeliveryOutcome()>>(
			[this, request]()
			{
			return this->listLiveRealtimeLogDelivery(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ListLiveRealtimeLogDeliveryDomainsOutcome LiveClient::listLiveRealtimeLogDeliveryDomains(const ListLiveRealtimeLogDeliveryDomainsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListLiveRealtimeLogDeliveryDomainsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListLiveRealtimeLogDeliveryDomainsOutcome(ListLiveRealtimeLogDeliveryDomainsResult(outcome.result()));
	else
		return ListLiveRealtimeLogDeliveryDomainsOutcome(outcome.error());
}

void LiveClient::listLiveRealtimeLogDeliveryDomainsAsync(const ListLiveRealtimeLogDeliveryDomainsRequest& request, const ListLiveRealtimeLogDeliveryDomainsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listLiveRealtimeLogDeliveryDomains(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ListLiveRealtimeLogDeliveryDomainsOutcomeCallable LiveClient::listLiveRealtimeLogDeliveryDomainsCallable(const ListLiveRealtimeLogDeliveryDomainsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListLiveRealtimeLogDeliveryDomainsOutcome()>>(
			[this, request]()
			{
			return this->listLiveRealtimeLogDeliveryDomains(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ListLiveRealtimeLogDeliveryInfosOutcome LiveClient::listLiveRealtimeLogDeliveryInfos(const ListLiveRealtimeLogDeliveryInfosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListLiveRealtimeLogDeliveryInfosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListLiveRealtimeLogDeliveryInfosOutcome(ListLiveRealtimeLogDeliveryInfosResult(outcome.result()));
	else
		return ListLiveRealtimeLogDeliveryInfosOutcome(outcome.error());
}

void LiveClient::listLiveRealtimeLogDeliveryInfosAsync(const ListLiveRealtimeLogDeliveryInfosRequest& request, const ListLiveRealtimeLogDeliveryInfosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listLiveRealtimeLogDeliveryInfos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ListLiveRealtimeLogDeliveryInfosOutcomeCallable LiveClient::listLiveRealtimeLogDeliveryInfosCallable(const ListLiveRealtimeLogDeliveryInfosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListLiveRealtimeLogDeliveryInfosOutcome()>>(
			[this, request]()
			{
			return this->listLiveRealtimeLogDeliveryInfos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ModifyCasterComponentOutcome LiveClient::modifyCasterComponent(const ModifyCasterComponentRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyCasterComponentOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyCasterComponentOutcome(ModifyCasterComponentResult(outcome.result()));
	else
		return ModifyCasterComponentOutcome(outcome.error());
}

void LiveClient::modifyCasterComponentAsync(const ModifyCasterComponentRequest& request, const ModifyCasterComponentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyCasterComponent(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ModifyCasterComponentOutcomeCallable LiveClient::modifyCasterComponentCallable(const ModifyCasterComponentRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyCasterComponentOutcome()>>(
			[this, request]()
			{
			return this->modifyCasterComponent(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ModifyCasterEpisodeOutcome LiveClient::modifyCasterEpisode(const ModifyCasterEpisodeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyCasterEpisodeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyCasterEpisodeOutcome(ModifyCasterEpisodeResult(outcome.result()));
	else
		return ModifyCasterEpisodeOutcome(outcome.error());
}

void LiveClient::modifyCasterEpisodeAsync(const ModifyCasterEpisodeRequest& request, const ModifyCasterEpisodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyCasterEpisode(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ModifyCasterEpisodeOutcomeCallable LiveClient::modifyCasterEpisodeCallable(const ModifyCasterEpisodeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyCasterEpisodeOutcome()>>(
			[this, request]()
			{
			return this->modifyCasterEpisode(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ModifyCasterLayoutOutcome LiveClient::modifyCasterLayout(const ModifyCasterLayoutRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyCasterLayoutOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyCasterLayoutOutcome(ModifyCasterLayoutResult(outcome.result()));
	else
		return ModifyCasterLayoutOutcome(outcome.error());
}

void LiveClient::modifyCasterLayoutAsync(const ModifyCasterLayoutRequest& request, const ModifyCasterLayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyCasterLayout(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ModifyCasterLayoutOutcomeCallable LiveClient::modifyCasterLayoutCallable(const ModifyCasterLayoutRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyCasterLayoutOutcome()>>(
			[this, request]()
			{
			return this->modifyCasterLayout(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ModifyCasterProgramOutcome LiveClient::modifyCasterProgram(const ModifyCasterProgramRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyCasterProgramOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyCasterProgramOutcome(ModifyCasterProgramResult(outcome.result()));
	else
		return ModifyCasterProgramOutcome(outcome.error());
}

void LiveClient::modifyCasterProgramAsync(const ModifyCasterProgramRequest& request, const ModifyCasterProgramAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyCasterProgram(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ModifyCasterProgramOutcomeCallable LiveClient::modifyCasterProgramCallable(const ModifyCasterProgramRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyCasterProgramOutcome()>>(
			[this, request]()
			{
			return this->modifyCasterProgram(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ModifyCasterVideoResourceOutcome LiveClient::modifyCasterVideoResource(const ModifyCasterVideoResourceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyCasterVideoResourceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyCasterVideoResourceOutcome(ModifyCasterVideoResourceResult(outcome.result()));
	else
		return ModifyCasterVideoResourceOutcome(outcome.error());
}

void LiveClient::modifyCasterVideoResourceAsync(const ModifyCasterVideoResourceRequest& request, const ModifyCasterVideoResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyCasterVideoResource(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ModifyCasterVideoResourceOutcomeCallable LiveClient::modifyCasterVideoResourceCallable(const ModifyCasterVideoResourceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyCasterVideoResourceOutcome()>>(
			[this, request]()
			{
			return this->modifyCasterVideoResource(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ModifyLiveDomainSchdmByPropertyOutcome LiveClient::modifyLiveDomainSchdmByProperty(const ModifyLiveDomainSchdmByPropertyRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyLiveDomainSchdmByPropertyOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyLiveDomainSchdmByPropertyOutcome(ModifyLiveDomainSchdmByPropertyResult(outcome.result()));
	else
		return ModifyLiveDomainSchdmByPropertyOutcome(outcome.error());
}

void LiveClient::modifyLiveDomainSchdmByPropertyAsync(const ModifyLiveDomainSchdmByPropertyRequest& request, const ModifyLiveDomainSchdmByPropertyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyLiveDomainSchdmByProperty(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ModifyLiveDomainSchdmByPropertyOutcomeCallable LiveClient::modifyLiveDomainSchdmByPropertyCallable(const ModifyLiveDomainSchdmByPropertyRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyLiveDomainSchdmByPropertyOutcome()>>(
			[this, request]()
			{
			return this->modifyLiveDomainSchdmByProperty(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ModifyLiveRealtimeLogDeliveryOutcome LiveClient::modifyLiveRealtimeLogDelivery(const ModifyLiveRealtimeLogDeliveryRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyLiveRealtimeLogDeliveryOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyLiveRealtimeLogDeliveryOutcome(ModifyLiveRealtimeLogDeliveryResult(outcome.result()));
	else
		return ModifyLiveRealtimeLogDeliveryOutcome(outcome.error());
}

void LiveClient::modifyLiveRealtimeLogDeliveryAsync(const ModifyLiveRealtimeLogDeliveryRequest& request, const ModifyLiveRealtimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyLiveRealtimeLogDelivery(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ModifyLiveRealtimeLogDeliveryOutcomeCallable LiveClient::modifyLiveRealtimeLogDeliveryCallable(const ModifyLiveRealtimeLogDeliveryRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyLiveRealtimeLogDeliveryOutcome()>>(
			[this, request]()
			{
			return this->modifyLiveRealtimeLogDelivery(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::RealTimeRecordCommandOutcome LiveClient::realTimeRecordCommand(const RealTimeRecordCommandRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RealTimeRecordCommandOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RealTimeRecordCommandOutcome(RealTimeRecordCommandResult(outcome.result()));
	else
		return RealTimeRecordCommandOutcome(outcome.error());
}

void LiveClient::realTimeRecordCommandAsync(const RealTimeRecordCommandRequest& request, const RealTimeRecordCommandAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, realTimeRecordCommand(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::RealTimeRecordCommandOutcomeCallable LiveClient::realTimeRecordCommandCallable(const RealTimeRecordCommandRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RealTimeRecordCommandOutcome()>>(
			[this, request]()
			{
			return this->realTimeRecordCommand(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::RealTimeSnapshotCommandOutcome LiveClient::realTimeSnapshotCommand(const RealTimeSnapshotCommandRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RealTimeSnapshotCommandOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RealTimeSnapshotCommandOutcome(RealTimeSnapshotCommandResult(outcome.result()));
	else
		return RealTimeSnapshotCommandOutcome(outcome.error());
}

void LiveClient::realTimeSnapshotCommandAsync(const RealTimeSnapshotCommandRequest& request, const RealTimeSnapshotCommandAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, realTimeSnapshotCommand(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::RealTimeSnapshotCommandOutcomeCallable LiveClient::realTimeSnapshotCommandCallable(const RealTimeSnapshotCommandRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RealTimeSnapshotCommandOutcome()>>(
			[this, request]()
			{
			return this->realTimeSnapshotCommand(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::ResumeLiveStreamOutcome LiveClient::resumeLiveStream(const ResumeLiveStreamRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ResumeLiveStreamOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ResumeLiveStreamOutcome(ResumeLiveStreamResult(outcome.result()));
	else
		return ResumeLiveStreamOutcome(outcome.error());
}

void LiveClient::resumeLiveStreamAsync(const ResumeLiveStreamRequest& request, const ResumeLiveStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, resumeLiveStream(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::ResumeLiveStreamOutcomeCallable LiveClient::resumeLiveStreamCallable(const ResumeLiveStreamRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ResumeLiveStreamOutcome()>>(
			[this, request]()
			{
			return this->resumeLiveStream(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::SendRoomNotificationOutcome LiveClient::sendRoomNotification(const SendRoomNotificationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SendRoomNotificationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SendRoomNotificationOutcome(SendRoomNotificationResult(outcome.result()));
	else
		return SendRoomNotificationOutcome(outcome.error());
}

void LiveClient::sendRoomNotificationAsync(const SendRoomNotificationRequest& request, const SendRoomNotificationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, sendRoomNotification(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::SendRoomNotificationOutcomeCallable LiveClient::sendRoomNotificationCallable(const SendRoomNotificationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SendRoomNotificationOutcome()>>(
			[this, request]()
			{
			return this->sendRoomNotification(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::SendRoomUserNotificationOutcome LiveClient::sendRoomUserNotification(const SendRoomUserNotificationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SendRoomUserNotificationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SendRoomUserNotificationOutcome(SendRoomUserNotificationResult(outcome.result()));
	else
		return SendRoomUserNotificationOutcome(outcome.error());
}

void LiveClient::sendRoomUserNotificationAsync(const SendRoomUserNotificationRequest& request, const SendRoomUserNotificationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, sendRoomUserNotification(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::SendRoomUserNotificationOutcomeCallable LiveClient::sendRoomUserNotificationCallable(const SendRoomUserNotificationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SendRoomUserNotificationOutcome()>>(
			[this, request]()
			{
			return this->sendRoomUserNotification(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::SetBoardCallbackOutcome LiveClient::setBoardCallback(const SetBoardCallbackRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetBoardCallbackOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetBoardCallbackOutcome(SetBoardCallbackResult(outcome.result()));
	else
		return SetBoardCallbackOutcome(outcome.error());
}

void LiveClient::setBoardCallbackAsync(const SetBoardCallbackRequest& request, const SetBoardCallbackAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setBoardCallback(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::SetBoardCallbackOutcomeCallable LiveClient::setBoardCallbackCallable(const SetBoardCallbackRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetBoardCallbackOutcome()>>(
			[this, request]()
			{
			return this->setBoardCallback(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::SetCasterChannelOutcome LiveClient::setCasterChannel(const SetCasterChannelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetCasterChannelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetCasterChannelOutcome(SetCasterChannelResult(outcome.result()));
	else
		return SetCasterChannelOutcome(outcome.error());
}

void LiveClient::setCasterChannelAsync(const SetCasterChannelRequest& request, const SetCasterChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setCasterChannel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::SetCasterChannelOutcomeCallable LiveClient::setCasterChannelCallable(const SetCasterChannelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetCasterChannelOutcome()>>(
			[this, request]()
			{
			return this->setCasterChannel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::SetCasterConfigOutcome LiveClient::setCasterConfig(const SetCasterConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetCasterConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetCasterConfigOutcome(SetCasterConfigResult(outcome.result()));
	else
		return SetCasterConfigOutcome(outcome.error());
}

void LiveClient::setCasterConfigAsync(const SetCasterConfigRequest& request, const SetCasterConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setCasterConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::SetCasterConfigOutcomeCallable LiveClient::setCasterConfigCallable(const SetCasterConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetCasterConfigOutcome()>>(
			[this, request]()
			{
			return this->setCasterConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::SetCasterSceneConfigOutcome LiveClient::setCasterSceneConfig(const SetCasterSceneConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetCasterSceneConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetCasterSceneConfigOutcome(SetCasterSceneConfigResult(outcome.result()));
	else
		return SetCasterSceneConfigOutcome(outcome.error());
}

void LiveClient::setCasterSceneConfigAsync(const SetCasterSceneConfigRequest& request, const SetCasterSceneConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setCasterSceneConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::SetCasterSceneConfigOutcomeCallable LiveClient::setCasterSceneConfigCallable(const SetCasterSceneConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetCasterSceneConfigOutcome()>>(
			[this, request]()
			{
			return this->setCasterSceneConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::SetCasterSyncGroupOutcome LiveClient::setCasterSyncGroup(const SetCasterSyncGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetCasterSyncGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetCasterSyncGroupOutcome(SetCasterSyncGroupResult(outcome.result()));
	else
		return SetCasterSyncGroupOutcome(outcome.error());
}

void LiveClient::setCasterSyncGroupAsync(const SetCasterSyncGroupRequest& request, const SetCasterSyncGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setCasterSyncGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::SetCasterSyncGroupOutcomeCallable LiveClient::setCasterSyncGroupCallable(const SetCasterSyncGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetCasterSyncGroupOutcome()>>(
			[this, request]()
			{
			return this->setCasterSyncGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::SetLiveDomainCertificateOutcome LiveClient::setLiveDomainCertificate(const SetLiveDomainCertificateRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetLiveDomainCertificateOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetLiveDomainCertificateOutcome(SetLiveDomainCertificateResult(outcome.result()));
	else
		return SetLiveDomainCertificateOutcome(outcome.error());
}

void LiveClient::setLiveDomainCertificateAsync(const SetLiveDomainCertificateRequest& request, const SetLiveDomainCertificateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setLiveDomainCertificate(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::SetLiveDomainCertificateOutcomeCallable LiveClient::setLiveDomainCertificateCallable(const SetLiveDomainCertificateRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetLiveDomainCertificateOutcome()>>(
			[this, request]()
			{
			return this->setLiveDomainCertificate(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::SetLiveLazyPullStreamInfoConfigOutcome LiveClient::setLiveLazyPullStreamInfoConfig(const SetLiveLazyPullStreamInfoConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetLiveLazyPullStreamInfoConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetLiveLazyPullStreamInfoConfigOutcome(SetLiveLazyPullStreamInfoConfigResult(outcome.result()));
	else
		return SetLiveLazyPullStreamInfoConfigOutcome(outcome.error());
}

void LiveClient::setLiveLazyPullStreamInfoConfigAsync(const SetLiveLazyPullStreamInfoConfigRequest& request, const SetLiveLazyPullStreamInfoConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setLiveLazyPullStreamInfoConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::SetLiveLazyPullStreamInfoConfigOutcomeCallable LiveClient::setLiveLazyPullStreamInfoConfigCallable(const SetLiveLazyPullStreamInfoConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetLiveLazyPullStreamInfoConfigOutcome()>>(
			[this, request]()
			{
			return this->setLiveLazyPullStreamInfoConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::SetLiveStreamDelayConfigOutcome LiveClient::setLiveStreamDelayConfig(const SetLiveStreamDelayConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetLiveStreamDelayConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetLiveStreamDelayConfigOutcome(SetLiveStreamDelayConfigResult(outcome.result()));
	else
		return SetLiveStreamDelayConfigOutcome(outcome.error());
}

void LiveClient::setLiveStreamDelayConfigAsync(const SetLiveStreamDelayConfigRequest& request, const SetLiveStreamDelayConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setLiveStreamDelayConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::SetLiveStreamDelayConfigOutcomeCallable LiveClient::setLiveStreamDelayConfigCallable(const SetLiveStreamDelayConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetLiveStreamDelayConfigOutcome()>>(
			[this, request]()
			{
			return this->setLiveStreamDelayConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::SetLiveStreamOptimizedFeatureConfigOutcome LiveClient::setLiveStreamOptimizedFeatureConfig(const SetLiveStreamOptimizedFeatureConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetLiveStreamOptimizedFeatureConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetLiveStreamOptimizedFeatureConfigOutcome(SetLiveStreamOptimizedFeatureConfigResult(outcome.result()));
	else
		return SetLiveStreamOptimizedFeatureConfigOutcome(outcome.error());
}

void LiveClient::setLiveStreamOptimizedFeatureConfigAsync(const SetLiveStreamOptimizedFeatureConfigRequest& request, const SetLiveStreamOptimizedFeatureConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setLiveStreamOptimizedFeatureConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::SetLiveStreamOptimizedFeatureConfigOutcomeCallable LiveClient::setLiveStreamOptimizedFeatureConfigCallable(const SetLiveStreamOptimizedFeatureConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetLiveStreamOptimizedFeatureConfigOutcome()>>(
			[this, request]()
			{
			return this->setLiveStreamOptimizedFeatureConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::SetLiveStreamsNotifyUrlConfigOutcome LiveClient::setLiveStreamsNotifyUrlConfig(const SetLiveStreamsNotifyUrlConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetLiveStreamsNotifyUrlConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetLiveStreamsNotifyUrlConfigOutcome(SetLiveStreamsNotifyUrlConfigResult(outcome.result()));
	else
		return SetLiveStreamsNotifyUrlConfigOutcome(outcome.error());
}

void LiveClient::setLiveStreamsNotifyUrlConfigAsync(const SetLiveStreamsNotifyUrlConfigRequest& request, const SetLiveStreamsNotifyUrlConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setLiveStreamsNotifyUrlConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::SetLiveStreamsNotifyUrlConfigOutcomeCallable LiveClient::setLiveStreamsNotifyUrlConfigCallable(const SetLiveStreamsNotifyUrlConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetLiveStreamsNotifyUrlConfigOutcome()>>(
			[this, request]()
			{
			return this->setLiveStreamsNotifyUrlConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::StartBoardRecordOutcome LiveClient::startBoardRecord(const StartBoardRecordRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartBoardRecordOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartBoardRecordOutcome(StartBoardRecordResult(outcome.result()));
	else
		return StartBoardRecordOutcome(outcome.error());
}

void LiveClient::startBoardRecordAsync(const StartBoardRecordRequest& request, const StartBoardRecordAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startBoardRecord(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::StartBoardRecordOutcomeCallable LiveClient::startBoardRecordCallable(const StartBoardRecordRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartBoardRecordOutcome()>>(
			[this, request]()
			{
			return this->startBoardRecord(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::StartCasterOutcome LiveClient::startCaster(const StartCasterRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartCasterOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartCasterOutcome(StartCasterResult(outcome.result()));
	else
		return StartCasterOutcome(outcome.error());
}

void LiveClient::startCasterAsync(const StartCasterRequest& request, const StartCasterAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startCaster(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::StartCasterOutcomeCallable LiveClient::startCasterCallable(const StartCasterRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartCasterOutcome()>>(
			[this, request]()
			{
			return this->startCaster(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::StartCasterSceneOutcome LiveClient::startCasterScene(const StartCasterSceneRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartCasterSceneOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartCasterSceneOutcome(StartCasterSceneResult(outcome.result()));
	else
		return StartCasterSceneOutcome(outcome.error());
}

void LiveClient::startCasterSceneAsync(const StartCasterSceneRequest& request, const StartCasterSceneAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startCasterScene(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::StartCasterSceneOutcomeCallable LiveClient::startCasterSceneCallable(const StartCasterSceneRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartCasterSceneOutcome()>>(
			[this, request]()
			{
			return this->startCasterScene(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::StartLiveDomainOutcome LiveClient::startLiveDomain(const StartLiveDomainRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartLiveDomainOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartLiveDomainOutcome(StartLiveDomainResult(outcome.result()));
	else
		return StartLiveDomainOutcome(outcome.error());
}

void LiveClient::startLiveDomainAsync(const StartLiveDomainRequest& request, const StartLiveDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startLiveDomain(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::StartLiveDomainOutcomeCallable LiveClient::startLiveDomainCallable(const StartLiveDomainRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartLiveDomainOutcome()>>(
			[this, request]()
			{
			return this->startLiveDomain(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::StartLiveIndexOutcome LiveClient::startLiveIndex(const StartLiveIndexRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartLiveIndexOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartLiveIndexOutcome(StartLiveIndexResult(outcome.result()));
	else
		return StartLiveIndexOutcome(outcome.error());
}

void LiveClient::startLiveIndexAsync(const StartLiveIndexRequest& request, const StartLiveIndexAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startLiveIndex(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::StartLiveIndexOutcomeCallable LiveClient::startLiveIndexCallable(const StartLiveIndexRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartLiveIndexOutcome()>>(
			[this, request]()
			{
			return this->startLiveIndex(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::StopCasterOutcome LiveClient::stopCaster(const StopCasterRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StopCasterOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StopCasterOutcome(StopCasterResult(outcome.result()));
	else
		return StopCasterOutcome(outcome.error());
}

void LiveClient::stopCasterAsync(const StopCasterRequest& request, const StopCasterAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, stopCaster(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::StopCasterOutcomeCallable LiveClient::stopCasterCallable(const StopCasterRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StopCasterOutcome()>>(
			[this, request]()
			{
			return this->stopCaster(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::StopCasterSceneOutcome LiveClient::stopCasterScene(const StopCasterSceneRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StopCasterSceneOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StopCasterSceneOutcome(StopCasterSceneResult(outcome.result()));
	else
		return StopCasterSceneOutcome(outcome.error());
}

void LiveClient::stopCasterSceneAsync(const StopCasterSceneRequest& request, const StopCasterSceneAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, stopCasterScene(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::StopCasterSceneOutcomeCallable LiveClient::stopCasterSceneCallable(const StopCasterSceneRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StopCasterSceneOutcome()>>(
			[this, request]()
			{
			return this->stopCasterScene(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::StopLiveDomainOutcome LiveClient::stopLiveDomain(const StopLiveDomainRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StopLiveDomainOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StopLiveDomainOutcome(StopLiveDomainResult(outcome.result()));
	else
		return StopLiveDomainOutcome(outcome.error());
}

void LiveClient::stopLiveDomainAsync(const StopLiveDomainRequest& request, const StopLiveDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, stopLiveDomain(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::StopLiveDomainOutcomeCallable LiveClient::stopLiveDomainCallable(const StopLiveDomainRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StopLiveDomainOutcome()>>(
			[this, request]()
			{
			return this->stopLiveDomain(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::StopLiveIndexOutcome LiveClient::stopLiveIndex(const StopLiveIndexRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StopLiveIndexOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StopLiveIndexOutcome(StopLiveIndexResult(outcome.result()));
	else
		return StopLiveIndexOutcome(outcome.error());
}

void LiveClient::stopLiveIndexAsync(const StopLiveIndexRequest& request, const StopLiveIndexAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, stopLiveIndex(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::StopLiveIndexOutcomeCallable LiveClient::stopLiveIndexCallable(const StopLiveIndexRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StopLiveIndexOutcome()>>(
			[this, request]()
			{
			return this->stopLiveIndex(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::TagLiveResourcesOutcome LiveClient::tagLiveResources(const TagLiveResourcesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return TagLiveResourcesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return TagLiveResourcesOutcome(TagLiveResourcesResult(outcome.result()));
	else
		return TagLiveResourcesOutcome(outcome.error());
}

void LiveClient::tagLiveResourcesAsync(const TagLiveResourcesRequest& request, const TagLiveResourcesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, tagLiveResources(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::TagLiveResourcesOutcomeCallable LiveClient::tagLiveResourcesCallable(const TagLiveResourcesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<TagLiveResourcesOutcome()>>(
			[this, request]()
			{
			return this->tagLiveResources(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UnTagLiveResourcesOutcome LiveClient::unTagLiveResources(const UnTagLiveResourcesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UnTagLiveResourcesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UnTagLiveResourcesOutcome(UnTagLiveResourcesResult(outcome.result()));
	else
		return UnTagLiveResourcesOutcome(outcome.error());
}

void LiveClient::unTagLiveResourcesAsync(const UnTagLiveResourcesRequest& request, const UnTagLiveResourcesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, unTagLiveResources(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UnTagLiveResourcesOutcomeCallable LiveClient::unTagLiveResourcesCallable(const UnTagLiveResourcesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UnTagLiveResourcesOutcome()>>(
			[this, request]()
			{
			return this->unTagLiveResources(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UpdateBoardOutcome LiveClient::updateBoard(const UpdateBoardRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateBoardOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateBoardOutcome(UpdateBoardResult(outcome.result()));
	else
		return UpdateBoardOutcome(outcome.error());
}

void LiveClient::updateBoardAsync(const UpdateBoardRequest& request, const UpdateBoardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateBoard(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UpdateBoardOutcomeCallable LiveClient::updateBoardCallable(const UpdateBoardRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateBoardOutcome()>>(
			[this, request]()
			{
			return this->updateBoard(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UpdateBoardCallbackOutcome LiveClient::updateBoardCallback(const UpdateBoardCallbackRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateBoardCallbackOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateBoardCallbackOutcome(UpdateBoardCallbackResult(outcome.result()));
	else
		return UpdateBoardCallbackOutcome(outcome.error());
}

void LiveClient::updateBoardCallbackAsync(const UpdateBoardCallbackRequest& request, const UpdateBoardCallbackAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateBoardCallback(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UpdateBoardCallbackOutcomeCallable LiveClient::updateBoardCallbackCallable(const UpdateBoardCallbackRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateBoardCallbackOutcome()>>(
			[this, request]()
			{
			return this->updateBoardCallback(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UpdateCasterSceneAudioOutcome LiveClient::updateCasterSceneAudio(const UpdateCasterSceneAudioRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateCasterSceneAudioOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateCasterSceneAudioOutcome(UpdateCasterSceneAudioResult(outcome.result()));
	else
		return UpdateCasterSceneAudioOutcome(outcome.error());
}

void LiveClient::updateCasterSceneAudioAsync(const UpdateCasterSceneAudioRequest& request, const UpdateCasterSceneAudioAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateCasterSceneAudio(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UpdateCasterSceneAudioOutcomeCallable LiveClient::updateCasterSceneAudioCallable(const UpdateCasterSceneAudioRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateCasterSceneAudioOutcome()>>(
			[this, request]()
			{
			return this->updateCasterSceneAudio(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UpdateCasterSceneConfigOutcome LiveClient::updateCasterSceneConfig(const UpdateCasterSceneConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateCasterSceneConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateCasterSceneConfigOutcome(UpdateCasterSceneConfigResult(outcome.result()));
	else
		return UpdateCasterSceneConfigOutcome(outcome.error());
}

void LiveClient::updateCasterSceneConfigAsync(const UpdateCasterSceneConfigRequest& request, const UpdateCasterSceneConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateCasterSceneConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UpdateCasterSceneConfigOutcomeCallable LiveClient::updateCasterSceneConfigCallable(const UpdateCasterSceneConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateCasterSceneConfigOutcome()>>(
			[this, request]()
			{
			return this->updateCasterSceneConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UpdateLiveASRConfigOutcome LiveClient::updateLiveASRConfig(const UpdateLiveASRConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateLiveASRConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateLiveASRConfigOutcome(UpdateLiveASRConfigResult(outcome.result()));
	else
		return UpdateLiveASRConfigOutcome(outcome.error());
}

void LiveClient::updateLiveASRConfigAsync(const UpdateLiveASRConfigRequest& request, const UpdateLiveASRConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateLiveASRConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UpdateLiveASRConfigOutcomeCallable LiveClient::updateLiveASRConfigCallable(const UpdateLiveASRConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateLiveASRConfigOutcome()>>(
			[this, request]()
			{
			return this->updateLiveASRConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UpdateLiveAppSnapshotConfigOutcome LiveClient::updateLiveAppSnapshotConfig(const UpdateLiveAppSnapshotConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateLiveAppSnapshotConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateLiveAppSnapshotConfigOutcome(UpdateLiveAppSnapshotConfigResult(outcome.result()));
	else
		return UpdateLiveAppSnapshotConfigOutcome(outcome.error());
}

void LiveClient::updateLiveAppSnapshotConfigAsync(const UpdateLiveAppSnapshotConfigRequest& request, const UpdateLiveAppSnapshotConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateLiveAppSnapshotConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UpdateLiveAppSnapshotConfigOutcomeCallable LiveClient::updateLiveAppSnapshotConfigCallable(const UpdateLiveAppSnapshotConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateLiveAppSnapshotConfigOutcome()>>(
			[this, request]()
			{
			return this->updateLiveAppSnapshotConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UpdateLiveAudioAuditConfigOutcome LiveClient::updateLiveAudioAuditConfig(const UpdateLiveAudioAuditConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateLiveAudioAuditConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateLiveAudioAuditConfigOutcome(UpdateLiveAudioAuditConfigResult(outcome.result()));
	else
		return UpdateLiveAudioAuditConfigOutcome(outcome.error());
}

void LiveClient::updateLiveAudioAuditConfigAsync(const UpdateLiveAudioAuditConfigRequest& request, const UpdateLiveAudioAuditConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateLiveAudioAuditConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UpdateLiveAudioAuditConfigOutcomeCallable LiveClient::updateLiveAudioAuditConfigCallable(const UpdateLiveAudioAuditConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateLiveAudioAuditConfigOutcome()>>(
			[this, request]()
			{
			return this->updateLiveAudioAuditConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UpdateLiveAudioAuditNotifyConfigOutcome LiveClient::updateLiveAudioAuditNotifyConfig(const UpdateLiveAudioAuditNotifyConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateLiveAudioAuditNotifyConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateLiveAudioAuditNotifyConfigOutcome(UpdateLiveAudioAuditNotifyConfigResult(outcome.result()));
	else
		return UpdateLiveAudioAuditNotifyConfigOutcome(outcome.error());
}

void LiveClient::updateLiveAudioAuditNotifyConfigAsync(const UpdateLiveAudioAuditNotifyConfigRequest& request, const UpdateLiveAudioAuditNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateLiveAudioAuditNotifyConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UpdateLiveAudioAuditNotifyConfigOutcomeCallable LiveClient::updateLiveAudioAuditNotifyConfigCallable(const UpdateLiveAudioAuditNotifyConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateLiveAudioAuditNotifyConfigOutcome()>>(
			[this, request]()
			{
			return this->updateLiveAudioAuditNotifyConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UpdateLiveDetectNotifyConfigOutcome LiveClient::updateLiveDetectNotifyConfig(const UpdateLiveDetectNotifyConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateLiveDetectNotifyConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateLiveDetectNotifyConfigOutcome(UpdateLiveDetectNotifyConfigResult(outcome.result()));
	else
		return UpdateLiveDetectNotifyConfigOutcome(outcome.error());
}

void LiveClient::updateLiveDetectNotifyConfigAsync(const UpdateLiveDetectNotifyConfigRequest& request, const UpdateLiveDetectNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateLiveDetectNotifyConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UpdateLiveDetectNotifyConfigOutcomeCallable LiveClient::updateLiveDetectNotifyConfigCallable(const UpdateLiveDetectNotifyConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateLiveDetectNotifyConfigOutcome()>>(
			[this, request]()
			{
			return this->updateLiveDetectNotifyConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UpdateLiveRecordNotifyConfigOutcome LiveClient::updateLiveRecordNotifyConfig(const UpdateLiveRecordNotifyConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateLiveRecordNotifyConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateLiveRecordNotifyConfigOutcome(UpdateLiveRecordNotifyConfigResult(outcome.result()));
	else
		return UpdateLiveRecordNotifyConfigOutcome(outcome.error());
}

void LiveClient::updateLiveRecordNotifyConfigAsync(const UpdateLiveRecordNotifyConfigRequest& request, const UpdateLiveRecordNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateLiveRecordNotifyConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UpdateLiveRecordNotifyConfigOutcomeCallable LiveClient::updateLiveRecordNotifyConfigCallable(const UpdateLiveRecordNotifyConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateLiveRecordNotifyConfigOutcome()>>(
			[this, request]()
			{
			return this->updateLiveRecordNotifyConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UpdateLiveSnapshotDetectPornConfigOutcome LiveClient::updateLiveSnapshotDetectPornConfig(const UpdateLiveSnapshotDetectPornConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateLiveSnapshotDetectPornConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateLiveSnapshotDetectPornConfigOutcome(UpdateLiveSnapshotDetectPornConfigResult(outcome.result()));
	else
		return UpdateLiveSnapshotDetectPornConfigOutcome(outcome.error());
}

void LiveClient::updateLiveSnapshotDetectPornConfigAsync(const UpdateLiveSnapshotDetectPornConfigRequest& request, const UpdateLiveSnapshotDetectPornConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateLiveSnapshotDetectPornConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UpdateLiveSnapshotDetectPornConfigOutcomeCallable LiveClient::updateLiveSnapshotDetectPornConfigCallable(const UpdateLiveSnapshotDetectPornConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateLiveSnapshotDetectPornConfigOutcome()>>(
			[this, request]()
			{
			return this->updateLiveSnapshotDetectPornConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UpdateLiveTopLevelDomainOutcome LiveClient::updateLiveTopLevelDomain(const UpdateLiveTopLevelDomainRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateLiveTopLevelDomainOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateLiveTopLevelDomainOutcome(UpdateLiveTopLevelDomainResult(outcome.result()));
	else
		return UpdateLiveTopLevelDomainOutcome(outcome.error());
}

void LiveClient::updateLiveTopLevelDomainAsync(const UpdateLiveTopLevelDomainRequest& request, const UpdateLiveTopLevelDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateLiveTopLevelDomain(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UpdateLiveTopLevelDomainOutcomeCallable LiveClient::updateLiveTopLevelDomainCallable(const UpdateLiveTopLevelDomainRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateLiveTopLevelDomainOutcome()>>(
			[this, request]()
			{
			return this->updateLiveTopLevelDomain(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

LiveClient::UpdateMixStreamOutcome LiveClient::updateMixStream(const UpdateMixStreamRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateMixStreamOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateMixStreamOutcome(UpdateMixStreamResult(outcome.result()));
	else
		return UpdateMixStreamOutcome(outcome.error());
}

void LiveClient::updateMixStreamAsync(const UpdateMixStreamRequest& request, const UpdateMixStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateMixStream(request), context);
	};

	asyncExecute(new Runnable(fn));
}

LiveClient::UpdateMixStreamOutcomeCallable LiveClient::updateMixStreamCallable(const UpdateMixStreamRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateMixStreamOutcome()>>(
			[this, request]()
			{
			return this->updateMixStream(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


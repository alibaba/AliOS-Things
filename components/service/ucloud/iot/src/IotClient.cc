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

#include <alibabacloud/iot/IotClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

namespace
{
	const std::string SERVICE_NAME = "Iot";
}

IotClient::IotClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "iot");
}

IotClient::IotClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "iot");
}

IotClient::IotClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "iot");
}

IotClient::~IotClient()
{}

IotClient::BatchAddDeviceGroupRelationsOutcome IotClient::batchAddDeviceGroupRelations(const BatchAddDeviceGroupRelationsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchAddDeviceGroupRelationsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchAddDeviceGroupRelationsOutcome(BatchAddDeviceGroupRelationsResult(outcome.result()));
	else
		return BatchAddDeviceGroupRelationsOutcome(outcome.error());
}

void IotClient::batchAddDeviceGroupRelationsAsync(const BatchAddDeviceGroupRelationsRequest& request, const BatchAddDeviceGroupRelationsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchAddDeviceGroupRelations(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchAddDeviceGroupRelationsOutcomeCallable IotClient::batchAddDeviceGroupRelationsCallable(const BatchAddDeviceGroupRelationsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchAddDeviceGroupRelationsOutcome()>>(
			[this, request]()
			{
			return this->batchAddDeviceGroupRelations(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchAddThingTopoOutcome IotClient::batchAddThingTopo(const BatchAddThingTopoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchAddThingTopoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchAddThingTopoOutcome(BatchAddThingTopoResult(outcome.result()));
	else
		return BatchAddThingTopoOutcome(outcome.error());
}

void IotClient::batchAddThingTopoAsync(const BatchAddThingTopoRequest& request, const BatchAddThingTopoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchAddThingTopo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchAddThingTopoOutcomeCallable IotClient::batchAddThingTopoCallable(const BatchAddThingTopoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchAddThingTopoOutcome()>>(
			[this, request]()
			{
			return this->batchAddThingTopo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchBindDeviceToEdgeInstanceWithDriverOutcome IotClient::batchBindDeviceToEdgeInstanceWithDriver(const BatchBindDeviceToEdgeInstanceWithDriverRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchBindDeviceToEdgeInstanceWithDriverOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchBindDeviceToEdgeInstanceWithDriverOutcome(BatchBindDeviceToEdgeInstanceWithDriverResult(outcome.result()));
	else
		return BatchBindDeviceToEdgeInstanceWithDriverOutcome(outcome.error());
}

void IotClient::batchBindDeviceToEdgeInstanceWithDriverAsync(const BatchBindDeviceToEdgeInstanceWithDriverRequest& request, const BatchBindDeviceToEdgeInstanceWithDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchBindDeviceToEdgeInstanceWithDriver(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchBindDeviceToEdgeInstanceWithDriverOutcomeCallable IotClient::batchBindDeviceToEdgeInstanceWithDriverCallable(const BatchBindDeviceToEdgeInstanceWithDriverRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchBindDeviceToEdgeInstanceWithDriverOutcome()>>(
			[this, request]()
			{
			return this->batchBindDeviceToEdgeInstanceWithDriver(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchCheckDeviceNamesOutcome IotClient::batchCheckDeviceNames(const BatchCheckDeviceNamesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchCheckDeviceNamesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchCheckDeviceNamesOutcome(BatchCheckDeviceNamesResult(outcome.result()));
	else
		return BatchCheckDeviceNamesOutcome(outcome.error());
}

void IotClient::batchCheckDeviceNamesAsync(const BatchCheckDeviceNamesRequest& request, const BatchCheckDeviceNamesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchCheckDeviceNames(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchCheckDeviceNamesOutcomeCallable IotClient::batchCheckDeviceNamesCallable(const BatchCheckDeviceNamesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchCheckDeviceNamesOutcome()>>(
			[this, request]()
			{
			return this->batchCheckDeviceNames(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchClearEdgeInstanceDeviceConfigOutcome IotClient::batchClearEdgeInstanceDeviceConfig(const BatchClearEdgeInstanceDeviceConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchClearEdgeInstanceDeviceConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchClearEdgeInstanceDeviceConfigOutcome(BatchClearEdgeInstanceDeviceConfigResult(outcome.result()));
	else
		return BatchClearEdgeInstanceDeviceConfigOutcome(outcome.error());
}

void IotClient::batchClearEdgeInstanceDeviceConfigAsync(const BatchClearEdgeInstanceDeviceConfigRequest& request, const BatchClearEdgeInstanceDeviceConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchClearEdgeInstanceDeviceConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchClearEdgeInstanceDeviceConfigOutcomeCallable IotClient::batchClearEdgeInstanceDeviceConfigCallable(const BatchClearEdgeInstanceDeviceConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchClearEdgeInstanceDeviceConfigOutcome()>>(
			[this, request]()
			{
			return this->batchClearEdgeInstanceDeviceConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchDeleteDeviceGroupRelationsOutcome IotClient::batchDeleteDeviceGroupRelations(const BatchDeleteDeviceGroupRelationsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchDeleteDeviceGroupRelationsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchDeleteDeviceGroupRelationsOutcome(BatchDeleteDeviceGroupRelationsResult(outcome.result()));
	else
		return BatchDeleteDeviceGroupRelationsOutcome(outcome.error());
}

void IotClient::batchDeleteDeviceGroupRelationsAsync(const BatchDeleteDeviceGroupRelationsRequest& request, const BatchDeleteDeviceGroupRelationsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchDeleteDeviceGroupRelations(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchDeleteDeviceGroupRelationsOutcomeCallable IotClient::batchDeleteDeviceGroupRelationsCallable(const BatchDeleteDeviceGroupRelationsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchDeleteDeviceGroupRelationsOutcome()>>(
			[this, request]()
			{
			return this->batchDeleteDeviceGroupRelations(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchDeleteEdgeInstanceChannelOutcome IotClient::batchDeleteEdgeInstanceChannel(const BatchDeleteEdgeInstanceChannelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchDeleteEdgeInstanceChannelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchDeleteEdgeInstanceChannelOutcome(BatchDeleteEdgeInstanceChannelResult(outcome.result()));
	else
		return BatchDeleteEdgeInstanceChannelOutcome(outcome.error());
}

void IotClient::batchDeleteEdgeInstanceChannelAsync(const BatchDeleteEdgeInstanceChannelRequest& request, const BatchDeleteEdgeInstanceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchDeleteEdgeInstanceChannel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchDeleteEdgeInstanceChannelOutcomeCallable IotClient::batchDeleteEdgeInstanceChannelCallable(const BatchDeleteEdgeInstanceChannelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchDeleteEdgeInstanceChannelOutcome()>>(
			[this, request]()
			{
			return this->batchDeleteEdgeInstanceChannel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchGetDeviceBindStatusOutcome IotClient::batchGetDeviceBindStatus(const BatchGetDeviceBindStatusRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchGetDeviceBindStatusOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchGetDeviceBindStatusOutcome(BatchGetDeviceBindStatusResult(outcome.result()));
	else
		return BatchGetDeviceBindStatusOutcome(outcome.error());
}

void IotClient::batchGetDeviceBindStatusAsync(const BatchGetDeviceBindStatusRequest& request, const BatchGetDeviceBindStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchGetDeviceBindStatus(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchGetDeviceBindStatusOutcomeCallable IotClient::batchGetDeviceBindStatusCallable(const BatchGetDeviceBindStatusRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchGetDeviceBindStatusOutcome()>>(
			[this, request]()
			{
			return this->batchGetDeviceBindStatus(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchGetDeviceStateOutcome IotClient::batchGetDeviceState(const BatchGetDeviceStateRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchGetDeviceStateOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchGetDeviceStateOutcome(BatchGetDeviceStateResult(outcome.result()));
	else
		return BatchGetDeviceStateOutcome(outcome.error());
}

void IotClient::batchGetDeviceStateAsync(const BatchGetDeviceStateRequest& request, const BatchGetDeviceStateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchGetDeviceState(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchGetDeviceStateOutcomeCallable IotClient::batchGetDeviceStateCallable(const BatchGetDeviceStateRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchGetDeviceStateOutcome()>>(
			[this, request]()
			{
			return this->batchGetDeviceState(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchGetEdgeDriverOutcome IotClient::batchGetEdgeDriver(const BatchGetEdgeDriverRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchGetEdgeDriverOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchGetEdgeDriverOutcome(BatchGetEdgeDriverResult(outcome.result()));
	else
		return BatchGetEdgeDriverOutcome(outcome.error());
}

void IotClient::batchGetEdgeDriverAsync(const BatchGetEdgeDriverRequest& request, const BatchGetEdgeDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchGetEdgeDriver(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchGetEdgeDriverOutcomeCallable IotClient::batchGetEdgeDriverCallable(const BatchGetEdgeDriverRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchGetEdgeDriverOutcome()>>(
			[this, request]()
			{
			return this->batchGetEdgeDriver(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchGetEdgeInstanceChannelOutcome IotClient::batchGetEdgeInstanceChannel(const BatchGetEdgeInstanceChannelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchGetEdgeInstanceChannelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchGetEdgeInstanceChannelOutcome(BatchGetEdgeInstanceChannelResult(outcome.result()));
	else
		return BatchGetEdgeInstanceChannelOutcome(outcome.error());
}

void IotClient::batchGetEdgeInstanceChannelAsync(const BatchGetEdgeInstanceChannelRequest& request, const BatchGetEdgeInstanceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchGetEdgeInstanceChannel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchGetEdgeInstanceChannelOutcomeCallable IotClient::batchGetEdgeInstanceChannelCallable(const BatchGetEdgeInstanceChannelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchGetEdgeInstanceChannelOutcome()>>(
			[this, request]()
			{
			return this->batchGetEdgeInstanceChannel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchGetEdgeInstanceDeviceChannelOutcome IotClient::batchGetEdgeInstanceDeviceChannel(const BatchGetEdgeInstanceDeviceChannelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchGetEdgeInstanceDeviceChannelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchGetEdgeInstanceDeviceChannelOutcome(BatchGetEdgeInstanceDeviceChannelResult(outcome.result()));
	else
		return BatchGetEdgeInstanceDeviceChannelOutcome(outcome.error());
}

void IotClient::batchGetEdgeInstanceDeviceChannelAsync(const BatchGetEdgeInstanceDeviceChannelRequest& request, const BatchGetEdgeInstanceDeviceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchGetEdgeInstanceDeviceChannel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchGetEdgeInstanceDeviceChannelOutcomeCallable IotClient::batchGetEdgeInstanceDeviceChannelCallable(const BatchGetEdgeInstanceDeviceChannelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchGetEdgeInstanceDeviceChannelOutcome()>>(
			[this, request]()
			{
			return this->batchGetEdgeInstanceDeviceChannel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchGetEdgeInstanceDeviceConfigOutcome IotClient::batchGetEdgeInstanceDeviceConfig(const BatchGetEdgeInstanceDeviceConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchGetEdgeInstanceDeviceConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchGetEdgeInstanceDeviceConfigOutcome(BatchGetEdgeInstanceDeviceConfigResult(outcome.result()));
	else
		return BatchGetEdgeInstanceDeviceConfigOutcome(outcome.error());
}

void IotClient::batchGetEdgeInstanceDeviceConfigAsync(const BatchGetEdgeInstanceDeviceConfigRequest& request, const BatchGetEdgeInstanceDeviceConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchGetEdgeInstanceDeviceConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchGetEdgeInstanceDeviceConfigOutcomeCallable IotClient::batchGetEdgeInstanceDeviceConfigCallable(const BatchGetEdgeInstanceDeviceConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchGetEdgeInstanceDeviceConfigOutcome()>>(
			[this, request]()
			{
			return this->batchGetEdgeInstanceDeviceConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchGetEdgeInstanceDeviceDriverOutcome IotClient::batchGetEdgeInstanceDeviceDriver(const BatchGetEdgeInstanceDeviceDriverRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchGetEdgeInstanceDeviceDriverOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchGetEdgeInstanceDeviceDriverOutcome(BatchGetEdgeInstanceDeviceDriverResult(outcome.result()));
	else
		return BatchGetEdgeInstanceDeviceDriverOutcome(outcome.error());
}

void IotClient::batchGetEdgeInstanceDeviceDriverAsync(const BatchGetEdgeInstanceDeviceDriverRequest& request, const BatchGetEdgeInstanceDeviceDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchGetEdgeInstanceDeviceDriver(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchGetEdgeInstanceDeviceDriverOutcomeCallable IotClient::batchGetEdgeInstanceDeviceDriverCallable(const BatchGetEdgeInstanceDeviceDriverRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchGetEdgeInstanceDeviceDriverOutcome()>>(
			[this, request]()
			{
			return this->batchGetEdgeInstanceDeviceDriver(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchGetEdgeInstanceDriverConfigsOutcome IotClient::batchGetEdgeInstanceDriverConfigs(const BatchGetEdgeInstanceDriverConfigsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchGetEdgeInstanceDriverConfigsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchGetEdgeInstanceDriverConfigsOutcome(BatchGetEdgeInstanceDriverConfigsResult(outcome.result()));
	else
		return BatchGetEdgeInstanceDriverConfigsOutcome(outcome.error());
}

void IotClient::batchGetEdgeInstanceDriverConfigsAsync(const BatchGetEdgeInstanceDriverConfigsRequest& request, const BatchGetEdgeInstanceDriverConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchGetEdgeInstanceDriverConfigs(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchGetEdgeInstanceDriverConfigsOutcomeCallable IotClient::batchGetEdgeInstanceDriverConfigsCallable(const BatchGetEdgeInstanceDriverConfigsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchGetEdgeInstanceDriverConfigsOutcome()>>(
			[this, request]()
			{
			return this->batchGetEdgeInstanceDriverConfigs(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchQueryDeviceDetailOutcome IotClient::batchQueryDeviceDetail(const BatchQueryDeviceDetailRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchQueryDeviceDetailOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchQueryDeviceDetailOutcome(BatchQueryDeviceDetailResult(outcome.result()));
	else
		return BatchQueryDeviceDetailOutcome(outcome.error());
}

void IotClient::batchQueryDeviceDetailAsync(const BatchQueryDeviceDetailRequest& request, const BatchQueryDeviceDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchQueryDeviceDetail(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchQueryDeviceDetailOutcomeCallable IotClient::batchQueryDeviceDetailCallable(const BatchQueryDeviceDetailRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchQueryDeviceDetailOutcome()>>(
			[this, request]()
			{
			return this->batchQueryDeviceDetail(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchRegisterDeviceOutcome IotClient::batchRegisterDevice(const BatchRegisterDeviceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchRegisterDeviceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchRegisterDeviceOutcome(BatchRegisterDeviceResult(outcome.result()));
	else
		return BatchRegisterDeviceOutcome(outcome.error());
}

void IotClient::batchRegisterDeviceAsync(const BatchRegisterDeviceRequest& request, const BatchRegisterDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchRegisterDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchRegisterDeviceOutcomeCallable IotClient::batchRegisterDeviceCallable(const BatchRegisterDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchRegisterDeviceOutcome()>>(
			[this, request]()
			{
			return this->batchRegisterDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchRegisterDeviceWithApplyIdOutcome IotClient::batchRegisterDeviceWithApplyId(const BatchRegisterDeviceWithApplyIdRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchRegisterDeviceWithApplyIdOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchRegisterDeviceWithApplyIdOutcome(BatchRegisterDeviceWithApplyIdResult(outcome.result()));
	else
		return BatchRegisterDeviceWithApplyIdOutcome(outcome.error());
}

void IotClient::batchRegisterDeviceWithApplyIdAsync(const BatchRegisterDeviceWithApplyIdRequest& request, const BatchRegisterDeviceWithApplyIdAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchRegisterDeviceWithApplyId(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchRegisterDeviceWithApplyIdOutcomeCallable IotClient::batchRegisterDeviceWithApplyIdCallable(const BatchRegisterDeviceWithApplyIdRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchRegisterDeviceWithApplyIdOutcome()>>(
			[this, request]()
			{
			return this->batchRegisterDeviceWithApplyId(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchSetEdgeInstanceDeviceChannelOutcome IotClient::batchSetEdgeInstanceDeviceChannel(const BatchSetEdgeInstanceDeviceChannelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchSetEdgeInstanceDeviceChannelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchSetEdgeInstanceDeviceChannelOutcome(BatchSetEdgeInstanceDeviceChannelResult(outcome.result()));
	else
		return BatchSetEdgeInstanceDeviceChannelOutcome(outcome.error());
}

void IotClient::batchSetEdgeInstanceDeviceChannelAsync(const BatchSetEdgeInstanceDeviceChannelRequest& request, const BatchSetEdgeInstanceDeviceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchSetEdgeInstanceDeviceChannel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchSetEdgeInstanceDeviceChannelOutcomeCallable IotClient::batchSetEdgeInstanceDeviceChannelCallable(const BatchSetEdgeInstanceDeviceChannelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchSetEdgeInstanceDeviceChannelOutcome()>>(
			[this, request]()
			{
			return this->batchSetEdgeInstanceDeviceChannel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchSetEdgeInstanceDeviceConfigOutcome IotClient::batchSetEdgeInstanceDeviceConfig(const BatchSetEdgeInstanceDeviceConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchSetEdgeInstanceDeviceConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchSetEdgeInstanceDeviceConfigOutcome(BatchSetEdgeInstanceDeviceConfigResult(outcome.result()));
	else
		return BatchSetEdgeInstanceDeviceConfigOutcome(outcome.error());
}

void IotClient::batchSetEdgeInstanceDeviceConfigAsync(const BatchSetEdgeInstanceDeviceConfigRequest& request, const BatchSetEdgeInstanceDeviceConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchSetEdgeInstanceDeviceConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchSetEdgeInstanceDeviceConfigOutcomeCallable IotClient::batchSetEdgeInstanceDeviceConfigCallable(const BatchSetEdgeInstanceDeviceConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchSetEdgeInstanceDeviceConfigOutcome()>>(
			[this, request]()
			{
			return this->batchSetEdgeInstanceDeviceConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchUnbindDeviceFromEdgeInstanceOutcome IotClient::batchUnbindDeviceFromEdgeInstance(const BatchUnbindDeviceFromEdgeInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchUnbindDeviceFromEdgeInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchUnbindDeviceFromEdgeInstanceOutcome(BatchUnbindDeviceFromEdgeInstanceResult(outcome.result()));
	else
		return BatchUnbindDeviceFromEdgeInstanceOutcome(outcome.error());
}

void IotClient::batchUnbindDeviceFromEdgeInstanceAsync(const BatchUnbindDeviceFromEdgeInstanceRequest& request, const BatchUnbindDeviceFromEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchUnbindDeviceFromEdgeInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchUnbindDeviceFromEdgeInstanceOutcomeCallable IotClient::batchUnbindDeviceFromEdgeInstanceCallable(const BatchUnbindDeviceFromEdgeInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchUnbindDeviceFromEdgeInstanceOutcome()>>(
			[this, request]()
			{
			return this->batchUnbindDeviceFromEdgeInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BatchUpdateDeviceNicknameOutcome IotClient::batchUpdateDeviceNickname(const BatchUpdateDeviceNicknameRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchUpdateDeviceNicknameOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchUpdateDeviceNicknameOutcome(BatchUpdateDeviceNicknameResult(outcome.result()));
	else
		return BatchUpdateDeviceNicknameOutcome(outcome.error());
}

void IotClient::batchUpdateDeviceNicknameAsync(const BatchUpdateDeviceNicknameRequest& request, const BatchUpdateDeviceNicknameAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchUpdateDeviceNickname(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BatchUpdateDeviceNicknameOutcomeCallable IotClient::batchUpdateDeviceNicknameCallable(const BatchUpdateDeviceNicknameRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchUpdateDeviceNicknameOutcome()>>(
			[this, request]()
			{
			return this->batchUpdateDeviceNickname(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BindApplicationToEdgeInstanceOutcome IotClient::bindApplicationToEdgeInstance(const BindApplicationToEdgeInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BindApplicationToEdgeInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BindApplicationToEdgeInstanceOutcome(BindApplicationToEdgeInstanceResult(outcome.result()));
	else
		return BindApplicationToEdgeInstanceOutcome(outcome.error());
}

void IotClient::bindApplicationToEdgeInstanceAsync(const BindApplicationToEdgeInstanceRequest& request, const BindApplicationToEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, bindApplicationToEdgeInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BindApplicationToEdgeInstanceOutcomeCallable IotClient::bindApplicationToEdgeInstanceCallable(const BindApplicationToEdgeInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BindApplicationToEdgeInstanceOutcome()>>(
			[this, request]()
			{
			return this->bindApplicationToEdgeInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BindDriverToEdgeInstanceOutcome IotClient::bindDriverToEdgeInstance(const BindDriverToEdgeInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BindDriverToEdgeInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BindDriverToEdgeInstanceOutcome(BindDriverToEdgeInstanceResult(outcome.result()));
	else
		return BindDriverToEdgeInstanceOutcome(outcome.error());
}

void IotClient::bindDriverToEdgeInstanceAsync(const BindDriverToEdgeInstanceRequest& request, const BindDriverToEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, bindDriverToEdgeInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BindDriverToEdgeInstanceOutcomeCallable IotClient::bindDriverToEdgeInstanceCallable(const BindDriverToEdgeInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BindDriverToEdgeInstanceOutcome()>>(
			[this, request]()
			{
			return this->bindDriverToEdgeInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BindGatewayToEdgeInstanceOutcome IotClient::bindGatewayToEdgeInstance(const BindGatewayToEdgeInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BindGatewayToEdgeInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BindGatewayToEdgeInstanceOutcome(BindGatewayToEdgeInstanceResult(outcome.result()));
	else
		return BindGatewayToEdgeInstanceOutcome(outcome.error());
}

void IotClient::bindGatewayToEdgeInstanceAsync(const BindGatewayToEdgeInstanceRequest& request, const BindGatewayToEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, bindGatewayToEdgeInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BindGatewayToEdgeInstanceOutcomeCallable IotClient::bindGatewayToEdgeInstanceCallable(const BindGatewayToEdgeInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BindGatewayToEdgeInstanceOutcome()>>(
			[this, request]()
			{
			return this->bindGatewayToEdgeInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::BindSceneRuleToEdgeInstanceOutcome IotClient::bindSceneRuleToEdgeInstance(const BindSceneRuleToEdgeInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BindSceneRuleToEdgeInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BindSceneRuleToEdgeInstanceOutcome(BindSceneRuleToEdgeInstanceResult(outcome.result()));
	else
		return BindSceneRuleToEdgeInstanceOutcome(outcome.error());
}

void IotClient::bindSceneRuleToEdgeInstanceAsync(const BindSceneRuleToEdgeInstanceRequest& request, const BindSceneRuleToEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, bindSceneRuleToEdgeInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::BindSceneRuleToEdgeInstanceOutcomeCallable IotClient::bindSceneRuleToEdgeInstanceCallable(const BindSceneRuleToEdgeInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BindSceneRuleToEdgeInstanceOutcome()>>(
			[this, request]()
			{
			return this->bindSceneRuleToEdgeInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CancelOTAStrategyByJobOutcome IotClient::cancelOTAStrategyByJob(const CancelOTAStrategyByJobRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CancelOTAStrategyByJobOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CancelOTAStrategyByJobOutcome(CancelOTAStrategyByJobResult(outcome.result()));
	else
		return CancelOTAStrategyByJobOutcome(outcome.error());
}

void IotClient::cancelOTAStrategyByJobAsync(const CancelOTAStrategyByJobRequest& request, const CancelOTAStrategyByJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, cancelOTAStrategyByJob(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CancelOTAStrategyByJobOutcomeCallable IotClient::cancelOTAStrategyByJobCallable(const CancelOTAStrategyByJobRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CancelOTAStrategyByJobOutcome()>>(
			[this, request]()
			{
			return this->cancelOTAStrategyByJob(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CancelOTATaskByDeviceOutcome IotClient::cancelOTATaskByDevice(const CancelOTATaskByDeviceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CancelOTATaskByDeviceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CancelOTATaskByDeviceOutcome(CancelOTATaskByDeviceResult(outcome.result()));
	else
		return CancelOTATaskByDeviceOutcome(outcome.error());
}

void IotClient::cancelOTATaskByDeviceAsync(const CancelOTATaskByDeviceRequest& request, const CancelOTATaskByDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, cancelOTATaskByDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CancelOTATaskByDeviceOutcomeCallable IotClient::cancelOTATaskByDeviceCallable(const CancelOTATaskByDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CancelOTATaskByDeviceOutcome()>>(
			[this, request]()
			{
			return this->cancelOTATaskByDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CancelOTATaskByJobOutcome IotClient::cancelOTATaskByJob(const CancelOTATaskByJobRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CancelOTATaskByJobOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CancelOTATaskByJobOutcome(CancelOTATaskByJobResult(outcome.result()));
	else
		return CancelOTATaskByJobOutcome(outcome.error());
}

void IotClient::cancelOTATaskByJobAsync(const CancelOTATaskByJobRequest& request, const CancelOTATaskByJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, cancelOTATaskByJob(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CancelOTATaskByJobOutcomeCallable IotClient::cancelOTATaskByJobCallable(const CancelOTATaskByJobRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CancelOTATaskByJobOutcome()>>(
			[this, request]()
			{
			return this->cancelOTATaskByJob(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ClearEdgeInstanceDriverConfigsOutcome IotClient::clearEdgeInstanceDriverConfigs(const ClearEdgeInstanceDriverConfigsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ClearEdgeInstanceDriverConfigsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ClearEdgeInstanceDriverConfigsOutcome(ClearEdgeInstanceDriverConfigsResult(outcome.result()));
	else
		return ClearEdgeInstanceDriverConfigsOutcome(outcome.error());
}

void IotClient::clearEdgeInstanceDriverConfigsAsync(const ClearEdgeInstanceDriverConfigsRequest& request, const ClearEdgeInstanceDriverConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, clearEdgeInstanceDriverConfigs(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ClearEdgeInstanceDriverConfigsOutcomeCallable IotClient::clearEdgeInstanceDriverConfigsCallable(const ClearEdgeInstanceDriverConfigsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ClearEdgeInstanceDriverConfigsOutcome()>>(
			[this, request]()
			{
			return this->clearEdgeInstanceDriverConfigs(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CloseEdgeInstanceDeploymentOutcome IotClient::closeEdgeInstanceDeployment(const CloseEdgeInstanceDeploymentRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CloseEdgeInstanceDeploymentOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CloseEdgeInstanceDeploymentOutcome(CloseEdgeInstanceDeploymentResult(outcome.result()));
	else
		return CloseEdgeInstanceDeploymentOutcome(outcome.error());
}

void IotClient::closeEdgeInstanceDeploymentAsync(const CloseEdgeInstanceDeploymentRequest& request, const CloseEdgeInstanceDeploymentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, closeEdgeInstanceDeployment(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CloseEdgeInstanceDeploymentOutcomeCallable IotClient::closeEdgeInstanceDeploymentCallable(const CloseEdgeInstanceDeploymentRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CloseEdgeInstanceDeploymentOutcome()>>(
			[this, request]()
			{
			return this->closeEdgeInstanceDeployment(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CopyThingModelOutcome IotClient::copyThingModel(const CopyThingModelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CopyThingModelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CopyThingModelOutcome(CopyThingModelResult(outcome.result()));
	else
		return CopyThingModelOutcome(outcome.error());
}

void IotClient::copyThingModelAsync(const CopyThingModelRequest& request, const CopyThingModelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, copyThingModel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CopyThingModelOutcomeCallable IotClient::copyThingModelCallable(const CopyThingModelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CopyThingModelOutcome()>>(
			[this, request]()
			{
			return this->copyThingModel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateConsumerGroupOutcome IotClient::createConsumerGroup(const CreateConsumerGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateConsumerGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateConsumerGroupOutcome(CreateConsumerGroupResult(outcome.result()));
	else
		return CreateConsumerGroupOutcome(outcome.error());
}

void IotClient::createConsumerGroupAsync(const CreateConsumerGroupRequest& request, const CreateConsumerGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createConsumerGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateConsumerGroupOutcomeCallable IotClient::createConsumerGroupCallable(const CreateConsumerGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateConsumerGroupOutcome()>>(
			[this, request]()
			{
			return this->createConsumerGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateConsumerGroupSubscribeRelationOutcome IotClient::createConsumerGroupSubscribeRelation(const CreateConsumerGroupSubscribeRelationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateConsumerGroupSubscribeRelationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateConsumerGroupSubscribeRelationOutcome(CreateConsumerGroupSubscribeRelationResult(outcome.result()));
	else
		return CreateConsumerGroupSubscribeRelationOutcome(outcome.error());
}

void IotClient::createConsumerGroupSubscribeRelationAsync(const CreateConsumerGroupSubscribeRelationRequest& request, const CreateConsumerGroupSubscribeRelationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createConsumerGroupSubscribeRelation(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateConsumerGroupSubscribeRelationOutcomeCallable IotClient::createConsumerGroupSubscribeRelationCallable(const CreateConsumerGroupSubscribeRelationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateConsumerGroupSubscribeRelationOutcome()>>(
			[this, request]()
			{
			return this->createConsumerGroupSubscribeRelation(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateDataAPIServiceOutcome IotClient::createDataAPIService(const CreateDataAPIServiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateDataAPIServiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateDataAPIServiceOutcome(CreateDataAPIServiceResult(outcome.result()));
	else
		return CreateDataAPIServiceOutcome(outcome.error());
}

void IotClient::createDataAPIServiceAsync(const CreateDataAPIServiceRequest& request, const CreateDataAPIServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createDataAPIService(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateDataAPIServiceOutcomeCallable IotClient::createDataAPIServiceCallable(const CreateDataAPIServiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateDataAPIServiceOutcome()>>(
			[this, request]()
			{
			return this->createDataAPIService(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateDeviceGroupOutcome IotClient::createDeviceGroup(const CreateDeviceGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateDeviceGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateDeviceGroupOutcome(CreateDeviceGroupResult(outcome.result()));
	else
		return CreateDeviceGroupOutcome(outcome.error());
}

void IotClient::createDeviceGroupAsync(const CreateDeviceGroupRequest& request, const CreateDeviceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createDeviceGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateDeviceGroupOutcomeCallable IotClient::createDeviceGroupCallable(const CreateDeviceGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateDeviceGroupOutcome()>>(
			[this, request]()
			{
			return this->createDeviceGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateEdgeDriverOutcome IotClient::createEdgeDriver(const CreateEdgeDriverRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateEdgeDriverOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateEdgeDriverOutcome(CreateEdgeDriverResult(outcome.result()));
	else
		return CreateEdgeDriverOutcome(outcome.error());
}

void IotClient::createEdgeDriverAsync(const CreateEdgeDriverRequest& request, const CreateEdgeDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createEdgeDriver(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateEdgeDriverOutcomeCallable IotClient::createEdgeDriverCallable(const CreateEdgeDriverRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateEdgeDriverOutcome()>>(
			[this, request]()
			{
			return this->createEdgeDriver(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateEdgeDriverVersionOutcome IotClient::createEdgeDriverVersion(const CreateEdgeDriverVersionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateEdgeDriverVersionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateEdgeDriverVersionOutcome(CreateEdgeDriverVersionResult(outcome.result()));
	else
		return CreateEdgeDriverVersionOutcome(outcome.error());
}

void IotClient::createEdgeDriverVersionAsync(const CreateEdgeDriverVersionRequest& request, const CreateEdgeDriverVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createEdgeDriverVersion(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateEdgeDriverVersionOutcomeCallable IotClient::createEdgeDriverVersionCallable(const CreateEdgeDriverVersionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateEdgeDriverVersionOutcome()>>(
			[this, request]()
			{
			return this->createEdgeDriverVersion(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateEdgeInstanceOutcome IotClient::createEdgeInstance(const CreateEdgeInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateEdgeInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateEdgeInstanceOutcome(CreateEdgeInstanceResult(outcome.result()));
	else
		return CreateEdgeInstanceOutcome(outcome.error());
}

void IotClient::createEdgeInstanceAsync(const CreateEdgeInstanceRequest& request, const CreateEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createEdgeInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateEdgeInstanceOutcomeCallable IotClient::createEdgeInstanceCallable(const CreateEdgeInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateEdgeInstanceOutcome()>>(
			[this, request]()
			{
			return this->createEdgeInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateEdgeInstanceChannelOutcome IotClient::createEdgeInstanceChannel(const CreateEdgeInstanceChannelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateEdgeInstanceChannelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateEdgeInstanceChannelOutcome(CreateEdgeInstanceChannelResult(outcome.result()));
	else
		return CreateEdgeInstanceChannelOutcome(outcome.error());
}

void IotClient::createEdgeInstanceChannelAsync(const CreateEdgeInstanceChannelRequest& request, const CreateEdgeInstanceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createEdgeInstanceChannel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateEdgeInstanceChannelOutcomeCallable IotClient::createEdgeInstanceChannelCallable(const CreateEdgeInstanceChannelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateEdgeInstanceChannelOutcome()>>(
			[this, request]()
			{
			return this->createEdgeInstanceChannel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateEdgeInstanceDeploymentOutcome IotClient::createEdgeInstanceDeployment(const CreateEdgeInstanceDeploymentRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateEdgeInstanceDeploymentOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateEdgeInstanceDeploymentOutcome(CreateEdgeInstanceDeploymentResult(outcome.result()));
	else
		return CreateEdgeInstanceDeploymentOutcome(outcome.error());
}

void IotClient::createEdgeInstanceDeploymentAsync(const CreateEdgeInstanceDeploymentRequest& request, const CreateEdgeInstanceDeploymentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createEdgeInstanceDeployment(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateEdgeInstanceDeploymentOutcomeCallable IotClient::createEdgeInstanceDeploymentCallable(const CreateEdgeInstanceDeploymentRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateEdgeInstanceDeploymentOutcome()>>(
			[this, request]()
			{
			return this->createEdgeInstanceDeployment(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateEdgeOssPreSignedAddressOutcome IotClient::createEdgeOssPreSignedAddress(const CreateEdgeOssPreSignedAddressRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateEdgeOssPreSignedAddressOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateEdgeOssPreSignedAddressOutcome(CreateEdgeOssPreSignedAddressResult(outcome.result()));
	else
		return CreateEdgeOssPreSignedAddressOutcome(outcome.error());
}

void IotClient::createEdgeOssPreSignedAddressAsync(const CreateEdgeOssPreSignedAddressRequest& request, const CreateEdgeOssPreSignedAddressAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createEdgeOssPreSignedAddress(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateEdgeOssPreSignedAddressOutcomeCallable IotClient::createEdgeOssPreSignedAddressCallable(const CreateEdgeOssPreSignedAddressRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateEdgeOssPreSignedAddressOutcome()>>(
			[this, request]()
			{
			return this->createEdgeOssPreSignedAddress(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateLoRaNodesTaskOutcome IotClient::createLoRaNodesTask(const CreateLoRaNodesTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateLoRaNodesTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateLoRaNodesTaskOutcome(CreateLoRaNodesTaskResult(outcome.result()));
	else
		return CreateLoRaNodesTaskOutcome(outcome.error());
}

void IotClient::createLoRaNodesTaskAsync(const CreateLoRaNodesTaskRequest& request, const CreateLoRaNodesTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createLoRaNodesTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateLoRaNodesTaskOutcomeCallable IotClient::createLoRaNodesTaskCallable(const CreateLoRaNodesTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateLoRaNodesTaskOutcome()>>(
			[this, request]()
			{
			return this->createLoRaNodesTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateOTADynamicUpgradeJobOutcome IotClient::createOTADynamicUpgradeJob(const CreateOTADynamicUpgradeJobRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateOTADynamicUpgradeJobOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateOTADynamicUpgradeJobOutcome(CreateOTADynamicUpgradeJobResult(outcome.result()));
	else
		return CreateOTADynamicUpgradeJobOutcome(outcome.error());
}

void IotClient::createOTADynamicUpgradeJobAsync(const CreateOTADynamicUpgradeJobRequest& request, const CreateOTADynamicUpgradeJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createOTADynamicUpgradeJob(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateOTADynamicUpgradeJobOutcomeCallable IotClient::createOTADynamicUpgradeJobCallable(const CreateOTADynamicUpgradeJobRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateOTADynamicUpgradeJobOutcome()>>(
			[this, request]()
			{
			return this->createOTADynamicUpgradeJob(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateOTAFirmwareOutcome IotClient::createOTAFirmware(const CreateOTAFirmwareRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateOTAFirmwareOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateOTAFirmwareOutcome(CreateOTAFirmwareResult(outcome.result()));
	else
		return CreateOTAFirmwareOutcome(outcome.error());
}

void IotClient::createOTAFirmwareAsync(const CreateOTAFirmwareRequest& request, const CreateOTAFirmwareAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createOTAFirmware(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateOTAFirmwareOutcomeCallable IotClient::createOTAFirmwareCallable(const CreateOTAFirmwareRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateOTAFirmwareOutcome()>>(
			[this, request]()
			{
			return this->createOTAFirmware(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateOTAStaticUpgradeJobOutcome IotClient::createOTAStaticUpgradeJob(const CreateOTAStaticUpgradeJobRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateOTAStaticUpgradeJobOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateOTAStaticUpgradeJobOutcome(CreateOTAStaticUpgradeJobResult(outcome.result()));
	else
		return CreateOTAStaticUpgradeJobOutcome(outcome.error());
}

void IotClient::createOTAStaticUpgradeJobAsync(const CreateOTAStaticUpgradeJobRequest& request, const CreateOTAStaticUpgradeJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createOTAStaticUpgradeJob(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateOTAStaticUpgradeJobOutcomeCallable IotClient::createOTAStaticUpgradeJobCallable(const CreateOTAStaticUpgradeJobRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateOTAStaticUpgradeJobOutcome()>>(
			[this, request]()
			{
			return this->createOTAStaticUpgradeJob(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateOTAVerifyJobOutcome IotClient::createOTAVerifyJob(const CreateOTAVerifyJobRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateOTAVerifyJobOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateOTAVerifyJobOutcome(CreateOTAVerifyJobResult(outcome.result()));
	else
		return CreateOTAVerifyJobOutcome(outcome.error());
}

void IotClient::createOTAVerifyJobAsync(const CreateOTAVerifyJobRequest& request, const CreateOTAVerifyJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createOTAVerifyJob(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateOTAVerifyJobOutcomeCallable IotClient::createOTAVerifyJobCallable(const CreateOTAVerifyJobRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateOTAVerifyJobOutcome()>>(
			[this, request]()
			{
			return this->createOTAVerifyJob(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateProductOutcome IotClient::createProduct(const CreateProductRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateProductOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateProductOutcome(CreateProductResult(outcome.result()));
	else
		return CreateProductOutcome(outcome.error());
}

void IotClient::createProductAsync(const CreateProductRequest& request, const CreateProductAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createProduct(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateProductOutcomeCallable IotClient::createProductCallable(const CreateProductRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateProductOutcome()>>(
			[this, request]()
			{
			return this->createProduct(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateProductTagsOutcome IotClient::createProductTags(const CreateProductTagsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateProductTagsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateProductTagsOutcome(CreateProductTagsResult(outcome.result()));
	else
		return CreateProductTagsOutcome(outcome.error());
}

void IotClient::createProductTagsAsync(const CreateProductTagsRequest& request, const CreateProductTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createProductTags(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateProductTagsOutcomeCallable IotClient::createProductTagsCallable(const CreateProductTagsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateProductTagsOutcome()>>(
			[this, request]()
			{
			return this->createProductTags(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateProductTopicOutcome IotClient::createProductTopic(const CreateProductTopicRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateProductTopicOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateProductTopicOutcome(CreateProductTopicResult(outcome.result()));
	else
		return CreateProductTopicOutcome(outcome.error());
}

void IotClient::createProductTopicAsync(const CreateProductTopicRequest& request, const CreateProductTopicAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createProductTopic(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateProductTopicOutcomeCallable IotClient::createProductTopicCallable(const CreateProductTopicRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateProductTopicOutcome()>>(
			[this, request]()
			{
			return this->createProductTopic(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateRuleOutcome IotClient::createRule(const CreateRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateRuleOutcome(CreateRuleResult(outcome.result()));
	else
		return CreateRuleOutcome(outcome.error());
}

void IotClient::createRuleAsync(const CreateRuleRequest& request, const CreateRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateRuleOutcomeCallable IotClient::createRuleCallable(const CreateRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateRuleOutcome()>>(
			[this, request]()
			{
			return this->createRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateRuleActionOutcome IotClient::createRuleAction(const CreateRuleActionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateRuleActionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateRuleActionOutcome(CreateRuleActionResult(outcome.result()));
	else
		return CreateRuleActionOutcome(outcome.error());
}

void IotClient::createRuleActionAsync(const CreateRuleActionRequest& request, const CreateRuleActionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createRuleAction(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateRuleActionOutcomeCallable IotClient::createRuleActionCallable(const CreateRuleActionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateRuleActionOutcome()>>(
			[this, request]()
			{
			return this->createRuleAction(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateSceneRuleOutcome IotClient::createSceneRule(const CreateSceneRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateSceneRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateSceneRuleOutcome(CreateSceneRuleResult(outcome.result()));
	else
		return CreateSceneRuleOutcome(outcome.error());
}

void IotClient::createSceneRuleAsync(const CreateSceneRuleRequest& request, const CreateSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createSceneRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateSceneRuleOutcomeCallable IotClient::createSceneRuleCallable(const CreateSceneRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateSceneRuleOutcome()>>(
			[this, request]()
			{
			return this->createSceneRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateSubscribeRelationOutcome IotClient::createSubscribeRelation(const CreateSubscribeRelationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateSubscribeRelationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateSubscribeRelationOutcome(CreateSubscribeRelationResult(outcome.result()));
	else
		return CreateSubscribeRelationOutcome(outcome.error());
}

void IotClient::createSubscribeRelationAsync(const CreateSubscribeRelationRequest& request, const CreateSubscribeRelationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createSubscribeRelation(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateSubscribeRelationOutcomeCallable IotClient::createSubscribeRelationCallable(const CreateSubscribeRelationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateSubscribeRelationOutcome()>>(
			[this, request]()
			{
			return this->createSubscribeRelation(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateThingModelOutcome IotClient::createThingModel(const CreateThingModelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateThingModelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateThingModelOutcome(CreateThingModelResult(outcome.result()));
	else
		return CreateThingModelOutcome(outcome.error());
}

void IotClient::createThingModelAsync(const CreateThingModelRequest& request, const CreateThingModelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createThingModel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateThingModelOutcomeCallable IotClient::createThingModelCallable(const CreateThingModelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateThingModelOutcome()>>(
			[this, request]()
			{
			return this->createThingModel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::CreateTopicRouteTableOutcome IotClient::createTopicRouteTable(const CreateTopicRouteTableRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateTopicRouteTableOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateTopicRouteTableOutcome(CreateTopicRouteTableResult(outcome.result()));
	else
		return CreateTopicRouteTableOutcome(outcome.error());
}

void IotClient::createTopicRouteTableAsync(const CreateTopicRouteTableRequest& request, const CreateTopicRouteTableAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createTopicRouteTable(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::CreateTopicRouteTableOutcomeCallable IotClient::createTopicRouteTableCallable(const CreateTopicRouteTableRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateTopicRouteTableOutcome()>>(
			[this, request]()
			{
			return this->createTopicRouteTable(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteConsumerGroupOutcome IotClient::deleteConsumerGroup(const DeleteConsumerGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteConsumerGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteConsumerGroupOutcome(DeleteConsumerGroupResult(outcome.result()));
	else
		return DeleteConsumerGroupOutcome(outcome.error());
}

void IotClient::deleteConsumerGroupAsync(const DeleteConsumerGroupRequest& request, const DeleteConsumerGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteConsumerGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteConsumerGroupOutcomeCallable IotClient::deleteConsumerGroupCallable(const DeleteConsumerGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteConsumerGroupOutcome()>>(
			[this, request]()
			{
			return this->deleteConsumerGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteConsumerGroupSubscribeRelationOutcome IotClient::deleteConsumerGroupSubscribeRelation(const DeleteConsumerGroupSubscribeRelationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteConsumerGroupSubscribeRelationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteConsumerGroupSubscribeRelationOutcome(DeleteConsumerGroupSubscribeRelationResult(outcome.result()));
	else
		return DeleteConsumerGroupSubscribeRelationOutcome(outcome.error());
}

void IotClient::deleteConsumerGroupSubscribeRelationAsync(const DeleteConsumerGroupSubscribeRelationRequest& request, const DeleteConsumerGroupSubscribeRelationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteConsumerGroupSubscribeRelation(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteConsumerGroupSubscribeRelationOutcomeCallable IotClient::deleteConsumerGroupSubscribeRelationCallable(const DeleteConsumerGroupSubscribeRelationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteConsumerGroupSubscribeRelationOutcome()>>(
			[this, request]()
			{
			return this->deleteConsumerGroupSubscribeRelation(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteDeviceOutcome IotClient::deleteDevice(const DeleteDeviceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteDeviceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteDeviceOutcome(DeleteDeviceResult(outcome.result()));
	else
		return DeleteDeviceOutcome(outcome.error());
}

void IotClient::deleteDeviceAsync(const DeleteDeviceRequest& request, const DeleteDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteDeviceOutcomeCallable IotClient::deleteDeviceCallable(const DeleteDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteDeviceOutcome()>>(
			[this, request]()
			{
			return this->deleteDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteDeviceFileOutcome IotClient::deleteDeviceFile(const DeleteDeviceFileRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteDeviceFileOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteDeviceFileOutcome(DeleteDeviceFileResult(outcome.result()));
	else
		return DeleteDeviceFileOutcome(outcome.error());
}

void IotClient::deleteDeviceFileAsync(const DeleteDeviceFileRequest& request, const DeleteDeviceFileAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteDeviceFile(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteDeviceFileOutcomeCallable IotClient::deleteDeviceFileCallable(const DeleteDeviceFileRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteDeviceFileOutcome()>>(
			[this, request]()
			{
			return this->deleteDeviceFile(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteDeviceGroupOutcome IotClient::deleteDeviceGroup(const DeleteDeviceGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteDeviceGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteDeviceGroupOutcome(DeleteDeviceGroupResult(outcome.result()));
	else
		return DeleteDeviceGroupOutcome(outcome.error());
}

void IotClient::deleteDeviceGroupAsync(const DeleteDeviceGroupRequest& request, const DeleteDeviceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteDeviceGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteDeviceGroupOutcomeCallable IotClient::deleteDeviceGroupCallable(const DeleteDeviceGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteDeviceGroupOutcome()>>(
			[this, request]()
			{
			return this->deleteDeviceGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteDevicePropOutcome IotClient::deleteDeviceProp(const DeleteDevicePropRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteDevicePropOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteDevicePropOutcome(DeleteDevicePropResult(outcome.result()));
	else
		return DeleteDevicePropOutcome(outcome.error());
}

void IotClient::deleteDevicePropAsync(const DeleteDevicePropRequest& request, const DeleteDevicePropAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteDeviceProp(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteDevicePropOutcomeCallable IotClient::deleteDevicePropCallable(const DeleteDevicePropRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteDevicePropOutcome()>>(
			[this, request]()
			{
			return this->deleteDeviceProp(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteEdgeDriverOutcome IotClient::deleteEdgeDriver(const DeleteEdgeDriverRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteEdgeDriverOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteEdgeDriverOutcome(DeleteEdgeDriverResult(outcome.result()));
	else
		return DeleteEdgeDriverOutcome(outcome.error());
}

void IotClient::deleteEdgeDriverAsync(const DeleteEdgeDriverRequest& request, const DeleteEdgeDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteEdgeDriver(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteEdgeDriverOutcomeCallable IotClient::deleteEdgeDriverCallable(const DeleteEdgeDriverRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteEdgeDriverOutcome()>>(
			[this, request]()
			{
			return this->deleteEdgeDriver(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteEdgeDriverVersionOutcome IotClient::deleteEdgeDriverVersion(const DeleteEdgeDriverVersionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteEdgeDriverVersionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteEdgeDriverVersionOutcome(DeleteEdgeDriverVersionResult(outcome.result()));
	else
		return DeleteEdgeDriverVersionOutcome(outcome.error());
}

void IotClient::deleteEdgeDriverVersionAsync(const DeleteEdgeDriverVersionRequest& request, const DeleteEdgeDriverVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteEdgeDriverVersion(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteEdgeDriverVersionOutcomeCallable IotClient::deleteEdgeDriverVersionCallable(const DeleteEdgeDriverVersionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteEdgeDriverVersionOutcome()>>(
			[this, request]()
			{
			return this->deleteEdgeDriverVersion(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteEdgeInstanceOutcome IotClient::deleteEdgeInstance(const DeleteEdgeInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteEdgeInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteEdgeInstanceOutcome(DeleteEdgeInstanceResult(outcome.result()));
	else
		return DeleteEdgeInstanceOutcome(outcome.error());
}

void IotClient::deleteEdgeInstanceAsync(const DeleteEdgeInstanceRequest& request, const DeleteEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteEdgeInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteEdgeInstanceOutcomeCallable IotClient::deleteEdgeInstanceCallable(const DeleteEdgeInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteEdgeInstanceOutcome()>>(
			[this, request]()
			{
			return this->deleteEdgeInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteOTAFirmwareOutcome IotClient::deleteOTAFirmware(const DeleteOTAFirmwareRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteOTAFirmwareOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteOTAFirmwareOutcome(DeleteOTAFirmwareResult(outcome.result()));
	else
		return DeleteOTAFirmwareOutcome(outcome.error());
}

void IotClient::deleteOTAFirmwareAsync(const DeleteOTAFirmwareRequest& request, const DeleteOTAFirmwareAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteOTAFirmware(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteOTAFirmwareOutcomeCallable IotClient::deleteOTAFirmwareCallable(const DeleteOTAFirmwareRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteOTAFirmwareOutcome()>>(
			[this, request]()
			{
			return this->deleteOTAFirmware(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteProductOutcome IotClient::deleteProduct(const DeleteProductRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteProductOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteProductOutcome(DeleteProductResult(outcome.result()));
	else
		return DeleteProductOutcome(outcome.error());
}

void IotClient::deleteProductAsync(const DeleteProductRequest& request, const DeleteProductAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteProduct(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteProductOutcomeCallable IotClient::deleteProductCallable(const DeleteProductRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteProductOutcome()>>(
			[this, request]()
			{
			return this->deleteProduct(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteProductTagsOutcome IotClient::deleteProductTags(const DeleteProductTagsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteProductTagsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteProductTagsOutcome(DeleteProductTagsResult(outcome.result()));
	else
		return DeleteProductTagsOutcome(outcome.error());
}

void IotClient::deleteProductTagsAsync(const DeleteProductTagsRequest& request, const DeleteProductTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteProductTags(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteProductTagsOutcomeCallable IotClient::deleteProductTagsCallable(const DeleteProductTagsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteProductTagsOutcome()>>(
			[this, request]()
			{
			return this->deleteProductTags(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteProductTopicOutcome IotClient::deleteProductTopic(const DeleteProductTopicRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteProductTopicOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteProductTopicOutcome(DeleteProductTopicResult(outcome.result()));
	else
		return DeleteProductTopicOutcome(outcome.error());
}

void IotClient::deleteProductTopicAsync(const DeleteProductTopicRequest& request, const DeleteProductTopicAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteProductTopic(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteProductTopicOutcomeCallable IotClient::deleteProductTopicCallable(const DeleteProductTopicRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteProductTopicOutcome()>>(
			[this, request]()
			{
			return this->deleteProductTopic(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteRuleOutcome IotClient::deleteRule(const DeleteRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteRuleOutcome(DeleteRuleResult(outcome.result()));
	else
		return DeleteRuleOutcome(outcome.error());
}

void IotClient::deleteRuleAsync(const DeleteRuleRequest& request, const DeleteRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteRuleOutcomeCallable IotClient::deleteRuleCallable(const DeleteRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteRuleOutcome()>>(
			[this, request]()
			{
			return this->deleteRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteRuleActionOutcome IotClient::deleteRuleAction(const DeleteRuleActionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteRuleActionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteRuleActionOutcome(DeleteRuleActionResult(outcome.result()));
	else
		return DeleteRuleActionOutcome(outcome.error());
}

void IotClient::deleteRuleActionAsync(const DeleteRuleActionRequest& request, const DeleteRuleActionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteRuleAction(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteRuleActionOutcomeCallable IotClient::deleteRuleActionCallable(const DeleteRuleActionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteRuleActionOutcome()>>(
			[this, request]()
			{
			return this->deleteRuleAction(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteSceneRuleOutcome IotClient::deleteSceneRule(const DeleteSceneRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteSceneRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteSceneRuleOutcome(DeleteSceneRuleResult(outcome.result()));
	else
		return DeleteSceneRuleOutcome(outcome.error());
}

void IotClient::deleteSceneRuleAsync(const DeleteSceneRuleRequest& request, const DeleteSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteSceneRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteSceneRuleOutcomeCallable IotClient::deleteSceneRuleCallable(const DeleteSceneRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteSceneRuleOutcome()>>(
			[this, request]()
			{
			return this->deleteSceneRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteSubscribeRelationOutcome IotClient::deleteSubscribeRelation(const DeleteSubscribeRelationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteSubscribeRelationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteSubscribeRelationOutcome(DeleteSubscribeRelationResult(outcome.result()));
	else
		return DeleteSubscribeRelationOutcome(outcome.error());
}

void IotClient::deleteSubscribeRelationAsync(const DeleteSubscribeRelationRequest& request, const DeleteSubscribeRelationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteSubscribeRelation(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteSubscribeRelationOutcomeCallable IotClient::deleteSubscribeRelationCallable(const DeleteSubscribeRelationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteSubscribeRelationOutcome()>>(
			[this, request]()
			{
			return this->deleteSubscribeRelation(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteThingModelOutcome IotClient::deleteThingModel(const DeleteThingModelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteThingModelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteThingModelOutcome(DeleteThingModelResult(outcome.result()));
	else
		return DeleteThingModelOutcome(outcome.error());
}

void IotClient::deleteThingModelAsync(const DeleteThingModelRequest& request, const DeleteThingModelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteThingModel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteThingModelOutcomeCallable IotClient::deleteThingModelCallable(const DeleteThingModelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteThingModelOutcome()>>(
			[this, request]()
			{
			return this->deleteThingModel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DeleteTopicRouteTableOutcome IotClient::deleteTopicRouteTable(const DeleteTopicRouteTableRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteTopicRouteTableOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteTopicRouteTableOutcome(DeleteTopicRouteTableResult(outcome.result()));
	else
		return DeleteTopicRouteTableOutcome(outcome.error());
}

void IotClient::deleteTopicRouteTableAsync(const DeleteTopicRouteTableRequest& request, const DeleteTopicRouteTableAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteTopicRouteTable(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DeleteTopicRouteTableOutcomeCallable IotClient::deleteTopicRouteTableCallable(const DeleteTopicRouteTableRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteTopicRouteTableOutcome()>>(
			[this, request]()
			{
			return this->deleteTopicRouteTable(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DisableSceneRuleOutcome IotClient::disableSceneRule(const DisableSceneRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DisableSceneRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DisableSceneRuleOutcome(DisableSceneRuleResult(outcome.result()));
	else
		return DisableSceneRuleOutcome(outcome.error());
}

void IotClient::disableSceneRuleAsync(const DisableSceneRuleRequest& request, const DisableSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, disableSceneRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DisableSceneRuleOutcomeCallable IotClient::disableSceneRuleCallable(const DisableSceneRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DisableSceneRuleOutcome()>>(
			[this, request]()
			{
			return this->disableSceneRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::DisableThingOutcome IotClient::disableThing(const DisableThingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DisableThingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DisableThingOutcome(DisableThingResult(outcome.result()));
	else
		return DisableThingOutcome(outcome.error());
}

void IotClient::disableThingAsync(const DisableThingRequest& request, const DisableThingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, disableThing(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::DisableThingOutcomeCallable IotClient::disableThingCallable(const DisableThingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DisableThingOutcome()>>(
			[this, request]()
			{
			return this->disableThing(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::EnableSceneRuleOutcome IotClient::enableSceneRule(const EnableSceneRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return EnableSceneRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return EnableSceneRuleOutcome(EnableSceneRuleResult(outcome.result()));
	else
		return EnableSceneRuleOutcome(outcome.error());
}

void IotClient::enableSceneRuleAsync(const EnableSceneRuleRequest& request, const EnableSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, enableSceneRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::EnableSceneRuleOutcomeCallable IotClient::enableSceneRuleCallable(const EnableSceneRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<EnableSceneRuleOutcome()>>(
			[this, request]()
			{
			return this->enableSceneRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::EnableThingOutcome IotClient::enableThing(const EnableThingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return EnableThingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return EnableThingOutcome(EnableThingResult(outcome.result()));
	else
		return EnableThingOutcome(outcome.error());
}

void IotClient::enableThingAsync(const EnableThingRequest& request, const EnableThingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, enableThing(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::EnableThingOutcomeCallable IotClient::enableThingCallable(const EnableThingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<EnableThingOutcome()>>(
			[this, request]()
			{
			return this->enableThing(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GenerateOTAUploadURLOutcome IotClient::generateOTAUploadURL(const GenerateOTAUploadURLRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GenerateOTAUploadURLOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GenerateOTAUploadURLOutcome(GenerateOTAUploadURLResult(outcome.result()));
	else
		return GenerateOTAUploadURLOutcome(outcome.error());
}

void IotClient::generateOTAUploadURLAsync(const GenerateOTAUploadURLRequest& request, const GenerateOTAUploadURLAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, generateOTAUploadURL(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GenerateOTAUploadURLOutcomeCallable IotClient::generateOTAUploadURLCallable(const GenerateOTAUploadURLRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GenerateOTAUploadURLOutcome()>>(
			[this, request]()
			{
			return this->generateOTAUploadURL(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetDataAPIServiceDetailOutcome IotClient::getDataAPIServiceDetail(const GetDataAPIServiceDetailRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDataAPIServiceDetailOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDataAPIServiceDetailOutcome(GetDataAPIServiceDetailResult(outcome.result()));
	else
		return GetDataAPIServiceDetailOutcome(outcome.error());
}

void IotClient::getDataAPIServiceDetailAsync(const GetDataAPIServiceDetailRequest& request, const GetDataAPIServiceDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDataAPIServiceDetail(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetDataAPIServiceDetailOutcomeCallable IotClient::getDataAPIServiceDetailCallable(const GetDataAPIServiceDetailRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDataAPIServiceDetailOutcome()>>(
			[this, request]()
			{
			return this->getDataAPIServiceDetail(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetDeviceShadowOutcome IotClient::getDeviceShadow(const GetDeviceShadowRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDeviceShadowOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDeviceShadowOutcome(GetDeviceShadowResult(outcome.result()));
	else
		return GetDeviceShadowOutcome(outcome.error());
}

void IotClient::getDeviceShadowAsync(const GetDeviceShadowRequest& request, const GetDeviceShadowAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDeviceShadow(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetDeviceShadowOutcomeCallable IotClient::getDeviceShadowCallable(const GetDeviceShadowRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDeviceShadowOutcome()>>(
			[this, request]()
			{
			return this->getDeviceShadow(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetDeviceStatusOutcome IotClient::getDeviceStatus(const GetDeviceStatusRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDeviceStatusOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDeviceStatusOutcome(GetDeviceStatusResult(outcome.result()));
	else
		return GetDeviceStatusOutcome(outcome.error());
}

void IotClient::getDeviceStatusAsync(const GetDeviceStatusRequest& request, const GetDeviceStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDeviceStatus(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetDeviceStatusOutcomeCallable IotClient::getDeviceStatusCallable(const GetDeviceStatusRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDeviceStatusOutcome()>>(
			[this, request]()
			{
			return this->getDeviceStatus(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetEdgeDriverVersionOutcome IotClient::getEdgeDriverVersion(const GetEdgeDriverVersionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetEdgeDriverVersionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetEdgeDriverVersionOutcome(GetEdgeDriverVersionResult(outcome.result()));
	else
		return GetEdgeDriverVersionOutcome(outcome.error());
}

void IotClient::getEdgeDriverVersionAsync(const GetEdgeDriverVersionRequest& request, const GetEdgeDriverVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getEdgeDriverVersion(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetEdgeDriverVersionOutcomeCallable IotClient::getEdgeDriverVersionCallable(const GetEdgeDriverVersionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetEdgeDriverVersionOutcome()>>(
			[this, request]()
			{
			return this->getEdgeDriverVersion(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetEdgeInstanceOutcome IotClient::getEdgeInstance(const GetEdgeInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetEdgeInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetEdgeInstanceOutcome(GetEdgeInstanceResult(outcome.result()));
	else
		return GetEdgeInstanceOutcome(outcome.error());
}

void IotClient::getEdgeInstanceAsync(const GetEdgeInstanceRequest& request, const GetEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getEdgeInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetEdgeInstanceOutcomeCallable IotClient::getEdgeInstanceCallable(const GetEdgeInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetEdgeInstanceOutcome()>>(
			[this, request]()
			{
			return this->getEdgeInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetEdgeInstanceDeploymentOutcome IotClient::getEdgeInstanceDeployment(const GetEdgeInstanceDeploymentRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetEdgeInstanceDeploymentOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetEdgeInstanceDeploymentOutcome(GetEdgeInstanceDeploymentResult(outcome.result()));
	else
		return GetEdgeInstanceDeploymentOutcome(outcome.error());
}

void IotClient::getEdgeInstanceDeploymentAsync(const GetEdgeInstanceDeploymentRequest& request, const GetEdgeInstanceDeploymentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getEdgeInstanceDeployment(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetEdgeInstanceDeploymentOutcomeCallable IotClient::getEdgeInstanceDeploymentCallable(const GetEdgeInstanceDeploymentRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetEdgeInstanceDeploymentOutcome()>>(
			[this, request]()
			{
			return this->getEdgeInstanceDeployment(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetGatewayBySubDeviceOutcome IotClient::getGatewayBySubDevice(const GetGatewayBySubDeviceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetGatewayBySubDeviceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetGatewayBySubDeviceOutcome(GetGatewayBySubDeviceResult(outcome.result()));
	else
		return GetGatewayBySubDeviceOutcome(outcome.error());
}

void IotClient::getGatewayBySubDeviceAsync(const GetGatewayBySubDeviceRequest& request, const GetGatewayBySubDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getGatewayBySubDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetGatewayBySubDeviceOutcomeCallable IotClient::getGatewayBySubDeviceCallable(const GetGatewayBySubDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetGatewayBySubDeviceOutcome()>>(
			[this, request]()
			{
			return this->getGatewayBySubDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetLoraNodesTaskOutcome IotClient::getLoraNodesTask(const GetLoraNodesTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetLoraNodesTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetLoraNodesTaskOutcome(GetLoraNodesTaskResult(outcome.result()));
	else
		return GetLoraNodesTaskOutcome(outcome.error());
}

void IotClient::getLoraNodesTaskAsync(const GetLoraNodesTaskRequest& request, const GetLoraNodesTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getLoraNodesTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetLoraNodesTaskOutcomeCallable IotClient::getLoraNodesTaskCallable(const GetLoraNodesTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetLoraNodesTaskOutcome()>>(
			[this, request]()
			{
			return this->getLoraNodesTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetNodesAddingTaskOutcome IotClient::getNodesAddingTask(const GetNodesAddingTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetNodesAddingTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetNodesAddingTaskOutcome(GetNodesAddingTaskResult(outcome.result()));
	else
		return GetNodesAddingTaskOutcome(outcome.error());
}

void IotClient::getNodesAddingTaskAsync(const GetNodesAddingTaskRequest& request, const GetNodesAddingTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getNodesAddingTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetNodesAddingTaskOutcomeCallable IotClient::getNodesAddingTaskCallable(const GetNodesAddingTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetNodesAddingTaskOutcome()>>(
			[this, request]()
			{
			return this->getNodesAddingTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetRuleOutcome IotClient::getRule(const GetRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRuleOutcome(GetRuleResult(outcome.result()));
	else
		return GetRuleOutcome(outcome.error());
}

void IotClient::getRuleAsync(const GetRuleRequest& request, const GetRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetRuleOutcomeCallable IotClient::getRuleCallable(const GetRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRuleOutcome()>>(
			[this, request]()
			{
			return this->getRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetRuleActionOutcome IotClient::getRuleAction(const GetRuleActionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetRuleActionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetRuleActionOutcome(GetRuleActionResult(outcome.result()));
	else
		return GetRuleActionOutcome(outcome.error());
}

void IotClient::getRuleActionAsync(const GetRuleActionRequest& request, const GetRuleActionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getRuleAction(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetRuleActionOutcomeCallable IotClient::getRuleActionCallable(const GetRuleActionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetRuleActionOutcome()>>(
			[this, request]()
			{
			return this->getRuleAction(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetSceneRuleOutcome IotClient::getSceneRule(const GetSceneRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetSceneRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetSceneRuleOutcome(GetSceneRuleResult(outcome.result()));
	else
		return GetSceneRuleOutcome(outcome.error());
}

void IotClient::getSceneRuleAsync(const GetSceneRuleRequest& request, const GetSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getSceneRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetSceneRuleOutcomeCallable IotClient::getSceneRuleCallable(const GetSceneRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetSceneRuleOutcome()>>(
			[this, request]()
			{
			return this->getSceneRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetThingModelTslOutcome IotClient::getThingModelTsl(const GetThingModelTslRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetThingModelTslOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetThingModelTslOutcome(GetThingModelTslResult(outcome.result()));
	else
		return GetThingModelTslOutcome(outcome.error());
}

void IotClient::getThingModelTslAsync(const GetThingModelTslRequest& request, const GetThingModelTslAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getThingModelTsl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetThingModelTslOutcomeCallable IotClient::getThingModelTslCallable(const GetThingModelTslRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetThingModelTslOutcome()>>(
			[this, request]()
			{
			return this->getThingModelTsl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetThingTemplateOutcome IotClient::getThingTemplate(const GetThingTemplateRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetThingTemplateOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetThingTemplateOutcome(GetThingTemplateResult(outcome.result()));
	else
		return GetThingTemplateOutcome(outcome.error());
}

void IotClient::getThingTemplateAsync(const GetThingTemplateRequest& request, const GetThingTemplateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getThingTemplate(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetThingTemplateOutcomeCallable IotClient::getThingTemplateCallable(const GetThingTemplateRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetThingTemplateOutcome()>>(
			[this, request]()
			{
			return this->getThingTemplate(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::GetThingTopoOutcome IotClient::getThingTopo(const GetThingTopoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetThingTopoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetThingTopoOutcome(GetThingTopoResult(outcome.result()));
	else
		return GetThingTopoOutcome(outcome.error());
}

void IotClient::getThingTopoAsync(const GetThingTopoRequest& request, const GetThingTopoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getThingTopo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::GetThingTopoOutcomeCallable IotClient::getThingTopoCallable(const GetThingTopoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetThingTopoOutcome()>>(
			[this, request]()
			{
			return this->getThingTopo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ImportThingModelTslOutcome IotClient::importThingModelTsl(const ImportThingModelTslRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ImportThingModelTslOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ImportThingModelTslOutcome(ImportThingModelTslResult(outcome.result()));
	else
		return ImportThingModelTslOutcome(outcome.error());
}

void IotClient::importThingModelTslAsync(const ImportThingModelTslRequest& request, const ImportThingModelTslAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, importThingModelTsl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ImportThingModelTslOutcomeCallable IotClient::importThingModelTslCallable(const ImportThingModelTslRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ImportThingModelTslOutcome()>>(
			[this, request]()
			{
			return this->importThingModelTsl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::InvokeDataAPIServiceOutcome IotClient::invokeDataAPIService(const InvokeDataAPIServiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return InvokeDataAPIServiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return InvokeDataAPIServiceOutcome(InvokeDataAPIServiceResult(outcome.result()));
	else
		return InvokeDataAPIServiceOutcome(outcome.error());
}

void IotClient::invokeDataAPIServiceAsync(const InvokeDataAPIServiceRequest& request, const InvokeDataAPIServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, invokeDataAPIService(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::InvokeDataAPIServiceOutcomeCallable IotClient::invokeDataAPIServiceCallable(const InvokeDataAPIServiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<InvokeDataAPIServiceOutcome()>>(
			[this, request]()
			{
			return this->invokeDataAPIService(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::InvokeThingServiceOutcome IotClient::invokeThingService(const InvokeThingServiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return InvokeThingServiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return InvokeThingServiceOutcome(InvokeThingServiceResult(outcome.result()));
	else
		return InvokeThingServiceOutcome(outcome.error());
}

void IotClient::invokeThingServiceAsync(const InvokeThingServiceRequest& request, const InvokeThingServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, invokeThingService(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::InvokeThingServiceOutcomeCallable IotClient::invokeThingServiceCallable(const InvokeThingServiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<InvokeThingServiceOutcome()>>(
			[this, request]()
			{
			return this->invokeThingService(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::InvokeThingsServiceOutcome IotClient::invokeThingsService(const InvokeThingsServiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return InvokeThingsServiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return InvokeThingsServiceOutcome(InvokeThingsServiceResult(outcome.result()));
	else
		return InvokeThingsServiceOutcome(outcome.error());
}

void IotClient::invokeThingsServiceAsync(const InvokeThingsServiceRequest& request, const InvokeThingsServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, invokeThingsService(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::InvokeThingsServiceOutcomeCallable IotClient::invokeThingsServiceCallable(const InvokeThingsServiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<InvokeThingsServiceOutcome()>>(
			[this, request]()
			{
			return this->invokeThingsService(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ListOTAFirmwareOutcome IotClient::listOTAFirmware(const ListOTAFirmwareRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListOTAFirmwareOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListOTAFirmwareOutcome(ListOTAFirmwareResult(outcome.result()));
	else
		return ListOTAFirmwareOutcome(outcome.error());
}

void IotClient::listOTAFirmwareAsync(const ListOTAFirmwareRequest& request, const ListOTAFirmwareAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listOTAFirmware(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ListOTAFirmwareOutcomeCallable IotClient::listOTAFirmwareCallable(const ListOTAFirmwareRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListOTAFirmwareOutcome()>>(
			[this, request]()
			{
			return this->listOTAFirmware(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ListOTAJobByDeviceOutcome IotClient::listOTAJobByDevice(const ListOTAJobByDeviceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListOTAJobByDeviceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListOTAJobByDeviceOutcome(ListOTAJobByDeviceResult(outcome.result()));
	else
		return ListOTAJobByDeviceOutcome(outcome.error());
}

void IotClient::listOTAJobByDeviceAsync(const ListOTAJobByDeviceRequest& request, const ListOTAJobByDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listOTAJobByDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ListOTAJobByDeviceOutcomeCallable IotClient::listOTAJobByDeviceCallable(const ListOTAJobByDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListOTAJobByDeviceOutcome()>>(
			[this, request]()
			{
			return this->listOTAJobByDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ListOTAJobByFirmwareOutcome IotClient::listOTAJobByFirmware(const ListOTAJobByFirmwareRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListOTAJobByFirmwareOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListOTAJobByFirmwareOutcome(ListOTAJobByFirmwareResult(outcome.result()));
	else
		return ListOTAJobByFirmwareOutcome(outcome.error());
}

void IotClient::listOTAJobByFirmwareAsync(const ListOTAJobByFirmwareRequest& request, const ListOTAJobByFirmwareAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listOTAJobByFirmware(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ListOTAJobByFirmwareOutcomeCallable IotClient::listOTAJobByFirmwareCallable(const ListOTAJobByFirmwareRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListOTAJobByFirmwareOutcome()>>(
			[this, request]()
			{
			return this->listOTAJobByFirmware(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ListOTATaskByJobOutcome IotClient::listOTATaskByJob(const ListOTATaskByJobRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListOTATaskByJobOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListOTATaskByJobOutcome(ListOTATaskByJobResult(outcome.result()));
	else
		return ListOTATaskByJobOutcome(outcome.error());
}

void IotClient::listOTATaskByJobAsync(const ListOTATaskByJobRequest& request, const ListOTATaskByJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listOTATaskByJob(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ListOTATaskByJobOutcomeCallable IotClient::listOTATaskByJobCallable(const ListOTATaskByJobRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListOTATaskByJobOutcome()>>(
			[this, request]()
			{
			return this->listOTATaskByJob(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ListProductByTagsOutcome IotClient::listProductByTags(const ListProductByTagsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListProductByTagsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListProductByTagsOutcome(ListProductByTagsResult(outcome.result()));
	else
		return ListProductByTagsOutcome(outcome.error());
}

void IotClient::listProductByTagsAsync(const ListProductByTagsRequest& request, const ListProductByTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listProductByTags(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ListProductByTagsOutcomeCallable IotClient::listProductByTagsCallable(const ListProductByTagsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListProductByTagsOutcome()>>(
			[this, request]()
			{
			return this->listProductByTags(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ListProductTagsOutcome IotClient::listProductTags(const ListProductTagsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListProductTagsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListProductTagsOutcome(ListProductTagsResult(outcome.result()));
	else
		return ListProductTagsOutcome(outcome.error());
}

void IotClient::listProductTagsAsync(const ListProductTagsRequest& request, const ListProductTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listProductTags(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ListProductTagsOutcomeCallable IotClient::listProductTagsCallable(const ListProductTagsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListProductTagsOutcome()>>(
			[this, request]()
			{
			return this->listProductTags(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ListRuleOutcome IotClient::listRule(const ListRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListRuleOutcome(ListRuleResult(outcome.result()));
	else
		return ListRuleOutcome(outcome.error());
}

void IotClient::listRuleAsync(const ListRuleRequest& request, const ListRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ListRuleOutcomeCallable IotClient::listRuleCallable(const ListRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListRuleOutcome()>>(
			[this, request]()
			{
			return this->listRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ListRuleActionsOutcome IotClient::listRuleActions(const ListRuleActionsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListRuleActionsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListRuleActionsOutcome(ListRuleActionsResult(outcome.result()));
	else
		return ListRuleActionsOutcome(outcome.error());
}

void IotClient::listRuleActionsAsync(const ListRuleActionsRequest& request, const ListRuleActionsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listRuleActions(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ListRuleActionsOutcomeCallable IotClient::listRuleActionsCallable(const ListRuleActionsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListRuleActionsOutcome()>>(
			[this, request]()
			{
			return this->listRuleActions(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ListThingModelVersionOutcome IotClient::listThingModelVersion(const ListThingModelVersionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListThingModelVersionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListThingModelVersionOutcome(ListThingModelVersionResult(outcome.result()));
	else
		return ListThingModelVersionOutcome(outcome.error());
}

void IotClient::listThingModelVersionAsync(const ListThingModelVersionRequest& request, const ListThingModelVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listThingModelVersion(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ListThingModelVersionOutcomeCallable IotClient::listThingModelVersionCallable(const ListThingModelVersionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListThingModelVersionOutcome()>>(
			[this, request]()
			{
			return this->listThingModelVersion(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ListThingTemplatesOutcome IotClient::listThingTemplates(const ListThingTemplatesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListThingTemplatesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListThingTemplatesOutcome(ListThingTemplatesResult(outcome.result()));
	else
		return ListThingTemplatesOutcome(outcome.error());
}

void IotClient::listThingTemplatesAsync(const ListThingTemplatesRequest& request, const ListThingTemplatesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listThingTemplates(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ListThingTemplatesOutcomeCallable IotClient::listThingTemplatesCallable(const ListThingTemplatesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListThingTemplatesOutcome()>>(
			[this, request]()
			{
			return this->listThingTemplates(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::NotifyAddThingTopoOutcome IotClient::notifyAddThingTopo(const NotifyAddThingTopoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return NotifyAddThingTopoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return NotifyAddThingTopoOutcome(NotifyAddThingTopoResult(outcome.result()));
	else
		return NotifyAddThingTopoOutcome(outcome.error());
}

void IotClient::notifyAddThingTopoAsync(const NotifyAddThingTopoRequest& request, const NotifyAddThingTopoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, notifyAddThingTopo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::NotifyAddThingTopoOutcomeCallable IotClient::notifyAddThingTopoCallable(const NotifyAddThingTopoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<NotifyAddThingTopoOutcome()>>(
			[this, request]()
			{
			return this->notifyAddThingTopo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::PubOutcome IotClient::pub(const PubRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return PubOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return PubOutcome(PubResult(outcome.result()));
	else
		return PubOutcome(outcome.error());
}

void IotClient::pubAsync(const PubRequest& request, const PubAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, pub(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::PubOutcomeCallable IotClient::pubCallable(const PubRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<PubOutcome()>>(
			[this, request]()
			{
			return this->pub(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::PubBroadcastOutcome IotClient::pubBroadcast(const PubBroadcastRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return PubBroadcastOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return PubBroadcastOutcome(PubBroadcastResult(outcome.result()));
	else
		return PubBroadcastOutcome(outcome.error());
}

void IotClient::pubBroadcastAsync(const PubBroadcastRequest& request, const PubBroadcastAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, pubBroadcast(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::PubBroadcastOutcomeCallable IotClient::pubBroadcastCallable(const PubBroadcastRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<PubBroadcastOutcome()>>(
			[this, request]()
			{
			return this->pubBroadcast(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::PublishThingModelOutcome IotClient::publishThingModel(const PublishThingModelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return PublishThingModelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return PublishThingModelOutcome(PublishThingModelResult(outcome.result()));
	else
		return PublishThingModelOutcome(outcome.error());
}

void IotClient::publishThingModelAsync(const PublishThingModelRequest& request, const PublishThingModelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, publishThingModel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::PublishThingModelOutcomeCallable IotClient::publishThingModelCallable(const PublishThingModelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<PublishThingModelOutcome()>>(
			[this, request]()
			{
			return this->publishThingModel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryAppDeviceListOutcome IotClient::queryAppDeviceList(const QueryAppDeviceListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryAppDeviceListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryAppDeviceListOutcome(QueryAppDeviceListResult(outcome.result()));
	else
		return QueryAppDeviceListOutcome(outcome.error());
}

void IotClient::queryAppDeviceListAsync(const QueryAppDeviceListRequest& request, const QueryAppDeviceListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryAppDeviceList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryAppDeviceListOutcomeCallable IotClient::queryAppDeviceListCallable(const QueryAppDeviceListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryAppDeviceListOutcome()>>(
			[this, request]()
			{
			return this->queryAppDeviceList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryBatchRegisterDeviceStatusOutcome IotClient::queryBatchRegisterDeviceStatus(const QueryBatchRegisterDeviceStatusRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryBatchRegisterDeviceStatusOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryBatchRegisterDeviceStatusOutcome(QueryBatchRegisterDeviceStatusResult(outcome.result()));
	else
		return QueryBatchRegisterDeviceStatusOutcome(outcome.error());
}

void IotClient::queryBatchRegisterDeviceStatusAsync(const QueryBatchRegisterDeviceStatusRequest& request, const QueryBatchRegisterDeviceStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryBatchRegisterDeviceStatus(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryBatchRegisterDeviceStatusOutcomeCallable IotClient::queryBatchRegisterDeviceStatusCallable(const QueryBatchRegisterDeviceStatusRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryBatchRegisterDeviceStatusOutcome()>>(
			[this, request]()
			{
			return this->queryBatchRegisterDeviceStatus(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryCertUrlByApplyIdOutcome IotClient::queryCertUrlByApplyId(const QueryCertUrlByApplyIdRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryCertUrlByApplyIdOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryCertUrlByApplyIdOutcome(QueryCertUrlByApplyIdResult(outcome.result()));
	else
		return QueryCertUrlByApplyIdOutcome(outcome.error());
}

void IotClient::queryCertUrlByApplyIdAsync(const QueryCertUrlByApplyIdRequest& request, const QueryCertUrlByApplyIdAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryCertUrlByApplyId(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryCertUrlByApplyIdOutcomeCallable IotClient::queryCertUrlByApplyIdCallable(const QueryCertUrlByApplyIdRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryCertUrlByApplyIdOutcome()>>(
			[this, request]()
			{
			return this->queryCertUrlByApplyId(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryConsumerGroupByGroupIdOutcome IotClient::queryConsumerGroupByGroupId(const QueryConsumerGroupByGroupIdRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryConsumerGroupByGroupIdOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryConsumerGroupByGroupIdOutcome(QueryConsumerGroupByGroupIdResult(outcome.result()));
	else
		return QueryConsumerGroupByGroupIdOutcome(outcome.error());
}

void IotClient::queryConsumerGroupByGroupIdAsync(const QueryConsumerGroupByGroupIdRequest& request, const QueryConsumerGroupByGroupIdAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryConsumerGroupByGroupId(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryConsumerGroupByGroupIdOutcomeCallable IotClient::queryConsumerGroupByGroupIdCallable(const QueryConsumerGroupByGroupIdRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryConsumerGroupByGroupIdOutcome()>>(
			[this, request]()
			{
			return this->queryConsumerGroupByGroupId(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryConsumerGroupListOutcome IotClient::queryConsumerGroupList(const QueryConsumerGroupListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryConsumerGroupListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryConsumerGroupListOutcome(QueryConsumerGroupListResult(outcome.result()));
	else
		return QueryConsumerGroupListOutcome(outcome.error());
}

void IotClient::queryConsumerGroupListAsync(const QueryConsumerGroupListRequest& request, const QueryConsumerGroupListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryConsumerGroupList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryConsumerGroupListOutcomeCallable IotClient::queryConsumerGroupListCallable(const QueryConsumerGroupListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryConsumerGroupListOutcome()>>(
			[this, request]()
			{
			return this->queryConsumerGroupList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryConsumerGroupStatusOutcome IotClient::queryConsumerGroupStatus(const QueryConsumerGroupStatusRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryConsumerGroupStatusOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryConsumerGroupStatusOutcome(QueryConsumerGroupStatusResult(outcome.result()));
	else
		return QueryConsumerGroupStatusOutcome(outcome.error());
}

void IotClient::queryConsumerGroupStatusAsync(const QueryConsumerGroupStatusRequest& request, const QueryConsumerGroupStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryConsumerGroupStatus(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryConsumerGroupStatusOutcomeCallable IotClient::queryConsumerGroupStatusCallable(const QueryConsumerGroupStatusRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryConsumerGroupStatusOutcome()>>(
			[this, request]()
			{
			return this->queryConsumerGroupStatus(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDetailSceneRuleLogOutcome IotClient::queryDetailSceneRuleLog(const QueryDetailSceneRuleLogRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDetailSceneRuleLogOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDetailSceneRuleLogOutcome(QueryDetailSceneRuleLogResult(outcome.result()));
	else
		return QueryDetailSceneRuleLogOutcome(outcome.error());
}

void IotClient::queryDetailSceneRuleLogAsync(const QueryDetailSceneRuleLogRequest& request, const QueryDetailSceneRuleLogAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDetailSceneRuleLog(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDetailSceneRuleLogOutcomeCallable IotClient::queryDetailSceneRuleLogCallable(const QueryDetailSceneRuleLogRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDetailSceneRuleLogOutcome()>>(
			[this, request]()
			{
			return this->queryDetailSceneRuleLog(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceOutcome IotClient::queryDevice(const QueryDeviceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceOutcome(QueryDeviceResult(outcome.result()));
	else
		return QueryDeviceOutcome(outcome.error());
}

void IotClient::queryDeviceAsync(const QueryDeviceRequest& request, const QueryDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceOutcomeCallable IotClient::queryDeviceCallable(const QueryDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceOutcome()>>(
			[this, request]()
			{
			return this->queryDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceByStatusOutcome IotClient::queryDeviceByStatus(const QueryDeviceByStatusRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceByStatusOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceByStatusOutcome(QueryDeviceByStatusResult(outcome.result()));
	else
		return QueryDeviceByStatusOutcome(outcome.error());
}

void IotClient::queryDeviceByStatusAsync(const QueryDeviceByStatusRequest& request, const QueryDeviceByStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceByStatus(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceByStatusOutcomeCallable IotClient::queryDeviceByStatusCallable(const QueryDeviceByStatusRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceByStatusOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceByStatus(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceByTagsOutcome IotClient::queryDeviceByTags(const QueryDeviceByTagsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceByTagsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceByTagsOutcome(QueryDeviceByTagsResult(outcome.result()));
	else
		return QueryDeviceByTagsOutcome(outcome.error());
}

void IotClient::queryDeviceByTagsAsync(const QueryDeviceByTagsRequest& request, const QueryDeviceByTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceByTags(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceByTagsOutcomeCallable IotClient::queryDeviceByTagsCallable(const QueryDeviceByTagsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceByTagsOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceByTags(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceCertOutcome IotClient::queryDeviceCert(const QueryDeviceCertRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceCertOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceCertOutcome(QueryDeviceCertResult(outcome.result()));
	else
		return QueryDeviceCertOutcome(outcome.error());
}

void IotClient::queryDeviceCertAsync(const QueryDeviceCertRequest& request, const QueryDeviceCertAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceCert(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceCertOutcomeCallable IotClient::queryDeviceCertCallable(const QueryDeviceCertRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceCertOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceCert(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceDesiredPropertyOutcome IotClient::queryDeviceDesiredProperty(const QueryDeviceDesiredPropertyRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceDesiredPropertyOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceDesiredPropertyOutcome(QueryDeviceDesiredPropertyResult(outcome.result()));
	else
		return QueryDeviceDesiredPropertyOutcome(outcome.error());
}

void IotClient::queryDeviceDesiredPropertyAsync(const QueryDeviceDesiredPropertyRequest& request, const QueryDeviceDesiredPropertyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceDesiredProperty(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceDesiredPropertyOutcomeCallable IotClient::queryDeviceDesiredPropertyCallable(const QueryDeviceDesiredPropertyRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceDesiredPropertyOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceDesiredProperty(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceDetailOutcome IotClient::queryDeviceDetail(const QueryDeviceDetailRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceDetailOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceDetailOutcome(QueryDeviceDetailResult(outcome.result()));
	else
		return QueryDeviceDetailOutcome(outcome.error());
}

void IotClient::queryDeviceDetailAsync(const QueryDeviceDetailRequest& request, const QueryDeviceDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceDetail(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceDetailOutcomeCallable IotClient::queryDeviceDetailCallable(const QueryDeviceDetailRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceDetailOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceDetail(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceEventDataOutcome IotClient::queryDeviceEventData(const QueryDeviceEventDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceEventDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceEventDataOutcome(QueryDeviceEventDataResult(outcome.result()));
	else
		return QueryDeviceEventDataOutcome(outcome.error());
}

void IotClient::queryDeviceEventDataAsync(const QueryDeviceEventDataRequest& request, const QueryDeviceEventDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceEventData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceEventDataOutcomeCallable IotClient::queryDeviceEventDataCallable(const QueryDeviceEventDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceEventDataOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceEventData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceFileOutcome IotClient::queryDeviceFile(const QueryDeviceFileRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceFileOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceFileOutcome(QueryDeviceFileResult(outcome.result()));
	else
		return QueryDeviceFileOutcome(outcome.error());
}

void IotClient::queryDeviceFileAsync(const QueryDeviceFileRequest& request, const QueryDeviceFileAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceFile(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceFileOutcomeCallable IotClient::queryDeviceFileCallable(const QueryDeviceFileRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceFileOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceFile(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceFileListOutcome IotClient::queryDeviceFileList(const QueryDeviceFileListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceFileListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceFileListOutcome(QueryDeviceFileListResult(outcome.result()));
	else
		return QueryDeviceFileListOutcome(outcome.error());
}

void IotClient::queryDeviceFileListAsync(const QueryDeviceFileListRequest& request, const QueryDeviceFileListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceFileList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceFileListOutcomeCallable IotClient::queryDeviceFileListCallable(const QueryDeviceFileListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceFileListOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceFileList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceGroupByDeviceOutcome IotClient::queryDeviceGroupByDevice(const QueryDeviceGroupByDeviceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceGroupByDeviceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceGroupByDeviceOutcome(QueryDeviceGroupByDeviceResult(outcome.result()));
	else
		return QueryDeviceGroupByDeviceOutcome(outcome.error());
}

void IotClient::queryDeviceGroupByDeviceAsync(const QueryDeviceGroupByDeviceRequest& request, const QueryDeviceGroupByDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceGroupByDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceGroupByDeviceOutcomeCallable IotClient::queryDeviceGroupByDeviceCallable(const QueryDeviceGroupByDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceGroupByDeviceOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceGroupByDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceGroupByTagsOutcome IotClient::queryDeviceGroupByTags(const QueryDeviceGroupByTagsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceGroupByTagsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceGroupByTagsOutcome(QueryDeviceGroupByTagsResult(outcome.result()));
	else
		return QueryDeviceGroupByTagsOutcome(outcome.error());
}

void IotClient::queryDeviceGroupByTagsAsync(const QueryDeviceGroupByTagsRequest& request, const QueryDeviceGroupByTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceGroupByTags(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceGroupByTagsOutcomeCallable IotClient::queryDeviceGroupByTagsCallable(const QueryDeviceGroupByTagsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceGroupByTagsOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceGroupByTags(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceGroupInfoOutcome IotClient::queryDeviceGroupInfo(const QueryDeviceGroupInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceGroupInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceGroupInfoOutcome(QueryDeviceGroupInfoResult(outcome.result()));
	else
		return QueryDeviceGroupInfoOutcome(outcome.error());
}

void IotClient::queryDeviceGroupInfoAsync(const QueryDeviceGroupInfoRequest& request, const QueryDeviceGroupInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceGroupInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceGroupInfoOutcomeCallable IotClient::queryDeviceGroupInfoCallable(const QueryDeviceGroupInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceGroupInfoOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceGroupInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceGroupListOutcome IotClient::queryDeviceGroupList(const QueryDeviceGroupListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceGroupListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceGroupListOutcome(QueryDeviceGroupListResult(outcome.result()));
	else
		return QueryDeviceGroupListOutcome(outcome.error());
}

void IotClient::queryDeviceGroupListAsync(const QueryDeviceGroupListRequest& request, const QueryDeviceGroupListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceGroupList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceGroupListOutcomeCallable IotClient::queryDeviceGroupListCallable(const QueryDeviceGroupListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceGroupListOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceGroupList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceGroupTagListOutcome IotClient::queryDeviceGroupTagList(const QueryDeviceGroupTagListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceGroupTagListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceGroupTagListOutcome(QueryDeviceGroupTagListResult(outcome.result()));
	else
		return QueryDeviceGroupTagListOutcome(outcome.error());
}

void IotClient::queryDeviceGroupTagListAsync(const QueryDeviceGroupTagListRequest& request, const QueryDeviceGroupTagListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceGroupTagList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceGroupTagListOutcomeCallable IotClient::queryDeviceGroupTagListCallable(const QueryDeviceGroupTagListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceGroupTagListOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceGroupTagList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceListByDeviceGroupOutcome IotClient::queryDeviceListByDeviceGroup(const QueryDeviceListByDeviceGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceListByDeviceGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceListByDeviceGroupOutcome(QueryDeviceListByDeviceGroupResult(outcome.result()));
	else
		return QueryDeviceListByDeviceGroupOutcome(outcome.error());
}

void IotClient::queryDeviceListByDeviceGroupAsync(const QueryDeviceListByDeviceGroupRequest& request, const QueryDeviceListByDeviceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceListByDeviceGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceListByDeviceGroupOutcomeCallable IotClient::queryDeviceListByDeviceGroupCallable(const QueryDeviceListByDeviceGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceListByDeviceGroupOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceListByDeviceGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDevicePropOutcome IotClient::queryDeviceProp(const QueryDevicePropRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDevicePropOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDevicePropOutcome(QueryDevicePropResult(outcome.result()));
	else
		return QueryDevicePropOutcome(outcome.error());
}

void IotClient::queryDevicePropAsync(const QueryDevicePropRequest& request, const QueryDevicePropAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceProp(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDevicePropOutcomeCallable IotClient::queryDevicePropCallable(const QueryDevicePropRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDevicePropOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceProp(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDevicePropertiesDataOutcome IotClient::queryDevicePropertiesData(const QueryDevicePropertiesDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDevicePropertiesDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDevicePropertiesDataOutcome(QueryDevicePropertiesDataResult(outcome.result()));
	else
		return QueryDevicePropertiesDataOutcome(outcome.error());
}

void IotClient::queryDevicePropertiesDataAsync(const QueryDevicePropertiesDataRequest& request, const QueryDevicePropertiesDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDevicePropertiesData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDevicePropertiesDataOutcomeCallable IotClient::queryDevicePropertiesDataCallable(const QueryDevicePropertiesDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDevicePropertiesDataOutcome()>>(
			[this, request]()
			{
			return this->queryDevicePropertiesData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDevicePropertyDataOutcome IotClient::queryDevicePropertyData(const QueryDevicePropertyDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDevicePropertyDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDevicePropertyDataOutcome(QueryDevicePropertyDataResult(outcome.result()));
	else
		return QueryDevicePropertyDataOutcome(outcome.error());
}

void IotClient::queryDevicePropertyDataAsync(const QueryDevicePropertyDataRequest& request, const QueryDevicePropertyDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDevicePropertyData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDevicePropertyDataOutcomeCallable IotClient::queryDevicePropertyDataCallable(const QueryDevicePropertyDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDevicePropertyDataOutcome()>>(
			[this, request]()
			{
			return this->queryDevicePropertyData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDevicePropertyStatusOutcome IotClient::queryDevicePropertyStatus(const QueryDevicePropertyStatusRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDevicePropertyStatusOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDevicePropertyStatusOutcome(QueryDevicePropertyStatusResult(outcome.result()));
	else
		return QueryDevicePropertyStatusOutcome(outcome.error());
}

void IotClient::queryDevicePropertyStatusAsync(const QueryDevicePropertyStatusRequest& request, const QueryDevicePropertyStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDevicePropertyStatus(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDevicePropertyStatusOutcomeCallable IotClient::queryDevicePropertyStatusCallable(const QueryDevicePropertyStatusRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDevicePropertyStatusOutcome()>>(
			[this, request]()
			{
			return this->queryDevicePropertyStatus(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceServiceDataOutcome IotClient::queryDeviceServiceData(const QueryDeviceServiceDataRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceServiceDataOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceServiceDataOutcome(QueryDeviceServiceDataResult(outcome.result()));
	else
		return QueryDeviceServiceDataOutcome(outcome.error());
}

void IotClient::queryDeviceServiceDataAsync(const QueryDeviceServiceDataRequest& request, const QueryDeviceServiceDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceServiceData(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceServiceDataOutcomeCallable IotClient::queryDeviceServiceDataCallable(const QueryDeviceServiceDataRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceServiceDataOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceServiceData(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryDeviceStatisticsOutcome IotClient::queryDeviceStatistics(const QueryDeviceStatisticsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryDeviceStatisticsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryDeviceStatisticsOutcome(QueryDeviceStatisticsResult(outcome.result()));
	else
		return QueryDeviceStatisticsOutcome(outcome.error());
}

void IotClient::queryDeviceStatisticsAsync(const QueryDeviceStatisticsRequest& request, const QueryDeviceStatisticsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryDeviceStatistics(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryDeviceStatisticsOutcomeCallable IotClient::queryDeviceStatisticsCallable(const QueryDeviceStatisticsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryDeviceStatisticsOutcome()>>(
			[this, request]()
			{
			return this->queryDeviceStatistics(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryEdgeDriverOutcome IotClient::queryEdgeDriver(const QueryEdgeDriverRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryEdgeDriverOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryEdgeDriverOutcome(QueryEdgeDriverResult(outcome.result()));
	else
		return QueryEdgeDriverOutcome(outcome.error());
}

void IotClient::queryEdgeDriverAsync(const QueryEdgeDriverRequest& request, const QueryEdgeDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryEdgeDriver(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryEdgeDriverOutcomeCallable IotClient::queryEdgeDriverCallable(const QueryEdgeDriverRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryEdgeDriverOutcome()>>(
			[this, request]()
			{
			return this->queryEdgeDriver(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryEdgeDriverVersionOutcome IotClient::queryEdgeDriverVersion(const QueryEdgeDriverVersionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryEdgeDriverVersionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryEdgeDriverVersionOutcome(QueryEdgeDriverVersionResult(outcome.result()));
	else
		return QueryEdgeDriverVersionOutcome(outcome.error());
}

void IotClient::queryEdgeDriverVersionAsync(const QueryEdgeDriverVersionRequest& request, const QueryEdgeDriverVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryEdgeDriverVersion(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryEdgeDriverVersionOutcomeCallable IotClient::queryEdgeDriverVersionCallable(const QueryEdgeDriverVersionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryEdgeDriverVersionOutcome()>>(
			[this, request]()
			{
			return this->queryEdgeDriverVersion(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryEdgeInstanceOutcome IotClient::queryEdgeInstance(const QueryEdgeInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryEdgeInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryEdgeInstanceOutcome(QueryEdgeInstanceResult(outcome.result()));
	else
		return QueryEdgeInstanceOutcome(outcome.error());
}

void IotClient::queryEdgeInstanceAsync(const QueryEdgeInstanceRequest& request, const QueryEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryEdgeInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryEdgeInstanceOutcomeCallable IotClient::queryEdgeInstanceCallable(const QueryEdgeInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryEdgeInstanceOutcome()>>(
			[this, request]()
			{
			return this->queryEdgeInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryEdgeInstanceChannelOutcome IotClient::queryEdgeInstanceChannel(const QueryEdgeInstanceChannelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryEdgeInstanceChannelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryEdgeInstanceChannelOutcome(QueryEdgeInstanceChannelResult(outcome.result()));
	else
		return QueryEdgeInstanceChannelOutcome(outcome.error());
}

void IotClient::queryEdgeInstanceChannelAsync(const QueryEdgeInstanceChannelRequest& request, const QueryEdgeInstanceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryEdgeInstanceChannel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryEdgeInstanceChannelOutcomeCallable IotClient::queryEdgeInstanceChannelCallable(const QueryEdgeInstanceChannelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryEdgeInstanceChannelOutcome()>>(
			[this, request]()
			{
			return this->queryEdgeInstanceChannel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryEdgeInstanceDeviceOutcome IotClient::queryEdgeInstanceDevice(const QueryEdgeInstanceDeviceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryEdgeInstanceDeviceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryEdgeInstanceDeviceOutcome(QueryEdgeInstanceDeviceResult(outcome.result()));
	else
		return QueryEdgeInstanceDeviceOutcome(outcome.error());
}

void IotClient::queryEdgeInstanceDeviceAsync(const QueryEdgeInstanceDeviceRequest& request, const QueryEdgeInstanceDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryEdgeInstanceDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryEdgeInstanceDeviceOutcomeCallable IotClient::queryEdgeInstanceDeviceCallable(const QueryEdgeInstanceDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryEdgeInstanceDeviceOutcome()>>(
			[this, request]()
			{
			return this->queryEdgeInstanceDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryEdgeInstanceDeviceByDriverOutcome IotClient::queryEdgeInstanceDeviceByDriver(const QueryEdgeInstanceDeviceByDriverRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryEdgeInstanceDeviceByDriverOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryEdgeInstanceDeviceByDriverOutcome(QueryEdgeInstanceDeviceByDriverResult(outcome.result()));
	else
		return QueryEdgeInstanceDeviceByDriverOutcome(outcome.error());
}

void IotClient::queryEdgeInstanceDeviceByDriverAsync(const QueryEdgeInstanceDeviceByDriverRequest& request, const QueryEdgeInstanceDeviceByDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryEdgeInstanceDeviceByDriver(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryEdgeInstanceDeviceByDriverOutcomeCallable IotClient::queryEdgeInstanceDeviceByDriverCallable(const QueryEdgeInstanceDeviceByDriverRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryEdgeInstanceDeviceByDriverOutcome()>>(
			[this, request]()
			{
			return this->queryEdgeInstanceDeviceByDriver(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryEdgeInstanceDriverOutcome IotClient::queryEdgeInstanceDriver(const QueryEdgeInstanceDriverRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryEdgeInstanceDriverOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryEdgeInstanceDriverOutcome(QueryEdgeInstanceDriverResult(outcome.result()));
	else
		return QueryEdgeInstanceDriverOutcome(outcome.error());
}

void IotClient::queryEdgeInstanceDriverAsync(const QueryEdgeInstanceDriverRequest& request, const QueryEdgeInstanceDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryEdgeInstanceDriver(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryEdgeInstanceDriverOutcomeCallable IotClient::queryEdgeInstanceDriverCallable(const QueryEdgeInstanceDriverRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryEdgeInstanceDriverOutcome()>>(
			[this, request]()
			{
			return this->queryEdgeInstanceDriver(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryEdgeInstanceGatewayOutcome IotClient::queryEdgeInstanceGateway(const QueryEdgeInstanceGatewayRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryEdgeInstanceGatewayOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryEdgeInstanceGatewayOutcome(QueryEdgeInstanceGatewayResult(outcome.result()));
	else
		return QueryEdgeInstanceGatewayOutcome(outcome.error());
}

void IotClient::queryEdgeInstanceGatewayAsync(const QueryEdgeInstanceGatewayRequest& request, const QueryEdgeInstanceGatewayAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryEdgeInstanceGateway(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryEdgeInstanceGatewayOutcomeCallable IotClient::queryEdgeInstanceGatewayCallable(const QueryEdgeInstanceGatewayRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryEdgeInstanceGatewayOutcome()>>(
			[this, request]()
			{
			return this->queryEdgeInstanceGateway(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryEdgeInstanceHistoricDeploymentOutcome IotClient::queryEdgeInstanceHistoricDeployment(const QueryEdgeInstanceHistoricDeploymentRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryEdgeInstanceHistoricDeploymentOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryEdgeInstanceHistoricDeploymentOutcome(QueryEdgeInstanceHistoricDeploymentResult(outcome.result()));
	else
		return QueryEdgeInstanceHistoricDeploymentOutcome(outcome.error());
}

void IotClient::queryEdgeInstanceHistoricDeploymentAsync(const QueryEdgeInstanceHistoricDeploymentRequest& request, const QueryEdgeInstanceHistoricDeploymentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryEdgeInstanceHistoricDeployment(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryEdgeInstanceHistoricDeploymentOutcomeCallable IotClient::queryEdgeInstanceHistoricDeploymentCallable(const QueryEdgeInstanceHistoricDeploymentRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryEdgeInstanceHistoricDeploymentOutcome()>>(
			[this, request]()
			{
			return this->queryEdgeInstanceHistoricDeployment(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryEdgeInstanceSceneRuleOutcome IotClient::queryEdgeInstanceSceneRule(const QueryEdgeInstanceSceneRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryEdgeInstanceSceneRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryEdgeInstanceSceneRuleOutcome(QueryEdgeInstanceSceneRuleResult(outcome.result()));
	else
		return QueryEdgeInstanceSceneRuleOutcome(outcome.error());
}

void IotClient::queryEdgeInstanceSceneRuleAsync(const QueryEdgeInstanceSceneRuleRequest& request, const QueryEdgeInstanceSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryEdgeInstanceSceneRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryEdgeInstanceSceneRuleOutcomeCallable IotClient::queryEdgeInstanceSceneRuleCallable(const QueryEdgeInstanceSceneRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryEdgeInstanceSceneRuleOutcome()>>(
			[this, request]()
			{
			return this->queryEdgeInstanceSceneRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryLoRaJoinPermissionsOutcome IotClient::queryLoRaJoinPermissions(const QueryLoRaJoinPermissionsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryLoRaJoinPermissionsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryLoRaJoinPermissionsOutcome(QueryLoRaJoinPermissionsResult(outcome.result()));
	else
		return QueryLoRaJoinPermissionsOutcome(outcome.error());
}

void IotClient::queryLoRaJoinPermissionsAsync(const QueryLoRaJoinPermissionsRequest& request, const QueryLoRaJoinPermissionsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryLoRaJoinPermissions(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryLoRaJoinPermissionsOutcomeCallable IotClient::queryLoRaJoinPermissionsCallable(const QueryLoRaJoinPermissionsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryLoRaJoinPermissionsOutcome()>>(
			[this, request]()
			{
			return this->queryLoRaJoinPermissions(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryOTAFirmwareOutcome IotClient::queryOTAFirmware(const QueryOTAFirmwareRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryOTAFirmwareOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryOTAFirmwareOutcome(QueryOTAFirmwareResult(outcome.result()));
	else
		return QueryOTAFirmwareOutcome(outcome.error());
}

void IotClient::queryOTAFirmwareAsync(const QueryOTAFirmwareRequest& request, const QueryOTAFirmwareAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryOTAFirmware(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryOTAFirmwareOutcomeCallable IotClient::queryOTAFirmwareCallable(const QueryOTAFirmwareRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryOTAFirmwareOutcome()>>(
			[this, request]()
			{
			return this->queryOTAFirmware(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryOTAJobOutcome IotClient::queryOTAJob(const QueryOTAJobRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryOTAJobOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryOTAJobOutcome(QueryOTAJobResult(outcome.result()));
	else
		return QueryOTAJobOutcome(outcome.error());
}

void IotClient::queryOTAJobAsync(const QueryOTAJobRequest& request, const QueryOTAJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryOTAJob(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryOTAJobOutcomeCallable IotClient::queryOTAJobCallable(const QueryOTAJobRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryOTAJobOutcome()>>(
			[this, request]()
			{
			return this->queryOTAJob(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryPageByApplyIdOutcome IotClient::queryPageByApplyId(const QueryPageByApplyIdRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryPageByApplyIdOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryPageByApplyIdOutcome(QueryPageByApplyIdResult(outcome.result()));
	else
		return QueryPageByApplyIdOutcome(outcome.error());
}

void IotClient::queryPageByApplyIdAsync(const QueryPageByApplyIdRequest& request, const QueryPageByApplyIdAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryPageByApplyId(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryPageByApplyIdOutcomeCallable IotClient::queryPageByApplyIdCallable(const QueryPageByApplyIdRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryPageByApplyIdOutcome()>>(
			[this, request]()
			{
			return this->queryPageByApplyId(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryProductOutcome IotClient::queryProduct(const QueryProductRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryProductOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryProductOutcome(QueryProductResult(outcome.result()));
	else
		return QueryProductOutcome(outcome.error());
}

void IotClient::queryProductAsync(const QueryProductRequest& request, const QueryProductAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryProduct(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryProductOutcomeCallable IotClient::queryProductCallable(const QueryProductRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryProductOutcome()>>(
			[this, request]()
			{
			return this->queryProduct(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryProductCertInfoOutcome IotClient::queryProductCertInfo(const QueryProductCertInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryProductCertInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryProductCertInfoOutcome(QueryProductCertInfoResult(outcome.result()));
	else
		return QueryProductCertInfoOutcome(outcome.error());
}

void IotClient::queryProductCertInfoAsync(const QueryProductCertInfoRequest& request, const QueryProductCertInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryProductCertInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryProductCertInfoOutcomeCallable IotClient::queryProductCertInfoCallable(const QueryProductCertInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryProductCertInfoOutcome()>>(
			[this, request]()
			{
			return this->queryProductCertInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryProductListOutcome IotClient::queryProductList(const QueryProductListRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryProductListOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryProductListOutcome(QueryProductListResult(outcome.result()));
	else
		return QueryProductListOutcome(outcome.error());
}

void IotClient::queryProductListAsync(const QueryProductListRequest& request, const QueryProductListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryProductList(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryProductListOutcomeCallable IotClient::queryProductListCallable(const QueryProductListRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryProductListOutcome()>>(
			[this, request]()
			{
			return this->queryProductList(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryProductTopicOutcome IotClient::queryProductTopic(const QueryProductTopicRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryProductTopicOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryProductTopicOutcome(QueryProductTopicResult(outcome.result()));
	else
		return QueryProductTopicOutcome(outcome.error());
}

void IotClient::queryProductTopicAsync(const QueryProductTopicRequest& request, const QueryProductTopicAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryProductTopic(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryProductTopicOutcomeCallable IotClient::queryProductTopicCallable(const QueryProductTopicRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryProductTopicOutcome()>>(
			[this, request]()
			{
			return this->queryProductTopic(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QuerySceneRuleOutcome IotClient::querySceneRule(const QuerySceneRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QuerySceneRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QuerySceneRuleOutcome(QuerySceneRuleResult(outcome.result()));
	else
		return QuerySceneRuleOutcome(outcome.error());
}

void IotClient::querySceneRuleAsync(const QuerySceneRuleRequest& request, const QuerySceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, querySceneRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QuerySceneRuleOutcomeCallable IotClient::querySceneRuleCallable(const QuerySceneRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QuerySceneRuleOutcome()>>(
			[this, request]()
			{
			return this->querySceneRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QuerySubscribeRelationOutcome IotClient::querySubscribeRelation(const QuerySubscribeRelationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QuerySubscribeRelationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QuerySubscribeRelationOutcome(QuerySubscribeRelationResult(outcome.result()));
	else
		return QuerySubscribeRelationOutcome(outcome.error());
}

void IotClient::querySubscribeRelationAsync(const QuerySubscribeRelationRequest& request, const QuerySubscribeRelationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, querySubscribeRelation(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QuerySubscribeRelationOutcomeCallable IotClient::querySubscribeRelationCallable(const QuerySubscribeRelationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QuerySubscribeRelationOutcome()>>(
			[this, request]()
			{
			return this->querySubscribeRelation(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QuerySummarySceneRuleLogOutcome IotClient::querySummarySceneRuleLog(const QuerySummarySceneRuleLogRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QuerySummarySceneRuleLogOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QuerySummarySceneRuleLogOutcome(QuerySummarySceneRuleLogResult(outcome.result()));
	else
		return QuerySummarySceneRuleLogOutcome(outcome.error());
}

void IotClient::querySummarySceneRuleLogAsync(const QuerySummarySceneRuleLogRequest& request, const QuerySummarySceneRuleLogAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, querySummarySceneRuleLog(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QuerySummarySceneRuleLogOutcomeCallable IotClient::querySummarySceneRuleLogCallable(const QuerySummarySceneRuleLogRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QuerySummarySceneRuleLogOutcome()>>(
			[this, request]()
			{
			return this->querySummarySceneRuleLog(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QuerySuperDeviceGroupOutcome IotClient::querySuperDeviceGroup(const QuerySuperDeviceGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QuerySuperDeviceGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QuerySuperDeviceGroupOutcome(QuerySuperDeviceGroupResult(outcome.result()));
	else
		return QuerySuperDeviceGroupOutcome(outcome.error());
}

void IotClient::querySuperDeviceGroupAsync(const QuerySuperDeviceGroupRequest& request, const QuerySuperDeviceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, querySuperDeviceGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QuerySuperDeviceGroupOutcomeCallable IotClient::querySuperDeviceGroupCallable(const QuerySuperDeviceGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QuerySuperDeviceGroupOutcome()>>(
			[this, request]()
			{
			return this->querySuperDeviceGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryThingModelOutcome IotClient::queryThingModel(const QueryThingModelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryThingModelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryThingModelOutcome(QueryThingModelResult(outcome.result()));
	else
		return QueryThingModelOutcome(outcome.error());
}

void IotClient::queryThingModelAsync(const QueryThingModelRequest& request, const QueryThingModelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryThingModel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryThingModelOutcomeCallable IotClient::queryThingModelCallable(const QueryThingModelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryThingModelOutcome()>>(
			[this, request]()
			{
			return this->queryThingModel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryTopicReverseRouteTableOutcome IotClient::queryTopicReverseRouteTable(const QueryTopicReverseRouteTableRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryTopicReverseRouteTableOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryTopicReverseRouteTableOutcome(QueryTopicReverseRouteTableResult(outcome.result()));
	else
		return QueryTopicReverseRouteTableOutcome(outcome.error());
}

void IotClient::queryTopicReverseRouteTableAsync(const QueryTopicReverseRouteTableRequest& request, const QueryTopicReverseRouteTableAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryTopicReverseRouteTable(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryTopicReverseRouteTableOutcomeCallable IotClient::queryTopicReverseRouteTableCallable(const QueryTopicReverseRouteTableRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryTopicReverseRouteTableOutcome()>>(
			[this, request]()
			{
			return this->queryTopicReverseRouteTable(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::QueryTopicRouteTableOutcome IotClient::queryTopicRouteTable(const QueryTopicRouteTableRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return QueryTopicRouteTableOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return QueryTopicRouteTableOutcome(QueryTopicRouteTableResult(outcome.result()));
	else
		return QueryTopicRouteTableOutcome(outcome.error());
}

void IotClient::queryTopicRouteTableAsync(const QueryTopicRouteTableRequest& request, const QueryTopicRouteTableAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, queryTopicRouteTable(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::QueryTopicRouteTableOutcomeCallable IotClient::queryTopicRouteTableCallable(const QueryTopicRouteTableRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<QueryTopicRouteTableOutcome()>>(
			[this, request]()
			{
			return this->queryTopicRouteTable(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::RRpcOutcome IotClient::rRpc(const RRpcRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RRpcOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RRpcOutcome(RRpcResult(outcome.result()));
	else
		return RRpcOutcome(outcome.error());
}

void IotClient::rRpcAsync(const RRpcRequest& request, const RRpcAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, rRpc(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::RRpcOutcomeCallable IotClient::rRpcCallable(const RRpcRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RRpcOutcome()>>(
			[this, request]()
			{
			return this->rRpc(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::RegisterDeviceOutcome IotClient::registerDevice(const RegisterDeviceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RegisterDeviceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RegisterDeviceOutcome(RegisterDeviceResult(outcome.result()));
	else
		return RegisterDeviceOutcome(outcome.error());
}

void IotClient::registerDeviceAsync(const RegisterDeviceRequest& request, const RegisterDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, registerDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::RegisterDeviceOutcomeCallable IotClient::registerDeviceCallable(const RegisterDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RegisterDeviceOutcome()>>(
			[this, request]()
			{
			return this->registerDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ReleaseEdgeDriverVersionOutcome IotClient::releaseEdgeDriverVersion(const ReleaseEdgeDriverVersionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ReleaseEdgeDriverVersionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ReleaseEdgeDriverVersionOutcome(ReleaseEdgeDriverVersionResult(outcome.result()));
	else
		return ReleaseEdgeDriverVersionOutcome(outcome.error());
}

void IotClient::releaseEdgeDriverVersionAsync(const ReleaseEdgeDriverVersionRequest& request, const ReleaseEdgeDriverVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, releaseEdgeDriverVersion(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ReleaseEdgeDriverVersionOutcomeCallable IotClient::releaseEdgeDriverVersionCallable(const ReleaseEdgeDriverVersionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ReleaseEdgeDriverVersionOutcome()>>(
			[this, request]()
			{
			return this->releaseEdgeDriverVersion(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::RemoveThingTopoOutcome IotClient::removeThingTopo(const RemoveThingTopoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveThingTopoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveThingTopoOutcome(RemoveThingTopoResult(outcome.result()));
	else
		return RemoveThingTopoOutcome(outcome.error());
}

void IotClient::removeThingTopoAsync(const RemoveThingTopoRequest& request, const RemoveThingTopoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeThingTopo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::RemoveThingTopoOutcomeCallable IotClient::removeThingTopoCallable(const RemoveThingTopoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveThingTopoOutcome()>>(
			[this, request]()
			{
			return this->removeThingTopo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ReplaceEdgeInstanceGatewayOutcome IotClient::replaceEdgeInstanceGateway(const ReplaceEdgeInstanceGatewayRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ReplaceEdgeInstanceGatewayOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ReplaceEdgeInstanceGatewayOutcome(ReplaceEdgeInstanceGatewayResult(outcome.result()));
	else
		return ReplaceEdgeInstanceGatewayOutcome(outcome.error());
}

void IotClient::replaceEdgeInstanceGatewayAsync(const ReplaceEdgeInstanceGatewayRequest& request, const ReplaceEdgeInstanceGatewayAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, replaceEdgeInstanceGateway(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ReplaceEdgeInstanceGatewayOutcomeCallable IotClient::replaceEdgeInstanceGatewayCallable(const ReplaceEdgeInstanceGatewayRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ReplaceEdgeInstanceGatewayOutcome()>>(
			[this, request]()
			{
			return this->replaceEdgeInstanceGateway(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ResetConsumerGroupPositionOutcome IotClient::resetConsumerGroupPosition(const ResetConsumerGroupPositionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ResetConsumerGroupPositionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ResetConsumerGroupPositionOutcome(ResetConsumerGroupPositionResult(outcome.result()));
	else
		return ResetConsumerGroupPositionOutcome(outcome.error());
}

void IotClient::resetConsumerGroupPositionAsync(const ResetConsumerGroupPositionRequest& request, const ResetConsumerGroupPositionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, resetConsumerGroupPosition(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ResetConsumerGroupPositionOutcomeCallable IotClient::resetConsumerGroupPositionCallable(const ResetConsumerGroupPositionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ResetConsumerGroupPositionOutcome()>>(
			[this, request]()
			{
			return this->resetConsumerGroupPosition(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::ResetThingOutcome IotClient::resetThing(const ResetThingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ResetThingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ResetThingOutcome(ResetThingResult(outcome.result()));
	else
		return ResetThingOutcome(outcome.error());
}

void IotClient::resetThingAsync(const ResetThingRequest& request, const ResetThingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, resetThing(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::ResetThingOutcomeCallable IotClient::resetThingCallable(const ResetThingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ResetThingOutcome()>>(
			[this, request]()
			{
			return this->resetThing(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::SaveDevicePropOutcome IotClient::saveDeviceProp(const SaveDevicePropRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SaveDevicePropOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SaveDevicePropOutcome(SaveDevicePropResult(outcome.result()));
	else
		return SaveDevicePropOutcome(outcome.error());
}

void IotClient::saveDevicePropAsync(const SaveDevicePropRequest& request, const SaveDevicePropAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, saveDeviceProp(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::SaveDevicePropOutcomeCallable IotClient::saveDevicePropCallable(const SaveDevicePropRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SaveDevicePropOutcome()>>(
			[this, request]()
			{
			return this->saveDeviceProp(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::SetDeviceDesiredPropertyOutcome IotClient::setDeviceDesiredProperty(const SetDeviceDesiredPropertyRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetDeviceDesiredPropertyOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetDeviceDesiredPropertyOutcome(SetDeviceDesiredPropertyResult(outcome.result()));
	else
		return SetDeviceDesiredPropertyOutcome(outcome.error());
}

void IotClient::setDeviceDesiredPropertyAsync(const SetDeviceDesiredPropertyRequest& request, const SetDeviceDesiredPropertyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setDeviceDesiredProperty(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::SetDeviceDesiredPropertyOutcomeCallable IotClient::setDeviceDesiredPropertyCallable(const SetDeviceDesiredPropertyRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetDeviceDesiredPropertyOutcome()>>(
			[this, request]()
			{
			return this->setDeviceDesiredProperty(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::SetDeviceGroupTagsOutcome IotClient::setDeviceGroupTags(const SetDeviceGroupTagsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetDeviceGroupTagsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetDeviceGroupTagsOutcome(SetDeviceGroupTagsResult(outcome.result()));
	else
		return SetDeviceGroupTagsOutcome(outcome.error());
}

void IotClient::setDeviceGroupTagsAsync(const SetDeviceGroupTagsRequest& request, const SetDeviceGroupTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setDeviceGroupTags(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::SetDeviceGroupTagsOutcomeCallable IotClient::setDeviceGroupTagsCallable(const SetDeviceGroupTagsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetDeviceGroupTagsOutcome()>>(
			[this, request]()
			{
			return this->setDeviceGroupTags(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::SetDevicePropertyOutcome IotClient::setDeviceProperty(const SetDevicePropertyRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetDevicePropertyOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetDevicePropertyOutcome(SetDevicePropertyResult(outcome.result()));
	else
		return SetDevicePropertyOutcome(outcome.error());
}

void IotClient::setDevicePropertyAsync(const SetDevicePropertyRequest& request, const SetDevicePropertyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setDeviceProperty(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::SetDevicePropertyOutcomeCallable IotClient::setDevicePropertyCallable(const SetDevicePropertyRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetDevicePropertyOutcome()>>(
			[this, request]()
			{
			return this->setDeviceProperty(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::SetDevicesPropertyOutcome IotClient::setDevicesProperty(const SetDevicesPropertyRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetDevicesPropertyOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetDevicesPropertyOutcome(SetDevicesPropertyResult(outcome.result()));
	else
		return SetDevicesPropertyOutcome(outcome.error());
}

void IotClient::setDevicesPropertyAsync(const SetDevicesPropertyRequest& request, const SetDevicesPropertyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setDevicesProperty(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::SetDevicesPropertyOutcomeCallable IotClient::setDevicesPropertyCallable(const SetDevicesPropertyRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetDevicesPropertyOutcome()>>(
			[this, request]()
			{
			return this->setDevicesProperty(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::SetEdgeInstanceDriverConfigsOutcome IotClient::setEdgeInstanceDriverConfigs(const SetEdgeInstanceDriverConfigsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetEdgeInstanceDriverConfigsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetEdgeInstanceDriverConfigsOutcome(SetEdgeInstanceDriverConfigsResult(outcome.result()));
	else
		return SetEdgeInstanceDriverConfigsOutcome(outcome.error());
}

void IotClient::setEdgeInstanceDriverConfigsAsync(const SetEdgeInstanceDriverConfigsRequest& request, const SetEdgeInstanceDriverConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setEdgeInstanceDriverConfigs(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::SetEdgeInstanceDriverConfigsOutcomeCallable IotClient::setEdgeInstanceDriverConfigsCallable(const SetEdgeInstanceDriverConfigsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetEdgeInstanceDriverConfigsOutcome()>>(
			[this, request]()
			{
			return this->setEdgeInstanceDriverConfigs(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::SetProductCertInfoOutcome IotClient::setProductCertInfo(const SetProductCertInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetProductCertInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetProductCertInfoOutcome(SetProductCertInfoResult(outcome.result()));
	else
		return SetProductCertInfoOutcome(outcome.error());
}

void IotClient::setProductCertInfoAsync(const SetProductCertInfoRequest& request, const SetProductCertInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setProductCertInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::SetProductCertInfoOutcomeCallable IotClient::setProductCertInfoCallable(const SetProductCertInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetProductCertInfoOutcome()>>(
			[this, request]()
			{
			return this->setProductCertInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::StartRuleOutcome IotClient::startRule(const StartRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartRuleOutcome(StartRuleResult(outcome.result()));
	else
		return StartRuleOutcome(outcome.error());
}

void IotClient::startRuleAsync(const StartRuleRequest& request, const StartRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::StartRuleOutcomeCallable IotClient::startRuleCallable(const StartRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartRuleOutcome()>>(
			[this, request]()
			{
			return this->startRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::StopRuleOutcome IotClient::stopRule(const StopRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StopRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StopRuleOutcome(StopRuleResult(outcome.result()));
	else
		return StopRuleOutcome(outcome.error());
}

void IotClient::stopRuleAsync(const StopRuleRequest& request, const StopRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, stopRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::StopRuleOutcomeCallable IotClient::stopRuleCallable(const StopRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StopRuleOutcome()>>(
			[this, request]()
			{
			return this->stopRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::TriggerSceneRuleOutcome IotClient::triggerSceneRule(const TriggerSceneRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return TriggerSceneRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return TriggerSceneRuleOutcome(TriggerSceneRuleResult(outcome.result()));
	else
		return TriggerSceneRuleOutcome(outcome.error());
}

void IotClient::triggerSceneRuleAsync(const TriggerSceneRuleRequest& request, const TriggerSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, triggerSceneRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::TriggerSceneRuleOutcomeCallable IotClient::triggerSceneRuleCallable(const TriggerSceneRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<TriggerSceneRuleOutcome()>>(
			[this, request]()
			{
			return this->triggerSceneRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UnbindApplicationFromEdgeInstanceOutcome IotClient::unbindApplicationFromEdgeInstance(const UnbindApplicationFromEdgeInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UnbindApplicationFromEdgeInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UnbindApplicationFromEdgeInstanceOutcome(UnbindApplicationFromEdgeInstanceResult(outcome.result()));
	else
		return UnbindApplicationFromEdgeInstanceOutcome(outcome.error());
}

void IotClient::unbindApplicationFromEdgeInstanceAsync(const UnbindApplicationFromEdgeInstanceRequest& request, const UnbindApplicationFromEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, unbindApplicationFromEdgeInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UnbindApplicationFromEdgeInstanceOutcomeCallable IotClient::unbindApplicationFromEdgeInstanceCallable(const UnbindApplicationFromEdgeInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UnbindApplicationFromEdgeInstanceOutcome()>>(
			[this, request]()
			{
			return this->unbindApplicationFromEdgeInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UnbindDriverFromEdgeInstanceOutcome IotClient::unbindDriverFromEdgeInstance(const UnbindDriverFromEdgeInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UnbindDriverFromEdgeInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UnbindDriverFromEdgeInstanceOutcome(UnbindDriverFromEdgeInstanceResult(outcome.result()));
	else
		return UnbindDriverFromEdgeInstanceOutcome(outcome.error());
}

void IotClient::unbindDriverFromEdgeInstanceAsync(const UnbindDriverFromEdgeInstanceRequest& request, const UnbindDriverFromEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, unbindDriverFromEdgeInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UnbindDriverFromEdgeInstanceOutcomeCallable IotClient::unbindDriverFromEdgeInstanceCallable(const UnbindDriverFromEdgeInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UnbindDriverFromEdgeInstanceOutcome()>>(
			[this, request]()
			{
			return this->unbindDriverFromEdgeInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UnbindSceneRuleFromEdgeInstanceOutcome IotClient::unbindSceneRuleFromEdgeInstance(const UnbindSceneRuleFromEdgeInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UnbindSceneRuleFromEdgeInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UnbindSceneRuleFromEdgeInstanceOutcome(UnbindSceneRuleFromEdgeInstanceResult(outcome.result()));
	else
		return UnbindSceneRuleFromEdgeInstanceOutcome(outcome.error());
}

void IotClient::unbindSceneRuleFromEdgeInstanceAsync(const UnbindSceneRuleFromEdgeInstanceRequest& request, const UnbindSceneRuleFromEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, unbindSceneRuleFromEdgeInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UnbindSceneRuleFromEdgeInstanceOutcomeCallable IotClient::unbindSceneRuleFromEdgeInstanceCallable(const UnbindSceneRuleFromEdgeInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UnbindSceneRuleFromEdgeInstanceOutcome()>>(
			[this, request]()
			{
			return this->unbindSceneRuleFromEdgeInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateConsumerGroupOutcome IotClient::updateConsumerGroup(const UpdateConsumerGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateConsumerGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateConsumerGroupOutcome(UpdateConsumerGroupResult(outcome.result()));
	else
		return UpdateConsumerGroupOutcome(outcome.error());
}

void IotClient::updateConsumerGroupAsync(const UpdateConsumerGroupRequest& request, const UpdateConsumerGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateConsumerGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateConsumerGroupOutcomeCallable IotClient::updateConsumerGroupCallable(const UpdateConsumerGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateConsumerGroupOutcome()>>(
			[this, request]()
			{
			return this->updateConsumerGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateDeviceGroupOutcome IotClient::updateDeviceGroup(const UpdateDeviceGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateDeviceGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateDeviceGroupOutcome(UpdateDeviceGroupResult(outcome.result()));
	else
		return UpdateDeviceGroupOutcome(outcome.error());
}

void IotClient::updateDeviceGroupAsync(const UpdateDeviceGroupRequest& request, const UpdateDeviceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateDeviceGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateDeviceGroupOutcomeCallable IotClient::updateDeviceGroupCallable(const UpdateDeviceGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateDeviceGroupOutcome()>>(
			[this, request]()
			{
			return this->updateDeviceGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateDeviceShadowOutcome IotClient::updateDeviceShadow(const UpdateDeviceShadowRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateDeviceShadowOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateDeviceShadowOutcome(UpdateDeviceShadowResult(outcome.result()));
	else
		return UpdateDeviceShadowOutcome(outcome.error());
}

void IotClient::updateDeviceShadowAsync(const UpdateDeviceShadowRequest& request, const UpdateDeviceShadowAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateDeviceShadow(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateDeviceShadowOutcomeCallable IotClient::updateDeviceShadowCallable(const UpdateDeviceShadowRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateDeviceShadowOutcome()>>(
			[this, request]()
			{
			return this->updateDeviceShadow(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateEdgeDriverVersionOutcome IotClient::updateEdgeDriverVersion(const UpdateEdgeDriverVersionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateEdgeDriverVersionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateEdgeDriverVersionOutcome(UpdateEdgeDriverVersionResult(outcome.result()));
	else
		return UpdateEdgeDriverVersionOutcome(outcome.error());
}

void IotClient::updateEdgeDriverVersionAsync(const UpdateEdgeDriverVersionRequest& request, const UpdateEdgeDriverVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateEdgeDriverVersion(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateEdgeDriverVersionOutcomeCallable IotClient::updateEdgeDriverVersionCallable(const UpdateEdgeDriverVersionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateEdgeDriverVersionOutcome()>>(
			[this, request]()
			{
			return this->updateEdgeDriverVersion(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateEdgeInstanceOutcome IotClient::updateEdgeInstance(const UpdateEdgeInstanceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateEdgeInstanceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateEdgeInstanceOutcome(UpdateEdgeInstanceResult(outcome.result()));
	else
		return UpdateEdgeInstanceOutcome(outcome.error());
}

void IotClient::updateEdgeInstanceAsync(const UpdateEdgeInstanceRequest& request, const UpdateEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateEdgeInstance(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateEdgeInstanceOutcomeCallable IotClient::updateEdgeInstanceCallable(const UpdateEdgeInstanceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateEdgeInstanceOutcome()>>(
			[this, request]()
			{
			return this->updateEdgeInstance(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateEdgeInstanceChannelOutcome IotClient::updateEdgeInstanceChannel(const UpdateEdgeInstanceChannelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateEdgeInstanceChannelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateEdgeInstanceChannelOutcome(UpdateEdgeInstanceChannelResult(outcome.result()));
	else
		return UpdateEdgeInstanceChannelOutcome(outcome.error());
}

void IotClient::updateEdgeInstanceChannelAsync(const UpdateEdgeInstanceChannelRequest& request, const UpdateEdgeInstanceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateEdgeInstanceChannel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateEdgeInstanceChannelOutcomeCallable IotClient::updateEdgeInstanceChannelCallable(const UpdateEdgeInstanceChannelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateEdgeInstanceChannelOutcome()>>(
			[this, request]()
			{
			return this->updateEdgeInstanceChannel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateProductOutcome IotClient::updateProduct(const UpdateProductRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateProductOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateProductOutcome(UpdateProductResult(outcome.result()));
	else
		return UpdateProductOutcome(outcome.error());
}

void IotClient::updateProductAsync(const UpdateProductRequest& request, const UpdateProductAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateProduct(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateProductOutcomeCallable IotClient::updateProductCallable(const UpdateProductRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateProductOutcome()>>(
			[this, request]()
			{
			return this->updateProduct(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateProductFilterConfigOutcome IotClient::updateProductFilterConfig(const UpdateProductFilterConfigRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateProductFilterConfigOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateProductFilterConfigOutcome(UpdateProductFilterConfigResult(outcome.result()));
	else
		return UpdateProductFilterConfigOutcome(outcome.error());
}

void IotClient::updateProductFilterConfigAsync(const UpdateProductFilterConfigRequest& request, const UpdateProductFilterConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateProductFilterConfig(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateProductFilterConfigOutcomeCallable IotClient::updateProductFilterConfigCallable(const UpdateProductFilterConfigRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateProductFilterConfigOutcome()>>(
			[this, request]()
			{
			return this->updateProductFilterConfig(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateProductTagsOutcome IotClient::updateProductTags(const UpdateProductTagsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateProductTagsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateProductTagsOutcome(UpdateProductTagsResult(outcome.result()));
	else
		return UpdateProductTagsOutcome(outcome.error());
}

void IotClient::updateProductTagsAsync(const UpdateProductTagsRequest& request, const UpdateProductTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateProductTags(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateProductTagsOutcomeCallable IotClient::updateProductTagsCallable(const UpdateProductTagsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateProductTagsOutcome()>>(
			[this, request]()
			{
			return this->updateProductTags(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateProductTopicOutcome IotClient::updateProductTopic(const UpdateProductTopicRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateProductTopicOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateProductTopicOutcome(UpdateProductTopicResult(outcome.result()));
	else
		return UpdateProductTopicOutcome(outcome.error());
}

void IotClient::updateProductTopicAsync(const UpdateProductTopicRequest& request, const UpdateProductTopicAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateProductTopic(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateProductTopicOutcomeCallable IotClient::updateProductTopicCallable(const UpdateProductTopicRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateProductTopicOutcome()>>(
			[this, request]()
			{
			return this->updateProductTopic(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateRuleOutcome IotClient::updateRule(const UpdateRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateRuleOutcome(UpdateRuleResult(outcome.result()));
	else
		return UpdateRuleOutcome(outcome.error());
}

void IotClient::updateRuleAsync(const UpdateRuleRequest& request, const UpdateRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateRuleOutcomeCallable IotClient::updateRuleCallable(const UpdateRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateRuleOutcome()>>(
			[this, request]()
			{
			return this->updateRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateRuleActionOutcome IotClient::updateRuleAction(const UpdateRuleActionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateRuleActionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateRuleActionOutcome(UpdateRuleActionResult(outcome.result()));
	else
		return UpdateRuleActionOutcome(outcome.error());
}

void IotClient::updateRuleActionAsync(const UpdateRuleActionRequest& request, const UpdateRuleActionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateRuleAction(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateRuleActionOutcomeCallable IotClient::updateRuleActionCallable(const UpdateRuleActionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateRuleActionOutcome()>>(
			[this, request]()
			{
			return this->updateRuleAction(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateSceneRuleOutcome IotClient::updateSceneRule(const UpdateSceneRuleRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateSceneRuleOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateSceneRuleOutcome(UpdateSceneRuleResult(outcome.result()));
	else
		return UpdateSceneRuleOutcome(outcome.error());
}

void IotClient::updateSceneRuleAsync(const UpdateSceneRuleRequest& request, const UpdateSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateSceneRule(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateSceneRuleOutcomeCallable IotClient::updateSceneRuleCallable(const UpdateSceneRuleRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateSceneRuleOutcome()>>(
			[this, request]()
			{
			return this->updateSceneRule(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateSubscribeRelationOutcome IotClient::updateSubscribeRelation(const UpdateSubscribeRelationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateSubscribeRelationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateSubscribeRelationOutcome(UpdateSubscribeRelationResult(outcome.result()));
	else
		return UpdateSubscribeRelationOutcome(outcome.error());
}

void IotClient::updateSubscribeRelationAsync(const UpdateSubscribeRelationRequest& request, const UpdateSubscribeRelationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateSubscribeRelation(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateSubscribeRelationOutcomeCallable IotClient::updateSubscribeRelationCallable(const UpdateSubscribeRelationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateSubscribeRelationOutcome()>>(
			[this, request]()
			{
			return this->updateSubscribeRelation(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IotClient::UpdateThingModelOutcome IotClient::updateThingModel(const UpdateThingModelRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateThingModelOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateThingModelOutcome(UpdateThingModelResult(outcome.result()));
	else
		return UpdateThingModelOutcome(outcome.error());
}

void IotClient::updateThingModelAsync(const UpdateThingModelRequest& request, const UpdateThingModelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateThingModel(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IotClient::UpdateThingModelOutcomeCallable IotClient::updateThingModelCallable(const UpdateThingModelRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateThingModelOutcome()>>(
			[this, request]()
			{
			return this->updateThingModel(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


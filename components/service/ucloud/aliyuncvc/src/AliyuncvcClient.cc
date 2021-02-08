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

#include <alibabacloud/aliyuncvc/AliyuncvcClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

namespace
{
	const std::string SERVICE_NAME = "aliyuncvc";
}

AliyuncvcClient::AliyuncvcClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "aliyuncvc");
}

AliyuncvcClient::AliyuncvcClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "aliyuncvc");
}

AliyuncvcClient::AliyuncvcClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "aliyuncvc");
}

AliyuncvcClient::~AliyuncvcClient()
{}

AliyuncvcClient::ActiveDeviceOutcome AliyuncvcClient::activeDevice(const ActiveDeviceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ActiveDeviceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ActiveDeviceOutcome(ActiveDeviceResult(outcome.result()));
	else
		return ActiveDeviceOutcome(outcome.error());
}

void AliyuncvcClient::activeDeviceAsync(const ActiveDeviceRequest& request, const ActiveDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, activeDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::ActiveDeviceOutcomeCallable AliyuncvcClient::activeDeviceCallable(const ActiveDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ActiveDeviceOutcome()>>(
			[this, request]()
			{
			return this->activeDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::ActiveMeetingOutcome AliyuncvcClient::activeMeeting(const ActiveMeetingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ActiveMeetingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ActiveMeetingOutcome(ActiveMeetingResult(outcome.result()));
	else
		return ActiveMeetingOutcome(outcome.error());
}

void AliyuncvcClient::activeMeetingAsync(const ActiveMeetingRequest& request, const ActiveMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, activeMeeting(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::ActiveMeetingOutcomeCallable AliyuncvcClient::activeMeetingCallable(const ActiveMeetingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ActiveMeetingOutcome()>>(
			[this, request]()
			{
			return this->activeMeeting(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::BatchCreateDeviceOutcome AliyuncvcClient::batchCreateDevice(const BatchCreateDeviceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchCreateDeviceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchCreateDeviceOutcome(BatchCreateDeviceResult(outcome.result()));
	else
		return BatchCreateDeviceOutcome(outcome.error());
}

void AliyuncvcClient::batchCreateDeviceAsync(const BatchCreateDeviceRequest& request, const BatchCreateDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchCreateDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::BatchCreateDeviceOutcomeCallable AliyuncvcClient::batchCreateDeviceCallable(const BatchCreateDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchCreateDeviceOutcome()>>(
			[this, request]()
			{
			return this->batchCreateDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::BatchDeleteDevicesOutcome AliyuncvcClient::batchDeleteDevices(const BatchDeleteDevicesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return BatchDeleteDevicesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return BatchDeleteDevicesOutcome(BatchDeleteDevicesResult(outcome.result()));
	else
		return BatchDeleteDevicesOutcome(outcome.error());
}

void AliyuncvcClient::batchDeleteDevicesAsync(const BatchDeleteDevicesRequest& request, const BatchDeleteDevicesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, batchDeleteDevices(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::BatchDeleteDevicesOutcomeCallable AliyuncvcClient::batchDeleteDevicesCallable(const BatchDeleteDevicesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<BatchDeleteDevicesOutcome()>>(
			[this, request]()
			{
			return this->batchDeleteDevices(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::CallDeviceOutcome AliyuncvcClient::callDevice(const CallDeviceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CallDeviceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CallDeviceOutcome(CallDeviceResult(outcome.result()));
	else
		return CallDeviceOutcome(outcome.error());
}

void AliyuncvcClient::callDeviceAsync(const CallDeviceRequest& request, const CallDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, callDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::CallDeviceOutcomeCallable AliyuncvcClient::callDeviceCallable(const CallDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CallDeviceOutcome()>>(
			[this, request]()
			{
			return this->callDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::ConferenceToLiveOutcome AliyuncvcClient::conferenceToLive(const ConferenceToLiveRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ConferenceToLiveOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ConferenceToLiveOutcome(ConferenceToLiveResult(outcome.result()));
	else
		return ConferenceToLiveOutcome(outcome.error());
}

void AliyuncvcClient::conferenceToLiveAsync(const ConferenceToLiveRequest& request, const ConferenceToLiveAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, conferenceToLive(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::ConferenceToLiveOutcomeCallable AliyuncvcClient::conferenceToLiveCallable(const ConferenceToLiveRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ConferenceToLiveOutcome()>>(
			[this, request]()
			{
			return this->conferenceToLive(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::CreateDeviceMeetingOutcome AliyuncvcClient::createDeviceMeeting(const CreateDeviceMeetingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateDeviceMeetingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateDeviceMeetingOutcome(CreateDeviceMeetingResult(outcome.result()));
	else
		return CreateDeviceMeetingOutcome(outcome.error());
}

void AliyuncvcClient::createDeviceMeetingAsync(const CreateDeviceMeetingRequest& request, const CreateDeviceMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createDeviceMeeting(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::CreateDeviceMeetingOutcomeCallable AliyuncvcClient::createDeviceMeetingCallable(const CreateDeviceMeetingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateDeviceMeetingOutcome()>>(
			[this, request]()
			{
			return this->createDeviceMeeting(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::CreateEvaluationOutcome AliyuncvcClient::createEvaluation(const CreateEvaluationRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateEvaluationOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateEvaluationOutcome(CreateEvaluationResult(outcome.result()));
	else
		return CreateEvaluationOutcome(outcome.error());
}

void AliyuncvcClient::createEvaluationAsync(const CreateEvaluationRequest& request, const CreateEvaluationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createEvaluation(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::CreateEvaluationOutcomeCallable AliyuncvcClient::createEvaluationCallable(const CreateEvaluationRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateEvaluationOutcome()>>(
			[this, request]()
			{
			return this->createEvaluation(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::CreateLiveOutcome AliyuncvcClient::createLive(const CreateLiveRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateLiveOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateLiveOutcome(CreateLiveResult(outcome.result()));
	else
		return CreateLiveOutcome(outcome.error());
}

void AliyuncvcClient::createLiveAsync(const CreateLiveRequest& request, const CreateLiveAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createLive(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::CreateLiveOutcomeCallable AliyuncvcClient::createLiveCallable(const CreateLiveRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateLiveOutcome()>>(
			[this, request]()
			{
			return this->createLive(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::CreateMeetingOutcome AliyuncvcClient::createMeeting(const CreateMeetingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateMeetingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateMeetingOutcome(CreateMeetingResult(outcome.result()));
	else
		return CreateMeetingOutcome(outcome.error());
}

void AliyuncvcClient::createMeetingAsync(const CreateMeetingRequest& request, const CreateMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createMeeting(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::CreateMeetingOutcomeCallable AliyuncvcClient::createMeetingCallable(const CreateMeetingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateMeetingOutcome()>>(
			[this, request]()
			{
			return this->createMeeting(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::CreateMeetingInternationalOutcome AliyuncvcClient::createMeetingInternational(const CreateMeetingInternationalRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateMeetingInternationalOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateMeetingInternationalOutcome(CreateMeetingInternationalResult(outcome.result()));
	else
		return CreateMeetingInternationalOutcome(outcome.error());
}

void AliyuncvcClient::createMeetingInternationalAsync(const CreateMeetingInternationalRequest& request, const CreateMeetingInternationalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createMeetingInternational(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::CreateMeetingInternationalOutcomeCallable AliyuncvcClient::createMeetingInternationalCallable(const CreateMeetingInternationalRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateMeetingInternationalOutcome()>>(
			[this, request]()
			{
			return this->createMeetingInternational(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::CreateUserOutcome AliyuncvcClient::createUser(const CreateUserRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateUserOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateUserOutcome(CreateUserResult(outcome.result()));
	else
		return CreateUserOutcome(outcome.error());
}

void AliyuncvcClient::createUserAsync(const CreateUserRequest& request, const CreateUserAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createUser(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::CreateUserOutcomeCallable AliyuncvcClient::createUserCallable(const CreateUserRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateUserOutcome()>>(
			[this, request]()
			{
			return this->createUser(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::CreateUserInternationalOutcome AliyuncvcClient::createUserInternational(const CreateUserInternationalRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateUserInternationalOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateUserInternationalOutcome(CreateUserInternationalResult(outcome.result()));
	else
		return CreateUserInternationalOutcome(outcome.error());
}

void AliyuncvcClient::createUserInternationalAsync(const CreateUserInternationalRequest& request, const CreateUserInternationalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createUserInternational(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::CreateUserInternationalOutcomeCallable AliyuncvcClient::createUserInternationalCallable(const CreateUserInternationalRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateUserInternationalOutcome()>>(
			[this, request]()
			{
			return this->createUserInternational(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::CustomGonggeLayoutOutcome AliyuncvcClient::customGonggeLayout(const CustomGonggeLayoutRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CustomGonggeLayoutOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CustomGonggeLayoutOutcome(CustomGonggeLayoutResult(outcome.result()));
	else
		return CustomGonggeLayoutOutcome(outcome.error());
}

void AliyuncvcClient::customGonggeLayoutAsync(const CustomGonggeLayoutRequest& request, const CustomGonggeLayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, customGonggeLayout(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::CustomGonggeLayoutOutcomeCallable AliyuncvcClient::customGonggeLayoutCallable(const CustomGonggeLayoutRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CustomGonggeLayoutOutcome()>>(
			[this, request]()
			{
			return this->customGonggeLayout(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::CustomLayoutOutcome AliyuncvcClient::customLayout(const CustomLayoutRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CustomLayoutOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CustomLayoutOutcome(CustomLayoutResult(outcome.result()));
	else
		return CustomLayoutOutcome(outcome.error());
}

void AliyuncvcClient::customLayoutAsync(const CustomLayoutRequest& request, const CustomLayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, customLayout(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::CustomLayoutOutcomeCallable AliyuncvcClient::customLayoutCallable(const CustomLayoutRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CustomLayoutOutcome()>>(
			[this, request]()
			{
			return this->customLayout(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::DeleteDeviceOutcome AliyuncvcClient::deleteDevice(const DeleteDeviceRequest &request) const
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

void AliyuncvcClient::deleteDeviceAsync(const DeleteDeviceRequest& request, const DeleteDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::DeleteDeviceOutcomeCallable AliyuncvcClient::deleteDeviceCallable(const DeleteDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteDeviceOutcome()>>(
			[this, request]()
			{
			return this->deleteDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::DeleteLiveOutcome AliyuncvcClient::deleteLive(const DeleteLiveRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteLiveOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteLiveOutcome(DeleteLiveResult(outcome.result()));
	else
		return DeleteLiveOutcome(outcome.error());
}

void AliyuncvcClient::deleteLiveAsync(const DeleteLiveRequest& request, const DeleteLiveAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteLive(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::DeleteLiveOutcomeCallable AliyuncvcClient::deleteLiveCallable(const DeleteLiveRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteLiveOutcome()>>(
			[this, request]()
			{
			return this->deleteLive(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::DeleteMeetingOutcome AliyuncvcClient::deleteMeeting(const DeleteMeetingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteMeetingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteMeetingOutcome(DeleteMeetingResult(outcome.result()));
	else
		return DeleteMeetingOutcome(outcome.error());
}

void AliyuncvcClient::deleteMeetingAsync(const DeleteMeetingRequest& request, const DeleteMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteMeeting(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::DeleteMeetingOutcomeCallable AliyuncvcClient::deleteMeetingCallable(const DeleteMeetingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteMeetingOutcome()>>(
			[this, request]()
			{
			return this->deleteMeeting(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::DeleteUserOutcome AliyuncvcClient::deleteUser(const DeleteUserRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteUserOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteUserOutcome(DeleteUserResult(outcome.result()));
	else
		return DeleteUserOutcome(outcome.error());
}

void AliyuncvcClient::deleteUserAsync(const DeleteUserRequest& request, const DeleteUserAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteUser(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::DeleteUserOutcomeCallable AliyuncvcClient::deleteUserCallable(const DeleteUserRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteUserOutcome()>>(
			[this, request]()
			{
			return this->deleteUser(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::EnableLiveSpeakerOutcome AliyuncvcClient::enableLiveSpeaker(const EnableLiveSpeakerRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return EnableLiveSpeakerOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return EnableLiveSpeakerOutcome(EnableLiveSpeakerResult(outcome.result()));
	else
		return EnableLiveSpeakerOutcome(outcome.error());
}

void AliyuncvcClient::enableLiveSpeakerAsync(const EnableLiveSpeakerRequest& request, const EnableLiveSpeakerAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, enableLiveSpeaker(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::EnableLiveSpeakerOutcomeCallable AliyuncvcClient::enableLiveSpeakerCallable(const EnableLiveSpeakerRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<EnableLiveSpeakerOutcome()>>(
			[this, request]()
			{
			return this->enableLiveSpeaker(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::EndDeviceMeetingOutcome AliyuncvcClient::endDeviceMeeting(const EndDeviceMeetingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return EndDeviceMeetingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return EndDeviceMeetingOutcome(EndDeviceMeetingResult(outcome.result()));
	else
		return EndDeviceMeetingOutcome(outcome.error());
}

void AliyuncvcClient::endDeviceMeetingAsync(const EndDeviceMeetingRequest& request, const EndDeviceMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, endDeviceMeeting(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::EndDeviceMeetingOutcomeCallable AliyuncvcClient::endDeviceMeetingCallable(const EndDeviceMeetingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<EndDeviceMeetingOutcome()>>(
			[this, request]()
			{
			return this->endDeviceMeeting(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::EndLiveOutcome AliyuncvcClient::endLive(const EndLiveRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return EndLiveOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return EndLiveOutcome(EndLiveResult(outcome.result()));
	else
		return EndLiveOutcome(outcome.error());
}

void AliyuncvcClient::endLiveAsync(const EndLiveRequest& request, const EndLiveAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, endLive(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::EndLiveOutcomeCallable AliyuncvcClient::endLiveCallable(const EndLiveRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<EndLiveOutcome()>>(
			[this, request]()
			{
			return this->endLive(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::GetAccountInfoOutcome AliyuncvcClient::getAccountInfo(const GetAccountInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetAccountInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetAccountInfoOutcome(GetAccountInfoResult(outcome.result()));
	else
		return GetAccountInfoOutcome(outcome.error());
}

void AliyuncvcClient::getAccountInfoAsync(const GetAccountInfoRequest& request, const GetAccountInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getAccountInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::GetAccountInfoOutcomeCallable AliyuncvcClient::getAccountInfoCallable(const GetAccountInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetAccountInfoOutcome()>>(
			[this, request]()
			{
			return this->getAccountInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::GetDeviceActiveCodeOutcome AliyuncvcClient::getDeviceActiveCode(const GetDeviceActiveCodeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDeviceActiveCodeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDeviceActiveCodeOutcome(GetDeviceActiveCodeResult(outcome.result()));
	else
		return GetDeviceActiveCodeOutcome(outcome.error());
}

void AliyuncvcClient::getDeviceActiveCodeAsync(const GetDeviceActiveCodeRequest& request, const GetDeviceActiveCodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDeviceActiveCode(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::GetDeviceActiveCodeOutcomeCallable AliyuncvcClient::getDeviceActiveCodeCallable(const GetDeviceActiveCodeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDeviceActiveCodeOutcome()>>(
			[this, request]()
			{
			return this->getDeviceActiveCode(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::GetDeviceInfoOutcome AliyuncvcClient::getDeviceInfo(const GetDeviceInfoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDeviceInfoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDeviceInfoOutcome(GetDeviceInfoResult(outcome.result()));
	else
		return GetDeviceInfoOutcome(outcome.error());
}

void AliyuncvcClient::getDeviceInfoAsync(const GetDeviceInfoRequest& request, const GetDeviceInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDeviceInfo(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::GetDeviceInfoOutcomeCallable AliyuncvcClient::getDeviceInfoCallable(const GetDeviceInfoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDeviceInfoOutcome()>>(
			[this, request]()
			{
			return this->getDeviceInfo(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::GetDeviceTokenOutcome AliyuncvcClient::getDeviceToken(const GetDeviceTokenRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDeviceTokenOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDeviceTokenOutcome(GetDeviceTokenResult(outcome.result()));
	else
		return GetDeviceTokenOutcome(outcome.error());
}

void AliyuncvcClient::getDeviceTokenAsync(const GetDeviceTokenRequest& request, const GetDeviceTokenAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDeviceToken(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::GetDeviceTokenOutcomeCallable AliyuncvcClient::getDeviceTokenCallable(const GetDeviceTokenRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDeviceTokenOutcome()>>(
			[this, request]()
			{
			return this->getDeviceToken(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::GetMeetingOutcome AliyuncvcClient::getMeeting(const GetMeetingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetMeetingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetMeetingOutcome(GetMeetingResult(outcome.result()));
	else
		return GetMeetingOutcome(outcome.error());
}

void AliyuncvcClient::getMeetingAsync(const GetMeetingRequest& request, const GetMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getMeeting(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::GetMeetingOutcomeCallable AliyuncvcClient::getMeetingCallable(const GetMeetingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetMeetingOutcome()>>(
			[this, request]()
			{
			return this->getMeeting(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::GetMeetingInternationalOutcome AliyuncvcClient::getMeetingInternational(const GetMeetingInternationalRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetMeetingInternationalOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetMeetingInternationalOutcome(GetMeetingInternationalResult(outcome.result()));
	else
		return GetMeetingInternationalOutcome(outcome.error());
}

void AliyuncvcClient::getMeetingInternationalAsync(const GetMeetingInternationalRequest& request, const GetMeetingInternationalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getMeetingInternational(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::GetMeetingInternationalOutcomeCallable AliyuncvcClient::getMeetingInternationalCallable(const GetMeetingInternationalRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetMeetingInternationalOutcome()>>(
			[this, request]()
			{
			return this->getMeetingInternational(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::GetMeetingMemberOutcome AliyuncvcClient::getMeetingMember(const GetMeetingMemberRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetMeetingMemberOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetMeetingMemberOutcome(GetMeetingMemberResult(outcome.result()));
	else
		return GetMeetingMemberOutcome(outcome.error());
}

void AliyuncvcClient::getMeetingMemberAsync(const GetMeetingMemberRequest& request, const GetMeetingMemberAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getMeetingMember(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::GetMeetingMemberOutcomeCallable AliyuncvcClient::getMeetingMemberCallable(const GetMeetingMemberRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetMeetingMemberOutcome()>>(
			[this, request]()
			{
			return this->getMeetingMember(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::GetMembersOutcome AliyuncvcClient::getMembers(const GetMembersRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetMembersOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetMembersOutcome(GetMembersResult(outcome.result()));
	else
		return GetMembersOutcome(outcome.error());
}

void AliyuncvcClient::getMembersAsync(const GetMembersRequest& request, const GetMembersAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getMembers(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::GetMembersOutcomeCallable AliyuncvcClient::getMembersCallable(const GetMembersRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetMembersOutcome()>>(
			[this, request]()
			{
			return this->getMembers(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::GetStatisticOutcome AliyuncvcClient::getStatistic(const GetStatisticRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetStatisticOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetStatisticOutcome(GetStatisticResult(outcome.result()));
	else
		return GetStatisticOutcome(outcome.error());
}

void AliyuncvcClient::getStatisticAsync(const GetStatisticRequest& request, const GetStatisticAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getStatistic(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::GetStatisticOutcomeCallable AliyuncvcClient::getStatisticCallable(const GetStatisticRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetStatisticOutcome()>>(
			[this, request]()
			{
			return this->getStatistic(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::GetUserOutcome AliyuncvcClient::getUser(const GetUserRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetUserOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetUserOutcome(GetUserResult(outcome.result()));
	else
		return GetUserOutcome(outcome.error());
}

void AliyuncvcClient::getUserAsync(const GetUserRequest& request, const GetUserAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getUser(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::GetUserOutcomeCallable AliyuncvcClient::getUserCallable(const GetUserRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetUserOutcome()>>(
			[this, request]()
			{
			return this->getUser(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::InviteUserOutcome AliyuncvcClient::inviteUser(const InviteUserRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return InviteUserOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return InviteUserOutcome(InviteUserResult(outcome.result()));
	else
		return InviteUserOutcome(outcome.error());
}

void AliyuncvcClient::inviteUserAsync(const InviteUserRequest& request, const InviteUserAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, inviteUser(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::InviteUserOutcomeCallable AliyuncvcClient::inviteUserCallable(const InviteUserRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<InviteUserOutcome()>>(
			[this, request]()
			{
			return this->inviteUser(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::JoinDeviceMeetingOutcome AliyuncvcClient::joinDeviceMeeting(const JoinDeviceMeetingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return JoinDeviceMeetingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return JoinDeviceMeetingOutcome(JoinDeviceMeetingResult(outcome.result()));
	else
		return JoinDeviceMeetingOutcome(outcome.error());
}

void AliyuncvcClient::joinDeviceMeetingAsync(const JoinDeviceMeetingRequest& request, const JoinDeviceMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, joinDeviceMeeting(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::JoinDeviceMeetingOutcomeCallable AliyuncvcClient::joinDeviceMeetingCallable(const JoinDeviceMeetingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<JoinDeviceMeetingOutcome()>>(
			[this, request]()
			{
			return this->joinDeviceMeeting(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::JoinLiveOutcome AliyuncvcClient::joinLive(const JoinLiveRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return JoinLiveOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return JoinLiveOutcome(JoinLiveResult(outcome.result()));
	else
		return JoinLiveOutcome(outcome.error());
}

void AliyuncvcClient::joinLiveAsync(const JoinLiveRequest& request, const JoinLiveAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, joinLive(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::JoinLiveOutcomeCallable AliyuncvcClient::joinLiveCallable(const JoinLiveRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<JoinLiveOutcome()>>(
			[this, request]()
			{
			return this->joinLive(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::JoinMeetingOutcome AliyuncvcClient::joinMeeting(const JoinMeetingRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return JoinMeetingOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return JoinMeetingOutcome(JoinMeetingResult(outcome.result()));
	else
		return JoinMeetingOutcome(outcome.error());
}

void AliyuncvcClient::joinMeetingAsync(const JoinMeetingRequest& request, const JoinMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, joinMeeting(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::JoinMeetingOutcomeCallable AliyuncvcClient::joinMeetingCallable(const JoinMeetingRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<JoinMeetingOutcome()>>(
			[this, request]()
			{
			return this->joinMeeting(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::JoinMeetingInternationalOutcome AliyuncvcClient::joinMeetingInternational(const JoinMeetingInternationalRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return JoinMeetingInternationalOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return JoinMeetingInternationalOutcome(JoinMeetingInternationalResult(outcome.result()));
	else
		return JoinMeetingInternationalOutcome(outcome.error());
}

void AliyuncvcClient::joinMeetingInternationalAsync(const JoinMeetingInternationalRequest& request, const JoinMeetingInternationalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, joinMeetingInternational(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::JoinMeetingInternationalOutcomeCallable AliyuncvcClient::joinMeetingInternationalCallable(const JoinMeetingInternationalRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<JoinMeetingInternationalOutcome()>>(
			[this, request]()
			{
			return this->joinMeetingInternational(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::ListConferenceDevicesOutcome AliyuncvcClient::listConferenceDevices(const ListConferenceDevicesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListConferenceDevicesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListConferenceDevicesOutcome(ListConferenceDevicesResult(outcome.result()));
	else
		return ListConferenceDevicesOutcome(outcome.error());
}

void AliyuncvcClient::listConferenceDevicesAsync(const ListConferenceDevicesRequest& request, const ListConferenceDevicesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listConferenceDevices(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::ListConferenceDevicesOutcomeCallable AliyuncvcClient::listConferenceDevicesCallable(const ListConferenceDevicesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListConferenceDevicesOutcome()>>(
			[this, request]()
			{
			return this->listConferenceDevices(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::ListDeviceIpOutcome AliyuncvcClient::listDeviceIp(const ListDeviceIpRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListDeviceIpOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListDeviceIpOutcome(ListDeviceIpResult(outcome.result()));
	else
		return ListDeviceIpOutcome(outcome.error());
}

void AliyuncvcClient::listDeviceIpAsync(const ListDeviceIpRequest& request, const ListDeviceIpAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listDeviceIp(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::ListDeviceIpOutcomeCallable AliyuncvcClient::listDeviceIpCallable(const ListDeviceIpRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListDeviceIpOutcome()>>(
			[this, request]()
			{
			return this->listDeviceIp(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::ListDevicesOutcome AliyuncvcClient::listDevices(const ListDevicesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListDevicesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListDevicesOutcome(ListDevicesResult(outcome.result()));
	else
		return ListDevicesOutcome(outcome.error());
}

void AliyuncvcClient::listDevicesAsync(const ListDevicesRequest& request, const ListDevicesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listDevices(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::ListDevicesOutcomeCallable AliyuncvcClient::listDevicesCallable(const ListDevicesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListDevicesOutcome()>>(
			[this, request]()
			{
			return this->listDevices(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::ListEvaluationsOutcome AliyuncvcClient::listEvaluations(const ListEvaluationsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListEvaluationsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListEvaluationsOutcome(ListEvaluationsResult(outcome.result()));
	else
		return ListEvaluationsOutcome(outcome.error());
}

void AliyuncvcClient::listEvaluationsAsync(const ListEvaluationsRequest& request, const ListEvaluationsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listEvaluations(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::ListEvaluationsOutcomeCallable AliyuncvcClient::listEvaluationsCallable(const ListEvaluationsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListEvaluationsOutcome()>>(
			[this, request]()
			{
			return this->listEvaluations(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::ListIsvStatisticsOutcome AliyuncvcClient::listIsvStatistics(const ListIsvStatisticsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListIsvStatisticsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListIsvStatisticsOutcome(ListIsvStatisticsResult(outcome.result()));
	else
		return ListIsvStatisticsOutcome(outcome.error());
}

void AliyuncvcClient::listIsvStatisticsAsync(const ListIsvStatisticsRequest& request, const ListIsvStatisticsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listIsvStatistics(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::ListIsvStatisticsOutcomeCallable AliyuncvcClient::listIsvStatisticsCallable(const ListIsvStatisticsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListIsvStatisticsOutcome()>>(
			[this, request]()
			{
			return this->listIsvStatistics(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::ListMembersOutcome AliyuncvcClient::listMembers(const ListMembersRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListMembersOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListMembersOutcome(ListMembersResult(outcome.result()));
	else
		return ListMembersOutcome(outcome.error());
}

void AliyuncvcClient::listMembersAsync(const ListMembersRequest& request, const ListMembersAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listMembers(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::ListMembersOutcomeCallable AliyuncvcClient::listMembersCallable(const ListMembersRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListMembersOutcome()>>(
			[this, request]()
			{
			return this->listMembers(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::ListUsersOutcome AliyuncvcClient::listUsers(const ListUsersRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListUsersOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListUsersOutcome(ListUsersResult(outcome.result()));
	else
		return ListUsersOutcome(outcome.error());
}

void AliyuncvcClient::listUsersAsync(const ListUsersRequest& request, const ListUsersAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listUsers(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::ListUsersOutcomeCallable AliyuncvcClient::listUsersCallable(const ListUsersRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListUsersOutcome()>>(
			[this, request]()
			{
			return this->listUsers(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::ModifyDeviceBackgroundOutcome AliyuncvcClient::modifyDeviceBackground(const ModifyDeviceBackgroundRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyDeviceBackgroundOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyDeviceBackgroundOutcome(ModifyDeviceBackgroundResult(outcome.result()));
	else
		return ModifyDeviceBackgroundOutcome(outcome.error());
}

void AliyuncvcClient::modifyDeviceBackgroundAsync(const ModifyDeviceBackgroundRequest& request, const ModifyDeviceBackgroundAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyDeviceBackground(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::ModifyDeviceBackgroundOutcomeCallable AliyuncvcClient::modifyDeviceBackgroundCallable(const ModifyDeviceBackgroundRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyDeviceBackgroundOutcome()>>(
			[this, request]()
			{
			return this->modifyDeviceBackground(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::ModifyMeetingPasswordOutcome AliyuncvcClient::modifyMeetingPassword(const ModifyMeetingPasswordRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyMeetingPasswordOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyMeetingPasswordOutcome(ModifyMeetingPasswordResult(outcome.result()));
	else
		return ModifyMeetingPasswordOutcome(outcome.error());
}

void AliyuncvcClient::modifyMeetingPasswordAsync(const ModifyMeetingPasswordRequest& request, const ModifyMeetingPasswordAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyMeetingPassword(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::ModifyMeetingPasswordOutcomeCallable AliyuncvcClient::modifyMeetingPasswordCallable(const ModifyMeetingPasswordRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyMeetingPasswordOutcome()>>(
			[this, request]()
			{
			return this->modifyMeetingPassword(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::ModifyMeetingPasswordInternationalOutcome AliyuncvcClient::modifyMeetingPasswordInternational(const ModifyMeetingPasswordInternationalRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ModifyMeetingPasswordInternationalOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ModifyMeetingPasswordInternationalOutcome(ModifyMeetingPasswordInternationalResult(outcome.result()));
	else
		return ModifyMeetingPasswordInternationalOutcome(outcome.error());
}

void AliyuncvcClient::modifyMeetingPasswordInternationalAsync(const ModifyMeetingPasswordInternationalRequest& request, const ModifyMeetingPasswordInternationalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, modifyMeetingPasswordInternational(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::ModifyMeetingPasswordInternationalOutcomeCallable AliyuncvcClient::modifyMeetingPasswordInternationalCallable(const ModifyMeetingPasswordInternationalRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ModifyMeetingPasswordInternationalOutcome()>>(
			[this, request]()
			{
			return this->modifyMeetingPasswordInternational(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::RefreshDeviceScreenCodeOutcome AliyuncvcClient::refreshDeviceScreenCode(const RefreshDeviceScreenCodeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RefreshDeviceScreenCodeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RefreshDeviceScreenCodeOutcome(RefreshDeviceScreenCodeResult(outcome.result()));
	else
		return RefreshDeviceScreenCodeOutcome(outcome.error());
}

void AliyuncvcClient::refreshDeviceScreenCodeAsync(const RefreshDeviceScreenCodeRequest& request, const RefreshDeviceScreenCodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, refreshDeviceScreenCode(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::RefreshDeviceScreenCodeOutcomeCallable AliyuncvcClient::refreshDeviceScreenCodeCallable(const RefreshDeviceScreenCodeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RefreshDeviceScreenCodeOutcome()>>(
			[this, request]()
			{
			return this->refreshDeviceScreenCode(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::RegisterDeviceOutcome AliyuncvcClient::registerDevice(const RegisterDeviceRequest &request) const
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

void AliyuncvcClient::registerDeviceAsync(const RegisterDeviceRequest& request, const RegisterDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, registerDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::RegisterDeviceOutcomeCallable AliyuncvcClient::registerDeviceCallable(const RegisterDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RegisterDeviceOutcome()>>(
			[this, request]()
			{
			return this->registerDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::RegisterUemDeviceOutcome AliyuncvcClient::registerUemDevice(const RegisterUemDeviceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RegisterUemDeviceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RegisterUemDeviceOutcome(RegisterUemDeviceResult(outcome.result()));
	else
		return RegisterUemDeviceOutcome(outcome.error());
}

void AliyuncvcClient::registerUemDeviceAsync(const RegisterUemDeviceRequest& request, const RegisterUemDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, registerUemDevice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::RegisterUemDeviceOutcomeCallable AliyuncvcClient::registerUemDeviceCallable(const RegisterUemDeviceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RegisterUemDeviceOutcome()>>(
			[this, request]()
			{
			return this->registerUemDevice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::SendMeetingCommandOutcome AliyuncvcClient::sendMeetingCommand(const SendMeetingCommandRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SendMeetingCommandOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SendMeetingCommandOutcome(SendMeetingCommandResult(outcome.result()));
	else
		return SendMeetingCommandOutcome(outcome.error());
}

void AliyuncvcClient::sendMeetingCommandAsync(const SendMeetingCommandRequest& request, const SendMeetingCommandAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, sendMeetingCommand(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::SendMeetingCommandOutcomeCallable AliyuncvcClient::sendMeetingCommandCallable(const SendMeetingCommandRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SendMeetingCommandOutcome()>>(
			[this, request]()
			{
			return this->sendMeetingCommand(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::StartLiveOutcome AliyuncvcClient::startLive(const StartLiveRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartLiveOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartLiveOutcome(StartLiveResult(outcome.result()));
	else
		return StartLiveOutcome(outcome.error());
}

void AliyuncvcClient::startLiveAsync(const StartLiveRequest& request, const StartLiveAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startLive(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::StartLiveOutcomeCallable AliyuncvcClient::startLiveCallable(const StartLiveRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartLiveOutcome()>>(
			[this, request]()
			{
			return this->startLive(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::UpdateDeviceHeartBeatOutcome AliyuncvcClient::updateDeviceHeartBeat(const UpdateDeviceHeartBeatRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateDeviceHeartBeatOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateDeviceHeartBeatOutcome(UpdateDeviceHeartBeatResult(outcome.result()));
	else
		return UpdateDeviceHeartBeatOutcome(outcome.error());
}

void AliyuncvcClient::updateDeviceHeartBeatAsync(const UpdateDeviceHeartBeatRequest& request, const UpdateDeviceHeartBeatAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateDeviceHeartBeat(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::UpdateDeviceHeartBeatOutcomeCallable AliyuncvcClient::updateDeviceHeartBeatCallable(const UpdateDeviceHeartBeatRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateDeviceHeartBeatOutcome()>>(
			[this, request]()
			{
			return this->updateDeviceHeartBeat(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::UpdateDeviceStartupPictureOutcome AliyuncvcClient::updateDeviceStartupPicture(const UpdateDeviceStartupPictureRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateDeviceStartupPictureOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateDeviceStartupPictureOutcome(UpdateDeviceStartupPictureResult(outcome.result()));
	else
		return UpdateDeviceStartupPictureOutcome(outcome.error());
}

void AliyuncvcClient::updateDeviceStartupPictureAsync(const UpdateDeviceStartupPictureRequest& request, const UpdateDeviceStartupPictureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateDeviceStartupPicture(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::UpdateDeviceStartupPictureOutcomeCallable AliyuncvcClient::updateDeviceStartupPictureCallable(const UpdateDeviceStartupPictureRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateDeviceStartupPictureOutcome()>>(
			[this, request]()
			{
			return this->updateDeviceStartupPicture(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::UpdateGonggeLayoutOutcome AliyuncvcClient::updateGonggeLayout(const UpdateGonggeLayoutRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateGonggeLayoutOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateGonggeLayoutOutcome(UpdateGonggeLayoutResult(outcome.result()));
	else
		return UpdateGonggeLayoutOutcome(outcome.error());
}

void AliyuncvcClient::updateGonggeLayoutAsync(const UpdateGonggeLayoutRequest& request, const UpdateGonggeLayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateGonggeLayout(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::UpdateGonggeLayoutOutcomeCallable AliyuncvcClient::updateGonggeLayoutCallable(const UpdateGonggeLayoutRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateGonggeLayoutOutcome()>>(
			[this, request]()
			{
			return this->updateGonggeLayout(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

AliyuncvcClient::UpdateLivePasswordOutcome AliyuncvcClient::updateLivePassword(const UpdateLivePasswordRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateLivePasswordOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateLivePasswordOutcome(UpdateLivePasswordResult(outcome.result()));
	else
		return UpdateLivePasswordOutcome(outcome.error());
}

void AliyuncvcClient::updateLivePasswordAsync(const UpdateLivePasswordRequest& request, const UpdateLivePasswordAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateLivePassword(request), context);
	};

	asyncExecute(new Runnable(fn));
}

AliyuncvcClient::UpdateLivePasswordOutcomeCallable AliyuncvcClient::updateLivePasswordCallable(const UpdateLivePasswordRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateLivePasswordOutcome()>>(
			[this, request]()
			{
			return this->updateLivePassword(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


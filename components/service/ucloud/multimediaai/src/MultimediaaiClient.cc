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

#include <alibabacloud/multimediaai/MultimediaaiClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Multimediaai;
using namespace AlibabaCloud::Multimediaai::Model;

namespace
{
	const std::string SERVICE_NAME = "multimediaai";
}

MultimediaaiClient::MultimediaaiClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "");
}

MultimediaaiClient::MultimediaaiClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "");
}

MultimediaaiClient::MultimediaaiClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "");
}

MultimediaaiClient::~MultimediaaiClient()
{}

MultimediaaiClient::CreateCoverTaskOutcome MultimediaaiClient::createCoverTask(const CreateCoverTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateCoverTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateCoverTaskOutcome(CreateCoverTaskResult(outcome.result()));
	else
		return CreateCoverTaskOutcome(outcome.error());
}

void MultimediaaiClient::createCoverTaskAsync(const CreateCoverTaskRequest& request, const CreateCoverTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createCoverTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::CreateCoverTaskOutcomeCallable MultimediaaiClient::createCoverTaskCallable(const CreateCoverTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateCoverTaskOutcome()>>(
			[this, request]()
			{
			return this->createCoverTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::CreateFaceGroupOutcome MultimediaaiClient::createFaceGroup(const CreateFaceGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateFaceGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateFaceGroupOutcome(CreateFaceGroupResult(outcome.result()));
	else
		return CreateFaceGroupOutcome(outcome.error());
}

void MultimediaaiClient::createFaceGroupAsync(const CreateFaceGroupRequest& request, const CreateFaceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createFaceGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::CreateFaceGroupOutcomeCallable MultimediaaiClient::createFaceGroupCallable(const CreateFaceGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateFaceGroupOutcome()>>(
			[this, request]()
			{
			return this->createFaceGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::CreateFacePersonOutcome MultimediaaiClient::createFacePerson(const CreateFacePersonRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateFacePersonOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateFacePersonOutcome(CreateFacePersonResult(outcome.result()));
	else
		return CreateFacePersonOutcome(outcome.error());
}

void MultimediaaiClient::createFacePersonAsync(const CreateFacePersonRequest& request, const CreateFacePersonAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createFacePerson(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::CreateFacePersonOutcomeCallable MultimediaaiClient::createFacePersonCallable(const CreateFacePersonRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateFacePersonOutcome()>>(
			[this, request]()
			{
			return this->createFacePerson(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::CreateGifTaskOutcome MultimediaaiClient::createGifTask(const CreateGifTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateGifTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateGifTaskOutcome(CreateGifTaskResult(outcome.result()));
	else
		return CreateGifTaskOutcome(outcome.error());
}

void MultimediaaiClient::createGifTaskAsync(const CreateGifTaskRequest& request, const CreateGifTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createGifTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::CreateGifTaskOutcomeCallable MultimediaaiClient::createGifTaskCallable(const CreateGifTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateGifTaskOutcome()>>(
			[this, request]()
			{
			return this->createGifTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::CreateLabelTaskOutcome MultimediaaiClient::createLabelTask(const CreateLabelTaskRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateLabelTaskOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateLabelTaskOutcome(CreateLabelTaskResult(outcome.result()));
	else
		return CreateLabelTaskOutcome(outcome.error());
}

void MultimediaaiClient::createLabelTaskAsync(const CreateLabelTaskRequest& request, const CreateLabelTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createLabelTask(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::CreateLabelTaskOutcomeCallable MultimediaaiClient::createLabelTaskCallable(const CreateLabelTaskRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateLabelTaskOutcome()>>(
			[this, request]()
			{
			return this->createLabelTask(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::CreateTemplateOutcome MultimediaaiClient::createTemplate(const CreateTemplateRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateTemplateOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateTemplateOutcome(CreateTemplateResult(outcome.result()));
	else
		return CreateTemplateOutcome(outcome.error());
}

void MultimediaaiClient::createTemplateAsync(const CreateTemplateRequest& request, const CreateTemplateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createTemplate(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::CreateTemplateOutcomeCallable MultimediaaiClient::createTemplateCallable(const CreateTemplateRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateTemplateOutcome()>>(
			[this, request]()
			{
			return this->createTemplate(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::DeleteFaceGroupOutcome MultimediaaiClient::deleteFaceGroup(const DeleteFaceGroupRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteFaceGroupOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteFaceGroupOutcome(DeleteFaceGroupResult(outcome.result()));
	else
		return DeleteFaceGroupOutcome(outcome.error());
}

void MultimediaaiClient::deleteFaceGroupAsync(const DeleteFaceGroupRequest& request, const DeleteFaceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteFaceGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::DeleteFaceGroupOutcomeCallable MultimediaaiClient::deleteFaceGroupCallable(const DeleteFaceGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteFaceGroupOutcome()>>(
			[this, request]()
			{
			return this->deleteFaceGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::DeleteFaceImageOutcome MultimediaaiClient::deleteFaceImage(const DeleteFaceImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteFaceImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteFaceImageOutcome(DeleteFaceImageResult(outcome.result()));
	else
		return DeleteFaceImageOutcome(outcome.error());
}

void MultimediaaiClient::deleteFaceImageAsync(const DeleteFaceImageRequest& request, const DeleteFaceImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteFaceImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::DeleteFaceImageOutcomeCallable MultimediaaiClient::deleteFaceImageCallable(const DeleteFaceImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteFaceImageOutcome()>>(
			[this, request]()
			{
			return this->deleteFaceImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::DeleteFacePersonOutcome MultimediaaiClient::deleteFacePerson(const DeleteFacePersonRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteFacePersonOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteFacePersonOutcome(DeleteFacePersonResult(outcome.result()));
	else
		return DeleteFacePersonOutcome(outcome.error());
}

void MultimediaaiClient::deleteFacePersonAsync(const DeleteFacePersonRequest& request, const DeleteFacePersonAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteFacePerson(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::DeleteFacePersonOutcomeCallable MultimediaaiClient::deleteFacePersonCallable(const DeleteFacePersonRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteFacePersonOutcome()>>(
			[this, request]()
			{
			return this->deleteFacePerson(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::GetTaskResultOutcome MultimediaaiClient::getTaskResult(const GetTaskResultRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetTaskResultOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetTaskResultOutcome(GetTaskResultResult(outcome.result()));
	else
		return GetTaskResultOutcome(outcome.error());
}

void MultimediaaiClient::getTaskResultAsync(const GetTaskResultRequest& request, const GetTaskResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getTaskResult(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::GetTaskResultOutcomeCallable MultimediaaiClient::getTaskResultCallable(const GetTaskResultRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetTaskResultOutcome()>>(
			[this, request]()
			{
			return this->getTaskResult(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::GetTaskStatusOutcome MultimediaaiClient::getTaskStatus(const GetTaskStatusRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetTaskStatusOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetTaskStatusOutcome(GetTaskStatusResult(outcome.result()));
	else
		return GetTaskStatusOutcome(outcome.error());
}

void MultimediaaiClient::getTaskStatusAsync(const GetTaskStatusRequest& request, const GetTaskStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getTaskStatus(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::GetTaskStatusOutcomeCallable MultimediaaiClient::getTaskStatusCallable(const GetTaskStatusRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetTaskStatusOutcome()>>(
			[this, request]()
			{
			return this->getTaskStatus(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::GetTemplateOutcome MultimediaaiClient::getTemplate(const GetTemplateRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetTemplateOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetTemplateOutcome(GetTemplateResult(outcome.result()));
	else
		return GetTemplateOutcome(outcome.error());
}

void MultimediaaiClient::getTemplateAsync(const GetTemplateRequest& request, const GetTemplateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getTemplate(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::GetTemplateOutcomeCallable MultimediaaiClient::getTemplateCallable(const GetTemplateRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetTemplateOutcome()>>(
			[this, request]()
			{
			return this->getTemplate(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::ListFaceGroupsOutcome MultimediaaiClient::listFaceGroups(const ListFaceGroupsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListFaceGroupsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListFaceGroupsOutcome(ListFaceGroupsResult(outcome.result()));
	else
		return ListFaceGroupsOutcome(outcome.error());
}

void MultimediaaiClient::listFaceGroupsAsync(const ListFaceGroupsRequest& request, const ListFaceGroupsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listFaceGroups(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::ListFaceGroupsOutcomeCallable MultimediaaiClient::listFaceGroupsCallable(const ListFaceGroupsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListFaceGroupsOutcome()>>(
			[this, request]()
			{
			return this->listFaceGroups(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::ListFaceImagesOutcome MultimediaaiClient::listFaceImages(const ListFaceImagesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListFaceImagesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListFaceImagesOutcome(ListFaceImagesResult(outcome.result()));
	else
		return ListFaceImagesOutcome(outcome.error());
}

void MultimediaaiClient::listFaceImagesAsync(const ListFaceImagesRequest& request, const ListFaceImagesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listFaceImages(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::ListFaceImagesOutcomeCallable MultimediaaiClient::listFaceImagesCallable(const ListFaceImagesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListFaceImagesOutcome()>>(
			[this, request]()
			{
			return this->listFaceImages(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::ListFacePersonsOutcome MultimediaaiClient::listFacePersons(const ListFacePersonsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListFacePersonsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListFacePersonsOutcome(ListFacePersonsResult(outcome.result()));
	else
		return ListFacePersonsOutcome(outcome.error());
}

void MultimediaaiClient::listFacePersonsAsync(const ListFacePersonsRequest& request, const ListFacePersonsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listFacePersons(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::ListFacePersonsOutcomeCallable MultimediaaiClient::listFacePersonsCallable(const ListFacePersonsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListFacePersonsOutcome()>>(
			[this, request]()
			{
			return this->listFacePersons(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::ListTemplatesOutcome MultimediaaiClient::listTemplates(const ListTemplatesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListTemplatesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListTemplatesOutcome(ListTemplatesResult(outcome.result()));
	else
		return ListTemplatesOutcome(outcome.error());
}

void MultimediaaiClient::listTemplatesAsync(const ListTemplatesRequest& request, const ListTemplatesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listTemplates(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::ListTemplatesOutcomeCallable MultimediaaiClient::listTemplatesCallable(const ListTemplatesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListTemplatesOutcome()>>(
			[this, request]()
			{
			return this->listTemplates(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::ProcessFaceAlgorithmOutcome MultimediaaiClient::processFaceAlgorithm(const ProcessFaceAlgorithmRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ProcessFaceAlgorithmOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ProcessFaceAlgorithmOutcome(ProcessFaceAlgorithmResult(outcome.result()));
	else
		return ProcessFaceAlgorithmOutcome(outcome.error());
}

void MultimediaaiClient::processFaceAlgorithmAsync(const ProcessFaceAlgorithmRequest& request, const ProcessFaceAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, processFaceAlgorithm(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::ProcessFaceAlgorithmOutcomeCallable MultimediaaiClient::processFaceAlgorithmCallable(const ProcessFaceAlgorithmRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ProcessFaceAlgorithmOutcome()>>(
			[this, request]()
			{
			return this->processFaceAlgorithm(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::ProcessImageTagAlgorithmOutcome MultimediaaiClient::processImageTagAlgorithm(const ProcessImageTagAlgorithmRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ProcessImageTagAlgorithmOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ProcessImageTagAlgorithmOutcome(ProcessImageTagAlgorithmResult(outcome.result()));
	else
		return ProcessImageTagAlgorithmOutcome(outcome.error());
}

void MultimediaaiClient::processImageTagAlgorithmAsync(const ProcessImageTagAlgorithmRequest& request, const ProcessImageTagAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, processImageTagAlgorithm(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::ProcessImageTagAlgorithmOutcomeCallable MultimediaaiClient::processImageTagAlgorithmCallable(const ProcessImageTagAlgorithmRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ProcessImageTagAlgorithmOutcome()>>(
			[this, request]()
			{
			return this->processImageTagAlgorithm(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::ProcessLandmarkAlgorithmOutcome MultimediaaiClient::processLandmarkAlgorithm(const ProcessLandmarkAlgorithmRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ProcessLandmarkAlgorithmOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ProcessLandmarkAlgorithmOutcome(ProcessLandmarkAlgorithmResult(outcome.result()));
	else
		return ProcessLandmarkAlgorithmOutcome(outcome.error());
}

void MultimediaaiClient::processLandmarkAlgorithmAsync(const ProcessLandmarkAlgorithmRequest& request, const ProcessLandmarkAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, processLandmarkAlgorithm(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::ProcessLandmarkAlgorithmOutcomeCallable MultimediaaiClient::processLandmarkAlgorithmCallable(const ProcessLandmarkAlgorithmRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ProcessLandmarkAlgorithmOutcome()>>(
			[this, request]()
			{
			return this->processLandmarkAlgorithm(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::ProcessLogoAlgorithmOutcome MultimediaaiClient::processLogoAlgorithm(const ProcessLogoAlgorithmRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ProcessLogoAlgorithmOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ProcessLogoAlgorithmOutcome(ProcessLogoAlgorithmResult(outcome.result()));
	else
		return ProcessLogoAlgorithmOutcome(outcome.error());
}

void MultimediaaiClient::processLogoAlgorithmAsync(const ProcessLogoAlgorithmRequest& request, const ProcessLogoAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, processLogoAlgorithm(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::ProcessLogoAlgorithmOutcomeCallable MultimediaaiClient::processLogoAlgorithmCallable(const ProcessLogoAlgorithmRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ProcessLogoAlgorithmOutcome()>>(
			[this, request]()
			{
			return this->processLogoAlgorithm(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::ProcessNewsAlgorithmOutcome MultimediaaiClient::processNewsAlgorithm(const ProcessNewsAlgorithmRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ProcessNewsAlgorithmOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ProcessNewsAlgorithmOutcome(ProcessNewsAlgorithmResult(outcome.result()));
	else
		return ProcessNewsAlgorithmOutcome(outcome.error());
}

void MultimediaaiClient::processNewsAlgorithmAsync(const ProcessNewsAlgorithmRequest& request, const ProcessNewsAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, processNewsAlgorithm(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::ProcessNewsAlgorithmOutcomeCallable MultimediaaiClient::processNewsAlgorithmCallable(const ProcessNewsAlgorithmRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ProcessNewsAlgorithmOutcome()>>(
			[this, request]()
			{
			return this->processNewsAlgorithm(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::ProcessNlpAlgorithmOutcome MultimediaaiClient::processNlpAlgorithm(const ProcessNlpAlgorithmRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ProcessNlpAlgorithmOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ProcessNlpAlgorithmOutcome(ProcessNlpAlgorithmResult(outcome.result()));
	else
		return ProcessNlpAlgorithmOutcome(outcome.error());
}

void MultimediaaiClient::processNlpAlgorithmAsync(const ProcessNlpAlgorithmRequest& request, const ProcessNlpAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, processNlpAlgorithm(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::ProcessNlpAlgorithmOutcomeCallable MultimediaaiClient::processNlpAlgorithmCallable(const ProcessNlpAlgorithmRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ProcessNlpAlgorithmOutcome()>>(
			[this, request]()
			{
			return this->processNlpAlgorithm(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::ProcessOcrAlgorithmOutcome MultimediaaiClient::processOcrAlgorithm(const ProcessOcrAlgorithmRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ProcessOcrAlgorithmOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ProcessOcrAlgorithmOutcome(ProcessOcrAlgorithmResult(outcome.result()));
	else
		return ProcessOcrAlgorithmOutcome(outcome.error());
}

void MultimediaaiClient::processOcrAlgorithmAsync(const ProcessOcrAlgorithmRequest& request, const ProcessOcrAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, processOcrAlgorithm(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::ProcessOcrAlgorithmOutcomeCallable MultimediaaiClient::processOcrAlgorithmCallable(const ProcessOcrAlgorithmRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ProcessOcrAlgorithmOutcome()>>(
			[this, request]()
			{
			return this->processOcrAlgorithm(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::RegisterFaceImageOutcome MultimediaaiClient::registerFaceImage(const RegisterFaceImageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RegisterFaceImageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RegisterFaceImageOutcome(RegisterFaceImageResult(outcome.result()));
	else
		return RegisterFaceImageOutcome(outcome.error());
}

void MultimediaaiClient::registerFaceImageAsync(const RegisterFaceImageRequest& request, const RegisterFaceImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, registerFaceImage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::RegisterFaceImageOutcomeCallable MultimediaaiClient::registerFaceImageCallable(const RegisterFaceImageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RegisterFaceImageOutcome()>>(
			[this, request]()
			{
			return this->registerFaceImage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

MultimediaaiClient::UpdateTemplateOutcome MultimediaaiClient::updateTemplate(const UpdateTemplateRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UpdateTemplateOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UpdateTemplateOutcome(UpdateTemplateResult(outcome.result()));
	else
		return UpdateTemplateOutcome(outcome.error());
}

void MultimediaaiClient::updateTemplateAsync(const UpdateTemplateRequest& request, const UpdateTemplateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, updateTemplate(request), context);
	};

	asyncExecute(new Runnable(fn));
}

MultimediaaiClient::UpdateTemplateOutcomeCallable MultimediaaiClient::updateTemplateCallable(const UpdateTemplateRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UpdateTemplateOutcome()>>(
			[this, request]()
			{
			return this->updateTemplate(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


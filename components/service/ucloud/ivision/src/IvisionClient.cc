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

#include <alibabacloud/ivision/IvisionClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Ivision;
using namespace AlibabaCloud::Ivision::Model;

namespace
{
	const std::string SERVICE_NAME = "ivision";
}

IvisionClient::IvisionClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "ivision");
}

IvisionClient::IvisionClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "ivision");
}

IvisionClient::IvisionClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "ivision");
}

IvisionClient::~IvisionClient()
{}

IvisionClient::CreateFaceGroupOutcome IvisionClient::createFaceGroup(const CreateFaceGroupRequest &request) const
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

void IvisionClient::createFaceGroupAsync(const CreateFaceGroupRequest& request, const CreateFaceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createFaceGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IvisionClient::CreateFaceGroupOutcomeCallable IvisionClient::createFaceGroupCallable(const CreateFaceGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateFaceGroupOutcome()>>(
			[this, request]()
			{
			return this->createFaceGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IvisionClient::CreateStreamPredictOutcome IvisionClient::createStreamPredict(const CreateStreamPredictRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateStreamPredictOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateStreamPredictOutcome(CreateStreamPredictResult(outcome.result()));
	else
		return CreateStreamPredictOutcome(outcome.error());
}

void IvisionClient::createStreamPredictAsync(const CreateStreamPredictRequest& request, const CreateStreamPredictAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createStreamPredict(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IvisionClient::CreateStreamPredictOutcomeCallable IvisionClient::createStreamPredictCallable(const CreateStreamPredictRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateStreamPredictOutcome()>>(
			[this, request]()
			{
			return this->createStreamPredict(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IvisionClient::DeleteFaceGroupOutcome IvisionClient::deleteFaceGroup(const DeleteFaceGroupRequest &request) const
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

void IvisionClient::deleteFaceGroupAsync(const DeleteFaceGroupRequest& request, const DeleteFaceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteFaceGroup(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IvisionClient::DeleteFaceGroupOutcomeCallable IvisionClient::deleteFaceGroupCallable(const DeleteFaceGroupRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteFaceGroupOutcome()>>(
			[this, request]()
			{
			return this->deleteFaceGroup(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IvisionClient::DeleteStreamPredictOutcome IvisionClient::deleteStreamPredict(const DeleteStreamPredictRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteStreamPredictOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteStreamPredictOutcome(DeleteStreamPredictResult(outcome.result()));
	else
		return DeleteStreamPredictOutcome(outcome.error());
}

void IvisionClient::deleteStreamPredictAsync(const DeleteStreamPredictRequest& request, const DeleteStreamPredictAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteStreamPredict(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IvisionClient::DeleteStreamPredictOutcomeCallable IvisionClient::deleteStreamPredictCallable(const DeleteStreamPredictRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteStreamPredictOutcome()>>(
			[this, request]()
			{
			return this->deleteStreamPredict(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IvisionClient::DescribeFaceGroupsOutcome IvisionClient::describeFaceGroups(const DescribeFaceGroupsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeFaceGroupsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeFaceGroupsOutcome(DescribeFaceGroupsResult(outcome.result()));
	else
		return DescribeFaceGroupsOutcome(outcome.error());
}

void IvisionClient::describeFaceGroupsAsync(const DescribeFaceGroupsRequest& request, const DescribeFaceGroupsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeFaceGroups(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IvisionClient::DescribeFaceGroupsOutcomeCallable IvisionClient::describeFaceGroupsCallable(const DescribeFaceGroupsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeFaceGroupsOutcome()>>(
			[this, request]()
			{
			return this->describeFaceGroups(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IvisionClient::DescribeStreamPredictResultOutcome IvisionClient::describeStreamPredictResult(const DescribeStreamPredictResultRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeStreamPredictResultOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeStreamPredictResultOutcome(DescribeStreamPredictResultResult(outcome.result()));
	else
		return DescribeStreamPredictResultOutcome(outcome.error());
}

void IvisionClient::describeStreamPredictResultAsync(const DescribeStreamPredictResultRequest& request, const DescribeStreamPredictResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeStreamPredictResult(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IvisionClient::DescribeStreamPredictResultOutcomeCallable IvisionClient::describeStreamPredictResultCallable(const DescribeStreamPredictResultRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeStreamPredictResultOutcome()>>(
			[this, request]()
			{
			return this->describeStreamPredictResult(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IvisionClient::DescribeStreamPredictsOutcome IvisionClient::describeStreamPredicts(const DescribeStreamPredictsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DescribeStreamPredictsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DescribeStreamPredictsOutcome(DescribeStreamPredictsResult(outcome.result()));
	else
		return DescribeStreamPredictsOutcome(outcome.error());
}

void IvisionClient::describeStreamPredictsAsync(const DescribeStreamPredictsRequest& request, const DescribeStreamPredictsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, describeStreamPredicts(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IvisionClient::DescribeStreamPredictsOutcomeCallable IvisionClient::describeStreamPredictsCallable(const DescribeStreamPredictsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DescribeStreamPredictsOutcome()>>(
			[this, request]()
			{
			return this->describeStreamPredicts(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IvisionClient::ImagePredictOutcome IvisionClient::imagePredict(const ImagePredictRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ImagePredictOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ImagePredictOutcome(ImagePredictResult(outcome.result()));
	else
		return ImagePredictOutcome(outcome.error());
}

void IvisionClient::imagePredictAsync(const ImagePredictRequest& request, const ImagePredictAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, imagePredict(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IvisionClient::ImagePredictOutcomeCallable IvisionClient::imagePredictCallable(const ImagePredictRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ImagePredictOutcome()>>(
			[this, request]()
			{
			return this->imagePredict(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IvisionClient::RegisterFaceOutcome IvisionClient::registerFace(const RegisterFaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RegisterFaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RegisterFaceOutcome(RegisterFaceResult(outcome.result()));
	else
		return RegisterFaceOutcome(outcome.error());
}

void IvisionClient::registerFaceAsync(const RegisterFaceRequest& request, const RegisterFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, registerFace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IvisionClient::RegisterFaceOutcomeCallable IvisionClient::registerFaceCallable(const RegisterFaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RegisterFaceOutcome()>>(
			[this, request]()
			{
			return this->registerFace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IvisionClient::SearchFaceOutcome IvisionClient::searchFace(const SearchFaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SearchFaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SearchFaceOutcome(SearchFaceResult(outcome.result()));
	else
		return SearchFaceOutcome(outcome.error());
}

void IvisionClient::searchFaceAsync(const SearchFaceRequest& request, const SearchFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, searchFace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IvisionClient::SearchFaceOutcomeCallable IvisionClient::searchFaceCallable(const SearchFaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SearchFaceOutcome()>>(
			[this, request]()
			{
			return this->searchFace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IvisionClient::StartStreamPredictOutcome IvisionClient::startStreamPredict(const StartStreamPredictRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StartStreamPredictOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StartStreamPredictOutcome(StartStreamPredictResult(outcome.result()));
	else
		return StartStreamPredictOutcome(outcome.error());
}

void IvisionClient::startStreamPredictAsync(const StartStreamPredictRequest& request, const StartStreamPredictAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, startStreamPredict(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IvisionClient::StartStreamPredictOutcomeCallable IvisionClient::startStreamPredictCallable(const StartStreamPredictRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StartStreamPredictOutcome()>>(
			[this, request]()
			{
			return this->startStreamPredict(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IvisionClient::StopStreamPredictOutcome IvisionClient::stopStreamPredict(const StopStreamPredictRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return StopStreamPredictOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return StopStreamPredictOutcome(StopStreamPredictResult(outcome.result()));
	else
		return StopStreamPredictOutcome(outcome.error());
}

void IvisionClient::stopStreamPredictAsync(const StopStreamPredictRequest& request, const StopStreamPredictAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, stopStreamPredict(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IvisionClient::StopStreamPredictOutcomeCallable IvisionClient::stopStreamPredictCallable(const StopStreamPredictRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<StopStreamPredictOutcome()>>(
			[this, request]()
			{
			return this->stopStreamPredict(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

IvisionClient::UnregisterFaceOutcome IvisionClient::unregisterFace(const UnregisterFaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return UnregisterFaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return UnregisterFaceOutcome(UnregisterFaceResult(outcome.result()));
	else
		return UnregisterFaceOutcome(outcome.error());
}

void IvisionClient::unregisterFaceAsync(const UnregisterFaceRequest& request, const UnregisterFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, unregisterFace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

IvisionClient::UnregisterFaceOutcomeCallable IvisionClient::unregisterFaceCallable(const UnregisterFaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<UnregisterFaceOutcome()>>(
			[this, request]()
			{
			return this->unregisterFace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


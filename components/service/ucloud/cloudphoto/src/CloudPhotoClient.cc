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

#include <alibabacloud/cloudphoto/CloudPhotoClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

namespace
{
	const std::string SERVICE_NAME = "CloudPhoto";
}

CloudPhotoClient::CloudPhotoClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "cloudphoto");
}

CloudPhotoClient::CloudPhotoClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "cloudphoto");
}

CloudPhotoClient::CloudPhotoClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "cloudphoto");
}

CloudPhotoClient::~CloudPhotoClient()
{}

CloudPhotoClient::ActivatePhotosOutcome CloudPhotoClient::activatePhotos(const ActivatePhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ActivatePhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ActivatePhotosOutcome(ActivatePhotosResult(outcome.result()));
	else
		return ActivatePhotosOutcome(outcome.error());
}

void CloudPhotoClient::activatePhotosAsync(const ActivatePhotosRequest& request, const ActivatePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, activatePhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ActivatePhotosOutcomeCallable CloudPhotoClient::activatePhotosCallable(const ActivatePhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ActivatePhotosOutcome()>>(
			[this, request]()
			{
			return this->activatePhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::AddAlbumPhotosOutcome CloudPhotoClient::addAlbumPhotos(const AddAlbumPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return AddAlbumPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return AddAlbumPhotosOutcome(AddAlbumPhotosResult(outcome.result()));
	else
		return AddAlbumPhotosOutcome(outcome.error());
}

void CloudPhotoClient::addAlbumPhotosAsync(const AddAlbumPhotosRequest& request, const AddAlbumPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, addAlbumPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::AddAlbumPhotosOutcomeCallable CloudPhotoClient::addAlbumPhotosCallable(const AddAlbumPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<AddAlbumPhotosOutcome()>>(
			[this, request]()
			{
			return this->addAlbumPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::CreateAlbumOutcome CloudPhotoClient::createAlbum(const CreateAlbumRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateAlbumOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateAlbumOutcome(CreateAlbumResult(outcome.result()));
	else
		return CreateAlbumOutcome(outcome.error());
}

void CloudPhotoClient::createAlbumAsync(const CreateAlbumRequest& request, const CreateAlbumAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createAlbum(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::CreateAlbumOutcomeCallable CloudPhotoClient::createAlbumCallable(const CreateAlbumRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateAlbumOutcome()>>(
			[this, request]()
			{
			return this->createAlbum(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::CreateEventOutcome CloudPhotoClient::createEvent(const CreateEventRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateEventOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateEventOutcome(CreateEventResult(outcome.result()));
	else
		return CreateEventOutcome(outcome.error());
}

void CloudPhotoClient::createEventAsync(const CreateEventRequest& request, const CreateEventAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createEvent(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::CreateEventOutcomeCallable CloudPhotoClient::createEventCallable(const CreateEventRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateEventOutcome()>>(
			[this, request]()
			{
			return this->createEvent(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::CreatePhotoOutcome CloudPhotoClient::createPhoto(const CreatePhotoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreatePhotoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreatePhotoOutcome(CreatePhotoResult(outcome.result()));
	else
		return CreatePhotoOutcome(outcome.error());
}

void CloudPhotoClient::createPhotoAsync(const CreatePhotoRequest& request, const CreatePhotoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createPhoto(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::CreatePhotoOutcomeCallable CloudPhotoClient::createPhotoCallable(const CreatePhotoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreatePhotoOutcome()>>(
			[this, request]()
			{
			return this->createPhoto(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::CreatePhotoStoreOutcome CloudPhotoClient::createPhotoStore(const CreatePhotoStoreRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreatePhotoStoreOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreatePhotoStoreOutcome(CreatePhotoStoreResult(outcome.result()));
	else
		return CreatePhotoStoreOutcome(outcome.error());
}

void CloudPhotoClient::createPhotoStoreAsync(const CreatePhotoStoreRequest& request, const CreatePhotoStoreAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createPhotoStore(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::CreatePhotoStoreOutcomeCallable CloudPhotoClient::createPhotoStoreCallable(const CreatePhotoStoreRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreatePhotoStoreOutcome()>>(
			[this, request]()
			{
			return this->createPhotoStore(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::CreateTransactionOutcome CloudPhotoClient::createTransaction(const CreateTransactionRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return CreateTransactionOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return CreateTransactionOutcome(CreateTransactionResult(outcome.result()));
	else
		return CreateTransactionOutcome(outcome.error());
}

void CloudPhotoClient::createTransactionAsync(const CreateTransactionRequest& request, const CreateTransactionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, createTransaction(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::CreateTransactionOutcomeCallable CloudPhotoClient::createTransactionCallable(const CreateTransactionRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<CreateTransactionOutcome()>>(
			[this, request]()
			{
			return this->createTransaction(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::DeleteAlbumsOutcome CloudPhotoClient::deleteAlbums(const DeleteAlbumsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteAlbumsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteAlbumsOutcome(DeleteAlbumsResult(outcome.result()));
	else
		return DeleteAlbumsOutcome(outcome.error());
}

void CloudPhotoClient::deleteAlbumsAsync(const DeleteAlbumsRequest& request, const DeleteAlbumsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteAlbums(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::DeleteAlbumsOutcomeCallable CloudPhotoClient::deleteAlbumsCallable(const DeleteAlbumsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteAlbumsOutcome()>>(
			[this, request]()
			{
			return this->deleteAlbums(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::DeleteEventOutcome CloudPhotoClient::deleteEvent(const DeleteEventRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteEventOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteEventOutcome(DeleteEventResult(outcome.result()));
	else
		return DeleteEventOutcome(outcome.error());
}

void CloudPhotoClient::deleteEventAsync(const DeleteEventRequest& request, const DeleteEventAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteEvent(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::DeleteEventOutcomeCallable CloudPhotoClient::deleteEventCallable(const DeleteEventRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteEventOutcome()>>(
			[this, request]()
			{
			return this->deleteEvent(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::DeleteFacesOutcome CloudPhotoClient::deleteFaces(const DeleteFacesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeleteFacesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeleteFacesOutcome(DeleteFacesResult(outcome.result()));
	else
		return DeleteFacesOutcome(outcome.error());
}

void CloudPhotoClient::deleteFacesAsync(const DeleteFacesRequest& request, const DeleteFacesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deleteFaces(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::DeleteFacesOutcomeCallable CloudPhotoClient::deleteFacesCallable(const DeleteFacesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeleteFacesOutcome()>>(
			[this, request]()
			{
			return this->deleteFaces(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::DeletePhotoStoreOutcome CloudPhotoClient::deletePhotoStore(const DeletePhotoStoreRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeletePhotoStoreOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeletePhotoStoreOutcome(DeletePhotoStoreResult(outcome.result()));
	else
		return DeletePhotoStoreOutcome(outcome.error());
}

void CloudPhotoClient::deletePhotoStoreAsync(const DeletePhotoStoreRequest& request, const DeletePhotoStoreAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deletePhotoStore(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::DeletePhotoStoreOutcomeCallable CloudPhotoClient::deletePhotoStoreCallable(const DeletePhotoStoreRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeletePhotoStoreOutcome()>>(
			[this, request]()
			{
			return this->deletePhotoStore(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::DeletePhotosOutcome CloudPhotoClient::deletePhotos(const DeletePhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DeletePhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DeletePhotosOutcome(DeletePhotosResult(outcome.result()));
	else
		return DeletePhotosOutcome(outcome.error());
}

void CloudPhotoClient::deletePhotosAsync(const DeletePhotosRequest& request, const DeletePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, deletePhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::DeletePhotosOutcomeCallable CloudPhotoClient::deletePhotosCallable(const DeletePhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DeletePhotosOutcome()>>(
			[this, request]()
			{
			return this->deletePhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::EditEventOutcome CloudPhotoClient::editEvent(const EditEventRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return EditEventOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return EditEventOutcome(EditEventResult(outcome.result()));
	else
		return EditEventOutcome(outcome.error());
}

void CloudPhotoClient::editEventAsync(const EditEventRequest& request, const EditEventAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, editEvent(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::EditEventOutcomeCallable CloudPhotoClient::editEventCallable(const EditEventRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<EditEventOutcome()>>(
			[this, request]()
			{
			return this->editEvent(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::EditPhotoStoreOutcome CloudPhotoClient::editPhotoStore(const EditPhotoStoreRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return EditPhotoStoreOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return EditPhotoStoreOutcome(EditPhotoStoreResult(outcome.result()));
	else
		return EditPhotoStoreOutcome(outcome.error());
}

void CloudPhotoClient::editPhotoStoreAsync(const EditPhotoStoreRequest& request, const EditPhotoStoreAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, editPhotoStore(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::EditPhotoStoreOutcomeCallable CloudPhotoClient::editPhotoStoreCallable(const EditPhotoStoreRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<EditPhotoStoreOutcome()>>(
			[this, request]()
			{
			return this->editPhotoStore(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::EditPhotosOutcome CloudPhotoClient::editPhotos(const EditPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return EditPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return EditPhotosOutcome(EditPhotosResult(outcome.result()));
	else
		return EditPhotosOutcome(outcome.error());
}

void CloudPhotoClient::editPhotosAsync(const EditPhotosRequest& request, const EditPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, editPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::EditPhotosOutcomeCallable CloudPhotoClient::editPhotosCallable(const EditPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<EditPhotosOutcome()>>(
			[this, request]()
			{
			return this->editPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::FetchAlbumTagPhotosOutcome CloudPhotoClient::fetchAlbumTagPhotos(const FetchAlbumTagPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return FetchAlbumTagPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return FetchAlbumTagPhotosOutcome(FetchAlbumTagPhotosResult(outcome.result()));
	else
		return FetchAlbumTagPhotosOutcome(outcome.error());
}

void CloudPhotoClient::fetchAlbumTagPhotosAsync(const FetchAlbumTagPhotosRequest& request, const FetchAlbumTagPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, fetchAlbumTagPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::FetchAlbumTagPhotosOutcomeCallable CloudPhotoClient::fetchAlbumTagPhotosCallable(const FetchAlbumTagPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<FetchAlbumTagPhotosOutcome()>>(
			[this, request]()
			{
			return this->fetchAlbumTagPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::FetchLibrariesOutcome CloudPhotoClient::fetchLibraries(const FetchLibrariesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return FetchLibrariesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return FetchLibrariesOutcome(FetchLibrariesResult(outcome.result()));
	else
		return FetchLibrariesOutcome(outcome.error());
}

void CloudPhotoClient::fetchLibrariesAsync(const FetchLibrariesRequest& request, const FetchLibrariesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, fetchLibraries(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::FetchLibrariesOutcomeCallable CloudPhotoClient::fetchLibrariesCallable(const FetchLibrariesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<FetchLibrariesOutcome()>>(
			[this, request]()
			{
			return this->fetchLibraries(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::FetchMomentPhotosOutcome CloudPhotoClient::fetchMomentPhotos(const FetchMomentPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return FetchMomentPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return FetchMomentPhotosOutcome(FetchMomentPhotosResult(outcome.result()));
	else
		return FetchMomentPhotosOutcome(outcome.error());
}

void CloudPhotoClient::fetchMomentPhotosAsync(const FetchMomentPhotosRequest& request, const FetchMomentPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, fetchMomentPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::FetchMomentPhotosOutcomeCallable CloudPhotoClient::fetchMomentPhotosCallable(const FetchMomentPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<FetchMomentPhotosOutcome()>>(
			[this, request]()
			{
			return this->fetchMomentPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::FetchPhotosOutcome CloudPhotoClient::fetchPhotos(const FetchPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return FetchPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return FetchPhotosOutcome(FetchPhotosResult(outcome.result()));
	else
		return FetchPhotosOutcome(outcome.error());
}

void CloudPhotoClient::fetchPhotosAsync(const FetchPhotosRequest& request, const FetchPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, fetchPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::FetchPhotosOutcomeCallable CloudPhotoClient::fetchPhotosCallable(const FetchPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<FetchPhotosOutcome()>>(
			[this, request]()
			{
			return this->fetchPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetAlbumsByNamesOutcome CloudPhotoClient::getAlbumsByNames(const GetAlbumsByNamesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetAlbumsByNamesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetAlbumsByNamesOutcome(GetAlbumsByNamesResult(outcome.result()));
	else
		return GetAlbumsByNamesOutcome(outcome.error());
}

void CloudPhotoClient::getAlbumsByNamesAsync(const GetAlbumsByNamesRequest& request, const GetAlbumsByNamesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getAlbumsByNames(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetAlbumsByNamesOutcomeCallable CloudPhotoClient::getAlbumsByNamesCallable(const GetAlbumsByNamesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetAlbumsByNamesOutcome()>>(
			[this, request]()
			{
			return this->getAlbumsByNames(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetDownloadUrlOutcome CloudPhotoClient::getDownloadUrl(const GetDownloadUrlRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDownloadUrlOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDownloadUrlOutcome(GetDownloadUrlResult(outcome.result()));
	else
		return GetDownloadUrlOutcome(outcome.error());
}

void CloudPhotoClient::getDownloadUrlAsync(const GetDownloadUrlRequest& request, const GetDownloadUrlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDownloadUrl(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetDownloadUrlOutcomeCallable CloudPhotoClient::getDownloadUrlCallable(const GetDownloadUrlRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDownloadUrlOutcome()>>(
			[this, request]()
			{
			return this->getDownloadUrl(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetDownloadUrlsOutcome CloudPhotoClient::getDownloadUrls(const GetDownloadUrlsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetDownloadUrlsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetDownloadUrlsOutcome(GetDownloadUrlsResult(outcome.result()));
	else
		return GetDownloadUrlsOutcome(outcome.error());
}

void CloudPhotoClient::getDownloadUrlsAsync(const GetDownloadUrlsRequest& request, const GetDownloadUrlsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getDownloadUrls(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetDownloadUrlsOutcomeCallable CloudPhotoClient::getDownloadUrlsCallable(const GetDownloadUrlsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetDownloadUrlsOutcome()>>(
			[this, request]()
			{
			return this->getDownloadUrls(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetEventOutcome CloudPhotoClient::getEvent(const GetEventRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetEventOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetEventOutcome(GetEventResult(outcome.result()));
	else
		return GetEventOutcome(outcome.error());
}

void CloudPhotoClient::getEventAsync(const GetEventRequest& request, const GetEventAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getEvent(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetEventOutcomeCallable CloudPhotoClient::getEventCallable(const GetEventRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetEventOutcome()>>(
			[this, request]()
			{
			return this->getEvent(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetFramedPhotoUrlsOutcome CloudPhotoClient::getFramedPhotoUrls(const GetFramedPhotoUrlsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetFramedPhotoUrlsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetFramedPhotoUrlsOutcome(GetFramedPhotoUrlsResult(outcome.result()));
	else
		return GetFramedPhotoUrlsOutcome(outcome.error());
}

void CloudPhotoClient::getFramedPhotoUrlsAsync(const GetFramedPhotoUrlsRequest& request, const GetFramedPhotoUrlsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getFramedPhotoUrls(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetFramedPhotoUrlsOutcomeCallable CloudPhotoClient::getFramedPhotoUrlsCallable(const GetFramedPhotoUrlsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetFramedPhotoUrlsOutcome()>>(
			[this, request]()
			{
			return this->getFramedPhotoUrls(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetLibraryOutcome CloudPhotoClient::getLibrary(const GetLibraryRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetLibraryOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetLibraryOutcome(GetLibraryResult(outcome.result()));
	else
		return GetLibraryOutcome(outcome.error());
}

void CloudPhotoClient::getLibraryAsync(const GetLibraryRequest& request, const GetLibraryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getLibrary(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetLibraryOutcomeCallable CloudPhotoClient::getLibraryCallable(const GetLibraryRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetLibraryOutcome()>>(
			[this, request]()
			{
			return this->getLibrary(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetPhotoStoreOutcome CloudPhotoClient::getPhotoStore(const GetPhotoStoreRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetPhotoStoreOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetPhotoStoreOutcome(GetPhotoStoreResult(outcome.result()));
	else
		return GetPhotoStoreOutcome(outcome.error());
}

void CloudPhotoClient::getPhotoStoreAsync(const GetPhotoStoreRequest& request, const GetPhotoStoreAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getPhotoStore(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetPhotoStoreOutcomeCallable CloudPhotoClient::getPhotoStoreCallable(const GetPhotoStoreRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetPhotoStoreOutcome()>>(
			[this, request]()
			{
			return this->getPhotoStore(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetPhotosOutcome CloudPhotoClient::getPhotos(const GetPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetPhotosOutcome(GetPhotosResult(outcome.result()));
	else
		return GetPhotosOutcome(outcome.error());
}

void CloudPhotoClient::getPhotosAsync(const GetPhotosRequest& request, const GetPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetPhotosOutcomeCallable CloudPhotoClient::getPhotosCallable(const GetPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetPhotosOutcome()>>(
			[this, request]()
			{
			return this->getPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetPhotosByMd5sOutcome CloudPhotoClient::getPhotosByMd5s(const GetPhotosByMd5sRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetPhotosByMd5sOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetPhotosByMd5sOutcome(GetPhotosByMd5sResult(outcome.result()));
	else
		return GetPhotosByMd5sOutcome(outcome.error());
}

void CloudPhotoClient::getPhotosByMd5sAsync(const GetPhotosByMd5sRequest& request, const GetPhotosByMd5sAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getPhotosByMd5s(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetPhotosByMd5sOutcomeCallable CloudPhotoClient::getPhotosByMd5sCallable(const GetPhotosByMd5sRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetPhotosByMd5sOutcome()>>(
			[this, request]()
			{
			return this->getPhotosByMd5s(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetPrivateAccessUrlsOutcome CloudPhotoClient::getPrivateAccessUrls(const GetPrivateAccessUrlsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetPrivateAccessUrlsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetPrivateAccessUrlsOutcome(GetPrivateAccessUrlsResult(outcome.result()));
	else
		return GetPrivateAccessUrlsOutcome(outcome.error());
}

void CloudPhotoClient::getPrivateAccessUrlsAsync(const GetPrivateAccessUrlsRequest& request, const GetPrivateAccessUrlsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getPrivateAccessUrls(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetPrivateAccessUrlsOutcomeCallable CloudPhotoClient::getPrivateAccessUrlsCallable(const GetPrivateAccessUrlsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetPrivateAccessUrlsOutcome()>>(
			[this, request]()
			{
			return this->getPrivateAccessUrls(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetPublicAccessUrlsOutcome CloudPhotoClient::getPublicAccessUrls(const GetPublicAccessUrlsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetPublicAccessUrlsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetPublicAccessUrlsOutcome(GetPublicAccessUrlsResult(outcome.result()));
	else
		return GetPublicAccessUrlsOutcome(outcome.error());
}

void CloudPhotoClient::getPublicAccessUrlsAsync(const GetPublicAccessUrlsRequest& request, const GetPublicAccessUrlsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getPublicAccessUrls(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetPublicAccessUrlsOutcomeCallable CloudPhotoClient::getPublicAccessUrlsCallable(const GetPublicAccessUrlsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetPublicAccessUrlsOutcome()>>(
			[this, request]()
			{
			return this->getPublicAccessUrls(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetQuotaOutcome CloudPhotoClient::getQuota(const GetQuotaRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetQuotaOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetQuotaOutcome(GetQuotaResult(outcome.result()));
	else
		return GetQuotaOutcome(outcome.error());
}

void CloudPhotoClient::getQuotaAsync(const GetQuotaRequest& request, const GetQuotaAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getQuota(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetQuotaOutcomeCallable CloudPhotoClient::getQuotaCallable(const GetQuotaRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetQuotaOutcome()>>(
			[this, request]()
			{
			return this->getQuota(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetSimilarPhotosOutcome CloudPhotoClient::getSimilarPhotos(const GetSimilarPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetSimilarPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetSimilarPhotosOutcome(GetSimilarPhotosResult(outcome.result()));
	else
		return GetSimilarPhotosOutcome(outcome.error());
}

void CloudPhotoClient::getSimilarPhotosAsync(const GetSimilarPhotosRequest& request, const GetSimilarPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getSimilarPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetSimilarPhotosOutcomeCallable CloudPhotoClient::getSimilarPhotosCallable(const GetSimilarPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetSimilarPhotosOutcome()>>(
			[this, request]()
			{
			return this->getSimilarPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetThumbnailOutcome CloudPhotoClient::getThumbnail(const GetThumbnailRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetThumbnailOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetThumbnailOutcome(GetThumbnailResult(outcome.result()));
	else
		return GetThumbnailOutcome(outcome.error());
}

void CloudPhotoClient::getThumbnailAsync(const GetThumbnailRequest& request, const GetThumbnailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getThumbnail(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetThumbnailOutcomeCallable CloudPhotoClient::getThumbnailCallable(const GetThumbnailRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetThumbnailOutcome()>>(
			[this, request]()
			{
			return this->getThumbnail(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetThumbnailsOutcome CloudPhotoClient::getThumbnails(const GetThumbnailsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetThumbnailsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetThumbnailsOutcome(GetThumbnailsResult(outcome.result()));
	else
		return GetThumbnailsOutcome(outcome.error());
}

void CloudPhotoClient::getThumbnailsAsync(const GetThumbnailsRequest& request, const GetThumbnailsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getThumbnails(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetThumbnailsOutcomeCallable CloudPhotoClient::getThumbnailsCallable(const GetThumbnailsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetThumbnailsOutcome()>>(
			[this, request]()
			{
			return this->getThumbnails(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::GetVideoCoverOutcome CloudPhotoClient::getVideoCover(const GetVideoCoverRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetVideoCoverOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetVideoCoverOutcome(GetVideoCoverResult(outcome.result()));
	else
		return GetVideoCoverOutcome(outcome.error());
}

void CloudPhotoClient::getVideoCoverAsync(const GetVideoCoverRequest& request, const GetVideoCoverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getVideoCover(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::GetVideoCoverOutcomeCallable CloudPhotoClient::getVideoCoverCallable(const GetVideoCoverRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetVideoCoverOutcome()>>(
			[this, request]()
			{
			return this->getVideoCover(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::InactivatePhotosOutcome CloudPhotoClient::inactivatePhotos(const InactivatePhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return InactivatePhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return InactivatePhotosOutcome(InactivatePhotosResult(outcome.result()));
	else
		return InactivatePhotosOutcome(outcome.error());
}

void CloudPhotoClient::inactivatePhotosAsync(const InactivatePhotosRequest& request, const InactivatePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, inactivatePhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::InactivatePhotosOutcomeCallable CloudPhotoClient::inactivatePhotosCallable(const InactivatePhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<InactivatePhotosOutcome()>>(
			[this, request]()
			{
			return this->inactivatePhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::LikePhotoOutcome CloudPhotoClient::likePhoto(const LikePhotoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return LikePhotoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return LikePhotoOutcome(LikePhotoResult(outcome.result()));
	else
		return LikePhotoOutcome(outcome.error());
}

void CloudPhotoClient::likePhotoAsync(const LikePhotoRequest& request, const LikePhotoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, likePhoto(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::LikePhotoOutcomeCallable CloudPhotoClient::likePhotoCallable(const LikePhotoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<LikePhotoOutcome()>>(
			[this, request]()
			{
			return this->likePhoto(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListAlbumPhotosOutcome CloudPhotoClient::listAlbumPhotos(const ListAlbumPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListAlbumPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListAlbumPhotosOutcome(ListAlbumPhotosResult(outcome.result()));
	else
		return ListAlbumPhotosOutcome(outcome.error());
}

void CloudPhotoClient::listAlbumPhotosAsync(const ListAlbumPhotosRequest& request, const ListAlbumPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listAlbumPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListAlbumPhotosOutcomeCallable CloudPhotoClient::listAlbumPhotosCallable(const ListAlbumPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListAlbumPhotosOutcome()>>(
			[this, request]()
			{
			return this->listAlbumPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListAlbumsOutcome CloudPhotoClient::listAlbums(const ListAlbumsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListAlbumsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListAlbumsOutcome(ListAlbumsResult(outcome.result()));
	else
		return ListAlbumsOutcome(outcome.error());
}

void CloudPhotoClient::listAlbumsAsync(const ListAlbumsRequest& request, const ListAlbumsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listAlbums(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListAlbumsOutcomeCallable CloudPhotoClient::listAlbumsCallable(const ListAlbumsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListAlbumsOutcome()>>(
			[this, request]()
			{
			return this->listAlbums(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListEventsOutcome CloudPhotoClient::listEvents(const ListEventsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListEventsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListEventsOutcome(ListEventsResult(outcome.result()));
	else
		return ListEventsOutcome(outcome.error());
}

void CloudPhotoClient::listEventsAsync(const ListEventsRequest& request, const ListEventsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listEvents(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListEventsOutcomeCallable CloudPhotoClient::listEventsCallable(const ListEventsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListEventsOutcome()>>(
			[this, request]()
			{
			return this->listEvents(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListFacePhotosOutcome CloudPhotoClient::listFacePhotos(const ListFacePhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListFacePhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListFacePhotosOutcome(ListFacePhotosResult(outcome.result()));
	else
		return ListFacePhotosOutcome(outcome.error());
}

void CloudPhotoClient::listFacePhotosAsync(const ListFacePhotosRequest& request, const ListFacePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listFacePhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListFacePhotosOutcomeCallable CloudPhotoClient::listFacePhotosCallable(const ListFacePhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListFacePhotosOutcome()>>(
			[this, request]()
			{
			return this->listFacePhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListFacesOutcome CloudPhotoClient::listFaces(const ListFacesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListFacesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListFacesOutcome(ListFacesResult(outcome.result()));
	else
		return ListFacesOutcome(outcome.error());
}

void CloudPhotoClient::listFacesAsync(const ListFacesRequest& request, const ListFacesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listFaces(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListFacesOutcomeCallable CloudPhotoClient::listFacesCallable(const ListFacesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListFacesOutcome()>>(
			[this, request]()
			{
			return this->listFaces(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListMomentPhotosOutcome CloudPhotoClient::listMomentPhotos(const ListMomentPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListMomentPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListMomentPhotosOutcome(ListMomentPhotosResult(outcome.result()));
	else
		return ListMomentPhotosOutcome(outcome.error());
}

void CloudPhotoClient::listMomentPhotosAsync(const ListMomentPhotosRequest& request, const ListMomentPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listMomentPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListMomentPhotosOutcomeCallable CloudPhotoClient::listMomentPhotosCallable(const ListMomentPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListMomentPhotosOutcome()>>(
			[this, request]()
			{
			return this->listMomentPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListMomentsOutcome CloudPhotoClient::listMoments(const ListMomentsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListMomentsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListMomentsOutcome(ListMomentsResult(outcome.result()));
	else
		return ListMomentsOutcome(outcome.error());
}

void CloudPhotoClient::listMomentsAsync(const ListMomentsRequest& request, const ListMomentsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listMoments(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListMomentsOutcomeCallable CloudPhotoClient::listMomentsCallable(const ListMomentsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListMomentsOutcome()>>(
			[this, request]()
			{
			return this->listMoments(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListPhotoFacesOutcome CloudPhotoClient::listPhotoFaces(const ListPhotoFacesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListPhotoFacesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListPhotoFacesOutcome(ListPhotoFacesResult(outcome.result()));
	else
		return ListPhotoFacesOutcome(outcome.error());
}

void CloudPhotoClient::listPhotoFacesAsync(const ListPhotoFacesRequest& request, const ListPhotoFacesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listPhotoFaces(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListPhotoFacesOutcomeCallable CloudPhotoClient::listPhotoFacesCallable(const ListPhotoFacesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListPhotoFacesOutcome()>>(
			[this, request]()
			{
			return this->listPhotoFaces(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListPhotoStoresOutcome CloudPhotoClient::listPhotoStores(const ListPhotoStoresRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListPhotoStoresOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListPhotoStoresOutcome(ListPhotoStoresResult(outcome.result()));
	else
		return ListPhotoStoresOutcome(outcome.error());
}

void CloudPhotoClient::listPhotoStoresAsync(const ListPhotoStoresRequest& request, const ListPhotoStoresAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listPhotoStores(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListPhotoStoresOutcomeCallable CloudPhotoClient::listPhotoStoresCallable(const ListPhotoStoresRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListPhotoStoresOutcome()>>(
			[this, request]()
			{
			return this->listPhotoStores(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListPhotoTagsOutcome CloudPhotoClient::listPhotoTags(const ListPhotoTagsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListPhotoTagsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListPhotoTagsOutcome(ListPhotoTagsResult(outcome.result()));
	else
		return ListPhotoTagsOutcome(outcome.error());
}

void CloudPhotoClient::listPhotoTagsAsync(const ListPhotoTagsRequest& request, const ListPhotoTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listPhotoTags(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListPhotoTagsOutcomeCallable CloudPhotoClient::listPhotoTagsCallable(const ListPhotoTagsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListPhotoTagsOutcome()>>(
			[this, request]()
			{
			return this->listPhotoTags(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListPhotosOutcome CloudPhotoClient::listPhotos(const ListPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListPhotosOutcome(ListPhotosResult(outcome.result()));
	else
		return ListPhotosOutcome(outcome.error());
}

void CloudPhotoClient::listPhotosAsync(const ListPhotosRequest& request, const ListPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListPhotosOutcomeCallable CloudPhotoClient::listPhotosCallable(const ListPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListPhotosOutcome()>>(
			[this, request]()
			{
			return this->listPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListRegisteredTagsOutcome CloudPhotoClient::listRegisteredTags(const ListRegisteredTagsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListRegisteredTagsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListRegisteredTagsOutcome(ListRegisteredTagsResult(outcome.result()));
	else
		return ListRegisteredTagsOutcome(outcome.error());
}

void CloudPhotoClient::listRegisteredTagsAsync(const ListRegisteredTagsRequest& request, const ListRegisteredTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listRegisteredTags(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListRegisteredTagsOutcomeCallable CloudPhotoClient::listRegisteredTagsCallable(const ListRegisteredTagsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListRegisteredTagsOutcome()>>(
			[this, request]()
			{
			return this->listRegisteredTags(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListTagPhotosOutcome CloudPhotoClient::listTagPhotos(const ListTagPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListTagPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListTagPhotosOutcome(ListTagPhotosResult(outcome.result()));
	else
		return ListTagPhotosOutcome(outcome.error());
}

void CloudPhotoClient::listTagPhotosAsync(const ListTagPhotosRequest& request, const ListTagPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listTagPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListTagPhotosOutcomeCallable CloudPhotoClient::listTagPhotosCallable(const ListTagPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListTagPhotosOutcome()>>(
			[this, request]()
			{
			return this->listTagPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListTagsOutcome CloudPhotoClient::listTags(const ListTagsRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListTagsOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListTagsOutcome(ListTagsResult(outcome.result()));
	else
		return ListTagsOutcome(outcome.error());
}

void CloudPhotoClient::listTagsAsync(const ListTagsRequest& request, const ListTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listTags(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListTagsOutcomeCallable CloudPhotoClient::listTagsCallable(const ListTagsRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListTagsOutcome()>>(
			[this, request]()
			{
			return this->listTags(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListTimeLinePhotosOutcome CloudPhotoClient::listTimeLinePhotos(const ListTimeLinePhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListTimeLinePhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListTimeLinePhotosOutcome(ListTimeLinePhotosResult(outcome.result()));
	else
		return ListTimeLinePhotosOutcome(outcome.error());
}

void CloudPhotoClient::listTimeLinePhotosAsync(const ListTimeLinePhotosRequest& request, const ListTimeLinePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listTimeLinePhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListTimeLinePhotosOutcomeCallable CloudPhotoClient::listTimeLinePhotosCallable(const ListTimeLinePhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListTimeLinePhotosOutcome()>>(
			[this, request]()
			{
			return this->listTimeLinePhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ListTimeLinesOutcome CloudPhotoClient::listTimeLines(const ListTimeLinesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ListTimeLinesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ListTimeLinesOutcome(ListTimeLinesResult(outcome.result()));
	else
		return ListTimeLinesOutcome(outcome.error());
}

void CloudPhotoClient::listTimeLinesAsync(const ListTimeLinesRequest& request, const ListTimeLinesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, listTimeLines(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ListTimeLinesOutcomeCallable CloudPhotoClient::listTimeLinesCallable(const ListTimeLinesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ListTimeLinesOutcome()>>(
			[this, request]()
			{
			return this->listTimeLines(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::MergeFacesOutcome CloudPhotoClient::mergeFaces(const MergeFacesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return MergeFacesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return MergeFacesOutcome(MergeFacesResult(outcome.result()));
	else
		return MergeFacesOutcome(outcome.error());
}

void CloudPhotoClient::mergeFacesAsync(const MergeFacesRequest& request, const MergeFacesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, mergeFaces(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::MergeFacesOutcomeCallable CloudPhotoClient::mergeFacesCallable(const MergeFacesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<MergeFacesOutcome()>>(
			[this, request]()
			{
			return this->mergeFaces(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::MoveAlbumPhotosOutcome CloudPhotoClient::moveAlbumPhotos(const MoveAlbumPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return MoveAlbumPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return MoveAlbumPhotosOutcome(MoveAlbumPhotosResult(outcome.result()));
	else
		return MoveAlbumPhotosOutcome(outcome.error());
}

void CloudPhotoClient::moveAlbumPhotosAsync(const MoveAlbumPhotosRequest& request, const MoveAlbumPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, moveAlbumPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::MoveAlbumPhotosOutcomeCallable CloudPhotoClient::moveAlbumPhotosCallable(const MoveAlbumPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<MoveAlbumPhotosOutcome()>>(
			[this, request]()
			{
			return this->moveAlbumPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::MoveFacePhotosOutcome CloudPhotoClient::moveFacePhotos(const MoveFacePhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return MoveFacePhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return MoveFacePhotosOutcome(MoveFacePhotosResult(outcome.result()));
	else
		return MoveFacePhotosOutcome(outcome.error());
}

void CloudPhotoClient::moveFacePhotosAsync(const MoveFacePhotosRequest& request, const MoveFacePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, moveFacePhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::MoveFacePhotosOutcomeCallable CloudPhotoClient::moveFacePhotosCallable(const MoveFacePhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<MoveFacePhotosOutcome()>>(
			[this, request]()
			{
			return this->moveFacePhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ReactivatePhotosOutcome CloudPhotoClient::reactivatePhotos(const ReactivatePhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ReactivatePhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ReactivatePhotosOutcome(ReactivatePhotosResult(outcome.result()));
	else
		return ReactivatePhotosOutcome(outcome.error());
}

void CloudPhotoClient::reactivatePhotosAsync(const ReactivatePhotosRequest& request, const ReactivatePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, reactivatePhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ReactivatePhotosOutcomeCallable CloudPhotoClient::reactivatePhotosCallable(const ReactivatePhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ReactivatePhotosOutcome()>>(
			[this, request]()
			{
			return this->reactivatePhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::RegisterPhotoOutcome CloudPhotoClient::registerPhoto(const RegisterPhotoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RegisterPhotoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RegisterPhotoOutcome(RegisterPhotoResult(outcome.result()));
	else
		return RegisterPhotoOutcome(outcome.error());
}

void CloudPhotoClient::registerPhotoAsync(const RegisterPhotoRequest& request, const RegisterPhotoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, registerPhoto(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::RegisterPhotoOutcomeCallable CloudPhotoClient::registerPhotoCallable(const RegisterPhotoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RegisterPhotoOutcome()>>(
			[this, request]()
			{
			return this->registerPhoto(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::RegisterTagOutcome CloudPhotoClient::registerTag(const RegisterTagRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RegisterTagOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RegisterTagOutcome(RegisterTagResult(outcome.result()));
	else
		return RegisterTagOutcome(outcome.error());
}

void CloudPhotoClient::registerTagAsync(const RegisterTagRequest& request, const RegisterTagAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, registerTag(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::RegisterTagOutcomeCallable CloudPhotoClient::registerTagCallable(const RegisterTagRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RegisterTagOutcome()>>(
			[this, request]()
			{
			return this->registerTag(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::RemoveAlbumPhotosOutcome CloudPhotoClient::removeAlbumPhotos(const RemoveAlbumPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveAlbumPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveAlbumPhotosOutcome(RemoveAlbumPhotosResult(outcome.result()));
	else
		return RemoveAlbumPhotosOutcome(outcome.error());
}

void CloudPhotoClient::removeAlbumPhotosAsync(const RemoveAlbumPhotosRequest& request, const RemoveAlbumPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeAlbumPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::RemoveAlbumPhotosOutcomeCallable CloudPhotoClient::removeAlbumPhotosCallable(const RemoveAlbumPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveAlbumPhotosOutcome()>>(
			[this, request]()
			{
			return this->removeAlbumPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::RemoveFacePhotosOutcome CloudPhotoClient::removeFacePhotos(const RemoveFacePhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RemoveFacePhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RemoveFacePhotosOutcome(RemoveFacePhotosResult(outcome.result()));
	else
		return RemoveFacePhotosOutcome(outcome.error());
}

void CloudPhotoClient::removeFacePhotosAsync(const RemoveFacePhotosRequest& request, const RemoveFacePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, removeFacePhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::RemoveFacePhotosOutcomeCallable CloudPhotoClient::removeFacePhotosCallable(const RemoveFacePhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RemoveFacePhotosOutcome()>>(
			[this, request]()
			{
			return this->removeFacePhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::RenameAlbumOutcome CloudPhotoClient::renameAlbum(const RenameAlbumRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RenameAlbumOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RenameAlbumOutcome(RenameAlbumResult(outcome.result()));
	else
		return RenameAlbumOutcome(outcome.error());
}

void CloudPhotoClient::renameAlbumAsync(const RenameAlbumRequest& request, const RenameAlbumAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, renameAlbum(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::RenameAlbumOutcomeCallable CloudPhotoClient::renameAlbumCallable(const RenameAlbumRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RenameAlbumOutcome()>>(
			[this, request]()
			{
			return this->renameAlbum(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::RenameFaceOutcome CloudPhotoClient::renameFace(const RenameFaceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RenameFaceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RenameFaceOutcome(RenameFaceResult(outcome.result()));
	else
		return RenameFaceOutcome(outcome.error());
}

void CloudPhotoClient::renameFaceAsync(const RenameFaceRequest& request, const RenameFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, renameFace(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::RenameFaceOutcomeCallable CloudPhotoClient::renameFaceCallable(const RenameFaceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RenameFaceOutcome()>>(
			[this, request]()
			{
			return this->renameFace(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::SearchPhotosOutcome CloudPhotoClient::searchPhotos(const SearchPhotosRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SearchPhotosOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SearchPhotosOutcome(SearchPhotosResult(outcome.result()));
	else
		return SearchPhotosOutcome(outcome.error());
}

void CloudPhotoClient::searchPhotosAsync(const SearchPhotosRequest& request, const SearchPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, searchPhotos(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::SearchPhotosOutcomeCallable CloudPhotoClient::searchPhotosCallable(const SearchPhotosRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SearchPhotosOutcome()>>(
			[this, request]()
			{
			return this->searchPhotos(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::SetAlbumCoverOutcome CloudPhotoClient::setAlbumCover(const SetAlbumCoverRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetAlbumCoverOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetAlbumCoverOutcome(SetAlbumCoverResult(outcome.result()));
	else
		return SetAlbumCoverOutcome(outcome.error());
}

void CloudPhotoClient::setAlbumCoverAsync(const SetAlbumCoverRequest& request, const SetAlbumCoverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setAlbumCover(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::SetAlbumCoverOutcomeCallable CloudPhotoClient::setAlbumCoverCallable(const SetAlbumCoverRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetAlbumCoverOutcome()>>(
			[this, request]()
			{
			return this->setAlbumCover(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::SetFaceCoverOutcome CloudPhotoClient::setFaceCover(const SetFaceCoverRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetFaceCoverOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetFaceCoverOutcome(SetFaceCoverResult(outcome.result()));
	else
		return SetFaceCoverOutcome(outcome.error());
}

void CloudPhotoClient::setFaceCoverAsync(const SetFaceCoverRequest& request, const SetFaceCoverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setFaceCover(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::SetFaceCoverOutcomeCallable CloudPhotoClient::setFaceCoverCallable(const SetFaceCoverRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetFaceCoverOutcome()>>(
			[this, request]()
			{
			return this->setFaceCover(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::SetMeOutcome CloudPhotoClient::setMe(const SetMeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetMeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetMeOutcome(SetMeResult(outcome.result()));
	else
		return SetMeOutcome(outcome.error());
}

void CloudPhotoClient::setMeAsync(const SetMeRequest& request, const SetMeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setMe(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::SetMeOutcomeCallable CloudPhotoClient::setMeCallable(const SetMeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetMeOutcome()>>(
			[this, request]()
			{
			return this->setMe(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::SetQuotaOutcome CloudPhotoClient::setQuota(const SetQuotaRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return SetQuotaOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return SetQuotaOutcome(SetQuotaResult(outcome.result()));
	else
		return SetQuotaOutcome(outcome.error());
}

void CloudPhotoClient::setQuotaAsync(const SetQuotaRequest& request, const SetQuotaAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, setQuota(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::SetQuotaOutcomeCallable CloudPhotoClient::setQuotaCallable(const SetQuotaRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<SetQuotaOutcome()>>(
			[this, request]()
			{
			return this->setQuota(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::TagPhotoOutcome CloudPhotoClient::tagPhoto(const TagPhotoRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return TagPhotoOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return TagPhotoOutcome(TagPhotoResult(outcome.result()));
	else
		return TagPhotoOutcome(outcome.error());
}

void CloudPhotoClient::tagPhotoAsync(const TagPhotoRequest& request, const TagPhotoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, tagPhoto(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::TagPhotoOutcomeCallable CloudPhotoClient::tagPhotoCallable(const TagPhotoRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<TagPhotoOutcome()>>(
			[this, request]()
			{
			return this->tagPhoto(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

CloudPhotoClient::ToggleFeaturesOutcome CloudPhotoClient::toggleFeatures(const ToggleFeaturesRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return ToggleFeaturesOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return ToggleFeaturesOutcome(ToggleFeaturesResult(outcome.result()));
	else
		return ToggleFeaturesOutcome(outcome.error());
}

void CloudPhotoClient::toggleFeaturesAsync(const ToggleFeaturesRequest& request, const ToggleFeaturesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, toggleFeatures(request), context);
	};

	asyncExecute(new Runnable(fn));
}

CloudPhotoClient::ToggleFeaturesOutcomeCallable CloudPhotoClient::toggleFeaturesCallable(const ToggleFeaturesRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<ToggleFeaturesOutcome()>>(
			[this, request]()
			{
			return this->toggleFeatures(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


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

#ifndef ALIBABACLOUD_CLOUDPHOTO_CLOUDPHOTOCLIENT_H_
#define ALIBABACLOUD_CLOUDPHOTO_CLOUDPHOTOCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "CloudPhotoExport.h"
#include "model/ActivatePhotosRequest.h"
#include "model/ActivatePhotosResult.h"
#include "model/AddAlbumPhotosRequest.h"
#include "model/AddAlbumPhotosResult.h"
#include "model/CreateAlbumRequest.h"
#include "model/CreateAlbumResult.h"
#include "model/CreateEventRequest.h"
#include "model/CreateEventResult.h"
#include "model/CreatePhotoRequest.h"
#include "model/CreatePhotoResult.h"
#include "model/CreatePhotoStoreRequest.h"
#include "model/CreatePhotoStoreResult.h"
#include "model/CreateTransactionRequest.h"
#include "model/CreateTransactionResult.h"
#include "model/DeleteAlbumsRequest.h"
#include "model/DeleteAlbumsResult.h"
#include "model/DeleteEventRequest.h"
#include "model/DeleteEventResult.h"
#include "model/DeleteFacesRequest.h"
#include "model/DeleteFacesResult.h"
#include "model/DeletePhotoStoreRequest.h"
#include "model/DeletePhotoStoreResult.h"
#include "model/DeletePhotosRequest.h"
#include "model/DeletePhotosResult.h"
#include "model/EditEventRequest.h"
#include "model/EditEventResult.h"
#include "model/EditPhotoStoreRequest.h"
#include "model/EditPhotoStoreResult.h"
#include "model/EditPhotosRequest.h"
#include "model/EditPhotosResult.h"
#include "model/FetchAlbumTagPhotosRequest.h"
#include "model/FetchAlbumTagPhotosResult.h"
#include "model/FetchLibrariesRequest.h"
#include "model/FetchLibrariesResult.h"
#include "model/FetchMomentPhotosRequest.h"
#include "model/FetchMomentPhotosResult.h"
#include "model/FetchPhotosRequest.h"
#include "model/FetchPhotosResult.h"
#include "model/GetAlbumsByNamesRequest.h"
#include "model/GetAlbumsByNamesResult.h"
#include "model/GetDownloadUrlRequest.h"
#include "model/GetDownloadUrlResult.h"
#include "model/GetDownloadUrlsRequest.h"
#include "model/GetDownloadUrlsResult.h"
#include "model/GetEventRequest.h"
#include "model/GetEventResult.h"
#include "model/GetFramedPhotoUrlsRequest.h"
#include "model/GetFramedPhotoUrlsResult.h"
#include "model/GetLibraryRequest.h"
#include "model/GetLibraryResult.h"
#include "model/GetPhotoStoreRequest.h"
#include "model/GetPhotoStoreResult.h"
#include "model/GetPhotosRequest.h"
#include "model/GetPhotosResult.h"
#include "model/GetPhotosByMd5sRequest.h"
#include "model/GetPhotosByMd5sResult.h"
#include "model/GetPrivateAccessUrlsRequest.h"
#include "model/GetPrivateAccessUrlsResult.h"
#include "model/GetPublicAccessUrlsRequest.h"
#include "model/GetPublicAccessUrlsResult.h"
#include "model/GetQuotaRequest.h"
#include "model/GetQuotaResult.h"
#include "model/GetSimilarPhotosRequest.h"
#include "model/GetSimilarPhotosResult.h"
#include "model/GetThumbnailRequest.h"
#include "model/GetThumbnailResult.h"
#include "model/GetThumbnailsRequest.h"
#include "model/GetThumbnailsResult.h"
#include "model/GetVideoCoverRequest.h"
#include "model/GetVideoCoverResult.h"
#include "model/InactivatePhotosRequest.h"
#include "model/InactivatePhotosResult.h"
#include "model/LikePhotoRequest.h"
#include "model/LikePhotoResult.h"
#include "model/ListAlbumPhotosRequest.h"
#include "model/ListAlbumPhotosResult.h"
#include "model/ListAlbumsRequest.h"
#include "model/ListAlbumsResult.h"
#include "model/ListEventsRequest.h"
#include "model/ListEventsResult.h"
#include "model/ListFacePhotosRequest.h"
#include "model/ListFacePhotosResult.h"
#include "model/ListFacesRequest.h"
#include "model/ListFacesResult.h"
#include "model/ListMomentPhotosRequest.h"
#include "model/ListMomentPhotosResult.h"
#include "model/ListMomentsRequest.h"
#include "model/ListMomentsResult.h"
#include "model/ListPhotoFacesRequest.h"
#include "model/ListPhotoFacesResult.h"
#include "model/ListPhotoStoresRequest.h"
#include "model/ListPhotoStoresResult.h"
#include "model/ListPhotoTagsRequest.h"
#include "model/ListPhotoTagsResult.h"
#include "model/ListPhotosRequest.h"
#include "model/ListPhotosResult.h"
#include "model/ListRegisteredTagsRequest.h"
#include "model/ListRegisteredTagsResult.h"
#include "model/ListTagPhotosRequest.h"
#include "model/ListTagPhotosResult.h"
#include "model/ListTagsRequest.h"
#include "model/ListTagsResult.h"
#include "model/ListTimeLinePhotosRequest.h"
#include "model/ListTimeLinePhotosResult.h"
#include "model/ListTimeLinesRequest.h"
#include "model/ListTimeLinesResult.h"
#include "model/MergeFacesRequest.h"
#include "model/MergeFacesResult.h"
#include "model/MoveAlbumPhotosRequest.h"
#include "model/MoveAlbumPhotosResult.h"
#include "model/MoveFacePhotosRequest.h"
#include "model/MoveFacePhotosResult.h"
#include "model/ReactivatePhotosRequest.h"
#include "model/ReactivatePhotosResult.h"
#include "model/RegisterPhotoRequest.h"
#include "model/RegisterPhotoResult.h"
#include "model/RegisterTagRequest.h"
#include "model/RegisterTagResult.h"
#include "model/RemoveAlbumPhotosRequest.h"
#include "model/RemoveAlbumPhotosResult.h"
#include "model/RemoveFacePhotosRequest.h"
#include "model/RemoveFacePhotosResult.h"
#include "model/RenameAlbumRequest.h"
#include "model/RenameAlbumResult.h"
#include "model/RenameFaceRequest.h"
#include "model/RenameFaceResult.h"
#include "model/SearchPhotosRequest.h"
#include "model/SearchPhotosResult.h"
#include "model/SetAlbumCoverRequest.h"
#include "model/SetAlbumCoverResult.h"
#include "model/SetFaceCoverRequest.h"
#include "model/SetFaceCoverResult.h"
#include "model/SetMeRequest.h"
#include "model/SetMeResult.h"
#include "model/SetQuotaRequest.h"
#include "model/SetQuotaResult.h"
#include "model/TagPhotoRequest.h"
#include "model/TagPhotoResult.h"
#include "model/ToggleFeaturesRequest.h"
#include "model/ToggleFeaturesResult.h"


namespace AlibabaCloud
{
	namespace CloudPhoto
	{
		class ALIBABACLOUD_CLOUDPHOTO_EXPORT CloudPhotoClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::ActivatePhotosResult> ActivatePhotosOutcome;
			typedef std::future<ActivatePhotosOutcome> ActivatePhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ActivatePhotosRequest&, const ActivatePhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ActivatePhotosAsyncHandler;
			typedef Outcome<Error, Model::AddAlbumPhotosResult> AddAlbumPhotosOutcome;
			typedef std::future<AddAlbumPhotosOutcome> AddAlbumPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::AddAlbumPhotosRequest&, const AddAlbumPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddAlbumPhotosAsyncHandler;
			typedef Outcome<Error, Model::CreateAlbumResult> CreateAlbumOutcome;
			typedef std::future<CreateAlbumOutcome> CreateAlbumOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::CreateAlbumRequest&, const CreateAlbumOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateAlbumAsyncHandler;
			typedef Outcome<Error, Model::CreateEventResult> CreateEventOutcome;
			typedef std::future<CreateEventOutcome> CreateEventOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::CreateEventRequest&, const CreateEventOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateEventAsyncHandler;
			typedef Outcome<Error, Model::CreatePhotoResult> CreatePhotoOutcome;
			typedef std::future<CreatePhotoOutcome> CreatePhotoOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::CreatePhotoRequest&, const CreatePhotoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreatePhotoAsyncHandler;
			typedef Outcome<Error, Model::CreatePhotoStoreResult> CreatePhotoStoreOutcome;
			typedef std::future<CreatePhotoStoreOutcome> CreatePhotoStoreOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::CreatePhotoStoreRequest&, const CreatePhotoStoreOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreatePhotoStoreAsyncHandler;
			typedef Outcome<Error, Model::CreateTransactionResult> CreateTransactionOutcome;
			typedef std::future<CreateTransactionOutcome> CreateTransactionOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::CreateTransactionRequest&, const CreateTransactionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateTransactionAsyncHandler;
			typedef Outcome<Error, Model::DeleteAlbumsResult> DeleteAlbumsOutcome;
			typedef std::future<DeleteAlbumsOutcome> DeleteAlbumsOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::DeleteAlbumsRequest&, const DeleteAlbumsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteAlbumsAsyncHandler;
			typedef Outcome<Error, Model::DeleteEventResult> DeleteEventOutcome;
			typedef std::future<DeleteEventOutcome> DeleteEventOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::DeleteEventRequest&, const DeleteEventOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteEventAsyncHandler;
			typedef Outcome<Error, Model::DeleteFacesResult> DeleteFacesOutcome;
			typedef std::future<DeleteFacesOutcome> DeleteFacesOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::DeleteFacesRequest&, const DeleteFacesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteFacesAsyncHandler;
			typedef Outcome<Error, Model::DeletePhotoStoreResult> DeletePhotoStoreOutcome;
			typedef std::future<DeletePhotoStoreOutcome> DeletePhotoStoreOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::DeletePhotoStoreRequest&, const DeletePhotoStoreOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeletePhotoStoreAsyncHandler;
			typedef Outcome<Error, Model::DeletePhotosResult> DeletePhotosOutcome;
			typedef std::future<DeletePhotosOutcome> DeletePhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::DeletePhotosRequest&, const DeletePhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeletePhotosAsyncHandler;
			typedef Outcome<Error, Model::EditEventResult> EditEventOutcome;
			typedef std::future<EditEventOutcome> EditEventOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::EditEventRequest&, const EditEventOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EditEventAsyncHandler;
			typedef Outcome<Error, Model::EditPhotoStoreResult> EditPhotoStoreOutcome;
			typedef std::future<EditPhotoStoreOutcome> EditPhotoStoreOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::EditPhotoStoreRequest&, const EditPhotoStoreOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EditPhotoStoreAsyncHandler;
			typedef Outcome<Error, Model::EditPhotosResult> EditPhotosOutcome;
			typedef std::future<EditPhotosOutcome> EditPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::EditPhotosRequest&, const EditPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EditPhotosAsyncHandler;
			typedef Outcome<Error, Model::FetchAlbumTagPhotosResult> FetchAlbumTagPhotosOutcome;
			typedef std::future<FetchAlbumTagPhotosOutcome> FetchAlbumTagPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::FetchAlbumTagPhotosRequest&, const FetchAlbumTagPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> FetchAlbumTagPhotosAsyncHandler;
			typedef Outcome<Error, Model::FetchLibrariesResult> FetchLibrariesOutcome;
			typedef std::future<FetchLibrariesOutcome> FetchLibrariesOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::FetchLibrariesRequest&, const FetchLibrariesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> FetchLibrariesAsyncHandler;
			typedef Outcome<Error, Model::FetchMomentPhotosResult> FetchMomentPhotosOutcome;
			typedef std::future<FetchMomentPhotosOutcome> FetchMomentPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::FetchMomentPhotosRequest&, const FetchMomentPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> FetchMomentPhotosAsyncHandler;
			typedef Outcome<Error, Model::FetchPhotosResult> FetchPhotosOutcome;
			typedef std::future<FetchPhotosOutcome> FetchPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::FetchPhotosRequest&, const FetchPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> FetchPhotosAsyncHandler;
			typedef Outcome<Error, Model::GetAlbumsByNamesResult> GetAlbumsByNamesOutcome;
			typedef std::future<GetAlbumsByNamesOutcome> GetAlbumsByNamesOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetAlbumsByNamesRequest&, const GetAlbumsByNamesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetAlbumsByNamesAsyncHandler;
			typedef Outcome<Error, Model::GetDownloadUrlResult> GetDownloadUrlOutcome;
			typedef std::future<GetDownloadUrlOutcome> GetDownloadUrlOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetDownloadUrlRequest&, const GetDownloadUrlOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDownloadUrlAsyncHandler;
			typedef Outcome<Error, Model::GetDownloadUrlsResult> GetDownloadUrlsOutcome;
			typedef std::future<GetDownloadUrlsOutcome> GetDownloadUrlsOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetDownloadUrlsRequest&, const GetDownloadUrlsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDownloadUrlsAsyncHandler;
			typedef Outcome<Error, Model::GetEventResult> GetEventOutcome;
			typedef std::future<GetEventOutcome> GetEventOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetEventRequest&, const GetEventOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetEventAsyncHandler;
			typedef Outcome<Error, Model::GetFramedPhotoUrlsResult> GetFramedPhotoUrlsOutcome;
			typedef std::future<GetFramedPhotoUrlsOutcome> GetFramedPhotoUrlsOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetFramedPhotoUrlsRequest&, const GetFramedPhotoUrlsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetFramedPhotoUrlsAsyncHandler;
			typedef Outcome<Error, Model::GetLibraryResult> GetLibraryOutcome;
			typedef std::future<GetLibraryOutcome> GetLibraryOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetLibraryRequest&, const GetLibraryOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetLibraryAsyncHandler;
			typedef Outcome<Error, Model::GetPhotoStoreResult> GetPhotoStoreOutcome;
			typedef std::future<GetPhotoStoreOutcome> GetPhotoStoreOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetPhotoStoreRequest&, const GetPhotoStoreOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetPhotoStoreAsyncHandler;
			typedef Outcome<Error, Model::GetPhotosResult> GetPhotosOutcome;
			typedef std::future<GetPhotosOutcome> GetPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetPhotosRequest&, const GetPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetPhotosAsyncHandler;
			typedef Outcome<Error, Model::GetPhotosByMd5sResult> GetPhotosByMd5sOutcome;
			typedef std::future<GetPhotosByMd5sOutcome> GetPhotosByMd5sOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetPhotosByMd5sRequest&, const GetPhotosByMd5sOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetPhotosByMd5sAsyncHandler;
			typedef Outcome<Error, Model::GetPrivateAccessUrlsResult> GetPrivateAccessUrlsOutcome;
			typedef std::future<GetPrivateAccessUrlsOutcome> GetPrivateAccessUrlsOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetPrivateAccessUrlsRequest&, const GetPrivateAccessUrlsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetPrivateAccessUrlsAsyncHandler;
			typedef Outcome<Error, Model::GetPublicAccessUrlsResult> GetPublicAccessUrlsOutcome;
			typedef std::future<GetPublicAccessUrlsOutcome> GetPublicAccessUrlsOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetPublicAccessUrlsRequest&, const GetPublicAccessUrlsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetPublicAccessUrlsAsyncHandler;
			typedef Outcome<Error, Model::GetQuotaResult> GetQuotaOutcome;
			typedef std::future<GetQuotaOutcome> GetQuotaOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetQuotaRequest&, const GetQuotaOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetQuotaAsyncHandler;
			typedef Outcome<Error, Model::GetSimilarPhotosResult> GetSimilarPhotosOutcome;
			typedef std::future<GetSimilarPhotosOutcome> GetSimilarPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetSimilarPhotosRequest&, const GetSimilarPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetSimilarPhotosAsyncHandler;
			typedef Outcome<Error, Model::GetThumbnailResult> GetThumbnailOutcome;
			typedef std::future<GetThumbnailOutcome> GetThumbnailOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetThumbnailRequest&, const GetThumbnailOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetThumbnailAsyncHandler;
			typedef Outcome<Error, Model::GetThumbnailsResult> GetThumbnailsOutcome;
			typedef std::future<GetThumbnailsOutcome> GetThumbnailsOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetThumbnailsRequest&, const GetThumbnailsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetThumbnailsAsyncHandler;
			typedef Outcome<Error, Model::GetVideoCoverResult> GetVideoCoverOutcome;
			typedef std::future<GetVideoCoverOutcome> GetVideoCoverOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::GetVideoCoverRequest&, const GetVideoCoverOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetVideoCoverAsyncHandler;
			typedef Outcome<Error, Model::InactivatePhotosResult> InactivatePhotosOutcome;
			typedef std::future<InactivatePhotosOutcome> InactivatePhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::InactivatePhotosRequest&, const InactivatePhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> InactivatePhotosAsyncHandler;
			typedef Outcome<Error, Model::LikePhotoResult> LikePhotoOutcome;
			typedef std::future<LikePhotoOutcome> LikePhotoOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::LikePhotoRequest&, const LikePhotoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> LikePhotoAsyncHandler;
			typedef Outcome<Error, Model::ListAlbumPhotosResult> ListAlbumPhotosOutcome;
			typedef std::future<ListAlbumPhotosOutcome> ListAlbumPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListAlbumPhotosRequest&, const ListAlbumPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListAlbumPhotosAsyncHandler;
			typedef Outcome<Error, Model::ListAlbumsResult> ListAlbumsOutcome;
			typedef std::future<ListAlbumsOutcome> ListAlbumsOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListAlbumsRequest&, const ListAlbumsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListAlbumsAsyncHandler;
			typedef Outcome<Error, Model::ListEventsResult> ListEventsOutcome;
			typedef std::future<ListEventsOutcome> ListEventsOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListEventsRequest&, const ListEventsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListEventsAsyncHandler;
			typedef Outcome<Error, Model::ListFacePhotosResult> ListFacePhotosOutcome;
			typedef std::future<ListFacePhotosOutcome> ListFacePhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListFacePhotosRequest&, const ListFacePhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListFacePhotosAsyncHandler;
			typedef Outcome<Error, Model::ListFacesResult> ListFacesOutcome;
			typedef std::future<ListFacesOutcome> ListFacesOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListFacesRequest&, const ListFacesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListFacesAsyncHandler;
			typedef Outcome<Error, Model::ListMomentPhotosResult> ListMomentPhotosOutcome;
			typedef std::future<ListMomentPhotosOutcome> ListMomentPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListMomentPhotosRequest&, const ListMomentPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListMomentPhotosAsyncHandler;
			typedef Outcome<Error, Model::ListMomentsResult> ListMomentsOutcome;
			typedef std::future<ListMomentsOutcome> ListMomentsOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListMomentsRequest&, const ListMomentsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListMomentsAsyncHandler;
			typedef Outcome<Error, Model::ListPhotoFacesResult> ListPhotoFacesOutcome;
			typedef std::future<ListPhotoFacesOutcome> ListPhotoFacesOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListPhotoFacesRequest&, const ListPhotoFacesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListPhotoFacesAsyncHandler;
			typedef Outcome<Error, Model::ListPhotoStoresResult> ListPhotoStoresOutcome;
			typedef std::future<ListPhotoStoresOutcome> ListPhotoStoresOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListPhotoStoresRequest&, const ListPhotoStoresOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListPhotoStoresAsyncHandler;
			typedef Outcome<Error, Model::ListPhotoTagsResult> ListPhotoTagsOutcome;
			typedef std::future<ListPhotoTagsOutcome> ListPhotoTagsOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListPhotoTagsRequest&, const ListPhotoTagsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListPhotoTagsAsyncHandler;
			typedef Outcome<Error, Model::ListPhotosResult> ListPhotosOutcome;
			typedef std::future<ListPhotosOutcome> ListPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListPhotosRequest&, const ListPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListPhotosAsyncHandler;
			typedef Outcome<Error, Model::ListRegisteredTagsResult> ListRegisteredTagsOutcome;
			typedef std::future<ListRegisteredTagsOutcome> ListRegisteredTagsOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListRegisteredTagsRequest&, const ListRegisteredTagsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListRegisteredTagsAsyncHandler;
			typedef Outcome<Error, Model::ListTagPhotosResult> ListTagPhotosOutcome;
			typedef std::future<ListTagPhotosOutcome> ListTagPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListTagPhotosRequest&, const ListTagPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListTagPhotosAsyncHandler;
			typedef Outcome<Error, Model::ListTagsResult> ListTagsOutcome;
			typedef std::future<ListTagsOutcome> ListTagsOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListTagsRequest&, const ListTagsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListTagsAsyncHandler;
			typedef Outcome<Error, Model::ListTimeLinePhotosResult> ListTimeLinePhotosOutcome;
			typedef std::future<ListTimeLinePhotosOutcome> ListTimeLinePhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListTimeLinePhotosRequest&, const ListTimeLinePhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListTimeLinePhotosAsyncHandler;
			typedef Outcome<Error, Model::ListTimeLinesResult> ListTimeLinesOutcome;
			typedef std::future<ListTimeLinesOutcome> ListTimeLinesOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ListTimeLinesRequest&, const ListTimeLinesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListTimeLinesAsyncHandler;
			typedef Outcome<Error, Model::MergeFacesResult> MergeFacesOutcome;
			typedef std::future<MergeFacesOutcome> MergeFacesOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::MergeFacesRequest&, const MergeFacesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> MergeFacesAsyncHandler;
			typedef Outcome<Error, Model::MoveAlbumPhotosResult> MoveAlbumPhotosOutcome;
			typedef std::future<MoveAlbumPhotosOutcome> MoveAlbumPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::MoveAlbumPhotosRequest&, const MoveAlbumPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> MoveAlbumPhotosAsyncHandler;
			typedef Outcome<Error, Model::MoveFacePhotosResult> MoveFacePhotosOutcome;
			typedef std::future<MoveFacePhotosOutcome> MoveFacePhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::MoveFacePhotosRequest&, const MoveFacePhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> MoveFacePhotosAsyncHandler;
			typedef Outcome<Error, Model::ReactivatePhotosResult> ReactivatePhotosOutcome;
			typedef std::future<ReactivatePhotosOutcome> ReactivatePhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ReactivatePhotosRequest&, const ReactivatePhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ReactivatePhotosAsyncHandler;
			typedef Outcome<Error, Model::RegisterPhotoResult> RegisterPhotoOutcome;
			typedef std::future<RegisterPhotoOutcome> RegisterPhotoOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::RegisterPhotoRequest&, const RegisterPhotoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RegisterPhotoAsyncHandler;
			typedef Outcome<Error, Model::RegisterTagResult> RegisterTagOutcome;
			typedef std::future<RegisterTagOutcome> RegisterTagOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::RegisterTagRequest&, const RegisterTagOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RegisterTagAsyncHandler;
			typedef Outcome<Error, Model::RemoveAlbumPhotosResult> RemoveAlbumPhotosOutcome;
			typedef std::future<RemoveAlbumPhotosOutcome> RemoveAlbumPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::RemoveAlbumPhotosRequest&, const RemoveAlbumPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RemoveAlbumPhotosAsyncHandler;
			typedef Outcome<Error, Model::RemoveFacePhotosResult> RemoveFacePhotosOutcome;
			typedef std::future<RemoveFacePhotosOutcome> RemoveFacePhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::RemoveFacePhotosRequest&, const RemoveFacePhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RemoveFacePhotosAsyncHandler;
			typedef Outcome<Error, Model::RenameAlbumResult> RenameAlbumOutcome;
			typedef std::future<RenameAlbumOutcome> RenameAlbumOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::RenameAlbumRequest&, const RenameAlbumOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RenameAlbumAsyncHandler;
			typedef Outcome<Error, Model::RenameFaceResult> RenameFaceOutcome;
			typedef std::future<RenameFaceOutcome> RenameFaceOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::RenameFaceRequest&, const RenameFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RenameFaceAsyncHandler;
			typedef Outcome<Error, Model::SearchPhotosResult> SearchPhotosOutcome;
			typedef std::future<SearchPhotosOutcome> SearchPhotosOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::SearchPhotosRequest&, const SearchPhotosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SearchPhotosAsyncHandler;
			typedef Outcome<Error, Model::SetAlbumCoverResult> SetAlbumCoverOutcome;
			typedef std::future<SetAlbumCoverOutcome> SetAlbumCoverOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::SetAlbumCoverRequest&, const SetAlbumCoverOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetAlbumCoverAsyncHandler;
			typedef Outcome<Error, Model::SetFaceCoverResult> SetFaceCoverOutcome;
			typedef std::future<SetFaceCoverOutcome> SetFaceCoverOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::SetFaceCoverRequest&, const SetFaceCoverOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetFaceCoverAsyncHandler;
			typedef Outcome<Error, Model::SetMeResult> SetMeOutcome;
			typedef std::future<SetMeOutcome> SetMeOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::SetMeRequest&, const SetMeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetMeAsyncHandler;
			typedef Outcome<Error, Model::SetQuotaResult> SetQuotaOutcome;
			typedef std::future<SetQuotaOutcome> SetQuotaOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::SetQuotaRequest&, const SetQuotaOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetQuotaAsyncHandler;
			typedef Outcome<Error, Model::TagPhotoResult> TagPhotoOutcome;
			typedef std::future<TagPhotoOutcome> TagPhotoOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::TagPhotoRequest&, const TagPhotoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> TagPhotoAsyncHandler;
			typedef Outcome<Error, Model::ToggleFeaturesResult> ToggleFeaturesOutcome;
			typedef std::future<ToggleFeaturesOutcome> ToggleFeaturesOutcomeCallable;
			typedef std::function<void(const CloudPhotoClient*, const Model::ToggleFeaturesRequest&, const ToggleFeaturesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ToggleFeaturesAsyncHandler;

			CloudPhotoClient(const Credentials &credentials, const ClientConfiguration &configuration);
			CloudPhotoClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			CloudPhotoClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~CloudPhotoClient();
			ActivatePhotosOutcome activatePhotos(const Model::ActivatePhotosRequest &request)const;
			void activatePhotosAsync(const Model::ActivatePhotosRequest& request, const ActivatePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ActivatePhotosOutcomeCallable activatePhotosCallable(const Model::ActivatePhotosRequest& request) const;
			AddAlbumPhotosOutcome addAlbumPhotos(const Model::AddAlbumPhotosRequest &request)const;
			void addAlbumPhotosAsync(const Model::AddAlbumPhotosRequest& request, const AddAlbumPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddAlbumPhotosOutcomeCallable addAlbumPhotosCallable(const Model::AddAlbumPhotosRequest& request) const;
			CreateAlbumOutcome createAlbum(const Model::CreateAlbumRequest &request)const;
			void createAlbumAsync(const Model::CreateAlbumRequest& request, const CreateAlbumAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateAlbumOutcomeCallable createAlbumCallable(const Model::CreateAlbumRequest& request) const;
			CreateEventOutcome createEvent(const Model::CreateEventRequest &request)const;
			void createEventAsync(const Model::CreateEventRequest& request, const CreateEventAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateEventOutcomeCallable createEventCallable(const Model::CreateEventRequest& request) const;
			CreatePhotoOutcome createPhoto(const Model::CreatePhotoRequest &request)const;
			void createPhotoAsync(const Model::CreatePhotoRequest& request, const CreatePhotoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreatePhotoOutcomeCallable createPhotoCallable(const Model::CreatePhotoRequest& request) const;
			CreatePhotoStoreOutcome createPhotoStore(const Model::CreatePhotoStoreRequest &request)const;
			void createPhotoStoreAsync(const Model::CreatePhotoStoreRequest& request, const CreatePhotoStoreAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreatePhotoStoreOutcomeCallable createPhotoStoreCallable(const Model::CreatePhotoStoreRequest& request) const;
			CreateTransactionOutcome createTransaction(const Model::CreateTransactionRequest &request)const;
			void createTransactionAsync(const Model::CreateTransactionRequest& request, const CreateTransactionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateTransactionOutcomeCallable createTransactionCallable(const Model::CreateTransactionRequest& request) const;
			DeleteAlbumsOutcome deleteAlbums(const Model::DeleteAlbumsRequest &request)const;
			void deleteAlbumsAsync(const Model::DeleteAlbumsRequest& request, const DeleteAlbumsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteAlbumsOutcomeCallable deleteAlbumsCallable(const Model::DeleteAlbumsRequest& request) const;
			DeleteEventOutcome deleteEvent(const Model::DeleteEventRequest &request)const;
			void deleteEventAsync(const Model::DeleteEventRequest& request, const DeleteEventAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteEventOutcomeCallable deleteEventCallable(const Model::DeleteEventRequest& request) const;
			DeleteFacesOutcome deleteFaces(const Model::DeleteFacesRequest &request)const;
			void deleteFacesAsync(const Model::DeleteFacesRequest& request, const DeleteFacesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteFacesOutcomeCallable deleteFacesCallable(const Model::DeleteFacesRequest& request) const;
			DeletePhotoStoreOutcome deletePhotoStore(const Model::DeletePhotoStoreRequest &request)const;
			void deletePhotoStoreAsync(const Model::DeletePhotoStoreRequest& request, const DeletePhotoStoreAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeletePhotoStoreOutcomeCallable deletePhotoStoreCallable(const Model::DeletePhotoStoreRequest& request) const;
			DeletePhotosOutcome deletePhotos(const Model::DeletePhotosRequest &request)const;
			void deletePhotosAsync(const Model::DeletePhotosRequest& request, const DeletePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeletePhotosOutcomeCallable deletePhotosCallable(const Model::DeletePhotosRequest& request) const;
			EditEventOutcome editEvent(const Model::EditEventRequest &request)const;
			void editEventAsync(const Model::EditEventRequest& request, const EditEventAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EditEventOutcomeCallable editEventCallable(const Model::EditEventRequest& request) const;
			EditPhotoStoreOutcome editPhotoStore(const Model::EditPhotoStoreRequest &request)const;
			void editPhotoStoreAsync(const Model::EditPhotoStoreRequest& request, const EditPhotoStoreAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EditPhotoStoreOutcomeCallable editPhotoStoreCallable(const Model::EditPhotoStoreRequest& request) const;
			EditPhotosOutcome editPhotos(const Model::EditPhotosRequest &request)const;
			void editPhotosAsync(const Model::EditPhotosRequest& request, const EditPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EditPhotosOutcomeCallable editPhotosCallable(const Model::EditPhotosRequest& request) const;
			FetchAlbumTagPhotosOutcome fetchAlbumTagPhotos(const Model::FetchAlbumTagPhotosRequest &request)const;
			void fetchAlbumTagPhotosAsync(const Model::FetchAlbumTagPhotosRequest& request, const FetchAlbumTagPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			FetchAlbumTagPhotosOutcomeCallable fetchAlbumTagPhotosCallable(const Model::FetchAlbumTagPhotosRequest& request) const;
			FetchLibrariesOutcome fetchLibraries(const Model::FetchLibrariesRequest &request)const;
			void fetchLibrariesAsync(const Model::FetchLibrariesRequest& request, const FetchLibrariesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			FetchLibrariesOutcomeCallable fetchLibrariesCallable(const Model::FetchLibrariesRequest& request) const;
			FetchMomentPhotosOutcome fetchMomentPhotos(const Model::FetchMomentPhotosRequest &request)const;
			void fetchMomentPhotosAsync(const Model::FetchMomentPhotosRequest& request, const FetchMomentPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			FetchMomentPhotosOutcomeCallable fetchMomentPhotosCallable(const Model::FetchMomentPhotosRequest& request) const;
			FetchPhotosOutcome fetchPhotos(const Model::FetchPhotosRequest &request)const;
			void fetchPhotosAsync(const Model::FetchPhotosRequest& request, const FetchPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			FetchPhotosOutcomeCallable fetchPhotosCallable(const Model::FetchPhotosRequest& request) const;
			GetAlbumsByNamesOutcome getAlbumsByNames(const Model::GetAlbumsByNamesRequest &request)const;
			void getAlbumsByNamesAsync(const Model::GetAlbumsByNamesRequest& request, const GetAlbumsByNamesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetAlbumsByNamesOutcomeCallable getAlbumsByNamesCallable(const Model::GetAlbumsByNamesRequest& request) const;
			GetDownloadUrlOutcome getDownloadUrl(const Model::GetDownloadUrlRequest &request)const;
			void getDownloadUrlAsync(const Model::GetDownloadUrlRequest& request, const GetDownloadUrlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDownloadUrlOutcomeCallable getDownloadUrlCallable(const Model::GetDownloadUrlRequest& request) const;
			GetDownloadUrlsOutcome getDownloadUrls(const Model::GetDownloadUrlsRequest &request)const;
			void getDownloadUrlsAsync(const Model::GetDownloadUrlsRequest& request, const GetDownloadUrlsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDownloadUrlsOutcomeCallable getDownloadUrlsCallable(const Model::GetDownloadUrlsRequest& request) const;
			GetEventOutcome getEvent(const Model::GetEventRequest &request)const;
			void getEventAsync(const Model::GetEventRequest& request, const GetEventAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetEventOutcomeCallable getEventCallable(const Model::GetEventRequest& request) const;
			GetFramedPhotoUrlsOutcome getFramedPhotoUrls(const Model::GetFramedPhotoUrlsRequest &request)const;
			void getFramedPhotoUrlsAsync(const Model::GetFramedPhotoUrlsRequest& request, const GetFramedPhotoUrlsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetFramedPhotoUrlsOutcomeCallable getFramedPhotoUrlsCallable(const Model::GetFramedPhotoUrlsRequest& request) const;
			GetLibraryOutcome getLibrary(const Model::GetLibraryRequest &request)const;
			void getLibraryAsync(const Model::GetLibraryRequest& request, const GetLibraryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetLibraryOutcomeCallable getLibraryCallable(const Model::GetLibraryRequest& request) const;
			GetPhotoStoreOutcome getPhotoStore(const Model::GetPhotoStoreRequest &request)const;
			void getPhotoStoreAsync(const Model::GetPhotoStoreRequest& request, const GetPhotoStoreAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetPhotoStoreOutcomeCallable getPhotoStoreCallable(const Model::GetPhotoStoreRequest& request) const;
			GetPhotosOutcome getPhotos(const Model::GetPhotosRequest &request)const;
			void getPhotosAsync(const Model::GetPhotosRequest& request, const GetPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetPhotosOutcomeCallable getPhotosCallable(const Model::GetPhotosRequest& request) const;
			GetPhotosByMd5sOutcome getPhotosByMd5s(const Model::GetPhotosByMd5sRequest &request)const;
			void getPhotosByMd5sAsync(const Model::GetPhotosByMd5sRequest& request, const GetPhotosByMd5sAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetPhotosByMd5sOutcomeCallable getPhotosByMd5sCallable(const Model::GetPhotosByMd5sRequest& request) const;
			GetPrivateAccessUrlsOutcome getPrivateAccessUrls(const Model::GetPrivateAccessUrlsRequest &request)const;
			void getPrivateAccessUrlsAsync(const Model::GetPrivateAccessUrlsRequest& request, const GetPrivateAccessUrlsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetPrivateAccessUrlsOutcomeCallable getPrivateAccessUrlsCallable(const Model::GetPrivateAccessUrlsRequest& request) const;
			GetPublicAccessUrlsOutcome getPublicAccessUrls(const Model::GetPublicAccessUrlsRequest &request)const;
			void getPublicAccessUrlsAsync(const Model::GetPublicAccessUrlsRequest& request, const GetPublicAccessUrlsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetPublicAccessUrlsOutcomeCallable getPublicAccessUrlsCallable(const Model::GetPublicAccessUrlsRequest& request) const;
			GetQuotaOutcome getQuota(const Model::GetQuotaRequest &request)const;
			void getQuotaAsync(const Model::GetQuotaRequest& request, const GetQuotaAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetQuotaOutcomeCallable getQuotaCallable(const Model::GetQuotaRequest& request) const;
			GetSimilarPhotosOutcome getSimilarPhotos(const Model::GetSimilarPhotosRequest &request)const;
			void getSimilarPhotosAsync(const Model::GetSimilarPhotosRequest& request, const GetSimilarPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetSimilarPhotosOutcomeCallable getSimilarPhotosCallable(const Model::GetSimilarPhotosRequest& request) const;
			GetThumbnailOutcome getThumbnail(const Model::GetThumbnailRequest &request)const;
			void getThumbnailAsync(const Model::GetThumbnailRequest& request, const GetThumbnailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetThumbnailOutcomeCallable getThumbnailCallable(const Model::GetThumbnailRequest& request) const;
			GetThumbnailsOutcome getThumbnails(const Model::GetThumbnailsRequest &request)const;
			void getThumbnailsAsync(const Model::GetThumbnailsRequest& request, const GetThumbnailsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetThumbnailsOutcomeCallable getThumbnailsCallable(const Model::GetThumbnailsRequest& request) const;
			GetVideoCoverOutcome getVideoCover(const Model::GetVideoCoverRequest &request)const;
			void getVideoCoverAsync(const Model::GetVideoCoverRequest& request, const GetVideoCoverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetVideoCoverOutcomeCallable getVideoCoverCallable(const Model::GetVideoCoverRequest& request) const;
			InactivatePhotosOutcome inactivatePhotos(const Model::InactivatePhotosRequest &request)const;
			void inactivatePhotosAsync(const Model::InactivatePhotosRequest& request, const InactivatePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			InactivatePhotosOutcomeCallable inactivatePhotosCallable(const Model::InactivatePhotosRequest& request) const;
			LikePhotoOutcome likePhoto(const Model::LikePhotoRequest &request)const;
			void likePhotoAsync(const Model::LikePhotoRequest& request, const LikePhotoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			LikePhotoOutcomeCallable likePhotoCallable(const Model::LikePhotoRequest& request) const;
			ListAlbumPhotosOutcome listAlbumPhotos(const Model::ListAlbumPhotosRequest &request)const;
			void listAlbumPhotosAsync(const Model::ListAlbumPhotosRequest& request, const ListAlbumPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListAlbumPhotosOutcomeCallable listAlbumPhotosCallable(const Model::ListAlbumPhotosRequest& request) const;
			ListAlbumsOutcome listAlbums(const Model::ListAlbumsRequest &request)const;
			void listAlbumsAsync(const Model::ListAlbumsRequest& request, const ListAlbumsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListAlbumsOutcomeCallable listAlbumsCallable(const Model::ListAlbumsRequest& request) const;
			ListEventsOutcome listEvents(const Model::ListEventsRequest &request)const;
			void listEventsAsync(const Model::ListEventsRequest& request, const ListEventsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListEventsOutcomeCallable listEventsCallable(const Model::ListEventsRequest& request) const;
			ListFacePhotosOutcome listFacePhotos(const Model::ListFacePhotosRequest &request)const;
			void listFacePhotosAsync(const Model::ListFacePhotosRequest& request, const ListFacePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListFacePhotosOutcomeCallable listFacePhotosCallable(const Model::ListFacePhotosRequest& request) const;
			ListFacesOutcome listFaces(const Model::ListFacesRequest &request)const;
			void listFacesAsync(const Model::ListFacesRequest& request, const ListFacesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListFacesOutcomeCallable listFacesCallable(const Model::ListFacesRequest& request) const;
			ListMomentPhotosOutcome listMomentPhotos(const Model::ListMomentPhotosRequest &request)const;
			void listMomentPhotosAsync(const Model::ListMomentPhotosRequest& request, const ListMomentPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListMomentPhotosOutcomeCallable listMomentPhotosCallable(const Model::ListMomentPhotosRequest& request) const;
			ListMomentsOutcome listMoments(const Model::ListMomentsRequest &request)const;
			void listMomentsAsync(const Model::ListMomentsRequest& request, const ListMomentsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListMomentsOutcomeCallable listMomentsCallable(const Model::ListMomentsRequest& request) const;
			ListPhotoFacesOutcome listPhotoFaces(const Model::ListPhotoFacesRequest &request)const;
			void listPhotoFacesAsync(const Model::ListPhotoFacesRequest& request, const ListPhotoFacesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListPhotoFacesOutcomeCallable listPhotoFacesCallable(const Model::ListPhotoFacesRequest& request) const;
			ListPhotoStoresOutcome listPhotoStores(const Model::ListPhotoStoresRequest &request)const;
			void listPhotoStoresAsync(const Model::ListPhotoStoresRequest& request, const ListPhotoStoresAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListPhotoStoresOutcomeCallable listPhotoStoresCallable(const Model::ListPhotoStoresRequest& request) const;
			ListPhotoTagsOutcome listPhotoTags(const Model::ListPhotoTagsRequest &request)const;
			void listPhotoTagsAsync(const Model::ListPhotoTagsRequest& request, const ListPhotoTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListPhotoTagsOutcomeCallable listPhotoTagsCallable(const Model::ListPhotoTagsRequest& request) const;
			ListPhotosOutcome listPhotos(const Model::ListPhotosRequest &request)const;
			void listPhotosAsync(const Model::ListPhotosRequest& request, const ListPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListPhotosOutcomeCallable listPhotosCallable(const Model::ListPhotosRequest& request) const;
			ListRegisteredTagsOutcome listRegisteredTags(const Model::ListRegisteredTagsRequest &request)const;
			void listRegisteredTagsAsync(const Model::ListRegisteredTagsRequest& request, const ListRegisteredTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListRegisteredTagsOutcomeCallable listRegisteredTagsCallable(const Model::ListRegisteredTagsRequest& request) const;
			ListTagPhotosOutcome listTagPhotos(const Model::ListTagPhotosRequest &request)const;
			void listTagPhotosAsync(const Model::ListTagPhotosRequest& request, const ListTagPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListTagPhotosOutcomeCallable listTagPhotosCallable(const Model::ListTagPhotosRequest& request) const;
			ListTagsOutcome listTags(const Model::ListTagsRequest &request)const;
			void listTagsAsync(const Model::ListTagsRequest& request, const ListTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListTagsOutcomeCallable listTagsCallable(const Model::ListTagsRequest& request) const;
			ListTimeLinePhotosOutcome listTimeLinePhotos(const Model::ListTimeLinePhotosRequest &request)const;
			void listTimeLinePhotosAsync(const Model::ListTimeLinePhotosRequest& request, const ListTimeLinePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListTimeLinePhotosOutcomeCallable listTimeLinePhotosCallable(const Model::ListTimeLinePhotosRequest& request) const;
			ListTimeLinesOutcome listTimeLines(const Model::ListTimeLinesRequest &request)const;
			void listTimeLinesAsync(const Model::ListTimeLinesRequest& request, const ListTimeLinesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListTimeLinesOutcomeCallable listTimeLinesCallable(const Model::ListTimeLinesRequest& request) const;
			MergeFacesOutcome mergeFaces(const Model::MergeFacesRequest &request)const;
			void mergeFacesAsync(const Model::MergeFacesRequest& request, const MergeFacesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			MergeFacesOutcomeCallable mergeFacesCallable(const Model::MergeFacesRequest& request) const;
			MoveAlbumPhotosOutcome moveAlbumPhotos(const Model::MoveAlbumPhotosRequest &request)const;
			void moveAlbumPhotosAsync(const Model::MoveAlbumPhotosRequest& request, const MoveAlbumPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			MoveAlbumPhotosOutcomeCallable moveAlbumPhotosCallable(const Model::MoveAlbumPhotosRequest& request) const;
			MoveFacePhotosOutcome moveFacePhotos(const Model::MoveFacePhotosRequest &request)const;
			void moveFacePhotosAsync(const Model::MoveFacePhotosRequest& request, const MoveFacePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			MoveFacePhotosOutcomeCallable moveFacePhotosCallable(const Model::MoveFacePhotosRequest& request) const;
			ReactivatePhotosOutcome reactivatePhotos(const Model::ReactivatePhotosRequest &request)const;
			void reactivatePhotosAsync(const Model::ReactivatePhotosRequest& request, const ReactivatePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ReactivatePhotosOutcomeCallable reactivatePhotosCallable(const Model::ReactivatePhotosRequest& request) const;
			RegisterPhotoOutcome registerPhoto(const Model::RegisterPhotoRequest &request)const;
			void registerPhotoAsync(const Model::RegisterPhotoRequest& request, const RegisterPhotoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RegisterPhotoOutcomeCallable registerPhotoCallable(const Model::RegisterPhotoRequest& request) const;
			RegisterTagOutcome registerTag(const Model::RegisterTagRequest &request)const;
			void registerTagAsync(const Model::RegisterTagRequest& request, const RegisterTagAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RegisterTagOutcomeCallable registerTagCallable(const Model::RegisterTagRequest& request) const;
			RemoveAlbumPhotosOutcome removeAlbumPhotos(const Model::RemoveAlbumPhotosRequest &request)const;
			void removeAlbumPhotosAsync(const Model::RemoveAlbumPhotosRequest& request, const RemoveAlbumPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RemoveAlbumPhotosOutcomeCallable removeAlbumPhotosCallable(const Model::RemoveAlbumPhotosRequest& request) const;
			RemoveFacePhotosOutcome removeFacePhotos(const Model::RemoveFacePhotosRequest &request)const;
			void removeFacePhotosAsync(const Model::RemoveFacePhotosRequest& request, const RemoveFacePhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RemoveFacePhotosOutcomeCallable removeFacePhotosCallable(const Model::RemoveFacePhotosRequest& request) const;
			RenameAlbumOutcome renameAlbum(const Model::RenameAlbumRequest &request)const;
			void renameAlbumAsync(const Model::RenameAlbumRequest& request, const RenameAlbumAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RenameAlbumOutcomeCallable renameAlbumCallable(const Model::RenameAlbumRequest& request) const;
			RenameFaceOutcome renameFace(const Model::RenameFaceRequest &request)const;
			void renameFaceAsync(const Model::RenameFaceRequest& request, const RenameFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RenameFaceOutcomeCallable renameFaceCallable(const Model::RenameFaceRequest& request) const;
			SearchPhotosOutcome searchPhotos(const Model::SearchPhotosRequest &request)const;
			void searchPhotosAsync(const Model::SearchPhotosRequest& request, const SearchPhotosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SearchPhotosOutcomeCallable searchPhotosCallable(const Model::SearchPhotosRequest& request) const;
			SetAlbumCoverOutcome setAlbumCover(const Model::SetAlbumCoverRequest &request)const;
			void setAlbumCoverAsync(const Model::SetAlbumCoverRequest& request, const SetAlbumCoverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetAlbumCoverOutcomeCallable setAlbumCoverCallable(const Model::SetAlbumCoverRequest& request) const;
			SetFaceCoverOutcome setFaceCover(const Model::SetFaceCoverRequest &request)const;
			void setFaceCoverAsync(const Model::SetFaceCoverRequest& request, const SetFaceCoverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetFaceCoverOutcomeCallable setFaceCoverCallable(const Model::SetFaceCoverRequest& request) const;
			SetMeOutcome setMe(const Model::SetMeRequest &request)const;
			void setMeAsync(const Model::SetMeRequest& request, const SetMeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetMeOutcomeCallable setMeCallable(const Model::SetMeRequest& request) const;
			SetQuotaOutcome setQuota(const Model::SetQuotaRequest &request)const;
			void setQuotaAsync(const Model::SetQuotaRequest& request, const SetQuotaAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetQuotaOutcomeCallable setQuotaCallable(const Model::SetQuotaRequest& request) const;
			TagPhotoOutcome tagPhoto(const Model::TagPhotoRequest &request)const;
			void tagPhotoAsync(const Model::TagPhotoRequest& request, const TagPhotoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			TagPhotoOutcomeCallable tagPhotoCallable(const Model::TagPhotoRequest& request) const;
			ToggleFeaturesOutcome toggleFeatures(const Model::ToggleFeaturesRequest &request)const;
			void toggleFeaturesAsync(const Model::ToggleFeaturesRequest& request, const ToggleFeaturesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ToggleFeaturesOutcomeCallable toggleFeaturesCallable(const Model::ToggleFeaturesRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_CLOUDPHOTO_CLOUDPHOTOCLIENT_H_

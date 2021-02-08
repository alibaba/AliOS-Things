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

#ifndef ALIBABACLOUD_IMGSEARCH_IMGSEARCHCLIENT_H_
#define ALIBABACLOUD_IMGSEARCH_IMGSEARCHCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "ImgsearchExport.h"
#include "model/AddImageRequest.h"
#include "model/AddImageResult.h"
#include "model/CreateImageDbRequest.h"
#include "model/CreateImageDbResult.h"
#include "model/DeleteImageRequest.h"
#include "model/DeleteImageResult.h"
#include "model/DeleteImageDbRequest.h"
#include "model/DeleteImageDbResult.h"
#include "model/ListImageDbsRequest.h"
#include "model/ListImageDbsResult.h"
#include "model/ListImagesRequest.h"
#include "model/ListImagesResult.h"
#include "model/SearchImageRequest.h"
#include "model/SearchImageResult.h"


namespace AlibabaCloud
{
	namespace Imgsearch
	{
		class ALIBABACLOUD_IMGSEARCH_EXPORT ImgsearchClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::AddImageResult> AddImageOutcome;
			typedef std::future<AddImageOutcome> AddImageOutcomeCallable;
			typedef std::function<void(const ImgsearchClient*, const Model::AddImageRequest&, const AddImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddImageAsyncHandler;
			typedef Outcome<Error, Model::CreateImageDbResult> CreateImageDbOutcome;
			typedef std::future<CreateImageDbOutcome> CreateImageDbOutcomeCallable;
			typedef std::function<void(const ImgsearchClient*, const Model::CreateImageDbRequest&, const CreateImageDbOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateImageDbAsyncHandler;
			typedef Outcome<Error, Model::DeleteImageResult> DeleteImageOutcome;
			typedef std::future<DeleteImageOutcome> DeleteImageOutcomeCallable;
			typedef std::function<void(const ImgsearchClient*, const Model::DeleteImageRequest&, const DeleteImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteImageAsyncHandler;
			typedef Outcome<Error, Model::DeleteImageDbResult> DeleteImageDbOutcome;
			typedef std::future<DeleteImageDbOutcome> DeleteImageDbOutcomeCallable;
			typedef std::function<void(const ImgsearchClient*, const Model::DeleteImageDbRequest&, const DeleteImageDbOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteImageDbAsyncHandler;
			typedef Outcome<Error, Model::ListImageDbsResult> ListImageDbsOutcome;
			typedef std::future<ListImageDbsOutcome> ListImageDbsOutcomeCallable;
			typedef std::function<void(const ImgsearchClient*, const Model::ListImageDbsRequest&, const ListImageDbsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListImageDbsAsyncHandler;
			typedef Outcome<Error, Model::ListImagesResult> ListImagesOutcome;
			typedef std::future<ListImagesOutcome> ListImagesOutcomeCallable;
			typedef std::function<void(const ImgsearchClient*, const Model::ListImagesRequest&, const ListImagesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListImagesAsyncHandler;
			typedef Outcome<Error, Model::SearchImageResult> SearchImageOutcome;
			typedef std::future<SearchImageOutcome> SearchImageOutcomeCallable;
			typedef std::function<void(const ImgsearchClient*, const Model::SearchImageRequest&, const SearchImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SearchImageAsyncHandler;

			ImgsearchClient(const Credentials &credentials, const ClientConfiguration &configuration);
			ImgsearchClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			ImgsearchClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~ImgsearchClient();
			AddImageOutcome addImage(const Model::AddImageRequest &request)const;
			void addImageAsync(const Model::AddImageRequest& request, const AddImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddImageOutcomeCallable addImageCallable(const Model::AddImageRequest& request) const;
			CreateImageDbOutcome createImageDb(const Model::CreateImageDbRequest &request)const;
			void createImageDbAsync(const Model::CreateImageDbRequest& request, const CreateImageDbAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateImageDbOutcomeCallable createImageDbCallable(const Model::CreateImageDbRequest& request) const;
			DeleteImageOutcome deleteImage(const Model::DeleteImageRequest &request)const;
			void deleteImageAsync(const Model::DeleteImageRequest& request, const DeleteImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteImageOutcomeCallable deleteImageCallable(const Model::DeleteImageRequest& request) const;
			DeleteImageDbOutcome deleteImageDb(const Model::DeleteImageDbRequest &request)const;
			void deleteImageDbAsync(const Model::DeleteImageDbRequest& request, const DeleteImageDbAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteImageDbOutcomeCallable deleteImageDbCallable(const Model::DeleteImageDbRequest& request) const;
			ListImageDbsOutcome listImageDbs(const Model::ListImageDbsRequest &request)const;
			void listImageDbsAsync(const Model::ListImageDbsRequest& request, const ListImageDbsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListImageDbsOutcomeCallable listImageDbsCallable(const Model::ListImageDbsRequest& request) const;
			ListImagesOutcome listImages(const Model::ListImagesRequest &request)const;
			void listImagesAsync(const Model::ListImagesRequest& request, const ListImagesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListImagesOutcomeCallable listImagesCallable(const Model::ListImagesRequest& request) const;
			SearchImageOutcome searchImage(const Model::SearchImageRequest &request)const;
			void searchImageAsync(const Model::SearchImageRequest& request, const SearchImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SearchImageOutcomeCallable searchImageCallable(const Model::SearchImageRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_IMGSEARCH_IMGSEARCHCLIENT_H_

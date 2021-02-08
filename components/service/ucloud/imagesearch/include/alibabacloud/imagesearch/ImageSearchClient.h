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

#ifndef ALIBABACLOUD_IMAGESEARCH_IMAGESEARCHCLIENT_H_
#define ALIBABACLOUD_IMAGESEARCH_IMAGESEARCHCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RoaServiceClient.h>
#include "ImageSearchExport.h"
#include "model/AddItemRequest.h"
#include "model/AddItemResult.h"
#include "model/DeleteItemRequest.h"
#include "model/DeleteItemResult.h"
#include "model/SearchItemRequest.h"
#include "model/SearchItemResult.h"


namespace AlibabaCloud
{
	namespace ImageSearch
	{
		class ALIBABACLOUD_IMAGESEARCH_EXPORT ImageSearchClient : public RoaServiceClient
		{
		public:
			typedef Outcome<Error, Model::AddItemResult> AddItemOutcome;
			typedef std::future<AddItemOutcome> AddItemOutcomeCallable;
			typedef std::function<void(const ImageSearchClient*, const Model::AddItemRequest&, const AddItemOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddItemAsyncHandler;
			typedef Outcome<Error, Model::DeleteItemResult> DeleteItemOutcome;
			typedef std::future<DeleteItemOutcome> DeleteItemOutcomeCallable;
			typedef std::function<void(const ImageSearchClient*, const Model::DeleteItemRequest&, const DeleteItemOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteItemAsyncHandler;
			typedef Outcome<Error, Model::SearchItemResult> SearchItemOutcome;
			typedef std::future<SearchItemOutcome> SearchItemOutcomeCallable;
			typedef std::function<void(const ImageSearchClient*, const Model::SearchItemRequest&, const SearchItemOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SearchItemAsyncHandler;

			ImageSearchClient(const Credentials &credentials, const ClientConfiguration &configuration);
			ImageSearchClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			ImageSearchClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~ImageSearchClient();
			AddItemOutcome addItem(const Model::AddItemRequest &request)const;
			void addItemAsync(const Model::AddItemRequest& request, const AddItemAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddItemOutcomeCallable addItemCallable(const Model::AddItemRequest& request) const;
			DeleteItemOutcome deleteItem(const Model::DeleteItemRequest &request)const;
			void deleteItemAsync(const Model::DeleteItemRequest& request, const DeleteItemAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteItemOutcomeCallable deleteItemCallable(const Model::DeleteItemRequest& request) const;
			SearchItemOutcome searchItem(const Model::SearchItemRequest &request)const;
			void searchItemAsync(const Model::SearchItemRequest& request, const SearchItemAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SearchItemOutcomeCallable searchItemCallable(const Model::SearchItemRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_IMAGESEARCH_IMAGESEARCHCLIENT_H_

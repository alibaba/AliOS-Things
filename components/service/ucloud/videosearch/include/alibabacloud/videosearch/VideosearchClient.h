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

#ifndef ALIBABACLOUD_VIDEOSEARCH_VIDEOSEARCHCLIENT_H_
#define ALIBABACLOUD_VIDEOSEARCH_VIDEOSEARCHCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "VideosearchExport.h"
#include "model/AddDeletionVideoTaskRequest.h"
#include "model/AddDeletionVideoTaskResult.h"
#include "model/AddSearchVideoTaskRequest.h"
#include "model/AddSearchVideoTaskResult.h"
#include "model/AddStorageVideoTaskRequest.h"
#include "model/AddStorageVideoTaskResult.h"
#include "model/GetTaskStatusRequest.h"
#include "model/GetTaskStatusResult.h"


namespace AlibabaCloud
{
	namespace Videosearch
	{
		class ALIBABACLOUD_VIDEOSEARCH_EXPORT VideosearchClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::AddDeletionVideoTaskResult> AddDeletionVideoTaskOutcome;
			typedef std::future<AddDeletionVideoTaskOutcome> AddDeletionVideoTaskOutcomeCallable;
			typedef std::function<void(const VideosearchClient*, const Model::AddDeletionVideoTaskRequest&, const AddDeletionVideoTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddDeletionVideoTaskAsyncHandler;
			typedef Outcome<Error, Model::AddSearchVideoTaskResult> AddSearchVideoTaskOutcome;
			typedef std::future<AddSearchVideoTaskOutcome> AddSearchVideoTaskOutcomeCallable;
			typedef std::function<void(const VideosearchClient*, const Model::AddSearchVideoTaskRequest&, const AddSearchVideoTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddSearchVideoTaskAsyncHandler;
			typedef Outcome<Error, Model::AddStorageVideoTaskResult> AddStorageVideoTaskOutcome;
			typedef std::future<AddStorageVideoTaskOutcome> AddStorageVideoTaskOutcomeCallable;
			typedef std::function<void(const VideosearchClient*, const Model::AddStorageVideoTaskRequest&, const AddStorageVideoTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddStorageVideoTaskAsyncHandler;
			typedef Outcome<Error, Model::GetTaskStatusResult> GetTaskStatusOutcome;
			typedef std::future<GetTaskStatusOutcome> GetTaskStatusOutcomeCallable;
			typedef std::function<void(const VideosearchClient*, const Model::GetTaskStatusRequest&, const GetTaskStatusOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetTaskStatusAsyncHandler;

			VideosearchClient(const Credentials &credentials, const ClientConfiguration &configuration);
			VideosearchClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			VideosearchClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~VideosearchClient();
			AddDeletionVideoTaskOutcome addDeletionVideoTask(const Model::AddDeletionVideoTaskRequest &request)const;
			void addDeletionVideoTaskAsync(const Model::AddDeletionVideoTaskRequest& request, const AddDeletionVideoTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddDeletionVideoTaskOutcomeCallable addDeletionVideoTaskCallable(const Model::AddDeletionVideoTaskRequest& request) const;
			AddSearchVideoTaskOutcome addSearchVideoTask(const Model::AddSearchVideoTaskRequest &request)const;
			void addSearchVideoTaskAsync(const Model::AddSearchVideoTaskRequest& request, const AddSearchVideoTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddSearchVideoTaskOutcomeCallable addSearchVideoTaskCallable(const Model::AddSearchVideoTaskRequest& request) const;
			AddStorageVideoTaskOutcome addStorageVideoTask(const Model::AddStorageVideoTaskRequest &request)const;
			void addStorageVideoTaskAsync(const Model::AddStorageVideoTaskRequest& request, const AddStorageVideoTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddStorageVideoTaskOutcomeCallable addStorageVideoTaskCallable(const Model::AddStorageVideoTaskRequest& request) const;
			GetTaskStatusOutcome getTaskStatus(const Model::GetTaskStatusRequest &request)const;
			void getTaskStatusAsync(const Model::GetTaskStatusRequest& request, const GetTaskStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetTaskStatusOutcomeCallable getTaskStatusCallable(const Model::GetTaskStatusRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_VIDEOSEARCH_VIDEOSEARCHCLIENT_H_

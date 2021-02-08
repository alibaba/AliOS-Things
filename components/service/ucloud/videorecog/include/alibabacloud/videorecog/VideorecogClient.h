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

#ifndef ALIBABACLOUD_VIDEORECOG_VIDEORECOGCLIENT_H_
#define ALIBABACLOUD_VIDEORECOG_VIDEORECOGCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "VideorecogExport.h"
#include "model/DetectVideoShotRequest.h"
#include "model/DetectVideoShotResult.h"
#include "model/GenerateVideoCoverRequest.h"
#include "model/GenerateVideoCoverResult.h"
#include "model/GetAsyncJobResultRequest.h"
#include "model/GetAsyncJobResultResult.h"


namespace AlibabaCloud
{
	namespace Videorecog
	{
		class ALIBABACLOUD_VIDEORECOG_EXPORT VideorecogClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::DetectVideoShotResult> DetectVideoShotOutcome;
			typedef std::future<DetectVideoShotOutcome> DetectVideoShotOutcomeCallable;
			typedef std::function<void(const VideorecogClient*, const Model::DetectVideoShotRequest&, const DetectVideoShotOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DetectVideoShotAsyncHandler;
			typedef Outcome<Error, Model::GenerateVideoCoverResult> GenerateVideoCoverOutcome;
			typedef std::future<GenerateVideoCoverOutcome> GenerateVideoCoverOutcomeCallable;
			typedef std::function<void(const VideorecogClient*, const Model::GenerateVideoCoverRequest&, const GenerateVideoCoverOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GenerateVideoCoverAsyncHandler;
			typedef Outcome<Error, Model::GetAsyncJobResultResult> GetAsyncJobResultOutcome;
			typedef std::future<GetAsyncJobResultOutcome> GetAsyncJobResultOutcomeCallable;
			typedef std::function<void(const VideorecogClient*, const Model::GetAsyncJobResultRequest&, const GetAsyncJobResultOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetAsyncJobResultAsyncHandler;

			VideorecogClient(const Credentials &credentials, const ClientConfiguration &configuration);
			VideorecogClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			VideorecogClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~VideorecogClient();
			DetectVideoShotOutcome detectVideoShot(const Model::DetectVideoShotRequest &request)const;
			void detectVideoShotAsync(const Model::DetectVideoShotRequest& request, const DetectVideoShotAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DetectVideoShotOutcomeCallable detectVideoShotCallable(const Model::DetectVideoShotRequest& request) const;
			GenerateVideoCoverOutcome generateVideoCover(const Model::GenerateVideoCoverRequest &request)const;
			void generateVideoCoverAsync(const Model::GenerateVideoCoverRequest& request, const GenerateVideoCoverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GenerateVideoCoverOutcomeCallable generateVideoCoverCallable(const Model::GenerateVideoCoverRequest& request) const;
			GetAsyncJobResultOutcome getAsyncJobResult(const Model::GetAsyncJobResultRequest &request)const;
			void getAsyncJobResultAsync(const Model::GetAsyncJobResultRequest& request, const GetAsyncJobResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetAsyncJobResultOutcomeCallable getAsyncJobResultCallable(const Model::GetAsyncJobResultRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_VIDEORECOG_VIDEORECOGCLIENT_H_

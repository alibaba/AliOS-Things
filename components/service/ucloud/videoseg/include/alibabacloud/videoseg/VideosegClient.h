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

#ifndef ALIBABACLOUD_VIDEOSEG_VIDEOSEGCLIENT_H_
#define ALIBABACLOUD_VIDEOSEG_VIDEOSEGCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "VideosegExport.h"
#include "model/GetAsyncJobResultRequest.h"
#include "model/GetAsyncJobResultResult.h"
#include "model/SegmentHalfBodyRequest.h"
#include "model/SegmentHalfBodyResult.h"
#include "model/SegmentVideoBodyRequest.h"
#include "model/SegmentVideoBodyResult.h"


namespace AlibabaCloud
{
	namespace Videoseg
	{
		class ALIBABACLOUD_VIDEOSEG_EXPORT VideosegClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::GetAsyncJobResultResult> GetAsyncJobResultOutcome;
			typedef std::future<GetAsyncJobResultOutcome> GetAsyncJobResultOutcomeCallable;
			typedef std::function<void(const VideosegClient*, const Model::GetAsyncJobResultRequest&, const GetAsyncJobResultOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetAsyncJobResultAsyncHandler;
			typedef Outcome<Error, Model::SegmentHalfBodyResult> SegmentHalfBodyOutcome;
			typedef std::future<SegmentHalfBodyOutcome> SegmentHalfBodyOutcomeCallable;
			typedef std::function<void(const VideosegClient*, const Model::SegmentHalfBodyRequest&, const SegmentHalfBodyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentHalfBodyAsyncHandler;
			typedef Outcome<Error, Model::SegmentVideoBodyResult> SegmentVideoBodyOutcome;
			typedef std::future<SegmentVideoBodyOutcome> SegmentVideoBodyOutcomeCallable;
			typedef std::function<void(const VideosegClient*, const Model::SegmentVideoBodyRequest&, const SegmentVideoBodyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SegmentVideoBodyAsyncHandler;

			VideosegClient(const Credentials &credentials, const ClientConfiguration &configuration);
			VideosegClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			VideosegClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~VideosegClient();
			GetAsyncJobResultOutcome getAsyncJobResult(const Model::GetAsyncJobResultRequest &request)const;
			void getAsyncJobResultAsync(const Model::GetAsyncJobResultRequest& request, const GetAsyncJobResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetAsyncJobResultOutcomeCallable getAsyncJobResultCallable(const Model::GetAsyncJobResultRequest& request) const;
			SegmentHalfBodyOutcome segmentHalfBody(const Model::SegmentHalfBodyRequest &request)const;
			void segmentHalfBodyAsync(const Model::SegmentHalfBodyRequest& request, const SegmentHalfBodyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentHalfBodyOutcomeCallable segmentHalfBodyCallable(const Model::SegmentHalfBodyRequest& request) const;
			SegmentVideoBodyOutcome segmentVideoBody(const Model::SegmentVideoBodyRequest &request)const;
			void segmentVideoBodyAsync(const Model::SegmentVideoBodyRequest& request, const SegmentVideoBodyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SegmentVideoBodyOutcomeCallable segmentVideoBodyCallable(const Model::SegmentVideoBodyRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_VIDEOSEG_VIDEOSEGCLIENT_H_

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

#ifndef ALIBABACLOUD_METERING_METERINGCLIENT_H_
#define ALIBABACLOUD_METERING_METERINGCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RoaServiceClient.h>
#include "MeteringExport.h"
#include "model/PostDataRequest.h"
#include "model/PostDataResult.h"
#include "model/SyncDataRequest.h"
#include "model/SyncDataResult.h"


namespace AlibabaCloud
{
	namespace Metering
	{
		class ALIBABACLOUD_METERING_EXPORT MeteringClient : public RoaServiceClient
		{
		public:
			typedef Outcome<Error, Model::PostDataResult> PostDataOutcome;
			typedef std::future<PostDataOutcome> PostDataOutcomeCallable;
			typedef std::function<void(const MeteringClient*, const Model::PostDataRequest&, const PostDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> PostDataAsyncHandler;
			typedef Outcome<Error, Model::SyncDataResult> SyncDataOutcome;
			typedef std::future<SyncDataOutcome> SyncDataOutcomeCallable;
			typedef std::function<void(const MeteringClient*, const Model::SyncDataRequest&, const SyncDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SyncDataAsyncHandler;

			MeteringClient(const Credentials &credentials, const ClientConfiguration &configuration);
			MeteringClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			MeteringClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~MeteringClient();
			PostDataOutcome postData(const Model::PostDataRequest &request)const;
			void postDataAsync(const Model::PostDataRequest& request, const PostDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			PostDataOutcomeCallable postDataCallable(const Model::PostDataRequest& request) const;
			SyncDataOutcome syncData(const Model::SyncDataRequest &request)const;
			void syncDataAsync(const Model::SyncDataRequest& request, const SyncDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SyncDataOutcomeCallable syncDataCallable(const Model::SyncDataRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_METERING_METERINGCLIENT_H_

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

#ifndef ALIBABACLOUD_IVISION_IVISIONCLIENT_H_
#define ALIBABACLOUD_IVISION_IVISIONCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "IvisionExport.h"
#include "model/CreateFaceGroupRequest.h"
#include "model/CreateFaceGroupResult.h"
#include "model/CreateStreamPredictRequest.h"
#include "model/CreateStreamPredictResult.h"
#include "model/DeleteFaceGroupRequest.h"
#include "model/DeleteFaceGroupResult.h"
#include "model/DeleteStreamPredictRequest.h"
#include "model/DeleteStreamPredictResult.h"
#include "model/DescribeFaceGroupsRequest.h"
#include "model/DescribeFaceGroupsResult.h"
#include "model/DescribeStreamPredictResultRequest.h"
#include "model/DescribeStreamPredictResultResult.h"
#include "model/DescribeStreamPredictsRequest.h"
#include "model/DescribeStreamPredictsResult.h"
#include "model/ImagePredictRequest.h"
#include "model/ImagePredictResult.h"
#include "model/RegisterFaceRequest.h"
#include "model/RegisterFaceResult.h"
#include "model/SearchFaceRequest.h"
#include "model/SearchFaceResult.h"
#include "model/StartStreamPredictRequest.h"
#include "model/StartStreamPredictResult.h"
#include "model/StopStreamPredictRequest.h"
#include "model/StopStreamPredictResult.h"
#include "model/UnregisterFaceRequest.h"
#include "model/UnregisterFaceResult.h"


namespace AlibabaCloud
{
	namespace Ivision
	{
		class ALIBABACLOUD_IVISION_EXPORT IvisionClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::CreateFaceGroupResult> CreateFaceGroupOutcome;
			typedef std::future<CreateFaceGroupOutcome> CreateFaceGroupOutcomeCallable;
			typedef std::function<void(const IvisionClient*, const Model::CreateFaceGroupRequest&, const CreateFaceGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateFaceGroupAsyncHandler;
			typedef Outcome<Error, Model::CreateStreamPredictResult> CreateStreamPredictOutcome;
			typedef std::future<CreateStreamPredictOutcome> CreateStreamPredictOutcomeCallable;
			typedef std::function<void(const IvisionClient*, const Model::CreateStreamPredictRequest&, const CreateStreamPredictOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateStreamPredictAsyncHandler;
			typedef Outcome<Error, Model::DeleteFaceGroupResult> DeleteFaceGroupOutcome;
			typedef std::future<DeleteFaceGroupOutcome> DeleteFaceGroupOutcomeCallable;
			typedef std::function<void(const IvisionClient*, const Model::DeleteFaceGroupRequest&, const DeleteFaceGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteFaceGroupAsyncHandler;
			typedef Outcome<Error, Model::DeleteStreamPredictResult> DeleteStreamPredictOutcome;
			typedef std::future<DeleteStreamPredictOutcome> DeleteStreamPredictOutcomeCallable;
			typedef std::function<void(const IvisionClient*, const Model::DeleteStreamPredictRequest&, const DeleteStreamPredictOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteStreamPredictAsyncHandler;
			typedef Outcome<Error, Model::DescribeFaceGroupsResult> DescribeFaceGroupsOutcome;
			typedef std::future<DescribeFaceGroupsOutcome> DescribeFaceGroupsOutcomeCallable;
			typedef std::function<void(const IvisionClient*, const Model::DescribeFaceGroupsRequest&, const DescribeFaceGroupsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeFaceGroupsAsyncHandler;
			typedef Outcome<Error, Model::DescribeStreamPredictResultResult> DescribeStreamPredictResultOutcome;
			typedef std::future<DescribeStreamPredictResultOutcome> DescribeStreamPredictResultOutcomeCallable;
			typedef std::function<void(const IvisionClient*, const Model::DescribeStreamPredictResultRequest&, const DescribeStreamPredictResultOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeStreamPredictResultAsyncHandler;
			typedef Outcome<Error, Model::DescribeStreamPredictsResult> DescribeStreamPredictsOutcome;
			typedef std::future<DescribeStreamPredictsOutcome> DescribeStreamPredictsOutcomeCallable;
			typedef std::function<void(const IvisionClient*, const Model::DescribeStreamPredictsRequest&, const DescribeStreamPredictsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeStreamPredictsAsyncHandler;
			typedef Outcome<Error, Model::ImagePredictResult> ImagePredictOutcome;
			typedef std::future<ImagePredictOutcome> ImagePredictOutcomeCallable;
			typedef std::function<void(const IvisionClient*, const Model::ImagePredictRequest&, const ImagePredictOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ImagePredictAsyncHandler;
			typedef Outcome<Error, Model::RegisterFaceResult> RegisterFaceOutcome;
			typedef std::future<RegisterFaceOutcome> RegisterFaceOutcomeCallable;
			typedef std::function<void(const IvisionClient*, const Model::RegisterFaceRequest&, const RegisterFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RegisterFaceAsyncHandler;
			typedef Outcome<Error, Model::SearchFaceResult> SearchFaceOutcome;
			typedef std::future<SearchFaceOutcome> SearchFaceOutcomeCallable;
			typedef std::function<void(const IvisionClient*, const Model::SearchFaceRequest&, const SearchFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SearchFaceAsyncHandler;
			typedef Outcome<Error, Model::StartStreamPredictResult> StartStreamPredictOutcome;
			typedef std::future<StartStreamPredictOutcome> StartStreamPredictOutcomeCallable;
			typedef std::function<void(const IvisionClient*, const Model::StartStreamPredictRequest&, const StartStreamPredictOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartStreamPredictAsyncHandler;
			typedef Outcome<Error, Model::StopStreamPredictResult> StopStreamPredictOutcome;
			typedef std::future<StopStreamPredictOutcome> StopStreamPredictOutcomeCallable;
			typedef std::function<void(const IvisionClient*, const Model::StopStreamPredictRequest&, const StopStreamPredictOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StopStreamPredictAsyncHandler;
			typedef Outcome<Error, Model::UnregisterFaceResult> UnregisterFaceOutcome;
			typedef std::future<UnregisterFaceOutcome> UnregisterFaceOutcomeCallable;
			typedef std::function<void(const IvisionClient*, const Model::UnregisterFaceRequest&, const UnregisterFaceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UnregisterFaceAsyncHandler;

			IvisionClient(const Credentials &credentials, const ClientConfiguration &configuration);
			IvisionClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			IvisionClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~IvisionClient();
			CreateFaceGroupOutcome createFaceGroup(const Model::CreateFaceGroupRequest &request)const;
			void createFaceGroupAsync(const Model::CreateFaceGroupRequest& request, const CreateFaceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateFaceGroupOutcomeCallable createFaceGroupCallable(const Model::CreateFaceGroupRequest& request) const;
			CreateStreamPredictOutcome createStreamPredict(const Model::CreateStreamPredictRequest &request)const;
			void createStreamPredictAsync(const Model::CreateStreamPredictRequest& request, const CreateStreamPredictAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateStreamPredictOutcomeCallable createStreamPredictCallable(const Model::CreateStreamPredictRequest& request) const;
			DeleteFaceGroupOutcome deleteFaceGroup(const Model::DeleteFaceGroupRequest &request)const;
			void deleteFaceGroupAsync(const Model::DeleteFaceGroupRequest& request, const DeleteFaceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteFaceGroupOutcomeCallable deleteFaceGroupCallable(const Model::DeleteFaceGroupRequest& request) const;
			DeleteStreamPredictOutcome deleteStreamPredict(const Model::DeleteStreamPredictRequest &request)const;
			void deleteStreamPredictAsync(const Model::DeleteStreamPredictRequest& request, const DeleteStreamPredictAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteStreamPredictOutcomeCallable deleteStreamPredictCallable(const Model::DeleteStreamPredictRequest& request) const;
			DescribeFaceGroupsOutcome describeFaceGroups(const Model::DescribeFaceGroupsRequest &request)const;
			void describeFaceGroupsAsync(const Model::DescribeFaceGroupsRequest& request, const DescribeFaceGroupsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeFaceGroupsOutcomeCallable describeFaceGroupsCallable(const Model::DescribeFaceGroupsRequest& request) const;
			DescribeStreamPredictResultOutcome describeStreamPredictResult(const Model::DescribeStreamPredictResultRequest &request)const;
			void describeStreamPredictResultAsync(const Model::DescribeStreamPredictResultRequest& request, const DescribeStreamPredictResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeStreamPredictResultOutcomeCallable describeStreamPredictResultCallable(const Model::DescribeStreamPredictResultRequest& request) const;
			DescribeStreamPredictsOutcome describeStreamPredicts(const Model::DescribeStreamPredictsRequest &request)const;
			void describeStreamPredictsAsync(const Model::DescribeStreamPredictsRequest& request, const DescribeStreamPredictsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeStreamPredictsOutcomeCallable describeStreamPredictsCallable(const Model::DescribeStreamPredictsRequest& request) const;
			ImagePredictOutcome imagePredict(const Model::ImagePredictRequest &request)const;
			void imagePredictAsync(const Model::ImagePredictRequest& request, const ImagePredictAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ImagePredictOutcomeCallable imagePredictCallable(const Model::ImagePredictRequest& request) const;
			RegisterFaceOutcome registerFace(const Model::RegisterFaceRequest &request)const;
			void registerFaceAsync(const Model::RegisterFaceRequest& request, const RegisterFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RegisterFaceOutcomeCallable registerFaceCallable(const Model::RegisterFaceRequest& request) const;
			SearchFaceOutcome searchFace(const Model::SearchFaceRequest &request)const;
			void searchFaceAsync(const Model::SearchFaceRequest& request, const SearchFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SearchFaceOutcomeCallable searchFaceCallable(const Model::SearchFaceRequest& request) const;
			StartStreamPredictOutcome startStreamPredict(const Model::StartStreamPredictRequest &request)const;
			void startStreamPredictAsync(const Model::StartStreamPredictRequest& request, const StartStreamPredictAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartStreamPredictOutcomeCallable startStreamPredictCallable(const Model::StartStreamPredictRequest& request) const;
			StopStreamPredictOutcome stopStreamPredict(const Model::StopStreamPredictRequest &request)const;
			void stopStreamPredictAsync(const Model::StopStreamPredictRequest& request, const StopStreamPredictAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StopStreamPredictOutcomeCallable stopStreamPredictCallable(const Model::StopStreamPredictRequest& request) const;
			UnregisterFaceOutcome unregisterFace(const Model::UnregisterFaceRequest &request)const;
			void unregisterFaceAsync(const Model::UnregisterFaceRequest& request, const UnregisterFaceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UnregisterFaceOutcomeCallable unregisterFaceCallable(const Model::UnregisterFaceRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_IVISION_IVISIONCLIENT_H_

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

#ifndef ALIBABACLOUD_MULTIMEDIAAI_MULTIMEDIAAICLIENT_H_
#define ALIBABACLOUD_MULTIMEDIAAI_MULTIMEDIAAICLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "MultimediaaiExport.h"
#include "model/CreateCoverTaskRequest.h"
#include "model/CreateCoverTaskResult.h"
#include "model/CreateFaceGroupRequest.h"
#include "model/CreateFaceGroupResult.h"
#include "model/CreateFacePersonRequest.h"
#include "model/CreateFacePersonResult.h"
#include "model/CreateGifTaskRequest.h"
#include "model/CreateGifTaskResult.h"
#include "model/CreateLabelTaskRequest.h"
#include "model/CreateLabelTaskResult.h"
#include "model/CreateTemplateRequest.h"
#include "model/CreateTemplateResult.h"
#include "model/DeleteFaceGroupRequest.h"
#include "model/DeleteFaceGroupResult.h"
#include "model/DeleteFaceImageRequest.h"
#include "model/DeleteFaceImageResult.h"
#include "model/DeleteFacePersonRequest.h"
#include "model/DeleteFacePersonResult.h"
#include "model/GetTaskResultRequest.h"
#include "model/GetTaskResultResult.h"
#include "model/GetTaskStatusRequest.h"
#include "model/GetTaskStatusResult.h"
#include "model/GetTemplateRequest.h"
#include "model/GetTemplateResult.h"
#include "model/ListFaceGroupsRequest.h"
#include "model/ListFaceGroupsResult.h"
#include "model/ListFaceImagesRequest.h"
#include "model/ListFaceImagesResult.h"
#include "model/ListFacePersonsRequest.h"
#include "model/ListFacePersonsResult.h"
#include "model/ListTemplatesRequest.h"
#include "model/ListTemplatesResult.h"
#include "model/ProcessFaceAlgorithmRequest.h"
#include "model/ProcessFaceAlgorithmResult.h"
#include "model/ProcessImageTagAlgorithmRequest.h"
#include "model/ProcessImageTagAlgorithmResult.h"
#include "model/ProcessLandmarkAlgorithmRequest.h"
#include "model/ProcessLandmarkAlgorithmResult.h"
#include "model/ProcessLogoAlgorithmRequest.h"
#include "model/ProcessLogoAlgorithmResult.h"
#include "model/ProcessNewsAlgorithmRequest.h"
#include "model/ProcessNewsAlgorithmResult.h"
#include "model/ProcessNlpAlgorithmRequest.h"
#include "model/ProcessNlpAlgorithmResult.h"
#include "model/ProcessOcrAlgorithmRequest.h"
#include "model/ProcessOcrAlgorithmResult.h"
#include "model/RegisterFaceImageRequest.h"
#include "model/RegisterFaceImageResult.h"
#include "model/UpdateTemplateRequest.h"
#include "model/UpdateTemplateResult.h"


namespace AlibabaCloud
{
	namespace Multimediaai
	{
		class ALIBABACLOUD_MULTIMEDIAAI_EXPORT MultimediaaiClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::CreateCoverTaskResult> CreateCoverTaskOutcome;
			typedef std::future<CreateCoverTaskOutcome> CreateCoverTaskOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::CreateCoverTaskRequest&, const CreateCoverTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateCoverTaskAsyncHandler;
			typedef Outcome<Error, Model::CreateFaceGroupResult> CreateFaceGroupOutcome;
			typedef std::future<CreateFaceGroupOutcome> CreateFaceGroupOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::CreateFaceGroupRequest&, const CreateFaceGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateFaceGroupAsyncHandler;
			typedef Outcome<Error, Model::CreateFacePersonResult> CreateFacePersonOutcome;
			typedef std::future<CreateFacePersonOutcome> CreateFacePersonOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::CreateFacePersonRequest&, const CreateFacePersonOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateFacePersonAsyncHandler;
			typedef Outcome<Error, Model::CreateGifTaskResult> CreateGifTaskOutcome;
			typedef std::future<CreateGifTaskOutcome> CreateGifTaskOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::CreateGifTaskRequest&, const CreateGifTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateGifTaskAsyncHandler;
			typedef Outcome<Error, Model::CreateLabelTaskResult> CreateLabelTaskOutcome;
			typedef std::future<CreateLabelTaskOutcome> CreateLabelTaskOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::CreateLabelTaskRequest&, const CreateLabelTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateLabelTaskAsyncHandler;
			typedef Outcome<Error, Model::CreateTemplateResult> CreateTemplateOutcome;
			typedef std::future<CreateTemplateOutcome> CreateTemplateOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::CreateTemplateRequest&, const CreateTemplateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateTemplateAsyncHandler;
			typedef Outcome<Error, Model::DeleteFaceGroupResult> DeleteFaceGroupOutcome;
			typedef std::future<DeleteFaceGroupOutcome> DeleteFaceGroupOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::DeleteFaceGroupRequest&, const DeleteFaceGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteFaceGroupAsyncHandler;
			typedef Outcome<Error, Model::DeleteFaceImageResult> DeleteFaceImageOutcome;
			typedef std::future<DeleteFaceImageOutcome> DeleteFaceImageOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::DeleteFaceImageRequest&, const DeleteFaceImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteFaceImageAsyncHandler;
			typedef Outcome<Error, Model::DeleteFacePersonResult> DeleteFacePersonOutcome;
			typedef std::future<DeleteFacePersonOutcome> DeleteFacePersonOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::DeleteFacePersonRequest&, const DeleteFacePersonOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteFacePersonAsyncHandler;
			typedef Outcome<Error, Model::GetTaskResultResult> GetTaskResultOutcome;
			typedef std::future<GetTaskResultOutcome> GetTaskResultOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::GetTaskResultRequest&, const GetTaskResultOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetTaskResultAsyncHandler;
			typedef Outcome<Error, Model::GetTaskStatusResult> GetTaskStatusOutcome;
			typedef std::future<GetTaskStatusOutcome> GetTaskStatusOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::GetTaskStatusRequest&, const GetTaskStatusOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetTaskStatusAsyncHandler;
			typedef Outcome<Error, Model::GetTemplateResult> GetTemplateOutcome;
			typedef std::future<GetTemplateOutcome> GetTemplateOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::GetTemplateRequest&, const GetTemplateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetTemplateAsyncHandler;
			typedef Outcome<Error, Model::ListFaceGroupsResult> ListFaceGroupsOutcome;
			typedef std::future<ListFaceGroupsOutcome> ListFaceGroupsOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::ListFaceGroupsRequest&, const ListFaceGroupsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListFaceGroupsAsyncHandler;
			typedef Outcome<Error, Model::ListFaceImagesResult> ListFaceImagesOutcome;
			typedef std::future<ListFaceImagesOutcome> ListFaceImagesOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::ListFaceImagesRequest&, const ListFaceImagesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListFaceImagesAsyncHandler;
			typedef Outcome<Error, Model::ListFacePersonsResult> ListFacePersonsOutcome;
			typedef std::future<ListFacePersonsOutcome> ListFacePersonsOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::ListFacePersonsRequest&, const ListFacePersonsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListFacePersonsAsyncHandler;
			typedef Outcome<Error, Model::ListTemplatesResult> ListTemplatesOutcome;
			typedef std::future<ListTemplatesOutcome> ListTemplatesOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::ListTemplatesRequest&, const ListTemplatesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListTemplatesAsyncHandler;
			typedef Outcome<Error, Model::ProcessFaceAlgorithmResult> ProcessFaceAlgorithmOutcome;
			typedef std::future<ProcessFaceAlgorithmOutcome> ProcessFaceAlgorithmOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::ProcessFaceAlgorithmRequest&, const ProcessFaceAlgorithmOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ProcessFaceAlgorithmAsyncHandler;
			typedef Outcome<Error, Model::ProcessImageTagAlgorithmResult> ProcessImageTagAlgorithmOutcome;
			typedef std::future<ProcessImageTagAlgorithmOutcome> ProcessImageTagAlgorithmOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::ProcessImageTagAlgorithmRequest&, const ProcessImageTagAlgorithmOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ProcessImageTagAlgorithmAsyncHandler;
			typedef Outcome<Error, Model::ProcessLandmarkAlgorithmResult> ProcessLandmarkAlgorithmOutcome;
			typedef std::future<ProcessLandmarkAlgorithmOutcome> ProcessLandmarkAlgorithmOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::ProcessLandmarkAlgorithmRequest&, const ProcessLandmarkAlgorithmOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ProcessLandmarkAlgorithmAsyncHandler;
			typedef Outcome<Error, Model::ProcessLogoAlgorithmResult> ProcessLogoAlgorithmOutcome;
			typedef std::future<ProcessLogoAlgorithmOutcome> ProcessLogoAlgorithmOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::ProcessLogoAlgorithmRequest&, const ProcessLogoAlgorithmOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ProcessLogoAlgorithmAsyncHandler;
			typedef Outcome<Error, Model::ProcessNewsAlgorithmResult> ProcessNewsAlgorithmOutcome;
			typedef std::future<ProcessNewsAlgorithmOutcome> ProcessNewsAlgorithmOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::ProcessNewsAlgorithmRequest&, const ProcessNewsAlgorithmOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ProcessNewsAlgorithmAsyncHandler;
			typedef Outcome<Error, Model::ProcessNlpAlgorithmResult> ProcessNlpAlgorithmOutcome;
			typedef std::future<ProcessNlpAlgorithmOutcome> ProcessNlpAlgorithmOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::ProcessNlpAlgorithmRequest&, const ProcessNlpAlgorithmOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ProcessNlpAlgorithmAsyncHandler;
			typedef Outcome<Error, Model::ProcessOcrAlgorithmResult> ProcessOcrAlgorithmOutcome;
			typedef std::future<ProcessOcrAlgorithmOutcome> ProcessOcrAlgorithmOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::ProcessOcrAlgorithmRequest&, const ProcessOcrAlgorithmOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ProcessOcrAlgorithmAsyncHandler;
			typedef Outcome<Error, Model::RegisterFaceImageResult> RegisterFaceImageOutcome;
			typedef std::future<RegisterFaceImageOutcome> RegisterFaceImageOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::RegisterFaceImageRequest&, const RegisterFaceImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RegisterFaceImageAsyncHandler;
			typedef Outcome<Error, Model::UpdateTemplateResult> UpdateTemplateOutcome;
			typedef std::future<UpdateTemplateOutcome> UpdateTemplateOutcomeCallable;
			typedef std::function<void(const MultimediaaiClient*, const Model::UpdateTemplateRequest&, const UpdateTemplateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateTemplateAsyncHandler;

			MultimediaaiClient(const Credentials &credentials, const ClientConfiguration &configuration);
			MultimediaaiClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			MultimediaaiClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~MultimediaaiClient();
			CreateCoverTaskOutcome createCoverTask(const Model::CreateCoverTaskRequest &request)const;
			void createCoverTaskAsync(const Model::CreateCoverTaskRequest& request, const CreateCoverTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateCoverTaskOutcomeCallable createCoverTaskCallable(const Model::CreateCoverTaskRequest& request) const;
			CreateFaceGroupOutcome createFaceGroup(const Model::CreateFaceGroupRequest &request)const;
			void createFaceGroupAsync(const Model::CreateFaceGroupRequest& request, const CreateFaceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateFaceGroupOutcomeCallable createFaceGroupCallable(const Model::CreateFaceGroupRequest& request) const;
			CreateFacePersonOutcome createFacePerson(const Model::CreateFacePersonRequest &request)const;
			void createFacePersonAsync(const Model::CreateFacePersonRequest& request, const CreateFacePersonAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateFacePersonOutcomeCallable createFacePersonCallable(const Model::CreateFacePersonRequest& request) const;
			CreateGifTaskOutcome createGifTask(const Model::CreateGifTaskRequest &request)const;
			void createGifTaskAsync(const Model::CreateGifTaskRequest& request, const CreateGifTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateGifTaskOutcomeCallable createGifTaskCallable(const Model::CreateGifTaskRequest& request) const;
			CreateLabelTaskOutcome createLabelTask(const Model::CreateLabelTaskRequest &request)const;
			void createLabelTaskAsync(const Model::CreateLabelTaskRequest& request, const CreateLabelTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateLabelTaskOutcomeCallable createLabelTaskCallable(const Model::CreateLabelTaskRequest& request) const;
			CreateTemplateOutcome createTemplate(const Model::CreateTemplateRequest &request)const;
			void createTemplateAsync(const Model::CreateTemplateRequest& request, const CreateTemplateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateTemplateOutcomeCallable createTemplateCallable(const Model::CreateTemplateRequest& request) const;
			DeleteFaceGroupOutcome deleteFaceGroup(const Model::DeleteFaceGroupRequest &request)const;
			void deleteFaceGroupAsync(const Model::DeleteFaceGroupRequest& request, const DeleteFaceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteFaceGroupOutcomeCallable deleteFaceGroupCallable(const Model::DeleteFaceGroupRequest& request) const;
			DeleteFaceImageOutcome deleteFaceImage(const Model::DeleteFaceImageRequest &request)const;
			void deleteFaceImageAsync(const Model::DeleteFaceImageRequest& request, const DeleteFaceImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteFaceImageOutcomeCallable deleteFaceImageCallable(const Model::DeleteFaceImageRequest& request) const;
			DeleteFacePersonOutcome deleteFacePerson(const Model::DeleteFacePersonRequest &request)const;
			void deleteFacePersonAsync(const Model::DeleteFacePersonRequest& request, const DeleteFacePersonAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteFacePersonOutcomeCallable deleteFacePersonCallable(const Model::DeleteFacePersonRequest& request) const;
			GetTaskResultOutcome getTaskResult(const Model::GetTaskResultRequest &request)const;
			void getTaskResultAsync(const Model::GetTaskResultRequest& request, const GetTaskResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetTaskResultOutcomeCallable getTaskResultCallable(const Model::GetTaskResultRequest& request) const;
			GetTaskStatusOutcome getTaskStatus(const Model::GetTaskStatusRequest &request)const;
			void getTaskStatusAsync(const Model::GetTaskStatusRequest& request, const GetTaskStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetTaskStatusOutcomeCallable getTaskStatusCallable(const Model::GetTaskStatusRequest& request) const;
			GetTemplateOutcome getTemplate(const Model::GetTemplateRequest &request)const;
			void getTemplateAsync(const Model::GetTemplateRequest& request, const GetTemplateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetTemplateOutcomeCallable getTemplateCallable(const Model::GetTemplateRequest& request) const;
			ListFaceGroupsOutcome listFaceGroups(const Model::ListFaceGroupsRequest &request)const;
			void listFaceGroupsAsync(const Model::ListFaceGroupsRequest& request, const ListFaceGroupsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListFaceGroupsOutcomeCallable listFaceGroupsCallable(const Model::ListFaceGroupsRequest& request) const;
			ListFaceImagesOutcome listFaceImages(const Model::ListFaceImagesRequest &request)const;
			void listFaceImagesAsync(const Model::ListFaceImagesRequest& request, const ListFaceImagesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListFaceImagesOutcomeCallable listFaceImagesCallable(const Model::ListFaceImagesRequest& request) const;
			ListFacePersonsOutcome listFacePersons(const Model::ListFacePersonsRequest &request)const;
			void listFacePersonsAsync(const Model::ListFacePersonsRequest& request, const ListFacePersonsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListFacePersonsOutcomeCallable listFacePersonsCallable(const Model::ListFacePersonsRequest& request) const;
			ListTemplatesOutcome listTemplates(const Model::ListTemplatesRequest &request)const;
			void listTemplatesAsync(const Model::ListTemplatesRequest& request, const ListTemplatesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListTemplatesOutcomeCallable listTemplatesCallable(const Model::ListTemplatesRequest& request) const;
			ProcessFaceAlgorithmOutcome processFaceAlgorithm(const Model::ProcessFaceAlgorithmRequest &request)const;
			void processFaceAlgorithmAsync(const Model::ProcessFaceAlgorithmRequest& request, const ProcessFaceAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ProcessFaceAlgorithmOutcomeCallable processFaceAlgorithmCallable(const Model::ProcessFaceAlgorithmRequest& request) const;
			ProcessImageTagAlgorithmOutcome processImageTagAlgorithm(const Model::ProcessImageTagAlgorithmRequest &request)const;
			void processImageTagAlgorithmAsync(const Model::ProcessImageTagAlgorithmRequest& request, const ProcessImageTagAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ProcessImageTagAlgorithmOutcomeCallable processImageTagAlgorithmCallable(const Model::ProcessImageTagAlgorithmRequest& request) const;
			ProcessLandmarkAlgorithmOutcome processLandmarkAlgorithm(const Model::ProcessLandmarkAlgorithmRequest &request)const;
			void processLandmarkAlgorithmAsync(const Model::ProcessLandmarkAlgorithmRequest& request, const ProcessLandmarkAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ProcessLandmarkAlgorithmOutcomeCallable processLandmarkAlgorithmCallable(const Model::ProcessLandmarkAlgorithmRequest& request) const;
			ProcessLogoAlgorithmOutcome processLogoAlgorithm(const Model::ProcessLogoAlgorithmRequest &request)const;
			void processLogoAlgorithmAsync(const Model::ProcessLogoAlgorithmRequest& request, const ProcessLogoAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ProcessLogoAlgorithmOutcomeCallable processLogoAlgorithmCallable(const Model::ProcessLogoAlgorithmRequest& request) const;
			ProcessNewsAlgorithmOutcome processNewsAlgorithm(const Model::ProcessNewsAlgorithmRequest &request)const;
			void processNewsAlgorithmAsync(const Model::ProcessNewsAlgorithmRequest& request, const ProcessNewsAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ProcessNewsAlgorithmOutcomeCallable processNewsAlgorithmCallable(const Model::ProcessNewsAlgorithmRequest& request) const;
			ProcessNlpAlgorithmOutcome processNlpAlgorithm(const Model::ProcessNlpAlgorithmRequest &request)const;
			void processNlpAlgorithmAsync(const Model::ProcessNlpAlgorithmRequest& request, const ProcessNlpAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ProcessNlpAlgorithmOutcomeCallable processNlpAlgorithmCallable(const Model::ProcessNlpAlgorithmRequest& request) const;
			ProcessOcrAlgorithmOutcome processOcrAlgorithm(const Model::ProcessOcrAlgorithmRequest &request)const;
			void processOcrAlgorithmAsync(const Model::ProcessOcrAlgorithmRequest& request, const ProcessOcrAlgorithmAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ProcessOcrAlgorithmOutcomeCallable processOcrAlgorithmCallable(const Model::ProcessOcrAlgorithmRequest& request) const;
			RegisterFaceImageOutcome registerFaceImage(const Model::RegisterFaceImageRequest &request)const;
			void registerFaceImageAsync(const Model::RegisterFaceImageRequest& request, const RegisterFaceImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RegisterFaceImageOutcomeCallable registerFaceImageCallable(const Model::RegisterFaceImageRequest& request) const;
			UpdateTemplateOutcome updateTemplate(const Model::UpdateTemplateRequest &request)const;
			void updateTemplateAsync(const Model::UpdateTemplateRequest& request, const UpdateTemplateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateTemplateOutcomeCallable updateTemplateCallable(const Model::UpdateTemplateRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_MULTIMEDIAAI_MULTIMEDIAAICLIENT_H_

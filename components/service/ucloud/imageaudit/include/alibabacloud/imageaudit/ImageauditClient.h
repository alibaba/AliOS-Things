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

#ifndef ALIBABACLOUD_IMAGEAUDIT_IMAGEAUDITCLIENT_H_
#define ALIBABACLOUD_IMAGEAUDIT_IMAGEAUDITCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "ImageauditExport.h"
#include "model/ScanImageRequest.h"
#include "model/ScanImageResult.h"
#include "model/ScanTextRequest.h"
#include "model/ScanTextResult.h"


namespace AlibabaCloud
{
	namespace Imageaudit
	{
		class ALIBABACLOUD_IMAGEAUDIT_EXPORT ImageauditClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::ScanImageResult> ScanImageOutcome;
			typedef std::future<ScanImageOutcome> ScanImageOutcomeCallable;
			typedef std::function<void(const ImageauditClient*, const Model::ScanImageRequest&, const ScanImageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ScanImageAsyncHandler;
			typedef Outcome<Error, Model::ScanTextResult> ScanTextOutcome;
			typedef std::future<ScanTextOutcome> ScanTextOutcomeCallable;
			typedef std::function<void(const ImageauditClient*, const Model::ScanTextRequest&, const ScanTextOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ScanTextAsyncHandler;

			ImageauditClient(const Credentials &credentials, const ClientConfiguration &configuration);
			ImageauditClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			ImageauditClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~ImageauditClient();
			ScanImageOutcome scanImage(const Model::ScanImageRequest &request)const;
			void scanImageAsync(const Model::ScanImageRequest& request, const ScanImageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ScanImageOutcomeCallable scanImageCallable(const Model::ScanImageRequest& request) const;
			ScanTextOutcome scanText(const Model::ScanTextRequest &request)const;
			void scanTextAsync(const Model::ScanTextRequest& request, const ScanTextAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ScanTextOutcomeCallable scanTextCallable(const Model::ScanTextRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_IMAGEAUDIT_IMAGEAUDITCLIENT_H_

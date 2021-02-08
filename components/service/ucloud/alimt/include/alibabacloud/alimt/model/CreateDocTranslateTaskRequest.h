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

#ifndef ALIBABACLOUD_ALIMT_MODEL_CREATEDOCTRANSLATETASKREQUEST_H_
#define ALIBABACLOUD_ALIMT_MODEL_CREATEDOCTRANSLATETASKREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/alimt/AlimtExport.h>

namespace AlibabaCloud
{
	namespace Alimt
	{
		namespace Model
		{
			class ALIBABACLOUD_ALIMT_EXPORT CreateDocTranslateTaskRequest : public RpcServiceRequest
			{

			public:
				CreateDocTranslateTaskRequest();
				~CreateDocTranslateTaskRequest();

				std::string getSourceLanguage()const;
				void setSourceLanguage(const std::string& sourceLanguage);
				std::string getClientToken()const;
				void setClientToken(const std::string& clientToken);
				std::string getScene()const;
				void setScene(const std::string& scene);
				std::string getFileUrl()const;
				void setFileUrl(const std::string& fileUrl);
				std::string getTargetLanguage()const;
				void setTargetLanguage(const std::string& targetLanguage);
				std::string getCallbackUrl()const;
				void setCallbackUrl(const std::string& callbackUrl);

            private:
				std::string sourceLanguage_;
				std::string clientToken_;
				std::string scene_;
				std::string fileUrl_;
				std::string targetLanguage_;
				std::string callbackUrl_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALIMT_MODEL_CREATEDOCTRANSLATETASKREQUEST_H_
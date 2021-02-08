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

#ifndef ALIBABACLOUD_ALIMT_MODEL_GETIMAGETRANSLATEREQUEST_H_
#define ALIBABACLOUD_ALIMT_MODEL_GETIMAGETRANSLATEREQUEST_H_

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
			class ALIBABACLOUD_ALIMT_EXPORT GetImageTranslateRequest : public RpcServiceRequest
			{

			public:
				GetImageTranslateRequest();
				~GetImageTranslateRequest();

				std::string getSourceLanguage()const;
				void setSourceLanguage(const std::string& sourceLanguage);
				std::string getUrl()const;
				void setUrl(const std::string& url);
				std::string getExtra()const;
				void setExtra(const std::string& extra);
				std::string getTargetLanguage()const;
				void setTargetLanguage(const std::string& targetLanguage);

            private:
				std::string sourceLanguage_;
				std::string url_;
				std::string extra_;
				std::string targetLanguage_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALIMT_MODEL_GETIMAGETRANSLATEREQUEST_H_
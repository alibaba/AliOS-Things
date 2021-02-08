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

#ifndef ALIBABACLOUD_MULTIMEDIAAI_MODEL_CREATECOVERTASKREQUEST_H_
#define ALIBABACLOUD_MULTIMEDIAAI_MODEL_CREATECOVERTASKREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/multimediaai/MultimediaaiExport.h>

namespace AlibabaCloud
{
	namespace Multimediaai
	{
		namespace Model
		{
			class ALIBABACLOUD_MULTIMEDIAAI_EXPORT CreateCoverTaskRequest : public RpcServiceRequest
			{

			public:
				CreateCoverTaskRequest();
				~CreateCoverTaskRequest();

				long getTemplateId()const;
				void setTemplateId(long templateId);
				std::string getVideoUrl()const;
				void setVideoUrl(const std::string& videoUrl);
				std::string getScales()const;
				void setScales(const std::string& scales);
				std::string getVideoName()const;
				void setVideoName(const std::string& videoName);
				std::string getCallbackUrl()const;
				void setCallbackUrl(const std::string& callbackUrl);
				std::string getApplicationId()const;
				void setApplicationId(const std::string& applicationId);

            private:
				long templateId_;
				std::string videoUrl_;
				std::string scales_;
				std::string videoName_;
				std::string callbackUrl_;
				std::string applicationId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_MULTIMEDIAAI_MODEL_CREATECOVERTASKREQUEST_H_
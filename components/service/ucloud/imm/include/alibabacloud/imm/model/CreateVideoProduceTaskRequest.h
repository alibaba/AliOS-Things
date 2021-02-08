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

#ifndef ALIBABACLOUD_IMM_MODEL_CREATEVIDEOPRODUCETASKREQUEST_H_
#define ALIBABACLOUD_IMM_MODEL_CREATEVIDEOPRODUCETASKREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/imm/ImmExport.h>

namespace AlibabaCloud
{
	namespace Imm
	{
		namespace Model
		{
			class ALIBABACLOUD_IMM_EXPORT CreateVideoProduceTaskRequest : public RpcServiceRequest
			{

			public:
				CreateVideoProduceTaskRequest();
				~CreateVideoProduceTaskRequest();

				std::string getProject()const;
				void setProject(const std::string& project);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getMusic()const;
				void setMusic(const std::string& music);
				std::string getNotifyEndpoint()const;
				void setNotifyEndpoint(const std::string& notifyEndpoint);
				std::string getTargetUri()const;
				void setTargetUri(const std::string& targetUri);
				std::string getTemplateName()const;
				void setTemplateName(const std::string& templateName);
				int getHeight()const;
				void setHeight(int height);
				std::string getCustomMessage()const;
				void setCustomMessage(const std::string& customMessage);
				std::string getImages()const;
				void setImages(const std::string& images);
				std::string getNotifyTopicName()const;
				void setNotifyTopicName(const std::string& notifyTopicName);
				int getWidth()const;
				void setWidth(int width);

            private:
				std::string project_;
				std::string accessKeyId_;
				std::string music_;
				std::string notifyEndpoint_;
				std::string targetUri_;
				std::string templateName_;
				int height_;
				std::string customMessage_;
				std::string images_;
				std::string notifyTopicName_;
				int width_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_CREATEVIDEOPRODUCETASKREQUEST_H_
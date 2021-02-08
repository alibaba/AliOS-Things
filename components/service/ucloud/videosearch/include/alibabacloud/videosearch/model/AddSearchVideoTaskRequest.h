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

#ifndef ALIBABACLOUD_VIDEOSEARCH_MODEL_ADDSEARCHVIDEOTASKREQUEST_H_
#define ALIBABACLOUD_VIDEOSEARCH_MODEL_ADDSEARCHVIDEOTASKREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/videosearch/VideosearchExport.h>

namespace AlibabaCloud
{
	namespace Videosearch
	{
		namespace Model
		{
			class ALIBABACLOUD_VIDEOSEARCH_EXPORT AddSearchVideoTaskRequest : public RpcServiceRequest
			{

			public:
				AddSearchVideoTaskRequest();
				~AddSearchVideoTaskRequest();

				std::string getClientToken()const;
				void setClientToken(const std::string& clientToken);
				int getStorageType()const;
				void setStorageType(int storageType);
				std::string getRegionId()const;
				void setRegionId(const std::string& regionId);
				std::string getVideoTags()const;
				void setVideoTags(const std::string& videoTags);
				int getReturnVideoNumber()const;
				void setReturnVideoNumber(int returnVideoNumber);
				std::string getVideoId()const;
				void setVideoId(const std::string& videoId);
				float getReplaceStorageThreshold()const;
				void setReplaceStorageThreshold(float replaceStorageThreshold);
				std::string getInstanceId()const;
				void setInstanceId(const std::string& instanceId);
				std::string getVideoUrl()const;
				void setVideoUrl(const std::string& videoUrl);
				std::string getQueryTags()const;
				void setQueryTags(const std::string& queryTags);
				std::string getCallbackUrl()const;
				void setCallbackUrl(const std::string& callbackUrl);

            private:
				std::string clientToken_;
				int storageType_;
				std::string regionId_;
				std::string videoTags_;
				int returnVideoNumber_;
				std::string videoId_;
				float replaceStorageThreshold_;
				std::string instanceId_;
				std::string videoUrl_;
				std::string queryTags_;
				std::string callbackUrl_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_VIDEOSEARCH_MODEL_ADDSEARCHVIDEOTASKREQUEST_H_